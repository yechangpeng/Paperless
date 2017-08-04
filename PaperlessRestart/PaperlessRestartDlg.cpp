
// PaperlessRestartDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PaperlessRestart.h"
#include "PaperlessRestartDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPaperlessRestartDlg �Ի���


CPaperlessRestartDlg::CPaperlessRestartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPaperlessRestartDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// �����ļ���ȡ����
	CString strSettingDir = GetAppPath()+"\\win.ini";
	// ��ȡ�ȴ�����ʱ��
	char sWaitTime[32] = {0};
	memset(sAppDir, 0, sizeof(sAppDir));
	sprintf_s(sAppDir, sizeof(sAppDir)-1, "%s\\XXbgService.exe", GetAppPath().GetBuffer());
	//GetPrivateProfileString("Setting", "AppDir", "./Paperless.exe", sAppDir, sizeof(sAppDir)-1, strSettingDir);
	GetPrivateProfileString("Information", "WaitTime", "3", sWaitTime, sizeof(sWaitTime)-1, strSettingDir);
	nTime = atoi(sWaitTime);
}

void CPaperlessRestartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPaperlessRestartDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CPaperlessRestartDlg ��Ϣ�������

BOOL CPaperlessRestartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_STATIC);
	CString sTip;
	if (((CPaperlessRestartApp*)AfxGetApp())->sParam == "EXCEPTION_RESTART")
	{
		// �ж�����ʱ��
		if (nTime <= 0)
		{
			// ֱ������
			RestartPaperlessApp();
		}
		else
		{
			// �����ͨ���������쳣����ĳ����������������
			sTip.Format(_T("Ӧ�ó����쳣�˳�������%d�������������"), nTime);
			pEdit->SetWindowText(sTip);
			nTime--;
			// ��Ӷ�ʱ��
			SetTimer(1, 1000, TimerProc);
		}
	}
	else
	{
		// ����������������Ч
		sTip.Format(_T("����������������Ч��"), nTime);
		pEdit->SetWindowText(sTip);
		// IDCANCEL
		CPaperlessRestartDlg *pDlg= (CPaperlessRestartDlg*)AfxGetApp()->m_pMainWnd;
		CButton* pButton = (CButton*)pDlg->GetDlgItem(IDCANCEL);
		pButton->SetWindowText("ȷ��");

	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPaperlessRestartDlg::OnPaint()
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
HCURSOR CPaperlessRestartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����Ӧ�ó���
int CPaperlessRestartDlg::RestartPaperlessApp()
{
	CPaperlessRestartDlg *pDlg= (CPaperlessRestartDlg*)AfxGetApp()->m_pMainWnd;
	CEdit* pEdit = (CEdit*)pDlg->GetDlgItem(IDC_STATIC);
	CString sTip;
	sTip.Format(_T("Ӧ�ó������쳣��������������..."), pDlg->nTime);
	pEdit->SetWindowText(sTip);

	// ����������
	//ShellExecute(NULL, "open", sAppDir, (LPCSTR)"EXCEPTION_RESTART", NULL, SW_SHOWNORMAL);
	ShellExecute(NULL, "open", sAppDir, NULL, NULL, SW_SHOWNORMAL);
	pDlg->KillTimer(1);
	pDlg->DestroyWindow();

	return 0;
}


// ��ʱ���ص�����
void CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{
	CPaperlessRestartDlg *pDlg= (CPaperlessRestartDlg*)AfxGetApp()->m_pMainWnd;
	CEdit* pEdit = (CEdit*)pDlg->GetDlgItem(IDC_STATIC);
	CString sTip;
	if (pDlg->nTime > 0)
	{
		sTip.Format(_T("Ӧ�ó����쳣�˳�������%d�������������"), pDlg->nTime);
		pEdit->SetWindowText(sTip);
		pDlg->nTime--;
	}
	else
	{
		pDlg->RestartPaperlessApp();
	}
}


/* ��ȡ��ǰ����ִ��·��
*/
CString GetAppPath()
{
	char szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath = szPath;
	int nPos = strPath.ReverseFind('\\');
	int nLen = strPath.GetLength();
	if(nPos > 0)
		strPath = strPath.Left(nPos);
	return strPath;
}