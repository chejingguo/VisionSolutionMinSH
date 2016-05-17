#include "StdAfx.h"
#include "NetWork.h"


TNetWork::TNetWork()
{
	NetWorkInit();
}

TNetWork::~TNetWork(void)
{
}
void TNetWork::NetWorkInit()
{

	if(!m_sockServer.Create(1003))
	{
		int nError = GetLastError();
		AfxMessageBox(_T("服务器网络端口创建失败！"));
	}

	m_sockServer.Listen();
}

void TNetWork::Send(CString info)
{

	m_sockServer.SendInfo2Client(info);
}
void TNetWork::Send(LPCTSTR lpBuffer, int nBuffer, int nFlags)
{

	m_sockServer.SendInfo2Client(lpBuffer, nBuffer,nFlags);
}
CString TNetWork::Recieve()
{
	return m_sockServer.GetInfoReceive();
}