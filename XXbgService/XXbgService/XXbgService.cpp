
// XXbgService.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "XXbgService.h"
#include "MainFrm.h"
#include "MyTTrace.h"
#include "utils.h"

#include "XXbgServiceDoc.h"
#include "XXbgServiceView.h"
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.Lib")  //MiniDumpWriteDump����ʱ�õ�

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXXbgServiceApp

BEGIN_MESSAGE_MAP(CXXbgServiceApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CXXbgServiceApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CXXbgServiceApp ����

CXXbgServiceApp::CXXbgServiceApp()
{
	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("XXbgService.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_hwndDlg = NULL;
	m_pHttpThread = NULL;
	bIsHttpThreadRun = FALSE;
}

// Ψһ��һ�� CXXbgServiceApp ����

CXXbgServiceApp theApp;


// CXXbgServiceApp ��ʼ��

BOOL CXXbgServiceApp::InitInstance()
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
	GtSetTraceFilePrefix(GetFilePath()+"\\log\\Trace");
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


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("��ͨ�Ƽ�"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CXXbgServiceDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CXXbgServiceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
//	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->ShowWindow(SW_HIDE); 
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����

	// ���ÿ�������
	// SetAutoRun();

	// ɾ����ʱĿ¼�³��� M�� ���ļ��������ļ���ȡ�������������ȡ����Ĭ�ϱ���7�����־�����������죩
	//PreCleanUpFiles();

	return TRUE;
}

int CXXbgServiceApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CXXbgServiceApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CXXbgServiceApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// ���ÿ�������
BOOL CXXbgServiceApp::SetAutoRun()
{
	char subKeyDir[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	char sValue[256] = {0};

	// �������ƣ�keyֵ
	char sMyAppName[] = "XXbgService";
	if (!MyReadRegedit(subKeyDir, sMyAppName, sValue, 256))
	{
		// ��ȡע�����Ϣʧ�ܣ����ÿ�������
		char pFileName[MAX_PATH] = {0};
		// �õ����������ȫ·��
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		if (!MyWriteRegedit(subKeyDir, sMyAppName, pFileName))
		{
			GtWriteTrace(30, "[XXbgService]SetAutoRun() ���ÿ�������ʧ�ܣ�");
			::MessageBox(NULL, "���ÿ�������ʧ�ܣ�", "����", MB_OK);
		}
		else
		{
			GtWriteTrace(30, "[XXbgService]SetAutoRun() ���ÿ��������ɹ���");
			::MessageBox(NULL, "���ÿ��������ɹ���", "��ʾ", MB_OK);
		}
	}
	else
	{
		GtWriteTrace(30, "[XXbgService]SetAutoRun() �����ÿ���������Ӧ�ó���λ�ã�%s", sValue);
	}

	return TRUE;
}


// ������־����ͼ�����֤�յ��ļ��������ļ���ȡ���������ļ�
BOOL CXXbgServiceApp::PreCleanUpFiles()
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
BOOL CXXbgServiceApp::CleanUpFiles(CString sDir, int nDay)
{
	if (sDir.IsEmpty() || nDay <= 0)
	{
		return FALSE;
	}
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() ��������Ŀ¼[%s]�³���[%d]����ļ�", sDir.GetBuffer(), nDay);
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
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() ��ǰʱ��[%04d-%02d-%02d %02d:%02d:%02d]", 
		pNowTime->tm_year + 1900, pNowTime->tm_mon + 1, pNowTime->tm_mday, pNowTime->tm_hour,
		pNowTime->tm_min, pNowTime->tm_sec);
	// ����ת�������� 0ʱ0��0��
	pNowTime->tm_hour = 0;
	pNowTime->tm_min = 0;
	pNowTime->tm_sec = 0;
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() ������ʱʱ��[%04d-%02d-%02d %02d:%02d:%02d]",
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
				GtWriteTrace(30, "[XXbgService]CleanUpFiles()   ��ȡ�ļ�[%d]����ʱ��ʧ�ܣ�", sFileName.GetBuffer());
				continue;
			}
			// ����ʱ���ֵ���ж��Ƿ���Ҫɾ��(��������ȥ�ļ�����ʱ���ֵ���Ƿ񳬹�nDay��)
			struct tm tmpTime;
			struct tm *pTmpTime = &tmpTime;
			// ת��Ϊ�ճ����ڸ�ʽ
			localtime_s(pTmpTime, &stat.st_ctime);
			//GtWriteTrace(30, "[XXbgService]CleanUpFiles()   �ļ�����ʱ��[%04d-%02d-%02d %02d:%02d:%02d]", 
			//	pTmpTime->tm_year + 1900, pTmpTime->tm_mon + 1, pTmpTime->tm_mday, pTmpTime->tm_hour, 
			//	pTmpTime->tm_min, pTmpTime->tm_sec);
			lDiffTime = difftime(beginTime, stat.st_ctime);
			//GtWriteTrace(30, "[XXbgService]CleanUpFiles()   ʱ��%lf", lDiffTime);
			if (lDiffTime > lSaveDays)
			//if (lDiffTime > 120)
			{
				// �������õı���ʱ�䣬ɾ�����ļ�
				if (DeleteFile(sFilePath))
				{
					GtWriteTrace(30, "[XXbgService]CleanUpFiles()   �����ļ�[%s]�ɹ���", sFileName.GetBuffer());
				}
				else
				{
					GtWriteTrace(30, "[XXbgService]CleanUpFiles()   �����ļ�[%s]ʧ�ܣ�", sFileName.GetBuffer());
				}
			}
		}
	}
	return true;
}

// CXXbgServiceApp ��Ϣ�������

//********************************************************************************
#define SHIFTED 0x8000 
//********************************************************************************
BOOL CXXbgServiceApp::ProcessMessageFilter(int code, LPMSG lpMsg)
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
			CMainFrame * pMainFrm=(CMainFrame *)AfxGetMainWnd();
			CScreenshotDlg * pDlg = pMainFrm->screenshotDlg;
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
		NOTIFYICONDATA *pNid = &((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_nid;
		Shell_NotifyIcon(NIM_DELETE, pNid);
		flag = 1;
		// ��������/�����ύ���������������
		LPCSTR lpcsOperate = _T("open");
		LPCSTR lpcsDir = _T("H:\\�����ļ�\\gotop\\������Ŀ\\�����_20170709\\XXbgService\\Debug\\PaperlessRestart.exe");
		//WinExec(lpcsDir, SW_SHOWNORMAL);
		ShellExecute(NULL, lpcsOperate, lpcsDir, (LPCSTR)"EXCEPTION_RESTART", NULL, SW_SHOWNORMAL);
	}

	return EXCEPTION_EXECUTE_HANDLER;	//���ر��ص������Ĵ�����
}