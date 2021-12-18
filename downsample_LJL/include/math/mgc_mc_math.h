#ifndef __MGC_MC_MATH_H
#define __MGC_MC_MATH_H

#ifndef __MC_TYPES_DONT_WARN
#if defined(__EDG__) || defined(__GNUC__)
#warning Deprecated include mgc_mc_math.h, replace with mgc_ac_math.h
#endif
#endif

#define __MC_TYPES_DONT_WARN
#include "mc_fixed.h"
#undef __MC_TYPES_DONT_WARN

#include "mgc_ac_math.h"

#endif
