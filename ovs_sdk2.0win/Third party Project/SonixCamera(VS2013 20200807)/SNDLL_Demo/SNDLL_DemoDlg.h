
// SNDLL_DemoDlg.h : 头文件
//

#pragma once
#include <functional>

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) { if (x) delete []x; x = NULL; }

#include <string>
#include "afxwin.h"
using std::string;

static const GUID PROPSETID_VIDCAP_EXTENSION_UNIT =
{ 0x28f03370, 0x6311, 0x4a2e, { 0xba, 0x2c, 0x68, 0x90, 0xeb, 0x33, 0x40, 0x16 } };

static const GUID PROPSETID_UVC_EXTENSION_UNIT =
{ 0xDDDF7394, 0x973E, 0x4727, { 0xBE, 0xD9, 0x04, 0xED, 0x64, 0x26, 0xDC, 0x67 } };

//{ 0xBD5321B4, 0xD635, 0xCA45, { 0xB2, 0x03, 0x4E, 0x01, 0x49, 0xB3, 0x01, 0xBC } };
//DDDF7394 - 973E-4727 - BED9 - 04ED6426DC67
// CSNDLL_DemoDlg 对话框
class CSNDLL_DemoDlg : public CDialogEx
{
// 构造
public:
	CSNDLL_DemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SNDLL_DEMO_DIALOG };

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
	afx_msg void OnBnClickedBtnAsicRegisterRead();
	afx_msg void OnBnClickedBtnAsicRegisterWrite();
	afx_msg void OnBnClickedBtnSensorRegisterRead();
	afx_msg void OnBnClickedBtnSensorRegisterWrite();
	afx_msg void OnBnClickedBtnFlashRead();
	afx_msg void OnBnClickedBtnFlashWrite();
	afx_msg void OnBnClickedBtnBurnerFW();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnExportFW();
	afx_msg void OnBnClickedBtnGetFwVersion();
	afx_msg void OnBnClickedBtnGetVidPid();
	afx_msg void OnBnClickedBtnGetErrorCode();
	afx_msg void OnBnClickedBtnGetSfType();
	afx_msg void OnBnClickedBtnGetManufacturer();
	afx_msg void OnBnClickedBtnGetProduct();
	afx_msg void OnBnClickedBtnGetSerialNumber();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnGetString3();
	afx_msg void OnBnClickedBtnFlashCustomRead();
	afx_msg void OnBnClickedBtnFlashCustomWrite();
	afx_msg void OnBnClickedBtnMd();
	afx_msg void OnBnClickedBtnXuRead();
	afx_msg void OnBnClickedBtnXuWrite();
	afx_msg void OnBnClickedBtnGetNodeid();
	afx_msg void OnBnClickedBtnGetInterface();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL LoadFW(const CString &fwPath);
	BOOL BurnerFW();

private:
	CString m_editString3;
	CString m_asicAddr;
	CString m_asicLength;
	CString m_sensorSlaveID;
	CString m_sensorAddr;
	CString m_sensorLength;
	CString m_sfDataLen;
	CString m_sfAddr;
	CString m_editOutput;
	CString m_editFwVersion;
	CString m_editVidPid;
	CString m_editErrorCode;
	CString m_editSFType;
	CString m_editManufacturer;
	CString m_editProduct;
	CString m_editSerialNumber;
	CString m_editInterface;

	CString m_editNodeId;
	CString m_editLength;
	CString m_editCS;

	BYTE *m_pFwBuf;
	LONG m_lFwBufLen;

	CString m_nodeId;
	CString m_editXuOutput;

};
