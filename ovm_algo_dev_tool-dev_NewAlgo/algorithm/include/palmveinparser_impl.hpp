#pragma once

#include <vector>
#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "utils.hpp"

#include <chrono>
#include "aux_utils.h"  //auxdev file add by sunzk
#define AUXDEVSTATUS   
namespace OVS
{

class PalmVeinParser_impl
{
public:
    explicit PalmVeinParser_impl(const unsigned char* image_data, 
                            const uint32_t width, const uint32_t height, const uint32_t depth=3);
    
    ProcessingState parse_info(PalmImageInfo& info);
    ProcessingState find_roi(unsigned char* roi_data, uint32_t& width, uint32_t& height, uint32_t& roi_data_len);
    ProcessingState compute_feature(std::vector<uint8_t>& features);
    ProcessingState compute_feature(std::vector<float>& features);
    float compare_feature(const std::vector<float>& src, const std::vector<float>& dst);
    int match_features(const std::vector<cv::Point2f>& kps_src, const cv::Mat& desc_src,
                        const std::vector<cv::Point2f>& kps_dst, const cv::Mat& desc_dst);

    bool loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path);
    AuxDev::StatusInfo_ImplCpp _info_impl;

private:
    enum PalmState {PS_UNKNOWN, PS_NO_PALM, PS_NO_FINGER, PS_OK};
    void segment(const cv::Mat& src, cv::Mat& dst, const cv::Size& sz_=cv::Size(0, 0));

    ProcessingState compute_roi();
    void build_filter_bank();
    void enhance_roi();
    void extract_lbp_image(cv::Mat& lbp);
    void compute_lbp(const cv::Mat& src, cv::Mat& dst);
    void compute_nbp(const cv::Mat& src, cv::Mat& dst);

    bool extract_fingers(std::list<cv::Point>& pts);
    bool palm_exists(const cv::Mat& src, cv::Rect& finger_roots);
    bool find_fingers(const cv::Mat& src, const cv::Rect& roi, std::vector<cv::Point2f>& pts2f);

    void compute_vein_features(std::vector<cv::Point2f>& kps, cv::Mat& descriptors);
    void convert_vein_features(const std::vector<cv::Point2f>& kps, const cv::Mat& descriptors, std::vector<float>& features);

    cv::Mat raw_image_;
    PalmState palm_state_ = PS_UNKNOWN;
    cv::Mat roi_;
    cv::Mat roi_enhanced_;
    cv::Point2f palm_center_;
    std::vector<cv::Mat> gabor_kernels_;
};

class PalmDetector
{
public:
    static PalmDetector* getInstance();
    bool loadModel(const std::string& model_path);
    float detect(const cv::Mat& src, cv::Rect& finger_roots);
private:
    static PalmDetector* instance;
    PalmDetector();
    cv::Ptr<cv::HOGDescriptor> hog_;
    // cv::Ptr<cv::ml::SVM> svm_;
    // cv::PCA pca_;
    // cv::Mat pca_mean_;

};

class FingerDetector
{
public:
    static FingerDetector* getInstance();
    bool loadModel(const std::string& mode_path);
    bool detect(const cv::Mat& src, const cv::Rect& roi, std::vector<cv::Point2f>& pts2f);

private:
    static FingerDetector* instance;
    FingerDetector();
    static std::vector<float> get_svm_detector(const cv::Ptr< cv::ml::SVM >& svm);

    std::vector<cv::Point2f> group_roi_2f(const std::vector<cv::Point>& pts);
    std::vector<cv::Point> group_roi(const std::vector<cv::Point>& pts);   
    std::vector<cv::Point> group_roi(const std::vector<cv::Rect>& roi_list);

    // cv::Ptr<cv::ml::SVM> svm_;
    cv::Ptr<cv::HOGDescriptor> hog_;
public:
    cv::Mat aux_im_svm_;
};
}