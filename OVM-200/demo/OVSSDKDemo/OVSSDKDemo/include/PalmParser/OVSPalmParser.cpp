#include "OVSPalmParser.h"

OVSPalmParser::OVSPalmParser(Mat& in_mat)
{
	int depth = 3;
	assert(in_mat.cols > 0 && in_mat.rows > 0 && (depth == 1 || depth == 3 || depth == 4));
	int type_ = CV_8UC3;
	if (depth == 1)
	{
		type_ = CV_8UC1;
	}
	else if (depth == 4)
	{
		type_ = CV_8UC4;
	}
	//  将图像转为灰度图像
	Mat gray;
	{
		const Mat data_(Size(in_mat.cols, in_mat.rows), type_, const_cast<unsigned char*>(in_mat.data));
		if (type_ == CV_8UC1)
		{
			data_.copyTo(gray);
		}
		else if (type_ == CV_8UC3)
		{
			cvtColor(data_, gray, cv::COLOR_RGB2GRAY);
		}
		else
		{
			cvtColor(data_, gray, cv::COLOR_RGBA2GRAY);
		}
	}
	segment(gray, raw_image_);
}

OVSPalmParser::~OVSPalmParser()
{

}

EProcessingState OVSPalmParser::ParseInfo(shared_ptr<SPalmImageInfo>& info)
{
	if (!roi_.data)
	{
		EProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}

	auto mean_illumination = mean(roi_);
	Mat lap;
	Laplacian(roi_, lap, CV_64F);
	Scalar mean, std_dev;
	meanStdDev(lap, mean, std_dev);

	info->center_x = palm_center_.x;
	info->center_y = palm_center_.y;
	info->score = std_dev(0);
	info->mean_illumination = mean_illumination(0);

	return OVS_SUCCESS;
}

EProcessingState OVSPalmParser::FindRoi(unsigned char* roi_data, uint32_t& roi_width, uint32_t& roi_height, uint32_t& roi_data_len)
{
	if (!roi_.data)
	{
		EProcessingState rlt = compute_roi();
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

EProcessingState OVSPalmParser::computeFeature(vector<uint8_t>& features)
{
	if (!roi_.data)
	{
		EProcessingState rlt = compute_roi();
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

EProcessingState OVSPalmParser::computeFeature(std::vector<float>& features)
{
	if (!roi_.data)
	{
		EProcessingState rlt = compute_roi();
		if (rlt != OVS_SUCCESS)
			return rlt;
	}

	build_filter_bank();
	enhance_roi();

	const Size winSize(128, 128), blockSize(128, 128), blockStride(32, 32), cellSize(16, 16);
	int nBins = 9;
	const Size winStride(32, 32), padding(0, 0);
	auto hog = HOGDescriptor(winSize, blockSize, blockStride, cellSize, nBins);
	hog.compute(roi_enhanced_, features, winStride, padding);

	return OVS_SUCCESS;
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

double OVSPalmParser::compareFeature(const uint8_t* src, const uint8_t* dst, uint32_t len)
{
	assert(src && dst);

	uint32_t hamming = 0;
	for (uint32_t i = 0; i < len; ++i)
	{
		uint8_t v = src[i] ^ dst[i];
		hamming += numbits_lookup_table[v];
	}
	// 输出正确率！！！！！！！！！！和山大算法不一致！！！！！！！！
	//return (1 - hamming / 2048.0f);
	return hamming;
}

float OVSPalmParser::compareFeature(const float* src, const float* dst, uint32_t len)
{
	assert(src && dst);

	double l2 = 0;
	for (uint32_t i = 0; i < len; ++i)
	{
		double v = src[i] - dst[i];
		l2 += v * v;
	}

	return (float)sqrt(1000.0f * l2 / len);
}

bool OVSPalmParser::loadModelFiles(const std::string& palm_model_path, const std::string& finger_model_path)
{
	shared_ptr<OVSPalmDetector> palm_detector = OVSPalmDetector::getInstance();
	if (!palm_detector->loadModel(palm_model_path))
	{
		return false;
	}

	shared_ptr<OVSFingerDetector>  finger_detector = OVSFingerDetector::getInstance();
	if (!finger_detector->loadModel(finger_model_path))
	{
		return false;
	}

	return true;
}

// 切割掉图像多余边缘，只保留长宽相等的中心部分。然后旋转90度
void OVSPalmParser::segment(const cv::Mat& src, cv::Mat& dst, const cv::Size& sz_ /*= cv::Size(540, 540)*/)
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

	resize(src(roi), dst, sz_, 0, 0, INTER_CUBIC);
	Mat rotationMatrix = getRotationMatrix2D(Point(dst.cols / 2, dst.rows / 2), -90.0, 1);
	warpAffine(dst, dst, rotationMatrix, dst.size());
}

EProcessingState OVSPalmParser::compute_roi()
{
	list<Point> finger_pts;
	bool ok = extract_fingers(finger_pts);
	if (!ok)
	{
		return OVS_ERR_NO_FOUND_PALM;
	}

	// 计算最左和最右指缝的距离和角度
	double d = finger_pts.back().x - finger_pts.front().x;
	double xx = d, yy = finger_pts.back().y - finger_pts.front().y;
	Point ctr = (finger_pts.back() + finger_pts.front()) / 2;
	double angle = atan2(yy, xx) * 180.0 / M_PI;
	Mat rotationMatrix = getRotationMatrix2D(ctr, angle, 1);

	// 归一化角度
	Mat warped;
	warpAffine(raw_image_, warped, rotationMatrix, raw_image_.size());

	const double r = 0.2;
	if (ctr.x - d / 2 < 0 || ctr.x + d / 2 >= warped.cols ||
		ctr.y + r * d + 1.6 * d >= warped.rows)
	{
		return OVS_ERR_GET_ROI_FAILED;
	}
	cout << "ctr: " << ctr << ", d: " << d << ", angle: " << angle << endl;
	palm_center_.x = ctr.x;
	palm_center_.y = ctr.y + r * d;
	// 归一化ROI尺寸
	resize(warped(Rect(ctr.x - d / 2, ctr.y + r * d, d, 1.6 * d)), roi_, Size(128, 128), INTER_LINEAR);

	return OVS_SUCCESS;
}

void OVSPalmParser::build_filter_bank()
{
	gabor_kernels_.clear();
	const int ksize = 31;
	const double sigma = 3.3, lambd = 18.3, gamma = 4.5, psi = 0.89;

	for (int i = 0; i < 8; ++i)
	{
		double theta = i * M_PI / 8;
		Mat kernel = getGaborKernel(Size(ksize, ksize),
			sigma, theta, lambd, gamma, psi, CV_32F);

		auto sum = cv::sum(kernel)(0);
		gabor_kernels_.push_back(kernel / (1.5 * sum));
	}
}

void OVSPalmParser::enhance_roi()
{
	roi_enhanced_ = Mat::zeros(roi_.size(), CV_8UC1);
	for (auto kernel : gabor_kernels_)
	{
		Mat mat;
		filter2D(roi_, mat, -1, kernel);
		roi_enhanced_ = max(roi_enhanced_, mat);
	}
	// clahe -> bilateral
	// clahe方法做直方图均衡
	Mat temp1, temp2;
	auto clahe1 = createCLAHE(25.0, Size(8, 8));
	clahe1->apply(roi_enhanced_, temp1);
	//bilateral filter 做降噪
	bilateralFilter(temp1, roi_enhanced_, -1, 20, 9.0);
}

void OVSPalmParser::extract_lbp_image(cv::Mat& lbp)
{
	//下采样
	Mat downsampled = Mat::zeros(roi_enhanced_.rows / 8, roi_enhanced_.cols / 8, CV_8UC1);

	resize(roi_enhanced_, downsampled, Size(roi_enhanced_.cols / 8, roi_enhanced_.rows / 8),
		0.0, 0.0, INTER_AREA);

	//平均滤波
	boxFilter(downsampled, lbp, CV_8UC1, Size(3, 3));
}

void OVSPalmParser::compute_lbp(const cv::Mat& src, cv::Mat& dst)
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

void OVSPalmParser::compute_nbp(const cv::Mat& src, cv::Mat& dst)
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

bool OVSPalmParser::extract_fingers(std::list<cv::Point>& finger_pts)
{
	if (palm_state_ == PS_UNKNOWN)
	{
		// 是否存在手掌
		bool ok = palm_exists(raw_image_);
		if (!ok)
		{
			palm_state_ = PS_NO_PALM;
			return false;
		}

		ok = find_fingers(raw_image_, finger_pts);
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

	if (finger_pts.size() != 3 && finger_pts.size() != 4)
	{
		palm_state_ = PS_NO_FINGER;
		return false;
	}
	// 去除多余的指缝，一般是拇指指缝
	if (finger_pts.size() == 4)
	{
		auto mit = finger_pts.begin();
		int max_y = mit->y;
		for (auto it = mit; it != finger_pts.end(); ++it)
		{
			if (it->y > max_y)
			{
				max_y = it->y;
				mit = it;
			}
		}
		finger_pts.erase(mit);
	}

	finger_pts.sort([](const Point& a, const Point& b) { return a.x < b.x; });

	//check if finger positions are valid
	{
		auto d2 = [](const Point& a, const Point& b) { return (a - b).ddot(a - b); };
		auto it = finger_pts.begin();
		Point p1 = *(it++);
		Point p2 = *(it++);
		Point p3 = *(it++);

		double d12 = d2(p1, p2), d23 = d2(p2, p3), d13 = d2(p1, p3);
		double angle = 180.0 / M_PI * acos(0.5 * (d12 + d23 - d13) / (sqrt(d12 * d23)));
		// cout << "finger angle: " << angle << endl;
		if (fabs(angle) < 145.0) //TODO hard-coded threshold
		{
			palm_state_ = PS_NO_FINGER;
			return false;
		}
	}

	palm_state_ = PS_OK;

	return true;
}

bool OVSPalmParser::palm_exists(const cv::Mat& src)
{
	Mat im;
	resize(src, im, Size(128, 128));


	shared_ptr<OVSPalmDetector> detector = OVSPalmDetector::getInstance();
	float value = detector->detect(im);
	// cout << value << endl;
	return (value < 0);
}

bool OVSPalmParser::find_fingers(const cv::Mat& src, std::list<cv::Point>& pts)
{
	shared_ptr<OVSFingerDetector> detector = OVSFingerDetector::getInstance();
	vector<Point> vpts;
	bool ok = detector->detect(src, vpts);
	if (!ok)
	{
		return false;
	}

	copy(vpts.begin(), vpts.end(), back_inserter(pts));
	return true;
}

/**
 * OVSPalmDetector
 */
shared_ptr<OVSPalmDetector> OVSPalmDetector::instance = nullptr;

OVSPalmDetector::OVSPalmDetector()
{

}

OVSPalmDetector::~OVSPalmDetector()
{

}

shared_ptr<OVSPalmDetector> OVSPalmDetector::getInstance()
{
	if (!instance)
	{
		instance = make_shared<OVSPalmDetector>();
	}

	return instance;
}

bool OVSPalmDetector::loadModel(const std::string& model_path)
{
	if (svm_.get())
		return true;
	svm_ = ml::SVM::load(model_path);
	FileStorage fs_(model_path, FileStorage::READ);
	Mat EigenValues, EigenVectors, Mean;
	fs_["eigenvalues"] >> EigenValues;
	fs_["eigenvectors"] >> EigenVectors;
	fs_["mean"] >> Mean;
	pca_.eigenvalues = EigenValues.clone();
	pca_.eigenvectors = EigenVectors.clone();
	pca_.mean = Mean.clone();
	fs_["pca_mean"] >> pca_mean_;

	return svm_.get() != nullptr;
}

float OVSPalmDetector::detect(const cv::Mat& src)
{
	assert(svm_.get());
	assert(src.cols == 128 && src.rows == 128);

	const Size winSize(128, 128), blockSize(32, 32), blockStride(16, 16), cellSize(8, 8);
	int nBins = 4;
	const Size winStride(32, 32), padding(0, 0);
	auto hog = HOGDescriptor(winSize, blockSize, blockStride, cellSize, nBins);
	vector<float> desc;
	hog.compute(src, desc, winStride, padding);
	const auto minmax = minmax_element(desc.begin(), desc.end());
	float min_v = *minmax.first, max_v = *minmax.second;
	auto range = max_v - min_v;
	for (auto& v : desc)
	{
		v = (v - min_v) / range;
	}

	Mat data(1, desc.size(), CV_32F);
	for (size_t i = 0; i < desc.size(); ++i)
	{
		data.at<float>(i) = desc[i];
	}

	Mat projected = pca_.project(data);

	for (int j = 0; j < projected.cols; ++j)
	{
		float mean = (pca_mean_.at<float>(0, j) + pca_mean_.at<float>(1, j)) / 2.0f;
		float range = (pca_mean_.at<float>(1, j) - pca_mean_.at<float>(0, j)) / 2.0f;
		projected.at<float>(j) = (projected.at<float>(j) - mean) / range;
		if (projected.at<float>(j) < -1.0)
			projected.at<float>(j) = -1.0;
		if (projected.at<float>(j) > 1.0)
			projected.at<float>(j) = 1.0;
	}


	Mat rlt;
	svm_->predict(projected, rlt, ml::SVM::RAW_OUTPUT);
	return rlt.at<float>(0);
}


/**
 * OVSFingerDetector
 */

shared_ptr<OVSFingerDetector> OVSFingerDetector::instance = nullptr;

OVSFingerDetector::OVSFingerDetector()
{

}

OVSFingerDetector::~OVSFingerDetector()
{

}

shared_ptr<OVSFingerDetector> OVSFingerDetector::getInstance()
{
	if (!instance)
	{
		instance = make_shared<OVSFingerDetector>();
	}

	return instance;
}

bool OVSFingerDetector::loadModel(const std::string& model_path)
{
	if (svm_.get())
		return true;
	svm_ = ml::SVM::load(model_path);

	return (svm_.get() != nullptr);
}

bool OVSFingerDetector::detect(const cv::Mat& src, std::vector<cv::Point>& pts)
{
	const Size winSize(32, 32), blockSize(32, 32), blockStride(8, 8), cellSize(8, 8);
	int nBins = 4;
	const Size winStride(4, 4), padding(0, 0);
	auto hog = HOGDescriptor(winSize, blockSize, blockStride, cellSize, nBins);
	hog.setSVMDetector(OVSFingerDetector::get_svm_detector(svm_));

	vector<Point> foundLocations;
	vector<double> foundWeights;
	hog.detect(src, foundLocations, foundWeights, 0.0, Size(4, 4), Size(0, 0));

	vector<Rect> roi_list;
	for (size_t i = 0; i < foundLocations.size(); ++i)
	{
		if (foundWeights[i] < 0.3) //TODO hard-coded threshold
			continue;

		Rect roi(foundLocations[i].x, foundLocations[i].y, 32, 32);
		Scalar mean, stddev;
		meanStdDev(src(roi), mean, stddev);
		if (stddev[0] < 10.0) //TODO hard-coded threshold
			continue;

		roi_list.push_back(roi);
	}

	pts = group_roi(roi_list);

	return pts.size() > 0;
}
std::vector<float> OVSFingerDetector::get_svm_detector(const cv::Ptr< cv::ml::SVM >& svm)
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

std::vector<cv::Point> OVSFingerDetector::group_roi(const std::vector<cv::Rect>& roi_list)
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
					if (a / a_roi > 0.3) //TODO hard-coded threshold
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
