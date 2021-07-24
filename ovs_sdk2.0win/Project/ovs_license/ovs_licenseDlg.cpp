
// ovs_licenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ovs_license.h"
#include "ovs_licenseDlg.h"
#include "afxdialogex.h"
#include "ovs_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// Covs_licenseDlg dialog



Covs_licenseDlg::Covs_licenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Covs_licenseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Covs_licenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_ETERNEL, m_btnEternel);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_ExpirationDate);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(Covs_licenseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GENERATE_CODE, &Covs_licenseDlg::OnBnClickedBtnGenerateCode)
END_MESSAGE_MAP()


// Covs_licenseDlg message handlers

BOOL Covs_licenseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Covs_licenseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Covs_licenseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Covs_licenseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Covs_licenseDlg::OnBnClickedBtnGenerateCode()
{
	bool bEternal = m_btnEternel.GetCheck();

	CString strExDate, strExDate2, strCode, strIniString;
	if (bEternal) {
		strExDate = _T("Eternal");
		strExDate2 = _T("SDKExpireDate=\"Eternal\"\r\n");
	}
	else {
		CTime date;
		m_ExpirationDate.GetCurSel(date);
		strExDate = ovs_util::getDateString(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetDayOfWeek());
		strExDate2.Format("SDKExpireDate=\"%s\"\r\n", strExDate);
	}
	char szCode[1024];
	sprintf_s(szCode, "Purpose=%s;ExpireDate=%s", _T("ONLY_USE_MODULE"), strExDate);
	strCode.Format("SDKLicence=\"%s\"\r\n", ovs_util::generateAuthenticationCode(szCode, "Q!ngDa0 N@ve1&e*m T2chN01O9y c0. !tD. (2018)"));

	strIniString = strExDate2 + strCode;

	m_editLog.SetWindowText(strIniString);
}
