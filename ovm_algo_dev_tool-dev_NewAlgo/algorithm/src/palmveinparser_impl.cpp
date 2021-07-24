/**
 * File：
 * 使用版本：2021-02-08
 *
 * 已修改：palm坐标点 --坐标变换[Y] --输出float[Y]
 * 
 * 已添加：aux_utils算法可视化[Y]
 * 
 * 已修改：FingerSVM速度提升 [Y]
 *        指缝坐标精度修改   [Y]
 * 
 **/

#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "algorithm/include/utils.hpp"
#include "algorithm/include/palmveinparser_impl.hpp"

using namespace std;
using namespace cv;

namespace OVS
{

PalmVeinParser_impl::PalmVeinParser_impl(const unsigned char* image_data, 
                            const uint32_t width, const uint32_t height, const uint32_t depth)
{
#ifdef AUXDEVSTATUS
	AuxDev::init_auxinfo_impl(_info_impl);
#endif
	assert(width > 0 && height > 0 && (depth == 1 || depth == 3 || depth == 4));
	int type_ = CV_8UC3;
	if (depth == 1)
	{
		type_ = CV_8UC1;
	}
	else if (depth == 4)
	{
		type_ = CV_8UC4;
	}

	Mat gray;
	{
        //转为灰度图
		const Mat data_(Size(width, height), type_, const_cast<unsigned char*>(image_data));
		if (type_ == CV_8UC1)
		{
			data_.copyTo(gray);
		}
		else if (type_ == CV_8UC3)
		{
			cvtColor(data_, gray, COLOR_RGB2GRAY);
		}
		else
		{
			cvtColor(data_, gray, COLOR_RGBA2GRAY);
		}
	}

	segment(gray, raw_image_);
}

//切割掉图像多余边缘，只保留长宽相等的中心部分。然后旋转90度
void PalmVeinParser_impl::segment(const Mat& src, Mat& dst, const Size& sz_)
{
	assert(src.data);
	const int height = src.rows, width = src.cols;
	assert(width >= sz_.width && height >= sz_.height);

	Rect roi;
	if (height > width)
	{
		roi.x = 0;
		roi.y = (height >> 1) - (width >> 1);
		roi.width = width;
		roi.height = width;
	}
	else
	{
		roi.x = (width >> 1) - (height >> 1);
		roi.y = 0;
		roi.width = height;
		roi.height = height;
	}

	if (sz_.width == 0)
		src(roi).copyTo(dst);
	else
		resize(src(roi), dst, sz_, 0, 0, INTER_CUBIC);
	Mat rotationMatrix = getRotationMatrix2D(Point(dst.cols / 2, dst.rows / 2), -90.0, 1);
	warpAffine(dst, dst, rotationMatrix, dst.size());
#ifdef AUXDEVSTATUS
	dst.copyTo(_info_impl.image_src);
#endif

}

ProcessingState PalmVeinParser_impl::parse_info(PalmImageInfo& info)
{
	if (!roi_.data)
	{
		ProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}

	auto mean_illumination = mean(roi_);
	Mat lap;
	Laplacian(roi_, lap, CV_64F);
	Scalar mean, std_dev;
	meanStdDev(lap, mean, std_dev);

	info.center_x = palm_center_.x;
	info.center_y = palm_center_.y;
	info.score = std_dev(0);
	info.mean_illumination = mean_illumination(0);

	return OVS_SUCCESS;
}

ProcessingState PalmVeinParser_impl::find_roi(unsigned char* roi_data, 
                    uint32_t& roi_width, uint32_t& roi_height, uint32_t& roi_data_len)
{
	if (!roi_.data)
	{
		ProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}

	roi_width = roi_.cols;
	roi_height = roi_.rows;
	roi_data_len = roi_.cols * roi_.rows;
	if (roi_data)
	{
		memcpy(roi_data, roi_.data, roi_data_len);
	}

	return OVS_SUCCESS;
}

//检测指缝候选点
bool PalmVeinParser_impl::find_fingers(const cv::Mat& src, const cv::Rect& roi, std::vector<cv::Point2f>& pts2f)
{
	auto detector = FingerDetector::getInstance();
	bool ok;
	ok = detector->detect(src, roi, pts2f);

	if (!ok)
	{
		return false;
	}

#ifdef AUXDEVSTATUS
	detector->aux_im_svm_.copyTo(_info_impl.image_svm);
#endif

	return true;
}

//检测图像中是否存在手掌，如存在则尝试提取指缝位置，并检查指缝位置是否合理
bool PalmVeinParser_impl::extract_fingers(std::list<cv::Point>& finger_pts)
{
	std::list<cv::Point2f> list_pts2f;
	std::vector<cv::Point2f> vect_pts2f;

#ifdef AUXDEVSTATUS
	float time_fin = 0.0;
	float time_palm = 0.0;
#endif
	if (palm_state_ == PS_UNKNOWN)
	{
#ifdef AUXDEVSTATUS
		auto start_palm = std::chrono::system_clock::now();
#endif
		//是否存在手掌
		Rect roi;
		Mat norm;
		resize(raw_image_, norm, Size(256, 256), cv::INTER_CUBIC);
		bool ok = palm_exists(norm, roi);
#ifdef AUXDEVSTATUS
		if (ok) {
			auto end_palm = std::chrono::system_clock::now();
			auto duration_palm = std::chrono::duration_cast<std::chrono::microseconds> (end_palm - start_palm);
			time_palm += 1000 * float(duration_palm.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;;
			_info_impl.a1_palm.success = true;
			_info_impl.a1_palm.time_consume += time_palm;
			_info_impl.a1_palm.rect.x = int(roi.x * 1080.0 / 256.0);
			_info_impl.a1_palm.rect.y = int(roi.y * 1080.0 / 256.0);
			_info_impl.a1_palm.rect.width = int(roi.width * 1080.0 / 256.0);
			_info_impl.a1_palm.rect.height = int(roi.height * 1080.0 / 256.0);
		}
#endif
		if (!ok)
		{
			palm_state_ = PS_NO_PALM;
			return false;
		}

#ifdef AUXDEVSTATUS
		auto start_fin1 = std::chrono::system_clock::now();
#endif
		//检测指缝
		ok = find_fingers(norm, roi, vect_pts2f);

#ifdef AUXDEVSTATUS
		auto end_fin1 = std::chrono::system_clock::now();
		auto duration_fin1 = std::chrono::duration_cast<std::chrono::microseconds> (end_fin1 - start_fin1);
		time_fin += 1000 * float(duration_fin1.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
#endif
		if (!ok)
		{
			palm_state_ = PS_NO_FINGER;
			return false;
		}
	}
	else if (palm_state_ != PS_OK)
	{
		return false;
	}

#ifdef AUXDEVSTATUS
	auto start_fin2 = std::chrono::system_clock::now();
#endif
	if (vect_pts2f.size() > 0) {
		copy(vect_pts2f.begin(), vect_pts2f.end(), back_inserter(list_pts2f));
	}
	if (list_pts2f.size() != 3 && list_pts2f.size() != 4)
	{
		palm_state_ = PS_NO_FINGER;
		return false;
	}

	//去除多余的指缝，一般是拇指指缝
	if (list_pts2f.size() == 4)
	{
		auto mit = list_pts2f.begin();
		float max_y = mit->y;
		for (auto it = mit; it != list_pts2f.end(); ++it)
		{
			if (it->y > max_y)
			{
				max_y = it->y;
				mit = it;
			}
		}
		list_pts2f.erase(mit);
	}

	list_pts2f.sort([](const Point& a, const Point& b) { return a.x < b.x; });

	//check if finger positions are valid
	{
		auto d2 = [](const Point& a, const Point& b) { return (a - b).ddot(a - b); };
		auto it = list_pts2f.begin();
		Point p1 = *(it++);
		Point p2 = *(it++);
		Point p3 = *(it++);
		double d12 = d2(p1, p2), d23 = d2(p2, p3), d13 = d2(p1, p3);
		double angle = 180.0 / CV_PI * acos(0.5 * (d12 + d23 - d13) / (sqrt(d12 * d23)));
		if (fabs(angle) < 130.0) //TODO hard-coded threshold
		{
			palm_state_ = PS_NO_FINGER;
			return false;
		}
	}

	palm_state_ = PS_OK;
	double ratio = raw_image_.cols / 256.0;
	std::list<cv::Point> list_pts;
	for (auto& pt : list_pts2f)
	{
		int x_ = (int)round(pt.x * ratio);
		int y_ = (int)round(pt.y * ratio);
		list_pts.push_back(Point(x_, y_));
	}
	finger_pts.assign(list_pts.begin(), list_pts.end());
#ifdef AUXDEVSTATUS
	auto end_fin2 = std::chrono::system_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::microseconds> (end_fin2 - start_fin2);
	time_fin += 1000 * float(duration2.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	_info_impl.a2_finger.success = true;
	_info_impl.a2_finger.time_consume += time_fin;

	std::vector<cv::Point> pts_;
	for (auto& pt : finger_pts) {
		pts_.push_back(pt);
	}
	for (int i_ = 0; i_ < 3; i_++) {
		_info_impl.a2_finger.pos[i_].x = pts_[i_].x;
		_info_impl.a2_finger.pos[i_].y = pts_[i_].y;
	}
#endif

    return true;
}

//根据指缝位置确定 ROI 区域坐标
ProcessingState PalmVeinParser_impl::compute_roi()
{
	list<Point> finger_pts;
	bool ok = extract_fingers(finger_pts);
	if (!ok)
	{
		switch (palm_state_)
		{
		case PS_NO_PALM:
			return OVS_ERR_NO_FOUND_PALM;
		case PS_NO_FINGER:
			return OVS_ERR_NO_FOUND_FINGER;
		default:
			return OVS_ERR_NO_FOUND_PALM;
		}
	}

#ifdef AUXDEVSTATUS
	auto start_roi = std::chrono::system_clock::now();
#endif
	//计算最左和最右指缝的距离和角度
	double d = finger_pts.back().x - finger_pts.front().x;
	double xx = d, yy = finger_pts.back().y - finger_pts.front().y;
	Point ctr = (finger_pts.back() + finger_pts.front()) / 2;
	double angle = atan2(yy, xx) * 180.0 / CV_PI;
	Mat rotationMatrix = getRotationMatrix2D(ctr, angle, 1);

	//归一化角度
	Mat warped;
	warpAffine(raw_image_, warped, rotationMatrix, raw_image_.size());

	const double r = 0.2;
	const double hw_ratio = 1.2;
	if (ctr.x - d / 2 < 0 || ctr.x + d / 2 >= warped.cols ||
		ctr.y + r * d + hw_ratio * d >= warped.rows)
	{
		return OVS_ERR_GET_ROI_FAILED;
	}

	float scale = 100;
	double len = (r + hw_ratio / 2) * d;
	palm_center_.x = ctr.x - d * sin(angle * CV_PI / 180.0);
	palm_center_.y = ctr.y + d * cos(angle * CV_PI / 180.0);
	palm_center_.x = (1080.0 - palm_center_.x) / 1080.0 * scale;
	palm_center_.y = palm_center_.y / 1080.0 * scale;

	resize(warped(Rect(ctr.x - d / 2, ctr.y + r * d, d, hw_ratio * d)), roi_, Size(128, 128), INTER_LINEAR);

#ifdef AUXDEVSTATUS
	auto end_roi = std::chrono::system_clock::now();
	auto duration_roi = std::chrono::duration_cast<std::chrono::microseconds> (end_roi - start_roi);
	float runtime_ = 1000 * float(duration_roi.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	_info_impl.a3_roi.success = true;
	_info_impl.a3_roi.time_consume = runtime_;

	//box区域   
	int x_ = ctr.x - d / 2;
	int y_ = ctr.y + r * d;
	int w_ = d;
	int h_ = 1.6 * d;
	Point pt_upL(finger_pts.front().x, finger_pts.front().y);
	Point pt_upR(finger_pts.back().x, finger_pts.back().y);
	Point pt_dwL(x_, y_ + h_);
	Point pt_dwR(x_ + w_, y_ + h_);
	std::vector<cv::Point> pts_;
	pts_.push_back(pt_dwL);
	pts_.push_back(pt_dwR);
	pts_.push_back(Point(x_, y_));
	pts_.push_back(Point(x_ + d, y_));
	Mat M_ = getRotationMatrix2D(ctr, -1.0 * angle, 1);
	std::vector<cv::Point> pts_affine_ = AuxDev::pointAffine(pts_, M_);
	//_info_impl.a3_roi.rect_point[0] = pt_upL;
	//_info_impl.a3_roi.rect_point[1] = pt_upR;    
	_info_impl.a3_roi.rect_point[0] = pts_affine_[2];
	_info_impl.a3_roi.rect_point[1] = pts_affine_[3];
	_info_impl.a3_roi.rect_point[2] = pts_affine_[0];
	_info_impl.a3_roi.rect_point[3] = pts_affine_[1];

	_info_impl.a3_roi.rect.x = ctr.x - d / 2;
	_info_impl.a3_roi.rect.y = ctr.y + r * d;
	_info_impl.a3_roi.rect.width = d;
	_info_impl.a3_roi.rect.height = hw_ratio * d;
#endif

	return OVS_SUCCESS;
}

void PalmVeinParser_impl::build_filter_bank()
{
	gabor_kernels_.clear();
	const int ksize = 31;
	const double sigma = 3.3, lambd = 18.3, gamma = 4.5, psi = 0.89;

	for (int i = 0; i < 8; ++i)
	{
		double theta = i * CV_PI / 8;
		Mat kernel = getGaborKernel(Size(ksize, ksize),
			sigma, theta, lambd, gamma, psi, CV_32F);

		auto sum = cv::sum(kernel)(0);
		gabor_kernels_.push_back(kernel / (1.5 * sum));
	}
}

void PalmVeinParser_impl::enhance_roi()
{
    Mat dog;
    Mat base;
    roi_.convertTo(base, CV_64FC1);
    Mat temp1, temp2;
    const double weight = 4.8, scale = 1.4; //TODO: hard-coded parameter
    GaussianBlur(base, temp1, Size(11, 11), weight);
    GaussianBlur(base, temp2, Size(11, 11), weight*scale);
    double w = 1.0 / (pow(weight, 2.0) * (scale - 1.0));
    dog = (temp1 - temp2) * w;
    normalize(dog, dog, 0, 255, NORM_MINMAX);
    convertScaleAbs(dog, roi_enhanced_);
}

void PalmVeinParser_impl::extract_lbp_image(Mat& lbp)
{
	//下采样
	Mat downsampled = Mat::zeros(roi_enhanced_.rows / 8, roi_enhanced_.cols / 8, CV_8UC1);
	resize(roi_enhanced_, downsampled, Size(roi_enhanced_.cols / 8, roi_enhanced_.rows / 8),
		0.0, 0.0, INTER_AREA);

	//平均滤波
	Mat filtered;
	boxFilter(downsampled, lbp, CV_8UC1, Size(3, 3));

}

void PalmVeinParser_impl::compute_lbp(const Mat& src, Mat& dst)
{
	Mat padding = Mat::zeros(Size(src.cols + 2, src.rows + 2), src.depth());
	src.copyTo(padding(Rect(1, 1, src.cols, src.rows)));

	dst = Mat::zeros(src.size(), CV_8UC1);
	for (int i = 1; i < src.rows + 1; ++i)
	{
		const uchar* ptr = padding.ptr<uchar>(i);
		const uchar* prev = padding.ptr<uchar>(i - 1);
		const uchar* next = padding.ptr<uchar>(i + 1);
		uchar* lbp = dst.ptr<uchar>(i - 1);
		for (int j = 1; j < src.cols + 1; ++j)
		{
			const uchar v = ptr[j];
			uchar b = (((prev[j - 1] >= v) << 7) |
				((prev[j] >= v) << 6) |
				((prev[j + 1] >= v) << 5) |
				((ptr[j + 1] >= v) << 4) |
				((next[j + 1] >= v) << 3) |
				((next[j] >= v) << 2) |
				((next[j - 1] >= v) << 1) |
				(ptr[j - 1] >= v)) & 0xFF;
			lbp[j - 1] = b;
		}
	}
}

void PalmVeinParser_impl::compute_nbp(const Mat& src, Mat& dst)
{
	Mat padding = Mat::zeros(Size(src.cols + 2, src.rows + 2), src.depth());
	src.copyTo(padding(Rect(1, 1, src.cols, src.rows)));

	dst = Mat::zeros(src.size(), CV_8UC1);
	for (int i = 1; i < src.rows + 1; ++i)
	{
		const uchar* ptr = padding.ptr<uchar>(i);
		const uchar* prev = padding.ptr<uchar>(i - 1);
		const uchar* next = padding.ptr<uchar>(i + 1);
		uchar* nbp = dst.ptr<uchar>(i - 1);
		for (int j = 1; j < src.cols + 1; ++j)
		{
			uchar b = (((prev[j - 1] > prev[j]) << 7) |
				((prev[j] > prev[j + 1]) << 6) |
				((prev[j + 1] > ptr[j + 1]) << 5) |
				((ptr[j + 1] > next[j + 1]) << 4) |
				((next[j + 1] > next[j]) << 3) |
				((next[j] > next[j - 1]) << 2) |
				((next[j - 1] >= ptr[j - 1]) << 1) |
				(ptr[j - 1] >= ptr[j])) & 0xFF;
			nbp[j - 1] = b;
		}
	}
}

//用 nbp 计算手指特征，得到一组binary向量
ProcessingState PalmVeinParser_impl::compute_feature(vector<uint8_t>& features)
{
	if (!roi_.data)
	{
		ProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}

	build_filter_bank();
	enhance_roi();

	Mat lbp;
	extract_lbp_image(lbp);
	Mat nbp;
	compute_nbp(lbp, nbp);

	features.clear();
	features.resize(nbp.rows * nbp.cols);
	for (int i = 0; i < nbp.rows; ++i)
	{
		const uint8_t* ptr = nbp.ptr<uint8_t>(i);
		for (int j = 0; j < nbp.cols; ++j)
		{
			features[i * nbp.cols + j] = ptr[j];
		}
	}

    return OVS_SUCCESS;
}

//用HOG计算特征，得到一组 float point 向量
ProcessingState PalmVeinParser_impl::compute_feature(vector<float>& features)
{
	if (!roi_.data)
	{
		ProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}
#ifdef AUXDEVSTATUS
	auto start_enhance = std::chrono::system_clock::now();
#endif
	enhance_roi();
#ifdef AUXDEVSTATUS
	auto end_enhance = std::chrono::system_clock::now();
	auto duration_enhance = std::chrono::duration_cast<std::chrono::microseconds> (end_enhance - start_enhance);
	float runtime_ = 1000 * float(duration_enhance.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	_info_impl.a4_enhance.success = true;
	_info_impl.a4_enhance.time_consume = runtime_;
	roi_enhanced_.copyTo(_info_impl.a4_enhance.image);
#endif

#ifdef AUXDEVSTATUS
	auto start_computhog = std::chrono::system_clock::now();
#endif
	vector<Point2f> kps;
	Mat desc;
	compute_vein_features(kps, desc);
	convert_vein_features(kps, desc, features);

#ifdef AUXDEVSTATUS
	//添加 02-08
	for (auto pt : kps) {
		circle(_info_impl.a4_enhance.image, pt, 3, Scalar::all(200), -1);
	}
#endif

#ifdef AUXDEVSTATUS
	auto end_computhog = std::chrono::system_clock::now();
	auto duration_computhog = std::chrono::duration_cast<std::chrono::microseconds> (end_computhog - start_computhog);
	float runtime_computhog = 1000 * float(duration_computhog.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	_info_impl.a5_fcompute.time_consume = runtime_computhog;
	_info_impl.a5_fcompute.success = true;
#endif
	return OVS_SUCCESS;
}

bool PalmVeinParser_impl::palm_exists(const Mat& src, Rect& finger_roots)
{
    Mat im;
    im = src;

    auto detector = PalmDetector::getInstance();
    Rect rect;
    float value = detector->detect(im, rect);
    finger_roots = rect;

    return (value < 0);
}

bool PalmVeinParser_impl::loadModelFiles(const string& palm_model_path, const string& finger_model_path)
{
    auto palm_detector = PalmDetector::getInstance();
    if(!palm_detector->loadModel(palm_model_path))
    {
        return false;
    }

    auto finger_detector = FingerDetector::getInstance();
    if(!finger_detector->loadModel(finger_model_path))
    {
        return false;
    }

    return true;
}

void PalmVeinParser_impl::compute_vein_features(std::vector<cv::Point2f>& kps, cv::Mat& descriptors)
{
    auto sift = SIFT::create(50, 3);
    vector<KeyPoint> key_points;
    sift->detect(roi_enhanced_, key_points);
    sift->compute(roi_enhanced_, key_points, descriptors);

    for (auto kp : key_points)
    {
        kps.push_back(kp.pt);
    }
}

void PalmVeinParser_impl::convert_vein_features(const std::vector<cv::Point2f>& kps, const cv::Mat& descriptors, std::vector<float>& features)
{
    features.clear();
    features.reserve(kps.size() * (2 + descriptors.cols));
    for(size_t i = 0; i < kps.size(); ++i)
    {
        features.push_back(kps[i].x);
        features.push_back(kps[i].y);
        auto ptr = descriptors.ptr<float>(i);
        for(int j = 0; j < descriptors.cols; ++j)
        {
            features.push_back(ptr[j]);
        }
    }
}

float PalmVeinParser_impl::compare_feature(const std::vector<float>& src, const std::vector<float>& dst)
{
	const int sift_len = 128;
	const int feature_len = 2 + sift_len;
	const int num_src = src.size() / feature_len;
	const int num_dst = dst.size() / feature_len;
	vector<Point2f> kps_src(num_src), kps_dst(num_dst);
	Mat desc_src(num_src, 128, CV_32FC1), desc_dst(num_dst, 128, CV_32FC1);
	int idx_src = 0, idx_dst = 0;
	for (int i = 0; i < num_src; ++i)
	{
		Point2f& pt = kps_src[i];
		pt.x = src[idx_src++];
		pt.y = src[idx_src++];
		auto ptr = desc_src.ptr<float>(i);
		for (int j = 0; j < sift_len; ++j)
		{
			ptr[j] = src[idx_src++];
		}

	}

	for (int i = 0; i < num_dst; ++i)
	{
		Point2f& pt = kps_dst[i];
		pt.x = dst[idx_dst++];
		pt.y = dst[idx_dst++];
		auto ptr = desc_dst.ptr<float>(i);
		for (int j = 0; j < sift_len; ++j)
		{
			ptr[j] = dst[idx_dst++];
		}
	}

	return match_features(kps_src, desc_src, kps_dst, desc_dst);
}

int PalmVeinParser_impl::match_features(const std::vector<cv::Point2f>& kps_src, const cv::Mat& desc_src,
                        const std::vector<cv::Point2f>& kps_dst, const cv::Mat& desc_dst)
{
	const float sift_match_thresh = 300;
	const float inlier_thresh = 3.0;
	auto matcher = BFMatcher::create(4, true);
	vector<DMatch> result;
	matcher->match(desc_src, desc_dst, result);

	if (result.size() <= 4)
		return 0;

	vector<Point2f> pts_src, pts_dst;
	for (auto m : result)
	{
		if (m.distance > sift_match_thresh)
			continue;
		pts_src.push_back(kps_src[m.queryIdx]);
		pts_dst.push_back(kps_dst[m.trainIdx]);
	}
	if (pts_src.size() <= 4)
		return 0;

	vector<Point3f> homo_pts0, est_homo_pts1;
	vector<Point2f> est_pts1;
	Mat H = findHomography(pts_src, pts_dst, RANSAC, inlier_thresh);
	int inlier_num = 0;
	if (!H.empty())
	{
		convertPointsToHomogeneous(pts_src, homo_pts0);
		for (auto pt : homo_pts0)
		{
			Mat v(Size(1, 3), CV_64FC1);
			v.at<double>(0) = pt.x;
			v.at<double>(1) = pt.y;
			v.at<double>(2) = pt.z;
			Mat h = H * v;
			Point3f hp;
			hp.x = h.at<double>(0);
			hp.y = h.at<double>(1);
			hp.z = h.at<double>(2);
			est_homo_pts1.push_back(hp);
		}
		convertPointsFromHomogeneous(est_homo_pts1, est_pts1);

		for (size_t i = 0; i < pts_src.size(); ++i)
		{
			Point2f diff = pts_dst[i] - est_pts1[i];
			double v = sqrt(diff.ddot(diff));
			if (v < inlier_thresh)
			{
				inlier_num++;
			}
		}
	}

	return inlier_num;
}

/**
 * PalmDetector
 * 手掌检测器的输入为128x128的图片
 * 首先计算 HOG 特性，然后归一化
 * 把归一化的特征序例做一次PCA，提取前20维数据
 * 将20维数据投入训练好的SVM检测器
 */

PalmDetector* PalmDetector::instance = nullptr;

PalmDetector* PalmDetector::getInstance()
{
    if(!instance)
    {
        instance = new PalmDetector();
    }

    return instance;
}

bool PalmDetector::loadModel(const string& model_path)
{
    if(hog_.get())
        return true;

    hog_.reset(new HOGDescriptor());
    return hog_->load(model_path);
}

PalmDetector::PalmDetector()
{
    //
}

float PalmDetector::detect(const Mat& src, cv::Rect& finger_roots)
{
	assert(hog_.get());
	vector<Rect> foundLocations;
	vector<double> foundWeights;
	hog_->detectMultiScale(src, foundLocations, foundWeights, 0.0, Size(4, 4), Size(0, 0), 1.1);
	//TODO 
	int idx = 0;
	double max_weight = -1;
	for (size_t i = 0; i < foundLocations.size(); ++i)
	{
		if (foundWeights[i] > max_weight)
		{
			max_weight = foundWeights[i];
			idx = i;
		}
	}
	if (max_weight > 0)
	{
		finger_roots = foundLocations[idx];
		return -1;
	}
	else
	{
		return 1;
	}
	return foundLocations.size() == 1 ? -1 : 1;
}

/**
 * FingerDetector
 * 指缝检测器使用 32x32输入的HOG描述子和SVM回归器来检测指缝位置。
 * 一般会出现多个检测框对应一个指缝的情况，这时再做一个分组合并。
 */

FingerDetector* FingerDetector::instance = nullptr;

FingerDetector::FingerDetector()
{

}

FingerDetector* FingerDetector::getInstance()
{
    if(!instance)
    {
        instance = new FingerDetector();
    }

    return instance;
}

vector< float > FingerDetector::get_svm_detector( const Ptr< ml::SVM >& svm )
{
	// get the support vectors
	Mat sv = svm->getSupportVectors();
	const int sv_total = sv.rows;
	// get the decision function
	Mat alpha, svidx;
	double rho = svm->getDecisionFunction(0, alpha, svidx);
	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	vector< float > hog_detector(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));
	hog_detector[sv.cols] = (float)-rho;
	return hog_detector;
}

bool FingerDetector::loadModel(const string& model_path)
{
	if (hog_.get())
		return true;

	hog_.reset(new HOGDescriptor());
	return hog_->load(model_path);
}

bool FingerDetector::detect(const cv::Mat& src, const cv::Rect& roi, std::vector<cv::Point2f>& pts2f)
{
	double ratio = 128.0 / roi.width;
	int im_h = (int)round(ratio * src.rows);
	int im_w = (int)round(ratio * src.cols);
	Rect res_roi;
	res_roi.x = roi.x * ratio;
	res_roi.y = roi.y * ratio;
	res_roi.width = 128;
	res_roi.height = 64;

	//svm detect region - palm svm output rate 2:1
	int unit = res_roi.width / 4;
	int x = res_roi.x - unit;
	int y = res_roi.y - unit * 0.5;
	int w = 6 * unit;
	int h = 3.5 * unit;

	//palm region
	Rect res_rect_palm = res_roi;
	int x1 = x; int y1 = y;
	int x2 = x + w - 1; int y2 = y + h - 1;
	x1 = (x1 < 0) ? 0 : x1;
	y1 = (y1 < 0) ? 0 : y1;
	x2 = (x2 > im_w - 1) ? (im_w - 1) : x2;
	y2 = (y2 > im_h - 1) ? (im_h - 1) : y2;
	cv::Rect res_rect_overlap;
	res_rect_overlap.x = x1;
	res_rect_overlap.y = y1;
	res_rect_overlap.width = std::abs(x1 - x2) + 1;
	res_rect_overlap.height = std::abs(y1 - y2) + 1;

	Rect res_relative_rect_palm;
	res_relative_rect_palm.x = res_rect_palm.x - res_rect_overlap.x;
	res_relative_rect_palm.y = res_rect_palm.y - res_rect_overlap.y;
	if (res_relative_rect_palm.x < 0 or res_relative_rect_palm.y < 0) {
		return false;
	}
	int x2_ = res_relative_rect_palm.x + res_rect_palm.width - 1;
	x2_ = (x2_ > res_rect_overlap.width - 1) ? (res_rect_overlap.width - 1) : x2_;
	int y2_ = res_relative_rect_palm.y + res_rect_palm.height - 1;
	y2_ = (y2_ > res_rect_overlap.height - 1) ? (res_rect_overlap.height - 1) : y2_;
	res_relative_rect_palm.width = std::abs(x2_ - res_relative_rect_palm.x) + 1;
	res_relative_rect_palm.height = std::abs(y2_ - res_relative_rect_palm.y) + 1;

	//resize region
	int w_dst = 192;    //206
	int h_dst = 112;    //120
	Mat im;
	resize(src, im, Size(), ratio, ratio);
	cv::Mat res_detect_region = cv::Mat::zeros(cv::Size(w, h), CV_8UC1);
	im(res_rect_overlap).copyTo(res_detect_region(cv::Rect(0, 0, res_rect_overlap.width, res_rect_overlap.height)));

	//detect finger
	vector<Point> foundLocations;
	vector<double> foundWeights;
	hog_->detect(res_detect_region, foundLocations, foundWeights, 0.0, Size(2, 2), Size(0, 0));

	vector<Rect> roi_list;
	vector<Point> pt_list;
	for (size_t i = 0; i < foundLocations.size(); ++i)
	{
		Point pt(foundLocations[i].x + 16, foundLocations[i].y + 16);
		if (!pt.inside(res_relative_rect_palm))
			continue;
		// x'= det_x/scale+x, y'=det_y/scale+y 
		int xv = pt.x - res_relative_rect_palm.x + res_rect_palm.x;
		int yv = pt.y - res_relative_rect_palm.y + res_rect_palm.y;
		pt_list.push_back(Point(xv, yv));
		// if(foundWeights[i] < 0.1) //TODO hard-coded threshold
		//     continue;
		Rect roi(xv - 16, yv - 16, 32, 32);
		roi_list.push_back(roi);
	}

	vector<Point2f> points2f = group_roi_2f(pt_list);
	for (int i = 0; i < points2f.size(); i++) {
		float xf = points2f[i].x / ratio;
		float yf = points2f[i].y / ratio;
		pts2f.push_back(Point2f(xf, yf));
	}

#ifdef AUXDEVSTATUS
	Mat imaux_svm_;
	im.copyTo(imaux_svm_);
	rectangle(imaux_svm_, res_roi, Scalar::all(127));
	for (auto rc : roi_list)
	{
		rectangle(imaux_svm_, rc, Scalar::all(127));
	}

	for (auto pt : points2f)
	{
		int xi_ = (int)round(pt.x);
		int yi_ = (int)round(pt.y);
		circle(imaux_svm_, Point(xi_, yi_), 3, Scalar::all(255), -1);
	}
	resize(imaux_svm_, imaux_svm_, Size(400, 400));
	imaux_svm_.copyTo(aux_im_svm_);
#endif

	return pts2f.size() > 0;

}


std::vector<cv::Point2f> FingerDetector::group_roi_2f(const std::vector<cv::Point>& pts)
{
	vector<Point2f> data;
	for (auto s : pts)
	{
		data.push_back(Point2f(s.x, s.y));
	}
	Mat labels, ctrs;
	kmeans(data, 3, labels,
		TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 0.1),
		3, KMEANS_PP_CENTERS, ctrs);

	vector<Point2f> centers_2f;
	for (int i = 0; i < ctrs.rows; ++i)
	{
		Point ctr;
		ctr.x = ctrs.at<float>(i, 0);
		ctr.y = ctrs.at<float>(i, 1);
		bool close = false;
		for (auto& pt : centers_2f)
		{
			double d = sqrt((pt.x - ctr.x) * (pt.x - ctr.x) + (pt.y - ctr.y) * (pt.y - ctr.y));
			if (d < 20) //TODO hard-coded threshold
			{
				pt.x = (ctr.x + pt.x) / 2.0;
				pt.y = (ctr.y + pt.y) / 2.0;
				close = true;
				break;
			}
		}
		if (!close)
			centers_2f.push_back(Point2f(ctr.x, ctr.y));
	}

	return centers_2f;
}

std::vector<cv::Point> FingerDetector::group_roi(const std::vector<cv::Point>& pts)
{
	vector<Point2f> data;
	for (auto s : pts)
	{
		data.push_back(Point2f(s.x, s.y));
	}
	Mat labels, ctrs;
	kmeans(data, 3, labels,
		TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 0.1),
		3, KMEANS_PP_CENTERS, ctrs);

	vector<Point> centers;
	for (int i = 0; i < ctrs.rows; ++i)
	{
		Point ctr;
		ctr.x = ctrs.at<float>(i, 0);
		ctr.y = ctrs.at<float>(i, 1);
		bool close = false;
		for (auto& pt : centers)
		{
			double d = sqrt((pt.x - ctr.x) * (pt.x - ctr.x) + (pt.y - ctr.y) * (pt.y - ctr.y));
			if (d < 20) //TODO hard-coded threshold
			{
				pt.x = (ctr.x + pt.x) / 2.0;
				pt.y = (ctr.y + pt.y) / 2.0;
				close = true;
				break;
			}
		}
		if (!close)
			centers.push_back(ctr);
	}


	return centers;
}

vector<cv::Point> FingerDetector::group_roi(const vector<cv::Rect>& roi_list)
{
	list<vector<Rect>> groups;
	for (const auto& roi : roi_list)
	{
		bool ok = false;
		int a_roi = roi.area();
		for (auto& group : groups)
		{
			for (const auto& rect : group)
			{
				if ((roi & rect).area() > 0) //two rects are overlapping
				{
					double a = (roi & rect).area();
					if (a / a_roi > 0.5) //TODO hard-coded threshold
					{
						group.push_back(roi);
						ok = true;
					}
				}
				if (ok)  break;
			}
			if (ok)  break;
		}
		if (!ok)
		{
			vector<Rect> new_group;
			new_group.push_back(roi);
			groups.push_back(new_group);
		}
	}

	vector<Point> ctr_list;
	for (const auto& group : groups)
	{
		double sum_x = 0, sum_y = 0;
		for (const auto& rect : group)
		{
			sum_x += rect.x + rect.width / 2.0;
			sum_y += rect.y + rect.height / 2.0;
		}

		ctr_list.push_back(Point(sum_x / group.size(), sum_y / group.size()));
	}

	return ctr_list;
}

} //namespace OVS
