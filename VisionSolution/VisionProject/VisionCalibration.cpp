// VisionCalibration.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VisionProject.h"
#include "VisionCalibration.h"
#include "afxdialogex.h"


// CVisionCalibration �Ի���
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
	CImage image; //����ͼƬ��   
	//image.Load(rphoto); //����ͼƬ·������ͼƬ   

	MatToCImage(img, image);

	CRect rect;//���������   
	int cx = image.GetWidth();//��ȡͼƬ���   
	int cy = image.GetHeight();//��ȡͼƬ�߶�   

	GetDlgItem(ID)->GetWindowRect(&rect);//�����ھ���ѡ�е�picture�ؼ���   
	ScreenToClient(&rect);//���ͻ���ѡ�е�Picture�ؼ���ʾ�ľ���������   
	GetDlgItem(ID)->MoveWindow(rect.left, rect.top, cx, cy, TRUE);//�������ƶ���Picture�ؼ���ʾ�ľ�������   

	CWnd *pWnd = GetDlgItem(ID);//���pictrue�ؼ����ڵľ��   

	pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������   

	CDC *pDC = pWnd->GetDC();//���pictrue�ؼ���DC   

	//
	CRect rs;
	GetClientRect(&rs);  //

	CRect rect1(rs.left, rs.top, rs.Width() - rs.Width() / 4, rs.Height() * 4 / 5);

	image.StretchBlt(pDC->m_hDC, rect1, rect);

	//image.Draw(pDC->m_hDC, rect); //��ͼƬ����Picture�ؼ���ʾ�ľ�������   
	ReleaseDC(pDC);//�ͷ�picture�ؼ���DC 
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


// CVisionCalibration ��Ϣ�������
