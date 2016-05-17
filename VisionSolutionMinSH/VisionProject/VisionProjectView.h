
// VisionProjectView.h : CVisionProjectView ��Ľӿ�
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
protected: // �������л�����
	CVisionProjectView();
	DECLARE_DYNCREATE(CVisionProjectView)

public:
	enum{ IDD = IDD_VISIONPROJECT_FORM };

// ����
public:
	CVisionProjectDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CVisionProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // VisionProjectView.cpp �еĵ��԰汾
inline CVisionProjectDoc* CVisionProjectView::GetDocument() const
   { return reinterpret_cast<CVisionProjectDoc*>(m_pDocument); }
#endif

