
// PaperlessRestartDlg.h : ͷ�ļ�
//

#pragma once

// ��ʱ���ص�����
void CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);
// ��ȡ��ǰ��������Ŀ¼
CString GetAppPath();

// CPaperlessRestartDlg �Ի���
class CPaperlessRestartDlg : public CDialogEx
{
// ����
public:
	CPaperlessRestartDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PAPERLESSRESTART_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int nTime;
	// ����������·��
	char sAppDir[256];

public:
	int RestartPaperlessApp();
};

