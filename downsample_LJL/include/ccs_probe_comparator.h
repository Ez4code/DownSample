#ifndef _INCLUDED_CCS_PROBE_COMPARATOR_
#define _INCLUDED_CCS_PROBE_COMPARATOR_

template <int Tw>
SC_MODULE(ccs_probe_comparator)
{
  sc_port<tlm::tlm_fifo_get_if<sc_lv<Tw> > > PROBE_fifo;
  sc_port<tlm::tlm_fifo_get_if<sc_lv<Tw> > > RTL_fifo;
  sc_lv<Tw>                                  PROBE_current_value;
  sc_lv<Tw>                                  RTL_current_value;
  bool                                       PROBE_current_value_ready;
  bool                                       RTL_current_value_ready;
  sc_lv<Tw>                                  PROBE_last_value;
  sc_lv<Tw>                                  RTL_last_value;
  bool                                       PROBE_last_value_set;
  bool                                       RTL_last_value_set;
  int                                        PROBE_value_count;
  int                                        RTL_value_count;
  int                                        compare_cnt;
  int                                        error_cnt;
  std::ostringstream                         _msg;

  SC_HAS_PROCESS(ccs_probe_comparator);
  ccs_probe_comparator(const sc_module_name& name)
    : sc_module(name)
    , PROBE_fifo("PROBE_fifo")
    , RTL_fifo("RTL_fifo")
    , PROBE_current_value_ready(false)
    , RTL_current_value_ready(false)
    , PROBE_last_value_set(false)
    , RTL_last_value_set(false)
    , compare_cnt(0)
    , error_cnt(0)
  {
    SC_METHOD(compare);
  }
  virtual void start_of_simulation() {
  }
  void compare() {
    while (true) {
      if (!PROBE_current_value_ready && PROBE_fifo->nb_get(PROBE_current_value)) {
        PROBE_current_value_ready = true;
      }
      if (!RTL_current_value_ready   && RTL_fifo->nb_get(RTL_current_value)) {
        RTL_current_value_ready   = true;
      }
      if (!PROBE_current_value_ready || !RTL_current_value_ready) break;
      PROBE_last_value = PROBE_current_value;  PROBE_last_value_set = true;
      RTL_last_value = RTL_current_value;      RTL_last_value_set = true;
      ++compare_cnt;
      PROBE_current_value_ready = RTL_current_value_ready = false;
      if (PROBE_current_value != RTL_current_value) {
        ++error_cnt;
        _msg.str("");
        _msg << "Incorrect Data Detected (probe=" << std::hex << PROBE_current_value.to_string(SC_HEX) << ", rtl=" << std::hex << RTL_current_value.to_string(SC_HEX) << ") @ " << std::dec << sc_time_stamp();
        SC_REPORT_WARNING(name(), _msg.str().c_str());
      }
    }
    if      (RTL_current_value_ready)     next_trigger(PROBE_fifo->ok_to_get());  // waiting for PROBE data
    else if (PROBE_current_value_ready)   next_trigger(RTL_fifo->ok_to_get());    // waiting for RTL data
    else                                  next_trigger(PROBE_fifo->ok_to_get() | RTL_fifo->ok_to_get());
  }
};
#endif

