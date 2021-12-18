////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2011 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
////////////////////////////////////////////////////////////////////////////////

#ifndef MC_COMPARATOR_H
#define MC_COMPARATOR_H

#include <sstream>
#include <iomanip>
#include <systemc.h>
#include <tlm.h>

#include "mc_container_types.h"

// Check for macro definitions that will conflict with template parameter names in this file
#if defined(Tclass)
#define Tclass 0
#error The macro name 'Tclass' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tclass' giving the location of the previous definition.
#endif
#if defined(Tsize)
#define Tsize 0
#error The macro name 'Tsize' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tsize' giving the location of the previous definition.
#endif

template <class Tclass, class Tmask>
struct mc_golden_info {
  mc_golden_info() {}
  explicit mc_golden_info(bool ignore, bool use_mask, unsigned int iteration) 
    : _mask(~0), _use_mask(use_mask), _ignore(ignore), _iteration(iteration) {}
  mc_golden_info(const Tclass& data, bool ignore, const Tmask& mask, bool use_mask, unsigned int iteration) 
    : _data(data), _mask(mask), _use_mask(use_mask), _ignore(ignore), _iteration(iteration) {}
  Tclass    _data;
  Tmask     _mask;
  bool      _use_mask;
  bool _ignore;
  unsigned int _iteration;
};

template <class Tclass, class Tmask>
inline
std::ostream& operator<<(std::ostream& os, const mc_golden_info<Tclass,Tmask>& gi) {
  os << gi._data;
  return os;
}

template<class Tclass>
struct mc_report_data {
  void report(const char* id, const Tclass& golden_value, const Tclass& dut_value) {
    std::ostringstream msg;
    msg.str(""); msg << "          golden = " << golden_value;
    SC_REPORT_INFO(id, msg.str().c_str());
    msg.str(""); msg << "             dut = " << dut_value;
    SC_REPORT_INFO(id, msg.str().c_str());
  }
  bool partial_compare(const char* id, const Tclass& golden_value) { return false; }
};

template<class Tclass, int Tsize>
struct mc_report_data< mgc_sysc_ver_array1D<Tclass,Tsize> >
{
  void report(const char* id,
              const mgc_sysc_ver_array1D<Tclass,Tsize>& golden_value,
              const mgc_sysc_ver_array1D<Tclass,Tsize>& dut_value)
  {
    int LB = (golden_value.get_lower() < 0) ? 0       : golden_value.get_lower();
    int UB = (golden_value.get_upper() < 0) ? Tsize-1 : golden_value.get_upper();
    std::ostringstream msg;
    msg << std::setw(3+6) << "index" <<  ' '
        << std::setw(20) << "golden value" << ' '
        << std::setw(20) << "dut value";
    SC_REPORT_INFO(id, msg.str().c_str());
    for (int i = LB; i <= UB; ++i) {
#ifdef CCS_MISMATCHED_OUTPUTS_ONLY
      if (golden_value[i] != dut_value[i]) {
        msg.str("###");
        msg << std::setw(6) << i << ' '
            << std::setw(20) << golden_value[i] << ' '
            << std::setw(20) << dut_value[i];
        SC_REPORT_INFO(id, msg.str().c_str());
      }
#else
        msg.str("");
        msg << (golden_value[i] == dut_value[i] ? "===" : "###");
        msg << std::setw(6) << i << ' '
            << std::setw(20) << golden_value[i] << ' '
            << std::setw(20) << dut_value[i];
        SC_REPORT_INFO(id, msg.str().c_str());
#endif
    }
  }
  bool partial_compare(const char* id, const mgc_sysc_ver_array1D<Tclass,Tsize>& golden_value) {
    int LB = (golden_value.get_lower() < 0) ? 0       : golden_value.get_lower();
    int UB = (golden_value.get_upper() < 0) ? Tsize-1 : golden_value.get_upper();
    if ((LB != 0) || (UB != Tsize-1)) {
       std::ostringstream msg;
       msg.str(""); msg << "Array element range comparison limited from [0..." << Tsize-1 << "] to [" << LB << "..." << UB << "]";
       SC_REPORT_INFO(id, msg.str().c_str());
       return true;
    }
    return false;
  }
};

template <class Tclass, class Tmask> 
SC_MODULE(mc_comparator)
{    
  sc_port< tlm::tlm_fifo_get_if<Tclass> >                  data_in;
  sc_port< tlm::tlm_fifo_get_if<mc_golden_info<Tclass,Tmask> > > data_golden;
 
  int  get_compare_count() const { return _compare_cnt*_chan_factor; }
  int  get_partial_compare_count() const { return _partial_compare_cnt*_chan_factor; }
  int  get_mask_compare_count() const { return _mask_compare_cnt*_chan_factor; }
  int  get_ignore_count() const { return _ignore_cnt*_chan_factor; }
  int  get_error_count() const { return _error_cnt*_chan_factor; }
  int  get_success_count() const { return (_compare_cnt - _error_cnt - _ignore_cnt)*_chan_factor; }
  bool get_last_comparison_result() const { return _last_result; }

  /* triggers after each comparison; the above functions will include
     the data of this comparison.
  */
  const sc_event& get_compare_event() const { return _compare; }

  /* triggers after each failing comparison; the above functions will
     include the data of this comparison.
  */
  const sc_event& get_error_event() const { return _error; }

  /* triggers after each successful comparison; the above functions
     will include the data of this comparison.
  */
  const sc_event& get_success_event() const { return _success; }

  /* triggers after each suppressed comparison; the above functions
     will include the data of this comparison.
  */
  const sc_event& get_ignore_event() const { return _ignore; }

  bool check_results(int capture_count, bool skip_noerr) {
    bool _failed = false;
    cout     << "'" << _formal_name << "'" << std::endl;
    cout     << "   capture count        = " << capture_count << endl;
    cout     << "   comparison count     = " << this->get_compare_count();
    if (this->get_partial_compare_count())
      cout   << " (" << this->get_partial_compare_count() << " partial)";
    if (this->get_mask_compare_count())
      cout   << " (" << this->get_mask_compare_count() << " masked)";
    cout     << endl;
    cout     << "   ignore count         = " << this->get_ignore_count() << endl;
    cout     << "   error count          = " << this->get_error_count() << endl;
    cout     << "   stuck in dut fifo    = " << data_in->used() << endl;
    cout     << "   stuck in golden fifo = " << data_golden->used() << endl;
    if (this->get_error_count() > 0)
      cout   << "   Error: output '" << _formal_name << "' had comparison errors" << endl;
    if (this->get_compare_count() < capture_count)
      cout   << "   Error: output '" << _formal_name << "' has incomplete comparisons" << endl;
    if (capture_count == 0) {
      if (_is_chan || skip_noerr) {
        cout << "   Warning: output '" << _formal_name << "' has no golden values to compare against" << endl;
      } else {
        cout << "   Error: output '" << _formal_name << "' has no golden values to compare against" << endl;
        _failed = true;
      }
    }
    _failed = _failed || this->get_error_count() > 0;
    _failed = _failed || this->get_compare_count() < capture_count;
    return _failed;
  }

  SC_HAS_PROCESS(mc_comparator);
  explicit mc_comparator(const sc_module_name& name, std::string formal_name, bool is_chan, int max_error_cnt = -1, int chan_factor = 1) 
    : sc_module(name)
    , data_in("data_in")
    , data_golden("data_golden")
    , _formal_name(formal_name)
    , _is_chan(is_chan)
    , _error_cnt_sig("_error_cnt_sig")
    , _compare_cnt_sig("_compare_cnt_sig")
    , _error_max(max_error_cnt)
    , _chan_factor(chan_factor)
    , _golden_sig("_golden_sig")
    , _dut_sig("_dut_sig")
    {
      SC_METHOD(compare);
    }

  virtual void start_of_simulation() {
    _compare_cnt = 0;
    _partial_compare_cnt = 0;
    _mask_compare_cnt = 0;
    _ignore_cnt  = 0;
    _error_cnt   = 0;
    _received_golden = false;
    _received_dut = false;
  }

 private:
  void compare();
public:  
  std::string        _formal_name;
  bool               _is_chan;
  int                _compare_cnt;
  int                _partial_compare_cnt;
  int                _mask_compare_cnt;
  int                _ignore_cnt;
  int                _error_cnt;
  sc_signal<int>     _error_cnt_sig;
  sc_signal<int>     _compare_cnt_sig;
  const int          _error_max;
  const int          _chan_factor;
  bool               _last_result;

  sc_event           _compare;
  sc_event           _ignore;
  sc_event           _success;
  sc_event           _error;

  mc_golden_info<Tclass,Tmask>  _golden;
  sc_signal<Tclass>  _golden_sig;
  Tclass                  _dut;
  sc_signal<Tclass>  _dut_sig;
  bool               _received_golden;
  bool               _received_dut;
  std::ostringstream _msg;
  mc_report_data<Tclass>  _rpt;
};

template <class Tclass, class Tmask>
void mc_comparator<Tclass,Tmask>::compare()
{
  while (true) {
    if (!_received_golden && data_golden->nb_get(_golden))
    {
#ifdef CCS_SYSC_DEBUG
      _msg.str("");
      _msg << "obtained GOLDEN value " 
            << _golden._data << " @ " << sc_time_stamp();
      SC_REPORT_INFO(name(), _msg.str().c_str());
#endif
      _received_golden = true;
    }

    if (!_received_dut && data_in->nb_get(_dut))
    {
      _golden_sig.write(_golden._data);
      _dut_sig.write(_dut);
#ifdef CCS_SYSC_DEBUG
      _msg.str("");
      _msg << " obtained DUT    value " 
            << _dut << " @ " << sc_time_stamp();
      SC_REPORT_INFO(name(), _msg.str().c_str());
#endif
      _received_dut = true;
    }

    if (!_received_dut || !_received_golden) break;
    
    ++_compare_cnt;
    _compare_cnt_sig.write(_compare_cnt);
    if (_rpt.partial_compare(name(), _golden._data)) ++_partial_compare_cnt;

    if (_golden._use_mask) {
#ifdef CCS_SYSC_DEBUG
      SC_REPORT_INFO(name(), "Masking golden value and DUT value");
      _msg.str(""); _msg << "Before mask: GOLDEN value = " << _golden._data;
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << "                DUT value = " << _dut;
      SC_REPORT_INFO(name(), _msg.str().c_str());
#endif
      _golden._data &= _golden._mask;
      _dut &= _golden._mask;
       ++_mask_compare_cnt;
#ifdef CCS_SYSC_DEBUG
      _msg.str(""); _msg << "After  mask: GOLDEN value = " << _golden._data;
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << "                DUT value = " << _dut;
      SC_REPORT_INFO(name(), _msg.str().c_str());
#endif
    }
    _last_result = (_golden._data == _dut);
    if (_golden._ignore) {
      ++_ignore_cnt;
#ifdef CCS_SYSC_DEBUG
      SC_REPORT_INFO(name(), "Ignoring Data");
#endif
      _ignore.notify(SC_ZERO_TIME);
    }
    else if (_last_result) {
#ifdef CCS_SYSC_DEBUG
      SC_REPORT_INFO(name(), "Correct Data Detected");
#endif
      _success.notify(SC_ZERO_TIME);
    } else {
      ++_error_cnt;
      _error_cnt_sig.write(_error_cnt);
      _msg.str(""); _msg << "Incorrect Data Detected @ " << sc_time_stamp();
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << " iteration count = " << _golden._iteration;
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << "   compare count = " << _compare_cnt*_chan_factor;
      if (_partial_compare_cnt) _msg << " (" << _partial_compare_cnt*_chan_factor << " partial)";
      if (_mask_compare_cnt) _msg << " (" << _mask_compare_cnt*_chan_factor << " masked)";
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << "     error count = " << _error_cnt*_chan_factor;
      SC_REPORT_INFO(name(), _msg.str().c_str());
      _msg.str(""); _msg << "    ignore count = " << _ignore_cnt*_chan_factor;
      SC_REPORT_INFO(name(), _msg.str().c_str());
      if (_golden._use_mask) {
         _msg.str(""); _msg << "    current mask = " << _golden._mask;
         SC_REPORT_INFO(name(), _msg.str().c_str());
      }

      _rpt.report(name(), _golden._data, _dut);

      _error.notify(SC_ZERO_TIME);
    }
    _compare.notify(SC_ZERO_TIME);

    if (_error_max > 0 && _error_cnt == _error_max) {
      SC_REPORT_ERROR(name(), "reached maximal number of errors.");
    }

    _received_dut = _received_golden = false;
  }

  if (_received_dut) {
    next_trigger(data_golden->ok_to_get()); // waiting for golden data
  } else if (_received_golden) {
    next_trigger(data_in->ok_to_get()); // waiting for dut data
  } else {
    next_trigger(data_golden->ok_to_get() | data_in->ok_to_get());
  }
}

#endif //MC_COMPARATOR_H
