#ifndef MC_END_OF_TESTBENCH_H
#define MC_END_OF_TESTBENCH_H

struct mc_end_of_testbench { 
  virtual void end_of_testbench() = 0; 
  virtual ~mc_end_of_testbench() {};
};

#endif
