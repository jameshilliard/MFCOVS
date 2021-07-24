#pragma once
/**
*  OVS Application Functions Declaration
*
*  @author		Xu huabin, Ji renze
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			OVS Application API Declaration
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
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <opencv.hpp>
#include <dshow.h>
#include <spdlog.h>
#include "sinks/rotating_file_sink.h"

#include "OVSDevice.h"
#include "OVSStatus.h"
#include "palm_vein.hpp"

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")
using namespace std;
using namespace cv;
using namespace OVS;
using namespace spdlog;

struct SDeviceParamInfo
{
	SDeviceParamInfo(int temp_exposure, int temp_led_lightness)
	{
		this->exposure = temp_exposure;
		this->led_lightness = temp_led_lightness;
	}
	// 曝光
	int exposure;
	// led
	int led_lightness;
};

class OVSManager
{
public:
	OVSManager();
	~OVSManager();

	// 初始化
	OVSSTATUS OVSInit();
	// 添加用户数据
	OVSSTATUS AddUserSingleInfo(string& in_user_id, string& featureLeft, string& featureRight);
	// 删除用户数据
	OVSSTATUS DeleteUserInfo(const string& in_user_id);

	// 开始录入
	OVSSTATUS StartFeatureForEnroll();

	// 开始识别
	OVSSTATUS StartFeatureForMatch();
	// 提取SDK显示mat
	OVSSTATUS GetPreviewImage(Image& image, Information& info);

	// 提取SDK显示mat
	OVSSTATUS GetPreviewImageForEnroll(Image& image);

	// 获取录入模板
	OVSSTATUS GetEnrollFeature(string& feature);

	// 获取识别模板
	OVSSTATUS GetMatchFeature(string& feature);

	// 识别成功用户id
	string user_success_id;

	// 注册
	bool UserRegisterTemplate();

	// 对比
	bool UserIdenfiyTemplate();
	bool MatchUser(string& feature, string& user_id);

	// 获取模板（通用）
	bool UserTemplate();




	// 取消录入过程
	bool AbortCaptureRegister();

	// 取消比对过程
	bool AbortCaptureIdenfiy();

public:
	// SDK和设备状态
	bool b_device_status;
	// 用户注册
	bool b_user_register;
	// 用户识别
	bool b_user_idenfiy;

	// 设备控制
	shared_ptr<OVSDevice> p_ovs_device;

	// 距离值
	int distance_value;

private:
	// 初始化摄像头参数
	bool DeviceParamInit();
	// 获得设备
	int ListDevices();
	// 从摄像头读取图像
	bool ExtractImageFromDevice();
	// 从设备读取距离
	void DistanceFromDevice();

	// 提取单个特征点
	ProcessingState GetSingleFeature(Mat& in_mat, string& out_feature);

	// 对比
	// 处理对比模板
	ProcessingState GetIdenfiyTemplate(Mat& in_mat);

	// 图像转换
	void ImageTranslate(Mat& in_mat, Mat& out_mat);
	// 十六进制转十进制
	static const int HexToTen(string& in_str);
	// 十进制转十六进制
	static const string TenToHex(int& in_hec);

	// 图像模糊转换
	void ImageFuzzyTranslate(Mat& in_mat, Mat& out_mat);
private:
	// sqdlog
	shared_ptr<logger> p_logger;
	// 设备曝光和LED信息
	shared_ptr<SDeviceParamInfo> p_device_param;

	// 私有用户数据
	map<string, vector<string>> all_user_info;

	// 单个用户已经注册的数据，模板数量为 template_num_limit
	vector<string> single_user_info;

	// 读写锁
	mutex ovs_mutex;
	// 公有mat 设备直接读取
	Mat common_mat;
	// 处理mat 注册和对比使用
	Mat process_mat;
	// 显示mat SDK输出显示使用
	Mat display_mat;
	// 开发者使用
	Mat development_mat;

	// 全局信息
	Information global_info;

	// 全局特征值
	string global_register_feature;
	string global_idenfiy_feature;
	string global_feature;

	// 用户识别成功
	bool b_idenfiy_success;

	// 用户注册取消
	bool b_exit_register;


	// 距离合格小于240
	bool b_distance_ok;

	// 实时提取的模板数量
	int template_num;
	// 是否继续获取模板
	bool b_get_template;

	/********************
	*
	*      阀值参数
	*
	*********************/
	// SDK输出显示图像像素
	const int display_mat_width = 135;
	const int display_mat_height = 135;
	int user_success_score;

	// 识别正确率阀值
	const int idenfiy_success_limit = 512;

	// 模糊度阀值
	const float ambiguity_limit = 12.0f;
	// 平均亮度阀值
	const float mean_illumination_upper_limit = 150.0f;
	const float mean_illumination_lower_limit = 100.0f;
	// led值阀值
	const int led_value_upper_limit = 7;
	const int led_value_lower_limit = 6;

	// 模板数量阀值
	const int template_num_limit = 5;
	const int template_vector_length = 576;
	// 距离阀值
	const int distance_upper_limit = 80;
	const int distance_lower_limit = 30;

};