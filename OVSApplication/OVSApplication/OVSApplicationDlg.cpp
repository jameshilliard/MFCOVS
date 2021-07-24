
// OVSApplicationDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "OVSApplication.h"
#include "OVSApplicationDlg.h"
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


// COVSApplicationDlg 对话框



COVSApplicationDlg::COVSApplicationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OVSAPPLICATION_DIALOG, pParent)
	, b_user_idenfiy(false)
	, b_user_register(false)
	, edit_user_id_str(_T(""))
	, current_user_id("")
	, current_template_num(1)
	, illumination(0.0f)
	, b_hardware_change(false)
	, b_get_template(true)
	, template_num(0)
	, hand_center_x(0)
	, hand_center_y(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COVSApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPEN_DEVICE, button_open_device);
	DDX_Control(pDX, IDC_BUTTON_USER_REGISTER, button_user_register);
	DDX_Control(pDX, IDC_BUTTON_USER_IDENFIY, button_user_idenfiy);
	DDX_Control(pDX, IDC_EDIT_LOG, edit_log);
	DDX_Control(pDX, IDC_STATIC_USER_ID, static_user_id);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, button_close_device);
	DDX_Control(pDX, IDC_EDIT_USER_ID, edit_user_id);
	DDX_Text(pDX, IDC_EDIT_USER_ID, edit_user_id_str);
	DDX_Control(pDX, IDC_BUTTON_CHECK_USER, button_check_user);
	DDX_Control(pDX, IDC_EDIT_DISTANCE_LOG, edit_distance_log);
	DDX_Control(pDX, IDC_BUTTON_IDENTIFY_CLOSE, button_identify_close);
}

BEGIN_MESSAGE_MAP(COVSApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DEVICE, &COVSApplicationDlg::OnBnClickedButtonOpenDevice)
	ON_BN_CLICKED(IDC_BUTTON_USER_REGISTER, &COVSApplicationDlg::OnBnClickedButtonUserRegister)
	ON_BN_CLICKED(IDC_BUTTON_USER_IDENFIY, &COVSApplicationDlg::OnBnClickedButtonUserIdenfiy)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &COVSApplicationDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_USER, &COVSApplicationDlg::OnBnClickedButtonCheckUser)
	ON_BN_CLICKED(IDC_BUTTON_IDENTIFY_CLOSE, &COVSApplicationDlg::OnBnClickedButtonIdentifyClose)
END_MESSAGE_MAP()


// COVSApplicationDlg 消息处理程序

BOOL COVSApplicationDlg::OnInitDialog()
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

	// 设置字体大小
	edit_font.CreatePointFont(170, L"Arial");
	edit_user_id.SetFont(&edit_font);
	GetDlgItem(IDC_STATIC_USER_ID)->SetWindowText(_T("用户ID:"));
	GetDlgItem(IDC_STATIC_USER_ID)->SetFont(&edit_font);

	if (!RunInit())
	{
		string temp_log = "运行初始化失败";
		return false;
	}

	button_open_device.EnableWindow(true);
	button_close_device.EnableWindow(false);
	button_check_user.EnableWindow(false);
	button_user_register.EnableWindow(false);
	button_user_idenfiy.EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COVSApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COVSApplicationDlg::OnPaint()
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
HCURSOR COVSApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool COVSApplicationDlg::RunInit()
{
	p_ovs_device = make_shared<OVSDevice>();
	p_sqlite_control = make_shared<OVSSqliteControl>();

	// 设备SDK初始化
	if (!p_ovs_device->DeviceInit())
	{
		string temp_log = "初始化失败";
		ShowLog(temp_log);
		return false;
	}
	// 读取数据库所有信息
	if (!p_sqlite_control->GetUserData(p_all_user_info_vector))
	{
		string temp_log = "读取数据库用户信息失败";
		ShowLog(temp_log);
		return false;
	}

	return true;
}

bool COVSApplicationDlg::ImageOutputToScreen()
{
	// 获得摄像头
	int com_id = ListDevices();
	VideoCapture capture(com_id + cv::CAP_DSHOW);
	//VideoCapture capture(com_id + cv::CAP_DSHOW);
	capture.set(CAP_PROP_FRAME_WIDTH, 1920);
	capture.set(CAP_PROP_FRAME_HEIGHT, 1080);
	// 设备初始化参数
	if (!DeviceParamInit())
	{
		string temp_log = "设备设置初始化参数失败";
		return false;
	}

	// 从摄像头读取的Mat
	Mat frame/*(1080,1920, CV_8UC3)*/;
	// 显示到屏幕的Mat
	Mat display_dst;

	// 获得pictrue控件窗口的句柄 
	shared_ptr<CWnd> p_wnd(GetDlgItem(IDC_STATIC_PICTURE_CAMERA));
	// 获得pictrue控件的D
	shared_ptr<CDC> p_dc(p_wnd->GetDC());

	CImage image;
	CRect rect;
	GetClientRect(&rect);
	GetDlgItem(IDC_STATIC_PICTURE_CAMERA)->GetClientRect(&rect);
	//CPen p_temp_pen(PS_SOLID, 3, RGB(5, 225, 225));
	//CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	// 录制视频
	/*string output_video_path = "match1.avi";
	cv::Size S = cv::Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT));

	VideoWriter output_video;
	output_video.open(output_video_path, output_video.fourcc('M', 'J', 'P', 'G'),30.0, S);*/

	while (1)
	{
		capture >> frame;
		if (frame.empty())
		{
			continue;
		}

		//output_video.write(frame);
		Mat translate_dst;
		ImageTranslate(translate_dst, frame);
		resize(translate_dst, display_dst, Size(rect.Width(), rect.Height()));
		// 注册用户和识别用户
		if (b_user_register || b_user_idenfiy)
		{
			{
				lock_guard<mutex> lk(ovs_mutex);
				frame.copyTo(common_mat);
			}
			if ((hand_center_x != 0 || hand_center_y != 0) && distance_value < 250)
			{
				circle(display_dst, Point(rect.Width() - hand_center_x, hand_center_y), 100, CV_RGB(225.0f, 85.0f, 1.0f), 2);
			}
		}

		MatToCImage(display_dst, image);
		image.Draw(p_dc->m_hDC, rect);
	}

	p_wnd->ReleaseDC(p_dc.get());
	capture.release();
	return true;
}

bool COVSApplicationDlg::UserIdenfiy()
{
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.release();
	}

	// 设置默认值 因为现在LED灯没有随着距离改变
	DeviceParamInit();

	while (b_user_idenfiy)
	{
		if (distance_value > 240)
		{
			/*	string temp_log = "请将手掌靠近设备.";
				ShowLog(temp_log);*/
			continue;
		}
		// 由于设备硬件问题，设备运行随机出现<30
		if (distance_value < 60 && distance_value > 30)
		{
			/*string temp_log = "请将手掌远离设备.";
			ShowLog(temp_log);*/
			continue;
		}
		// 采集图像
		{
			lock_guard<mutex> lk(ovs_mutex);
			common_mat.copyTo(temp_register_mat);
			if (temp_register_mat.empty())
			{
				continue;
			}
		}
		if (IdenfiyTemp(temp_register_mat) != ENUM_OK)
		{
			/*string temp_log = "fail to idenify.";
			ShowLog(temp_log);*/
			continue;
		}
		else
		{
			b_user_idenfiy = false;
			string temp_log = "idenify Success.";
			ShowLog(temp_log);
			return true;
		}
	}
	string temp_log = "idenify stop.";
	ShowLog(temp_log);
	return false;
}

EProcessingState COVSApplicationDlg::IdenfiyTemp(Mat& in_mat)
{
	// 算法
	shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);

	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);

	SPalmImageInfo image_info;

	// 解析图像
	if (p_palm_parser->ParseInfo(image_info) != OVS_SUCCESS)
	{
		/*string temp_log = "fail to parse palm infomation.";
		ShowLog(temp_log);*/
		hand_center_x = 0;
		hand_center_y = 0;
		return OVS_ERR_NO_FOUND_PALM;
	}

	// 更新手掌信息
	hand_center_x = image_info.center_x;
	hand_center_y = image_info.center_y;
	// 模糊度
	if (image_info.score > 12.0f)
	{
		string temp_log = "roi is too blurry.";
		ShowLog(temp_log);
		return OVS_ERR_ROI_BLURRY;
	}
	// 平均亮度
	string temp_log = to_string(image_info.mean_illumination);
	illumination = image_info.mean_illumination;

	ShowLog(temp_log);
	if (image_info.mean_illumination > 140.f && b_hardware_change)
	{
		/*string temp_log = "mean_illumination is too strong.";
		ShowLog(temp_log);*/
		string temp_led_lightness = "";
		p_ovs_device->Get_LEDCurrent(temp_led_lightness);
		// led亮度值 string转为int
		int temp_int_led_value = HexToTen(temp_led_lightness);

		//测试
		string temp_log1 = to_string(temp_int_led_value);
		ShowLog(temp_log1);


		if (temp_int_led_value >= 8)
		{
			string temp_log = "led value is min.";
			ShowLog(temp_log);
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调低亮度
		++temp_int_led_value;
		p_ovs_device->Set_LEDCurrent(TenToHex(temp_int_led_value));
		thread hardware_control(&COVSApplicationDlg::ControlToHardware, this);
		hardware_control.detach();
		return OVS_ERR_ROI_TOOSTRONG;
	}
	if (image_info.mean_illumination < 80.0 && b_hardware_change)
	{
		string temp_log = "mean_illumination is too bright.";
		ShowLog(temp_log);
		string temp_led_lightness = "";
		p_ovs_device->Get_LEDCurrent(temp_led_lightness);
		// led亮度值 string转为int
		int temp_int_led_value = HexToTen(temp_led_lightness);

		if (temp_int_led_value <= 4)
		{
			string temp_log = "led value is max.";
			ShowLog(temp_log);
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调高亮度
		--temp_int_led_value;
		p_ovs_device->Set_LEDCurrent(TenToHex(temp_int_led_value));
		thread hardware_control(&COVSApplicationDlg::ControlToHardware, this);
		hardware_control.detach();
		return OVS_ERR_ROI_TOOWEAK;
	}


	// 查找ROI区域
	uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
	if (p_palm_parser->FindRoi(nullptr, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		string temp_log = "fail to extract roi.";
		ShowLog(temp_log);
		return OVS_ERR_ROI_INVALID_POSITION;
	}

	uint8_t* roi_data = new uint8_t[roi_byte_size];
	if (p_palm_parser->FindRoi(roi_data, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		string temp_log = "fail to extract roi.";
		ShowLog(temp_log);
		delete[] roi_data;
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	std::vector<uint8_t> feat;
	if (p_palm_parser->computeFeature(feat) != OVS_SUCCESS)
	{
		string temp_log = "fail to computeFeature";
		ShowLog(temp_log);
		return OVS_ERR_GET_FEATURE_FAILED;
	}

	// 所有分数 测试使用
	vector<double> user_score;
	// 识别出的用户id
	string user_ok = "";
	// 识别分数
	double idenfiy_source = 0.0f;
	for (auto& user_info : p_all_user_info_vector)
	{
		//double temp_source = p_palm_parser->compareFeature(&feat[0], (unsigned char*)user_info->template_str, feat.size());
		double temp_source = p_palm_parser->compareFeature(&feat[0], (unsigned char*)user_info->template_str.c_str(), feat.size());
		user_score.push_back(temp_source);
		if (temp_source > 0.75f && temp_source > idenfiy_source)
		{
			idenfiy_source = temp_source;
			user_ok = user_info->user_id;
		}
	}
	if (idenfiy_source > 0.75f)
	{
		string temp_log1 = to_string(idenfiy_source);
		ShowLog(temp_log1);
		string message_str2 = "识别的用户为 " + user_ok;
		MessageBoxW(CA2T(message_str2.c_str()), NULL, MB_YESNO);
	}
	else
	{
		return OVS_ERR_MATCHING_FAILED;
	}

	return ENUM_OK;
}

bool COVSApplicationDlg::UserRegister()
{
	// 设置默认值 因为现在LED灯没有随着距离改变
	DeviceParamInit();
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.release();
	}

	string message_str = "掌静脉信息开始采集。";
	{
		lock_guard<mutex> lk(ovs_mutex);
		template_num = 0;
	}

	while (b_user_register)
		/*while ((current_template_num < 10) ?
			(IDYES == MessageBoxW(CA2T(message_str.c_str()), NULL, MB_YESNO)) :
			(IDYES == MessageBoxW(_T("掌静脉采集完成，是否继续添加掌静脉信息"), NULL, MB_YESNO)))*/
	{
		if (template_num >= template_limitation)
		{
			b_user_register = true;
			break;
		}

		if (distance_value > 240)
		{
			string temp_log = "请将手掌靠近设备.";
			ShowLog(temp_log);
			continue;
		}
		// 由于设备硬件问题，设备运行随机出现<30
		if (distance_value < 60 && distance_value > 30)
		{
			string temp_log = "请将手掌远离设备.";
			ShowLog(temp_log);
			continue;
		}
		// 采集图像
		{
			lock_guard<mutex> lk(ovs_mutex);
			common_mat.copyTo(temp_register_mat);
			if (temp_register_mat.empty())
			{
				continue;
			}
		}
		if (b_get_template == false)
		{
			thread user_register_template_thread(&COVSApplicationDlg::GetTemplateTime, this);
			user_register_template_thread.join();
		}
		if (GetTemplate(temp_register_mat) != OVS_SUCCESS)
		{
			string temp_log = "fail to GetTemplate.";
			ShowLog(temp_log);
			continue;
		}
		else
		{
			lock_guard<mutex> lk(ovs_mutex);
			b_get_template = false;
		}

		/*++current_template_num;
		message_str = "掌静脉信息第 " + to_string(current_template_num) + " 次采集。";*/
	}
	/*current_template_num = 1;
	b_user_register = false;*/

	// 将用户信息添加到数据库
	if (!p_sqlite_control->AddUserData(p_user_info_vector))
	{
		string temp_log = "fail to add user data.";
		ShowLog(temp_log);
		return OVS_ERR_ROI_INVALID_POSITION;
	}
	p_all_user_info_vector.insert(p_all_user_info_vector.end(), p_user_info_vector.begin(), p_user_info_vector.end());
	p_user_info_vector.clear();
	return true;
}

EProcessingState COVSApplicationDlg::GetTemplate(Mat& in_mat)
{
	// 算法
	shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);

	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);

	SPalmImageInfo image_info;

	// 解析图像
	if (p_palm_parser->ParseInfo(image_info) != OVS_SUCCESS)
	{
		string temp_log = "fail to parse palm infomation.";
		ShowLog(temp_log);
		return OVS_ERR_NO_FOUND_PALM;
	}

	// 模糊度
	if (image_info.score > 12.0f)
	{
		string temp_log = "roi is too blurry.";
		ShowLog(temp_log);
		return OVS_ERR_ROI_BLURRY;
	}
	// 平均亮度
	string temp_log = to_string(image_info.mean_illumination);
	ShowLog(temp_log);
	if (image_info.mean_illumination > 140.0)
	{
		string temp_log = "mean_illumination is too strong.";
		ShowLog(temp_log);
		string temp_LED_lightness = "";
		p_ovs_device->Get_LEDCurrent(temp_LED_lightness);
		// led亮度值 string转为int
		int temp_int_LED_value = HexToTen(temp_LED_lightness);
		if (temp_int_LED_value == 8)
		{
			string temp_log = "led value is min.";
			ShowLog(temp_log);
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调低亮度
		++temp_int_LED_value;
		p_ovs_device->Set_LEDCurrent(TenToHex(temp_int_LED_value));
		string temp_LED_lightness1 = "";
		p_ovs_device->Get_LEDCurrent(temp_LED_lightness1);
		return OVS_ERR_ROI_TOOSTRONG;
	}

	if (image_info.mean_illumination < 80.0)
	{
		string temp_log = "mean_illumination is too bright.";
		ShowLog(temp_log);
		string temp_led_lightness = "";
		p_ovs_device->Get_LEDCurrent(temp_led_lightness);
		// led亮度值 string转为int 
		int temp_int_led_value = HexToTen(temp_led_lightness);
		if (temp_int_led_value == 4)
		{
			string temp_log = "led value is max.";
			ShowLog(temp_log);
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调高亮度
		--temp_int_led_value;
		p_ovs_device->Set_LEDCurrent(TenToHex(temp_int_led_value));
		string temp_LED_lightness1 = "";
		p_ovs_device->Get_LEDCurrent(temp_LED_lightness1);
		return OVS_ERR_ROI_TOOWEAK;
	}

	// 查找ROI区域
	uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
	if (p_palm_parser->FindRoi(nullptr, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		string temp_log = "fail to extract roi.";
		ShowLog(temp_log);
		return OVS_ERR_ROI_INVALID_POSITION;
	}

	uint8_t* roi_data = new uint8_t[roi_byte_size];
	if (p_palm_parser->FindRoi(roi_data, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		string temp_log = "fail to extract roi.";
		ShowLog(temp_log);
		delete[] roi_data;
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	std::vector<uint8_t> feat;
	if (p_palm_parser->computeFeature(feat) != OVS_SUCCESS)
	{
		string temp_log = "fail to computeFeature";
		ShowLog(temp_log);
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	char temp_feat[256];

	for (int i = 0; i < feat.size(); ++i)
	{
		temp_feat[i] = feat[i];
	}

	shared_ptr<SSqliteUserInfo> p_user_info = make_shared<SSqliteUserInfo>(current_user_id, temp_feat);
	p_user_info_vector.push_back(p_user_info);
	return OVS_SUCCESS;
}

bool COVSApplicationDlg::UserIdenfiyTimeControl()
{
	clock_t now = clock();
	while (clock() - now < 30000);
	b_user_idenfiy = false;
	return 0;
}

bool COVSApplicationDlg::ControlToHardware()
{
	clock_t now = clock();
	while (clock() - now < 25);
	b_user_idenfiy = true;
	return 0;
}

bool COVSApplicationDlg::GetTemplateTime()
{

	clock_t now = clock();
	while (clock() - now < 25);

	{
		lock_guard<mutex> lk(ovs_mutex);
		b_get_template = true;
		++template_num;
	}

	return true;
}

int COVSApplicationDlg::ListDevices()
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

bool COVSApplicationDlg::DeviceParamInit()
{
	// 默认增益
	int default_gain = 10;
	if (p_ovs_device->Set_Gain(TenToHex(default_gain)) != S_OK)
	{
		return false;
	}

	//  默认曝光
	int default_exp = 40;
	if (p_ovs_device->Set_Exposure(TenToHex(default_exp)) != S_OK)
	{
		return false;
	}
	// 默认LED
	int default_LED_lightness = 5;
	if (p_ovs_device->Set_LEDCurrent(TenToHex(default_LED_lightness)) != S_OK)
	{
		return false;
	}
	string led_value = "";
	p_ovs_device->Get_LEDCurrent(led_value);
	// 默认图像压缩率
	string default_compression_ratio = "05";
	if (p_ovs_device->Set_CompressionRatio(default_compression_ratio) != S_OK)
	{
		return false;
	}
	string default_BL = "01";
	if (p_ovs_device->Set_BL(default_BL) != S_OK)
	{
		return false;
	}
	return true;
}

void COVSApplicationDlg::MatToCImage(Mat& mat, CImage& cimage)
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

void COVSApplicationDlg::CImageToMat(CImage& cimage, Mat& mat)
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

void COVSApplicationDlg::ImageTranslate(Mat& out_mat, Mat& in_mat)
{
	Mat ok_dst(in_mat, Range(0, 1080), Range(540, 1620));
	out_mat = Mat(ok_dst.rows, ok_dst.cols, ok_dst.depth());
	transpose(ok_dst, out_mat);
	//flip(ok_dst_copy, ok_dst_copy, 1);
}

void COVSApplicationDlg::DistanceOfDevice()
{
	string distance_str = "";
	while (true)
	{
		//Sleep(1);
		p_ovs_device->Get_Distance(distance_str);
		distance_value = HexToTen(distance_str);
		ShowDistance(to_string(distance_value));
	}
}

string COVSApplicationDlg::StaticImageFileName()
{
	string temp_suffix = ".bmp";

	string filename = "./image/a" + temp_suffix;
	return filename;
}

void COVSApplicationDlg::ShowLog(string& msg_str)
{
	CString Show_str;
	CTime time = CTime::GetCurrentTime();
	CString sTime = time.Format(_T("[%Y/%B/%d, %H:%M:%S]   "));
	CString temp_Msg = sTime + msg_str.c_str() + _T("\r\n");
	Show_str += temp_Msg;

	ASSERT(edit_log && edit_log.GetSafeHwnd());
	edit_log.SetSel(-1, true);
	edit_log.ReplaceSel(Show_str);
	int line_numbers = edit_log.GetLineCount();
	if (line_numbers > 500)
	{
		GetDlgItem(IDC_EDIT_LOG)->SetWindowText(_T(""));
	}
}

void COVSApplicationDlg::ShowDistance(string& msg_str)
{
	CString show_distance_str;
	CTime time = CTime::GetCurrentTime();
	CString sTime = time.Format(_T("[%Y/%B/%d, %H:%M:%S]   "));
	CString temp_Msg = sTime + msg_str.c_str() + _T("\r\n");
	show_distance_str += temp_Msg;

	ASSERT(edit_distance_log && edit_distance_log.GetSafeHwnd());
	edit_distance_log.SetSel(-1, true);
	edit_distance_log.ReplaceSel(show_distance_str);
	int line_numbers = edit_distance_log.GetLineCount();
	if (line_numbers > 500)
	{
		GetDlgItem(IDC_EDIT_DISTANCE_LOG)->SetWindowText(_T(""));
	}
}

const int COVSApplicationDlg::HexToTen(string& in_str)
{
	int dec_out = stoi(in_str, nullptr, 16);
	return dec_out;
}

const std::string COVSApplicationDlg::TenToHex(int& in_hec)
{
	string out;
	stringstream ss;
	ss << std::hex << in_hec;
	ss >> out;
	transform(out.begin(), out.end(), out.begin(), ::toupper);
	return out;
}

void COVSApplicationDlg::OnBnClickedButtonOpenDevice()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	button_open_device.EnableWindow(false);
	button_close_device.EnableWindow(true);
	button_check_user.EnableWindow(true);
	button_user_register.EnableWindow(false);
	button_user_idenfiy.EnableWindow(true);

	thread capture_thread(&COVSApplicationDlg::ImageOutputToScreen, this);
	thread distance_thread(&COVSApplicationDlg::DistanceOfDevice, this);
	capture_thread.detach();
	distance_thread.detach();


	UpdateData(false);
}

void COVSApplicationDlg::OnBnClickedButtonUserRegister()
{
	// TODO: 在此添加控件通知处理程序代码
	thread user_register_thread(&COVSApplicationDlg::UserRegister, this);

	user_register_thread.detach();
	b_user_register = true;
}

void COVSApplicationDlg::OnBnClickedButtonUserIdenfiy()
{
	// TODO: 在此添加控件通知处理程序代码
	thread user_idenfiy_thread(&COVSApplicationDlg::UserIdenfiy, this);
	//thread user_idenfiy_time_control_thread(&COVSApplicationDlg::UserIdenfiyTimeControl, this);
	user_idenfiy_thread.detach();
	//user_idenfiy_time_control_thread.detach();
	b_user_idenfiy = true;
}


void COVSApplicationDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	p_ovs_device->CloseDevice();
	HANDLE MyProcess = GetCurrentProcess();
	TerminateProcess(MyProcess, 0);
	UpdateData(false);
}


void COVSApplicationDlg::OnBnClickedButtonCheckUser()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	// 获取用户ID
	GetDlgItem(IDC_EDIT_USER_ID)->GetWindowTextW(edit_user_id_str);

	// 通过用户ID获取信息
	current_user_id = CT2A(edit_user_id_str.GetBuffer());

	p_sqlite_control->SelectUserData(current_user_id, p_user_info_vector);
	// 测试
	//ofstream out_file("test.txt");

	//out_file << p_user_info_vector[8]->template_str;

	//out_file.close();


	if (!p_user_info_vector.empty())
	{
		if (IDYES == MessageBoxW(_T("用户已注册，是否重新注册录入"), NULL, MB_YESNO))
		{
			p_sqlite_control->DeleteUserData(current_user_id);

			vector<shared_ptr<SSqliteUserInfo>>::iterator iter;
			for (iter = p_all_user_info_vector.begin(); iter != p_all_user_info_vector.end();)
			{
				if (iter->get()->user_id == current_user_id)
				{
					iter = p_all_user_info_vector.erase(iter);
				}
				else
				{
					++iter;
				}
			}
			p_user_info_vector.clear();
			button_user_register.EnableWindow(true);
		}
		else
		{
			button_user_register.EnableWindow(false);
		}
	}
	else
	{
		button_user_register.EnableWindow(true);
	}
	UpdateData(false);
}


void COVSApplicationDlg::OnBnClickedButtonIdentifyClose()
{
	b_user_idenfiy = false;
}
