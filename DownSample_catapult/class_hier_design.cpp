#include "class_hier_design.h"

#pragma hls_design block
void DownSample (ac_channel<uint8> &Image, ac_channel<uint8> &DownImage){

    static down_ln raw_ln_data;
    static down_shreg raw_shreg_data;
    if (Image.available(1))
    {
        for (uint9 ln = 0; ln < 480; ln++)
        {
            for (uint10 px = 0; px < 752; px++)
            {
                uint8 pixel = Image.read();
                raw_ln_data.process(pixel, ln, px);
                raw_shreg_data.process(raw_ln_data.rd_data, ln, px);

                if (ln % 2 == 1 && px % 2 == 1)
                {
                    //图像降采样
                    uint8 down_pixel = (raw_shreg_data.shreg_y0[0] + raw_shreg_data.shreg_y0[1] +
                                        raw_shreg_data.shreg_y1[0] + raw_shreg_data.shreg_y1[1]) /
                                       4;
                    DownImage.write(down_pixel);
                    uint9 down_ln, down_px;
                    down_ln = ln / 2;
                    down_px = px / 2;
                }
            }
        }
    }
}       //end DownSample block


#pragma hls_design top
void go (ac_channel<uint8> &in,
          ac_channel<uint8> &out){

    DownSample(in, out);
}



