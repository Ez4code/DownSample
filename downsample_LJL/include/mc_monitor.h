//file: monitor.h
//  Catapult monitors which keep track of latency and throughput
//  The layout of this pair of classes is dictated somewhat by the functions 
//  available in the code which instantiates the monitors. 
//  The class Catapult monitor fires once per active clock edge and requests the trios monitors
//  to observe their input.  

//  An alternative would be to make catapult_monitor static data shared among all the 
//  trios_monitors.  Doing so is a bit awkward to handle the "start_of_simulation" and 
//  "end_of_simulation" constructs.

#include <vector>
#include <algorithm>
#include <systemc.h>
#include "mc_end_of_testbench.h"

class catapult_monitor;
/// A mc_trios_base_monitor is the base class monitoring trio signal on the clock edge.
class mc_trios_base_monitor: public sc_module {
private:
  bool    _const_period;   // true if all periods after first are of constant length
public:
  sc_time max_period;      // length of time of longest period
  sc_time first_time;      // first firing after time 0 == length of time of first period 
  sc_time second_time;     // second firing after time 0 
  sc_time last_time;
  int     cnt;             // number of transactions.
  
  SC_CTOR(mc_trios_base_monitor) {
    reset();
    SC_METHOD(record_transaction);
  }

  // Called by ctor and catapult monitor 
  virtual  void reset ();
  virtual  bool isInput() const = 0;
  virtual  bool isOutput() const = 0;

  virtual void register_mon(catapult_monitor *cm);
  /// called on active edge of my trios.
  virtual void observe() = 0; // make pure virtual

  virtual  void record_transaction() {
    sc_time now = sc_time_stamp();
    if (last_time == SC_ZERO_TIME) {
      first_time = now;
    } else {
      sc_time period = now - last_time;
      // There is some variability on the first cycle (started when reset is 
      // deasserted) and the second cycle (which may have some constant initialization
      // time.  If we are running several cycles wait until we have seen the first
      // before drawing any conclusions about constant periods.
      if (max_period != SC_ZERO_TIME && max_period != period && cnt > 1) 
        _const_period = false;
      if (max_period < period || cnt==2) {
	max_period = period;
      }
    }
    if (first_time >= second_time) {
      second_time = now;
    }
    last_time = now;
    if (last_time > SC_ZERO_TIME) {
      cnt++;
    }
  }

  /// At the end of the simulation, report what we found out 
  virtual  void report() {
    std::ostringstream msg;
    msg << "has period [first second  max const cnt] [ " 
        << first_time << " " << second_time << " " 
        << max_period << " " << _const_period << " " 
        << cnt << " ] " ;
    SC_REPORT_INFO(this->name(), msg.str().c_str());
  }

};

class mc_trios_inout_monitor: public mc_trios_base_monitor {
public:
  sc_in<sc_logic> trios;

  virtual void observe(){
    if (trios == SC_LOGIC_1 || first_time == SC_ZERO_TIME) {
      record_transaction();
    }
  }

  virtual  bool isOutput() const { return false; }
  virtual  bool isInput()  const { return true; }

  SC_HAS_PROCESS(mc_trios_inout_monitor);
  mc_trios_inout_monitor(const sc_module_name& name)
    : mc_trios_base_monitor(name)
  {}
};

class mc_trios_input_monitor: public mc_trios_base_monitor {
public:
  sc_in<sc_logic> trios;

  virtual void observe(){
    if (trios == SC_LOGIC_1 || first_time == SC_ZERO_TIME) {
      record_transaction();
    }
  }

  virtual  bool isOutput() const { return false; }
  virtual  bool isInput()  const { return true; }
  SC_HAS_PROCESS(mc_trios_input_monitor);
  mc_trios_input_monitor(const sc_module_name& name)
    : mc_trios_base_monitor(name)
  {}

};

class mc_trios_output_monitor: public mc_trios_base_monitor {
public:
  sc_in<sc_logic> trios;

  virtual void observe(){
    if (trios == SC_LOGIC_1) {
      record_transaction();
    }
  }

  virtual  bool isOutput() const { return true; }
  virtual  bool isInput()  const { return false; }

  SC_HAS_PROCESS(mc_trios_output_monitor);
  mc_trios_output_monitor(const sc_module_name& name)
    : mc_trios_base_monitor(name)
  {}
};


// This is a module so we don't have to do hacky stuff on start_simulation at end_of simulation
// 
#define CATMON_EX_TYPE long long

class catapult_monitor: public sc_module  , public mc_end_of_testbench   {
private:
  std::vector<mc_trios_base_monitor *> _data;
  int     _clk_cycles;
  sc_time _clkperiod;
  sc_time _lasttime;
  sc_time _resettime;
  bool    _constperiod;
  CATMON_EX_TYPE     _expectedlatency;
  CATMON_EX_TYPE     _expectedperiod;
public:
  sc_in<bool> clk;
  sc_in<sc_logic> rst;
  void connect(mc_trios_base_monitor *m) { _data.push_back(m); }

  SC_HAS_PROCESS(catapult_monitor);
  catapult_monitor(const sc_module_name& name, sc_clock & clkp, bool phase,
                   CATMON_EX_TYPE expected_latency, /* in clock cycles */
                   CATMON_EX_TYPE expected_period  /* in clock cycles */ )
    : sc_module(name),
      _expectedlatency(expected_latency),
      _expectedperiod(expected_period)
  {
    clk(clkp);
    SC_METHOD(observe);
    sensitive << (phase ? clk.pos() : clk.neg());
  }

  // Common to CTOR and observe
  void reset () {
    _clk_cycles = 0;
    _clkperiod = SC_ZERO_TIME;
    _resettime = SC_ZERO_TIME;
    _lasttime  = SC_ZERO_TIME;
    _constperiod = true;
  }


  void observe() {
    if (rst == SC_LOGIC_1)  {
      reset();
      for (unsigned i=0; i<_data.size(); ++i)
        _data[i]->reset();
    } else  if (rst == SC_LOGIC_0) {
      _clk_cycles++;
      sc_time now = sc_time_stamp();
      sc_time p = now - _lasttime;
      if (_resettime == SC_ZERO_TIME) {
        _resettime = now;
      }
      _lasttime = now;

      if (_clk_cycles > 2) {
        if (_clkperiod == SC_ZERO_TIME) {
          _clkperiod = p;
        }
        _constperiod &= (p == _clkperiod);
      }
      for (unsigned i=0; i<_data.size(); ++i)
        _data[i]->observe();
    }
  }


  virtual void start_of_simulation() {
    reset();
  }

  void dump() const {
    for (unsigned i=0; i<_data.size(); ++i)
        _data[i]->report();
  }

  void end_of_testbench() {}

  virtual void end_of_simulation()  {
    std::ostringstream msg0;
    msg0 << "runs with " <<  (_constperiod ? "constant" : "non-constant") << " clock period " << _clkperiod;
    SC_REPORT_INFO(this->name(), msg0.str().c_str());

    // For some systems, it might be interesting to check if all
    // signals have completed completed the same number of
    // transactions.  This property doesn't hold for pipelined systems
    // though (the testbench generates dummy inputs to flush the
    // pipe.)
    int min_cnt = INT_MAX;

    if (_data.size() <= 0) {
      return;
    }
    // Since all signals should have the same period, pick one arbitrarily from an output
    sc_time period = _data[_data.size()-1]->max_period;

    for (unsigned i=0; i<_data.size(); ++i) {
      if (min_cnt > _data[i]->cnt)   min_cnt = _data[i]->cnt;
      if (_data[i]->max_period != period) {
        std::ostringstream msg;
        msg << "Signal " << _data[i]->name()  << " has period " << _data[i]->max_period 
            << " instead of expected " << period; 
        SC_REPORT_WARNING(this->name(), msg.str().c_str());
      }
    }
    // Latency calculation
    sc_time last_output = SC_ZERO_TIME;
    sc_time first_input = sc_time_stamp();
    for (unsigned i=0; i<_data.size(); ++i) {
      if (_data[i]->isOutput()) 
        if (last_output < _data[i]->second_time) last_output = _data[i]->second_time;
      if (_data[i]->isInput()) 
        if (first_input > _data[i]->second_time) first_input = _data[i]->second_time;
    }


    // Report on Throughput 
    std::ostringstream msg1;

    msg1 << "Throughput: 1 transaction per " << period ;
    SC_REPORT_INFO(this->name(), msg1.str().c_str());

    if (min_cnt < 1) {
      std::ostringstream msg;
      msg << "No transactions completed.  Skipping latency and throughput reports";
      SC_REPORT_INFO(this->name(), msg.str().c_str());

    } else {
      if (min_cnt == 1 ) {
        std::ostringstream msg;
        msg << "Only 1 transaction observed.  Latency and throughput numbers may be inaccurate.";
        SC_REPORT_INFO(this->name(), msg.str().c_str());
      }

      if (_constperiod) {
        CATMON_EX_TYPE observed_period  = static_cast<int>((period  / _clkperiod));  // express in clock cycles
        if (_expectedperiod != observed_period) {
          std::ostringstream msg;
          msg << "Throughput period: " << observed_period << " cycles does not match expected "
              << _expectedperiod << " cycles (" << min_cnt << " transactions).";
          SC_REPORT_WARNING(this->name(), msg.str().c_str());
        }
      }

      // If the first input is at time 0, there was no firing on input signals.
      // Don't bother with latency:
      if (last_output >  first_input && first_input >  SC_ZERO_TIME) { 
        sc_time latency =  last_output - first_input;
        std::ostringstream msg;
        msg << "Latency: " << latency ;
        SC_REPORT_INFO(this->name(), msg.str().c_str());
        
        if (_constperiod) {
          CATMON_EX_TYPE observed_latency = static_cast<int>((latency / _clkperiod));  // express in clock cycles

          if (_expectedlatency != observed_latency) {
            std::ostringstream msg;
            msg << "Latency: " << observed_latency << " cycles does not match expected "
                << _expectedlatency << " cycles.";
            SC_REPORT_WARNING(this->name(), msg.str().c_str());
          }
        }
      }
    }
  }

};



/* -------------------------------------- */
void mc_trios_base_monitor::register_mon(catapult_monitor *cm) {
  cm->connect(this); 
}


void mc_trios_base_monitor::reset () 
{
  _const_period = true;
  max_period  = SC_ZERO_TIME;
  first_time  = SC_ZERO_TIME;
  second_time  = SC_ZERO_TIME;
  last_time = SC_ZERO_TIME;
  cnt = 0;
}



