#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

#include "SemiGlobalMatching.h"
#include "sgmUtils.h"

int main(int argv, char** argc)
{
    if(argv < 3)
    {
        return 0;
    }

    std::string leftPath = argc[1];
    std::string rightPath = argc[2];

    cv::Mat imgL = cv::imread(leftPath, cv::IMREAD_GRAYSCALE);
    cv::Mat imgR = cv::imread(rightPath, cv::IMREAD_GRAYSCALE);

    if(imgL.data == nullptr || imgR.data == nullptr)
    {
        std::cout << "img loading fail!" << std::endl;
        return -1; 
    }

    if(imgL.rows != imgR.rows || imgL.cols != imgR.cols)
    {
        std::cout << "different size for left and right img!" << std::endl;
    }

    uint32_t rows = static_cast<uint32_t>(imgL.rows);
    uint32_t cols = static_cast<uint32_t>(imgL.cols);

    SemiGlobalMatching::SGMOptions sgmOpt;

    sgmOpt.numPaths = 8;
    sgmOpt.minDisp = 0;
    sgmOpt.maxDisp = 64;
    sgmOpt.p1 = 10;
    sgmOpt.p2_int = 150;

    SemiGlobalMatching sys;

    if(!sys.Initialize(cols, rows, sgmOpt))
    {
        std::cout << "Initialize failed!" << std::endl;
        return -1;
    }
    
    auto dispLeft = new float[rows * cols]();
    if(!sys.Match(imgL.data, imgR.data, dispLeft))
    {
        std::cout << "Disp failed!" << std::endl;
        return -1;
    }

    cv::Mat dispMat = cv::Mat(rows, cols, CV_8UC1);

    // visualize
    for(uint32_t i=0; i<rows; i++)
    {
        for(uint32_t j=0; j<cols; j++)
        {
            const float dispVal = dispLeft[i*cols + j];

            if(dispVal == sgmUtils::invalidFloat)
            {
                dispMat.data[i*cols + j] = 0;
            }
            else
            {
                dispMat.data[i*cols + j] = 2 * static_cast<uchar>(dispLeft[i*cols + j]);
            }
        }
    }

    cv::imwrite(argc[3], dispMat);
    // cv::imshow("disp", dispMat);
    // cv::waitKey(0);

    delete[] dispLeft;
    dispLeft = nullptr;
    
	return 0;
}