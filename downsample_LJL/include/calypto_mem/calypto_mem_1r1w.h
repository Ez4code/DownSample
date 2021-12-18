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

#ifndef __CALYPTO_MEM_1r1w_H__
#define __CALYPTO_MEM_1r1w_H__
//=========================================================================================
// 1read/1write PORT SYNCHRONOUS MEMORY
//  -- Models cycle accurate behaviour
//  -- No memory hazard / coruption  is modeled
//  -- Models Advanced Power Modes (Light Sleep, Deep Sleep, Shutdown)
//  -- Models bit-level write bitmask (if mask bit is 1 then write is not done on that bit)
//  -- Models write-through behavior
//  -- Models read-write contention
// NOTE:  This is a SystemC wrapper around the Verilog model.
//        Only Questa is supported for the SYN view simulation.
//        TLM view should work in any SystemC simulator.
//==========================================================================================
#include <ccs_types.h>

#pragma map_to_operator calypto_mem_1r1w
template < int AW = 5
         , int DW = 8
         , int NW = 32
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>              // Allows consistent library for structural and array-based RAMs
class calypto_mem_1r1w : public sc_module
{
public:
   sc_out< sc_lv<DW> > q;    // read data (valid on read)
   sc_in< bool >       rclk; // clock positive edge triggered
   sc_in< bool >       rme;  // read port selected when high
   sc_in< sc_lv<AW> >  radr; // read  address
   sc_in< bool >       wclk; // clock positive edge triggered
   sc_in< bool >       wme;  // write port selected when high
   sc_in< sc_lv<AW> >  wadr; // write  address
   sc_in< sc_lv<DW> >  d;    // write data (used on write)
   sc_in< sc_lv<DW> >  wm;   // write mask (if bit == 1 write is masked)
   sc_in< bool >       ls;   // lightsleep
   sc_in< bool >       ds;   // deep sleep
   sc_in< bool >       sd;   // shut down

#if !defined(__SYNTHESIS__)
   sc_signal<sc_lv<DW> >mem[NW];
#endif

   //calypto_mem_1r1w(const sc_module_name &nm, const char* hdl_name)
   //calypto_mem_1r1w(const sc_module_name &nm)
   SC_CTOR(calypto_mem_1r1w) :
      q("q")
      , rclk("rclk")
      , rme("rme")
      , radr("radr")
      , wclk("wclk")
      , wme("wme")
      , wadr("wadr")
      , d("d")
      , wm("wm")
      , ls("ls")
      , ds("ds")
      , sd("sd")
   {

      SC_METHOD(seq_proc_read);
      dont_initialize();
      sensitive << rclk.pos();

      SC_METHOD(seq_proc_write);
      dont_initialize();
      sensitive << wclk.pos();

   }

   void seq_proc_read() {
#if !defined(__SYNTHESIS__)
       int radr_int;
       sc_lv<AW> radr_lv;
       radr_lv = radr.read();

       if (rme.read()) {
           vector_to_type(radr_lv, false, &radr_int);
           if(!READ_BEFORE_WRITE & wme.read() & (radr.read() == wadr.read())) {
               sc_lv<DW > dc;
               q.write((WT == 0) ? (mem[radr_int].read() ^ (dc & ~wm.read())) : d.read());
           } else {
               q.write(mem[radr_int].read());
           }
       } else {              
           if (WT & wme.read()) {
               q.write(d.read());
           }
       }
#endif
   }

   void seq_proc_write() {
#if !defined(__SYNTHESIS__)
       int wadr_int;
       sc_lv<AW> wadr_lv;
       wadr_lv = wadr.read();
       if (wme.read()) {
           vector_to_type(wadr_lv, false, &wadr_int);
           sc_lv<DW > din = (d.read() & ~wm.read()) | (mem[wadr_int].read() & wm.read());
           mem[wadr_int] = din;
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

   ~calypto_mem_1r1w() {}
};

template < int AW = 5
         , int DW = 8
         , int NW = 32
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>              // Allows consistent library for structural and array-based RAMs
class calypto_mem_1r1w_wrapper : public sc_module
{
public:
   sc_out< sc_lv<DW> > q;    // read data (valid on read)
   sc_in< bool >       rclk; // clock positive edge triggered
   sc_in< bool >       rme;  // read port selected when high
   sc_in< sc_lv<AW> >  radr; // read  address
   sc_in< bool >       wclk; // clock positive edge triggered
   sc_in< bool >       wme;  // write port selected when high
   sc_in< sc_lv<AW> >  wadr; // write  address
   sc_in< sc_lv<DW> >  d;    // write data (used on write)
   sc_in< sc_lv<DW> >  wm;   // write mask (if bit == 1 write is masked)

   sc_signal< bool >       ls;   // lightsleep
   sc_signal< bool >       ds;   // deep sleep
   sc_signal< bool >       sd;   // shut down


   calypto_mem_1r1w<AW, DW, NW, WT, ds_sd_zero, UHD_arch, READ_BEFORE_WRITE, LS_DEPTH, ram_id> i_calypto_mem_1r1w;
   SC_CTOR(calypto_mem_1r1w_wrapper) :
       i_calypto_mem_1r1w("i_calypto_mem_1r1w")
      , q("q")
      , rclk("rclk")
      , rme("rme")
      , radr("radr")
      , wclk("wclk")
      , wme("wme")
      , wadr("wadr")
      , d("d")
      , wm("wm")
      , ls("ls")
      , ds("ds")
      , sd("sd")
    {

        i_calypto_mem_1r1w.q(q);
        i_calypto_mem_1r1w.rclk(rclk);
        i_calypto_mem_1r1w.rme(rme);
        i_calypto_mem_1r1w.radr(radr);
        i_calypto_mem_1r1w.wclk(wclk);
        i_calypto_mem_1r1w.wme(wme);
        i_calypto_mem_1r1w.wadr(wadr);
        i_calypto_mem_1r1w.d(d);
        i_calypto_mem_1r1w.wm(wm);
        i_calypto_mem_1r1w.ls(ls);
        i_calypto_mem_1r1w.ds(ds);
        i_calypto_mem_1r1w.sd(sd);

        SC_METHOD(force_constants);
    }

   void force_constants() {
       ls.write(false);
       ds.write(false);
       sd.write(false);
   }

   sc_lv<DW> tb_read( int address ) const {
      return i_calypto_mem_1r1w.tb_read(address);
   }

   void tb_write( int address, sc_lv<DW> data ) {
      i_calypto_mem_1r1w.tb_write(address, data);
   }

};

#endif

