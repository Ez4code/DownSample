#ifndef MODULAR_RAM_SEPERATERW_H
#define MODULAR_RAM_SEPERATERW_H

#include <modular_io.h>
#include <cassert>
template <class A, class D>
class rw_memory {
public:
    sc_signal<A> addr_read;
    sc_signal<D> data_read;
    sc_signal<A> addr_write;
    sc_signal<D> data_write;
};
 
template <class A, class D>
class rw_memory_impl {
public:
    sc_in<A> addr_read;
    sc_out<D> data_read;
    sc_in<A> addr_write;
    sc_in<D> data_write;

    template <class C>
      void bind (C& c){
      addr_read(c.addr_read); 
      data_read(c.data_read);   
      addr_write(c.addr_write); 
      data_write(c.data_write);   
    }

    template <class C>
      void operator() (C& c) {
      bind(c);
    }
};
   


#pragma ungroup
template <int ID, class A, class D>
class ram_wire : public mem_read_if<A, D>, 
                      mem_write_if<A, D> {
public:
  sc_out<A> addr_read;
  sc_in<D> data_read;
  sc_out<A> addr_write;
  sc_out<D> data_write;

  ram_wire (  const char *name) :
    addr_read(ccs_concat(name, "addr_read")), 
    data_write(ccs_concat(name, "data_write")), 
    addr_write(ccs_concat(name, "addr_write")), 
    data_read(ccs_concat(name, "data_read")) {}

  void reset() { };

#pragma design  modulario
    void mem_write(A a, D d) { 
        addr_write.write(a);
        data_write.write(d);   
        wait();
    }

#pragma design  modulario
    D mem_read(A a) { 
        addr_read.write(a);
        wait();
        return data_read.read();
    }

    template <class C>
      void bind (C& c){
      addr_read(c.addr_read); 
      data_read(c.data_read);   
      addr_write(c.addr_write); 
      data_write(c.data_write);   
    }

    template <class C>
      void operator() (C& c) {
      bind(c);
    }
};


#ifdef __SYNTHESIS__
// this name has to be exactly right
#pragma hls_map_to_operator RAM_separateRW
template <int ram_id, int words, int width, int addr_width, int a_reset_active=1, int s_reset_active=1, int enable_active=1, int re_active=1, int we_active=1, int num_byte_enables=1, int clock_edge=1, int no_of_RAM_read_port=1, int no_of_RAM_write_port=1>
SC_MODULE(ram_sync_separateRW_be) {

  //  Ports
  sc_in<sc_uint<width> >               data_in;
  sc_in<sc_uint<addr_width> >          addr_rd;
  sc_in<sc_uint<addr_width> >          addr_wr;
  sc_in<bool>                         re;
  sc_in<bool>                         we;
  sc_out<sc_uint<width> >              data_out;
  sc_in<bool>                         clk;
  sc_in<bool>                         a_rst;
  sc_in<bool>                         s_rst;
  sc_in<bool>                         en;

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
    en("en")
  {

  }
};

#else

template <int ram_id, int words, int width, int addr_width, int a_reset_active=1, int s_reset_active=1, int enable_active=1, int re_active=1, int we_active=1, int num_byte_enables=1, int clock_edge=1, int no_of_RAM_read_port=1, int no_of_RAM_write_port=1>
SC_MODULE(ram_sync_separateRW_be) {

  //  Ports
  sc_in<sc_uint<width> >               data_in;
  sc_in<sc_uint<addr_width> >          addr_rd;
  sc_in<sc_uint<addr_width> >          addr_wr;
  sc_in<bool>                          re;
  sc_in<bool>                          we;
  sc_out<sc_uint<width> >              data_out;
  sc_in<bool>                          clk;
  sc_in<bool>                          a_rst;
  sc_in<bool>                          s_rst;
  sc_in<bool>                          en;

  sc_lv<width>                         mem[words];
  sc_event                             mem_read_trigger;


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
    SC_CTHREAD(seq_proc, (clock_edge ? clk.pos() : clk.neg()));
    dont_initialize();

    SC_METHOD(comb_proc);
    sensitive  << addr_rd << mem_read_trigger;
    dont_initialize();
  }

  void seq_proc() {
    for ( int i = 0; i < words; ++i )
    mem[i] = 0;
    const  sc_logic phase_en(enable_active ? SC_LOGIC_1 : SC_LOGIC_0);
    while(true) {
      wait();
      if ( en.read() == phase_en ) {
        for (int i = 0; i < num_byte_enables; i++) {
          if ( we.read() == sc_logic(we_active) ) {
            int addr = addr_wr.read();
            sc_lv<width> mem_word = mem[addr];
            mem_word.range((i+1)*byte_width-1, i*byte_width) = data_in.read().range((i+1)*byte_width-1, i*byte_width);
            mem[addr] = mem_word;
            mem_read_trigger.notify(SC_ZERO_TIME);
          }
        }
      }
    }
  }

  void comb_proc() {
    if (re.read() == sc_logic(re_active)) {
      int addr = addr_rd.read();
      data_out = mem[addr];
    }
  }
};
#endif


#pragma ungroup
template <int ID, int S, class A, class D, int A_WIDTH=32, int D_WIDTH=32>
SC_MODULE(ram) {
public:
  sc_in_clk clk; 
  sc_in<bool> rst;

  ram_sync_separateRW_be<ID, S, D_WIDTH, A_WIDTH > rtlmem;

  SC_CTOR(ram) :
    clk("clk")  , 
    rst ("rst"), 
    rtlmem("rtlmem")
  {      
       en = true;
       rtlmem.clk(clk);
       rtlmem.s_rst(rst);
       rtlmem.a_rst(rst);
       rtlmem.data_in(data_in);
       rtlmem.addr_rd(addr_rd);
       rtlmem.addr_wr(addr_wr);
       rtlmem.re(re);
       rtlmem.we(we);
       rtlmem.data_out(data_out);
       rtlmem.en(en);
       assert(A_WIDTH >= 0 && A_WIDTH <= 32);
       assert(D_WIDTH >= 0 && D_WIDTH <= 32);
  }

  void reset() { };
  void r_reset() { re.write(0); };
  void w_reset() { we.write(0); };

  D operator[](A a) {
    mem_set_raddr(a);
    return mem_fetch_rdata();
  }

#pragma REMOVE_OUT_REG
#pragma design  modulario   
  void mem_write(A a, D d) {     
    data_in.write(static_cast<unsigned>(d));
    addr_wr.write(static_cast<unsigned>(a));
    we.write(1);
    wait();
    we.write(0);
  }

#pragma REMOVE_OUT_REG
#pragma design  modulario
    D mem_read(A a) {
      addr_rd.write(static_cast<unsigned>(a));
      re.write(1);
      wait();     
      re.write(0);
      unsigned t (data_out.read());
      return static_cast<D>(t);
  }


#pragma REMOVE_OUT_REG
#pragma design  modulario
    void mem_set_raddr(A a) {
      addr_rd.write(static_cast<unsigned>(a));
      re.write(1);
      wait();     
      re.write(0);
    }

#pragma REMOVE_OUT_REG
#pragma design  modulario
    D mem_fetch_rdata() {
      wait();
      unsigned t (data_out.read());
      return static_cast<D>(t);
  }


  private:
  sc_signal<sc_uint<D_WIDTH> >    data_in;
  sc_signal<sc_uint<A_WIDTH> >    addr_rd;
  sc_signal<sc_uint<A_WIDTH> >    addr_wr;
  sc_signal<bool>                re;
  sc_signal<bool>                we;
  sc_signal<sc_uint<D_WIDTH> >    data_out;
  sc_signal<bool>                en;

  };


#endif
