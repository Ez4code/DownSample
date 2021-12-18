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

#ifndef __AC_WINDOW_1D_ARRAY_H
#define __AC_WINDOW_1D_ARRAY_H

#ifndef __SYNTHESIS__
#include <cassert>
#endif

#include "ac_window_1d_flag.h"
template<class T, int AC_WN, int AC_NCOL, ac_window_mode AC_WMODE = AC_WIN>
class ac_window_1d_array{
public:
  ac_window_1d_array(T *s, int n);
  int operator++ ();
  int operator++ (int) { // non-canonical form (s.b. const window)
    return this->operator++(); }
  T & operator[]      (int i);
  const T & operator[](int i) const ;
  int pos(){
    return cnt_;
  }
  bool valid(){
    return w.valid();
  }
private:
  enum {
    logAC_NCOL = (AC_NCOL < 2 ? 1 : AC_NCOL < 4 ? 2 : AC_NCOL < 8 ? 3 : AC_NCOL < 16 ? 4 : 
                  AC_NCOL < 32 ? 5 : AC_NCOL < 64 ? 6 : AC_NCOL < 128 ? 7 : AC_NCOL < 256 ? 8 :
                  AC_NCOL < 512 ? 9 : AC_NCOL < 1024 ? 10 : AC_NCOL < 2048 ? 11 : AC_NCOL < 4096 ? 12 :
                  AC_NCOL < 8192 ? 13 : AC_NCOL < 16384 ? 14 : AC_NCOL < 32768 ? 15 : 
                  AC_NCOL < 65536 ? 16 : 32)
  };
  ac_window_1d_flag<T,AC_WN,AC_WMODE> w;   //window flag class
  ac_int<logAC_NCOL+1> cnt_;                               // Counter, state variable used to determine if:
  //  1/ window has ramped up and data is valid
  //  2/ what and when and how to mux value from data_ to wout_
  ac_int<logAC_NCOL,false>  sz_;                   // size of current input array
  T  *src_;       // the array for which this is a window
  bool sol_;
  bool eol_;
};

// The only defined CTOR
template<class T, int AC_WN, int AC_NCOL, ac_window_mode AC_WMODE>
ac_window_1d_array<T, AC_WN, AC_NCOL, AC_WMODE>::ac_window_1d_array(T *s, int sz) : 
  cnt_(-AC_WN/2), sz_(sz), src_(s), eol_(0) 
{
}

template<class T, int AC_WN, int AC_NCOL, ac_window_mode AC_WMODE>
int ac_window_1d_array<T, AC_WN, AC_NCOL, AC_WMODE>::operator++ () {
  T tmp=0;  
  if(cnt_==-AC_WN/2)
    sol_ = 1;
  else
    sol_ = 0;     
  if (cnt_ == sz_ -1 - AC_WN/2) 
    eol_ = 1;
  else
    eol_ = 0; 
  if(cnt_ < sz_ - AC_WN/2 && cnt_ < AC_NCOL -AC_WN/2)
    tmp =  src_[cnt_+AC_WN/2];
  //w.write(src_[cnt_ + AC_WN/2 + 1],sol_,eol_);
  w.write(tmp,sol_,eol_);
  
  //if (cnt_<(sz_prev_-1)) cnt_++; else cnt_=0;
  cnt_++;//count is always reset in the constructor
  return cnt_;
}

template<class T, int AC_WN, int AC_NCOL, ac_window_mode AC_WMODE>
inline  T & ac_window_1d_array<T, AC_WN, AC_NCOL, AC_WMODE>::operator[] (int i)
{
  return w[i];
}
template<class T, int AC_WN, int AC_NCOL, ac_window_mode AC_WMODE>
inline  const T & ac_window_1d_array<T, AC_WN, AC_NCOL, AC_WMODE>::operator[](int i) const
{
  return w[i];
}
#endif
