
// MFCDeviceDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCDevice.h"
#include "MFCDeviceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDeviceDlg 对话框



CMFCDeviceDlg::CMFCDeviceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDEVICE_DIALOG, pParent)
	, b_device_status(true)
	, exp_value(_T(""))
	, gain_value(_T(""))
	, LED_lightness(5)
	, LED_value(_T(""))
	, b_save_image(0)
	, b_save_video(false)
	, cuser_ID(_T(""))
	, user_dynamic_number(0)
	, user_serial_number(0)
	, b_dynamic_collect(false)
	, distance_value("")
	, b_python_init(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	p_device_control = make_shared<DeviceControl>();
}

CMFCDeviceDlg::~CMFCDeviceDlg()
{
	b_device_status = false;
}

void CMFCDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPEN_DEVICE, open_device);
	DDX_Control(pDX, IDC_BUTTON_CLOSE_DEVICE, close_device);
	DDX_Control(pDX, IDC_BUTTON_EXP_DOWN, button_exp_down);
	DDX_Control(pDX, IDC_BUTTON_EXP_UP, button_exp_up);
	DDX_Control(pDX, IDC_SLIDER_EXP_VALUE, slider_exp_value);
	DDX_Control(pDX, IDC_EDIT_LOG, edit_log);
	DDX_Text(pDX, IDC_EDIT_EXP_VALUE, exp_value);
	DDX_Control(pDX, IDC_BUTTON_GAIN_VALUE_DOWN, button_gain_down);
	DDX_Control(pDX, IDC_BUTTON_GAIN_VALUE_UP, button_gain_up);
	DDX_Control(pDX, IDC_SLIDER_GAIN_VALUE, slider_gain_value);
	DDX_Text(pDX, IDC_EDIT_GAIN_VALUE, gain_value);
	DDX_Control(pDX, IDC_BUTTON_LED_VALUE_DOWN, button_LED_down);
	DDX_Control(pDX, IDC_BUTTON_LED_VALUE_UP, button_LED_up);
	DDX_Text(pDX, IDC_EDIT_LED, LED_value);
	DDX_Control(pDX, IDC_EDIT_DISTANCE_VALUE, edit_distance_value);
	DDX_Control(pDX, IDC_BUTTON_SAVE_IMAGE, button_save_image);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_STOP, button_video_stop);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_START, button_video_start);
	DDX_Control(pDX, IDC_EDIT_USER_ID, edit_user_ID);
	DDX_Text(pDX, IDC_EDIT_USER_ID, cuser_ID);
	DDX_Control(pDX, IDC_BUTTON_CHECK_USER_ID, button_check_user_ID);
	DDX_Control(pDX, IDC_BUTTON_RESET, button_reset);
	DDX_Control(pDX, IDC_BUTTON_RIGHT_MOVING, button_right_moving);
	DDX_Control(pDX, IDC_BUTTON_LEFT_MOVING, button_left_moving);
	DDX_Control(pDX, IDC_BUTTON_QUIT, button_moving_quit);
}

BEGIN_MESSAGE_MAP(CMFCDeviceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DEVICE, &CMFCDeviceDlg::OnBnClickedOpenDevice)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_DEVICE, &CMFCDeviceDlg::OnBnClickedCloseDevice)
	ON_BN_CLICKED(IDC_BUTTON_EXP_DOWN, &CMFCDeviceDlg::OnBnClickedBtnExpDown)
	ON_BN_CLICKED(IDC_BUTTON_EXP_UP, &CMFCDeviceDlg::OnBnClickedBtnExpUp)
	ON_EN_CHANGE(IDC_EDIT_LOG, &CMFCDeviceDlg::OnEnChangeEditLog)
	ON_EN_CHANGE(IDC_EDIT_EXP_VALUE, &CMFCDeviceDlg::OnEnChangeEditExpValue)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_GAIN_VALUE_DOWN, &CMFCDeviceDlg::OnBnClickedGainValueDown)
	ON_BN_CLICKED(IDC_BUTTON_GAIN_VALUE_UP, &CMFCDeviceDlg::OnBnClickedGainValueUp)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_EXP_VALUE, &CMFCDeviceDlg::OnNMReleasedcaptureSliderExpValue)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GAIN_VALUE, &CMFCDeviceDlg::OnNMReleasedcaptureSliderGainValue)
	ON_BN_CLICKED(IDC_BUTTON_LED_VALUE_DOWN, &CMFCDeviceDlg::OnBnClickedLedValueDown)
	ON_BN_CLICKED(IDC_BUTTON_LED_VALUE_UP, &CMFCDeviceDlg::OnBnClickedLedValueUp)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CMFCDeviceDlg::OnBnClickedButtonSaveImage)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_STOP, &CMFCDeviceDlg::OnBnClickedButtonVideoStop)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_START, &CMFCDeviceDlg::OnBnClickedButtonVideoStart)
	ON_EN_CHANGE(IDC_EDIT_LED, &CMFCDeviceDlg::OnEnChangeEditLed)
	ON_EN_CHANGE(IDC_EDIT_USER_ID, &CMFCDeviceDlg::OnEnChangeEditUserId)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_USER_ID, &CMFCDeviceDlg::OnBnClickedButtonCheckUserID)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMFCDeviceDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_MOVING, &CMFCDeviceDlg::OnBnClickedButtonRightMoving)
	ON_BN_CLICKED(IDC_BUTTON_LEFT_MOVING, &CMFCDeviceDlg::OnBnClickedButtonLeftMoving)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CMFCDeviceDlg::OnBnClickedButtonQuit)
	ON_EN_CHANGE(IDC_EDIT_DISTANCE_VALUE, &CMFCDeviceDlg::OnEnChangeEditDistanceValue)
END_MESSAGE_MAP()


// CMFCDeviceDlg 消息处理程序

BOOL CMFCDeviceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
	edit_font.CreatePointFont(170, L"Arial");//创建字体和大小
	edit_user_ID.SetFont(&edit_font);//第一个框内的字体

	// 设置曝光、增益滑动条属性
	slider_exp_value.SetRange(1, 67);
	slider_exp_value.SetPos(32);
	slider_exp_value.SetTicFreq(1);
	exp_value.Format(_T("0x%02X(%d)"), slider_exp_value.GetPos(), slider_exp_value.GetPos());
	slider_gain_value.SetRange(1, 64);
	slider_gain_value.SetPos(20);
	slider_gain_value.SetTicFreq(1);
	gain_value.Format(_T("0x%02X(%d)"), slider_gain_value.GetPos(), slider_gain_value.GetPos());
	LED_value.Format(_T("0x%02X(%d)"), 8, 8);

	button_save_image.EnableWindow(false);
	slider_gain_value.EnableWindow(false);
	button_gain_up.EnableWindow(false);
	button_gain_down.EnableWindow(false);
	button_reset.EnableWindow(false);
	button_moving_quit.EnableWindow(false);
	button_right_moving.EnableWindow(false);
	button_left_moving.EnableWindow(false);
	// 距离线程
	p_distance_thread = AfxBeginThread((AFX_THREADPROC)DistanceThread, this, THREAD_PRIORITY_NORMAL, 0, 0, nullptr);
	p_distance_thread->SuspendThread();


	// 图像线程
	p_capture_thread = AfxBeginThread((AFX_THREADPROC)CpatureThread, this, THREAD_PRIORITY_NORMAL, 0, 0, nullptr);
	p_capture_thread->SuspendThread();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCDeviceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCDeviceDlg::OnPaint()
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
HCURSOR CMFCDeviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMFCDeviceDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CMFCDeviceDlg::OnBnClickedOpenDevice()
{

	if (p_device_control->OpenDevice() != S_OK)
	{
		int a = 0;
	}
	//p_distance_thread->ResumeThread();
	p_capture_thread->ResumeThread();
	button_reset.EnableWindow(true);
	button_check_user_ID.EnableWindow(true);
	button_exp_down.EnableWindow(true);
	button_exp_up.EnableWindow(true);
	slider_exp_value.EnableWindow(true);
	close_device.EnableWindow(true);
}

void CMFCDeviceDlg::OnBnClickedCloseDevice()
{
	// TODO: 在此添加控件通知处理程序代码
	p_device_control->CloseDevice();
	p_capture_thread->SuspendThread();
	Serialization();
	button_reset.EnableWindow(false);
	button_save_image.EnableWindow(false);
	button_check_user_ID.EnableWindow(false);
	button_exp_down.EnableWindow(false);
	button_exp_up.EnableWindow(false);
	slider_exp_value.EnableWindow(false);
	close_device.EnableWindow(false);
	HANDLE MyProcess = GetCurrentProcess();
	TerminateProcess(MyProcess, 0);
}

void CMFCDeviceDlg::MatToCImage(Mat& mat, CImage& cimage)
{
	if (0 == mat.total())
	{
		return;
	}

	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//重建cimage
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//拷贝数据


	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负


	if (1 == nChannels)								//对于单通道的图像需要初始化调色板
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}

void CMFCDeviceDlg::CImageToMat(CImage& cimage, Mat& mat)
{
	if (true == cimage.IsNull())
	{
		return;
	}
	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();


	//重建mat
	if (1 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC3);
	}

	//拷贝数据

	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

int CMFCDeviceDlg::ListDevices()
{
	vector<string> capture_list;
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK)
		{
			IMoniker* pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag* pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
				if (FAILED(hr))
				{
					pMoniker->Release();
					continue;
				}
				// Find the description or friendly name.
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);

				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				}
				if (SUCCEEDED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);

					int count = 0;
					char tmp[255] = { 0 };
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					capture_list.push_back(tmp);
				}

				pPropBag->Release();
				pPropBag = NULL;

				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;

			pEnum->Release();
			pEnum = NULL;
		}
	}
	//return deviceCounter;

	// 获取摄像头ID
	int capture_id = 0;
	const string omec_device = "OMEC Vein Scanner";
	for (int i = 0; i < capture_list.size(); ++i)
	{
		if (capture_list[i] == omec_device)
		{
			capture_id = i;
			break;
		}
	}
	return capture_id;
}

UINT CMFCDeviceDlg::CpatureThread(LPVOID param)
{
	shared_ptr<CMFCDeviceDlg>  p_device_Dlg((CMFCDeviceDlg*)param);
	int com_id = p_device_Dlg->ListDevices();
	// 获取摄像头ID
	VideoCapture capture(com_id + cv::CAP_DSHOW);
	Mat frame/*(1080,1920, CV_8UC3)*/;
	CImage image;
	CRect rect;
	capture.set(CAP_PROP_FRAME_WIDTH, 1920);
	capture.set(CAP_PROP_FRAME_HEIGHT, 1080);
	// 读取user_id json
	p_device_Dlg->InitJsonCheck();
	p_device_Dlg->InitDecivce();
	// 图片路径
	string image_path_value = "";

	while (p_device_Dlg->b_device_status)
	{
		capture >> frame;

		p_device_Dlg->MatToCImage(frame, image);

		CWnd* pWnd = p_device_Dlg->GetDlgItem(IDC_PIC_VIDEO);//获得pictrue控件窗口的句柄   
		CDC* pDC = pWnd->GetDC();//获得pictrue控件的DC  
		HDC hDC = pDC->GetSafeHdc();

		p_device_Dlg->GetClientRect(&rect);
		p_device_Dlg->GetDlgItem(IDC_PIC_VIDEO)->GetClientRect(&rect);
		Mat dst;
		int x = rect.Width();
		int y = rect.Height();
		resize(frame, dst, Size(x, y));
		p_device_Dlg->MatToCImage(dst, image);
		image.Draw(pDC->m_hDC, rect);

		bool b_dynamic_next = false;

		// 距离
		p_device_Dlg->p_device_control->Get_Distance(p_device_Dlg->distance_value);
		int temp_distance_value = p_device_Dlg->HexToTen(p_device_Dlg->distance_value);
		p_device_Dlg->ShowDistance(to_string(temp_distance_value));

		// 保存图片
		if (p_device_Dlg->b_save_image)
		{
			// 增加序号
			++p_device_Dlg->user_serial_number;
			p_device_Dlg->user_info->serial_number = p_device_Dlg->user_serial_number;
			p_device_Dlg->p_device_control->Get_Exposure(p_device_Dlg->user_info->exp_value);
			p_device_Dlg->p_device_control->Get_BL(p_device_Dlg->user_info->BL_value);
			// 图片路径
			image_path_value = p_device_Dlg->ImageFileName();
			imwrite(image_path_value, frame);
			// 图片亮度
			double temp_bright = 0.0f;
			p_device_Dlg->PythonBright(image_path_value, temp_bright);


			string current_brightness = "当前图像亮度值：" + to_string(temp_bright);
			p_device_Dlg->ShowLog(current_brightness);

			p_device_Dlg->b_save_image = false;
		}
		if (p_device_Dlg->b_dynamic_collect && temp_distance_value < 240 && temp_distance_value>60)
		{
			++p_device_Dlg->user_dynamic_number;
			p_device_Dlg->InitMovingLEDExp();
			p_device_Dlg->user_info->dynamic_number = p_device_Dlg->user_dynamic_number;
			p_device_Dlg->p_device_control->Get_Exposure(p_device_Dlg->user_info->exp_value);
			p_device_Dlg->p_device_control->Get_BL(p_device_Dlg->user_info->BL_value);
			image_path_value = p_device_Dlg->ImageDynamicFileName();
			imwrite(image_path_value, frame);
		}
		/*if (p_device_Dlg->b_save_video == true)
		{
			p_device_Dlg->video_writer.write(frame);
		}*/
		pWnd->ReleaseDC(pDC);
	}
	capture.release();
	return 0;
}

UINT CMFCDeviceDlg::DistanceThread(LPVOID param)
{
	shared_ptr<CMFCDeviceDlg>  p_device_Dlg((CMFCDeviceDlg*)param);
	// Set Default Exposure Value
	p_device_Dlg->p_device_control = make_shared<DeviceControl>();
	string distance_value = "";
	while (p_device_Dlg->b_device_status)
	{
		p_device_Dlg->p_device_control->Get_Distance(distance_value);
		p_device_Dlg->ShowDistance(distance_value);
	}
	return 0;
}

void CMFCDeviceDlg::ShowLog(string& Msg)
{
	CString Show_str;
	CTime time = CTime::GetCurrentTime();
	CString sTime = time.Format(_T("[%Y/%B/%d, %H:%M:%S]   "));
	CString temp_Msg = sTime + Msg.c_str() + _T("\r\n");

	/*ASSERT(edit_log && edit_log.GetSafeHwnd());
	edit_log.SetSel(-1, true);
	edit_log.ReplaceSel(Show_str);*/
	edit_log.GetWindowText(Show_str);
	Show_str += temp_Msg;
	edit_log.SetWindowText(Show_str);
	edit_log.LineScroll(edit_log.GetLineCount());
	if (edit_log.GetLineCount() > 500)
	{
		edit_log.Clear();
	}
}

void CMFCDeviceDlg::ShowDistance(string distance_str)
{
	CString show_distance_str;
	CTime time = CTime::GetCurrentTime();
	CString sTime = time.Format(_T("[%Y/%B/%d, %H:%M:%S]   "));
	CString temp_Msg = sTime + distance_str.c_str() + _T("\r\n");
	show_distance_str += temp_Msg;

	ASSERT(edit_distance_value && edit_distance_value.GetSafeHwnd());
	edit_distance_value.SetSel(-1, true);
	edit_distance_value.ReplaceSel(show_distance_str);
	int line_numbers = edit_distance_value.GetLineCount();
	if (line_numbers > 1000)
	{
		GetDlgItem(IDC_EDIT_DISTANCE_VALUE)->SetWindowText(_T(""));
	}
}

string CMFCDeviceDlg::ImageFileName()
{
	string temp_suffix = ".bmp";
	/*char systime[50];
	time_t aclock;
	time(&aclock);
	strftime(systime, 50, "%Y%m%d%H%M%S", _localtime64(&aclock));
	string filename = systime + temp_suffix;*/
	string hand_rl;
	if (user_info->is_right_hand == true)
	{
		hand_rl = "R";
	}
	else
	{
		hand_rl = "L";
	}
	string filename = "./image/" + to_string(user_info->serial_number) + "_" + hand_rl + "_" + user_info->user_ID + "_" + to_string(user_info->collection_times) + "_" + user_info->exp_value + "_" + user_info->BL_value + "_" + to_string(user_info->distance_value) + "_" + user_info->device_ID + temp_suffix;
	return filename;
}

string CMFCDeviceDlg::ImageDynamicFileName()
{
	string temp_suffix = ".bmp";
	/*char systime[50];
	time_t aclock;
	time(&aclock);
	strftime(systime, 50, "%Y%m%d%H%M%S", _localtime64(&aclock));
	string filename = systime + temp_suffix;*/
	string hand_rl;
	if (user_info->is_right_hand == true)
	{
		hand_rl = "R";
	}
	else
	{
		hand_rl = "L";
	}
	string filename = "./dynamicimage/" + to_string(user_info->dynamic_number) + "_" + hand_rl + "_" + user_info->user_ID + "_" + user_info->exp_value + "_" + user_info->BL_value + "_" + user_info->device_ID + temp_suffix;
	return filename;
}

string CMFCDeviceDlg::VideoFileName()
{
	string temp_suffix = ".avi";
	/*char systime[50];
	time_t aclock;
	time(&aclock);
	strftime(systime, 50, "%Y%m%d%H%M%S", _localtime64(&aclock));*/
	string filename = "s" + temp_suffix;
	return filename;
}

void CMFCDeviceDlg::InitLEDExp()
{
	//  默认曝光
	int exp_value_default = 32;
	int LED_lightness_default = 6;
	switch (user_info->distance_value)
	{
	case 3:
		exp_value_default = 30;
		LED_lightness_default = 6;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);

		break;
	case 4:
		exp_value_default = 35;
		LED_lightness_default = 5;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 5:
		exp_value_default = 39;
		LED_lightness_default = 4;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 6:
		exp_value_default = 46;
		LED_lightness_default = 3;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 7:
		exp_value_default = 48;
		LED_lightness_default = 2;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 8:
		exp_value_default = 49;
		LED_lightness_default = 1;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 9:
		exp_value_default = 41;
		LED_lightness_default = 0;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	case 10:
		exp_value_default = 52;
		LED_lightness_default = 0;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
		exp_value.Format(_T("0x%02X(%d)"), exp_value_default, exp_value_default);
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness_default, LED_lightness_default);
		break;
	default:
		break;
	}
}

void CMFCDeviceDlg::InitMovingLEDExp()
{
	//  默认曝光
	int exp_value_default = 40;
	int LED_lightness_default = 8;
	//3cm-4cm
	if (user_info->distance_value>60 && user_info->distance_value<110)
	{
		exp_value_default = 33;
		LED_lightness_default = 5;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
		// 设置滑动条和显示
		slider_exp_value.SetPos(exp_value_default);
	}
	//5cm-6cm
	else if (user_info->distance_value >= 110 && user_info->distance_value < 190)
	{
		exp_value_default = 52;
		LED_lightness_default = 4;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
	}
	//7cm-8cm
	else if (user_info->distance_value >= 190 && user_info->distance_value < 240)
	{
		exp_value_default = 52;
		LED_lightness_default = 2;
		//设置LED 曝光值
		p_device_control->Set_Exposure(TenToHex(exp_value_default));
		p_device_control->Set_LEDCurrent(TenToHex(LED_lightness_default));
	}
}

bool CMFCDeviceDlg::InitJsonCheck()
{
	// 判定json文件是否存在
	ifstream is("test.json");
	if (!is.is_open())
	{
		string temp_str_failure = "读取json失败";
		ShowLog(temp_str_failure);
		return false;
	}
	Json::Reader reader;
	Json::Value root;
	// 读取json
	if (reader.parse(is, root))
	{
		if (!root["user_info"].isNull())
		{
			for (int i = 0; i < root["user_info"].size(); ++i)
			{
				shared_ptr<USERJSON> temp_userjson = make_shared<USERJSON>("", 1);
				temp_userjson->user_ID = root["user_info"][i]["user_ID"].asString();
				temp_userjson->collection_times = root["user_info"][i]["collection"].asInt();
				user_json_vec.push_back(temp_userjson);
			}
		}
		if (!root["serial_number"].isNull())
		{
			user_serial_number = root["serial_number"].asInt();
		}
		if (!root["dynamic_number"].isNull())
		{
			user_dynamic_number = root["dynamic_number"].asInt();
		}
	}
	return true;
}

void CMFCDeviceDlg::InitDecivce()
{
	// 默认增益
	string default_gain = "10";
	p_device_control->Set_Gain(default_gain);

	//  默认曝光
	string default_exp = "32";
	p_device_control->Set_Exposure(default_exp);

	// 默认LED
	string default_LED_lightness = "6";
	p_device_control->Set_LEDCurrent(default_LED_lightness);

	// 默认图像压缩率
	string default_compression_ratio = "05";
	p_device_control->Set_CompressionRatio(default_compression_ratio);
}

bool CMFCDeviceDlg::OpenDB()
{
	/*sqlite3* p_new_db = new sqlite3;
	int result = sqlite3_open("D:/mytest.db", *p_new_db);*/
	return true;
}

int CMFCDeviceDlg::HexToTen(string& in_str)
{
	int dec_out = stoi(in_str, nullptr, 16);
	return dec_out;
}

string CMFCDeviceDlg::TenToHex(int& in_hec)
{
	string out;
	stringstream ss;
	ss << std::hex << in_hec;
	ss >> out;
	transform(out.begin(), out.end(), out.begin(), ::toupper);
	return out;
}

void CMFCDeviceDlg::PythonBright(string& in_dir, double& in_result)
{
	if (b_python_init == false)
	{
		b_python_init = true;
		Py_Initialize();
	}
	//导入sys，添加模块路径
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject* pModule = PyImport_ImportModule("bright");
	// 加载模块失败
	if (!pModule)
	{
		cout << "[ERROR] Python get module failed." << endl;
		return;
	}
	// 加载函数
	PyObject* pv = PyObject_GetAttrString(pModule, "readimage");
	if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
	{
		cout << "[ERROR] Can't find funftion (readimage)" << endl;
		return;
	}
	// 设置参数
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, Py_BuildValue("s", in_dir.c_str()));
	// 调用函数
	PyObject* pResult = PyObject_CallObject(pv, args);
	// 获取参数
	if (pResult)
	{
		PyArg_Parse(pResult, "d", &in_result);
	}
}

bool CMFCDeviceDlg::Serialization()
{
	Json::FastWriter writer;
	Json::Value value;
	Json::Value user_value;
	value["serial_number"] = user_serial_number;
	value["dynamic_number"] = user_dynamic_number;
	for (auto& user_json : user_json_vec)
	{
		user_value["user_ID"] = user_json->user_ID;
		user_value["collection"] = user_json->collection_times;
		value["user_info"].append(user_value);
	}

	Json::StyledWriter sw;
	ofstream os("test.json");
	if (!os.is_open())
	{
		string temp_str_failure = "读取json失败";
		ShowLog(temp_str_failure);
		return false;
	}
	string str = sw.write(value);
	os << str;
	os.close();
	return true;
}

void CMFCDeviceDlg::OnBnClickedBtnExpDown()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 滑动条的曝光值
	int temp_exp_value = slider_exp_value.GetPos();
	// 设置的曝光值
	string exp_val_str;
	if (temp_exp_value == 1)
	{
		string temp_str = "曝光到达最小值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 曝光值 -1
		temp_exp_value -= 1;
		exp_val_str = to_string(temp_exp_value);
		string temp_exp_str = "当前曝光值： " + exp_val_str;
		ShowLog(temp_exp_str);
		if (p_device_control->Set_Exposure(TenToHex(temp_exp_value)) != S_OK)
		{
			string temp_str_failure = "曝光设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		slider_exp_value.SetPos(temp_exp_value);
		exp_value.Format(_T("0x%02X(%d)"), temp_exp_value, temp_exp_value);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedBtnExpUp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 滑动条的曝光值
	int temp_exp_value = slider_exp_value.GetPos();
	// 设置的曝光值
	string exp_val_str;
	if (temp_exp_value == 1)
	{
		string temp_str = "曝光到达最小值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 曝光值 +1
		temp_exp_value += 1;
		exp_val_str = to_string(temp_exp_value);
		string temp_exp_str = "当前曝光值： " + exp_val_str;
		ShowLog(temp_exp_str);
		if (p_device_control->Set_Exposure(TenToHex(temp_exp_value)) != S_OK)
		{
			string temp_str_failure = "曝光设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		slider_exp_value.SetPos(temp_exp_value);
		exp_value.Format(_T("0x%02X(%d)"), temp_exp_value, temp_exp_value);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnEnChangeEditLog()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CMFCDeviceDlg::OnEnChangeEditExpValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CMFCDeviceDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* slider = (CSliderCtrl*)pScrollBar;

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMFCDeviceDlg::OnBnClickedGainValueDown()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 滑动条的增益值
	int temp_gain_value = slider_gain_value.GetPos();
	// 设置的增益值
	string gain_val_str;
	if (temp_gain_value == 1)
	{
		string temp_str = "增益到达最小值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 增益值-1
		temp_gain_value -= 1;
		gain_val_str = to_string(temp_gain_value);
		string temp_gain_str = "当前增益值： " + gain_val_str;
		ShowLog(temp_gain_str);
		if (p_device_control->Set_Gain(TenToHex(temp_gain_value)) != S_OK)
		{
			string temp_str_failure = "增益设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		slider_gain_value.SetPos(temp_gain_value);
		gain_value.Format(_T("0x%02X(%d)"), temp_gain_value, temp_gain_value);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedGainValueUp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 滑动条的增益值
	int temp_gain_value = slider_gain_value.GetPos();
	// 设置的增益值
	string gain_val_str;
	if (temp_gain_value == 1)
	{
		string temp_str = "增益到达最小值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 增益值+1
		temp_gain_value += 1;
		gain_val_str = to_string(temp_gain_value);
		string temp_gain_str = "当前增益值： " + gain_val_str;
		ShowLog(temp_gain_str);
		if (p_device_control->Set_Gain(TenToHex(temp_gain_value)) != S_OK)
		{
			string temp_str_failure = "增益设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		slider_gain_value.SetPos(temp_gain_value);
		gain_value.Format(_T("0x%02X(%d)"), temp_gain_value, temp_gain_value);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnNMReleasedcaptureSliderExpValue(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	int temp_exp_value = slider_exp_value.GetPos();
	// 改动曝光值
	string exp_val_str;
	exp_val_str = to_string(temp_exp_value);
	string temp_exp_str = "设置曝光值为： " + exp_val_str;
	ShowLog(temp_exp_str);

	// 设置曝光值
	if (p_device_control->Set_Exposure(TenToHex(temp_exp_value)) != S_OK)
	{
		string temp_str_failure = "曝光设置失败";
		ShowLog(temp_str_failure);
	}
	// 设置滑动条和显示
	slider_exp_value.SetPos(temp_exp_value);
	exp_value.Format(_T("0x%02X(%d)"), temp_exp_value, temp_exp_value);
	UpdateData(false);
	*pResult = 0;
}

void CMFCDeviceDlg::OnNMReleasedcaptureSliderGainValue(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	int temp_gain_value = slider_gain_value.GetPos();
	// 改动增益值
	string gain_val_str;
	gain_val_str = to_string(temp_gain_value);
	string temp_gain_str = "设置增益值为： " + gain_val_str;
	ShowLog(temp_gain_str);

	// 设置增益值
	if (p_device_control->Set_Gain(TenToHex(temp_gain_value)) != S_OK)
	{
		string temp_str_failure = "增益设置失败";
		ShowLog(temp_str_failure);
	}
	// 设置滑动条和显示
	slider_gain_value.SetPos(temp_gain_value);
	gain_value.Format(_T("0x%02X(%d)"), temp_gain_value, temp_gain_value);
	UpdateData(false);
	*pResult = 0;
}

void CMFCDeviceDlg::OnBnClickedLedValueDown()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 判定亮度范围
	if (LED_lightness == 10)
	{
		string temp_str = "亮度到达最小值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 亮度调低
		LED_lightness += 1;
		string LED_lightness_str = to_string(LED_lightness);
		string temp_LED_lightness_str = "当前亮度：" + LED_lightness_str;
		ShowLog(temp_LED_lightness_str);
		if (p_device_control->Set_LEDCurrent(TenToHex(LED_lightness)) != S_OK)
		{
			string temp_str_failure = "LED亮度设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness, LED_lightness);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedLedValueUp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 判定亮度范围
	if (LED_lightness == 0)
	{
		string temp_str = "亮度到达最大值";
		ShowLog(temp_str);
		return;
	}
	else
	{
		// 亮度调高
		LED_lightness -= 1;
		string LED_lightness_str = to_string(LED_lightness);
		string temp_LED_lightness_str = "当前亮度：" + LED_lightness_str;
		ShowLog(temp_LED_lightness_str);
		if (p_device_control->Set_LEDCurrent(TenToHex(LED_lightness)) != S_OK)
		{
			string temp_str_failure = "LED亮度设置失败";
			ShowLog(temp_str_failure);
		}
		// 设置滑动条和显示
		LED_value.Format(_T("0x%02X(%d)"), LED_lightness, LED_lightness);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedButtonSaveImage()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	b_save_image = true;
	button_save_image.EnableWindow(false);
	// 转为左手
	if (user_info->distance_value == 10)
	{
		if (user_info->is_right_hand == false)
		{
			if (IDYES == MessageBoxW(_T("采集结束，进行动态采集！"), NULL, MB_YESNO))
			{
				//重置
				button_check_user_ID.EnableWindow(false);
				button_save_image.EnableWindow(false);
				button_reset.EnableWindow(false);
				button_left_moving.EnableWindow(true);
				button_right_moving.EnableWindow(true);
				cuser_ID = _T("");
			}
			else
			{
				button_save_image.EnableWindow(true);
			}
		}
		else if (IDYES == MessageBoxW(_T("是否采集左手！"), NULL, MB_YESNO))
		{
			user_info->is_right_hand = false;
			user_info->distance_value = 3;
			InitLEDExp();

			button_save_image.EnableWindow(true);
		}
		else
		{
			button_save_image.EnableWindow(true);
		}
	}
	else
	{
		string message_str = "请判断灰度值是否为100 - 130\n当前距离为" + to_string(user_info->distance_value) + "\n是否采集下一个距离的图像";

		if (IDYES == MessageBoxW(CA2T(message_str.c_str()), NULL, MB_YESNO))
		{
			++user_info->distance_value;
			InitLEDExp();
		}
		button_save_image.EnableWindow(true);
	}
	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedButtonVideoStop()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	b_save_video = false;

	UpdateData(false);
}

void CMFCDeviceDlg::OnBnClickedButtonVideoStart()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	//int video_width = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
	//int video_height = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	Size s(1920, 1080);
	int temp_fourcc = video_writer.fourcc('M', 'P', 'J', 'A');

	video_writer.open(VideoFileName(), temp_fourcc, 30.0, s, true);
	b_save_video = true;
	UpdateData(false);
}

void CMFCDeviceDlg::OnEnChangeEditLed()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CMFCDeviceDlg::OnEnChangeEditUserId()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CMFCDeviceDlg::OnBnClickedButtonCheckUserID()
{
	Serialization();
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT_USER_ID)->GetWindowTextW(cuser_ID);
	user_ID = CT2A(cuser_ID.GetBuffer());
	user_info = make_shared<USERINFO>(0, true, "", 1, "", "", 3, "", 0);
	// LED值和曝光值初始化
	InitLEDExp();

	// 设置用户信息
	user_info->serial_number = user_serial_number;
	user_info->dynamic_number = user_dynamic_number;
	user_info->user_ID = user_ID;
	bool b_user_exist = false;
	// 从json查找用户
	for (auto& temp_user_json : user_json_vec)
	{
		if (temp_user_json->user_ID == user_ID)
		{
			temp_user_json->collection_times += 1;
			user_info->collection_times = temp_user_json->collection_times;
			b_user_exist = true;
			break;
		}
	}
	if (b_user_exist == false)
	{
		shared_ptr<USERJSON> temp_user_json = make_shared<USERJSON>(user_info->user_ID, user_info->collection_times);
		user_json_vec.push_back(temp_user_json);
	}

	button_check_user_ID.EnableWindow(false);
	button_save_image.EnableWindow(true);
	button_reset.EnableWindow(true);
	UpdateData(false);
}


void CMFCDeviceDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	button_check_user_ID.EnableWindow(true);
	button_save_image.EnableWindow(false);
	cuser_ID = _T("");
	UpdateData(false);
}


void CMFCDeviceDlg::OnBnClickedButtonRightMoving()
{
	// TODO: 在此添加控件通知处理程序代码
	b_dynamic_collect = true;
	user_info->is_right_hand = true;
	button_right_moving.EnableWindow(false);
	button_left_moving.EnableWindow(false);
	button_moving_quit.EnableWindow(true);

	button_check_user_ID.EnableWindow(false);
}


void CMFCDeviceDlg::OnBnClickedButtonLeftMoving()
{
	// TODO: 在此添加控件通知处理程序代码
	b_dynamic_collect = true;
	user_info->is_right_hand = false;
	button_right_moving.EnableWindow(false);
	button_left_moving.EnableWindow(false);
	button_moving_quit.EnableWindow(true);

	button_check_user_ID.EnableWindow(false);
}


void CMFCDeviceDlg::OnBnClickedButtonQuit()
{
	// TODO: 在此添加控件通知处理程序代码
	b_dynamic_collect = false;
	button_right_moving.EnableWindow(true);
	button_left_moving.EnableWindow(true);
	button_moving_quit.EnableWindow(false);
	button_check_user_ID.EnableWindow(true);
}


void CMFCDeviceDlg::OnEnChangeEditDistanceValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
