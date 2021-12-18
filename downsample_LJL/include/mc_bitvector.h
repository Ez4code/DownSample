////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2011 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Source:         mc_bitvector.h
//  Description:    bitvector class representation for signed/unsigned
//                  bitvectors in C++.
///////////////////////////////////////////////////////////////////////////


#ifndef __MC_BITVECTOR_H
#define __MC_BITVECTOR_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

//  Include main header
#include "mc_base.h"


//////////////////////////////////////////////////////////////////////////////
//  Bit-Vectors
//////////////////////////////////////////////////////////////////////////////
template <int Twidth, class T=Ulong>
struct bitvector {
#pragma builtin
  T val : Twidth;
  bitvector () {}
  bitvector ( T n ) { val = n; }
  bitvector ( const bitvector &b ) { val = b.val; }
  bitvector &operator = ( const bitvector &b ) { val = b.val; return *this; }
  int width() { return Twidth; }
  T operator = ( T n ) { val = n; return val; }
  operator T () const { return val; }
  T operator -= ( T n ) { val -= n; return val; }
  T operator += ( T n ) { val += n; return val; }
  T operator *= ( T n ) { val *= n; return val; }
  T operator /= ( T n ) { val /= n; return val; }
  T operator %= ( T n ) { val %= n; return val; }
  T operator |= ( T n ) { val |= n; return val; }
  T operator &= ( T n ) { val &= n; return val; }
  T operator ^= ( T n ) { val ^= n; return val; }
  T operator <<= ( T n ) { val <<= n; return val; }
  T operator >>= ( T n ) { val >>= n; return val; }
  T operator ++ () { return ++val; }
  T operator ++ ( int ) { return val++; }
  T operator -- () { return --val; }
  T operator -- ( int ) { return val--; }
  class bitvector_ref {
#pragma builtin
    bitvector &d_bv;
    int d_index;
  public:
    bitvector_ref( bitvector *bv, int index=0 ) : d_bv(*bv), d_index(index) {}
    operator bool () const { return d_bv.val>>d_index & 1; }
    T operator = ( bool val ) const {
      d_bv.val ^= (d_bv.val ^ (val?-1:0)) & (T)1<<d_index;
      return d_bv.val;
    }
    T operator = ( const bitvector_ref &b ) const { return operator =((bool)b); }
  };
  const bitvector_ref operator [] ( unsigned char n ) {
    bitvector_ref bvh( this, n );
    return bvh;
  }
};



///////////////////////////////////////////////////////////////////////////////
//  Predefined for ease of use
///////////////////////////////////////////////////////////////////////////////
typedef bitvector<1,            int>   int1;
typedef bitvector<1,   unsigned int>  uint1;
typedef bitvector<2,     signed int>   int2;
typedef bitvector<2,   unsigned int>  uint2;
typedef bitvector<3,     signed int>   int3;
typedef bitvector<3,   unsigned int>  uint3;
typedef bitvector<4,     signed int>   int4;
typedef bitvector<4,   unsigned int>  uint4;
typedef bitvector<5,     signed int>   int5;
typedef bitvector<5,   unsigned int>  uint5;
typedef bitvector<6,     signed int>   int6;
typedef bitvector<6,   unsigned int>  uint6;
typedef bitvector<7,     signed int>   int7;
typedef bitvector<7,   unsigned int>  uint7;
typedef bitvector<8,     signed int>   int8;
typedef bitvector<8,   unsigned int>  uint8;
typedef bitvector<9,     signed int>   int9;
typedef bitvector<9,   unsigned int>  uint9;
typedef bitvector<10,    signed int>   int10;
typedef bitvector<10,  unsigned int>  uint10;
typedef bitvector<11,    signed int>   int11;
typedef bitvector<11,  unsigned int>  uint11;
typedef bitvector<12,    signed int>   int12;
typedef bitvector<12,  unsigned int>  uint12;
typedef bitvector<13,    signed int>   int13;
typedef bitvector<13,  unsigned int>  uint13;
typedef bitvector<14,    signed int>   int14;
typedef bitvector<14,  unsigned int>  uint14;
typedef bitvector<15,    signed int>   int15;
typedef bitvector<15,  unsigned int>  uint15;
typedef bitvector<16,    signed int>   int16;
typedef bitvector<16,  unsigned int>  uint16;
typedef bitvector<17,    signed int>   int17;
typedef bitvector<17,  unsigned int>  uint17;
typedef bitvector<18,    signed int>   int18;
typedef bitvector<18,  unsigned int>  uint18;
typedef bitvector<19,    signed int>   int19;
typedef bitvector<19,  unsigned int>  uint19;
typedef bitvector<20,    signed int>   int20;
typedef bitvector<20,  unsigned int>  uint20;
typedef bitvector<21,    signed int>   int21;
typedef bitvector<21,  unsigned int>  uint21;
typedef bitvector<22,    signed int>   int22;
typedef bitvector<22,  unsigned int>  uint22;
typedef bitvector<23,    signed int>   int23;
typedef bitvector<23,  unsigned int>  uint23;
typedef bitvector<24,    signed int>   int24;
typedef bitvector<24,  unsigned int>  uint24;
typedef bitvector<25,    signed int>   int25;
typedef bitvector<25,  unsigned int>  uint25;
typedef bitvector<26,    signed int>   int26;
typedef bitvector<26,  unsigned int>  uint26;
typedef bitvector<27,    signed int>   int27;
typedef bitvector<27,  unsigned int>  uint27;
typedef bitvector<28,    signed int>   int28;
typedef bitvector<28,  unsigned int>  uint28;
typedef bitvector<29,    signed int>   int29;
typedef bitvector<29,  unsigned int>  uint29;
typedef bitvector<30,    signed int>   int30;
typedef bitvector<30,  unsigned int>  uint30;
typedef bitvector<31,    signed int>   int31;
typedef bitvector<31,  unsigned int>  uint31;
typedef bitvector<32,    signed int>   int32;
typedef bitvector<32,  unsigned int>  uint32;
typedef bitvector<33,         Slong>   int33;
typedef bitvector<33,         Ulong>  uint33;
typedef bitvector<34,         Slong>   int34;
typedef bitvector<34,         Ulong>  uint34;
typedef bitvector<35,         Slong>   int35;
typedef bitvector<35,         Ulong>  uint35;
typedef bitvector<36,         Slong>   int36;
typedef bitvector<36,         Ulong>  uint36;
typedef bitvector<37,         Slong>   int37;
typedef bitvector<37,         Ulong>  uint37;
typedef bitvector<38,         Slong>   int38;
typedef bitvector<38,         Ulong>  uint38;
typedef bitvector<39,         Slong>   int39;
typedef bitvector<39,         Ulong>  uint39;
typedef bitvector<40,         Slong>   int40;
typedef bitvector<40,         Ulong>  uint40;
typedef bitvector<41,         Slong>   int41;
typedef bitvector<41,         Ulong>  uint41;
typedef bitvector<42,         Slong>   int42;
typedef bitvector<42,         Ulong>  uint42;
typedef bitvector<43,         Slong>   int43;
typedef bitvector<43,         Ulong>  uint43;
typedef bitvector<44,         Slong>   int44;
typedef bitvector<44,         Ulong>  uint44;
typedef bitvector<45,         Slong>   int45;
typedef bitvector<45,         Ulong>  uint45;
typedef bitvector<46,         Slong>   int46;
typedef bitvector<46,         Ulong>  uint46;
typedef bitvector<47,         Slong>   int47;
typedef bitvector<47,         Ulong>  uint47;
typedef bitvector<48,         Slong>   int48;
typedef bitvector<48,         Ulong>  uint48;
typedef bitvector<49,         Slong>   int49;
typedef bitvector<49,         Ulong>  uint49;
typedef bitvector<50,         Slong>   int50;
typedef bitvector<50,         Ulong>  uint50;
typedef bitvector<51,         Slong>   int51;
typedef bitvector<51,         Ulong>  uint51;
typedef bitvector<52,         Slong>   int52;
typedef bitvector<52,         Ulong>  uint52;
typedef bitvector<53,         Slong>   int53;
typedef bitvector<53,         Ulong>  uint53;
typedef bitvector<54,         Slong>   int54;
typedef bitvector<54,         Ulong>  uint54;
typedef bitvector<55,         Slong>   int55;
typedef bitvector<55,         Ulong>  uint55;
typedef bitvector<56,         Slong>   int56;
typedef bitvector<56,         Ulong>  uint56;
typedef bitvector<57,         Slong>   int57;
typedef bitvector<57,         Ulong>  uint57;
typedef bitvector<58,         Slong>   int58;
typedef bitvector<58,         Ulong>  uint58;
typedef bitvector<59,         Slong>   int59;
typedef bitvector<59,         Ulong>  uint59;
typedef bitvector<60,         Slong>   int60;
typedef bitvector<60,         Ulong>  uint60;
typedef bitvector<61,         Slong>   int61;
typedef bitvector<61,         Ulong>  uint61;
typedef bitvector<62,         Slong>   int62;
typedef bitvector<62,         Ulong>  uint62;
typedef bitvector<63,         Slong>   int63;
typedef bitvector<63,         Ulong>  uint63;
typedef bitvector<64,         Slong>   int64;
typedef bitvector<64,         Ulong>  uint64;

///////////////////////////////////////////////////////////////////////////////
#endif // __MC_BITVECTOR_H

