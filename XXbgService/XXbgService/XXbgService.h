
// XXbgService.h : XXbgService 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号



// CXXbgServiceApp:
// 有关此类的实现，请参阅 XXbgService.cpp
//

class CXXbgServiceApp : public CWinApp
{
public:
	CXXbgServiceApp();
	
private:
	// 设置开机启动
	BOOL SetAutoRun();
	// 清理日志、截图、身份证照等文件，配置文件获取保留几天文件
	BOOL PreCleanUpFiles();
	// 清理sDir目录下超过nDay的文件
	BOOL CleanUpFiles(CString sDir, int nDay);

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
//*******************************
	HWND m_hwndDlg;
	// 发送http报文线程
	CWinThread *m_pHttpThread;
//*******************************
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
};

// 程序崩溃处理程序
LONG WINAPI FreeEIM_UnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo);
extern CXXbgServiceApp theApp;
