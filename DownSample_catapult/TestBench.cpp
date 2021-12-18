#include <ac_int.h>
#include <ac_channel.h>
#include "class_hier_design.h"
#include "ccs_bmpfile.h"
#include <fstream>
#include <bitset>
#include "mc_scverify.h"

using namespace std;

CCS_MAIN(int argc, char *argv[])
//int main()
{
    ac_channel<uint8> din;
    ac_channel<uint8> dout;
    uint32 test = 0;
    uint9 ln_out = 0;
    uint9 px_out = 0;
    uint8 src_R = 0;

   const char * srcFilename = "/home/tiger/2.bmp";
   ccs_bmpfile *src = new ccs_bmpfile(srcFilename);


    //write input into din
    for (uint9 ln = 0; ln < 480; ln++)
    {
        for (uint10 px = 0; px < 752; px++)
        {
            src_R = src->red(ln,px);
            din.write(src_R);
        }
    }
    while (din.available(1))
    {
        CCS_DESIGN(go)(din,dout);
    }

    //return 0;
    CCS_RETURN(0);
}