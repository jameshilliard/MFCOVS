#ifndef ALGO_MATH_H
#define ALGO_MATH_H

#include <stdint.h>

class AlgoMatch{
public:
   static AlgoMatch* instance();
    AlgoMatch();
    ~AlgoMatch();

    bool computeFeature(const float* src, const float* dst, uint32_t len);
};

#endif
