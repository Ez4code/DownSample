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
//  Source:         ccs_wavefile.cpp
//  Description:    Functions to read and write .WAV format audio files
////////////////////////////////////////////////////////////////////////////////

#include <ac_channel.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//  Description:    Typedefs and defines for RIFF .WAV file formats
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    char chunk_id[4];
    unsigned int chunk_size;
} WAVE_HDR;

typedef struct {
    char form_type[8];
    unsigned int hdr_size;
} CHUNK_HDR;


typedef struct {
    char data_type[4];
    unsigned int data_size;
} DATA_HDR;

/* specific waveform format structure for PCM data */
typedef struct waveformatpe_tag {
    short int    wFormatTag;        /* format type */
    short int    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    int          nSamplesPerSec;    /* sample rate */
    int          nAvgBytesPerSec;   /* for buffer estimation */
    short int    nBlockAlign;       /* block size of data */
    short int    wBitsPerSample;
} WAVEFORMATPE;

#ifndef _INC_MMSYSTEM
/* From MMSystem.h */
/* OLD general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
    short int    wFormatTag;        /* format type */
    short int    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    int          nSamplesPerSec;    /* sample rate */
    int          nAvgBytesPerSec;   /* for buffer estimation */
    short int    nBlockAlign;       /* block size of data */
} WAVEFORMAT;
/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1
/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    short int        wBitsPerSample;
} PCMWAVEFORMAT;
#endif

int read_wave_file(const char* filename, ac_channel<int> &left, ac_channel<int> &right, unsigned long int &sample_freq)
{
	// input WAV format header with null init
	WAVE_HDR 	win 	 = { "", 0 };
	CHUNK_HDR	chunkIn  = { "", 0 };
	DATA_HDR 	din 	 = { "", 0 };
	WAVEFORMATPE   wavin	 = { 1, 1, 0, 0, 1, 8 };
	
	FILE *fp_getwav = NULL;	
	int channel_number = 0;
	short int int_data[4];		  // max of 4 channels can be read
	unsigned char byte_data[4];	  // max of 4 channels can be read
	int i;
	long int number_of_samples = 0;
	
	fp_getwav = fopen(filename,"rb");
	if (!fp_getwav) {
		std::cerr << "Error opening WAV input file '" << filename << "'." << std::endl;
		return(-1);
	}
	
	// read and display header information
	fread(&win,sizeof(WAVE_HDR),1,fp_getwav);
	if (strncmp(win.chunk_id,"RIFF",4) != 0) {
		std::cerr << "Error in RIFF header of input wave file." << std::endl;
		return(-1);
	}
	
	fread(&chunkIn,sizeof(CHUNK_HDR),1,fp_getwav);
	if (strncmp(chunkIn.form_type,"WAVEfmt ",8) != 0) {
		std::cerr << "Error in WAVE header of input wave file" << std::endl;
		return(-1);
	}

	if (chunkIn.hdr_size != sizeof(WAVEFORMATPE)) {
		std::cerr << "Error in WAVE header of input wave file" << std::endl;
		return(-1);
	}
	
	fread(&wavin,sizeof(WAVEFORMATPE),1,fp_getwav);
	if (wavin.wFormatTag != WAVE_FORMAT_PCM) {
		std::cerr << "Error - input wave file not PCM" << std::endl;
		return(-1);
	}
	
	// check channel number and block size are good
	if(wavin.nChannels > 4 || wavin.nBlockAlign > 8) {
		std::cerr << "Error - Channel/BlockSize not supported" << std::endl;
		return(-1);
	}
	
	fread(&din,sizeof(DATA_HDR),1,fp_getwav);
	if (strncmp(din.data_type, "PAD ", 4) == 0) {
	   // skip padding
	   for (unsigned int pad_cnt=0; pad_cnt<(din.data_size/wavin.nBlockAlign); pad_cnt++) {
	       if (fread(int_data,wavin.nBlockAlign,1,fp_getwav) != 1) exit(1);
	   }
	   fread(&din,sizeof(DATA_HDR),1,fp_getwav);
	}
	if (strncmp(din.data_type, "data", 4) != 0) {
		std::cerr << "Error - Data type not audio data" << std::endl;
		return(-1);
	}

	number_of_samples = din.data_size/wavin.nBlockAlign;
	
#ifdef DEBUG
	// Output statistics about input wave file
	std::cout << "Number of channels:   " << wavin.nChannels << std::endl;
	std::cout << "Sample Rate:          " << wavin.nSamplesPerSec << std::endl;
	std::cout << "Bits per Sample:      " << wavin.wBitsPerSample << std::endl;
	std::cout << "Block Align:          " << wavin.nBlockAlign << std::endl;
	std::cout << "Number of Samples per Channel: " << number_of_samples << std::endl;
#endif
	sample_freq = wavin.nSamplesPerSec;
	
	if (wavin.nChannels > 1) {
		do {
			// currently, always get the LEFT channel if multiple channels
			i=0;
		} while(i < 0 || i >= wavin.nChannels);
		channel_number = i;
	}
	int x_leftInt;
	int x_rightInt;
	
	for (i = 0 ; i < number_of_samples ; i++) {
		// read data until end of file
		if (wavin.wBitsPerSample == 16) {
			if (fread(int_data,wavin.nBlockAlign,1,fp_getwav) != 1) {
				exit(1);
			}
			x_rightInt = int_data[0];
			x_leftInt = int_data[1];
		} else {
			if(fread(byte_data,wavin.nBlockAlign,1,fp_getwav) != 1) {
				exit(1);
			}
			x_leftInt = byte_data[1];
			x_leftInt ^= 0x80;
			x_leftInt <<= 8;
			x_rightInt = byte_data[0];
			x_rightInt ^= 0x80;
			x_rightInt <<= 8;
		}
		left.write(x_leftInt);
		right.write(x_rightInt);
	}
	fclose(fp_getwav);
	return(number_of_samples);
}

int write_wave_file(const char* filename, ac_channel<int> &left, ac_channel<int> &right, unsigned long int sample_freq=44100)
{
	// output WAV format header init
	WAVE_HDR     wout     = { {'R','I','F','F'}, 0L };
	CHUNK_HDR    chunkOut = { {'W','A','V','E','f','m','t',' '}, sizeof(WAVEFORMATPE) };
	DATA_HDR     dout     = { {'d','a','t','a'}, 0L };
	WAVEFORMATPE wavout   = { 1, 1, 0L, 0L, 1, 16 };
	unsigned long int nSamplesPerSec = sample_freq;
	
	FILE *fp_wavout = NULL;
	short int int_data[4];         // max of 4 channels can be read
	long int number_of_samples = (int)left.debug_size();
	
	fp_wavout = fopen(filename,"wb");
	if (!fp_wavout) {
		std::cerr << "Error opening WAV output file '" << filename << "'." << std::endl;
		return(-1);
	}
	// Set up WRITE
	wavout.wFormatTag = WAVE_FORMAT_PCM;
	wavout.wBitsPerSample = 16;
	wavout.nChannels = 2;
	wavout.nBlockAlign = wavout.nChannels*2;
	wavout.nSamplesPerSec = nSamplesPerSec;
	wavout.nAvgBytesPerSec = wavout.nSamplesPerSec*wavout.nBlockAlign;
	dout.data_size= number_of_samples * wavout.nBlockAlign;
	wout.chunk_size=dout.data_size+sizeof(DATA_HDR)+sizeof(CHUNK_HDR)+sizeof(WAVEFORMATPE);
	fwrite(&wout,sizeof(WAVE_HDR),1,fp_wavout);
	fwrite(&chunkOut,sizeof(CHUNK_HDR),1,fp_wavout);
	fwrite(&wavout,sizeof(WAVEFORMATPE),1,fp_wavout);
	fwrite(&dout,sizeof(DATA_HDR),1,fp_wavout);
	
	// processing loop
   for (unsigned int i=0; i<left.debug_size(); i++) {
		int_data[1] = (short int)(left[i]);
		int_data[0] = (short int)(right[i]);
		// write 16 bit data
		if (fwrite(int_data,wavout.nBlockAlign,1,fp_wavout) != 1) {
			std::cerr << "Error writing output wave file" << std::endl;
			return(-1);
		}
	}
	fclose(fp_wavout);
	return(0);
}

