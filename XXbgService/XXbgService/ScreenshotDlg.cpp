// ScreenshotDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XXbgService.h"
#include "ScreenshotDlg.h"
#include "afxdialogex.h"
#include "CBase64.h"
#include "Json/json.h"
#include "utils.h"

#include "MyTTrace.h"

#define BUFFER_LEN 1024
char sendBuff[BUFFER_LEN] = {0};

// CScreenshotDlg 对话框

IMPLEMENT_DYNAMIC(CScreenshotDlg, CDialogEx)

CScreenshotDlg::CScreenshotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScreenshotDlg::IDD, pParent)
{
	//*******************************************************************************
    //初始化像皮筋类,新增的resizeMiddle 类型
	m_rectTracker.m_nStyle=CMyTracker::resizeMiddle|CMyTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1,-2,-3,-4);
	//设置矩形颜色
	m_rectTracker.SetRectColor(RGB(10,100,130));
	//设置矩形调整时光标,默认的太小了,弄了个大点的
	m_rectTracker.SetResizeCursor(IDC_CURSOR6,IDC_CURSOR5,IDC_CURSOR2,IDC_CURSOR3,IDC_CURSOR4);

    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);  
    
	// 保存系统光标
	m_hOldCursor = AfxGetApp()-> LoadStandardCursor(IDC_ARROW);
    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);
	
	m_bDraw=FALSE;
	m_bFirstDraw=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
    m_startPt=0;
    
	//获取屏幕分辩率
	m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	m_yScreen = GetSystemMetrics(SM_CYSCREEN);

	//截取屏幕到位图中
	CRect rect(0, 0,m_xScreen,m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));
    
	//初始化刷新窗口区域 m_rgn
    m_rgn.CreateRectRgn(0,0,50,50);
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

	ON_BN_CLICKED(IDC_MY_OK_BUTTON, OnMyOkButton)
	ON_BN_CLICKED(IDC_MY_CANCLE_BUTTON, OnMyCancelButton)
END_MESSAGE_MAP()
// CScreenshotDlg 消息处理程序


BOOL CScreenshotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//**************************************************************************
	//把对化框设置成全屏顶层窗口
	SetWindowPos(&wndTopMost,0,0,m_xScreen,m_yScreen,SWP_SHOWWINDOW);

    
	// 确认、取消按钮初始化
	myOkButton = new CButton();
	myOkButton->Create( "确定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, BTN_WIDTH, BTN_HIGH), this, IDC_MY_OK_BUTTON );
	myOkButton->MoveWindow(0, 0, BTN_WIDTH, BTN_HIGH);
	myOkButton->ShowWindow(false);
	myCancelButton = new CButton();
	myCancelButton->Create( "取消", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, BTN_WIDTH, BTN_HIGH), this, IDC_MY_CANCLE_BUTTON );
	myCancelButton->MoveWindow(0, 0, BTN_WIDTH, BTN_HIGH);
	myCancelButton->ShowWindow(false);
	
	/*
	myEdit = new CEdit();
	myEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER, CRect(600, 100, BTN_WIDTH, BTN_HIGH), this, IDC_MY_EDIT);
	myEdit->MoveWindow(600, 300, EDIT_WIDTH, EDIT_HIGH);
	myEdit->ShowWindow(true);
	*/


	//移动操作提示窗口
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
    m_tipEdit.MoveWindow(10,10,rect.Width(),rect.Height());

	//显示操作提示窗口文字
	DrawTip();
	
	//捕获按键消息窗口,将对话框的句柄传递到CCatchScreenApp中
	((CXXbgServiceApp *)AfxGetApp())->m_hwndDlg=m_hWnd;
	//**************************************************************************
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CScreenshotDlg::OnPaint()
{
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
	//	dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
//**************************************************************************
		CPaintDC dc(this);
         
		//显示截取矩形大小信息
		if(m_bShowMsg&&m_bFirstDraw)
		{
			//得到当前矩形大小
			CRect rect;
			m_rectTracker.GetTrueRect(&rect);
			//传递CPaintDC 是为了不在了窗口上画信息
			DrawMessage(rect,&dc);
		}

		//画出像皮筋矩形
		if(m_bFirstDraw)
		{
			m_rectTracker.Draw(&dc);
		}

//*************************************************************************
		CDialog::OnPaint();
	}
}


// ----------------------add ycp-------------
void CScreenshotDlg::OnMyOkButton()
{
	//MessageBox( "点击了确定按钮" );
    //保存位图,bSave 为TRUE,
	CopyScreenToBitmap(m_rectTracker.m_rect, TRUE);
	CDialog::OnOK();
}

void CScreenshotDlg::OnMyCancelButton()
{
	//MessageBox( "点击了取消按钮" );
	CDialog::OnCancel();
}


// 显示 确定、取消 按钮，并移动它们位置到截图框边缘
void CScreenshotDlg::MoveShowOkCancelBtn()
{
	int x_tmp = 0;
	int y_tmp = 0;
	int x_abs = 0;
	int y_abs = 0;
	// 获取 截图框窗口大小 left左上角X坐标，top左上角Y坐标，right右下角X坐标，bottom右下角Y坐标
	CRect rect = m_rectTracker.m_rect;
	// 获取截图区绝对右下角的坐标
	x_abs = rect.left - rect.right > 0 ? rect.left : rect.right;
	y_abs = rect.bottom - rect.top > 0 ? rect.bottom : rect.top;
	// 判断 截图区绝对右下角是否靠近屏幕下边缘，是否足够按钮放置，计算按钮y坐标
	y_tmp = (y_abs + BTN_HIGH + 5 > m_yScreen) ? (y_abs - BTN_HIGH - 5) : (y_abs + 5);
	// 判断 截图区绝对右下角是否靠近屏幕左边缘，是否足够按钮放置，计算按钮x坐标
	x_tmp = (x_abs - 2 * (BTN_WIDTH + 5) < 0) ? 0 : (x_abs - 2 * (BTN_WIDTH + 5));
	// 移动按钮到截图框边缘位置
	myOkButton->MoveWindow(x_tmp + BTN_WIDTH + 5, y_tmp, BTN_WIDTH, BTN_HIGH);
	myCancelButton->MoveWindow(x_tmp, y_tmp, BTN_WIDTH, BTN_HIGH);
	// 显示按钮
	myOkButton->ShowWindow(true);
	myCancelButton->ShowWindow(true);
}

// 隐藏 确定、取消 按钮
void CScreenshotDlg::HideenOkCancelBtn()
{
	myOkButton->ShowWindow(false);
	myCancelButton->ShowWindow(false);
}
// ----------------------add ycp-------------

void CScreenshotDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnOK();
}


void CScreenshotDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//***************************************************************
	if(m_bFirstDraw)
	{
		// 获取到ESC按键弹起，隐藏 确认、取消按钮
		HideenOkCancelBtn();
		//取消已画矩形变量
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
//*******************************************************************
}


void CScreenshotDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30,"[ScreenDlg]OnMouseMove()");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//**************************************************************************************
	   if(m_bDraw)
	   {
			//动态调整矩形大小,并刷新画出
		    m_rectTracker.m_rect.SetRect(m_startPt.x+4,m_startPt.y+4,point.x,point.y);
			PaintWindow();
	   }
	   
	   //弥补调整大小和位置时,接收不到MouseMove消息
	   CRect rect;
	   m_tipEdit.GetWindowRect(&rect);
	   if(rect.PtInRect(point))
		   m_tipEdit.SendMessage(WM_MOUSEMOVE);
       
	   ChangeRGB();
	//*****************************************************************************************
	CDialogEx::OnMouseMove(nFlags, point);
}


void CScreenshotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30,"[ScreenDlg]OnLButtonDown()");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//*****************************************************************************************
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //判断击中位置
	if(nHitTest<0)
	{
		if(!m_bFirstDraw)
		{
			// 隐藏 确认、取消按钮
			HideenOkCancelBtn();

			//第一次画矩形
			m_startPt=point;
			m_bDraw=TRUE;
			m_bFirstDraw=TRUE;
			//设置当当鼠标按下时最小的矩形大小
			m_rectTracker.m_rect.SetRect(point.x,point.y,point.x+4,point.y+4);	
			
			//保证当鼠标当下时立刻显示信息
			if(m_bFirstDraw)
			  m_bShowMsg=TRUE;		
			DrawTip();
			PaintWindow();
		}
		else
		{
			// 已选中截图区，且点击选中区之外，不进行处理
		}
	}
	else
	{
		// 点击截图区内部，隐藏 确认、取消按钮
		HideenOkCancelBtn();

		//保证当鼠标当下时立刻显示信息
		m_bShowMsg=TRUE;		
		PaintWindow();
		
		if(m_bFirstDraw)
		{
			//调束大小时,Track会自动调整矩形大小,在些期间,消息归CRectTracker内部处理
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
	//GtWriteTrace(30,"[ScreenDlg]OnLButtonUp()");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//****************************************************************************************    
	m_bShowMsg=FALSE;
	m_bDraw=FALSE;
	DrawTip();
	PaintWindow();
//****************************************************************************************
	CDialogEx::OnLButtonUp(nFlags, point);

	// 左键弹起，已选中截图区，显示 确定、取消按钮
	MoveShowOkCancelBtn();
}


void CScreenshotDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30,"[ScreenDlg]OnLButtonDblClk()");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//****************************************************************************************
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);
	//如果在是矩形内部双击
	if(nHitTest==8)      
	{
		//保存位图到粘贴板中,bSave 为TRUE,
		CopyScreenToBitmap(m_rectTracker.m_rect,TRUE);
		CDialog::OnOK();
	}
//****************************************************************************************
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


BOOL CScreenshotDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//**************************************************************************************
	//用整个桌面填充全屏对话框背景
	
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//***********************************************************************
	//设置改变截取矩形大小时光标
	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
		             &&!m_bDraw &&m_bFirstDraw) //此处判断保截取时当标始中为彩色光标
    {
		return TRUE; 
	}

	//设置彩色光标
	SetCursor(m_hCursor);
	// SetCursor(m_hOldCursor);
	return TRUE;
//*******************************************************************
//	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
void CScreenshotDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	//GtWriteTrace(30,"[ScreenDlg]OnRButtonUp()");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//****************************************************************************************
	if(m_bFirstDraw)
	{
		//如果已经截取矩则清除截取矩形
		m_bFirstDraw=FALSE;
		//清除矩形大小
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		DrawTip();
		PaintWindow();
	}
	else
	{
		 // 对话框退出
		 CDialog::OnOK();
	}
	// 右键弹起，取消选中的截图区，隐藏确认、取消按钮
	HideenOkCancelBtn();
//****************************************************************************************
	CDialogEx::OnRButtonUp(nFlags, point);
}


HBRUSH CScreenshotDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
//***********************************************************
	//设置操作提示窗口文本颜色
	if(pWnd->GetDlgCtrlID()==IDC_EDIT1)
	{
		pDC->SetTextColor(RGB(255,255,255));
	}
//***************************************************************
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

//*********************增加的函数**********************************************************
//考贝屏幕,这段代码是拿来主义 呵呵
HBITMAP CScreenshotDlg::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)
//lpRect 代表选定区域
{
	HDC       hScrDC, hMemDC;      
	// 屏幕和内存设备描述表
	HBITMAP    hBitmap, hOldBitmap;   
	// 位图句柄
	int       nX, nY, nX2, nY2;      
	// 选定区域坐标
	int       nWidth, nHeight;
	
	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//确保选定区域是可见的
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
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap
		(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	if(bSave)
	{
		//创建军兼容DC,当bSave为中时把开始保存的全屏位图,按截取矩形大小保存
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
	//得到屏幕位图的句柄
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	
	if(bSave)
	{
				
		if (OpenClipboard()) 
		{
			//清空剪贴板
			//EmptyClipboard();
			//把屏幕内容粘贴到剪贴板上,
			//hBitmap 为刚才的屏幕位图句柄
			//SetClipboardData(CF_BITMAP, hBitmap);
			//关闭剪贴板
			//CloseClipboard();
			// 获取当前程序运行相对路径
			CString strDir = GetAppPath();
			// 判断截图文件存储目录是否存在，不存在则创建
			CString strSubDir = strDir + "\\CatchScreen";
			if (!PathIsDirectory(strSubDir))
			{
				//创建目录,已有的话不影响
				::CreateDirectory(strSubDir, NULL);
			}
			// 获取系统时间，
			SYSTEMTIME st;
			GetLocalTime(&st);
			// 组截图文件相对路径文件名
			CString strPath;
			strPath.Format("\\CatchScreen\\CatchScreen-%04d%02d%02d-%02d%02d%02d-%03d.png",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			// 拼截图文件绝对路径
			strDir.Append(strPath);
			// MessageBox(strDir);
			// 截屏内容存到文件中
			//SaveBitmapToFile(hBitmap, "C:\\Users\\Public\\Pictures\\test.png");
			if (SaveBitmapToFile(hBitmap, strDir))
			{
				// 保存图片成功
				// 存储上传的数据
				string msgStr_rtn;
				// 读取 文件内容
				FILE * pFile= NULL;
				char *fileBuffer = NULL;
				long lSize = 0;
				ZBase64 zBase;
				string encodeBase64;
				do
				{
					size_t result = 0;
					pFile = fopen (strDir.GetBuffer(strDir.GetLength()), "rb");
					strDir.ReleaseBuffer();
					if (pFile == NULL)
					{
						GtWriteTrace(30, "[ScreenDlg]Open picture [%s] failed!", strDir.GetBuffer(strDir.GetLength()));
						strDir.ReleaseBuffer();
						MessageBox("打开截图文件失败！请重试！");
						break;
					}
					fseek (pFile, 0, SEEK_END);
					lSize = ftell(pFile);
					rewind (pFile);
					// 判断文件是否大于10M
					if (lSize > 1024 * 1024 * 10)
					{
						GtWriteTrace(30, "[ScreenDlg]Picture size is greater than 10M, size=%lfM!", (((double)lSize) / 1024 / 1024));
						CString tmp;
						tmp.Format("截图文件大小≈%.2lfM(大于10M)，请重新截取！", (((double)lSize) / 1024 / 1024));
						MessageBox(tmp);
						break;
					}
					// 分配内存存储整个文件
					fileBuffer = (char*) malloc(sizeof(char) * lSize);
					if (fileBuffer == NULL)
					{
						GtWriteTrace(30, "[ScreenDlg]Malloc fileBuffer failed!");
						MessageBox("准备读取截图文件时分配内存失败！请重试！");
						break;
					}
					// 将文件拷贝到fileBuffer中
					result = fread(fileBuffer, 1, lSize, pFile);
					if (result != lSize)
					{
						GtWriteTrace(30, "[ScreenDlg]Read file failed!");
						MessageBox("读取截图文件失败！请重试！");
						break;
					}
					// 读取截图文件成功，进行base64编码
					encodeBase64 = zBase.Encode((const unsigned char*)fileBuffer, (int)lSize);
					// 释放内存
					free(fileBuffer);
					fileBuffer = NULL;

					// 组待发送的json报文
					Json::Value msgStr_json;//表示一个json格式的对象
					msgStr_json["type"] = "0";
					msgStr_json["num"] = "";
					msgStr_json["picSource"] = encodeBase64.c_str();
					// 转string
					msgStr_rtn = msgStr_json.toStyledString();
					// 发送到web端
					GtWriteTrace(30, "[ScreenDlg]Send picture message, sendBuffer=[%s], size=[%d]", 
						msgStr_rtn.c_str(), (int)(msgStr_rtn.length()));
					if (!SendHttp(1, msgStr_rtn.c_str(), (int)(msgStr_rtn.length())))
					{
						MessageBox("发送截图消息失败！请重试！");
						break;
					}
				}while(0);
				// 清理内存，关闭文件
				if (fileBuffer != NULL)
				{
					free(fileBuffer);
					fileBuffer = NULL;
				}
				if (pFile != NULL)
				{
					fclose (pFile);
					pFile = NULL;
				}
			}
			else
			{
				MessageBox("截图保存失败！请重试！");
			}
      }
	}
	// 返回位图句柄
	return hBitmap;
}


//---------------------------------------------------------------
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)userp);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}
	GtWriteTrace(30, "[ScreenDlg]Receive buffer UTF-8 = [%s], size = [%d]", buffer, size * nmemb);
	// printf("buffer%s\n",buffer);
	//sprintf((char *)buffer, "{\"code\":\"0\", \"msg\":\"成功\", \"url\":\"http://www.baidu.com\"}");//测试字符串
	//sprintf((char *)buffer, "{\"code\":\"1\", \"msg\":\"失败\", \"url\":\"\"}");//测试字符串
	//sprintf((char *)buffer, "{\"code\":\"2\", \"msg\":\"失败\", \"url\":\"\"}");//测试字符串
	//sprintf((char *)buffer, "{\"TYPE\":\"2\", \"RET_CODE\":\"0\"}");//测试字符串
	//GtWriteTrace(30, "[ScreenDlg]Receive buffer = [%s], size = [%d]", buffer, strlen((char *)buffer));
	// utf-8转gbk
	CString utf8String = (char *)buffer;
	ConvertUtf8ToGBK(utf8String);
	GtWriteTrace(30, "[ScreenDlg]Receive buffer GBK = [%s], size = [%d]", utf8String.GetBuffer(), utf8String.GetLength());
	utf8String.ReleaseBuffer();
	string recvBuff = (char *)buffer;
	// 解析服务端返回的json类型数据，获取交易类型
	//json解析
	Json::Reader reader;
	//表示一个json格式的对象
	Json::Value value;
	// 获取返回信息
	string code;
	string msg;
	string url;
	//解析json报文，存到value中
	if(reader.parse(recvBuff, value))
	{
		// 获取返回码
		code = value["code"].asString();
		if (code != "")
		{
			// 获取返回信息
			msg = value["msg"].asString();
			// 获取url或者失败信息
			url = value["url"].asString();
			//if (code == "0")
			if (code.compare("0") == 0)
			{
				// 解析成功，截取第一个"|"前的网址
				int ret;
				char strUrl[512] = {0};
				ret = splitString(strUrl, url.c_str(), 0);
				if (ret != 0)
				{
					// 获取url地址失败
					GtWriteTrace(30, "[ScreenDlg]splitString() failed! buff=[%s], ret=[%d]!", url.c_str(), ret);
				}
				else 
				{
					GtWriteTrace(30, "[ScreenDlg]splitString() succeed! url=[%s]!", strUrl);
					memset(sendBuff, 0, sizeof(sendBuff));
					// 赋值网址到全局变量中
					memcpy(sendBuff, strUrl, strlen(strUrl));
					//::MessageBoxA(0, sendBuff, "测试3", NULL);
					// 显示网页(发消息给指定窗口)
					::PostMessageA(((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetSafeHwnd(),
						WM_HTML_SHOW, (WPARAM)sendBuff, NULL);
				}
			}
			else if(code.compare("1") == 0)
			{
				// 二维码图片识别失败
				GtWriteTrace(30, "[ScreenDlg]Ret=[%s], msg=[%s], Server recognizing images failed! Will send message to MainFrm!", code.c_str(), msg.c_str());
				// 发消息到主窗口处理
				::PostMessageA(((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetSafeHwnd(),
					WM_SCREENDLG_MSG, (WPARAM)RECOGNIZE_PICTURE_FAILED, NULL);
			}else if(code.compare("2") == 0)
			{
				// 二维码编号不存在
				GtWriteTrace(30, "[ScreenDlg]Ret=[%s], msg=[%s], QR code number is not exist! Will send message to MainFrm!", code.c_str(), msg.c_str());
				// 发消息到主窗口处理
				::PostMessageA(((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetSafeHwnd(),
					WM_SCREENDLG_MSG, (WPARAM)QR_CODE_NOT_EXIST, NULL);
			}
			else
			{
				// 其他交易类型
				GtWriteTrace(30, "[ScreenDlg]Recvive illegal code[%d]!", code.c_str());
				//::MessageBoxA(NULL, "web端返回未定义交易类型！", "提示", MB_OK);
			}
			if (1 == 1)
			{

			}
		}

		string type = value["TYPE"].asString();
		string ret_code = value["RET_CODE"].asString();
		if (type != "")
		{
			CString msg;
			msg.Format("web端返回：%s", ret_code.c_str());
			::MessageBoxA(NULL, msg, "提示", MB_OK);
		}
	}
	else
	{
		// json解析失败
		GtWriteTrace(30, "[ScreenDlg]Read json message failed!");
		//::MessageBoxA(NULL, "解析json报文失败！", "提示", MB_OK);
	}

	return nmemb;
}

// flag 1-截图上传，2-编号上传
BOOL SendHttp(int flag, const char *str, int size)
{
	BOOL ret = true;

	CString keyStr = (flag == 1 ? "POST_URL_PIC" : "POST_URL_NO");
	// 读取配置文件的url地址
	CString urlString;
	char urlChar[512] = {0};
	GetPrivateProfileString("Information", keyStr.GetBuffer(keyStr.GetLength()), "http://192.168.1.53:8181/happyApp//a/sys/office/picGetUrl", 
		urlString.GetBuffer(512), 512, GetAppPath()+"\\win.ini");
	keyStr.ReleaseBuffer();
	urlString.ReleaseBuffer();
	memcpy(urlChar, (const char*)urlString.GetBuffer(urlString.GetLength()), urlString.GetLength());
	urlString.ReleaseBuffer();

	// url 打印到日志中
	GtWriteTrace(30, "[ScreenDlg]url = [%s]", urlChar);

#if 0
	// get 方式发送
	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	// 超时时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_URL, urlChar);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size);

	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{ 
		GtWriteTrace(30,"[ScreenDlg]curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		ret = false;
	}
	curl_easy_cleanup(curl);
	curl_global_cleanup();
#endif
#if 1
	// post方式发送
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *conn = curl_easy_init();
	curl_easy_setopt(conn, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(conn, CURLOPT_URL, urlChar);
	curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &write_data);
	// 设置json发送方式
	curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");

	curl_easy_setopt(conn, CURLOPT_HTTPHEADER, plist);
	curl_easy_setopt(conn, CURLOPT_POSTFIELDS, str);
	curl_easy_setopt(conn, CURLOPT_POSTFIELDSIZE, size);
	curl_easy_setopt(conn, CURLOPT_POST, true);
	CURLcode code = curl_easy_perform(conn);
	if(code != CURLE_OK)
	{ 
		GtWriteTrace(30,"[ScreenDlg]curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
		ret = false;
	}
	curl_easy_cleanup(conn);
#endif

#if 0
	// post方式发送
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *conn = curl_easy_init();
	curl_easy_setopt(conn, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(conn, CURLOPT_VERBOSE, 1L);
	curl_slist *http_headers = NULL;
	// 设置json发送方式
	http_headers = curl_slist_append(http_headers, "Accept: application/json");
	http_headers = curl_slist_append(http_headers, "Content-Type: application/json");
	http_headers = curl_slist_append(http_headers, "charsets: utf-8");
	curl_easy_setopt(conn, CURLOPT_CUSTOMREQUEST, "POST");
	CURLcode code = curl_easy_setopt(conn, CURLOPT_HTTPHEADER, http_headers);

	code = curl_easy_setopt(conn, CURLOPT_URL, urlChar);
	if (code != CURLE_OK)
	{
		GtWriteTrace(EM_TraceDebug, "Failed to set URL [%d]\n", code);
		return -3 ;
	}
	code =curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &write_data);
	//code =curl_easy_setopt(m_pCurlHandlder, CURLOPT_WRITEFUNCTION, CHTTPParse::OnWriteData);
	string strResp;
	//code =curl_easy_setopt(conn, CURLOPT_WRITEDATA, (void *)&strResp); 
	curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(conn,CURLOPT_FOLLOWLOCATION, 1);

	code = curl_easy_perform(conn);
	if(code != CURLE_OK)
	{ 
		GtWriteTrace(30,"[ScreenDlg]curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
		ret = false;
	}
	curl_easy_cleanup(conn);
#endif 
	return ret;
}


// http://blog.csdn.net/oldmtn/article/details/7480166
BOOL CScreenshotDlg::SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName )
{ 
	HDC hDC; //设备描述表 
	int iBits; //当前显示分辨率下每个像素所占字节数 
	WORD wBitCount; //位图中每个像素所占字节数 
	DWORD dwPaletteSize=0, //定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数 
	dwBmBitsSize, 
	dwDIBSize, dwWritten; 
	BITMAP Bitmap; //位图属性结构 
	BITMAPFILEHEADER bmfHdr; //位图文件头结构 
	BITMAPINFOHEADER bi; //位图信息头结构 
	LPBITMAPINFOHEADER lpbi; //指向位图信息头结构 

	HANDLE fh, hDib, hPal,hOldPal=NULL; //定义文件，分配内存句柄，调色板句柄 

	//计算位图文件每个像素所占字节数 
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

	//计算调色板大小 
	if (wBitCount <= 8) 
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD); 

	//设置位图信息头结构 
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

	//为位图内容分配内存
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	// 处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值 
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(LPBITMAPINFO )lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize; 
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 

	// 写入位图文件头 
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 

	// 写入位图文件其余内容 
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 

	//清除 
	GlobalUnlock(hDib); 
	GlobalFree(hDib); 
	CloseHandle(fh); 

	return TRUE; 
}


//显示操作提示信息
void CScreenshotDlg::DrawTip()
{
	//GtWriteTrace(30,"[ScreenDlg]DrawTip()");
    //得当前坐标像素,
	CPoint pt;
	GetCursorPos(&pt);
	
	//当到当前R,G,B,各像素值
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//按格式排放字符串
	CString string;
	CString strTemp;
	string.Format("\r\n\r\n\r\n ·当前像素RGB (%d,%d,%d)\r\n",rValue,gValue,bValue);
    
	if(!m_bDraw&&!m_bFirstDraw)
	{
		strTemp="\r\n ·按下鼠标左键不放选择截取\r\n 范围\r\n\r\n ·按ESC键或鼠标右键退出";
	}
	else
	if(m_bDraw&&m_bFirstDraw)
	{
		strTemp="\r\n ·松开鼠标左键确定截取范围\r\n\r\n ·按ESC键退出";
	}
	else
	if(m_bFirstDraw)
	{
		strTemp="\r\n ·用鼠标左键调整截取范围的\r\n 大小和位置\r\n\r\n ·截取范围内双击鼠标左键保\r\n 存图像，结束操作\r\n\r\n ·点击鼠标右键重新选择";
	}
	string+=strTemp;
	//显示到编缉框中,操作提示窗口
	m_tipEdit.SetWindowText(string);
}

//显示截取矩形信息
void CScreenshotDlg::DrawMessage(CRect &inRect,CDC * pDC)
{
	//截取矩形大小信息离鼠标间隔
	const int space=3;
    
	//设置字体颜色大小
	
	CPoint pt;
	CPen pen(PS_SOLID,1,RGB(147,147,147));

	//dc.SetTextColor(RGB(147,147,147));
	CFont font;
	CFont * pOldFont;
	font.CreatePointFont(90,"宋体");
	pOldFont=pDC->SelectObject(&font);

	//得到字体宽度和高度
	GetCursorPos(&pt);
	int OldBkMode;
	OldBkMode=pDC->SetBkMode(TRANSPARENT);

	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	int	lineLength;
	pDC->GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;
	size=pDC->GetTextExtent("顶点位置  ",strlen("顶点位置  "));
	lineLength=size.cx;
    
	//初始化矩形, 以保证写下六行文字
	CRect rect(pt.x+space,pt.y-charHeight*6-space,pt.x+lineLength+space,pt.y-space);
    
    //创建临时矩形
    CRect rectTemp;
	//当矩形到达桌面边缘时调整方向和大小
	if((pt.x+rect.Width())>=m_xScreen)
	{
		//桌面上方显示不下矩形
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((pt.y-rect.Height())<=0)
	{
		//桌面右方显示不下矩形
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
		
	}

	//创建空画刷画矩形
	CBrush * pOldBrush;
    pOldBrush=pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	
	pDC->Rectangle(rect);
   	rect.top+=2;
	//在矩形中显示文字
	CRect outRect(rect.left,rect.top,rect.left+lineLength,rect.top+charHeight);
	CString string("顶点位置");
	pDC->DrawText(string,outRect,DT_CENTER);
    
	outRect.SetRect(rect.left,rect.top+charHeight,rect.left+lineLength,charHeight+rect.top+charHeight);
	string.Format("(%d,%d)",inRect.left,inRect.top);
	pDC->DrawText(string,outRect,DT_CENTER);
	

	outRect.SetRect(rect.left,rect.top+charHeight*2,rect.left+lineLength,charHeight+rect.top+charHeight*2);
	string="矩形大小";
	pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*3,rect.left+lineLength,charHeight+rect.top+charHeight*3);
	string.Format("(%d,%d)",inRect.Width(),inRect.Height());
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*4,rect.left+lineLength,charHeight+rect.top+charHeight*4);
	string="光标坐标";
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*5,rect.left+lineLength,charHeight+rect.top+charHeight*5);
	string.Format("(%d,%d)",pt.x,pt.y);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	pDC->SetBkMode(OldBkMode);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);
	
}
//重画窗口
void CScreenshotDlg::PaintWindow()
{
	//获取当全屏对话框窗口大小
	CRect rect1;
	GetWindowRect(rect1);

	//获取编辑框窗口大小
	CRect rect2;
	m_tipEdit.GetWindowRect(rect2);

	CRgn rgn1,rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rect2);

	//获取更新区域,就是除了编辑框窗口不更新
	m_rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	
	InvalidateRgn(&m_rgn);
	
}
//改变操作提示窗口当RGB值
void CScreenshotDlg::ChangeRGB()
{
	//保存旧的RGB值字符串
	static CString strOld("");

	CPoint pt;
	GetCursorPos(&pt);

	//当到当前R,G,B,各像素值
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//按格式排放字符串
	CString string;
	string.Format("(%d,%d,%d)",rValue,gValue,bValue);
	//如果当前颜色没变则不刷新RGB值,以免窗口有更多闪烁
    if(strOld!=string)
	{
		//CString tmp2;
		//m_tipEdit.GetWindowTextA(tmp2);
		//GtWriteTrace(30,"[ScreenDlg]ChangeRGB() 111111111111%s", tmp2.GetBuffer());
		//得到RGB文本那一行的文本长度
		int LineLength=m_tipEdit.LineLength(6);
		//复选RGB值文本,也就是(255,255,255)形式
	    m_tipEdit.SetSel(20, LineLength+6);
        
		//替换RGB内容
		m_tipEdit.ReplaceSel(string);

		//m_tipEdit.GetWindowTextA(tmp2);
		//GtWriteTrace(30,"[ScreenDlg]ChangeRGB() 22222222222%s", tmp2.GetBuffer());
	}
	
	strOld=string;

}

//*******************************************************************************************

