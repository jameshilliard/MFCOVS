
// VideoPreviewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VideoPreview.h"
#include "VideoPreviewDlg.h"
#include "afxdialogex.h"

#include "../SonixCamera/SonixCamera.h"
#include "../SonixCamera/MultiCamera.h"
#include "../SonixCamera/AudioVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool m_bCaptureBuffer = false;
int iBufferCount = 0;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVideoPreviewDlg 对话框



CVideoPreviewDlg::CVideoPreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoPreviewDlg::IDD, pParent)
	, m_StatusOutput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ZeroMemory(m_ci, sizeof(m_ci));
	cameraCount = 0;
	m_iPreviewFormat = 0;
	m_iStillFormat = 0;
	m_CurSelectDevice = 0;
}

void CVideoPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STATUS_SHOW, m_StatusOutput);
	DDX_Control(pDX, IDC_EDIT_STATUS_SHOW, m_StatusShow);
	DDX_Text(pDX, IDC_EDIT_FORMAT, m_iPreviewFormat);
	DDX_Control(pDX, IDC_LIST_CAMERA_LIST, m_CameraList);
	DDX_Control(pDX, IDC_COMBO_PROPERTY, m_PropertyCB);
	DDX_Control(pDX, IDC_COMBO_CONTROL, m_ControlCB);
}

BEGIN_MESSAGE_MAP(CVideoPreviewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START_PREVIEW, &CVideoPreviewDlg::OnBnClickedBtnStartPreview)
	ON_BN_CLICKED(IDC_BTN_STOP_PREVIEW, &CVideoPreviewDlg::OnBnClickedBtnStopPreview)
	ON_BN_CLICKED(IDC_BTN_GET_FORMAT, &CVideoPreviewDlg::OnBnClickedBtnGetFormat)
	ON_BN_CLICKED(IDC_BTN_SET_FORMAT, &CVideoPreviewDlg::OnBnClickedBtnSetFormat)
	ON_BN_CLICKED(IDC_BTN_PROPERTY_SET, &CVideoPreviewDlg::OnBnClickedBtnPropertySet)
	ON_BN_CLICKED(IDC_BTN_CONTROL_SET, &CVideoPreviewDlg::OnBnClickedBtnControlSet)
	ON_CBN_SELCHANGE(IDC_COMBO_PROPERTY, &CVideoPreviewDlg::OnCbnSelchangeComboProperty)
	ON_CBN_SELCHANGE(IDC_COMBO_CONTROL, &CVideoPreviewDlg::OnCbnSelchangeComboControl)
	ON_LBN_DBLCLK(IDC_LIST_CAMERA_LIST, &CVideoPreviewDlg::OnLbnDblclkListDevice)
	ON_LBN_SELCHANGE(IDC_LIST_CAMERA_LIST, &CVideoPreviewDlg::OnLbnDblclkListDevice)
	ON_BN_CLICKED(IDC_BTN_OPEN_CAMERA, &CVideoPreviewDlg::OnBnClickedBtnOpenCamera)
	ON_BN_CLICKED(IDC_BTN_CLOSE_CAMERA, &CVideoPreviewDlg::OnBnClickedBtnCloseCamera)
	ON_CBN_SELCHANGE(IDC_COMBO_PROPERTY, &CVideoPreviewDlg::OnCbnSelchangeProperty)
	ON_CBN_SELCHANGE(IDC_COMBO_CONTROL, &CVideoPreviewDlg::OnCbnSelchangeControl)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CVideoPreviewDlg 消息处理程序


int CALLBACK SampleGrabFun(BYTE* pData, LONG BufferLength)
{
	TRACE("pThread1 %d\n", BufferLength);

	/*if (m_bCaptureBuffer)
	{
		CString FileName;
		FileName.Format(L"%d.jpg", iBufferCount++);
		m_bCaptureBuffer = false;
		CFile SaveFile;
		SaveFile.Open(FileName, CFile::modeWrite | CFile::modeCreate, NULL);
		SaveFile.Write(pData, BufferLength);
		SaveFile.Close();
	}*/
	return 0;
}

BOOL CVideoPreviewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	if (NOERROR != SonixCam_Init())
	{
		MessageBox(L"Can't find video device!", L"ERROR", 0);
		EndDialog(-1);
		return FALSE;
	};

	if (!getEnumDevice())
	{
		MessageBox(L"Can't find video device!", L"ERROR", 0);
		EndDialog(-1);
		return FALSE;
	}
	SonixCam_SelectDevice(m_CurSelectDevice);

	CString tempStr;
	tempStr.Format(L"CameraCount: %d \r\n", cameraCount);
	m_StatusOutput += tempStr;
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


BOOL CVideoPreviewDlg::getEnumDevice()
{
	LONG devNum = 0;
	for (int i = 0; i < 1; i++)
	{
		if (NOERROR == SonixCam_EnumDevice(m_ci, devNum, 5))
			break;
	}

	m_CameraList.ResetContent();
	for (LONG i = 0; i < devNum; i++)
	{
		m_CameraList.InsertString(i, m_ci[i].deviceName);
	}

	if ((m_CurSelectDevice + 1) > devNum)
		m_CurSelectDevice = 0;

	CString str;
	if (devNum)
	{
		m_CameraList.GetText(m_CurSelectDevice, str);
		m_CameraList.SetCurSel(m_CurSelectDevice);
		OnLbnDblclkListDevice();
	}
	cameraCount = devNum;
	UpdateData(TRUE);
	return TRUE;
}


void CVideoPreviewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoPreviewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoPreviewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoPreviewDlg::OnBnClickedBtnStartPreview()
{
	// TODO:  在此添加控件通知处理程序代码
	if (NOERROR != SonixCam_StartPreview())
		m_StatusOutput += "Start preview failed!\r\n";
	else
		m_StatusOutput += "Start preview success!\r\n";

	//Sleep(1000);
	BYTE data[2] = { 0 };
	HRESULT hr = SonixCam_AsicRegisterRead(0x1003, data, 1);
	data[0] &= 0xFE;
	hr = SonixCam_AsicRegisterWrite(0x101f, data, 1);

	//hr = SonixCam_SensorRegisterCustomRead(0x36, 0x380c, data, 1);
	//data[0]++;
	//hr = SonixCam_SensorRegisterCustomWrite(0x36, 0x380c, data, 1);

	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}


void CVideoPreviewDlg::OnBnClickedBtnStopPreview()
{
	// TODO:  在此添加控件通知处理程序代码
	if (NOERROR != SonixCam_StopPreview())
		m_StatusOutput += "Stop preview failed!\r\n";
	else
		m_StatusOutput += "Stop preview success!\r\n";
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
	Sleep(100);
	GetDlgItem(IDC_VIDEO_PREVIEW)->Invalidate();
}

void CVideoPreviewDlg::OnBnClickedBtnGetFormat()
{
	// TODO:  在此添加控件通知处理程序代码

	m_bCaptureBuffer = TRUE;

	long count = 0;
	SonixCam_GetPreviewResolutionCount(&count);
	VideoOutFormat outFormat;
	CString tempStr;
	for (int i = 0; i < count; i++)
	{
		SonixCam_GetPreviewResolutionInfo(i, &outFormat);
		switch (outFormat.vcs)
		{
		case VCS_Unknow:
			tempStr.Format(L"No.%d: Color:Unknow,Solution:%d*%d \r\n", i, outFormat.width, outFormat.height);
			break;
		case VCS_H264:
			tempStr.Format(L"No.%d: Color:H264,Solution:%d*%d \r\n", i, outFormat.width, outFormat.height);
			break;
		case VCS_Mjpg:
			tempStr.Format(L"No.%d: Color:MJPG,Solution:%d*%d \r\n", i, outFormat.width, outFormat.height);
			break;
		case VCS_Yuy2:
			tempStr.Format(L"No.%d: Color:YUY2,Solution:%d*%d \r\n", i, outFormat.width, outFormat.height);
			break;
		case VCS_Y8:
			tempStr.Format(L"No.%d: Color:Y8,Solution:%d*%d \r\n", i, outFormat.width, outFormat.height);
			break;
		}
		m_StatusOutput += tempStr;
	}
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}


void CVideoPreviewDlg::OnBnClickedBtnSetFormat()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (NOERROR != SonixCam_SetVideoFormat(0, m_iPreviewFormat))
		m_StatusOutput += L"Set video format failed!\r\n";
	else
		m_StatusOutput += L"Set video format success!\r\n";
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}



void CVideoPreviewDlg::OnBnClickedBtnPropertySet()
{
	// TODO:  在此添加控件通知处理程序代码

	/*if (NOERROR != SonixCam_PropertySet(0, m_iPreviewFormat))
		m_StatusOutput += L"Set video format failed!\r\n";
	else
		m_StatusOutput += L"Set video format success!\r\n";*/

	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}


void CVideoPreviewDlg::OnBnClickedBtnControlSet()
{
	// TODO:  在此添加控件通知处理程序代码


}


void CVideoPreviewDlg::OnCbnSelchangeComboProperty()
{
	// TODO:  在此添加控件通知处理程序代码
	LONG  iMin, iMax, iStep, iDefault, iFlag = 0;
	CString strTemp;
	int iIndex = m_PropertyCB.GetCurSel();
	HRESULT hr = SonixCam_PropertyGetRange((CameraProperty)iIndex, &iMin, &iMax, &iStep, &iDefault, (PropertyFlags*)&iFlag);
	if (hr != NOERROR)
		m_StatusOutput += L"Negative!\r\n";
	else
		strTemp.Format(L"Property:%d: Min:%d Max:%d Step:%d Default:%d Flag:%d \r\n", iIndex, iMin, iMax, iStep, iDefault, iFlag);

	//hr = SonixCam_PropertyGet((CameraProperty)iIndex, (LONG*)&m_iControl, (DS_CONTROL_FLAGS*)&m_iControlAuto);
	//if (2 == m_iControlAuto)
	//	m_iControlAuto = 0;
	//else
	//	m_iControlAuto = 1;
	m_StatusOutput += strTemp;
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}


void CVideoPreviewDlg::OnCbnSelchangeComboControl()
{
	// TODO:  在此添加控件通知处理程序代码
	LONG  iMin, iMax, iStep, iDefault, iFlag = 0;
	CString strTemp;
	int iIndex = m_ControlCB.GetCurSel();
	HRESULT hr = SonixCam_ControlGetRange((CameraControl)iIndex, &iMin, &iMax, &iStep, &iDefault, (PropertyFlags*)&iFlag);
	if (hr != NOERROR)
		m_StatusOutput += L"Negative!\r\n";
	else
		strTemp.Format(L"Control:%d: Min:%d Max:%d Step:%d Default:%d Flag:%d \r\n", iIndex, iMin, iMax, iStep, iDefault, iFlag);

	m_StatusOutput += strTemp;
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}

void CVideoPreviewDlg::OnLbnDblclkListDevice()
{
	// TODO: 在此添加控件通知处理程序代码
	LONG lastSelectDevice = m_CurSelectDevice;
	m_CurSelectDevice = m_CameraList.GetCurSel();

	bool bOpening = FALSE;
	SonixCam_IsOpenCamera(bOpening);
	if (bOpening) 
		SonixCam_CloseCamera();

	CString tempStr;
	if (NOERROR != SonixCam_SelectDevice(m_CurSelectDevice))
		tempStr.Format(L"Select device: %d failed!\r\n", m_CurSelectDevice);
	else
		tempStr.Format(L"Select device: %d success!\r\n", m_CurSelectDevice);
	m_StatusOutput += tempStr;
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}

void CVideoPreviewDlg::OnBnClickedBtnOpenCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	LONG lastSelectDevice = m_CurSelectDevice;
	m_CurSelectDevice = m_CameraList.GetCurSel();

	if (SonixCam_IsOpenCamera)
		SonixCam_CloseCamera();

	if (NOERROR != SonixCam_OpenCamera(GetDlgItem(IDC_VIDEO_PREVIEW)->m_hWnd, TRUE, SampleGrabFun))
		m_StatusOutput += "Open camera failed!\r\n";
	else
		m_StatusOutput += "Open camera success!\r\n";
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}


void CVideoPreviewDlg::OnBnClickedBtnCloseCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	bool opening = FALSE;
	SonixCam_IsOpenCamera(opening);
	if (!opening) return;

	bool previewing = FALSE;
	SonixCam_IsPreviewing(opening);
	if (previewing) 
		SonixCam_StopPreview();

	if (NOERROR != SonixCam_CloseCamera())
		m_StatusOutput += "Close camera failed!\r\n";
	else
		m_StatusOutput += "Close camera success!\r\n";
	UpdateData(FALSE);
	m_StatusShow.LineScroll(m_StatusShow.GetLineCount());
}

void CVideoPreviewDlg::OnCbnSelchangeProperty()
{
	// TODO:  在此添加控件通知处理程序代码


}


void CVideoPreviewDlg::OnCbnSelchangeControl()
{
	// TODO:  在此添加控件通知处理程序代码

	
}



void CVideoPreviewDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	SonixCam_UnInit();

	CDialogEx::OnClose();
}
