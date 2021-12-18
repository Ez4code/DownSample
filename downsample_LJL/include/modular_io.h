#ifndef MODULAR_IO_H
#define MODULAR_IO_H

#include <systemc.h>

#define ccs_concat(n1,n2) (n1 ? ((std::string(n1)+"_"+n2).c_str()) : 0)

// This file contains samples from the Catapult Modular IO interface library.  
// The names are subject to change and we will adjust our naming conventions 
// to follow whatever conventions exist among our customers but the basic structure will
// look like this.

// There are 4 basic interfaces we expect user code to contain:
template <class T> struct put_if { 
  virtual void put(T) = 0;
  virtual ~put_if() {};
};
template <class T> struct get_if { 
virtual T    get() = 0;
  virtual ~get_if() {};
};
template <class A, class D> struct mem_write_if {
  virtual void mem_write(A, D) = 0;
  virtual ~mem_write_if() {};
};
template <class A, class D> struct mem_read_if  {
 virtual D    mem_read(A)     = 0;
  virtual  ~mem_read_if() {};
};

//----------------------------------------------------------------------------
// Module: wait_hs
//
// A class which encapsulates the two wire handshake signal.
template <class T>
class wait_hs
#ifdef MGC_VISTA
  : public sc_interface
#endif
{
public:
  sc_signal <bool> vld;
  sc_signal <T>    dat;
  sc_signal <bool> rdy;
        
  wait_hs() {}
  wait_hs (const char *name) :
    vld(ccs_concat(name, "vld")),
    dat(ccs_concat(name, "dat")),
    rdy(ccs_concat(name, "rdy")) {}

  void reset_get() { rdy.write(false); }
  void reset_put() { vld.write(false); dat.write(0); }

#pragma design  modulario
  T get() {
    while (true) {
      rdy.write(true);
      wait();
      if ( vld.read() ) break;
    }
    rdy.write(false);
    return dat.read();
  }
	
#pragma design  modulario
  void put(T d) {
    while (true) {
      dat.write(d);
      vld.write(true);
      wait();
      if (rdy.read() ) break;
    }
    vld.write(false);
  }
};

//----------------------------------------------------------------------------
// Module: en_hs
//
// A class which encapsulates the one wire handshake signal.
template <class T>
class en_hs
#ifdef MGC_VISTA
  : public sc_interface
#endif
{
public:
  sc_signal <T>    dat;
  sc_signal <bool> vld;


  en_hs () {}
  en_hs (const char *name) :
    dat(ccs_concat(name, "dat")),
    vld(ccs_concat(name, "vld")) {}
};


//----------------------------------------------------------------------------
// Module: w_hs
//
// Similar to a normal signal but uses modulario scheduling rules
template <class T>
class w_hs
#ifdef MGC_VISTA
  : public sc_interface
#endif
{
public:
  sc_signal <T>    dat;

  w_hs () {}
  w_hs (const char *name) :
    dat(ccs_concat(name, "dat")) {}
};


class wait_event_hs
#ifdef MGC_VISTA
	: public sc_interface
#endif
{
public:
	sc_signal<bool> val;

	wait_event_hs (const char *name) :
          val(ccs_concat(name, "val")) {}

};



/*
   This file contains modularIO matching the basic functionality of mgc_ioport.lib
     Component                 Modular IO
   mgc_in_wire            (normal SystemC signal or w_in
   mgc_in_wire_en         en_in
   mgc_in_wire_wait       wait_in
   mgc_out_stdreg         (normal SystemC signal) or w_out
   mgc_out_stdreg_en      en_out
   mgc_out_stdreg_wait    wait_out
*/

//----------------------------------------------------------------------------
// Module: wait_in
//
// Similar to mgc_in_wire_wait
template <class T>
class wait_in : 
#ifdef MGC_VISTA
  public sc_port < wait_hs <T> > ,
#endif
  public get_if<T> 
{
public:
  sc_in<bool> i_vld;
  sc_in<T> i_dat;
  sc_out<bool> o_rdy;

  wait_in (const char *name) : 
    i_vld(ccs_concat(name, "i_vld")), 
    i_dat(ccs_concat(name, "i_dat")), 
    o_rdy(ccs_concat(name, "o_rdy")) {}

  void reset() { o_rdy.write(false); }
  bool available() const { return i_vld.read(); }

#pragma design  modulario
  T get() {  
    while (true) {
      o_rdy.write(true); 
      wait();      
      if ( i_vld.read() ) break;
    }
    o_rdy.write(false);
    return i_dat.read();  
  } 

  void bind (wait_in<T >& c) {
    i_vld(c.i_vld);
    i_dat(c.i_dat);
    o_rdy(c.o_rdy);
  }

  template <class C>
  void bind (C& c) {
    i_vld(c.vld);
    i_dat(c.dat);
    o_rdy(c.rdy);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};

//----------------------------------------------------------------------------
// Module: wait_out
//
// Similar to mgc_out_stdreg_wait
template <class T>
class wait_out : 
#ifdef MGC_VISTA
  public sc_port < wait_hs <T> > ,
#endif
  public put_if<T> 
{
public:
  sc_out<bool> o_vld;
  sc_out<T>    o_dat;
  sc_in<bool>  i_rdy;

  wait_out (  const char *name) :
    o_vld(ccs_concat(name, "o_vld")), 
    o_dat(ccs_concat(name, "o_dat")), 
    i_rdy(ccs_concat(name, "o_rdy")) {}

  void reset() { o_vld.write(false); o_dat.write(0); }

#pragma design  modulario
  void put(T d) { 
    while (true) {
      o_dat.write(d);   
      o_vld.write(true);
      wait();
      if (i_rdy.read() ) break;
    }
    o_vld.write(false);
  }

  void bind (wait_out<T >& c){
    o_vld(c.o_vld);
    o_dat(c.o_dat);
    i_rdy(c.i_rdy);
  }

  template <class C>
  void bind (C& c){
    o_vld(c.vld); 
    o_dat(c.dat);   
    i_rdy(c.rdy);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};

//----------------------------------------------------------------------------
// Module: en_in
//
// Similar to wait class but with no rdy
template <class T>
class en_in : public get_if<T> 
{
public:
  sc_in<T>    i_dat;
  sc_in<bool> i_vld;


  en_in (const char *name) : 
    i_dat(ccs_concat(name, "i_dat")), 
    i_vld(ccs_concat(name, "i_vld")) {}

  void reset() { };

#pragma design  modulario
  T get() {  
    while (true) {
      wait();      
      if ( i_vld.read() ) break;
    }
    return i_dat.read();  
  } 

  void bind (en_in< T >& c){
    i_dat(c.i_dat);
    i_vld(c.i_vld);
  }

  template <class C>
  void bind (C& c){
    i_dat(c.dat);
    i_vld(c.vld);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};




//----------------------------------------------------------------------------
// Module: en_out
//
// Similar to mgc_out_stdreg_en
template <class T>
class en_out : public put_if<T> 
{
public:
  sc_out<bool> o_vld;
  sc_out<T>    o_dat;

  en_out (const char *name) : 
    o_vld(ccs_concat(name, "o_vld")), 
    o_dat(ccs_concat(name, "o_dat")) {}

  void reset() { o_vld.write(false); o_dat.write(0); }

#pragma design  modulario
  void put(T d) { 
    o_dat.write(d);   
    o_vld.write(true);
    wait();
    o_vld.write(false);
  }

  void bind (en_out<T >& c){
    o_vld(c.o_vld);
    o_dat(c.o_dat);
  }

  void bind (en_hs<T >& c){
    o_vld(c.vld);
    o_dat(c.dat);
  }

  template <class C>
  void bind (C& c){
    o_vld(c.vld);
    o_dat(c.dat);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};

//----------------------------------------------------------------------------
// Module: w_in
//
// a "schedulable" signal
template <class T>
class w_in : public get_if<T> 
{
public:
  sc_in<T>    i_dat;

  w_in (const char *name) : 
    i_dat(ccs_concat(name, "i_dat")) {}


#pragma design  modulario
  T get() {  
    return i_dat.read();  
  } 

  void bind (w_in< T >& c){
    i_dat(c.i_dat);
  }

  template <class C>
  void bind (C& c){
    i_dat(c.dat);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};


//----------------------------------------------------------------------------
// Module: w_out
//
// a "schedulable" signal
template <class T>
class w_out : public put_if<T> 
{
public:
  sc_out<T>    o_dat;

  w_out (const char *name) : 
    o_dat(ccs_concat(name, "o_dat")) {}

  void reset() {
    o_dat.write(0);
  }

#pragma design  modulario
  void put(T d) { 
    o_dat.write(d);   
    wait();
  }

  void bind (w_out<T >& c){
    o_dat(c.o_dat);
  }

  void bind (w_hs<T >& c){
    o_dat(c.dat);
  }

  template <class C>
  void bind (C& c){
    o_dat(c.dat);
  }

  template <class C>
  void operator() (C& c) {
    bind(c);
  }
};




class sync_chan {
public:
    sc_signal<bool> vld;
    sc_signal<bool> rdy;
    
    sync_chan (const char *name) :
        vld(ccs_concat(name, "vld")),
        rdy(ccs_concat(name, "rdy")) {}

    void reset_out () {
        vld = false;
    }
    
    void reset_in() {
        rdy = false;
    }

    // Bool return type means to wait until completion of function
    #pragma design modulario
    bool sync_in() {
        rdy = true;
        do {
            wait();
        } while (!vld.read());
        rdy = false;
        return vld;
    }

    #pragma design modulario
    bool sync_out() {
        vld = true;
        do {
            wait();
        } while (!rdy.read());
        vld = false;
        return rdy;
    }
};


class sync_in {
public:
    sc_in<bool> i_vld;
    sc_out<bool> o_rdy;
    
    sync_in (const char *name) :
        i_vld(ccs_concat(name, "vld")),
        o_rdy(ccs_concat(name, "o_rdy")) {}

    void reset() {
        o_rdy = false;
    }

    // Bool return type means to wait until completion of function
    #pragma design modulario
    bool sync() {
        o_rdy = true;
        do {
            wait();
        } while (!i_vld);
        o_rdy = false;
        return i_vld;
    }

    void bind (sync_in &c){
      i_vld(c.i_vld);
      o_rdy(c.o_rdy);
    }

    template <class C>
      void bind (C& c){
      i_vld(c.vld);
      o_rdy(c.rdy);
    }

    template <class C>
      void operator() (C& c) {
      bind(c);
    }

};

class sync_out {
public:
    sc_out<bool> o_vld;
    sc_in<bool> i_rdy;

    sync_out (const char *name) :
        o_vld(ccs_concat(name, "o_vld")),
        i_rdy(ccs_concat(name, "i_rdy")) {}

    void reset () {
        o_vld = false;
    }

    #pragma design modulario
    bool sync() {
        o_vld = true;
        do {
            wait();
        } while (!i_rdy);
        o_vld = false;
        return i_rdy;
    }

    void bind (sync_out &c){
      o_vld(c.o_vld);
      i_rdy(c.i_rdy);
    }

    template <class C>
      void bind (C& c){
      o_vld(c.vld);
      i_rdy(c.rdy);
    }

    template <class C>
      void operator() (C& c) {
      bind(c);
    }

};



class wait_event_in 
#ifdef MGC_VISTA
     : public sc_port < wait_event_hs > 
#endif
{
public:
	sc_in<bool> i_val;

	wait_event_in (const char *name) :
          i_val(ccs_concat(name, "i_val")) {}

#pragma design modulario
	void wait_for() {
          while (true) {
            wait();
            if (i_val.read()) break;
          }
	}

	void bind (wait_event_in &c) {
		i_val(c.i_val);
	}

	void bind (wait_event_hs &c) {
		i_val(c.val);
	}

	void operator() (wait_event_hs &c) {
		bind(c);
	}
};

class wait_event_out 
#ifdef MGC_VISTA
     : public sc_port < wait_event_hs > 
#endif
{
public:
	sc_out<bool> o_val;

	wait_event_out (const char *name) :
          o_val(ccs_concat(name, "o_val")) {}
	void reset() { 
          o_val = false;
	}
	#pragma design modulario
	void pulse() {
          o_val = true;
          wait();
          o_val = false;
	}

	void bind (wait_event_out &c) {
          o_val(c.o_val);
	}

	void bind (wait_event_hs &c) {
          o_val(c.val);
	}

	void operator() (wait_event_hs &c) {
          bind(c);
	}
};

class event_chan {
public:
        sc_signal<bool> val;

        event_chan (const char *name) :
                val(ccs_concat(name, "val")) {}

        void reset () {
                val = false;
        }

        #pragma design modulario
        bool wait_for() {
                do {
                        wait();
                } while (!val);
                return val;
        }

        #pragma design modulario
        void notify() {
                val = true;
                wait();
                val = false;
        }
};

class event_in {
public:
        sc_in<bool> i_val;

        event_in (const char *name) :
                i_val(ccs_concat(name, "i_val")) {}

#pragma design modulario
        bool wait_for() {
                do {
                        wait();
                } while (!i_val);
                return i_val;
        }

        void bind (event_chan &c) {
                i_val(c.val);
        }

        void operator() (event_chan &c) {
                bind(c);
        }
};

class event_out {
public:
        sc_out<bool> o_val;

        event_out (const char *name) :
                o_val(ccs_concat(name, "o_val")) {}

        void reset() { 
                o_val = false;
        }
        #pragma design modulario
        void notify() {
                o_val = true;
                wait();
                o_val = false;
        }

        void bind (event_chan &c) {
                o_val(c.val);
        }

        void operator() (event_chan &c) {
                bind(c);
        }
};

#endif
