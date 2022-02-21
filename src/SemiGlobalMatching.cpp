#include <cstring>
#include <algorithm>

#include "SemiGlobalMatching.h"
#include "sgmUtils.h"

SemiGlobalMatching::SemiGlobalMatching()
{

}

SemiGlobalMatching::~SemiGlobalMatching()
{
    Release();
}

bool SemiGlobalMatching::Initialize(const uint32_t& width, const uint32_t& height, const SGMOptions& options)
{

    if(width == 0 || height == 0)
    {
        return false;
    }

    width_ = width;
    height_ = height;
    options_ = options;

    const int32_t dispRange = options.maxDisp - options.minDisp;

    if(dispRange <= 0) 
    {
        return false;
    }
    // memory initialize
    censusLeft_ = new uint32_t[width * height]();
    censusRight_ = new uint32_t[width * height]();

    // path <= 8
    costInit_ = new uint8_t[width * height * dispRange]();
    costAggre_ = new uint16_t[width * height * dispRange]();

    dispLeft_ = new float[width * height]();

    isInitialize_ = censusLeft_ && censusRight_ && costInit_ && costAggre_ && dispLeft_;

    return isInitialize_;
}

bool SemiGlobalMatching::Match(uint8_t* imgLeft, uint8_t* imgRight, float* dispLeft)
{
    if(!isInitialize_) return false;

    if(imgLeft == nullptr || imgRight == nullptr) return false;

    imgLeft_ = imgLeft;
    imgRight_ = imgRight;

    CensusTransform();
    ComputeCost();
    CostAggregation();
    ComputeDisp();

    memcpy(dispLeft, dispLeft_, width_ * height_ * sizeof(float));

    return true;
}

void SemiGlobalMatching::CensusTransform()
{
    sgmUtils::census5x5(imgLeft_, censusLeft_, width_, height_);
    sgmUtils::census5x5(imgRight_, censusRight_, width_, height_);
}

void SemiGlobalMatching::ComputeCost()
{
    int16_t dispRange = options_.maxDisp - options_.minDisp;

    for(int i=2; i<height_-2; i++)
    {
        for(int j=2; j<width_-2; j++)
        {
            for(int k=options_.minDisp; k<options_.maxDisp; k++)
            {
                auto& cost = costInit_[i*width_*dispRange + j*dispRange + k - options_.minDisp];
                
                if(j-k < 0 || j-k >= width_)
                {
                    cost = UINT8_MAX;
                    continue;
                } 

                cost = sgmUtils::hammingDist(censusLeft_[i*width_ + j], censusRight_[i*width_ + j - k]);
            }
        }
    }
}

void SemiGlobalMatching::CostAggregation()
{
    const int size = width_ * height_ * (options_.maxDisp - options_.minDisp);

    for(int i=0; i<size; i++) costAggre_[i] = 0;

    sgmUtils::costAggreLeft(
        imgLeft_, costInit_, costAggre_, width_, height_, 
        options_.p1, options_.p2_int, options_.minDisp, options_.maxDisp
    );

    sgmUtils::costAggreRight(
        imgLeft_, costInit_, costAggre_, width_, height_, 
        options_.p1, options_.p2_int, options_.minDisp, options_.maxDisp
    );

    sgmUtils::costAggreTop(
        imgLeft_, costInit_, costAggre_, width_, height_, 
        options_.p1, options_.p2_int, options_.minDisp, options_.maxDisp
    );

    sgmUtils::costAggreBottom(
        imgLeft_, costInit_, costAggre_, width_, height_, 
        options_.p1, options_.p2_int, options_.minDisp, options_.maxDisp
    );
}

void SemiGlobalMatching::ComputeDisp()
{
    const int16_t& minDisp = options_.minDisp;
    const int16_t& maxDisp = options_.maxDisp;
    int16_t dispRange = maxDisp - minDisp;

    auto costPtr = costAggre_;

    for(int i=0; i<height_; i++)
    {
        for(int j=0; j<width_; j++)
        {
            uint16_t minCost = UINT16_MAX;
            uint16_t maxCost = 0;
            float bestDisp = 0;

            for(int k=minDisp; k<maxDisp; k++)
            {
                // winner take all
                const auto& cost = costPtr[i*width_*dispRange + j*dispRange + k-minDisp];
                if(cost < minCost)
                {
                    bestDisp = k;
                    minCost = cost;
                }
                maxCost = std::max(maxCost, static_cast<uint16_t>(cost));
            }

            if(maxCost > minCost)
            {
                dispLeft_[i*width_ + j] = bestDisp;
            }
            else
            {
                dispLeft_[i*width_ + j] = sgmUtils::invalidFloat;
            }
        }
    }
}

bool SemiGlobalMatching::Reset(const uint32_t& width, const uint32_t& height, const SGMOptions& options)
{
    Release();

    return Initialize(width, height, options);
}

void SemiGlobalMatching::Release()
{
    if(censusLeft_ != nullptr)
    {
        delete[] censusLeft_;
        censusLeft_ = nullptr;
    }

    if(censusRight_ != nullptr)
    {
        delete[] censusRight_;
        censusRight_ = nullptr;
    }

    if(costInit_ != nullptr)
    {
        delete[] costInit_;
        costInit_ = nullptr;
    }

    if(costAggre_ != nullptr)
    {
        delete[] costAggre_;
        costAggre_ = nullptr;
    }

    if(dispLeft_ != nullptr)
    {
        delete[] dispLeft_;
        dispLeft_ = nullptr;
    }

    isInitialize_ = false;
}