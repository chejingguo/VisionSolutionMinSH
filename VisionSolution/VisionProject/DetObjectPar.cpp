// DetObjectPar.cpp : 实现文件
//

#include "stdafx.h"
#include "VisionProject.h"
#include "DetObjectPar.h"
#include "afxdialogex.h"


// CDetObjectPar 对话框

IMPLEMENT_DYNAMIC(CDetObjectPar, CDialog)

CDetObjectPar::CDetObjectPar(CWnd* pParent /*=NULL*/)
	: CDialog(CDetObjectPar::IDD, pParent)
{

}

CDetObjectPar::~CDetObjectPar()
{
}

void CDetObjectPar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDetObjectPar, CDialog)
END_MESSAGE_MAP()


// CDetObjectPar 消息处理程序
