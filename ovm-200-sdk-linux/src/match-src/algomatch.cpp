#include "algomatch.h"
#include "palm_vein.hpp"
#include "logger.h"
#include "math.h"

#include <string>

using namespace std;

const float conStandardHamming = 0.60f;

AlgoMatch::~AlgoMatch()
{

}

bool AlgoMatch::computeFeature(const float *src,
                               const float *dst,
                               uint32_t len)
{
    if((src == nullptr) || (dst == nullptr) ){
        return false;
    }

    //display data
#if 0
    for(int i = 0; i < len; i++){
        logde<<"compute feature src:"<<i<<"|"<<src[i];
//        logde<<"compute feature dst:"<<dst[i];
    }
#endif



    float hamming = 0.0f;
    hamming = OVS::PalmVein::compareFeature(src,dst,len);

    if(conStandardHamming >= hamming){
        logde<<"hamming:"<<hamming;
        return true;
    }else{
        return false;
    }
}

AlgoMatch *AlgoMatch::instance()
{
    static AlgoMatch ins;
    return &ins;
}

AlgoMatch::AlgoMatch()
{
    Logger::instance();
}
