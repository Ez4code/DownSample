#include <ac_int.h>
#include <ac_channel.h>
#include "class_hier_design.h"
#include "ccs_bmpfile.h"
#include <fstream>
#include <bitset>
#include "mc_scverify.h"

using namespace std;

#pragma hls_design top
CCS_MAIN(int argc, char *argv[])
//int main()
{
    top_DownSample inst;
    ac_channel<uint8> din;
    ac_channel<uint8> dout;
    uint32 test = 0;
    uint9 ln_out = 0;
    uint9 px_out = 0;

    const char * srcFilename = "/home/boy666/workspace/CLionWs/DownSample/2.bmp";
    ccs_bmpfile *src = new ccs_bmpfile(srcFilename);


    //write input into din
    for (uint9 ln = 0; ln < 480; ln++)
    {
        for (uint10 px = 0; px < 752; px++)
        {
            uint8 src_R = src->red(ln,px);
            din.write(src_R);
        }
    }
    while (din.available(1))
    {
        inst.run(din,dout);
    }


    while (dout.available(1))
    {
        if (px_out >= 376)
        {
            px_out = 0;
            ln_out = ln_out + 1;
        }
        if (ln_out >= 240)
        {
            ln_out = 0;
        }
        uint8 temp = dout.read();
        if((px_out == 120) && (ln_out%75 == 0))
        {
            test.set_slc(ln_out*8/75,temp);
        }
        px_out = px_out + 1;
    }
    cout << bitset<32>(test)  << endl;
    //return 0;
    CCS_RETURN(0);
}