// ComPart.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VisionProject.h"
#include "ComPart.h"
#include "afxdialogex.h"


// CComPart �Ի���

IMPLEMENT_DYNAMIC(CComPart, CDialogEx)

CComPart::CComPart(CWnd* pParent, TNetWork* pNet)
	: CDialogEx(CComPart::IDD, pParent)
{
	pTNetWork = pNet;


}

CComPart::~CComPart()
{
}

void CComPart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CComPart, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CComPart::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CComPart::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SEND, &CComPart::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_RECIEVE, &CComPart::OnBnClickedBtnRecieve)
END_MESSAGE_MAP()


// CComPart ��Ϣ�������


void CComPart::OnBnClickedBtnSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString strPort;
	GetDlgItemText(IDC_EDIT_SERVER_PORT, strPort);
	WritePrivateProfileStringW(L"IP", L"Port", strPort, L"d:\\par.ini");
	MessageBox(L"�޸ĳɹ���");
}


void CComPart::OnBnClickedBtnLoad()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strPort = NULL;
	GetPrivateProfileStringW(L"IP", L"Port", L"0", strPort.GetBuffer(MAX_PATH), MAX_PATH, L"d:\\par.ini");
	SetDlgItemText(IDC_EDIT_SERVER_PORT,strPort);
	
}


void CComPart::OnBnClickedBtnSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString csSend;
	GetDlgItemText(IDC_EDIT_SEND, csSend);

	pTNetWork->Send(csSend);

}


void CComPart::OnBnClickedBtnRecieve()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString csRecieve=pTNetWork->Recieve();
	SetDlgItemText(IDC_EDIT_RECIVE, csRecieve);
}
