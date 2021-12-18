////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
//
// Copyright (c) 2003-2015 Mentor Graphics Corp.
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

//
// 6/13/09 - Mike F. Enhanced for singleport RAM support and rewind capability
////////////////////////////////////////////////////////////////////////////////

#ifndef __AC_WINDOW_2D_FLAG_H
#define __AC_WINDOW_2D_FLAG_H

#include <ac_int.h>
#ifndef __SYNTHESIS__
#include <cassert>
#endif

#include "ac_buffer_2d.h"
#include "ac_window_1d_flag.h"

#ifndef __SYNTHESIS__
#include <iostream>
using namespace std;
#endif

template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
class ac_window_2d_flag
{
public:
  ac_window_2d_flag();
  ac_window_2d_flag(T bval);
  void reset();
  T &operator()      (int r,int c);
  const T &operator()(int r, int c) const ;
  void write(T src, bool sof, bool eof, bool sol, bool eol);
  void exec(T src, bool sof, bool eof, bool sol, bool eol, bool w);
  bool valid();
  void readFlags(bool &sof, bool &eof, bool &sol, bool &eol);
  void rewind();
  enum {AC_EVEN_ROW = ((AC_WN_ROW%2)==0)};
  enum {AC_EVEN_COL = ((AC_WN_COL%2)==0)};
  // If the row/column size is even and the windowing mode is set to AC_MIRROR, we have
  // to increase the size of the mirrors by one.
  static const int PLUS_1_MIRROR_ROW = int((bool(AC_EVEN_ROW)) && (bool(AC_WMODE &AC_MIRROR)));
  static const int PLUS_1_MIRROR_COL = int((bool(AC_EVEN_COL)) && (bool(AC_WMODE &AC_MIRROR)));
  enum {
    logAC_NCOL = ac::nbits< AC_NCOL >::val
  };
  void writeInt(T src, bool sol, bool eol);
  ac_buffer_2d<T,AC_NCOL,AC_WN_ROW + PLUS_1_MIRROR_ROW,AC_WMODE> vWind;  //Vertical window

  T   data_[AC_WN_ROW + PLUS_1_MIRROR_ROW][AC_WN_COL + PLUS_1_MIRROR_COL];             // This array stores the input samples
  T   woutH_[AC_WN_ROW + PLUS_1_MIRROR_ROW][AC_WN_COL + PLUS_1_MIRROR_COL];             // This array is what really gets read
  bool sol_[AC_WN_COL + PLUS_1_MIRROR_COL];  // Start of line control
  bool eol_[AC_WN_COL + PLUS_1_MIRROR_COL];  // End of line control
  bool s_;                    // start bit set when start detected in shift reg
  bool e_;                    // end bit set when start detected in shift reg
  int m_;                     // Mirror center reflection point

  T   wout_[AC_WN_ROW + PLUS_1_MIRROR_ROW];             // This array stores the vWind output
  ac_int<logAC_NCOL,false> addr;  // Address counter for line buffers
  bool sofOut_[AC_WN_COL + PLUS_1_MIRROR_COL];  // Start of frame output control
  bool eofOut_[AC_WN_COL + PLUS_1_MIRROR_COL];  // End of frame output control
  bool sof_[AC_WN_ROW + PLUS_1_MIRROR_ROW];  // Start of frame control
  bool eof_[AC_WN_ROW + PLUS_1_MIRROR_ROW];  // End of frame control
  bool solOut;  // Start of line output control
  bool eolOut;  // End of line output control

  bool rampup_;              // Has the window ramped up?
  bool s;                   // start bit set when start detected in shift reg
  bool e;                   // end bit set when start detected in shift reg
  int m;

  T boundaryVal;   //constant for boundary condition
};


template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE> ac_window_2d_flag<T, AC_WN_ROW, AC_WN_COL,AC_NCOL,AC_WMODE>::ac_window_2d_flag():
  addr(0), rampup_(false), s(false), e(false), m(0)
{

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
#ifndef __SYNTHESIS__

  assert((AC_WN_ROW > 0) && "ac_window must have positive window width");
  assert((AC_WN_COL > 0) && "ac_window must have positive window height");
  assert((AC_WN_COL < AC_NCOL) && "ac_window width must be smaller than array width");
#endif
  for (int i=0; i<AC_WN_ROW + PLUS_1_MIRROR_ROW; i++) {
    wout_[i] = 0;
    sof_[i] = 0;
    eof_[i] = 0;
  }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL + PLUS_1_MIRROR_COL; i++) {
    sol_[i] = 0;
    eol_[i] = 0;
    sofOut_[i] = 0;
    eofOut_[i] = 0;
  }
}

template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE> ac_window_2d_flag<T, AC_WN_ROW, AC_WN_COL,AC_NCOL,AC_WMODE>::ac_window_2d_flag(T bval):
  addr(0), rampup_(false), s(false), e(false), m(0)
{

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
#ifndef __SYNTHESIS__

  assert((AC_WN_ROW > 0) && "ac_window must have positive window width");
  assert((AC_WN_COL > 0) && "ac_window must have positive window height");
  assert((AC_WN_COL < AC_NCOL) && "ac_window width must be smaller than array width");
#endif
  for (int i=0; i<AC_WN_ROW + PLUS_1_MIRROR_ROW; i++) {
    wout_[i] = 0;
    sofOut_[i] = 0;
    eofOut_[i] = 0;
    sof_[i] = 0;
    eof_[i] = 0;
  }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL + PLUS_1_MIRROR_COL; i++) {
    sol_[i] = 0;
    eol_[i] = 0;
  }
  boundaryVal = bval;
}

// Reset member variables in order to start with new configurations with different
// frame sizes.
template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
void ac_window_2d_flag<T, AC_WN_ROW, AC_WN_COL,AC_NCOL,AC_WMODE>::reset()
{
  addr = 0;
  rampup_ = false;
  s = false;
  e = false;
  m = 0;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_ROW + PLUS_1_MIRROR_ROW; i++) {
    wout_[i] = 0;
    sofOut_[i] = 0;
    eofOut_[i] = 0;
    sof_[i] = 0;
    eof_[i] = 0;
  }

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL + PLUS_1_MIRROR_COL; i++) {
    sol_[i] = 0;
    eol_[i] = 0;
  }
}

// Have we passed the first AC_WN_ROW/2+1 rampup cycles? Is this now valid data?
template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
bool ac_window_2d_flag<T, AC_WN_ROW, AC_WN_COL,AC_NCOL,AC_WMODE>::valid()
{
  return rampup_;
};

template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
void ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::write(T src, bool sof, bool eof, bool sol, bool eol)
{
  if (sol) {                     //Store sof at start of each new line
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_ROW - 1 + PLUS_1_MIRROR_ROW; i++)
    { sof_[i] = sof_[i+1]; }
    sof_[AC_WN_ROW - 1 + PLUS_1_MIRROR_ROW] = sof;
  }

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL - 1 + PLUS_1_MIRROR_COL; i++)
  { sofOut_[i] = sofOut_[i+1];  }

  if (AC_WMODE&AC_WIN)
  { sofOut_[AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = sol ? sof_[0]:(bool)0; }
  else
  { sofOut_[AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = sol ? sof_[AC_WN_ROW/2 - AC_EVEN_ROW + PLUS_1_MIRROR_ROW + ((AC_WMODE&AC_CLIP)&&AC_EVEN_ROW)]:(bool)0; }

  if (sol)                      //Address resets at each start of line
  { addr = 0; }

  vWind.write(src,(int)(addr),1);

  if (addr<AC_NCOL-1)
  { addr++; }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_ROW + PLUS_1_MIRROR_ROW; i++) {   //get vertical window output
    wout_[i] = vWind[i];
  }

  if (AC_WMODE&AC_CLIP) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes // vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      wout_[i] = s ? wout_[i+1] : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW; i++) {
      e |= eof_[i];
      wout_[i] = e ? wout_[i-1] : wout_[i];
    }
  }

  if (AC_WMODE&AC_MIRROR) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      if (sof_[i+1]==1)
      { m = i+1; }
      wout_[i] = s ? wout_[m*2-i] : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW + PLUS_1_MIRROR_ROW; i++) {
      //!!!!!!!compare againt i instead of i-1 to go handle eol
      e |= eof_[i];
      if (eof_[i]==1)
      { m = i-1; }
      wout_[i] = e ? wout_[m*2-i] : wout_[i];
    }
  }

  if (AC_WMODE&AC_BOUNDARY) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes // vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      wout_[i] = s ? boundaryVal : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW; i++) {
      e |= eof_[i];
      wout_[i] = e ? boundaryVal : wout_[i];
    }
  }

  if (eol) {                     //Store eof at end of each new line
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_ROW - 1 + PLUS_1_MIRROR_ROW; i++)
    { eof_[i] = eof_[i+1]; }
    eof_[AC_WN_ROW - 1 + PLUS_1_MIRROR_ROW] = eof;
  }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL - 1 + PLUS_1_MIRROR_COL; i++)
  { eofOut_[i] = eofOut_[i+1]; }
  eofOut_[AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = eol ? eof_[AC_WN_ROW/2]:(bool)0;


#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL - 1 + PLUS_1_MIRROR_COL; i++)
  { sol_[i] = sol_[i+1]; }
  sol_[AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = sol;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL - 1 + PLUS_1_MIRROR_COL; i++)
  { eol_[i] = eol_[i+1]; }
  eol_[AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = eol;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int j=0; j<AC_WN_ROW + PLUS_1_MIRROR_ROW; j++) {
    //HORIZONTAL WINDOWS
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_COL - 1 + PLUS_1_MIRROR_COL; i++)
    { data_[j][i] = data_[j][i+1]; }
    data_[j][AC_WN_COL - 1 + PLUS_1_MIRROR_COL] = wout_[j];

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_COL + PLUS_1_MIRROR_COL; i++)
    { woutH_[j][i] = data_[j][i]; }

    if (AC_WMODE&AC_CLIP) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2-1; i>=0; i--) {
        s_ |= sol_[i+1];
        woutH_[j][i] = s_ ? woutH_[j][i+1] : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL; i++) {
        e_ |= eol_[i-1];
        woutH_[j][i] = e_ ? woutH_[j][i-1] : woutH_[j][i];
      }
    }

    if (AC_WMODE&AC_MIRROR) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2 - 1; i>=0; i--) {
        s_ |= sol_[i+1];
        if (sol_[i+1]==1)
        { m_ = i+1; }
        woutH_[j][i] = s_ ? woutH_[j][m_*2 - i] : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL + PLUS_1_MIRROR_COL; i++) {
        e_ |= eol_[i-1];
        if (eol_[i-1]==1)
        { m_ = i-1; }
        woutH_[j][i] = e_ ? woutH_[j][m_*2-i] : woutH_[j][i];
      }
    }

    if (AC_WMODE&AC_BOUNDARY) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2-1; i>=0; i--) {
        s_ |= sol_[i+1];
        woutH_[j][i] = s_ ? boundaryVal : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL; i++) {
        e_ |= eol_[i-1];
        woutH_[j][i] = e_ ? boundaryVal : woutH_[j][i];
      }
    }
  }

  if (AC_WMODE&AC_WIN) { //No clipping or mirroring means that flags are ready after prefill
    solOut = sol_[0];
    eolOut = eol_[0];
  } else {
    solOut = sol_[AC_WN_COL/2 - AC_EVEN_COL + PLUS_1_MIRROR_COL + ((AC_WMODE&AC_CLIP)&&AC_EVEN_COL)];
    eolOut = eol_[AC_WN_COL/2];
  }

  if (AC_WMODE&AC_WIN) {
    if (sof_[0]==1 && solOut) //prefill finished
    { rampup_=true; }
  } else {
    if (sof_[AC_WN_ROW/2 - AC_EVEN_ROW + PLUS_1_MIRROR_ROW + ((AC_WMODE&AC_CLIP)&&AC_EVEN_ROW)]==1 && solOut) //prefill finished
    { rampup_=true; }
  }
}


template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
inline  T &ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::operator() (int r, int c)
{
#ifndef __SYNTHESIS__
  if (!(AC_WMODE&AC_LIN_INDEX)) {
    assert((-AC_WN_ROW/2 <= r) && (r <= AC_WN_ROW/2));
    assert((-AC_WN_COL/2 <= c) && (c <= AC_WN_COL/2));
  } else {
    assert((0 <= r) && (r <= AC_WN_ROW));
    assert((0 <= c) && (c <= AC_WN_COL));
  }
#endif
  if (!(AC_WMODE&AC_LIN_INDEX))
  { return woutH_[r+AC_WN_ROW/2][c+AC_WN_COL/2]; }
  else
  { return woutH_[r][c]; }
}
template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
inline  const T &ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::operator()(int r,int c) const
{
#ifndef __SYNTHESIS__
  if (!(AC_WMODE&AC_LIN_INDEX)) {
    assert((-AC_WN_ROW/2 <= r) && (r <= AC_WN_ROW/2));
    assert((-AC_WN_COL/2 <= c) && (c <= AC_WN_COL/2));
  } else {
    assert((0 <= r) && (r <= AC_WN_ROW));
    assert((0 <= c) && (c <= AC_WN_COL));
  }
#endif
  if (!(AC_WMODE&AC_LIN_INDEX))
  { return woutH_[r+AC_WN_ROW/2][c+AC_WN_COL/2]; }
  else
  { return woutH_[r][c]; }
}

template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
void ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::readFlags(bool &sof, bool &eof, bool &sol, bool &eol)
{
  if (AC_WMODE == AC_WIN) {
    sof = sofOut_[0];
    eof = eofOut_[0];
  } else {
    sof = sofOut_[AC_WN_COL/2 - AC_EVEN_ROW + PLUS_1_MIRROR_COL + ((AC_WMODE&AC_CLIP)&&AC_EVEN_ROW)];
    eof = eofOut_[AC_WN_COL/2];
  }
  sol = solOut & rampup_;
  eol = eolOut & rampup_;
}

//Rewind support added for video scalers, etc.
template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
void ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::exec(T src, bool sof, bool eof, bool sol, bool eol, bool w)
{

  if (sol&w) {                     //Store sof at start of each new line when writing
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_ROW-1; i++)
    { sof_[i] = sof_[i+1]; }
    sof_[AC_WN_ROW-1] = sof;
  }

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL-1; i++)
  { sofOut_[i] = sofOut_[i+1]; }

  if (AC_WMODE&AC_WIN)
  { sofOut_[AC_WN_COL-1] = sol ? sof_[0]:(bool)0; }
  else
  { sofOut_[AC_WN_COL-1] = sol ? sof_[AC_WN_ROW/2-AC_EVEN_ROW]:(bool)0; }

  if (sol)                      //Address resets at each start of line
  { addr = 0; }

  vWind.write(src,(int)(addr),w);//line buffers

  if (addr!=AC_NCOL-1) //don't allow address out of bounds
  { addr++; }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_ROW; i++)    //get vertical window output
  { wout_[i] = vWind[i]; }

  if (AC_WMODE&AC_CLIP) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes // vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      wout_[i] = s ? wout_[i+1] : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW; i++) {
      e |= eof_[i];
      wout_[i] = e ? wout_[i-1] : wout_[i];
    }
  }

  if (AC_WMODE&AC_MIRROR) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      if (sof_[i+1]==1)
      { m = i+1; }
      wout_[i] = s ? wout_[m*2-i] : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW; i++) {
      //!!!!!!!compare againt i instead of i-1 to go handle eol
      e |= eof_[i];
      if (eof_[i]==1)
      { m = i-1; }
      wout_[i] = e ? wout_[m*2-i] : wout_[i];
    }
  }

  if (AC_WMODE&AC_BOUNDARY) {
    s = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes // vertical lower clipping
#endif
    for (int i=AC_WN_ROW/2-1; i>=0; i--) {
      s |= sof_[i+1];
      wout_[i] = s ? boundaryVal : wout_[i];
    }

    e = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //  vertical upper clipping
#endif
    for (int i=AC_WN_ROW/2+1; i< AC_WN_ROW; i++) {
      e |= eof_[i];
      wout_[i] = e ? boundaryVal : wout_[i];
    }
  }

  if (eol&w) {                     //Store eof at end of each new line when writing
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_ROW-1; i++)
    { eof_[i] = eof_[i+1]; }
    eof_[AC_WN_ROW-1] = eof;
  }
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL-1; i++)
  { eofOut_[i] = eofOut_[i+1]; }
  eofOut_[AC_WN_COL-1] = eol ? eof_[AC_WN_ROW/2]:(bool)0;


#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL-1; i++)
  { sol_[i] = sol_[i+1]; }
  sol_[AC_WN_COL-1] = sol;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN_COL-1; i++)
  { eol_[i] = eol_[i+1]; }
  eol_[AC_WN_COL-1] = eol;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int j=0; j<AC_WN_ROW; j++) {
    //HORIZONTAL WINDOWS
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_COL-1; i++)
    { data_[j][i] = data_[j][i+1]; }
    data_[j][AC_WN_COL-1] = wout_[j];

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
    for (int i=0; i<AC_WN_COL; i++)
    { woutH_[j][i] = data_[j][i]; }

    if (AC_WMODE&AC_CLIP) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2-1; i>=0; i--) {
        s_ |= sol_[i+1];
        woutH_[j][i] = s_ ? woutH_[j][i+1] : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL; i++) {
        e_ |= eol_[i-1];
        woutH_[j][i] = e_ ? woutH_[j][i-1] : woutH_[j][i];
      }
    }

    if (AC_WMODE&AC_MIRROR) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2-1; i>=0; i--) {
        s_ |= sol_[i+1];
        if (sol_[i+1]==1)
        { m_ = i+1; }
        woutH_[j][i] = s_ ? woutH_[j][m_*2 - i] : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL; i++) {
        e_ |= eol_[i-1];
        if (eol_[i-1]==1)
        { m_ = i-1; }
        woutH_[j][i] = e_ ? woutH_[j][m_*2-i] : woutH_[j][i];
      }
    }

    if (AC_WMODE&AC_BOUNDARY) {
      s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
      for (int i=AC_WN_COL/2-1; i>=0; i--) {
        s_ |= sol_[i+1];
        woutH_[j][i] = s_ ? boundaryVal : woutH_[j][i];
      }

      e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
      for (int i=AC_WN_COL/2+1; i< AC_WN_COL; i++) {
        e_ |= eol_[i-1];
        woutH_[j][i] = e_ ? boundaryVal : woutH_[j][i];
      }
    }

  }

  if (AC_WMODE&AC_WIN) { //No clipping or mirroring means that flags are ready after prefill
    solOut = sol_[0];
    eolOut = eol_[0];
  } else {
    solOut = sol_[AC_WN_COL/2-AC_EVEN_COL];
    eolOut = eol_[AC_WN_COL/2];
  }

  if (AC_WMODE&AC_WIN) {
    if (sof_[0]==1 && solOut) //prefill finished
    { rampup_=true; }
  } else {
    if (sof_[AC_WN_ROW/2-AC_EVEN_ROW]==1 && solOut) //prefill finished
    { rampup_=true; }
  }
}

//Rewind is used to reset the address into the line buffers.  This is used when performing scaling and
//the programmable line width is less than the maximum line width.
template<class T, int AC_WN_ROW, int AC_WN_COL, int AC_NCOL, int AC_WMODE>
void ac_window_2d_flag<T,AC_WN_ROW,AC_WN_COL,AC_NCOL,AC_WMODE>::rewind()
{
  addr = 0;
}

#endif
