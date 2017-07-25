
// XXbgServiceView.cpp : CXXbgServiceView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "XXbgService.h"
#endif

#include "XXbgServiceDoc.h"
#include "XXbgServiceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "utils.h"

// CXXbgServiceView

IMPLEMENT_DYNCREATE(CXXbgServiceView, CHtmlView)

BEGIN_MESSAGE_MAP(CXXbgServiceView, CHtmlView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
	ON_MESSAGE(WM_HTML_SHOW,OnMyMessage)
END_MESSAGE_MAP()

// CXXbgServiceView 构造/析构

CXXbgServiceView::CXXbgServiceView()
{
	// TODO: 在此处添加构造代码
	//this->SetActiveWindow();
}

CXXbgServiceView::~CXXbgServiceView()
{
	
}

BOOL CXXbgServiceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	return CHtmlView::PreCreateWindow(cs);
}

void CXXbgServiceView::OnInitialUpdate()
{
	
	CHtmlView::OnInitialUpdate();
	
//	Navigate2(_T("http://www.msdn.microsoft.com/visualc/"),NULL,NULL);
	
}

LRESULT CXXbgServiceView::OnMyMessage(WPARAM wParam, LPARAM iParam)
{
	//char s_string[1000+1] = {0};
	//sprintf(s_string,"已应答%s,请点击查看",(char *)wParam);
	//::MessageBox(NULL,s_string,"提示",MB_OK);
	CHtmlView::OnInitialUpdate();
	
	Navigate2(_T((char *)wParam),NULL,NULL);

	if (AfxGetApp()->m_pMainWnd->IsZoomed())
	{
		// 最大化，恢复最大化
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	}
	else if (AfxGetApp()->m_pMainWnd->IsIconic())
	{
		// 最小化，恢复原先状态
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
	{
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
	}

	return 0;
}
// CXXbgServiceView 打印



// CXXbgServiceView 诊断

#ifdef _DEBUG
void CXXbgServiceView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CXXbgServiceView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CXXbgServiceDoc* CXXbgServiceView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXXbgServiceDoc)));
	return (CXXbgServiceDoc*)m_pDocument;
}
#endif //_DEBUG


// CXXbgServiceView 消息处理程序


