
// SNDLL_Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSNDLL_DemoApp: 
// �йش����ʵ�֣������ SNDLL_Demo.cpp
//

class CSNDLL_DemoApp : public CWinApp
{
public:
	CSNDLL_DemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSNDLL_DemoApp theApp;