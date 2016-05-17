#pragma once

#include"NetWork.h"
// CComPart 对话框

class CComPart : public CDialogEx
{
	DECLARE_DYNAMIC(CComPart)

public:
	CComPart(CWnd* pParent, TNetWork* pNet);   // 标准构造函数
	virtual ~CComPart();

// 对话框数据
	enum { IDD = IDD_DLG_COM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	TNetWork* pTNetWork;
public:
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnRecieve();
};
