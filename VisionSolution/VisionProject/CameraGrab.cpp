// CameraGrab.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VisionProject.h"
#include "CameraGrab.h"
#include "afxdialogex.h"


// CCameraGrab �Ի���

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


// CCameraGrab ��Ϣ�������


void CCameraGrab::OnBnClickedBtnGrab()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"�����ʼ��ʧ�ܣ�");
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
		   AfxMessageBox(L"�ɼ�ͼ��ʧ��");
		}
		//SetDlgItemText(IDC_BTN_GRAB,"ֹͣ�ɼ�");
		m_bIsGrab = TRUE;

	} 
	else
	{
		//SetDlgItemText(IDC_BTN_GRAB,"�����ɼ�");
		m_bIsGrab = FALSE;
		pDocScPTGrab2->CloseTriggerMode();
    	pDocScPTGrab2->Freeze();

	}
}


void CCameraGrab::OnBnClickedBtnSnap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"�����ʼ��ʧ�ܣ�");
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
		SetDlgItemText(IDC_BTN_GRAB,L"�����ɼ�");
		m_bIsGrab = FALSE;
    	pDocScPTGrab2->Freeze();
		pDocScPTGrab2->CloseTriggerMode();
	}

	if(!pDocScPTGrab2->SnapImage())
	{
	    MessageBox(L"�ɼ�ͼ��ʧ��!");	
	}
}


void CCameraGrab::OnBnClickedBtnSoftTriagger()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"�����ʼ��ʧ�ܣ�");
		pDocScPTGrab2->Destroy();
	}
	if (m_bSoftTraggerOn == false)
	{
		m_bSoftTraggerOn = true;
		
		// ���������������ɼ�������ͣ�����ɼ�
		if (pDocScPTGrab2->IsGrabbing())
		{
			pDocScPTGrab2->Freeze();
		}
		
		// ���������ڴ���ģʽ����رմ���ģʽ
		if (pDocScPTGrab2->m_nTriggerMode != ePTNotTrigger)
		{
			pDocScPTGrab2->CloseTriggerMode();
		}
		// �������Ϊ����ģʽ
		if (!pDocScPTGrab2->SetTriggerMode(ePTSoftwareTrigger))
		{
			return;
		}
		else
		{
			// ��ʼ�����ɼ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!pDocScPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"�����ʼ��ʧ�ܣ�");
		pDocScPTGrab2->Destroy();
	}
	else
	{
		pDocScPTGrab2->CameraInit();
	}

	// ���������������ɼ�������ͣ�����ɼ�
	if (pDocScPTGrab2->IsGrabbing())
	{
		pDocScPTGrab2->Freeze();
	}
	
	// ���������ڴ���ģʽ����رմ���ģʽ
	if (pDocScPTGrab2->m_nTriggerMode != ePTNotTrigger)
	{
		pDocScPTGrab2->CloseTriggerMode();
	}
	
	// �������ΪӲ����ģʽ
	if (!pDocScPTGrab2->SetTriggerMode(ePTHardwareTrigger))
	{
		return;
	}
	
	// ��ʼӲ�����ɼ�
	pDocScPTGrab2->Grab();
	pDocScPTGrab2->m_bSendGrabTragger = false;
	m_bSoftTraggerOn = false;

	AfxMessageBox(L"�趨��ɣ����ȷ����");
}


void CCameraGrab::OnBnClickedBtnLoad()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	float fValue = GetDlgItemInt(IDC_EDIT_GAIN);
	pDocScPTGrab2->SetCurGain(fValue);

	fValue = GetDlgItemInt(IDC_EDIT_SHUTTER);
	pDocScPTGrab2->SetCurShutter(fValue);

	fValue = GetDlgItemInt(IDC_EDIT_FRAMERATE);
	pDocScPTGrab2->SetCurShutter(fValue);
	//pScPTGrab2->SetCurFrameRate(0);
}
