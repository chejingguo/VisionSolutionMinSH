#pragma once
#include "Cvgdi.h"
class Cvgdi
{
public:
	Cvgdi();
	~Cvgdi();

private:
	static LPBYTE		m_bmpInfo;
	static INT			m_RefCounter;
private:
	Mat			        m_tmp;
private:
	void		InitBmpInfo();
	void		DeleteBmpInfo();
	void		RefreshBmpInfo(const cv::Mat &src);
	void		CreateTempMat(const Mat &src);
public:
	enum DrawMode
	{
		//ʹԴͼ��������������
		DM_RESIZESRCTOFIT,
		//����Դͼ������������������
		DM_KEEPSRCTOFIT,
		//���ϽǶ��룬���ֱ���
		DM_KEEPSRCALIANTOPLEFT,
		//�ߴ粻��,�м����
		DM_NORESIZEALIANCENTER,
		//�ߴ粻��,���ϽǶ���
		DM_NORESIZEALIANTOPLEFT
	};
public:
	RECT  CvRect2RectStruct(cv::Rect crt);

	Rect  RectStruct2CvRect(RECT rt);



	void		DrawMatToDC(HDC hdst, int xDst, int yDst, int DstWidth, int DstHeight
		, const cv::Mat &src, int xSrc, int ySrc, int SrcWidth, int SrcHeight);
	void		DrawMatToDC(HDC hdst, const LPRECT lpDst, const cv::Mat &src, DrawMode dm);
	void		DrawMatToWnd(HWND hWnd, const cv::Mat &src, DrawMode dm);
};

