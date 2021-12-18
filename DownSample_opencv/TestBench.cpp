#include <ac_int.h>
#include <ac_channel.h>
#include <opencv2/opencv.hpp>
#include "class_hier_design.h"
#include "opencv/highgui.h"

using namespace std;
using namespace cv;

int main()
{
    top_DownSample inst;
    ac_channel<uint8> din;
    ac_channel<uint8> dout;

    cv::Mat src = cv::imread("/home/boy666/workspace/CLionWs/test/2.jpg", 1);
    cv::Mat rgb2grayImg;
    cv::Mat DownSample = cv::Mat::zeros( 240, 376, CV_8UC1);
    cvtColor(src, rgb2grayImg, CV_RGB2GRAY);

    //write input into din
    for (uint9 ln = 0; ln < 480; ln++)
    {
        for (uint10 px = 0; px < 752; px++)
        {
            uint8 temp = int(rgb2grayImg.at<uchar>(ln, px));
            din.write(temp);
        }
    }
    while (din.available(1))
    {
        inst.run(din,dout);
    }

    uint9 ln_out, px_out = 0;
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

        DownSample.at<uchar>(ln_out, px_out) = dout.read();
        px_out = px_out + 1;
    }
    imshow("rgb2grayImg",rgb2grayImg);
    imshow("DownSample",DownSample);
    cvWaitKey(0);
    return 0;
}