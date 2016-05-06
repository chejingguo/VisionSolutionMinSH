
// VisionProjectView.cpp : CVisionProjectView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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

// CVisionProjectView 构造/析构

CVisionProjectView::CVisionProjectView()
	: CFormView(CVisionProjectView::IDD)
{
	// TODO:  在此处添加构造代码

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
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CVisionProjectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	//
	CString StrShowInfo;
	StrShowInfo.Format(L"\r\n识别数量: %d\r\n\r\n坐  标X1: %.1f\r\n\r\n坐  标Y1: %.1f\r\n\r\n角  度Rz1: %.1f\r\n\r\n坐  标X2: %.1f\r\n\r\n坐  标Y2: %.1f\r\n\r\n角  度Rz2: %.1f\r\n\r\n", TDetectData::iCount, TDetectData::X1, TDetectData::Y1, TDetectData::Rz1,TDetectData::X2, TDetectData::Y2, TDetectData::Rz2);

	m_CEditShow.SetWindowText(StrShowInfo);

	CString StrStateInfo;
	CString csIP(TShowData::sConnectIP.c_str());
	//StrStateInfo.Format(L"检测型号: %d\r\n\r\n工作状态: %s\r\n\r\n连接信息: %s", 0,L"准备",L"192.168.0.10:1003");
	StrStateInfo.Format(L"检测型号: %d\r\n\r\n工作模式: %s\r\n\r\n连接信息: %s", 0, L"准备", csIP);
	m_CEdit_State.SetWindowText(StrStateInfo);

	//
	CRect rect;
	// 获取编程语言列表视图控件的位置和大小   
	m_listShow.GetClientRect(&rect);
	// 为列表视图控件添加全行选中和栅格风格   
	m_listShow.SetExtendedStyle(m_listShow.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 为列表视图控件添加三列   
	m_listShow.InsertColumn(0, _T("序号"), LVCFMT_CENTER, rect.Width() / 4, 0);
	m_listShow.InsertColumn(1, _T("类型"), LVCFMT_CENTER, rect.Width() / 3, 1);
	m_listShow.InsertColumn(2, _T("时间"), LVCFMT_CENTER, rect.Width() / 2, 2);
	m_listShow.InsertColumn(3, _T("信息"), LVCFMT_CENTER, rect.Width() / 2, 3);

	// 在列表视图控件中插入列表项，并设置列表子项文本   

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
	StrShowInfo.Format(L"\r\n识别数量: %d\r\n\r\n坐  标X1: %.1f\r\n\r\n坐  标Y1: %.1f\r\n\r\n角  度Rz1: %.1f\r\n\r\n坐  标X2: %.1f\r\n\r\n坐  标Y2: %.1f\r\n\r\n角  度Rz2: %.1f\r\n\r\n", TDetectData::iCount, TDetectData::X1, TDetectData::Y1, TDetectData::Rz1, TDetectData::X2, TDetectData::Y2, TDetectData::Rz2);

	m_CEditShow.SetWindowText(StrShowInfo);

	if (TShowData::bConnectFlag)
	{
		CString StrStateInfo;
		CString csIP(TShowData::sConnectIP.c_str());
		StrStateInfo.Format(L"检测型号: %d\r\n\r\n工作模式: %s\r\n\r\n连接信息: %s\r\n", 0, TShowData::sWorkMode, csIP);
		m_CEdit_State.SetWindowText(StrStateInfo);

		TShowData::bConnectFlag = false;
	}
}

// CVisionProjectView 诊断

#ifdef _DEBUG
void CVisionProjectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVisionProjectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVisionProjectDoc* CVisionProjectView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisionProjectDoc)));
	return (CVisionProjectDoc*)m_pDocument;
}
#endif //_DEBUG


// CVisionProjectView 消息处理程序


void CVisionProjectView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
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
	CImage image; //创建图片类     
	MatToCImage(img, image);

	CRect rect;GetDlgItem(ID)->GetWindowRect(&rect); 
	ScreenToClient(&rect);
	CWnd *pWnd = GetDlgItem(ID);//获得pictrue控件窗口的句柄   
	pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
	CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC   
	image.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域   
	ReleaseDC(pDC);//释放picture控件的DC 
}

void CVisionProjectView::On32778()
{
	// TODO:  在此添加命令处理程序代码
	Mat mat = imread("1.bmp");
	DrawImage(mat, IDC_IMG);
}


void CVisionProjectView::OnDraw(CDC* /*pDC*/)
{
	// TODO:  在此添加专用代码和/或调用基类
	CVisionProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (pDoc->bUpdateForm)
	{
		CString StrShowInfo;
		if (pDoc->m_scPTGrab2->detectInfoUI.size() == 0)
		{
			StrShowInfo.Format(L"\r\n识别数量: %d\r\n\r\n坐  标X1: %.1f\r\n\r\n坐  标Y1: %.1f\r\n\r\n角  度Rz1: %.1f\r\n\r\n",
				TDetectData::iIndex, 0.0, 0.0, 0.0);
		}
		else
		{
			StrShowInfo.Format(L"\r\n识别数量: %d\r\n\r\n坐  标X1: %.1f\r\n\r\n坐  标Y1: %.1f\r\n\r\n角  度Rz1: %.1f\r\n\r\n",
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
	// TODO:  在此添加专用代码和/或调用基类

	GetDocument()->UpdateAllViews(this);
}
