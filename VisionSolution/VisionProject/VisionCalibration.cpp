// VisionCalibration.cpp : 实现文件
//

#include "stdafx.h"
#include "VisionProject.h"
#include "VisionCalibration.h"
#include "afxdialogex.h"


// CVisionCalibration 对话框
void CVisionCalibration::MatToCImage(Mat &mat, CImage &cImage)
{
	//create new CImage  
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();

	cImage.Destroy(); //clear  
	cImage.Create(width,
		height, //positive: left-bottom-up   or negative: left-top-down  
		8 * channels); //numbers of bits per pixel  

	//copy values  
	uchar* ps;
	uchar* pimg = (uchar*)cImage.GetBits(); //A pointer to the bitmap buffer  

	//The pitch is the distance, in bytes. represent the beginning of   
	// one bitmap line and the beginning of the next bitmap line  
	int step = cImage.GetPitch();

	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) //gray  
			{
				*(pimg + i*step + j) = ps[j];
			}
			else if (channels == 3) //color  
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i*step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}

}

void CVisionCalibration::DrawImage(Mat &img, UINT ID)
{
	CImage image; //创建图片类   
	//image.Load(rphoto); //根据图片路径加载图片   

	MatToCImage(img, image);

	CRect rect;//定义矩形类   
	int cx = image.GetWidth();//获取图片宽度   
	int cy = image.GetHeight();//获取图片高度   

	GetDlgItem(ID)->GetWindowRect(&rect);//将窗口矩形选中到picture控件上   
	ScreenToClient(&rect);//将客户区选中到Picture控件表示的矩形区域内   
	GetDlgItem(ID)->MoveWindow(rect.left, rect.top, cx, cy, TRUE);//将窗口移动到Picture控件表示的矩形区域   

	CWnd *pWnd = GetDlgItem(ID);//获得pictrue控件窗口的句柄   

	pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   

	CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC   

	//
	CRect rs;
	GetClientRect(&rs);  //

	CRect rect1(rs.left, rs.top, rs.Width() - rs.Width() / 4, rs.Height() * 4 / 5);

	image.StretchBlt(pDC->m_hDC, rect1, rect);

	//image.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域   
	ReleaseDC(pDC);//释放picture控件的DC 
}

IMPLEMENT_DYNAMIC(CVisionCalibration, CDialog)

CVisionCalibration::CVisionCalibration(CWnd* pParent, Mat image)
	: CDialog(CVisionCalibration::IDD, pParent)
{
	img=image;
}

CVisionCalibration::~CVisionCalibration()
{
}

void CVisionCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVisionCalibration, CDialog)
END_MESSAGE_MAP()


// CVisionCalibration 消息处理程序
