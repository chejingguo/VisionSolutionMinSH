#include "StdAfx.h"
#include "DoubleCrane.h"


DoubleCrane::DoubleCrane(void)
{
	contNumber=0;
	areaThresholdMin=3000;
	areaThresholdMax=100000;

	
	widthMin = 190;
	widthMax = 250;
	lengthMin = 260;//big 410 small 300
	lengthMax = 310;//big 480 small 400
	
	/*
	caliData.xRatio=0.5747;
	caliData.yRatio=0.5681;

	caliData.coodCenter.x=220*2;
	caliData.coodCenter.y=117*2;
	*/
	//che modify
	caliData.xRatio = 0.5988;
	caliData.yRatio = 0.5988;
	
	caliData.coodCenter.x = 390-54;// TS420 TP386
	caliData.coodCenter.y = 305 - 72;//TS325 TP288
	/*	
	caliData.xRatio = 0.5747;
	caliData.yRatio = 0.5681;
	caliData.coodCenter.x = 402;// TS420 TP386
	caliData.coodCenter.y = 324-55;//TS325 TP288
	*/
	cannyFirstPara = 80;
	cannySecondPara = 40;
}

DoubleCrane::~DoubleCrane(void)
{
}



int DoubleCrane::contProcess(Mat &contCompute)
{
	float area=0;
	int nRet=0;
	area = fabs(contourArea(contCompute));
	RotatedRect contRect = minAreaRect(contCompute);

	if(area<areaThresholdMax&&area>areaThresholdMin)
	{
		if((contRect.size.width>lengthMin&&contRect.size.width<lengthMax
			&&contRect.size.height>widthMin&&contRect.size.height<widthMax)
			||
			(contRect.size.width>widthMin&&contRect.size.width<widthMax
			&&contRect.size.height>lengthMin&&contRect.size.height<lengthMax)
			)
		{
			nRet=0;
		}
		else
		{
			nRet=1;
		}
	}
	else
		nRet =2;

	return nRet;

}


int DoubleCrane::contourSelect(vector<Mat> & contSelect,vector<Mat> &contRem)
{
	int nCont =0;
	contRem.clear();
	for(auto it=contSelect.begin();it!=contSelect.end();++it)
	{
		if(0==contProcess(*it))
		{
			contRem.push_back(*it);
			++nCont;

		}
	}
	return nCont;
}


cv::Mat DoubleCrane::RotationMatrix2D( Point2f center,double angle, double scale ,Point subColRow)
{
  angle *= CV_PI/180;
  double alpha = cos(angle)*scale;
  double beta = sin(angle)*scale;
 
  Mat M(2, 3, CV_64F);
  double* m = (double*)M.data;
 
  m[0] = alpha;
  m[1] = beta;
  m[2] = (1-alpha)*center.x - beta*center.y;
  m[3] = -beta;
  m[4] = alpha;
  m[5] = beta*center.x + (1-alpha)*center.y;

  m[2] += subColRow.x;    
  m[5] += subColRow.y; 
 
  return M;
}


int DoubleCrane::rotateImage(Mat& rotateImageSrc,Mat& rotateImageDst,float contAngle,Point2f rotateCenter)
{
	int nRet=0;

	float angle=0;
	if(contAngle<0)
		angle=90+contAngle;
	else angle=contAngle;
   
	//旋转后的图像长度，宽度
    double a = sin(angle*3.1415/180.0), b = cos(angle*3.1415/180.0);   
	int width = rotateImageSrc.cols;    
	int height = rotateImageSrc.rows; 

    int width_rotate= int(height * fabs(a) + width * fabs(b));    
    int height_rotate=int(width * fabs(a) + height * fabs(b));    
    // 旋转中心
	Point subColRow;
	subColRow.x= (width_rotate - width) / 2;
	subColRow.y=(height_rotate - height) / 2;
	//mapMatrix.data[2] += (width_rotate - width) / 2;    
	//mapMatrix.data[5] += (height_rotate - height) / 2; 

	Mat mapMatrix =RotationMatrix2D(rotateCenter,angle,1.0,subColRow);
	rotateImageDst=Mat::zeros(Size(width_rotate,height_rotate),CV_8UC1);

    warpAffine( rotateImageSrc,rotateImageDst,mapMatrix,Size(width_rotate,height_rotate));    

	return 0;
}


Point DoubleCrane::truncateImage(Mat& truncateImageSrc,Mat& truncateImageDst,RotatedRect& rectRot)
{
	Point pointRet;
	Point2f pts[4];
	rectRot.points(pts);
	//计算截取点的坐标
	Point2f LeftTop;
	Point2f RightButtom;
	LeftTop.x=RightButtom.x=pts[0].x;
	LeftTop.y=RightButtom.y=pts[0].y;
	for(int i=1;i<4;i++)
	{
		if(pts[i].x<LeftTop.x)
			LeftTop.x=pts[i].x;
		if(pts[i].y<LeftTop.y)
			LeftTop.y=pts[i].y;
		if(pts[i].x>RightButtom.x)
			RightButtom.x=pts[i].x;
		if(pts[i].y>RightButtom.y)
			RightButtom.y=pts[i].y;
	}
	if(LeftTop.x<0)LeftTop.x=0;
	if(LeftTop.y<0)LeftTop.y=0;
	if(RightButtom.x>truncateImageSrc.cols)RightButtom.x=truncateImageSrc.cols;
	if(RightButtom.y>truncateImageSrc.rows)RightButtom.y=truncateImageSrc.rows;
	pointRet.x=LeftTop.x;
	pointRet.y=LeftTop.y;

	Size size=Size((int)((int)RightButtom.x-(int)LeftTop.x),(int)((int)RightButtom.y-(int)LeftTop.y));
	Rect roiRect=Rect((int)LeftTop.x,(int)LeftTop.y,size.width, size.height);
	truncateImageDst=truncateImageSrc(roiRect);
	
	return pointRet;
}

int DoubleCrane::directionCaculate(Mat& dirImage)
{
	int nDir=5;
	int imCols=dirImage.cols;
	int imRows=dirImage.rows;

	int nCornerMass[4];//0--左上，1--右上，2--左下，3--右下
	for(int i=0;i<4;++i)
	{
		nCornerMass[i]=0;
	}

//	int ltMass=0;
//	int lbMass=0;
//	int rtMass=0;
//	int rbMass=0;

	if(imCols>imRows)
	{
		int calCols=imCols/12;
		int calRows=imRows/4;

		for(int i=0;i<imRows;++i)
		{
			uchar* data = dirImage.ptr<uchar>(i);
			for(int j=0;j<imCols;j++)
			{
				if(i<calRows&&j<calCols)
				{
					if(data[j])
						nCornerMass[0]+=1;
				}
				if(i<calRows&&j>imCols-calCols)
				{
					if(data[j])
						nCornerMass[1]+=1;
				}
				if(i>imRows-calRows&&j<calCols)
				{
					if(data[j])
						nCornerMass[2]+=1;
				}
				if(i>imRows-calRows&&j>imCols-calCols)
				{
					if(data[j])
						nCornerMass[3]+=1;
				}			
			}
		}

		int maxCornerMass,minCornerMass;
		int	maxFlag,minFlag;
		minCornerMass=nCornerMass[0];
		minFlag=0;
		maxCornerMass=0;

		for (int i= 0; i < 4; i++)
		{
			if (nCornerMass[i]>maxCornerMass)
			{
				maxCornerMass>nCornerMass[i];
				maxFlag=i;
			}
			if(nCornerMass[i]<minCornerMass)
			{
				minCornerMass=nCornerMass[i];
				minFlag=i;
			}
		}

//		if(ltMass+lbMass>rtMass+rbMass)
		if(maxFlag==0||maxFlag==2)
		{
			nDir=4;
		}
		else if(maxFlag==1||maxFlag==3)
		{
			nDir=2;
		}
		else
			nDir=5;
	}
	else
	{
		int calCols=imCols/4;
		int calRows=imRows/12;

		for(int i=0;i<imRows;++i)
		{
			uchar* data = dirImage.ptr<uchar>(i);
			for(int j=0;j<imCols;j++)
			{
				if(i<calRows&&j<calCols)
				{
					if(data[j])
				//		ltMass+=1;
					nCornerMass[0]+=1;
				}
				if(i<calRows&&j>imCols-calCols)
				{
					if(data[j])
					//	rtMass+=1;
					nCornerMass[1]+=1;
				}
				if(i>imRows-calRows&&j<calCols)
				{
					if(data[j])
				//		lbMass+=1;
					nCornerMass[2]+=1;
				}
				if(i>imRows-calRows&&j>imRows-calCols)
				{
					if(data[j])
				//		rbMass+=1;
					nCornerMass[3]+=1;
				}
				
			}
		}



		int maxCornerMass,minCornerMass;
		int	maxFlag,minFlag;
		minCornerMass=nCornerMass[0];
		minFlag=0;
		maxCornerMass=0;

		for (int i= 0; i < 4; i++)
		{
			if (nCornerMass[i]>maxCornerMass)
			{
				maxCornerMass>nCornerMass[i];
				maxFlag=i;
			}
			if(nCornerMass[i]<minCornerMass)
			{
				minCornerMass=nCornerMass[i];
				minFlag=i;
			}
		}


///		if(ltMass+rtMass<lbMass+rbMass)
		if(maxFlag==2||maxFlag==3)
		{
			nDir=1;
		}
		else if(maxFlag==1||maxFlag==0)
		{
			nDir=3;
		}
		else
			nDir=5;
	}

	return nDir;
}


int DoubleCrane::locationCaculate(objectInfo& object)
{
	int nRet=0;
	int nDirect = directionCaculate(object.objectTruncatedImage);
	CLOG::Out0(L"grab2", L"IngetObjectInfo %d", nDirect);

	CLOG::Out0(L"grab2", L"IngetObjectInfo %f", object.objectRotRect.angle);

	if (object.objectRotRect.angle == 0)
	{
		switch (nDirect)
		{
		case 1:
			object.objLoca.locationAngle = 90;
			break;
		case 2:
			object.objLoca.locationAngle = 180;//
			break;
		case 3:
			object.objLoca.locationAngle = 270;//
			break;
		case 4:
			object.objLoca.locationAngle = 0;//
			break;
		case 5:
			nRet = -1;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (nDirect)
		{
		case 1:
			object.objLoca.locationAngle = -object.objectRotRect.angle;
			break;
		case 2:
			object.objLoca.locationAngle = 90 - object.objectRotRect.angle;//
			break;
		case 3:
			object.objLoca.locationAngle = 180 - object.objectRotRect.angle;//
			break;
		case 4:
			object.objLoca.locationAngle = 270 - object.objectRotRect.angle;//
			break;
		case 5:
			nRet = -1;
			break;
		default:
			break;
		}
	}


	CLOG::Out0(L"grab2", L"IngetObjectInfo %f", object.objLoca.locationAngle);

	if(nRet==0)
	{
		float rx = 20 * cos(object.objLoca.locationAngle / 180.0*3.1415);
		float ry = 20 * sin(object.objLoca.locationAngle / 180.0*3.1415);
		object.objLoca.locationCenter.x = (object.objectRotRect.center.x-caliData.coodCenter.x)*caliData.xRatio-rx;
		object.objLoca.locationCenter.y = -((object.objectRotRect.center.y - caliData.coodCenter.y)*caliData.yRatio+ry);
	}
	return nRet;
}

void DoubleCrane::drawArrow(Mat& imSrc,Point pStart, int arrowLen, double arrowAngle,int smallLineLen,
	int smallLineAngle,Scalar& color, int thickness ,int lineType)
{
	const double PI =3.1415926;
	Point pEnd;
	pEnd.x=pStart.x+arrowLen*cos(arrowAngle*PI/180.0);
	pEnd.y=pStart.y-arrowLen*sin(arrowAngle*PI/180.0);
	line(imSrc,pStart,pEnd,color,thickness,lineType);

	Point arrow;
	arrow.x = pEnd.x + smallLineLen * cos((arrowAngle +	smallLineAngle+180)*PI/180.0);
	arrow.y = pEnd.y - smallLineLen * sin((arrowAngle + smallLineAngle+180)*PI/180.0);
	line(imSrc,pEnd,arrow,color,thickness,lineType);
	arrow.x = pEnd.x + smallLineLen * cos((arrowAngle -	smallLineAngle+180)*PI/180.0);
	arrow.y = pEnd.y - smallLineLen * sin((arrowAngle - smallLineAngle+180)*PI/180.0);
	line(imSrc,pEnd,arrow,color,thickness,lineType);
}

void DoubleCrane::drawObjectRect(Mat& imSrc)
{
	for(auto it=objectInfoVect.begin();it!=objectInfoVect.end();++it)
	{
		Point2f vertices[4];
		it->objectRotRect.points(vertices);
		for(int i=0;i<4;++i)
		{
			line(imSrc,vertices[i],vertices[(i+1)%4],Scalar(0,255,0));
		}
		drawArrow(imSrc,it->objectRotRect.center,60,it->objLoca.locationAngle,10,30,Scalar(0,255,0),2);
	}
}


int DoubleCrane::getObjectInfo(Mat& imSrc,vector<Mat>& contCompute)
{


	for(auto it=contCompute.begin();it!=contCompute.end();++it)
	{	
		RotatedRect rectTemp = minAreaRect(*it);
		if(rectTemp.angle==-90||rectTemp.angle==0)
		{
			if (omitEdgeConnectContour(imSrc, *it))
			{
				contNumber--;
				continue;
			}
			else
			{
				Mat imTruncateZero;
				Point LeftTop = truncateImage(imSrc, imTruncateZero, rectTemp);

				objectInfo objectInfoTemp;
				objectInfoTemp.objectRotRect = RotatedRect(rectTemp);//保存旋转前的位置信息
				objectInfoTemp.objectTruncatedImage = imTruncateZero;
				objectInfoVect.push_back(objectInfoTemp);
			}
		}
		else
		{
			Mat imTruncateFirst;
			Point LeftTop = truncateImage(imSrc, imTruncateFirst, rectTemp);


#ifdef IMAGE_SHOW
			namedWindow("imTruncateFirst", CV_WINDOW_AUTOSIZE);
			imshow("imTruncateFirst", imTruncateFirst);
#endif
			Mat imRotateImage;
			rotateImage(imTruncateFirst, imRotateImage, rectTemp.angle,
				Point2f(rectTemp.center.x - LeftTop.x, rectTemp.center.y - LeftTop.y));//第一次旋转图像，将物体旋转到平行位置
#ifdef IMAGE_SHOW
			namedWindow("imRotateImage", CV_WINDOW_AUTOSIZE);
			imshow("imRotateImage", imRotateImage);
#endif
			Mat imageCont;
			imageCont = imRotateImage.clone();
			conts.clear();
			//vector<Mat> conts;
			findContours(imageCont, conts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			if (false == conts.empty())
			{
				Mat objectCont;
				for (auto it2 = conts.begin(); it2<conts.end(); ++it2)
				{
					int areaTemp2 = areaThresholdMin;
					int area2 = fabs(contourArea(*it2));
					if (area2>areaTemp2)
					{
						objectCont = *it2;
					}
				}

				RotatedRect rectTemp2 = minAreaRect(objectCont);
				Mat imTruncateSecond;
				truncateImage(imRotateImage, imTruncateSecond, rectTemp2);
#ifdef IMAGE_SHOW
				namedWindow("imTruncateSecond", CV_WINDOW_AUTOSIZE);
				imshow("imTruncateSecond", imTruncateSecond);
#endif
				objectInfo objectInfoTemp;
				objectInfoTemp.objectRotRect = RotatedRect(rectTemp);//保存旋转前的位置信息
				objectInfoTemp.objectTruncatedImage = imTruncateSecond;
				objectInfoVect.push_back(objectInfoTemp);
			}

		}

	}

	vector<objectInfo>		objectInfoVectTemp;

	for(auto it=objectInfoVect.begin();it<objectInfoVect.end();++it)
	{
		int ret = locationCaculate(*it);
		if(ret==0)
		{
			objectInfo objectInfoTemp;
			objectInfoTemp.objectRotRect=it->objectRotRect;
			objectInfoTemp.objectTruncatedImage=it->objectTruncatedImage;
			objectInfoTemp.objLoca.locationAngle=it->objLoca.locationAngle;
			objectInfoTemp.objLoca.locationCenter=it->objLoca.locationCenter;
			objectInfoVectTemp.push_back(objectInfoTemp);
		}
		else
		{
			contNumber--;
		}
	}

	objectInfoVect.clear();
	for (auto it=objectInfoVectTemp.begin(); it!=objectInfoVectTemp.end(); ++it)
	{
			objectInfo objectInfoTemp;
			objectInfoTemp.objectRotRect=it->objectRotRect;
			objectInfoTemp.objectTruncatedImage=it->objectTruncatedImage;
			objectInfoTemp.objLoca.locationAngle=it->objLoca.locationAngle;
			objectInfoTemp.objLoca.locationCenter=it->objLoca.locationCenter;
			objectInfoVect.push_back(objectInfoTemp);
	}

	return 0;
}


bool DoubleCrane::omitEdgeConnectContour(Mat& imageSrc,Mat& ContCheek)
{


	bool nRet = false;

	int nRows = imageSrc.rows;
	int nCols = imageSrc.cols;

	for (int i = 0; i < nRows; i++)
	{
		uchar* data = imageSrc.ptr<uchar>(i);
		for (int j = 0; j < nCols; j++)
		{
			if (i == 0 || i == nRows - 1 || j == 0 || j == nCols - 1)
			{
				if (data[j] == 255)
				{
					int dist = pointPolygonTest(ContCheek, Point2f(i, j), false);
					if (dist < 2)
						nRet = true;
				}
			}
		}
	}
	return nRet;
}


int DoubleCrane::imageProcess(Mat& imageSrc)
{
	//清除数据
	objectInfoVect.clear();
	contNumber=0;

	if(imageSrc.empty())
	{
		MessageBox(NULL,_T("传输图像为空！"),NULL,NULL);
		return 1;
	}
	Mat image;

	if(imageSrc.channels()==3)
	{
		cvtColor(imageSrc,image,CV_BGR2GRAY);
	}
	else if(imageSrc.channels()==1)
	{
		image=imageSrc.clone();
	}
	else 
	{
		return 0;
	}
#ifdef IMAGE_SHOW
	namedWindow("Gray", CV_WINDOW_AUTOSIZE);
	imshow("Gray",image);
#endif
	Mat imageFliter;
	medianBlur(image,imageFliter,3);
	Mat imageCanny;
	Canny(imageFliter,imageCanny,cannyFirstPara,cannySecondPara);
//	imwrite("ImageCanny.bmp",imageCanny);
	//imwrite("ImageCanny.bmp",imageCanny);

#ifdef IMAGE_SHOW
	namedWindow("ImageCanny", CV_WINDOW_AUTOSIZE);
	imshow("ImageCanny",imageCanny);
#endif

	Mat imageDilate;
	Mat kernelElement = getStructuringElement(MORPH_RECT,Size(5,5),Point(0,0));
	//Mat kernelElement = getStructuringElement(MORPH_RECT,Size(2,2),Point(0,0));
	dilate(imageCanny,imageDilate,kernelElement);
	//imwrite("imageDilate.bmp",imageDilate);
#ifdef IMAGE_SHOW
	namedWindow("imageDilate", CV_WINDOW_AUTOSIZE);
	imshow("imageDilate",imageDilate);
#endif
	Mat imageContour;
	imageContour = imageDilate.clone();
	contours.clear();
	
	//vector<Mat> contours;
	findContours(imageContour,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	
	contNumber=0;
	//vector<Mat> contRemain;
	if(false==contours.empty())
	{
		contNumber = contourSelect(contours,contRemain);
	}
	else
	{
		return 0;
	}

	
	Mat imageDrawCont = Mat::zeros(imageContour.rows,imageContour.cols,CV_8UC1);
//	imageDrawCont.zeros(imageDrawCont.rows,imageDrawCont.cols,CV_8UC1);

 	drawContours(imageDrawCont,contRemain,-1,CV_RGB(255,255,255),3);
#ifdef IMAGE_SHOW
	namedWindow("imageCont",CV_WINDOW_AUTOSIZE);
	imshow("imageCont",imageDrawCont);
#endif

	CLOG::Out0(L"grab2", L"beforegetObjectInfo %f", 1.1);
	
	getObjectInfo(imageDrawCont,contRemain);

	CLOG::Out0(L"grab2", L"AftergetObjectInfo %f", 2.2);
 
	//画图
	drawObjectRect(imageSrc);
	
#ifdef IMAGE_SHOW
	namedWindow("imageSrc",CV_WINDOW_AUTOSIZE);
	imshow("imageSrc",imageSrc);
#endif
	
	return 0;
	
}