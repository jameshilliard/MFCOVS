#ifndef _AUX_UTILS_H_
#define _AUX_UTILS_H_


#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace AuxDev
{
    struct onefeature_succ //add by zjq 2021.1.15
    {
        int success = false;
        float time_consume = 0.0;
    };

	struct palm
	{
		int success = false;
		float time_consume = 0.0;
		cv::Rect rect;
	};

	struct finger
	{
		int success = false;
		float time_consume = 0.0;
		cv::Point pos[3];
	};
	
	struct roi
	{
		int success = false;
		float time_consume = 0.0;
		cv::Rect rect;
		cv::Point rect_point[4];
	};

	struct roi_enhance
	{
		int success = false;
		float time_consume = 0.0;
		cv::Mat image;
	};

	struct feature_compare
	{
		int success = false;
		float time_consume = 0.0;
		float value = -1.0;
	};

	struct feature_compute
	{
		int success = false;
		float time_consume = 0.0;
	};

	struct hog_visual
	{
		int success = false;
		float time_consume = 0.0;
		cv::Mat image;
	};

	struct StatusInfo {

		palm a1_palm;
		finger a2_finger;
		roi a3_roi;
		roi_enhance a4_enhance;
		feature_compute a5_fcompute;
        onefeature_succ a6_1fetturesucc;

		cv::Mat image_wnd;
		cv::Mat image_roi;
		cv::Mat image_svm;
        cv::Mat image_raw;
	};

	struct StatusInfo_ImplCpp
	{
		palm a1_palm;
		finger a2_finger;
		roi a3_roi;
		roi_enhance a4_enhance;
		feature_compute a5_fcompute;
		cv::Mat image_src;
		cv::Mat image_svm;
	};

	int init_auxinfo_impl(StatusInfo_ImplCpp& auxinfo);
	std::vector<cv::Point> pointAffine(const std::vector<cv::Point>& points, const cv::Mat& trans_mat);
}


#endif
