#pragma once

#include"NetWork.h"
// CComPart �Ի���

class CComPart : public CDialogEx
{
	DECLARE_DYNAMIC(CComPart)

public:
	CComPart(CWnd* pParent, TNetWork* pNet);   // ��׼���캯��
	virtual ~CComPart();

// �Ի�������
	enum { IDD = IDD_DLG_COM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	TNetWork* pTNetWork;
public:
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnRecieve();
};
