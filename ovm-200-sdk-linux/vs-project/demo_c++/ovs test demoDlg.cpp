
// ovs test demoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ovs test demo.h"
#include "ovs test demoDlg.h"
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


// CovstestdemoDlg 对话框



CovstestdemoDlg::CovstestdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OVSTESTDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CovstestdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CovstestdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CovstestdemoDlg::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_START_ENROLL, &CovstestdemoDlg::OnBnClickedButtonStartEnroll)
END_MESSAGE_MAP()


// CovstestdemoDlg 消息处理程序

BOOL CovstestdemoDlg::OnInitDialog()
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

void CovstestdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CovstestdemoDlg::OnPaint()
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
HCURSOR CovstestdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CovstestdemoDlg::OnBnClickedButtonInit()
{
	// TODO: 在此添加控件通知处理程序代码
	OVS::InitFeatureSDK();
	
}

void CovstestdemoDlg::PreviewImage(int hWnd, unsigned char* imageData, int imageWidth, int imageHeight, int imageChannel)
{
	CDC memDC;
	CBitmap newBitmap;
	CBitmap* pOldBitmap;
	CRect wndRect;

	CDC* pDC = GetDlgItem(hWnd)->GetDC();
	GetDlgItem(hWnd)->GetClientRect(&wndRect);

	unsigned char* pBmpBits = (unsigned char*)calloc(sizeof(unsigned char), imageWidth * imageHeight * 4);
	for (int i = 0; i < imageHeight; i++)
	{
		unsigned char* pSrc = imageData + i * imageWidth * imageChannel;
		unsigned char* pDst = pBmpBits + i * imageWidth * 4;

		for (int j = 0; j < imageWidth; j++)
		{
			if (imageChannel == 1) {
				unsigned char p = *(pSrc++);
				*(pDst++) = p;	//B Channel
				*(pDst++) = p;	//G Channel
				*(pDst++) = p;	//R Channel
				*(pDst++) = 0;	//Alpha Channel (fixed to 0)
			}
			else if (imageChannel == 3) {
				*(pDst++) = *(pSrc++);	//B Channel
				*(pDst++) = *(pSrc++);	//G Channel
				*(pDst++) = *(pSrc++);	//R Channel
				*(pDst++) = 0;			//Alpha Channel (fixed to 0)
			}
			else if (imageChannel == 4) {
				*(pDst++) = *(pSrc++);	//B Channel
				*(pDst++) = *(pSrc++);	//G Channel
				*(pDst++) = *(pSrc++);	//R Channel
				*(pDst++) = *(pSrc++);	//Alpha Channel (fixed to 0)
			}
		}
	}

	memDC.CreateCompatibleDC(pDC);
	newBitmap.CreateCompatibleBitmap(pDC, imageWidth, imageHeight);
	newBitmap.SetBitmapBits(imageWidth * imageHeight * 4, pBmpBits);

	pOldBitmap = (CBitmap*)memDC.SelectObject(&newBitmap);

	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(wndRect.left, wndRect.top, wndRect.Width(), wndRect.Height(), &memDC, 0, 0, imageWidth, imageHeight, SRCCOPY);

	free(pBmpBits);
	memDC.DeleteDC();
	newBitmap.DeleteObject();
	ReleaseDC(pDC);
}

bool CovstestdemoDlg::ExtractImageFromDevice()
{

	while (true)
	{
		Sleep(1);
		OVS::Image temp_image_full;
		string fea;
		OVS::Information info;
		GetPreviewImage(temp_image_full, fea, info);
		if (temp_image_full.dataVtr.data() == nullptr)
		{
			continue;
		}
		PreviewImage(IDC_IMAGE_PREVIEW, temp_image_full.dataVtr.data(), temp_image_full.width, temp_image_full.height, 3);
		if (info.is_feature_ok)
		{
			int i = 1;
		}
	}

	return true;
}




void CovstestdemoDlg::OnBnClickedButtonStartEnroll()
{
	// TODO: 在此添加控件通知处理程序代码
	StartEnrollMode(nullptr, nullptr);
	thread_display_imag = thread(&CovstestdemoDlg::ExtractImageFromDevice, this);
	thread_display_imag.detach();
}
