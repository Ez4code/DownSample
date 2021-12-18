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
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Source:         mc_base.h
//  Description:    Header file for C++ interface
//              
///////////////////////////////////////////////////////////////////////////

#ifndef __MC_BASE_H
#define __MC_BASE_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

//////////////////////////////////////////////////////////////////////////////
//  QuestaSim's C std_logic enumerated types
//////////////////////////////////////////////////////////////////////////////
enum mti_std_logic {  // TYPE std_ulogic IS( 
  std_logic_U,        //   U ,-- 0
  std_logic_X,        //   X ,-- 1
  std_logic_0,        //   0 ,-- 2
  std_logic_1,        //   1 ,-- 3
  std_logic_Z,        //   Z ,-- 4
  std_logic_W,        //   W ,-- 5
  std_logic_L,        //   L ,-- 6
  std_logic_H,        //   H ,-- 7
  std_logic_DC        //   -  -- 8
};                    // );


#if defined(_MSC_VER)
#if !defined(__EDG__) && _MSC_VER < 1400 && !defined(for)
# define for if(0);else for
#endif
typedef unsigned __int64 Ulong;
typedef signed   __int64 Slong;
#else
typedef unsigned long long Ulong;
typedef signed   long long Slong;
#endif


///////////////////////////////////////////////////////////////////////////////
//  Function:            signal2long
//  Arguments:           uchar *buffer -- the buffer containing the value
//                       int length -- the length of the buffer
//  Return Value:        long -- the signal value converted
//  Description:         This function converts a signal value(ie. '01001010')
//                       to a proper long value.
///////////////////////////////////////////////////////////////////////////////
inline Ulong signal2long( unsigned char *buffer, int length ) {
  Ulong value, mask;

  value = 0;
  mask = 1;
  for ( length--; length >= 0; length-- ) {
    if ( buffer[length] == 3 ) {
      value |= mask;
    }
    mask <<= 1;
  }
  return value;
}


///////////////////////////////////////////////////////////////////////////////
//  Function:            long2signal
//  Arguments:           uchar *buffer -- the buffer to save too.
//                       Ulong value -- the value to convert
//                       int length -- the length of the buffer
//  Description:         This function converts a ulong value to a buffer
//                       of type signal(ie. '01010111' where there is one
//                       byte for each bit).
///////////////////////////////////////////////////////////////////////////////
inline int long2signal( unsigned char *buffer, Ulong value, int length ) {
  Ulong mask;

  mask = 1;
  for ( length--; length >= 0; length-- ) {
    if ( value & mask ) {
      buffer[length] = std_logic_1;
    } else {
      buffer[length] = std_logic_0;
    }
    mask <<= 1;
  }
  return true;
}



#endif // __MC_BASE_H
