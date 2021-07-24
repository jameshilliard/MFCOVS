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

#pragma once
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <Python.h>
#include <list>
#include <vector>
#include <opencv.hpp>
#include <dshow.h>

#include <iostream>
#include <fstream>

#include "SonixCamera.h"
#include "MultiCamera.h"
#include "OVSSqliteControl.h"
#include "OVSDevice.h"
#include "OVSPalmParser.h"
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

using namespace std;
using namespace cv;
// COVSApplicationDlg 对话框
class COVSApplicationDlg : public CDialogEx
{
	// 构造
public:
	COVSApplicationDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OVSAPPLICATION_DIALOG };
#endif
	// 设备运行

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	// 运行初始化
	bool RunInit();
	// 获得设备
	int ListDevices();
	// 设备参数初始化
	bool DeviceParamInit();
	// 图像输出到屏幕
	bool ImageOutputToScreen();
	void MatToCImage(Mat& mat, CImage& cimage);
	void CImageToMat(CImage& cimage, Mat& mat);
	//图像转换
	void ImageTranslate(Mat& out_mat,Mat& in_mat);
	// 识别用户
	bool UserIdenfiy();
	// 临时
	EProcessingState IdenfiyTemp(Mat& in_mat);
	// 注册用户
	bool UserRegister();
	// 获取模板
	EProcessingState GetTemplate(Mat& in_mat);
	// 计时30秒
	bool UserIdenfiyTimeControl();
	// 控制写入硬件时间反应时间
	bool ControlToHardware();
	// 控制得到模板上时间
	bool GetTemplateTime();
	// 设备距离
	void DistanceOfDevice();

	// 存储静态图片文件名
	string StaticImageFileName();

	// log显示
	void ShowLog(string& msg_str);
	// distance显示
	void ShowDistance(string& msg_str);
	// 十六进制转十进制
	static const int HexToTen(string& in_str);
	// 十进制转十六进制
	static const string TenToHex(int& in_hec);
private:
	// 设备控制
	shared_ptr<OVSDevice> p_ovs_device;
	// 用户数据库
	shared_ptr<OVSSqliteControl> p_sqlite_control;
	// 用户信息
	vector<shared_ptr<SSqliteUserInfo>> p_user_info_vector;
	// 掌静脉图像处理
	vector<shared_ptr<SSqliteUserInfo>> p_all_user_info_vector;
	// 当前用户ID
	string current_user_id;
	// 当前模板采集次数
	int current_template_num;
	// 手掌中心坐标
	const int template_limitation = 100;
	const int hand_radius = 200;
	int hand_center_x;
	int hand_center_y;
	// 读写锁
	mutex ovs_mutex;
	// 线程共同读写的mat
	Mat common_mat;
	// 缓存mat
	Mat temp_register_mat;

	// 用户识别
	bool b_user_idenfiy;
	// 用户注册
	bool b_user_register;
	// 距离值
	int distance_value;
	// 全局平均亮度
	double illumination;
	bool b_hardware_change;

	// 临时 控制提取模板
	int template_num;
	bool b_get_template;
private:
	// 打开设备
	afx_msg void OnBnClickedButtonOpenDevice();
	// 关闭设备
	afx_msg void OnBnClickedButtonClose();

	// 检测用户
	afx_msg void OnBnClickedButtonCheckUser();
	// 用户注册
	afx_msg void OnBnClickedButtonUserRegister();
	// 用户识别
	afx_msg void OnBnClickedButtonUserIdenfiy();
private:
	// 打开设备
	CButton button_open_device;
	// 关闭设备
	CButton button_close_device;

	// 用户注册
	CButton button_user_register;
	// 用户识别
	CButton button_user_idenfiy;
	// 检测用户
	CButton button_check_user;
	// 用户ID
	CEdit edit_user_id;
	CString edit_user_id_str;
	// 距离log
	CEdit edit_distance_log;
	// log
	CEdit edit_log;
	CStatic static_user_id;
	// 字体
	CFont edit_font;
public:
	afx_msg void OnBnClickedButtonIdentifyClose();
private:
	// 用户识别停止
	CButton button_identify_close;
};

