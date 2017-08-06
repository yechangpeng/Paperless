// MyHtmlView.cpp : implementation file
//

#include "stdafx.h"
#include "Paperless.h"
#include "PaperlessDlg.h"
#include "MyHtmlView.h"
#include "MyTTrace.h"
#include "utils.h"


// CMyHtmlView
BEGIN_MESSAGE_MAP(CMyHtmlView, CHtmlView)
	ON_MESSAGE(WM_HTML_SHOW, OnMyMessage)
	ON_MESSAGE(WM_HTML_RESIZE, OnMyResize)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CMyHtmlView, CHtmlView)

CMyHtmlView::CMyHtmlView()
{

}

CMyHtmlView::~CMyHtmlView()
{
}

void CMyHtmlView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
}

BOOL CMyHtmlView::CreateFromStatic(CStatic* pStaticWnd, CWnd* pParent)
{ 
	ASSERT(NULL!=pStaticWnd && NULL!=pStaticWnd->GetSafeHwnd()); 
	ASSERT(NULL!=pParent && NULL!=pParent->GetSafeHwnd()); 

	CRect rc;
	pStaticWnd->GetClientRect(&rc);

	int nID = pStaticWnd->GetDlgCtrlID();
	LPCTSTR lpClassName = AfxRegisterWndClass(NULL);
	return Create(lpClassName, _T(""), WS_CHILD|WS_VISIBLE, rc, pParent, nID, NULL);
}


int CMyHtmlView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{ 
	// TODO: Add your message handler code here and/or call default 

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


void CMyHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}
// CMyHtmlView diagnostics
#ifdef _DEBUG
void CMyHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}
void CMyHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CMyHtmlView message handlers


LRESULT CMyHtmlView::OnMyMessage(WPARAM wParam, LPARAM iParam)
{
	//char s_string[1000+1] = {0};
	//sprintf(s_string,"已应答%s,请点击查看",(char *)wParam);
	//::MessageBox(NULL,s_string,"提示",MB_OK);

	CString sWelcomePath = GetAppPath();
	sWelcomePath += "\\PaperlessWelcome.htm";
	Navigate2(_T(sWelcomePath.GetBuffer()));
	CPaperlessDlg *pPaperlessDlg = (CPaperlessDlg *)AfxGetApp()->m_pMainWnd;
	// 判断是否是第一次弹出窗口
	if (pPaperlessDlg->isFirstDbClickMenu && pPaperlessDlg->isLastTimeExitZoomed)
	{
		// 将第一次单击置为否
		pPaperlessDlg->isFirstDbClickMenu = false;
		// 第一次单击，且上次退出程序是最大化状态，恢复最大化
		pPaperlessDlg->ShowWindow(SW_SHOWMAXIMIZED);
	}
	else
	{
		// 将第一次单击置为否
		pPaperlessDlg->isFirstDbClickMenu = false;
		if (pPaperlessDlg->IsZoomed())
		{
			// 最大化，恢复最大化
			pPaperlessDlg->ShowWindow(SW_SHOWMAXIMIZED);
		}
		else if (pPaperlessDlg->IsIconic())
		{
			// 最小化，恢复原先状态
			pPaperlessDlg->ShowWindow(SW_SHOWNOACTIVATE);
		}
		else
		{
			pPaperlessDlg->ShowWindow(SW_SHOWNORMAL);
		}
	}
	Navigate2(_T((char *)wParam),NULL,NULL);
	CHtmlView::OnInitialUpdate();

	return 0;
}


LRESULT CMyHtmlView::OnMyResize(WPARAM wParam, LPARAM iParam)
{
	int cx = LOWORD(iParam);
	int cy = HIWORD(iParam);
	MoveWindow(0, 0, cx, cy);

	//GtWriteTrace(30, "%s:%d: OnMyResize(%d, %d)!", __FUNCTION__, __LINE__, cx, cy);
	CHtmlView::OnInitialUpdate();

	return 0;
}