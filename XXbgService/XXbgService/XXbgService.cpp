
// XXbgService.cpp : 定义应用程序的类行为。
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
#pragma comment(lib,"DbgHelp.Lib")  //MiniDumpWriteDump链接时用到

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXXbgServiceApp

BEGIN_MESSAGE_MAP(CXXbgServiceApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CXXbgServiceApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CXXbgServiceApp 构造

CXXbgServiceApp::CXXbgServiceApp()
{
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("XXbgService.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_hwndDlg = NULL;
	m_pHttpThread = NULL;
	bIsHttpThreadRun = FALSE;
}

// 唯一的一个 CXXbgServiceApp 对象

CXXbgServiceApp theApp;


// CXXbgServiceApp 初始化

BOOL CXXbgServiceApp::InitInstance()
{
	// 创建log文件夹，截图文件夹
	// 获取当前程序运行相对路径
	CString strDir = GetAppPath();
	// 判断截图文件存储目录是否存在，不存在则创建
	CString strPicDir = strDir + "\\CatchScreen";
	if (!PathIsDirectory(strPicDir))
	{
		//创建目录,已有的话不影响
		::CreateDirectory(strPicDir, NULL);
	}
	// 判断log文件存储目录是否存在，不存在则创建
	CString strLogDir = strDir + "\\log";
	if (!PathIsDirectory(strLogDir))
	{
		//创建目录,已有的话不影响
		::CreateDirectory(strLogDir, NULL);
	}
	// 判断身份证正反面文件存储目录是否存在，不存在则创建
	CString strIDDir = strDir + "\\IDPicture";
	if (!PathIsDirectory(strIDDir))
	{
		//创建目录,已有的话不影响
		::CreateDirectory(strIDDir, NULL);
	}

	// 日志文件初始化
	GtSetTraceFilePrefix(GetFilePath()+"\\log\\Trace");
	GtSetTraceLevel(EM_TraceDebug);

	// 单一实例处理
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, this->m_pszAppName);// 检查错误代码
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果已有互斥量存在则释放句柄并复位互斥量　
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		// 程序退出
		::MessageBoxA(NULL, "程序正在运行！不允许重复启动！", "警告", MB_OK);
		return FALSE;
	}

	// 关联异常处理程序
	SetUnhandledExceptionFilter(FreeEIM_UnhandledExceptionFilter);

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("国通科技"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CXXbgServiceDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CXXbgServiceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
//	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->ShowWindow(SW_HIDE); 
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生

	// 设置开机启动
	// SetAutoRun();

	// 删除临时目录下超过 M天 的文件，配置文件获取清理的天数，获取不到默认保留7天的日志（不包括当天）
	//PreCleanUpFiles();

	return TRUE;
}

int CXXbgServiceApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CXXbgServiceApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CXXbgServiceApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// 设置开机启动
BOOL CXXbgServiceApp::SetAutoRun()
{
	char subKeyDir[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	char sValue[256] = {0};

	// 程序名称，key值
	char sMyAppName[] = "XXbgService";
	if (!MyReadRegedit(subKeyDir, sMyAppName, sValue, 256))
	{
		// 读取注册表信息失败，设置开机启动
		char pFileName[MAX_PATH] = {0};
		// 得到程序自身的全路径
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		if (!MyWriteRegedit(subKeyDir, sMyAppName, pFileName))
		{
			GtWriteTrace(30, "[XXbgService]SetAutoRun() 设置开机启动失败！");
			::MessageBox(NULL, "设置开机自启失败！", "警告", MB_OK);
		}
		else
		{
			GtWriteTrace(30, "[XXbgService]SetAutoRun() 设置开机启动成功！");
			::MessageBox(NULL, "设置开机自启成功！", "提示", MB_OK);
		}
	}
	else
	{
		GtWriteTrace(30, "[XXbgService]SetAutoRun() 已设置开机启动，应用程序位置：%s", sValue);
	}

	return TRUE;
}


// 清理日志、截图、身份证照等文件，配置文件获取保留几天文件
BOOL CXXbgServiceApp::PreCleanUpFiles()
{
	// 临时文件保留天数
	int nDay = 0;
	// 临时字符串
	CString tmpString;
    // 获取当前程序运行目录
	CString sPath = GetAppPath();
	// 配置文件位置
	CString iniDir = sPath + "\\win.ini";
	// 待清理日期的临时目录
	CString sTmpPath;

	// 配置文件获取 临时文件 保留天数
	GetPrivateProfileString("Information", "FilesSaveDay", "7", tmpString.GetBuffer(31), 31, iniDir);
	tmpString.ReleaseBuffer();
	// CString转int
	nDay = _ttoi(tmpString);

	// 清理截图文件
	sTmpPath = sPath + "\\CatchScreen\\";
	CleanUpFiles(sTmpPath, nDay);

	// 清理身份证照片
	sTmpPath = sPath + "\\IDPicture\\";
	CleanUpFiles(sTmpPath, nDay);

	// 清理日志
	sTmpPath = sPath + "\\log\\";
	CleanUpFiles(sTmpPath, nDay);

	// 测试目录
	sTmpPath = sPath + "\\test\\";
	CleanUpFiles(sTmpPath, nDay);

	return TRUE;
}


// 清理sDir目录下超过nDay的文件
BOOL CXXbgServiceApp::CleanUpFiles(CString sDir, int nDay)
{
	if (sDir.IsEmpty() || nDay <= 0)
	{
		return FALSE;
	}
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() 即将清理目录[%s]下超过[%d]天的文件", sDir.GetBuffer(), nDay);
	// 时间差
	double lDiffTime = 0.00;
	// 临时文件保存时间，秒为单位
	double lSaveDays = nDay * 24 * 60 * 60;
	// 当前时间
	struct tm sNowTime;
	// 当前时间指针
	struct tm *pNowTime = &sNowTime;
	// 以秒为单位的当前时间
	time_t nowTime;
	// 以秒为单位的当天0时0分0秒时间，用于与文件创建时间做比较，删除大于nDay的文件
	time_t beginTime;
	
	// 获取当前时间
    time(&nowTime);
	// 转化为日常日期格式
	localtime_s(pNowTime, &nowTime);
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() 当前时间[%04d-%02d-%02d %02d:%02d:%02d]", 
		pNowTime->tm_year + 1900, pNowTime->tm_mon + 1, pNowTime->tm_mday, pNowTime->tm_hour,
		pNowTime->tm_min, pNowTime->tm_sec);
	// 日期转换到当天 0时0分0秒
	pNowTime->tm_hour = 0;
	pNowTime->tm_min = 0;
	pNowTime->tm_sec = 0;
	GtWriteTrace(30, "[XXbgService]CleanUpFiles() 当天零时时间[%04d-%02d-%02d %02d:%02d:%02d]",
		pNowTime->tm_year + 1900, pNowTime->tm_mon + 1, pNowTime->tm_mday, pNowTime->tm_hour,
		pNowTime->tm_min, pNowTime->tm_sec);
	// 日常日期格式转换成日历时间（秒为单位）
	beginTime = mktime(pNowTime);
	

	// 遍历目录下的文件
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
		// 不是目录，包括 ".", ".."
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString sFilePath = ff.GetFilePath();
			CString sFileName = ff.GetFileName();
			struct _stati64 stat;

			//GtWriteTrace(30, "[XXbgService]CleanUpFiles() 查询文件：[%s]", sFileName.GetBuffer());
			if (FALSE == GetFileAttributes(&stat, sFilePath))
			{
				GtWriteTrace(30, "[XXbgService]CleanUpFiles()   获取文件[%d]创建时间失败！", sFileName.GetBuffer());
				continue;
			}
			// 计算时间差值，判断是否需要删除(当天零点减去文件创建时间的值，是否超过nDay天)
			struct tm tmpTime;
			struct tm *pTmpTime = &tmpTime;
			// 转化为日常日期格式
			localtime_s(pTmpTime, &stat.st_ctime);
			//GtWriteTrace(30, "[XXbgService]CleanUpFiles()   文件创建时间[%04d-%02d-%02d %02d:%02d:%02d]", 
			//	pTmpTime->tm_year + 1900, pTmpTime->tm_mon + 1, pTmpTime->tm_mday, pTmpTime->tm_hour, 
			//	pTmpTime->tm_min, pTmpTime->tm_sec);
			lDiffTime = difftime(beginTime, stat.st_ctime);
			//GtWriteTrace(30, "[XXbgService]CleanUpFiles()   时间差：%lf", lDiffTime);
			if (lDiffTime > lSaveDays)
			//if (lDiffTime > 120)
			{
				// 超过设置的保存时间，删除此文件
				if (DeleteFile(sFilePath))
				{
					GtWriteTrace(30, "[XXbgService]CleanUpFiles()   清理文件[%s]成功！", sFileName.GetBuffer());
				}
				else
				{
					GtWriteTrace(30, "[XXbgService]CleanUpFiles()   清理文件[%s]失败！", sFileName.GetBuffer());
				}
			}
		}
	}
	return true;
}

// CXXbgServiceApp 消息处理程序

//********************************************************************************
#define SHIFTED 0x8000 
//********************************************************************************
BOOL CXXbgServiceApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_hwndDlg!=NULL)
	{
		// 只接收到键盘事件？鼠标事件 lpMsg->wParam = 0；
		//判断消息，如果消息是从对话框发出的或者其子控件发出的，就进行处理
		if((lpMsg->hwnd==m_hwndDlg) || ::IsChild(m_hwndDlg,lpMsg->hwnd))
		{
			//如果消息是WM_KEYDOWN
			//用方向键调整位置
			CMainFrame * pMainFrm=(CMainFrame *)AfxGetMainWnd();
			CScreenshotDlg * pDlg = pMainFrm->screenshotDlg;
			if (pDlg != NULL)
			{
				if(lpMsg->message==WM_KEYDOWN)
				{
					CRect rect(0,0,0,0);
					rect=pDlg->m_rectTracker.m_rect;
					// 判断
					if(pDlg->m_bFirstDraw)
					{
						//如果Shift键按下则方向键调整大小
						BOOL isShifeDowm=FALSE;
						int nVirtKey;
						nVirtKey = GetKeyState(VK_SHIFT); 
						if (nVirtKey & SHIFTED) 
							isShifeDowm=TRUE;

						switch(lpMsg->wParam)
						{
						case VK_UP:
							//如果按下Shift,则只调整一边
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
						// 边缘调整，重新 定位显示 确认、取消按钮
						pDlg->MoveShowOkCancelBtn();
					}
				}
			}
			
		}
	} 
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
//********************************************************************************


// 程序崩溃处理程序
LONG WINAPI FreeEIM_UnhandledExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo)
{
	//AfxMessageBox("程序发生异常，即将退出程序！");	//调用回调函数成功

	static int flag = 0;
	// 写dump文件
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString sDmpFilename;
	sDmpFilename.Format("ExceDump-%04d%02d%02d-%02d%02d%02d-%03d.dmp", 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	LPCSTR pPath = sDmpFilename.GetBuffer();
	sDmpFilename.ReleaseBuffer();
	//MultiByteToWideChar(CP_ACP, 0, path.c_str(), size, buffer, size * sizeof(wchar_t));
	//buffer[size] = 0;  //确保以 '\0' 结尾 
	HANDLE hFile = ::CreateFile(pPath, GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{   
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = pExceptionInfo;
		//将内存堆栈dump到文件中
		//MiniDumpWriteDump需引入dbghelp头文件
		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(),hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
	}

	if (flag == 0)
	{
		// 删除托盘图标
		NOTIFYICONDATA *pNid = &((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_nid;
		Shell_NotifyIcon(NIM_DELETE, pNid);
		flag = 1;
		// 调用重启/错误提交程序（视情况而定）
		LPCSTR lpcsOperate = _T("open");
		LPCSTR lpcsDir = _T("H:\\个人文件\\gotop\\其他项目\\程序包_20170709\\XXbgService\\Debug\\PaperlessRestart.exe");
		//WinExec(lpcsDir, SW_SHOWNORMAL);
		ShellExecute(NULL, lpcsOperate, lpcsDir, (LPCSTR)"EXCEPTION_RESTART", NULL, SW_SHOWNORMAL);
	}

	return EXCEPTION_EXECUTE_HANDLER;	//返回本回调函数的处理结果
}