
#ifndef _INCLUDED_MC_TESTBENCH_UTIL_H_
#define _INCLUDED_MC_TESTBENCH_UTIL_H_

namespace mc_testbench {

static void process_wait_ctrl(
   const sc_string &var, // variable name
   mc_wait_ctrl &var_wait_ctrl, // new control variable
   tlm::tlm_fifo_put_if< mc_wait_ctrl > *ccs_wait_ctrl_fifo_if, // FIFO for wait_ctrl objects
   const int var_capture_count,
   const int var_stopat,
   const bool is_channel=false)
{
#ifdef MC_DEFAULT_TRANSACTOR_LOG
   const bool log_event = (MC_DEFAULT_TRANSACTOR_LOG & MC_TRANSACTOR_WAIT);
#else
   const bool log_event = true;
#endif
   if (var_capture_count < var_stopat) {
      var_wait_ctrl.ischannel = is_channel;
      if (var_wait_ctrl.cycles != 0) {
         var_wait_ctrl.iteration = var_capture_count;
         var_wait_ctrl.stopat = var_stopat;
         if (var_wait_ctrl.cycles < 0) {
            if (log_event) {
               std::ostringstream msg; msg.str("");
               msg << "Ignoring negative value (" << var_wait_ctrl.cycles << ") for testbench control testbench::" << var << "_wait_ctrl.cycles.";
               SC_REPORT_WARNING("User testbench", msg.str().c_str());
            }
            var_wait_ctrl.cycles = 0;
         }
         if (var_wait_ctrl.interval < 0) {
            if (log_event) {
               std::ostringstream msg; msg.str("");
               msg << "Ignoring negative value (" << var_wait_ctrl.interval << ") for testbench control testbench::" << var << "_wait_ctrl.interval.";
               SC_REPORT_WARNING("User testbench", msg.str().c_str());
            }
            var_wait_ctrl.interval = 0;
         }
         if (var_wait_ctrl.is_set()) {
            if (log_event) {
               std::ostringstream msg; msg.str("");
               msg << "Captured " << var << "_wait_ctrl request " << var_wait_ctrl;
               SC_REPORT_INFO("User testbench", msg.str().c_str());
            }
            ccs_wait_ctrl_fifo_if->put(var_wait_ctrl);
         }
      }
      var_wait_ctrl.clear(); // reset wait_ctrl
   }
}

} // end namespace mc_testbench

#endif
