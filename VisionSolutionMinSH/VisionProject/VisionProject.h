
// VisionProject.h : VisionProject Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include"CameraGrab.h"

// CVisionProjectApp:
// �йش����ʵ�֣������ VisionProject.cpp
//

class CVisionProjectApp : public CWinApp
{
public:
	CVisionProjectApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CVisionProjectApp theApp;
