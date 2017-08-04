
// PaperlessRestart.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PaperlessRestart.h"
#include "PaperlessRestartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPaperlessRestartApp

BEGIN_MESSAGE_MAP(CPaperlessRestartApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPaperlessRestartApp ����

CPaperlessRestartApp::CPaperlessRestartApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPaperlessRestartApp ����

CPaperlessRestartApp theApp;


// CPaperlessRestartApp ��ʼ��

BOOL CPaperlessRestartApp::InitInstance()
{
	// ��һʵ������
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, this->m_pszAppName);// ���������
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ������л������������ͷž������λ��������
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		// �����˳�
		//::MessageBoxA(NULL, "�����Ѿ����У��������ظ�������", "����", MB_OK);
		return FALSE;
	}

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


	AfxEnableControlContainer();

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

	// ��ȡ���ó���Ĳ���
	for (int i = 1; i < __argc; i++)
	{
		sParam += __targv[i];
	}
	//::MessageBoxA(NULL, (const char*)sParam.GetBuffer(), "��ʾ", MB_OK);

	CPaperlessRestartDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

