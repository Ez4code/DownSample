#ifndef _CLASS_HIER_DESIGN_H_
#define _CLASS_HIER_DESIGN_H_

#include <ac_int.h>
#include <ac_channel.h>
#include "down_ln.h"
#include "down_shreg.h"
#include <ccs_bmpfile.h>
#include "mc_scverify.h"

#pragma hls_design block
void DownSample (ac_channel<uint8> &Image, ac_channel<uint8> &DownImage);     //end DownSample block


#pragma hls_design top
void go (ac_channel<uint8> &in,
          ac_channel<uint8> &out);

#endif


