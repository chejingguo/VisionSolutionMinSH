#pragma once


// CVisionCalibration 对话框

class CVisionCalibration : public CDialog
{
	DECLARE_DYNAMIC(CVisionCalibration)

public:
	CVisionCalibration(CWnd* pParent,Mat image);   // 标准构造函数
	virtual ~CVisionCalibration();

	Mat img;
	void MatToCImage(Mat &mat, CImage &cImage);
	void DrawImage(Mat &img, UINT ID);
// 对话框数据
	enum { IDD = IDD_DLG_CALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
