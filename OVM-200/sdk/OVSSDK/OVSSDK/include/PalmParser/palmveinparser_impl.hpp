#pragma once

#include <vector>
#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include "utils.hpp"

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
    

    bool loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path);
    
private:
    enum PalmState {PS_UNKNOWN, PS_NO_PALM, PS_NO_FINGER, PS_OK};
    void segment(const cv::Mat& src, cv::Mat& dst, const cv::Size& sz_=cv::Size(540, 540));

    ProcessingState compute_roi();
    void build_filter_bank();
    void enhance_roi();
    void extract_lbp_image(cv::Mat& lbp);
    void compute_lbp(const cv::Mat& src, cv::Mat& dst);
    void compute_nbp(const cv::Mat& src, cv::Mat& dst);

    bool extract_fingers(std::list<cv::Point>& pts);
    bool palm_exists(const cv::Mat& src);
    bool find_fingers(const cv::Mat& src, std::list<cv::Point>& pts);

    cv::Mat raw_image_;
    PalmState palm_state_ = PS_UNKNOWN;
    // cv::Mat mask_;
    cv::Mat roi_;
    cv::Mat roi_enhanced_;
    cv::Point palm_center_;
    std::vector<cv::Mat> gabor_kernels_;
};

class PalmDetector
{
public:
    static PalmDetector* getInstance();
    bool loadModel(const std::string& model_path);
    float detect(const cv::Mat& src);
private:
    static PalmDetector* instance;
    PalmDetector();
    cv::Ptr<cv::ml::SVM> svm_;
    cv::PCA pca_;
    cv::Mat pca_mean_;
};

class FingerDetector
{
public:
    static FingerDetector* getInstance();
    bool loadModel(const std::string& mode_path);
    bool detect(const cv::Mat& src, std::vector<cv::Point>& pts);
private:
    static FingerDetector* instance;
    FingerDetector();
    static std::vector<float> get_svm_detector(const cv::Ptr< cv::ml::SVM >& svm);
    std::vector<cv::Point> group_roi(const std::vector<cv::Rect>& roi_list);
    cv::Ptr<cv::ml::SVM> svm_;
};
}