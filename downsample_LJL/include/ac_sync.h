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

#ifndef __AC_SYNC_H
#define __AC_SYNC_H

#include "ac_channel.h"
////////////////////////////////////////////////////////////////////////////////
// Class: ac_sync
////////////////////////////////////////////////////////////////////////////////
#if defined(__SYNTHESIS__)
#pragma builtin
#define INHERIT_MODE private
#else
#define INHERIT_MODE public
#endif
class ac_sync : INHERIT_MODE ac_channel<bool>
{
  public:
  typedef ac_channel<bool> Base;

  // constructor
  ac_sync(): Base() { }

  inline bool sync_in() {
    bool rval = true;
    #ifndef __SYNTHESIS__
    rval = Base::read(); // During synthesis -- builtin treatment
    #endif
    return rval;
  }

  inline void sync_out() {
    #ifndef __SYNTHESIS__
    Base::write(true);
    #endif
  }

  inline bool nb_sync_in() {
    bool rval = true;
    #ifndef __SYNTHESIS__
    bool dummy_obj;
    rval = Base::nb_read(dummy_obj); // During synthesis -- builtin treatment
    #endif
    return rval;
  }

#if 0
  inline bool nb_sync_out();
#else
  // C simulation always returns true -- So, 'else' branch based on the
  // successs of 'nb_write' is not exercisable in C simulation, as the
  // underlying buffer is unbounded in C model.
  // But, in RTL, when mapped to two-way handshake component, both 'if' and
  // 'else' branch are exercisable in RTL
  inline bool nb_sync_out() {
    bool rval = true;
    #ifndef __SYNTHESIS__
    rval = Base::nb_write(rval);
    #endif
    return rval;
  }
#endif

  inline bool available( unsigned int cnt) {
    #ifndef __SYNTHESIS__
    return Base::available(cnt); 
    #endif
  }

  private:
  // Prevent the compiler from autogenerating these.
  // This enforces that ac_sync are always passed by reference.
  ac_sync(const ac_sync &); 
  ac_sync& operator=(const ac_sync &);
};

#endif
