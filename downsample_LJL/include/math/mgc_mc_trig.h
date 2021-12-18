#ifndef __MGC_MC_TRIG_H
#define __MGC_MC_TRIG_H

#ifndef __MC_TYPES_DONT_WARN
#if defined(__EDG__) || defined(__GNUC__)
#warning Deprecated include mgc_mc_trig.h, replace with mgc_ac_trig.h
#endif
#endif

#define __MC_TYPES_DONT_WARN
#include "mc_fixed.h"
#undef __MC_TYPES_DONT_WARN
#include "mgc_ac_trig.h"

#endif
