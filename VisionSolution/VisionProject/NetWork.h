#pragma once
//socket file

#include "SocketServer.h"

typedef enum tagNetWorkMode
{
	SERVER_TCP = 0,		    
	CLIENT_TCP = 1, 
	SERVER_UDP = 2,
	CLIENT_UDP = 3,	    
	SERIAL_Port = 4, 
}NetWorkMode;

class TNetWork
{
public:
	//TNetWork(void);
	TNetWork();
	~TNetWork(void);
private:
	CSocketServer     m_sockServer;

public:
	void NetWorkInit();
	void Send(LPCTSTR lpBuffer, int nBuffer, int nFlags);
	void Send(CString info);
	CString Recieve();
};

