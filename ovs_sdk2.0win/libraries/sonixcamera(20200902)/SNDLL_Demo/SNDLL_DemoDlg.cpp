
// SNDLL_DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SNDLL_Demo.h"
#include "SNDLL_DemoDlg.h"
#include "afxdialogex.h"
#include "../SonixCamera/SonixCamera.h"

#include <dshow.h>
#include <ks.h>
#include <ksproxy.h>
#include <ksmedia.h>
#include <vidcap.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


wchar_t ERROR_CODE_ARRAY[][50] =
{
	L"EC_UnKnow",
	L"EC_EnumDeviceFail",
	L"EC_DisableFlashWriteProtectFail",
	L"EC_EraseFlashFail",
	L"EC_EraseFlashSectorFail",
	L"EC_GetAsicIdFail",
	L"EC_GetAsicRomVersionFail",
	L"EC_GetAsicRomTypeFail",
	L"EC_ReadAsicRegisterFail",
	L"EC_WriteAsicRegisterFail",
	L"EC_UnKnowSerialFlashType",
	L"EC_BurnerCheckFail",
	L"EC_CoInitializeFail",
	L"EC_NoFindDevice",
	L"EC_MallocMemoryFail",
};


wchar_t SERIAL_FLASH_TYPE_ARRAY[][50] =
{
	L"SFT_UNKNOW",
	L"SFT_MXIC",
	L"SFT_ST",
	L"SFT_SST",
	L"SFT_ATMEL_AT25F",
	L"SFT_ATMEL_AT25FS",
	L"SFT_ATMEL_AT45DB",
	L"SFT_WINBOND",
	L"SFT_PMC",
	L"SFT_MXIC_LIKE",
	L"SFT_AMIC",
	L"SFT_EON",
	L"SF_ESMT",
	L"SFT_GIGA",
	L"SFT_FENTECH"
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

static BYTE ubPROF_Param[6][1024] = { {},{} };

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


// CSNDLL_DemoDlg 对话框



CSNDLL_DemoDlg::CSNDLL_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSNDLL_DemoDlg::IDD, pParent)
	, m_editFwVersion(_T(""))
	, m_editVidPid(_T(""))
	, m_editErrorCode(_T(""))
	, m_editSFType(_T(""))
	, m_editManufacturer(_T(""))
	, m_editProduct(_T(""))
	, m_editSerialNumber(_T(""))
	, m_editString3(_T(""))
	, m_editNodeId(_T(""))
	,m_editLength(_T(""))
	,m_editCS(_T(""))
	, m_editXuOutput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFwBuf = NULL;
	m_lFwBufLen = 0;
}

void CSNDLL_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ASIC_ADDR, m_asicAddr);
	DDX_Text(pDX, IDC_EDIT_ASIC_LENGTH, m_asicLength);
	DDX_Text(pDX, IDC_EDIT_SENSOR_SLAVEID, m_sensorSlaveID);
	DDX_Text(pDX, IDC_EDIT_SENSOR_ADDR, m_sensorAddr);
	DDX_Text(pDX, IDC_EDIT_SENSOR_LENGTH, m_sensorLength);
	DDX_Text(pDX, IDC_EDIT_SF_DATA_LEN, m_sfDataLen);
	DDX_Text(pDX, IDC_EDIT_SF_ADDR, m_sfAddr);
	DDX_Text(pDX, IDC_EDIT_OUTPUT2, m_editOutput);
	DDX_Text(pDX, IDC_EDIT_FW_VERSION, m_editFwVersion);
	DDX_Text(pDX, IDC_EDIT_VID_PID, m_editVidPid);
	DDX_Text(pDX, IDC_EDIT_ERROR_CODE, m_editErrorCode);
	DDX_Text(pDX, IDC_EDIT_SF_TYPE, m_editSFType);
	DDX_Text(pDX, IDC_EDIT_GET_MANUFACTURER, m_editManufacturer);
	DDX_Text(pDX, IDC_EDIT_GET_PRODUCT, m_editProduct);
	DDX_Text(pDX, IDC_EDIT_GET_SERIAL_NUMBER, m_editSerialNumber);
	DDX_Text(pDX, IDC_EDIT_GET_STRING3, m_editString3);
	DDX_Text(pDX, IDC_EDIT_XU_NODEID, m_editNodeId);
	DDX_Text(pDX, IDC_EDIT_XU_LENGTH, m_editLength);
	DDX_Text(pDX, IDC_EDIT_XU_CS, m_editCS);
	DDX_Text(pDX, IDC_EDIT_XU_OUTPUT, m_editXuOutput);
	DDX_Text(pDX, IDC_EDIT_GET_INTERFACE, m_editInterface);
}

BEGIN_MESSAGE_MAP(CSNDLL_DemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ASIC_REGISTER_READ, &CSNDLL_DemoDlg::OnBnClickedBtnAsicRegisterRead)
	ON_BN_CLICKED(IDC_BTN_ASIC_REGISTER_WRITE, &CSNDLL_DemoDlg::OnBnClickedBtnAsicRegisterWrite)
	ON_BN_CLICKED(IDC_BTN_SENSOR_REGISTER_READ, &CSNDLL_DemoDlg::OnBnClickedBtnSensorRegisterRead)
	ON_BN_CLICKED(IDC_BTN_SENSOR_REGISTER_WRITE, &CSNDLL_DemoDlg::OnBnClickedBtnSensorRegisterWrite)
	ON_BN_CLICKED(IDC_BTN_FLASH_READ, &CSNDLL_DemoDlg::OnBnClickedBtnFlashRead)
	ON_BN_CLICKED(IDC_BTN_FLASH_WRITE, &CSNDLL_DemoDlg::OnBnClickedBtnFlashWrite)
	ON_BN_CLICKED(IDC_BTN_BURNER_FW, &CSNDLL_DemoDlg::OnBnClickedBtnBurnerFW)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_GET_FW_VERSION, &CSNDLL_DemoDlg::OnBnClickedBtnGetFwVersion)
	ON_BN_CLICKED(IDC_BTN_GET_VID_PID, &CSNDLL_DemoDlg::OnBnClickedBtnGetVidPid)
	ON_BN_CLICKED(IDC_BTN_GET_ERROR_CODE, &CSNDLL_DemoDlg::OnBnClickedBtnGetErrorCode)
	ON_BN_CLICKED(IDC_BTN_GET_SF_TYPE, &CSNDLL_DemoDlg::OnBnClickedBtnGetSfType)
	ON_BN_CLICKED(IDC_BTN_GET_MANUFACTURER, &CSNDLL_DemoDlg::OnBnClickedBtnGetManufacturer)
	ON_BN_CLICKED(IDC_BTN_GET_PRODUCT, &CSNDLL_DemoDlg::OnBnClickedBtnGetProduct)
	ON_BN_CLICKED(IDC_BTN_GET_SERIAL_NUMBER, &CSNDLL_DemoDlg::OnBnClickedBtnGetSerialNumber)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_GET_STRING3, &CSNDLL_DemoDlg::OnBnClickedBtnGetString3)
	ON_BN_CLICKED(IDC_BTN_FLASH_CUSTOM_READ, &CSNDLL_DemoDlg::OnBnClickedBtnFlashCustomRead)
	ON_BN_CLICKED(IDC_BTN_FLASH_CUSTOM_WRITE, &CSNDLL_DemoDlg::OnBnClickedBtnFlashCustomWrite)
	ON_BN_CLICKED(IDC_BTN_MD, &CSNDLL_DemoDlg::OnBnClickedBtnMd)
	ON_BN_CLICKED(IDC_BTN_XU_READ, &CSNDLL_DemoDlg::OnBnClickedBtnXuRead)
	ON_BN_CLICKED(IDC_BTN_XU_WRITE, &CSNDLL_DemoDlg::OnBnClickedBtnXuWrite)
	ON_BN_CLICKED(IDC_BTN_GET_NODEID, &CSNDLL_DemoDlg::OnBnClickedBtnGetNodeid)
	ON_BN_CLICKED(IDC_BTN_GET_INTERFACE, &CSNDLL_DemoDlg::OnBnClickedBtnGetInterface)
END_MESSAGE_MAP()


// CSNDLL_DemoDlg 消息处理程序
BOOL CSNDLL_DemoDlg::OnInitDialog()
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
	//if (NOERROR != SonixCam_Init("2bc50621"))
	if (NOERROR != SonixCam_Init())
	{
		AfxMessageBox(L"Can't find video device!");
		EndDialog(-1);
		return TRUE;
	};

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSNDLL_DemoDlg::OnBnClickedBtnMd()
{
	// TODO:  在此添加控件通知处理程序代码

	BYTE data[100] = { 0 };
	LONG id = 0;
	LONG nodeId = 2;


	UpdateData(TRUE);

	SonixCam_GetNodeId(nodeId, id);
	ZeroMemory(data, sizeof(data));
	data[0] = 0x9a;
	data[1] = 0x03;
	//HRESULT hr = SonixCam_XuWrite(24, data, 5, 2);

	ZeroMemory(data, sizeof(data));
	for (int i = 0; i < 16; i++)
		data[i] = (i + 1) % 16;
	//hr = SonixCam_XuWrite(24, data, 5, 2);

	ZeroMemory(data, sizeof(data));
	//hr = SonixCam_XuRead(24, data, 5, 2);

	m_editXuOutput = "";
	for (LONG i = 0; i < 24; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
		{
			value.Format(L"\r\n%02X ", data[i]);
		}
		else
		{
			value.Format(L"%02X ", data[i]);
		}
		m_editXuOutput += value;
	}
	UpdateData(FALSE);
	return;

	UpdateData(TRUE);
	//hr = SonixCam_XuRead(0x16, data, 5, nodeId);
	m_editOutput = "";
	BYTE arr[100] = { 0 };
	CopyMemory(arr, data, 100);
	for (LONG i = 0; i < 0x16; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
		{
			value.Format(L"\r\n%02X ", data[i]);
		}
		else
		{
			value.Format(L"%02X ", data[i]);
		}
		m_editOutput += value;
	}
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSNDLL_DemoDlg::OnPaint()
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
HCURSOR CSNDLL_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSNDLL_DemoDlg::OnBnClickedBtnAsicRegisterRead()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_editOutput = "";
	LONG asicAddr = _tcstoul(m_asicAddr, NULL, 16);
	LONG length = _tcstoul(m_asicLength, NULL, 16);
	if (length <= 0){
		return;
	}

	BYTE *pData = new BYTE[length];
	ZeroMemory(pData, length);
	if (NOERROR != SonixCam_AsicRegisterRead(asicAddr, pData, length))
	{
		AfxMessageBox(L"读取ASIC寄存器失败！");
		SAFE_DELETE_ARRAY(pData);
		return;
	}
	
	for (LONG i = 0; i < length; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
			value.Format(L"\r\n%02X ", pData[i]);
		else
			value.Format(L"%02X ", pData[i]);
		m_editOutput += value;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnAsicRegisterWrite()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LONG asicAddr = _tcstoul(m_asicAddr, NULL, 16);
	BYTE length = _tcstoul(m_asicLength, NULL, 16);

	if (length <= 0)
		return;

	BYTE *pData = new BYTE[length];
	memset(pData, 0xff, length);
	LONG sIndex = -1;
	CString sTemp = m_editOutput;
	LONG srcLength = m_editOutput.GetLength();
	BYTE add = 0;
	LONG i = 0;
	wchar_t temp[2] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20){
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2){
			add = 0;
			pData[i++] = _tcstoull(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);
	if (add)
		pData[i] = _tcstoul(temp, NULL, 16);
	if (NOERROR != SonixCam_AsicRegisterWrite(asicAddr, pData, length))
	{
		AfxMessageBox(L"写ASIC寄存器失败！");
		SAFE_DELETE_ARRAY(pData);
		return;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnSensorRegisterRead()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	USHORT sensorSlaveID = _tcstoul(m_sensorSlaveID, NULL, 16);
	USHORT sensorAddr = _tcstoul(m_sensorAddr, NULL, 16);
	USHORT length = _tcstoul(m_sensorLength, NULL, 16);

	m_editOutput = "";
	BYTE *pData = new BYTE[length];
	//if (NOERROR != SonixCam_SensorRegisterRead(sensorSlaveID, sensorAddr, pData, length))
	if (NOERROR != SonixCam_SensorRegisterCustomRead(sensorSlaveID, sensorAddr, pData, length))
	{
		SAFE_DELETE_ARRAY(pData);
		AfxMessageBox(L"读取sensor寄存器失败！");
		return;
	}

	for (LONG i = 0; i < length; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
			value.Format(L"\r\n%02X ", pData[i]);
		else
			value.Format(L"%02X ", pData[i]);
		m_editOutput += value;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnSensorRegisterWrite()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	USHORT sensorSlaveID = _tcstoul(m_sensorSlaveID, NULL, 16);
	USHORT sensorAddr = _tcstoul(m_sensorAddr, NULL, 16);
	BYTE length = _tcstoul(m_sensorLength, NULL, 16);

	BYTE *pData = new BYTE[length];
	memset(pData, 0xff, length);
	LONG sIndex = -1;
	CString sTemp = m_editOutput;
	LONG srcLength = m_editOutput.GetLength();
	BYTE add = 0;
	LONG i = 0;
	wchar_t temp[2] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20){
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2){
			add = 0;
			pData[i++] = _tcstoul(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);
	if (add)
		pData[i] = _tcstoul(temp, NULL, 16);

	if (NOERROR != SonixCam_SensorRegisterCustomWrite(sensorSlaveID, sensorAddr, pData, length))
	{
		SAFE_DELETE_ARRAY(pData);
		AfxMessageBox(L"写入sensor寄存器失败！");
		return;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnFlashRead()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
	LONG addr = _tcstoul(m_sfAddr, NULL, 16);
	LONG len = _tcstoul(m_sfDataLen, NULL, 16);
	BYTE *pData = new BYTE[len];
	if (NOERROR != SonixCam_SerialFlashRead(addr, pData, len))
	{
		AfxMessageBox(L"读FLASH失败！");
		return;
	}

	//GetDlgItem(IDC_EDIT_OUTPUT)->SetFont(m_EditFont);
	m_editOutput = "";
	BYTE arr[100] = { 0 };
	CopyMemory(arr, pData, 100);
	for (LONG i = 0; i < len; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
		{
			value.Format(L"\r\n%02X ", pData[i]);
		}
		else
		{
			value.Format(L"%02X ", pData[i]);
		}
		m_editOutput += value;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}

#define DE_TEST_NUM 0x50
#define DE_TEST_ADDR 0x0F0000

void CSNDLL_DemoDlg::OnBnClickedBtnFlashWrite()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
	if (m_sfAddr.IsEmpty())
		return;
	LONG addr = _tcstoul(m_sfAddr, NULL, 16);
	LONG len = _tcstoul(m_sfDataLen, NULL, 16);
	SERIAL_FLASH_TYPE sfType = SFT_UNKNOW;
	SonixCam_GetSerialFlashType(sfType, true);
	if (sfType == SFT_UNKNOW)
	{
		sfType = SFT_MXIC;
		//当库里面没有找到flash的型号时，必须对sfType赋值，给定一个型号。
		//AfxMessageBox("当库里面没有找到flash的型号时，必须对sfType赋值，给定一个型号", "错误！", FALSE);
	}

	LONG sIndex = -1;
	CString sTemp = m_editOutput;
	BYTE *pData = new BYTE[sTemp.GetLength()];
	memset(pData, 0xff, sTemp.GetLength());
	LONG srcLength = m_editOutput.GetLength();

	BYTE add = 0;
	LONG i = 0;
	wchar_t temp[2] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20){
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2){
			add = 0;
			pData[i++] = _tcstoul(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);
	if (add)
		pData[i] = _tcstoul(temp, NULL, 16);

	if (NOERROR != SonixCam_SerialFlashSectorWrite(addr, pData, len, sfType))
	{
		SAFE_DELETE_ARRAY(pData);
		AfxMessageBox(L"写FLASH失败！");
		return;
	}
	SAFE_DELETE_ARRAY(pData);
	AfxMessageBox(L"写FLASH完成！");
}

void CSNDLL_DemoDlg::OnBnClickedBtnFlashCustomRead()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LONG addr = _tcstoul(m_sfAddr, NULL, 16);
	LONG len = _tcstoul(m_sfDataLen, NULL, 16);
	BYTE *pData = new BYTE[len];
	if (!pData){
		AfxMessageBox(L"Malloc Fail!");
		return;
	}
	if (NOERROR != SonixCam_SerialFlashCustomRead(addr, pData, len))
	{
		AfxMessageBox(L"读FLASH失败！");
		return;
	}

	m_editOutput = "";
	BYTE arr[100] = { 0 };
	CopyMemory(arr, pData, 100);
	for (LONG i = 0; i < len; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
		{
			value.Format(L"\r\n%02X ", pData[i]);
		}
		else
		{
			value.Format(L"%02X ", pData[i]);
		}
		m_editOutput += value;
	}
	SAFE_DELETE_ARRAY(pData);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnFlashCustomWrite()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_sfAddr.IsEmpty())
		return;
	LONG addr = _tcstoul(m_sfAddr, NULL, 16);
	LONG len = _tcstoul(m_sfDataLen, NULL, 16);
	SERIAL_FLASH_TYPE sfType = SFT_UNKNOW;
	SonixCam_GetSerialFlashType(sfType, true);
	if (sfType == SFT_UNKNOW)
	{
		sfType = SFT_MXIC;
		//当库里面没有找到flash的型号时，必须对sfType赋值，给定一个型号。
		//AfxMessageBox("当库里面没有找到flash的型号时，必须对sfType赋值，给定一个型号", "错误！", FALSE);
	}

	CString sTemp = m_editOutput;
	BYTE *pData = new BYTE[sTemp.GetLength()];
	memset(pData, 0xff, sTemp.GetLength());
	LONG sIndex = -1;
	LONG srcLength = m_editOutput.GetLength();

	BYTE add = 0;
	LONG i = 0;
	wchar_t temp[2] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20){
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2){
			add = 0;
			pData[i++] = _tcstoul(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);
	if (add)
		pData[i] = _tcstoul(temp, NULL, 16);

	if (NOERROR != SonixCam_SerialFlashSectorCustomWrite(addr, pData, len, sfType))
	{
		SAFE_DELETE_ARRAY(pData);
		AfxMessageBox(L"写FLASH失败！");
		return;
	}
	SAFE_DELETE_ARRAY(pData);
	AfxMessageBox(L"写FLASH完成！");
}


BOOL CSNDLL_DemoDlg::LoadFW(const CString &fwPath)
{
	CFile file;
	BYTE *pFW = NULL;
	if (!file.Open(fwPath, CFile::modeRead | CFile::typeBinary, NULL))	// 打开文件
	{
		AfxMessageBox(_T("Can't Open File！"));
		return FALSE;
	}
	ULONGLONG length = file.GetLength();	// 获取文件长度
	if (length != 0x10000 && length != 0x20000 && length != 0x8000){
		AfxMessageBox(_T("不是标准的固件文件！"));
		goto LoadFail;
	}
	pFW = new BYTE[length];
	if (!pFW){
		AfxMessageBox(_T("没有足够的内存可供加载固件!"));
		goto LoadFail;
	}
	FillMemory(pFW, 0xFF, length);
	file.Read(pFW, length);
	file.Close();
	if (*pFW == 0xff && *(pFW + 1) == 0xff){
		AfxMessageBox(_T("It isn't Sonix Rom File!"));
		goto LoadFail;
	}
	if (*pFW != 0x53 && *(pFW + 1) != 0x4E &&
		*(pFW + 2) != 0x39 && *(pFW + 3) != 0x43)
	{
		AfxMessageBox(_T("It isn't Sonix Rom File!"));
		goto LoadFail;
	}
	SAFE_DELETE_ARRAY(m_pFwBuf);
	m_pFwBuf = new BYTE[length];
	if (!m_pFwBuf){
		AfxMessageBox(_T("没有足够的内存可供加载固件!"));
		goto LoadFail;
	}
	CopyMemory(m_pFwBuf, pFW, length);
	m_lFwBufLen = length;
	SAFE_DELETE_ARRAY(pFW);
	return TRUE;
LoadFail:
	SAFE_DELETE_ARRAY(pFW);
	return FALSE;
}

void CSNDLL_DemoDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	TCHAR tempPath[MAX_PATH];
	CString filePath;
	int hr = DragQueryFile(hDropInfo, 0, tempPath, sizeof(tempPath));
	if (hr <= 0){
		return;
	}
	DragFinish(hDropInfo);
	CString fwPath = tempPath;
	if (LoadFW(fwPath)){
		GetDlgItem(IDC_EDIT_FW_PATH)->SetWindowText(fwPath);
	}

	CDialogEx::OnDropFiles(hDropInfo);
}

void SetProgress(void *ptrClass, float fProcess)
{
	//通过发送自定义的Messagebox在主线程中更新界面更好一些，下面代码没有在主线程中更新。
	CSNDLL_DemoDlg *pDlg = (CSNDLL_DemoDlg*)ptrClass;
	CProgressCtrl *pProgress = (CProgressCtrl*)pDlg->GetDlgItem(IDC_PROGRESS_CLASS);
	pProgress->SetPos(fProcess * 10000);
	//pDlg->UpdateData(FALSE);
}

void BurnerThread(LPVOID lpParameter)
{
	CSNDLL_DemoDlg *pDlg = static_cast<CSNDLL_DemoDlg*>(lpParameter);
	pDlg->BurnerFW();
}

void CSNDLL_DemoDlg::OnBnClickedBtnBurnerFW()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_pFwBuf)
	{
		AfxMessageBox(_T("没有加载固件!\n请将固件拖入到对话框内。"));
		return;
	}

	_beginthread(BurnerThread, 0, this);
}

BOOL CSNDLL_DemoDlg::BurnerFW()
{
	GetDlgItem(IDC_BTN_BURNER_FW)->EnableWindow(FALSE);
	SERIAL_FLASH_TYPE sfType = SFT_UNKNOW;
	SonixCam_GetSerialFlashType(sfType, true);
	if (sfType == SFT_UNKNOW)
	{
		//sfType = SFT_XXXX
		//当库里面没有找到flash的型号时，必须对sfType赋值，给定一个型号。
		sfType = SFT_MXIC;
	}

	CProgressCtrl *pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_CLASS);
	pProgress->SetRange(0, 10000);
	pProgress->SetPos(0);

	//HRESULT hr = SonixCam_DisableSerialFlashWriteProtect(sfType);
	//hr = SonixCam_EraseBlockFlash(0, sfType); //一般Flash的block大小是64k
	//Sleep(1000);
	//hr = SonixCam_EraseBlockFlash(0x10000, sfType);
	//Sleep(1000);
	//if (NOERROR != SonixCam_WriteFwToFlash(m_pFwBuf, m_lFwBufLen, SetProgress, this, FALSE))
	
	if (NOERROR != SonixCam_BurnerFW(m_pFwBuf, m_lFwBufLen, SetProgress, this, sfType, FALSE))
	{
		AfxMessageBox(_T("烧录固件失败!"));
		return FALSE;
	}


	//烧录完成后可以重启设备
	//SonixCam_RestartDevice();

	GetDlgItem(IDC_BTN_BURNER_FW)->EnableWindow(TRUE);
	AfxMessageBox(_T("烧录固件完成!"));
	return TRUE;
}

void CSNDLL_DemoDlg::OnBnClickedBtnGetFwVersion()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE fwVersion[60];
	ZeroMemory(fwVersion, 60);

	DWORD curTime = GetTickCount();
	if (NOERROR != SonixCam_GetFWVersion(fwVersion, 60))
	{
		AfxMessageBox(_T("获取固件版本失败!"));
		return;
	}
	DWORD tickCount = GetTickCount() - curTime;

	m_editFwVersion = fwVersion;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetVidPid()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE vidPid[10];
	ZeroMemory(vidPid, 10);
	if (NOERROR != SonixCam_GetVidPid(vidPid, 10))
	{
		AfxMessageBox(_T("获取VidPid失败!"));
		return;
	}
	CString sVidPid, szTemp;
	for (int i = 0; i<4; i++)
	{
		szTemp.Format(_T("%02x"), vidPid[i]);
		sVidPid += szTemp;
	}

	m_editVidPid = sVidPid;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetErrorCode()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE vidPid[10];
	ZeroMemory(vidPid, 10);
	ERROR_CODE ec;
	SonixCam_GetErrorCode(ec);
	CString sec = ERROR_CODE_ARRAY[ec];
	m_editErrorCode = sec;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetSfType()
{
	// TODO:  在此添加控件通知处理程序代码
	SERIAL_FLASH_TYPE sft;
	if (NOERROR != SonixCam_GetSerialFlashType(sft, true))
	{
		AfxMessageBox(_T("获取Flash类型失败!"));
		return;
	}
	CString csft = SERIAL_FLASH_TYPE_ARRAY[sft];
	m_editSFType = csft;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetManufacturer()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE manufacturerBuf[100];
	ZeroMemory(manufacturerBuf, sizeof(manufacturerBuf));
	if (NOERROR != SonixCam_GetManufacturer(manufacturerBuf, sizeof(manufacturerBuf)))
	{
		AfxMessageBox(_T("获取Manufacturer失败!"));
		return;
	}
	m_editManufacturer = manufacturerBuf;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetProduct()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE productBuf[100];
	ZeroMemory(productBuf, sizeof(productBuf));
	if (NOERROR != SonixCam_GetProduct(productBuf, sizeof(productBuf)))
	{
		AfxMessageBox(_T("获取Product失败!"));
		return;
	}
	m_editProduct = productBuf;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetSerialNumber()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE serialNumberBuf[100];
	ZeroMemory(serialNumberBuf, sizeof(serialNumberBuf));
	if (NOERROR != SonixCam_GetSerialNumber(serialNumberBuf, sizeof(serialNumberBuf)))
	{
		AfxMessageBox(_T("获取SerialNumber失败!"));
		return;
	}
	m_editSerialNumber = serialNumberBuf;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetString3()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE string3Buf[100];
	ZeroMemory(string3Buf, sizeof(string3Buf));
	if (NOERROR != SonixCam_GetString3(string3Buf, sizeof(string3Buf)))
	{
		AfxMessageBox(_T("获取String3失败!"));
		return;
	}
	m_editString3 = string3Buf;
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//AfxAfxMessageBox("OnClose():: call SonixCam_UnInit");
	SonixCam_UnInit();
	CDialogEx::OnClose();
}



BOOL CSNDLL_DemoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;

		case VK_ESCAPE:
			return TRUE;

		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



void CSNDLL_DemoDlg::OnBnClickedBtnXuRead()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LONG length = _tcstoul(m_editLength, NULL, 16);
	if (length <= 0)
	{
		AfxMessageBox(L"Length不能小于0");
		return;
	}
	LONG cs = _tcstoul(m_editCS, NULL, 16);
	if (cs < 0)
	{
		AfxMessageBox(L"CS不能小于0");
		return;
	}
	LONG nodeId = _tcstoul(m_editNodeId, NULL, 16);
	if (nodeId < 0)
	{
		AfxMessageBox(L"NodeId不能小于0");
		return;
	}

	m_editXuOutput = "";
	BYTE *data = new BYTE[length];
	ZeroMemory(data, sizeof(data));
	SonixCam_XuRead(data, length, cs, nodeId);
	for (LONG i = 0; i < length; i++)
	{
		CString value;
		if ((i % 16) == 0 && i != 0)
		{
			value.Format(L"\r\n%02X ", data[i]);
		}
		else
		{
			value.Format(L"%02X ", data[i]);
		}
		m_editXuOutput += value;
	}
	SAFE_DELETE_ARRAY(data);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnXuWrite()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LONG length = _tcstoul(m_editLength, NULL, 16);
	if (length <= 0)
	{
		AfxMessageBox(L"Length不能小于0");
		return;
	}
	LONG cs = _tcstoul(m_editCS, NULL, 16);
	if (length < 0)
	{
		AfxMessageBox(L"CS不能小于0");
		return;
	}
	LONG nodeId = _tcstoul(m_editNodeId, NULL, 16);
	if (nodeId < 0)
	{
		AfxMessageBox(L"NodeId不能小于0");
		return;
	}

	BYTE* data = new BYTE[length];
	LONG sIndex = -1;
	CString sTemp = m_editXuOutput;
	LONG srcLength = m_editXuOutput.GetLength();
	BYTE add = 0;
	LONG i = 0;
	wchar_t temp[2] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20){
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2){
			add = 0;
			data[i++] = _tcstoul(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);

	SonixCam_XuWrite(data, length, cs, nodeId);
	UpdateData(FALSE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetNodeid()
{
	// TODO:  在此添加控件通知处理程序代码
	LONG nodeId = 0;
	if (S_OK == (SonixCam_GetNodeId(nodeId, 0)))
	{
		CString str;
		str.Format(L"%x", nodeId);
		GetDlgItem(IDC_EDIT_XU_NODEID)->SetWindowText(str);
	}
	UpdateData(TRUE);
}


void CSNDLL_DemoDlg::OnBnClickedBtnGetInterface()
{
	// TODO:  在此添加控件通知处理程序代码
	BYTE interfaceBuf[100];
	ZeroMemory(interfaceBuf, sizeof(interfaceBuf));
	if (NOERROR != SonixCam_GetInterface(interfaceBuf, sizeof(interfaceBuf)))
	{
		AfxMessageBox(_T("获取String3失败!"));
		return;
	}
	m_editInterface = interfaceBuf;
	UpdateData(FALSE);
}

