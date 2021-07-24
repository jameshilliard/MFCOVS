
// VideoPreviewDlg.h : 头文件
//

#pragma once
#include "../SonixCamera/util.h"
#include "afxwin.h"


// CVideoPreviewDlg 对话框
class CVideoPreviewDlg : public CDialogEx
{
// 构造
public:
	CVideoPreviewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VIDEOPREVIEW_DIALOG };

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
	CameraInfo m_ci[5];
	LONG cameraCount;
	CString m_StatusOutput;
	CEdit m_StatusShow;

	int m_iPreviewFormat;
	int m_iStillFormat;
	LONG m_CurSelectDevice;
public:
	afx_msg void OnLbnDblclkListDevice();
	afx_msg void OnBnClickedBtnStartPreview();
	afx_msg void OnBnClickedBtnStopPreview();
	afx_msg void OnBnClickedBtnGetFormat();
	afx_msg void OnBnClickedBtnSetFormat();
	afx_msg void OnBnClickedBtnSnapCaptureBuffer();
	afx_msg void OnBnClickedBtnPropertySet();
	afx_msg void OnBnClickedBtnControlSet();
	afx_msg void OnCbnSelchangeComboProperty();
	afx_msg void OnCbnSelchangeComboControl();
	afx_msg void OnCbnSelchangeProperty();
	afx_msg void OnCbnSelchangeControl();
	BOOL getEnumDevice();
	CListBox m_CameraList;
	afx_msg void OnBnClickedBtnOpenCamera();
	afx_msg void OnBnClickedBtnCloseCamera();
	CComboBox m_PropertyCB;
	CComboBox m_ControlCB;
	afx_msg void OnClose();
};
