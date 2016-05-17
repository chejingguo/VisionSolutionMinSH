// CameraGrab.cpp : 实现文件
//

#include "stdafx.h"
#include "VisionProject.h"
#include "CameraGrab.h"
#include "afxdialogex.h"


// CCameraGrab 对话框

IMPLEMENT_DYNAMIC(CCameraGrab, CDialogEx)

CCameraGrab::CCameraGrab(CWnd* pParent,scPTGrab2* scPTGrab2)
	: CDialogEx(CCameraGrab::IDD, pParent)
{
	pDocScPTGrab2=scPTGrab2;
	m_bIsGrab =false;
	m_bSoftTraggerOn = false;
}

CCameraGrab::~CCameraGrab()
{
}

void CCameraGrab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCameraGrab, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_GRAB, &CCameraGrab::OnBnClickedBtnGrab)
	ON_BN_CLICKED(IDC_BTN_SNAP, &CCameraGrab::OnBnClickedBtnSnap)
	ON_BN_CLICKED(IDC_BTN_SOFTLRIAGGER, &CCameraGrab::OnBnClickedBtnSoftTriagger)
	ON_BN_CLICKED(IDC_BTN_HARDLRIAGGER, &CCameraGrab::OnBnClickedBtnHardTriagger)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CCameraGrab::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CCameraGrab::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CCameraGrab 消息处理程序


void CCameraGrab::OnBnClickedBtnGrab()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"相机初始化失败！");
		pDocScPTGrab2->Destroy();
	}
	else
	{
		pDocScPTGrab2->CameraInit();
	}
	if (!m_bIsGrab)
	{
		if (pDocScPTGrab2->m_bIsGrabbing)
		{
			pDocScPTGrab2->CloseTriggerMode();
        	pDocScPTGrab2->Freeze();
		}
		if (!pDocScPTGrab2->Grab()) 
		{
		   AfxMessageBox(L"采集图像失败");
		}
		//SetDlgItemText(IDC_BTN_GRAB,"停止采集");
		m_bIsGrab = TRUE;

	} 
	else
	{
		//SetDlgItemText(IDC_BTN_GRAB,"连续采集");
		m_bIsGrab = FALSE;
		pDocScPTGrab2->CloseTriggerMode();
    	pDocScPTGrab2->Freeze();

	}
}


void CCameraGrab::OnBnClickedBtnSnap()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"相机初始化失败！");
		pDocScPTGrab2->Destroy();
	}
	if (!pDocScPTGrab2->m_bOnline)
	{
		return;
	}
	if (pDocScPTGrab2 == NULL)
	{
		return;
	}

	if (pDocScPTGrab2->m_bIsGrabbing)
	{
		SetDlgItemText(IDC_BTN_GRAB,L"连续采集");
		m_bIsGrab = FALSE;
    	pDocScPTGrab2->Freeze();
		pDocScPTGrab2->CloseTriggerMode();
	}

	if(!pDocScPTGrab2->SnapImage())
	{
	    MessageBox(L"采集图像失败!");	
	}
}


void CCameraGrab::OnBnClickedBtnSoftTriagger()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"相机初始化失败！");
		pDocScPTGrab2->Destroy();
	}
	if (m_bSoftTraggerOn == false)
	{
		m_bSoftTraggerOn = true;
		
		// 如果相机正在连续采集，则暂停连续采集
		if (pDocScPTGrab2->IsGrabbing())
		{
			pDocScPTGrab2->Freeze();
		}
		
		// 如果相机处于触发模式，则关闭触发模式
		if (pDocScPTGrab2->m_nTriggerMode != ePTNotTrigger)
		{
			pDocScPTGrab2->CloseTriggerMode();
		}
		// 设置相机为软触发模式
		if (!pDocScPTGrab2->SetTriggerMode(ePTSoftwareTrigger))
		{
			return;
		}
		else
		{
			// 开始软触发采集
			pDocScPTGrab2->Grab();
		}
		m_bIsGrab = FALSE;
		pDocScPTGrab2->m_bSendGrabTragger = false;

	}
	else
	{
		pDocScPTGrab2->m_bSendGrabTragger = false;
		pDocScPTGrab2->SendSoftTrigger();
	}	
}


void CCameraGrab::OnBnClickedBtnHardTriagger()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"相机初始化失败！");
		pDocScPTGrab2->Destroy();
	}
	else
	{
		pDocScPTGrab2->CameraInit();
	}

	// 如果相机正在连续采集，则暂停连续采集
	if (pDocScPTGrab2->IsGrabbing())
	{
		pDocScPTGrab2->Freeze();
	}
	
	// 如果相机处于触发模式，则关闭触发模式
	if (pDocScPTGrab2->m_nTriggerMode != ePTNotTrigger)
	{
		pDocScPTGrab2->CloseTriggerMode();
	}
	
	// 设置相机为硬触发模式
	if (!pDocScPTGrab2->SetTriggerMode(ePTHardwareTrigger))
	{
		return;
	}
	
	// 开始硬触发采集
	pDocScPTGrab2->Grab();
	pDocScPTGrab2->m_bSendGrabTragger = false;
	m_bSoftTraggerOn = false;

	AfxMessageBox(L"设定完成，点击确定！");
}


void CCameraGrab::OnBnClickedBtnLoad()
{
	// TODO:  在此添加控件通知处理程序代码
	float fValue;
	pDocScPTGrab2->GetCurGain(&fValue);
	CString strValue;
	strValue.Format(L"%4.2f", fValue);
	GetDlgItem(IDC_EDIT_GAIN)->SetWindowText(strValue);

	fValue;
	pDocScPTGrab2->GetCurShutter(&fValue);
	strValue.Format(L"%4.2f", fValue);
	GetDlgItem(IDC_EDIT_SHUTTER)->SetWindowText(strValue);
	pDocScPTGrab2->GetCurFrameRate(&fValue);

	strValue.Format(L"%4.2f", fValue);
	GetDlgItem(IDC_EDIT_FRAMERATE)->SetWindowText(strValue);
}


void CCameraGrab::OnBnClickedBtnSave()
{
	// TODO:  在此添加控件通知处理程序代码
	float fValue = GetDlgItemInt(IDC_EDIT_GAIN);
	pDocScPTGrab2->SetCurGain(fValue);

	fValue = GetDlgItemInt(IDC_EDIT_SHUTTER);
	pDocScPTGrab2->SetCurShutter(fValue);

	fValue = GetDlgItemInt(IDC_EDIT_FRAMERATE);
	pDocScPTGrab2->SetCurShutter(fValue);
	//pScPTGrab2->SetCurFrameRate(0);
}
