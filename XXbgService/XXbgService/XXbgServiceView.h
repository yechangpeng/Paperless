
// XXbgServiceView.h : CXXbgServiceView 类的接口
//

#pragma once


class CXXbgServiceView : public CHtmlView
{
protected: // 仅从序列化创建
	CXXbgServiceView();
	DECLARE_DYNCREATE(CXXbgServiceView)

// 特性
public:
	CXXbgServiceDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CXXbgServiceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM iParam);
	DECLARE_MESSAGE_MAP()

};

#ifndef _DEBUG  // XXbgServiceView.cpp 中的调试版本
inline CXXbgServiceDoc* CXXbgServiceView::GetDocument() const
   { return reinterpret_cast<CXXbgServiceDoc*>(m_pDocument); }
#endif

