
// Paperless.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Paperless.h"
#include "PaperlessDlg.h"

#include "ScreenshotDlg.h"
#include "MyTTrace.h"
#include "utils.h"
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.Lib")  //MiniDumpWriteDump����ʱ�õ�

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPaperlessApp
BEGIN_MESSAGE_MAP(CPaperlessApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPaperlessApp ����
CPaperlessApp::CPaperlessApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_pMainWnd = NULL;
	m_hwndDlg = NULL;
	m_pHttpThread = NULL;
	bIsHttpThreadRun = FALSE;
	nSystemBit = 0;
}


// Ψһ��һ�� CPaperlessApp ����
CPaperlessApp theApp;


// CPaperlessApp ��ʼ��
BOOL CPaperlessApp::InitInstance()
{
	// ����log�ļ��У���ͼ�ļ���
	// ��ȡ��ǰ�����������·��
	CString strDir = GetAppPath();
	// �жϽ�ͼ�ļ��洢Ŀ¼�Ƿ���ڣ��������򴴽�
	CString strPicDir = strDir + "\\CatchScreen";
	if (!PathIsDirectory(strPicDir))
	{
		//����Ŀ¼,���еĻ���Ӱ��
		::CreateDirectory(strPicDir, NULL);
	}
	// �ж�log�ļ��洢Ŀ¼�Ƿ���ڣ��������򴴽�
	CString strLogDir = strDir + "\\log";
	if (!PathIsDirectory(strLogDir))
	{
		//����Ŀ¼,���еĻ���Ӱ��
		::CreateDirectory(strLogDir, NULL);
	}
	// �ж����֤�������ļ��洢Ŀ¼�Ƿ���ڣ��������򴴽�
	CString strIDDir = strDir + "\\IDPicture";
	if (!PathIsDirectory(strIDDir))
	{
		//����Ŀ¼,���еĻ���Ӱ��
		::CreateDirectory(strIDDir, NULL);
	}

	// ��־�ļ���ʼ��
	GtSetTraceFilePrefix(GetFilePath()+"\\log\\Paperless");
	GtSetTraceLevel(EM_TraceDebug);

	// ��һʵ������
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, this->m_pszAppName);// ���������
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ������л������������ͷž������λ��������
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		// �����˳�
		::MessageBoxA(NULL, "�����������У��������ظ�������", "����", MB_OK);
		return FALSE;
	}

	// �����쳣�������
	SetUnhandledExceptionFilter(FreeEIM_UnhandledExceptionFilter);


	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

// 	// ��ʼ�� OLE ��
// 	if (!AfxOleInit())
// 	{
// 		AfxMessageBox(IDP_OLE_INIT_FAILED);
// 		return FALSE;
// 	}

//	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("��ͨ�Ƽ�"));

	CPaperlessDlg *dlg = new CPaperlessDlg();
	dlg->Create(IDD_PAPERLESS_DIALOG);
	m_pMainWnd = dlg;
	m_pMainWnd->UpdateWindow();

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ÿ�������
	//SetAutoRun();

	// ɾ����ʱĿ¼�³��� M�� ���ļ��������ļ���ȡ�������������ȡ����Ĭ�ϱ���7�����־�����������죩
	//PreCleanUpFiles();

	// ����IE�İ汾
	//SetIEVersion();

	// ����Ӧ�ó������Ϣ�á�
	return TRUE;
}


int CPaperlessApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	//AfxOleTerm(FALSE);
	if (m_pMainWnd)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}


	return CWinApp::ExitInstance();
}


// ���ÿ�������
BOOL CPaperlessApp::SetAutoRun()
{
	GtWriteTrace(30, "%s:%d: ��ʼ���ÿ�������......", __FUNCTION__, __LINE__);
	HKEY rootKeyDir = HKEY_LOCAL_MACHINE;
	char subKeyDir[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	char sValue[256] = {0};

	// �������ƣ�keyֵ
	char sMyAppName[] = "Paperless";
	if (!MyReadRegedit(rootKeyDir, subKeyDir, sMyAppName, sValue, 256, REG_SZ))
	{
		// ��ȡע�����Ϣʧ�ܣ����ÿ�������
		char pFileName[MAX_PATH] = {0};
		// �õ����������ȫ·��
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		if (!MyWriteRegedit(rootKeyDir, subKeyDir, sMyAppName, pFileName, REG_SZ))
		{
			GtWriteTrace(30, "%s:%d: \t���ÿ�������ʧ�ܣ�", __FUNCTION__, __LINE__);
			::MessageBox(NULL, "���ÿ�������ʧ�ܣ�", "����", MB_OK);
		}
		else
		{
			GtWriteTrace(30, "%s:%d: \t���ÿ��������ɹ���", __FUNCTION__, __LINE__);
			::MessageBox(NULL, "���ÿ��������ɹ���", "��ʾ", MB_OK);
		}
	}
	else
	{
		GtWriteTrace(30, "%s:%d: \t�����ÿ���������Ӧ�ó���λ�ã�%s", __FUNCTION__, __LINE__, sValue);
	}
	GtWriteTrace(30, "%s:%d: ���ÿ�����������\n", __FUNCTION__, __LINE__);
	return TRUE;
}


// ������־����ͼ�����֤�յ��ļ��������ļ���ȡ���������ļ�
BOOL CPaperlessApp::PreCleanUpFiles()
{
	// ��ʱ�ļ���������
	int nDay = 0;
	// ��ʱ�ַ���
	CString tmpString;
    // ��ȡ��ǰ��������Ŀ¼
	CString sPath = GetAppPath();
	// �����ļ�λ��
	CString iniDir = sPath + "\\win.ini";
	// ���������ڵ���ʱĿ¼
	CString sTmpPath;

	// �����ļ���ȡ ��ʱ�ļ� ��������
	GetPrivateProfileString("Information", "FilesSaveDay", "7", tmpString.GetBuffer(31), 31, iniDir);
	tmpString.ReleaseBuffer();
	// CStringתint
	nDay = _ttoi(tmpString);

	// �����ͼ�ļ�
	sTmpPath = sPath + "\\CatchScreen\\";
	CleanUpFiles(sTmpPath, nDay);

	// �������֤��Ƭ
	sTmpPath = sPath + "\\IDPicture\\";
	CleanUpFiles(sTmpPath, nDay);

	// ������־
	sTmpPath = sPath + "\\log\\";
	CleanUpFiles(sTmpPath, nDay);

	// ����Ŀ¼
	sTmpPath = sPath + "\\test\\";
	CleanUpFiles(sTmpPath, nDay);

	return TRUE;
}


// ����sDirĿ¼�³���nDay���ļ�
BOOL CPaperlessApp::CleanUpFiles(CString sDir, int nDay)
{
	GtWriteTrace(30, "%s:%d: ��ʼ����sDirĿ¼�¹����ļ�......", __FUNCTION__, __LINE__);
	if (sDir.IsEmpty() || nDay <= 0)
	{
		GtWriteTrace(30, "%s:%d: ��μ��ʧ�ܣ�\n.", __FUNCTION__, __LINE__);
		return FALSE;
	}
	GtWriteTrace(30, "%s:%d: \t��������Ŀ¼[%s]�³���[%d]����ļ�", __FUNCTION__, __LINE__, sDir.GetBuffer(), nDay);
	// ʱ���
	double lDiffTime = 0.00;
	// ��ʱ�ļ�����ʱ�䣬��Ϊ��λ
	double lSaveDays = nDay * 24 * 60 * 60;
	// ��ǰʱ��
	struct tm sNowTime;
	// ��ǰʱ��ָ��
	struct tm *pNowTime = &sNowTime;
	// ����Ϊ��λ�ĵ�ǰʱ��
	time_t nowTime;
	// ����Ϊ��λ�ĵ���0ʱ0��0��ʱ�䣬�������ļ�����ʱ�����Ƚϣ�ɾ������nDay���ļ�
	time_t beginTime;
	
	// ��ȡ��ǰʱ��
    time(&nowTime);
	// ת��Ϊ�ճ����ڸ�ʽ
	localtime_s(pNowTime, &nowTime);
	GtWriteTrace(30, "%s:%d: \t��ǰʱ��[%04d-%02d-%02d %02d:%02d:%02d]", __FUNCTION__, __LINE__, nDay, 
		pNowTime->tm_year + 1900, pNowTime->tm_mon + 1, pNowTime->tm_mday, pNowTime->tm_hour,
		pNowTime->tm_min, pNowTime->tm_sec);
	// ����ת�������� 0ʱ0��0��
	pNowTime->tm_hour = 0;
	pNowTime->tm_min = 0;
	pNowTime->tm_sec = 0;
	GtWriteTrace(30, "%s:%d: \t������ʱʱ��[%04d-%02d-%02d %02d:%02d:%02d]", __FUNCTION__, __LINE__, nDay, 
		pNowTime->tm_year + 1900, pNowTime->tm_mon + 1, pNowTime->tm_mday, pNowTime->tm_hour,
		pNowTime->tm_min, pNowTime->tm_sec);
	// �ճ����ڸ�ʽת��������ʱ�䣨��Ϊ��λ��
	beginTime = mktime(pNowTime);
	

	// ����Ŀ¼�µ��ļ�
	CFileFind ff;
	if (sDir.Right(1) != "\\")
	{
		sDir += "\\";
	}
	sDir += "*.*";
	BOOL ret = ff.FindFile(sDir);
	while (ret)
	{
		ret = ff.FindNextFile();
		// ����Ŀ¼������ ".", ".."
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString sFilePath = ff.GetFilePath();
			CString sFileName = ff.GetFileName();
			struct _stati64 stat;

			//GtWriteTrace(30, "[XXbgService]CleanUpFiles() ��ѯ�ļ���[%s]", sFileName.GetBuffer());
			if (FALSE == GetFileAttributes(&stat, sFilePath))
			{
				GtWriteTrace(30, "%s:%d: \t��ȡ�ļ�[%d]����ʱ��ʧ�ܣ�", __FUNCTION__, __LINE__, sFileName.GetBuffer());
				continue;
			}
			// ����ʱ���ֵ���ж��Ƿ���Ҫɾ��(��������ȥ�ļ�����ʱ���ֵ���Ƿ񳬹�nDay��)
			struct tm tmpTime;
			struct tm *pTmpTime = &tmpTime;
			// ת��Ϊ�ճ����ڸ�ʽ
			localtime_s(pTmpTime, &stat.st_ctime);
			lDiffTime = difftime(beginTime, stat.st_ctime);
			if (lDiffTime > lSaveDays)
			//if (lDiffTime > 120)
			{
				// �������õı���ʱ�䣬ɾ�����ļ�
				if (DeleteFile(sFilePath))
				{
					GtWriteTrace(30, "%s:%d: \t�����ļ�[%s]�ɹ���", __FUNCTION__, __LINE__, sFileName.GetBuffer());
				}
				else
				{
					GtWriteTrace(30, "%s:%d: \t�����ļ�[%s]ʧ�ܣ�", __FUNCTION__, __LINE__, sFileName.GetBuffer());
				}
			}
		}
	}
	GtWriteTrace(30, "%s:%d: ����sDirĿ¼�¹����ļ������˳�\n", __FUNCTION__, __LINE__);
	return true;
}


// ͨ��дע������ñ�����������IE�汾
BOOL CPaperlessApp::SetIEVersion()
{
	GtWriteTrace(30, "%s:%d: ��������IE�汾����......", __FUNCTION__, __LINE__);
	int nIEVersion = 0;
	// ע����ѯ��ǰIE��߰汾
	HKEY rootKey = HKEY_LOCAL_MACHINE;
	char subKeyIEVersion[] = "SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector";
	char keyNameIE[] = "IE";
	char keyValueIE[256] = {0};
	if (MyReadRegedit(rootKey, subKeyIEVersion, keyNameIE, keyValueIE, sizeof(keyValueIE), REG_SZ))
	{
		nIEVersion = atoi(keyValueIE) * 1000;
		GtWriteTrace(30, "%s:%d: \tע����ѯ����ǰIE�汾[%s]������[%d]\n", __FUNCTION__, __LINE__, keyValueIE, nIEVersion);
	}
	else
	{
		nIEVersion = 6 * 1000;
		GtWriteTrace(30, "%s:%d: \tע����ѯ��ǰIE�汾ʧ�ܣ�����Ĭ�ϰ汾��IE6������[%d]\n", __FUNCTION__, __LINE__, nIEVersion);
	}

	// ����IE�汾��ֵ
	// ���ȼ��ע����Ƿ��Ѿ�����ֵ
	char subKeyWrite[] = "SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION";
	// �������ƣ�key����
	char sMyAppName[] = "Paperless.exe";
	DWORD sValue = 0;

	// δ���ü�ֵ ���� �����˼�ֵ������Ϊ��ǰIE���°汾
	if ( (!MyReadRegedit(rootKey, subKeyWrite, sMyAppName, (void *)&sValue, sizeof(DWORD), REG_DWORD)) && (sValue != nIEVersion) )
	{
		// �����ڸü�ֵ�����
		if (!MyWriteRegedit(rootKey, subKeyWrite, sMyAppName, (void *)&nIEVersion, REG_DWORD))
		{
			GtWriteTrace(30, "%s:%d: \t����IE�汾[%s]ʧ�ܣ�\n", __FUNCTION__, __LINE__, "FEATURE_BROWSER_EMULATION");
		}
		else
		{
			GtWriteTrace(30, "%s:%d: \t����IE�汾[%s]�ɹ���\n", __FUNCTION__, __LINE__, "FEATURE_BROWSER_EMULATION");
		}
	}
	else
	{
		GtWriteTrace(30, "%s:%d: \t������IE�汾[%d]\n", __FUNCTION__, __LINE__, sValue);
	}

	// ����������ֵ
	if (SetIEVersionChild("FEATURE_ACTIVEX_REPURPOSEDETECTION", 1) == false || 
		SetIEVersionChild("FEATURE_BLOCK_LMZ_IMG", 1) == false || 
		SetIEVersionChild("FEATURE_BLOCK_LMZ_OBJECT", 1) == false || 
		SetIEVersionChild("FEATURE_BLOCK_LMZ_SCRIPT", 1) == false || 
		SetIEVersionChild("FEATURE_ENABLE_SCRIPT_PASTE_URLACTION_IF_PROMPT", 1) == false || 
		SetIEVersionChild("FEATURE_LOCALMACHINE_LOCKDOWN", 1) == false ||
		SetIEVersionChild("FEATURE_GPU_RENDERING", 1) == false || 
		SetIEVersionChild("FEATURE_Cross_Domain_Redirect_Mitigation", 1) == false )
	{
		GtWriteTrace(30, "%s:%d: \t�Ӽ�����ʧ�ܣ�", __FUNCTION__, __LINE__);
	}


	GtWriteTrace(30, "%s:%d: ����IE�汾���������˳�\n", __FUNCTION__, __LINE__);
	return TRUE;
}


// дע�������IE�汾
BOOL CPaperlessApp::SetIEVersionChild(const char *pKeyName, int pKeyValue)
{
	GtWriteTrace(30, "%s:%d: ��������IE�汾����key����......", __FUNCTION__, __LINE__, pKeyName);
	if (pKeyName == false)
	{
		GtWriteTrace(30, "%s:%d: ��μ��ʧ�ܣ�\n", __FUNCTION__, __LINE__, pKeyName);
		return FALSE;
	}
	// ���ȼ��ע����Ƿ��Ѿ�����ֵ
	HKEY rootKey = HKEY_LOCAL_MACHINE;
	CString subKeyDir = "SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\";
	subKeyDir += pKeyName;

	// ���ȴ�����key����Ϊ�����䲻����
	MyAddRegedit(rootKey, subKeyDir.GetBuffer());

	// �������ƣ�keyֵ
	char sMyAppName[] = "Paperless.exe";
	DWORD sValue = 0;

	// 
	if (!MyReadRegedit(rootKey, subKeyDir.GetBuffer(), sMyAppName, (void *)&sValue, sizeof(DWORD), REG_DWORD))
	{
		// �����ڸü�ֵ�����
		if (!MyWriteRegedit(rootKey, subKeyDir.GetBuffer(), sMyAppName, (void *)&pKeyValue, REG_DWORD))
		{
			GtWriteTrace(30, "%s:%d: \t����key[%s]ʧ�ܣ�\n", __FUNCTION__, __LINE__, pKeyName);
			return FALSE;
		}
		else
		{
			GtWriteTrace(30, "%s:%d: \t����key[%s]�ɹ���", __FUNCTION__, __LINE__, pKeyName);
		}
	}
	else
	{
		GtWriteTrace(30, "%s:%d: \t�Ѵ��ڴ˼�[%s], ֵ��%d", __FUNCTION__, __LINE__, pKeyName, sValue);
	}
	GtWriteTrace(30, "%s:%d: ����IE�汾����key���������˳���\n", __FUNCTION__, __LINE__, pKeyName);
	return TRUE;
}

//********************************************************************************
#define SHIFTED 0x8000 
//********************************************************************************
BOOL CPaperlessApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_hwndDlg!=NULL)
	{
		// ֻ���յ������¼�������¼� lpMsg->wParam = 0��
		//�ж���Ϣ�������Ϣ�ǴӶԻ��򷢳��Ļ������ӿؼ������ģ��ͽ��д���
		if((lpMsg->hwnd==m_hwndDlg) || ::IsChild(m_hwndDlg,lpMsg->hwnd))
		{
			//�����Ϣ��WM_KEYDOWN
			//�÷��������λ��
			CScreenshotDlg * pDlg = ((CPaperlessDlg*)theApp.m_pMainWnd)->pScreenshotDlg;
			if (pDlg != NULL)
			{
				if(lpMsg->message==WM_KEYDOWN)
				{
					CRect rect(0,0,0,0);
					rect=pDlg->m_rectTracker.m_rect;
					// �ж�
					if(pDlg->m_bFirstDraw)
					{
						//���Shift�����������������С
						BOOL isShifeDowm=FALSE;
						int nVirtKey;
						nVirtKey = GetKeyState(VK_SHIFT); 
						if (nVirtKey & SHIFTED) 
							isShifeDowm=TRUE;

						switch(lpMsg->wParam)
						{
						case VK_UP:
							//�������Shift,��ֻ����һ��
							if(!isShifeDowm)
								rect.top-=1;
							rect.bottom-=1;
							pDlg->m_rectTracker.m_rect=rect;
							pDlg->PaintWindow();
							break;
						case VK_DOWN:
							if(!isShifeDowm)
								rect.top+=1;
							rect.bottom+=1;
							pDlg->m_rectTracker.m_rect=rect;
							pDlg->PaintWindow();
							break;
						case VK_LEFT:
							if(!isShifeDowm)
								rect.left-=1;
							rect.right-=1;
							pDlg->m_rectTracker.m_rect=rect;
							pDlg->PaintWindow();
							break;
						case VK_RIGHT:
							if(!isShifeDowm)
								rect.left+=1;
							rect.right+=1;
							pDlg->m_rectTracker.m_rect=rect;
							pDlg->PaintWindow();
							break;
						}
						// ��Ե���������� ��λ��ʾ ȷ�ϡ�ȡ����ť
						pDlg->MoveShowOkCancelBtn();
					}
				}
			}
			
		}
	} 
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
//********************************************************************************


// ��������������
LONG WINAPI FreeEIM_UnhandledExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo)
{
	//AfxMessageBox("�������쳣�������˳�����");	//���ûص������ɹ�

	static int flag = 0;
	// дdump�ļ�
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString sDmpFilename;
	sDmpFilename.Format("ExceDump-%04d%02d%02d-%02d%02d%02d-%03d.dmp", 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	LPCSTR pPath = sDmpFilename.GetBuffer();
	sDmpFilename.ReleaseBuffer();
	//MultiByteToWideChar(CP_ACP, 0, path.c_str(), size, buffer, size * sizeof(wchar_t));
	//buffer[size] = 0;  //ȷ���� '\0' ��β 
	HANDLE hFile = ::CreateFile(pPath, GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{   
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = pExceptionInfo;
		//���ڴ��ջdump���ļ���
		//MiniDumpWriteDump������dbghelpͷ�ļ�
		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(),hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
	}

	if (flag == 0)
	{
		// ɾ������ͼ��
// 		NOTIFYICONDATA *pNid = &((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_nid;
// 		Shell_NotifyIcon(NIM_DELETE, pNid);
		flag = 1;
		// ��������/�����ύ���������������
		LPCSTR lpcsOperate = _T("open");
		LPCSTR lpcsDir = _T("H:\\�����ļ�\\gotop\\������Ŀ\\�����_20170709\\XXbgService\\Debug\\PaperlessRestart.exe");
		//WinExec(lpcsDir, SW_SHOWNORMAL);
		ShellExecute(NULL, lpcsOperate, lpcsDir, (LPCSTR)"EXCEPTION_RESTART", NULL, SW_SHOWNORMAL);
	}

	return EXCEPTION_EXECUTE_HANDLER;	//���ر��ص������Ĵ�����
}