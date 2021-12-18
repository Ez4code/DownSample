#ifndef MC_TRANSACTOR_H_INC
#define MC_TRANSACTOR_H_INC

#include <sstream>
#include <deque>
#include <string>
#include <vector>

#include <math.h>

#include <systemc.h>
#if !defined(MTI_SYSTEMC) && !defined(NCSC)
#include <sysc/kernel/sc_spawn_options.h>
#endif
#include <tlm.h>

#include "mc_container_types.h"
#include "mc_typeconv.h"

#if (defined(MTI_SYSTEMC) && SYSTEMC_VERSION < 20060505)

#define MC_EXPOSE_NAMES_OF_BASE(BASECLASS)

#define MC_METHOD(func) \
    {		\
        sc_method_process* handle = 								               \
			  this->simcontext()->register_method_process(#func,				 \
            SC_MAKE_FUNC_PTR(SC_CURRENT_USER_MODULE, func), this); \
        this->sensitive << handle;                                 \
        this->sensitive_pos << handle;                             \
        this->sensitive_neg << handle;                             \
    }
#else

#define MC_EXPOSE_NAMES_OF_BASE(BASECLASS) \
   using BASECLASS::sensitive;     \
   using BASECLASS::simcontext;    \
   using BASECLASS::sensitive_pos; \
   using BASECLASS::sensitive_neg;

#define MC_METHOD(func) SC_METHOD(func)

#endif

// Check for macro definitions that will conflict with template parameter names in this file
#if defined(Tclass)
#define Tclass 0
#error The macro name 'Tclass' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tclass' giving the location of the previous definition.
#endif
#if defined(Telemcnt)
#define Telemcnt 0
#error The macro name 'Telemcnt' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Telemcnt' giving the location of the previous definition.
#endif
#if defined(Telemwidth)
#define Telemwidth 0
#error The macro name 'Telemwidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Telemwidth' giving the location of the previous definition.
#endif
#if defined(Tsigned)
#define Tsigned 0
#error The macro name 'Tsigned' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tsigned' giving the location of the previous definition.
#endif
#if defined(Tbwidth)
#define Tbwidth 0
#error The macro name 'Tbwidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tbwidth' giving the location of the previous definition.
#endif
#if defined(Tblkwidth)
#define Tblkwidth 0
#error The macro name 'Tblkwidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tblkwidth' giving the location of the previous definition.
#endif
#if defined(Twidth)
#define Twidth 0
#error The macro name 'Twidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Twidth' giving the location of the previous definition.
#endif
#if defined(Trows)
#define Trows 0
#error The macro name 'Trows' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Trows' giving the location of the previous definition.
#endif
#if defined(Twords)
#define Twords 0
#error The macro name 'Twords' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Twords' giving the location of the previous definition.
#endif
#if defined(Taddrwidth)
#define Taddrwidth 0
#error The macro name 'Taddrwidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Taddrwidth' giving the location of the previous definition.
#endif
#if defined(TrdPorts)
#define TrdPorts 0
#error The macro name 'TrdPorts' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TrdPorts' giving the location of the previous definition.
#endif
#if defined(TwrPorts)
#define TwrPorts 0
#error The macro name 'TwrPorts' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TwrPorts' giving the location of the previous definition.
#endif
#if defined(Registerd)
#define Registerd 0
#error The macro name 'Registerd' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Registerd' giving the location of the previous definition.
#endif
#if defined(TseqDelay)
#define TseqDelay 0
#error The macro name 'TseqDelay' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TseqDelay' giving the location of the previous definition.
#endif
#if defined(TwriteFirst)
#define TwriteFirst 0
#error The macro name 'TwriteFirst' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TwriteFirst' giving the location of the previous definition.
#endif
#if defined(TweType)
#define TweType 0
#error The macro name 'TweType' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TweType' giving the location of the previous definition.
#endif
#if defined(pwidth)
#define pwidth 0
#error The macro name 'pwidth' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'pwidth' giving the location of the previous definition.
#endif
#if defined(element_count)
#define element_count 0
#error The macro name 'element_count' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'element_count' giving the location of the previous definition.
#endif
#if defined(width)
#define width 0
#error The macro name 'width' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'width' giving the location of the previous definition.
#endif
#if defined(COLS)
#define COLS 0
#error The macro name 'COLS' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'COLS' giving the location of the previous definition.
#endif
#if defined(RD_ADDR_REG)
#define RD_ADDR_REG 0
#error The macro name 'RD_ADDR_REG' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'RD_ADDR_REG' giving the location of the previous definition.
#endif
#if defined(WR_DATA_REG)
#define WR_DATA_REG 0
#error The macro name 'WR_DATA_REG' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'WR_DATA_REG' giving the location of the previous definition.
#endif
#if defined(WR_ADDR_REG)
#define WR_ADDR_REG 0
#error The macro name 'WR_ADDR_REG' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'WR_ADDR_REG' giving the location of the previous definition.
#endif
#if defined(WR_EN_REG)
#define WR_EN_REG 0
#error The macro name 'WR_EN_REG' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'WR_EN_REG' giving the location of the previous definition.
#endif
#if defined(UNIFORM)
#define UNIFORM 0
#error The macro name 'UNIFORM' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'UNIFORM' giving the location of the previous definition.
#endif
#if defined(RANDOM)
#define RANDOM 0
#error The macro name 'RANDOM' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'RANDOM' giving the location of the previous definition.
#endif
#if defined(INITIAL)
#define INITIAL 0
#error The macro name 'INITIAL' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'INITIAL' giving the location of the previous definition.
#endif
#if defined(ALL)
#define ALL 0
#error The macro name 'ALL' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'ALL' giving the location of the previous definition.
#endif
#if defined(FIXED_INTERVAL)
#define FIXED_INTERVAL 0
#error The macro name 'FIXED_INTERVAL' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'FIXED_INTERVAL' giving the location of the previous definition.
#endif
#if defined(RANDOM_INTERVAL)
#define RANDOM_INTERVAL 0
#error The macro name 'RANDOM_INTERVAL' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'RANDOM_INTERVAL' giving the location of the previous definition.
#endif
#if defined(IDLE_FLUSH)
#define IDLE_FLUSH 0
#error The macro name 'IDLE_FLUSH' conflicts with an enum in this file.
#error The compiler should have produced a warning about redefinition of 'IDLE_FLUSH' giving the location of the previous definition.
#endif




// Function prototypes for random number generator in mt19937ar.c
extern void mt19937_init_genrand(unsigned long s);
extern double mt19937_genrand_real1(void);


inline int scverify_rand(int max) { return (int)(mt19937_genrand_real1() * max); }

////////////////////////////////////////////////////////////////////////////////
// helper struct for handling single-bit-vectors
////////////////////////////////////////////////////////////////////////////////
template <class Tclass> struct mc_sc_logic2sc_lv_traits;
template <int Tlvwidth>
struct mc_sc_logic2sc_lv_traits< sc_lv<Tlvwidth> > {
   typedef sc_lv<Tlvwidth>  data_type;
   enum {pwidth = Tlvwidth};
};

// This version allows sc_lv<1> to resolve to sc_logic
template <class Tclass> struct mc_SBV_sc_logic2sc_lv_traits;
template <int Tlvwidth>
struct mc_SBV_sc_logic2sc_lv_traits< sc_lv<Tlvwidth> > {
   typedef sc_lv<Tlvwidth>  data_type;
   enum {pwidth = Tlvwidth};
};
template <>
struct mc_SBV_sc_logic2sc_lv_traits< sc_lv<1> > {
   typedef sc_logic         data_type;
   enum {pwidth = 1};
};

////////////////////////////////////////////////////////////////////////////////
// mc_wait_ctrl
////////////////////////////////////////////////////////////////////////////////

struct mc_wait_ctrl {
  enum enum_wait_mode     {UNIFORM  = 0, RANDOM = 1};
  enum enum_wait_elements {INITIAL  = 0, ALL  = 1, FIXED_INTERVAL = 2, RANDOM_INTERVAL = 3, IDLE_FLUSH = 4};

  explicit mc_wait_ctrl() :
    iteration(-1),
    stopat(-1),
    cycles(-1),
    interval(0),
    mode(UNIFORM),
    elements(INITIAL),
    ischannel(false)
    {}

  mc_wait_ctrl(
    int i,
    int s,
    int c,
    enum_wait_mode m,
    enum_wait_elements e,
    int n,
    bool chan=false) : iteration(i), stopat(s), cycles(c),
    interval(n), mode(m), elements(e), ischannel(chan)
    {}
  void clear() { iteration=0; stopat=0; cycles=0; mode=UNIFORM; elements=INITIAL; interval=0; }
  bool is_set() { return (cycles > 0); }
  int                   iteration;  // testbench iteration (transaction) to which this info applies
  int                   stopat;     // testbench value capture count to stopat (clear) current waits (for ac_channels)
  int                   cycles;     // num of cycles to wait (constant or 1/2 average of random)
  unsigned              interval;   // streamed element interval (fixed or max rand) at which wait cycles applied
  enum_wait_mode        mode;       // UNIFORM or RANDOM
  enum_wait_elements    elements;   // INITIAL (begin of transaction) or ALL (all elements in array/channel)
  bool                  ischannel;  // true if I/O is an ac_channel
};

inline
std::ostream& operator<<(std::ostream& os, const mc_wait_ctrl& wi) {
  if (wi.mode == mc_wait_ctrl::UNIFORM)
    os << " a UNIFORM delay of ";
  else
    os << " a RANDOM delay of up to ";
  os << wi.cycles << " cycles";
  switch (wi.elements) {
    case mc_wait_ctrl::INITIAL: {
      os << " applied to the INITIAL element";
      break;
    }
    case mc_wait_ctrl::ALL: {
      os << " applied to ALL elements";
      break;
    }
    case mc_wait_ctrl::FIXED_INTERVAL: {
      os << " applied at a FIXED_INTERVAL (every " << wi.interval << " elements)";
      break;
    }
    case mc_wait_ctrl::RANDOM_INTERVAL: {
      os << " applied at a RANDOM_INTERVAL (up to every " << wi.interval << " elements)";
      break;
    }
    case mc_wait_ctrl::IDLE_FLUSH: {
      os << " applied at after IDLE flush";
      break;
    }
  }
  if (wi.stopat != wi.iteration+1) {
    if (wi.ischannel) os << " for values ";
    else              os << " for iterations ";
    os << wi.iteration+1 << " to " << wi.stopat;
  } else {
    if (wi.ischannel) os << " for value ";
    else              os << " for iteration ";
    os << wi.iteration+1;
  }
  return os;
}


////////////////////////////////////////////////////////////////////////////////
// DEBUGGING AIDS
////////////////////////////////////////////////////////////////////////////////

enum mc_transactor_event {
   MC_TRANSACTOR_NONE      = 0,
   MC_TRANSACTOR_EMPTY     = 1,
   MC_TRANSACTOR_FULL      = 2,
   MC_TRANSACTOR_UNDERFLOW = 4,
   MC_TRANSACTOR_READ      = 8,
   MC_TRANSACTOR_WRITE     = 16,
   MC_TRANSACTOR_LOAD      = 32,
   MC_TRANSACTOR_DUMP      = 64,
   MC_TRANSACTOR_STREAMCNT = 128,
   MC_TRANSACTOR_WAIT      = 256,
   MC_TRANSACTOR_SIZE      = 512
};

static void mc_transactor_stop_debugger() {
  int lineno = __LINE__; (void) lineno;
   // move up the call stack to find event
}

struct mc_transactor_event_module : sc_module
{
  explicit mc_transactor_event_module(const sc_module_name& name) : sc_module(name) {}
  virtual bool log_event(mc_transactor_event event_mask) {
    return (this->get_attribute("MC_TRANSACTOR_EVENT") && ((sc_attribute<int>*)(this->get_attribute("MC_TRANSACTOR_EVENT")))->value & event_mask);
  }
};

////////////////////////////////////////////////////////////////////////////////
// mc_transactor_base
////////////////////////////////////////////////////////////////////////////////

struct mc_transactor_base {
  virtual void exchange_value(int block_id, bool waiting=false) = 0;
  // RH and BaseAddr are in block metric of block block_id
  virtual void initiate_driving_value_adjustments(int block_id, int RH, int BaseAddr, int CH, int BaseBit) = 0;
  virtual ~mc_transactor_base() {}
  // only overwritten by mc_channel_input_transactor
  virtual int count_sizelz_event(bool hi) = 0;
  virtual unsigned int input_fifo_size() = 0; // current input fifo elements used (overwritten by mc_channel_input_transactor)
  // expose transactor log to attached rsc block
  virtual bool trans_log_event(mc_transactor_event event_mask) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// resource blocks
////////////////////////////////////////////////////////////////////////////////

//  A resource block needs to provide this interface to let the
// transactor pick up the current transaction's value (for output
// components) and insert the next value (for input components).
//
// In more detail: all transactors connected to this resource block
// register themselves via register_transactor(...). At the active
// clock edge, the component shall call exchange_value() after it has
// updated its internal value according to its state and input ports.
// Calling exchange_value() might cause several calls to
// inject_value(...) and/or extract_value(...). It is important to
// call exchange_value() right at the active clock edge! It is the
// block's responsibility to trigger a driving value update in case of
// inject_value(...) being called.
//
// A combinational resource block, i.e. a resource block
//  a) without sequential delay,
//  b) (some) unregistered inputs and
//  c) at least one output,
// needs a more complex communication with its connected
// transactors. In general, the process updating the output port(s) of
// the resource (in short "driving value") needs only to consider the
// internal state of the resource. If the transaction signal of a
// connected transactor goes high though, the process might need to
// show (parts of) the "next" transaction value of the corresponding
// variable. Therefore a combinational resource block provides the
// member function require_driving_value_adjustments(...) that lets a
// connected transactor tell the resource block that the driving value
// in the given range needs to be changed. The resource block then
// schedules a driving value update, if the driving value corresponds
// to an affected area. During the driving value update, the resource
// block updates its driving value according to its internal current
// value, and then calls the member function
// initiate_driving_value_adjustments(...), which forwrads this call
// to all registered transactors. Those might in return call the
// resource block's member function adjust_driving_value(...)  to tell
// the resource block that a different value from the current value
// shall be displayed for a certain area.
//

class mc_rsc_block_base {
protected:
  mc_rsc_block_base() { _is_disabled = false; _stall_count = 0;}
  virtual ~mc_rsc_block_base() {}

  struct trans_ref {
    trans_ref(mc_transactor_base* t, int id) : transactor(t), block_id(id) {}
    mc_transactor_base* transactor;
    int                 block_id;
  };

  typedef std::vector<trans_ref> trans_ref_list;

public:
  //! used only by a transactor to send the new transaction's value
  virtual void inject_value(int row, int idx, int width, sc_lv_base& in, int idx_in) = 0;

  //! used only by a transactor to get the old transaction's value
  virtual void extract_value(int row, int idx, int width, sc_lv_base& out, int idx_out) = 0;

  /*! reset_stream() is called whenever a connected transactor has
   * received a triosync signal for this block. This is a hack for
   * streams, and supports single variable resources only. For
   * streams, reset_stream() resets the read and write position. For
   * all others of our resources, this method is meaningless.
   *
   * Since a transactor does not know the type of the resource it is
   * connected to, this member function is part of the base for all
   * resource blocks.
   */
  virtual void reset_stream() {};

  //! only relevant for resources with wait handshaking (must override in derived class)
  virtual void wait_cycles(mc_wait_ctrl wait_ctrl, unsigned short iodir)  {}
  virtual void set_is_disabled(bool is_disabled)                    { _is_disabled = is_disabled; }
  bool get_is_disabled() const { return _is_disabled; }
  virtual bool wait_signal_active(unsigned short iodir)                   { return false; }
  virtual bool io_request(unsigned short iodir)                           { return true; }

  virtual unsigned int in_used() {
    int tmp = 0;
    for (trans_ref_list::iterator i = _trans.begin(); i != _trans.end(); ++i) {
      tmp += i->transactor->input_fifo_size();
    }
    return tmp;
  }

  // Called on every rising edge of sizelz signal of mgc_chan_in component
  virtual int count_sizelz_event(bool hi) {
    int tmp = 0;
    for (trans_ref_list::iterator i = _trans.begin(); i != _trans.end(); ++i) {
      tmp += i->transactor->count_sizelz_event(hi);
    }
    return tmp;
  }

  //! needs to return true if this resource requires a bus transfer prior to receiving_from objblk
  virtual bool bus_transfer_required() { return false; }
  virtual void begin_transfer() {}

  //! needs to return true if this is a combinational resource block
  virtual bool is_combinational() { return false; }

  //! only relevant for combinational resource blocks
  virtual void require_driving_value_adjustments(int RH, int BaseAddr, int CH, int BaseBit) {}

  //! only relevant for combinational resource blocks
  virtual void adjust_driving_value(int row, int idx, int width, sc_lv_base& in, int idx_in) {}

  //! used only by a transactor to register itself
  void register_transactor(mc_transactor_base* transactor, int block_id) {
    _trans.push_back(trans_ref(transactor, block_id));
  }

  /*! the block needs to call this function at the positive clock edge
      in general after updating the internal value
  */
  void exchange_value(bool waiting=false) {
    for (trans_ref_list::iterator i = _trans.begin(); i != _trans.end(); ++i) {
      i->transactor->exchange_value(i->block_id, waiting);
    }
  }
  int stall_count(unsigned short iodir) const { return _stall_count; }
  void incr_stall_count(unsigned short iodir) { _stall_count++; }

  // Check attached transactors for the event logging flags
  bool trans_log_event(mc_transactor_event event_mask) {
    for (trans_ref_list::const_iterator i = _trans.begin(); i != _trans.end(); ++i)
      if (i->transactor->trans_log_event(event_mask))
        return true;
    return false;
  }

protected:
  /*! combinational resource block needs to call this function before
      updating the driving value
  */
  void initiate_driving_value_adjustments(int RH, int BaseAddr, int CH, int BaseBit) {
    for (trans_ref_list::const_iterator i = _trans.begin(); i != _trans.end(); ++i)
      i->transactor->initiate_driving_value_adjustments(i->block_id, RH, BaseAddr, CH, BaseBit);
  }
 bool _is_disabled;
private:
  trans_ref_list _trans;
  int _stall_count;
};


////////////////////////////////////////////////////////////////////////////////
// mc_transactor_type_traits
//   extracts element type and element count info from the abstract
//   datatype on the formal function interface
////////////////////////////////////////////////////////////////////////////////

template <class Tclass>
struct mc_transactor_type_traits
{
  enum {element_count = 1};
  typedef Tclass element_type;
};

template <class Tclass, int Telemcnt>
struct mc_transactor_type_traits< mgc_sysc_ver_array1D<Tclass,Telemcnt> >
{
  enum {element_count = Telemcnt};
  typedef Tclass element_type;
};

////////////////////////////////////////////////////////////////////////////////
// mc_transactor
////////////////////////////////////////////////////////////////////////////////

template <class Tclass, int Telemwidth, bool Tsigned>
class mc_transactor : public mc_transactor_event_module, public mc_transactor_base
{
public:
  void bind_clk(sc_signal_in_if<bool>& clk, bool phase) {
    _clk.bind(clk);
    _phase = phase;
  }

  void register_block(mc_rsc_block_base*   blk,
                      const char*          name,
                      sc_signal<sc_logic>& triosy,
                      int first, int last, int step);

  virtual void reset_streams() {
    for (objblocks_iterator i = _objblocks.begin(); i != _objblocks.end(); ++i)
      (*i)->_mc_rsc_block->reset_stream(); // force streamed resources to be reset
  }

  void set_auto_wait_limit(int l) { _auto_wait_limit = l; }

  virtual bool trans_log_event(mc_transactor_event event_mask) {
    return this->log_event(event_mask);
  }

protected:
  // value queue management
  enum {Telemcnt = mc_transactor_type_traits<Tclass>::element_count};
  typedef sc_lv<Telemcnt*Telemwidth> IT;

  struct objblock {
    objblock(mc_rsc_block_base* b, const char* n,
          sc_signal<sc_logic>& triosy,
          int fb, int lb, int fv, int s,
          int ch, int cl)
      : _mc_rsc_block(b), _name(n)
      , _triosy(&triosy)
      , _fbr(fb), _lbr(lb), _fvr(fv), _step(s)
      , _ch(ch), _cl(cl), _cc(_ch - _cl +1)
    {}

    typedef void (mc_rsc_block_base::* access_fct)(int row, int idx, int width, sc_lv_base& val, int idx_val);

    // transfers the relevant rows, brh and brl is in block metric
    void transfer(access_fct fct, int brh, int brl, IT& val) {
      sc_assert(_fbr <= brl);
      sc_assert(_lbr >= brh);
      for (; brl <= brh; ++brl) {
        const int vrow_idx = (_fvr + (brl - _fbr) * _step) * _cc;

        if (vrow_idx + _cc <= Telemwidth*Telemcnt)
          (_mc_rsc_block->*fct)(brl, _cl, _cc, val, vrow_idx);
        else
          (_mc_rsc_block->*fct)(brl, _cl, Telemwidth*Telemcnt - vrow_idx, val, vrow_idx);
      }
    }

    mc_rsc_block_base* const   _mc_rsc_block;
    const std::string          _name;
    const sc_signal<sc_logic>* _triosy;
    const int                  _fbr;   // first relevant row in block metric
    const int                  _lbr;   // last relevant row in block metric
    const int                  _fvr;   // first relevant row in variable metric
    const int                  _step;
    const int                  _ch;
    const int                  _cl;
    const int                  _cc;
    int                        _trans; // transaction number of current value
  };

  struct objblock_proc {
    explicit objblock_proc(objblock* that) : _that(that) {}
    objblock* const _that;
    void operator()() {
      _that->_mc_rsc_block->require_driving_value_adjustments(_that->_lbr, _that->_fbr, _that->_ch, _that->_cl);
    }
  };

  mc_transactor(const sc_module_name& name, int BaseAddr, int WordWidth, int BaseBit, int auto_wait_limit=0)
    : mc_transactor_event_module(name)
    , _CH(BaseBit+WordWidth-1)
	 , _BaseBit(BaseBit)
	 , _WordWidth(WordWidth)
    , _RH(int(ceil(double(Telemwidth * Telemcnt) / _WordWidth)) + BaseAddr - 1)
	 , _BaseAddr(BaseAddr)
    , _empty(false)
	 , _pending_triosy(false)
    , _auto_wait_limit(auto_wait_limit)
    , _clk("_clk")
  {}

  ~mc_transactor() {
    for (objblocks_iterator i = _objblocks.begin(); i != _objblocks.end(); ++i)
      delete *i;
  }


  // only overwritten by mc_channel_input_transactor
  virtual int count_sizelz_event(bool hi) { return 0; }
  virtual unsigned int input_fifo_size() { return 0; } // current input fifo elements used (overwritten by mc_input_transactor)

  virtual void start_of_simulation() {
    _values.clear();
    _trans_begin = 1;
    for (objblocks_iterator i = _objblocks.begin(); i != _objblocks.end(); ++i)
      (*i)->_trans = 0;
  }

  // These three methods return a reference to an sc_lv<Telemcnt*Telemwidth> entry
  // in the values queue (corresponds to type-converted testbench data)
  IT& get_value(int transno) {
    sc_assert(transno >= _trans_begin);
    while (int(_values.size()) + _trans_begin <= transno)
      receive_value_from_tb();
    return _values[transno - _trans_begin];
  }

  IT& get_latest_value() {
    sc_assert(!_values.empty());
    return _values.back();
  }

  IT& get_oldest_value() {
    sc_assert(!_values.empty());
    return _values.front();
  }

  typedef std::vector<objblock*> objblocks;
  typedef typename objblocks::iterator objblocks_iterator;

  // overwritten by input_transactor ...

  virtual void send_value_to_objblock(objblock* blk) {}
  virtual void initiate_driving_value_adjustments(int blk_id, int RH, int BaseAddr, int CH, int BaseBit) {}
  virtual void receive_value_from_tb() { _values.push_back(IT('0')); }
  virtual bool check_disabled_input() { return false; }

  // overwritten by output_transactor ...

  virtual void receive_value_from_objblock(objblock* blk) {}
  virtual void send_value_to_tb() {
    sc_assert(!_values.empty());
    _values.pop_front();
    ++_trans_begin;
  }

  const int _CH;
  const int _BaseBit;
  const int _WordWidth;
  const int _RH;
  const int _BaseAddr;
  objblocks    _objblocks;
  bool      _empty;
  bool         _pending_triosy;
  int         _auto_wait_limit;

private:
  typedef std::deque<IT> value_queue;

  virtual void exchange_value(int blk_id, bool waiting);

  sc_in<bool>  _clk;
  bool         _phase;
  value_queue  _values;
  int          _trans_begin;
};

template <class Tclass, int Telemwidth, bool Tsigned>
void mc_transactor<Tclass,Telemwidth,Tsigned>::exchange_value(int blk_id, bool waiting)
{
//  sc_assert(_phase ? _clk.posedge() : _clk.negedge()); // must be called at active clock edge

  objblock* const blk = _objblocks[blk_id];

  if (blk->_trans == 0) {
    ++blk->_trans;
    send_value_to_objblock(blk);
  } else {
    bool cdi = check_disabled_input();
    _pending_triosy = _pending_triosy | cdi;
    if (waiting && !(_pending_triosy & cdi)) {
      if (blk->_triosy->read() == SC_LOGIC_1) {
        _pending_triosy = true;
      }
    } else {
      if ((blk->_triosy->read() == SC_LOGIC_1) || _pending_triosy) {  // only valid at active clock edge
        if (blk->_mc_rsc_block->bus_transfer_required()) {
          if (!_pending_triosy) {
            blk->_mc_rsc_block->begin_transfer(); // notify trans rsc to start transfer
          }
          _pending_triosy = true; // and hold off on transaction done until complete
        } else {
          _pending_triosy = false;
          receive_value_from_objblock(blk);        // grab old value first ...
          blk->_mc_rsc_block->reset_stream();          // reset stream pointers

          if (blk->_trans++ == _trans_begin) {
            bool send_value = true;
            for (objblocks_iterator i = _objblocks.begin(); send_value && i != _objblocks.end(); ++i)
              if ((*i)->_trans == _trans_begin)
                send_value = false;             // old value is still needed by other block
            if (send_value) {
              send_value_to_tb();
            }
          }
          send_value_to_objblock(blk);             // .. and place new value afterwards
        }
      }
    }
  }
}

template <class Tclass, int Telemwidth, bool Tsigned>
void mc_transactor<Tclass,Telemwidth,Tsigned>::register_block(mc_rsc_block_base*   blk,
                                             const char*          name,
                                             sc_signal<sc_logic>& triosy,
                                             int first, int last, int step)
{
  if (_RH >= first && _BaseAddr <= last) {
    // block and variable may overlap, so compute the intersection
    // _RH, _BaseAddr, first, last are in resource metrics

    int fbr; // first row in intersection in "block metrics"
    int fvr; // first row in intersection in "var metrics"

    if (_BaseAddr <= first) {
      fbr = 0;
      fvr = first - _BaseAddr;
    } else {
      fbr = int(ceil(double(_BaseAddr - first) / step));
      fvr = fbr * step + first - _BaseAddr;
    }

    int lbr; // last row in intersection in "block metrics"

    if (_RH >= last) {
      lbr = (last - first) / step;
    } else {
      lbr = int(floor(double(_RH - first) / step));
    }

    if (fbr <= lbr) {
      const int blk_id = _objblocks.size();
      blk->register_transactor(this, blk_id);
      _objblocks.push_back(new objblock(blk, name, triosy, fbr, lbr, fvr, step, _CH, _BaseBit));

      if (blk->is_combinational()) {
        sc_spawn_options so;
        so.spawn_method();
        so.dont_initialize();
        so.set_sensitivity(&triosy.value_changed_event());

        std::string proc_name = std::string("triosy_observer_for_") + name;

        sc_spawn(objblock_proc(_objblocks.back()), proc_name.c_str(), &so);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// mc_input_transactor
////////////////////////////////////////////////////////////////////////////////

template <class Tclass, int Telemwidth, bool Tsigned>
class mc_input_transactor : virtual public mc_transactor<Tclass,Telemwidth,Tsigned>
{
public:
  typedef tlm::tlm_fifo_get_if<Tclass> IN_IF;

  sc_port<IN_IF> in_fifo;
  sc_port< tlm::tlm_get_peek_if< mc_wait_ctrl > > in_wait_ctrl_fifo;

  mc_input_transactor(const sc_module_name& name, int BaseAddr, int WordWidth, int BaseBit, bool disable_on_empty=false, int auto_wait_limit=0)
    : mc_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
    , in_fifo("in_fifo")
    , _ld_count(0)
    , _disable_on_empty(disable_on_empty)
    , _input_disabled(false)
  {}

protected:
  typedef mc_transactor<Tclass,Telemwidth,Tsigned>        base;
  typedef typename base::objblock           objblock;
  typedef typename base::IT              IT;
  typedef typename base::objblocks_iterator obi;

  virtual void initiate_driving_value_adjustments(int blk_id, int RH, int BaseAddr, int CH, int BaseBit) {
    objblock* blk = this->_objblocks[blk_id];
    sc_assert(blk->_mc_rsc_block->is_combinational());
    if (!this->_empty && CH >= this->_BaseBit && BaseBit <= this->_CH && RH >= blk->_fbr && BaseAddr <= blk->_lbr && blk->_triosy->read() == SC_LOGIC_1) {
      IT& v = this->get_value(blk->_trans + 1);
      if (!this->_empty) blk->transfer(&mc_rsc_block_base::adjust_driving_value, RH, BaseAddr, v);
    }
  }

  virtual void send_value_to_objblock(objblock* blk) {
    IT& v = this->get_value(blk->_trans);
    if (!this->_empty) {
      blk->transfer(&mc_rsc_block_base::inject_value, blk->_lbr, blk->_fbr, v);

      if (this->log_event(MC_TRANSACTOR_READ)) {
        std::ostringstream msg;
        msg << "MC_TRANSACTOR_READ - sending value #" << blk->_trans << " to " << blk->_name << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }
    }
  }

  virtual bool check_disabled_input() {
    if (_disable_on_empty && _input_disabled && (in_fifo->used()>0)) {
      for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
        ((*i)->_mc_rsc_block)->set_is_disabled(false);
      return true;
    }
    return false;
  }

  virtual void receive_value_from_tb() {
    std::ostringstream msg;
    base::receive_value_from_tb();
    IT& v = this->get_latest_value();
    Tclass   t;

    if (!in_fifo->nb_get(t)) {
      // If distributed pipelining requires disabling inputs on empty to prove flushing occurs
      // then set an arbitrarily large wait cycles on the transactor resources (each time we hit empty)
      if (_disable_on_empty) {
        _input_disabled = true;
        for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
          ((*i)->_mc_rsc_block)->set_is_disabled(true);
      }
      if (!this->_empty && this->log_event(MC_TRANSACTOR_EMPTY)) {
        if (_disable_on_empty) {
           msg << "MC_TRANSACTOR_EMPTY - fifo is empty (assuming pipe is flushing and disabling input) @ " << sc_time_stamp();
        } else {
           msg << "MC_TRANSACTOR_EMPTY - fifo is empty (assuming pipe is flushing) @ " << sc_time_stamp();
        }
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }
      this->_empty = true;
    } else {
// MC_INPUT_TRANSACTOR
      if (this->_empty && this->log_event(MC_TRANSACTOR_UNDERFLOW)) {
        std::ostringstream msg3;
        if (!_input_disabled) {
          msg3 << "MC_TRANSACTOR_UNDERFLOW - fifo underflow detected (garbage has been read) @ " << sc_time_stamp();
        } else {
          msg3 << "MC_TRANSACTOR_UNDERFLOW - fifo underflow detected (garbage has been read and input re-enabled) @ " << sc_time_stamp();
        }
        SC_REPORT_WARNING(this->name(), msg3.str().c_str());
      }
      if (_input_disabled) {
          _input_disabled = false;
          for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i) {
            ((*i)->_mc_rsc_block)->set_is_disabled(false);
          }
      }
      this->_empty = false;

     if (this->_auto_wait_limit == 0) {
      int user_wait_cycles = 0;
      mc_wait_ctrl wait_ctrl;
      if (in_wait_ctrl_fifo->nb_peek(wait_ctrl)) {
         if (wait_ctrl.iteration == _ld_count) {
            in_wait_ctrl_fifo->nb_get(wait_ctrl); // pop
            user_wait_cycles = wait_ctrl.cycles;
            if (this->log_event(MC_TRANSACTOR_WAIT)) {
               std::ostringstream msg2;
               msg2 << "(input) MC_TRANSACTOR_WAIT - Initiating user specified wait: " << wait_ctrl << " @ " << sc_time_stamp();
               SC_REPORT_INFO(this->name(), msg2.str().c_str());
            }
            if (user_wait_cycles) {
               for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
                  ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,0); // iodir=0 means input
            }
         }
      }

      if (!user_wait_cycles) {
         // clear out wait_ctrl
         wait_ctrl.clear();
         for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
            ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,0);
      }

     } else {
      // Automatic random stall insertion based on active I/O
      mc_wait_ctrl auto_wait_ctrl;
      auto_wait_ctrl.iteration = _ld_count;
      auto_wait_ctrl.stopat = _ld_count+5;
      auto_wait_ctrl.cycles = 5;
      auto_wait_ctrl.mode = mc_wait_ctrl::RANDOM;
      auto_wait_ctrl.interval = 0;
      auto_wait_ctrl.elements = mc_wait_ctrl::INITIAL;
      for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i) {
        bool messaged = false;
        if (1
            && !((*i)->_mc_rsc_block)->get_is_disabled()
            && !((*i)->_mc_rsc_block)->wait_signal_active(0)
            && ( ( ((*i)->_mc_rsc_block)->stall_count(0) < this->_auto_wait_limit ) || ( this->_auto_wait_limit == -1) )
            && ((*i)->_mc_rsc_block)->io_request(0) 
        ) {
          ((*i)->_mc_rsc_block)->wait_cycles(auto_wait_ctrl,0); // iodir=0 means input
          ((*i)->_mc_rsc_block)->incr_stall_count(0); // iodir=0 means input
          if (!messaged && this->log_event(MC_TRANSACTOR_WAIT)) {
            std::ostringstream msg;
            msg << "(input) MC_TRANSACTOR_WAIT - Initiating automatic I/O wait: " << auto_wait_ctrl << " @ " << sc_time_stamp();
            SC_REPORT_INFO(this->name(), msg.str().c_str());
          }
          messaged = true;
        }
      }
     }

      ++_ld_count;

      if (this->log_event(MC_TRANSACTOR_LOAD)) {
        msg << "MC_TRANSACTOR_LOAD - fifo loaded value #" << _ld_count << " : " << t << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }

      type_to_vector(t, this->Telemcnt * Telemwidth, v);


      if (this->get_attribute("MC_TRANSACTOR_COUNT")) {
         if (((sc_attribute<int>*)(this->get_attribute("MC_TRANSACTOR_COUNT")))->value == _ld_count) {
           std::ostringstream msg;
           msg << "MC_TRANSACTOR_COUNT - fifo transaction # " << _ld_count << " reached @ " << sc_time_stamp();
           SC_REPORT_INFO(this->name(), msg.str().c_str());
           mc_transactor_stop_debugger();
         }
      }
    }
  }

private:
  int  _ld_count;
  bool _disable_on_empty;
  bool _input_disabled;
};

////////////////////////////////////////////////////////////////////////////////
// mc_output_transactor
////////////////////////////////////////////////////////////////////////////////

template <class Tclass, int Telemwidth, bool Tsigned>
class mc_output_transactor : virtual public mc_transactor<Tclass,Telemwidth,Tsigned>
{
public:
  typedef tlm::tlm_nonblocking_put_if<Tclass> OUT_IF;

  sc_port<OUT_IF> out_fifo;
  sc_port< tlm::tlm_get_peek_if< mc_wait_ctrl > > out_wait_ctrl_fifo;

  mc_output_transactor(const sc_module_name& name, int BaseAddr, int WordWidth, int BaseBit, int auto_wait_limit=0)
    : mc_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
    , out_fifo("out_fifo")
    , _dmp_count(0)
  {}

protected:
  typedef mc_transactor<Tclass,Telemwidth,Tsigned>        base;
  typedef typename base::objblock           objblock;
  typedef typename base::IT              IT;
  typedef typename base::objblocks_iterator obi;

  void receive_value_from_objblock(objblock* blk) {
    IT& v = this->get_value(blk->_trans);
    blk->transfer(&mc_rsc_block_base::extract_value, blk->_lbr, blk->_fbr, v);

    if (this->log_event(MC_TRANSACTOR_WRITE)) {
      std::ostringstream msg;
      msg << "MC_TRANSACTOR_WRITE - received value #" << blk->_trans << " from " << blk->_name << " @ " << sc_time_stamp();
      SC_REPORT_INFO(this->name(), msg.str().c_str());
    }
  }

  virtual void send_value_to_tb() {
   if (this->_auto_wait_limit == 0) {
    //Modified wait controls
    // This function "send_value_to_tb" is not called before the first value from the dut is available.
    //  This means the first dut capture will not use the wait controls.
    // The functionality is now similar to the input_channel transactor (below)
    //
    mc_wait_ctrl wait_ctrl;
    if (out_wait_ctrl_fifo->nb_peek(wait_ctrl)) {
       if (wait_ctrl.stopat <= _dmp_count)
       {
          out_wait_ctrl_fifo->nb_get(wait_ctrl); // pop
          if (this->log_event(MC_TRANSACTOR_WAIT)) {
             std::ostringstream msg2;
             msg2 << "(output) MC_TRANSACTOR_WAIT - Clearing user specified wait: " << wait_ctrl << " @ " << sc_time_stamp();
             SC_REPORT_INFO(this->name(), msg2.str().c_str());
          }
          wait_ctrl.clear();
          for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
             ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,1);
       }
    }
    // next
    if (out_wait_ctrl_fifo->nb_peek(wait_ctrl)) {
       if (wait_ctrl.iteration == _dmp_count)
       {
          if (this->log_event(MC_TRANSACTOR_WAIT)) {
             std::ostringstream msg2;
             msg2 << "(output) MC_TRANSACTOR_WAIT - Initiating user specified wait: " << wait_ctrl << " @ " << sc_time_stamp();
             SC_REPORT_INFO(this->name(), msg2.str().c_str());
          }
          for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
             ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,1); //iodir=1 means output
       }
    }
   } else {
    // Automatic random stall insertion based on active I/O
    mc_wait_ctrl auto_wait_ctrl;
    auto_wait_ctrl.iteration = _dmp_count;
    auto_wait_ctrl.stopat = _dmp_count+5;
    auto_wait_ctrl.cycles = 5;
    auto_wait_ctrl.mode = mc_wait_ctrl::RANDOM;
    auto_wait_ctrl.interval = 0;
    auto_wait_ctrl.elements = mc_wait_ctrl::INITIAL;
    for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i) {
      bool messaged = false;
      if (1
          && !((*i)->_mc_rsc_block)->get_is_disabled()
          && !((*i)->_mc_rsc_block)->wait_signal_active(1)
          && ( ( ((*i)->_mc_rsc_block)->stall_count(1) < this->_auto_wait_limit ) || ( this->_auto_wait_limit == -1) )
          && ((*i)->_mc_rsc_block)->io_request(1) 
      ) {
        ((*i)->_mc_rsc_block)->wait_cycles(auto_wait_ctrl,1); // iodir=1 means output
        ((*i)->_mc_rsc_block)->incr_stall_count(1); // iodir=1 means output
        if (!messaged && this->log_event(MC_TRANSACTOR_WAIT)) {
          std::ostringstream msg;
          msg << "(output) MC_TRANSACTOR_WAIT - Initiating automatic I/O wait: " << auto_wait_ctrl << " @ " << sc_time_stamp();
          SC_REPORT_INFO(this->name(), msg.str().c_str());
        }
        messaged = true;
      }
    }
   }

    IT& v = this->get_oldest_value();
    Tclass   t;

    vector_to_type(v, Tsigned, &t);
    if (!out_fifo->nb_put(t)) {
      std::ostringstream msg;
      msg << "MC_TRANSACTOR_FULL - fifo is full @ " << sc_time_stamp();
      SC_REPORT_ERROR(this->name(), msg.str().c_str());
    } else {
      if (this->log_event(MC_TRANSACTOR_DUMP)) {
        std::ostringstream msg;
        msg << "MC_TRANSACTOR_DUMP - fifo dumped value " << t << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }
      if (this->get_attribute("MC_TRANSACTOR_COUNT")) {
        if (((sc_attribute<int>*)(this->get_attribute("MC_TRANSACTOR_COUNT")))->value == _dmp_count) {
          std::ostringstream msg;
          msg << "MC_TRANSACTOR_COUNT - fifo transaction # "
              << _dmp_count << " reached @ " << sc_time_stamp();
          SC_REPORT_INFO(this->name(), msg.str().c_str());
          mc_transactor_stop_debugger();
        }
      }
      _dmp_count++;
    }

    base::send_value_to_tb();
  }

private:
  int  _dmp_count;
};

////////////////////////////////////////////////////////////////////////////////
// mc_inout_transactor
////////////////////////////////////////////////////////////////////////////////

template <class Tclass, int Telemwidth, bool Tsigned>
struct mc_inout_transactor : mc_input_transactor<Tclass,Telemwidth,Tsigned>,  mc_output_transactor<Tclass,Telemwidth,Tsigned>
{
  mc_inout_transactor(const sc_module_name& name, int BaseAddr, int WordWidth, int BaseBit, int auto_wait_limit=0)
    : mc_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
    , mc_input_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
    , mc_output_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
  {}
};

////////////////////////////////////////////////////////////////////////////////
// mc_channel_input_transactor
////////////////////////////////////////////////////////////////////////////////

template <class Tclass, int Telemwidth, bool Tsigned>
class mc_channel_input_transactor : virtual public mc_transactor<Tclass,Telemwidth,Tsigned>
{
public:
  //typedef tlm::tlm_nonblocking_get_if<Tclass> IN_IF;
  typedef tlm::tlm_fifo_get_if<Tclass> IN_IF;

  sc_port<IN_IF> in_fifo;
  sc_port< tlm::tlm_get_peek_if< mc_wait_ctrl > > in_wait_ctrl_fifo;
  sc_port< tlm::tlm_get_peek_if< int > > sizecount_fifo;

  SC_HAS_PROCESS(mc_channel_input_transactor);
  mc_channel_input_transactor(const sc_module_name& name, int BaseAddr, int WordWidth, int BaseBit, bool disable_on_empty=false, int auto_wait_limit=0)
    : mc_transactor<Tclass,Telemwidth,Tsigned>(name,BaseAddr,WordWidth,BaseBit,auto_wait_limit)
    , in_fifo("in_fifo")
    , _ld_count(0)
    , _disable_on_empty(disable_on_empty)
    , _input_disabled(false)
  {
    sizelz_count = 0;
    _input_fifo_size = 0;
  }

  virtual unsigned int input_fifo_size() { return _input_fifo_size; }

  // Called on every rising edge of sizelz signal of mgc_chan_in component
  int count_sizelz_event(bool hi) {
    if (hi) {
      sizelz_count++;
      if (this->log_event(MC_TRANSACTOR_SIZE)) {
          std::ostringstream msg1;
          msg1 << "MC_TRANSACTOR_SIZE - H/W sizelz rising edge detected, sizelz_count is now " << sizelz_count << " @ " << sc_time_stamp();
          SC_REPORT_INFO(this->name(), msg1.str().c_str());
      }
      int software_size_count;
      if (sizecount_fifo->nb_peek(software_size_count)) { // value ready to peek
        if (software_size_count > 0) {
          if (sizelz_count >= software_size_count) {
            sizelz_count = 0;
            sizecount_fifo->nb_get(software_size_count); // pop
          }
        }
      }
    }
    _input_fifo_size = (this->_empty ? 0 : 1) + in_fifo->used(); // update cached value of input fifo size on each transaction_done prior to getting values
    if (hi) {
      if (this->log_event(MC_TRANSACTOR_SIZE)) {
        std::ostringstream msg1;
        msg1 << "MC_TRANSACTOR_SIZE - updating input fifo size to be " << _input_fifo_size << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg1.str().c_str());
      }
      // if going from _empty to non-zero _input_fifo_size, then force a transaction done
      if (this->_empty && (_input_fifo_size > 0)) {
        if (this->log_event(MC_TRANSACTOR_SIZE)) {
          std::ostringstream msg1;
          msg1 << "MC_TRANSACTOR_SIZE - forcing transaction update @ " << sc_time_stamp();
          SC_REPORT_INFO(this->name(), msg1.str().c_str());
        }
        this->_empty = false; // to avoid GARBAGE message
        this->_pending_triosy=true;
        for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
          ((*i)->_mc_rsc_block)->exchange_value(false);
      }
    }
    return sizelz_count; // in_fifo->used(); // return the current size of the data in fifo
  }
  int sizelz_count;
  int _input_fifo_size;

protected:
  typedef mc_transactor<Tclass,Telemwidth,Tsigned>        base;
  typedef typename base::objblock           objblock;
  typedef typename base::IT              IT;
  typedef typename base::objblocks_iterator obi;

  virtual void initiate_driving_value_adjustments(int blk_id, int RH, int BaseAddr, int CH, int BaseBit) {
    objblock* blk = this->_objblocks[blk_id];
    sc_assert(blk->_mc_rsc_block->is_combinational());
    if (!this->_empty && CH >= this->_BaseBit && BaseBit <= this->_CH && RH >= blk->_fbr && BaseAddr <= blk->_lbr && blk->_triosy->read() == SC_LOGIC_1) {
      IT& v = this->get_value(blk->_trans + 1);
      if (!this->_empty) blk->transfer(&mc_rsc_block_base::adjust_driving_value, RH, BaseAddr, v);
    }
  }

  virtual void send_value_to_objblock(objblock* blk) {
    IT& v = this->get_value(blk->_trans);
    if (!this->_empty) {
      blk->transfer(&mc_rsc_block_base::inject_value, blk->_lbr, blk->_fbr, v);

      if (this->log_event(MC_TRANSACTOR_READ)) {
        std::ostringstream msg;
        msg << "MC_TRANSACTOR_READ - sending value #" << blk->_trans << " to " << blk->_name << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }
    }
  }

  virtual bool check_disabled_input() {
    if (_disable_on_empty && _input_disabled && (in_fifo->used()>0)) {
      for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
        ((*i)->_mc_rsc_block)->set_is_disabled(false);
      return true;
    }
    return false;
  }

  virtual void receive_value_from_tb() {
    std::ostringstream msg;
    base::receive_value_from_tb();
    IT& v = this->get_latest_value();
    Tclass   t;

    if (!in_fifo->nb_get(t)) {
      // If distributed pipelining requires disabling inputs on empty to prove flushing occurs
      // then set an arbitrarily large wait cycles on the transactor resources (each time we hit empty)
      if (_disable_on_empty) {
        _input_disabled = true;
        for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
          ((*i)->_mc_rsc_block)->set_is_disabled(true);
      }
      if (!this->_empty && this->log_event(MC_TRANSACTOR_EMPTY)) {
        if (_disable_on_empty) {
           msg << "MC_TRANSACTOR_EMPTY - fifo is empty (assuming pipe is flushing and disabling input) @ " << sc_time_stamp();
        } else {
           msg << "MC_TRANSACTOR_EMPTY - fifo is empty (assuming pipe is flushing) @ " << sc_time_stamp();
        }
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }
      this->_empty = true;
    } else {
      if (this->_empty && this->log_event(MC_TRANSACTOR_UNDERFLOW)) {
        std::ostringstream msg3;
        if (!_input_disabled) {
          msg3 << "MC_TRANSACTOR_UNDERFLOW - fifo underflow detected (garbage has been read) @ " << sc_time_stamp();
        } else {
          msg3 << "MC_TRANSACTOR_UNDERFLOW - fifo underflow detected (garbage has been read and input re-enabled) @ " << sc_time_stamp();
        }
        SC_REPORT_WARNING(this->name(), msg3.str().c_str());
      }
      this->_input_disabled = false;
      this->_empty = false;

     if (this->_auto_wait_limit == 0) {
      mc_wait_ctrl wait_ctrl;
      if (in_wait_ctrl_fifo->nb_peek(wait_ctrl)) {
         if (wait_ctrl.stopat <= _ld_count) {
            in_wait_ctrl_fifo->nb_get(wait_ctrl); // pop
            if (this->log_event(MC_TRANSACTOR_WAIT)) {
               std::ostringstream msg2;
               msg2 << "(input) MC_TRANSACTOR_WAIT - Clearing user specified wait: " << wait_ctrl << " @ " << sc_time_stamp();
               SC_REPORT_INFO(this->name(), msg2.str().c_str());
            }
            // clear out wait_ctrl
            wait_ctrl.clear();
            for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
               ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,0);
         }
      }
      if (in_wait_ctrl_fifo->nb_peek(wait_ctrl)) {
         if (wait_ctrl.iteration == _ld_count) {
            if (this->log_event(MC_TRANSACTOR_WAIT)) {
               std::ostringstream msg2;
               msg2 << "(input) MC_TRANSACTOR_WAIT - Initiating user specified wait: " << wait_ctrl << " @ " << sc_time_stamp();
               SC_REPORT_INFO(this->name(), msg2.str().c_str());
            }
            for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i)
               ((*i)->_mc_rsc_block)->wait_cycles(wait_ctrl,0); // iodir=0 means input
         }
      }
     } else {
      // Automatic random stall insertion based on active I/O
      mc_wait_ctrl auto_wait_ctrl;
      auto_wait_ctrl.iteration = _ld_count;
      auto_wait_ctrl.stopat = _ld_count+5;
      auto_wait_ctrl.cycles = 5;
      auto_wait_ctrl.mode = mc_wait_ctrl::RANDOM;
      auto_wait_ctrl.interval = 0;
      auto_wait_ctrl.elements = mc_wait_ctrl::INITIAL;
      for (obi i = this->_objblocks.begin(); i != this->_objblocks.end(); ++i) {
        bool messaged = false;
        if (1
            && !((*i)->_mc_rsc_block)->get_is_disabled()
            && !((*i)->_mc_rsc_block)->wait_signal_active(0)
            && ( ( ((*i)->_mc_rsc_block)->stall_count(0) < this->_auto_wait_limit ) || ( this->_auto_wait_limit == -1) )
            && ((*i)->_mc_rsc_block)->io_request(0) 
        ) {
          ((*i)->_mc_rsc_block)->wait_cycles(auto_wait_ctrl,0); // iodir=0 means input
          ((*i)->_mc_rsc_block)->incr_stall_count(0); // iodir=0 means input
          if (!messaged && this->log_event(MC_TRANSACTOR_WAIT)) {
            std::ostringstream msg;
            msg << "(input) MC_TRANSACTOR_WAIT - Initiating automatic I/O wait: " << auto_wait_ctrl << " @ " << sc_time_stamp();
            SC_REPORT_INFO(this->name(), msg.str().c_str());
          }
          messaged = true;
        }
      }
     }

      ++_ld_count;

      _input_fifo_size = (this->_empty ? 0 : 1) + in_fifo->used(); // update cached value of input fifo size on each transaction_done prior to getting values
      if (this->log_event(MC_TRANSACTOR_LOAD)) {
        msg << "MC_TRANSACTOR_LOAD - fifo loaded value #" << _ld_count << " : " << t << " @ " << sc_time_stamp();
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }

      type_to_vector(t, this->Telemcnt * Telemwidth, v);


      if (this->get_attribute("MC_TRANSACTOR_COUNT")) {
         if (((sc_attribute<int>*)(this->get_attribute("MC_TRANSACTOR_COUNT")))->value == _ld_count) {
           std::ostringstream msg;
           msg << "MC_TRANSACTOR_COUNT - fifo transaction # " << _ld_count << " reached @ " << sc_time_stamp();
           SC_REPORT_INFO(this->name(), msg.str().c_str());
           mc_transactor_stop_debugger();
         }
      }
    }
  }

private:
  int  _ld_count;
  bool _disable_on_empty;
  bool _input_disabled;
};

////////////////////////////////////////////////////////////////////////////////
// mc_rsc_block_data_traits
//    abstract array of sc_lv<...> or sc_logic with read and write acces
//    to (slices) of words
//////////////////////////////////////////////////////////////////////////////

template <int Tblkwidth> struct mc_rsc_block_data_traits;

template <int Tblkwidth>
struct mc_rsc_block_data_traits {
  typedef sc_lv<Tblkwidth>             data_type;
  typedef sc_inout< sc_lv<Tblkwidth> > inout_port_type;
  typedef sc_in< sc_lv<Tblkwidth> >    in_port_type;
  typedef sc_out< sc_lv<Tblkwidth> >   out_port_type;
  enum {width = Tblkwidth};

  template <int Tbwidth>
  static data_type extract_value(const sc_lv<Tbwidth>& inp, int idx) {
    return inp.range(idx + Tblkwidth-1, idx);
  }

  template <int Tbwidth>
  static void inject_value(sc_lv<Tbwidth>& lhs, int idx, const data_type& rhs) {
    lhs.range(idx + Tblkwidth-1, idx) = rhs;
  }
};

#ifdef USE_SC_LOGIC_FOR_SINGLE_BIT_VECTORS
template <>
struct mc_rsc_block_data_traits<1> {
  typedef sc_logic           data_type;
  typedef sc_inout<sc_logic> inout_port_type;
  typedef sc_in<sc_logic>    in_port_type;
  typedef sc_out<sc_logic>   out_port_type;
  enum {width = 1};

  template <int Tbwidth>
  static data_type extract_value(const sc_lv<Tbwidth>& inp, int idx) {
    return inp[idx];
  }

  template <int Tbwidth>
  static void inject_value(sc_lv<Tbwidth>& lhs, int idx, const data_type& rhs) {
    lhs[idx] = rhs;
  }
};
#endif

////////////////////////////////////////////////////////////////////////////////
// mc_rsc_block_data
//   class holding the bit-vector data corresponding to a resource block
////////////////////////////////////////////////////////////////////////////////

template <int Twidth, int Trows>
class mc_rsc_block_data
{
protected:
  typedef mc_rsc_block_data_traits<Twidth>      traits;
  typedef typename traits::data_type       data_type;
  typedef typename traits::inout_port_type port_type;
  enum { COLS = traits::width };

public:
  void set_value(int row, int idx_lhs, int width, sc_lv_base& rhs, int idx_rhs)
  {
    sc_assert(0 <= row && row < Trows);
    _data.range(row*COLS + idx_lhs + width-1, row*COLS + idx_lhs) =
      rhs.range(idx_rhs + width-1, idx_rhs);
  }

  void get_value(int row, int idx_rhs, int width, sc_lv_base& lhs, int idx_lhs)
  {
    sc_assert(0 <= row && row < Trows);
    lhs.range(idx_lhs + width-1, idx_lhs) =
      _data.range(row*COLS + idx_rhs + width-1, row*COLS + idx_rhs);
  }

protected:

  void zero_data() { _data = 0; }

  data_type read_row(int row) const {
    sc_assert(0 <= row && row < Trows);

    return traits::extract_value(_data, row*COLS);
  }

  void write_row(int row, const data_type& rhs) {
    sc_assert(0 <= row && row < Trows);

    traits::inject_value(_data, row*COLS, rhs);
  }

private:
  sc_lv<COLS*Trows> _data;
};

////////////////////////////////////////////////////////////////////////////////
// mc_rsc_memory
//    Derived resource class suitable for basic memory models
//
//  This is the generic memory resource. Any of the input ports may
//  be registered. If sequential delay is zero, then there might be a
//  combinational process sensitive to rd_addr, wr_addr, wr_data and
//  we_en, depending on these ports being registered and TwriteFirst
//  being true.
//
////////////////////////////////////////////////////////////////////////////////
template <int Twords, int Twidth, int Taddrwidth,
          int TrdPorts, int TwrPorts,
          int Tregistered, int TseqDelay,
          bool TwriteFirst, class TweType>
class mc_rsc_memory
  : public mc_transactor_event_module
  , public mc_rsc_block_base
  , protected mc_rsc_block_data<Twidth,Twords>
{
  enum { RD_ADDR_REG = (Tregistered >> 0 & 1),
         WR_DATA_REG = (Tregistered >> 1 & 1),
         WR_ADDR_REG = (Tregistered >> 2 & 1),
         WR_EN_REG   = (Tregistered >> 3 & 1),
  };

public:
  sc_in< bool >                     clk;
  //sc_in< sc_logic >                 a_rst;
  //sc_in< sc_logic >                 s_rst;
  //sc_in< sc_logic >                 en;
  sc_out< sc_lv<TrdPorts*Twidth> >    rd_data;
  sc_in< sc_lv<TrdPorts*Taddrwidth> > rd_addr;
  sc_in< sc_lv<TwrPorts*Twidth> >     wr_data;
  sc_in< sc_lv<TwrPorts*Taddrwidth> > wr_addr;
  sc_in< TweType >                   wr_en;

  SC_HAS_PROCESS(mc_rsc_memory);
  mc_rsc_memory(const sc_module_name& name, bool phase)
    : mc_transactor_event_module(name)
    , clk("clk")
//     , a_rst("a_rst")
//     , s_rst("s_rst")
//     , en("en")
    , rd_data("rd_data")
    , rd_addr("rd_addr")
    , wr_data("wr_data")
    , wr_addr("wr_addr")
    , wr_en("wr_en")
  {
    SC_METHOD(at_active_clock_edge);
    sensitive << (phase ? clk.pos() : clk.neg()); // active clock edge
    dont_initialize();

    if (TseqDelay == 0) {
      SC_METHOD(update_rd_data_comb);
      sensitive << _value_changed;                // transactor exchanged current value
      if (RD_ADDR_REG == 0)
        sensitive << rd_addr;
      if (TwriteFirst && WR_ADDR_REG == 0)
        sensitive << wr_addr;
      if (TwriteFirst && WR_DATA_REG == 0)
        sensitive << wr_data;
      if (TwriteFirst && WR_EN_REG == 0)
        sensitive << wr_en;
      dont_initialize();
    }
  }

  virtual void start_of_simulation() { reset_memory(); }

  virtual void inject_value(int row, int idx_lhs, int width, sc_lv_base& rhs, int idx_rhs) {
    this->set_value(row, idx_lhs, width, rhs, idx_rhs);
    if (TseqDelay == 0) {
      for (int i = 0; i < TrdPorts; ++i) {
        const int addr = get_rd_addr(RD_ADDR_REG ? _rd_addr[0] : rd_addr.read(), false);
        if (row == addr) {
          _value_changed.notify(SC_ZERO_TIME);
          break;
        }
      }
    }
  }

  virtual void extract_value(int row, int idx_rhs, int width, sc_lv_base& lhs, int idx_lhs) {
    this->get_value(row, idx_rhs, width, lhs, idx_lhs);
  }

private:

  void at_active_clock_edge() {
    // capture in ports
    shift_and_add(_rd_addr, RD_ADDR_REG+1, rd_addr.read());
    shift_and_add(_wr_data, WR_DATA_REG+1, wr_data.read());
    shift_and_add(_wr_addr, WR_ADDR_REG+1, wr_addr.read());
    shift_and_add(_wr_en,   WR_EN_REG+1,   wr_en.read());

    for (int i = 1; i < TseqDelay; ++i)
      _rd_data[i-1] = _rd_data[i];

    if (TseqDelay > 0 && !TwriteFirst) {
      for (int i = 0; i < TrdPorts; ++i)
        copy_mem_value(_rd_addr[0], i, true, _rd_data[TseqDelay-1]);
    }

    // update memory contents
    for (int i = 0; i < TwrPorts; ++i) {
      if (wr_enabled(_wr_en[0], i)) {
        const int addr = get_wr_addr(_wr_addr[0], i, true);
        if (addr >= 0)
          inject_value(addr, 0, Twidth, _wr_data[0], i*Twidth);
      }
    }

    if (TseqDelay > 0 && TwriteFirst) {
      for (int i = 0; i < TrdPorts; ++i)
        copy_mem_value(_rd_addr[0], i, true, _rd_data[TseqDelay-1]);
    }

    if (TseqDelay > 0)
      rd_data = _rd_data[0];
    else
      _value_changed.notify(SC_ZERO_TIME);

    this->exchange_value(false);
  }

  void update_rd_data_comb() {
    sc_lv<TrdPorts*Twidth> val;
    for (int i = 0; i < TrdPorts; ++i) {
      const int raddr = copy_mem_value(RD_ADDR_REG ? _rd_addr[1] : rd_addr.read(), i, true, val);

      if (raddr >= 0 && TwriteFirst) {
        for (int j = 0; j < TwrPorts; ++j) {
          if (wr_enabled(WR_EN_REG ? _wr_en[1] : wr_en.read(), j)) {
            const int waddr = get_wr_addr(WR_ADDR_REG ? _wr_addr[1] : wr_addr.read(), j, false);
            if (waddr == raddr) {
              const sc_lv<TwrPorts*Twidth>& wdatav = WR_DATA_REG ? _wr_data[1] : wr_data.read();
              val.range(i*Twidth+Twidth-1, i*Twidth) = wdatav.range(j*Twidth+Twidth-1, j*Twidth);
            }
          }
        }
      }
    }
    rd_data = val;
  }

  int copy_mem_value(const sc_lv<Taddrwidth*TrdPorts>& raddrv, int i, bool warn, sc_lv<Twidth*TrdPorts>& val) {
    const int raddr = get_rd_addr(raddrv, i, warn);
    if (raddr >= 0) {
      this->get_value(raddr, 0, Twidth, val, i*Twidth);
      return raddr;
    } else {
      const static sc_lv<Twidth> all_x(SC_LOGIC_X);
      val.range(i*Twidth+Twidth-1, i*Twidth) = all_x;
      return raddr;
    }
  }

  int get_rd_addr(const sc_lv<Taddrwidth*TrdPorts>& raddr,  int i, bool warn = false) {
    const sc_lv<Taddrwidth> vaddr = raddr.range(i*Taddrwidth+Taddrwidth-1, i*Taddrwidth);

    if (vaddr.is_01()) {
      const int addr = vaddr.to_uint();
      if (addr < 0 || addr >= Twords) {
        if (warn)
          SC_REPORT_WARNING(rd_addr.name(), "value out of range --- driving 'X's");
        return -1;
      } else {
        return addr;
      }
    } else {
      if (warn)
        SC_REPORT_WARNING(rd_addr.name(), "value contains 'X' or 'Z' --- driving 'X's");
      return -1;
    }
  }

  int get_wr_addr(const sc_lv<Taddrwidth*TwrPorts>& waddr,  int i, bool warn = false) {
    const sc_lv<Taddrwidth> vaddr = waddr.range(i*Taddrwidth+Taddrwidth-1, i*Taddrwidth);
    if (vaddr.is_01()) {
      const int addr = vaddr.to_uint();
      if (addr < 0 || addr >= Twords) {
        if (warn)
          SC_REPORT_WARNING(wr_addr.name(), "value out of range --- ignoring write request");
        return -1;
      } else {
        return addr;
      }
    } else {
      if (warn)
        SC_REPORT_WARNING(wr_addr.name(), "value contains 'X' or 'Z' --- ignoring write request");
      return -1;
    }
  }

  void reset_memory() {
    this->zero_data();
    for (int i = 0; i <= TseqDelay; ++i)
      _rd_data[i] = 0;
    for (int i = 0; i <= RD_ADDR_REG; ++i)
      _rd_addr[i] = 0;
    for (int i = 0; i <= WR_DATA_REG; ++i)
      _wr_data[i] = 0;
    for (int i = 0; i <= WR_ADDR_REG; ++i)
      _wr_addr[i] = 0;
    for (int i = 0; i <= WR_EN_REG; ++i)
      _wr_en[i] = 0;
  }

  template <class Tclass>
  static void shift_and_add(Tclass regs[], int size, const Tclass& val) {
    for (int i = 1; i < size; ++i) regs[i-1] = regs[i];
    regs[size-1] = val;
  }

  static bool wr_enabled(const sc_logic& we, int n) {
    sc_assert(n == 0);
    return we == SC_LOGIC_1;
  }

  static bool wr_enabled(const sc_lv<TwrPorts>& we, int n) {
    sc_assert(n < TwrPorts);
    return we[n] == SC_LOGIC_1;
  }

  sc_lv<TrdPorts*Twidth>     _rd_data[TseqDelay+1];
  sc_lv<TrdPorts*Taddrwidth> _rd_addr[RD_ADDR_REG+1];
  sc_lv<TwrPorts*Twidth>     _wr_data[WR_DATA_REG+1];
  sc_lv<TwrPorts*Taddrwidth> _wr_addr[WR_ADDR_REG+1];
  TweType                   _wr_en[WR_EN_REG+1];

  sc_event _value_changed; // memory value has changed the value
  sc_event _clk_skew_event;
};



////////////////////////////////////////////////////////////////////////////////
// mc_wire_trans_rsc_base
//    Base class for all wire and stream type wires
////////////////////////////////////////////////////////////////////////////////

template<int Twidth, int Trows>
class mc_wire_trans_rsc_base
  : public mc_transactor_event_module
  , public mc_rsc_block_base
  , protected mc_rsc_block_data<Twidth,Trows+1>
{
public:
  typedef typename mc_rsc_block_data<Twidth, Trows+1>::data_type data_type;
  typedef typename mc_rsc_block_data<Twidth, Trows+1>::port_type port_type;
  enum { COLS = mc_rsc_block_data<Twidth,Trows+1>::COLS };

  virtual void inject_value(int row, int idx_lhs, int width, sc_lv_base& rhs, int idx_rhs) {
    this->set_value(row, idx_lhs, width, rhs, idx_rhs);
    if (row == get_current_in_row()) _value_changed.notify();
  }

  virtual void extract_value(int row, int idx_rhs, int width, sc_lv_base& lhs, int idx_lhs) {
    this->get_value(row, idx_rhs, width, lhs, idx_lhs);
  }


  SC_HAS_PROCESS(mc_wire_trans_rsc_base);
  mc_wire_trans_rsc_base(const sc_module_name& name, bool phase, double holdtime=0.0)
    : mc_transactor_event_module(name)
    , _phase(phase)
    , _holdtime(holdtime)
  {
    _wait_cycles_cntr[0] = 0;
    _wait_cycles_cntr[1] = 0;
    _wait_io_request[0] = true;
    _wait_io_request[1] = true;
  }

  virtual void start_of_simulation() {
    if ((_holdtime == 0.0) && this->get_attribute("CLK_SKEW_DELAY")) {
      _holdtime = ((sc_attribute<double>*)(this->get_attribute("CLK_SKEW_DELAY")))->value;
    }
//#ifdef DEBUG
    if (_holdtime > 0) {
      std::ostringstream msg;
      msg << "MC_TRANSACTOR_STARTUP - CLK_SKEW_DELAY = " << _holdtime << " ps @ " << sc_time_stamp();
      SC_REPORT_INFO(this->name(), msg.str().c_str());
    }
//#endif
    this->zero_data(); reset_stream();
  }
  virtual void reset_stream() { _in_row = _out_row = 0; }
  virtual bool is_stream() { return (Trows > 1); }
  virtual bool bus_transfer_required() { return false; }
  virtual void begin_transfer() { _value_changed.notify(); }
  virtual void set_is_disabled(bool is_disabled) {
    this->_is_disabled = is_disabled; drive_wait_signals(!is_disabled,0); _wait_cycles_changed[0].notify();
  }

  // If the logging atttributes are not applied to the block rsc then check the attached transactor/s
  virtual bool log_event(mc_transactor_event event_mask) {
    if (this->get_attribute("MC_TRANSACTOR_EVENT"))
      return (((sc_attribute<int>*)(this->get_attribute("MC_TRANSACTOR_EVENT")))->value & event_mask);
    return this->trans_log_event(event_mask);
  }
  // Wait controller methods
  virtual void wait_cycles(mc_wait_ctrl wait_ctrl,unsigned short iodir) {
    _wait_ctrl[iodir] = wait_ctrl; _wait_elem_cntr[iodir] = 0;
    _wait_random_interval[iodir] = (_wait_ctrl[iodir].interval > 0) ? scverify_rand(_wait_ctrl[iodir].interval) : 0;
    start_wait_count(iodir);
  }
  void start_wait_count(unsigned short iodir) {
    int i = ((_wait_ctrl[iodir].elements == mc_wait_ctrl::RANDOM_INTERVAL) ? _wait_random_interval[iodir] : _wait_ctrl[iodir].interval)+1;
    if ((_wait_ctrl[iodir].elements < mc_wait_ctrl::FIXED_INTERVAL) ||  ((_wait_elem_cntr[iodir] % i) == 0)) {
      _wait_cycles_cntr[iodir] = (_wait_ctrl[iodir].mode == mc_wait_ctrl::UNIFORM) ? _wait_ctrl[iodir].cycles : scverify_rand(_wait_ctrl[iodir].cycles);
      _wait_random_interval[iodir] = (_wait_ctrl[iodir].interval > 0) ? scverify_rand(_wait_ctrl[iodir].interval) : 0;
      _wait_io_request[iodir] = !_wait_cycles_cntr[iodir];
    }
    _wait_elem_cntr[iodir]++;
  }
  void repeat_wait(unsigned short iodir)  { if (_wait_ctrl[iodir].elements != mc_wait_ctrl::INITIAL) start_wait_count(iodir); }
  bool is_waiting(unsigned short iodir)   { return (_wait_cycles_cntr[iodir] > 0); }
  void wait_controller(unsigned short iodir) {
    if (get_is_disabled()) {
      _wait_io_request[iodir] = true;
      drive_wait_signals(true,iodir);
    } else {
      if (_wait_cycles_cntr[iodir] <= 0) {
        _wait_io_request[iodir] |= (io_request(iodir) && wait_signal_active(iodir));
        if (!_wait_io_request[iodir] && this->log_event(MC_TRANSACTOR_WAIT)) {
          std::ostringstream msg;
          msg << (iodir? "(output)": "(input)");
          msg << " MC_TRANSACTOR_WAIT - IO request did not occur within user specified wait ending" << " @ " << sc_time_stamp();
          SC_REPORT_WARNING(this->name(), msg.str().c_str());
          _wait_io_request[iodir] = true;
        }
        drive_wait_signals(false,iodir);
      } else {
        _wait_cycles_cntr[iodir]--;
        _wait_io_request[iodir] |= (io_request(iodir) && wait_signal_active(iodir));
        drive_wait_signals(true,iodir);
      }
    }
  }

  void at_active_clk() {
    if (_holdtime > 0)
      this->_clk_skew_event.notify(_holdtime, SC_PS);
    else
      this->_clk_skew_event.notify();
  }

  // Derived transactor resource classes must override these virtual functions
  virtual void drive_wait_signals(bool wait_active, unsigned short iodir) {
#ifdef DEBUG
    std::cout << "Info: " << this->name() << ": mc_wire_trans_rsc_base::drive_wait_signals() not overloaded in derived class" << std::endl;
#endif
  }

  sc_event _value_changed;
  sc_event _clk_skew_event;

protected:
  virtual int get_current_in_row()  const { return _in_row;  }
  virtual int get_current_out_row() const { return _out_row; }
  virtual bool incr_current_in_row()  { _in_row++;  if ((_in_row  != Trows) || (_wait_ctrl[0].iteration != _wait_ctrl[0].stopat)) repeat_wait(0/*input*/);  if (_in_row == Trows)  {_in_row=0;  return true;} else { return false; } }
  virtual bool incr_current_out_row() { _out_row++; if ((_out_row != Trows) || (_wait_ctrl[1].iteration != _wait_ctrl[1].stopat)) repeat_wait(1/*output*/); if (_out_row == Trows) {_out_row=0; return true;} else { return false; } }
  int _in_row;
  int _out_row;

  mc_wait_ctrl _wait_ctrl[2];
  sc_event _wait_cycles_changed[2];
  int _wait_cycles_cntr[2];
  int _wait_elem_cntr[2];
  int _wait_random_interval[2];
  bool _wait_io_request[2];
  const bool _phase;
  double _holdtime;
};



class ccs_sc_logic_to_sc_lv1_adapter : public sc_module
{
public:
  sc_in<sc_logic>   inSCALAR;
  sc_out<sc_lv<1> > outVECTOR;
  SC_HAS_PROCESS(ccs_sc_logic_to_sc_lv1_adapter);
  ccs_sc_logic_to_sc_lv1_adapter(const sc_module_name& name) : sc_module(name)
  {
    SC_METHOD(update);
    sensitive << inSCALAR;
    this->dont_initialize();
  }
  void update() {
    sc_lv<1> tmp;
    tmp[0] = inSCALAR.read();
    outVECTOR.write(tmp);
  }
};

class ccs_sc_lv1_to_sc_logic_adapter : public sc_module
{
public:
  sc_in<sc_lv<1> >   inVECTOR;
  sc_out<sc_logic>   outSCALAR;
  SC_HAS_PROCESS(ccs_sc_lv1_to_sc_logic_adapter);
  ccs_sc_lv1_to_sc_logic_adapter(const sc_module_name& name) : sc_module(name)
  {
    SC_METHOD(update);
    sensitive << inVECTOR;
    this->dont_initialize();
  }
  void update() {
    outSCALAR.write(inVECTOR.read()[0]);
  }
};


////////////////////////////////////////////////////////////////////////////////
// SystemC module to generate sync singals for SCVerify top
SC_MODULE(sync_generator) {
  sc_in<bool>          clk;
  sc_in<sc_logic>      rst;

  sc_out<sc_logic>     in_sync;
  sc_out<sc_logic>     out_sync;
  sc_out<sc_logic>     inout_sync;

  sc_inout<unsigned>   wait_for_init;
  sc_out<sc_logic>     catapult_start;
  sc_in<sc_logic>      catapult_done;
  sc_in<sc_logic>      catapult_ready;

  // generate sync here
  void generate() {
    if (!done_flag) {
      generate_sync();
    } else {
      generate_insync();
      generate_outsync();
    }
  }

  SC_HAS_PROCESS(sync_generator);

  sync_generator(const sc_module_name& name,
                 bool       _clock_phase,
                 bool       _top_loop_pipelined,
                 bool       _ready_flag,
                 bool       _done_flag,
                 long long  _latency,
                 long long  _csteps,
                 int        _init_interval)
    : sc_module(name)
    , clock_phase(_clock_phase)
    , top_loop_pipelined(_top_loop_pipelined)
    , ready_flag(_ready_flag)
    , done_flag(_done_flag)
    , latency(_latency)
    , csteps(_csteps)
    , init_interval(_init_interval)
  {
    SC_METHOD(generate);
    sensitive << clk << rst << catapult_ready << catapult_done;
  }

 private:
  void generate_sync() {
    static long long duration = latency - csteps + init_interval;
    static long long latest_write = csteps;
    static long long max_state = init_interval ? duration : latency;
    static long long initial_in_state = ((init_interval>0)&&top_loop_pipelined&&(latency!=csteps)) ? 0 : 1;
    static long long initial_out_state = init_interval ? init_interval-latest_write+1 : 1;
    static long long in_state  = initial_in_state;
    static long long out_state = initial_out_state;
    static sc_logic  last_done = SC_LOGIC_0;

    if (rst.read() == 1) {
      in_sync.write(SC_LOGIC_0);
      out_sync.write(SC_LOGIC_0);
      inout_sync.write(SC_LOGIC_0);
      catapult_start.write(SC_LOGIC_0);
      in_state  = initial_in_state;
      last_done = SC_LOGIC_1;
      out_state = initial_out_state;
    } else {
      if ( (clk.read() == clock_phase ) && (!wait_for_init) ) {
        if (out_state >= max_state) {
          out_sync.write(SC_LOGIC_1);
          inout_sync.write(SC_LOGIC_1);
          out_state = 0;
          if (!init_interval) last_done = SC_LOGIC_1;
          if (init_interval == 0) in_state = max_state; // force in_sync to align with out_sync
        } else {
          if ( (init_interval == 0) && (catapult_done.read() == 0) && (last_done == SC_LOGIC_1) )
            last_done = SC_LOGIC_0;
          out_sync.write(SC_LOGIC_0);
          inout_sync.write(SC_LOGIC_0);
        }
        out_state++;
      }

      if ( (clk.read() == clock_phase ) && (!wait_for_init) ) {
        if ( in_state >= max_state ) {
          in_sync.write(SC_LOGIC_1);
          inout_sync.write(SC_LOGIC_1);
          catapult_start.write(SC_LOGIC_1);
          in_state = 0;
        } else {
          if ( in_state == 1 ) {
            catapult_start.write(SC_LOGIC_1);
          }
          in_sync.write(SC_LOGIC_0);
          inout_sync.write(SC_LOGIC_0);
          catapult_start.write(SC_LOGIC_0);
        }
        in_state++;
      }
    }
  }

  void generate_insync(){
    static bool seen_first_ready_pulse = false;
    if (rst.read() == 1) {
      catapult_start.write(SC_LOGIC_0);
      in_sync.write(SC_LOGIC_0);
    } else {
      catapult_start.write(SC_LOGIC_1);
      if ( clk.read() == clock_phase ) {
        if (!ready_flag && done_flag) {
          in_sync.write(catapult_done.read());
        } else {
          if (seen_first_ready_pulse) {
            in_sync.write(catapult_ready.read());
          }
          if (catapult_ready.read() == SC_LOGIC_1) {
            if (!seen_first_ready_pulse) {
              seen_first_ready_pulse = true;
            }
          }
        }
      }
    }
  }

  void generate_outsync(){
    if (rst.read() == 1) {
      out_sync.write(SC_LOGIC_0);
      inout_sync.write(SC_LOGIC_0);
    } else {
      if ( clk.read() == clock_phase ) {
        out_sync.write(catapult_done.read());
        inout_sync.write(catapult_done.read());
      }
    }
  }

 private:
  bool      clock_phase;
  bool      top_loop_pipelined;
  bool      ready_flag;
  bool      done_flag;
  long long latency;
  long long csteps;
  int       init_interval;
};


#endif
