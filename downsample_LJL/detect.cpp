#pragma hls_design block
void FP1_detect(ac_channel<uint8> &image, ac_channel<uint8> &down_image
                )
{

    static down_ln      raw_ln_data;
    static down_shreg   raw_shreg_data;


    for(uint9 ln = 0; ln < 480; ln++) {
       for (uint10 px = 0; px < 752; px++) {

           uint8 pixel = image.read();
           raw_ln_data.process(pixel,ln, px);
           raw_shreg_data.process(raw_ln_data.rd_data, ln, px);

           if ( ln%2 == 1 && px%2 == 1 )
           {
               //图像降采样
               uint8 down_pixel= (raw_shreg_data.shreg_y0[0] + raw_shreg_data.shreg_y0[1] +
                       raw_shreg_data.shreg_y1[0] + raw_shreg_data.shreg_y1[1])/4;
               down_image.write(down_pixel);
               uint9 down_ln, down_px;
               down_ln = ln / 2;
               down_px = px / 2;
           }
       }
    }
}
