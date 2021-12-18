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
// File: ac_shift_reg.h
//
// Description: Provides a generic register-based shift register.
//
// Class Templates
//    T        - typename specifies internal data type
//    NUM_TAPS - integer specifies the number of register array elements
//
// Usage:
//
// Notes:
//    Relies on the ac_array class.
//
// Revision History:
//
//***************************************************************************

#ifndef _INCLUDED_AC_SHIFT_REG_H_
#define _INCLUDED_AC_SHIFT_REG_H_

#include <assert.h>
#include <stdio.h>
#include <ac_array.h>

//=========================================================================
// Class: ac_shift_reg
//
//-------------------------------------------------------------------------

template<typename T, int NUM_TAPS>
class ac_shift_reg
{
public:

  // Constructor: (new empty)
  ac_shift_reg() {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      regs[i] =0;
    }
  }

  // Constructor: (prefill with value)
  ac_shift_reg(int din) {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      regs[i] =din;
    }
  }

  // Copy Constructor:

  // Destructor:

public: // Class Member Functions

  // Shift operation
  void operator << (T din) {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      if (i==NUM_TAPS-1) {
        regs[i] = din;
      } else {
        regs[i] = regs[i+1];
      }
    }
  }

  // Element Access (Constant)
  const T operator [](int idx) const {
#ifndef __SYNTHESIS__
    assert(idx<NUM_TAPS);
#endif
    return regs[idx];
  }

  // Element Access (Modifiable)
  T operator [](int idx) {
#ifndef __SYNTHESIS__
    assert(idx<NUM_TAPS);
#endif
    return regs[idx];
  }

  // Copy register contents to dout
  void get(ac_array<T,NUM_TAPS> &dout) {
    dout = regs;
  }

  // Modifiable access to registers
  ac_array<T,NUM_TAPS> get() {
    return regs;
  }

  // Load registers from C-style array
  void load(T din[NUM_TAPS]) {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      regs[i] = din[i];
    }
  }

  // Clear registers
  void clear() {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      regs[i] = 0;
    }
  }

  // Set all registers to value
  void set(int din) {
#pragma unroll yes
    for (int i=0; i<NUM_TAPS; i++) {
      regs[i] = din;
    }
  }

  // Set specific register to value
  void set(T din, int i) {
    regs[i] = din;
  }

  // Print register contents
  void print() {
#ifndef __SYNTHESIS__
    for (int i=0; i<NUM_TAPS; i++) {
      printf("%3d ",(int)regs[i]);
    }
    printf("\n");
#endif
  }

private: // data
  ac_array<T,NUM_TAPS> regs;
};

//----------------------------------------------------------------------
// Function: ostream operator<<
// Description: print the contents of an ac_shift_reg to output stream
#include <iostream>
template<typename T, int NUM_TAPS>
std::ostream &operator<<(std::ostream &os, const ac_shift_reg<T,NUM_TAPS> &m)
{
  bool first = true;
  os << "[";
  for (unsigned i=0; i<NUM_TAPS; i++) {
    if (first) {
      os << " ";
      first = false;
    } else {
      os << ", ";
    }
    os << m[i];
  }
  os << "]";
  return os;
}

#endif

