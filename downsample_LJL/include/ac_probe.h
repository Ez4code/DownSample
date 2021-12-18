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

#ifndef __AC_PROBE_H
#define __AC_PROBE_H

#include <string>
#include <ac_int.h>

// Defining classes and functions to avoid compilation errors in scverify
#ifdef __AC_NAMESPACE
namespace __AC_NAMESPACE {
#endif
using std::string;
namespace ac {
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(string prbnm, T val);
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(string prbnm, T* val);
  #endif

  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(const char *prbnm, T val);
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(const char *prbnm, T* val);
  #endif

  template <typename T>
  void probe_map(const char*prbnm, T inp);

  template <typename T>
  T passthrough(T inp);

  template <class T, int size>
  struct probed_array;
}
#ifdef __AC_NAMESPACE
}
#endif


#if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
#include "ccs_probes.h"
#endif

#ifdef __AC_NAMESPACE
namespace __AC_NAMESPACE {
#endif
using std::string;
namespace ac {
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(string prbnm, T val) {
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
    enum {thislv = ccs_scverify_probe_traits<T>::lvwidth};
    sc_dt::sc_lv<thislv> tmp;
	 type_to_vector(val,thislv,tmp);
    ccs_probe_fifo_put<thislv>(prbnm, tmp);
  #endif
  }
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(string prbnm, T* val) {
    enum {thislv = ccs_scverify_probe_traits<T>::lvwidth};
    sc_dt::sc_lv<thislv> tmp;
	 type_to_vector(*val,thislv,tmp);
    ccs_probe_fifo_put<thislv>(prbnm, tmp);
  }
  #endif

  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(const char *prbnm, T val) {
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
    enum {thislv = ccs_scverify_probe_traits<T>::lvwidth};
    sc_dt::sc_lv<thislv> tmp;
	 type_to_vector(val,thislv,tmp);
    ccs_probe_fifo_put<thislv>(prbnm, tmp);
  #endif
  }
  #if defined(CCS_SCVERIFY) && defined(CCS_DUT_RTL) && !defined(CCS_DUT_SYSC) && !defined(CCS_SYSC)
  #pragma calypto_flag AC_PROBE
  template <class T>
  inline void probe(const char *prbnm, T* val) {
    enum {thislv = ccs_scverify_probe_traits<T>::lvwidth};
    sc_dt::sc_lv<thislv> tmp;
	 type_to_vector(*val,thislv,tmp);
    ccs_probe_fifo_put<thislv>(prbnm, tmp);
  }
  #endif

  #pragma map_to_operator [CCORE]
  #pragma ccore_type combinational
  #pragma calypto_flag AC_PROBE_MAP
  #pragma preserve
  template <typename T>
  void probe_map(const char*prbnm, T inp) { }

  #pragma map_to_operator [CCORE]
  #pragma ccore_type combinational
  template <typename T>
  T passthrough(T inp) { return inp; }

  // This class allows all reads (but not writes) performed
  // via the [] operator (not via pointer dereferencing yet)
  // to be automatically probed.
  #define PROBED_ARRAY 1
  template <class T, int size>
  struct probed_array {
       T arr[size];

       inline T &operator[](int i) {
            T &t = arr[i];
            ac::probe_map("mem", t);
            return t;
       }
  };
}

#ifdef __AC_NAMESPACE
}
#endif

#endif
