#pragma once

#include <memory>
#include <opencv2/core/core.hpp>
#include "utils.hpp"
#include "aux_utils.h" //auxdev file add by szk
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
    static float compareFeature(const float* src, const float* dst, uint32_t len);
    float compareFeature(const float* src, uint32_t src_len, const float* dst, uint32_t dst_len);

    ProcessingState parseInfo(PalmImageInfo& info);
    ProcessingState findRoi(unsigned char* roi_data, uint32_t& width, uint32_t& height, uint32_t& roi_data_len);
    ProcessingState computeFeature(std::vector<uint8_t>& features);
    ProcessingState computeFeature(std::vector<float>& features);
    bool loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path);
    AuxDev::StatusInfo _statusInfo;
    int loadStatusInfo();
    int clearStatusInfo();
private:
    PalmVeinParser_impl* impl_;
};

class PalmVeinTemplate_impl;
class PalmVeinTemplate
{
public:
    explicit PalmVeinTemplate(const uint32_t id);
    virtual ~PalmVeinTemplate();
    void feed(const float* feat, uint32_t len);
    int check(bool &ok);
    bool buildTemplate(std::vector<float>& model);
private:
    PalmVeinTemplate_impl* impl_;
};

}
