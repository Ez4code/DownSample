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
////////////////////////////////////////////////////////////////////////////////
// NO WARRANTY. MENTOR GRAPHICS EXPRESSLY DISCLAIMS ALL WARRANTY 
// FOR THE SOFTWARE. TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE 
// LAW, THE SOFTWARE AND ANY RELATED DOCUMENTATION IS PROVIDED "AS IS"
// AND WITH ALL FAULTS AND WITHOUT WARRANTIES OR CONDITIONS OF ANY 
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE, OR NONINFRINGEMENT. THE ENTIRE RISK ARISING OUT OF USE OR 
// DISTRIBUTION OF THE SOFTWARE REMAINS WITH YOU.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  Source:         ccs_wavefile.h
//  Description:    Header defining functions contained in wavefile.cpp
//                  for reading/writing .WAV format audio files
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_CCS_WAVEFILE_H
#define INCLUDED_CCS_WAVEFILE_H

#include <ac_channel.h>

//=============================================
// RIFF WAV file format functions
//---------------------------------------------
// read_wave_file
//    Reads the .WAV file specified in the filename
// parameter and returns the left and right samples
// in an STL list. The .WAV file data is by default
// a 32 bit integer value. Scaling and type conversion
// should be performed on the returned list.
int read_wave_file(const char* filename, ac_channel<int> &left, ac_channel<int> &right, unsigned long int &sample_freq);

//---------------------------------------------
// write_wave_file
//    Writes the 32 bit integer values to a .WAV
// format file named in the filename parameter.
int write_wave_file(const char* filename, ac_channel<int> &left, ac_channel<int> &right, unsigned long int sample_freq);

#endif

