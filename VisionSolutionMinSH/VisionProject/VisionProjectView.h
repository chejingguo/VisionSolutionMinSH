
// VisionProjectView.h : CVisionProjectView 类的接口
//

#pragma once

#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TShowData.h"
#include "cvgdi.h"

#include "svPTGrab2.h"

class CVisionProjectView : public CFormView
{
protected: // 仅从序列化创建
	CVisionProjectView();
	DECLARE_DYNCREATE(CVisionProjectView)

public:
	enum{ IDD = IDD_VISIONPROJECT_FORM };

// 特性
public:
	CVisionProjectDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CVisionProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	void MatToCImage(Mat &mat, CImage &cImage);
public:
	void DrawImage(Mat img,UINT ID);
	afx_msg void OnBnClickedButton1();
	afx_msg void On32778();
	virtual void OnDraw(CDC* /*pDC*/);
	void UpdateWindowInfo();
	CEdit m_CEditShow;
	CEdit m_CEdit_State;
	CListCtrl m_listShow;

	Cvgdi m_gdiAssist;
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // VisionProjectView.cpp 中的调试版本
inline CVisionProjectDoc* CVisionProjectView::GetDocument() const
   { return reinterpret_cast<CVisionProjectDoc*>(m_pDocument); }
#endif

