/**
*  OVS Palmparser Functions Declaration
*
*  @author		Xu huabin, Ji renze
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			OVS Palmparser API Declaration
*
**/

/**
*
*  Copyright (c) 2020 Qingdao NovelBeam Technology Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao NovelBeam Technology Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao NovelBeam Technology Co., Ltd.
*
**/
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <list>
#include <opencv.hpp>
#include <core.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

enum EProcessingState
{
	ENUM_OK = 4,
	OVS_SUCCESS = 0,
	OVS_WAITING_DATA = 1,
	OVS_INIT = 2,
	OVS_NOT_READY = 3,
	OVS_ERR_TIME_OUT = -1,
	// 参数无效
	OVS_ERR_INVALID_PARAM = -10,
	// 没有发现手掌
	OVS_ERR_NO_FOUND_PALM = -100,
	// ROI区域获取错误
	OVS_ERR_GET_ROI_FAILED = -101,
	// ROI照片太模糊
	OVS_ERR_ROI_BLURRY = -102,
	// ROI区域亮度太弱
	OVS_ERR_ROI_TOOWEAK = -103,
	// ROI区域亮度太强
	OVS_ERR_ROI_TOOSTRONG = -104,
	// ROI区域位置无效
	OVS_ERR_ROI_INVALID_POSITION = -105,
	// 获取特征失败
	OVS_ERR_GET_FEATURE_FAILED = -106,
	// 匹配失败
	OVS_ERR_MATCHING_FAILED = -107,
	// 匹配超时
	OVS_ERR_MATCHING_TIMEOUT = -108
};

struct SPalmImageInfo
{
public:
	int center_x = 0;
	int center_y = 0;
	double score = 0.0f;
	double mean_illumination = 0.0f;
};

class OVSPalmParser
{
public:
	OVSPalmParser(Mat& in_mat);
	~OVSPalmParser();

	static double compareFeature(const uint8_t* src, const uint8_t* dst, uint32_t len);

	// 解析图像信息
	EProcessingState ParseInfo(SPalmImageInfo& info);
	
	EProcessingState FindRoi(unsigned char* roi_data, uint32_t& width, uint32_t& height, uint32_t& roi_data_len);

	EProcessingState computeFeature(std::vector<uint8_t>& features);

	bool loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path);

private:
	enum EPalmState { PS_UNKNOWN, PS_NO_PALM, PS_NO_FINGER, PS_OK };
	void segment(const cv::Mat& src, cv::Mat& dst, const cv::Size& sz_ = cv::Size(540, 540));

	EProcessingState compute_roi();
	void build_filter_bank();
	void enhance_roi();
	void extract_lbp_image(cv::Mat& lbp);
	void compute_lbp(const cv::Mat& src, cv::Mat& dst); 
	void compute_nbp(const cv::Mat& src, cv::Mat& dst);

	bool extract_fingers(std::list<cv::Point>& pts);
	bool palm_exists(const cv::Mat& src);
	bool find_fingers(const cv::Mat& src, std::list<cv::Point>& pts);

	cv::Mat raw_image_;
	EPalmState palm_state_ = PS_UNKNOWN;
	// cv::Mat mask_;
	cv::Mat roi_;
	cv::Mat roi_enhanced_;
	cv::Point palm_center_;
	std::vector<cv::Mat> gabor_kernels_;
};

class OVSPalmDetector
{
public:
	OVSPalmDetector();
	~OVSPalmDetector();
	static shared_ptr<OVSPalmDetector> getInstance();
	bool loadModel(const std::string& model_path);
	float detect(const cv::Mat& src);
private:
	static shared_ptr<OVSPalmDetector> instance;

	cv::Ptr<cv::ml::SVM> svm_;
	cv::PCA pca_;
	cv::Mat pca_mean_;
};

class OVSFingerDetector
{
public:
	OVSFingerDetector();
	~OVSFingerDetector();
	static shared_ptr<OVSFingerDetector> getInstance();
	bool loadModel(const std::string& mode_path);
	bool detect(const cv::Mat& src, std::vector<cv::Point>& pts);
private:
	
	static shared_ptr<OVSFingerDetector> instance;
	static std::vector<float> get_svm_detector(const cv::Ptr< cv::ml::SVM >& svm);
	std::vector<cv::Point> group_roi(const std::vector<cv::Rect>& roi_list);
	cv::Ptr<cv::ml::SVM> svm_;
};
