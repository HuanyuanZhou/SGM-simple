#pragma once
#include <stdint.h>

class SemiGlobalMatching
{
public:
    SemiGlobalMatching();
    ~SemiGlobalMatching();

public:
    struct SGMOptions
    {
        uint8_t numPaths;
        int16_t minDisp;
        int16_t maxDisp;

        int32_t p1;
        int32_t p2_int;

        SGMOptions(): numPaths(8), minDisp(0), maxDisp(64), p1(10), p2_int(150){}
    };

    bool Initialize(const uint32_t& width, const uint32_t& height, const SGMOptions& options);
    bool Match(uint8_t* imgLeft, uint8_t* imgRight, float* dispLeft);
    bool Reset(const uint32_t& width, const uint32_t& height, const SGMOptions& options);

private:
    void CensusTransform();
    void ComputeCost();
    void CostAggregation();
    void ComputeDisp();
    void LRcheck();
    void Release();

private:
    SGMOptions options_;
    uint32_t width_;
    uint32_t height_;
    
    const uint8_t* imgLeft_;
    const uint8_t* imgRight_;

    uint32_t* censusLeft_;
    uint32_t* censusRight_;

    uint8_t* costInit_;
    uint16_t* costAggre_;
    float* dispLeft_;

    bool isInitialize_;
    
};