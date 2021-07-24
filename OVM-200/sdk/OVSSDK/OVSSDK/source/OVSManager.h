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
	// �ع�
	int exposure;
	// led
	int led_lightness;
};

class OVSManager
{
public:
	OVSManager();
	~OVSManager();

	// ��ʼ��
	OVSSTATUS OVSInit();
	// ����û�����
	OVSSTATUS AddUserSingleInfo(string& in_user_id, string& featureLeft, string& featureRight);
	// ɾ���û�����
	OVSSTATUS DeleteUserInfo(const string& in_user_id);

	// ��ʼ¼��
	OVSSTATUS StartFeatureForEnroll();

	// ��ʼʶ��
	OVSSTATUS StartFeatureForMatch();
	// ��ȡSDK��ʾmat
	OVSSTATUS GetPreviewImage(Image& image, Information& info);

	// ��ȡSDK��ʾmat
	OVSSTATUS GetPreviewImageForEnroll(Image& image);

	// ��ȡ¼��ģ��
	OVSSTATUS GetEnrollFeature(string& feature);

	// ��ȡʶ��ģ��
	OVSSTATUS GetMatchFeature(string& feature);

	// ʶ��ɹ��û�id
	string user_success_id;

	// ע��
	bool UserRegisterTemplate();

	// �Ա�
	bool UserIdenfiyTemplate();
	bool MatchUser(string& feature, string& user_id);

	// ��ȡģ�壨ͨ�ã�
	bool UserTemplate();




	// ȡ��¼�����
	bool AbortCaptureRegister();

	// ȡ���ȶԹ���
	bool AbortCaptureIdenfiy();

public:
	// SDK���豸״̬
	bool b_device_status;
	// �û�ע��
	bool b_user_register;
	// �û�ʶ��
	bool b_user_idenfiy;

	// �豸����
	shared_ptr<OVSDevice> p_ovs_device;

	// ����ֵ
	int distance_value;

private:
	// ��ʼ������ͷ����
	bool DeviceParamInit();
	// ����豸
	int ListDevices();
	// ������ͷ��ȡͼ��
	bool ExtractImageFromDevice();
	// ���豸��ȡ����
	void DistanceFromDevice();

	// ��ȡ����������
	ProcessingState GetSingleFeature(Mat& in_mat, string& out_feature);

	// �Ա�
	// ����Ա�ģ��
	ProcessingState GetIdenfiyTemplate(Mat& in_mat);

	// ͼ��ת��
	void ImageTranslate(Mat& in_mat, Mat& out_mat);
	// ʮ������תʮ����
	static const int HexToTen(string& in_str);
	// ʮ����תʮ������
	static const string TenToHex(int& in_hec);

	// ͼ��ģ��ת��
	void ImageFuzzyTranslate(Mat& in_mat, Mat& out_mat);
private:
	// sqdlog
	shared_ptr<logger> p_logger;
	// �豸�ع��LED��Ϣ
	shared_ptr<SDeviceParamInfo> p_device_param;

	// ˽���û�����
	map<string, vector<string>> all_user_info;

	// �����û��Ѿ�ע������ݣ�ģ������Ϊ template_num_limit
	vector<string> single_user_info;

	// ��д��
	mutex ovs_mutex;
	// ����mat �豸ֱ�Ӷ�ȡ
	Mat common_mat;
	// ����mat ע��ͶԱ�ʹ��
	Mat process_mat;
	// ��ʾmat SDK�����ʾʹ��
	Mat display_mat;
	// ������ʹ��
	Mat development_mat;

	// ȫ����Ϣ
	Information global_info;

	// ȫ������ֵ
	string global_register_feature;
	string global_idenfiy_feature;
	string global_feature;

	// �û�ʶ��ɹ�
	bool b_idenfiy_success;

	// �û�ע��ȡ��
	bool b_exit_register;


	// ����ϸ�С��240
	bool b_distance_ok;

	// ʵʱ��ȡ��ģ������
	int template_num;
	// �Ƿ������ȡģ��
	bool b_get_template;

	/********************
	*
	*      ��ֵ����
	*
	*********************/
	// SDK�����ʾͼ������
	const int display_mat_width = 135;
	const int display_mat_height = 135;
	int user_success_score;

	// ʶ����ȷ�ʷ�ֵ
	const int idenfiy_success_limit = 512;

	// ģ���ȷ�ֵ
	const float ambiguity_limit = 12.0f;
	// ƽ�����ȷ�ֵ
	const float mean_illumination_upper_limit = 150.0f;
	const float mean_illumination_lower_limit = 100.0f;
	// ledֵ��ֵ
	const int led_value_upper_limit = 7;
	const int led_value_lower_limit = 6;

	// ģ��������ֵ
	const int template_num_limit = 5;
	const int template_vector_length = 576;
	// ���뷧ֵ
	const int distance_upper_limit = 80;
	const int distance_lower_limit = 30;

};