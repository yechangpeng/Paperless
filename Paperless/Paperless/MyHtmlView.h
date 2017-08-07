#pragma once

#ifdef _WIN32_WCE
#error "CHtmlView is not supported for Windows CE."
#endif 

// CMyHtmlView html view

class CMyHtmlView : public CHtmlView
{
	DECLARE_DYNCREATE(CMyHtmlView)

public:
	CMyHtmlView();           // protected constructor used by dynamic creation

protected:
	virtual ~CMyHtmlView();
	virtual int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	// 构造后第一次调用
	virtual void OnInitialUpdate();
	virtual void OnDestroy();

public:
	BOOL CreateFromStatic(CStatic* pStaticWnd, CWnd* pParent);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM iParam);
	afx_msg LRESULT OnMyResize(WPARAM wParam, LPARAM iParam);

	DECLARE_MESSAGE_MAP()
};


