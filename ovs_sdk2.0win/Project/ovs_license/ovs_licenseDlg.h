
// ovs_licenseDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// Covs_licenseDlg dialog
class Covs_licenseDlg : public CDialogEx
{
// Construction
public:
	Covs_licenseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OVS_LICENSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnGenerateCode();
	DECLARE_MESSAGE_MAP()

public:
	CButton m_btnEternel;
	CMonthCalCtrl m_ExpirationDate;
	CEdit m_editLog;
};
