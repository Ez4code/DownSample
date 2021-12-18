////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
//
// Copyright (c) 2003-2015 Calypto Design Systems, Inc.
//       All Rights Reserved
//
// This document contains information that is proprietary to Calypto Design
// Systems, Inc. The original recipient of this document may duplicate this
// document in whole or in part for internal business purposes only, provided
// that this entire notice appears in all copies. In duplicating any part of
// this document, the recipient agrees to make every reasonable effort to
// prevent the unauthorized use and distribution of the proprietary information.
//
// The design information contained in this file is intended to be an example
// of the functionality which the end user may study in preparation for creating
// their own custom interfaces. This design does not present a complete
// implementation of the named protocol or standard.
//
// NO WARRANTY.
// CALYPTO DESIGN SYSTEMS, INC. EXPRESSLY DISCLAIMS ALL WARRANTY
// FOR THE SOFTWARE. TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
// LAW, THE SOFTWARE AND ANY RELATED DOCUMENTATION IS PROVIDED "AS IS"
// AND WITH ALL FAULTS AND WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NONINFRINGEMENT. THE ENTIRE RISK ARISING OUT OF USE OR
// DISTRIBUTION OF THE SOFTWARE REMAINS WITH YOU.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  Source:         ccs_bmpfile.h
//  Description:    Header defining functions contained in ccs_bmpfile.cpp
//                  for reading/writing .BMP format image files
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_CCS_BMPFILE_H
#define INCLUDED_CCS_BMPFILE_H

#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

class ccs_bmpheader
{
public:
  ccs_bmpheader() {}
  ~ccs_bmpheader() {}
  void read(FILE *fp);
  void write(unsigned int realfilesize, FILE *fp);
private:
  char signature[2];
  unsigned filesize;
  unsigned reserved;
  unsigned dataoffset;
};

class ccs_bmpinfoheader
{
public:
  ccs_bmpinfoheader() {}
  ~ccs_bmpinfoheader() {}

  unsigned width;
  unsigned height;
  unsigned imagesize;
  unsigned short bitcount;
  unsigned compression;

  void read(FILE *fp);
  void write(FILE *fp);

private:
  unsigned size;

  unsigned short planes;
  unsigned xpixelsperm;
  unsigned ypixelsperm;
  unsigned colorsused;
  unsigned colorsimportant;
};

class ccs_bmpcolortable
{

public:
  ccs_bmpcolortable() {};
  ~ccs_bmpcolortable() {};

  void dumparray();

  void read(int bitsperpixel, FILE *fp);
  unsigned char * bgr(unsigned int index);

private:
  int numcolors;
  unsigned char array[1024];
};

// LIMITED tO no compression, 1-bit, 4-bit, 24-bit support only
class ccs_bmpfile
{
public:
  // reads in a bit-map file on construction
  ccs_bmpfile(const char * f);

  ~ccs_bmpfile() {}

  inline unsigned rows() { return infoheader.height; }
  inline unsigned columns() { return infoheader.width; }

  // returns pixel colors with 0-index in top left (rather than bottom left as in bmp file)
  inline char blue(unsigned row, unsigned column) { return array[3 * ((rows() - row - 1) * columns() + column)]; }
  inline char green(unsigned row, unsigned column) { return array[3 * ((rows() - row - 1) * columns() + column) + 1]; }
  inline char red(unsigned row, unsigned column) { return array[3 * ((rows() - row - 1) * columns() + column) + 2]; }

  // debug the array data
  void dumparray();

  // write out a bmp file called f (in 24-bit irrespective of the resolution read in)
  void write(char * f);

private: // function

  //
  // Always reads in 4-byte blocks so handles end-line padding
  //
  unsigned char getbyte(FILE *fp);

  //
  // returns 4-byte value representing a pixel
  // without any colortable expansion
  //
  unsigned int getpixel(FILE * fp);

  //
  // writes just the array data in 24-bit bmp format
  //
  void writearray(FILE * ofp);

  //
  // reads array data with colortable expansion if necessary
  //
  void readarray(ccs_bmpcolortable * colortable, FILE *fp);

private: // data
  const char * filename;
  ccs_bmpheader header;
  ccs_bmpinfoheader infoheader;
  ccs_bmpcolortable colortable;
  unsigned char * array;
  unsigned short bytesleft;
  unsigned int fourbytebuffer;
  unsigned short bitsleft;
  unsigned char pixelbuffer;
};

#endif

