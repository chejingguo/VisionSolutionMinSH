#ifndef __DOUBLECRANE_H_
#define __DOUBLECRANE_H_
/*
#include <opencv2\opencv.hpp>
#include <cxcore.h>
#include <highgui.h>
#include <cv.h>
#include <vector>
#include <Windows.h>
*/
//#define IMAGE_SHOW 

#include "log.h"
using namespace cv;
using namespace std;

struct calibrateData
{
	double xRatio;
	double yRatio;
	Point coodCenter;
};


struct objectLocation
{
	Point2f				locationCenter;
	float				locationAngle;
};

struct objectInfo
{
	RotatedRect			objectRotRect;
	Mat					objectTruncatedImage;
	objectLocation		objLoca;
};



class DoubleCrane
{
public:
	DoubleCrane(void);
	~DoubleCrane(void);

public:
	int						areaThresholdMin;
	int						areaThresholdMax;
	int						widthMin;
	int						widthMax;
	int						lengthMin;
	int						lengthMax;

	int						contNumber;
	vector<objectInfo>		objectInfoVect;

private:
	int						cannyFirstPara;
	int						cannySecondPara;
	calibrateData			caliData;
	//
	// Critical section to protect access to the processed image
	CCriticalSection m_csData;

private:
	int				contProcess(Mat &contCompute);
	int				contourSelect(vector<Mat> & contSelect,vector<Mat> &contRem);
	cv::Mat			RotationMatrix2D( Point2f center,double angle, double scale ,Point subColRow);
	int				rotateImage(Mat& rotateImageSrc,Mat& rotateImageDst,float contAngle,Point2f rotateCenter);
	Point			truncateImage(Mat& truncateImageSrc,Mat& truncateImageDst,RotatedRect& rectRot);
	int				directionCaculate(Mat& dirImage);
	int				locationCaculate(objectInfo& object);
	void			drawArrow(Mat& imSrc,Point pStart, int arrowLen, double arrowAngle,int smallLineLen,int smallLineAngle,Scalar& color, int thickness =1 ,int lineType=8);
	void			drawObjectRect(Mat& imSrc);
	int				getObjectInfo(Mat& imSrc,vector<Mat>& contCompute);
	bool			omitEdgeConnectContour(Mat& imageSrc,Mat& ContCheek);

public:
	int				imageProcess(Mat& imSrc);
public:
	vector<Mat> contRemain;
	vector<Mat> contours;
	vector<Mat> conts;


};

#endif



