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
	// 任务栏显示
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	// 显示居中
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
	GetDlgItem(IDC_STATIC2)->SetWindowText("正在发送消息，请等待......");

	CEdit* pEdit1 = NULL;
    CString str = NULL;
	string msgStr_rtn;

	pEdit1 = (CEdit*) GetDlgItem(IDC_EDIT1);
	pEdit1->GetWindowText(str);

	// 组待发送的json报文
	Json::Value msgStr_json;//表示一个json格式的对象
	msgStr_json["type"] = "1";
	msgStr_json["num"] = str.GetBuffer(str.GetLength());
	str.ReleaseBuffer();
	msgStr_json["picSource"] = "";
	// 转string
	msgStr_rtn = msgStr_json.toStyledString();

    // MessageBox(str, _T("程序运行结果"), MB_OK);
	GtWriteTrace(30, "send number message, buffer=[%s], size=[%d]", msgStr_rtn.c_str(), msgStr_rtn.length());
	if (!SendHttp(2, msgStr_rtn.c_str(), msgStr_rtn.length()))
	{
		GetDlgItem(IDC_STATIC2)->SetWindowText("发送失败，请重试！");
	}
	else
	{
		GetDlgItem(IDC_STATIC2)->SetWindowText("发送成功！");
		CDialog::OnOK();
	}
}


void CManualInput::OnBnClickedBtnCancel()
{
	CDialog::OnCancel();
}
