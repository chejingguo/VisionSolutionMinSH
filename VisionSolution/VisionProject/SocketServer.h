#pragma once
// CSocketServer command target
//#include "VisionProjectView.h"
#include "afxsock.h"
#include "TShowData.h"

#include "StaticClass.h"

struct ClientIPAddress
{
	UINT   iPort;
	CString csIP;
};


class CSocketServer : public CSocket
{
public:
	CSocketServer();
	virtual ~CSocketServer();
public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
public:
	void SendInfo2Client(CString info);
	void SendInfo2Client(LPCTSTR lpBuffer,int nBuffer,int nFlags);

public:
	CString GetInfoReceive()
	{
		return infoReceive;
	};
	CString GetClientIP()
	{
		return clientIP;
	};
	UINT    GetClientPort()
	{
		return clientPort;
	};
	void    SetInfoReceive(CString newInfoReceive)
	{
		infoReceive=newInfoReceive;
	};
	void    SetClientIP(CString newClientIP)
	{
		clientIP=newClientIP;
	};
	void    SetClientPort(UINT  newClientPort)
	{
		clientPort=newClientPort;
	};
private:
	ClientIPAddress tempClientIPAddress;
	static vector<ClientIPAddress> clientInfo;

	static CString infoReceive;
	static CString clientIP;
	static UINT    clientPort;
	static CList<CSocketServer*,CSocketServer*> m_list;

};


