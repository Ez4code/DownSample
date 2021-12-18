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

// 05-26-09 - Mike Fingeroff - Structs that allow doubling of singleport width
// and user specialization to support custom classes in ac_window
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __AC_WINDOW_STRUCTS__
#define __AC_WINDOW_STRUCTS__
#include <ac_int.h>
#include <ac_fixed.h>
#ifndef __SYNHTESIS__
#include <stdio.h>
#endif

template<typename T> struct ac_width2x{
    typedef T data;
    enum { WIDTH = 0};
    static void set_half(bool sel_half, T din, data& dout)
    {
#ifndef __SYNTHESIS__
        printf("ERROR: A user-defined specialization of this struct ac_width2x must be created when using\n");
        printf("user-defined classes and true singleport RAM.  Refer to the ac_window section of the Catapult\n"); 
        printf("users guide\n");
        assert(0);
#endif      
    }
    static void get_half(bool sel_half, data  din, T& dout)
    {
#ifndef __SYNTHESIS__
        printf("ERROR: A user-defined specialization of this struct ac_width2x must be created when using\n");
        printf("user-defined classes and true singleport RAM.  Refer to the ac_window section of the Catapult\n"); 
        printf("users guide\n");
        assert(0);
#endif             
    }
};

//ac_int support
template<int AC_WIDTH, bool AC_SIGN> 
    struct ac_width2x<ac_int<AC_WIDTH,AC_SIGN> >{
        typedef ac_int<AC_WIDTH*2,false> data;
        enum { WIDTH = AC_WIDTH};
        static void set_half(bool sel_half, ac_int<AC_WIDTH,AC_SIGN> din, data& dout)
        {
            int tmp = sel_half?AC_WIDTH:0;
            dout.set_slc(tmp,din);
        }
        static void get_half(bool sel_half, data  din, ac_int<AC_WIDTH,AC_SIGN>& dout)
        {
            dout = sel_half ? din.template slc<AC_WIDTH>(AC_WIDTH):din.template slc<AC_WIDTH>(0);
        }
    };

//ac_fixed support
template<int AC_WIDTH, int AC_INTEGER, bool AC_SIGN> 
    struct ac_width2x<ac_fixed<AC_WIDTH,AC_INTEGER,AC_SIGN> >{
        typedef ac_fixed<AC_WIDTH*2,AC_WIDTH*2,false> data;
        enum { WIDTH = AC_WIDTH};
        static void set_half(bool sel_half, ac_fixed<AC_WIDTH,AC_INTEGER,AC_SIGN> din, data& dout)
        {
            int tmp = sel_half?AC_WIDTH:0;
            ac_int<AC_WIDTH,false> din_tmp = din.template slc<AC_WIDTH>(0);
            dout.template set_slc(tmp,din_tmp);
        }
        static void get_half(bool sel_half, data  din, ac_fixed<AC_WIDTH,AC_INTEGER,AC_SIGN>& dout)
        {
            dout = sel_half ? din.template slc<AC_WIDTH>(AC_WIDTH):din.template slc<AC_WIDTH>(0);
        }
    };

template<> 
struct ac_width2x<int>{
    typedef int data;
    enum { WIDTH = 32};

    static void set_half(bool sel_half, int din, data& dout)
    {

#ifndef __SYNTHESIS__
        printf("ERROR: Only ac_int, ac_fixed, and custom classes that implement set_half and get_half are +supported\n");
        printf("when using true singleport RAM.  Refer to the ac_window section of the Catapult users guide on how to\n");
        printf("create a custom class\n");
        assert(0);
#endif
    }

    static void get_half(bool sel_half, data  din, int& dout){

    }
};

template<> 
struct ac_width2x<unsigned int>{
    typedef unsigned int data;
    enum { WIDTH = 32};

    static void set_half(bool sel_half, unsigned int din, data& dout)
    {

#ifndef __SYNTHESIS__
        printf("ERROR: Only ac_int, ac_fixed, and custom classes that implement set_half and get_half are +supported\n");
        printf("when using true singleport RAM.  Refer to the ac_window section of the Catapult users guide on how to\n");
        printf("create a custom class\n");
        assert(0);
#endif
    }

    static void get_half(bool sel_half, data  din, unsigned int& dout){

    }
};

template<> 
struct ac_width2x<char>{
    typedef char data;
    enum { WIDTH = 32};

    static void set_half(bool sel_half, char din, data& dout)
    {

#ifndef __SYNTHESIS__
        printf("ERROR: Only ac_int, ac_fixed, and custom classes that implement set_half and get_half are +supported\n");
        printf("when using true singleport RAM.  Refer to the ac_window section of the Catapult users guide on how to\n");
        printf("create a custom class\n");
        assert(0);
#endif
    }

    static void get_half(bool sel_half, data  din, char& dout){

    }
};

template<> 
struct ac_width2x<unsigned char>{
    typedef unsigned char data;
    enum { WIDTH = 32};

    static void set_half(bool sel_half, unsigned char din, data& dout)
    {

#ifndef __SYNTHESIS__
        printf("ERROR: Only ac_int, ac_fixed, and custom classes that implement set_half and get_half are +supported\n");
        printf("when using true singleport RAM.  Refer to the ac_window section of the Catapult users guide on how to\n");
        printf("create a custom class\n");
        assert(0);
#endif
    }

    static void get_half(bool sel_half, data  din, unsigned char& dout){

    }
};

#endif
