#ifndef __INCLUDE_MGC_RESET_SIGNAL_IS
#define __INCLUDE_MGC_RESET_SIGNAL_IS

#include <systemc.h>

#ifdef __SYNTHESIS__
  namespace ac {
    inline void reset_signal_is(const sc_signal_in_if<bool>& iface, bool level) { }
    inline void reset_signal_is(const sc_in<bool>& iface, bool level) { }
    inline void async_reset_signal_is(const sc_signal_in_if<bool>& iface, bool level) { }
    inline void async_reset_signal_is(const sc_in<bool>& iface, bool level) { }
  }
# define reset_signal_is(iface,level) \
    if (sc_get_curr_process_kind() == SC_METHOD_PROC_) \
      ac::reset_signal_is(iface,level); \
    else if (sc_get_curr_process_kind() == SC_THREAD_PROC_) \
      ac::reset_signal_is(iface,level); \
    else \
      this->reset_signal_is(iface,level)
    using ac::async_reset_signal_is;
#else
# define reset_signal_is(iface,level) \
    { \
      sc_process_b *process_p = (sc_process_b*)sc_get_current_process_handle(); \
      const sc_curr_proc_kind pkind = process_p ? process_p->proc_kind() : SC_NO_PROC_; \
      if (pkind == SC_METHOD_PROC_) { /* ignore */ } \
      else if (pkind == SC_THREAD_PROC_) { /* ignore */ } \
      else { this->reset_signal_is(iface,level); } \
    }
# define async_reset_signal_is(iface,level) \
    { \
      sc_process_b *process_p = (sc_process_b*)sc_get_current_process_handle(); \
      const sc_curr_proc_kind pkind = process_p ? process_p->proc_kind() : SC_NO_PROC_; \
      if (pkind == SC_METHOD_PROC_) { \
         if (!!(level)) { this->sensitive_pos << (iface); } \
         else { this->sensitive_neg << (iface); } \
      } else if (pkind == SC_THREAD_PROC_) { /* ignore */ } \
      else { reset_signal_is(iface,level); /* at least make it synchronus, NOTE: the #define macro will be used here so do not use this-> */ } \
    }
#endif

#endif
