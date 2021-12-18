#include <map>
#include <string>
#include <utility>
#include <sstream>

template <unsigned int BLOCKS, unsigned int LDVDpairs>
SC_MODULE(mc_stall_ctrl2) {
  sc_in<bool>               clk;
  sc_in<sc_logic>           rst;
  sc_in<sc_logic>           enable_stalls;
  sc_out<unsigned short>    stall_coverage; // percent of stall flags wiggled

  sc_out<sc_logic > stall_out[BLOCKS];
  sc_in<sc_logic>  ld_in[LDVDpairs];
  sc_in<sc_logic>  vd_in[LDVDpairs];

  bool                      reset_done;
  sc_lv<LDVDpairs>          stalled;
  unsigned int              ldvd_pos;
  enum States { MC_STALL_CTRL_RESET, MC_STALL_CTRL_INIT, MC_STALL_CTRL_WAIT, MC_STALL_CTRL_SEEK, MC_STALL_CTRL_STALL2, MC_STALL_CTRL_STALL3, MC_STALL_CTRL_DONE };
  States                    cstate;
  unsigned int              interval;    // interval (in clock cycles) between stall attempts
  unsigned int              hold;        // number of cycles to assert stall
  unsigned int              intervalcnt; // countdown interval
  unsigned int              holdcnt;     // countdown hold
  unsigned int              repetition;  // repetition count
  bool                      done;
  unsigned int              next_ldvd;
  unsigned int              next_stall;
  unsigned int              map_ldvd2stall[LDVDpairs];
  std::map<std::string,unsigned int> stall_idx;

  SC_HAS_PROCESS(mc_stall_ctrl2);
  mc_stall_ctrl2(const sc_module_name& name)
    : sc_module(name)
    , clk("clk")
    , rst("rst")
    , reset_done(false)
    , stalled(0)
    , ldvd_pos(0)
    , cstate(MC_STALL_CTRL_RESET)
    , interval(1)
    , hold(3)
    , holdcnt(0)
    , repetition(10) // repeat each stall five times
    , done(false)
    , next_ldvd(0)
    , next_stall(0)
  {
    SC_METHOD(generate);
    sensitive << clk;
  }

  void bind_ld_vd_stall(sc_signal_in_if<sc_logic> &ld, sc_signal_in_if<sc_logic> &vd, std::string stall_name, sc_signal_out_if<sc_logic> &stall)
  {
    assert(next_stall<=BLOCKS); // guard
    assert(next_ldvd<LDVDpairs); // guard
    ld_in[next_ldvd].bind(ld);
    vd_in[next_ldvd].bind(vd);
    std::map<std::string,unsigned int>::iterator pos = stall_idx.find(stall_name);
    if (pos == stall_idx.end()) {
      stall_idx.insert(std::pair<std::string,unsigned int>(stall_name,next_stall));
      stall_out[next_stall].bind(stall);
      next_stall++;
    }
    pos = stall_idx.find(stall_name);
    map_ldvd2stall[next_ldvd] = (*pos).second;
    next_ldvd++;
  }

  void generate() {
    // Verify all ports bound before doing anything real
    if ((next_stall!=BLOCKS)&&(next_ldvd!=LDVDpairs)) { return; }
    if (rst.read() == 1) {
      if (!reset_done) {
        for (int i=0;i<BLOCKS;i++) { stall_out[i].write(sc_dt::Log_0); }
        stalled = 0;
        ldvd_pos = 0;
        intervalcnt = 0;
        repetition = 5;
        holdcnt = 0;
        reset_done = true;
        done = false;
        stall_coverage.write(0);
      }
    } else {
      if (clk.read() == SC_LOGIC_1) {
        switch(cstate) {
          case MC_STALL_CTRL_RESET: { 
            ldvd_pos = 0;
            stalled = 0;
            for (int i=0;i<BLOCKS;i++) { stall_out[i].write(sc_dt::Log_0); }
            cstate = MC_STALL_CTRL_INIT; 
            break; 
          }
          case MC_STALL_CTRL_INIT: { 
            intervalcnt = interval;
            holdcnt = hold;
            if (enable_stalls.read() == sc_dt::Log_1) cstate = MC_STALL_CTRL_WAIT; 
            break; 
          }
          case MC_STALL_CTRL_WAIT: {
            if (--intervalcnt == 0) cstate = MC_STALL_CTRL_SEEK; // wait atleast 'interval' clock cycles before seeking a stall pin
            break; 
          }
          case MC_STALL_CTRL_SEEK: {
            // ldvd_pos is viable candidate for stalling iff not yet stalled AND ld==1 AND vd==1
            std::ostringstream msg;
            for (ldvd_pos=0; ldvd_pos<LDVDpairs; ldvd_pos++) {
              if ( (stalled[ldvd_pos] == sc_dt::Log_0)       &&
                   (ld_in[ldvd_pos].read() == sc_dt::Log_1)  &&
                   (vd_in[ldvd_pos].read() == sc_dt::Log_1) ) {
                stalled[ldvd_pos] = 1;
                stall_out[map_ldvd2stall[ldvd_pos]].write(sc_dt::Log_1);
                msg << "   STALL: driving stall_out[" << map_ldvd2stall[ldvd_pos] << "] to 1 " << sc_time_stamp();
                SC_REPORT_INFO(this->name(), msg.str().c_str());
                holdcnt = hold;
                cstate = MC_STALL_CTRL_STALL2;
                break;
              } 
            }
            break;
          }
          case MC_STALL_CTRL_STALL2: {
            if (--holdcnt == 0) cstate = MC_STALL_CTRL_STALL3; // hold stal for 'hold' clock cycles
            break;
          }
          case MC_STALL_CTRL_STALL3: {
            std::ostringstream msg;
            msg << "   STALL: driving stall_out[" << map_ldvd2stall[ldvd_pos] << "] to 0 " << sc_time_stamp();
            SC_REPORT_INFO(this->name(), msg.str().c_str());
            stall_out[map_ldvd2stall[ldvd_pos]].write(sc_dt::Log_0);
            // update coverage percent
            unsigned short n = stalled.to_uint();
            unsigned short count = 0;
            while (n) {
              count += n & 0x1u;
              n >>= 1;
            }
            if (!done) stall_coverage.write((unsigned short)100*count/LDVDpairs);
            if (count==LDVDpairs) {
              done = true;
              if (--repetition==0) {
                cstate = MC_STALL_CTRL_DONE;
              } else {
                cstate = MC_STALL_CTRL_RESET;
              }
            } else {
              cstate = MC_STALL_CTRL_INIT;
            }
            break;
          }
          case MC_STALL_CTRL_DONE: {
            break;
          }
          default: {
            cstate = MC_STALL_CTRL_INIT;
          }
        }
      }
    }
  }
};

