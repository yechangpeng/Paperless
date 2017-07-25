#pragma once


// CManualInput dialog

class CManualInput : public CDialogEx
{
	DECLARE_DYNAMIC(CManualInput)

public:
	CManualInput(CString tip, CWnd* pParent = NULL);   // standard constructor
	virtual ~CManualInput();
	void SetTip(char *pStrTip);
	void ResetEdit();

// Dialog Data
	enum { IDD = IDD_MANUALINPUT };
private:
	CString tip;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnCancel();
	virtual BOOL OnInitDialog();
};
