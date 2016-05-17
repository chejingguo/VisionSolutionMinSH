#include "stdafx.h"
#include "Cvgdi.h"


Cvgdi::Cvgdi()
{
	InitBmpInfo();
	m_RefCounter++;
}

Cvgdi::~Cvgdi()
{
	DeleteBmpInfo();
	if (!m_tmp.empty())
		m_tmp.release();
}

LPBYTE Cvgdi::m_bmpInfo = nullptr;
INT		Cvgdi::m_RefCounter = 0;

void Cvgdi::InitBmpInfo()
{
	m_bmpInfo = new BYTE[sizeof(BITMAPINFOHEADER)+256 * sizeof(RGBQUAD)];
	LPBITMAPINFOHEADER lpInfoHeader = (LPBITMAPINFOHEADER)m_bmpInfo;
	lpInfoHeader->biCompression = BI_RGB;
	lpInfoHeader->biClrImportant = 0;
	lpInfoHeader->biClrUsed = 0;
	lpInfoHeader->biPlanes = 1;
	lpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);

	LPRGBQUAD	lpRgbQuad = (LPRGBQUAD(m_bmpInfo + sizeof(BITMAPINFOHEADER)));
	for (int i = 0; i< 256; i++)
	{
		(lpRgbQuad + i)->rgbRed = i;
		(lpRgbQuad + i)->rgbGreen = i;
		(lpRgbQuad + i)->rgbBlue = i;
	}
}

void Cvgdi::DeleteBmpInfo()
{
	m_RefCounter--;
	if (m_RefCounter == 0 && m_bmpInfo != nullptr)
	{
		delete[] m_bmpInfo;
		m_bmpInfo = nullptr;
	}


}

void Cvgdi::CreateTempMat(const Mat &src)
{
	if (!m_tmp.empty())
		m_tmp.release();
	m_tmp = Mat::zeros(src.rows, (src.cols + 3) / 4 * 4, src.type());
	for (int i = 0; i < src.rows; i++)
	{
		memcpy(m_tmp.ptr<unsigned char>(i), src.ptr<unsigned char>(i), src.cols*src.elemSize());
	}
	RefreshBmpInfo(m_tmp);
}

void Cvgdi::RefreshBmpInfo(const cv::Mat &src)
{
	LPBITMAPINFOHEADER lpInfoHeader = (LPBITMAPINFOHEADER)m_bmpInfo;
	lpInfoHeader->biBitCount = src.channels() * 8;
	lpInfoHeader->biWidth = src.cols;
	lpInfoHeader->biHeight = -src.rows;
	lpInfoHeader->biSizeImage = src.rows*src.cols;
}

void Cvgdi::DrawMatToDC(HDC hdst, int xDst, int yDst, int DstWidth, int DstHeight
	, const cv::Mat &src, int xSrc, int ySrc, int SrcWidth, int SrcHeight)
{
	CreateTempMat(src);
	::SetStretchBltMode(hdst, COLORONCOLOR);
	::StretchDIBits(hdst, xDst, yDst, DstWidth, DstHeight
		, xSrc, ySrc, SrcWidth, SrcHeight, m_tmp.data, (LPBITMAPINFO)m_bmpInfo, DIB_RGB_COLORS, SRCCOPY);
}

void Cvgdi::DrawMatToDC(HDC hdst, const LPRECT lpDst, const cv::Mat &src, DrawMode dm)
{
	if (&src == NULL || src.dims <= 0)
		return;
	CreateTempMat(src);
	int		xSrc(0);
	int		ySrc(0);
	int		SrcWidth = src.cols;
	int		SrcHeight = src.rows;
	int		xDst = lpDst->left;
	int		yDst = lpDst->top;
	int		DstWidth = lpDst->right - lpDst->left;
	int		DstHeight = lpDst->bottom - lpDst->top;
	bool	bFullWidth = ((float)SrcWidth / (float)DstWidth > (float)SrcHeight / (float)DstHeight) ? true : false;
	float	resizeFactor(1.0);
	switch (dm)
	{
	case Cvgdi::DM_RESIZESRCTOFIT:
		//д╛хо
		break;
	case Cvgdi::DM_KEEPSRCTOFIT:
		if (bFullWidth)
		{
			resizeFactor = float(SrcWidth) / float(DstWidth);
			DstHeight = static_cast<int>(SrcHeight / resizeFactor + 0.5f);
			yDst = (lpDst->bottom - lpDst->top - DstHeight) / 2;
		}
		else
		{
			resizeFactor = float(SrcHeight) / float(DstHeight);
			DstWidth = static_cast<int>(SrcWidth / resizeFactor + 0.5f);
			xDst = (lpDst->right - lpDst->left - DstWidth) / 2;
		}
		break;
	case Cvgdi::DM_KEEPSRCALIANTOPLEFT:
		if (bFullWidth)
		{
			resizeFactor = float(SrcWidth) / float(DstWidth);
			DstHeight = static_cast<int>(SrcHeight / resizeFactor + 0.5f);
		}
		else
		{
			resizeFactor = float(SrcHeight) / float(DstHeight);
			DstWidth = static_cast<int>(SrcWidth / resizeFactor + 0.5f);
		}
		break;
	case Cvgdi::DM_NORESIZEALIANCENTER:
		if (SrcWidth > DstWidth)
		{
			xSrc = (SrcWidth - DstWidth) / 2;
			SrcWidth = DstWidth;
		}
		else
		{
			xDst = (DstWidth - SrcWidth) / 2;
			DstWidth = SrcWidth;
		}
		if (SrcHeight > DstHeight)
		{
			ySrc = (SrcHeight - SrcHeight) / 2;
			SrcHeight = DstHeight;
		}
		else
		{
			yDst = (DstHeight - SrcHeight) / 2;
			DstHeight = SrcHeight;
		}
		break;
	case Cvgdi::DM_NORESIZEALIANTOPLEFT:
		if (SrcWidth> DstWidth)
		{
			SrcWidth = DstWidth;
		}
		else
		{
			DstWidth = SrcWidth;
		}
		if (SrcHeight > DstHeight)
		{
			SrcHeight = DstHeight;
		}
		else
		{
			DstHeight = SrcHeight;
		}
		break;
	default:
		break;
	}
	::SetStretchBltMode(hdst, COLORONCOLOR);
	::StretchDIBits(hdst, xDst, yDst, DstWidth, DstHeight
		, xSrc, ySrc, SrcWidth, SrcHeight, m_tmp.data, (LPBITMAPINFO)m_bmpInfo, DIB_RGB_COLORS, SRCCOPY);
}

void Cvgdi::DrawMatToWnd(HWND hWnd, const cv::Mat &src, DrawMode dm)
{
	HDC		dc = ::GetDC(hWnd);
	RECT	clientRect;
	::GetClientRect(hWnd, &clientRect);
	DrawMatToDC(dc, &clientRect, src, dm);
	::ReleaseDC(hWnd, dc);
	dc = NULL;
}



RECT  Cvgdi::CvRect2RectStruct(cv::Rect crt)
{
	RECT rt;
	rt.left = crt.x;
	rt.top = crt.y;
	rt.right = crt.width + rt.left;
	rt.bottom = crt.height + rt.top;
	return rt;
}


Rect  Cvgdi::RectStruct2CvRect(RECT rt)
{
	cv::Rect crt = Rect(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
	return crt;
}