#pragma once
#include "afxsock.h"
// CSocketClient command target

class CSocketClient: public CSocket
{
public:
	CSocketClient();
	virtual ~CSocketClient();
	virtual void OnReceive(int nErrorCode);

public:
	CString GetInfoReceive()
	{
		return infoReceive;
	};
	CString GetServerIP()
	{
		return serverIP;
	};
	UINT  GetServerPort()
	{
		return serverPort;
	};
	void    SetInfoReceive(CString newInfoReceive)
	{
		infoReceive=newInfoReceive;
	};
	void    SetServerIP(CString newServerIP)
	{
		serverIP=newServerIP;
	};
	void    SetServerPort(UINT  newServerPort)
	{
		serverPort=newServerPort;
	};
private:
	CString infoReceive;
	CString serverIP;
	UINT    serverPort;
};


