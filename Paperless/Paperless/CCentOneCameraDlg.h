#pragma once

#include "CBaseReadIDCard.h"
#include "CentermIFImp.h"

// CCentOneCameraDlg dialog
class CCentOneCameraDlg : public CDialogEx, public CBaseSaveDeskPic
{
	DECLARE_DYNAMIC(CCentOneCameraDlg)

public:
	CCentOneCameraDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCentOneCameraDlg();
	virtual BOOL OnInitDialog();
public:
	// ��д����������ͷ��ȡ���֤��Ƭ
	virtual int MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm);
	// ��д����������ͷ��ȡ������Ƭ
	virtual int MySaveEnvPic(const char *pSaveEnvPicFilenm);

// Dialog Data
	enum { IDD = IDD_CENT_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
	CStatic *pVideoWnd;
	CCentermIFImp *mHighCamera;

	DECLARE_MESSAGE_MAP()
};
