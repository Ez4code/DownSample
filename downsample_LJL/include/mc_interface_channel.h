#ifndef MC_INTERFACE_CHANNEL
#define MC_INTERFACE_CHANNEL

#include <systemc.h>
#if !defined(MTI_SYSTEMC) && !defined(NCSC)
#include <sysc/kernel/sc_spawn.h>
#include <sysc/kernel/sc_spawn_options.h>
#endif
#include <tlm.h>
#include "mc_typeconv.h"

////////////////////////////////////////////////////////////////////////////////
// TLM interface wires
////////////////////////////////////////////////////////////////////////////////

#undef MC_INTERFACE_CHANNEL_TRACE


template<typename Ex, typename In>
void mc_ifc_extern_to_intern(const Ex& ex, In& res);

template<typename Ex, typename In>
void mc_ifc_intern_to_extern(const In& in, bool issigned, Ex& res);

template<typename Ex, int W>
inline void mc_ifc_extern_to_intern(const Ex& ex, sc_lv<W>& res)
{
	type_to_vector(ex, W, res);
}

template<typename Ex, int W>
inline void mc_ifc_intern_to_extern(const sc_lv<W>& in, bool issigned, Ex& res)
{
	vector_to_type(in, issigned, &res);
}

template<>
inline void mc_ifc_extern_to_intern(const bool& ex, sc_logic& res)
{
	res = ex;
}

template<>
inline void mc_ifc_intern_to_extern(const sc_logic& in, bool issigned, bool& res)
{
	res = in.is_01() && in.to_bool();
}


template<typename Ex, typename In, bool IsSigned>
class mc_in_channel : public sc_port< tlm::tlm_fifo_get_if<Ex> >
{
public:
	explicit mc_in_channel(bool trans_signal, bool auto_trans_done = false ) 
		: _need_update(true)
		, _trans_signal(trans_signal), _auto_trans_done(auto_trans_done)
#ifdef MC_INTERFACE_CHANNEL_TRACE
    , _cnt(0)
#endif
	{}

	explicit mc_in_channel(const char *name, bool trans_signal, bool auto_trans_done = false)
		: sc_port< tlm::tlm_fifo_get_if<Ex> >(name)
		, _need_update(true)
		, _trans_signal(trans_signal), _auto_trans_done(auto_trans_done)
#ifdef MC_INTERFACE_CHANNEL_TRACE
    , _cnt(0)
#endif
{}

	const In& read() {
		if (_need_update) {
#ifdef MC_INTERFACE_CHANNEL_TRACE
      cout<<name()<<" reads from fifo input no "<<++_cnt<<endl;
#endif
			mc_ifc_extern_to_intern((*this)->get(), _val);
      if (_trans_signal) _need_update = false;
#ifdef MC_INTERFACE_CHANNEL_TRACE
      cout<<name()<<" received "<<_val<<endl;
#endif
		}
                if(_auto_trans_done)
                  transaction_done();
		return _val;
	}

	void transaction_done() { 
    if (_need_update) {
      // value has not yet been read from the input fifo
#ifdef MC_INTERFACE_CHANNEL_TRACE
      cout<<name()<<" transaction done reads from fifo input no "<<++_cnt<<endl;
#endif
      mc_ifc_extern_to_intern((*this)->get(), _val);
#ifdef MC_INTERFACE_CHANNEL_TRACE
      cout<<name()<<" received "<<_val<<endl;
#endif
    } else {
      _need_update = true;
#ifdef MC_INTERFACE_CHANNEL_TRACE
      cout<<name()<<" transaction done"<<endl;
#endif
    }
  }

private:
	In         _val;
	bool       _need_update;
	const bool _trans_signal;
        bool       _auto_trans_done;
#ifdef MC_INTERFACE_CHANNEL_TRACE
  unsigned   _cnt;
#endif
};


// initializes the internal value to 0
template<typename Ex, typename In, bool Is_Signed>
class mc_out_channel : public sc_port< tlm::tlm_fifo_put_if<Ex> >
{
public:
	explicit mc_out_channel(bool trans_signal, bool auto_trans_done = false )
		: _val(0) 
    , _trans_signal(trans_signal), _auto_trans_done(auto_trans_done)
#ifdef MC_INTERFACE_CHANNEL_TRACE
    , _cnt(0)
#endif
	{}

	explicit mc_out_channel(const char *name, bool trans_signal, bool auto_trans_done = false)
		: sc_port< tlm::tlm_fifo_put_if<Ex> >(name)
    , _val(0)
		, _trans_signal(trans_signal), _auto_trans_done(auto_trans_done)
#ifdef MC_INTERFACE_CHANNEL_TRACE
    , _cnt(0)
#endif
	{}

	const In& read() const {
		return _val;
	}

	void write(const In& val) { 
		_val = val; 
		if (!_trans_signal || _auto_trans_done ) transaction_done();
	}

	template<class Val>
	void write(int pos, const Val& val) { 
		_val[pos] = val;
		if (!_trans_signal || _auto_trans_done ) transaction_done();
	}

	template<class Val>
	void write(int idx, int ele_size, int pos, const Val& v) {
    write(idx * ele_size + pos, v);
	}

	template<class Val>
	void write(int pos, int len, const Val& v) {
		_val.range(pos + len - 1, pos) = v; 
		if (!_trans_signal || _auto_trans_done ) transaction_done();
	}

	template<class Val>
	void write(int idx, int ele_size, int pos, int len, const Val& v) {
    write(idx * ele_size + pos, len, v);
	}

	void transaction_done() { 
#ifdef MC_INTERFACE_CHANNEL_TRACE
    cout<<name()<<" writes to fifo "<<_val<<endl;
#endif
		Ex ex_val;
		mc_ifc_intern_to_extern(_val, Is_Signed, ex_val);
		(*this)->put(ex_val);
#ifdef MC_INTERFACE_CHANNEL_TRACE
    cout<<name()<<" transaction done no "<<++_cnt<<endl;
#endif

	}

private:
	In         _val;
	const bool _trans_signal;
        bool       _auto_trans_done;
#ifdef MC_INTERFACE_CHANNEL_TRACE
  unsigned   _cnt;
#endif
};

// if you do a partial write, it requires a read transaction to have
// completed.
template<typename Ex, typename In, bool Is_Signed>
class mc_inout_channel
{
private:
	std::string _name_in;
	std::string _name_out;
public:
 	sc_port< tlm::tlm_fifo_get_if<Ex> >  in;
 	sc_port< tlm::tlm_fifo_put_if<Ex> >   out;

	explicit mc_inout_channel(bool trans_signal)
    : _need_update(true)
		, _trans_signal(trans_signal)
	{}

	explicit mc_inout_channel(std::string name, bool trans_signal) 
		: _name_in(name+".in")
		, _name_out (name+".out")
		, in(_name_in.c_str())
		, out(_name_out.c_str())
		, _need_update(true)
		, _trans_signal(trans_signal)
	{}
	
	const In& read() {
		if (_need_update) {
			mc_ifc_extern_to_intern(in->get(), _val);
			_need_update = false;
		}
		return _val;
	}

	void write(const In& val) { 
    read();
		_val = val; 
		if (!_trans_signal) transaction_done();
	}

	template<class Val>
	void write(int pos, const Val& val) { 
    read();
		_val[pos] = val;
		if (!_trans_signal) transaction_done();
	}

	template<class Val>
	void write(int idx, int ele_size, int pos, const Val& v) {
    write(idx * ele_size + pos, v);
	}

	template<class Val>
	void write(int pos, int len, const Val& v) {
    read();
    _val.range(pos + len - 1, pos) = v;
		if (!_trans_signal) transaction_done();
	}

	template<class Val>
	void write(int idx, int ele_size, int pos, int len, const Val& v) {
    write(idx * ele_size + pos, len, v);
	}

	void transaction_done() {
    read();
		Ex ex_val;
		mc_ifc_intern_to_extern(_val, Is_Signed, ex_val);
		out->put(ex_val);
		_need_update = true; 
	}
private:
	In         _val;
	bool       _need_update;
	const bool _trans_signal;
};


////////////////////////////////////////////////////////////////////////////////
// rtl interface wires
////////////////////////////////////////////////////////////////////////////////


template <class T> class mc_in;
template<class T>  class mc_out;
template<class T>  class mc_inout;


template <>
class mc_in<bool> : public sc_signal_in_if<sc_dt::sc_logic>
{
public:
  explicit mc_in(sc_in<bool>& ep) : _ep(ep) 
  {
    sc_spawn_options options;
    options.spawn_method();
    options.set_sensitivity(&ep.value_changed());
    sc_spawn(FwdChg(this), "", &options);
  }

  // sc_signal_in_if<sc_dt::sc_logic>

  virtual const sc_dt::sc_logic& read() const { return _val; } 

  virtual const sc_dt::sc_logic& get_data_ref() const { return _val; }

  virtual const sc_signal_logic_deval& delayed() const {
    const sc_signal_in_if<sc_dt::sc_logic>* ifp = this;
    return *reinterpret_cast<const sc_signal_logic_deval*>(ifp);
  }

  virtual const sc_event& default_event() const { return _chg; }

  virtual const sc_event& value_changed_event() const { return _chg; }

  virtual const sc_event& posedge_event() const { return _pos; }

  virtual const sc_event& negedge_event() const { return _neg; }

  virtual bool event() const { return _ep->event(); }

  virtual bool posedge() const {  return _ep->posedge(); }

  virtual bool negedge() const { return _ep->negedge(); }

  // sc_in<sc_dt::sc_logic>

  typedef sc_signal_in_if<sc_dt::sc_logic> IIF;

  const IIF* operator-> () const { return this; }
  IIF* operator->() { return this; };

  operator const sc_dt::sc_logic&() const { return _val; }

private:
  struct FwdChg; friend struct FwdChg;

  struct FwdChg {
    FwdChg(mc_in<bool>* that) : _that(that) {}
    mc_in<bool>* _that;
    
    void operator()() {
      _that->_val = _that->_ep.read();
      _that->_chg.notify(); 
      if (_that->_ep.posedge()) _that->_pos.notify();
      if (_that->_ep.negedge()) _that->_neg.notify();
    };
  };

  sc_in<bool>&            _ep;
  mutable sc_dt::sc_logic _val;
  sc_event                _pos;
  sc_event                _neg;
  sc_event                _chg;
};

template <int N>
class mc_signal_rv 
  : public sc_prim_channel
  , public sc_signal_inout_if< sc_dt::sc_lv<N> >
{
  typedef sc_dt::sc_lv<N> Val;
public:
  mc_signal_rv() : _delta(~sc_dt::UINT64_ONE)
  {
    //for (int i = 0; i < N; ++i) _val[i] = SC_LOGIC_Z;
  }

  explicit mc_signal_rv(const char* name)
    : sc_prim_channel(name)
    , _delta(~sc_dt::UINT64_ONE)
  {
    //for (int i = 0; i < N; ++i) _val[i] = SC_LOGIC_Z;
  }

  // sc_interface
  virtual void register_port(sc_port_base&, const char*) {}
  virtual const sc_event& default_event() const { return _chg; }

  // signal_in_if<Val>
  virtual const Val& read() const { return _val; }
  virtual const Val& get_data_ref() const { return _val; }

  virtual const sc_event& value_changed_event() const { return _chg; }
  virtual bool event() const { return _delta + 1 == sc_delta_count(); }

  // signal_out_if<Val>
  virtual void write(const Val& rhs) {
    insert_replace(N - 1, 0, rhs);
  }

  void operator=(const Val& rhs) { write(rhs); }

  // and in addition partial writes ...

  void write(int pos,  const sc_dt::sc_logic v) {
    insert_replace(pos, pos, &v);
  }

	void write(int idx, int ele_size, int pos, const sc_dt::sc_logic v) {
    write(idx * ele_size + pos, v);
	}

  template<class Val>
	void write(int pos, int len, const Val& v) {
    insert_replace(pos + len - 1, pos, v);
	}

	template<class Val>
	void write(int idx, int ele_size, int pos, int len, const Val& v) {
    write(idx * ele_size + pos, len, v);
	}

protected:
  virtual void update() {
    if (resolve()) {
      _delta = sc_delta_count();
      _chg.notify(SC_ZERO_TIME);
//      cout<< name() << " = " << _val << endl;
    }
  }
  
private:
  
  // !!!TR!!!  sc_get_current_process_handle() is not provided by 2.1.v1
  struct ProcVal {
    explicit ProcVal(const sc_process_b* id = 0) 
      : _id(id) 
      , _next(0)
    {
      for (int i = 0; i < N; ++i) _val[i] = SC_LOGIC_Z;
    }
    ~ProcVal() { if (_next) delete _next; }

    const sc_process_b* _id;
    Val                 _val;
    ProcVal*            _next;
  };
  
  template <class T>
  void insert_replace(int hi, int lo, const T& rhs) {
    const sc_process_b* const my_id = sc_get_curr_process_handle();
    if (_vals._id == my_id) {        // optimization for single driver
      _vals._val.range(hi, lo) = rhs;
    } else if (_vals._id == 0) {    // no driver yet
      _vals._val.range(hi, lo) = rhs;
      _vals._id = my_id;
    } else {
      ProcVal* i = &_vals;
      while (i->_next && i->_next->_id != my_id) i = i->_next;
      if (i->_next == 0) i->_next = new ProcVal(my_id);
      i->_next->_val.range(hi, lo) = rhs;
    }
    request_update();
  }
  
  bool resolve() {
    const static sc_dt::sc_logic res[4][4]  = {
      {SC_LOGIC_0, SC_LOGIC_X, SC_LOGIC_0, SC_LOGIC_X},
      {SC_LOGIC_X, SC_LOGIC_1, SC_LOGIC_1, SC_LOGIC_X},
      {SC_LOGIC_0, SC_LOGIC_1, SC_LOGIC_Z, SC_LOGIC_X},
      {SC_LOGIC_X, SC_LOGIC_X, SC_LOGIC_X, SC_LOGIC_X},
    };

//    cout << "resolve at " << sc_delta_count();
      
    Val old = _val;
    _val = _vals._val;
//    cout << "\t " << _val;
    for (const ProcVal* i = _vals._next; i; i = i->_next) {
      for (int j = 0; j < N; ++j) {
        _val[j] = res[_val[j].value()][i->_val[j].value()];
      }
//      cout << "\t ~ " << i->_val << " = " << _val << endl;
    }
    
    return old != _val;
  }
  
  ProcVal       _vals;
  Val           _val;
  sc_event      _chg;
  sc_dt::uint64 _delta;
};

template <>
class mc_out<sc_dt::sc_logic> : public sc_signal<sc_dt::sc_logic>
{
  struct DrivePort {

    DrivePort(sc_signal<sc_dt::sc_logic>* drv, sc_inout<sc_dt::sc_logic>* port) 
      : _drv(drv), _port(port) 
    {}

    sc_signal<sc_dt::sc_logic>* _drv;
    sc_inout<sc_dt::sc_logic>*  _port;
    
    void operator()() const { _port->write(_drv->read()); }
  };

public: 
  explicit mc_out(sc_inout<sc_dt::sc_logic>& ep)
  {
    sc_spawn_options options;
    options.spawn_method();
    options.set_sensitivity(&sc_signal<sc_dt::sc_logic>::value_changed_event());
    sc_spawn(DrivePort(this, &ep), "", &options);
  }

  using sc_signal<sc_dt::sc_logic>::operator=;
};

template <int N>
class mc_out< sc_dt::sc_lv<N> > : public mc_signal_rv<N>
{
  typedef sc_dt::sc_lv<N> Val;

  struct DrivePort {

    DrivePort(mc_signal_rv<N>* drv, sc_inout<Val>* port) 
      : _drv(drv), _port(port) 
    {}

    mc_signal_rv<N>* _drv;
    sc_inout<Val>*   _port;
    
    void operator()() const { _port->write(_drv->mc_signal_rv<N>::read()); }
  };
public: 
  explicit mc_out(sc_inout<Val>& ep)
  {
    sc_spawn_options options;
    options.spawn_method();
    options.set_sensitivity(&mc_signal_rv<N>::value_changed_event());
    sc_spawn(DrivePort(this, &ep), "", &options);
  }

  using mc_signal_rv<N>::operator=;
};


template <int N>
class mc_inout< sc_dt::sc_lv<N> >  : public mc_out< sc_dt::sc_lv<N> >
{
  typedef sc_dt::sc_lv<N> Val;
public:
  explicit mc_inout(sc_inout<Val>& ep) : mc_out<Val>(ep), _ep(ep)
  {
    sc_spawn_options options;
    options.spawn_method();
    options.set_sensitivity(&ep.value_changed());
    sc_spawn(FwdChg(_ext_chg), "", &options);
  }

  using mc_signal_rv<N>::operator=;

  // signal_in_if<Val>
  virtual const Val& read() const { return _ep.read(); }

  virtual const sc_event& default_event() const { return _ext_chg; }
  virtual const sc_event& value_changed_event() const { return _ext_chg; }
  virtual bool event() const { return _ep.event(); }

private:
  struct FwdChg {
    explicit FwdChg(sc_event& chg) : _chg(&chg) {}
    sc_event* _chg;
    
    void operator()() { _chg->notify(); };
  };
  
  sc_inout<Val>& _ep;
  sc_event       _ext_chg;
};


// for now, we are just a usual inout ...
template <>
class mc_inout<sc_dt::sc_logic>  : public sc_signal_inout_if<sc_dt::sc_logic>
{
public:
  explicit mc_inout(sc_inout<sc_dt::sc_logic>& ep) : _ep(ep) {}

  mc_inout& operator=(const sc_dt::sc_logic& rhs) { _ep = rhs; return *this;}
  
  // signal_out_if<sc_dt::sc_logic>
  virtual void write(const sc_dt::sc_logic& rhs) { _ep = rhs; } 
  
  // signal_in_if<sc_dt::sc_logic>
  virtual const sc_logic& read() const { return _ep.read(); }

  virtual const sc_dt::sc_logic& get_data_ref() const { return _ep->get_data_ref(); }
#if (defined(MTI_SYSTEMC) && SYSTEMC_VERSION < 20070125)
  virtual const sc_signal_logic_deval& delayed() const { return _ep->delayed(); }
#endif

  virtual const sc_event& default_event() const { return _ep.default_event(); }
  virtual const sc_event& value_changed_event() const { return _ep.value_changed_event(); }
  virtual const sc_event& posedge_event() const { return _ep.posedge_event(); }
  virtual const sc_event& negedge_event() const { return _ep.negedge_event(); }

  virtual bool event() const { return _ep.event(); }
  virtual bool posedge() const { return _ep.posedge(); }
  virtual bool negedge() const { return _ep.negedge(); }

private:  
  sc_inout<sc_dt::sc_logic>& _ep;
};

#endif
