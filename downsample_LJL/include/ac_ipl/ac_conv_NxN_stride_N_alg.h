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

#ifndef _INCLUDED_AC_CONV_NXN_STRIDE_N_ALG_H_
#define _INCLUDED_AC_CONV_NXN_STRIDE_N_ALG_H_

#include <stdio.h>
#include <cassert>
#include <ac_channel.h>
#include <ac_ipl/ac_convolution.h>
#include <ac_ipl/ac_boundary.h>
#include <ac_fixed.h>
template<typename T, typename T2, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int STRIDE=2,int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class conv_stride_N_pad_N_alg
{
  static const int AC_NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(AC_WIN_SIZE/2)))/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(AC_WIN_SIZE/2)))/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + AC_WIN_SIZE - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(AC_WIN_SIZE/2));
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + AC_WIN_SIZE - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(AC_WIN_SIZE/2));
  /* T inputImg[AC_NUM_ROWS][AC_NUM_COLS]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */

public:
  conv_stride_N_pad_N_alg() {
    printf("%dx%d conv stride %d zero pad\n",AC_WIN_SIZE,AC_WIN_SIZE,STRIDE);
    printf("input %3d x %3d\n", AC_NUM_ROWS, AC_NUM_COLS);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(AC_NUM_ROWS == EXP_AC_NUM_ROWS_IN);
    assert(AC_NUM_COLS == EXP_AC_NUM_COLS_IN);
  }
  int boundsH(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >AC_NUM_COLS-1)
      { idx = AC_NUM_COLS-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >AC_NUM_COLS-1)
      { idx = 2*AC_NUM_COLS-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >AC_NUM_COLS-1)
      { idx = 2*AC_NUM_COLS-1 - idx -1; }
      return idx;
    } else if (AC_BOUNDARY&AC_CONSTANT) {
      if (idx<0)
      { idx = AC_BOUNDS_CONST; }
      else if (idx >AC_NUM_COLS-1)
      { idx = AC_BOUNDS_CONST; }
      return idx;
    }
    return idx;
  }
  int boundsV(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = AC_NUM_ROWS-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = 2*AC_NUM_ROWS-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = 2*AC_NUM_ROWS-1 - idx -1; }
      return idx;
    }
    return idx;
  }
  void filter(T inputImg[AC_NUM_ROWS][AC_NUM_COLS], T2 kernel[AC_WIN_SIZE][AC_WIN_SIZE], T2 outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    T2 acc = 0;
    int row=0;
    int col = 0;
    T window[AC_WIN_SIZE][AC_WIN_SIZE];
    for (int i=0; i<AC_NUM_ROWS-((AC_BOUNDARY == AC_NOPAD) ? AC_WIN_SIZE:AC_WIN_SIZE/2)+1; i+=STRIDE) {
      col = 0;
      for (int j=0 ; j<AC_NUM_COLS-((AC_BOUNDARY == AC_NOPAD) ? AC_WIN_SIZE:AC_WIN_SIZE/2)+1; j+=STRIDE) {
        acc = 0;
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            if (AC_BOUNDARY&AC_CONSTANT) {
              if (i+r-AC_WIN_SIZE/2<0 || i+r-AC_WIN_SIZE/2 > AC_NUM_ROWS-1 || j+c-AC_WIN_SIZE/2<0 || j+c-AC_WIN_SIZE/2 > AC_NUM_COLS-1)
              { window[r][c] = AC_BOUNDS_CONST; }
              else
              { window[r][c] = inputImg[i+r-AC_WIN_SIZE/2][j+c-AC_WIN_SIZE/2]; }
            } else
            { window[r][c] = inputImg[boundsV(i+r-((AC_BOUNDARY == AC_NOPAD)?0:AC_WIN_SIZE/2))][boundsH(j+c-((AC_BOUNDARY == AC_NOPAD)?0:AC_WIN_SIZE/2))]; }

          }
        }
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            acc+=window[r][c]*kernel[r][c];
          }
        }
        outputImg[row][col] = acc;
        col++;
      }
      row++;
    }
    printInput(inputImg);
    printf("\n");
    printOutput(outputImg);
  }

  void printInput(T inputImg[AC_NUM_ROWS][AC_NUM_COLS]) {
    for (int i=0; i<AC_NUM_ROWS; i++) {
      for (int j=0 ; j<AC_NUM_COLS; j++) {
        printf("%3d ", (int)inputImg[i][j]);
      }
      printf("\n");
    }
  }
  void printOutput(T2 outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
      for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
        printf("%3d ", (int)outputImg[i][j]);
      }
      printf("\n");
    }
  }
};

template<typename T, typename T2, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int OUT_CHANNELS, int PADDING, int FILTER_HEIGHT_WIDTH, int STRIDE=1,int AC_BOUNDARY = AC_CONSTANT, int AC_BOUNDS_CONST=0>
class conv2d_alg
{
  static const int AC_NUM_ROWS_OUT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (IN_WIDTH - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  /* T inputImg[AC_NUM_ROWS][AC_NUM_COLS]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */

public:
  conv2d_alg() {
    printf("%dx%d conv stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  int boundsH(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_WIDTH-1)
      { idx = IN_WIDTH-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx -1; }
      return idx;
    } else if (AC_BOUNDARY&AC_CONSTANT) {
      if (idx<0)
      { idx = AC_BOUNDS_CONST; }
      else if (idx >IN_WIDTH-1)
      { idx = AC_BOUNDS_CONST; }
      return idx;
    }
    return idx;
  }
  int boundsV(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_HEIGHT-1)
      { idx = IN_HEIGHT-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx -1; }
      return idx;
    }
    return idx;
  }

  //TODO change kernel array to match tensor dimensions
  void filter(T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH], T2 kernel[IN_CHANNELS][OUT_CHANNELS][FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH], T2 outputImg[OUT_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
      T2 acc = 0;
      int row=0;
      int col = 0;
      T window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; j+=STRIDE) {
          acc = 0;
          IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                if (AC_BOUNDARY&AC_CONSTANT) {
                  if (i+r-FILTER_HEIGHT_WIDTH/2<0 || i+r-FILTER_HEIGHT_WIDTH/2 > IN_HEIGHT-1 || j+c-FILTER_HEIGHT_WIDTH/2<0 || j+c-FILTER_HEIGHT_WIDTH/2 > IN_WIDTH-1)
                  { window[r][c] = AC_BOUNDS_CONST; }
                  else
                  { window[r][c] = inputImg[c1][i+r-FILTER_HEIGHT_WIDTH/2][j+c-FILTER_HEIGHT_WIDTH/2]; }
                } else
                { window[r][c] = inputImg[c1][boundsV(i+r-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))][boundsH(j+c-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))]; }

              }
            }
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                acc+=window[r][c]*kernel[c1][d][r][c];
              }
            }
          }
          outputImg[d][row][col] = acc;
          col++;
        }
        row++;
      }
    }
  }

  void printInput(T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH]) {
    IN_CHAN:for (int c=0; c<IN_CHANNELS; c++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[c][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
  void printOutput(T2 outputImg[OUT_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {

    DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
      for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
        for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
          printf("%3d ", (int)outputImg[d][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
    printf("\n");
  }
};
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int FILTER_HEIGHT_WIDTH, int STRIDE=2>
class max_pool_alg
{
  static const int AC_NUM_ROWS_OUT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (IN_WIDTH - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  /* T inputImg[AC_NUM_ROWS][IN_WIDTH]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */

public:
  max_pool_alg() {
    printf("%dx%d max pool stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  void filter(T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH], T outputImg[IN_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {

    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      T max = 0;
      int row=0;
      int col = 0;
      T window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT- FILTER_HEIGHT_WIDTH +1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-FILTER_HEIGHT_WIDTH+1; j+=STRIDE) {
          max = 0;
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              window[r][c] = inputImg[c1][i+r][j+c];
            }
          }
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              if (window[r][c]>max)
              { max = window[r][c]; }
            }
          }
          outputImg[c1][row][col] = max;
          col++;
        }
        row++;
      }
    }

  }

  void printInput(T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH]) {
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
  void printOutput(T outputImg[IN_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
        for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
          printf("%3d ", (int)outputImg[c1][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
};

template<typename T0, typename T1, typename T2, typename T3, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int OUT_CHANNELS, int FILTER_HEIGHT_WIDTH, int STRIDE=2,int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class conv2d_alg_stream
{
  static const int AC_NUM_ROWS_OUT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (IN_WIDTH - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  /* T inputImg[AC_NUM_ROWS][AC_NUM_COLS]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */
  T0 inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH];
  T3 outputImg[OUT_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT];
public:
  conv2d_alg_stream() {
    printf("%dx%d conv stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  int boundsH(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_WIDTH-1)
      { idx = IN_WIDTH-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx -1; }
      return idx;
    } else if (AC_BOUNDARY&AC_CONSTANT) {
      if (idx<0)
      { idx = AC_BOUNDS_CONST; }
      else if (idx >IN_WIDTH-1)
      { idx = AC_BOUNDS_CONST; }
      return idx;
    }
    return idx;
  }
  int boundsV(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_HEIGHT-1)
      { idx = IN_HEIGHT-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx -1; }
      return idx;
    }
    return idx;
  }
  void filter(ac_channel<T0> &inputImgIn, T1 kernel[IN_CHANNELS][OUT_CHANNELS][FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH], T2 bias[OUT_CHANNELS], ac_channel<T3> &outputImgOut) {

    INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          inputImg[c1][i][j] = inputImgIn.read();
        }
      }
    }

    DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
      ac_fixed<T0::width+T1::width+ac::nbits<FILTER_HEIGHT_WIDTH *FILTER_HEIGHT_WIDTH+IN_CHANNELS>::val,T0::width+T1::i_width +ac::nbits<FILTER_HEIGHT_WIDTH *FILTER_HEIGHT_WIDTH+IN_CHANNELS>::val> acc = 0;
      int row=0;
      int col = 0;
      T0 window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; j+=STRIDE) {
          acc = 0;
          IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                if (AC_BOUNDARY&AC_CONSTANT) {
                  if (i+r-FILTER_HEIGHT_WIDTH/2<0 || i+r-FILTER_HEIGHT_WIDTH/2 > IN_HEIGHT-1 || j+c-FILTER_HEIGHT_WIDTH/2<0 || j+c-FILTER_HEIGHT_WIDTH/2 > IN_WIDTH-1)
                  { window[r][c] = AC_BOUNDS_CONST; }
                  else
                  { window[r][c] = inputImg[c1][i+r-FILTER_HEIGHT_WIDTH/2][j+c-FILTER_HEIGHT_WIDTH/2]; }
                } else
                { window[r][c] = inputImg[c1][boundsV(i+r-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))][boundsH(j+c-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))]; }

              }
            }
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                acc+=window[r][c]*kernel[c1][d][r][c];
              }
            }
          }
          outputImg[d][row][col] = (acc + bias[d]).to_int();
          col++;
        }
        row++;
      }
    }


    DEPTHO:for (int d=0; d<OUT_CHANNELS; d++) {
      for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
        for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
          outputImgOut.write(outputImg[d][i][j]);
        }
      }
    }
  }

  void printInput() {
    IN_CHAN:for (int c=0; c<IN_CHANNELS; c++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[c][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
  void printOutput() {
    DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
      for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
        for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
          printf("%3d ", (int)outputImg[d][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
    printf("\n");
  }
};
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int FILTER_HEIGHT_WIDTH, int STRIDE=2>
class max_pool_alg_stream
{
  static const int OUT_HEIGHT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int OUT_WIDTH = (IN_WIDTH - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = OUT_HEIGHT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  static const int EXP_AC_NUM_COLS_IN = OUT_WIDTH*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  /* T inputImg[AC_NUM_ROWS][IN_WIDTH]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */
  T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH];
  T outputImg[IN_CHANNELS][OUT_HEIGHT][OUT_WIDTH];
public:
  max_pool_alg_stream() {
    printf("%dx%d max pool stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", OUT_HEIGHT, OUT_WIDTH);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  void run(ac_channel<T> &inputImgIn, ac_channel<T> &outputImgOut) {


    INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          inputImg[c1][i][j] = inputImgIn.read();
        }
      }
    }
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      T max = 0;
      int row=0;
      int col = 0;
      T window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT- FILTER_HEIGHT_WIDTH +1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-FILTER_HEIGHT_WIDTH+1; j+=STRIDE) {
          max = 0;
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              window[r][c] = inputImg[c1][i+r][j+c];
            }
          }
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              if (window[r][c]>max)
              { max = window[r][c]; }
            }
          }
          outputImg[c1][row][col] = max;
          col++;
        }
        row++;
      }
    }
    OUT_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<OUT_HEIGHT; i++) {
        for (int j=0 ; j<OUT_WIDTH; j++) {
          outputImgOut.write(outputImg[c1][i][j]);
        }
      }
    }
  }

  void printInput() {
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[c1][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
  void printOutput() {
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<OUT_HEIGHT; i++) {
        for (int j=0 ; j<OUT_WIDTH; j++) {
          printf("%3d ", (int)outputImg[c1][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
  }
};
template<typename T, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int STRIDE=2>
class max_pool_stride_N_pad_0_alg
{
  static const int AC_NUM_ROWS_OUT = (AC_NUM_ROWS - AC_WIN_SIZE )/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (AC_NUM_COLS - AC_WIN_SIZE )/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + AC_WIN_SIZE ;
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + AC_WIN_SIZE ;
  /* T inputImg[IN_HEIGHT][IN_WIDTH]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */

public:
  max_pool_stride_N_pad_0_alg() {
    printf("%dx%d max pool stride %d zero pad\n",AC_WIN_SIZE,AC_WIN_SIZE,STRIDE);
    printf("input %3d x %3d\n", AC_NUM_ROWS, AC_NUM_COLS);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(AC_NUM_ROWS == EXP_AC_NUM_ROWS_IN);
    assert(AC_NUM_COLS == EXP_AC_NUM_COLS_IN);
  }
  void filter(T inputImg[AC_NUM_ROWS][AC_NUM_COLS], T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    T max = 0;
    int row=0;
    int col = 0;
    T window[AC_WIN_SIZE][AC_WIN_SIZE];
    for (int i=0; i<AC_NUM_ROWS- AC_WIN_SIZE +1; i+=STRIDE) {
      col = 0;
      for (int j=0 ; j<AC_NUM_COLS-AC_WIN_SIZE+1; j+=STRIDE) {
        max = 0;
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            window[r][c] = inputImg[i+r][j+c];
          }
        }
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            if (window[r][c]>max)
            { max = window[r][c]; }
          }
        }
        outputImg[row][col] = max;
        col++;
      }
      row++;
    }
    printInput(inputImg);
    printf("\n");
    printOutput(outputImg);
  }

  void printInput(T inputImg[AC_NUM_ROWS][AC_NUM_COLS]) {
    for (int i=0; i<AC_NUM_ROWS; i++) {
      for (int j=0 ; j<AC_NUM_COLS; j++) {
        printf("%3d ", (int)inputImg[i][j]);
      }
      printf("\n");
    }
  }
  void printOutput(T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]) {
    for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
      for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
        printf("%3d ", (int)outputImg[i][j]);
      }
      printf("\n");
    }
  }
};

template<typename T, int AC_WIN_SIZE, int AC_NUM_ROWS, int AC_NUM_COLS, int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class window_stride_1_pad_1_alg
{

public:
  window_stride_1_pad_1_alg() {
    printf("NxN conv stride 1 zero pad\n");
    printf("input %3d x %3d\n", AC_NUM_ROWS, AC_NUM_COLS);
  }

  int boundsH(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >AC_NUM_COLS-1)
      { idx = AC_NUM_COLS-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >AC_NUM_COLS-1)
      { idx = 2*AC_NUM_COLS-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >AC_NUM_COLS-1)
      { idx = 2*AC_NUM_COLS-1 - idx -1; }
      return idx;
    } else if (AC_BOUNDARY&AC_CONSTANT) {
      if (idx<0)
      { idx = AC_BOUNDS_CONST; }
      else if (idx >AC_NUM_COLS-1)
      { idx = AC_BOUNDS_CONST; }
      return idx;
    }
    return idx;
  }
  int boundsV(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = AC_NUM_ROWS-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = 2*AC_NUM_ROWS-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >AC_NUM_ROWS-1)
      { idx = 2*AC_NUM_ROWS-1 - idx -1; }
      return idx;
    }
    return idx;
  }
  void windows(T inputImg[AC_NUM_ROWS][AC_NUM_COLS], T window[AC_NUM_ROWS][AC_NUM_COLS][AC_WIN_SIZE][AC_WIN_SIZE]) {

    for (int i=0; i<AC_NUM_ROWS; i+=1) {
      for (int j=0 ; j<AC_NUM_COLS; j+=1) {
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            if (AC_BOUNDARY&AC_CONSTANT) {
              if (i+r-AC_WIN_SIZE/2<0 || i+r-AC_WIN_SIZE/2 > AC_NUM_ROWS-1 || j+c-AC_WIN_SIZE/2<0 || j+c-AC_WIN_SIZE/2 > AC_NUM_COLS-1)
              { window[i][j][r][c] = AC_BOUNDS_CONST; }
              else
              { window[i][j][r][c] = inputImg[i+r-AC_WIN_SIZE/2][j+c-AC_WIN_SIZE/2]; }
            } else
            { window[i][j][r][c] = inputImg[boundsV(i+r-AC_WIN_SIZE/2)][boundsH(j+c-AC_WIN_SIZE/2)]; }
          }
        }
      }
    }
  }

  void printInput(T inputImg[AC_NUM_ROWS][AC_NUM_COLS]) {
    for (int i=0; i<AC_NUM_ROWS; i++) {
      for (int j=0 ; j<AC_NUM_COLS; j++) {
        printf("%3d ", (int)inputImg[i][j]);
      }
      printf("\n");
    }
  }

  void printOutput(T windows[AC_NUM_ROWS][AC_NUM_COLS][AC_WIN_SIZE][AC_WIN_SIZE]) {
    for (int i=0; i<AC_NUM_ROWS; i++) {
      for (int j=0 ; j<AC_NUM_COLS; j++) {
        for (int r=0; r<AC_WIN_SIZE; r++) {
          for (int c=0; c<AC_WIN_SIZE; c++) {
            printf("%3d ", (int)windows[i][j][r][c]);
          }
          printf("\n");
        }
        printf("\n");
      }
    }
  }
};


template<typename T0, typename T1, typename T2, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int OUT_CHANNELS, int FILTER_HEIGHT_WIDTH, int STRIDE=2,int AC_BOUNDARY = AC_REPLICATE, int AC_BOUNDS_CONST=0>
class conv2d_stream
{
  static const int AC_NUM_ROWS_OUT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int AC_NUM_COLS_OUT = (IN_WIDTH - FILTER_HEIGHT_WIDTH + ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2)))/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = AC_NUM_ROWS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  static const int EXP_AC_NUM_COLS_IN = AC_NUM_COLS_OUT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH - ((AC_BOUNDARY == AC_NOPAD) ? 0: 2*(FILTER_HEIGHT_WIDTH/2));
  /* T inputImg[AC_NUM_ROWS][AC_NUM_COLS]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */
  T0 inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH];
  T2 outputImg[IN_CHANNELS][OUT_CHANNELS][AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT];
public:
  conv2d_stream() {
    printf("%dx%d conv stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", AC_NUM_ROWS_OUT, AC_NUM_COLS_OUT);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  int boundsH(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_WIDTH-1)
      { idx = IN_WIDTH-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_WIDTH-1)
      { idx = 2*IN_WIDTH-1 - idx -1; }
      return idx;
    } else if (AC_BOUNDARY&AC_CONSTANT) {
      if (idx<0)
      { idx = AC_BOUNDS_CONST; }
      else if (idx >IN_WIDTH-1)
      { idx = AC_BOUNDS_CONST; }
      return idx;
    }
    return idx;
  }
  int boundsV(int idxIn) {
    int idx = idxIn;
    if (AC_BOUNDARY&AC_REPLICATE) {
      if (idx<0)
      { idx = 0; }
      else if (idx >IN_HEIGHT-1)
      { idx = IN_HEIGHT-1; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR) {
      if (idx<0)
      { idx = -idx-1; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx; }
      return idx;
    } else if (AC_BOUNDARY&AC_MIRROR_101) {
      if (idx<0)
      { idx = -idx; }
      else if (idx >IN_HEIGHT-1)
      { idx = 2*IN_HEIGHT-1 - idx -1; }
      return idx;
    }
    return idx;
  }
  void filter(ac_channel<T0> &inputImgIn, const T1 kernel[OUT_CHANNELS][FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH], T2 bias, ac_channel<T2> &outputImgOut) {


    for (int i=0; i<IN_HEIGHT; i++) {
      for (int j=0 ; j<IN_WIDTH; j++) {
        INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          inputImg[c1][i][j] = inputImgIn.read();
        }
      }
    }

    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
        //ac_int * ac_fixed and accumulated across IN_CHANNELS filters
        ac_fixed<T0::width+T1::width+ac::log2_ceil<FILTER_HEIGHT_WIDTH *FILTER_HEIGHT_WIDTH+IN_CHANNELS>::val,T0::width+T1::i_width +ac::log2_ceil<FILTER_HEIGHT_WIDTH *FILTER_HEIGHT_WIDTH+IN_CHANNELS>::val> acc = 0;
        int row=0;
        int col = 0;
        T0 window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
        for (int i=0; i<IN_HEIGHT-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; i+=STRIDE) {
          col = 0;
          for (int j=0 ; j<IN_WIDTH-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; j+=STRIDE) {
            acc = 0;
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                if (AC_BOUNDARY&AC_CONSTANT) {
                  if (i+r-FILTER_HEIGHT_WIDTH/2<0 || i+r-FILTER_HEIGHT_WIDTH/2 > IN_HEIGHT-1 || j+c-FILTER_HEIGHT_WIDTH/2<0 || j+c-FILTER_HEIGHT_WIDTH/2 > IN_WIDTH-1)
                  { window[r][c] = AC_BOUNDS_CONST; }
                  else
                  { window[r][c] = inputImg[c1][i+r-FILTER_HEIGHT_WIDTH/2][j+c-FILTER_HEIGHT_WIDTH/2]; }
                } else
                { window[r][c] = inputImg[c1][boundsV(i+r-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))][boundsH(j+c-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))]; }

              }
            }
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                acc+=window[r][c]*kernel[d*IN_CHANNELS + c1][r][c];
              }
            }
            outputImg[c1][d][row][col] = acc + bias;
            col++;
          }
          row++;
        }
      }
    }

    for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
      for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
        O_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          DEPTHO:for (int d=0; d<OUT_CHANNELS; d++) {
            outputImgOut.write(outputImg[c1][d][i][j]);
          }
        }
      }
    }

  }

  void filter_axi_stream(ac_channel<axi4VideoStream<T0,1> > &inputImgIn, const T1 kernel[IN_CHANNELS][OUT_CHANNELS][FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH], ac_channel<axi4VideoStream<T2,1> > &outputImgOut) {


    for (int i=0; i<IN_HEIGHT; i++) {
      for (int j=0 ; j<IN_WIDTH; j++) {
        INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          inputImg[c1][i][j] = inputImgIn.read().data;
        }
      }
    }
    DEPTH:for (int d=0; d<OUT_CHANNELS; d++) {
      T2 acc = 0;
      int row=0;
      int col = 0;
      T0 window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-((AC_BOUNDARY == AC_NOPAD) ? FILTER_HEIGHT_WIDTH:FILTER_HEIGHT_WIDTH/2)+1; j+=STRIDE) {

          IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
            acc = 0;
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                if (AC_BOUNDARY&AC_CONSTANT) {
                  if (i+r-FILTER_HEIGHT_WIDTH/2<0 || i+r-FILTER_HEIGHT_WIDTH/2 > IN_HEIGHT-1 || j+c-FILTER_HEIGHT_WIDTH/2<0 || j+c-FILTER_HEIGHT_WIDTH/2 > IN_WIDTH-1)
                  { window[r][c] = AC_BOUNDS_CONST; }
                  else
                  { window[r][c] = inputImg[c1][i+r-FILTER_HEIGHT_WIDTH/2][j+c-FILTER_HEIGHT_WIDTH/2]; }
                } else
                { window[r][c] = inputImg[c1][boundsV(i+r-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))][boundsH(j+c-((AC_BOUNDARY == AC_NOPAD)?0:FILTER_HEIGHT_WIDTH/2))]; }

              }
            }
            for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
              for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
                acc+=window[r][c]*kernel[d*IN_CHANNELS + c1][r][c];
              }
            }
            outputImg[c1][d][row][col] = acc;
          }
          col++;
        }
        row++;
      }
    }

    for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
      for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
        DEPTHO:for (int d=0; d<OUT_CHANNELS/IN_CHANNELS; d++) {
          O_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
            axi4VideoStream<T0,1> tmp;
            tmp.data = outputImg[d*IN_CHANNELS + c1][i][j];
            outputImgOut.write(tmp);
          }
        }
      }
    }

  }
  void printInput() {
#ifndef __SYNTHESIS__
    IN_CHAN:for (int c=0; c<IN_CHANNELS; c++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[c][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
#endif
  }
  void printOutput() {
#ifndef __SYNTHESIS__
    DEPTH:for (int d=0; d<OUT_CHANNELS/IN_CHANNELS; d++) {
      IN_CHAN:for (int c=0; c<IN_CHANNELS; c++) {
        for (int i=0; i<AC_NUM_ROWS_OUT; i++) {
          for (int j=0 ; j<AC_NUM_COLS_OUT; j++) {
            printf("%3d ", (int)outputImg[d*IN_CHANNELS+c][i][j]);
          }
          printf("\n");
        }
        printf("\n");
      }
      printf("\n");
    }
#endif
  }
};
template<typename T, int IN_HEIGHT, int IN_WIDTH, int IN_CHANNELS, int FILTER_HEIGHT_WIDTH, int STRIDE=2>
class max_pool_stream
{
  static const int OUT_HEIGHT = (IN_HEIGHT - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int OUT_WIDTH = (IN_WIDTH - FILTER_HEIGHT_WIDTH )/STRIDE+1;
  static const int EXP_AC_NUM_ROWS_IN = OUT_HEIGHT*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  static const int EXP_AC_NUM_COLS_IN = OUT_WIDTH*STRIDE - STRIDE + FILTER_HEIGHT_WIDTH ;
  /* T inputImg[AC_NUM_ROWS][IN_WIDTH]; */
  /* T outputImg[AC_NUM_ROWS_OUT][AC_NUM_COLS_OUT]; */
  T inputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH];
  T outputImg[IN_CHANNELS][IN_HEIGHT][IN_WIDTH];
public:
  max_pool_stream() {
    printf("%dx%d max pool stride %d zero pad\n",FILTER_HEIGHT_WIDTH,FILTER_HEIGHT_WIDTH,STRIDE);
    printf("input %3d x %3d\n", IN_HEIGHT, IN_WIDTH);
    printf("expected input %3d x %3d\n", EXP_AC_NUM_ROWS_IN, EXP_AC_NUM_COLS_IN);
    printf("output %3d x %3d\n", OUT_HEIGHT, OUT_WIDTH);
    assert(IN_HEIGHT == EXP_AC_NUM_ROWS_IN);
    assert(IN_WIDTH == EXP_AC_NUM_COLS_IN);
  }
  void filter(ac_channel<T> &inputImgIn, ac_channel<T> &outputImgOut) {

    for (int i=0; i<IN_HEIGHT; i++) {
      for (int j=0 ; j<IN_WIDTH; j++) {
        INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          inputImg[c1][i][j] = inputImgIn.read();
        }
      }
    }
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      T max = 0;
      int row=0;
      int col = 0;
      T window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT- FILTER_HEIGHT_WIDTH +1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-FILTER_HEIGHT_WIDTH+1; j+=STRIDE) {
          max = 0;
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              window[r][c] = inputImg[c1][i+r][j+c];
            }
          }
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              if (window[r][c]>max)
              { max = window[r][c]; }
            }
          }
          outputImg[c1][row][col] = max;
          col++;
        }
        row++;
      }
    }

    for (int i=0; i<OUT_HEIGHT; i++) {
      for (int j=0 ; j<OUT_WIDTH; j++) {
        OUT_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          outputImgOut.write(outputImg[c1][i][j]);
        }
      }
    }
  }

  void filter_axi_stream(ac_channel<axi4VideoStream<T,1> > &inputImgIn, ac_channel<axi4VideoStream<T,1> > &outputImgOut) {

    for (int i=0; i<IN_HEIGHT; i++) {
      for (int j=0 ; j<IN_WIDTH; j++) {
        INP_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          inputImg[c1][i][j] = inputImgIn.read().data;
        }
      }
    }
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      T max = 0;
      int row=0;
      int col = 0;
      T window[FILTER_HEIGHT_WIDTH][FILTER_HEIGHT_WIDTH];
      for (int i=0; i<IN_HEIGHT- FILTER_HEIGHT_WIDTH +1; i+=STRIDE) {
        col = 0;
        for (int j=0 ; j<IN_WIDTH-FILTER_HEIGHT_WIDTH+1; j+=STRIDE) {
          max = 0;
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              window[r][c] = inputImg[c1][i+r][j+c];
            }
          }
          for (int r=0; r<FILTER_HEIGHT_WIDTH; r++) {
            for (int c=0; c<FILTER_HEIGHT_WIDTH; c++) {
              if (window[r][c]>max)
              { max = window[r][c]; }
            }
          }
          outputImg[c1][row][col] = max;
          col++;
        }
        row++;
      }
    }

    for (int i=0; i<OUT_HEIGHT; i++) {
      for (int j=0 ; j<OUT_WIDTH; j++) {
        OUT_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
          axi4VideoStream<T,1> tmp;
          tmp.data = outputImg[c1][i][j];
          outputImgOut.write(tmp);
        }
      }
    }
  }

  void printInput() {
#ifndef __SYNTHESIS__
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<IN_HEIGHT; i++) {
        for (int j=0 ; j<IN_WIDTH; j++) {
          printf("%3d ", (int)inputImg[c1][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
#endif
  }
  void printOutput() {
#ifndef __SYNTHESIS__
    IN_CHAN:for (int c1=0; c1<IN_CHANNELS; c1++) {
      for (int i=0; i<OUT_HEIGHT; i++) {
        for (int j=0 ; j<OUT_WIDTH; j++) {
          printf("%3d ", (int)outputImg[c1][i][j]);
        }
        printf("\n");
      }
      printf("\n");
    }
#endif
  }
};

#endif
