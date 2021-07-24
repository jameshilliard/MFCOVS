
// OVSSDKDemoDlg.h: 头文件
//

#pragma once
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <mutex>

#include <opencv.hpp>
#include <dshow.h>

#include "OVSAPI.h"

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")


using namespace std;
using namespace cv;

// COVSSDKDemoDlg 对话框
class COVSSDKDemoDlg : public CDialogEx
{
// 构造
public:
	COVSSDKDemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OVSSDKDEMO_DIALOG };
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
	bool RunSDK();
	// 入口
	bool Entrance();
	bool Entrance1();
	// 注册
	bool UserRegister();
	// 比对
	bool UserIdenfiy();
private:
	// 读写锁
	mutex ovs_mutex;

	bool b_sdk_status;
	bool b_user_register;
	bool b_user_idenfiy;

	/**************************************************************/
	/*                     显示使用                               */
	/**************************************************************/
	shared_ptr<CWnd> p_wnd;
	shared_ptr<CDC> p_dc;
	CImage display_image;
	CRect display_rect;
	// 显示到屏幕的mat
	Mat display_dst;



public:
	afx_msg void OnBnClickedButtonOpendevice();
	afx_msg void OnBnClickedButtonClosedevice();
	afx_msg void OnBnClickedButtonUserRegister();
	afx_msg void OnBnClickedButtonUserIdenfiy();
	afx_msg void OnBnClickedButtonStop();
	CButton button_open_device;
	CButton button_close_device;
	CButton button_user_register;
	CButton button_user_idenfiy;
	CButton button_stop;
	CEdit edit_user_id;
	CString edit_user_id_str;
	CProgressCtrl progress_ctrl_register;
};
