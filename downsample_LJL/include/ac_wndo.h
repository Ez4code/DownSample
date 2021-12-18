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
// File: ac_wndo.h
//
// Description:
//
// Usage:
//
// Notes:
//    Relies on the ac_array class.
//
// Revision History:
//
//***************************************************************************

#ifndef _INCLUDED_AC_WNDO_H_
#define _INCLUDED_AC_WNDO_H_

#include <ac_ipl/ac_line_buffer.h>
#include <ac_ipl/ac_shift_reg.h>
#include <ac_ipl/ac_boundary.h>

// TODO check dflush logic, not being cleared after flush
struct ac_flags_struct {
  bool sof;
  bool eof;
  bool sol;
  bool eol;
  ac_flags_struct():sof(0),eof(0),sol(0),eol(0) {}
  ac_flags_struct(int i):sof(i),eof(i),sol(i),eol(i) {}
};

template<typename T>
struct ac_data_w_flags {
  T data;
  ac_flags_struct flags;
};

//=========================================================================
// Class: ac_flag_gen
//
//-------------------------------------------------------------------------

template<int AC_NUM_ROWS, int AC_NUM_COLS>
class ac_flag_gen
{
public:
  ac_flag_gen():d_flags(0),d_rowCnt(0),d_colCnt(0), d_err(0), d_state(SEEK_SOF) {}

  bool generate_flags(bool sof, bool eol, ac_flags_struct &flagsOut) {
    switch (d_state) {
      case SEEK_SOF:
        d_flags.eof = 0;
        d_flags.eol = 0;
        if (eol) {
          d_err = 1;
        } else if (sof) {
          d_state = SEEK_EOL;
          d_flags.sof = 1;
          d_flags.sol = 1;
          d_flags.eof = 0;
          d_flags.eol = 0;
          d_colCnt++;
        }
        break;
      case SEEK_EOL:
        d_flags.sof = 0;
        d_flags.sol = 0;
        d_flags.eol = 0;
        if (d_colCnt ==0) {
          d_flags.sol = 1;
        }
        if (eol && d_colCnt < AC_NUM_COLS-1) { // early end of line
          d_err = 1;
        } else if (sof) { // early start of frame;
          d_err = 1;
        } else if (eol && d_colCnt == AC_NUM_COLS-1) { // end of line
          if (d_rowCnt == AC_NUM_ROWS-1) { // end of frame
            d_colCnt = 0;
            d_flags.eof = 1;
            d_rowCnt = 0;
            //					if(AC_FLUSH)
            //						d_state = FLUSH;
            //					else
            d_state = SEEK_SOF;
          } else {
            d_rowCnt ++;
          }
          d_flags.eol = 1;
          d_colCnt = 0;
        } else if (d_colCnt >= AC_NUM_COLS) { // late end of line
          d_err = 1;
        } else {
          d_colCnt++;
        }
        break;
    }

    flagsOut = d_flags;
    return d_err;
  }

  void print() {
#ifndef __SYNTHESIS__
    printf("Gen: sof = %d eof = %d sol = %d eol = %d\n\n",d_flags.sof,d_flags.eof,d_flags.sol,d_flags.eol);
#endif
  }

private: // data
  ac_flags_struct d_flags;
  ac_int<ac::log2_ceil<AC_NUM_ROWS>::val,false> d_rowCnt;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
  bool d_err;
  enum STATE {SEEK_SOF=0, SEEK_EOL=1};
  STATE d_state;
};

//=========================================================================
// Class: ac_window2d
//
//-------------------------------------------------------------------------

template<typename T, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_STRIDE, int AC_BOUNDARY, int AC_BOUNDS_CONST=0, bool AC_SINGLEPORT=false>
class ac_window2d
{
public:
  // Constructor: (new empty)
  ac_window2d():d_din(0),d_colCnt(0),d_vld(0),d_vldOut(0),d_sofReg(0),d_eofReg(0),d_solReg(0),d_eolReg(0),d_hStrideCnt(1),d_vStrideCnt(1),d_flush(0) {
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

  void slide_window(T din, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, ac_array<T,AC_WIN_SIZE,AC_WIN_SIZE> &dout) {
    d_buf.rd_wr_buffer(d_colCnt, d_din, d_rowData); // Store in line buffers and get data from N rows

#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) { // Shift a column of data from the line buffers into the window registers
      d_reg[i] << d_rowData[i];
    }

#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) { // windowed data
      dout[i] = d_reg[i].get();
    }
    flags.sof = d_sofReg[AC_WIN_SIZE-1] & d_solReg[AC_WIN_SIZE-1]; // generate output flags
    flags.eof = d_eofReg[0];
    flags.sol = d_solReg[AC_WIN_SIZE-1];
    flags.eol = d_eolReg[0];
    flagsOut = flags;

    // Valid out depends on stride counters and window having ramped up d_vld==1
    d_vldOut = d_vld && d_hStrideCnt==0 && d_vWinCnt==AC_WIN_SIZE-1 && d_colCnt >=AC_WIN_SIZE-1;
    d_hStrideCntCurr = d_hStrideCnt; // Store current stride counts/locations
    d_vStrideCntCurr = d_vStrideCnt;
    if (d_eofReg[AC_WIN_SIZE-1]) { // disable at end of frame
      d_vld = 0;
    }

    if (d_colCnt == AC_NUM_COLS-1) { // Track stride position in window
      d_colCnt = 0;
      d_vld = 0;
      if (d_vStrideCnt == AC_STRIDE-1) { // resets at stride-1
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
    if (d_hWinCnt == AC_WIN_SIZE-1) { // window count resets to AC_WIN_SIZE-stride
      d_hWinCnt = AC_WIN_SIZE-AC_STRIDE;
    } else {
      d_hWinCnt++;
    }
    if (flagsIn.sol || (d_flush && d_colCnt == 0)) {
      d_sofReg <<= 1;
      d_sofReg[0] = flagsIn.sof; // track position of start of frame
    }
    if (flagsIn.eol || flagsIn.sol || (d_flush && d_colCnt == 0)) {
      d_eofReg <<= 1;
      d_eofReg[0] = flagsIn.eof; // track position of end of frame
    }
    if (flagsIn.eof) {
      d_flush = 1;
    }
    d_solReg <<= 1; // Shift sol and eol flags
    d_solReg[0] = flagsIn.sol || (d_flush && d_colCnt == 0);
    d_eolReg <<= 1;
    d_eolReg[0] = flagsIn.eol;

    if (flagsIn.sof) { // vertical stride and window position count reset
      d_vStrideCnt = 0;
      d_vWinCnt = 0;
      d_flush = 0;
    }
    if (flagsIn.sol || flagsIn.sof) { // horizontal stride and window position count reset
      d_colCnt = 0; // sol or sof forces column counter reset to beginning of line
      d_hStrideCnt = 0;
      d_hWinCnt = 0;
    }
    d_din = din;
    if (d_hWinCnt == AC_WIN_SIZE-1 && d_vWinCnt == AC_WIN_SIZE-1) { // First window valid signal
      d_vld = 1;
      d_hStrideCnt = 0;
    }
  }

  ac_int<ac::log2_ceil<AC_WIN_SIZE>::val,false> getStrideCntH() {
    return d_hStrideCntCurr;
  }

  ac_int<ac::log2_ceil<AC_WIN_SIZE>::val,false> getStrideCntV() {
    return d_vStrideCntCurr;
  }

  void print() { // Print the 3x3 window data
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
  ac_line_buffer<T,AC_WIN_SIZE,AC_NUM_COLS,AC_SINGLEPORT> d_buf;
  ac_shift_reg<T, AC_WIN_SIZE> d_reg[AC_WIN_SIZE];
  T d_regVect[AC_WIN_SIZE*AC_WIN_SIZE];
  ac_int<AC_WIN_SIZE,false> d_sofReg;
  ac_int<AC_WIN_SIZE,false> d_eofReg;
  ac_int<AC_WIN_SIZE,false> d_solReg;
  ac_int<AC_WIN_SIZE,false> d_eolReg;
  ac_array<T,AC_WIN_SIZE> d_rowData;
  ac_flags_struct flags;
  T d_din;
  bool d_vld;
  bool d_vldOut;
  bool d_flush; // used to flush window after eof and no sof
  static const int NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE + 2*0)/AC_STRIDE+1;
  static const int NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + 2*0)/AC_STRIDE+1;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_hStrideCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_vStrideCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_hStrideCntCurr;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_vStrideCntCurr;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_hWinCnt;
  ac_int<ac::nbits<AC_WIN_SIZE>::val,false> d_vWinCnt;
};

//=========================================================================
// Specialized Class: ac_window2d (w/ AC_STRIDE = 1)
//
//-------------------------------------------------------------------------

template<typename T, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY, int AC_BOUNDS_CONST, bool AC_SINGLEPORT>
class ac_window2d<T,AC_WIN_SIZE,AC_NUM_ROWS,AC_NUM_COLS,1,AC_BOUNDARY, AC_BOUNDS_CONST,AC_SINGLEPORT>
{
public:
  // Constructor: (new empty)
  ac_window2d():d_colCnt(0),d_vld(0),d_sofReg(0),d_eofReg(0),d_solReg(0),d_eolReg(0),d_sofDel(0),d_eofDel(0),d_flush(0),d_vldOut(0) {}

public: // Class Member Functions

  void reset() {
    d_colCnt = 0;
    d_vld = 0;
    d_sofReg = 0;
    d_eofReg = 0;
    d_solReg = 0;
    d_eolReg = 0;
    d_sofDel = 0;
    d_eofDel = 0;
  }

  // Apply left/right and top/bottom boundary conditions
  void apply_bounds() {
    ac_boundary<T,AC_WIN_SIZE,AC_BOUNDARY,AC_BOUNDS_CONST> bounds;
    ac_array<T,AC_WIN_SIZE> col;
#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) {
      d_win[i] = d_reg[i].get();
    }

    // left and right boundary processing
#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) {
      bounds.apply_bounds(d_solReg,d_eolReg,d_win[i],d_win[i]);
    }
#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) {
#pragma unroll yes
      for (int j=0; j<AC_WIN_SIZE; j++) {
        col[j] = d_win[j][i];
      }
      // top and bottom boundary processing
      bounds.apply_bounds(d_sofReg,d_eofReg,col,col);
#pragma unroll yes
      for (int j=0; j<AC_WIN_SIZE; j++) {
        d_win[j][i] = col[j];
      }
    }
  }

  bool canRead() {
    return !d_flush;
  }

  bool isValid() {
    return d_vldOut;
  }

  void slide_window(T din, bool vldIn, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, ac_array<T,AC_WIN_SIZE,AC_WIN_SIZE> &dout) {
    if (vldIn || d_flush) {
      if (d_solReg[AC_WIN_SIZE/2-1]) { // Wait to advance sof until window has ramped up
        d_sofReg <<= 1;
        d_sofReg[0]= d_sofDel[AC_WIN_SIZE/2-1];
      }
      if (d_solReg[AC_WIN_SIZE/2-1]) { // Wait to advance eof until window has ramped up
        d_eofReg <<= 1;
      }
      if (flagsIn.eof) {
        d_eofReg[0] = 1;
        d_flush = 1;
      }
      d_solReg <<= 1; // Shift start of line and end of line flags, used to process left/right boundary conditions
      d_solReg[0] = (flagsIn.sol && !d_flush) || (d_flush && d_colCnt == 0);
      d_eolReg <<= 1;
      d_eolReg[0] = (flagsIn.eol && !d_flush) || (d_flush && d_colCnt == AC_NUM_COLS-1);
      d_sofDel <<= 1;
      d_sofDel[0] = flagsIn.sof && !d_flush;
      d_eofDel <<= 1;
      d_eofDel[0] = flagsIn.eof && !d_flush; // TODO remove

      if ((flagsIn.sol || flagsIn.sof) && !d_flush) {
        d_colCnt = 0;
      }
      d_buf.rd_wr_buffer(d_colCnt, din, d_rowData); // Store in line buffers and get data from 3 rows
      if (d_colCnt == AC_NUM_COLS-1) {
        d_colCnt = 0;
      } else {
        d_colCnt++;
      }
#pragma unroll yes
      for (int i=0; i<AC_WIN_SIZE; i++) {
        d_reg[i] << d_rowData[i];
      }

      apply_bounds();
      dout = d_win;

      flags.sof = d_sofReg[AC_WIN_SIZE/2] & d_solReg[AC_WIN_SIZE/2];
      flags.eof = d_eofReg[AC_WIN_SIZE/2] & d_eolReg[AC_WIN_SIZE/2];
      flags.sol = d_solReg[AC_WIN_SIZE/2];
      flags.eol = d_eolReg[AC_WIN_SIZE/2];
      flagsOut = flags;
      if (d_eofReg[AC_WIN_SIZE/2] & d_eolReg[AC_WIN_SIZE/2]) {
        d_flush = 0;
      }
      if (d_sofReg[AC_WIN_SIZE/2] && d_solReg[AC_WIN_SIZE/2]) {
        d_vld = 1;
      }
      d_vldOut = d_vld;
      if (d_eofReg[AC_WIN_SIZE-1]) {
        d_vld = 0;
      }
    } else {
      d_vldOut = 0;
    }
  }

  void print() { // Print the 3x3 window data
#ifndef __SYNTHESIS__
    for (int i=0; i<AC_WIN_SIZE; i++) {
      for (int j=0; j<AC_WIN_SIZE; j++) {
        printf("%3d ", (int)d_win[i][j]);
      }
      printf("\n");
    }
#endif
  }

  void print_flags() {
#ifndef __SYNTHESIS__
    printf("vld = %d  sof = %d eof = %d sol = %d eol = %d\n\n",d_vld,flags.sof,flags.eof,flags.sol,flags.eol);
#endif
  }

private: // data
  ac_line_buffer<T,AC_WIN_SIZE,AC_NUM_COLS,AC_SINGLEPORT> d_buf;
  ac_shift_reg<T, AC_WIN_SIZE> d_reg[AC_WIN_SIZE];
  ac_int<AC_WIN_SIZE,false> d_sofReg;
  ac_int<AC_WIN_SIZE,false> d_eofReg;
  ac_int<AC_WIN_SIZE,false> d_solReg;
  ac_int<AC_WIN_SIZE,false> d_eolReg;
  ac_int<AC_WIN_SIZE,false> d_sofDel;
  ac_int<AC_WIN_SIZE,false> d_eofDel;
  ac_array<T,AC_WIN_SIZE> d_rowData;
  ac_array<T,AC_WIN_SIZE,AC_WIN_SIZE> d_win;
  ac_flags_struct flags;
  bool d_vld;
  bool d_vldOut;
  bool d_flush; // used to flush window after eof and no sof
  static const int NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE + 2*0)/1+1;
  static const int NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + 2*0)/1+1;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
};

//=========================================================================
// Class: ac_window1xN_stride_1_pad_N
//
//-------------------------------------------------------------------------

template<typename T, int AC_WIN_SIZE, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_window1xN_stride_1_pad_N
{
public:
  // Constructor: (new empty)
  ac_window1xN_stride_1_pad_N(): d_sofReg(0),d_eofReg(0),d_solReg(0),d_eolReg(0),d_sofDel(0),d_eofDel(0),d_vld(0),d_vldOut(0),d_flush(0),d_colCnt(0) {}

public: // Class Member Functions
  void reset() {
    d_colCnt = 0;
    d_vld = 0;
    d_sofReg = 0;
    d_eofReg = 0;
    d_solReg = 0;
    d_eolReg = 0;
    d_sofDel = 0;
    d_eofDel = 0;
  }

  // Apply left/right and top/bottom boundary conditions
  void apply_bounds() {
    ac_boundary<T,AC_WIN_SIZE,AC_BOUNDARY,AC_BOUNDS_CONST> bounds;
    ac_array<T,AC_WIN_SIZE> col;
#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) {
      d_win[i] = d_reg[i];
    }
    bounds.apply_bounds(d_solReg,d_eolReg,d_win,d_win);
  }

  bool canRead() {
    return !d_flush;
  }

  bool isValid() {
    return d_vldOut;
  }

  void slide_window(T din, bool vldIn, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, ac_array<T,AC_WIN_SIZE> &dout) {
    if (vldIn || d_flush) {
      if (flagsIn.eol) {
        d_flush = 1;
      }
      d_solReg <<= 1; // Shift start of line and end of line flags, used to process left/right boundary conditions
      d_solReg[0] = (flagsIn.sol &!d_flush);
      d_eolReg <<= 1;
      d_eolReg[0] = (flagsIn.eol);
      d_sofReg <<= 1;
      d_sofReg[0] = (flagsIn.sof);
      d_eofReg <<= 1;
      d_eofReg[0] = (flagsIn.eof);

      if (flagsIn.sol && !d_flush) {
        d_colCnt = 0;
      }

      if (d_colCnt == AC_NUM_COLS-1) {
        d_colCnt = 0;
      } else {
        d_colCnt++;
      }

      d_reg << din;

      apply_bounds();
      dout = d_win;

      flags.sol = d_solReg[AC_WIN_SIZE/2];
      flags.eol = d_eolReg[AC_WIN_SIZE/2];
      flags.sof = d_sofReg[AC_WIN_SIZE/2];
      flags.eof = d_eofReg[AC_WIN_SIZE/2];
      flagsOut = flags;
      if (d_eolReg[AC_WIN_SIZE/2]) {  // finished flushing TODO - add for others
        d_flush = 0;
      }
      if (d_solReg[AC_WIN_SIZE/2]) {
        d_vld = 1;
      }
      d_vldOut = d_vld;
      if (d_eolReg[AC_WIN_SIZE/2]) {
        d_vld = 0;
      }
    } else {
      d_vldOut = 0;
    }
  }

  void print() { // Print the 3x3 window data
#ifndef __SYNTHESIS__
    for (int i=0; i<AC_WIN_SIZE; i++) {
      for (int j=0; j<AC_WIN_SIZE; j++) {
        printf("%3d ", (int)d_win[i][j]);
      }
      printf("\n");
    }

#endif
  }
  void print_flags() {
#ifndef __SYNTHESIS__
    printf("vld = %d  sof = %d eof = %d sol = %d eol = %d\n\n",d_vld,flags.sof,flags.eof,flags.sol,flags.eol);
#endif
  }

private: // data
  ac_shift_reg<T, AC_WIN_SIZE> d_reg;
  ac_int<AC_WIN_SIZE,false> d_sofReg;
  ac_int<AC_WIN_SIZE,false> d_eofReg;
  ac_int<AC_WIN_SIZE,false> d_solReg;
  ac_int<AC_WIN_SIZE,false> d_eolReg;
  ac_int<AC_WIN_SIZE,false> d_sofDel;
  ac_int<AC_WIN_SIZE,false> d_eofDel;
  ac_array<T,AC_WIN_SIZE> d_rowData;
  ac_array<T,AC_WIN_SIZE> d_win;
  ac_flags_struct flags;
  bool d_vld;
  bool d_vldOut;
  bool d_flush; // used to flush window after eof and no sof
  static const int NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + 2*0)/1+1;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
};

//=========================================================================
// Class: ac_windowNx1_stride_1_pad_N
//
//-------------------------------------------------------------------------

template<typename T, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0, bool AC_SINGLEPORT=false>
class ac_windowNx1_stride_1_pad_N
{
public:
  // Constructor: (new empty)
  ac_windowNx1_stride_1_pad_N():d_sofReg(0),d_eofReg(0),d_solReg(0),d_eolReg(0),d_sofDel(0),d_eofDel(0),d_vld(0),d_vldOut(0),d_colCnt(0) {}

public: // Class Member Functions
  void reset() {
    d_colCnt = 0;
    d_vld = 0;
    d_sofReg = 0;
    d_eofReg = 0;
    d_solReg = 0;
    d_eolReg = 0;
    d_sofDel = 0;
    d_eofDel = 0;
  }

  // Apply left/right and top/bottom boundary conditions
  void apply_bounds() {
    ac_boundary<T,AC_WIN_SIZE,AC_BOUNDARY,AC_BOUNDS_CONST> bounds;
#pragma unroll yes
    for (int i=0; i<AC_WIN_SIZE; i++) {
      d_win[i] = d_reg[i];
    }
    // top and bottom boundary processing
    bounds.apply_bounds(d_sofReg,d_eofReg,d_win,d_win);
  }

  bool canRead() {
    return !d_flush;
  }

  bool isValid() {
    return d_vldOut;
  }

  void slide_window(T &din, bool vldIn, ac_flags_struct flagsIn, ac_flags_struct &flagsOut, ac_array<T,AC_WIN_SIZE> &dout) {
    if (vldIn || d_flush) {
      if (flagsIn.sol) { // Wait to advance sof until window has ramped up
        d_sofReg <<= 1;
        d_sofReg[0]= flagsIn.sof;
      }
      if (flagsIn.sol  || (d_flush && d_colCnt == 0)) { // Wait to advance eof until window has ramped up
        d_eofReg <<= 1;
      }
      if (flagsIn.eof) {
        d_eofReg[0] = 1;
        d_flush = 1;
      }
      d_solReg <<= 1; // Shift start of line and end of line flags, used to process left/right boundary conditions
      d_solReg[0] = (flagsIn.sol && !d_flush) || (d_flush && d_colCnt == 0);
      d_eolReg <<= 1;
      d_eolReg[0] = (flagsIn.eol && !d_flush) || (d_flush && d_colCnt == AC_NUM_COLS-1);
      d_sofDel <<= 1;
      d_sofDel[0] = flagsIn.sof && !d_flush;
      d_eofDel <<= 1;
      d_eofDel[0] = flagsIn.eof && !d_flush;

      if ((flagsIn.sol || flagsIn.sof) && !d_flush) {
        d_colCnt = 0;
      }
      d_buf.rd_wr_buffer(d_colCnt, din, d_rowData); // Store in line buffers and get data from 3 rows
      if (d_colCnt == AC_NUM_COLS-1) {
        d_colCnt = 0;
      } else {
        d_colCnt++;
      }

#pragma unroll yes
      for (int i=0; i<AC_WIN_SIZE; i++) {
        d_reg.set(d_rowData[i],i);
      }

      apply_bounds();
      dout = d_win;

      flags.sof = d_sofReg[AC_WIN_SIZE/2] & d_solReg[0]; // TODO remove unused bits
      flags.eof = d_eofReg[AC_WIN_SIZE/2] & d_eolReg[0];
      flags.sol = d_solReg[0];
      flags.eol = d_eolReg[0];
      flagsOut = flags;
      if (d_eofReg[AC_WIN_SIZE/2] & d_eolReg[0]) { // finished flushing
        d_flush = 0;
      }
      if (d_sofReg[AC_WIN_SIZE/2] && d_solReg[0]) {
        d_vld = 1;
      }
      d_vldOut = d_vld;
      if (d_eofReg[AC_WIN_SIZE-1]) {
        d_vld = 0;
      }
    } else {
      d_vldOut = 0;
    }
  }

  void print() { // Print the 3x3 window data
#ifndef __SYNTHESIS__
    for (int i=0; i<AC_WIN_SIZE; i++) {
      for (int j=0; j<AC_WIN_SIZE; j++) {
        printf("%3d ", (int)d_win[i][j]);
      }
      printf("\n");
    }
#endif
  }

  void print_flags() {
#ifndef __SYNTHESIS__
    printf("vld = %d  sof = %d eof = %d sol = %d eol = %d\n\n",d_vld,flags.sof,flags.eof,flags.sol,flags.eol);
#endif
  }

private: // data
  ac_line_buffer<T,AC_WIN_SIZE,AC_NUM_COLS,AC_SINGLEPORT> d_buf;
  ac_shift_reg<T, AC_WIN_SIZE> d_reg;
  ac_int<AC_WIN_SIZE,false> d_sofReg;
  ac_int<AC_WIN_SIZE,false> d_eofReg;
  ac_int<AC_WIN_SIZE,false> d_solReg;
  ac_int<AC_WIN_SIZE,false> d_eolReg;
  ac_int<AC_WIN_SIZE,false> d_sofDel;
  ac_int<AC_WIN_SIZE,false> d_eofDel;
  ac_array<T,AC_WIN_SIZE> d_rowData;
  ac_array<T,AC_WIN_SIZE> d_win;
  ac_flags_struct flags;
  bool d_vld;
  bool d_vldOut;
  bool d_flush; // used to flush window after eof and no sof
  static const int NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE + 2*0)/1+1;
  static const int NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + 2*0)/1+1;
  ac_int<ac::log2_ceil<AC_NUM_COLS>::val,false> d_colCnt;
};

#endif

