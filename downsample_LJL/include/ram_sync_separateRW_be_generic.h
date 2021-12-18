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

#ifndef __ram_sync_separateRW_be_h
#define __ram_sync_separateRW_be_h

#include <systemc.h>

#pragma map_to_operator RAM_separateRW
template <int ram_id, int words, int width, int addr_width, int a_reset_active, int s_reset_active, int enable_active, int re_active, int we_active, int num_byte_enables, int clock_edge, int no_of_RAM_read_port, int no_of_RAM_write_port>
SC_MODULE(ram_sync_separateRW_be) {

  //  Ports
  sc_in<sc_lv<width> > data_in;
  sc_in<sc_lv<addr_width> > addr_rd;
  sc_in<sc_lv<addr_width> > addr_wr;
  sc_in<sc_lv<num_byte_enables> > re;
  sc_in<sc_lv<num_byte_enables> > we;
  sc_out<sc_lv<width> >     data_out;
  sc_in<bool>            clk;
  sc_in<sc_logic>            a_rst;
  sc_in<sc_logic>            s_rst;
  sc_in<sc_logic>            en;

  sc_lv<width>                  mem[words];
  sc_logic phase_en;

  const int byte_width;

  //  Constructor 'rtl'
  SC_CTOR(ram_sync_separateRW_be) :
    data_in("data_in"),
    addr_rd("addr_rd"),
    addr_wr("addr_wr"),
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
    phase_en = (enable_active ? sc_dt::Log_1 : sc_dt::Log_0);
    SC_METHOD(seq_proc);
    sensitive << (clock_edge ? clk.pos() : clk.neg());
#endif
  }

#ifndef __SYNTHESIS__
  template <int Bound, class T>
  static int conv_int(const T& addr) {
    if (addr.read().is_01()) {
      const int val = addr.read().to_uint();
      if (val < Bound) {
        return val;
      } else {
        SC_REPORT_FATAL(addr.name(),"out of bounds");
        return 0; // just to silence the compiler
      }
    } else {
      SC_REPORT_WARNING(addr.name(),"contains 'X' or 'Z' -- converting to 0");
      return 0;
    }
  }

  void seq_proc() {
    if ( en.read() == phase_en ) {
      for (int i = 0; i < num_byte_enables; i++) {
        data_out = mem[conv_int<words>(addr_rd)];
        if ( we.read()[i] == sc_logic(we_active) ) {
          sc_lv<width> mem_word = mem[conv_int<words>(addr_wr)];
          mem_word.range((i+1)*byte_width-1, i*byte_width) = data_in.read().range((i+1)*byte_width-1, i*byte_width);
       	  mem[conv_int<words>(addr_wr)] = mem_word;
        }
      }
    }
  }
#endif

};
#endif // __ram_sync_dualRW_be_h
