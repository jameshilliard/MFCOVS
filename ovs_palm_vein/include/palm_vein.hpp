#pragma once

#include <memory>
#include <opencv2/core/core.hpp>
#include "utils.hpp"

namespace OVS
{

class PalmVeinParser_impl;

class PalmVein
{
public:
    explicit PalmVein(const unsigned char* image_data, 
                    const uint32_t width, const uint32_t height, const uint32_t depth=3);
    virtual ~PalmVein();

    static uint32_t compareFeature(const uint8_t* src, const uint8_t* dst, uint32_t len);

    ProcessingState parseInfo(PalmImageInfo& info);
    ProcessingState findRoi(unsigned char* roi_data, uint32_t& width, uint32_t& height, uint32_t& roi_data_len);
    ProcessingState computeFeature(std::vector<uint8_t>& features);

    bool loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path);
private:
    PalmVeinParser_impl* impl_;
};


}