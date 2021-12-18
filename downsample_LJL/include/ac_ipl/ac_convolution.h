////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2018 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
//
////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// File: ac_convolution.h
//
// Description:
//
// Usage:
//
// Notes:
//
// Revision History:
//
//***************************************************************************

#ifndef _INCLUDED_AC_CONVOLUTION_H_
#define _INCLUDED_AC_CONVOLUTION_H_

#include <ac_ipl/ac_line_buffer.h>
#include <ac_ipl/ac_shift_reg.h>
#include <ac_ipl/ac_boundary.h>
#include <ac_wndo.h>
#include <ac_channel.h>

template<int W0>
struct constArrStruct {
  uint8 data[W0][3][3];
};

template<int WIDTH=8>
struct AXI4VideoStream {
  ac_int<WIDTH,false> data;
  bool             sof;
  bool             eol;
};

template<typename T, int IN_CHANNELS>
struct axi4VideoStream {
  ac_array<T,IN_CHANNELS> data;
  bool TUSER;//start of frame
  bool TLAST;//end of line
};

template<typename T>
struct axi4VideoStream<T,1> {
  T data;
  bool TUSER;//start of frame
  bool TLAST;//end of line
};

template<typename T1, typename T2>
struct pack2 {
  T1 dat0;
  T2 dat1;
};

//=========================================================================
// Class: ac_convNxN_stride_N_pad_0
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_STRIDE>
class ac_convNxN_stride_N_pad_0
{
public:
  // Constructor: (new empty)
  ac_convNxN_stride_N_pad_0():d_colCnt(0),d_vld(0),d_vldOut(0),d_sofReg(0),d_eofReg(0),d_solReg(0),d_eolReg(0),d_hStrideCnt(1),d_vStrideCnt(1),d_flush(0) {
    // This implementation does not pad if the input image does not divide equally by the window size and stride
    // The leftover values are simply discarded
    // Formula below shows how output volume can be calculated
    // May want to add an assert to detect this since currently line buffers are set based on input ROW and COL size
    // Output volume calculation based on image size, window size, stride, and padding
    // W1 = input image width
    // H1 = input image height
    // F  = window size, e.g. 3x3
    // P  = padding
    // S  = stride
    // W2(output width)=(W1-F+2P)/S+1
    // H2(output height)=(H1-F+2P)/S+1
    // W1 = W2*S-S+F-2P
  }

public: // Class Member Functions
  void reset() {
    d_colCnt = 0;
    d_vld = 0;
    d_sofReg = 0;
    d_eofReg = 0;
    d_solReg = 0;
    d_eolReg = 0;
    d_hStrideCnt = 1;
    d_vStrideCnt = 1;
  }

  bool canRead() { // This function is used to gate of the reading of new inputs from a blocking read during flush
    return !d_flush;
  }

  bool isValid() {
    return d_vldOut;
  }

  void filter(T din, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, T2 kernel[AC_WIN_SIZE][AC_WIN_SIZE], T2 &dout) {
    if (flagsIn.sol || (d_flush && d_colCnt == 0)) {
      d_sofReg <<= 1;
      d_sofReg[0] = flagsIn.sof;//track position of start of frame
    }
    if (flagsIn.eol || flagsIn.sol || (d_flush && d_colCnt == 0)) {
      d_eofReg <<= 1;
      d_eofReg[0] = flagsIn.eof;//track position of end of frame
    }
    if (flagsIn.eof) {
      d_flush = 1;
    }
    d_solReg <<= 1;//Shift sol and eol flags
    d_solReg[0] = flagsIn.sol || (d_flush && d_colCnt == 0);
    d_eolReg <<= 1;
    d_eolReg[0] = flagsIn.eol;

    if (flagsIn.sof) { //vertical stride and window position count reset
      d_vStrideCnt = 0;
      d_vWinCnt = 0;
      d_flush = 0;
    }
    if (flagsIn.sol || flagsIn.sof) { //horizontal stride and window position count reset
      d_colCnt = 0;//sol or sof forces column counter reset to beginning of line
      d_hStrideCnt = 0;
      d_hWinCnt = 0;
    }

    d_buf.rd_wr_buffer(d_colCnt, din, d_rowData);//Store in line buffers and get data from N rows

#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) { // Shift a column of data from the line buffers into the window registers
      d_reg[i] << d_rowData[i];
    }
#pragma unroll yes
    for (int j=0; j<AC_WIN_SIZE; j++) { //Lay the window registers and kernel out linearly by column
#pragma unroll yes
      for (int i=0; i<AC_WIN_SIZE; i++) {
        d_regVect[j*AC_WIN_SIZE+i] = d_reg[i][j];
        d_kernelVect[j*AC_WIN_SIZE+i] = kernel[i][j];
      }
    }

    flags.sof = d_sofReg[AC_WIN_SIZE-1] & d_solReg[AC_WIN_SIZE-1];//generate output flags
    flags.eof = d_eofReg[0];
    flags.sol = d_solReg[AC_WIN_SIZE-1];
    flags.eol = d_eolReg[0];
    flagsOut = flags;

    if (d_hWinCnt == AC_WIN_SIZE-AC_STRIDE && d_vWinCnt == AC_WIN_SIZE-1) { //First window valid signal
      d_vld = 1;
      d_hStrideCnt = 0;
      acc = 0;
      d_regVectMux = 0;
      d_kernelVectMux = 0;
    }

    //Time multiplex kernel computation over stride number of cycles
#pragma unroll yes
    for (int t=0; t<AC_STRIDE; t++) {
      if (d_hStrideCnt == t && d_vld) { //gate computation
#pragma unroll yes
        for (int i=(AC_WIN_SIZE*AC_WIN_SIZE/AC_STRIDE)*t, p=0; i<(t+1)*AC_WIN_SIZE*AC_WIN_SIZE/AC_STRIDE; i++,p++) {
          d_regVectMux[p] = d_regVect[i+(AC_STRIDE-1 -t)*AC_WIN_SIZE];
          d_kernelVectMux[p] = d_kernelVect[i];
        }
      }
    }

    // Time multiplex kernel computation over stride number of cycles
    if ( d_vld) { //gate computation
#pragma unroll yes
      for (int i=0; i<AC_WIN_SIZE*AC_WIN_SIZE/AC_STRIDE+1; i++) {
        acc += d_regVectMux[i]*d_kernelVectMux[i];
      }
    }

    dout = acc;
    // Valid out depends on stride counters and window having ramped up d_vld==1
    d_vldOut = d_vld && d_hStrideCnt==AC_STRIDE-1 && d_vWinCnt==AC_WIN_SIZE-1 && d_colCnt >=AC_WIN_SIZE-1;

    if (d_eofReg[AC_WIN_SIZE-1]) { //disable at end of frame
      d_vld = 0;
    }

    if (d_colCnt == AC_NUM_COLS-1) { //Track stride position in window
      d_colCnt = 0;
      d_vld = 0;
      if (d_vStrideCnt == AC_STRIDE-1) { //resets at stride-1
        d_vStrideCnt = 0;
      } else {
        d_vStrideCnt++;
      }
      if (d_vWinCnt == AC_WIN_SIZE-1) { // window count resets to AC_WIN_SIZE-stride
        d_vWinCnt = AC_WIN_SIZE-AC_STRIDE;
      } else {
        d_vWinCnt++;
      }
    } else {
      d_colCnt++;
    }
    if (d_hStrideCnt == AC_STRIDE-1) { // resets at stride-1
      d_hStrideCnt = 0;
    } else {
      d_hStrideCnt++;
    }
    if (d_hWinCnt == AC_WIN_SIZE-1) { //window count resets to AC_WIN_SIZE-stride
      d_hWinCnt = AC_WIN_SIZE-AC_STRIDE;
    } else {
      d_hWinCnt++;
    }
  }

  ac_int<ac::log2_ceil<AC_WIN_SIZE>::val,false> getStrideCntH() {
    return d_hStrideCnt;
  }

  ac_int<ac::log2_ceil<AC_WIN_SIZE>::val,false> getStrideCntV() {
    return d_vStrideCnt;
  }

  void print() { //Print the 3x3 window data
#ifndef __SYNTHESIS__
    for (int i=0; i<AC_WIN_SIZE; i++) {
      d_reg[i].print();
    }
#endif
  }

  void print_flags() {
#ifndef __SYNTHESIS__
    printf("vld = %d  sof = %d eof = %d sol = %d eol = %d\n\n",d_vld,flags.sof,flags.eof,flags.sol,flags.eol);
#endif
  }

private: // data
  ac_line_buffer<T,AC_WIN_SIZE,AC_NUM_COLS> d_buf;
  ac_shift_reg<T, AC_WIN_SIZE> d_reg[AC_WIN_SIZE];
  T d_regVect[AC_WIN_SIZE*AC_WIN_SIZE];
  ac_array<T,AC_WIN_SIZE *AC_WIN_SIZE/AC_STRIDE+1> d_regVectMux;
  T2 d_kernelVect[AC_WIN_SIZE*AC_WIN_SIZE];
  ac_array<T2,AC_WIN_SIZE *AC_WIN_SIZE/AC_STRIDE+1> d_kernelVectMux;
  ac_int<AC_WIN_SIZE,false> d_sofReg;
  ac_int<AC_WIN_SIZE,false> d_eofReg;
  ac_int<AC_WIN_SIZE,false> d_solReg;
  ac_int<AC_WIN_SIZE,false> d_eolReg;
  ac_array<T,AC_WIN_SIZE> d_rowData;
  ac_flags_struct flags;
  bool d_vld;
  bool d_vldOut;
  bool d_flush;//used to flush window after eof and no sof
  static const int NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE + 2*0)/AC_STRIDE+1;
  static const int NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + 2*0)/AC_STRIDE+1;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_hStrideCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_vStrideCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_hWinCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_vWinCnt;
  T2 acc;
};

//=========================================================================
// Class: ac_convNxN_stride_1_pad_N
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_convNxN_stride_1_pad_N
{
public:
  // Constructor: (new empty)
  ac_convNxN_stride_1_pad_N():d_vldOut(0) { }

public: // Class Member Functions
  void reset() { }

  bool canRead() {
    return d_window.canRead();
  }

  bool isValid() {
    return d_vldOut;
  }

  void filter(T din, bool vldIn, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, T2 kernel[AC_WIN_SIZE][AC_WIN_SIZE], T2 &dout) {
    d_window.slide_window(din,vldIn,flagsIn,flagsOut,d_win);
    d_vldOut = d_window.isValid();

    if (d_vldOut) {
      T2 acc = 0;
#pragma unroll yes
      COPY_ROW:for (int i=0; i<AC_WIN_SIZE; i++) { //Make linear vectors of d_win and kernel.This allows fine grained loop unrolling
#pragma unroll yes
        COPY_COL:for (int j=0; j<AC_WIN_SIZE; j++) {
          d_winVect[i*AC_WIN_SIZE+j] = d_win[i][j];
          d_kernelVect[i*AC_WIN_SIZE+j] = kernel[i][j];
        }
      }
      MAC:for (int i=0; i<AC_WIN_SIZE*AC_WIN_SIZE; i++) { //This loop can be unrolled for performance
        acc += d_winVect[i] * d_kernelVect[i];
      }
      dout = acc;
    }
  }

private: // data
  ac_window2d<T,AC_WIN_SIZE,AC_NUM_ROWS,AC_NUM_COLS,1,AC_BOUNDARY,AC_BOUNDS_CONST> d_window;
  ac_array<T,AC_WIN_SIZE,AC_WIN_SIZE> d_win;
  T d_winVect[AC_WIN_SIZE*AC_WIN_SIZE];
  T2 d_kernelVect[AC_WIN_SIZE*AC_WIN_SIZE];
  ac_flags_struct d_flags;
  bool d_vldOut;
};

//=========================================================================
// Class: ac_conv1xN
//   Horizontal convolution, stride 1, programmable padding on boundary
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_conv1xN
{
public:
  // Constructor: (new empty)
  ac_conv1xN():d_vldOut(0) { }

public: // Class Member Functions
  void reset() { }

  bool canRead() {
    return d_window.canRead();
  }

  bool isValid() {
    return d_vldOut;
  }

  T2 getData() {
    return d_dout;
  }

  void setFlags(ac_flags_struct flags) {
    d_flagsIn = flags;
  }

  ac_flags_struct getFlags() {
    return d_flagsOut;
  }

  bool getEol() {
    return d_flagsOut.eol;
  }

  axi4VideoStream<T2,1> getAXI4Video() {
    axi4VideoStream<T2,1> tmp;
    tmp.data = d_dout;
    tmp.TUSER = d_flagsOut.sof;
    tmp.TLAST = d_flagsOut.eol;
    return tmp;
  }

  void filter(T din, bool vldIn, const T2 kernel[AC_WIN_SIZE]) {
    d_window.slide_window(din,vldIn,d_flagsIn,d_flagsOut,d_win);
    d_vldOut = d_window.isValid();

    if (d_vldOut) {
      T2 acc = 0;
#pragma unroll yes
      COPY_COL:for (int j=0; j<AC_WIN_SIZE; j++) {
        d_winVect[j] = d_win[j];
        d_kernelVect[j] = kernel[j];
      }
      MAC:for (int i=0; i<AC_WIN_SIZE; i++) { // This loop can be unrolled for performance
        acc += d_winVect[i] * d_kernelVect[i];
      }
      d_dout = acc;
    }
  }

  void filter_stream_IF(ac_channel<axi4VideoStream<T,1> > &din,const T2 kernel[AC_WIN_SIZE],ac_channel<axi4VideoStream<T2,1> > &dout) {
    ac_flag_gen<AC_NUM_ROWS,AC_NUM_COLS> flagsGen;//Flag generator from axi video stream interface
    axi4VideoStream<T,1> pixTmp;
#ifndef __SYNTHESIS__
    while (din.available(AC_NUM_COLS))
#endif
    {
      //If image available
      PIXEL:do {
        if (canRead()) { // Only read valid frame data, then wait till flush finishes
          pixTmp = din.read(); //Read streaming interface
        }
        flagsGen.generate_flags(pixTmp.TUSER,pixTmp.TLAST,d_flagsIn);
        setFlags(d_flagsIn);
        filter(pixTmp.data,true,kernel);
        if (isValid()) {
          dout.write(getAXI4Video());
        }
      } while (!getEol()); //Run until eof received from conv filter
    }
  }

private: // data
  ac_window1xN_stride_1_pad_N<T,AC_WIN_SIZE,AC_NUM_COLS,AC_BOUNDARY> d_window;
  ac_array<T,AC_WIN_SIZE> d_win;
  T d_winVect[AC_WIN_SIZE];
  T2 d_kernelVect[AC_WIN_SIZE];
  ac_flags_struct d_flagsIn;
  ac_flags_struct d_flagsOut;
  bool d_vldOut;
  T2 d_dout;
};

//=========================================================================
// Class: ac_conv1xN_stride_1_pad_N_stream
//   Horizonatl convolution, stride 1, programmable padding on boundary
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_conv1xN_stride_1_pad_N_stream
{
public:
  // Constructor: (new empty)
  ac_conv1xN_stride_1_pad_N_stream(const T2 (&kernel)[AC_WIN_SIZE]):d_kernel(kernel) {}

public: // Class Member Functions

#pragma hls_design interface
  void filter(ac_channel<axi4VideoStream<T,1> > &din,ac_channel<axi4VideoStream<T2,1> > &dout) {
    ac_flag_gen<AC_NUM_ROWS,AC_NUM_COLS> flagsGen;//Flag generator from axi video stream interface
    axi4VideoStream<T,1> pixTmp;
#ifndef __SYNTHESIS__
    while (din.available(AC_NUM_COLS))
#endif
    {
      //If image available
      PIXEL:do {
        if (conv.canRead()) { // Only read valid frame data, then wait till flush finishes
          pixTmp = din.read(); // Read streaming interface
        }
        flagsGen.generate_flags(pixTmp.TUSER,pixTmp.TLAST,d_flagsIn);
        conv.setFlags(d_flagsIn);
        conv.filter(pixTmp.data,true,d_kernel);
        if (conv.isValid()) {
          dout.write(conv.getAXI4Video());
        }
      } while (!conv.getEol()); //Run until eof received from conv filter
    }
  }

private: // data
  ac_conv1xN<T,T2,AC_WIN_SIZE,AC_NUM_ROWS,AC_NUM_COLS,AC_MIRROR_101> conv;
  ac_flags_struct d_flagsIn;
  const T2 d_kernel[AC_WIN_SIZE];
};

//=========================================================================
// Class: ac_convNx1
//
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_convNx1
{
public:
  // Constructor: (new empty)
  ac_convNx1():d_vldOut(0) { }

public: // Class Member Functions
  void reset() { }

  bool canRead() {
    return d_window.canRead();
  }

  bool isValid() {
    return d_vldOut;
  }

  T getPassThroughData() {
    return d_win[AC_WIN_SIZE/2];
  }

  axi4VideoStream<T,1> getAXI4VideoPassThrough() {
    axi4VideoStream<T,1> tmp;
    tmp.data =  d_win[AC_WIN_SIZE/2];
    tmp.TUSER = d_flagsOut.sof;
    tmp.TLAST = d_flagsOut.eol;
    return tmp;
  }

  T2 getData() {
    return d_dout;
  }

  void setFlags(ac_flags_struct flags) {
    d_flagsIn = flags;
  }

  ac_flags_struct getFlags() {
    return d_flagsOut;
  }

  bool getEof() {
    return d_flagsOut.eof;
  }

  axi4VideoStream<T2,1> getAXI4Video() {
    axi4VideoStream<T2,1> tmp;
    tmp.data = d_dout;
    tmp.TUSER = d_flagsOut.sof;
    tmp.TLAST = d_flagsOut.eol;
    return tmp;
  }

  void filter(T &din, bool vldIn, const T2 kernel[AC_WIN_SIZE]) {
    d_window.slide_window(din,vldIn,d_flagsIn,d_flagsOut,d_win);
    d_vldOut = d_window.isValid();

    if (d_vldOut) {
      T2 acc = 0;
#pragma unroll yes
      COPY_ROW:for (int i=0; i<AC_WIN_SIZE; i++) { //Make linear vectors of d_win and kernel.This allows fine grained loop unrolling
        d_winVect[i] = d_win[i];
        d_kernelVect[i] = kernel[i];
      }
      MAC:for (int i=0; i<AC_WIN_SIZE; i++) { //This loop can be unrolled for performance
        acc += d_winVect[i] * d_kernelVect[i];
      }
      d_dout = acc;
    }
  }

  void filter_stream_IF(ac_channel<axi4VideoStream<T,1> > &din, const T2 kernel[AC_WIN_SIZE], ac_channel<axi4VideoStream<T2,1> > &dout,ac_channel<axi4VideoStream<T,1> > &passThrough) {
    ac_flag_gen<AC_NUM_ROWS,AC_NUM_COLS> flagsGen;//Flag generator from axi video stream interface
    axi4VideoStream<T,1> pixTmp;

    if (din.available(AC_NUM_ROWS*AC_NUM_COLS)) { //If image available
      PIXEL:do {
        if (canRead()) { //Only read valid frame data, then wait till flush finishes
          pixTmp = din.read(); //Read streaming interface
        }
        flagsGen.generate_flags(pixTmp.TUSER,pixTmp.TLAST,d_flagsIn);
        setFlags(d_flagsIn);
        filter(pixTmp.data,true,kernel);
        if (isValid()) {
          dout.write(getAXI4Video());
          passThrough.write(getAXI4VideoPassThrough());
        }
      } while (!getEof()); //Run until eof received from conv filter
    }
  }

private: // data
  ac_windowNx1_stride_1_pad_N<T,AC_WIN_SIZE,AC_NUM_ROWS,AC_NUM_COLS,AC_BOUNDARY> d_window;
  ac_array<T,AC_WIN_SIZE> d_win;
  T d_winVect[AC_WIN_SIZE];
  T2 d_kernelVect[AC_WIN_SIZE];
  ac_flags_struct d_flagsIn;
  ac_flags_struct d_flagsOut;
  bool d_vldOut;
  T2 d_dout;
};

//=========================================================================
// Class: ac_convNx1_stride_1_pad_N_stream
//
//
//-------------------------------------------------------------------------

template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_convNx1_stride_1_pad_N_stream
{
public:
  // Constructor: (new empty)
  ac_convNx1_stride_1_pad_N_stream(const T2 (&kernel)[AC_WIN_SIZE]):d_kernel(kernel) {}

public: // Class Member Functions

#pragma hls_design interface
  void filter(ac_channel<axi4VideoStream<T,1> > &din, ac_channel<axi4VideoStream<T2,1> > &dout,ac_channel<axi4VideoStream<T,1> > &passThrough) {
    ac_flag_gen<AC_NUM_ROWS,AC_NUM_COLS> flagsGen;//Flag generator from axi video stream interface
    axi4VideoStream<T,1> pixTmp;

    if (din.available(AC_NUM_ROWS*AC_NUM_COLS)) { //If image available
      PIXEL:do {
        if (conv.canRead()) { // Only read valid frame data, then wait till flush finishes
          pixTmp = din.read(); //Read streaming interface
        }
        flagsGen.generate_flags(pixTmp.TUSER,pixTmp.TLAST,d_flagsIn);
        conv.setFlags(d_flagsIn);
        conv.filter(pixTmp.data,true,d_kernel);
        if (conv.isValid()) {
          dout.write(conv.getAXI4Video());
          passThrough.write(conv.getAXI4VideoPassThrough());
        }
      } while (!conv.getEof()); //Run until eof received from conv filter
    }
  }

private: // data
  ac_convNx1<T,T2,AC_WIN_SIZE,AC_NUM_ROWS,AC_NUM_COLS,AC_MIRROR_101> conv;
  ac_flags_struct d_flagsIn;
  const T2 d_kernel[AC_WIN_SIZE];
};

//=========================================================================
// Class: conv2d_v
//
//
//-------------------------------------------------------------------------

#pragma hls_design
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS,int PREV_IN_CHANNELS>
class conv2d_v
{
public:
  // Constructor: (new empty)
  conv2d_v() {}

public: // Class Member Functions

#pragma hls_design interface
  void window(ac_channel<axi4VideoStream<T,1> > &din, ac_channel<ac_array<T,3> > &dout) {
    ac_boundary<T,3,AC_CONSTANT,0> bounds;
    ac_flags_struct flags,flagsOut;
    uint3 lower=0;
    uint3 upper=0;
    axi4VideoStream<T,1> dinTmp;
    ac_array<T,3> dat;

    for (int r=0; r<IN_HEIGHT+1; r++) {
      lower <<= 1;
      lower[0] = (r==0);
      upper <<= 1;
      upper[0] = (r==IN_HEIGHT-1);
      for (int v=0; v<PREV_IN_CHANNELS; v++) {
        for (int c=0; c<IN_WIDTH; c++) {
          for (int chan=0; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            if (r<IN_HEIGHT) {
              pass(dinTmp) = din.read();
            }
            if (chan==0) {
              d_buf.rd_wr_buffer(v,c,dinTmp.data,dat);
              bounds.apply_bounds(lower,upper,dat,dat);
              if (r>=1) {
                dout.write(dat);
              }
            } else {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,true,dinTmp.data,dat);
            }
          }
        }
        if (r>=1) {
          for (int chan=1; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            for (int c=0; c<IN_WIDTH; c++) {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,false,dinTmp.data,dat);
              bounds.apply_bounds(lower,upper,dat,dat);
              if (r>=1) {
                dout.write(dat);
              }
            }
          }
        }
      }
    }
  }

private: // data
  ac_line_buffer3_N_ni<T,IN_WIDTH,PREV_IN_CHANNELS> d_buf;
  ac_line_buffer3_N<T,IN_WIDTH,IN_CHANNELS> d_buf2;
};

//Convolution output can be deinterleaved as follows
//for(int i=0;i<IN_HEIGHT;i++){
//	OUT_CHAN:for(int c=0;c<3;c++){//c==OUT_CHANNELS when IN_CHANNELS==OUT_CHANNELS, other wise c==IN_CHANNELS
//		for(int j=0;j<IN_WIDTH;j++){
//			for(int p=0;p<2;p++){//OUT_CHANNELS/IN_CHANNELS
//				uint8 tmp = imgOuts.read().data;
//				imgO[c*2+p][i][j] = tmp;

//=========================================================================
// Class: conv2d_h
//
//
//-------------------------------------------------------------------------

#pragma hls_design
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int OUT_CHANNELS,int PREV_IN_CHANNELS>
class conv2d_h
{
public:
  // Constructor: (new empty)
  conv2d_h() {}

public: // Class Member Functions

#pragma hls_design interface
  void filter(ac_channel<ac_array<T,3> > &din, const constArrStruct<OUT_CHANNELS> &kernel0_in, ac_channel<axi4VideoStream<T,1> > &dout) {
    ac_boundary<T,3,AC_CONSTANT,0> bounds;
    uint3 lower=0;
    uint3 upper=0;
    ac_array<T,3> dinTmp;
    ac_flags_struct flags,flagsOut;
    ac_array<T,3,3> dat;
    ac_int<T::width+ac::log2_ceil<3*3>::val,T::sign> acc=0;//TODO fix signedness
    axi4VideoStream<T,1> outTmp;
    constArrStruct<OUT_CHANNELS> kernel0Tmp = kernel0_in;
    for (int r=0; r<IN_HEIGHT; r++) {
      for (int chan=0; chan<IN_CHANNELS; chan++) {
        for (int c=0; c<IN_WIDTH+1; c++) {
          flags.sol = (c==0);
          flags.eol = (c==IN_WIDTH-1);
          if (c<IN_WIDTH) {
            dinTmp = din.read();
          }
#pragma unroll yes
          for (int i=0; i<3; i++) {
            window[i].slide_window(dinTmp[i],true,flags,flagsOut,dat[i]);
          }
          if (c>0) {
            for (int outChan=0; outChan<OUT_CHANNELS/IN_CHANNELS; outChan++) {
              acc=0;
              for (int i=0; i<3; i++) {
                for (int j=0; j<3; j++) {
                  acc+= dat[i][j] * kernel0_in.data[chan*OUT_CHANNELS/IN_CHANNELS+outChan][i][j];
                }
              }
              outTmp.data = acc;
              outTmp.TUSER = (r==0)&&(c==1);
              outTmp.TLAST = (c==IN_WIDTH);
              dout.write(outTmp);
            }
          }
        }
      }
    }
  }

private: // data
  ac_window1xN_stride_1_pad_N<T,3,IN_WIDTH,AC_CONSTANT> window[3];
};

//=========================================================================
// Class: ac_conv2d
//
//
//-------------------------------------------------------------------------

//#pragma hls_design
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int OUT_CHANNELS,int PREV_IN_CHANNELS>
class ac_conv2d
{
public:
  // Constructor: (new empty)
  ac_conv2d() {}

public: // Class Member Functions

#pragma hls_design
  void windowV(ac_channel<axi4VideoStream<T,1> > &din, ac_channel<ac_array<T,3> > &dout) {
    ac_boundary<T,3,AC_CONSTANT,0> bounds;
    ac_flags_struct flags,flagsOut;
    uint3 lower=0;
    uint3 upper=0;
    axi4VideoStream<T,1> dinTmp;
    ac_array<T,3> dat;
    for (int r=0; r<IN_HEIGHT+1; r++) {
      lower <<= 1;
      lower[0] = (r==0);
      upper <<= 1;
      upper[0] = (r==IN_HEIGHT-1);
      for (int v=0; v<PREV_IN_CHANNELS; v++) {
        for (int c=0; c<IN_WIDTH; c++) {
          for (int chan=0; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            if (r<IN_HEIGHT) {
              pass(dinTmp) = din.read();
            }

            if (chan==0) {
              d_buf.rd_wr_buffer(v,c,dinTmp.data,dat);
              bounds.apply_bounds(lower,upper,dat,dat);
              if (r>=1) {
                dout.write(dat);
              }
            } else {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,true,dinTmp.data,dat);
            }
          }
        }
        if (r>=1) {
          for (int chan=1; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            for (int c=0; c<IN_WIDTH; c++) {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,false,dinTmp.data,dat);
              bounds.apply_bounds(lower,upper,dat,dat);
              if (r>=1) {
                dout.write(dat);
              }
            }
          }
        }
      }
    }
  }

#pragma hls_design
  void filter(ac_channel<ac_array<T,3> > &din, const constArrStruct<OUT_CHANNELS> &kernel0_in, ac_channel<axi4VideoStream<T,1> > &dout) {
    ac_boundary<T,3,AC_CONSTANT,0> bounds;
    uint3 lower=0;
    uint3 upper=0;
    ac_array<T,3> dinTmp;
    ac_flags_struct flags,flagsOut;
    ac_array<T,3,3> dat;
    ac_int<T::width+ac::log2_ceil<3*3>::val,T::sign> acc=0;//TODO fix signedness
    axi4VideoStream<T,1> outTmp;
    constArrStruct<OUT_CHANNELS> kernel0Tmp = kernel0_in;
    for (int r=0; r<IN_HEIGHT; r++) {
      for (int chan=0; chan<IN_CHANNELS; chan++) {
        for (int c=0; c<IN_WIDTH+1; c++) {
          flags.sol = (c==0);
          flags.eol = (c==IN_WIDTH-1);
          if (c<IN_WIDTH) {
            dinTmp = din.read();
          }
#pragma unroll yes
          for (int i=0; i<3; i++) {
            window[i].slide_window(dinTmp[i],true,flags,flagsOut,dat[i]);
          }
          if (c>0) {
            for (int outChan=0; outChan<OUT_CHANNELS/IN_CHANNELS; outChan++) {
              acc=0;
              for (int i=0; i<3; i++) {
                for (int j=0; j<3; j++) {
                  acc+= dat[i][j] * kernel0_in.data[chan*OUT_CHANNELS/IN_CHANNELS+outChan][i][j];
                }
              }
              outTmp.data = acc;
              outTmp.TUSER = (r==0)&&(c==1);
              outTmp.TLAST = (c==IN_WIDTH);
              dout.write(outTmp);
            }
          }
        }
      }
    }
  }

#pragma hls_design interface
  void filter(ac_channel<axi4VideoStream<T,1> > &din, const constArrStruct<OUT_CHANNELS> &kernel0_in, ac_channel<axi4VideoStream<T,1> > &dout) {
    windowV(din,conn);
    filter(conn,kernel0_in,dout);
  }


private: // data
  ac_channel<ac_array<T,3> > conn;
  ac_line_buffer3_N_ni<T,IN_WIDTH,PREV_IN_CHANNELS> d_buf;
  ac_line_buffer3_N<T,IN_WIDTH,IN_CHANNELS> d_buf2;
  ac_window1xN_stride_1_pad_N<T,3,IN_WIDTH,AC_CONSTANT> window[3];
};

//=========================================================================
// Class: max_pool_v
//
//
//-------------------------------------------------------------------------

#pragma hls_design
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS,int PREV_IN_CHANNELS>
class max_pool_v
{
public:
  // Constructor: (new empty)
  max_pool_v() {}

public: // Class Member Functions

#pragma hls_design interface
  void window(ac_channel<axi4VideoStream<T,1> > &din, ac_channel<ac_array<T,2> > &dout) {
    ac_flags_struct flags,flagsOut;
    uint3 lower=0;
    uint3 upper=0;
    axi4VideoStream<T,1> dinTmp;
    ac_array<T,2> dat;
    for (int r=0; r<IN_HEIGHT; r++) {
      lower <<= 1;
      lower[0] = (r==0);
      upper <<= 1;
      upper[0] = (r==IN_HEIGHT-1);
      for (int v=0; v<PREV_IN_CHANNELS; v++) {
        for (int c=0; c<IN_WIDTH; c++) {
          for (int chan=0; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            dinTmp = din.read();
            if (chan==0) {
              d_buf.rd_wr_buffer(v,c,dinTmp.data,dat);
              if (r&1) {
                dout.write(dat);
              }
            } else {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,true,dinTmp.data,dat);
            }
          }
        }
        if (r&1) {
          for (int chan=1; chan<IN_CHANNELS/PREV_IN_CHANNELS; chan++) {
            for (int c=0; c<IN_WIDTH; c++) {
              d_buf2.rd_wr_buffer(v*IN_CHANNELS/PREV_IN_CHANNELS+chan-1,c,false,dinTmp.data,dat);
              if (r&1) {
                dout.write(dat);
              }
            }
          }
        }
      }
    }
  }

private: // data
  ac_line_buffer2<T,IN_WIDTH,PREV_IN_CHANNELS> d_buf;
  ac_line_buffer2_N<T,IN_WIDTH,IN_CHANNELS> d_buf2;
};

//=========================================================================
// Class: max_pool_h
//
//
//-------------------------------------------------------------------------

#pragma hls_design
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS,int PREV_IN_CHANNELS>
class max_pool_h
{
public:
  // Constructor: (new empty)
  max_pool_h() {}

public: // Class Member Functions

#pragma hls_design interface
  void filter(ac_channel<ac_array<uint8,2> > &din, ac_channel<axi4VideoStream<uint8,1> > &dout) {
    uint3 lower=0;
    uint3 upper=0;
    ac_array<uint8,2> dinTmp;
    ac_flags_struct flags,flagsOut;
    int max=0;//TODO quantize
    axi4VideoStream<uint8,1> outTmp;
    for (int r=0; r<IN_HEIGHT/2; r++) {
      for (int chan=0; chan<IN_CHANNELS; chan++) {
        for (int c=0; c<IN_WIDTH; c++) {
          flags.sol = (c==0);
          flags.eol = (c==IN_WIDTH-1);
          dinTmp = din.read();
#pragma unroll yes
          for (int i=0; i<2; i++) {
            window[i] << dinTmp[i];;
          }
          if (c&1) {
            max=0;
            for (int i=0; i<2; i++) {
              for (int j=0; j<2; j++) {
                if (window[i][j]>max) {
                  max = window[i][j];
                }
              }
            }
            outTmp.data = max;
            outTmp.TUSER = (r==0)&&(c==1);
            outTmp.TLAST = (c==IN_WIDTH);
            dout.write(outTmp);
          }
        }
      }
    }
  }

private: // data
  ac_shift_reg<uint8,2> window[2];
};

#endif

