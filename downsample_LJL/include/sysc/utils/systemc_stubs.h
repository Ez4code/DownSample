
#ifndef SC_SYSTEMC_STUBS
#define SC_SYSTEMC_STUBS

#define SC_SIMCONTEXT_H
#define SC_EVENT_H
#define SC_WAIT_H
#define SC_WAIT_CTHREAD_H
#define SC_MODULE_H
#define SC_PRIM_CHANNEL_H
#define SC_SIGNAL_H
#define SC_BUFFER_H
#define SC_CLOCK_H
#define SC_SIGNAL_PORTS_H
#define SC_CLOCK_PORTS_H
#define SC_EVENT_QUEUE_H
#define SC_FIFO_H
#define SC_FIFO_PORTS_H
#define SC_MUTEX_H
#define SC_SEMAPHORE_H
#define SC_SIGNAL_RESOLVED_H
#define SC_SIGNAL_RV_H
#define SC_SIGNAL_RESOLVED_PORTS_H
#define SC_SIGNAL_RV_PORTS_H
#define SC_DYNAMIC_PROCESSES_H
#define sc_process_h_INCLUDED
#define sc_process_handle_h_INCLUDED

// Condensed version of sc_cmnhdr.h without include of windows.h
#define SC_CMNHDR_H
#if defined(_MSC_VER) || defined( __BORLANDC__ )
#ifndef WIN32
#define WIN32
#endif
#ifdef WIN64
#error WIN64 keyword is reserved for 64-bit Windows
#endif
#ifdef _MSC_VER
// Disable VC++ warnings that are harmless
// this : used in base member initializer list
#pragma warning(disable: 4355)
// new and delete warning when exception handling is turned on
#pragma warning(disable: 4291)
// in many places implicit conversion to bool
// from other integral types is performed
#pragma warning(disable: 4800)
// unary minus operator applied to unsigned
#pragma warning(disable: 4146)
// multiple copy constructors
#pragma warning(disable: 4521)
#define _WIN32_WINNT 0x0400
// DONT INCLUDE WINDOWS.H, JUST FPIEEE.H
#define _WINDOWS_
#include <fpieee.h>
#ifndef __SYNTHESIS__
// MSVC6.0 for() scope bug
#define for if ( false); else for
#endif
#endif
#ifdef __BORLANDC__
#define _WIN32_WINNT 0x0400
// DONT INCLUDE WINDOWS.H, JUST FPIEEE.H
#define _WINDOWS_
#include <fpieee.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include "sysc/kernel/sc_time.h"
#include "sysc/utils/sc_temporary.h"
#include "sysc/utils/sc_utils_ids.h"

namespace sc_core {
//-----------------------------------------------------------
// CLASS STUBS
//-----------------------------------------------------------
class sc_simcontext
{
public:
   sc_simcontext();
   ~sc_simcontext();
   const sc_time& time_stamp() const;
private:
    sc_time                    m_curr_time;
};
#if 0
class sc_simcontext
{
public:
   sc_simcontext();
   ~sc_simcontext();
};
#endif
class sc_process_b
{
public:
   sc_process_b();
   ~sc_process_b();
};
class sc_event
{
public:
   sc_event();
   ~sc_event();
};
class sc_event_list
{
public:
   sc_event_list();
   ~sc_event_list();
};
class sc_event_or_list
{
public:
   sc_event_or_list();
   ~sc_event_or_list();
};
class sc_event_and_list
{
public:
   sc_event_and_list();
   ~sc_event_and_list();
};

//-----------------------------------------------------------
// GLOBAL FUNCTION STUBS
//-----------------------------------------------------------

extern sc_process_b *sc_get_curr_process_handle();
extern sc_process_b *sc_get_current_process_b();
extern sc_simcontext *sc_get_curr_simcontext();
extern void wait(sc_simcontext * = 0);
extern void wait(const sc_event&, sc_simcontext * = 0);
extern void wait(sc_event_or_list&, sc_simcontext * = 0);
extern void wait(sc_event_and_list&, sc_simcontext * = 0);
extern void wait(const sc_time&, sc_simcontext * = 0);
extern void wait(double v, sc_time_unit tu, sc_simcontext *simc = 0);
extern const sc_time& sc_time_stamp();
extern void sc_stop();

} // namespace sc_core

#endif
