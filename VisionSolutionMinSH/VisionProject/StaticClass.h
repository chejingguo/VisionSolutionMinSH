#pragma once

struct comErrorCode
{
	string   errorLabel;
	int    errorType;
	int    errorNo;
};

class CStaticClass
{
public:
	CStaticClass();
	~CStaticClass();
public:
	static HANDLE				 m_hEventRecieve;
	static bool                  m_bControl;
	static CString               m_csReceiveInfo;

	static comErrorCode          m_comErrorCode;
	static string                m_errorInfo;
};

