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
// File: ac_boundary.h
//
// Description: Provides a class used to perform the boundary processing when 
//      at the left, right, top, or bottom of an image, or any 1-d or 2-d 
//      array of values.
//
// Boundary Processing Modes
//    AC_REPLICATE  - replicate the first and last row (or column) as many
//                    times as needed
//    AC_MIRROR_101 - ?
//                    
//    AC_MIRROR     - mirror (reflect) the image at the boundaries
//                    column[-1] = column[1], column[-2] = column[2]
//    AC_CONSTANT   - fill with zeros
//    AC_NOPAD      - ??
//
// Class Templates
//    T        - typename specifies internal data type
//    WIN_SIZE - integer size of input/output register arrays
//    AC_BOUNDARY - enum specifying the type of boundary processing
//    BOUNDS_CONST - integer value used for constant bounds
//
// Usage:
//
// Notes:
//
// Revision History:
//
//***************************************************************************

#ifndef _INCLUDED_AC_BOUNDARY_H_
#define _INCLUDED_AC_BOUNDARY_H_

#include <ac_array.h>
#include <ac_int.h>
#include <cassert>

enum ac_window_mode {
  AC_REPLICATE  = 1<<0,
  AC_MIRROR_101 = 1<<1,
  AC_MIRROR     = 1<<2,
  AC_CONSTANT   = 1<<3,
  AC_NOPAD      = 1<<4
};

//=========================================================================
// Class: ac_boundary
//
//	apply_bounds class member function has two inputs of type ac_array
//		lower - bit array that indicates left or top boundary
//		upper - bit array that indicates right or bottom boundary
//
//-------------------------------------------------------------------------

template<typename T, int AC_SIZE, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class ac_boundary
{
public:
  // Constructor: (new empty)
  ac_boundary() {
    assert(AC_SIZE <= 11 && AC_SIZE & 1); // Make sure usage falls within specializations
  }

public: // Class Member Functions
  void apply_bounds(ac_int<AC_SIZE,false> lower, ac_int<AC_SIZE,false> upper, ac_array<T,AC_SIZE> din, ac_array<T,AC_SIZE> &dout);
};

//=========================================================================
// Specialized Class: ac_boundary (w/AC_SIZE=3)
//
//-------------------------------------------------------------------------

template<typename T, int AC_BOUNDARY, int AC_BOUNDS_CONST>
class ac_boundary<T,3,AC_BOUNDARY,AC_BOUNDS_CONST>
{
public:
  // Constructors (new empty)
  ac_boundary() { }

public: // Class Member Functions

  void apply_bounds(ac_int<3,false> lower, ac_int<3,false> upper, ac_array<T,3> din, ac_array<T,3> &dout) {
    ac_array<T,3> win = din;

    if (AC_BOUNDARY & AC_REPLICATE) { // replicate boundary
      switch (lower) {
        case 2: // 010
          win[0] = win[1];
          break;
        default:
          break;
      }
      switch (upper) {
        case 2: // 010
          win[2] = win[1];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR_101) {
      switch (lower) {
        case 2: // 010
          win[0] = win[2];
          break;
        default:
          break;
      }
      switch (upper) {
        case 2: // 010
          win[2] = win[0];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR) {
      switch (lower) {
        case 2: // 010
          win[0] = win[1];
          break;
        default:
          break;
      }
      switch (upper) {
        case 2: // 100
          win[2] = win[1];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_CONSTANT) { // replicate boundary
      switch (lower) {
        case 2: // 010
          win[0] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
      switch (upper) {
        case 2: // 100
          win[2] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
    } else { // replicate boundary
      switch (lower) {
        case 2: // 010
          win[0] = win[1];
          break;
        default:
          break;
      }
      switch (upper) {
        case 2: // 010
          win[2] = win[1];
          break;
        default:
          break;
      }
    }
    dout = win;
  }
};

//=========================================================================
// Specialized Class: ac_boundary (w/AC_SIZE=5)
//
//-------------------------------------------------------------------------

template<typename T, int AC_BOUNDARY, int AC_BOUNDS_CONST>
class ac_boundary<T,5,AC_BOUNDARY,AC_BOUNDS_CONST>
{
public:
  // Constructor: (new empty)
  ac_boundary() { }

public: // Class Member Functions

  void apply_bounds(ac_int<5,false> lower, ac_int<5,false> upper, ac_array<T,5> din, ac_array<T,5> &dout) {
    ac_array<T,5> win = din;

    if (AC_BOUNDARY & AC_REPLICATE) {  // replicate boundary
      switch (lower) {
        case 8:
          win[0] = win[1];
          break;
        case 4:
          win[0] = win[2];
          win[1] = win[2];
          break;
        default:
          break;
      }
      switch (upper) {
        case 4:
          win[4] = win[2];
          win[3] = win[2];
          break;
        case 2:
          win[4] = win[3];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR_101) {
      switch (lower) {
        case 8:
          win[0] = win[2];
          break;
        case 4:
          win[0] = win[4];
          win[1] = win[3];
          break;
        default:
          break;
      }
      switch (upper) {
        case 4://
          win[4] = win[0];
          win[3] = win[1];
          break;
        case 2:
          win[4] = win[2];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR) {
      switch (lower) {
        case 8:
          win[0] = win[1];
          break;
        case 4:
          win[0] = win[3];
          win[1] = win[2];
          break;
        default:
          break;
      }
      switch (upper) {
        case 4:
          win[4] = win[1];
          win[3] = win[2];
          break;
        case 2:
          win[4] = win[3];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_CONSTANT) { // replicate boundary
      switch (lower) {
        case 8:
          win[0] = AC_BOUNDS_CONST;
          break;
        case 4:
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
      switch (upper) {
        case 4:
          win[4] = AC_BOUNDS_CONST;
          win[3] = AC_BOUNDS_CONST;
          break;
        case 2:
          win[4] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
    } else { // replicate boundary
      switch (lower) {
        case 8:
          win[0] = win[1];
          break;
        case 4:
          win[0] = win[2];
          win[1] = win[2];
          break;
        default:
          break;
      }
      switch (upper) {
        case 4:
          win[4] = win[2];
          win[3] = win[2];
          break;
        case 2:
          win[4] = win[3];
          break;
        default:
          break;
      }
    }
    dout = win;
  }
};

//=========================================================================
// Specialized Class: ac_boundary (w/AC_SIZE=7)
//
//-------------------------------------------------------------------------

template<typename T, int AC_BOUNDARY, int AC_BOUNDS_CONST>
class ac_boundary<T,7,AC_BOUNDARY,AC_BOUNDS_CONST>
{
public:
  // Constructor: (new empty)
  ac_boundary() { }

public: // Class Member Functions

  void apply_bounds(ac_int<7,false> lower, ac_int<7,false> upper, ac_array<T,7> din, ac_array<T,7> &dout) {
    ac_array<T,7> win = din;

    if (AC_BOUNDARY & AC_REPLICATE) { // replicate boundary
      switch (lower) {
        case 32:
          win[0] = win[1];
          break;
        case 16:
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 8:
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        default:
          break;
      }
      switch (upper) {
        case 8:
          win[6] = win[3];
          win[5] = win[3];
          win[4] = win[3];
          break;
        case 4:
          win[6] = win[4];
          win[5] = win[4];
          break;
        case 2:
          win[6] = win[5];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR_101) {
      switch (lower) {
        case 32:
          win[0] = win[2];
          break;
        case 16:
          win[0] = win[4];
          win[1] = win[3];
          break;
        case 8:
          win[0] = win[6];
          win[1] = win[5];
          win[2] = win[4];
          break;
        default:
          break;
      }
      switch (upper) {
        case 8:
          win[6] = win[0];
          win[5] = win[1];
          win[4] = win[2];
          break;
        case 4:
          win[6] = win[2];
          win[5] = win[3];
          break;
        case 2:
          win[6] = win[4];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR) {
      switch (lower) {
        case 32: // 000010
          win[0] = win[1];
          break;
        case 16: // 000100
          win[0] = win[3];
          win[1] = win[2];
          break;
        case 8:  // 001000
          win[0] = win[5];
          win[1] = win[4];
          win[2] = win[3];
          break;
        default:
          break;
      }
      switch (upper) {
        case 8:  // 10000000
          win[6] = win[1];
          win[5] = win[2];
          win[4] = win[3];
          break;
        case 4:  //
          win[6] = win[3];
          win[5] = win[4];
          break;
        case 2:  // 1000000000
          win[6] = win[5];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_CONSTANT) { // replicate boundary
      switch (lower) {
        case 32: // 000010
          win[0] = AC_BOUNDS_CONST;
          break;
        case 16: // 000100
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          break;
        case 8:  // 001000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
      switch (upper) {
        case 8:  // 10000000
          win[6] = AC_BOUNDS_CONST;
          win[5] = AC_BOUNDS_CONST;
          win[4] = AC_BOUNDS_CONST;
          break;
        case 4:  //
          win[6] = AC_BOUNDS_CONST;
          win[5] = AC_BOUNDS_CONST;
          break;
        case 2:  // 1000000000
          win[6] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
    } else { // replicate boundary
      switch (lower) {
        case 32: // 00010
          win[0] = win[1];
          break;
        case 16: // 00100
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 8:  // 01000
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        default:
          break;
      }
      switch (upper) {
        case 8:  // 10000000
          win[6] = win[3];
          win[5] = win[3];
          win[4] = win[3];
          break;
        case 4:  //
          win[6] = win[4];
          win[5] = win[4];
          break;
        case 2:  // 1000000000
          win[6] = win[5];
          break;
        default:
          break;
      }
    }
    dout = win;
  }
};

//=========================================================================
// Specialized Class: ac_boundary (w/AC_SIZE=9)
//
//-------------------------------------------------------------------------

template<typename T, int AC_BOUNDARY, int AC_BOUNDS_CONST>
class ac_boundary<T,9,AC_BOUNDARY,AC_BOUNDS_CONST>
{
public:
  // Constructor: (new empty)
  ac_boundary() { }

public: // Class Member Functions

  void apply_bounds(ac_int<9,false> lower, ac_int<9,false> upper, ac_array<T,9> din, ac_array<T,9> &dout) {
    ac_array<T,9> win = din;

    if (AC_BOUNDARY & AC_REPLICATE) { // replicate boundary
      switch (lower) {
        case 128:
          win[0] = win[1];
          break;
        case 64: // 00100
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 32: // 01000
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        case 16: // 10000
          win[0] = win[4];
          win[1] = win[4];
          win[2] = win[4];
          win[3] = win[4];
          break;
        default:
          break;
      }
      switch (upper) {
        case 16: // 10000
          win[8] = win[4];
          win[7] = win[4];
          win[6] = win[4];
          win[5] = win[4];
          break;
        case 8:  // 10000000
          win[8] = win[5];
          win[7] = win[5];
          win[6] = win[5];
          break;
        case 4:  //
          win[8] = win[6];
          win[7] = win[6];
          break;
        case 2:  // 1000000000
          win[8] = win[7];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR_101) {
      switch (lower) {
        case 128: // 000010
          win[0] = win[2];
          break;
        case 64:  // 000100
          win[0] = win[4];
          win[1] = win[3];
          break;
        case 32:  // 001000
          win[0] = win[6];
          win[1] = win[5];
          win[2] = win[4];
          break;
        case 16:  // 010000
          win[0] = win[8];
          win[1] = win[7];
          win[2] = win[6];
          win[3] = win[5];
          break;
        default:
          break;
      }
      switch (upper) {
        case 16: // 1000000
          win[8] = win[0];
          win[7] = win[1];
          win[6] = win[2];
          win[5] = win[3];
          break;
        case 8:  // 10000000
          win[8] = win[2];
          win[7] = win[3];
          win[6] = win[4];
          break;
        case 4:  //
          win[8] = win[4];
          win[7] = win[5];
          break;
        case 2:  // 1000000000
          win[8] = win[6];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR) {
      switch (lower) {
        case 128: // 000010
          win[0] = win[1];
          break;
        case 64:  // 000100
          win[0] = win[3];
          win[1] = win[2];
          break;
        case 32:  // 001000
          win[0] = win[5];
          win[1] = win[4];
          win[2] = win[3];
          break;
        case 16:  // 010000
          win[0] = win[7];
          win[1] = win[6];
          win[2] = win[5];
          win[3] = win[4];
          break;
        default:
          break;
      }
      switch (upper) {
        case 16:  // 1000000
          win[8] = win[1];
          win[7] = win[2];
          win[6] = win[3];
          win[5] = win[4];
          break;
        case 8:   // 10000000
          win[8] = win[3];
          win[7] = win[4];
          win[6] = win[5];
          break;
        case 4:   //
          win[8] = win[5];
          win[7] = win[6];
          break;
        case 2:   // 1000000000
          win[8] = win[7];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_CONSTANT) { // replicate boundary
      switch (lower) {
        case 128: // 000010
          win[0] = AC_BOUNDS_CONST;
          break;
        case 64:  // 000100
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          break;
        case 32:  // 001000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          break;
        case 16:  // 010000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          win[3] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
      switch (upper) {
        case 16:  // 1000000
          win[8] = AC_BOUNDS_CONST;
          win[7] = AC_BOUNDS_CONST;
          win[6] = AC_BOUNDS_CONST;
          win[5] = AC_BOUNDS_CONST;
          break;
        case 8:   // 10000000
          win[8] = AC_BOUNDS_CONST;
          win[7] = AC_BOUNDS_CONST;
          win[6] = AC_BOUNDS_CONST;
          break;
        case 4:   //
          win[8] = AC_BOUNDS_CONST;
          win[7] = AC_BOUNDS_CONST;
          break;
        case 2:   // 1000000000
          win[8] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
    } else { // replicate boundary
      switch (lower) {
        case 128: // 00010
          win[0] = win[1];
          break;
        case 64:  // 00100
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 32:  // 01000
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        case 16:  // 10000
          win[0] = win[4];
          win[1] = win[4];
          win[2] = win[4];
          win[3] = win[4];
          break;
        default:
          break;
      }
      switch (upper) {
        case 16:  // 10000
          win[8] = win[4];
          win[7] = win[4];
          win[6] = win[4];
          win[5] = win[4];
          break;
        case 8:   // 10000000
          win[8] = win[5];
          win[7] = win[5];
          win[6] = win[5];
          break;
        case 4:   //
          win[8] = win[6];
          win[7] = win[6];
          break;
        case 2:   // 1000000000
          win[8] = win[7];
          break;
        default:
          break;
      }
    }
    dout = win;
  }
};


//=========================================================================
// Specialized Class: ac_boundary (w/AC_SIZE=11)
//
//-------------------------------------------------------------------------

template<typename T, int AC_BOUNDARY, int AC_BOUNDS_CONST>
class ac_boundary<T,11,AC_BOUNDARY,AC_BOUNDS_CONST>
{
public:
  // Constructor: (new empty)
  ac_boundary() { }

public: // Class Member Functions

  void apply_bounds(ac_int<11,false> lower, ac_int<11,false> upper, ac_array<T,11> din, ac_array<T,11> &dout) {
    ac_array<T,11> win = din;

    if (AC_BOUNDARY & AC_REPLICATE) { // replicate boundary
      switch (lower) {
        case 512: // 000010
          win[0] = win[1];
          break;
        case 256: // 000100
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 128: // 001000
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        case 64:  // 010000
          win[0] = win[4];
          win[1] = win[4];
          win[2] = win[4];
          win[3] = win[4];
          break;
        case 32:  // 100000
          win[0] = win[5];
          win[1] = win[5];
          win[2] = win[5];
          win[3] = win[5];
          win[4] = win[5];
          break;
        default:
          break;
      }
      switch (upper) {
        case 32:  // 100000
          win[10] = win[5];
          win[9] = win[5];
          win[8] = win[5];
          win[7] = win[5];
          win[6] = win[5];
          break;
        case 16:  // 1000000
          win[10] = win[6];
          win[9] = win[6];
          win[8] = win[6];
          win[7] = win[6];
          break;
        case 8:   // 10000000
          win[10] = win[7];
          win[9] = win[7];
          win[8] = win[7];
          break;
        case 4:   //
          win[10] = win[8];
          win[9] = win[8];
          break;
        case 2:   // 1000000000
          win[10] = win[9];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR_101) {
      switch (lower) {
        case 512: // 000010
          win[0] = win[2];
          break;
        case 256: // 000100
          win[0] = win[4];
          win[1] = win[3];
          break;
        case 128: // 001000
          win[0] = win[6];
          win[1] = win[5];
          win[2] = win[4];
          break;
        case 64:  // 010000
          win[0] = win[8];
          win[1] = win[7];
          win[2] = win[6];
          win[3] = win[5];
          break;
        case 32:  // 100000
          win[0] = win[10];
          win[1] = win[9];
          win[2] = win[8];
          win[3] = win[7];
          win[4] = win[6];
          break;
        default:
          break;
      }
      switch (upper) {
        case 32:  // 100000
          win[10] = win[0];
          win[9] = win[1];
          win[8] = win[2];
          win[7] = win[3];
          win[6] = win[4];
          break;
        case 16:  // 1000000
          win[10] = win[2];
          win[9] = win[3];
          win[8] = win[4];
          win[7] = win[5];
          break;
        case 8:   // 10000000
          win[10] = win[4];
          win[9] = win[5];
          win[8] = win[6];
          break;
        case 4:   //
          win[10] = win[6];
          win[9] = win[7];
          break;
        case 2:   // 1000000000
          win[10] = win[8];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_MIRROR) {
      switch (lower) {
        case 512: // 000010
          win[0] = win[1];
          break;
        case 256: // 000100
          win[0] = win[3];
          win[1] = win[2];
          break;
        case 128: // 001000
          win[0] = win[5];
          win[1] = win[4];
          win[2] = win[3];
          break;
        case 64:  // 010000
          win[0] = win[7];
          win[1] = win[6];
          win[2] = win[5];
          win[3] = win[4];
          break;
        case 32:  // 100000
          win[0] = win[9];
          win[1] = win[8];
          win[2] = win[7];
          win[3] = win[6];
          win[4] = win[5];
          break;
        default:
          break;
      }
      switch (upper) {
        case 32:  // 100000
          win[10] = win[1];
          win[9] = win[2];
          win[8] = win[3];
          win[7] = win[4];
          win[6] = win[5];
          break;
        case 16:  // 1000000
          win[10] = win[3];
          win[9] = win[4];
          win[8] = win[5];
          win[7] = win[6];
          break;
        case 8:   // 10000000
          win[10] = win[5];
          win[9] = win[6];
          win[8] = win[7];
          break;
        case 4:   //
          win[10] = win[7];
          win[9] = win[8];
          break;
        case 2:   // 1000000000
          win[10] = win[9];
          break;
        default:
          break;
      }
    } else if (AC_BOUNDARY & AC_CONSTANT) { // replicate boundary
      switch (lower) {
        case 512: // 000010
          win[0] = AC_BOUNDS_CONST;
          break;
        case 256: // 000100
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          break;
        case 128: // 001000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          break;
        case 64:  // 010000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          win[3] = AC_BOUNDS_CONST;
          break;
        case 32:  // 100000
          win[0] = AC_BOUNDS_CONST;
          win[1] = AC_BOUNDS_CONST;
          win[2] = AC_BOUNDS_CONST;
          win[3] = AC_BOUNDS_CONST;
          win[4] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
      switch (upper) {
        case 32:  // 100000
          win[10] = AC_BOUNDS_CONST;
          win[9] = AC_BOUNDS_CONST;
          win[8] = AC_BOUNDS_CONST;
          win[7] = AC_BOUNDS_CONST;
          win[6] = AC_BOUNDS_CONST;
          break;
        case 16:  // 1000000
          win[10] = AC_BOUNDS_CONST;
          win[9] = AC_BOUNDS_CONST;
          win[8] = AC_BOUNDS_CONST;
          win[7] = AC_BOUNDS_CONST;
          break;
        case 8:   // 10000000
          win[10] = AC_BOUNDS_CONST;
          win[9] = AC_BOUNDS_CONST;
          win[8] = AC_BOUNDS_CONST;
          break;
        case 4:   //
          win[10] = AC_BOUNDS_CONST;
          win[9] = AC_BOUNDS_CONST;
          break;
        case 2:   // 1000000000
          win[10] = AC_BOUNDS_CONST;
          break;
        default:
          break;
      }
    } else { // replicate boundary
      switch (lower) {
        case 512: // 000010
          win[0] = win[1];
          break;
        case 256: // 000100
          win[0] = win[2];
          win[1] = win[2];
          break;
        case 128: // 001000
          win[0] = win[3];
          win[1] = win[3];
          win[2] = win[3];
          break;
        case 64:  // 010000
          win[0] = win[4];
          win[1] = win[4];
          win[2] = win[4];
          win[3] = win[4];
          break;
        case 32:  // 100000
          win[0] = win[5];
          win[1] = win[5];
          win[2] = win[5];
          win[3] = win[5];
          win[4] = win[5];
          break;
        default:
          break;
      }
      switch (upper) {
        case 32:  // 100000
          win[10] = win[5];
          win[9] = win[5];
          win[8] = win[5];
          win[7] = win[5];
          win[6] = win[5];
          break;
        case 16:  // 1000000
          win[10] = win[6];
          win[9] = win[6];
          win[8] = win[6];
          win[7] = win[6];
          break;
        case 8:   // 10000000
          win[10] = win[7];
          win[9] = win[7];
          win[8] = win[7];
          break;
        case 4:   //
          win[10] = win[8];
          win[9] = win[8];
          break;
        case 2:   // 1000000000
          win[10] = win[9];
          break;
        default:
          break;
      }
    }
    dout = win;
  }
};

#endif

