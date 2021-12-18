/*! /file This file provides random number generators mc_random(T&)
 *  for all types T Catapult considers as primitive types. These
 *  functions are used in the automatically generated testbench.
 */


#include <stdlib.h>
#include <limits.h>

#ifndef MC_RANDOM_H_INC
#define MC_RANDOM_H_INC

// Check for macro definitions that will conflict with template parameter names in this file
#if defined(T)
#define T 0
#error The macro name 'T' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'T' giving the location of the previous definition.
#endif
#if defined(size)
#define size 0
#error The macro name 'size' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'size' giving the location of the previous definition.
#endif
#if defined(SIZE)
#define SIZE 0
#error The macro name 'SIZE' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'SIZE' giving the location of the previous definition.
#endif
#if defined(SMALL)
#define SMALL 0
#error The macro name 'SMALL' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'SMALL' giving the location of the previous definition.
#endif
#if defined(W)
#define W 0
#error The macro name 'W' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'W' giving the location of the previous definition.
#endif
#if defined(S)
#define S 0
#error The macro name 'S' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'S' giving the location of the previous definition.
#endif
#if defined(I)
#define I 0
#error The macro name 'I' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'I' giving the location of the previous definition.
#endif
#if defined(Q)
#define Q 0
#error The macro name 'Q' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Q' giving the location of the previous definition.
#endif
#if defined(O)
#define O 0
#error The macro name 'O' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'O' giving the location of the previous definition.
#endif

template <typename T, int size>
struct mc_random_c_builtin_s
{
  void operator()(T& v);
};

template <typename T, int size>
void mc_random_c_builtin_s<T, size>::operator()(T& v)
{
  const int long mask = 0xffff;
  v = (rand() & mask);
  for (int i = 1; i < size; ++i) {
    v <<= 16;
    v |= (rand() & mask);
  }
}

template <typename T>
struct mc_random_c_builtin_s<T,0>
{
  void operator()(T& v) { v = rand(); }
};

template <typename T> 
struct mc_random_c_builtin_s<T,1>
{
  void operator()(T& v) { v = rand(); }
};

template <typename T>
inline void mc_random_c_builtin(T& v)
{
  enum { size = (sizeof(T) * CHAR_BIT) / 16 };
  mc_random_c_builtin_s<T,size>()(v);
}

inline void mc_random(long long int& v) { mc_random_c_builtin(v); }
inline void mc_random(long long unsigned& v) { mc_random_c_builtin(v); }
inline void mc_random(long int& v) { mc_random_c_builtin(v); }
inline void mc_random(long unsigned& v) { mc_random_c_builtin(v); }
inline void mc_random(int& v) { mc_random_c_builtin(v); }
inline void mc_random(unsigned& v) { mc_random_c_builtin(v); }
inline void mc_random(short int& v) { mc_random_c_builtin(v); }
inline void mc_random(short unsigned& v) { mc_random_c_builtin(v); }
inline void mc_random(char& v) { mc_random_c_builtin(v); }
inline void mc_random(signed char& v) { mc_random_c_builtin(v); }
inline void mc_random(unsigned char& v) { mc_random_c_builtin(v); }
inline void mc_random(bool& v) { v = rand() & 1; }
#endif // MC_RANDOM_H_INC

#if (defined(SYSTEMC_INCLUDED) || defined(SYSTEMC_H)) && !defined(MC_SYSTEMC)
#define MC_SYSTEMC
#endif

#if defined(MC_SYSTEMC) && !defined(MC_RANDOM_H_INC_SYSTEMC_INCLUDED)
#define MC_RANDOM_H_INC_SYSTEMC_INCLUDED
template <typename T>
void mc_systemc_builtin_it(T& v, const int size)
{
  const int long mask = 0xffff;
  int lo = 0;
  int hi = 15;
  for (; hi < size; lo += 16, hi += 16) {
    v.range(hi, lo) = (rand() & mask);
  }
  if (lo < size) {
    v.range(size - 1, lo) = (rand() & mask);
  }
}

inline void mc_random(sc_int_base& v) { mc_systemc_builtin_it(v, v.length()); }
inline void mc_random(sc_uint_base& v) { mc_systemc_builtin_it(v, v.length()); }
inline void mc_random(sc_signed& v) { mc_systemc_builtin_it(v, v.length()); }
inline void mc_random(sc_unsigned& v) { mc_systemc_builtin_it(v, v.length()); }
template <int Tlen>
inline void mc_random(sc_bv<Tlen>& v) { mc_systemc_builtin_it(v, Tlen); }
template <int Tlen>
inline void mc_random(sc_lv<Tlen>& v) { mc_systemc_builtin_it(v, Tlen); }
#endif // MC_RANDOM_H_INC_SYSTEMC_INCLUDED

#if defined(SC_INCLUDE_FX) && defined(MC_SYSTEMC) && !defined(MC_RANDOM_H_INC_SC_INCLUDE_FX)
#define MC_RANDOM_H_INC_SC_INCLUDE_FX

void mc_random(sc_fxnum& v) {  mc_systemc_builtin_it(v, v.wl()); }

#endif // MC_RANDOM_H_INC_SC_INCLUDE_FX


#if defined(__AC_INT_H) && !defined(MC_RANDOM_H_INC__AC_INT_H)
#define MC_RANDOM_H_INC__AC_INT_H

template <class T, int SIZE, bool SMALL>
struct mc_random_ac_s
{
  void operator()(T& v);
};

template <class T, int SIZE, bool SMALL>
void mc_random_ac_s<T,SIZE,SMALL>::operator()(T& v)
{
  const ac_int<6> ac_16 = 16;
  ac_int<16> rv = rand();
  v.set_slc(0, rv);
  for (int i = 16; i < SIZE; i += 16) {
    v <<= ac_16;
    rv = rand();
    v.set_slc(0, rv);
  }
}

template <class T, int SIZE>
struct mc_random_ac_s<T,SIZE,true>
{
  void operator()(T& v) { v.set_slc(0, ac_int<SIZE>(rand())); }
};

template <int W, bool S>
inline void mc_random(ac_int<W,S>& v)
{
  mc_random_ac_s<ac_int<W,S>, W, W<=16>()(v);
}


#endif // MC_RANDOM_H_INC__AC_INT_H

#if defined(__AC_FIXED_H) && !defined(MC_RANDOM_H_INC__AC_FIXED_H)
#define MC_RANDOM_H_INC__AC_FIXED_H

template <int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
inline void mc_random(ac_fixed<W,I,S,Q,O>& v)
{
  mc_random_ac_s<ac_fixed<W,I,S,Q,O>, W, W<=16>()(v);
}

#endif // MC_RANDOM_H_INC__AC_FIXED_H

#if defined(__AC_CHANNEL_H) && !defined(MC_RANDOM_H_INC__AC_CHANNEL_H)
#define MC_RANDOM_H_INC__AC_CHANNEL_H

// Used to extract the sub type of the channel
template <class Tchantype> struct mc_extract_chan_subtype;
template <class Tsubtype>
struct mc_extract_chan_subtype<ac_channel<Tsubtype> > {
   typedef Tsubtype chan_subtype;
};

#endif // MC_RANDOM_H_INC__AC_CHANNEL_H
