
// OVSMFCUserDlg.h: 头文件
//

#pragma once
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <mutex>
#include <list>

#include <opencv.hpp>
#include <dshow.h>

#include "SonixCamera.h"
#include "MultiCamera.h"
#include "OVSSqlite.h"
#include "OVSDevice.h"
#include "OVSPalmParser.h"

#include "TextProgressCtrl.h"
#include "spdlog.h"
#include "sinks/basic_file_sink.h"
#include "SerialPort.h"
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

using namespace std;
using namespace spdlog;

// COVSMFCUserDlg 对话框
class COVSMFCUserDlg : public CDialogEx
{
// 构造
public:
	COVSMFCUserDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OVSMFCUSER_DIALOG };
#endif

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
	void DistanceOfDevice();
	// 从设备提取图像
	bool ExtractImageFromDevice();
	// 处理图像
	bool ProcessImage();
	// 图像显示到屏幕(不包含mat处理)
	bool DisplayImageCommon(Mat& in_mat);
	// 注册图像显示到屏幕(包含mat处理)
	bool DisplayImageRegister(Mat& in_mat);
	// 识别图像先是到屏幕(包含mat处理)
	bool DisplayImageIdenfiy(Mat& in_mat);

	// 用户注册
	bool UserRegister();
	// 处理提取模板
	EProcessingState GetRegisterTemplate(Mat& in_mat);
	// 控制模板间隔时间
	bool GetRegisterTemplateTime();

	// 保存指定图像
	bool SavePhoto();
	bool GetNoPalmPhoto(Mat& in_mat);
	// 用户识别
	bool UserIdenfiy();
	// 用户识别提取模板
	EProcessingState GetIdenfiyTemplate(Mat& in_mat);
	// 用户识别时间控制
	bool UserIdenfiyTimeControl();

	// 获取设备ID
	bool GetCameraID();
	// 设备默认参数
	bool DeviceParamInit();
	// 十六进制转十进制
	static const int HexToTen(string& in_str);
	// 十进制转十六进制
	static const string TenToHex(int& in_hec);
	// Mat转Image
	void MatToCImage(Mat& mat, CImage& cimage);
	// 处理Mat用于展示
	void ImageTranslate(Mat& out_mat, Mat& in_mat);
	//char[]转16进制 --> add by xhb
	void to_hex(char* in_char, int char_length, char* out_char);


private:
	mutex ovs_mutex;
	// 设备控制
	shared_ptr<OVSDevice> p_device_control;

	//串口 -->add by xhb
	shared_ptr<SerialPort> p_serial_port; 
	string serial_port = "COM3";
	bool command_flag = true;

	shared_ptr<SerialPort> p_serial_port_test;

	// 数据库控制
	shared_ptr<OVSSqlite> p_sqlite_control;

	// sqdlog
	// shared_ptr<logger> // p_logger;

	// 公共mat
	Mat common_mat;
	// 处理mat
	Mat process_mat;
	// 用户信息
	vector<shared_ptr<SSqliteUserInfo>> p_user_info_vector;
	// 所有用户信息
	vector<shared_ptr<SSqliteUserInfo>> p_all_user_info_vector;
	// 设备状态
	bool b_device_status;
	// 需要绘制的信息
	shared_ptr<SImageInfo> p_image_info;

	// 距离值
	int distance_value;
	// 距离合格大于250
	bool b_distance_ok;

	// 用户注册状态
	bool b_user_register;
	// 用户取消注册
	bool b_exit_register;

	// 用户识别状态
	bool b_user_idenfiy;
	// 一次识别成功的用户id
	string user_success_id;
	// 一次识别成功的用户分数
	int user_success_score;
	
	// 控制提取模板数量
	int template_num;
	// 是否继续获取模板
	bool b_get_template;
	// 成功用户缓存
	list<string> success_user_list;
	// 三次识别成功
	bool b_idenfiy_success;

	shared_ptr<CWnd> p_wnd;
	shared_ptr<CDC> p_dc;
	CImage display_image;
	CRect display_rect;
	// 显示到屏幕的mat
	Mat display_dst;

	bool b_save_photo;
	int no_palm_photo_number;
	// 识别正确率阀值
	const float idenfiy_success_limit = 400.0f;

	// 模糊度阀值
	const float ambiguity_limit = 12.0f;
	// 平均亮度阀值
	const float mean_illumination_upper_limit = 180.0f;
	const float mean_illumination_lower_limit = 80.0f;
	// led值阀值
	const int led_value_upper_limit = 7;
	const int led_value_lower_limit = 6;
	// 模板阀值
	const int template_num_limit = 20;
	// 手掌距离限制
	const int palm_distance_upper_limit = 110;
	const int palm_distance_lower_limit = 35;
	// 手掌圆圈设置
	const int palm_circle_offset = 50;
	const int palm_circle_radius = 100;

	// 绘制字体参数设置
	const int font_face = cv::FONT_HERSHEY_COMPLEX;
	const double font_scale = 0.7;
	const int thickness = 0.8;
private:
	// 打开设备
	afx_msg void OnBnClickedButtonOpendevice();
	// 关闭设备
	afx_msg void OnBnClickedButtonClosedevice();
	// 用户注册
	afx_msg void OnBnClickedButtonUserRegister();
	// 用户识别
	afx_msg void OnBnClickedButtonUserIdenfiy();
	// 停止
	afx_msg void OnBnClickedButtonStop();
	// 滑动条
	afx_msg void OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult);

private:
	// 打开设备
	CButton button_open_device;
	// 关闭设备
	CButton button_close_device;
	// 用户ID输入框
	CEdit edit_user_id;
	CString edit_user_id_str;
	// 用户ID说明
	CStatic static_user_id;
	// 用户识别
	CButton button_user_idenfiy;
	// 用户注册
	CButton button_user_register;
	// 停止
	CButton button_stop;
	// 滑动条
	CProgressCtrl progress_ctrl_register;
public:
	afx_msg void OnBnClickedButtonSavePhoto();
	CButton button_save_photo;
	afx_msg void OnBnClickedButtonTest();
};
