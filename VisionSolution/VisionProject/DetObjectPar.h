#pragma once


// CDetObjectPar �Ի���

class CDetObjectPar : public CDialog
{
	DECLARE_DYNAMIC(CDetObjectPar)

public:
	CDetObjectPar(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDetObjectPar();

// �Ի�������
	enum { IDD = IDD_DLG_DET_PAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
