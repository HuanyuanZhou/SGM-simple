#pragma once
#include <stdint.h>
#include <limits>
#include <vector>
#include <algorithm>

namespace sgmUtils
{
    const float invalidFloat = std::numeric_limits<float>::infinity();

    void census5x5(const uint8_t* img, uint32_t* census, const uint32_t& width, const uint32_t& height);
    uint8_t hammingDist(const uint32_t& x, const uint32_t& y);

    void costAggreLeft(
        const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
        const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp
    );

    void costAggreRight(
        const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
        const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp
    );

    void costAggreTop(
        const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
        const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp
    );

    void costAggreBottom(
        const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
        const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp
    );
};