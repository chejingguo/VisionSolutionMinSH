// SocketServer.cpp : implementation file
//

#include "stdafx.h"

#include "SocketServer.h"

// CSocketServer

CSocketServer::CSocketServer()
{
	AfxSocketInit();
}
CSocketServer::~CSocketServer()
{
}
vector<ClientIPAddress> CSocketServer::clientInfo;
CList<CSocketServer*, CSocketServer*> CSocketServer::m_list;
CString CSocketServer::infoReceive=NULL;
CString CSocketServer::clientIP=NULL;
UINT    CSocketServer::clientPort=0;

string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}
// CSocketServer member functions
void CSocketServer::OnAccept(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CSocketServer *p = new CSocketServer;
	if(!Accept(*p))
	{
		delete p;
		return;
	}
	//
	CString szIP;
	UINT szPort;
	p->GetPeerName(szIP,szPort);
	//connect to robot
	TShowData::iPort = szPort;
	TShowData::sConnectIP = WChar2Ansi(szIP.GetBuffer(szIP.GetLength()));
	/*
	tempClientIPAddress.csIP = szIP;
	tempClientIPAddress.iPort = szPort;

	clientInfo.pop_back();
	*/
	TShowData::bConnectFlag = true;
	//

	if (szIP==L"172.168.0.20")
	{
		m_list.AddTail(p);
	}
	else
	{
	}
	
	CSocket::OnAccept(nErrorCode);
}


void CSocketServer::SendInfo2Client(LPCTSTR lpBuffer,int nBuffer,int nFlags)
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = m_list.GetHeadPosition();
	//CString str=_T("hello");
	while(pos)
	{
		CSocketServer* pSock = m_list.GetNext(pos);
		pSock->Send(lpBuffer,nBuffer,nFlags);
	}

}
void CSocketServer::SendInfo2Client(CString info)
{
	// TODO: Add your specialized code here and/or call the base class
	int nBuffer = info.GetLength();

	//
	int nBytes = WideCharToMultiByte(CP_ACP, 0, info, nBuffer, NULL, 0, NULL, NULL);
	char* pInfo = new char[nBytes + 1];
	memset(pInfo, 0, nBuffer + 1);
	WideCharToMultiByte(CP_OEMCP, 0, info, nBuffer, pInfo, nBytes, NULL, NULL);
	pInfo[nBytes] = 0;
	//
	//CString szIP;
	//UINT szPort;
	int nFlags = 0;
	POSITION pos = m_list.GetHeadPosition();
	while (pos)
	{
		CSocketServer* pSock = m_list.GetNext(pos);

		//pSock->GetPeerName(szIP, szPort);

		pSock->Send(pInfo, nBuffer, nFlags);
	}

	delete pInfo;
}

void CSocketServer::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	char s[1024]; CString szIP; UINT szPort;

	GetPeerName(szIP,szPort);
	int nLen = Receive(s,1024);
	s[nLen]=L'\0';
	CString strText(s);

	SetInfoReceive(strText);
	SetClientIP(szIP);
	SetClientPort(szPort);
	if (strText.GetLength()>3 && (strText.Left(3) == L"con"))
	{
		CStaticClass::m_bControl = true;
		CStaticClass::m_csReceiveInfo = strText;
		SetEvent(CStaticClass::m_hEventRecieve);
	}


	CSocket::OnReceive(nErrorCode);
}


void CSocketServer::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = m_list.GetHeadPosition();
	while(pos)
	{
		if(m_list.GetAt(pos)==this)
		{
			m_list.RemoveAt(pos);
			break;
		}
		m_list.GetNext(pos);
	}
	delete this;

	
	//
	CSocket::OnClose(nErrorCode);
}
