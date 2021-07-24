
// OVSSDKDemoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "OVSSDKDemo.h"
#include "OVSSDKDemoDlg.h"
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


// COVSSDKDemoDlg 对话框



COVSSDKDemoDlg::COVSSDKDemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OVSSDKDEMO_DIALOG, pParent)
	, edit_user_id_str(_T(""))
	, b_sdk_status(false)
	, b_user_register(false)
	, b_user_idenfiy(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COVSSDKDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPENDEVICE, button_open_device);
	DDX_Control(pDX, IDC_BUTTON_CLOSEDEVICE, button_close_device);
	DDX_Control(pDX, IDC_BUTTON_USER_REGISTER, button_user_register);
	DDX_Control(pDX, IDC_BUTTON_USER_IDENFIY, button_user_idenfiy);
	DDX_Control(pDX, IDC_BUTTON_STOP, button_stop);
	DDX_Control(pDX, IDC_EDIT_USER_ID, edit_user_id);
	DDX_Text(pDX, IDC_EDIT_USER_ID, edit_user_id_str);
	DDX_Control(pDX, IDC_PROGRESS1, progress_ctrl_register);
}

BEGIN_MESSAGE_MAP(COVSSDKDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPENDEVICE, &COVSSDKDemoDlg::OnBnClickedButtonOpendevice)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEDEVICE, &COVSSDKDemoDlg::OnBnClickedButtonClosedevice)
	ON_BN_CLICKED(IDC_BUTTON_USER_REGISTER, &COVSSDKDemoDlg::OnBnClickedButtonUserRegister)
	ON_BN_CLICKED(IDC_BUTTON_USER_IDENFIY, &COVSSDKDemoDlg::OnBnClickedButtonUserIdenfiy)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &COVSSDKDemoDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// COVSSDKDemoDlg 消息处理程序

BOOL COVSSDKDemoDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COVSSDKDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COVSSDKDemoDlg::OnPaint()
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
HCURSOR COVSSDKDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool COVSSDKDemoDlg::RunSDK()
{
	OVS_InitSDK();
	char* left_feature = "";
	char* right_feature = "";
	char* user_id = "";
	if (OVS_AddUser(user_id, left_feature, right_feature) == SUCCESS)
	{
		int a = 0;
	}
	b_sdk_status = true;
	return true;
}

bool COVSSDKDemoDlg::Entrance()
{
	// 获得pictrue控件窗口的句柄 
	p_wnd = shared_ptr<CWnd>((GetDlgItem(IDC_STATIC_PICTURE)));
	// 获得pictrue控件的D
	p_dc = shared_ptr<CDC>(p_wnd->GetDC());
	GetClientRect(&display_rect);
	GetDlgItem(IDC_STATIC_PICTURE)->GetClientRect(&display_rect);

	namedWindow("ImageShow");
	HWND hWnd = (HWND)cvGetWindowHandle("ImageShow");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_PICTURE)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);

	while (b_sdk_status)
	{
		//lock_guard<mutex> lk(ovs_mutex);

		// 用户注册
		if (b_user_register)
		{
			UserRegister();
		}
		else if (b_user_idenfiy)
		{
			UserIdenfiy();
		}

	}
	p_wnd->ReleaseDC(p_dc.get());
	return true;
}

bool COVSSDKDemoDlg::UserRegister()
{
	Image temp_image;
	Information temp_information;
	OVS_GetPreviewImage(temp_image, temp_information);

	if (temp_image.data == nullptr)
	{
		return false;
	}

	Mat temp_mat(temp_image.width, temp_image.height, CV_8UC3, temp_image.data);

	lock_guard<mutex> lk(ovs_mutex);
	resize(temp_mat, display_dst, Size(display_rect.Width(), display_rect.Height()));

	imshow("ImageShow", display_dst);
	waitKey(1);
	if (temp_information.is_feature_ok)
	{
		b_user_register = false;
	}
	return true;
}

bool COVSSDKDemoDlg::UserIdenfiy()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	Image temp_image;
	Information temp_information;
	OVS_GetPreviewImage(temp_image, temp_information);
	if (temp_image.data == nullptr)
	{
		return false;
	}
	Mat temp_mat(temp_image.height, temp_image.width, CV_8UC3, temp_image.data);

	lock_guard<mutex> lk(ovs_mutex);
	cv::resize(temp_mat, display_dst, Size(display_rect.Width(), display_rect.Height()));

	imshow("ImageShow", display_dst);
	waitKey(1);
	if (temp_information.is_feature_ok)
	{
		b_user_idenfiy = false;
	}
	return true;
}
bool COVSSDKDemoDlg::Entrance1()
{
	while (true)
	{
		//lock_guard<mutex> lk(ovs_mutex);
		int a = 0;


	}
	return true;
}
void COVSSDKDemoDlg::OnBnClickedButtonOpendevice()
{
	// TODO: 在此添加控件通知处理程序代码
	//RunSDK();
	//thread thread_entrance(&COVSSDKDemoDlg::Entrance, this);
	//thread_entrance.detach();
	RunSDK();
	/*if (RunSDK())
	{

		thread thread_entrance(&COVSSDKDemoDlg::Entrance1, this);
		thread_entrance.detach();
	}*/
	//if (RunSDK())
	//{
	//	thread thread_entrance(Entrance1);
	//	thread_entrance.detach();
	//}
}


void COVSSDKDemoDlg::OnBnClickedButtonClosedevice()
{
	// TODO: 在此添加控件通知处理程序代码

}


void COVSSDKDemoDlg::OnBnClickedButtonUserRegister()
{
	// TODO: 在此添加控件通知处理程序代码

	lock_guard<mutex> lk(ovs_mutex);
	b_user_register = true;
	OVS_StartFeatureForEnroll();
}


void COVSSDKDemoDlg::OnBnClickedButtonUserIdenfiy()
{
	// TODO: 在此添加控件通知处理程序代码
	lock_guard<mutex> lk(ovs_mutex);
	b_user_idenfiy = true;
	OVS_StartFeatureForMatch();
}


void COVSSDKDemoDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	OVS_Set_Exposure(28);
	OVS_Set_LEDCurrent(5);
}
