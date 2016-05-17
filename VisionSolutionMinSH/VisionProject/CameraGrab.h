#pragma once
#include"svPTGrab2.h"

// CCameraGrab �Ի���

class CCameraGrab : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraGrab)

public:
	CCameraGrab(CWnd* pParent,scPTGrab2* scPTGrab2);   // ��׼���캯��
	virtual ~CCameraGrab();

public:
	scPTGrab2* pDocScPTGrab2;
	bool m_bIsGrab;   //�ж��Ƿ��������ɼ�
	bool m_bSoftTraggerOn;

// �Ի�������
	enum { IDD = IDD_DLG_GRAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGrab();
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnSoftTriagger();
	afx_msg void OnBnClickedBtnHardTriagger();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
};
