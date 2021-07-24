
// VideoPreviewDlg.h : ͷ�ļ�
//

#pragma once
#include "../SonixCamera/util.h"
#include "afxwin.h"


// CVideoPreviewDlg �Ի���
class CVideoPreviewDlg : public CDialogEx
{
// ����
public:
	CVideoPreviewDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VIDEOPREVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
