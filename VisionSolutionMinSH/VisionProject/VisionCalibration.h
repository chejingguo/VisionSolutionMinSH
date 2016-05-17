#pragma once


// CVisionCalibration �Ի���

class CVisionCalibration : public CDialog
{
	DECLARE_DYNAMIC(CVisionCalibration)

public:
	CVisionCalibration(CWnd* pParent,Mat image);   // ��׼���캯��
	virtual ~CVisionCalibration();

	Mat img;
	void MatToCImage(Mat &mat, CImage &cImage);
	void DrawImage(Mat &img, UINT ID);
// �Ի�������
	enum { IDD = IDD_DLG_CALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
