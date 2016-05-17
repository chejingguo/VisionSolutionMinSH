#pragma once


// CDetObjectPar 对话框

class CDetObjectPar : public CDialog
{
	DECLARE_DYNAMIC(CDetObjectPar)

public:
	CDetObjectPar(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDetObjectPar();

// 对话框数据
	enum { IDD = IDD_DLG_DET_PAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
