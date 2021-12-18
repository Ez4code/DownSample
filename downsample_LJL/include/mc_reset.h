#ifndef MC_RESET_H
#define MC_RESET_H

#include <sstream>
#include <iomanip>
#include <systemc.h>

class mc_programmable_reset : public sc_module
{
public:
	sc_out< sc_logic > reset_out;

   SC_HAS_PROCESS(mc_programmable_reset);
   mc_programmable_reset(const sc_module_name& name, double duration, bool phaseneg)
      : sc_module(name)
      , reset_out("reset_out")
      , d_duration(duration)
      , d_phaseneg(phaseneg)
   {
      SC_METHOD(reset_driver);
      sensitive << reset_deactivation_event;
   }

   void reset_driver()
   {
      static bool first = true;
      if (first || sc_time_stamp() == SC_ZERO_TIME) {
         first = false;
         reset_out = (d_phaseneg ? SC_LOGIC_0 : SC_LOGIC_1);
         reset_deactivation_event.notify(d_duration, SC_NS);
      } else {
         reset_out = (d_phaseneg ? SC_LOGIC_1 : SC_LOGIC_0);
      }
   }

private:
   double     d_duration;
   bool       d_phaseneg;
   sc_event   reset_deactivation_event;
};

class mc_sync_timer : public sc_module
{
public:
	sc_in_clk clk;
	sc_in< sc_logic > rst;
   sc_out< sc_logic > sync_out;

   SC_HAS_PROCESS(mc_sync_timer);
   mc_sync_timer(const sc_module_name& name, unsigned int reset_length, unsigned int offset, unsigned int duration, sc_logic active_edge)
      : sc_module(name)
      , clk("clk")
      , rst("rst")
      , d_active_edge(active_edge)
      , d_reset_length(reset_length?reset_length-1:0) // reset_length-1 downto 0 OR ELSE 0 (no reset loop)
      , d_offset(reset_length?(offset?offset-1:0):0)
      , d_duration(duration-1) // duration-1 downto 0
      , d_count(0)
   {
      SC_METHOD(sync_driver);
      sensitive << clk << rst;
   }

   void sync_driver()
   {
      if (rst.read() == SC_LOGIC_1) {
         sync_out.write(SC_LOGIC_0);
         d_count = d_offset;
      } else {
         if (clk.read() == d_active_edge) {
            if (d_reset_length) {
               d_reset_length--; // reset loop still executing
            } else {
               if (d_count) {
                  sync_out.write(SC_LOGIC_0);
                  d_count--;
               } else {
                  sync_out.write(SC_LOGIC_1);
                  d_count = d_duration;
               }
            }
         }
      }
   }

private:
   sc_logic        d_active_edge;
   unsigned int    d_reset_length;
   unsigned int    d_offset;
   unsigned int    d_duration;
   unsigned int    d_count;
};

#endif

