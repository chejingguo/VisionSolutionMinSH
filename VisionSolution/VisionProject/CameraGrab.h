#pragma once
#include"svPTGrab2.h"

// CCameraGrab 对话框

class CCameraGrab : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraGrab)

public:
	CCameraGrab(CWnd* pParent,scPTGrab2* scPTGrab2);   // 标准构造函数
	virtual ~CCameraGrab();

public:
	scPTGrab2* pDocScPTGrab2;
	bool m_bIsGrab;   //判断是否在连续采集
	bool m_bSoftTraggerOn;

// 对话框数据
	enum { IDD = IDD_DLG_GRAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGrab();
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnSoftTriagger();
	afx_msg void OnBnClickedBtnHardTriagger();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
};
