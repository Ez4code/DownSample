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

#ifndef __ram_sync_dualRW_be_h
#define __ram_sync_dualRW_be_h

#include <systemc.h>

#pragma map_to_operator RAM_dualRW
template <int ram_id, int words, int width, int addr_width, int a_reset_active, int s_reset_active, int enable_active, int re_active, int we_active, int num_byte_enables, int clock_edge, int no_of_RAM_dualRW_readwrite_port>
SC_MODULE(ram_sync_dualRW_be) {

  //  Ports
  sc_in<sc_lv<no_of_RAM_dualRW_readwrite_port * width> > data_in;
  sc_in<sc_lv<no_of_RAM_dualRW_readwrite_port * addr_width> > addr;
  sc_in<sc_lv<no_of_RAM_dualRW_readwrite_port*num_byte_enables> > re;
  sc_in<sc_lv<no_of_RAM_dualRW_readwrite_port*num_byte_enables> > we;
  sc_out<sc_lv<no_of_RAM_dualRW_readwrite_port * width> > data_out;
  sc_in<bool> clk;
  sc_in<sc_logic> a_rst;
  sc_in<sc_logic> s_rst;
  sc_in<sc_logic> en;

  sc_lv<width> mem[words];
  sc_signal<sc_lv<width> > data_ina;
  sc_signal<sc_lv<width> > data_inb;
  sc_signal<sc_lv<num_byte_enables> > rea;
  sc_signal<sc_lv<num_byte_enables> > reb;
  sc_signal<sc_lv<num_byte_enables> > wea;
  sc_signal<sc_lv<num_byte_enables> > web;
  sc_signal<sc_lv<addr_width> > addra;
  sc_signal<sc_lv<addr_width> > addrb;
  sc_signal<sc_lv<width> > data_outa;
  sc_signal<sc_lv<width> > data_outb;

  const int byte_width;

  sc_logic phase_en;

  //  Constructor 'rtl'
  SC_CTOR(ram_sync_dualRW_be) :
    data_in("data_in"),
    addr("addr"),
    re("re"),
    we("we"),
    data_out("data_out"),
    clk("clk"),
    a_rst("a_rst"),
    s_rst("s_rst"),
    en("en"),
    data_ina("data_ina"),
    data_inb("data_inb"),
    rea("rea"),
    reb("reb"),
    wea("wea"),
    web("web"),
    addra("addra"),
    addrb("addrb"),
    data_outa("data_outa"),
    data_outb("data_outb"),
    byte_width(width / num_byte_enables)
  {
#ifndef __SYNTHESIS__
    for ( int i = 0; i < words; ++i )
      mem[i] = 0;
    phase_en = (enable_active ? SC_LOGIC_1 : SC_LOGIC_0);
    SC_METHOD(seq_proc);
    sensitive << (clock_edge ? clk.pos(): clk.neg());

    SC_METHOD(comb_proc);
    sensitive << data_outa;
    sensitive << data_outb;
    sensitive << addr;
    sensitive << data_in;
    sensitive << re;
	sensitive << we;
#endif
  }

#ifndef __SYNTHESIS__
  int get_addr(const sc_signal<sc_lv<addr_width> >& addr_sig) {
    if (addr_sig.read().is_01()) {
      const int addr = addr_sig.read().to_uint();
      sc_assert(addr >= 0 && addr < words);
      return addr;
    } else {
      return 0;
    }
  }

  void seq_proc() {
    if ( en.read() == phase_en ) {
      const int addr_vala = addra.read().to_uint();
      for (int i = 0; i < num_byte_enables; i++) {
        if (rea.read()[i] == sc_logic(re_active) ) {
          if ( addra.read().is_01() ) {
            if (addr_vala >= words)
              SC_REPORT_FATAL(name(), "addra_in is out of bounds");
            data_outa = mem[get_addr(addra)].range((i + 1)*byte_width-1, i*byte_width);   
          }
          else {
            SC_REPORT_WARNING(name(), "addra_in contains 'X' or 'Z' for write operation");
            data_outa = mem[0];
          }
        }
        if ( wea.read()[i] == sc_logic(we_active) ) {
          sc_lv<width> mem_word_a = mem[get_addr(addra)];
          mem_word_a.range((i+1)*byte_width-1, i*byte_width) = data_ina.read().range((i+1)*byte_width-1, i*byte_width);
          mem[get_addr(addra)] = mem_word_a;
        }
      }
      const int addr_valb = addrb.read().to_uint();
      for (int i = 0; i < num_byte_enables; i++) {
        if (reb.read()[i] == sc_logic(re_active) ) {
          if ( addrb.read().is_01() ) {
            if (addr_valb >= words)
              SC_REPORT_FATAL(name(), "addrb_in is out of bounds");
            data_outb = mem[get_addr(addrb)].range((i + 1)*byte_width-1, i*byte_width);   
          }
          else {
            SC_REPORT_WARNING(name(), "addrb_in contains 'X' or 'Z' for write operation");
            data_outb = mem[0];
          }
        }
        if ( web.read()[i] == sc_logic(we_active) ) {
          sc_lv<width> mem_word_b = mem[get_addr(addrb)];
          mem_word_b.range((i+1)*byte_width-1, i*byte_width) = data_inb.read().range((i+1)*byte_width-1, i*byte_width);
	        mem[get_addr(addrb)] = mem_word_b;
        }
      }
    }
  }

  void comb_proc() {
    data_out  = (data_outa.read(), data_outb.read());
    addra     = addr.read().range((2 * addr_width) - 1, addr_width);
    addrb     = addr.read().range((addr_width - 1), 0);
    data_ina  = data_in.read().range((2 * width) - 1, width);
    data_inb  = data_in.read().range(width - 1, 0);
    wea       = we.read().range(2*num_byte_enables-1, 1*num_byte_enables);
    web       = we.read().range(1*num_byte_enables-1, 0*num_byte_enables);
    rea       = re.read().range(2*num_byte_enables-1, 1*num_byte_enables);
    reb       = re.read().range(1*num_byte_enables-1, 0*num_byte_enables);
  }
#endif
};

template <int Tram_id, int Twords, int Twidth, int Taddr_width, int Ta_reset_active, int Ts_reset_active, int Tenable_active, int Tre_active, int Twe_active, int Tnum_byte_enables, int Tclock_edge, int Tno_of_RAM_dualRW_readwrite_port>
SC_MODULE(ram_sync_dualRW_be_port) {

  //  Ports
  sc_in<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Twidth> > data_in_d;
  sc_in<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Taddr_width> > addr_d;
  sc_in<sc_lv<Tno_of_RAM_dualRW_readwrite_port*Tnum_byte_enables> > re_d;
  sc_in<sc_lv<Tno_of_RAM_dualRW_readwrite_port*Tnum_byte_enables> > we_d;
  sc_out<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Twidth> > data_out_d;
  sc_out<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Twidth> > data_in;
  sc_out<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Taddr_width> > addr;
  sc_out<sc_lv<Tno_of_RAM_dualRW_readwrite_port*Tnum_byte_enables> > re;
  sc_out<sc_lv<Tno_of_RAM_dualRW_readwrite_port*Tnum_byte_enables> > we;
  sc_in<sc_lv<Tno_of_RAM_dualRW_readwrite_port * Twidth> > data_out;
  sc_in<bool> clk;
  sc_in<sc_logic> a_rst;
  sc_in<sc_logic> s_rst;
  sc_in<sc_logic> en;

  //  Constructor 'rtl'
  SC_CTOR(ram_sync_dualRW_be_port) :
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

#endif // __ram_sync_dualRW_be_h
