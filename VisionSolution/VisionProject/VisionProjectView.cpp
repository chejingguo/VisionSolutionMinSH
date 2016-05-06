
// VisionProjectView.cpp : CVisionProjectView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "VisionProject.h"
#endif

#include "VisionProjectDoc.h"
#include "VisionProjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVisionProjectView

IMPLEMENT_DYNCREATE(CVisionProjectView, CFormView)

BEGIN_MESSAGE_MAP(CVisionProjectView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_32778, &CVisionProjectView::On32778)
END_MESSAGE_MAP()

// CVisionProjectView ����/����

CVisionProjectView::CVisionProjectView()
	: CFormView(CVisionProjectView::IDD)
{
	// TODO:  �ڴ˴���ӹ������

}

CVisionProjectView::~CVisionProjectView()
{
}

void CVisionProjectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHOW, m_CEditShow);
	DDX_Control(pDX, IDC_EDIT_STATE, m_CEdit_State);
	DDX_Control(pDX, IDC_LIST_INFO, m_listShow);
}

BOOL CVisionProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}

void CVisionProjectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	//
	CString StrShowInfo;
	StrShowInfo.Format(L"\r\nʶ������: %d\r\n\r\n��  ��X1: %.1f\r\n\r\n��  ��Y1: %.1f\r\n\r\n��  ��Rz1: %.1f\r\n\r\n��  ��X2: %.1f\r\n\r\n��  ��Y2: %.1f\r\n\r\n��  ��Rz2: %.1f\r\n\r\n", TDetectData::iCount, TDetectData::X1, TDetectData::Y1, TDetectData::Rz1,TDetectData::X2, TDetectData::Y2, TDetectData::Rz2);

	m_CEditShow.SetWindowText(StrShowInfo);

	CString StrStateInfo;
	CString csIP(TShowData::sConnectIP.c_str());
	//StrStateInfo.Format(L"����ͺ�: %d\r\n\r\n����״̬: %s\r\n\r\n������Ϣ: %s", 0,L"׼��",L"192.168.0.10:1003");
	StrStateInfo.Format(L"����ͺ�: %d\r\n\r\n����ģʽ: %s\r\n\r\n������Ϣ: %s", 0, L"׼��", csIP);
	m_CEdit_State.SetWindowText(StrStateInfo);

	//
	CRect rect;
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	m_listShow.GetClientRect(&rect);
	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_listShow.SetExtendedStyle(m_listShow.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Ϊ�б���ͼ�ؼ��������   
	m_listShow.InsertColumn(0, _T("���"), LVCFMT_CENTER, rect.Width() / 4, 0);
	m_listShow.InsertColumn(1, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 1);
	m_listShow.InsertColumn(2, _T("ʱ��"), LVCFMT_CENTER, rect.Width() / 2, 2);
	m_listShow.InsertColumn(3, _T("��Ϣ"), LVCFMT_CENTER, rect.Width() / 2, 3);

	// ���б���ͼ�ؼ��в����б���������б������ı�   

	m_listShow.InsertItem(0, _T("01"));
	m_listShow.InsertItem(1, _T("02"));
	//m_listShow.InsertItem(2, _T("03"));

	CString strPart_Time; {SYSTEMTIME sysTime = { 0 }; GetLocalTime(&sysTime); strPart_Time.Format(_T("[%d-%d-%d-%d:%d:%d:%d]"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds); }
	//m_listShow.InsertItem(1, strNo);
	for (int i = 0; i < 2; i++)
	{
		m_listShow.SetItemText(i, 1, L"0");
		m_listShow.SetItemText(i, 2, strPart_Time);
		m_listShow.SetItemText(i, 3, L"null");
	}
}
void CVisionProjectView::UpdateWindowInfo()
{
	//TDetectData::iCount++;

	CString StrShowInfo;
	StrShowInfo.Format(L"\r\nʶ������: %d\r\n\r\n��  ��X1: %.1f\r\n\r\n��  ��Y1: %.1f\r\n\r\n��  ��Rz1: %.1f\r\n\r\n��  ��X2: %.1f\r\n\r\n��  ��Y2: %.1f\r\n\r\n��  ��Rz2: %.1f\r\n\r\n", TDetectData::iCount, TDetectData::X1, TDetectData::Y1, TDetectData::Rz1, TDetectData::X2, TDetectData::Y2, TDetectData::Rz2);

	m_CEditShow.SetWindowText(StrShowInfo);

	if (TShowData::bConnectFlag)
	{
		CString StrStateInfo;
		CString csIP(TShowData::sConnectIP.c_str());
		StrStateInfo.Format(L"����ͺ�: %d\r\n\r\n����ģʽ: %s\r\n\r\n������Ϣ: %s\r\n", 0, TShowData::sWorkMode, csIP);
		m_CEdit_State.SetWindowText(StrStateInfo);

		TShowData::bConnectFlag = false;
	}
}

// CVisionProjectView ���

#ifdef _DEBUG
void CVisionProjectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVisionProjectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVisionProjectDoc* CVisionProjectView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisionProjectDoc)));
	return (CVisionProjectDoc*)m_pDocument;
}
#endif //_DEBUG


// CVisionProjectView ��Ϣ�������


void CVisionProjectView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (GetDlgItem(IDC_IMG)->GetSafeHwnd() && GetDlgItem(IDC_EDIT_SHOW)->GetSafeHwnd() && GetDlgItem(IDC_LIST_INFO)->GetSafeHwnd()){
		if (nType == SIZE_RESTORED){
			CRect rs; GetClientRect(&rs);  //
			GetDlgItem(IDC_EDIT_SHOW)->SetWindowPos(this, rs.left, rs.top, rs.Width() / 6, rs.Height() * 3 / 7, SWP_SHOWWINDOW | SWP_NOZORDER);//
			GetDlgItem(IDC_EDIT_STATE)->SetWindowPos(this, rs.left, rs.top+rs.Height() * 3 / 7, rs.Width() / 6, rs.Height() * 3 / 7, SWP_SHOWWINDOW | SWP_NOZORDER);//
			GetDlgItem(IDC_IMG)->SetWindowPos(this, rs.left + rs.Width() / 6, rs.top, rs.Width() - rs.Width() / 6, rs.Height() * 6 / 7, SWP_SHOWWINDOW | SWP_NOZORDER);//
			GetDlgItem(IDC_LIST_INFO)->SetWindowPos(this, rs.left, rs.top + rs.Height() * 6 / 7, rs.Width(), rs.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
		}
	}
}

void CVisionProjectView::MatToCImage(Mat &mat, CImage &cImage)
{  
    //create new CImage  
    int width    = mat.cols;  
    int height   = mat.rows;  
    int channels = mat.channels();  
  
    cImage.Destroy(); //clear  
    cImage.Create(width,height, 8*channels );
  
    //copy values  
    uchar* ps;  
    uchar* pimg = (uchar*)cImage.GetBits(); //A pointer to the bitmap buffer  
          
    //The pitch is the distance, in bytes. represent the beginning of   
    // one bitmap line and the beginning of the next bitmap line  
    int step = cImage.GetPitch();  
  
    for (int i = 0; i < height; ++i)  
    {  
        ps = (mat.ptr<uchar>(i));  
        for ( int j = 0; j < width; ++j )  
        {  
            if ( channels == 1 ) //gray  
            {  
                *(pimg + i*step + j) = ps[j];  
            }  
            else if ( channels == 3 ) //color  
            {  
                for (int k = 0 ; k < 3; ++k )  
                {  
                    *(pimg + i*step + j*3 + k ) = ps[j*3 + k];  
                }             
            }  
        }     
    }  
  
}  

void CVisionProjectView::DrawImage(Mat img, UINT ID)
{
	CImage image; //����ͼƬ��     
	MatToCImage(img, image);

	CRect rect;GetDlgItem(ID)->GetWindowRect(&rect); 
	ScreenToClient(&rect);
	CWnd *pWnd = GetDlgItem(ID);//���pictrue�ؼ����ڵľ��   
	pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������   
	CDC *pDC = pWnd->GetDC();//���pictrue�ؼ���DC   
	image.Draw(pDC->m_hDC, rect); //��ͼƬ����Picture�ؼ���ʾ�ľ�������   
	ReleaseDC(pDC);//�ͷ�picture�ؼ���DC 
}

void CVisionProjectView::On32778()
{
	// TODO:  �ڴ���������������
	Mat mat = imread("1.bmp");
	DrawImage(mat, IDC_IMG);
}


void CVisionProjectView::OnDraw(CDC* /*pDC*/)
{
	// TODO:  �ڴ����ר�ô����/����û���
	CVisionProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (pDoc->bUpdateForm)
	{
		CString StrShowInfo;
		if (pDoc->m_scPTGrab2->detectInfoUI.size() == 0)
		{
			StrShowInfo.Format(L"\r\nʶ������: %d\r\n\r\n��  ��X1: %.1f\r\n\r\n��  ��Y1: %.1f\r\n\r\n��  ��Rz1: %.1f\r\n\r\n",
				TDetectData::iIndex, 0.0, 0.0, 0.0);
		}
		else
		{
			StrShowInfo.Format(L"\r\nʶ������: %d\r\n\r\n��  ��X1: %.1f\r\n\r\n��  ��Y1: %.1f\r\n\r\n��  ��Rz1: %.1f\r\n\r\n",
				pDoc->m_scPTGrab2->detectInfoUI.size(),
				pDoc->m_scPTGrab2->detectInfoUI[0].x, pDoc->m_scPTGrab2->detectInfoUI[0].y, pDoc->m_scPTGrab2->detectInfoUI[0].rz);

		}
		
		m_CEditShow.SetWindowText(StrShowInfo);

		pDoc->m_scPTGrab2->detectInfoUI.clear();

		/*
		//
		CString strPart_Time; {SYSTEMTIME sysTime = { 0 }; GetLocalTime(&sysTime); strPart_Time.Format(_T("[%d-%d-%d-%d:%d:%d:%d]"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds); }
		for (int i = 1; i < 3; i++)
		{
			m_listShow.SetItemText(i, 1, L"0");
			m_listShow.SetItemText(i, 2, strPart_Time);
			m_listShow.SetItemText(i, 3, L"null");
		}
		*/
		CWnd *picWnd = GetDlgItem(IDC_IMG);
		//m_gdiAssist.DrawMatToWnd(picWnd->GetSafeHwnd(), pDoc->GetIndexImage(1), Cvgdi::DM_KEEPSRCTOFIT);
		//m_gdiAssist.DrawMatToWnd(picWnd->GetSafeHwnd(), scPTGrab2::m_srcMat, Cvgdi::DM_RESIZESRCTOFIT);
		m_gdiAssist.DrawMatToWnd(picWnd->GetSafeHwnd(), scPTGrab2::m_srcMat, Cvgdi::DM_NORESIZEALIANCENTER);
		//UpdateWindowInfo();
		

		pDoc->bUpdateForm = false;
	}
	
}


void CVisionProjectView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO:  �ڴ����ר�ô����/����û���

	GetDocument()->UpdateAllViews(this);
}
