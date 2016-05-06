
// VisionProjectDoc.cpp : CVisionProjectDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "VisionProject.h"
#endif

#include "VisionProjectDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVisionProjectDoc

IMPLEMENT_DYNCREATE(CVisionProjectDoc, CDocument)

BEGIN_MESSAGE_MAP(CVisionProjectDoc, CDocument)
	ON_COMMAND(IDM_CAMERA_GRAB, &CVisionProjectDoc::OnCameraGrab)
	ON_COMMAND(IDM_COM, &CVisionProjectDoc::OnCom)
	ON_COMMAND(ID_CALIBRATION, &CVisionProjectDoc::OnCalibration)
	ON_COMMAND(ID_DET_OBJECT_PAR, &CVisionProjectDoc::OnDetObjectPar)
	ON_COMMAND(ID_32784, &CVisionProjectDoc::On32784)
END_MESSAGE_MAP()

Mat CVisionProjectDoc::m_imageSrc;


void CVisionProjectDoc::GrapInit()
{
	
	m_scPTGrab2 = new scPTGrab2(pTNetWork,eWaitEvent);

	m_scPTGrab2->SetOwner(this, GrabDisplayCallback);
	m_scPTGrab2->StartHardGrab();
	/*
	if (!m_scPTGrab2->InitFromIndex(0))
	{
		AfxMessageBox(L"相机初始化失败！");
		m_scPTGrab2->Destroy();
	}
	else
	{
		m_scPTGrab2->CameraInit();

	}
	*/
	m_scPTGrab2->CreateProcessThread();
	m_scPTGrab2->CreateStartStopThread();

	
}
void CVisionProjectDoc::GrabDisplayCallback(void* pOwner, PTStatus status, unsigned char* pImgData, long lWidth, long lHeight, PTPixelFormat pixelFormat)
{
	CVisionProjectDoc *pDoc = (CVisionProjectDoc*)pOwner;
	/*
	if (!pImgData) return;

	pDoc->ImageProcess(pImgData, lWidth, lHeight);
	*/
	WaitForSingleObject(pDoc->m_scPTGrab2->m_hEventToUpdateUI, INFINITE);

	pDoc->RedrawAllViews();
	ResetEvent(pDoc->m_scPTGrab2->m_hEventToUpdateUI);
}
void CVisionProjectDoc::ImageProcess(unsigned char* pImgData, long lWidth, long lHeight)
{
	Mat image(lHeight, lWidth, CV_8UC1, pImgData);
	cvtColor(image, m_imageSrc, CV_GRAY2RGB);
	Size size = Size(700, 476);
	Rect roiRect = Rect(54, 70, size.width, size.height);
	m_imageSrc = m_imageSrc(roiRect);

	CaculateDetectData();
	SendInfo2Robot();
}

int CVisionProjectDoc::CaculateDetectData()
{
	m_DoubleCrane.imageProcess(m_imageSrc);
	int inum = m_DoubleCrane.contNumber;
	if (inum==1)
	{
		TDetectData::iType1 = 0;
		TDetectData::iCount = inum;
		TDetectData::X1=m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.x;
		TDetectData::Y1=m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.y;
		TDetectData::Rz1=m_DoubleCrane.objectInfoVect[0].objLoca.locationAngle;
	}
	else if (inum == 2)
	{
		TDetectData::iType1 = 0;
		TDetectData::iCount = inum;
		TDetectData::X1 = m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.x;
		TDetectData::Y1 = m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.y;
		TDetectData::Rz1 = m_DoubleCrane.objectInfoVect[0].objLoca.locationAngle;
		TDetectData::iType2 = 0;
		TDetectData::X2 = m_DoubleCrane.objectInfoVect[1].objLoca.locationCenter.x;
		TDetectData::Y2 = m_DoubleCrane.objectInfoVect[1].objLoca.locationCenter.y;
		TDetectData::Rz2 = m_DoubleCrane.objectInfoVect[1].objLoca.locationAngle;
	}
	else
	{
		inum = 0;
	}

	return inum;
}
void CVisionProjectDoc::SendInfo2Robot()
{
	CString strSend;

	if (0 == m_DoubleCrane.contNumber)
	{
		strSend.Format(L"%d;%d;%.1f;%.1f;%.1f;$",10,m_DoubleCrane.contNumber, +000.0, +000.0, 000.0);
		tNetWork.Send(strSend);
	}
	if (1 == m_DoubleCrane.contNumber)
	{
		strSend.Format(L"%d;%d;%.1f;%.1f;%.1f;$", 10, m_DoubleCrane.contNumber, m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.x, m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.y, m_DoubleCrane.objectInfoVect[0].objLoca.locationAngle);
		tNetWork.Send(strSend);
	}

	else if (2 == m_DoubleCrane.contNumber)
	{
		strSend.Format(L"%d;%d;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;$",
			10, m_DoubleCrane.contNumber, m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.x, m_DoubleCrane.objectInfoVect[0].objLoca.locationCenter.y, m_DoubleCrane.objectInfoVect[0].objLoca.locationAngle,
			m_DoubleCrane.objectInfoVect[1].objLoca.locationCenter.x, m_DoubleCrane.objectInfoVect[1].objLoca.locationCenter.y, m_DoubleCrane.objectInfoVect[1].objLoca.locationAngle);
		tNetWork.Send(strSend);
	}
}
void CVisionProjectDoc::RedrawAllViews()
{
	bUpdateForm = true;

	POSITION pos = GetFirstViewPosition();

	
	while (pos != NULL)
	{
		InvalidateRect(GetNextView(pos)->GetSafeHwnd(), NULL, FALSE);
	}

}
Mat CVisionProjectDoc::GetIndexImage(UINT iIndex)
{
	Mat result;
	if (iIndex == 1)
	{
		result = m_imageSrc;
	}
	return result;
}
void CVisionProjectDoc::GrapExit()
{
	if (m_scPTGrab2 != NULL)
	{
		delete m_scPTGrab2;
	}
}
// CVisionProjectDoc 构造/析构

CVisionProjectDoc::CVisionProjectDoc()
{
	// TODO:  在此添加一次性构造代码
	GrapInit();
	/*
	pCStartCtrl = new CStartCtrl(pTNetWork);

	bool bthreadIsSuccess=pCStartCtrl->CreateThread();
	
	if (!bthreadIsSuccess)
	{
		AfxMessageBox(L"线程创建失败！");
	}
	*/
}

CVisionProjectDoc::~CVisionProjectDoc()
{
	//GrapExit();
}

BOOL CVisionProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CVisionProjectDoc 序列化

void CVisionProjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CVisionProjectDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CVisionProjectDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CVisionProjectDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVisionProjectDoc 诊断

#ifdef _DEBUG
void CVisionProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVisionProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVisionProjectDoc 命令


void CVisionProjectDoc::OnCameraGrab()
{
	// TODO:  在此添加命令处理程序代码
	CCameraGrab cCameraGrab(NULL, m_scPTGrab2);
	cCameraGrab.DoModal();
}


void CVisionProjectDoc::OnCom()
{
	// TODO:  在此添加命令处理程序代码
	CComPart m_CComParameter(NULL, pTNetWork);
	m_CComParameter.DoModal();
}


void CVisionProjectDoc::OnCalibration()
{
	// TODO:  在此添加命令处理程序代码
	CVisionCalibration m_VisionCalibration(NULL, m_imageSrc);
	m_VisionCalibration.DoModal();
}


void CVisionProjectDoc::OnDetObjectPar()
{
	// TODO:  在此添加命令处理程序代码

	CDetObjectPar  m_DetObjectPar;
	m_DetObjectPar.DoModal();
}


void CVisionProjectDoc::On32784()
{
	// TODO:  在此添加命令处理程序代码
	if (m_scPTGrab2->IsOnline())
	{
		if (m_scPTGrab2->m_pCamera->IsConnected())
		{
			AfxMessageBox(L"camera is connected！");
		}
		else
		{
			AfxMessageBox(L"camera is not connected！");
		}
	}
	else
	{
		AfxMessageBox(L"camera is not online!");
	}

}
