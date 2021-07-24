
// OVSMFCUserDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "OVSMFCUser.h"
#include "OVSMFCUserDlg.h"
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


// COVSMFCUserDlg 对话框



COVSMFCUserDlg::COVSMFCUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OVSMFCUSER_DIALOG, pParent)
	, b_device_status(false)
	, edit_user_id_str(_T(""))
	, b_user_register(false)
	, b_user_idenfiy(false)
	, b_save_photo(false)
	, b_distance_ok(false)
	, user_success_score(0)
	, template_num(0)
	, b_get_template(false)
	, b_idenfiy_success(false)
	, no_palm_photo_number(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COVSMFCUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPENDEVICE, button_open_device);
	DDX_Control(pDX, IDC_BUTTON_CLOSEDEVICE, button_close_device);
	DDX_Control(pDX, IDC_EDIT_USER_ID, edit_user_id);
	DDX_Control(pDX, IDC_STATIC_USER_ID, static_user_id);
	DDX_Text(pDX, IDC_EDIT_USER_ID, edit_user_id_str);
	DDX_Control(pDX, IDC_BUTTON_USER_IDENFIY, button_user_idenfiy);
	DDX_Control(pDX, IDC_BUTTON_USER_REGISTER, button_user_register);
	DDX_Control(pDX, IDC_BUTTON_STOP, button_stop);
	DDX_Control(pDX, IDC_PROGRESS1, progress_ctrl_register);
}

BEGIN_MESSAGE_MAP(COVSMFCUserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPENDEVICE, &COVSMFCUserDlg::OnBnClickedButtonOpendevice)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEDEVICE, &COVSMFCUserDlg::OnBnClickedButtonClosedevice)
	ON_BN_CLICKED(IDC_BUTTON_USER_REGISTER, &COVSMFCUserDlg::OnBnClickedButtonUserRegister)
	ON_BN_CLICKED(IDC_BUTTON_USER_IDENFIY, &COVSMFCUserDlg::OnBnClickedButtonUserIdenfiy)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &COVSMFCUserDlg::OnBnClickedButtonStop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &COVSMFCUserDlg::OnNMCustomdrawProgress1)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &COVSMFCUserDlg::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// COVSMFCUserDlg 消息处理程序

BOOL COVSMFCUserDlg::OnInitDialog()
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
	button_open_device.EnableWindow(true);
	button_close_device.EnableWindow(false);
	button_user_register.EnableWindow(false);
	button_user_idenfiy.EnableWindow(false);
	button_stop.EnableWindow(false);
	button_save_photo.EnableWindow(false);
	progress_ctrl_register.SetRange(0, template_num_limit);
	progress_ctrl_register.SetPos(0);
	RunInit();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COVSMFCUserDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COVSMFCUserDlg::OnPaint()
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
HCURSOR COVSMFCUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool COVSMFCUserDlg::RunInit()
{
	// 关闭控制台
	FreeConsole();

	// log设置
	string log_name = "basic_logger";
	string log_file_name = "logs/basic.txt";
	// p_logger = basic_logger_mt(log_name, log_file_name);
	// p_logger->flush_on(level::warn);

	p_device_control = make_shared<OVSDevice>();
	p_sqlite_control = make_shared<OVSSqlite>();

	p_serial_port = make_shared<SerialPort>();       //add by xhb

	if (!p_serial_port->SerialPortInit("0483", "5740"))
	{
		//显示log
		//ShowLog(string("motor connect false--->"), m_opt_log, IDC_EDIT_DISTANCE_LOG);
		return false;
	}

	// 设备SDK初始化
	if (!p_device_control->DeviceInit())
	{
		return false;
	}

	p_device_control->Set_BL("00");
	p_device_control->Set_Exposure("35");

	// 读取数据库所有信息
	/*if (!p_sqlite_control->GetUserData(p_all_user_info_vector))
	{

		return false;
	}*/
	p_serial_port->ledHeartBeat();
	return true;
}

void COVSMFCUserDlg::DistanceOfDevice()
{
	string distance_str = "";
	while (true)
	{
		p_device_control->Get_Distance(distance_str);
		distance_value = HexToTen(distance_str);
		if (distance_value > palm_distance_upper_limit || distance_value < palm_distance_lower_limit)
		{
			lock_guard<mutex> lk(ovs_mutex);
			b_distance_ok = false;

			if (command_flag == false) //add by xhb
			{
				command_flag = true;
				p_serial_port->ledHeartBeat();
			}
		}
		else
		{
			lock_guard<mutex> lk(ovs_mutex);
			b_distance_ok = true;
		}
	}
}

bool COVSMFCUserDlg::ExtractImageFromDevice()
{
	int camera_id = GetCameraID();
	VideoCapture capture(camera_id + cv::CAP_DSHOW);
	capture.set(CAP_PROP_FRAME_WIDTH, 1920);
	capture.set(CAP_PROP_FRAME_HEIGHT, 1080);
	// 设置设备默认参数
	if (DeviceParamInit() == false)
	{
		return false;
	}
	// 从摄像头读取的Mat
	Mat frame/*(1080,1920, CV_8UC3)*/;
	while (1)
	{
		if (b_device_status == false)
		{
			break;
		}
		capture >> frame;
		if (frame.empty())
		{
			continue;
		}
		lock_guard<mutex> lk(ovs_mutex);
		frame.copyTo(common_mat);
	}
	capture.release();
	return true;
}

bool COVSMFCUserDlg::ProcessImage()
{
	DeviceParamInit();

	p_image_info = make_shared<SImageInfo>("", "", 0, 0, 0.0f, 0.0f, 6, 0);

	// 获得pictrue控件窗口的句柄 
	p_wnd = shared_ptr<CWnd>((GetDlgItem(IDC_STATIC_PICTURE_CAMERA)));
	// 获得pictrue控件的D
	p_dc = shared_ptr<CDC>(p_wnd->GetDC());

	GetClientRect(&display_rect);
	GetDlgItem(IDC_STATIC_PICTURE_CAMERA)->GetClientRect(&display_rect);

	namedWindow("ImageShow");
	HWND hWnd = (HWND)cvGetWindowHandle("ImageShow");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_PICTURE_CAMERA)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);

	while (b_device_status)
	{
		// 用户注册
		if (b_user_register)
		{
			UserRegister();
		}
		else if (b_user_idenfiy)
		{
			UserIdenfiy();
		}
		else if (b_save_photo)
		{
			SavePhoto();
		}
		else
		{
			{
				lock_guard<mutex> lk(ovs_mutex);
				common_mat.copyTo(process_mat);
				if (process_mat.empty())
				{
					continue;
				}
			}
			DisplayImageCommon(process_mat);
		}
	}
	p_wnd->ReleaseDC(p_dc.get());
	return true;
}

bool COVSMFCUserDlg::DisplayImageRegister(Mat& in_mat)
{
	// 显示到屏幕上，并完成旋转，绘制等操作
	Mat translate_dst;
	ImageTranslate(translate_dst, in_mat);
	resize(translate_dst, display_dst, Size(display_rect.Width(), display_rect.Height()));
	if ((p_image_info->center_x != 0 || p_image_info->center_x != 0) && distance_value < palm_distance_upper_limit)
	{
		// 画圆 
		circle(display_dst, Point(display_rect.Width() - p_image_info->center_x - palm_circle_offset, p_image_info->center_x + palm_circle_offset), palm_circle_radius, CV_RGB(225.0f, 85.0f, 1.0f), 2);
		// 写字
		int baseline = 0;
		int distance_font = 30;
		//ceshi 
		string temp_user_id_str = "User_Id      : " + p_image_info->user_id;

		Size user_id_text_size = getTextSize(temp_user_id_str, font_face, font_scale, thickness, &baseline);
		Point user_id_origin;
		user_id_origin.x = 20;
		distance_font += user_id_text_size.height;
		user_id_origin.y = distance_font;
		putText(display_dst, temp_user_id_str, user_id_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// roi中心坐标
		string temp_center_str = "Roi_Center  : (" + to_string(p_image_info->center_x) + "," + to_string(p_image_info->center_y) + ")";
		Size center_text_size = getTextSize(temp_center_str, font_face, font_scale, thickness, &baseline);
		Point center_origin;
		center_origin.x = 20;
		distance_font += center_text_size.height;
		center_origin.y = distance_font + 10;
		putText(display_dst, temp_center_str, center_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// 模糊度
		string temp_ambiguity_str = "Ambiguity   : " + to_string(p_image_info->ambiguity);
		Size ambiguity_text_size = getTextSize(temp_ambiguity_str, font_face, font_scale, thickness, &baseline);
		Point ambiguity_origin;
		ambiguity_origin.x = 20;
		distance_font += ambiguity_text_size.height;
		ambiguity_origin.y = distance_font + 20;
		putText(display_dst, temp_ambiguity_str, ambiguity_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// 平均亮度
		string temp_mean_illumination_str = "Illumination : " + to_string(p_image_info->mean_illumination);
		Size mean_illumination_text_size = getTextSize(temp_mean_illumination_str, font_face, font_scale, thickness, &baseline);
		Point mean_illumination_origin;
		mean_illumination_origin.x = 20;
		distance_font += mean_illumination_text_size.height;
		mean_illumination_origin.y = distance_font + 30;
		putText(display_dst, temp_mean_illumination_str, mean_illumination_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// led亮度
		string temp_led_value_str = "Led          : " + to_string(p_image_info->led_value);
		Size led_value_text_size = getTextSize(temp_led_value_str, font_face, font_scale, thickness, &baseline);
		Point led_value_origin;
		led_value_origin.x = 20;
		distance_font += led_value_text_size.height;
		led_value_origin.y = distance_font + 40;
		putText(display_dst, temp_led_value_str, led_value_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// 距离
		string temp_distance_value_str = "Distance     : " + to_string(distance_value);
		Size distance_value_text_size = getTextSize(temp_distance_value_str, font_face, font_scale, thickness, &baseline);
		Point distance_value_origin;
		distance_value_origin.x = 20;
		distance_font += distance_value_text_size.height;
		distance_value_origin.y = distance_font + 50;
		putText(display_dst, temp_distance_value_str, distance_value_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);
	}
	imshow("ImageShow", display_dst);
	waitKey(1);
	return true;
}

bool COVSMFCUserDlg::DisplayImageIdenfiy(Mat& in_mat)
{
	// 显示到屏幕上，并完成旋转，绘制等操作
	Mat translate_dst;
	ImageTranslate(translate_dst, in_mat);
	resize(translate_dst, display_dst, Size(display_rect.Width(), display_rect.Height()));
	if ((p_image_info->center_x != 0 || p_image_info->center_x != 0) && distance_value < palm_distance_upper_limit)
	{
		// 写字
		int baseline = 0;
		int distance_font = 30;

		// 画圆 
		Scalar circle_RGB(0.0f, 0.0f, 0.0f, 0.0f);
		if (user_success_score > idenfiy_success_limit && b_idenfiy_success)
		{
			circle_RGB = Scalar(140.0f, 199.0f, 0.0f);
			string temp_user_id_str = "User id : " + user_success_id;

			Size user_id_text_size = getTextSize(temp_user_id_str, font_face, font_scale, thickness, &baseline);
			Point user_id_origin;
			user_id_origin.x = 20;
			distance_font += user_id_text_size.height;
			user_id_origin.y = distance_font;
			putText(display_dst, temp_user_id_str, user_id_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);
		}
		else
		{
			circle_RGB = Scalar(13.0f, 23.0f, 227.0f);
			string temp_user_id_str = "User id : ";

			Size user_id_text_size = getTextSize(temp_user_id_str, font_face, font_scale, thickness, &baseline);
			Point user_id_origin;
			user_id_origin.x = 20;
			distance_font += user_id_text_size.height;
			user_id_origin.y = distance_font;
			putText(display_dst, temp_user_id_str, user_id_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);
			p_serial_port->ledColorControl("255,0,0"); //add by xhb
			command_flag = false;
		}
		circle(display_dst, Point(display_rect.Width() - p_image_info->center_x - palm_circle_offset, p_image_info->center_x + palm_circle_offset), palm_circle_radius, circle_RGB, 2);
		rectangle(display_dst, Point(2.5, 2.5), Point(display_rect.Width() - 5, display_rect.Height() - 5), circle_RGB, 5);

		string temp_score_str = "Score   : " + to_string(user_success_score);
		Size score_text_size = getTextSize(temp_score_str, font_face, font_scale, thickness, &baseline);
		Point score_origin;
		score_origin.x = 20;
		distance_font += score_text_size.height;
		score_origin.y = distance_font + 20;
		putText(display_dst, temp_score_str, score_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

		// 距离
		string temp_distance_value_str = "Distance     : " + to_string(distance_value);
		Size distance_value_text_size = getTextSize(temp_distance_value_str, font_face, font_scale, thickness, &baseline);
		Point distance_value_origin;
		distance_value_origin.x = 20;
		distance_font += distance_value_text_size.height;
		distance_value_origin.y = distance_font + 30;
		putText(display_dst, temp_distance_value_str, distance_value_origin, font_face, font_scale, Scalar(0, 255, 255), thickness, 8, 0);

	}
	imshow("ImageShow", display_dst);
	waitKey(1);
	return true;
}

bool COVSMFCUserDlg::DisplayImageCommon(Mat& in_mat)
{
	Mat translate_dst;
	ImageTranslate(translate_dst, in_mat);
	resize(translate_dst, display_dst, Size(display_rect.Width(), display_rect.Height()));


	imshow("ImageShow", display_dst);
	waitKey(1);
	return true;
}

bool COVSMFCUserDlg::UserRegister()
{
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.release();
	}

	// 设置默认值 因为现在LED灯没有随着距离改变
	DeviceParamInit();
	{
		lock_guard<mutex> lk(ovs_mutex);
		template_num = 0;
	}
	// 注册终止打开
	b_exit_register = false;
	progress_ctrl_register.SetPos(template_num);
	// 显示 临时图像
	Mat temp_display_mat;
	while (b_user_register)
	{
		if (template_num >= template_num_limit)
		{
			b_user_register = false;
			break;
		}
		// 采集图像
		{
			lock_guard<mutex> lk(ovs_mutex);
			common_mat.copyTo(process_mat);
			if (process_mat.empty())
			{
				continue;
			}
		}
		if (distance_value > palm_distance_upper_limit)
		{
			string temp_log = "手掌距离采集设备太远";
			DisplayImageCommon(process_mat);
			continue;
		}
		// 由于设备硬件问题，设备运行随机出现<30
		else if (distance_value < palm_distance_lower_limit)
		{
			string temp_log = "手掌距离采集设备太近";
			DisplayImageCommon(process_mat);
			continue;
		}
		if (b_get_template == false)
		{
			thread user_register_template_thread(&COVSMFCUserDlg::GetRegisterTemplateTime, this);
			user_register_template_thread.join();
		}
		if (GetRegisterTemplate(process_mat) != OVS_SUCCESS)
		{
			string temp_log = "fail to GetTemplate.";
			DisplayImageRegister(process_mat);
			continue;
		}
		else
		{
			progress_ctrl_register.SetPos(template_num);
			if (template_num >= template_num_limit - 1)
			{
				progress_ctrl_register.SetBarColor(0x0000FF00); //add by xhb
			}
			else
			{
				progress_ctrl_register.SetBarColor(0x000000FF); //add by xhb
			}
			DisplayImageRegister(process_mat);
			lock_guard<mutex> lk(ovs_mutex);
			b_get_template = false;
		}
	}
	if (b_exit_register)
	{
		string temp_log = "用户取消注册";
		p_serial_port->ledHeartBeat(); //add by xhb
		return true;
	}
	// 将用户信息添加到数据库
	/*if (!p_sqlite_control->AddUserData(p_user_info_vector))
	{
		return false;
	}*/
	p_all_user_info_vector.insert(p_all_user_info_vector.end(), p_user_info_vector.begin(), p_user_info_vector.end());
	p_user_info_vector.clear();
	string temp_log = "用户注册完成";
	p_serial_port->ledHeartBeat();  //add by xhb

	return true;
}

EProcessingState COVSMFCUserDlg::GetRegisterTemplate(Mat& in_mat)
{
	// 算法
	shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);
	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);
	shared_ptr<SPalmImageInfo> temp_image_info = make_shared<SPalmImageInfo>();
	// 解析图像
	if (p_palm_parser->ParseInfo(temp_image_info) != OVS_SUCCESS)
	{
		return OVS_ERR_NO_FOUND_PALM;
	}

	p_image_info->center_x = temp_image_info->center_x;
	p_image_info->center_y = temp_image_info->center_y;
	p_image_info->ambiguity = temp_image_info->score;
	p_image_info->mean_illumination = temp_image_info->mean_illumination;


	// 模糊度
	if (p_image_info->ambiguity > ambiguity_limit)
	{
		return OVS_ERR_ROI_BLURRY;
	}
	// 平均亮度
	string temp_log = "mean illumination is " + to_string(p_image_info->mean_illumination);

	if (p_image_info->mean_illumination > mean_illumination_upper_limit)
	{
		if (p_image_info->led_value >= led_value_upper_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调低亮度
		++p_image_info->led_value;
		p_device_control->Set_LEDCurrent(TenToHex(p_image_info->led_value));
		return OVS_ERR_ROI_TOOSTRONG;
	}
	if (p_image_info->mean_illumination < mean_illumination_lower_limit)
	{
		if (p_image_info->led_value <= led_value_lower_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调高亮度
		--p_image_info->led_value;
		p_device_control->Set_LEDCurrent(TenToHex(p_image_info->led_value));
		return OVS_ERR_ROI_TOOWEAK;
	}

	// 查找ROI区域
	uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
	if (p_palm_parser->FindRoi(nullptr, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		return OVS_ERR_ROI_INVALID_POSITION;
	}

	uint8_t* roi_data = new uint8_t[roi_byte_size];
	if (p_palm_parser->FindRoi(roi_data, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		delete[] roi_data;
		return OVS_ERR_GET_FEATURE_FAILED;
	}
	std::vector<float> feat;
	if (p_palm_parser->computeFeature(feat) != OVS_SUCCESS)
	{
		return OVS_ERR_GET_FEATURE_FAILED;
	}/*
	char temp_feat[256];

	for (int i = 0; i < feat.size(); ++i)
	{
		temp_feat[i] = feat[i];
	}*/
	shared_ptr<SSqliteUserInfo> p_user_info = make_shared<SSqliteUserInfo>(p_image_info->user_id, feat);
	p_user_info_vector.push_back(p_user_info);

	return OVS_SUCCESS;
}

bool COVSMFCUserDlg::GetRegisterTemplateTime()
{
	clock_t now = clock();
	while (clock() - now < 60);

	{
		lock_guard<mutex> lk(ovs_mutex);
		b_get_template = true;
		++template_num;
	}
	return true;
}

bool COVSMFCUserDlg::SavePhoto()
{
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.release();
	}

	// 设置默认值 因为现在LED灯没有随着距离改变
	DeviceParamInit();
	// 显示 临时图像
	Mat temp_display_mat;
	while (b_save_photo)
	{
		// 采集图像
		{
			lock_guard<mutex> lk(ovs_mutex);
			common_mat.copyTo(process_mat);
			if (process_mat.empty())
			{
				continue;
			}
		}
		if (distance_value > palm_distance_upper_limit)
		{
			string temp_log = "手掌距离采集设备太远";
			DisplayImageCommon(process_mat);
			continue;
		}
		// 由于设备硬件问题，设备运行随机出现<30
		else if (distance_value < palm_distance_lower_limit)
		{
			string temp_log = "手掌距离采集设备太近";
			DisplayImageCommon(process_mat);
			continue;
		}
		if (GetNoPalmPhoto(process_mat))
		{
			string temp_log = "fail to GetTemplate.";
			DisplayImageCommon(process_mat);
			++no_palm_photo_number;
			imwrite("./palmimage/" + to_string(no_palm_photo_number) + ".bmp", process_mat);
			while (1)
			{
				lock_guard<mutex> lk(ovs_mutex);
				if (b_distance_ok == false)
				{
					return true;
				}
			}
		}
		else
		{
			DisplayImageCommon(process_mat);
			continue;
		}
	}
	return true;
}

bool COVSMFCUserDlg::GetNoPalmPhoto(Mat& in_mat)
{
	// 算法

	shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);

	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);
	shared_ptr<SPalmImageInfo> temp_image_info = make_shared<SPalmImageInfo>();
	// 分析手是否存在
	list<Point> finger_pts;
	if (p_palm_parser->extract_fingers(finger_pts))
	{
		return true;
	}
	return false;
}

bool COVSMFCUserDlg::UserIdenfiy()
{
	{
		lock_guard<mutex> lk(ovs_mutex);
		common_mat.release();
	}
	// 设置默认值 因为现在LED灯没有随着距离改变
	DeviceParamInit();/*
	success_user_list.clear();
	success_user_list.push_back("test1");
	success_user_list.push_back("test22");*/
	b_idenfiy_success = false;
	while (b_user_idenfiy)
	{
		// 采集图像
		{
			lock_guard<mutex> lk(ovs_mutex);
			common_mat.copyTo(process_mat);
			if (process_mat.empty())
			{
				continue;
			}
		}
		if (distance_value > palm_distance_upper_limit)
		{
			string temp_log = "手掌距离采集设备太远";
			DisplayImageCommon(process_mat);
			continue;
		}
		// 由于设备硬件问题，设备运行随机出现<30
		else if (distance_value < palm_distance_lower_limit)
		{
			string temp_log = "手掌距离采集设备太近";
			DisplayImageCommon(process_mat);
			continue;
		}

		if (GetIdenfiyTemplate(process_mat) != ENUM_OK)
		{
			string temp_log = "fail to idenify.";
			DisplayImageIdenfiy(process_mat);
			continue;
		}
		else
		{
			b_idenfiy_success = true;
			DisplayImageIdenfiy(process_mat);
			string temp_log = "比对成功";
			p_serial_port->ledColorControl("0,255,0"); //add by xhb
			while (1)
			{
				lock_guard<mutex> lk(ovs_mutex);
				if (b_distance_ok == false)
				{
					p_serial_port->ledHeartBeat();
					return true;
				}
			}
			// 判定list中满足连续二个相同
			/*bool b_temp_user_same = true;
			for (auto& user : success_user_list)
			{
				if (user != user_success_id)
				{
					b_temp_user_same = false;
				}
			}*/
			/*if (b_temp_user_same == true)
			{
				b_idenfiy_success = true;
				DisplayImageIdenfiy(process_mat);
				string temp_log = "比对成功";
				while (1)
				{
					lock_guard<mutex> lk(ovs_mutex);
					if (b_distance_ok == false)
					{
						return true;
					}
				}

			}*/
			/*else
			{
				DisplayImageIdenfiy(process_mat);
				success_user_list.pop_front();
				success_user_list.push_back(user_success_id);
				continue;
			}*/
		}
	}
	return false;
}

EProcessingState COVSMFCUserDlg::GetIdenfiyTemplate(Mat& in_mat)
{
	// 算法
	//shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);
	shared_ptr<OVSPalmParser> p_palm_parser = make_shared<OVSPalmParser>(in_mat);
	// 加载svm
	string temp_palm_svm_path = "palm_svm.yaml";
	string temp_finger_svm_path = "finger_svm.yaml";
	p_palm_parser->loadModelFiles(temp_palm_svm_path, temp_finger_svm_path);
	shared_ptr<SPalmImageInfo> temp_image_info = make_shared<SPalmImageInfo>();
	// 解析图像
	if (p_palm_parser->ParseInfo(temp_image_info) != OVS_SUCCESS)
	{
		return OVS_ERR_NO_FOUND_PALM;
	}
	p_image_info->center_x = temp_image_info->center_x;
	p_image_info->center_y = temp_image_info->center_y;
	p_image_info->ambiguity = temp_image_info->score;
	p_image_info->mean_illumination = temp_image_info->mean_illumination;


	// 模糊度
	if (p_image_info->ambiguity > ambiguity_limit)
	{
		return OVS_ERR_ROI_BLURRY;
	}
	// 平均亮度
	string temp_log = "mean illumination is " + to_string(p_image_info->mean_illumination);

	if (p_image_info->mean_illumination > mean_illumination_upper_limit)
	{
		if (p_image_info->led_value >= led_value_upper_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调低亮度
		++p_image_info->led_value;
		p_device_control->Set_LEDCurrent(TenToHex(p_image_info->led_value));
		return OVS_ERR_ROI_TOOSTRONG;
	}
	if (p_image_info->mean_illumination < mean_illumination_lower_limit)
	{

		if (p_image_info->led_value <= led_value_lower_limit)
		{
			return OVS_ERR_ROI_TOOSTRONG;
		}
		// 调高亮度
		--p_image_info->led_value;
		p_device_control->Set_LEDCurrent(TenToHex(p_image_info->led_value));
		return OVS_ERR_ROI_TOOWEAK;
	}

	// 查找ROI区域
	uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
	if (p_palm_parser->FindRoi(nullptr, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		return OVS_ERR_ROI_INVALID_POSITION;
	}

	uint8_t* roi_data = new uint8_t[roi_byte_size];
	if (p_palm_parser->FindRoi(roi_data, roi_width, roi_height, roi_byte_size) != OVS_SUCCESS)
	{
		// p_logger->warn("fail to extract roi!");

		delete[] roi_data;
		return OVS_ERR_GET_FEATURE_FAILED;
	}

	std::vector<float> feat;
	if (p_palm_parser->computeFeature(feat) != OVS_SUCCESS)
	{
		return OVS_ERR_GET_FEATURE_FAILED;
	}

	// 所有分数 测试使用
	vector<double> user_score;
	// 识别出的用户id
	user_success_id = "";
	// 识别分数
	float idenfiy_source = 0.0f;
	for (auto& user_info : p_all_user_info_vector)
	{
		double temp_source = p_palm_parser->compareFeature(&feat[0], &user_info->template_str[0], feat.size());
		temp_source = 1000 * (1 - temp_source);
		user_score.push_back(temp_source);
		if (temp_source > idenfiy_success_limit && temp_source > idenfiy_source)
		{
			idenfiy_source = temp_source;
			user_success_id = user_info->user_id;
		}
	}
	user_success_score = (idenfiy_source > idenfiy_success_limit) ? idenfiy_source : 0.0f;
	if (idenfiy_source < idenfiy_success_limit)
	{
		return OVS_ERR_MATCHING_FAILED;
	}
	return ENUM_OK;
}

bool COVSMFCUserDlg::UserIdenfiyTimeControl()
{

	return true;
}

bool COVSMFCUserDlg::GetCameraID()
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

bool COVSMFCUserDlg::DeviceParamInit()
{
	// 默认增益
	int default_gain = 10;
	if (p_device_control->Set_Gain(TenToHex(default_gain)) != S_OK)
	{
		return false;
	}

	//  默认曝光
	int default_exp = 40;
	if (p_device_control->Set_Exposure(TenToHex(default_exp)) != S_OK)
	{
		return false;
	}

	// 默认LED
	int default_LED_lightness = 5;
	if (p_device_control->Set_LEDCurrent(TenToHex(default_LED_lightness)) != S_OK)
	{
		return false;
	}
	// 默认图像压缩率
	string default_compression_ratio = "05";
	if (p_device_control->Set_CompressionRatio(default_compression_ratio) != S_OK)
	{
		return false;
	}
	return true;
}

const int COVSMFCUserDlg::HexToTen(string& in_str)
{
	int dec_out = stoi(in_str, nullptr, 16);
	return dec_out;
}

const string COVSMFCUserDlg::TenToHex(int& in_hec)
{
	string out;
	stringstream ss;
	ss << std::hex << in_hec;
	ss >> out;
	transform(out.begin(), out.end(), out.begin(), ::toupper);
	return out;
}


void COVSMFCUserDlg::MatToCImage(Mat& mat, CImage& cimage)
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

void COVSMFCUserDlg::ImageTranslate(Mat& out_mat, Mat& in_mat)
{
	Mat ok_dst(in_mat, Range(0, 1080), Range(420, 1500));
	out_mat = Mat(ok_dst.rows, ok_dst.cols, ok_dst.depth());
	transpose(ok_dst, out_mat);
}

void COVSMFCUserDlg::OnBnClickedButtonOpendevice()
{
	// TODO: 在此添加控件通知处理程序代码
	b_device_status = true;
	button_open_device.EnableWindow(false);
	button_close_device.EnableWindow(true);
	button_user_register.EnableWindow(true);
	button_user_idenfiy.EnableWindow(true);
	button_stop.EnableWindow(true);
	button_save_photo.EnableWindow(true);


	thread thread_open_device(&COVSMFCUserDlg::ExtractImageFromDevice, this);
	thread thread_display_image(&COVSMFCUserDlg::ProcessImage, this);

	thread thread_distance_device(&COVSMFCUserDlg::DistanceOfDevice, this);

	thread_open_device.detach();
	thread_display_image.detach();
	thread_distance_device.detach();
	UpdateData(false);
}


void COVSMFCUserDlg::OnBnClickedButtonClosedevice()
{
	// TODO: 在此添加控件通知处理程序代码
	p_serial_port->ledColorControl("0,0,0");
	b_device_status = false;
	p_device_control->CloseDevice();
	HANDLE MyProcess = GetCurrentProcess();
	TerminateProcess(MyProcess, 0);
}


void COVSMFCUserDlg::OnBnClickedButtonUserRegister()
{
	// TODO: 在此添加控件通知处理程序代码
	p_serial_port->ledColorControl("255,0,255"); //add by xhb
	UpdateData(true);
	// 获取用户ID
	GetDlgItem(IDC_EDIT_USER_ID)->GetWindowTextW(edit_user_id_str);

	// 通过用户ID获取信息
	p_image_info->user_id = CT2A(edit_user_id_str.GetBuffer());
	bool b_user_exist = false;
	for (auto& user_info : p_all_user_info_vector)
	{
		if (user_info->user_id == p_image_info->user_id)
		{
			b_user_exist = true;
			break;
		}
	}
	if (b_user_exist)
	{
		if (IDYES == MessageBoxW(_T("用户已注册，是否重新注册录入"), NULL, MB_YESNO))
		{
			// p_sqlite_control->DeleteUserData(p_image_info->user_id);

			vector<shared_ptr<SSqliteUserInfo>>::iterator iter;
			for (iter = p_all_user_info_vector.begin(); iter != p_all_user_info_vector.end();)
			{
				if (iter->get()->user_id == p_image_info->user_id)
				{
					iter = p_all_user_info_vector.erase(iter);
				}
				else
				{
					++iter;
				}
			}
			p_user_info_vector.clear();
			b_user_register = true;
		}
	}
	else
	{
		b_user_register = true;
	}
	button_stop.EnableWindow(true);
	button_user_idenfiy.EnableWindow(false);
	button_user_register.EnableWindow(false);

	UpdateData(false);
}

void COVSMFCUserDlg::OnBnClickedButtonUserIdenfiy()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	b_user_idenfiy = true;
	b_user_register = false;
	button_stop.EnableWindow(true);
	button_user_idenfiy.EnableWindow(false);
	button_user_register.EnableWindow(false);
	UpdateData(false);
}


void COVSMFCUserDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	b_user_idenfiy = false;
	b_user_register = false;
	b_save_photo = false;
	b_exit_register = true;
	button_stop.EnableWindow(false);
	button_user_idenfiy.EnableWindow(true);
	button_user_register.EnableWindow(true);
	button_save_photo.EnableWindow(true);
	progress_ctrl_register.SetPos(0);
	UpdateData(false);
}


void COVSMFCUserDlg::OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void COVSMFCUserDlg::to_hex(char* in_char, int char_length, char* out_char) //add by xhb
{
	while (char_length--)
	{
		*out_char = (*in_char & 0x40 ? *in_char + 9 : *in_char) << 4;
		++in_char;
		*out_char |= (*in_char & 0x40 ? *in_char + 9 : *in_char) & 0xF;
		++in_char;
		++out_char;
	}
}


void COVSMFCUserDlg::OnBnClickedButtonTest()
{
	// TODO: 在此添加控件通知处理程序代码
	float k = 0.0f;
	p_serial_port->GetTemperature(k);
}
