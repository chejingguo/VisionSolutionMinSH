// LOG.cpp : implementation file
//

#include "stdafx.h"
#include "log.h"

#define LOG_EVENT L"ChyLogWrite"  

IMPLEMENT_DYNAMIC(CLOG, CWnd)

CLOG::CLOG()
{

}

CLOG::~CLOG()
{
}

BEGIN_MESSAGE_MAP(CLOG, CWnd)
END_MESSAGE_MAP()

CString CLOG::s_strLogFile = L"";  

CString CLOG::s_strLogPrefix = L"";  

HANDLE CLOG::s_hWriteEvent = NULL;  
// CLOG
// CLOG message handlers
// 得到可执行程序所在目录
//  BOOL bIncludeSep -- 是否包含最后的分隔符"\"
CString GetExePath(BOOL bIncludeSep)
{
 // 得到当前的文件名
 CString strFileName;
 GetModuleFileName(AfxGetInstanceHandle(),strFileName.GetBuffer(_MAX_PATH),_MAX_PATH);
 strFileName.ReleaseBuffer();
 // 得到当前目录
 strFileName=strFileName.Left(strFileName.ReverseFind(L'\\')+1);
 if(bIncludeSep)
	 return strFileName;
 else
	 return strFileName.Left(strFileName.GetLength()-1);
}

//-获取最后的文件名 如果给定文件不是全路径，就是相对于exe-

CString GetFileForExePath(LPCTSTR strCurFileName)
{
   CString strPath = strCurFileName;
   if(!strPath.IsEmpty())
   {
	   //-相对路径-
       if(strPath.Find(L":")<=0)
	   {     
          strPath.Format(L"%s%s", GetExePath(FALSE), strCurFileName);    
       }     
  }     
  return strPath;  
}  

short CLOG::SetLogFile(LPCTSTR strPath)

{

 if(strPath==NULL || strPath[0]==0)

  s_strLogFile = GetFileForExePath(L".txt");

 else

  s_strLogFile = GetFileForExePath(strPath);

 return 1;

}



CString CLOG::GetLogFile()

{

 return s_strLogFile;

}



short CLOG::ViewLogFile()

{

 CString strLogFile = GetLogFile();

 ShellExecute(NULL, L"open", strLogFile, NULL, NULL, SW_SHOW);

 return strLogFile.IsEmpty()?0:1;

}



short CLOG::SetPrefix(LPCTSTR strPrefix)

{

 if(strPrefix && strPrefix[0])

 {

  s_strLogPrefix = strPrefix;

 }
 return 1;

}



CString CLOG::sOutV(LPCTSTR strType, LPCTSTR strFormat, va_list valist)

{ 

 CString   strPart_Prefix;

 if(!s_strLogPrefix.IsEmpty())

 {

  strPart_Prefix.Format(L"[%s]", s_strLogPrefix);

 }

 

 CString   strPartLime;

 {

  SYSTEMTIME sysTime  = {0};

  GetLocalTime(&sysTime);

  strPartLime.Format(L"[%2d-%2d %2d:%2d:%2d_%3d]", 

   sysTime.wMonth, sysTime.wDay,  

   sysTime.wHour, sysTime.wMinute, sysTime.wSecond, 

   sysTime.wMilliseconds);  

 }

 

 CString   strPartLype;

 if(strType && strType[0])

 {

  strPartLype.Format(L"[%s]", strType);

 }

 

 CString   strPart_Info;

 {   

  strPart_Info.FormatV(strFormat, valist);

 }

  

 CString str = strPart_Prefix + strPartLime + strPartLype+ strPart_Info;



 return str;

}



CString CLOG::sOut0(LPCTSTR strType, LPCTSTR strFormat,...)

{

 va_list  valist;

 va_start(valist, strFormat);    

 CString strInfo = sOutV(strType, strFormat, valist);

 va_end(valist); 



 return strInfo;

}
CString CLOG::sOut(LPCTSTR strFormat,...)

{
 va_list  valist;

 va_start(valist, strFormat);

 CString strInfo = sOutV(NULL, strFormat, valist);

 va_end(valist);
 return strInfo;

}

short CLOG::OutV(LPCTSTR strType, LPCTSTR strFormat, va_list valist)

{

 //--

 if(s_hWriteEvent==NULL)

 {

  s_hWriteEvent = OpenEvent(0, FALSE,LOG_EVENT);

  if(s_hWriteEvent==NULL)

   s_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT); 

 }



 WaitForSingleObject(s_hWriteEvent, INFINITE);



 //-打开关闭文件-

 if(s_strLogFile.IsEmpty())

  SetLogFile(NULL);

 CStdioFile file;

 if(file.Open(s_strLogFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
 {
  CString  strPart_NewLine = L"\n";
  CString  strInfo = sOutV(strType, strFormat, valist);  
  CString  str = strPart_NewLine + strInfo;
  file.SeekToEnd();
  file.WriteString(str);

  file.Close();

 }
 SetEvent(s_hWriteEvent);
 return 1;

}



short CLOG::Out0(LPCTSTR strType, LPCTSTR strFormat,...)

{
 va_list  valist;

 va_start(valist, strFormat);    

 short rtn = OutV(strType, strFormat, valist);
 va_end(valist);
 return rtn;

}

short CLOG::Out(LPCTSTR strFormat,...)

{

 va_list  valist;

 va_start(valist, strFormat);

 short rtn = OutV(NULL, strFormat, valist);

 va_end(valist);



 return rtn;

}

