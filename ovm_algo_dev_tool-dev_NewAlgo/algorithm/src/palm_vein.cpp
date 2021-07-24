#include <iostream>
#include "algorithm/include/palmveinparser_impl.hpp"
#include "algorithm/include/palmveintemplate_impl.hpp"
#include "algorithm/include/palm_vein.hpp"

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

int PalmVein::clearStatusInfo() {
    AuxDev::init_auxinfo_impl(impl_->_info_impl);
    return 1;
}

int PalmVein::loadStatusInfo() {

    //data params
    _statusInfo.a1_palm.success = impl_->_info_impl.a1_palm.success;
    _statusInfo.a1_palm.time_consume = impl_->_info_impl.a1_palm.time_consume;
    _statusInfo.a1_palm.rect = impl_->_info_impl.a1_palm.rect;

    _statusInfo.a2_finger.success = impl_->_info_impl.a2_finger.success;
    _statusInfo.a2_finger.time_consume = impl_->_info_impl.a2_finger.time_consume;
    for (int i = 0; i < 3; i++) {
        _statusInfo.a2_finger.pos[i] = impl_->_info_impl.a2_finger.pos[i];
    }

    _statusInfo.a3_roi.success = impl_->_info_impl.a3_roi.success;
    _statusInfo.a3_roi.time_consume = impl_->_info_impl.a3_roi.time_consume;
    _statusInfo.a3_roi.rect = impl_->_info_impl.a3_roi.rect;
    for (int i = 0; i < 4; i++) {
        _statusInfo.a3_roi.rect_point[i] = impl_->_info_impl.a3_roi.rect_point[i];
    }

    _statusInfo.a4_enhance.success = impl_->_info_impl.a4_enhance.success;
    _statusInfo.a4_enhance.time_consume = impl_->_info_impl.a4_enhance.time_consume;
    impl_->_info_impl.a4_enhance.image.copyTo(_statusInfo.a4_enhance.image);

    _statusInfo.a5_fcompute.success = impl_->_info_impl.a5_fcompute.success;
    _statusInfo.a5_fcompute.time_consume = impl_->_info_impl.a5_fcompute.time_consume;

    //draw image
    cv::Mat imsrc_;
    impl_->_info_impl.image_src.copyTo(imsrc_);
    std::vector<cv::Mat> channels;
    for (int i = 0; i < 3; i++) {
        channels.push_back(imsrc_);
    }
    cv::Mat im = cv::Mat::zeros(imsrc_.rows, imsrc_.cols, CV_8UC3);
    merge(channels, im);

    cv::Scalar sc3(200, 200, 0);
    if (_statusInfo.a3_roi.success) {
        int line_w = 2;
        cv::line(im, _statusInfo.a3_roi.rect_point[0], _statusInfo.a3_roi.rect_point[1], sc3, line_w);
        cv::line(im, _statusInfo.a3_roi.rect_point[1], _statusInfo.a3_roi.rect_point[3], sc3, line_w);
        cv::line(im, _statusInfo.a3_roi.rect_point[3], _statusInfo.a3_roi.rect_point[2], sc3, line_w);
        cv::line(im, _statusInfo.a3_roi.rect_point[2], _statusInfo.a3_roi.rect_point[0], sc3, line_w);
        for (int i = 0; i < 4; i++) {
            circle(im, _statusInfo.a3_roi.rect_point[i], 7, sc3);
        }
    }

    cv::Scalar sc2(0, 0, 255);
    if (_statusInfo.a2_finger.success) {
        for (int i = 0; i < 3; i++) {
            circle(im, _statusInfo.a2_finger.pos[i], 5, sc2, -1);
        }
    }
    im.copyTo(_statusInfo.image_wnd);

    //resize Todo

    impl_->_info_impl.a4_enhance.image.copyTo(_statusInfo.image_roi);
    impl_->_info_impl.image_svm.copyTo(_statusInfo.image_svm);

    return 1;
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

float PalmVein::compareFeature(const float* src, uint32_t src_len, const float* dst, uint32_t dst_len)
{
    assert(src && dst);
    std::vector<float> src_feat(src_len), dst_feat(dst_len);
    for(int i = 0; i < src_len; ++i)
    {
        src_feat[i] = src[i];
    }
    for(int i = 0; i < dst_len; ++i)
    {
        dst_feat[i] = dst[i];
    }
    
    return impl_->compare_feature(src_feat, dst_feat);
}

bool PalmVein::loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path)
{
    return impl_->loadModelFiles(palm_model_path, finger_model_path);
}

PalmVeinTemplate::PalmVeinTemplate(const uint32_t id)
    : impl_(new PalmVeinTemplate_impl())
{

}
PalmVeinTemplate::~PalmVeinTemplate()
{
    delete impl_;
}

void PalmVeinTemplate::feed(const float* feat, uint32_t len)
{
    impl_->feed(feat, len);
}

int PalmVeinTemplate::check(bool &ok)
{
    return impl_->check(ok);
}

bool PalmVeinTemplate::buildTemplate(std::vector<float>& t)
{
    return impl_->build_template(t);
}

} //namespace OVS
