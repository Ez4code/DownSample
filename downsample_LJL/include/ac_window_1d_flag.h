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

#ifndef __AC_WINDOW_1D_FLAG_H
#define __AC_WINDOW_1D_FLAG_H

#ifndef __SYNTHESIS__
#include <cassert>
#endif


#include <ac_int.h>
enum ac_window_mode { AC_CLIP       = 1<<0, 
                      AC_MIRROR     = 1<<1, 
                      AC_WIN        = 1<<2,
                      AC_BOUNDARY   = 1<<3,
                      AC_SINGLEPORT = 1<<8,
                      AC_DUALPORT   = 1<<9,
                      AC_SYM_INDEX  = 1<<16,
                      AC_LIN_INDEX  = 1<<17,
                      AC_NO_REWIND  = 1<<24,
                      AC_REWIND     = 1<<25};

template<class T, int AC_WN, int AC_WMODE = AC_WIN>
class ac_window_1d_flag {
    enum {AC_EVEN_ROW = ((AC_WN%2)==0)};
public:
  ac_window_1d_flag();
  T & operator[]      (int i);
  const T & operator[](int i) const ;
  void write(T src, bool sol, bool eol);
  void readFlags(bool &solOut, bool &eolOut) ;
  bool valid();

private:
  T   data_[AC_WN];             // This array stores the input samples
  T   wout_[AC_WN];             // This array is what really gets read
  bool sol_[AC_WN];  // Start of line control
  bool eol_[AC_WN];  // End of line control
  bool rampup_;              // Has the window ramped up?
  bool s_;                    // start bit set when start detected in shift reg
  bool e_;                    // end bit set when start detected in shift reg
  int m_;                     // Mirror center reflection point
};

// The only defined CTOR
template<class T, int AC_WN, int AC_WMODE>
ac_window_1d_flag<T, AC_WN, AC_WMODE>::ac_window_1d_flag():
  rampup_(false), s_(false), e_(false), m_(0)
{
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN; i++){
    sol_[i] = 0;
    eol_[i] = 0;
  }
}

// Have we passed the first AC_WN/2+1 rampup cycles? Is this now valid data?
template<class T, int AC_WN, int AC_WMODE>
bool ac_window_1d_flag<T, AC_WN, AC_WMODE>::valid() {
  return rampup_;
};

template<class T, int AC_WN, int AC_WMODE>
void ac_window_1d_flag<T, AC_WN, AC_WMODE>::write(T src, bool sol, bool eol) {
  
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN-1; i++)
    data_[i] = data_[i+1];
  data_[AC_WN-1] = src;
 
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN-1; i++)
    sol_[i] = sol_[i+1];
  sol_[AC_WN-1] = sol;

#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN-1; i++)
    eol_[i] = eol_[i+1];
  eol_[AC_WN-1] = eol;
          
#ifdef __SYNTHESIS__
#pragma hls_unroll yes
#endif
  for (int i=0; i<AC_WN; i++)
    wout_[i] = data_[i];

  if(AC_WMODE&AC_CLIP){
    s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
    for (int i=AC_WN/2-1; i>=0; i--){
      s_ |= sol_[i+1];
      wout_[i] = s_ ? wout_[i+1] : wout_[i];
    }
          
    e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
    for (int i=AC_WN/2+1; i< AC_WN; i++){
      e_ |= eol_[i-1];
      wout_[i] = e_ ? wout_[i-1] : wout_[i];
    }
  }

  if(AC_WMODE&AC_MIRROR){
    s_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //lower clipping
#endif
    for (int i=AC_WN/2-1; i>=0; i--){
      s_ |= sol_[i+1];
      if(sol_[i+1]==1)
        m_ = i+1;
      wout_[i] = s_ ? wout_[m_*2 - i] : wout_[i];
    }
          
    e_ = false;
#ifdef __SYNTHESIS__
#pragma hls_unroll yes //upper clipping
#endif
    for (int i=AC_WN/2+1; i< AC_WN; i++){
      e_ |= eol_[i-1];
      if(eol_[i-1]==1)
        m_ = i-1;
      wout_[i] = e_ ? wout_[m_*2-i] : wout_[i];
    }
  }

  if(AC_WMODE&AC_WIN){//No clipping or mirroring means that prefill takes AC_WN-1 iterations
    if(sol_[0]==1)//prefill finished
      rampup_=true;
  }
  else
    if(sol_[AC_WN/2-AC_EVEN_ROW]==1)//prefill finished
      rampup_=true;

}

template<class T, int AC_WN, int AC_WMODE>
void ac_window_1d_flag<T, AC_WN, AC_WMODE>::readFlags(bool &solOut, bool &eolOut) { 
  if(AC_WMODE&AC_WIN){//No clipping or mirroring means that flags are ready after prefill
    solOut = sol_[0];
    eolOut = eol_[0];   
  }
  else{
    solOut = sol_[AC_WN/2-AC_EVEN_ROW];
    eolOut = eol_[AC_WN/2];          
  }
}

template<class T, int AC_WN, int AC_WMODE>
inline  T & ac_window_1d_flag<T, AC_WN, AC_WMODE>::operator[] (int i)
{
#ifndef __SYNTHESIS__
    if(!(AC_WMODE&AC_LIN_INDEX))
        assert((-AC_WN/2 <= i) && (i <= AC_WN/2));
    else
        assert((0 <= i) && (i <= AC_WN)); 
#endif
    if(!(AC_WMODE&AC_LIN_INDEX))     //symmetrical indexing
        return wout_[i + (AC_WN/2)];
    else                             //linear indexing
        return wout_[i];
}
template<class T, int AC_WN, int AC_WMODE>
inline  const T & ac_window_1d_flag<T, AC_WN, AC_WMODE>::operator[](int i) const
{
#ifndef __SYNTHESIS__
    if(!(AC_WMODE&AC_LIN_INDEX))
        assert((-AC_WN/2 <= i) && (i <= AC_WN/2));
    else
        assert((0 <= i) && (i <= AC_WN)); 
#endif
    if(!(AC_WMODE&AC_LIN_INDEX))     //symmetrical indexing
        return wout_[i + (AC_WN/2)];
    else                             //linear indexing
        return wout_[i];
}

#endif
