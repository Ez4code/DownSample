#ifndef _DOWN_LN_
#define _DOWN_LN_

#include <ac_int.h>

using namespace std;
class down_ln
{
private:
    uint16 wr_buffer;
    uint16 rd_buffer;

public:
    uint16 line0[376];
    uint8 rd_data[2];
    void process(uint8 data, int ln, int px)
    {
        int addr = (px - 0) / 2;

        if (px % 2 == 0)
        {
            if (ln % 2 == 1) //read
            {
                rd_buffer = line0[addr];
                rd_data[0] = rd_buffer.slc<8>(0);
                rd_data[1] = data;
            }
            if (ln % 2 == 0) //write
            {
                wr_buffer.set_slc(0, data);
            }
        }
        else
        {
            if (ln % 2 == 1) //read
            {
                rd_data[0] = rd_buffer.slc<8>(8);
                rd_data[1] = data;
            }
            if (ln % 2 == 0) //write
            {
                wr_buffer.set_slc(8, data);
                line0[addr] = wr_buffer;
            }
        }
    }
};
#endif
