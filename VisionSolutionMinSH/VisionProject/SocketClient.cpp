// SocketClient.cpp : implementation file
//

#include "stdafx.h"
#include "SocketClient.h"

// CSocketClient

CSocketClient::CSocketClient()
{
	AfxSocketInit();
}

CSocketClient::~CSocketClient()
{
}


void CSocketClient::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	TCHAR s[2048];CString szIP;UINT szPort;

	GetPeerName(szIP,szPort);
	int nLen = Receive(s,2048);
	s[nLen]=_T('\0');
	CString strText(s);
	//
	SetInfoReceive(strText);
	SetServerIP(szIP);
	SetServerPort(szPort);
	//
	CSocket::OnReceive(nErrorCode);
}
