#include "algorithm/include/aux_utils.h"


std::vector<cv::Point> AuxDev::pointAffine(const std::vector<cv::Point>& points, const cv::Mat& trans_mat) {
	double* m = (double*)trans_mat.ptr<double>();
	std::vector<cv::Point> transPoints;
	for (size_t i = 0; i < points.size(); i++)
	{
		cv::Point p = points[i];
		int x = p.x * m[0] + p.y * m[1] + m[2];
		int y = p.x * m[3] + p.y * m[4] + m[5];
		transPoints.push_back(cv::Point(x, y));
	}
	return transPoints;
}


int AuxDev::init_auxinfo_impl(StatusInfo_ImplCpp& auxinfo){
	auxinfo.a1_palm.success = 0;
	auxinfo.a1_palm.time_consume = -1;
	auxinfo.a1_palm.rect.x = 0;
	auxinfo.a1_palm.rect.y = 0;
	auxinfo.a1_palm.rect.width = 0;
	auxinfo.a1_palm.rect.height = 0;

	auxinfo.a2_finger.success = 0;
	auxinfo.a2_finger.time_consume = -1;
	for (int i = 0; i < 3; i++) {
		auxinfo.a2_finger.pos[i].x = 0;
		auxinfo.a2_finger.pos[i].y = 0;
	}

	auxinfo.a3_roi.success = 0;
	auxinfo.a3_roi.time_consume = -1;
	auxinfo.a3_roi.rect.x = 0;
	auxinfo.a3_roi.rect.y = 0;
	auxinfo.a3_roi.rect.width = 0;
	auxinfo.a3_roi.rect.height = 0;
	for (int i = 0; i < 4; i++) {
		auxinfo.a3_roi.rect_point[i].x = 0;
		auxinfo.a3_roi.rect_point[i].y = 0;
	}

	auxinfo.a4_enhance.success = 0;
	auxinfo.a4_enhance.time_consume = -1;
	auxinfo.a4_enhance.image = cv::Mat::zeros(cv::Size(128, 128), CV_8UC1);

	auxinfo.a5_fcompute.success = 0;
	auxinfo.a5_fcompute.time_consume = -1;

	auxinfo.image_src = cv::Mat::zeros(cv::Size(400, 400), CV_8UC1);
	auxinfo.image_svm = cv::Mat::zeros(cv::Size(400, 400), CV_8UC1);
	return 1;
}






