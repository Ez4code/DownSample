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
//
////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
  ===========================================================================
  Source:         mgc_sc_math.h
  Description:    synthesizable math functions for SystemC integer (bigint)
                  and fixed point datatypes
  ===========================================================================

  Overview: the functions supplied here have been parameterized so that
    the precision of the input(s) and the output(s) determine the precision
    of the computation and thus the hardware that Catapult C will synthesize.
    These functions are generic (e.g, radix 2 division) and are meant to 
    provide flexibility in trading off performance and area by unrolling the
    iteration loop.  
     
  Datatypes: the functions support the SystemC datatypes:
    - sc_bigint, sc_biguint
    - sc_fixed, sc_ufixed
        + inputs: any rounding and overflow mode
        + outputs: any rounding/overflow modes 

  Functions currently supported:
    - Division: 
        + inputs: dividend, divisor
        + outputs: quotient, returns true if remainder is nonzero (for
            fixed point, the remainder depends on the LSB precision of
            the quotient). 
            For integer divisions, remainder output is also available. 
   
        * Integer Unsigned
          bool div(sc_biguint<NW> dividend, sc_biguint<DW> divisor, 
                   sc_biguint<QW> &quotient, sc_biguint<RW> &remainder) 
          bool div(sc_biguint<NW> dividend, sc_biguint<DW> divisor, 
                   sc_biguint<QW> &quotient) 

        * Integer Signed
          bool div(sc_bigint<NW> dividend, sc_bigint<DW> divisor, 
                   sc_bigint<QW> &quotient, sc_bigint<RW> &remainder) 
          bool div(sc_bigint<NW> dividend, sc_bigint<DW> divisor, 
                   sc_bigint<QW> &quotient) 
    
        * Fixed Point unsigned
          bool div(sc_ufixed<NW,NI,NQ,NO> dividend,
                   sc_ufixed<DW,DI,DQ,DO> divisor,
                   sc_ufixed<QW,QI,QQ,QO> &quotient)

        * Fixed Point signed
          bool div(sc_fixed<NW,NI,NQ,NO> dividend,
                   sc_fixed<DW,DI,DQ,DO> divisor,
                   sc_fixed<QW,QI,QQ,QO> &quotient)

    - Square Root: 
        + inputs: argument
        + outputs: square root
     
        * Integer Unsigned
          void sqrt(sc_biguint<XW> x, sc_biguint<OW> &sqrt)
         
        * Fixed Point Unsigned
          void sqrt(sc_ufixed<XW,XI,XQ,XO> x, sc_ufixed<OW,OI,OQ,OO> &sqrt) 

    - Shifts:
        + inputs: argument, shift count
        + output: shifted result

        * Fixed Point Unsigned, signed shift value
          void shift_right(sc_ufixed<XW,XI,XQ,XO> x, int n, 
                           sc_ufixed<OW,OI,OQ,OO> &sr)
          void shift_left(sc_ufixed<XW,XI,XQ,XO> x, int n, 
                          sc_ufixed<OW,OI,OQ,OO> &sl)

        * Fixed Point Unsigned, unsigned shift value
          void shift_right(sc_ufixed<XW,XI,XQ,XO> x, unsigned int n, 
                           sc_ufixed<OW,OI,OQ,OO> &sr)
          void shift_left(sc_ufixed<XW,XI,XQ,XO> x, unsigned int n, 
                          sc_ufixed<OW,OI,OQ,OO> &sl)
        
        * Fixed Point Signed, signed shift value
          void shift_right(sc_fixed<XW,XI,XQ,XO> x, int n, 
                           sc_fixed<OW,OI,OQ,OO> &sr)
          void shift_left(sc_fixed<XW,XI,XQ,XO> x, int n, 
                          sc_fixed<OW,OI,OQ,OO> &sl)

        * Fixed Point Signed, unsigned shift value
          void shift_right(sc_fixed<XW,XI,XQ,XO> x, unsigned int n, 
                           sc_fixed<OW,OI,OQ,OO> &sr)
          void shift_left(sc_fixed<XW,XI,XQ,XO> x, unsigned int n, 
                          sc_fixed<OW,OI,OQ,OO> &sl)

    - Sin/Cos:
        sin(angle_over_pi*PI):
          void sin(sc_fixed<AW,AI,AQ,AO> angle_over_pi,
                   sc_fixed<OW,OI,OQ,OO> &sin)
        
        cos(angle_over_pi*PI):
          void cos(sc_fixed<AW,AI,AQ,AO> angle_over_pi,
                   sc_fixed<OW,OI,OQ,OO> &cos)
      
        C*sin(angle_over_pi*PI), C*cos(angle_over_pi*PI):
          void sin_cos(sc_fixed<AW,AI,AQ,AO> angle_over_pi,
                       sc_fixed<OW,OI,OQ,OO> C,
                       sc_fixed<OW,OI,OQ,OO> &sin,
                       sc_fixed<OW,OI,OQ,OO> &cos)

    - Atan2: similar functionality as math.h atan2(y,x)
        void atan2(sc_fixed<YW,YI,YQ,YO> y,
                   sc_fixed<XW,XI,XQ,XO> x,
                   sc_fixed<OW,OI,OQ,OO> &atan)

  ===========================================================================

  Notes on Compilation: if during compilation you get errors about
  "no matching functions" check the following:
    - matching datatype (integer or fixed point, signed or unsigned) for
      inputs and outputs
    - when using sin_cos, the input argument C and output arguments 
      sin and cos should have the same datatype 

  ===========================================================================

  Notes on Simulation: div/sqrt/shift functions are bitwise accurate with 
  the correspoding math.h (double) function provided the datatypes dont
  go beyond the precision of double. For instance, 

     sc_fixed<20,6> n = ...;
     sc_fixed<5,0> d = ...;
     sc_fixed<40,14> q, q1; 
     div(n, d, q);
     q1 = (double)n/(double)/d;
     assert(q == q1);

  Transcendental functions (e.g, trigonometric) will not be bit accurate
  compared with the math.h (double) function. The difference should be
  confined to the quantization error of the requested output.
   
  ===========================================================================

  Notes on Synthesis: constraining the precision for both inputs and outputs
  will deliver the best quality of results.

****************************************************************************/

#ifndef __MGC_SC_MATH_H
#define __MGC_SC_MATH_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

#include <systemc.h>

#ifndef __MGC_MIN
#define __MGC_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef __MGC_MAX
#define __MGC_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

//////////////////////////////////////////////////////////////////////////////
//                              I N T E G E R 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Division 
//////////////////////////////////////////////////////////////////////////////

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int DW, int QW, int RW > 
bool div(
  sc_biguint<NW> dividend,
  sc_biguint<DW> divisor,
  sc_biguint<QW> &quotient,
  sc_biguint<RW> &remainder
){
  sc_biguint<QW> Q = 0;
  sc_bigint<DW+1> R = 0; 
  sc_bigint<DW+1> neg_divisor = -divisor;
  for(int i=0; i < NW; i++) {
    // take MSB of divd, shift it in from right to R
    R = ((R << 1) | ((dividend >> (NW-1)) & 1)) + 
      ( (R < 0) ? (sc_bigint<DW+1>) divisor : neg_divisor );
    Q = (Q << 1) | ((R >= 0) & 1);
    dividend <<= 1;
  }
  if(R < 0)
    R += divisor;
  quotient = Q;
  remainder = R;
  return R != 0;
}

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int DW, int QW >
bool div(
  sc_biguint<NW> dividend,
  sc_biguint<DW> divisor,
  sc_biguint<QW> &quotient
){
  sc_biguint<DW> remainder;
  div(dividend, divisor, quotient, remainder);
  return remainder != 0;
}

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int DW, int QW, int RW >
bool div(
  sc_bigint<NW> dividend,
  sc_bigint<DW> divisor,
  sc_bigint<QW> &quotient,
  sc_bigint<RW> &remainder
){
  bool neg_dividend = dividend < 0;
  sc_biguint<NW> uN = neg_dividend ? (sc_biguint<NW>) -dividend : (sc_biguint<NW>) dividend;
  bool neg_divisor = divisor < 0;
  sc_biguint<DW> uD = neg_divisor ? (sc_biguint<DW>) -divisor : (sc_biguint<DW>) divisor;
  sc_biguint<QW> uQ;
  sc_biguint<DW> uR; 
  div(uN, uD, uQ, uR);
  quotient = neg_dividend == neg_divisor ? (sc_bigint<QW>) uQ : (sc_bigint<QW>) -uQ; 
  sc_bigint<RW> rem = neg_dividend ? (sc_bigint<DW>) -uR : (sc_bigint<DW>) uR;
  remainder = rem;
  return rem != 0;
}

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int DW, int QW >
bool div(
  sc_bigint<NW> dividend,
  sc_bigint<DW> divisor,
  sc_bigint<QW> &quotient
){
  sc_bigint<DW> remainder;
  div(dividend, divisor, quotient, remainder);
  return remainder != 0;
}

//////////////////////////////////////////////////////////////////////////////
// Square Root 
//////////////////////////////////////////////////////////////////////////////
template< int XW, int OW >
void sqrt(
  sc_biguint<XW> x,
  sc_biguint<OW> &sqrt
) {
  const int RW = (XW+1)/2;
  // masks used only to hint synthesis on precision
  sc_biguint<RW+2> mask_d = 0;

  sc_biguint<RW+2> d = 0;
  sc_biguint<RW> r = 0;
  sc_biguint<2*RW> z = x; 

  // needs to pick 2 bits of z for each iteration starting from
  // the 2 MSB bits. Inside loop, z will be shifted left by 2 each
  // iteration. The bits of interest are always on the same
  // position (z_shift+1 downto z_shift)
  unsigned int z_shift = (RW-1)*2;

  for(int i = RW-1; i >= 0; i--) {
    r <<= 1;

    mask_d = (mask_d << 2) | 0x3;
    d = mask_d & (d << 2) | ((z >> z_shift) & 0x3 );

    sc_biguint<RW+2> t = d - (( ((sc_biguint<RW+1>)r) << 1) | 0x1);
    if( !t[RW+1] ) {  // since t is unsigned, look at MSB
      r |= 0x1;
      d = mask_d & t;
    }
    z <<= 2;
  }
  sqrt = r;
}
#endif // __MGC_SC_MATH_H

#ifdef SC_INCLUDE_FX
#ifndef __MGC_SC_MATH_FIXED_POINT_H
#define __MGC_SC_MATH_FIXED_POINT_H
//////////////////////////////////////////////////////////////////////////////
//                         F I X E D   P O I N T 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Shifts
//////////////////////////////////////////////////////////////////////////////

#pragma calypto_flag RSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_right(sc_ufixed<XW,XI,XQ,XO> x, int n, sc_ufixed<OW,OI,OQ,OO> &sr) {
  const int R_BIT = (OQ == SC_TRN || OQ == SC_TRN_ZERO) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND || OQ == SC_RND_INF) ? 0 : 1;
  const int S_OVER = (OO == SC_WRAP) ? 0 : 1; 
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_ufixed<TW,TI> t = x;
  t >>= n;

  sc_ufixed<TW+R_HALF+S_OVER,TI+S_OVER> t2 = t; 
  
  if(R_HALF || S_OVER) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 <<= n;
    sc_ufixed<XW,XI> mask = ~(sc_ufixed<XW,XI>) m1;
    if(n >= 0) {
      if(R_HALF)
        t2[0] = ((x & mask) != 0) ? 1 : 0;
    }
    else { 
      if(S_OVER)
        t2[TW+R_HALF+S_OVER-1] = ((x & mask) != 0) ? 1 : 0; 
    }
  }
  sr = t2;
}

#pragma calypto_flag RSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_right(sc_ufixed<XW,XI,XQ,XO> x, unsigned int n, sc_ufixed<OW,OI,OQ,OO> &sr) {
  const int R_BIT = (OQ == SC_TRN || OQ == SC_TRN_ZERO) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND || OQ == SC_RND_INF) ? 0 : 1;
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  unsigned un = 0x7FFFFFFF & n;
  sc_ufixed<TW,TI> t = x;
  t >>= un;

  sc_ufixed<TW+R_HALF,TI> t2 = t;
 
  if(R_HALF) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 <<= un;
    sc_ufixed<XW,XI> mask = ~(sc_ufixed<XW,XI>) m1;
    t2[0] = ((x & mask) != 0) ? 1 : 0;
  }
  sr = t2;
}

#pragma calypto_flag RSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_right(sc_fixed<XW,XI,XQ,XO> x, int n, sc_fixed<OW,OI,OQ,OO> &sr) {
  const int R_BIT = (OQ == SC_TRN) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND) ? 0 : 1;
  const int S_OVER = (OO == SC_WRAP) ? 1 : 2;
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_fixed<TW,TI> t = x;
  t >>= n;
  
  sc_fixed<TW+R_HALF+S_OVER,TI+S_OVER> t2 = t;

  if(R_HALF || S_OVER==2) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 <<= n;
    sc_fixed<XW,XI> mask = ~(sc_fixed<XW,XI>) m1;
    if(n >= 0) {
      if(R_HALF) 
        t2[0] = ((x & mask) != 0) ? 1 : 0;
    }
    else {
      t2[TW+R_HALF+S_OVER-1] = x[XW-1];
      if(S_OVER==2 && mask != 0) {
        t2[TW+R_HALF+S_OVER-2] = ((x & mask) == 0) ? 0 : ((~x & mask) == 0) ? 1 : x[XW-1] ? 0 : 1;
      }
    }
  }
  sr = t2;
}

#pragma calypto_flag RSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_right(sc_fixed<XW,XI,XQ,XO> x, unsigned int n, sc_fixed<OW,OI,OQ,OO> &sr) {
  const int R_BIT = (OQ == SC_TRN) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND) ? 0 : 1;
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_fixed<TW,TI> t = x;
  t >>= n;
  unsigned un = 0x7FFFFFFF & n;

  sc_fixed<TW+R_HALF,TI> t2 = t;

  if(R_HALF) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 <<= un;
    sc_fixed<XW,XI> mask = ~(sc_fixed<XW,XI>) m1;
    t2[0] = ((x & mask) != 0) ? 1 : 0;
  }
  sr = t2;
}

#pragma calypto_flag LSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_left(sc_ufixed<XW,XI,XQ,XO> x, int n, sc_ufixed<OW,OI,OQ,OO> &sl) {
  const int R_BIT = (OQ == SC_TRN || OQ == SC_TRN_ZERO) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND || OQ == SC_RND_INF) ? 0 : 1;
  const int S_OVER = (OO == SC_WRAP) ? 0 : 1;
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_ufixed<TW,TI> t = x;
  t <<= n;

  sc_ufixed<TW+R_HALF+S_OVER,TI+S_OVER> t2 = t;

  if(R_HALF || S_OVER) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 >>= n;
    sc_ufixed<XW,XI> mask = ~(sc_ufixed<XW,XI>) m1;
    if(n < 0) {
      if(R_HALF)
        t2[0] = ((x & mask) != 0) ? 1 : 0;
    }
    else {
      if(S_OVER)
        t2[TW+R_HALF+S_OVER-1] = ((x & mask) != 0) ? 1 : 0;
    }
  }
  sl = t2;
}

#pragma calypto_flag LSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_left(sc_ufixed<XW,XI,XQ,XO> x, unsigned int n, sc_ufixed<OW,OI,OQ,OO> &sl) {
  const int S_OVER = (OO == SC_WRAP) ? 0 : 1;
  const int TF = __MGC_MAX(XW-XI, OW-OI);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_ufixed<TW,TI> t = x;
  t <<= n;

  sc_ufixed<TW+S_OVER,TI+S_OVER> t2 = t;

  if(S_OVER) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 >>= n;
    sc_ufixed<XW,XI> mask = ~(sc_ufixed<XW,XI>) m1;
    t2[TW+S_OVER-1] = ((x & mask) != 0) ? 1 : 0;
  }
  sl = t2;
}

#pragma calypto_flag LSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_left(sc_fixed<XW,XI,XQ,XO> x, int n, sc_fixed<OW,OI,OQ,OO> &sl) {
  const int R_BIT = (OQ == SC_TRN) ? 0 : 1;
  const int R_HALF = (R_BIT == 0 || OQ == SC_RND) ? 0 : 1;
  const int S_OVER = (OO == SC_WRAP) ? 1 : 2;
  const int TF = __MGC_MAX(XW-XI, OW-OI+R_BIT);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_fixed<TW,TI> t = x;
  t <<= n;

  sc_fixed<TW+R_HALF+S_OVER,TI+S_OVER> t2 = t;

  if(R_HALF || S_OVER==2) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 >>= n;
    sc_fixed<XW,XI> mask = ~(sc_fixed<XW,XI>) m1;
    if(n < 0) {
      if(R_HALF)
        t2[0] = ((x & mask) != 0) ? 1 : 0;
    }
    else {
      t2[TW+R_HALF+S_OVER-1] = x[XW-1];
      if(S_OVER==2 && mask != 0) {
        t2[TW+R_HALF+S_OVER-2] = ((x & mask) == 0) ? 0 : ((~x & mask) == 0) ? 1 : x[XW-1] ? 0 : 1;
      }
    }
  }
  sl = t2;
}

#pragma calypto_flag LSHIFT_CHECK EXPR=1 CONSTANT=2
template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void shift_left(sc_fixed<XW,XI,XQ,XO> x, unsigned int n, sc_fixed<OW,OI,OQ,OO> &sl) {
  const int S_OVER = (OO == SC_WRAP) ? 1 : 2;
  const int TF = __MGC_MAX(XW-XI, OW-OI);
  const int TI = __MGC_MAX(XI, OI);
  const int TW = TI + TF;
  sc_fixed<TW,TI> t = x;
  t <<= n;

  sc_fixed<TW+S_OVER,TI+S_OVER> t2 = t;

  if(S_OVER==2) {
    sc_ufixed<TW,TI> m1 = ~(sc_ufixed<TW,TI>) 0;
    m1 >>= n;
    sc_fixed<XW,XI> mask = ~(sc_fixed<XW,XI>) m1;
    t2[TW+S_OVER-1] = x[XW-1];
    if(mask != 0) {
      t2[TW+S_OVER-2] = ((x & mask) == 0) ? 0 : ((~x & mask) == 0) ? 1 : x[XW-1] ? 0 : 1;
    }
  }
  sl = t2;
}

//////////////////////////////////////////////////////////////////////////////
// Division 
//////////////////////////////////////////////////////////////////////////////

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int NI, sc_q_mode NQ, sc_o_mode NO,
          int DW, int DI, sc_q_mode DQ, sc_o_mode DO, 
          int QW, int QI, sc_q_mode QQ, sc_o_mode QO > 
bool div(
  sc_ufixed<NW,NI,NQ,NO> dividend,
  sc_ufixed<DW,DI,DQ,DO> divisor,
  sc_ufixed<QW,QI,QQ,QO> &quotient
){
  // relevant bits for Q
 
  const int RBIT = (QQ == SC_TRN || QQ == SC_TRN_ZERO) ? 0 : 1; 
  const int QF = (QW-QI) + RBIT;
  const int ZI = NI+(DW-DI);
  if(ZI-1 < -QF) {
    // MSB of result is smaller than LSB of requested output
    quotient = 0;
    return dividend != 0;
  }
  // max is to used to avoid compilation problems with non pos bitwidth
  const int ZF = __MGC_MAX(QF, -ZI+1);
  const int ZW = ZI + ZF; 

  sc_ufixed<NW,NW> N_fx;
  shift_left(dividend, NW-NI, N_fx);
  sc_biguint<NW> N = (sc_biguint<NW>) N_fx;
  sc_ufixed<DW,DW> D_fx;
  shift_left(divisor, DW-DI, D_fx);
  sc_biguint<DW> D = (sc_biguint<DW>) D_fx;

  sc_biguint<ZW> Q = 0;
  sc_bigint<DW+1> R = 0;
  sc_bigint<DW+1> neg_D = -D;
  for(int i=0; i < ZW; i++) {
    // take MSB of N, shift it in from right to R
    R = ((R << 1) | ((N >> (NW-1)) & 1)) +
      ( (R < 0) ? (sc_bigint<DW+1>) D : neg_D );
    Q = (Q << 1) | ((R >= 0) & 1);
    N <<= 1;
  }
  if(R < 0)
    R += D; 

  bool rem = (R != 0) || ((N >> ZW) != 0);

  sc_ufixed<ZW+1,ZW> Q_fx = (sc_ufixed<ZW+1,ZW>)Q;
  if(QQ == SC_RND_ZERO || QQ == SC_RND_MIN_INF || QQ == SC_RND_CONV)
    Q_fx[0] = rem; 

  shift_right(Q_fx, (NW-NI) - (DW-DI) + (ZW-NW), quotient);
  return rem || (RBIT && Q[0]);
}

#pragma calypto_flag DIV0_CHECK DIVISOR=2
template< int NW, int NI, sc_q_mode NQ, sc_o_mode NO,
          int DW, int DI, sc_q_mode DQ, sc_o_mode DO, 
          int QW, int QI, sc_q_mode QQ, sc_o_mode QO >
bool div(
  sc_fixed<NW,NI,NQ,NO> dividend,
  sc_fixed<DW,DI,DQ,DO> divisor,
  sc_fixed<QW,QI,QQ,QO> &quotient
){
  const int ZI = (QO == SC_WRAP) ? QI : NI+(DW-DI);
  const int ZW = ZI+(QW-QI); 

  sc_fixed<NW,NI> N = dividend;
  sc_fixed<DW,DI> D = divisor;

  bool neg_N = N < 0;
  sc_ufixed<NW,NI> uN = neg_N ? (sc_ufixed<NW,NI>) -N : (sc_ufixed<NW,NI>) N;
  bool neg_D = D < 0;
  sc_ufixed<DW,DI> uD = neg_D ? (sc_ufixed<DW,DI>) -D : (sc_ufixed<DW,DI>) D;

  bool has_rem;
  if( QQ == SC_RND_ZERO || QQ == SC_RND_INF || 
      QQ == SC_RND_CONV || QQ == SC_TRN_ZERO ) {
    sc_ufixed<ZW,ZI,QQ> uQ;
    sc_fixed<ZW+1,ZI+1,QQ> Q;
    has_rem = div(uN, uD, uQ);
    if(neg_N == neg_D)
      Q = uQ;
    else
      Q = -uQ;
    quotient = Q;
  }
  else {
    const int RBIT = (QQ == SC_TRN) ? 0 : 1; 
    sc_ufixed<ZW+RBIT,ZI> uQ;
    sc_fixed<ZW+RBIT+2,ZI+1> Q;
    has_rem = div(uN, uD, uQ);
    if(neg_N == neg_D) {
      Q = uQ;
      if(QQ == SC_RND_MIN_INF)
        Q[0] = has_rem ? 1 : 0;
    }
    else {
      sc_ufixed<ZW+RBIT,ZI> lsb = 0;
      lsb[0] = has_rem && QQ != SC_RND_MIN_INF ? 1 : 0;
      Q = -(uQ + lsb);
    }
    quotient = Q;
    has_rem |= RBIT && uQ[0];
  }
  return has_rem;
}

//////////////////////////////////////////////////////////////////////////////
// Square Root 
//////////////////////////////////////////////////////////////////////////////

template< int XW, int XI, sc_q_mode XQ, sc_o_mode XO,
          int OW, int OI, sc_q_mode OQ, sc_o_mode OO >
void sqrt(
  sc_ufixed<XW,XI,XQ,XO> x,
  sc_ufixed<OW,OI,OQ,OO> &sqrt
) {
  const int RBIT = (OQ == SC_TRN || OQ == SC_TRN_ZERO) ? 0 : 1;
  const int OF = (OW-OI) + RBIT;

  const int RI = (XI+1)/2;
  if(RI-1 < -OF) {
    // MSB of result is smaller than LSB of requested output
    sqrt = 0;
    return;
  }

  // max is used to avoid compilation problems with non pos bitwidth
  const int RF = __MGC_MAX(OF, -RI+1);  // OF may be negative
  const int RW = RI + RF;

  // store relevant bits of x in z 
  const int ZF = 2*__MGC_MIN((XW-XI+1)/2, RF);
  const int ZW = 2*RI+ZF;
  sc_ufixed<ZW,ZW> z_fx;
  shift_left(x, ZF, z_fx); 
  sc_biguint<ZW> z = (sc_biguint<ZW>) z_fx; 
  
  // masks used only to hint synthesis on precision
  sc_biguint<RW+2> mask_d = 0;

  sc_biguint<RW+2> d = 0;
  sc_biguint<RW> r = 0;

  // needs to pick 2 bits of z for each iteration starting from
  // the 2 MSB bits. Inside loop, z will be shifted left by 2 each
  // iteration. The bits of interest are always on the same
  // position (z_shift+1 downto z_shift)
  unsigned int z_shift = ZW-2;

  for(int i = RW-1; i >= 0; i--) {
    r <<= 1;

    mask_d = (mask_d << 2) | 0x3;
    d = mask_d & (d << 2) | ((z >> z_shift) & 0x3 );

    sc_biguint<RW+2> t = d - (( ((sc_biguint<RW+1>)r) << 1) | 0x1); 
    if( !t[RW+1] ) {  // since t is unsigned, look at MSB
      r |= 0x1;
      d = mask_d & t;
    }
    z <<= 2;
  }

  sc_ufixed<RW+1,RW> r2 = (sc_ufixed<RW+1,RW>) r;
  if(OQ == SC_RND_ZERO || OQ == SC_RND_MIN_INF || OQ == SC_RND_CONV) {
    bool rem = (d != 0) || ((z >> 2*RW) != 0);
    if(ZF < (XW-XI)) {
      // max is to used to avoid compilation problems with non pos bitwidth
      const int rbits = __MGC_MAX((XW-XI)-ZF,1);
      sc_ufixed<rbits,-ZF> zr = x;
      rem |= (zr != 0);
    }
    r2[0] =  rem ? 1 : 0;
  }
  shift_right(r2, RF, sqrt);

}

//////////////////////////////////////////////////////////////////////////////
// Sine/Cosine/Arctan
//////////////////////////////////////////////////////////////////////////////

#include "mgc_sc_trig.h"

//////////////////////////////////////////////////////////////////////////////
#endif // __MGC_SC_MATH_FIXED_POINT_H
#endif // SC_INCLUDE_FX
