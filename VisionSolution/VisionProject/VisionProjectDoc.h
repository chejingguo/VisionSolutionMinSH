
// VisionProjectDoc.h : CVisionProjectDoc 类的接口
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
protected: // 仅从序列化创建
	CVisionProjectDoc();
	DECLARE_DYNCREATE(CVisionProjectDoc)

// 特性
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
// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CVisionProjectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnCameraGrab();
	afx_msg void OnCom();
	afx_msg void OnCalibration();
	afx_msg void OnDetObjectPar();
	afx_msg void On32784();
};
