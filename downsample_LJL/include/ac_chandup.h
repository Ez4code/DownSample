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

#ifndef __AC_CHANDUP_H
#define __AC_CHANDUP_H

#include <ac_channel.h>

template <typename T, int Tsize>
class ac_chandup {
    ac_channel<T> out;
    ac_channel<T> in[Tsize];
public:

    // constructors, same as ac_channel
    ac_chandup() {}
    ac_chandup(int init) : out(init) {}
    ac_chandup(int init, T val) : out(init, val) {}

    ac_channel<T> &output() { return this->out; }

    template <int Tidx>
    ac_channel<T> &input() {
        //assert( Tidx < Tsize );
        return this->in[Tidx];
    }

    #pragma hls_design
    void duplicate() {
        while ( out.available(1) ) {
            T data = out.read();
            #pragma hls_unroll yes
            for ( int i = 0; i < Tsize; ++i )
                in[i].write(data);
        }
    }
};

#endif // __AC_CHANDUP_H
