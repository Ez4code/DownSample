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

#if !defined(__CCS_SAMPLE_MEM_1R1W_H)
#define __CCS_SAMPLE_MEM_1R1W_H
#include <calypto_mem/calypto_mem_1r1w.h>

//----------------------------------------------------------------------------
// Interconnect: mem_1r1w
//   Structural shared memory.  Please refer to calypto_mem_1r1w.h for descriptions
//   of the pins on the memory model.
//   Supported Methods:
//     wr_port:
//         write(unsigned int addr, T data, ac_int<size T> wr_mask):  
//            Writes data into memory across the write port. mask is a bitmask for the writes.
//         [unsigned int addr]= operator: Writes data without ability to use bit mask.
//     rd_port:
//        T read(unsigned int addr):  Reads data from port.  Fixed timing, non-blocking
//        [unsigned int addr] operator:  Reads data
//   Most common declaration examples, which result in transaction level simulation:
//   NOTE: Second template argument to channel is the size of the memory.  Min size is 2.
//     mem_1r1w<>::mem<data_T, 128> my_RAM;
//     mem_1r1w<>::rd_port<data_T, 128> my_input_port;
//     mem_1r1w<>::wr_port<data_T, 128> my_output_port;
//   To declare pin-accurate interconnects use SYN template parameter:
//     mem_1r1w<SYN>::mem<data_T, 128> my_RAM;
//     mem_1r1w<SYN>::rd_port<data_T, 128> my_input_port;
//     mem_1r1w<SYN>::wr_port<data_T, 128> my_output_port;

// Base Template
template <abstraction_t source_abstraction = AUTO>
class mem_1r1w {
public:

// Base Template Definition, override defaults for synthesis
// Do not overried setting on impl_abstraction template parameter!

// For synthesis, always use SYN view
#if defined(__SYNTHESIS__)
   template <class T, unsigned int size, abstraction_t impl_abstraction = SYN> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = SYN> class rd_port{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = SYN> class wr_port{};

// For SCVerify, default to SYN unless hard-coded to TLM for testbench only channels
#elif defined (CCS_DUT_CYCLE) || defined (CCS_DUT_RTL)

   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? SYN : source_abstraction> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? SYN : source_abstraction> class rd_port{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? SYN : source_abstraction> class wr_port{};

// For source simulation, default to TLM unless hard-coded to SYN
#else

   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? P2P_DEFAULT_VIEW : source_abstraction> class mem{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? P2P_DEFAULT_VIEW : source_abstraction> class rd_port{};
   template <class T, unsigned int size, abstraction_t impl_abstraction = (source_abstraction==AUTO) ? P2P_DEFAULT_VIEW : source_abstraction> class wr_port{};

#endif

   // ***********************************************
   // TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM TLM
   //    This code is only for TLM simulation only
   // ***********************************************

   //==================================
   // RAM Core
   //    The shared variable "array" is not synthesizable
   //==================================

   template <class T, unsigned int size>
   class mem<T, size, TLM>
   {

     p2p_checker rd_chk, wr_chk, rclk_chk, wclk_chk;

   public:   
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};

      mem(const char* name) 
      : rd_chk(name, "call reset_read()", "read from this memory")
      , wr_chk(name, "call reset_write()", "write to this memory")
      , rclk_chk(name, "bind the rclk", "read from this memory")
      , wclk_chk(name, "bind the wclk", "write to this memory")
      {} 

      // Empty functions for clock binding (Not needed for TLM)
      void rclk (sc_in<bool> &clock) {rclk_chk.ok();}
      void rclk (sc_signal<bool> &clock) {rclk_chk.ok();}
      void wclk (sc_in<bool> &clock) {wclk_chk.ok();}
      void wclk (sc_signal<bool> &clock) {wclk_chk.ok();}

      // Empty reset functions
      void reset_read() {rd_chk.ok();}
      void reset_write() {wr_chk.ok();}

      void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
        wr_chk.test();
        wclk_chk.test();
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

      T read ( ac_int<addr_width,false> addr ) {
        rd_chk.test();
        rclk_chk.test();
        return array[addr]; 
      }

      T &operator [] (int index) { 
        wr_chk.test();
        wclk_chk.test();
        return array[index]; 
      }

      const T &operator [] (int index) const { 
        rd_chk.test();
        rclk_chk.test();
        return array[index]; 
      }
 
      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      T tb_read(int addr) const {
        return array[addr];
      }

      // NOTE: this method is intended for use by testbenches only. It is not synthesizable!
      void tb_write( int addr, T data) {
        array[addr] = data;
      }

   private:
      T array[size];   
   };

   //==================================
   // READ/WRITE PORTS
   //==================================

   template <class T, unsigned int size>
   class  rd_port<T, size, TLM> 
   {
     p2p_checker rd_chk;

   public:
      rd_port (const char *name) 
        : rd_chk(name, "call reset_read()", "read from this port")
        {}

      T read(unsigned int addr) {
         rd_chk.test();
         return (bound_to_port? port_ptr->read(addr) : chan_ptr->read(addr));
      }

      const T &operator [] (int index) const {
         rd_chk.test();
         return (bound_to_port? (*port_ptr)[index] : (*chan_ptr)[index]);
      }

      bool bound_to_port;

      void reset_read() {rd_chk.ok();}

      void operator() (mem<T,size,TLM> &c) {
         chan_ptr = &c;
         c.reset_read(); // Supress reset check after memory is bound
         bound_to_port = false;
      }

      void operator() (rd_port<T,size,TLM> &c) {
         port_ptr = &c;
         c.reset_read(); // Supress reset check of bound port
         bound_to_port = true;
      }

      mem<T,size,TLM> *chan_ptr;
      rd_port<T,size,TLM> *port_ptr;
   };

   template <class T, unsigned int size>
   class  wr_port<T, size, TLM> 
   {
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};
      p2p_checker wr_chk;
      
   public:
      wr_port (const char *name) 
        : wr_chk(name, "call reset_write()", "write to this port")
        {}

      void write(unsigned int addr, T data, ac_int<data_width,false> mask = 0 ) {
         wr_chk.test();
         if ( bound_to_port )
            port_ptr->write(addr,data,mask);
         else
            chan_ptr->write(addr,data,mask);
      }

      T &operator [] (int index) {
         wr_chk.test();
         return (bound_to_port? (*port_ptr)[index] : (*chan_ptr)[index]);
      }
   
      void reset_write() {wr_chk.ok();}

      bool bound_to_port;

      void operator() (mem<T,size,TLM> &c) {
         chan_ptr = &c;
         c.reset_write(); // Supress reset check after memory is bound
         bound_to_port = false;
      }

      void operator() (wr_port<T,size,TLM> &c) {
         port_ptr = &c;
         c.reset_write();  // Supress reset check on bound ports
         bound_to_port = true;
      }

      mem<T,size,TLM> *chan_ptr;
      wr_port<T,size,TLM> *port_ptr;
   };

   // ***********************************************
   // SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN SYN
   //    Synthesis code
   // ***********************************************

   //==================================
   // RAM Core
   //    Encapsulation for RAM
   //==================================

   template <class T, unsigned int size>
   class  mem<T,size,SYN> 
   {
     enum {addr_width = nbits<size-1>::val,
           signedtype = mc_typedef_T_traits<T>::issigned,
           data_width = mc_typedef_T_traits<T>::bitwidth};
     p2p_checker rd_chk, wr_chk, rclk_chk, wclk_chk;

     //---------------------------------------------------
     // Helper class for overloading [] operator 
     class mem_ref_synth 
     {
     public:
        mem_ref_synth( mem<T,size,SYN> &my_parent, ac_int<addr_width,false> addr)
           : address(addr), parent(my_parent) {}
 
        operator T() const { return parent.read(address); }
 
        inline mem_ref_synth operator = ( T val ) {
           parent.write(address,val);
           return *this;
        }
 
        inline mem_ref_synth operator = ( const mem_ref_synth &val ) { return operator = ((T)val); }
 
     private:
        ac_int<addr_width,false> address;
        mem<T,size,SYN> &parent;
     };
     //---------------------------------------------------

  public:

     // Synthesis interface
     sc_signal< sc_lv<data_width> >    q;    // read data (valid on read)
     sc_signal< bool >                 rme;  // read port selected when high
     sc_signal< sc_lv<addr_width> >    radr; // read  address
     sc_signal< bool >                 wme;  // write port selected when high
     sc_signal< sc_lv<addr_width> >    wadr; // write  address
     sc_signal< sc_lv<data_width> >    d;    // write data (used on write)
     sc_signal< sc_lv<data_width> >    wm;   // write mask (if bit == 1 write is masked)
  
     mem ( const char *name)
       : rd_chk(name, "call reset_read()", "read from this memory")
       , wr_chk(name, "call reset_write()", "write to this memory")
       , rclk_chk(name, "bind the rclk", "read from this memory")
       , wclk_chk(name, "bind the wclk", "write to this memory")
       , q (ccs_concat(name,"q"))
       , rme (ccs_concat(name,"rme"))
       , radr (ccs_concat(name,"radr"))
       , wme (ccs_concat(name,"wme"))
       , wadr (ccs_concat(name,"wadr"))
       , d (ccs_concat(name,"d"))
       , wm (ccs_concat(name,"wm"))
       , RAM(ccs_concat(name, "RAM"))
      {
         RAM.q(q);
         RAM.rme(rme);
         RAM.radr(radr);
         RAM.wme(wme);
         RAM.wadr(wadr);
         RAM.d(d);
         RAM.wm(wm);
      }

      void rclk (sc_in<bool> &clock) { rclk_chk.ok(); RAM.rclk(clock); }
      void rclk (sc_signal<bool> &clock) { rclk_chk.ok(); RAM.rclk(clock);}
      void wclk (sc_in<bool> &clock) { wclk_chk.ok(); RAM.wclk(clock);}
      void wclk (sc_signal<bool> &clock) { wclk_chk.ok(); RAM.wclk(clock);}
  
      void reset_write() {
         wme.write(false);
         wadr.write(sc_dt::Log_0);
         d.write(sc_dt::Log_0);
         wm.write(sc_dt::Log_0);
         wr_chk.ok();
      }
      
      void reset_read() {
         rme.write(false); 
         radr.write(sc_dt::Log_0);
         rd_chk.ok();
      }

      #pragma remove_out_reg
      #pragma design modulario
      void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
        wr_chk.test();
        wclk_chk.test();
        sc_lv<data_width> temp_data;
        type_to_vector(data, (signedtype?true:false), temp_data);

        sc_lv<addr_width> temp_addr;
        type_to_vector(addr, (signedtype?true:false), temp_addr);

        sc_lv<data_width> temp_mask;
        type_to_vector(mask, (signedtype?true:false), temp_mask);

        wme.write(true);
        wadr.write(temp_addr);
        d.write(temp_data);
        wm.write(temp_mask);
        wait();
        wme.write(false);
        // Return outputs to don't care to save area
#if !defined (CALYPTO_SC)
        sc_lv<data_width> dc;
        d.write(dc);
        wadr.write(dc);
        wm.write(dc);
#endif
      }

      #pragma mio_delay 0.3 
      #pragma remove_out_reg
      #pragma design modulario
      T read ( ac_int<addr_width,false> addr ) {
         rd_chk.test();
         rclk_chk.test();         
         sc_lv<addr_width> temp_addr;
         type_to_vector(addr,false,temp_addr);

         rme.write(true);
         radr.write(temp_addr);
         #pragma unroll
         for (ac_int<4,false> lcnt=0; lcnt<1; lcnt++) wait();
         // Return outputs to don't care to save area
#if defined(CALYPTO_SC)
         wait(); // Account for unreg outputs for SLEC
#elif !defined(__SYNTHESIS__)
         wait(0.3, SC_NS);  // Account for unreg outputs in simulation
#endif
         rme.write(false);
#if !defined (CALYPTO_SC)
         sc_lv<data_width> dc;
         radr.write(dc);
#endif

         T temp_data;
         vector_to_type(q.read(), (signedtype?true:false), &temp_data);
         return temp_data;
      }

      mem_ref_synth operator [] (int index) {
         mem_ref_synth write_ref(*this, index);
         return write_ref;
      }

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
      calypto_mem_1r1w_wrapper<addr_width, data_width, size> RAM;
   };

   // External Channels, used to connect to RAM
   // Local encapsulated port definitions
   template <class T, unsigned int size>
   class rd_port<T,size,SYN> 
   {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};
      p2p_checker rd_chk; 
       
      //---------------------------------------------------
      // Helper class for overloading [] operator
      class mem_ref_synth 
      {
      public:
         mem_ref_synth( rd_port<T,size,SYN> &my_parent, ac_int<addr_width,false> addr) 
            : address(addr), parent(my_parent) {}

         operator T() const { return parent.read(address); }

         inline mem_ref_synth operator = ( const mem_ref_synth &val ) { return operator = ((T)val); }

      private:
         ac_int<addr_width,false> address;
         rd_port<T,size,SYN> &parent;
      };
      //---------------------------------------------------
    
   public:
      sc_in<sc_lv<data_width> >    i_q; //read data (valid on read)
      sc_out<bool>                 o_rme; // read port selected when high
      sc_out<sc_lv<addr_width> >   o_radr; // read  address

      rd_port ( const char *name)
         : rd_chk(name, "call reset_read()", "read from this port")
         , i_q(ccs_concat(name,"i_q"))
         , o_rme(ccs_concat(name,"o_rme"))
         , o_radr(ccs_concat(name,"o_radr")) 
         {}

      void reset_read() {
         o_rme.write(false);
         o_radr.write(sc_dt::Log_0);
         rd_chk.ok();
      }

      #pragma mio_delay 0.3
      #pragma remove_out_reg
      #pragma design modulario
      T read ( ac_int<addr_width,false> addr ) {
         rd_chk.test();
         sc_lv<addr_width> temp_addr;
         type_to_vector(addr,false,temp_addr);

         o_rme.write(true);
         o_radr.write(temp_addr);
         #pragma unroll
         for (ac_int<4,false> lcnt=0; lcnt<1; lcnt++) wait();
#if defined(CALYPTO_SC)
         wait(); // Account for unreg outputs for SLEC
#elif !defined(__SYNTHESIS__)
         wait(0.3, SC_NS);  // Account for unreg outputs in simulation
#endif
         o_rme.write(false);
#if !defined(CALYPTO_SC)
         // Return outputs to don't care to save area
         sc_lv<addr_width> dc;
         o_radr.write(dc);
#endif

         T temp_data;
         vector_to_type(i_q.read(), (signedtype?true:false), &temp_data);
         return temp_data;
      }

      mem_ref_synth operator [] (int index) {
         mem_ref_synth write_ref(*this, index);
         return write_ref;
      }

      void bind ( rd_port<T,size> &c) {
         i_q(c.i_q);
         o_rme(c.o_rme);
         o_radr(c.o_radr);
      }

      template <class C>
      void bind(C& c) {
         i_q(c.q);
         o_rme(c.rme);
         o_radr(c.radr);
      }

      template <class C>
      void operator() (C& c) {
         bind(c);
      }
   };

   template <class T, unsigned int size>
   class wr_port<T, size, SYN> 
   {
      // Address Width
      enum {addr_width = nbits<size-1>::val,
            signedtype = mc_typedef_T_traits<T>::issigned,
            data_width = mc_typedef_T_traits<T>::bitwidth};
      p2p_checker wr_chk;
 
      //---------------------------------------------------
      // Helper class for overloading [] operator
      class mem_ref_synth 
      {
      public:
         mem_ref_synth( wr_port<T,size,SYN> &my_parent, ac_int<addr_width,false> addr) 
            : address(addr), parent(my_parent) {}

         inline mem_ref_synth operator = ( T val ) {
            parent.write(address,val);
            return *this;
         }


         inline mem_ref_synth operator = ( const mem_ref_synth &val ) { return operator = ((T)val); }

      private:
         ac_int<addr_width,false> address;
         wr_port<T,size,SYN> &parent;
      };
      //---------------------------------------------------

   public:
      sc_out< bool >                o_wme;   // write port selected when high
      sc_out< sc_lv<addr_width> >   o_wadr;  // write  address
      sc_out< sc_lv<data_width> >   o_d;     // write data (used on write)
      sc_out< sc_lv<data_width> >   o_wm;    // write mask (if bit == 1 write is masked)

      wr_port ( const char *name)
         : wr_chk(name, "call reset_write()", "write to this port")
         , o_wme(ccs_concat(name,"o_wme"))
         , o_wadr(ccs_concat(name,"o_wadr"))
         , o_d(ccs_concat(name,"o_d"))
         , o_wm(ccs_concat(name,"o_wm")) 
         {}

      void reset_write() {
         o_wme.write(false);
         o_wadr.write(sc_dt::Log_0);
         o_d.write(sc_dt::Log_0);
         o_wm.write(sc_dt::Log_0);
         wr_chk.ok();
      }

      #pragma remove_out_reg
      #pragma design modulario
      void write ( ac_int<addr_width,false> addr, T data, ac_int<data_width,false> mask = 0 ) {
         wr_chk.test();
         sc_lv<data_width> temp_data;
         type_to_vector(data, (signedtype?true:false), temp_data);

         sc_lv<addr_width> temp_addr;
         type_to_vector(addr, (signedtype?true:false), temp_addr);

         sc_lv<data_width> temp_mask;
         type_to_vector(mask, (signedtype?true:false), temp_mask);

         o_wme.write(true);
         o_wadr.write(temp_addr);
         o_d.write(temp_data);
         o_wm.write(temp_mask);
         wait();
         o_wme.write(false);
         // Return outputs to don't care to save area
#if !defined (CALYPTO_SC)
         sc_lv<addr_width> dc;
         o_wadr.write(dc);
         o_d.write(dc);
         o_wm.write(dc);
#endif
      }

      mem_ref_synth operator [] (int index) {
         mem_ref_synth write_ref(*this, index);
         return write_ref;
      }

      void bind ( wr_port<T,size> &c) {
         o_wme(c.o_wme);
         o_wadr(c.o_wadr);
         o_d(c.o_d);
         o_wm(c.o_wm);
      }

      template <class C>
      void bind(C& c) {
         o_wme(c.wme);
         o_wadr(c.wadr);
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

