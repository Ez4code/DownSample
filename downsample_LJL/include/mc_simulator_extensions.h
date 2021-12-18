#ifndef MC_SIMULATOR_EXTENSIONS_H_INC
#define MC_SIMULATOR_EXTENSIONS_H_INC

#include <systemc.h>

/*! \file This header defines a common interface to the systemc
 *  extensions of the various simulators used by the verification
 *  flow.
 */

#if defined MTI_SYSTEMC
//==============================================================================
// Mentor Graphics QuestaSim Support
//------------------------------------------------------------------------------

#define MC_SIMULATOR_QUESTASIM
#define MC_MODULE_EXPORT(classname) SC_MODULE_EXPORT(classname)
#define OBSERVE_FOREIGN_SIGNAL(signal,path) signal.observe_foreign_signal(#path)

//typedef sc_foreign_module mc_foreign_module;
class mc_foreign_module : public sc_foreign_module {
public:
  mc_foreign_module(const sc_module_name& name, const char* hdl_name)
    : sc_foreign_module(name)
    , _hdl_name(hdl_name)
  {}

  virtual const char* hdl_name() const { return _hdl_name.c_str(); }

private:
  std::string _hdl_name;
};

#elif defined NCSC
//==============================================================================
// Cadence NCSim Support
//------------------------------------------------------------------------------

#define MC_SIMULATOR_NCSIM
#define MC_MODULE_EXPORT(classname) NCSC_MODULE_EXPORT(classname)

// For NCSim, the "observe" function uses '.' as the separator instead of '/' and removing leading separator
#define OBSERVE_FOREIGN_SIGNAL(signal,path) {\
  std::string tmp(#path); \
  for (unsigned int i=0;i<tmp.length();i++) if (tmp[i]=='/') tmp[i]='.'; \
  signal.observe_foreign_signal(tmp.substr(1).c_str()); }

class mc_foreign_module : public ncsc_foreign_module {
public:
  mc_foreign_module(const sc_module_name& name, const char* hdl_name)
    : ncsc_foreign_module(name)
    , _hdl_name(hdl_name)
  {}

  virtual const char* hdl_name() const { return _hdl_name.c_str(); }

  // Add function that mimics QuestaSim parameter set function
  void add_parameter(const char *_name, int _val) { ncsc_set_hdl_param(_name,_val); }
  void elaborate_foreign_module(const char *_hdl_name) {}

private:
  std::string _hdl_name;
};

#elif defined VCS_SYSTEMC
//==============================================================================
// Synopsys VCS-MX Support
//------------------------------------------------------------------------------

extern "C" bool hdl_elaboration_only();

#define MC_SIMULATOR_VCS
#define MC_MODULE_EXPORT(classname)

// For VCS-MX, the "observe" functions vary by netlist language and use '.' as the separator instead of '/' and removing leading separator
#if defined(CCS_DUT_VHDL)
#include <hdl_connect_vhdl.h>
#define HDL_CONNECT_FN hdl_connect_vhdl
#else
#if defined(CCS_DUT_VERILOG)
#include <hdl_connect_v.h>
#define HDL_CONNECT_FN hdl_connect_v
#endif
#endif
#define OBSERVE_FOREIGN_SIGNAL(signal,path) {\
  std::string tmp(#path); \
  for (unsigned int i=0;i<tmp.length();i++) if (tmp[i]=='/') tmp[i]='.'; \
  HDL_CONNECT_FN(signal,tmp.substr(1).c_str(),HDL_INPUT,0); }

#ifndef CCS_DUT_SYSC
// If not building with SystemC DUT from Catapult, then 
// include the SystemC wrapper created by vhdlan/vlogan

// If Catapult SystemC-based design, ensure that wrapper is placed in namespace for SCVerify
#ifdef CCS_VCS_NAMESPACE
namespace CCS_VCS_NAMESPACE {
#endif

#define VCS_QUOTE_HDR(x) VCS_QUOTE_HDR1(x.h)
#define VCS_QUOTE_HDR1(x) #x
#include VCS_QUOTE_HDR(TOP_HDL_ENTITY)

#ifdef CCS_VCS_NAMESPACE
}
#define CCS_SCOPED_HDL CCS_VCS_NAMESPACE::TOP_HDL_ENTITY
#else
#define CCS_SCOPED_HDL TOP_HDL_ENTITY
#endif

// Synopsys VCS SystemC
class mc_foreign_module : public CCS_SCOPED_HDL {
public:
  mc_foreign_module(const sc_module_name& name, const char* hdl_name)
    : TOP_HDL_ENTITY(name)
    , _hdl_name(hdl_name)
  {}

  virtual const char* hdl_name() const { return _hdl_name.c_str(); }

  // Add function that mimics QuestaSim parameter set function
  void add_parameter(const char *_name, int _val) { }
  void elaborate_foreign_module(const char *_hdl_name) {}

private:
  std::string _hdl_name;
};

#endif

#else // OSCI

#define MC_SIMULATOR_OSCI
#define MC_MODULE_EXPORT(classname)

// no definition for mc_foreign_module

#endif


#endif // MC_SIMULATOR_EXTENSIONS_H_INC
