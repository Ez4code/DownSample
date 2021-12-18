////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2016 Mentor Graphics Corp.
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

//////////////////////////////////////////////////////////////////////////////
//  Source:         mgc_ac_hcordic.h
//  Description:    Hyperbolic CORDIC implementations of synthesizable
//                  log/exp/log2/exp2/pow functions for AC fixed point
//                  datatypes
/////////////////////////////////////////////////////////////////////////// */

#ifndef __MGC_AC_HCORDIC_H
#define __MGC_AC_HCORDIC_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

#include <ac_fixed.h>


template <bool b> struct MgcAcHtrigAssert { /* Compile error, no 'test' symbol. */ };
template <> struct MgcAcHtrigAssert<true> { enum { test }; };

namespace MgcAcHtrig {
  //  Multi-precision approximation of tanh(2^-j).
  //  53 + 3 + 1 -- double-precision + extra-iters + padding
  // + 6         -- ceil(log2(57))
  //  63
  static const ac_fixed<63,0,false> hcordic_table[] = {
#include "mgc_ac_hcordic.tab"
  };

  // Multi-precision approximation of tanh(2^-j)/ln(2)  
  static const ac_fixed<63,0,false> hcordic_table_inv_ln2[] = {
#include "mgc_ac_hcordic_inv_ln2.tab"
  };

  static const int shift_dist_table[60] =
    {1, 2, 3, 4, 4, 5, 6, 7, 8, 9 ,
     10,11,12,13,13,14,15,16,17,18,
     19,20,21,22,23,24,25,26,27,28,
     29,30,31,32,33,34,35,36,37,38,
     39,40,40,41,42,43,44,45,46,47,
     48,49,50,51,52,53,54,55,56,57};
  
  // Statically computed log2(x)
  template <int W>
  struct slog2 {
    enum {
      floor = 1 + slog2< (W >> 1) >::floor,
      pow2 = (1 << floor) == W,
      ceil = pow2 ? floor : floor + 1
    };
  };
  
  template <>
  struct slog2<1> {
    enum {
      floor = 0,
      pow2 = 1,
      ceil = 0
    };
  };
  
  template<int J>
  struct XtraIters {
    static const int valid = MgcAcHtrigAssert<(J <= 60)>::test;
    enum {
      // sum-of-cordic-terms for {i | i > j + 4*B2+2*B1+B0} < ulp(iteration(j))
      // j >=14 && j <= 60: B0 + B1 + B2 = 3
      // j >= 4 && j <= 13: B0 + B1 + B2 = 2
      // j >= 1 && j <=  3: B0 + B1 + B2 = 1
      B0 = (1 << 0)*((J >= 1 && J <= 3) || (J >= 14)),
      B1 = (1 << 1)*(J >= 4 && J <= 13),
      B2 = (1 << 2)*(J >= 14)
    };
  };

  static const ac_fixed<159,0,false> _ln2 =
    ac_fixed<159,0,false>(0.69314718055994528) +
    ac_fixed<159,0,false>(2.3190468138462995e-17) +
    ac_fixed<159,0,false>(5.7077084384162112e-34);

  template<int W>
  ac_fixed<W,0,false> ln2() {
    (void)MgcAcHtrigAssert< (W <= 159) >::test;
    return _ln2;
  }

  static const ac_fixed<159,1,false> _inv_ln2 =
    ac_fixed<159,1,false>(1.4426950408889633) +
    ac_fixed<159,1,false>(2.0355273740931030e-17) +
    ac_fixed<159,1,false>(2.0200219154078514e-33);

  template<int W>
  ac_fixed<W,1,false> inv_ln2() {
    (void)MgcAcHtrigAssert< (W <= 158) >::test;
    return _inv_ln2;
  }  

  static const ac_fixed<159,1,false> _inv_K =
    ac_fixed<159,1,false>(1.2074970677630720) +
    ac_fixed<159,1,false>(4.3877290122698160e-17) +
    ac_fixed<159,1,false>(7.4569481788958734e-34);

  template<int W>
  ac_fixed<W+1,1,false> inv_K() {
    (void)MgcAcHtrigAssert< (W <= 158) >::test;
    return _inv_K;
  }

  // Find the positition of the msb (zero-indexed) in an unsigned integer x.
  // r     : msb position.
  // valid : equal to zero if all bits of x are zero, otherwise 1.
  template <int W, int WO>
  struct LOD {
    static void get(ac_int<W,false> x, ac_int<WO,false> &r, bool &valid) {
      const int N = MgcAcHtrig::slog2<W>::ceil - 1;
      ac_int<W, true> mask(-1);
      mask <<= (1 << N);
      bool sel = (x & mask);
      valid |= sel;
      r |= sel << N;
      LOD<(1 << N), WO>::get(sel ? (x >> (1 << N)) : x, r, valid);
    }
  };
  
  template <int WO>
  struct LOD<1, WO> {
    static void get(ac_int<1,false> x, ac_int<WO,false> &r, bool &valid) {
      valid |= (x & 1);
    }
  };  

} // namespace MgcAcHtrig

template< int ZW >
static ac_fixed< ZW, 0, false > hcordic_table(int i) {
  (void)MgcAcHtrigAssert< ZW <= 56 >::test;
  return MgcAcHtrig::hcordic_table[i];
}

template< int ZW >
static int shift_dist(int i) {
  (void)MgcAcHtrigAssert< ZW <= 56 >::test;
  return MgcAcHtrig::shift_dist_table[i];
}


template< int ZW >
static ac_fixed< ZW, 0, false > hcordic_table_inv_ln2(int i) {
  (void)MgcAcHtrigAssert< ZW <= 56 >::test;
  return MgcAcHtrig::hcordic_table_inv_ln2[i];
}

struct HCordicConstants {
  enum mode {
    ROT_Y,
    ROT_Z,
  };
  enum scale {
    SCALE_1,
    SCALE_LN2,
  };
};

template< int W, int I, bool S, ac_q_mode Q, ac_o_mode V,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV,
          enum HCordicConstants::mode mode,
          enum HCordicConstants::scale scale >
void mgc_ac_hcordic(ac_fixed<W,I,S,Q,V> &x,
                    ac_fixed<W,I,S,Q,V> &y,
                    ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  const int L = ZW - ZI + (MgcAcHtrig::XtraIters<ZW-ZI>::B0 +
                           MgcAcHtrig::XtraIters<ZW-ZI>::B1 +
                           MgcAcHtrig::XtraIters<ZW-ZI>::B2);
  const int LW = L+MgcAcHtrig::slog2<L>::ceil;
  typedef ac_fixed<LW,I+1,true> dp_t;
  dp_t xi = x;
  dp_t yi = y;
  dp_t zi = z;
  dp_t yi_d, xi_d;
  dp_t xi_n, yi_n, zi_n;
  for (int j = 0; j < L; j++) {
    int step = shift_dist<LW>(j);
    dp_t table;
    if (scale == HCordicConstants::SCALE_1) {
      table = hcordic_table<LW>(step - 1);
    }
    if (scale == HCordicConstants::SCALE_LN2) {
      table = hcordic_table_inv_ln2<LW>(step - 1);
    }
    bool dir = false; // true -> +, false -> -
    if (mode == HCordicConstants::ROT_Y) {
      dir = yi < 0;
    }
    if (mode == HCordicConstants::ROT_Z) {
      dir = zi >= 0;
    }
    xi_d = xi >> step;
    yi_d = yi >> step;
    if(dir) {
      xi += yi_d;
      yi += xi_d;
      zi -= table;
    } else {
      xi -= yi_d;
      yi -= xi_d;
      zi += table;
    }
  }
  x = xi;
  y = yi;
  z = zi;
}

// Range Reduced to: 0.5 <= x < 1, -.69 < z < 0
template <int AW,int AI,bool AS,ac_q_mode AQ,ac_o_mode AV,
          int ZW,int ZI,bool ZS,ac_q_mode ZQ,ac_o_mode ZV>
void mgc_ac_ln_rr(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  const int EW = (AW-AI) > (ZW-ZI) ? (AW-AI) : (ZW-ZI);
  ac_fixed<EW+2,2,true> xc = x;
  xc += 1;
  ac_fixed<EW+2,2,true> yc = x;
  yc -= 1;
  ac_fixed<EW+3,2,true> zc = 0; // Post-multiply by 2. Compute an extra bit.
  mgc_ac_hcordic<EW+2,2,true,AC_TRN,AC_WRAP,
                 EW+3,2,true,AC_TRN,AC_WRAP,
                 HCordicConstants::ROT_Y,
                 HCordicConstants::SCALE_1>(xc,yc,zc);
  z = 2*zc;
}

// Range reduced to 0.5 <= x < 1, -1 <= z < 1
template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_log2_rr(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  const int EW = (AW-AI) > (ZW-ZI) ? (AW-AI) : (ZW-ZI);
  ac_fixed<EW+2,2,true> xc = x;
  xc += 1;
  ac_fixed<EW+2,2,true> yc = x;
  yc -= 1;
  ac_fixed<EW+3,2,true> zc = 0; // Post-multiply by 2. Compute an extra bit.
  mgc_ac_hcordic<EW+2,2,true,AC_TRN,AC_WRAP,
                 EW+3,2,true,AC_TRN,AC_WRAP,
                 HCordicConstants::ROT_Y,
                 HCordicConstants::SCALE_LN2>(xc,yc,zc);
  z = 2*zc;
}

// Range Reduced to: |x| < ln(2)  -0.69 < x < 0.69, -2 < z < 2
template <int AW,int AI,bool AS,ac_q_mode AQ,ac_o_mode AV,
          int ZW,int ZI,bool ZS,ac_q_mode ZQ,ac_o_mode ZV>
void mgc_ac_exp_rr(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  const int EW = ZW-ZI; // Evaluation width.
  ac_fixed<EW+3,3,true> xc = 1.0;
  ac_fixed<EW+3,3,true> yc = 1.0;
  ac_fixed<EW+3,3,true> zc = x;
  mgc_ac_hcordic<EW+3,3,true,AC_TRN,AC_WRAP,
                 EW+3,3,true,AC_TRN,AC_WRAP,
                 HCordicConstants::ROT_Z,
                 HCordicConstants::SCALE_1>(xc,yc,zc);
  ac_fixed<EW+3,3,true> k = MgcAcHtrig::inv_K<EW>();
  yc *= k;
  z = yc;
}

// Range Reduced to: 0 <= |x| < 1  0 <= x < 1, 1 <= z < 2
template <int AW,int AI,bool AS,ac_q_mode AQ,ac_o_mode AV,
          int ZW,int ZI,bool ZS,ac_q_mode ZQ,ac_o_mode ZV>
void mgc_ac_exp2_rr(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  const int EW = AW > ZW ? AW : ZW;
  ac_fixed<EW+3,3,true> xc = 1.0;
  ac_fixed<EW+3,3,true> yc = 1.0;
  ac_fixed<EW+3,3,true> ln2 = MgcAcHtrig::ln2<EW>();
  ac_fixed<EW+3,3,true> zc = x*ln2;
  mgc_ac_hcordic<EW+3,3,true,AC_TRN,AC_WRAP,
                 EW+3,3,true,AC_TRN,AC_WRAP,
                 HCordicConstants::ROT_Z,
                 HCordicConstants::SCALE_1>(xc,yc,zc);
  ac_fixed<EW+3,3,true> k = MgcAcHtrig::inv_K<EW>();
  yc *= k;
  z = yc;
}

struct MgcAcLogRR {
  enum base {
    BASE_E,
    BASE_2,
  };
};

template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV, enum MgcAcLogRR::base BASE >
void mgc_ac_log_(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  // BASE_E: RR: ln( m 2^q ) = ln(m) + q*ln(2)
  // BASE_2: RR: ln( m 2^q ) = log2(m) + q*log2(2) = log2(m) + q
  const int LODW = MgcAcHtrig::slog2<AW>::ceil+1;
  ac_int<LODW,false> leading_one = 0;
  bool no_bits_set;
  ac_int<AW,false> x_int;
  x_int.set_slc(0, x.template slc<AW>(0));
  MgcAcHtrig::LOD<AW,LODW>::get(x_int, leading_one, no_bits_set);
  ac_fixed<AW,0,false,AQ,AV> x_norm;
  int dist;
  bool right_shift;
  leading_one += 1; // From index to bits.
  if (leading_one > AW - AI) {
    dist = leading_one - (AW - AI);
    ac_fixed<2*AW,AW,false> x_shift = x;
    x_norm = x_shift >> dist;
    right_shift = true; 
  } else {
    dist = (AW - AI) - leading_one;
    ac_fixed<2*AW,AW,false> x_shift = x;    
    x_norm = x_shift << dist;
    right_shift = false;
  }
  ac_fixed<ZW+2,2,true> zc;
  if (BASE == MgcAcLogRR::BASE_E) {
    mgc_ac_ln_rr(x_norm, zc);
  }
  if (BASE == MgcAcLogRR::BASE_2) {
    mgc_ac_log2_rr(x_norm, zc);
  }
  // Max shift-distance is S = max(AW-AI,AI).
  //   BASE_E: max-offset = S*ln(2)
  //   BASE_2: max-offset = S
  const int OFW = MgcAcHtrig::slog2<(AW-AI>(AI-AS)?AW-AI:(AI-AS))>::ceil;
  ac_fixed<ZW+1+OFW+1,OFW+1,true> offset;
  if (BASE == MgcAcLogRR::BASE_E) {
    offset = MgcAcHtrig::ln2<ZW+1>();
    if (right_shift) {
      offset *= dist;
    } else {
      offset *= -dist;
    }
  }
  if (BASE == MgcAcLogRR::BASE_2) {
    if (right_shift) 
      offset = dist;
    else
      offset = -dist;
  }
  z = offset;
  z += zc;
}

template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_log(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  mgc_ac_log_<AW,AI,AS,AQ,AV,ZW,ZI,ZS,ZQ,ZV,MgcAcLogRR::BASE_E>(x, z);
}

template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_log2(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  mgc_ac_log_<AW,AI,AS,AQ,AV,ZW,ZI,ZS,ZQ,ZV,MgcAcLogRR::BASE_2>(x, z);
}

// The result is expected to have a range which accomodates all
// resulting values exp(x) for inputs x.
template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_exp(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  // Range reduction: prescale x by 1/ln(2)
  // Let R = trunc(1/ln(2), 2^-(AW - AI))
  // Q = trunc(R*x)
  // Then x = Q*ln(2) + M, where |M| < ln(2)
  //
  // exp(Q*ln(2) + M) = cosh(Q*ln(2) + M) + sinh(Q*ln(2) + M)
  //                  = [exp(Q*ln(2) + M) + exp(-Q*ln(2) - M)]/2 +
  //                    [exp(Q*ln(2) + M) - exp(-Q*ln(2) - M)]/2
  //                  = [exp(M)exp(Q*ln(2) + exp(-M)*exp(-Q*ln(2))]/2 +
  //                    [exp(M)exp(Q*ln(2) - exp(-M)*exp(-Q*ln(2))]/2
  // Given exp(Q*ln(2)) = (exp(ln(2)))^Q = 2^Q
  //                  = [2^Q*exp(M) + 2^-Q*exp(-M)]/2 + [2^Q*exp(M) - 2^-Q*exp(-M)]/2
  // Cancel terms 2^-Q*exp(M)/2 and -2^-Q*exp(-M)/2.
  // Add 2^Q*exp(-M)/2 - 2^Q*exp(-M)/2 = 0
  //                  = [2^Q*exp(M) + 2^Q*exp(-M)]/2 + [2^Q*exp(M) - 2^Q*exp(-M)]/2
  //                  = 2^Q[cosh(M) + sinh(M)]
  //
  // and exp(x) = 2^Q exp(M), |M| < 0.69
  
  // Output should be at least wide enough to represent exp(2^(AI-AS)), i.e.,
  //  log2(exp(2^(AI-AS)))
  //  = 2^(AI-AS)*log2(exp(1))
  const int QWE = (int)(1.443*(1 << (AI-AS)))+1+1;
  const int QW = MgcAcHtrig::slog2<QWE>::ceil+AS;
  ac_int<QW+1,true> q_int;
  ac_fixed<ZW-ZI+QWE+QW+1,QW+1,true> m;
  ac_fixed<ZW-ZI,1,false> inv_ln2 = MgcAcHtrig::inv_ln2<ZW-ZI>();
  ac_fixed<QW+1,QW+1,true> q = x*inv_ln2;
  q_int = 0;
  q_int.set_slc(0, q.template slc<QW+1>(0));
  const int MW = ZW-ZI > AW-AI ? ZW-ZI : AW-AI;
  m = MgcAcHtrig::ln2<MW+QWE>();
  m *= -q;
  m += x;
  // May need up to QWE extra bits forpost-scaling by 2^Q.
  ac_fixed<MW+QWE+3,3,true> zc;
  mgc_ac_exp_rr(m, zc);
  // May shift left or right by up to QWE positions.
  ac_fixed<MW+QWE+3+QWE,3+QWE,true> zs = zc;
  if (q_int < 0) {
    q_int = -q_int;
    zs = zs >> q_int;
  } else {
    zs = zs << q_int;
  }
  z = zs;
}

// Example range-reduction algorithm which invokes the mgc_ac_exp_rr
// routine.
template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_exp2(const ac_fixed<AW,AI,AS,AQ,AV> &x, ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {
  // If exp(t) = 2^x, then t = x ln(2), i.e., we can compute 2^x
  // using exp(x ln(2)).
  //
  // Recalling our original range reduction form, x = Qln(2) + M.
  // We see that t = x ln(2) = (xI + xF) ln(2), where xI is the
  // integer part of x and xF is the fractional part.
  //
  // Therefore t = xI ln(2) + xF ln(2), i.e., our Q value is
  // directly obtained from xI. Similarly our M value is obtained
  // from multiplying xF by ln(2), i.e., M = xF*ln(2).
  const int QW = AI-AS;
  const int QWE = (1<<QW)+1;
  ac_int<AI+1,true> q_int;
  ac_fixed<AW-AI+QWE+1,1,true> m;
  ac_fixed<AI+1,AI+1,true> q = x;
  q_int.set_slc(0, q.template slc<AI+1>(0));
  m = 0;
  m.set_slc(QWE, x.template slc<AW-AI>(0));
  const int MW = ZW-ZI > AW-AI ? ZW-ZI : AW-AI;
  ac_fixed<MW+QWE+1,1,true> mw = m; // Widen m if required.
  mw *= MgcAcHtrig::ln2<MW+QWE>();
  ac_fixed<ZW-ZI+QWE+3,3,true> zc;
  mgc_ac_exp_rr(mw, zc);
  ac_fixed<ZW-ZI+QWE+3+QWE,3+QWE,true> zs = zc;
  if (q_int < 0) {
    q_int = -q_int;
    zs = zs >> q_int;
  } else {
    zs = zs << q_int;
  }
  z = zs;
}

// This implementation tries to be as general as possible without
// taking into account the actual range of arguments 'a', and 'b'. A
// specific implementation should take full advantage of argument
// ranges to size the intermediate results of the computation.
//
// The following function implements pow(a,b)=a^b using the following
// decomposition.
//
// a^b = exp2(log2(a^b)) = exp2(b*log2(a))
// t = log(a)
// q = qI + qF = b*t
// z = exp2(qI + qF) = exp2(qI)*exp2(qF)
// 
// This bounds the argument qF of exp2(qF) between 0 and 1.
// Factor exp2(qI) is accounted for with a final shift.
template <int AW, int AI, bool AS, ac_q_mode AQ, ac_o_mode AV, 
          int BW, int BI, bool BS, ac_q_mode BQ, ac_o_mode BV,
          int ZW, int ZI, bool ZS, ac_q_mode ZQ, ac_o_mode ZV>
void mgc_ac_pow(const ac_fixed<AW,AI,AS,AQ,AV> &a,
                const ac_fixed<BW,BI,BS,BQ,BV> &b,
                ac_fixed<ZW,ZI,ZS,ZQ,ZV> &z) {  
  // log2(min(a)) = log(2^-(AW-AI))
  const int TI_MIN = MgcAcHtrig::slog2<AW-AI>::ceil;
  // log2(max(a)) = log(2^AI)  
  const int TI_MAX = MgcAcHtrig::slog2<AI>::ceil;
  const int TI = (TI_MIN < TI_MAX ? TI_MAX : TI_MIN)+1;
  const int TW = ZW-ZI+TI;
  ac_fixed<TW,TI,true> t;
  mgc_ac_log2(a, t);
  const int QI = TI+(BI-BS);
  // A left shift results in loss of precision if the bits to be shifted-in
  // aren't computed. This is possible when 'a' has a large number of fractional
  // positions and 'b' can be negative.
  // 
  // * Case b < 0: If TI is obtained from TI_MIN, i.e.,
  //   log2(ulp(a)) < 0, then log2(a)*b can produce a 2^(QI-1) shift distance.
  //
  // * Case b >= 0: If TI is obtained from TI_MAX, i.e.,
  //   log2(max(a)), then b*log2(a) <=  b*max(log2(a)) <= b*2^TI_MAX can produce
  //   a 2^(BI+TI_MAX) shift distance.
  //
  // When argument 'b' is an unsigned value, then the maximum shift distance is
  // dependent on the maximum value of 'a', and not on its number of fractional
  // bits.
  const int SHIFT_W = BS?(1<<(QI-1)):(1<<(BI+TI_MAX));
  const int QW = ZW-ZI+SHIFT_W+QI;
  ac_fixed<QW,QI,true> q = b*t;
  ac_fixed<QI,QI,true> q_i = 0;
  q_i.set_slc(0, q.template slc<QI>(QW-QI));
  ac_fixed<QW-QI,0,false> q_f = 0;
  q_f.set_slc(0, q.template slc<QW-QI>(0));
  ac_int<QI,true> q_int = 0;
  q_int.set_slc(0, q.template slc<QI>(QW-QI));
  // 0 <= q_f < 1
  const int ZCI = 2;
  const int ZCW = QW-QI+2;
  ac_fixed<ZCW,ZCI,false> zc;
  mgc_ac_exp2(q_f, zc);
  ac_fixed<ZCW+SHIFT_W,SHIFT_W+2,false> zc_shift = zc;
  if (q_int < 0) {
    q_int = -q_int;
    zc_shift >>= q_int;
  } else {
    zc_shift <<= q_int;
  }
  z = zc_shift;
}

#endif // __MGC_AC_HCORDIC_H
