////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2018 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_AC_PIXELS_H_
#define _INCLUDED_AC_PIXELS_H_

// structures, functions and coefficients for image pixel manipulation

#include <ac_int.h>
#include <ac_channel.h>
#include <ac_fixed.h>

namespace ac_ipl {

  // RGB format - 1 Pixel Per Clock
  template <unsigned CDEPTH>
  struct RGB_1PPC {
    ac_int<CDEPTH,false> G; // TDATA = ZeroPad + R + B + G
    ac_int<CDEPTH,false> B;
    ac_int<CDEPTH,false> R;
    bool                 TUSER;
    bool                 TLAST;
  };

  // RGB format - 2 Pixels Per Clock
  template <unsigned CDEPTH>
  struct RGB_2PPC {
    ac_int<CDEPTH,false> G0; // TDATA = ZeroPad + R1 + B1 + G1 + R0 + B0 + G0
    ac_int<CDEPTH,false> B0;
    ac_int<CDEPTH,false> R0;
    ac_int<CDEPTH,false> G1;
    ac_int<CDEPTH,false> B1;
    ac_int<CDEPTH,false> R1;
    bool                 TUSER;
    bool                 TLAST;
  };

  // YUV - 1 Pixel Per Clock
  template <unsigned CDEPTH>
  struct YUV_1PPC {
    ac_int<CDEPTH,false> Y; // TDATA = ZeroPad + Cr + Cb + Y
    ac_int<CDEPTH,false> Cb;
    ac_int<CDEPTH,false> Cr;
    bool                 TUSER;
    bool                 TLAST;
  };
  
  // YUV 4:2:2 - 2 Pixels Per Clock
  template <unsigned CDEPTH>
  struct YUV422_2PPC {
    ac_int<CDEPTH,false> Y0; // TDATA = ZeroPad + Cr0 + Y1 + Cb0 + Y0
    ac_int<CDEPTH,false> Cb0;
    ac_int<CDEPTH,false> Y1;
    ac_int<CDEPTH,false> Cr0;
    bool                 TUSER;
    bool                 TLAST;
  };

  // YUV 4:4:4 - 2 Pixels Per Clock
  template <unsigned CDEPTH>
  struct YUV444_2PPC {
    ac_int<CDEPTH,false> Y0; // TDATA = ZeroPad + Cr1 + Cb1 + Y1 + Cr0 + Cb0 + Y0
    ac_int<CDEPTH,false> Cb0;
    ac_int<CDEPTH,false> Cr0;
    ac_int<CDEPTH,false> Y1;
    ac_int<CDEPTH,false> Cb1;
    ac_int<CDEPTH,false> Cr1;
    bool                 TUSER;
    bool                 TLAST;
  };

  // RGB -> YUV
  template <unsigned CDEPTH>
  inline YUV_1PPC<CDEPTH> ac_rgb_2_yuv(const RGB_1PPC<CDEPTH> rgbin) {
    ac_fixed<16,1,true> RGB2YUV_BT601[3][3] = {
      { 0.299    ,  0.587    ,  0.114    },
      {-0.168935 , -0.331655 ,  0.50059  },
      { 0.499813 , -0.418531 , -0.08128  }
    };
    YUV_1PPC<CDEPTH> yuv;
    ac_fixed<16,8,true> tmp;
    tmp  = RGB2YUV_BT601[0][0]*rgbin.R + RGB2YUV_BT601[0][1]*rgbin.G + RGB2YUV_BT601[0][2]*rgbin.B +   0; yuv.Y = tmp.to_uint();
    tmp  = RGB2YUV_BT601[1][0]*rgbin.R + RGB2YUV_BT601[1][1]*rgbin.G + RGB2YUV_BT601[1][2]*rgbin.B + 128; yuv.Cb = tmp.to_uint();
    tmp  = RGB2YUV_BT601[2][0]*rgbin.R + RGB2YUV_BT601[2][1]*rgbin.G + RGB2YUV_BT601[2][2]*rgbin.B + 128; yuv.Cr = tmp.to_uint();
    yuv.TUSER = rgbin.TUSER;
    yuv.TLAST = rgbin.TLAST;
    return yuv;
  }

  // RGB -> YUV 4:2:2
  template <unsigned CDEPTH>
  inline YUV422_2PPC<CDEPTH> ac_rgb_2_yuv422(const RGB_2PPC<CDEPTH> rgbin) {
    ac_fixed<16,1,true> RGB2YUV_BT601[3][3] = {
      { 0.299    ,  0.587    ,  0.114    },
      {-0.168935 , -0.331655 ,  0.50059  },
      { 0.499813 , -0.418531 , -0.08128  }
    };
    YUV422_2PPC<CDEPTH> yuv;
    ac_fixed<16,8,true> tmp;
    tmp  = RGB2YUV_BT601[0][0]*rgbin.R0 + RGB2YUV_BT601[0][1]*rgbin.G0 + RGB2YUV_BT601[0][2]*rgbin.B0 +   0; yuv.Y0  = tmp.to_uint();
    tmp  = RGB2YUV_BT601[1][0]*rgbin.R0 + RGB2YUV_BT601[1][1]*rgbin.G0 + RGB2YUV_BT601[1][2]*rgbin.B0 + 128; yuv.Cb0 = tmp.to_uint();
    tmp  = RGB2YUV_BT601[2][0]*rgbin.R0 + RGB2YUV_BT601[2][1]*rgbin.G0 + RGB2YUV_BT601[2][2]*rgbin.B0 + 128; yuv.Cr0 = tmp.to_uint();
    tmp  = RGB2YUV_BT601[0][0]*rgbin.R1 + RGB2YUV_BT601[0][1]*rgbin.G1 + RGB2YUV_BT601[0][2]*rgbin.B1 +   0; yuv.Y1  = tmp.to_uint();
    yuv.TUSER = rgbin.TUSER;
    yuv.TLAST = rgbin.TLAST;
    return yuv;
  }

  // YUV -> RGB
  template <unsigned CDEPTH>
  inline RGB_1PPC<CDEPTH> ac_yuv_2_rgb(const YUV_1PPC<CDEPTH> yuvin) {
    ac_fixed<16,2,true> YUV2RGB_BT601[3][3] = {
      { 1.0      ,  0.0      ,  1.403    },
      { 1.0      , -0.344    , -0.714    },
      { 1.0      ,  1.770    ,  0.0      }
    };
    RGB_1PPC rgb;
    ac_fixed<16,8,true> tmp;
    ac_fixed<16,8,true> tmpY = yuvin.Y;
    ac_fixed<16,8,true> tmpU = yuvin.Cb-128;
    ac_fixed<16,8,true> tmpV = yuvin.Cr-128;
    tmp  = YUV2RGB_BT601[0][0]*tmpY + YUV2RGB_BT601[0][1]*tmpU + YUV2RGB_BT601[0][2]*tmpV; rgb.R = tmp.to_uint();
    tmp  = YUV2RGB_BT601[1][0]*tmpY + YUV2RGB_BT601[1][1]*tmpU + YUV2RGB_BT601[1][2]*tmpV; rgb.G = tmp.to_uint();
    tmp  = YUV2RGB_BT601[2][0]*tmpY + YUV2RGB_BT601[2][1]*tmpU + YUV2RGB_BT601[2][2]*tmpV; rgb.B = tmp.to_uint();
    rgb.TUSER = yuvin.TUSER;
    rgb.TLAST = yuvin.TLAST;
    return rgb;
  }

  // YUV -> RGB
  template <unsigned CDEPTH>
  inline RGB_2PPC<CDEPTH> ac_yuv422_2_rgb(const YUV422_2PPC<CDEPTH> yuvin) {
    ac_fixed<16,2,true> YUV2RGB_BT601[3][3] = {
      { 1.0      ,  0.0      ,  1.403    },
      { 1.0      , -0.344    , -0.714    },
      { 1.0      ,  1.770    ,  0.0      }
    };
    RGB_2PPC rgb;
    ac_fixed<16,8,true> tmp;
    ac_fixed<16,8,true> tmpY0 = yuvin.Y0;
    ac_fixed<16,8,true> tmpU = yuvin.Cb0-128;
    ac_fixed<16,8,true> tmpV = yuvin.Cr0-128;
    ac_fixed<16,8,true> tmpY1 = yuvin.Y1;
    tmp  = YUV2RGB_BT601[0][0]*tmpY0 + YUV2RGB_BT601[0][1]*tmpU + YUV2RGB_BT601[0][2]*tmpV; rgb.R0 = tmp.to_uint();
    tmp  = YUV2RGB_BT601[1][0]*tmpY0 + YUV2RGB_BT601[1][1]*tmpU + YUV2RGB_BT601[1][2]*tmpV; rgb.G0 = tmp.to_uint();
    tmp  = YUV2RGB_BT601[2][0]*tmpY0 + YUV2RGB_BT601[2][1]*tmpU + YUV2RGB_BT601[2][2]*tmpV; rgb.B0 = tmp.to_uint();
    tmp  = YUV2RGB_BT601[0][0]*tmpY1 + YUV2RGB_BT601[0][1]*tmpU + YUV2RGB_BT601[0][2]*tmpV; rgb.R1 = tmp.to_uint();
    tmp  = YUV2RGB_BT601[1][0]*tmpY1 + YUV2RGB_BT601[1][1]*tmpU + YUV2RGB_BT601[1][2]*tmpV; rgb.G1 = tmp.to_uint();
    tmp  = YUV2RGB_BT601[2][0]*tmpY1 + YUV2RGB_BT601[2][1]*tmpU + YUV2RGB_BT601[2][2]*tmpV; rgb.B1 = tmp.to_uint();
    rgb.TUSER = yuvin.TUSER;
    rgb.TLAST = yuvin.TLAST;
    return rgb;
  }

};

#endif

