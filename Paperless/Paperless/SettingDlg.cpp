// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Paperless.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "MyTTrace.h"
#include "GHook\GHook.h"
#include "utils.h"


#pragma comment(lib,"GHook.lib")
// SettingDlg dialog

IMPLEMENT_DYNAMIC(SettingDlg, CDialogEx)

SettingDlg::SettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SettingDlg::IDD, pParent)
{
	// ��ʼ������
	combKey.count = 0;
	combKey.keys[0] = 0;
	combKey.keys[1] = 0;
	combKey.keys[2] = 0;
	combKey.keys[3] = 0;

	isKeyUp = TRUE;
}

SettingDlg::~SettingDlg()
{
}

void SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &SettingDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &SettingDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()

// SettingDlg message handlers
void SettingDlg::OnBnClickedButtonOk()
{
	int specialKeyCount = 0;
	// �жϴ����õ��ȼ�ֵ�Ƿ�������������ַ� ctrl alt shift
	for (int i = 0; i < combKey.count; i++)
	{
		// �ж����а����Ƿ񺬿����ַ�
		if (isSpecialKey(combKey.keys[i]))
		{
			// �������ַ�������
			specialKeyCount++;
		}
	}
	if (specialKeyCount >= combKey.count)
	{
		MessageBox("�ȼ�����ֻ���� Ctrl Alt Shift �е�һ������������");
		return;
	}
	// �޸Ŀ�ݼ�
	SetXCombKey(0, combKey);

	// ��ȡ�ȼ����ƣ�д����־��
	CString cKeysName;
	CEdit *pEditDir = (CEdit *)GetDlgItem(IDC_EDIT_KEY);
	pEditDir->GetWindowText(cKeysName);
	GtWriteTrace(30, "[Setting]Save hot key succeed��key count=[%d], [%d], [%d], [%d], [%d], editBox=[%s]", combKey.count,
		combKey.keys[0], combKey.keys[1], combKey.keys[2], combKey.keys[3], cKeysName.GetBuffer());
	cKeysName.ReleaseBuffer();

	// ���ȼ�д�������ļ���
	CString iniDir = GetAppPath()+"\\win.ini";
	CString tmp;
	// �ȼ�ֵ
	tmp.Format("%d|%d|%d|%d|%d|", combKey.count, combKey.keys[0], combKey.keys[1], combKey.keys[2], combKey.keys[3]);
	WritePrivateProfileString("Information", "HotKeyValue", tmp, iniDir);
	// �ȼ�����
	WritePrivateProfileString("Information", "HotKeyName", cKeysName, iniDir);

	// ������ʾ
	CString cTmp;
	cTmp.Format("�����ȼ�[%s]�ɹ�", cKeysName.GetBuffer());
	cKeysName.ReleaseBuffer();
	//MessageBox(cTmp);

	// �رնԻ���
	CDialog::OnOK();
}


void SettingDlg::OnBnClickedButtonCancel()
{
	CDialog::OnCancel();
}



BOOL SettingDlg::OnInitDialog()
{
	MyInit();
	// ��������ʾ
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	// ��ʾ����
	CenterWindow();
	return TRUE;
}

void SettingDlg::MyInit()
{
	// �����ļ���ȡ��ǰ�ȼ�
	CString strHotKeyName;
	// �ȼ�����
	GetPrivateProfileString("Information", "HotKeyName", "Ctrl + ALT + S", strHotKeyName.GetBuffer(31), 31, GetAppPath()+"\\win.ini");
	strHotKeyName.ReleaseBuffer();
	CEdit *pStaHotKeyName = (CEdit *)GetDlgItem(IDC_STA_HOTKEY);
	pStaHotKeyName->SetWindowText(strHotKeyName);
}



BOOL SettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		do 
		{
			CWnd *pFocusWnd = GetFocus();
			// ������㲻��������ϣ��������˳�ѭ��
			if (pFocusWnd->GetDlgCtrlID() != IDC_EDIT_KEY)
			{
				break;
			}
			// ����ļ��� Shift, Ctrl, Alt, Space, 0~9, A~Z...�������������ж�Ӧ
			if (! (pMsg->wParam==VK_SHIFT || pMsg->wParam==VK_CONTROL || pMsg->wParam==VK_MENU || pMsg->wParam==VK_SPACE
				|| (pMsg->wParam>=48 && pMsg->wParam<=90)
				|| (pMsg->wParam>=96 && pMsg->wParam<=123 && pMsg->wParam != 108)
				|| (pMsg->wParam>=160 && pMsg->wParam<=165)
				|| (pMsg->wParam>=186 && pMsg->wParam<=192)
				|| (pMsg->wParam>=219 && pMsg->wParam<=222)
				))
			{
				break;
			}
			char str[32]={0};
			CString temp;
			CEdit *pEditDir = (CEdit *)GetDlgItem(IDC_EDIT_KEY);
			// �ж��Ƿ��¼��̰���������ϵͳ����İ��� ALT��F10
			if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
			{
				GetKeyNameText(pMsg->lParam, str, 31);
				// pMsg->wParam���Ǽ�ֵ,str�Ǽ�������
				//temp.Format("[%d]%d %s[%d]", WM_KEYFIRST, pMsg->wParam, str, WM_KEYLAST);
				//pEditDir->SetWindowText(temp);

				switch(pMsg->wParam)
				{
				case VK_SHIFT:
					if( GetKeyState(VK_LSHIFT) & 0x8000 )
					{
						// ��shift
						//temp.Format("[%d]%d %s[%d]", WM_KEYFIRST, VK_LSHIFT, str, WM_KEYLAST);
						//pEditDir->SetWindowText(temp);
						keyDown(VK_LSHIFT, str);
					}
					else if( GetKeyState(VK_RSHIFT) & 0x8000 )
					{
						// ��shift
						keyDown(VK_RSHIFT, str);
					}
					else if( GetKeyState(VK_SHIFT) & 0x8000 )
					{
						// shift
						keyDown(VK_SHIFT, str);
					}
					break;
				case VK_CONTROL:
					if( GetKeyState(VK_LCONTROL) & 0x8000 )
					{
						// ��ctrl
						keyDown(VK_LCONTROL, str);
					}
					else if( GetKeyState(VK_RCONTROL) & 0x8000 )
					{
						// ��ctrl
						keyDown(VK_RCONTROL, str);
					}
					else if( GetKeyState(VK_CONTROL) & 0x8000 )
					{
						// ctrl
						keyDown(VK_CONTROL, str);
					}
					break;
				case VK_MENU:
					if( GetKeyState(VK_LMENU) & 0x8000 )
					{
						// ��alt
						keyDown(VK_LMENU, str);
					}
					else if( GetKeyState(VK_RMENU) & 0x8000 )
					{
						// ��alt
						keyDown(VK_RMENU, str);
					}
					else if( GetKeyState(VK_MENU) & 0x8000 )
					{
						// alt
						keyDown(VK_MENU, str);
					}
					break;
				default:
					keyDown(pMsg->wParam, str);;
				}
			}
			else if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
			{
				keyFirstUp(pMsg->wParam);
			}
			return true;
		} while (0);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

int SettingDlg::keyDown(BYTE key, char *kyeName)
{
	CEdit *pEditDir = (CEdit *)GetDlgItem(IDC_EDIT_KEY);
	if (isKeyUp)
	{
		// ��������֮���ְ��µ��¼�����ս����ϵ���ʾ
		pEditDir->SetWindowText("");

		// ������ʱ�ȼ��ṹ��
		combKey.count = 0;
		combKey.keys[0] = 0;
		combKey.keys[1] = 0;
		combKey.keys[2] = 0;
		combKey.keys[3] = 0;
	}
	isKeyUp = false;
	if (combKey.count >= 4)
	{
		// ��ϼ�����4����������
		return -1;
	}
	else
	{
		if (isSpecialKey(key))
		{
			// �жϵ�ǰ�����Ƿ���������������ǰ��������ͨ��
			for (int i = 0; i < combKey.count; i++)
			{
				// �ж����а����Ƿ���ͨ��
				if (!isSpecialKey(combKey.keys[i]))
				{
					// ����ͨ����������ǰ����
					return -2;
				}
			}
		}
		// �����ɣ�����ǰ�������뵽combKey�У������½����ϵ�ֵ
		combKey.keys[combKey.count] = key;
		combKey.count++;
		//CString tmpEdit;
		//pEditDir->GetWindowText(tmpEdit);
		if (combKey.count == 1)
		{
			pEditDir->SetWindowText(kyeName);
		}
		else
		{
			CString tmp;
			tmp.Format(" + %s", kyeName);
			pEditDir->SetSel(-1);
			pEditDir->ReplaceSel(tmp);
		}
	}
	return 0;
}

int SettingDlg::keyFirstUp(BYTE key)
{
	isKeyUp = TRUE;
	return 0;
}

BOOL SettingDlg::isSpecialKey(BYTE key)
{
	if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL
		|| key == VK_MENU || key == VK_LMENU || key == VK_RMENU
		|| key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT)
	{
		// �������������
		return TRUE;
	}
	return false;
}