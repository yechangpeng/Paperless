// MyEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Paperless.h"
#include "MyEdit.h"
#include "MyTTrace.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{
	bitmap.LoadBitmap(IDB_BITMAP1);
	m_bMove=TRUE;
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CMyEdit ��Ϣ�������

int CMyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}


void CMyEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//***********************************************************************************
	
	CRect rect;
	GetWindowRect(&rect);

	int xScreen = GetSystemMetrics(SM_CXSCREEN);
	//int ySCreen = GetSystemMetrics(SM_CYSCREEN);

	if(m_bMove)
	{
		//�ƶ������Ͻ�
		MoveWindow(10,10,rect.Width(),rect.Height());
		m_bMove=FALSE;
	}
	else
	{
		//�ƶ������Ͻ�
		MoveWindow(xScreen-180,10,rect.Width(),rect.Height());
		m_bMove=TRUE;
	}
//**************************************************************************************
	CEdit::OnMouseMove(nFlags, point);
}


BOOL CMyEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//**************************************************************************************
    
	//ȡ�����ָ�ѡ
	this->SetSel(0,0);
	
	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);
	dcCompatible.SelectObject(&bitmap);

	BITMAP bmp;
	bitmap.GetBitmap(&bmp);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);
	dcCompatible.DeleteDC();
	return TRUE;
//*********************************************************************************
//	return CEdit::OnEraseBkgnd(pDC);
}

void CMyEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// TODO: �ڴ˴������Ϣ����������
	//**********************************************************************************
   //���ع����ʾ��,��ʵҲ�ò���
	this->HideCaret();
   //*********************************************************************************
}


HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
//	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	//***************************************************************************************
	//�������ֱ���͸��
	pDC->SetBkMode(TRANSPARENT);
	return NULL;
	//**************************************************************************************
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
//	return hbr;
}
