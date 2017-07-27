// ManualInput.cpp : implementation file
//

#include "stdafx.h"
#include "XXbgService.h"
#include "ManualInputDlg.h"
#include "afxdialogex.h"
#include "ScreenshotDlg.h"
#include "MyTTrace.h"
#include "Json/json.h"


// CManualInput dialog

IMPLEMENT_DYNAMIC(CManualInput, CDialogEx)

CManualInput::CManualInput(CString tip, CWnd* pParent /*=NULL*/)
	: CDialogEx(CManualInput::IDD, pParent)
{
	this->tip = tip;
}

CManualInput::~CManualInput()
{
}

BOOL CManualInput::OnInitDialog()
{
	SetTip(this->tip.GetBuffer());
	this->tip.ReleaseBuffer();
	ResetEdit();
	// ��������ʾ
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	// ��ʾ����
	CenterWindow();
	return false;
}

void CManualInput::SetTip(char *pStrTip)
{
	GetDlgItem(IDC_STATIC2)->SetWindowText(pStrTip);
}

void CManualInput::ResetEdit()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString str;
	str.Format("%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	CEdit* pEdit1 = (CEdit*) GetDlgItem(IDC_EDIT1);
	pEdit1->SetFocus();
	pEdit1->SetWindowTextA(str);
	pEdit1->SetSel(0, -1);
	pEdit1->SetSel(-1, 0);
}

void CManualInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualInput, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEND, &CManualInput::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CManualInput::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CManualInput message handlers


void CManualInput::OnBnClickedBtnSend()
{
	// 
	GetDlgItem(IDC_STATIC2)->SetWindowText("���ڷ�����Ϣ����ȴ�......");

	CEdit* pEdit1 = NULL;
    CString str = NULL;
	string msgStr_rtn;

	pEdit1 = (CEdit*) GetDlgItem(IDC_EDIT1);
	pEdit1->GetWindowText(str);

	// ������͵�json����
	Json::Value msgStr_json;//��ʾһ��json��ʽ�Ķ���
	msgStr_json["type"] = "1";
	msgStr_json["num"] = str.GetBuffer(str.GetLength());
	str.ReleaseBuffer();
	msgStr_json["picSource"] = "";
	// תstring
	msgStr_rtn = msgStr_json.toStyledString();

    // MessageBox(str, _T("�������н��"), MB_OK);
	GtWriteTrace(30, "send number message, buffer=[%s], size=[%d]", msgStr_rtn.c_str(), msgStr_rtn.length());
	if (!SendHttp(2, msgStr_rtn.c_str(), msgStr_rtn.length()))
	{
		GetDlgItem(IDC_STATIC2)->SetWindowText("����ʧ�ܣ������ԣ�");
	}
	else
	{
		GetDlgItem(IDC_STATIC2)->SetWindowText("���ͳɹ���");
		CDialog::OnOK();
	}
}


void CManualInput::OnBnClickedBtnCancel()
{
	CDialog::OnCancel();
}