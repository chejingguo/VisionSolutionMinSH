#include "stdafx.h"
#include "StaticClass.h"


CStaticClass::CStaticClass()
{
}


CStaticClass::~CStaticClass()
{
}

HANDLE CStaticClass::m_hEventRecieve = CreateEvent(NULL, true, false, L"");
bool CStaticClass::m_bControl = false;
CString  CStaticClass::m_csReceiveInfo;

string CStaticClass::m_errorInfo;
comErrorCode  CStaticClass::m_comErrorCode;