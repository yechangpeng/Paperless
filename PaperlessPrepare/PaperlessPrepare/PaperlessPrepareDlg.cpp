
// PaperlessPrepareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PaperlessPrepare.h"
#include "PaperlessPrepareDlg.h"
#include "afxdialogex.h"
#include "Json/json.h"
#include "Network/HttpComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPaperlessPrepareDlg �Ի���
CPaperlessPrepareDlg::CPaperlessPrepareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPaperlessPrepareDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nTime = 0;
	bTimerIsRun = FALSE;
	m_pThread = FALSE;
	// ���԰�ť
	pBtnRetry = FALSE;
	// ȡ����ť
	pBtnCancel = FALSE;
	// ״̬��ʾ
	pStatic = FALSE;
}

CPaperlessPrepareDlg::~CPaperlessPrepareDlg()
{
	MyDelFlushTimer();
}

void CPaperlessPrepareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPaperlessPrepareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CANCEL, &CPaperlessPrepareDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RETRY, &CPaperlessPrepareDlg::OnBnClickedRetry)
END_MESSAGE_MAP()


// CPaperlessPrepareDlg ��Ϣ�������

BOOL CPaperlessPrepareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ���԰�ť
	pBtnRetry = (CButton*) GetDlgItem(IDC_RETRY);
	// ȡ����ť
	pBtnCancel = (CButton*) GetDlgItem(IDC_CANCEL);
	// ״̬��ʾ
	pStatic = (CStatic*) GetDlgItem(IDC_STATIC);

	MyInitWin();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPaperlessPrepareDlg::OnPaint()
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
HCURSOR CPaperlessPrepareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ���ý���ˢ�¶�ʱ��
void CPaperlessPrepareDlg::MySetFlushTimer()
{
	if (!bTimerIsRun)
	{
		SetTimer(TIMEER_NO_REFLUSH, 500, TimerProc);
		bTimerIsRun = TRUE;
	}
}


// �������ˢ�¶�ʱ��
void CPaperlessPrepareDlg::MyDelFlushTimer()
{
	if (bTimerIsRun)
	{
		KillTimer(TIMEER_NO_REFLUSH);
		bTimerIsRun = FALSE;
	}
}


// ���ý�������
void CPaperlessPrepareDlg::MyInitWin()
{
	// ���ذ�ť
	pBtnRetry->ShowWindow(SW_HIDE);
	pBtnCancel->ShowWindow(SW_HIDE);

	// ������ʾ����
	CFont *font = new CFont();
	font->CreatePointFont(150, "����");
	pStatic->SetFont(font);
	delete font;
	font = NULL;

	// ���ø�����ʾ
	pStatic->SetWindowTextA("�������У����Ժ�   ");
	nTime = 0;

	// ��Ӷ�ʱ����ˢ����ʾ
	MySetFlushTimer();
	// �����̣߳����ͳ����������
	m_pThread = AfxBeginThread(ThreadFunc, this);
}


// ʧ��ʱ����Ĵ���
void CPaperlessPrepareDlg::MyRetryWin(const char *sTip)
{
	// �رն�ʱ��
	MyDelFlushTimer();

	// ��ʾ��ť
	pBtnRetry->ShowWindow(SW_SHOW);
	pBtnCancel->ShowWindow(SW_SHOW);

	// ������ʾ����
	CFont *font = new CFont();
	font->CreatePointFont(120, "����");
	pStatic->SetFont(font);
	delete font;
	font = NULL;
	// ���ø�����ʾ
	pStatic->SetWindowTextA(sTip);
	nTime = 0;
}


// ��ʱ���ص�����
void CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{
	CPaperlessPrepareDlg *pDlg= (CPaperlessPrepareDlg*)AfxGetApp()->m_pMainWnd;
	CStatic* pEdit = (CStatic*)pDlg->GetDlgItem(IDC_STATIC);
	CString sTip;

	pDlg->nTime = (pDlg->nTime + 1) % 4;
	switch (pDlg->nTime)
	{
	case 0:
		sTip.Format(_T("�������У����Ժ�   "));
		break;
	case 1:
		sTip.Format(_T("�������У����Ժ�.  "));
		break;
	case 2:
		sTip.Format(_T("�������У����Ժ�.. "));
		break;
	case 3:
		sTip.Format(_T("�������У����Ժ�..."));
		break;
	}
	pEdit->SetWindowText(sTip);
}

// �̺߳���
UINT ThreadFunc(LPVOID pParm)
{
	CPaperlessPrepareDlg *pDlg = (CPaperlessPrepareDlg *)pParm;
	
	// ��ȡ�����ļ�����ȡ��ǰ����汾
	char sVersion[32] = {0};
	// ��ȡ���³���http�����url
	char sUrl[256] = {0};
	// ���ͺ�������ֵ
	int nRet = 0;
	// ����ʧ�ܣ����ʧ��ԭ���λ��
	char sSendRet[128] = {0};

	GetPrivateProfileString("Information", "Version", "1.0", sVersion, sizeof(sVersion)-1, GetFilePath()+"\\win.ini");
	GetPrivateProfileString("Information", "POST_URL_UPDATE", "0", sUrl, sizeof(sUrl)-1, GetFilePath()+"\\win.ini");

	// �����ж�
	if (strcmp(sUrl, "0") == 0)
	{
		pDlg->MyRetryWin("��ⲻ����������ַ���ã����飡");
	}
	else
	{
		// ƴ������
		Json::Value jsonBuff;
		jsonBuff["type"] = "2";
		jsonBuff["string"] = "XXbgService.exe";
		jsonBuff["version"] = sVersion;
		jsonBuff["desVersion"] = "0";
		jsonBuff["other"] = "";
		string sData = jsonBuff.toStyledString();
		
		// ���Ĵ����ʱ�ļ���
		sprintf_s(g_sTmpFilePath, 256, "%s\\HttpRecvData.loadtmp", GetFilePath().GetBuffer());
		// ɾ��ԭ���ļ�
		DeleteFile(g_sTmpFilePath);
		// ���ͱ��ģ��������ر��Ĵ����ļ� g_sTmpFilePath ��
		nRet = SendData(sUrl, sData.c_str(), sData.length(), sizeof(sSendRet), sSendRet);
		char sTip[128] = {0};
		if (nRet != 0)
		{
			// ���ķ���ʧ��
			sprintf_s(sTip, sizeof(sTip)-1, "���ӷ�����ʧ�ܣ�%s", sSendRet);
			pDlg->MyRetryWin(sTip);
		}
		else
		{
			// ���ݽ�����ɣ������ѱ��浽������ʱ�ļ�
			// ��ȡ�ļ����������Ĳ�����
			AnalyzeData();
		}
	}
	return 0;
}



void CPaperlessPrepareDlg::OnBnClickedCancel()
{
	MyDelFlushTimer();
	CDialogEx::OnCancel();
}


void CPaperlessPrepareDlg::OnBnClickedRetry()
{
	MyInitWin();
}


BOOL CPaperlessPrepareDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
