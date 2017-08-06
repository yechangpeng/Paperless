// ScreenshotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Paperless.h"
#include "ScreenshotDlg.h"
#include "afxdialogex.h"
#include "utils.h"
#include "MyTTrace.h"
#include "Network/HttpComm.h"


// CScreenshotDlg dialog

IMPLEMENT_DYNAMIC(CScreenshotDlg, CDialogEx)

CScreenshotDlg::CScreenshotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScreenshotDlg::IDD, pParent)
{
	//*******************************************************************************
    //��ʼ����Ƥ����,������resizeMiddle ����
	m_rectTracker.m_nStyle=CMyTracker::resizeMiddle|CMyTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1, -2, -3, -4);
	//���þ�����ɫ
	m_rectTracker.SetRectColor(RGB(10, 100, 130));
	//���þ��ε���ʱ���,Ĭ�ϵ�̫С��,Ū�˸�����
	m_rectTracker.SetResizeCursor(IDC_CURSOR6,IDC_CURSOR5,IDC_CURSOR2,IDC_CURSOR3,IDC_CURSOR4);

    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);
    
	// ����ϵͳ���
	m_hOldCursor = AfxGetApp()-> LoadStandardCursor(IDC_ARROW);
    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);
	
	m_bDraw=FALSE;
	m_bFirstDraw=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
    m_startPt=0;
    
	//��ȡ��Ļ�ֱ���
	m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	m_yScreen = GetSystemMetrics(SM_CYSCREEN);

	//��ȡ��Ļ��λͼ��
	CRect rect(0, 0, m_xScreen, m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));
    
	//��ʼ��ˢ�´������� m_rgn
	m_rgn.CreateRectRgn(0, 0, 50, 50);
	//*******************************************************************************
}

CScreenshotDlg::~CScreenshotDlg()
{
}

void CScreenshotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tipEdit);
}


BEGIN_MESSAGE_MAP(CScreenshotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MY_OK_BUTTON, OnBnClickedMyOkButton)
	ON_BN_CLICKED(IDC_MY_CANCLE_BUTTON, OnBnClickedMyCancleButton)
END_MESSAGE_MAP()
// CScreenshotDlg message handlers


BOOL CScreenshotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//**************************************************************************
	//�ѶԻ������ó�ȫ�����㴰��
	SetWindowPos(&wndTopMost, 0, 0, m_xScreen, m_yScreen, SWP_SHOWWINDOW);

	// ȷ�ϡ�ȡ����ť��ʼ��
	myOkButton = (CButton *)GetDlgItem(IDC_MY_OK_BUTTON);
	myOkButton->MoveWindow(0, 0, BTN_WIDTH, BTN_HIGH);
	myOkButton->ShowWindow(false);

	myCancelButton = (CButton *)GetDlgItem(IDC_MY_CANCLE_BUTTON);
	myCancelButton->MoveWindow(0, 0, BTN_WIDTH, BTN_HIGH);
	myCancelButton->ShowWindow(false);
	
	/*
	myEdit = new CEdit();
	myEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER, CRect(600, 100, BTN_WIDTH, BTN_HIGH), this, IDC_MY_EDIT);
	myEdit->MoveWindow(600, 300, EDIT_WIDTH, EDIT_HIGH);
	myEdit->ShowWindow(true);
	*/

	//�ƶ�������ʾ����
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
    m_tipEdit.MoveWindow(10, 10, rect.Width(), rect.Height());

	//��ʾ������ʾ��������
	DrawTip();
	
	//���񰴼���Ϣ����,���Ի���ľ�����ݵ�CCatchScreenApp��
	((CPaperlessApp *)AfxGetApp())->m_hwndDlg = m_hWnd;
	//**************************************************************************
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CScreenshotDlg::OnPaint()
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
         
		//��ʾ��ȡ���δ�С��Ϣ
		if(m_bShowMsg && m_bFirstDraw)
		{
			//�õ���ǰ���δ�С
			CRect rect;
			m_rectTracker.GetTrueRect(&rect);
			//����CPaintDC ��Ϊ�˲����˴����ϻ���Ϣ
			DrawMessage(rect, &dc);
		}

		//������Ƥ�����
		if(m_bFirstDraw)
		{
			m_rectTracker.Draw(&dc);
		}
		CDialog::OnPaint();
	}
}


// ----------------------add ycp-------------
void CScreenshotDlg::OnBnClickedMyOkButton()
{
	//����λͼ,bSave ΪTRUE,
	CopyScreenToBitmap(m_rectTracker.m_rect, TRUE);
	CDialog::OnOK();
}

void CScreenshotDlg::OnBnClickedMyCancleButton()
{
	CDialog::OnCancel();
}


// ��ʾ ȷ����ȡ�� ��ť�����ƶ�����λ�õ���ͼ���Ե
void CScreenshotDlg::MoveShowOkCancelBtn()
{
	int x_tmp = 0;
	int y_tmp = 0;
	int x_abs = 0;
	int y_abs = 0;
	// ��ȡ ��ͼ�򴰿ڴ�С left���Ͻ�X���꣬top���Ͻ�Y���꣬right���½�X���꣬bottom���½�Y����
	CRect rect = m_rectTracker.m_rect;
	// ��ȡ��ͼ���������½ǵ�����
	x_abs = rect.left - rect.right > 0 ? rect.left : rect.right;
	y_abs = rect.bottom - rect.top > 0 ? rect.bottom : rect.top;
	// �ж� ��ͼ���������½��Ƿ񿿽���Ļ�±�Ե���Ƿ��㹻��ť���ã����㰴ťy����
	y_tmp = (y_abs + BTN_HIGH + 5 > m_yScreen) ? (y_abs - BTN_HIGH - 5) : (y_abs + 5);
	// �ж� ��ͼ���������½��Ƿ񿿽���Ļ���Ե���Ƿ��㹻��ť���ã����㰴ťx����
	x_tmp = (x_abs - 2 * (BTN_WIDTH + 5) < 0) ? 0 : (x_abs - 2 * (BTN_WIDTH + 5));
	// �ƶ���ť����ͼ���Եλ��
	myOkButton->MoveWindow(x_tmp + BTN_WIDTH + 5, y_tmp, BTN_WIDTH, BTN_HIGH);
	myCancelButton->MoveWindow(x_tmp, y_tmp, BTN_WIDTH, BTN_HIGH);
	// ��ʾ��ť
	myOkButton->ShowWindow(true);
	myCancelButton->ShowWindow(true);
}

// ���� ȷ����ȡ�� ��ť
void CScreenshotDlg::HideenOkCancelBtn()
{
	myOkButton->ShowWindow(false);
	myCancelButton->ShowWindow(false);
}
// ----------------------add ycp-------------

void CScreenshotDlg::OnOK()
{
	CDialogEx::OnOK();
}


void CScreenshotDlg::OnCancel()
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	if(m_bFirstDraw)
	{
		// ��ȡ��ESC������������ ȷ�ϡ�ȡ����ť
		HideenOkCancelBtn();
		// ȡ���ѻ����α���
		m_bFirstDraw=FALSE;
		m_bDraw=FALSE;
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	else
	{
		CDialogEx::OnCancel();
		//ShowWindow(SW_HIDE);
	}
}


void CScreenshotDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	if(m_bDraw)
	{
		//��̬�������δ�С,��ˢ�»���
		m_rectTracker.m_rect.SetRect(m_startPt.x+4,m_startPt.y+4,point.x,point.y);
		PaintWindow();
	}
	   
	//�ֲ�������С��λ��ʱ,���ղ���MouseMove��Ϣ
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		m_tipEdit.SendMessage(WM_MOUSEMOVE);
       
	ChangeRGB();
	CDialogEx::OnMouseMove(nFlags, point);
}


void CScreenshotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//GtWriteTrace(30,"[ScreenDlg]OnLButtonDown()");
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //�жϻ���λ��
	if(nHitTest<0)
	{
		if(!m_bFirstDraw)
		{
			// ���� ȷ�ϡ�ȡ����ť
			HideenOkCancelBtn();

			//��һ�λ�����
			m_startPt=point;
			m_bDraw=TRUE;
			m_bFirstDraw=TRUE;
			//���õ�����갴��ʱ��С�ľ��δ�С
			m_rectTracker.m_rect.SetRect(point.x,point.y,point.x+4,point.y+4);	
			
			//��֤����굱��ʱ������ʾ��Ϣ
			if(m_bFirstDraw)
			  m_bShowMsg=TRUE;		
			DrawTip();
			PaintWindow();
		}
		else
		{
			// ��ѡ�н�ͼ�����ҵ��ѡ����֮�⣬�����д���
		}
	}
	else
	{
		// �����ͼ���ڲ������� ȷ�ϡ�ȡ����ť
		HideenOkCancelBtn();

		//��֤����굱��ʱ������ʾ��Ϣ
		m_bShowMsg=TRUE;		
		PaintWindow();
		
		if(m_bFirstDraw)
		{
			//������Сʱ,Track���Զ��������δ�С,��Щ�ڼ�,��Ϣ��CRectTracker�ڲ�����
			m_rectTracker.Track(this,point,TRUE);
			//SendMessage(WM_LBUTTONUP,NULL,NULL);
			PaintWindow();

		}
	}
//****************************************************************************************
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CScreenshotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//****************************************************************************************    
	m_bShowMsg=FALSE;
	m_bDraw=FALSE;
	DrawTip();
	PaintWindow();
//****************************************************************************************
	CDialogEx::OnLButtonUp(nFlags, point);

	// ���������ѡ�н�ͼ������ʾ ȷ����ȡ����ť
	MoveShowOkCancelBtn();
}


void CScreenshotDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//GtWriteTrace(30,"[ScreenDlg]OnLButtonDblClk()");
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//****************************************************************************************
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);
	//������Ǿ����ڲ�˫��
	if(nHitTest==8)      
	{
		//����λͼ��ճ������,bSave ΪTRUE,
		CopyScreenToBitmap(m_rectTracker.m_rect,TRUE);
		CDialog::OnOK();
	}
//****************************************************************************************
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


BOOL CScreenshotDlg::OnEraseBkgnd(CDC* pDC)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//**************************************************************************************
	//�������������ȫ���Ի��򱳾�
	
	BITMAP bmp;
	m_pBitmap->GetBitmap(&bmp);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	dcCompatible.SelectObject(m_pBitmap);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);
	
	return TRUE;
//**************************************************************************************
//	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CScreenshotDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//***********************************************************************
	//���øı��ȡ���δ�Сʱ���
	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
		             &&!m_bDraw &&m_bFirstDraw) //�˴��жϱ���ȡʱ����ʼ��Ϊ��ɫ���
    {
		return TRUE; 
	}

	//���ò�ɫ���
	SetCursor(m_hCursor);
	// SetCursor(m_hOldCursor);
	return TRUE;
//*******************************************************************
//	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
void CScreenshotDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//GtWriteTrace(30,"[ScreenDlg]OnRButtonUp()");
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//****************************************************************************************
	if(m_bFirstDraw)
	{
		//����Ѿ���ȡ���������ȡ����
		m_bFirstDraw=FALSE;
		//������δ�С
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		DrawTip();
		PaintWindow();
	}
	else
	{
		 // �Ի����˳�
		 CDialog::OnOK();
	}
//****************************************************************************************
	CDialogEx::OnRButtonUp(nFlags, point);
	// �Ҽ�����ȡ��ѡ�еĽ�ͼ��������ȷ�ϡ�ȡ����ť
	HideenOkCancelBtn();
}


HBRUSH CScreenshotDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
//***********************************************************
	//���ò�����ʾ�����ı���ɫ
	if(pWnd->GetDlgCtrlID()==IDC_EDIT1)
	{
		pDC->SetTextColor(RGB(255,255,255));
	}
//***************************************************************
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

//*********************���ӵĺ���**********************************************************
//������Ļ,��δ������������� �Ǻ�
HBITMAP CScreenshotDlg::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)
//lpRect ����ѡ������
{
	HDC       hScrDC, hMemDC;
	// ��Ļ���ڴ��豸������
	HBITMAP    hBitmap, hOldBitmap;
	// λͼ���
	int       nX, nY, nX2, nY2;
	// ѡ����������
	int       nWidth, nHeight;
	
	// ȷ��ѡ������Ϊ�վ���
	if (IsRectEmpty(lpRect))
		return NULL;
	//Ϊ��Ļ�����豸������
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);

	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hMemDC = CreateCompatibleDC(hScrDC);
	// ���ѡ����������
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//ȷ��ѡ�������ǿɼ���
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > m_xScreen)
		nX2 = m_xScreen;
	if (nY2 > m_yScreen)
		nY2 = m_yScreen;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// ����һ������Ļ�豸��������ݵ�λͼ
	hBitmap = CreateCompatibleBitmap
		(hScrDC, nWidth, nHeight);
	// ����λͼѡ���ڴ��豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// ����Ļ�豸�����������ڴ��豸��������
	if(bSave)
	{
		//����������DC,��bSaveΪ��ʱ�ѿ�ʼ�����ȫ��λͼ,����ȡ���δ�С����
		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);
        
		BitBlt(hMemDC, 0, 0, nWidth, nHeight, dcCompatible, nX, nY, SRCCOPY);

	}
	else
	{
		BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
	}

	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//�õ���Ļλͼ�ľ��
	//��� 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	
	if(bSave)
	{
		// ��ȡ��ǰ�����������·��
		CString strDir = GetAppPath();
		// �жϽ�ͼ�ļ��洢Ŀ¼�Ƿ���ڣ��������򴴽�
		CString strSubDir = strDir + "\\CatchScreen";
		if (!PathIsDirectory(strSubDir))
		{
			//����Ŀ¼,���еĻ���Ӱ��
			::CreateDirectory(strSubDir, NULL);
		}
		// ��ȡϵͳʱ�䣬
		SYSTEMTIME st;
		GetLocalTime(&st);
		// ���ͼ�ļ����·���ļ���
		CString strPath;
		strPath.Format("\\CatchScreen\\CatchScreen-%04d%02d%02d-%02d%02d%02d-%03d.png",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		// ƴ��ͼ�ļ�����·��
		strDir.Append(strPath);
		// MessageBox(strDir);
		// �������ݴ浽�ļ���
		//SaveBitmapToFile(hBitmap, "C:\\Users\\Public\\Pictures\\test.png");
		if (SaveBitmapToFile(hBitmap, strDir))
		{
			// ����ͼƬ�ɹ�
			//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_HIDEWINDOW);
			ShowWindow(false);
			// ���ͱ���
			SendDataPrepare(0, strDir.GetBuffer());
			strDir.ReleaseBuffer();
		}
		else
		{
			MessageBox("��ͼ����ʧ�ܣ������ԣ�");
		}
	}
	// ����λͼ���
	return hBitmap;
}


// http://blog.csdn.net/oldmtn/article/details/7480166
BOOL CScreenshotDlg::SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName )
{ 
	HDC hDC; //�豸������ 
	int iBits; //��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ��� 
	WORD wBitCount; //λͼ��ÿ��������ռ�ֽ��� 
	DWORD dwPaletteSize=0, //�����ɫ���С�� λͼ�������ֽڴ�С ��λͼ�ļ���С �� д���ļ��ֽ��� 
	dwBmBitsSize, 
	dwDIBSize, dwWritten; 
	BITMAP Bitmap; //λͼ���Խṹ 
	BITMAPFILEHEADER bmfHdr; //λͼ�ļ�ͷ�ṹ 
	BITMAPINFOHEADER bi; //λͼ��Ϣͷ�ṹ 
	LPBITMAPINFOHEADER lpbi; //ָ��λͼ��Ϣͷ�ṹ 

	HANDLE fh, hDib, hPal,hOldPal=NULL; //�����ļ��������ڴ�������ɫ���� 

	//����λͼ�ļ�ÿ��������ռ�ֽ��� 
	HDC hWndDC = CreateDC("DISPLAY", NULL, NULL, NULL); 
	hDC = ::CreateCompatibleDC( hWndDC ) ; 
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
	DeleteDC(hDC); 

	if (iBits <= 1) 
		wBitCount = 1; 
	else if (iBits <= 4) 
		wBitCount = 4; 
	else if (iBits <= 8) 
		wBitCount = 8; 
	else if (iBits <= 24) 
		wBitCount = 24; 
	else 
		wBitCount = 24 ; 

	//�����ɫ���С 
	if (wBitCount <= 8) 
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD); 

	//����λͼ��Ϣͷ�ṹ 
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap); 
	bi.biSize = sizeof(BITMAPINFOHEADER); 
	bi.biWidth = Bitmap.bmWidth; 
	bi.biHeight = Bitmap.bmHeight; 
	bi.biPlanes = 1; 
	bi.biBitCount = wBitCount; 
	bi.biCompression = BI_RGB; 
	bi.biSizeImage = 0; 
	bi.biXPelsPerMeter = 0; 
	bi.biYPelsPerMeter = 0; 
	bi.biClrUsed = 0; 
	bi.biClrImportant = 0; 

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32) * 4 * Bitmap.bmHeight ; 

	//Ϊλͼ���ݷ����ڴ�
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	// �����ɫ��
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	// ��ȡ�õ�ɫ�����µ�����ֵ 
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(LPBITMAPINFO )lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize; 
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 

	// д��λͼ�ļ�ͷ 
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 

	// д��λͼ�ļ��������� 
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 

	//��� 
	GlobalUnlock(hDib); 
	GlobalFree(hDib); 
	CloseHandle(fh); 

	return TRUE; 
}


//��ʾ������ʾ��Ϣ
void CScreenshotDlg::DrawTip()
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
    //�õ�ǰ��������,
	CPoint pt;
	GetCursorPos(&pt);
	
	//������ǰR,G,B,������ֵ
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//����ʽ�ŷ��ַ���
	CString string;
	CString strTemp;
	string.Format("\r\n\r\n\r\n ����ǰ����RGB (%d,%d,%d)\r\n", rValue, gValue, bValue);
    
	if(!m_bDraw && !m_bFirstDraw)
	{
		strTemp="\r\n ����������������ѡ���ȡ\r\n ��Χ\r\n\r\n ����ESC��������Ҽ��˳�";
	}
	else
	if(m_bDraw && m_bFirstDraw)
	{
		strTemp="\r\n ���ɿ�������ȷ����ȡ��Χ\r\n\r\n ����ESC���˳�";
	}
	else
	if(m_bFirstDraw)
	{
		strTemp="\r\n ����������������ȡ��Χ��\r\n ��С��λ��\r\n\r\n ����ȡ��Χ��˫����������\r\n ��ͼ�񣬽�������\r\n\r\n ���������Ҽ�����ѡ��";
	}
	string+=strTemp;
	//GtWriteTrace(30, "%s:%d  [%s]", __FUNCTION__, __LINE__, string.GetBuffer());
	//��ʾ���༩����,������ʾ����
	m_tipEdit.SetWindowText(string);
}

//��ʾ��ȡ������Ϣ
void CScreenshotDlg::DrawMessage(CRect &inRect,CDC * pDC)
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//��ȡ���δ�С��Ϣ�������
	const int space=3;
    
	//����������ɫ��С
	
	CPoint pt;
	CPen pen(PS_SOLID,1,RGB(147,147,147));

	//dc.SetTextColor(RGB(147,147,147));
	CFont font;
	CFont * pOldFont;
	font.CreatePointFont(90,"����");
	pOldFont=pDC->SelectObject(&font);

	//�õ������Ⱥ͸߶�
	GetCursorPos(&pt);
	int OldBkMode;
	OldBkMode=pDC->SetBkMode(TRANSPARENT);

	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	int	lineLength;
	pDC->GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;
	size=pDC->GetTextExtent("����λ��  ",strlen("����λ��  "));
	lineLength=size.cx;
    
	//��ʼ������, �Ա�֤д����������
	CRect rect(pt.x+space,pt.y-charHeight*6-space,pt.x+lineLength+space,pt.y-space);
    
    //������ʱ����
    CRect rectTemp;
	//�����ε��������Եʱ��������ʹ�С
	if((pt.x+rect.Width())>=m_xScreen)
	{
		//�����Ϸ���ʾ���¾���
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((pt.y-rect.Height())<=0)
	{
		//�����ҷ���ʾ���¾���
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
	}

	//�����ջ�ˢ������
	CBrush * pOldBrush;
    pOldBrush=pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	
	pDC->Rectangle(rect);
   	rect.top+=2;
	//�ھ�������ʾ����
	CRect outRect(rect.left,rect.top,rect.left+lineLength,rect.top+charHeight);
	CString string("����λ��");
	pDC->DrawText(string,outRect,DT_CENTER);
    
	outRect.SetRect(rect.left,rect.top+charHeight,rect.left+lineLength,charHeight+rect.top+charHeight);
	string.Format("(%d,%d)",inRect.left,inRect.top);
	pDC->DrawText(string,outRect,DT_CENTER);
	

	outRect.SetRect(rect.left,rect.top+charHeight*2,rect.left+lineLength,charHeight+rect.top+charHeight*2);
	string="���δ�С";
	pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*3,rect.left+lineLength,charHeight+rect.top+charHeight*3);
	string.Format("(%d,%d)",inRect.Width(),inRect.Height());
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*4,rect.left+lineLength,charHeight+rect.top+charHeight*4);
	string="�������";
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*5,rect.left+lineLength,charHeight+rect.top+charHeight*5);
	string.Format("(%d,%d)",pt.x,pt.y);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	pDC->SetBkMode(OldBkMode);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);
	
}
//�ػ�����
void CScreenshotDlg::PaintWindow()
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//��ȡ��ȫ���Ի��򴰿ڴ�С
	CRect rect1;
	GetWindowRect(rect1);

	//��ȡ�༭�򴰿ڴ�С
	CRect rect2;
	m_tipEdit.GetWindowRect(rect2);

	CRgn rgn1,rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rect2);

	//��ȡ��������,���ǳ��˱༭�򴰿ڲ�����
	m_rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	
	InvalidateRgn(&m_rgn);
	
}
//�ı������ʾ���ڵ�RGBֵ
void CScreenshotDlg::ChangeRGB()
{
	//GtWriteTrace(30, "%s:%d", __FUNCTION__, __LINE__);
	//����ɵ�RGBֵ�ַ���
	static CString strOld("");

	CPoint pt;
	GetCursorPos(&pt);

	//������ǰR,G,B,������ֵ
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//����ʽ�ŷ��ַ���
	CString string;
	string.Format(" (%d,%d,%d)",rValue,gValue,bValue);
	//�����ǰ��ɫû����ˢ��RGBֵ,���ⴰ���и�����˸
    if(strOld!=string)
	{
		//CString tmp2;
		//m_tipEdit.GetWindowTextA(tmp2);
		//GtWriteTrace(30,"[ScreenDlg]ChangeRGB() 111111111111%s", tmp2.GetBuffer());
		//�õ�RGB�ı���һ�е��ı�����
		int LineLength=m_tipEdit.LineLength(6);
		//��ѡRGBֵ�ı�,Ҳ����(255,255,255)��ʽ
	    m_tipEdit.SetSel(20, LineLength+6);
        
		//�滻RGB����
		m_tipEdit.ReplaceSel(string);

		//m_tipEdit.GetWindowTextA(tmp2);
		//GtWriteTrace(30,"[ScreenDlg]ChangeRGB() 22222222222%s", tmp2.GetBuffer());
	}
	
	strOld=string;

}