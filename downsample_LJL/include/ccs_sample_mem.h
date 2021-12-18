#ifndef __CCS_SAMPLE_MEM_H
#define __CCS_SAMPLE_MEM_H
// This file implements an example library of shared memories.  These are intended as examples only and do not
// have a specific technology implementation.  The Verilog simulation models are based on Calypto PowerPro models
// to allow automated power optimization of memory power management ports.
//
// NOTE:  The file dependencies for the VHDL and Verilog models require the use of a Catapult library file.  A
//        "map_to_operator" pragma is used to tie the SystemC instance to the library to get these file dependencies.
//
// All memories are simulated using a shared array or a signal-level connection to an RTL style RAM.
// Each memory class contains:
//  - mem: The implementation of the memory.  This is an array at the TLM level and a SystemC pin-accurate 
//         simulation model for synthesis.
//  - <identfier>_port: The port used to bind ports through hierarchy to the memory.  Each memory has a different
//                      set of ports.
//
// The ports define an API for interacting with the memory.  The same API can be used directly with "mem". 
//
// This file defines the following memories, including a synthesis (SYN) and TLM view and any
// hardware that is required to implement the memory. 
//  - mem_1p:  This is a single port memory.  In this implementation, simple arbitration logic is added
//             around the memory to give priority to the READ operation.  External logic must be used for synchronization.
//  - mem_1r1w: This is a memory with one dedicated read port and one dedicated write port.  Each port is expected
//              to be connected to a different process.  No arbitration is included in the model.
//  - mem_2p:   This is a memory with two read/write ports.  Each port is expected to be connected to a different 
//              process.  No arbitration is included in the model.

#include "ccs_sample_mem/ccs_sample_mem_1p.h"
#include "ccs_sample_mem/ccs_sample_mem_2p.h"
#include "ccs_sample_mem/ccs_sample_mem_1r1w.h"

#endif
