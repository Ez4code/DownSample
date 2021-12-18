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

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	ac_line_buffer class is used to buffer up to 11 lines of a 2-d image or matrix
//
//
//	class templates
//		T - data type
//		AC_NUM_COLS - size of input/output register arrays
//
//
//	Member functions
//
//	rd_wr_buffer -
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_AC_LINE_BUFFER_H_
#define _INCLUDED_AC_LINE_BUFFER_H_

#include <stdio.h>
#include <ac_ipl/ac_shift_reg.h>
#include <ac_int.h>
#include <ac_array.h>

template<typename T, int AC_WORDS, bool AC_SINGLEPORT = false>
class ac_singleport
{
  T mem[AC_WORDS];
  ac_int<T::width*2,false> mem2x[AC_WORDS/2];
  ac_int<T::width*2,false> read_cache;
  ac_int<T::width*2,false> write_cache;
public:
  ac_singleport() {
    // TODO unit memory
  }
  void rd_wr_mem(ac_int<ac::nbits<AC_WORDS>::val,false> addr, T din,  T &dout) {
    if (!AC_SINGLEPORT) { // if dualport
      dout = mem[addr];
      mem[addr] = din;
    } else { // if singleport
      write_cache.set_slc(((addr&1)==0)?0:T::width,din.template slc<T::width>(0));
      if ((addr&1)==0) { // read on even
        read_cache = mem2x[addr/2];
      } else {
        if ((addr&1)==1) { // write on odd
          mem2x[addr/2] = write_cache;
        }
      }
      if ((addr&1)==0) {
        dout.set_slc(0,read_cache.template slc<T::width>(0));
      } else {
        dout.set_slc(0,read_cache.template slc<T::width>(T::width));
      }
    }
  }
  void rd_mem(ac_int<ac::nbits<AC_WORDS>::val,false> addr, T &dout) {
    if (!AC_SINGLEPORT) { // if dualport
      dout = mem[addr];
    } else { // if singleport
      if ((addr&1)==0) { // read on even
        read_cache = mem2x[addr/2];
      }
      if ((addr&1)==0) {
        dout.set_slc(0,read_cache.template slc<T::width>(0));
      } else {
        dout.set_slc(0,read_cache.template slc<T::width>(T::width));
      }
    }
  }
  void wr_mem(ac_int<ac::nbits<AC_WORDS>::val,false> addr, T din) {
    if (!AC_SINGLEPORT) { // if dualport
      mem[addr] = din;
    } else { // if singleport
      write_cache.set_slc(((addr&1)==0)?0:T::width,din);
      if ((addr&1)==1) { // write on odd
        mem2x[addr/2] = write_cache;
      }
    }
  }
};

// base class
template<typename T, int AC_NUM_ROWS, int AC_NUM_COLS, bool AC_SINGLEPORT=false>
class ac_line_buffer
{
public:
  ac_line_buffer() {
    // TODO unit memory
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din, T &dout);
};

// Specilizations to get exact and efficient memory buffering schem
template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,1,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer() {
    // TODO unit memory
  }

  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din, T &dout) {
    d_addrInt = addr;
    d_lineBuf0.rd_wr_mem(d_addrInt,din,dout);
  }
  void rd_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T &dout) {
    d_addrInt = addr;
    d_lineBuf0.rd_mem(d_addrInt,dout);
  }
  void wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din) {
    d_addrInt = addr;
    d_lineBuf0.wr_mem(d_addrInt,din);
  }
};

template<typename T, int AC_NUM_COLS,int NUM_CHANNELS>
class ac_line_buffer2
{
  T d_lineBuf0[NUM_CHANNELS][AC_NUM_COLS];
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer2() {
    // TODO unit memory
  }
  void rd_wr_buffer(ac_int<ac::nbits<NUM_CHANNELS>::val+1,false> chanAddr, ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din, ac_array<T,2> &dout) {
    d_addrInt = addr;

    dout[1] = din;
    dout[0] = d_lineBuf0[chanAddr][d_addrInt];
    d_lineBuf0[chanAddr][d_addrInt] = din;
  }
};

// TODO - productize
template<typename T, int AC_NUM_COLS,int NUM_CHANNELS>
class ac_line_buffer2_N
{
  T d_lineBuf0[NUM_CHANNELS][AC_NUM_COLS];
  T d_lineBuf1[NUM_CHANNELS][AC_NUM_COLS];
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
  ac_int<2,false> d_bufPtr;
public:
  ac_line_buffer2_N():d_bufPtr(1) {
    // TODO unit memory
  }
  void reset() {
    d_bufPtr = 1;
  }

  void rd_wr_buffer(ac_int<ac::nbits<NUM_CHANNELS>::val,false> chanAddr,ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, bool w, T din, ac_array<T,2> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0 && chanAddr == 0 && w) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==2) { d_bufPtr = 0; }
    switch (d_bufPtr) {
      case 0:
        dout[1] = d_lineBuf0[chanAddr][d_addrInt];
        dout[0] = d_lineBuf1[chanAddr][d_addrInt];
        if (w) { d_lineBuf0[chanAddr][d_addrInt] = din; }
        break;
      case 1:
        dout[1] = d_lineBuf1[chanAddr][d_addrInt];
        dout[0] = d_lineBuf0[chanAddr][d_addrInt];
        if (w) { d_lineBuf1[chanAddr][d_addrInt] = din; }
        break;
    }
  }
};

template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,3,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf1;
  ac_int<1,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer():d_bufPtr(1) {
    // TODO unit memory
  }
  void reset() {
    d_bufPtr = 1;
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din, ac_array<T,3> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0) { d_bufPtr++; } // Rotate the buffers

    switch (d_bufPtr) {
      case 0:
        dout[2] = din;
        d_lineBuf1.rd_mem(d_addrInt,dout[1]);
        d_lineBuf0.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 1:
        dout[2] = din;
        d_lineBuf0.rd_mem(d_addrInt,dout[1]);
        d_lineBuf1.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
    }
  }
};

// TODO - productize for interleaved buffer, each channel row is buffered and written out in order
template<typename T, int AC_NUM_COLS, int NUM_CHANNELS>
class ac_line_buffer3_N_ni
{
public:
  ac_line_buffer3_N_ni() : d_bufPtr(1) {
    // TODO unit memory
  }
  void reset() {
    d_bufPtr = 1;
  }
  void rd_wr_buffer(ac_int<ac::nbits<NUM_CHANNELS>::val+1,false> chanAddr, ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din, ac_array<T,3> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0 && chanAddr == 0) { d_bufPtr++; } // Rotate the buffers

    switch (d_bufPtr) {
      case 0:
        dout[2] = din;
        dout[1] = d_lineBuf1[chanAddr][d_addrInt];
        dout[0] = d_lineBuf0[chanAddr][d_addrInt];
        d_lineBuf0[chanAddr][d_addrInt] = din;
        break;
      case 1:
        dout[2] = din;
        dout[1] = d_lineBuf0[chanAddr][d_addrInt];
        dout[0] = d_lineBuf1[chanAddr][d_addrInt];
        d_lineBuf1[chanAddr][d_addrInt] = din;
        break;
    }
  }

private: // data
  T                                          d_lineBuf0[NUM_CHANNELS][AC_NUM_COLS];
  T                                          d_lineBuf1[NUM_CHANNELS][AC_NUM_COLS];
  ac_int<1,false>                            d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false>  d_addrInt;
};

#pragma map_to_operator [CCORE]
#pragma hls_ccore_type combinational
template<int ID, typename T>
T pass(T din)
{
  return din;
}

// TODO productize for CNN, added to support 3x3 with read only at points
template<typename T, int AC_NUM_COLS, int NUM_CHANNELS>
class ac_line_buffer3_N
{
public:
  ac_line_buffer3_N() : d_bufPtr(2) {
    // TODO unit memory
  }
  void reset() {
    d_bufPtr = 2;
  }

  void rd_wr_buffer(ac_int<ac::nbits<NUM_CHANNELS>::val+1,false> chanAddr,ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, bool w, T din, ac_array<T,3> &dout) {
    d_addrInt = pass<0>(addr);
    chanAddr = pass<1>(chanAddr);
    if (d_addrInt == 0 && chanAddr == 0 && w) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==3) { d_bufPtr = 0; }
    switch (d_bufPtr) {
      case 0:
        dout[2] = d_lineBuf0[chanAddr*AC_NUM_COLS+d_addrInt];;
        dout[1] = d_lineBuf2[chanAddr*AC_NUM_COLS+d_addrInt];
        dout[0] = d_lineBuf1[chanAddr*AC_NUM_COLS+d_addrInt];
        if (w) { d_lineBuf0[chanAddr*AC_NUM_COLS+d_addrInt] = din; }
        break;
      case 1:
        dout[2] = d_lineBuf1[chanAddr*AC_NUM_COLS+d_addrInt];;
        dout[1] = d_lineBuf0[chanAddr*AC_NUM_COLS+d_addrInt];
        dout[0] = d_lineBuf2[chanAddr*AC_NUM_COLS+d_addrInt];
        if (w) { d_lineBuf1[chanAddr*AC_NUM_COLS+d_addrInt] = din; }
        break;
      case 2:
        dout[2] = d_lineBuf2[chanAddr*AC_NUM_COLS+d_addrInt];;
        dout[1] = d_lineBuf1[chanAddr*AC_NUM_COLS+d_addrInt];
        dout[0] = d_lineBuf0[chanAddr*AC_NUM_COLS+d_addrInt];
        if (w) { d_lineBuf2[chanAddr*AC_NUM_COLS+d_addrInt] = din; }
        break;
    }
  }
private: // data
  T                                           d_lineBuf0[NUM_CHANNELS*AC_NUM_COLS];
  T                                           d_lineBuf1[NUM_CHANNELS*AC_NUM_COLS];
  T                                           d_lineBuf2[NUM_CHANNELS*AC_NUM_COLS];
  ac_int<2,false>                             d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false>   d_addrInt;
};

// TODO productize for CNN, added to support 3x3 with read only at points
template<typename T, int AC_NUM_COLS, int NUM_CHANNELS>
class ac_line_buffer3_N_2darr
{
  T d_lineBuf0[NUM_CHANNELS][AC_NUM_COLS];
  T d_lineBuf1[NUM_CHANNELS][AC_NUM_COLS];
  T d_lineBuf2[NUM_CHANNELS][AC_NUM_COLS];

  ac_int<2,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer3_N_2darr():d_bufPtr(2) {
    // TODO unit memory
  }
  void reset() {
    d_bufPtr = 2;
  }

  void rd_wr_buffer(ac_int<ac::nbits<NUM_CHANNELS>::val+1,false> chanAddr,ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, bool w, T din, ac_array<T,3> &dout) {
    d_addrInt = pass<0>(addr);
    if (d_addrInt == 0 && chanAddr == 0 && w) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==3) { d_bufPtr = 0; }
    switch (d_bufPtr) {
      case 0:
        dout[2] = d_lineBuf0[pass<1>(chanAddr)][d_addrInt];;
        dout[1] = d_lineBuf2[chanAddr][d_addrInt];
        dout[0] = d_lineBuf1[chanAddr][d_addrInt];
        if (w) { d_lineBuf0[chanAddr][d_addrInt] = din; }
        break;
      case 1:
        dout[2] = d_lineBuf1[chanAddr][d_addrInt];;
        dout[1] = d_lineBuf0[chanAddr][d_addrInt];
        dout[0] = d_lineBuf2[chanAddr][d_addrInt];
        if (w) { d_lineBuf1[chanAddr][d_addrInt] = din; }
        break;
      case 2:
        dout[2] = d_lineBuf2[chanAddr][d_addrInt];;
        dout[1] = d_lineBuf1[chanAddr][d_addrInt];
        dout[0] = d_lineBuf0[chanAddr][d_addrInt];
        if (w) { d_lineBuf2[chanAddr][d_addrInt] = din; }
        break;
    }
  }
};

template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,5,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf1;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf2;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf3;

  ac_int<3,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer():d_bufPtr(3) {
  }
  void reset() {
    d_bufPtr = 3;
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din,  ac_array<T,5> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==4) { d_bufPtr = 0; }

    switch (d_bufPtr) {
      case 0:
        dout[4] = din;
        d_lineBuf3.rd_mem(d_addrInt,dout[3]);
        d_lineBuf2.rd_mem(d_addrInt,dout[2]);
        d_lineBuf1.rd_mem(d_addrInt,dout[1]);
        d_lineBuf0.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 1:
        dout[4] = din;
        dout[4] = din;
        d_lineBuf0.rd_mem(d_addrInt,dout[3]);
        d_lineBuf3.rd_mem(d_addrInt,dout[2]);
        d_lineBuf2.rd_mem(d_addrInt,dout[1]);
        d_lineBuf1.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 2:
        dout[4] = din;
        d_lineBuf1.rd_mem(d_addrInt,dout[3]);
        d_lineBuf0.rd_mem(d_addrInt,dout[2]);
        d_lineBuf3.rd_mem(d_addrInt,dout[1]);
        d_lineBuf2.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 3:
        dout[4] = din;
        d_lineBuf2.rd_mem(d_addrInt,dout[3]);
        d_lineBuf1.rd_mem(d_addrInt,dout[2]);
        d_lineBuf0.rd_mem(d_addrInt,dout[1]);
        d_lineBuf3.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
    }
  }
};

template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,7,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf1;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf2;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf3;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf4;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf5;

  ac_int<3,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer() : d_bufPtr(5) { }
  void reset() {
    d_bufPtr = 5;
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din,  ac_array<T,7> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==6) { d_bufPtr = 0; }

    switch (d_bufPtr) {
      case 0:
        dout[6] = din;
        d_lineBuf5.rd_mem(d_addrInt,dout[5]);
        d_lineBuf4.rd_mem(d_addrInt,dout[4]);
        d_lineBuf3.rd_mem(d_addrInt,dout[3]);
        d_lineBuf2.rd_mem(d_addrInt,dout[2]);
        d_lineBuf1.rd_mem(d_addrInt,dout[1]);
        d_lineBuf0.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 1:
        dout[6] = din;
        d_lineBuf0.rd_mem(d_addrInt,dout[5]);
        d_lineBuf5.rd_mem(d_addrInt,dout[4]);
        d_lineBuf4.rd_mem(d_addrInt,dout[3]);
        d_lineBuf3.rd_mem(d_addrInt,dout[2]);
        d_lineBuf2.rd_mem(d_addrInt,dout[1]);
        d_lineBuf1.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 2:
        dout[6] = din;
        d_lineBuf1.rd_mem(d_addrInt,dout[5]);
        d_lineBuf0.rd_mem(d_addrInt,dout[4]);
        d_lineBuf5.rd_mem(d_addrInt,dout[3]);
        d_lineBuf4.rd_mem(d_addrInt,dout[2]);
        d_lineBuf3.rd_mem(d_addrInt,dout[1]);
        d_lineBuf2.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 3:
        dout[6] = din;
        d_lineBuf2.rd_mem(d_addrInt,dout[5]);
        d_lineBuf1.rd_mem(d_addrInt,dout[4]);
        d_lineBuf0.rd_mem(d_addrInt,dout[3]);
        d_lineBuf5.rd_mem(d_addrInt,dout[2]);
        d_lineBuf4.rd_mem(d_addrInt,dout[1]);
        d_lineBuf3.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 4:
        dout[6] = din;
        d_lineBuf3.rd_mem(d_addrInt,dout[5]);
        d_lineBuf2.rd_mem(d_addrInt,dout[4]);
        d_lineBuf1.rd_mem(d_addrInt,dout[3]);
        d_lineBuf0.rd_mem(d_addrInt,dout[2]);
        d_lineBuf5.rd_mem(d_addrInt,dout[1]);
        d_lineBuf4.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 5:
        dout[6] = din;
        d_lineBuf4.rd_mem(d_addrInt,dout[5]);
        d_lineBuf3.rd_mem(d_addrInt,dout[4]);
        d_lineBuf2.rd_mem(d_addrInt,dout[3]);
        d_lineBuf1.rd_mem(d_addrInt,dout[2]);
        d_lineBuf0.rd_mem(d_addrInt,dout[1]);
        d_lineBuf5.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
    }
  }
};

template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,9,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf1;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf2;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf3;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf4;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf5;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf6;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf7;

  ac_int<4,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer() : d_bufPtr(7) { }
  void reset() {
    d_bufPtr = 7;
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din,  ac_array<T,9> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==8) { d_bufPtr = 0; }

    switch (d_bufPtr) {
      case 0:
        dout[8] = din;
        d_lineBuf7.rd_mem(d_addrInt,dout[7]);
        d_lineBuf6.rd_mem(d_addrInt,dout[6]);
        d_lineBuf5.rd_mem(d_addrInt,dout[5]);
        d_lineBuf4.rd_mem(d_addrInt,dout[4]);
        d_lineBuf3.rd_mem(d_addrInt,dout[3]);
        d_lineBuf2.rd_mem(d_addrInt,dout[2]);
        d_lineBuf1.rd_mem(d_addrInt,dout[1]);
        d_lineBuf0.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 1:
        dout[8] = din;
        d_lineBuf0.rd_mem(d_addrInt,dout[7]);
        d_lineBuf7.rd_mem(d_addrInt,dout[6]);
        d_lineBuf6.rd_mem(d_addrInt,dout[5]);
        d_lineBuf5.rd_mem(d_addrInt,dout[4]);
        d_lineBuf4.rd_mem(d_addrInt,dout[3]);
        d_lineBuf3.rd_mem(d_addrInt,dout[2]);
        d_lineBuf2.rd_mem(d_addrInt,dout[1]);
        d_lineBuf1.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 2:
        dout[8] = din;
        d_lineBuf1.rd_mem(d_addrInt,dout[7]);
        d_lineBuf0.rd_mem(d_addrInt,dout[6]);
        d_lineBuf7.rd_mem(d_addrInt,dout[5]);
        d_lineBuf6.rd_mem(d_addrInt,dout[4]);
        d_lineBuf5.rd_mem(d_addrInt,dout[3]);
        d_lineBuf4.rd_mem(d_addrInt,dout[2]);
        d_lineBuf3.rd_mem(d_addrInt,dout[1]);
        d_lineBuf2.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 3:
        dout[8] = din;
        d_lineBuf2.rd_mem(d_addrInt,dout[7]);
        d_lineBuf1.rd_mem(d_addrInt,dout[6]);
        d_lineBuf0.rd_mem(d_addrInt,dout[5]);
        d_lineBuf7.rd_mem(d_addrInt,dout[4]);
        d_lineBuf6.rd_mem(d_addrInt,dout[3]);
        d_lineBuf5.rd_mem(d_addrInt,dout[2]);
        d_lineBuf4.rd_mem(d_addrInt,dout[1]);
        d_lineBuf3.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 4:
        dout[8] = din;
        d_lineBuf3.rd_mem(d_addrInt,dout[7]);
        d_lineBuf2.rd_mem(d_addrInt,dout[6]);
        d_lineBuf1.rd_mem(d_addrInt,dout[5]);
        d_lineBuf0.rd_mem(d_addrInt,dout[4]);
        d_lineBuf7.rd_mem(d_addrInt,dout[3]);
        d_lineBuf6.rd_mem(d_addrInt,dout[2]);
        d_lineBuf5.rd_mem(d_addrInt,dout[1]);
        d_lineBuf4.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 5:
        dout[8] = din;
        d_lineBuf4.rd_mem(d_addrInt,dout[7]);
        d_lineBuf3.rd_mem(d_addrInt,dout[6]);
        d_lineBuf2.rd_mem(d_addrInt,dout[5]);
        d_lineBuf1.rd_mem(d_addrInt,dout[4]);
        d_lineBuf0.rd_mem(d_addrInt,dout[3]);
        d_lineBuf7.rd_mem(d_addrInt,dout[2]);
        d_lineBuf6.rd_mem(d_addrInt,dout[1]);
        d_lineBuf5.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 6:
        dout[8] = din;
        d_lineBuf5.rd_mem(d_addrInt,dout[7]);
        d_lineBuf4.rd_mem(d_addrInt,dout[6]);
        d_lineBuf3.rd_mem(d_addrInt,dout[5]);
        d_lineBuf2.rd_mem(d_addrInt,dout[4]);
        d_lineBuf1.rd_mem(d_addrInt,dout[3]);
        d_lineBuf0.rd_mem(d_addrInt,dout[2]);
        d_lineBuf7.rd_mem(d_addrInt,dout[1]);
        d_lineBuf6.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 7:
        dout[8] = din;
        d_lineBuf6.rd_mem(d_addrInt,dout[7]);
        d_lineBuf5.rd_mem(d_addrInt,dout[6]);
        d_lineBuf4.rd_mem(d_addrInt,dout[5]);
        d_lineBuf3.rd_mem(d_addrInt,dout[4]);
        d_lineBuf2.rd_mem(d_addrInt,dout[3]);
        d_lineBuf1.rd_mem(d_addrInt,dout[2]);
        d_lineBuf0.rd_mem(d_addrInt,dout[1]);
        d_lineBuf7.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
    }
  }
};

template<typename T, int AC_NUM_COLS, bool AC_SINGLEPORT>
class ac_line_buffer<T,11,AC_NUM_COLS,AC_SINGLEPORT>
{
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf0;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf1;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf2;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf3;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf4;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf5;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf6;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf7;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf8;
  ac_singleport<T,AC_NUM_COLS,AC_SINGLEPORT> d_lineBuf9;

  ac_int<4,false> d_bufPtr;
  ac_int<ac::nbits<AC_NUM_COLS>::val,false> d_addrInt;
public:
  ac_line_buffer() : d_bufPtr(9) { }
  void reset() {
    d_bufPtr = 9;
  }
  void rd_wr_buffer(ac_int<ac::nbits<AC_NUM_COLS>::val,false> addr, T din,  ac_array<T,11> &dout) {
    d_addrInt = addr;
    if (d_addrInt == 0) { d_bufPtr++; } // Rotate the buffers
    if (d_bufPtr==10) { d_bufPtr = 0; }

    switch (d_bufPtr) {
      case 0:
        dout[10] = din;
        d_lineBuf9.rd_mem(d_addrInt,dout[9]);
        d_lineBuf8.rd_mem(d_addrInt,dout[8]);
        d_lineBuf7.rd_mem(d_addrInt,dout[7]);
        d_lineBuf6.rd_mem(d_addrInt,dout[6]);
        d_lineBuf5.rd_mem(d_addrInt,dout[5]);
        d_lineBuf4.rd_mem(d_addrInt,dout[4]);
        d_lineBuf3.rd_mem(d_addrInt,dout[3]);
        d_lineBuf2.rd_mem(d_addrInt,dout[2]);
        d_lineBuf1.rd_mem(d_addrInt,dout[1]);
        d_lineBuf0.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 1:
        dout[10] = din;
        d_lineBuf0.rd_mem(d_addrInt,dout[9]);
        d_lineBuf9.rd_mem(d_addrInt,dout[8]);
        d_lineBuf8.rd_mem(d_addrInt,dout[7]);
        d_lineBuf7.rd_mem(d_addrInt,dout[6]);
        d_lineBuf6.rd_mem(d_addrInt,dout[5]);
        d_lineBuf5.rd_mem(d_addrInt,dout[4]);
        d_lineBuf4.rd_mem(d_addrInt,dout[3]);
        d_lineBuf3.rd_mem(d_addrInt,dout[2]);
        d_lineBuf2.rd_mem(d_addrInt,dout[1]);
        d_lineBuf1.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 2:
        dout[10] = din;
        d_lineBuf1.rd_mem(d_addrInt,dout[9]);
        d_lineBuf0.rd_mem(d_addrInt,dout[8]);
        d_lineBuf9.rd_mem(d_addrInt,dout[7]);
        d_lineBuf8.rd_mem(d_addrInt,dout[6]);
        d_lineBuf7.rd_mem(d_addrInt,dout[5]);
        d_lineBuf6.rd_mem(d_addrInt,dout[4]);
        d_lineBuf5.rd_mem(d_addrInt,dout[3]);
        d_lineBuf4.rd_mem(d_addrInt,dout[2]);
        d_lineBuf3.rd_mem(d_addrInt,dout[1]);
        d_lineBuf2.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 3:
        dout[10] = din;
        d_lineBuf2.rd_mem(d_addrInt,dout[9]);
        d_lineBuf1.rd_mem(d_addrInt,dout[8]);
        d_lineBuf0.rd_mem(d_addrInt,dout[7]);
        d_lineBuf9.rd_mem(d_addrInt,dout[6]);
        d_lineBuf8.rd_mem(d_addrInt,dout[5]);
        d_lineBuf7.rd_mem(d_addrInt,dout[4]);
        d_lineBuf6.rd_mem(d_addrInt,dout[3]);
        d_lineBuf5.rd_mem(d_addrInt,dout[2]);
        d_lineBuf4.rd_mem(d_addrInt,dout[1]);
        d_lineBuf3.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 4:
        dout[10] = din;
        d_lineBuf3.rd_mem(d_addrInt,dout[9]);
        d_lineBuf2.rd_mem(d_addrInt,dout[8]);
        d_lineBuf1.rd_mem(d_addrInt,dout[7]);
        d_lineBuf0.rd_mem(d_addrInt,dout[6]);
        d_lineBuf9.rd_mem(d_addrInt,dout[5]);
        d_lineBuf8.rd_mem(d_addrInt,dout[4]);
        d_lineBuf7.rd_mem(d_addrInt,dout[3]);
        d_lineBuf6.rd_mem(d_addrInt,dout[2]);
        d_lineBuf5.rd_mem(d_addrInt,dout[1]);
        d_lineBuf4.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 5:
        dout[10] = din;
        d_lineBuf4.rd_mem(d_addrInt,dout[9]);
        d_lineBuf3.rd_mem(d_addrInt,dout[8]);
        d_lineBuf2.rd_mem(d_addrInt,dout[7]);
        d_lineBuf1.rd_mem(d_addrInt,dout[6]);
        d_lineBuf0.rd_mem(d_addrInt,dout[5]);
        d_lineBuf9.rd_mem(d_addrInt,dout[4]);
        d_lineBuf8.rd_mem(d_addrInt,dout[3]);
        d_lineBuf7.rd_mem(d_addrInt,dout[2]);
        d_lineBuf6.rd_mem(d_addrInt,dout[1]);
        d_lineBuf5.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 6:
        dout[10] = din;
        d_lineBuf5.rd_mem(d_addrInt,dout[9]);
        d_lineBuf4.rd_mem(d_addrInt,dout[8]);
        d_lineBuf3.rd_mem(d_addrInt,dout[7]);
        d_lineBuf2.rd_mem(d_addrInt,dout[6]);
        d_lineBuf1.rd_mem(d_addrInt,dout[5]);
        d_lineBuf0.rd_mem(d_addrInt,dout[4]);
        d_lineBuf9.rd_mem(d_addrInt,dout[3]);
        d_lineBuf8.rd_mem(d_addrInt,dout[2]);
        d_lineBuf7.rd_mem(d_addrInt,dout[1]);
        d_lineBuf6.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 7:
        dout[10] = din;
        d_lineBuf6.rd_mem(d_addrInt,dout[9]);
        d_lineBuf5.rd_mem(d_addrInt,dout[8]);
        d_lineBuf4.rd_mem(d_addrInt,dout[7]);
        d_lineBuf3.rd_mem(d_addrInt,dout[6]);
        d_lineBuf2.rd_mem(d_addrInt,dout[5]);
        d_lineBuf1.rd_mem(d_addrInt,dout[4]);
        d_lineBuf0.rd_mem(d_addrInt,dout[3]);
        d_lineBuf9.rd_mem(d_addrInt,dout[2]);
        d_lineBuf8.rd_mem(d_addrInt,dout[1]);
        d_lineBuf7.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 8:
        dout[10] = din;
        d_lineBuf7.rd_mem(d_addrInt,dout[9]);
        d_lineBuf6.rd_mem(d_addrInt,dout[8]);
        d_lineBuf5.rd_mem(d_addrInt,dout[7]);
        d_lineBuf4.rd_mem(d_addrInt,dout[6]);
        d_lineBuf3.rd_mem(d_addrInt,dout[5]);
        d_lineBuf2.rd_mem(d_addrInt,dout[4]);
        d_lineBuf1.rd_mem(d_addrInt,dout[3]);
        d_lineBuf0.rd_mem(d_addrInt,dout[2]);
        d_lineBuf9.rd_mem(d_addrInt,dout[1]);
        d_lineBuf8.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
      case 9:
        dout[10] = din;
        d_lineBuf8.rd_mem(d_addrInt,dout[9]);
        d_lineBuf7.rd_mem(d_addrInt,dout[8]);
        d_lineBuf6.rd_mem(d_addrInt,dout[7]);
        d_lineBuf5.rd_mem(d_addrInt,dout[6]);
        d_lineBuf4.rd_mem(d_addrInt,dout[5]);
        d_lineBuf3.rd_mem(d_addrInt,dout[4]);
        d_lineBuf2.rd_mem(d_addrInt,dout[3]);
        d_lineBuf1.rd_mem(d_addrInt,dout[2]);
        d_lineBuf0.rd_mem(d_addrInt,dout[1]);
        d_lineBuf9.rd_wr_mem(d_addrInt,din,dout[0]);
        break;
    }
  }
};

#endif

