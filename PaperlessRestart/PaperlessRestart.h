
// PaperlessRestart.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPaperlessRestartApp:
// �йش����ʵ�֣������ PaperlessRestart.cpp
//

class CPaperlessRestartApp : public CWinApp
{
public:
	CPaperlessRestartApp();

// ��д
public:
	virtual BOOL InitInstance();
	CString sParam;

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPaperlessRestartApp theApp;