
// MainFrm.h : CMainFrame 类的接口
//
#include "ScreenshotDlg.h"
#include "ManualInputDlg.h"
#include "MyTTrace.h"
#include "SettingDlg.h"
#include "CBaseReadIDCard.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	virtual void OnSysCommand( UINT nID, LPARAM lParam);
#endif

protected:  // 控件条嵌入成员
	CStatusBar        m_wndStatusBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);  //添加托盘消息响应函数的声明
	afx_msg LRESULT OnScreenshot(WPARAM wParam, LPARAM lParam);  //截屏消息
	afx_msg LRESULT OnIocpRead(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuit();
	afx_msg void StartKeyBoardHook();
	afx_msg void OnManualInput();
	afx_msg void ShowManualInput(char *tip);
	afx_msg void OnSettingWin();
	afx_msg void OnMyClose();
	afx_msg void OnGaoPaiYiCeShi();
	afx_msg LRESULT OnScreenDlgMessage(WPARAM wParam, LPARAM iParam);
	afx_msg LRESULT OnContinueInput(WPARAM wParam, LPARAM iParam);
	DECLARE_MESSAGE_MAP()
public:
	NOTIFYICONDATA m_nid; //定义托盘的相关结构体
	virtual void ActivateFrame(int nCmdShow = -1);
	BOOL SaveFrmPosToFile();
	void InitDevice();
public:
	CMenu m_menu;
	BOOL m_hookFlag;
	CScreenshotDlg *screenshotDlg;
	CManualInput *input;
	CSetting *settingDlg;
	CBaseReadIDCardInfo *pBaseReadIDCardInfo;
	CBaseSaveDeskPic *pBaseSaveDeskPic;
	CBaseSaveEnvPic *pBaseSaveEnvPic;
};


