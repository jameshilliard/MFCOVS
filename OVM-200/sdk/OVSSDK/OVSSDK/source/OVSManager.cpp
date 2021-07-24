#include"OVSManager.h"

OVSManager::OVSManager()
	:b_device_status(false),
	b_user_idenfiy(false),
	b_user_register(false),
	b_distance_ok(false),
	b_exit_register(false),
	b_get_template(false),
	distance_value(0),
	global_idenfiy_feature(""),
	user_success_score(0)
{
	p_ovs_device = make_shared<OVSDevice>();
	p_device_param = make_shared<SDeviceParamInfo>(35, 6);
	p_logger = spdlog::rotating_logger_mt("file_logger", "log.txt", 1024 * 1024 * 5, 1);
	p_logger->flush_on(spdlog::level::warn);
	spdlog::flush_every(std::chrono::seconds(3));
}

OVSManager::~OVSManager()
{

}

OVSSTATUS OVSManager::OVSInit()
{
	// 设备初始化 曝光;LED电流;压缩率;黑电平;增益。
	if (!p_ovs_device->DeviceInit())
	{
		return ERR_SDK_INIT_FAIL;
	}
	b_device_status = true;
	thread thread_distance_device(&OVSManager::DistanceFromDevice, this);
	thread thread_extrac_image(&OVSManager::ExtractImageFromDevice, this);
	thread_distance_device.detach();
	thread_extrac_image.detach();
	return SUCCESS;
}

OVSSTATUS OVSManager::AddUserSingleInfo(string& in_user_id, string& featureLeft, string& featureRight)
{

	if (!featureLeft.empty())
	{
		for (int i = 0; i < template_num_limit; ++i)
		{
			string temp_user_info = "";
			for (int j = 0; j < 256; ++j)
			{
				temp_user_info += featureLeft[template_num_limit * i + j];
			}
			single_user_info.push_back(temp_user_info);
		}
	}
	if (!featureRight.empty())
	{
		for (int i = 0; i < template_num_limit; ++i)
		{
			string temp_user_info = "";
			for (int j = 0; j < 256; ++j)
			{
				temp_user_info += featureRight[template_num_limit * i + j];
			}
			single_user_info.push_back(temp_user_info);
		}
	}
	if ((!featureLeft.empty()) || (!featureRight.empty()))
	{
		all_user_info.insert(make_pair(in_user_id, single_user_info));
	}
	p_logger->info("add user single info success");
	return SUCCESS;
}

OVSSTATUS OVSManager::DeleteUserInfo(const string& user_id)
{
	all_user_info.erase(user_id);
	return SUCCESS;
}

OVSSTATUS OVSManager::StartFeatureForEnroll()
{
	b_user_register = true;
	b_user_idenfiy = false;
	template_num = 0;
	return SUCCESS;
}

OVSSTATUS OVSManager::StartFeatureForMatch()
{
	b_user_register = false;
	b_user_idenfiy = true;
	return SUCCESS;
}

OVSSTATUS OVSManager::GetPreviewImage(Image& image, Information& info)
{
	if (b_distance_ok)
	{
		if (b_user_register)
		{
			UserRegisterTemplate();
		}
		else if (b_user_idenfiy)
		{
			UserIdenfiyTemplate();
		}
		lock_guard<mutex> lk(ovs_mutex);

		// 返回图像
		image.data = display_mat.data;
		image.width = display_mat.cols;
		image.height = display_mat.rows;

		// 返回图像信息
		info = global_info;
	}

	return SUCCESS;
}

OVSSTATUS OVSManager::GetPreviewImageForEnroll(Image& image)
{
	// 返回图像
	image.data = development_mat.data;
	image.width = development_mat.cols;
	image.height = development_mat.rows;
	return SUCCESS;
}

OVSSTATUS OVSManager::GetEnrollFeature(string& feature)
{
	lock_guard<mutex> lk(ovs_mutex);
	if (global_register_feature.empty())
	{
		return ERR_GET_FEATURE;
	}
	feature = global_register_feature;

	global_info.palm_position = Information::NoPalm;
	global_info.palm_cx = 0;
	global_info.palm_cy = 0;
	global_info.is_feature_ok = false;
	global_info.enroll_stage = 0;
	global_info.progress_enroll = 0;
	b_user_register = false;
	global_register_feature = "";
	template_num = 0;

	return SUCCESS;
}

OVSSTATUS OVSManager::GetMatchFeature(string& feature)
{
	lock_guard<mutex> lk(ovs_mutex);
	if (global_idenfiy_feature.empty())
	{
		return ERR_GET_FEATURE;
	}
	feature = global_idenfiy_feature;
	global_info.palm_position = Information::NoPalm;
	global_info.palm_cx = 0;
	global_info.palm_cy = 0;
	global_info.is_feature_ok = false;
	global_info.enroll_stage = 0;
	global_info.progress_enroll = 0;
	global_idenfiy_feature = "";
	user_success_id = "";
	return SUCCESS;
}

bool OVSManager::UserRegisterTemplate()
{
	// 控制距离
	if (template_num >= template_num_limit)
	{
		lock_guard<mutex> lk(ovs_mutex);
		b_user_register = false;
		global_info.is_feature_ok = true;
		return true;
	}
	if (!b_distance_ok)
	{
		lock_guard<mutex> lk(ovs_mutex);
		global_info.palm_position = Information::TooFar;
		return false;
	}

	// 采集图像
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.copyTo(process_mat);
		if (process_mat.empty())
		{
			return false;
		}
	}
	global_info.palm_position = Information::Suitable;
	// 获取特征点
	string temp_enroll_feature = "";
	if (GetSingleFeature(process_mat, temp_enroll_feature) != OVS_SUCCESS)
	{
		return false;
	}
	else
	{
		lock_guard<mutex> lk(ovs_mutex);
		++template_num;
		global_register_feature += temp_enroll_feature;
		global_info.progress_enroll = template_num;
	}
	return true;
}

bool OVSManager::UserIdenfiyTemplate()
{
	if (!b_distance_ok)
	{
		lock_guard<mutex> lk(ovs_mutex);
		global_info.palm_position = Information::TooFar;
		return false;
	}

	// 采集图像
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.copyTo(process_mat);
		if (process_mat.empty())
		{
			return false;
		}
	}
	global_info.palm_position = Information::Suitable;

	// 获取特征点
	string temp_enroll_feature = "";
	if (GetSingleFeature(process_mat, temp_enroll_feature) != OVS_SUCCESS)
	{
		return false;
	}
	else
	{
		lock_guard<mutex> lk(ovs_mutex);
		global_idenfiy_feature = temp_enroll_feature;
		global_info.is_feature_ok = true;
	}
	return false;
}

bool OVSManager::MatchUser(string& feature, string& user_id)
{
	// 识别出的用户id
	user_success_id = "";

	// 识别分数
	float idenfiy_source = 10.f;
	for (auto& single_user_info : all_user_info)
	{
		for (auto& user_info : single_user_info.second)
		{
			double temp_source = PalmVein::compareFeature((unsigned char*)feature.c_str(), (unsigned char*)user_info.c_str(), 256);
			temp_source = 1024 - temp_source;
			if (temp_source > idenfiy_success_limit && temp_source > idenfiy_source)
			{
				idenfiy_source = temp_source;
				user_success_id = single_user_info.first;
			}
		}
	}
	user_success_score = (idenfiy_source > 0) ? idenfiy_source : 0;
	if (idenfiy_source < idenfiy_success_limit)
	{
		return OVS_ERR_MATCHING_FAILED;
	}
	user_id = user_success_id;
	b_user_idenfiy = false;
	return true;
}

bool OVSManager::UserTemplate()
{
	if (!b_distance_ok)
	{
		lock_guard<mutex> lk(ovs_mutex);
		global_info.palm_position = Information::TooFar;
		return false;
	}

	// 采集图像
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.copyTo(process_mat);
		if (process_mat.empty())
		{
			return false;
		}
	}
	global_info.palm_position = Information::Suitable;

	// 获取特征点
	string temp_global_feature = "";
	if (GetSingleFeature(process_mat, temp_global_feature) != OVS_SUCCESS)
	{
		return false;
	}
	else
	{
		lock_guard<mutex> lk(ovs_mutex);
		global_feature = temp_global_feature;
		global_info.is_feature_ok = true;
	}
	return false;
}

bool OVSManager::AbortCaptureRegister()
{
	global_info.palm_position = Information::NoPalm;
	global_info.palm_cx = 0;
	global_info.palm_cy = 0;
	global_info.is_feature_ok = false;
	global_info.enroll_stage = 0;
	global_info.progress_enroll = 0;
	b_user_register = false;
	global_register_feature = "";
	template_num = 0;
	return true;
}

bool OVSManager::AbortCaptureIdenfiy()
{
	global_info.palm_position = Information::NoPalm;
	global_info.palm_cx = 0;
	global_info.palm_cy = 0;
	global_info.is_feature_ok = false;
	global_info.enroll_stage = 0;
	global_info.progress_enroll = 0;
	global_idenfiy_feature = "";
	b_user_idenfiy = false;
	user_success_id = "";
	return true;
}

bool OVSManager::DeviceParamInit()
{
	// 默认增益
	int default_gain = 10;
	if (p_ovs_device->Set_Gain(default_gain) != S_OK)
	{
		return false;
	}

	//  默认曝光
	int default_exp = 40;
	if (p_ovs_device->Set_Exposure(default_exp) != S_OK)
	{
		return false;
	}
	// 默认LED
	int default_LED_lightness = 6;
	if (p_ovs_device->Set_LEDCurrent(default_LED_lightness) != S_OK)
	{
		return false;
	}
	// 默认图像压缩率
	int default_compression_ratio = 5;
	if (p_ovs_device->Set_CompressionRatio(default_compression_ratio) != S_OK)
	{
		return false;
	}
	bool flag = false;
	if (p_ovs_device->Enable_BL(flag) != S_OK)
	{
		return false;
	}
	return true;
}

int OVSManager::ListDevices()
{
	vector<string> capture_list;
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK)
		{
			IMoniker* pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag* pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
				if (FAILED(hr))
				{
					pMoniker->Release();
					continue;
				}
				// Find the description or friendly name.
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);

				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				}
				if (SUCCEEDED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);

					int count = 0;
					char tmp[255] = { 0 };
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					capture_list.push_back(tmp);
				}

				pPropBag->Release();
				pPropBag = NULL;

				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;

			pEnum->Release();
			pEnum = NULL;
		}
	}
	//return deviceCounter;

	// 获取摄像头ID
	int capture_id = 0;
	const string omec_device = "OMEC Vein Scanner";
	for (int i = 0; i < capture_list.size(); ++i)
	{
		if (capture_list[i] == omec_device)
		{
			capture_id = i;
			break;
		}
	}
	return capture_id;
}

bool OVSManager::ExtractImageFromDevice()
{
	// 获得摄像头
	int com_id = ListDevices();
	VideoCapture capture(com_id + cv::CAP_DSHOW);
	capture.set(CAP_PROP_FRAME_WIDTH, 1920);
	capture.set(CAP_PROP_FRAME_HEIGHT, 1080);
	// 设备初始化参数
	if (!DeviceParamInit())
	{
		return false;
	}

	// 从摄像头读取的Mat
	Mat frame;/*(1080,1920, CV_8UC3)*/

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		capture >> frame;
		if (frame.empty())
		{
			continue;
		}

		lock_guard<mutex> lk(ovs_mutex);
		frame.copyTo(development_mat);
		// 注册用户和识别用户
		if (b_user_register || b_user_idenfiy)
		{
			frame.copyTo(common_mat);

			Mat temp_mat;
			frame.copyTo(temp_mat);
			Mat temp_resize_mat;
			ImageTranslate(temp_mat, temp_resize_mat);
			ImageFuzzyTranslate(temp_resize_mat, display_mat);
			//resize(temp_resize_mat, display_mat, cv::Size(display_mat_width, display_mat_height), 0, 0, INTER_NEAREST);
		}
	}

	capture.release();
	return true;
}

void OVSManager::DistanceFromDevice()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		p_ovs_device->Get_Distance(distance_value);
		p_logger->warn(distance_value);
		if (distance_value > distance_upper_limit || distance_value < distance_lower_limit)
		{
			lock_guard<mutex> lk(ovs_mutex);
			b_distance_ok = false;
		}
		else
		{
			lock_guard<mutex> lk(ovs_mutex);
			b_distance_ok = true;
		}
	}
}

ProcessingState OVSManager::GetSingleFeature(Mat& in_mat, string& out_feature)
{
	// 算法
	shared_ptr<PalmVein> p_palm_parser = make_shared<PalmVein>(in_mat.data, in_mat.cols, in_mat.rows, 3);
	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);
	PalmImageInfo temp_image_info;
	// 解析图像
	if (p_palm_parser->parseInfo(temp_image_info) != OVS_SUCCESS)
	{
		return OVS_ERR_NO_FOUND_PALM;
	}

	// 模糊度
	if (temp_image_info.score > ambiguity_limit)
	{
		return OVS_ERR_NO_FOUND_PALM;
	}
	if (temp_image_info.mean_illumination > mean_illumination_upper_limit)
	{
		if (p_device_param->led_lightness >= led_value_upper_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调低亮度
		++p_device_param->led_lightness;
		p_ovs_device->Set_LEDCurrent(p_device_param->led_lightness);
		return OVS_ERR_ROI_TOOSTRONG;
	}
	if (temp_image_info.mean_illumination < mean_illumination_lower_limit)
	{
		if (p_device_param->led_lightness <= led_value_lower_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}

		// 调高亮度
		--p_device_param->led_lightness;
		p_ovs_device->Set_LEDCurrent(p_device_param->led_lightness);
		return OVS_ERR_ROI_TOOWEAK;
	}

	// 查找ROI区域
	uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
	if (p_palm_parser->findRoi(nullptr, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		return OVS_ERR_ROI_INVALID_POSITION;
	}

	uint8_t* roi_data = new uint8_t[roi_byte_size];
	if (p_palm_parser->findRoi(roi_data, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		delete[] roi_data;
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	std::vector<uint8_t> feat;
	if (p_palm_parser->computeFeature(feat) != OVS_SUCCESS)
	{
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	out_feature.insert(out_feature.begin(), feat.begin(), feat.end());
	return OVS_SUCCESS;
}


void OVSManager::ImageTranslate(Mat& in_mat, Mat& out_mat)
{
	Mat ok_dst(in_mat, Range(0, 1080), Range(420, 1500));
	out_mat = Mat(ok_dst.rows, ok_dst.cols, ok_dst.depth());
	transpose(ok_dst, out_mat);
}

const int OVSManager::HexToTen(string& in_str)
{
	int dec_out = stoi(in_str, nullptr, 16);
	return dec_out;
}

const string OVSManager::TenToHex(int& in_hec)
{
	string out;
	stringstream ss;
	ss << std::hex << in_hec;
	ss >> out;
	transform(out.begin(), out.end(), out.begin(), ::toupper);
	return out;
}

void OVSManager::ImageFuzzyTranslate(Mat& in_mat, Mat& out_mat)
{
	//image gray
	cv::Mat dst;
	if (in_mat.channels() == 3) {
		cv::cvtColor(in_mat, out_mat, CV_BGR2GRAY);
	}
	else {
		in_mat.copyTo(out_mat);
	}

	//pixel processing
	cv::Mat mask;
	cv::threshold(out_mat, mask, 80, 90, THRESH_BINARY);
	for (int i = 0; i < out_mat.rows; i++) {
		for (int j = 0; j < out_mat.cols; j++) {
			float val = mask.at<uchar>(i, j);
			if (val == 90) {
				out_mat.at<uchar>(i, j) = 90;
			}
		}
	}
}
