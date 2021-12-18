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

////////////////////////////////////////////////////////////////////////////////
//
// 05-26-09 - Mike Fingeroff - added singleport RAM support
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __AC_BUFFER_2D_H
#define __AC_BUFFER_2D_H
#include <ac_int.h>
#include <ac_fixed.h>
#include <ac_window_1d_flag.h>
#ifndef AC_WINDOW_CUSTOM
#include <ac_window_structs.h>
#endif

#ifndef __SYNTHESIS__
#include <cassert>
#include <stdio.h>
#endif

template<typename DTYPE, int AC_WMODE, int AC_NCOL, int AC_NROW>
    class ac_linebuf{

    enum {
        logAC_NCOL = ac::log2_ceil< AC_NCOL >::val 
    };

    enum { AC_WIDTH = ac_width2x<DTYPE>::WIDTH};
    enum { AC_WIDTH2 =  ((AC_WMODE&AC_SINGLEPORT) ? 2:1)*AC_WIDTH};  
    enum { AC_NCOL2 = AC_NCOL/((AC_WMODE&AC_SINGLEPORT) ? 2:1)};

    ac_linebuf< DTYPE , AC_WMODE,AC_NCOL,AC_NROW-1> buf;
    typename ac_width2x<DTYPE>::data data[AC_NCOL2];
    ac_int<logAC_NCOL ,false> addr_int; 
    ac_int<1,false> cnt;
    typename ac_width2x<DTYPE>::data tmp_out;
    typename ac_width2x<DTYPE>::data tmp_in;
    DTYPE tmp;

public: 
    ac_linebuf(){
        typename ac_width2x<DTYPE>::data dummy;
        for(int i=0;i<AC_NCOL2;i++)
            data[i] =  dummy;
    }
 void print(){
#ifndef __SYNTHESIS__
        for(int i=0;i<AC_NCOL2;i++)
            printf("%d  ",data[i].to_int());
        printf("\n");
#endif
    }
    void access(DTYPE din[AC_NROW], DTYPE dout[AC_NROW], int addr, bool w[AC_NROW]){
        addr_int = addr;
        if(AC_WMODE&AC_SINGLEPORT){
            if(w[AC_NROW-1]){
                if(cnt==1)
                    ac_width2x<DTYPE>::set_half(1,din[AC_NROW-1],tmp_in);
                else
                    ac_width2x<DTYPE>::set_half(0,din[AC_NROW-1],tmp_in);
            }
  
            if((cnt&1)==0){//read on even
                tmp_out = data[addr_int>>1];
            }
            else{//write on odd
                if(w[AC_NROW-1]){
                    data[addr_int>>1] = tmp_in;
                }
            } 
         
            bool sel = (cnt==1)?1:0;
            ac_width2x<DTYPE>::get_half(sel,tmp_out,dout[AC_NROW-1]);
            cnt++;
        }else{
            dout[AC_NROW-1] = data[addr_int]; 
            if(w[AC_NROW-1]){
                data[addr_int] = din[AC_NROW-1];
            }
        }
        buf.access(din,dout,addr,w);
    }
 
};

template<typename DTYPE, int AC_WMODE, int AC_NCOL>
    class ac_linebuf<DTYPE , AC_WMODE,AC_NCOL,1>{ 

    enum {
        logAC_NCOL = ac::log2_ceil< AC_NCOL >::val 
    };
    
    enum { AC_WIDTH = ac_width2x<DTYPE>::WIDTH};
    enum { AC_WIDTH2 =  ((AC_WMODE&AC_SINGLEPORT) ? 2:1)*AC_WIDTH};  
    enum { AC_NCOL2 = AC_NCOL/((AC_WMODE&AC_SINGLEPORT) ? 2:1)};

    typename ac_width2x<DTYPE>::data data[AC_NCOL2];
    ac_int<logAC_NCOL ,false> addr_int; 
    ac_int<1,false> cnt;
    typename ac_width2x<DTYPE>::data tmp_out;
    typename ac_width2x<DTYPE>::data tmp_in;
    DTYPE tmp;

public: 
    ac_linebuf(){
        typename ac_width2x<DTYPE>::data dummy;
        for(int i=0;i<AC_NCOL2;i++)
            data[i] = dummy;
    }
    void print(){
#ifndef __SYNTHESIS__
        for(int i=0;i<AC_NCOL2;i++)
            printf("%d  ",data[i].to_int());
        printf("\n");
#endif
    }
    void access(DTYPE din[1], DTYPE dout[1], int addr, bool w[1]){
        addr_int = addr;
        if(AC_WMODE&AC_SINGLEPORT){
            if(w[0]){
                if(cnt==1)
                    ac_width2x<DTYPE>::set_half(1,din[0],tmp_in);
                else
                    ac_width2x<DTYPE>::set_half(0,din[0],tmp_in);
            }
            if((cnt&1)==0){//read on even
                tmp_out = data[addr_int>>1];
            }
            else{//write on odd
                if(w[0]){
                    data[addr_int>>1] = tmp_in;
                }
            }
 
            bool sel = (cnt==1)?1:0;
            ac_width2x<DTYPE>::get_half(sel,tmp_out,dout[0]);
            cnt++;
        }else{
            dout[0] = data[addr_int]; 
            if(w[0]){
                data[addr_int] = din[0];
            }
        }
    }
};

template<typename DTYPE, int AC_NCOL, int AC_NROW, int AC_WMODE=AC_DUALPORT>
    class ac_buffer_2d{ 
  
    enum {
        logAC_NROW = ac::log2_ceil< AC_NROW >::val 
    };
    enum {AC_REWIND_VAL = (AC_REWIND&AC_WMODE)?1:0 };
    int cptr;                     //points to current col written
    bool dummy[AC_NROW-1];
    DTYPE wout_[AC_NROW];             // This array is what really gets read
    ac_linebuf< DTYPE, AC_WMODE,AC_NCOL,AC_NROW-1+AC_REWIND_VAL> data; //recursive line buffer class
    ac_int<logAC_NROW,false> sel;
    ac_int<logAC_NROW+1,true> sel1;
    DTYPE b[AC_NROW];
    DTYPE t_tmp[AC_NROW-1+AC_REWIND_VAL]; 
    DTYPE t[AC_NROW];
    bool s[AC_NROW];
    DTYPE data_tmp[AC_NROW];
    public: 
    ac_buffer_2d() : cptr(0), sel(AC_NROW-2+AC_REWIND_VAL), sel1(0) {
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
        for(int i=0;i<AC_NROW;i++){
            wout_[i] = 0;
            b[i] = 0;
            t[i] = 0;
            s[i] = 0;
        }
    }
    DTYPE & operator[]      (int i);
    const DTYPE & operator[](int i) const ;
    void set_dummy(int idx, bool val) { dummy[idx] = val; }
    void set_cptr(int idx) {cptr = idx; }
    void set_wout(int idx, DTYPE val) { wout_[idx] = val; }
    void write(DTYPE src, int i, bool w);

#ifndef __SYNTHESIS__
    void print(){
        DTYPE w[AC_NROW-1+AC_REWIND_VAL];  
        bool ss[AC_NROW-1+AC_REWIND_VAL];
        for(int i=0;i<AC_NROW-1+AC_REWIND_VAL;i++)
            ss[i] = 0;
            
        for(int i=0;i<AC_NROW-1+AC_REWIND_VAL;i++){
            for(int j=0;j<AC_NCOL;j++){
                data.access(w,w,j,ss);
                printf("%d  ", w[i].to_int());
            }
            printf("\n");
        }
        printf("sel = %d  sel1 = %d, cptr = %d\n   DATA %d \n", sel.to_int(), sel1.to_int(), cptr, data_tmp[0].to_int());
    }
#endif

    DTYPE get_wout(int idx) const { return wout_[idx]; } 
};

template<typename DTYPE, int AC_NCOL, int AC_NROW, int AC_WMODE>
    inline  DTYPE & ac_buffer_2d<DTYPE, AC_NCOL,AC_NROW,AC_WMODE>::operator[] (int i)
{
#ifndef __SYNTHESIS__
    assert((i>=0) && (i<AC_NROW));
#endif
    return wout_[AC_NROW-1-i];
}

template<typename DTYPE, int AC_NCOL, int AC_NROW, int AC_WMODE>
    inline  const DTYPE & ac_buffer_2d<DTYPE, AC_NCOL,AC_NROW,AC_WMODE>::operator[](int i) const
{
#ifndef __SYNTHESIS__
    assert((i>=0) && (i<AC_NROW));
#endif
    return wout_[AC_NROW-1-i];
}


template<typename DTYPE, int AC_NCOL, int AC_NROW, int AC_WMODE>
    void ac_buffer_2d<DTYPE,AC_NCOL,AC_NROW,AC_WMODE>::write(DTYPE src,int i, bool w){
#ifndef __SYNTHESIS__
    assert((i>=0) && (i < AC_NCOL));
#endif
    ac_buffer_2d<DTYPE,AC_NCOL,AC_NROW,AC_WMODE>::set_cptr(i);

    if((i==0) & w){
        sel += 1;
    }
    if(sel==AC_NROW-1+AC_REWIND_VAL)
        sel = 0;
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
    for(int j=0;j<AC_NROW-1+AC_REWIND_VAL;j++)
        s[j] = (sel==j) ? w:0;
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
    for(int j=0;j<AC_NROW-1+AC_REWIND_VAL;j++)
        data_tmp[j] = src;
  
    data.access(data_tmp,t_tmp,i,s);
  
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
    for(int j=0;j<AC_NROW-1+AC_REWIND_VAL;j++)
        t[j] = t_tmp[j];

    if(!AC_REWIND_VAL){
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
        for(int j=0;j<AC_NROW-1;j++){
            sel1 = sel-j;
            if(sel1<=0)
                sel1 = sel1 + AC_NROW-1;
            b[sel1] = t_tmp[j];  
        } 
    
        b[0] =  w ? src : t[AC_NROW-1];
    }else{
#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
        for(int j=0;j<AC_NROW;j++){
            if(sel==j)
                t[j] = w ? src : t[j];
        }

#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
        for(int j=0;j<AC_NROW;j++){
            sel1 = sel-j;
            if(sel1<0)
                sel1 = sel1 + AC_NROW; 
            b[sel1] = t[j];  
        } 
    }

#ifdef __SYNTHESIS__
#pragma unroll yes
#endif
    for(int j=0;j<AC_NROW;j++)
        ac_buffer_2d<DTYPE,AC_NCOL,AC_NROW,AC_WMODE>::set_wout(j,b[j]);
}

#endif
