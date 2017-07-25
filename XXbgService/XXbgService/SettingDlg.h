#pragma once
#include "GHook/GHook.h"

// CSetting dialog

class CSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetting();

// Dialog Data
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	void MyInit();
private:
	COMBKEY combKey;
	int keyDown(BYTE key, char *kyeName);
	int keyFirstUp(BYTE key);
	BOOL isSpecialKey(BYTE key);
	BOOL isKeyUp;
};
