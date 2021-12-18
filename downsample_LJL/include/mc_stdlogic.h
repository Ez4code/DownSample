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
//  Source:         mc_stdlogic.h
//  Description:    std_logic and std_logic_vector structures for C++
//                  representation.
///////////////////////////////////////////////////////////////////////////

#ifndef __MC_STDLOGIC_H
#define __MC_STDLOGIC_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

//  Include main header
#include "mc_base.h"


//      -------------------------------------------------------------------    
//      -- tables for logical operations
//      -------------------------------------------------------------------    

//      -- truth table for "and" function
//      CONSTANT and_table : stdlogic_table := (
//      --      ----------------------------------------------------
//      --      |  U    X    0    1    Z    W    L    H    -         |   |  
//      --      ----------------------------------------------------
//              ( 'U', 'U', '0', 'U', 'U', 'U', '0', 'U', 'U' ),  -- | U |
//              ( 'U', 'X', '0', 'X', 'X', 'X', '0', 'X', 'X' ),  -- | X |
//              ( '0', '0', '0', '0', '0', '0', '0', '0', '0' ),  -- | 0 |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | 1 |
//              ( 'U', 'X', '0', 'X', 'X', 'X', '0', 'X', 'X' ),  -- | Z |
//              ( 'U', 'X', '0', 'X', 'X', 'X', '0', 'X', 'X' ),  -- | W |
//              ( '0', '0', '0', '0', '0', '0', '0', '0', '0' ),  -- | L |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | H |
//              ( 'U', 'X', '0', 'X', 'X', 'X', '0', 'X', 'X' )   -- | - |
//      );

const mti_std_logic and_table[9][9] = {
//----------------------------------------------------------------------------------------------------------------------------
//|           U            X            0            1            Z            W            L            H            -        |   |  
//----------------------------------------------------------------------------------------------------------------------------
//(           U,           U,           0,           U,           U,           U,           0,           U,           U ),  -- | U |
  { std_logic_U, std_logic_U, std_logic_0, std_logic_U, std_logic_U, std_logic_U, std_logic_0, std_logic_U, std_logic_U },
//(           U,           X,           0,           X,           X,           X,           0,           X,           X ),  -- | X |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_X, std_logic_X, std_logic_X, std_logic_0, std_logic_X, std_logic_X },
//(           0,           0,           0,           0,           0,           0,           0,           0,           0 ),  -- | 0 |
  { std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0 },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | 1 |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           U,           X,           0,           X,           X,           X,           0,           X,           X ),  -- | Z |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_X, std_logic_X, std_logic_X, std_logic_0, std_logic_X, std_logic_X },
//(           U,           X,           0,           X,           X,           X,           0,           X,           X ),  -- | W |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_X, std_logic_X, std_logic_X, std_logic_0, std_logic_X, std_logic_X },
//(           0,           0,           0,           0,           0,           0,           0,           0,           0 ),  -- | L |
  { std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0, std_logic_0 },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | H |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           U,           X,           0,           X,           X,           X,           0,           X,           X )   -- | - |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_X, std_logic_X, std_logic_X, std_logic_0, std_logic_X, std_logic_X }
};


//      -- truth table for "or" function
//      CONSTANT or_table : stdlogic_table := (
//      --      ----------------------------------------------------
//      --      |  U    X    0    1    Z    W    L    H    -         |   |  
//      --      ----------------------------------------------------
//              ( 'U', 'U', 'U', '1', 'U', 'U', 'U', '1', 'U' ),  -- | U |
//              ( 'U', 'X', 'X', '1', 'X', 'X', 'X', '1', 'X' ),  -- | X |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | 0 |
//              ( '1', '1', '1', '1', '1', '1', '1', '1', '1' ),  -- | 1 |
//              ( 'U', 'X', 'X', '1', 'X', 'X', 'X', '1', 'X' ),  -- | Z |
//              ( 'U', 'X', 'X', '1', 'X', 'X', 'X', '1', 'X' ),  -- | W |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | L |
//              ( '1', '1', '1', '1', '1', '1', '1', '1', '1' ),  -- | H |
//              ( 'U', 'X', 'X', '1', 'X', 'X', 'X', '1', 'X' )   -- | - |
//      );

const mti_std_logic or_table[9][9] = {
//----------------------------------------------------------------------------------------------------------------------------
//|           U            X            0            1            Z            W            L            H            -        |   |  
//----------------------------------------------------------------------------------------------------------------------------
//(           U,           U,           U,           1,           U,           U,           U,           1,           U ),  -- | U |
  { std_logic_U, std_logic_U, std_logic_U, std_logic_1, std_logic_U, std_logic_U, std_logic_U, std_logic_1, std_logic_U },
//(           U,           X,           X,           1,           X,           X,           X,           1,           X ),  -- | X |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_1, std_logic_X, std_logic_X, std_logic_X, std_logic_1, std_logic_X },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | 0 |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           1,           1,           1,           1,           1,           1,           1,           1,           1 ),  -- | 1 |
  { std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1 },
//(           U,           X,           X,           1,           X,           X,           X,           1,           X ),  -- | Z |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_1, std_logic_X, std_logic_X, std_logic_X, std_logic_1, std_logic_X },
//(           U,           X,           X,           1,           X,           X,           X,           1,           X ),  -- | W |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_1, std_logic_X, std_logic_X, std_logic_X, std_logic_1, std_logic_X },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | L |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           1,           1,           1,           1,           1,           1,           1,           1,           1 ),  -- | H |
  { std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1, std_logic_1 },
//(           U,           X,           X,           1,           X,           X,           X,           1,           X )   -- | - |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_1, std_logic_X, std_logic_X, std_logic_X, std_logic_1, std_logic_X }
};


//      -- truth table for "xor" function
//      CONSTANT xor_table : stdlogic_table := (
//      --      ----------------------------------------------------
//      --      |  U    X    0    1    Z    W    L    H    -         |   |  
//      --      ----------------------------------------------------
//              ( 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'U' ),  -- | U |
//              ( 'U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' ),  -- | X |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | 0 |
//              ( 'U', 'X', '1', '0', 'X', 'X', '1', '0', 'X' ),  -- | 1 |
//              ( 'U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' ),  -- | Z |
//              ( 'U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' ),  -- | W |
//              ( 'U', 'X', '0', '1', 'X', 'X', '0', '1', 'X' ),  -- | L |
//              ( 'U', 'X', '1', '0', 'X', 'X', '1', '0', 'X' ),  -- | H |
//              ( 'U', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' )   -- | - |
//      );

const mti_std_logic xor_table[9][9] = {
//----------------------------------------------------------------------------------------------------------------------------
//|           U            X            0            1            Z            W            L            H            -        |   |  
//----------------------------------------------------------------------------------------------------------------------------
//(           U,           U,           U,           U,           U,           U,           U,           U,           U ),  -- | U |
  { std_logic_U, std_logic_U, std_logic_U, std_logic_U, std_logic_U, std_logic_U, std_logic_U, std_logic_U, std_logic_U },
//(           U,           X,           X,           X,           X,           X,           X,           X,           X ),  -- | X |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | 0 |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           U,           X,           1,           0,           X,           X,           1,           0,           X ),  -- | 1 |
  { std_logic_U, std_logic_X, std_logic_1, std_logic_0, std_logic_X, std_logic_X, std_logic_1, std_logic_0, std_logic_X },
//(           U,           X,           X,           X,           X,           X,           X,           X,           X ),  -- | Z |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X },
//(           U,           X,           X,           X,           X,           X,           X,           X,           X ),  -- | W |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X },
//(           U,           X,           0,           1,           X,           X,           0,           1,           X ),  -- | L |
  { std_logic_U, std_logic_X, std_logic_0, std_logic_1, std_logic_X, std_logic_X, std_logic_0, std_logic_1, std_logic_X },
//(           U,           X,           1,           0,           X,           X,           1,           0,           X ),  -- | H |
  { std_logic_U, std_logic_X, std_logic_1, std_logic_0, std_logic_X, std_logic_X, std_logic_1, std_logic_0, std_logic_X },
//(           U,           X,           X,           X,           X,           X,           X,           X,           X )   -- | - |
  { std_logic_U, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X, std_logic_X }
};


//      -- truth table for "not" function
//      CONSTANT not_table: stdlogic_1d := 
//      --  -------------------------------------------------
//      --  |   U    X    0    1    Z    W    L    H    -   |
//      --  -------------------------------------------------
//           ( 'U', 'X', '1', '0', 'X', 'X', '1', '0', 'X' ); 

const mti_std_logic not_table[9] = {
//----------------------------------------------------------------------------------------------------------------------------
//|         U            X            0            1            Z            W            L            H            -        |
//----------------------------------------------------------------------------------------------------------------------------
//(         U,           X,           1,           0,           X,           X,           1,           0,           X  ); 
  std_logic_U, std_logic_X, std_logic_1, std_logic_0, std_logic_X, std_logic_X, std_logic_1, std_logic_0, std_logic_X
};



inline bool to_bit( mti_std_logic b, bool xmap=false ) {
  if ( b == std_logic_1 || b == std_logic_H )
    return true;
  if ( b == std_logic_0 || b == std_logic_L )
    return false;
  return xmap;
}


//////////////////////////////////////////////////////////////////////////////
//  std_logic
//////////////////////////////////////////////////////////////////////////////
struct std_logic {
#pragma builtin
  mti_std_logic val;

  std_logic () {}
  std_logic ( mti_std_logic b ) {
    val = b;
  }
  std_logic ( const std_logic &b ) {
    val = b.val;
  }
  std_logic ( const char b ) {
    *this = b;
  }
  std_logic &operator = ( const std_logic &b ) {
    val = b.val;
    return *this;
  }
  std_logic &operator = ( mti_std_logic b ) {
    val = b;
    return *this;
  }
  std_logic &operator = ( const char b ) {
    // U    X    0    1    Z    W    L    H    -   
    switch ( b ) {
    case 'u' :
    case 'U' : val = std_logic_U; break;
    case 'x' :
    case 'X' : val = std_logic_X; break;
    case '0' : val = std_logic_0; break;
    case '1' : val = std_logic_1; break;
    case 'z' :
    case 'Z' : val = std_logic_Z; break;
    case 'w' :
    case 'W' : val = std_logic_W; break;
    case 'l' :
    case 'L' : val = std_logic_L; break;
    case 'h' :
    case 'H' : val = std_logic_H; break;
    case '-' : val = std_logic_DC; break;
    }
    return *this;
  }
  operator mti_std_logic () const {
    return val;
  }

  //  Logical Operations
  mti_std_logic operator & ( const std_logic &b ) const {
//      cout << "op& ( " << this->to_bit() << " & " << b.to_bit() << " = "
//  	 << ::to_bit( and_table[val][b] ) << " )" << endl;
    return and_table[val][b];
  }
  mti_std_logic operator | ( const std_logic &b ) const {
//      cout << "op| ( " << this->to_bit() << " | " << b.to_bit() << " = "
//  	 << ::to_bit( or_table[val][b.val] ) << " )" << endl;
    return or_table[val][b.val];
  }
  mti_std_logic operator ^ ( const std_logic &b ) const {
//      cout << "op^ ( " << this->to_bit() << " ^ " << b.to_bit() << " = "
//  	 << ::to_bit( xor_table[val][b.val] ) << " )" << endl;
    return xor_table[val][b.val];
  }
  mti_std_logic operator ! () const {
//      cout << "op! ( ! " << this->to_bit() << " = "
//  	 << ::to_bit( not_table[val] ) << " )" << endl;
    return not_table[val];
  }

  bool to_bit( bool xmap=false ) const {
    if ( val == std_logic_1 || val == std_logic_H ) return true;
    if ( val == std_logic_0 || val == std_logic_L ) return false;
    return xmap;
  }
};



//////////////////////////////////////////////////////////////////////////////
//  std_logic_vector
//////////////////////////////////////////////////////////////////////////////
template <int W>
struct std_logic_vector {
#pragma builtin
  std_logic val[W];

  std_logic_vector () {}
  std_logic_vector ( std_logic_vector &b ) {
    for ( int i=0; i < W; ++i )
      val[i] = b[i];
  }
  std_logic_vector ( Ulong b ) {
    for ( int i=0; i < W; ++i )
      val[i] = (mti_std_logic)( (b & (1 << i)) != 0 ? std_logic_1 : std_logic_0 );
  }
  std_logic_vector ( const char *b ) {
    *this = b;
  }
  std_logic_vector<W> &operator = ( int b ) {
    for ( int i=0; i < W; ++i )
      val[i] = (mti_std_logic)( (b & (1 << i)) != 0 ? std_logic_1 : std_logic_0 );
    return *this;
  }
  std_logic_vector<W> &operator = ( std_logic_vector &b ) {
    for ( int i=0; i < W; ++i )
      val[i] = b[i];
    return *this;
  }
  std_logic_vector<W> &operator = ( const char *b ) {
    int len = strlen( b );
    for ( int i=0; i < W; ++i ) {
      if ( i >= len )
	break;
      val[i] = b[i];
    }
    return *this;
  }
  std_logic &operator [] ( unsigned char n ) {
    return val[n];
  }
  operator Ulong () const {
    return this->to_bitvector();
  }
  int width() const {
    return W;
  }

  //  Logical Operations
  std_logic_vector<W> operator & ( std_logic_vector<W> &b ) {
    std_logic_vector<W> rval;
    for ( int i=0; i < W; ++i )
      rval.val[i] = val[i] & b[i];
//      cout << "slv op& ( 0x" << hex << this->to_bitvector() << " & 0x"
//  	 << b.to_bitvector() << " = 0x" << rval.to_bitvector() << " )"
//  	 << dec << endl;
    return rval;
  }
  std_logic_vector<W> operator | ( std_logic_vector<W> &b ) {
    std_logic_vector<W> rval;
    for ( int i=0; i < W; ++i )
      rval.val[i] = val[i] | b[i];
    return rval;
  }
  std_logic_vector<W> operator ^ ( std_logic_vector<W> &b ) {
    std_logic_vector<W> rval;
    for ( int i=0; i < W; ++i )
      rval.val[i] = val[i] ^ b[i];
    return rval;
  }
  std_logic_vector<W> operator ! () {
    std_logic_vector<W> rval;
    for ( int i=0; i < W; ++i )
      rval.val[i] = !val[i];
    return rval;
  }

  std_logic_vector<W> operator &= ( std_logic_vector<W> &b ) {
    for ( int i=0; i < W; ++i )
      val[i] = val[i] & b[i];
    return *this;
  }
  std_logic_vector<W> operator |= ( std_logic_vector<W> &b ) {
    for ( int i=0; i < W; ++i )
      val[i] = val[i] | b[i];
    return *this;
  }
  std_logic_vector<W> operator ^= ( std_logic_vector<W> &b ) {
    for ( int i=0; i < W; ++i )
      val[i] = val[i] ^ b[i];
    return *this;
  }

  Ulong to_bitvector( bool xmap=false ) const {
    Ulong rval = 0;
    for ( int i = 0; i < W; ++i ) {
      if ( std_logic_1 == val[i] || std_logic_H == val[i] )
	rval |= ( true << i );
      else if ( std_logic_0 != val[i] && std_logic_L != val[i] )
	rval |= ( xmap << i );
    }
    return rval;
  }
};


template <int W>
inline Ulong to_bitvector( std_logic_vector<W> b, bool xmap=false ) {
#pragma builtin
  Ulong rval = 0;
  for ( int i = 0; i < b.width(); ++i ) {
    if ( std_logic_1 == b[i] || std_logic_H == b[i] )
      rval |= ( true << i );
    else if ( std_logic_0 != b[i] && std_logic_L != b[i] )
      rval |= ( xmap << i );
  }
  return rval;
}



#endif // __MC_STDLOGIC_H
