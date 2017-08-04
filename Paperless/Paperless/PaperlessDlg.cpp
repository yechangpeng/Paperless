
// PaperlessDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Paperless.h"
#include "PaperlessDlg.h"
#include "afxdialogex.h"
#include "utils.h"
#include "MyTTrace.h"
#include "GHook/GHook.h"
#include "Centerm/CENT_IDCard.h"
#include "CBaseReadIDCard.h"
#include "CCentOneCamera.h"
#include "CGeitCamera.h"
#include "CCentOneReader.h"
#include "CCentReader.h"
#include "ScreenshotDlg.h"
#include "ManualInputDlg.h"
#include "SettingDlg.h"
#include "CBaseReadIDCard.h"
#include "Network/SocketComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// ��Ϣӳ�亯��
BEGIN_MESSAGE_MAP(CPaperlessDlg, CDialogEx)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAYNOTIFY, OnTrayNotify)
	ON_MESSAGE(WM_SCREENSHOT, OnScreenshot)
	ON_MESSAGE(WM_SCREENDLG_MSG, OnScreenDlgMessage)
	ON_MESSAGE(WM_CONTINUE_INPUT, OnContinueInput)
	ON_COMMAND(ID_QUIT_MENU, OnQuit)
	ON_COMMAND(ID_INPUT_SQ_MENU, OnManualInput)
	ON_COMMAND(ID_SETTING_MENU, OnSettingWin)
	// ϵͳ�����
	ON_WM_SYSCOMMAND()
	// �ر��������¼�
	ON_MESSAGE_VOID(WM_CLOSE, OnMyClose)
END_MESSAGE_MAP()


CPaperlessDlg::CPaperlessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPaperlessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// TODO: �ڴ���ӳ�Ա��ʼ������
	pScreenshotDlg = NULL;
	pInputDlg = NULL;
	pSettingDlg = NULL;
	pBaseReadIDCardInfo = NULL;
	pBaseSaveCameraPic = NULL;
	pBaseSaveCameraPic = NULL;
	nReadIDDevice = -1;
	nDeskCameraDevice = -1;
	isLastTimeExitZoomed = false;
	isFirstDbClickMenu = true;
}


CPaperlessDlg::~CPaperlessDlg()
{
	GtWriteTrace(30, "%s:%d: ~CPaperlessDlg!", __FUNCTION__, __LINE__);
	// �������ͼ��
	if(m_nid.hIcon)
		::DestroyIcon(m_nid.hIcon);
	// ��ղ˵�
	if(m_menu)
		::DestroyMenu(m_menu);
	// ɾ��������ؽṹ��
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	// ����Լ�������
	if (pScreenshotDlg != NULL)
	{
		delete pScreenshotDlg;
		pScreenshotDlg = NULL;
	}
	if (pInputDlg != NULL)
	{
		delete pInputDlg;
		pInputDlg = NULL;
	}
	if (pSettingDlg != NULL)
	{
		delete pSettingDlg;
		pSettingDlg = NULL;
	}
	if (pBaseReadIDCardInfo != NULL)
	{
		delete pBaseReadIDCardInfo;
		pBaseReadIDCardInfo = NULL;
	}
	if (pBaseSaveCameraPic != NULL)
	{
		delete pBaseSaveCameraPic;
		pBaseSaveCameraPic = NULL;
	}
}


// ������̲˵����˳�����
void CPaperlessDlg::OnQuit()
{
	GtWriteTrace(30, "%s:%d: OnQuit!", __FUNCTION__, __LINE__);
	// �˳����򣬱��洰��λ�õ������ļ���
	SaveFrmPosToFile();
	// �������ͼ��
	if(m_nid.hIcon)
		::DestroyIcon(m_nid.hIcon);
	// ��ղ˵�
	if(m_menu)
		::DestroyMenu(m_menu);
	// ɾ��������ؽṹ��
	Shell_NotifyIcon(NIM_DELETE, &m_nid);

	// ����Լ�������
	if (pScreenshotDlg != NULL)
	{
		delete pScreenshotDlg;
		pScreenshotDlg = NULL;
	}
	if (pInputDlg != NULL)
	{
		delete pInputDlg;
		pInputDlg = NULL;
	}
	if (pSettingDlg != NULL)
	{
		delete pSettingDlg;
		pSettingDlg = NULL;
	}
	if (pBaseReadIDCardInfo != NULL)
	{
		delete pBaseReadIDCardInfo;
		pBaseReadIDCardInfo = NULL;
	}
	if (pBaseSaveCameraPic != NULL)
	{
		delete pBaseSaveCameraPic;
		pBaseSaveCameraPic = NULL;
	}
	DestroyWindow();
}


void CPaperlessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// �����Ի���
int CPaperlessDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	GtWriteTrace(30, "%s:%d: OnCreate!", __FUNCTION__, __LINE__);
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	/************* ����״̬�� ***************/ 
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	/************* ��������ͼ�� ***************/ 
	//���������ص����½�
	memset(&m_nid,0,sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	m_nid.uCallbackMessage = WM_TRAYNOTIFY;
	CString strToolTip = "���ϵͳ��̨����";
	strcpy(m_nid.szTip,strToolTip.GetBuffer());
	m_nid.uID = IDR_MAINFRAME;
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hIcon = hIcon;
	Shell_NotifyIcon(NIM_ADD, &m_nid);
	if(hIcon)
		::DestroyIcon(hIcon);
	BOOL flag = m_menu.LoadMenu(IDR_POPMENU); 

	/************* �������� ***************/ 
	// ����������־����ʼΪFALSEδ����״̬
	m_hookFlag = FALSE;
	// ��������
	StartKeyBoardHook();

	/************* ��ʼ��iocp������ ***************/ 
	// ��ʼ��iocp������
	InitIocpService(this);
	return 0;
}


// 
BOOL CPaperlessDlg::OnInitDialog()
{
	GtWriteTrace(30, "%s:%d: OnInitDialog!", __FUNCTION__, __LINE__);
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	/************* ��ʼ�����֤ʶ���ǣ���������������ͷ�������ǻ�������ͷ ***************/ 
	InitDevice();

	/************* �������ļ��лָ��ϴ��˳�����λ�� ***************/ 
	InitFrmPosFromFile();

	// ���ǽ��������õ��ؼ������򷵻� TRUE
	return TRUE;
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CPaperlessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPaperlessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPaperlessDlg::InitDevice()
{
	// �����ļ���ȡ�豸������Ϣ
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ���֤ʶ�����豸
	GetPrivateProfileString("Device", "ReadIDDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	nReadIDDevice = atoi(sTmpString);
	// ��������ͷ�豸
	GetPrivateProfileString("Device", "DesktopCameraDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	nDeskCameraDevice = atoi(sTmpString);

	GtWriteTrace(30, "%s:%d: �豸���ò�����ReadIDDevice=[%d], sDeskCameraDevice=[%d]!", __FUNCTION__
		, __LINE__, nReadIDDevice, nDeskCameraDevice);
	// �ж� ���֤ʶ�����豸���ͣ����ض�Ӧ��

	switch (nReadIDDevice)
	{
	case -1:
		::MessageBox(NULL, "���֤ʶ�����豸δ���ã����飡", "����", MB_OK);
		break;
	case 1:
		pBaseReadIDCardInfo = new CCentOneReader();
		break;
	case 2:
		pBaseReadIDCardInfo = new CCentReader();
		break;
	default:
		::MessageBox(NULL, "���õ����֤ʶ���ǲ����ڣ����飡", "����", MB_OK);
		break;
	}
	if (nDeskCameraDevice == -1)
	{
		::MessageBox(NULL, "����������ͷ�豸δ���ã����飡", "����", MB_OK);
	}
	else if (nDeskCameraDevice == 1)
	{
		pBaseSaveCameraPic = new CCentOneCamera(this);
	}
	else if (nDeskCameraDevice == 2)
	{
		pBaseSaveCameraPic = new CGeitCamera();
	}
	else
	{
		::MessageBox(NULL, "���õĸ���������ͷ�豸�����ڣ����飡", "����", MB_OK);
	}
}


// CMainFrame ���

#ifdef _DEBUG
void CPaperlessDlg::AssertValid() const
{
	CDialogEx::AssertValid();
}

void CPaperlessDlg::Dump(CDumpContext& dc) const
{
	CDialogEx::Dump(dc);
}
#endif //_DEBUG


// ����������λ�õ������ļ���
BOOL CPaperlessDlg::SaveFrmPosToFile()
{
	// �����ļ�·��
	CString iniDir = GetAppPath()+"\\win.ini";
	// �жϵ�ǰ����״̬����󻯡���С����������
	// ���ڴ˴�������󻯡���С��λ������Ϊ�����С��״̬ȡ������֮ǰ����Ϣ
	if (this->IsZoomed())
	{
		// ��󻯣�����״̬�������ļ���
		// ���״̬��1-��
		// ���豣�����ǰ��λ�ã���Ϊ������ʱ�Ѿ������
		WritePrivateProfileString("Information", "IsZoomed", "1", iniDir);
	}
	else if (this->IsIconic())
	{
		// ��С�������豣����С��ǰ��λ�ã�����Ϊ�ڵ����С��֮ǰ�Ѿ������
	}
	else
	{
		// ����󻯣����洰��λ�õ������ļ���
		RECT rect = {0};
		CString tmp;
		// ��ȡ��ǰ����λ��
		::GetWindowRect(this->m_hWnd, &rect);
		tmp.Format("%d", rect.left);
		WritePrivateProfileString("Information", "WinXPos", tmp, iniDir);
		tmp.Format("%d", rect.top);
		WritePrivateProfileString("Information", "WinYPos", tmp, iniDir);
		tmp.Format("%d", rect.right - rect.left);
		WritePrivateProfileString("Information", "WinWidth", tmp, iniDir);
		tmp.Format("%d", rect.bottom - rect.top);
		WritePrivateProfileString("Information", "WinHigh", tmp, iniDir);
		// ���״̬��0-��
		WritePrivateProfileString("Information", "IsZoomed", "0", iniDir);
	}
	return TRUE;
}


// ��ȡ�����ļ����ã��ָ�����λ��
BOOL CPaperlessDlg::InitFrmPosFromFile()
{
	GtWriteTrace(30, "%s:%d: InitFrmPosFromFile!", __FUNCTION__, __LINE__);
	// ���Ͻ�x����
	int nWinXPos = 0;
	// ���Ͻ�y����
	int nWinYPos = 0;
	// ���ڿ�
	int nWinWidth = 0;
	// ���ڸ�
	int nWinHigh = 0;
	// ��ʱ�ַ���
	char strTmp[32] = {0};
	CString iniDir = GetAppPath()+"\\win.ini";

	// ��ȡ�����ļ��еĴ�����ʾ��λ��
	GetPrivateProfileString("Information", "WinXPos", "-1", strTmp, sizeof(strTmp)-1, iniDir);
	nWinXPos = atoi(strTmp);

	GetPrivateProfileString("Information", "WinYPos", "-1", strTmp, sizeof(strTmp)-1, iniDir);
	nWinYPos = atoi(strTmp);

	GetPrivateProfileString("Information", "WinWidth", "-1", strTmp, sizeof(strTmp)-1, iniDir);
	nWinWidth = atoi(strTmp);

	GetPrivateProfileString("Information", "WinHigh", "-1", strTmp, sizeof(strTmp)-1, iniDir);
	nWinHigh = atoi(strTmp);

	// ��ȡ�����ļ����ϴ��˳��Ĵ����Ƿ������״̬���ǣ����´���ʾ��Ҫ�ָ����
	GetPrivateProfileString("Information", "IsZoomed", "0", strTmp, sizeof(strTmp)-1, iniDir);
	if (0 == strcmp(strTmp, "1"))
	{
		// �ϴ��˳�Ϊ���״̬�������´���ʾΪ���
		isLastTimeExitZoomed = true;
	}
	// �������״̬��λ��
	if (nWinXPos < 0 || nWinYPos < 0 || nWinWidth <= 0 || nWinHigh <= 0)
	{
		// �����ļ������ݣ�Ĭ��λ�ã���Ļ���·��ķ�֮һ��λ��
		GtWriteTrace(30, "%s:%d: Ĭ��λ�����ã���Ļ���·��ķ�֮һ!", __FUNCTION__, __LINE__);
		// ��ȡ������λ�ã�ֻ�����������ڵ׶˵����
		CRect rect;
		::GetWindowRect(::FindWindow(_T("Shell_TrayWNd"), NULL), &rect);
		// �����´���ʾ��λ�ã�������Ļ���·��ķ�֮һ��λ��
		this->SetWindowPos(&wndTop, rect.right / 2, rect.top / 2, rect.right / 2, rect.top / 2, SWP_HIDEWINDOW);
	}
	else
	{
		// �����´���ʾ��λ�ã��������ļ�λ����ʾ
		GtWriteTrace(30, "%s:%d: �������ļ�λ������!", __FUNCTION__, __LINE__);
		this->SetWindowPos(&wndTop, nWinXPos, nWinYPos, nWinWidth, nWinHigh, SWP_HIDEWINDOW);
	}
	return TRUE;
}


void CPaperlessDlg::OnMyClose()
{
	//MessageBox("OnMyClose!");
	SaveFrmPosToFile();
	this->ShowWindow(SW_HIDE);
}


// ���ù���
void CPaperlessDlg::StartKeyBoardHook()
{
	if(!m_hookFlag)
	{
		// �����õĿ�ݼ�
		COMBKEY combKey[4];
		HWND myHwnd[4];
		UINT myMsgType[4];
		memset(combKey, 0, sizeof(COMBKEY) * 4);
		memset(myHwnd, 0, sizeof(HWND) * 4);
		memset(myMsgType, 0, sizeof(UINT) * 4);

		char tmpKey[32] = {0};
		char sHotKeyValue[32] = {0};
		// �����ļ���ȡ��ͼ�ȼ�ֵ
		GetPrivateProfileString("Information", "HotKeyValue", "3|162|164|83|0|", sHotKeyValue,
			sizeof(sHotKeyValue)-1, GetAppPath()+"\\win.ini");
		// ������ݼ�
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 0);
		combKey[0].count = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 1);
		combKey[0].keys[0] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 2);
		combKey[0].keys[1] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 3);
		combKey[0].keys[2] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 4);
		combKey[0].keys[3] = atoi(tmpKey);
		myHwnd[0] = m_hWnd;
		myMsgType[0] = WM_SCREENSHOT;

		// �Զ�����������ݼ�
		combKey[1].count = 2;
		combKey[1].keys[0] = VK_LCONTROL;
		combKey[1].keys[1] = 80;
		combKey[1].keys[2] = 0;
		combKey[1].keys[3] = 0;
		myHwnd[1] = m_hWnd;
		myMsgType[1] = WM_CONTINUE_INPUT;


		// ����֪ͨ���ھ����֪ͨ��Ϣ����
		SetHWndAndMsgType(myHwnd, myMsgType);
		// ���ÿ�ݼ�
		SetCombKey(combKey);
		// ����ȫ�ּ��̹���
		SetGHook_KEYBOARD();
	}
	else
	{
		FreeGHook_KEYBOARD();
	}
}


// ������ͼ
LRESULT CPaperlessDlg::OnScreenshot(WPARAM wParam, LPARAM lParam)
{
	//	MessageBox("�յ�������Ϣ!");
	//	m_ScreenshotDlg->ShowWindow(SW_SHOW);

	if (pScreenshotDlg == NULL)
	{
		pScreenshotDlg = new CScreenshotDlg();
		pScreenshotDlg->DoModal();
		delete pScreenshotDlg;
		pScreenshotDlg = NULL;
	}
	else
	{
		pScreenshotDlg->ShowWindow(SW_SHOWNORMAL);
	}

	return 0;
}


// �յ���ͼ�Ի������Ϣ
LRESULT CPaperlessDlg::OnScreenDlgMessage(WPARAM wParam, LPARAM iParam)
{
	char *str = (char *)wParam;
	GtWriteTrace(30, "[MainFrm]Receive pScreenshotDlg message = [%s]", str);

	// �ж��յ�����Ϣ
	if (0 == strcmp(str, RECOGNIZE_PICTURE_FAILED))
	{
		// ��ά��ʶ��ʧ�ܣ������ֶ������ά����
		ShowManualInput("��ά��ʶ��ʧ�ܣ����ֶ�������ˮ�Ż����½�ȡ��ά�룡");
	}
	else if (0 == strcmp(str, QR_CODE_NOT_EXIST))
	{
		// ��ˮ�Ų����ڣ���ʾ�Ƿ����������ά����
		ShowManualInput("��ˮ�Ų����ڣ���ȷ�Ϻ��������룡");
	}
	return 0;
}


LRESULT CPaperlessDlg::OnContinueInput(WPARAM wParam, LPARAM iParam)
{
	SendToWindows();
	//MessageBox("tsadfd");
	return 0;
}


LRESULT CPaperlessDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_RBUTTONDOWN:
		{  
			// �Ҽ�����ʱ�����˵�  
			CMenu *pMenu = m_menu.GetSubMenu(0);
			if(pMenu){
				LPPOINT lpoint = new tagPOINT;  
				// �õ����λ��  
				::GetCursorPos(lpoint);
				SetForegroundWindow();
				pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, lpoint->x ,lpoint->y, AfxGetMainWnd());   
				delete lpoint;
			}	  
			break;  
		}  
	case WM_LBUTTONDBLCLK:
		// ˫������Ĵ���    
		// ��һ��˫���Ĵ�����ȡ�����ļ��˳�ǰ�Ƿ�����󻯣����������ʾ
		if (isFirstDbClickMenu && isLastTimeExitZoomed)
		{
			// ��һ�ε��������ϴ��˳����������״̬���ָ����
			this->ShowWindow(SW_SHOWMAXIMIZED);
		}
		else
		{
			if (this->IsZoomed())
			{
				// ��󻯣��ָ����
				this->ShowWindow(SW_SHOWMAXIMIZED);
			}
			else if (this->IsIconic())
			{
				// ��С�����ָ�ԭ��״̬
				this->ShowWindow(SW_SHOWNOACTIVATE);
			}
			else
			{
				this->ShowWindow(SW_SHOWNORMAL);
			}
		}
		// ����һ�ε�����Ϊ��
		isFirstDbClickMenu = false;
		break;  
	}
	return 0;
}


void CPaperlessDlg::OnSettingWin()
{
	if (pSettingDlg == NULL)
	{
		pSettingDlg = new SettingDlg();
		pSettingDlg->Create(IDD_SETTING, this);
		pSettingDlg->ShowWindow(SW_SHOWNA);
	}
	else
	{
		pSettingDlg->MyInit();
		pSettingDlg->ShowWindow(SW_SHOWNA);
	}
}


// ��ʾ��ά����ˮ�ŶԻ���
void CPaperlessDlg::ShowManualInput(char *tip)
{
	if (pInputDlg == NULL)
	{
		pInputDlg = new CManualInputDlg(tip, this);
		pInputDlg->Create(IDD_MANUALINPUT, this);
		pInputDlg->ShowWindow(SW_SHOWNA);
	}
	else
	{
		pInputDlg->SetTip(tip);
		pInputDlg->ResetEdit();
		pInputDlg->ShowWindow(SW_SHOWNA);
	}
}


// ���̲˵�->�ֶ�����
void CPaperlessDlg::OnManualInput()
{
	//int j = 0;
	//volatile int i = 10 / j;
	ShowManualInput("");
}


void CPaperlessDlg::OnSysCommand( UINT nID, LPARAM lParam)
{
	switch (nID)
	{
	case SC_MAXIMIZE:
		// ��󻯰�ť����¼�
		// ���洰��λ����Ϣ���Ƿ������Ϣ
		this->SaveFrmPosToFile();
		break;
	case SC_MINIMIZE:
		// ��С����ť����¼�
		// ���洰��λ����Ϣ���Ƿ������Ϣ
		this->SaveFrmPosToFile();
		break;
	default:
		break;
	}
	CDialogEx::OnSysCommand(nID, lParam);
}