#ifndef _DOWN_SHREG_
#define _DOWN_SHREG_

#include <ac_int.h>

class down_shreg
{
public:
    uint8 shreg_y0[2];
    uint8 shreg_y1[2];
    void process(uint8 *data, int ln, int px)
    {
        if (px % 2 == 0)
        {
            shreg_y0[0] = data[0];
            shreg_y1[0] = data[1];
        }
        if (px % 2 != 0)
        {
            shreg_y0[1] = data[0];
            shreg_y1[1] = data[1];
        }
    }
};
#endif
