#ifndef _INCLUDED_ccs_sysc_ram_H
#define _INCLUDED_ccs_sysc_ram_H

#include <systemc.h>
#undef for
#include <ac_int.h>
#include <ac_sc.h>
#include <mc_typeconv.h>

#define ccs_concat(n1,n2) (n1 ? ((std::string(n1)+"_"+n2).c_str()) : 0)

typedef enum {TLM = 0, SYN = 1} abstraction_t;
#ifdef __SYNTHESIS__
   const abstraction_t abstraction = SYN;
#else
   const abstraction_t abstraction = TLM;
#endif

// RAM instance, including synthesis and mixed-language simulation settings
#include <ram_sync_separateRW_be_generic.h>
#include <ram_sync_dualRW_be_generic.h>
#include <ram_sync_single_be_generic.h>

namespace mc_typedef_T_traits_private {
  // helper structs for statically computing log2 like functions (nbits, log2_floor, log2_ceil)
  //   using recursive templates
  template<unsigned char N>
  struct s_N {
    template<unsigned X>
    struct s_X {
      enum {
        X2 = X >> N,
        N_div_2 = N >> 1,
        nbits = X ? (X2 ? N + s_N<N_div_2>::template s_X<X2>::nbits : s_N<N_div_2>::template s_X<X>::nbits) : 0
      };
    };
  };
  template<> struct s_N<0> {
    template<unsigned X>
    struct s_X {
      enum {nbits = !!X };
    };
  };
};

// compiler time constant for log2 like functions
template<unsigned X>
struct nbits {
   enum { val = mc_typedef_T_traits_private::s_N<16>::s_X<X>::nbits };
};


// Helper struct for determining bitwidth of types
template <class T> struct mc_typedef_T_traits;

// INT <-> SC_LV
template <>
struct mc_typedef_T_traits< int > { 
   enum { bitwidth = 32,
          issigned = 1
        }; 
};

// UINT <-> SC_LV
template <>
struct mc_typedef_T_traits< unsigned int > { 
   enum { bitwidth = 32,
          issigned = 0
        }; 
};

// SHORT <-> SC_LV
template <>
struct mc_typedef_T_traits< short > { 
   enum { bitwidth = 32,
          issigned = 1
        }; 
};

// USHORT <-> SC_LV
template <>
struct mc_typedef_T_traits< unsigned short > { 
   enum { bitwidth = 32,
          issigned = 0
        }; 
};

// CHAR <-> SC_LV
template <>
struct mc_typedef_T_traits< char > { 
   enum { bitwidth = 32,
          issigned = 1
        }; 
};

// UCHAR <-> SC_LV
template <>
struct mc_typedef_T_traits< unsigned char > { 
   enum { bitwidth = 32,
          issigned = 0
        }; 
};
// LONG <-> SC_LV
template <>
struct mc_typedef_T_traits< long > { 
   enum { bitwidth = 32,
          issigned = 1
        }; 
};

// ULONG <-> SC_LV
template <>
struct mc_typedef_T_traits< unsigned long > { 
   enum { bitwidth = 32,
          issigned = 0
        }; 
};

// BOOL <-> SC_LV
template <>
struct mc_typedef_T_traits< bool > { 
   enum { bitwidth = 1,
          issigned = 0
        }; 
};


// SC_LV <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_lv<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};

// SC_LOGIC <-> SC_LV
template<>
struct mc_typedef_T_traits< sc_logic > { 
   enum { bitwidth = 1,
          issigned = 0
        }; 
};

// SC_UINT <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_uint<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};

// SC_BV <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_bv<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};

// SC_BIT <-> SC_LV
template<>
struct mc_typedef_T_traits< sc_bit > { 
   enum { bitwidth = 1,
          issigned = 0
        }; 
};

// SC_INT <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_int<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 1
        }; 
};

// SC_BIGUINT <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_biguint<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};

// SC_BIGINT <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< sc_bigint<Twidth> > { 
   enum { bitwidth = Twidth,
          issigned = 1
        }; 
};

#if defined(SC_INCLUDE_FX)
// SC_FIXED <-> SC_LV
template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
struct mc_typedef_T_traits< sc_fixed<Twidth,Ibits,Qmode,Omode,Nbits> > { 
   enum { bitwidth = Twidth,
          issigned = 1
        }; 
};
// SC_UFIXED <-> SC_LV
template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
struct mc_typedef_T_traits< sc_ufixed<Twidth,Ibits,Qmode,Omode,Nbits> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};
#endif

// AC_INT (signed) <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< ac_int<Twidth,true> > { 
   enum { bitwidth = Twidth,
          issigned = 1
        }; 
};

// AC_INT (unsigned) <-> SC_LV
template <int Twidth>
struct mc_typedef_T_traits< ac_int<Twidth,false> > { 
   enum { bitwidth = Twidth,
          issigned = 0
        }; 
};

// AC_FIXED (signed) <-> SC_LV
template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
struct mc_typedef_T_traits< ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> > { 
   enum { bitwidth = Twidth,
          issigned = Signed?1:0
        }; 
};


class ccs_ram_RWp {
public:
   // ----------------------------------------------------------------------
   // Base template specification is only for compiling, code is unreachable
   // Both TLM and SYN specializations are defined
   // ----------------------------------------------------------------------
   template <class T, unsigned int size, abstraction_t abstraction>
   class RAM_core {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class RAM_core_1p {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class RAM_core_2p {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class read_RAM_chan {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class write_RAM_chan {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class read_RAM_port {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class write_RAM_port {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class readwrite_RAM_chan {
   };

   template <class T, unsigned int size, abstraction_t abstraction>
   class readwrite_RAM_port {
   };

   // ***********************************************
   // TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM
   //    This code is only for TLM simulation only
   // ***********************************************

   //==================================
   // RAM Core
   //    The shared variable "mem" is not synthesizable
   //==================================

   template <class T, unsigned int size>
   class  RAM_core<T,size,TLM> : public sc_module 
   {
   public:   
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      sc_in<bool>                      clk;

      SC_CTOR(RAM_core) {}

      template <class C>
      void read_port ( C &c ) {
         c.mem_ptr = mem;
      }

      template <class C>
      void write_port ( C &c ) {
         c.mem_ptr = mem;
      }

      template <class C>
      void readwrite_port ( C &c ) {
         c.mem_ptr = mem;
      }

   private:
      T mem[size];   
   };

   template <class T, unsigned int size>
   class  RAM_core_1p<T,size,TLM> : public sc_module 
   {
   public:   
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      sc_in<bool>                      clk;

      SC_CTOR(RAM_core_1p) {}

#ifndef __SYNTHESIS__
      // For non-synthesis, these "ports" allow simplified testbench access
      template <class C>
      void read_port ( C &c ) {
         c.mem_ptr = mem;
      }

      template <class C>
      void write_port ( C &c ) {
         c.mem_ptr = mem;
      }
#endif

      template <class C>
      void readwrite_port ( C &c ) {
         c.mem_ptr = mem;
      }

   private:
      T mem[size];   
   };

   template <class T, unsigned int size>
   class  RAM_core_2p<T,size,TLM> : public sc_module 
   {
   public:   
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      sc_in<bool>                      clk;

      SC_CTOR(RAM_core_2p) {}

      template <class C>
      void readwrite_port_1 ( C &c ) {
         c.mem_ptr = mem;
      }

      template <class C>
      void readwrite_port_2 ( C &c ) {
         c.mem_ptr = mem;
      }

   private:
      T mem[size];   
   };

   //==================================
   // READ/WRITE CHANNELS
   //    These channels are bound to the RAM object
   //==================================

   template <class T, unsigned int size>
   class  read_RAM_chan<T,size,TLM> 
   {
   public:
      read_RAM_chan (const char *name) {}

      T read(unsigned int addr) {
         return mem_ptr[addr];
      }

      void reset() {}
   
      T *mem_ptr;
   };

   template <class T, unsigned int size>
   class  write_RAM_chan<T,size,TLM> 
   {
   public:
      write_RAM_chan (const char *name) {}

      void write(unsigned int addr, T data ) {
         mem_ptr[addr] = data;
      }
   
      void reset() {}

      T *mem_ptr;
   };

   template <class T, unsigned int size>
   class  readwrite_RAM_chan<T,size,TLM> 
   {
   public:
      readwrite_RAM_chan (const char *name) {}

      T access(unsigned int addr, T data, bool wenable) {
         if (wenable)
            mem_ptr[addr] = data;
         return mem_ptr[addr];
      }

      T read(unsigned int addr) {
         return access(addr,0,false);
      }

      void write(unsigned int addr, T data) {
         (void) access(addr,data,true);
      }

      void reset() {}
   
      T *mem_ptr;
   };


   //==================================
   // READ/WRITE PORTS
   //==================================

   template <class T, unsigned int size>
   class  read_RAM_port<T,size,TLM> 
   {
   public:
      read_RAM_port (const char *name) {}

      T read(unsigned int addr) {
         return (bound_to_port? port_ptr->read(addr) : chan_ptr->read(addr));
      }

      bool bound_to_port;

      void reset() {}

      void operator() (read_RAM_chan<T,size,TLM> &c) {
         chan_ptr = &c;
         bound_to_port = false;
      }

      void operator() (read_RAM_port<T,size,TLM> &c) {
         port_ptr = &c;
         bound_to_port = true;
      }

      read_RAM_chan<T,size,TLM> *chan_ptr;
      read_RAM_port<T,size,TLM> *port_ptr;
   };

   template <class T, unsigned int size>
   class  write_RAM_port<T,size,TLM> 
   {
   public:
      write_RAM_port (const char *name) {}

      void write(unsigned int addr, T data ) {
         if ( bound_to_port )
            port_ptr->write(addr,data);
         else
            chan_ptr->write(addr,data);
      }
   
      void reset() {}

      bool bound_to_port;

      void operator() (write_RAM_chan<T,size,TLM> &c) {
         chan_ptr = &c;
         bound_to_port = false;
      }

      void operator() (write_RAM_port<T,size,TLM> &c) {
         port_ptr = &c;
         bound_to_port = true;
      }

      write_RAM_chan<T,size,TLM> *chan_ptr;
      write_RAM_port<T,size,TLM> *port_ptr;
   };

   template <class T, unsigned int size>
   class  readwrite_RAM_port<T,size,TLM> 
   {
   public:
      readwrite_RAM_port (const char *name) {}

      T access(unsigned int addr, T data, bool wenable ) {
         if (wenable) {
            if ( bound_to_port )
               port_ptr->write(addr,data);
            else
               chan_ptr->write(addr,data);
         }
         return (bound_to_port? port_ptr->read(addr) : chan_ptr->read(addr));
      }

      T read(unsigned int addr) {
         return access(addr,0,false);
      }

      void write(unsigned int addr, T data ) {
         (void) access(addr,data,true);
      }

      bool bound_to_port;

      void reset() {}

      void operator() (readwrite_RAM_chan<T,size,TLM> &c) {
         chan_ptr = &c;
         bound_to_port = false;
      }

      void operator() (readwrite_RAM_port<T,size,TLM> &c) {
         port_ptr = &c;
         bound_to_port = true;
      }

      readwrite_RAM_chan<T,size,TLM> *chan_ptr;
      readwrite_RAM_port<T,size,TLM> *port_ptr;
   };

   // ***********************************************
   // SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN
   //    Synthesis code
   // ***********************************************

   //==================================
   // INTERFACES
   //    Local encapsulated port definitions
   //==================================

   template <class T, unsigned int size>
   struct read_RAM_internal_if {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
   
      sc_in<sc_lv<addr_width> >      addr_rd;
      sc_in< bool >                    re;
      sc_out<sc_lv<data_width> >       data_out;

      read_RAM_internal_if ( const char *name):
         addr_rd(ccs_concat(name,"addr_rd")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")) {}

      template <class C>
      void bind(C& c) {
         addr_rd(c.addr_rd);
         re(c.re);
         data_out(c.data_out);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   template <class T, unsigned int size>
   struct write_RAM_internal_if {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
   
      sc_in<sc_lv<addr_width> >      addr_wr;
      sc_in< bool >                    we;
      sc_in<sc_lv<data_width> >        data_in;

      write_RAM_internal_if ( const char *name):
         addr_wr(ccs_concat(name,"addr_wr")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

      template <class C>
      void bind(C& c) {
         addr_wr(c.addr_wr);
         we(c.we);
         data_in(c.data_in);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   template <class T, unsigned int size>
   struct readwrite_RAM_internal_if {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
   
      sc_in<sc_lv<addr_width> >      addr;
      sc_in< bool >                    re;
      sc_out<sc_lv<data_width> >       data_out;
      sc_in< bool >                    we;
      sc_in<sc_lv<data_width> >        data_in;

      readwrite_RAM_internal_if ( const char *name):
         addr(ccs_concat(name,"addr")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

      template <class C>
      void bind(C& c) {
         addr(c.addr);
         re(c.re);
         data_out(c.data_out);
         we(c.we);
         data_in(c.data_in);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   //==================================
   // RAM Core
   //    Encapsulation for RAM
   //    Introduces an extra level of hierarchy,
   //    which can be removed during synthesis, if needed
   //==================================

   template <class T, unsigned int size>
   class  RAM_core<T,size,SYN> : public sc_module 
   {
   public:
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      // Need to connect clock for RAM
      sc_in<bool>                      clk;
      read_RAM_internal_if<T,size>     read_port;
      write_RAM_internal_if<T,size>    write_port;
    
      sc_signal< sc_lv<1> >          ram_we;
      sc_signal< sc_lv<1> >          ram_re;
   
      SC_CTOR(RAM_core) 
         : clk("clk")
         , read_port("read_port")
         , write_port("write_port")
   #ifdef UNIT_TEST
         , RAM("RAM", "ram_sync_separateRW_be")
   #else
         , RAM("RAM")
   #endif
      {
         // Assign constant signals in the constructor
         a_rst.write(sc_dt::Log_0);
         s_rst.write(sc_dt::Log_0);
         en.write(sc_dt::Log_1);

         RAM.data_in(write_port.data_in);
         RAM.we(ram_we); // RAM.we(write_port.we);
         RAM.addr_wr(write_port.addr_wr);

         RAM.re(ram_re); // RAM.re(read_port.re);
         RAM.addr_rd(read_port.addr_rd);
         RAM.data_out(read_port.data_out);

         RAM.clk(clk);
         RAM.a_rst(a_rst);
         RAM.s_rst(s_rst);
         RAM.en(en);
      
         SC_METHOD(update_we_proc);
         sensitive << write_port.we;
         SC_METHOD(update_re_proc);
         sensitive << read_port.re;
      }

      void update_we_proc() { ram_we.write(write_port.we?1:0); }
      void update_re_proc() { ram_re.write(read_port.re?1:0); }
   
   private:
      // Unused library signals, driven to constants
      sc_signal<sc_logic >                 a_rst;
      sc_signal<sc_logic >                 s_rst;
      sc_signal<sc_logic >                 en;

      // RAM instance - sc_module wrapper mapped to Catapult RAM library via #pragma map_to_operator
      ram_sync_separateRW_be <0, // "ram_id" - unused for this flow
                              size, //number of elements in array
                              data_width, //required enum for T that defines number of bits 
                              nbits<size-1>::val, //bitwidth of address line, based on size
                              1, 1, 1, 1, 1, 1, 1, 1, 1 //Active edges for clock, reset, enable and byte enable
                             >         RAM;
   };

#ifdef NOT_YET_IMPLEMENTED
   template <class T, unsigned int size>
   class  RAM_core_1p<T,size,SYN> : public sc_module 
   {
   public:
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      // Need to connect clock for RAM
      sc_in<bool>                      clk;
      readwrite_RAM_internal_if<T,size> readwrite_port;
    
      sc_signal< sc_lv<1> >          ram_we;
      sc_signal< sc_lv<1> >          ram_re;
   
      SC_CTOR(RAM_core_1p) 
         : clk("clk")
         , readwrite_port("readwrite_port")
   #ifdef UNIT_TEST
         , RAM("RAM", "ram_sync_singleport_be")
   #else
         , RAM("RAM")
   #endif
      {
         // Assign constant signals in the constructor
         a_rst.write(0);
         s_rst.write(0);
         en.write(1);

         RAM.data_in(readwrite_port.data_in);
         RAM.we(ram_we); // RAM.we(readwrite_port.we);
         RAM.re(ram_re); // RAM.re(readwrite_port.re);
         RAM.data_out(readwrite_port.data_out);
         RAM.addr(readwrite_port.addr);

         RAM.clk(clk);
         RAM.a_rst(a_rst);
         RAM.s_rst(s_rst);
         RAM.en(en);
      
         SC_METHOD(update_we_proc);
         sensitive << readwrite_port.we;
         SC_METHOD(update_re_proc);
         sensitive << readwrite_port.re;
      }

      void update_we_proc() { ram_we.write(readwrite_port.we?1:0); }
      void update_re_proc() { ram_re.write(readwrite_port.re?1:0); }
   
   private:
      // Unused library signals, driven to constants
      sc_signal<bool >                 a_rst;
      sc_signal<bool >                 s_rst;
      sc_signal<bool >                 en;

      // RAM instance - sc_module wrapper mapped to Catapult RAM library via #pragma map_to_operator
      ram_sync_singleport_be <0, // "ram_id" - unused for this flow
                              size, //number of elements in array
                              data_width, //required enum for T that defines number of bits 
                              nbits<size-1>::val, //bitwidth of address line, based on size
                              1, 1, 1, 1, 1, 1, 1, 1, 1 //Active edges for clock, reset, enable and byte enable
                             >         RAM;
   };

   template <class T, unsigned int size>
   class  RAM_core_2p<T,size,SYN> : public sc_module 
   {
   public:
      enum {addr_width = nbits<size-1>::val,
                      data_width = mc_typedef_T_traits<T>::bitwidth};

      // Need to connect clock for RAM
      sc_in<bool>                      clk;
      readwrite_RAM_internal_if<T,size> readwrite_port_1;
      readwrite_RAM_internal_if<T,size> readwrite_port_2;
    
      sc_signal< sc_lv<1> >          ram_we_1;
      sc_signal< sc_lv<1> >          ram_re_1;
   
      SC_CTOR(RAM_core_2p) 
         : clk("clk")
         , readwrite_port_1("readwrite_port_1")
         , readwrite_port_2("readwrite_port_2")
   #ifdef UNIT_TEST
         , RAM("RAM", "ram_sync_dualport_be")
   #else
         , RAM("RAM")
   #endif
      {
         // Assign constant signals in the constructor
         a_rst.write(0);
         s_rst.write(0);
         en.write(1);

         RAM.data_in(readwrite_port.data_in);
         RAM.we(ram_we); // RAM.we(readwrite_port.we);
         RAM.re(ram_re); // RAM.re(readwrite_port.re);
         RAM.data_out(readwrite_port.data_out);
         RAM.addr(readwrite_port.addr);

         RAM.clk(clk);
         RAM.a_rst(a_rst);
         RAM.s_rst(s_rst);
         RAM.en(en);
      
         SC_METHOD(update_we_proc);
         sensitive << readwrite_port.we;
         SC_METHOD(update_re_proc);
         sensitive << readwrite_port.re;
      }

      void update_we_proc() { ram_we.write(readwrite_port.we?1:0); }
      void update_re_proc() { ram_re.write(readwrite_port.re?1:0); }
   
   private:
      // Unused library signals, driven to constants
      sc_signal<bool >                 a_rst;
      sc_signal<bool >                 s_rst;
      sc_signal<bool >                 en;

      // RAM instance - sc_module wrapper mapped to Catapult RAM library via #pragma map_to_operator
      ram_sync_dualRW_be <0, // "ram_id" - unused for this flow
                          size, //number of elements in array
                          data_width, //required enum for T that defines number of bits 
                          nbits<size-1>::val, //bitwidth of address line, based on size
                          1,  // a_reset_active
                          1, // s_reset_active
                          1, // enable_active
                          1, // re_active
                          1, // we_active
                          1, // num_byte_enables
                          1, // clock_edge
                          1 // no_of_RAM_dualRW_readwrite_port
                          >         RAM;
   };
#endif

   //==================================
   // READ/WRITE CHANNELS
   // External Channels, used to connect to RAM
   // Local encapsulated port definitions
   //==================================

   template <class T, unsigned int size>
   struct read_RAM_chan<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
     
      sc_signal<sc_lv<addr_width> >  addr_rd;
      sc_signal< bool >                re;
      sc_signal<sc_lv<data_width> >    data_out;

      read_RAM_chan ( const char *name):
         addr_rd(ccs_concat(name,"addr_rd")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")) {}

   #pragma REMOVE_OUT_REG
   #pragma mio_delay 1.5
   #pragma design modulario
   #pragma transactor ccs_ram_RWp::read_RAM_trans
      T read(sc_uint<addr_width> address) {
         re.write(1);
         addr_rd.write(address);
         wait();
      #ifdef UNIT_TEST
         // Accounts for lack of register removal in source simulation
         wait();
      #endif
         re.write(0);
         addr_rd.write(0);
         sc_lv<data_width> d_lv = data_out.read();
         T tmp; vector_to_type(d_lv,(signedtype?true:false),&tmp);
         return tmp;
      }  

      void reset() {
         re.write(0);
         addr_rd.write(0);
      }
   };

   template <class T, unsigned int size>
   struct write_RAM_chan<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
    
      sc_signal<sc_lv<addr_width> >     addr_wr;
      sc_signal< bool >           we;
      sc_signal<sc_lv<data_width> >    data_in;

      write_RAM_chan ( const char *name):
         addr_wr(ccs_concat(name,"addr_wr")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

   #pragma REMOVE_OUT_REG
   #pragma design modulario   
   #pragma transactor ccs_ram_RWp::write_RAM_trans
      void write( sc_uint<addr_width> address, T val ) {
         we.write(1);
         addr_wr.write(address);
         sc_lv<data_width> tmp; type_to_vector(val,0,tmp);
         data_in.write(tmp);
         //data_in.write(to_sc(val));
         wait();
         we.write(0);
         addr_wr.write(0);
      }

      void reset() {
         we.write(0);
         addr_wr.write(0);
         data_in.write(0);
      }
   };

   template <class T, unsigned int size>
   struct readwrite_RAM_chan<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
     
      sc_signal<sc_lv<addr_width> >     addr;
      sc_signal< bool >           re;
      sc_signal<sc_lv<data_width> >    data_out;
      sc_signal< bool >           we;
      sc_signal<sc_lv<data_width> >    data_in;

      readwrite_RAM_chan ( const char *name):
         addr(ccs_concat(name,"addr")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

   #pragma REMOVE_OUT_REG
   #pragma design modulario
      T access ( sc_uint<addr_width> address, T val, bool wenable, bool renable) {
         addr.write(address);
         if (wenable) {
            we.write(1);
            data_in.write(to_sc(val));
         }
         if (renable)
      re.write(1);
         wait();
         #ifdef UNIT_TEST
         // Accounts for lack of register removal in source simulation
         wait();
         #endif
         we.write(0);
         addr.write(0);
         re.write(0);
         sc_lv<data_width> d_lv = data_out.read();
         T tmp; vector_to_type(d_lv,(signedtype?true:false),&tmp);
         return tmp;
      }

      // Simple read and write access methods - calls modular IO
      T read(sc_uint<addr_width> address) { return access(address,0,false,true); }
      void write(sc_uint<addr_width> address, T val) { (void)access(address,val,true, false); }
      T readwrite(sc_uint<addr_width> address, T val) { return access(address, val, true, true); }

      void reset() {
         re.write(0);
         addr.write(0);
         we.write(0);
         data_in.write(0);
      }
   };

   // External Channels, used to connect to RAM
   // Local encapsulated port definitions
   template <class T, unsigned int size>
   struct read_RAM_port<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
     
      sc_out<sc_lv<addr_width> >     addr_rd;
      sc_out< bool >                   re;
      sc_in<sc_lv<data_width> >        data_out;

      read_RAM_port ( const char *name):
         addr_rd(ccs_concat(name,"addr_rd")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")) {}

   #pragma REMOVE_OUT_REG
   #pragma mio_delay 2
   #pragma design modulario
   #pragma transactor ccs_ram_RWp::read_RAM_trans
      T read(sc_uint<addr_width> address) {
         re.write(1);
         addr_rd.write(address);
         wait();
      #ifdef UNIT_TEST
         // Accounts for lack of register removal in source simulation
         wait();
      #endif
         re.write(0);
         addr_rd.write(0);
         sc_lv<data_width> d_lv = data_out.read();
         T tmp; vector_to_type(d_lv,(signedtype?true:false),&tmp);
         return tmp;
      }  

      void reset() {
         re.write(0);
         addr_rd.write(0);
      }

      template <class C>
      void bind(C& c) {
         addr_rd(c.addr_rd);
         re(c.re);
         data_out(c.data_out);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   template <class T, unsigned int size>
   struct write_RAM_port<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
    
      sc_out<sc_lv<addr_width> >     addr_wr;
      sc_out< bool >           we;
      sc_out<sc_lv<data_width> >    data_in;

      write_RAM_port ( const char *name):
         addr_wr(ccs_concat(name,"addr_wr")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

   #pragma REMOVE_OUT_REG
   #pragma design modulario   
   #pragma transactor ccs_ram_RWp::write_RAM_trans
      void write( sc_uint<addr_width> address, T val ) {
         we.write(1);
         addr_wr.write(address);
         //data_in.write(to_sc(val)); // convert T to sc_uint
         sc_lv<data_width> tmp; type_to_vector(val,0,tmp);
         data_in.write(tmp);
         wait();
         we.write(0);
         addr_wr.write(0);
      }

      void reset() {
         we.write(0);
         addr_wr.write(0);
         data_in.write(0);
      }

      template <class C>
      void bind(C& c) {
         addr_wr(c.addr_wr);
         we(c.we);
         data_in(c.data_in);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   template <class T, unsigned int size>
   struct readwrite_RAM_port<T,size,SYN> {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
     
      sc_out<sc_lv<addr_width> >     addr;
      sc_out< bool >                   re;
      sc_in<sc_lv<data_width> >        data_out;
      sc_out< bool >                   we;
      sc_out<sc_lv<data_width> >       data_in;

      readwrite_RAM_port ( const char *name):
         addr(ccs_concat(name,"addr")),
         re(ccs_concat(name,"re")),
         data_out(ccs_concat(name,"data_out")),
         we(ccs_concat(name,"we")),
         data_in(ccs_concat(name,"data_in")) {}

   #pragma REMOVE_OUT_REG
   #pragma design modulario   
      T access ( sc_uint<addr_width> address, T val, bool wenable, bool renable) {
         addr.write(address);
         if (wenable) {
            we.write(1);
            data_in.write(to_sc(val));
         }
         if (renable)
      re.write(1);
         wait();
         #ifdef UNIT_TEST
         // Accounts for lack of register removal in source simulation
         wait();
         #endif
         we.write(0);
         addr.write(0);
         re.write(0);
         sc_lv<data_width> d_lv = data_out.read();
         T tmp; vector_to_type(d_lv,(signedtype?true:false),&tmp);
         return tmp;
      }

      // Simple read and write access methods - calls modular IO
      T read(sc_uint<addr_width> address) { return access(address,0,false, true); }
      void write(sc_uint<addr_width> address, T val) { (void)access(address,val,true, false); }
      T readwrite(sc_uint<addr_width> address, T val) { return access(address, val, true, true); }

      void reset() {
         re.write(0);
         addr.write(0);
         we.write(0);
         data_in.write(0);
      }

      template <class C>
      void bind(C& c) {
         addr(c.addr);
         re(c.re);
         data_out(c.data_out);
         we(c.we);
         data_in(c.data_in);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   //==================================
   // TRANSACTORS
   //==================================

   // ----------------------------------------------------------------------
   // TRANSACTOR FOR READ RAM
   // ----------------------------------------------------------------------

   template <class T, unsigned int size>
   class read_RAM_trans : public sc_module
   {
   public:
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
                      data_width = mc_typedef_T_traits<T>::bitwidth};
    
      sc_in<bool>                      clk;
      sc_in<sc_logic>                  rst;
      read_RAM_port<T,size,TLM>        mio_mio_port; // connection from design's input port connected to TB ram
    
      sc_in< sc_lv<addr_width> >       addr_rd; // connection to design's read address port from MIO
      sc_in< sc_logic >                re;  /// connection to design's read enable port from MIO
      sc_out< sc_lv<data_width> >        data_out; // connection to design's ram data input port (data out of ram)

      // Constructor
      SC_HAS_PROCESS(read_RAM_trans);
      read_RAM_trans(
         const sc_module_name& _name, bool clk_neg
      ) : clk("clk")
         , rst("rst")
         , mio_mio_port("mio_mio_port")
         , addr_rd("addr_rd")
         , re("re")
         , data_out("data_out")
         , addr_rd_reg("addr_rd_reg")
         , re_reg("re_reg")
      {
          SC_METHOD(update_proc);
          sensitive << clk;
      }

      void update_proc() {
         addr_rd_reg = addr_rd.read();
         re_reg = (re.read()==SC_LOGIC_1)?1:0;
         if (re_reg.read().to_uint() == 1) {
            T tmp = mio_mio_port.read(addr_rd_reg.read().to_uint()); // TLM read of memory inside testbench
            sc_lv< data_width > data;
            type_to_vector(tmp,0,data);
            data_out.write(data);
         }
      }
      sc_signal< sc_lv<addr_width> > addr_rd_reg;
      sc_signal< sc_lv<1> > re_reg;
   };

   // ----------------------------------------------------------------------
   // TRANSACTOR FOR WRITE RAM
   // ----------------------------------------------------------------------

   template <class T, unsigned int size>
   class write_RAM_trans : public sc_module
   {
   public:
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};
    
       sc_in<bool>                     clk;
       sc_in<sc_logic>                 rst;
       write_RAM_port<T,size,TLM>      mio_mio_port; // connection from design's output port connected to TB ram
    
       sc_in< sc_lv<addr_width> >      addr_wr; // connection to design's write address port from MIO
       sc_in< sc_logic >               we;  /// connection to design's write enable port from MIO
       sc_in< sc_lv<data_width> >        data_in; // connection to design's ram data output port (data into ram)

       // Constructor
       SC_HAS_PROCESS(write_RAM_trans);
       write_RAM_trans(
         const sc_module_name& _name, bool clk_neg
       ) : clk("clk")
         , rst("rst")
         , mio_mio_port("mio_mio_port")
         , addr_wr("addr_wr")
         , we("we")
         , data_in("data_in")
      {
         SC_METHOD(update_proc);
         sensitive << clk;
      }

      void update_proc() {
         if (we.read() == SC_LOGIC_1) {
            T data;
            vector_to_type(data_in.read(),(signedtype?true:false),&data); //vector_to_type(data_in.read(),T::width,&data);
            mio_mio_port.write(addr_wr.read().to_uint(),data);
         }
      }
   };

};

#endif

