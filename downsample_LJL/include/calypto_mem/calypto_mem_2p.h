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

#ifndef __CALYPTO_MEM_2p_H__
#define __CALYPTO_MEM_2p_H__
//=========================================================================================
// DUAL PORT SYNCHRONOUS MEMORY
//  -- Models cycle accurate behaviour
//  -- No memory hazard / coruption  is modeled
//  -- Models Advanced Power Modes (Light Sleep, Deep Sleep, Shutdown)
//  -- Models bit-level write bitmask (if mask bit is 1 then write is not done on that bit)
//  -- Models write-through behavior
//  -- Models read-write & write-write contentions
// NOTE:  This is a SystemC wrapper around the Verilog model.
//        Only Questa is supported for the SYN view simulation.
//        TLM view should work in any SystemC simulator.
//==========================================================================================

#include <ccs_types.h>

#pragma map_to_operator calypto_mem_2p
template < int AW = 5
         , int DW = 8
         , int NW = 32
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>              // Allows consistent library for structural and array-based RAMs
class calypto_mem_2p : public sc_module
{
public:
   // Outputs for Port A and B
   sc_out< sc_lv<DW> > qa;    // read data 
   sc_out< sc_lv<DW> > qb;    // read data
   // Port A
   sc_in< bool >       clka; // clock positive edge triggered
   sc_in< bool >       mea;  // memory is off when me == 0
   sc_in< bool >       rwa;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadra; // write  address
   sc_in< sc_lv<AW> >  radra; // read  address
   sc_in< sc_lv<DW> >  da;    // write data (used on write)
   sc_in< sc_lv<DW> >  wma;   // write mask. if write mode and wm[bit]==1, then that bit is not written

   // Port B
   sc_in< bool >       clkb; // clock positive edge triggered
   sc_in< bool >       meb;  // memory is off when me == 0
   sc_in< bool >       rwb;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadrb; // write  address
   sc_in< sc_lv<AW> >  radrb; // read  address
   sc_in< sc_lv<DW> >  db;    // write data (used on write)
   sc_in< sc_lv<DW> >  wmb;   // write mask. if write mode and wm[bit]==1, then that bit is not written

   sc_in< bool >       ls;   // lightsleep
   sc_in< bool >       ds;   // deep sleep
   sc_in< bool >       sd;   // shut down

#if !defined(__SYNTHESIS__)
   sc_signal<sc_lv<DW> >mem[NW];
#endif


   SC_CTOR(calypto_mem_2p) :
      qa("qa")
      , clka("clka")
      , mea("mea")
      , rwa("rwa")
      , wadra("wadra")
      , radra("radra")
      , da("da")
      , wma("wma")

      , qb("qb")
      , clkb("clkb")
      , meb("meb")
      , rwb("rwb")
      , wadrb("wadrb")
      , radrb("radrb")
      , db("db")
      , wmb("wmb")

      , ls("ls")
      , ds("ds")
      , sd("sd")
   {

      SC_METHOD(seq_proc_clkA);
      dont_initialize();
      sensitive << clka.pos();
      SC_METHOD(seq_proc_clkB);
      dont_initialize();
      sensitive << clkb.pos();

   }

   void seq_proc_clkA () {
#if !defined(__SYNTHESIS__)
       int wadra_int, wadrb_int, radra_int;
       sc_lv<AW > wadra_lv = wadra.read();
       sc_lv<AW > wadrb_lv = wadrb.read();

       bool wena = mea.read() & rwa.read();
       bool wenb = meb.read() & rwb.read();
       bool rena = mea.read() & ~rwa.read();
       bool renb = meb.read() & ~rwb.read();

       sc_lv<DW > all_one = (1 << (DW + 1)) - 1;

       sc_lv<DW > dina;
       sc_lv<DW > dinb;
       // Guard "mem[].read()" from cycles where address is don't care
       if (wena) {
           vector_to_type(wadra_lv, false, &wadra_int);
           dina = ((da.read() & ~wma.read()) | (mem[wadra_int].read() & wma.read())) ^ (wma.read() ^ wma.read());
       }
       if (wenb) {
           vector_to_type(wadrb_lv, false, &wadrb_int);
           dinb = ((db.read() & ~wmb.read()) | (mem[wadrb_int].read() & wmb.read())) ^ (wmb.read() ^ wmb.read());
       }
       sc_lv<DW > dc;

       if (wena & wenb & (wadra.read() == wadrb.read()) & (~wma.read().and_reduce()) & (~wmb.read().and_reduce())) {
           sc_lv<DW > Xina = (dc & ~wma.read()) | (mem[wadra_int].read() & wma.read()) ^ (wma.read() ^ wma.read());
           mem[wadra_int] = Xina;
       } else if (wena && (wma.read() != all_one)) {
           mem[wadra_int] = dina;           
       }
 
       if (wena & wenb & (wadra.read() == wadrb.read()) & (~wma.read().and_reduce()) & (~wmb.read().and_reduce())) {
           sc_lv<DW > Xinb = (dc & ~wmb.read()) | (mem[wadrb_int].read() & wmb.read()) ^ (wmb.read() ^ wmb.read());
           mem[wadrb_int] = Xinb;
       } else if (wenb && (wmb.read() != all_one)) {
           mem[wadrb_int] = dinb;           
       }

       sc_lv<AW > radra_lv = radra.read();
       if (rena) {
           vector_to_type(radra_lv, false, &radra_int);
           if (wenb && (wadrb.read() == radra.read()) & (wmb.read() != all_one)) {
               qa.write( (WT == 0) ? (mem[radra_int].read() ^ (dc & ~wmb.read())) : dinb);
           } else {
               qa.write(mem[radra_int]);
           }
       } else if (WT & wena) {
           qa.write(dina);
       }
#endif
   }

   void seq_proc_clkB() {
#if !defined(__SYNTHESIS__)
       int radrb_int, wadra_int, wadrb_int;
       sc_lv<AW > radrb_lv = radrb.read();
       sc_lv<AW > wadra_lv = wadra.read();
       sc_lv<AW > wadrb_lv = wadrb.read();

       sc_lv<DW > all_one = (1 << (DW + 1)) - 1;
       
       bool wena = mea.read() & rwa.read();
       bool wenb = meb.read() & rwb.read();
       bool rena = mea.read() & ~rwa.read();
       bool renb = meb.read() & ~rwb.read();

       sc_lv<DW > dc;

       if (renb) {
           vector_to_type(radrb_lv, false, &radrb_int);
           if(wena && (wadra.read() == radrb.read()) & (wma.read() != all_one)) {
               vector_to_type(wadra_lv, false, &wadra_int);
               sc_lv<DW > dina = ((da.read() & ~wma.read()) | (mem[wadra_int].read() & wma.read())) ^ (wma.read() ^ wma.read());
               qb.write((WT == 0) ? (mem[radrb_int].read() ^ (dc & ~wma.read())) : dina);
           } else {
               qb.write(mem[radrb_int]);
           }
       } else if (WT & wenb) {
           vector_to_type(wadrb_lv, false, &wadrb_int);
           sc_lv<DW > dinb = ((db.read() & ~wmb.read()) | (mem[wadrb_int].read() & wmb.read())) ^ (wmb.read() ^ wmb.read());
           qb.write(dinb);
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

   ~calypto_mem_2p() {}
};

template < int AW = 5
         , int DW = 8
         , int NW = 32
         , int WT = 0
         , int ds_sd_zero = 1
         , int UHD_arch = 0
         , int READ_BEFORE_WRITE = 0 
         , int LS_DEPTH = 1
         , int ram_id = 0>
class calypto_mem_2p_wrapper : public sc_module
{
    public:
   // Outputs for Port A and B
   sc_out< sc_lv<DW> > qa;    // read data 
   sc_out< sc_lv<DW> > qb;    // read data
   // Port A
   sc_in< bool >       clka; // clock positive edge triggered
   sc_in< bool >       mea;  // memory is off when me == 0
   sc_in< bool >       rwa;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadra; // write  address
   sc_in< sc_lv<AW> >  radra; // read  address
   sc_in< sc_lv<DW> >  da;    // write data (used on write)
   sc_in< sc_lv<DW> >  wma;   // write mask. if write mode and wm[bit]==1, then that bit is not written

   // Port B
   sc_in< bool >       clkb; // clock positive edge triggered
   sc_in< bool >       meb;  // memory is off when me == 0
   sc_in< bool >       rwb;  // write happens if  1 else read happens
   sc_in< sc_lv<AW> >  wadrb; // write  address
   sc_in< sc_lv<AW> >  radrb; // read  address
   sc_in< sc_lv<DW> >  db;    // write data (used on write)
   sc_in< sc_lv<DW> >  wmb;   // write mask. if write mode and wm[bit]==1, then that bit is not written

   sc_signal< bool >       ls;   // lightsleep
   sc_signal< bool >       ds;   // deep sleep
   sc_signal< bool >       sd;   // shut down



    SC_CTOR(calypto_mem_2p_wrapper) 
      : i_calypto_mem_2p("i_calypto_mem_2p")
      , qa("qa")
      , clka("clka")
      , mea("mea")
      , rwa("rwa")
      , wadra("wadra")
      , radra("radra")
      , da("da")
      , wma("wma")

      , qb("qb")
      , clkb("clkb")
      , meb("meb")
      , rwb("rwb")
      , wadrb("wadrb")
      , radrb("radrb")
      , db("db")
      , wmb("wmb")

      , ls("ls")
      , ds("ds")
      , sd("sd")
      {

          i_calypto_mem_2p.qa(qa);
          i_calypto_mem_2p.clka(clka);
          i_calypto_mem_2p.mea(mea);
          i_calypto_mem_2p.rwa(rwa);
          i_calypto_mem_2p.wadra(wadra);
          i_calypto_mem_2p.radra(radra);
          i_calypto_mem_2p.da(da);
          i_calypto_mem_2p.wma(wma);
          i_calypto_mem_2p.qb(qb);
          i_calypto_mem_2p.clkb(clkb);
          i_calypto_mem_2p.meb(meb);
          i_calypto_mem_2p.rwb(rwb);
          i_calypto_mem_2p.wadrb(wadrb);
          i_calypto_mem_2p.radrb(radrb);
          i_calypto_mem_2p.db(db);
          i_calypto_mem_2p.wmb(wmb);
          i_calypto_mem_2p.ls(ls);
          i_calypto_mem_2p.sd(sd);
          i_calypto_mem_2p.ds(ds);

          SC_METHOD(force_constants);
      }

    void force_constants() {
        ls.write(false);
        ds.write(false);
        sd.write(false);
    }

   sc_lv<DW> tb_read( int address ) const {
      return i_calypto_mem_2p.tb_read(address);
   }

   void tb_write( int address, sc_lv<DW> data ) {
      i_calypto_mem_2p.tb_write(address, data);
   }


   calypto_mem_2p<AW, DW, NW, WT, ds_sd_zero, UHD_arch, READ_BEFORE_WRITE, LS_DEPTH, ram_id> i_calypto_mem_2p;
};
#endif

