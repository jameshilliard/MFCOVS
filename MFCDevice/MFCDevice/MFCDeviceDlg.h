
// MFCDeviceDlg.h: 头文件
//

#pragma once
#include <memory>
#include <string>
#include <direct.h>
#include <opencv.hpp>
#include <json.h>
#include <sqlite3.h>
#include "DeviceCommon/DeviceControl.h"
#include <dshow.h>
#include <Python.h>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

using namespace cv;
using namespace std;

struct USERINFO
{
	USERINFO(int temp_serial_number,
		bool temp_is_right_hand,
		string temp_user_ID,
		int temp_collcetion_times,
		string temp_exp_value,
		string temp_BL_value,
		int temp_distance_value,
		string temp_decive_ID,
		int temp_dynamic_number)
	{
		this->serial_number = temp_serial_number;
		this->is_right_hand = temp_is_right_hand;
		this->user_ID = temp_user_ID;
		this->collection_times = temp_collcetion_times;
		this->exp_value = temp_exp_value;
		this->BL_value = temp_BL_value;
		this->distance_value = temp_distance_value;
		this->device_ID = temp_decive_ID;
		this->dynamic_number = temp_dynamic_number;
	}
	// 序号
	int serial_number;
	// 右手（true） 左手（false）
	bool is_right_hand;
	// 用户ID
	string user_ID;
	// 采集次数
	int collection_times;
	// 当前曝光值
	string exp_value;
	// 当前电流值
	string BL_value;
	// 当前采集距离
	int distance_value;
	// 设备ID 
	string device_ID;
	// 动态采集序号
	int dynamic_number;
};

struct USERJSON
{
	USERJSON(string temp_user_ID,int temp_collcetion_times)
	{
		this->user_ID = temp_user_ID;
		this->collection_times = temp_collcetion_times;
	}
	// 用户ID
	string user_ID;
	// 采集次数
	int collection_times;
};

// CMFCDeviceDlg 对话框
class CMFCDeviceDlg : public CDialogEx
{
// 构造
public:
	CMFCDeviceDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMFCDeviceDlg();

	// json序列化
	bool Serialization();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDEVICE_DIALOG };
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
	/*MatToCImage
	*简介：
	*	OpenCV的Mat转ATL/MFC的CImage，仅支持单通道灰度或三通道彩色
	*参数：
	*	mat：OpenCV的Mat
	*	cimage：ATL/MFC的CImage
	*/
	void MatToCImage(Mat& mat, CImage& cimage);

	/*CImageToMat
	*简介：
	*	ATL/MFC的CImage转OpenCV的Mat，仅支持单通道灰度或三通道彩色
	*参数：
	*	cimage：ATL/MFC的CImage
	*	mat：OpenCV的Mat
	*/
	void CImageToMat(CImage& cimage, Mat& mat);

	// 获取摄像头ID
	int ListDevices();

	// 提取摄像头视频
	CWinThread* p_capture_thread;
	static UINT CpatureThread(LPVOID param);
	// 提取距离
	CWinThread* p_distance_thread;
	static UINT DistanceThread(LPVOID param);

	// 显示log
	void ShowLog(string& msg_str);
	// 显示距离
	void ShowDistance(string distance_str);

	// 存储图片文件名
	string ImageFileName();
	// 存储动态采集图片名
	string ImageDynamicFileName();
	// 存储视频文件名
	string VideoFileName();

	// 初始化曝光和LED值
	void InitLEDExp();
	// 初始化动态采集曝光LED
	void InitMovingLEDExp();
	// json检查
	bool InitJsonCheck();
	void InitDecivce();
	// 打开数据库
	bool OpenDB();

	// 十六进制转为十进制
	int HexToTen(string& in_str);
	// 十进制转十六进制
	string TenToHex(int& in_hec);
	//python
	void PythonBright(string& in_dir,double& in_result);

	// 设备控制
	shared_ptr<DeviceControl> p_device_control;

	// db
	shared_ptr<sqlite3> p_db;

	// 视频写入
	VideoWriter video_writer;

	// json读取用户ID 进行检查
	vector<shared_ptr<USERJSON>> user_json_vec;
	// 用户采集信息
	shared_ptr<USERINFO> user_info;
	// 正常采集序号
	int user_serial_number;
	// 动态采集序号
	int user_dynamic_number;
	// 设备状态
	bool b_device_status;
	// 是否保存图片  
	int b_save_image;
	// 保存视频
	bool b_save_video;
	// LED亮度值
	int LED_lightness;
	const int LED_range = 10;
	// 左右手阀值
	const int hand_threshold = 16;

	// 动态采集
	bool b_dynamic_collect;
	//Python初始化
	bool b_python_init;
	// 距离值
	string distance_value;

	// 字体
	CFont edit_font;

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 打开设备
	afx_msg void OnBnClickedOpenDevice();
	// 关闭设备
	afx_msg void OnBnClickedCloseDevice();

	// LOG
	afx_msg void OnEnChangeEditLog();

	// 曝光-
	afx_msg void OnBnClickedBtnExpDown();
	// 曝光+
	afx_msg void OnBnClickedBtnExpUp();
	// 曝光值
	afx_msg void OnEnChangeEditExpValue();
	// 滚动曝光值滑动条
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNMReleasedcaptureSliderExpValue(NMHDR* pNMHDR, LRESULT* pResult);

	// 增益-
	afx_msg void OnBnClickedGainValueDown();
	// 增益+
	afx_msg void OnBnClickedGainValueUp();
	// 滚动增益值滑动条
	afx_msg void OnNMReleasedcaptureSliderGainValue(NMHDR* pNMHDR, LRESULT* pResult);

	// LED亮度-
	afx_msg void OnBnClickedLedValueDown();
	// LED亮度+
	afx_msg void OnBnClickedLedValueUp();
	// LED亮度显示
	afx_msg void OnEnChangeEditLed();

	// 保存图片
	afx_msg void OnBnClickedButtonSaveImage();

	afx_msg void OnEnChangeEditUserId();

	// 停止录制视频
	afx_msg void OnBnClickedButtonVideoStop();
	// 开始录制视频
	afx_msg void OnBnClickedButtonVideoStart();

	// 检查用户是否存在
	afx_msg void OnBnClickedButtonCheckUserID();
	// 复位
	afx_msg void OnBnClickedButtonReset();

	// 关闭设备
	CButton close_device;
	// 打开设备
	CButton open_device;
	// log显示
	CEdit edit_log;

	// 曝光+
	CButton button_exp_down;
	// 曝光-
	CButton button_exp_up;
	// 曝光滑动
	CSliderCtrl slider_exp_value;
	// 曝光值
	CString exp_value;

	// 增益滑动
	CSliderCtrl slider_gain_value;
	// 增益-
	CButton button_gain_up;
	// 增益+
	CButton button_gain_down;
	// 增益值
	CString gain_value;

	// LED亮度-
	CButton button_LED_down;
	// LED亮度+
	CButton button_LED_up;
	// LED亮度显示
	CString LED_value;

	// 距离值
	CEdit edit_distance_value;

	// 保存图片
	CButton button_save_image;

	// 视频停止录制
	CButton button_video_stop;
	// 开始录制视频
	CButton button_video_start;

	// 用户ID
	CEdit edit_user_ID;
	CString cuser_ID;
	string user_ID;

	// 检测用户ID
	CButton button_check_user_ID;
	// 复位
	CButton button_reset;

	// 动态采集左右手
	CButton button_left_moving;
	CButton button_right_moving;
	// 动态采集退出
	CButton button_moving_quit;
public:
	afx_msg void OnBnClickedButtonRightMoving();
	afx_msg void OnBnClickedButtonLeftMoving();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnEnChangeEditDistanceValue();
};
