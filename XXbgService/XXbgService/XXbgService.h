
// XXbgService.h : XXbgService Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������



// CXXbgServiceApp:
// �йش����ʵ�֣������ XXbgService.cpp
//

class CXXbgServiceApp : public CWinApp
{
public:
	CXXbgServiceApp();
	
private:
	// ���ÿ�������
	BOOL SetAutoRun();
	// ������־����ͼ�����֤�յ��ļ��������ļ���ȡ���������ļ�
	BOOL PreCleanUpFiles();
	// ����sDirĿ¼�³���nDay���ļ�
	BOOL CleanUpFiles(CString sDir, int nDay);

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
//*******************************
	HWND m_hwndDlg;
	// ����http�����߳�
	CWinThread *m_pHttpThread;
//*******************************
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
};

// ��������������
LONG WINAPI FreeEIM_UnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo);
extern CXXbgServiceApp theApp;
