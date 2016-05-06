
// VisionProjectDoc.h : CVisionProjectDoc ��Ľӿ�
//
#include"svPTGrab2.h"
#include"DoubleCrane.h"
#include"NetWork.h"
#include"ComPart.h"
#include"VisionCalibration.h"
#include"DetectData.h"
#include "DetObjectPar.h"

#pragma once


class CVisionProjectDoc : public CDocument
{
protected: // �������л�����
	CVisionProjectDoc();
	DECLARE_DYNCREATE(CVisionProjectDoc)

// ����
public:
public:
	int CaculateDetectData();
	scPTGrab2* m_scPTGrab2;
	Image m_rawImage;
	static Mat m_imageSrc;

	TNetWork tNetWork;
	TNetWork *pTNetWork;
	DoubleCrane m_DoubleCrane;
	Mat GetIndexImage(UINT iIndex);
public:
	bool bUpdateForm;
public:
	void SendInfo2Robot();
	void GrapInit();
	void GrapExit();
	void RedrawAllViews();
	void ImageProcess(unsigned char* pImgData, long lWidth, long lHeight);
	static void WINAPI GrabDisplayCallback(void* pOwner, PTStatus status, unsigned char* pImgData, long lWidth, long lHeight, PTPixelFormat pixelFormat);
// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CVisionProjectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnCameraGrab();
	afx_msg void OnCom();
	afx_msg void OnCalibration();
	afx_msg void OnDetObjectPar();
	afx_msg void On32784();
};
