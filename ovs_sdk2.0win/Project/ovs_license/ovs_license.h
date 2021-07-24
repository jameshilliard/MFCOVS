
// ovs_license.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Covs_licenseApp:
// See ovs_license.cpp for the implementation of this class
//

class Covs_licenseApp : public CWinApp
{
public:
	Covs_licenseApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Covs_licenseApp theApp;