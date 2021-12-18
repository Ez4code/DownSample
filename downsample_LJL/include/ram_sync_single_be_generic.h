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

#ifndef __ram_sync_single_be_h
#define __ram_sync_single_be_h

#include <systemc.h>

#pragma map_to_operator RAM_single
template <int ram_id, int words, int width, int addr_width, int a_reset_active, int s_reset_active, int enable_active, int re_active, int we_active, int num_byte_enables, int clock_edge, int no_of_RAM_readwrite_port>
SC_MODULE(ram_sync_single_be) {

  //  Ports
  sc_in<sc_lv<width> > data_in;
  sc_in<sc_lv<addr_width> > addr;
  sc_in<sc_lv<num_byte_enables> > re;
  sc_in<sc_lv<num_byte_enables> > we;
  sc_out<sc_lv<width> > data_out;
  sc_in<bool> clk;
  sc_in<sc_logic> a_rst;
  sc_in<sc_logic> s_rst;
  sc_in<sc_logic> en;

  sc_lv<width>                  mem[words];

  sc_logic phase_en;

  const int byte_width;

  //  Constructor 'rtl'
  SC_CTOR(ram_sync_single_be) :
    data_in("data_in"),
    addr("addr"),
    re("re"),
    we("we"),
    data_out("data_out"),
    clk("clk"),
    a_rst("a_rst"),
    s_rst("s_rst"),
    en("en"),
    byte_width(width / num_byte_enables)
  {
#ifndef __SYNTHESIS__
    for ( int i = 0; i < words; ++i )
      mem[i] = 0;
    phase_en = (enable_active ? SC_LOGIC_1 : SC_LOGIC_0);
    SC_METHOD(seq_proc);
    sensitive << (clock_edge ? clk.pos() : clk.neg());
    dont_initialize();
#endif
  }

#ifndef __SYNTHESIS__
  bool is_valid_addr() {
    return addr.read().is_01() && addr.read().to_uint() < words;
  }


  void seq_proc() {
    if ( en.read() == phase_en ) {
      const unsigned addr_val = addr.read().to_uint();
      for (int i = 0; i < num_byte_enables; i++) {
        if (re.read()[i] == sc_logic(re_active) ) {
          if ( addr.read().is_01() ) {
            if (addr_val >= words)
              SC_REPORT_FATAL(name(), "addr_in is out of bounds");
            data_out = mem[addr_val].range((i + 1)*byte_width-1, i*byte_width);   
          }
          else {
            SC_REPORT_WARNING(name(), "addr_in contains 'X' or 'Z' -- converting to 0");
            data_out = mem[0];
          }
        }
        if ( we.read()[i] == sc_logic(we_active) ) {
          if ( addr.read().is_01() ) {
            if (addr_val >= words)
              SC_REPORT_FATAL(name(), "addr_in is out of bounds");
            sc_lv<width> mem_word = mem[addr_val];
            mem_word.range((i+1)*byte_width-1, i*byte_width) = data_in.read().range((i+1)*byte_width-1, i*byte_width);
            mem[addr_val] = mem_word;
          } else {
            SC_REPORT_WARNING(name(), "addr_in contains 'X' or 'Z' for write operation");
            mem[0] = data_in;
          }
        }
      }
    }
  }
#endif
};

template <int Tram_id, int Twords, int Twidth, int Taddr_width, int Ta_reset_active, int Ts_reset_active, int Tenable_active, int Tre_active, int Twe_active, int Tnum_byte_enables, int Tclock_edge, int Tno_of_RAM_readwrite_port>
SC_MODULE(ram_sync_single_be_port) {

  //  Ports
  sc_in<sc_lv<Twidth> > data_in_d;
  sc_in<sc_lv<Taddr_width> > addr_d;
  sc_in<sc_lv<Tnum_byte_enables> > re_d;
  sc_in<sc_lv<Tnum_byte_enables> > we_d;
  sc_out<sc_lv<Twidth> > data_out_d;
  sc_out<sc_lv<Twidth> > data_in;
  sc_out<sc_lv<Taddr_width> > addr;
  sc_out<sc_lv<Tnum_byte_enables> > re;
  sc_out<sc_lv<Tnum_byte_enables> > we;
  sc_in<sc_lv<Twidth> > data_out;
  sc_in<bool> clk;
  sc_in<sc_logic> a_rst;
  sc_in<sc_logic> s_rst;
  sc_in<sc_logic> en;

  //  Constructor 'rtl'
  SC_CTOR(ram_sync_single_be_port) :
    data_in_d("data_in_d"),
    addr_d("addr_d"),
    re_d("re_d"),
    we_d("we_d"),
    data_out_d("data_out_d"),
    data_in("data_in"),
    addr("addr"),
    re("re"),
    we("we"),
    data_out("data_out"),
    clk("clk"),
    a_rst("a_rst"),
    s_rst("s_rst"),
    en("en")
  {
    data_in(data_in_d);
    addr(addr_d);
    re(re_d);
    we(we_d);
    data_out_d(data_out);
  }

};

#endif // __ram_sync_single_be_h

