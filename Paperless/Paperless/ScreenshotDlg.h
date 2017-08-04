#pragma once
#include "MyEdit.h"

#ifndef MYTRACKER_
#include "MyTracker.h"
#endif
#include "afxwin.h"
#pragma once
#include "resource.h"

#define IDC_MY_EDIT 20000
#define EDIT_WIDTH 100
#define EDIT_HIGH 20
#define BTN_WIDTH 60
#define BTN_HIGH 20

// CScreenshotDlg dialog
class CScreenshotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenshotDlg)

public:
	CScreenshotDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScreenshotDlg();

// Dialog Data
	enum { IDD = IDD_SCREENSHOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedMyOkButton();
	afx_msg void OnBnClickedMyCancleButton();

public:
	int m_xScreen;
	int m_yScreen;
	BOOL m_bShowMsg;                //��ʾ��ȡ���δ�С��Ϣ
	BOOL m_bDraw;                   //�Ƿ�Ϊ��ȡ״̬
	BOOL m_bFirstDraw;              //�Ƿ�Ϊ�״ν�ȡ
	BOOL m_bQuit;                   //�Ƿ�Ϊ�˳�
	CPoint m_startPt;				//��ȡ�������Ͻ�
	CMyTracker m_rectTracker;     //��Ƥ����
	CBrush m_brush;					//
    HCURSOR m_hCursor;              //���
    HCURSOR m_hOldCursor;              //ԭ�ȹ��
	CBitmap * m_pBitmap;            //����λͼ
	CRgn m_rgn;						//������������

	CMyEdit m_tipEdit;
	CButton *myOkButton;
	CButton *myCancelButton;

public:
	HBITMAP CopyScreenToBitmap(LPRECT lpRect,BOOL bSave=FALSE);   //�������浽λͼ
	void DrawTip();                            //��ʾ������ʾ��Ϣ
	void DrawMessage(CRect &inRect,CDC * pDC);       //��ʾ��ȡ������Ϣ
	void PaintWindow();               //�ػ�����
	void ChangeRGB();
	
	BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName);
	// �ƶ� ȷ����ȡ����ť λ�õ���ͼ���Ե����ʾ
	void MoveShowOkCancelBtn();
	// ���� ȷ����ȡ����ť
	void HideenOkCancelBtn();
	

	DECLARE_MESSAGE_MAP()
};
