#pragma once
#include <thread>
#include "ovm_200_defines.h"
#include "ovm_200_feature.h"
#include "ovm_200_match.h"
// ovs test demoDlg.h: 头文件
//

#pragma once
using namespace OVS;

// CovstestdemoDlg 对话框
class CovstestdemoDlg : public CDialogEx
{
// 构造
public:
	CovstestdemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OVSTESTDEMO_DIALOG };
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
public:
	afx_msg void OnBnClickedButtonInit();
	//图像显示线程
	thread thread_display_imag;
	bool ExtractImageFromDevice();
	void PreviewImage(int hWnd, unsigned char* imageData, int imageWidth, int imageHeight, int imageChannel);
	afx_msg void OnBnClickedButtonStartEnroll();
};
