////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2018 Mentor Graphics Corp.
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

#ifndef __AC_STAGE_H
#define __AC_STAGE_H

#ifdef __AC_NAMESPACE
namespace __AC_NAMESPACE {
#endif

namespace ac {
  template <typename Rtype>
  inline const Rtype stage_count()
  {
    // dummy value for simulation
    // to be plugged in by SC-Verify after synthesis
    return 1;
  }
}

#ifdef __AC_NAMESPACE
}
#endif

#endif
