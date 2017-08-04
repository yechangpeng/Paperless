
// Paperless.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPaperlessApp:
// �йش����ʵ�֣������ Paperless.cpp
//

class CPaperlessApp : public CWinApp
{
public:
	CPaperlessApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);


public:
	HWND m_hwndDlg;
	// ����http�����߳�
	CWinThread *m_pHttpThread;
	volatile BOOL bIsHttpThreadRun;
private:
	// ���ÿ�������
	BOOL SetAutoRun();
	// ������־����ͼ������֤�յ��ļ��������ļ���ȡ���������ļ�
	BOOL PreCleanUpFiles();
	// ����sDirĿ¼�³���nDay���ļ�
	BOOL CleanUpFiles(CString sDir, int nDay);

	DECLARE_MESSAGE_MAP()
};

// ���������������
LONG WINAPI FreeEIM_UnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo);
extern CPaperlessApp theApp;