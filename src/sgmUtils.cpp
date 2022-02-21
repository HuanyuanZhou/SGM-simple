#include "sgmUtils.h"
#include <iostream>

void sgmUtils::census5x5(const uint8_t* img, uint32_t* census, const uint32_t& width, const uint32_t& height)
{
    if(img == nullptr || census == nullptr || width < 5u || height < 5u)
    {
        return;
    }

    for(int i=2; i<height-2; i++)
    {
        for(int j=2; j<width-2; j++)
        {
            uint8_t censusGray = img[i*width + j];
            uint32_t censusValue = 0u;

            for(int x=j-2; x<=j+2; x++)
            {
                for(int y=i-2; y<=i+2; y++)
                {
                    censusValue <<= 1;
                    if(img[y*width + x] < censusGray)
                    {
                        censusValue += 1;
                    }
                }
            }

            census[i*width + j] = censusValue;
        }   
    }
}


uint8_t sgmUtils::hammingDist(const uint32_t& x, const uint32_t& y)
{
    uint32_t val = x^y;
    uint8_t dist = 0u;

    while(val)
    {
        dist++;
        val &= val-1;
    }

    return dist;
}


void sgmUtils::costAggreLeft(
    const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
    const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp 
)
{
    int16_t dispRange = maxDisp - minDisp;

    for(int i=2; i<height-2; i++)
    {
        // calculate first dp[0]
        std::vector<uint16_t> pre(dispRange+2, UINT16_MAX/2);
        std::vector<uint16_t> now(dispRange+2, UINT16_MAX/2);
        
        uint16_t minNow = UINT16_MAX;
        int j = 2;

        for(int k=minDisp; k<maxDisp; k++)
        {
            int idx = k - minDisp + 1;
            pre[idx] = costInit[i*width*dispRange + j*dispRange + k-minDisp];
            minNow = std::min(minNow, pre[idx]);
        }
        
        uint16_t minPre = minNow;
        uint8_t grayPre = img[i*width + j];
        minNow = UINT16_MAX;
        j++;

        // dynamic calculation dp[i]
        for(; j<width-2; j++)
        {
            uint8_t grayNow = img[i*width + j];

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idxPre = k - minDisp + 1;
                int idxCost = i*width*dispRange + j*dispRange + k-minDisp;
                
                const uint16_t l0 = static_cast<uint16_t>(costInit[idxCost]);
                const uint16_t l1 = pre[idxPre-1] + p1;
                const uint16_t l2 = pre[idxPre];
                const uint16_t l3 = pre[idxPre+1] + p1;
                const uint16_t l4 = minPre + std::max(p1, p2_int / (abs(grayNow - grayPre) + 1));
                const uint16_t cost = l0 + std::min(l1, std::min(l2, std::min(l3, l4))) - minPre;
    
                now[idxPre] = cost;
                costAggre[idxCost] += cost; 
                minNow = std::min(minNow, now[idxPre]);
            }

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idx = k - minDisp + 1;
                pre[idx] = now[idx];
            }

            minPre = minNow;
            grayPre = grayNow;
            minNow = UINT16_MAX;
        }
    }
}


void sgmUtils::costAggreRight(
    const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
    const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp 
)
{
    int16_t dispRange = maxDisp - minDisp;

    for(int i=2; i<height-2; i++)
    {
        // calculate first dp[0]
        std::vector<uint16_t> pre(dispRange+2, UINT16_MAX/2);
        std::vector<uint16_t> now(dispRange+2, UINT16_MAX/2);
        
        uint16_t minNow = UINT16_MAX;
        int j = width - 3;

        for(int k=minDisp; k<maxDisp; k++)
        {
            int idx = k - minDisp + 1;
            pre[idx] = costInit[i*width*dispRange + j*dispRange + k-minDisp];
            minNow = std::min(minNow, pre[idx]);
        }
        
        uint16_t minPre = minNow;
        uint8_t grayPre = img[i*width + j];
        minNow = UINT16_MAX;
        j--;

        // dynamic calculation dp[i]
        for(; j>=2; j--)
        {
            uint8_t grayNow = img[i*width + j];

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idxPre = k - minDisp + 1;
                int idxCost = i*width*dispRange + j*dispRange + k-minDisp;
                
                const uint16_t l0 = static_cast<uint16_t>(costInit[idxCost]);
                const uint16_t l1 = pre[idxPre-1] + p1;
                const uint16_t l2 = pre[idxPre];
                const uint16_t l3 = pre[idxPre+1] + p1;
                const uint16_t l4 = minPre + std::max(p1, p2_int / (abs(grayNow - grayPre) + 1));
                const uint16_t cost = l0 + std::min(l1, std::min(l2, std::min(l3, l4))) - minPre;
    
                now[idxPre] = cost;
                costAggre[idxCost] += cost; 
                minNow = std::min(minNow, now[idxPre]);
            }

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idx = k - minDisp + 1;
                pre[idx] = now[idx];
            }

            minPre = minNow;
            grayPre = grayNow;
            minNow = UINT16_MAX;
        }
    }
}


void sgmUtils::costAggreTop(
    const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
    const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp 
)
{
    int16_t dispRange = maxDisp - minDisp;

    for(int j=2; j<width-2; j++)
    {
        // calculate first dp[0]
        std::vector<uint16_t> pre(dispRange+2, UINT16_MAX/2);
        std::vector<uint16_t> now(dispRange+2, UINT16_MAX/2);
        
        uint16_t minNow = UINT16_MAX;

        int i = 2;

        for(int k=minDisp; k<maxDisp; k++)
        {
            int idx = k - minDisp + 1;
            pre[idx] = costInit[i*width*dispRange + j*dispRange + k-minDisp];
            minNow = std::min(minNow, pre[idx]);
        }
        
        uint16_t minPre = minNow;
        uint8_t grayPre = img[i*width + j];
        minNow = UINT16_MAX;
        i++;

        // dynamic calculation dp[i]
        for(; i<height-2; i++)
        {
            uint8_t grayNow = img[i*width + j];

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idxPre = k - minDisp + 1;
                int idxCost = i*width*dispRange + j*dispRange + k-minDisp;
                
                const uint16_t l0 = static_cast<uint16_t>(costInit[idxCost]);
                const uint16_t l1 = pre[idxPre-1] + p1;
                const uint16_t l2 = pre[idxPre];
                const uint16_t l3 = pre[idxPre+1] + p1;
                const uint16_t l4 = minPre + std::max(p1, p2_int / (abs(grayNow - grayPre) + 1));
                const uint16_t cost = l0 + std::min(l1, std::min(l2, std::min(l3, l4))) - minPre;
    
                now[idxPre] = cost;
                costAggre[idxCost] += cost; 
                minNow = std::min(minNow, now[idxPre]);
            }

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idx = k - minDisp + 1;
                pre[idx] = now[idx];
            }

            minPre = minNow;
            grayPre = grayNow;
            minNow = UINT16_MAX;
        }
    }
}


void sgmUtils::costAggreBottom(
    const uint8_t* img, const uint8_t* costInit, uint16_t* costAggre, const uint32_t& width, const uint32_t& height,
    const int32_t& p1, const int32_t& p2_int, const int16_t& minDisp, const int16_t& maxDisp 
)
{
    int16_t dispRange = maxDisp - minDisp;

    for(int j=2; j<width-2; j++)
    {
        // calculate first dp[0]
        std::vector<uint16_t> pre(dispRange+2, UINT16_MAX/2);
        std::vector<uint16_t> now(dispRange+2, UINT16_MAX/2);
        
        uint16_t minNow = UINT16_MAX;

        int i = height-3;

        for(int k=minDisp; k<maxDisp; k++)
        {
            int idx = k - minDisp + 1;
            pre[idx] = costInit[i*width*dispRange + j*dispRange + k-minDisp];
            minNow = std::min(minNow, pre[idx]);
        }
        
        uint16_t minPre = minNow;
        uint8_t grayPre = img[i*width + j];
        minNow = UINT16_MAX;
        i--;

        // dynamic calculation dp[i]
        for(; i>=0; i--)
        {
            uint8_t grayNow = img[i*width + j];

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idxPre = k - minDisp + 1;
                int idxCost = i*width*dispRange + j*dispRange + k-minDisp;
                
                const uint16_t l0 = static_cast<uint16_t>(costInit[idxCost]);
                const uint16_t l1 = pre[idxPre-1] + p1;
                const uint16_t l2 = pre[idxPre];
                const uint16_t l3 = pre[idxPre+1] + p1;
                const uint16_t l4 = minPre + std::max(p1, p2_int / (abs(grayNow - grayPre) + 1));
                const uint16_t cost = l0 + std::min(l1, std::min(l2, std::min(l3, l4))) - minPre;
    
                now[idxPre] = cost;
                costAggre[idxCost] += cost; 
                minNow = std::min(minNow, now[idxPre]);
            }

            for(int k=minDisp; k<maxDisp; k++)
            {
                int idx = k - minDisp + 1;
                pre[idx] = now[idx];
            }

            minPre = minNow;
            grayPre = grayNow;
            minNow = UINT16_MAX;
        }
    }
}