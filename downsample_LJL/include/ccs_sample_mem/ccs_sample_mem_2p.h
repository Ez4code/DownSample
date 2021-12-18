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

#if !defined(__CCS_SAMPLE_MEM_2P_H)
#define __CCS_SAMPLE_MEM_2P_H
#include <calypto_mem/calypto_mem_2p.h>

//----------------------------------------------------------------------------
// Interconnect: mem_2p
//   Structural shared memory.  Please refer to calypto_mem_2p.h for descriptions
//   of the pins on the memory model.
//   Supported Methods:
//     rw_port:
//        readwrite(bool write, unsigned int addr, T data, ac_int<size T> mask=0):
//            Reads and optionally writes to the memory, applying the mask
//        write(unsigned int addr, T data, unsigned int wr_mask):  
//            Writes data into memory across the write port. wr_mask is a bitmask for the writes.
//            Fixed timing, non-blocking.
//        [unsigned int addr]= operator: Writes data without ability to use bit mask.
//        T read(unsigned int addr):  Reads data from port.  Fixed timing, non-blocking
//        [unsigned int addr] operator:  Reads data
//     Mem: provides rw ports A and B
//       Access methods are similar to rw_port, except they are on A or B ports.
//         A.readwrite, B.readwrite
//         A.write, B.write
//         A.read, B.read
//         A[], B[]  
//         NOTE:  Bracket operators are only available on A and B, not on Mem
//   Most common declaration examples, which result in transaction level simulation:
//   NOTE: Second template argument to channel is the size of the memory.  Min size is 2.
//     mem_2p<>::mem<data_T, 128> my_RAM;
//     mem_2p<>::rw_port<data_T, 128> my_RW_port;
//   To declare pin-accurate interconnects use SYN template parameter:
//     mem_2p<SYN>::mem<data_T, 128> my_RAM;
//     mem_2p<SYN>::rw_port<data_T, 128> my_RW_port;
//     NOTE:  When binding a rw_port to a mem, you must use A or B ( dut.my_RW_port(my_RAM.A) )

// Base Template
template <abstraction_t source_abstraction = AUTO>
class mem_2p {

public:

// Base Template Definition, override defaults for synthesis
// Do not overried setting on impl_abstraction template parameter!

// For synthesis, always use SYN view
#if defined(__SYNTHESIS__)
   template <class T, unsigned int size, abstraction_t impl_abstraction = SYN> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = SYN> class rw_port{};

// For SCVerify, default to SYN unless hard-coded to TLM for testbench only channels
#elif defined (CCS_DUT_CYCLE) || defined (CCS_DUT_RTL)
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? SYN : source_abstraction> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? SYN : source_abstraction> class rw_port{};

// For source simulation, default to TLM unless hard-coded to SYN
#else
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? P2P_DEFAULT_VIEW : source_abstraction> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? P2P_DEFAULT_VIEW : source_abstraction> class rw_port{};

#endif

   // ***********************************************
   // TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM
   //    This code is only for TLM simulation only
   // ***********************************************
private:
      template <class T, unsigned int size>
      class mem_port
      {
        enum {addr_width = nbits<size-1>::val,
              signedtype = mc_typedef_T_traits<T>::issigned,
              data_width = mc_typedef_T_traits<T>::bitwidth};
        p2p_checker rw_chk, clk_chk;

      public:
         mem_port(mem<T,size,TLM> &my_parent, const char *name)
            : rw_chk(name, "call reset()", "access this memory")
            , clk_chk(name, "bind the clock port", "access this memory")
            , parent(my_parent) 
            {}

        void reset() {rw_chk.ok();}
        void clk_ok() {clk_chk.ok();}
     
        T &operator [] (int index) { 
          rw_chk.test();
          clk_chk.test();
          return parent.array[index]; 
        }

        const T &operator [] (int index) const { 
          rw_chk.test();
          clk_chk.test();
          return parent.array[index]; 
        }

        T readwrite (bool en_write, ac_int<addr_width,false> addr, T data=0, ac_int<data_width> mask = 0) {
          rw_chk.test();
          clk_chk.test();
          parent.readwrite(en_write, addr, data, mask);
        }

        void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
          rw_chk.test();
          clk_chk.test();
          parent.readwrite(true,addr,data,mask);
        }

        T read ( ac_int<addr_width,false> addr ) { 
          rw_chk.test();
          clk_chk.test();
          return parent.readwrite(false,addr); 
        }

        private:
           mem<T,size,TLM> &parent;
      };

public:
   //==================================
   // RAM Core
   //    The shared variable "array" is not synthesizable
   //==================================

   template <class T, unsigned int size>
   class mem<T, size, TLM>
   {

      friend class mem_port<T,size>;

      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};

      T readwrite (bool en_write, ac_int<addr_width,false> addr, T data=0, ac_int<data_width> mask = 0) {
        T temp = array[addr]; // Assumes read-first RAM
        if (en_write ) {
          if ( mask != 0 )  { // Check if any mask bits are not set, mask must be signed for this to work
             // NOTE:  Inefficient.  Currently uses sc_lv as underlying type (could be sc_bv or ac type)
             sc_lv<data_width> read_vec, data_vec;
             type_to_vector (array[addr], (signedtype?true:false), read_vec);
             type_to_vector (data, (signedtype?true:false), data_vec);
             for ( int i = 0; i < data_width; i++ ) {
                if (!mask[i]) read_vec[i] = data_vec[i];
             }
             vector_to_type(read_vec, (signedtype?true:false),&(array[addr]));
          } else {
             array[addr] = data;
          }
        }
        return temp;
      }

      void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
        readwrite(true,addr,data,mask);
      }

      T read ( ac_int<addr_width,false> addr ) { return readwrite(false,addr); }

      T array[size];   

   public:   

      mem(const char* name)
        : A(*this,ccs_concat(name,"A"))
        , B(*this,ccs_concat(name,"B")) 
        {} 

      mem_port<T,size> A,B;

      // Empty functions for clock binding (Not needed for TLM)
      void clkA (sc_in<bool> &clock) {A.clk_ok();}
      void clkA (sc_signal<bool> &clock) {A.clk_ok();}
      void clkB (sc_in<bool> &clock) {B.clk_ok();}
      void clkB (sc_signal<bool> &clock) {B.clk_ok();}

      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      T tb_read(int addr) const {
        return array[addr];
      }

      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      void tb_write( int addr, T data) {
        array[addr] = data;
      }

   };

   //==================================
   // READ/WRITE PORT
   //==================================

   template <class T, unsigned int size>
   class  rw_port<T, size, TLM> 
   {
     enum {addr_width = nbits<size-1>::val,
           signedtype = mc_typedef_T_traits<T>::issigned,
           data_width = mc_typedef_T_traits<T>::bitwidth};
     p2p_checker rw_chk;

   public:
      rw_port (const char *name) 
        : rw_chk(name, "call reset()", "access this port")
        {}

      T readwrite(bool write, unsigned int addr, T data, ac_int<data_width> mask = 0 ) {
         rw_chk.test();
         if ( bound_to_port )
            port_ptr->readwrite(write,addr,data,mask);
         else
            chan_ptr->readwrite(write,addr,data,mask);
      }

      T read(unsigned int addr) {
         rw_chk.test();
         return (bound_to_port? port_ptr->read(addr) : chan_ptr->read(addr));
      }

      T &operator [] (int index) {
         rw_chk.test();
         return (bound_to_port? (*port_ptr)[index] : (*chan_ptr)[index]);
      }

      void write(unsigned int addr, T data, ac_int<data_width> mask = 0 ) {
         rw_chk.test();
         if ( bound_to_port )
            port_ptr->write(addr,data,mask);
         else
            chan_ptr->write(addr,data,mask);
      }

      bool bound_to_port;

      void reset() {rw_chk.ok();}

      void operator() (mem_port<T,size> &c) {
         chan_ptr = &c;
         c.reset(); // Supress reset error on bound memory
         bound_to_port = false;
      }

      void operator() (rw_port<T,size,TLM> &c) {
         port_ptr = &c;
         c.reset(); // Supress reset error on bound port
         bound_to_port = true;
      }

      mem_port<T,size> *chan_ptr;
      rw_port<T,size,TLM> *port_ptr;
   };

   // ***********************************************
   // SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN
   //    Synthesis code
   // ***********************************************

   //==================================
   // RAM Core
   //    Encapsulation for RAM
   //==================================

   // Helper classes for each RAM port (A and B);
   // Two different classes are needed to allow signals
   // to be declared in the scope of the mem class

   private:
     template <class T, unsigned int size>
     class mem_port_synth
     {
       enum {addr_width = nbits<size-1>::val,
             signedtype = mc_typedef_T_traits<T>::issigned,
             data_width = mc_typedef_T_traits<T>::bitwidth};
        p2p_checker rw_chk, clk_chk;

       //---------------------------------------------------
       // Helper class for overloading [] operator
       class mem_ref_synth
       {
       public:
          mem_ref_synth( mem_port_synth<T,size> &my_parent, ac_int<addr_width,false> addr)
             : address(addr), parent(my_parent) {}
    
          operator T() const { return parent.read(address); }
    
          inline mem_ref_synth operator = ( T val ) {
             parent.write(address,val);
             return *this;
          }
    
          inline mem_ref_synth operator = ( const mem_ref_synth &val ) { return operator = ((T)val); }
    
       private:
          ac_int<addr_width,false> address;
          mem_port_synth<T,size> &parent;
       };
      
     public:
       mem_port_synth(const char *name)
          : rw_chk(name, "call reset()", "access this memory")
          , clk_chk(name, "bind the clock port", "access this memory")
          , q(ccs_concat(name,"q"))
          , me(ccs_concat(name,"me"))
          , rw(ccs_concat(name,"rw"))
          , wadr(ccs_concat(name,"wadr"))
          , radr(ccs_concat(name,"radr"))
          , d(ccs_concat(name,"d"))
          , wm(ccs_concat(name,"wm")) 
          {}

       mem_ref_synth operator [] (int index) {
          mem_ref_synth write_ref(*this, index);
          return write_ref;
       }

       #pragma mio_delay 0.3
       #pragma remove_out_reg
       #pragma design modulario
       T readwrite (bool en_write, ac_int<addr_width,false> addr, T data=0, ac_int<data_width> mask = 0) {
          rw_chk.test();
          clk_chk.test();
          sc_lv<data_width> temp_data;
          type_to_vector(data, (signedtype?true:false), temp_data);

          sc_lv<addr_width> temp_addr;
          type_to_vector(addr, (signedtype?true:false), temp_addr);

          sc_lv<data_width> temp_mask;
          type_to_vector(mask, (signedtype?true:false), temp_mask);

          me.write(true);
          rw.write(en_write);
          wadr.write(temp_addr);
          radr.write(temp_addr);
          d.write(temp_data);
          wm.write(temp_mask);
          #pragma unroll
          for (ac_int<4,false> lcnt=0; lcnt<1; lcnt++) wait();
#if defined(CALYPTO_SC)
         wait(); // Account for unreg outputs for SLEC
#elif !defined(__SYNTHESIS__)
         wait(0.3, SC_NS);  // Account for unreg outputs in simulation
#endif
          me.write(false);
          rw.write(false);
#if !defined(CALYPTO_SC)
          // Return outputs to don't care to save area
          sc_lv<data_width> dc;
          d.write(dc);
          wadr.write(dc);
          radr.write(dc);
          wm.write(dc);
#endif

          T return_data;
          vector_to_type(q.read(), (signedtype?true:false), &return_data);
          return return_data;
       }

       void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
         readwrite(true,addr,data,mask);
       }

       T read ( ac_int<addr_width,false> addr ) {
         return readwrite(false,addr);
       }

       void reset () {
         me.write(false);
         rw.write(false);
         wadr.write(sc_dt::Log_0);
         radr.write(sc_dt::Log_0);
         d.write(sc_dt::Log_0);
         wm.write(sc_dt::Log_0);
         rw_chk.ok();
       }

       void clk_ok () {clk_chk.ok();}

       // Signal reference to simplify binding or signal access
       sc_signal< sc_lv<data_width> >    q;    // read data (valid on read)
       sc_signal< bool >                 me;  // memory is off when me == 0
       sc_signal< bool >                 rw;  // write happens if  1 else read happens
       sc_signal< sc_lv<addr_width> >    wadr; // write  address
       sc_signal< sc_lv<addr_width> >    radr; // read  address
       sc_signal< sc_lv<data_width> >    d;    // write data
       sc_signal< sc_lv<data_width> >    wm;   // write mask (if bit == 1 write is masked)

     };

public:
   template <class T, unsigned int size>
   class  mem<T,size,SYN>
   {
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};

   public:

      mem ( const char *name)
         : RAM(ccs_concat(name, "RAM"))
         , A(ccs_concat(name,"A"))
         , B(ccs_concat(name,"B"))
      {
         RAM.qa(A.q);
         RAM.qb(B.q);
         RAM.mea(A.me);
         RAM.rwa(A.rw);
         RAM.wadra(A.wadr);
         RAM.radra(A.radr);
         RAM.da(A.d);
         RAM.wma(A.wm);
         RAM.meb(B.me);
         RAM.rwb(B.rw);
         RAM.wadrb(B.wadr);
         RAM.radrb(B.radr);
         RAM.db(B.d);
         RAM.wmb(B.wm);
      }
 
      mem_port_synth<T,size> A,B; 

      void clkA (sc_in<bool> &clock) { A.clk_ok(); RAM.clka(clock); }
      void clkA (sc_signal<bool> &clock) { A.clk_ok(); RAM.clka(clock); }
      void clkB (sc_in<bool> &clock) { B.clk_ok(); RAM.clkb(clock); }
      void clkB (sc_signal<bool> &clock) { B.clk_ok(); RAM.clkb(clock); }

      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      T tb_read(int address) const {
        T return_data;
        vector_to_type( RAM.tb_read(address) , (signedtype?true:false), &return_data);
        return return_data;
      }

      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      void tb_write( int address, T val) {
         sc_lv<data_width> temp_data;
         type_to_vector(val, (signedtype?true:false), temp_data);
         RAM.tb_write(address, temp_data);
      }

   private:
      calypto_mem_2p_wrapper<addr_width, data_width, size> RAM;
   };

   // External Channels, used to connect to RAM
   // Local encapsulated port definitions
   template <class T, unsigned int size>
   class rw_port<T,size,SYN> 
   {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};
      p2p_checker rw_chk;
     
      //---------------------------------------------------
      // Helper class for overloading [] operator
      class mem_ref_synth 
      {
      public:
         mem_ref_synth( rw_port<T,size,SYN> &my_parent, ac_int<addr_width,false> addr) 
            : address(addr), parent(my_parent) {}

         operator T() const { return parent.read(address); }

         inline mem_ref_synth operator = ( const mem_ref_synth &val ) { return operator = ((T)val); }

         inline mem_ref_synth operator = ( T val ) {
            parent.write(address,val);
            return *this;
         }

      private:
         ac_int<addr_width,false> address;
         rw_port<T,size,SYN> &parent;
      };
      //---------------------------------------------------
    
   public:
      sc_in<sc_lv<data_width> >    i_q; //read data (valid on read)
      sc_out<bool>                 o_me; // read port selected when high
      sc_out<bool>                 o_rw; // write happens if  1 else read happens
      sc_out< sc_lv<addr_width> >  o_wadr;  // write  address
      sc_out<sc_lv<addr_width> >   o_radr; // read  address
      sc_out< sc_lv<data_width> >  o_d;     // write data (used on write)
      sc_out< sc_lv<data_width> >  o_wm;    // write mask (if bit == 1 write is masked)


      rw_port ( const char *name)
         : rw_chk(name, "call reset()", "access this port")
         , i_q(ccs_concat(name,"i_q"))
         , o_me(ccs_concat(name,"o_me"))
         , o_rw(ccs_concat(name,"o_rw"))
         , o_wadr(ccs_concat(name,"o_wadr"))
         , o_radr(ccs_concat(name,"o_radr"))
         , o_d(ccs_concat(name,"o_d"))
         , o_wm(ccs_concat(name,"o_wm")) 
         {}

      void reset() {
         o_me.write(false);
         o_rw.write(false);
         o_wadr.write(sc_dt::Log_0);
         o_radr.write(sc_dt::Log_0);
         o_d.write(sc_dt::Log_0);
         o_wm.write(sc_dt::Log_0);
         rw_chk.ok();
      }

      #pragma mio_delay 0.3
      #pragma remove_out_reg
      #pragma design modulario
      T readwrite (bool en_write, ac_int<addr_width,false> addr, T data=0, ac_int<data_width> mask = 0) {
         rw_chk.test();
         sc_lv<data_width> temp_data;
         type_to_vector(data, (signedtype?true:false), temp_data);

         sc_lv<addr_width> temp_addr;
         type_to_vector(addr, (signedtype?true:false), temp_addr);

         sc_lv<data_width> temp_mask;
         type_to_vector(mask, (signedtype?true:false), temp_mask);

         o_me.write(true);
         o_rw.write(en_write);
         o_wadr.write(temp_addr);
         o_radr.write(temp_addr);
         o_d.write(temp_data);
         o_wm.write(temp_mask);
         #pragma unroll
         for (ac_int<4,false> lcnt=0; lcnt<1; lcnt++) wait();
#if defined(CALYPTO_SC)
         wait(); // Account for unreg outputs for SLEC
#elif !defined(__SYNTHESIS__)
         wait(0.3, SC_NS);  // Account for unreg outputs in simulation
#endif
         o_me.write(false);
         o_rw.write(false);
         o_wm.write(sc_dt::Log_0);
#if !defined (CALYPTO_SC)
         // Return outputs to don't care to save area
         sc_lv<data_width> dc;
         o_d.write(dc);
         o_wadr.write(dc);
         o_radr.write(dc);
         o_wm.write(dc);
#endif

         T return_data;
         vector_to_type(i_q.read(), (signedtype?true:false), &return_data);
         return return_data;
      }

      void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width> mask = 0 ) {
        readwrite(true,addr,data,mask);
      }

      T read ( ac_int<addr_width,false> addr ) {
        return readwrite(false,addr);
      }

      mem_ref_synth operator [] (int index) {
         mem_ref_synth write_ref(*this, index);
         return write_ref;
      }

      void bind ( rw_port<T,size> &c) {
         i_q(c.i_q);
         o_me(c.o_me);
         o_rw(c.o_rw);
         o_wadr(c.o_wadr);
         o_radr(c.o_radr);
         o_d(c.o_d);
         o_wm(c.o_wm);
      }

      template <class C>
      void bind(C& c) {
         i_q(c.q);
         o_me(c.me);
         o_rw(c.rw);
         o_wadr(c.wadr);
         o_radr(c.radr);
         o_d(c.d);
         o_wm(c.wm);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };
};

#endif

