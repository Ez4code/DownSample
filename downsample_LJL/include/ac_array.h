/**************************************************************************
 *                                                                        *
 *  Algorithmic C (tm) Math Library                                       *
 *                                                                        *
 *  Software Version: 3.1                                                 *
 *                                                                        *
 *  Release Date    : Tue Nov 13 10:45:51 PST 2018                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 3.1.2                                               *
 *                                                                        *
 *  Copyright , Mentor Graphics Corporation,                     *
 *                                                                        *
 *  All Rights Reserved.                                                  *
 *  
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/
//  *************************************************************************
//  File : ac_array.h
//
//  Created on: Jun 14, 2017
//
//  Description:  Container class for multi-dimensional arrays, simplifies copy
//
//  Usage:
//
//  *************************************************************************

#ifndef _INCLUDED_AC_ARRAY_H_
#define _INCLUDED_AC_ARRAY_H_

#ifdef AC_ARRAY_ASSERT
#include <ac_assert.h>
#define AC_A_ASSERT(cond) ac::ac_assert(__FILE__, __LINE__, #cond, cond)
#else // !AC_ARRAY_ASSERT
#define AC_A_ASSERT(cond)
#endif // AC_ARRAY_ASSERT

// Forward declaration
template <typename T, unsigned D1, unsigned D2, unsigned D3> class ac_array;

//=========================================================================
// Class: ac_array_b
//
// Description: Base class for n-d array
//
// Usage:
//
// Notes:
//
//-------------------------------------------------------------------------

template <typename T, typename Td, unsigned D>
class ac_array_b
{
public:
  typedef T ElemType;

  ac_array_b() {}
  virtual ~ac_array_b() {}

  T &operator [] (unsigned i) {
    AC_A_ASSERT(i < D);
    return d[i];
  }

  const T &operator [] (unsigned i) const {
    AC_A_ASSERT(i < D);
    return d[i];
  }

  unsigned size() const { return D; }

  void set(const Td &ival) {
    for ( unsigned i = 0; i < D; ++i ) {
      set(i, ival);
    }
  }

  void set(unsigned i, const Td &ival) {
    AC_A_ASSERT(i < D);
    d[i] = ival;
  }

  void clearAll(bool dc=false) {
    for ( unsigned i = 0; i < D; ++i ) {
      clear(i, dc);
    }
  }

  void clear(unsigned i, bool dc=false) {
    AC_A_ASSERT(i < D);
    if ( !dc ) {
      d[i] = 0;
    } else {
      Td v;
      d[i] = v;
    }
  }

public: // data
  T d[D];
};

//=========================================================================
// Class: ac_array
//
// Description: Multi-dimension array
//
// Usage:
//
// Notes:
//
//-------------------------------------------------------------------------

template <typename T, unsigned D1, unsigned D2=0, unsigned D3=0>
class ac_array : public ac_array_b< ac_array<T,D2,D3>, T, D1>
{
  typedef ac_array_b< ac_array<T,D2,D3>, T, D1> Base;
public:

  ac_array() {}
  ac_array(const T &ival) { Base::set(ival); }
  virtual ~ac_array() {}

  ac_array &operator = (const T &v) { Base::set(v); return *this; }
};

//=========================================================================
// Specialization Class: ac_array for 1 dimension (row)
//
// Description: Multi-dimension array
//
// Usage:
//
// Notes:
//
//-------------------------------------------------------------------------

template <typename T, unsigned D1>
class ac_array<T,D1,0,0> : public ac_array_b<T,T,D1>
{
  typedef ac_array_b<T,T,D1> Base;
public:
  ac_array() {}
  ac_array(const T &ival) { Base::set(ival); }
  virtual ~ac_array() {}

  ac_array &operator = (const T &v) { Base::set(v); return *this; }

};

template <typename T>
class ac_array<T,0,0,0> : public ac_array_b<T,T,1>
{
  typedef ac_array_b<T,T,1> Base;
public:
  ac_array() {}
  ac_array(const T &ival) { Base::set(ival); }
  virtual ~ac_array() {}

  ac_array &operator = (const T &v) { Base::set(v); return *this; }
};

#endif

