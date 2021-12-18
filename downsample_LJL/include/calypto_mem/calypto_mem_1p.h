//------------------------------------------------------------------------------
// Catapult Synthesis - Sample I/O Port Library
//
// Copyright (c) 2003-2015 Mentor Graphics Corp.
//       All Rights Reserved
//
// This document may be used and distributed without restriction provided that
// this copyright statement is not removed from the file and that any derivative
// work contains this copyright notice.
//
// The design information contained in this file is intended to be an example
// of the functionality which the end user may study in preparation for creating
// their own custom interfaces. This design does not necessarily present a 
// complete implementation of the named protocol or standard.
//
//------------------------------------------------------------------------------

#ifndef __CALYPTO_MEM_1p_H__
#define __CALYPTO_MEM_1p_H__
//=========================================================================================
// SINGLE PORT SYNCHRONOUS MEMORY
//  -- Models cycle accurate behaviour
//  -- No memory hazard / coruption  is modeled
//  -- Models Advanced Power Modes (Light Sleep, Deep Sleep, Shutdown)
//  -- Models bit-level write bitmask (if mask bit is 1 then write is not done on that bit)
//  -- Models write-through behavior
// NOTE:  This is a SystemC wrapper around the Verilog model.
//        Only Questa is supported for the SYN view simulation.
//        TLM view should work in any SystemC simulator.
//==========================================================================================

#include <ccs_types.h>

#pragma map_to_operator calypto_mem_1p
template < int AW = 3
         , int DW = 8
         , int NW = 8
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>              // Allows consistent library for structural and array-based RAMs
class calypto_mem_1p : public sc_module
{
public:
   sc_out< sc_lv<DW> > q;    // read data 
   sc_in< bool >       clk; // clock positive edge triggered
   sc_in< bool >       me;  // memory is off when me == 0
   sc_in< bool >       rw;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadr; // write  address
   sc_in< sc_lv<AW> >  radr; // read  address
   sc_in< sc_lv<DW> >  d;    // write data (used on write)
   sc_in< sc_lv<DW> >  wm;   // write mask. if write mode and wm[bit]==1, then that bit is not written
   sc_in< bool >       ls;   // lightsleep
   sc_in< bool >       ds;   // deep sleep
   sc_in< bool >       sd;   // shut down

#if !defined(__SYNTHESIS__)
   sc_signal<sc_lv<DW> > mem[NW];
#endif

   SC_CTOR(calypto_mem_1p) :
      q("q")
      , clk("clk")
      , me("me")
      , rw("rw")
      , wadr("wadr")
      , radr("radr")
      , d("d")
      , wm("wm")
      , ls("ls")
      , ds("ds")
      , sd("sd")
   {

      SC_METHOD(seq_proc);
      dont_initialize();
      sensitive << clk.pos();

   }

   void seq_proc() {
#if !defined(__SYNTHESIS__)
       sc_lv<AW> addr = rw.read() ? wadr.read() : radr.read();
       bool wen = me & rw;
       bool ren = me & ~rw;
       int addr_int;
       // Avoid many warnings about converting don't care
       if ( wen || ren ) 
         vector_to_type(addr, false, &addr_int);
       if ( wen ) {
           mem[addr_int] = (d.read() & ~wm.read()) | (mem[addr_int].read() & wm.read());
       } else if ( ren ) {
           q = mem[addr_int];
       } 
#endif
   }

#if !defined(__SYNTHESIS__)
   sc_lv<DW> tb_read( int address ) const {
      return mem[address].read();
   }

   void tb_write( int address, sc_lv<DW> data ) {
      mem[address] = data;
   }
#endif

   ~calypto_mem_1p() {}
};


template < int AW = 3
         , int DW = 8
         , int NW = 8
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>              // Allows consistent library for structural and array-based RAMs
class calypto_mem_1p_wrapper : public sc_module
{
public:
   sc_out< sc_lv<DW> > q;    // read data 
   sc_in< bool >       clk; // clock positive edge triggered
   sc_in< bool >       me;  // memory is off when me == 0
   sc_in< bool >       rw;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadr; // write  address
   sc_in< sc_lv<AW> >  radr; // read  address
   sc_in< sc_lv<DW> >  d;    // write data (used on write)
   sc_in< sc_lv<DW> >  wm;   // write mask. if write mode and wm[bit]==1, then that bit is not written

   sc_signal< bool >       ls;   // lightsleep
   sc_signal< bool >       ds;   // deep sleep
   sc_signal< bool >       sd;   // shut down

   calypto_mem_1p<AW, DW, NW, WT, ds_sd_zero, UHD_arch, READ_BEFORE_WRITE, LS_DEPTH, ram_id> i_calypto_mem_1p;

   SC_CTOR(calypto_mem_1p_wrapper) :
       i_calypto_mem_1p("i_calypto_mem_1p")
      , q("q")
      , clk("clk")
      , me("me")
      , rw("rw")
      , wadr("wadr")
      , radr("radr")
      , d("d")
      , wm("wm")
      , ls("ls")
      , ds("ds")
      , sd("sd")
    {
        i_calypto_mem_1p.q(q);
        i_calypto_mem_1p.clk(clk);
        i_calypto_mem_1p.me(me);
        i_calypto_mem_1p.rw(rw);
        i_calypto_mem_1p.wadr(wadr);
        i_calypto_mem_1p.radr(radr);
        i_calypto_mem_1p.d(d);
        i_calypto_mem_1p.wm(wm);
        i_calypto_mem_1p.ls(ls);
        i_calypto_mem_1p.ds(ds);
        i_calypto_mem_1p.sd(sd);

        SC_METHOD(force_constants);
    }

   void force_constants() {
       ls.write(false);
       sd.write(false);
       ds.write(false);
   }

   sc_lv<DW> tb_read( int address ) const {
      return i_calypto_mem_1p.tb_read(address);
   }

   void tb_write( int address, sc_lv<DW> data ) {
      i_calypto_mem_1p.tb_write(address, data);
   }

};

#endif

