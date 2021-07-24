#include <iostream>
#include "palmveinparser_impl.hpp"
#include "palm_vein.hpp"

namespace OVS
{
PalmVein::PalmVein(const unsigned char* image_data, 
                    const uint32_t width, const uint32_t height, const uint32_t depth)
    : impl_(new PalmVeinParser_impl(image_data, width, height, depth))
{

}

PalmVein::~PalmVein()
{
    delete impl_;
}

ProcessingState PalmVein::parseInfo(PalmImageInfo& info)
{
    return impl_->parse_info(info);
}

ProcessingState PalmVein::findRoi(unsigned char* roi_data, 
                uint32_t& width, uint32_t& height, uint32_t& roi_data_len)
{
    return impl_->find_roi(roi_data, width, height, roi_data_len);
}

ProcessingState PalmVein::computeFeature(std::vector<uint8_t>& features)
{
    return impl_->compute_feature(features);
}

ProcessingState PalmVein::computeFeature(std::vector<float>& features)
{
    return impl_->compute_feature(features);
}

uint32_t numbits_lookup_table[256] = {

    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2,

    3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3,

    3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,

    4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4,

    3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,

    6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,

    4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,

    6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5,

    3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3,

    4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6,

      6, 7, 6, 7, 7, 8

};

uint32_t PalmVein::compareFeature(const uint8_t* src, const uint8_t* dst, uint32_t len)
{
    assert(src && dst);

    uint32_t hamming = 0;
    for(uint32_t i = 0; i < len; ++i)
    {
        uint8_t v = src[i] ^ dst[i];
        hamming += numbits_lookup_table[v];
    }

    return hamming;
}


float PalmVein::compareFeature(const float* src, const float* dst, uint32_t len)
{
    assert(src && dst);

    double l2 = 0;
    for(uint32_t i = 0; i < len; ++i)
    {
        double v = src[i] - dst[i];
        l2 += v * v;
    }

    return (float)sqrt(1000.0f * l2 / len);
}

bool PalmVein::loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path)
{
    return impl_->loadModelFiles(palm_model_path, finger_model_path);
}

} //namespace OVS