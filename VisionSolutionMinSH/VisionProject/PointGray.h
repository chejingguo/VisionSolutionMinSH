#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlyCapture2.h"
#include  <memory>

using namespace FlyCapture2;

#pragma pack(push,8)

// ����ӿ�����
typedef enum tagPTCameraType
{
	eInterfaceUnknow = 0,		    // δ֪�ӿ�
	eIEEE1394 = 1,                  // 1394�ӿ�
	eGigE,						    // GigE�ӿ�
	eUSB2,						    // USB2.0�ӿ�
	eUSB3						    // USB3.0�ӿ�
}PTCameraType;

// �����Ϣ
typedef struct tagPTCameraInfo
{
	unsigned int m_uiIndex;         // ���
	unsigned int m_uiSerialNum;     // ���к�
	PTCameraType m_CameraType;      // �������
	IPAddress	 m_ipAddress;	    // IP��ַ
	IPAddress    m_subnetMask;      // ��������
	IPAddress    m_defaultGateway;  // Ĭ������
	char		 m_cModelName[512];	// ����ͺ�
	char		 m_cResolution[512];// ����ֱ���
	tagPTCameraInfo()
	{
		m_uiIndex = -1;
		m_uiSerialNum = 0;
		memset(m_cModelName, 0, 512);
		memset(m_cResolution, 0, 512);
	}
}PTCameraInfo, *pPTCameraInfo;

// Format7��ʽ
typedef struct tagPTFormat7Info
{
	Mode         m_mode;			// ģʽ
	unsigned int m_uiStartX;        // ��ʼX
	unsigned int m_uiStartY;		// ��ʼY
	unsigned int m_uiWidth;         // ���
	unsigned int m_uiHeight;		// �߶�
	float        m_fBusSpeed;       // ������
	PixelFormat  m_pixelFormat;     // ���ظ�ʽ

	tagPTFormat7Info()
	{
		m_mode = MODE_0;
		m_uiStartX = 0;
		m_uiStartY = 0;
		m_uiWidth = 0;
		m_uiHeight = 0;
		m_fBusSpeed = 100.0;
		m_pixelFormat = PIXEL_FORMAT_MONO8;
	}
}PTFormat7Info, *pPTFormat7Info;

// �ɼ�ͼ��ģʽ
typedef enum tagGrabImageMode
{
	eWaitEvent = 0,                 // �ڲ��¼�����
	eGrabThread					    // �̷߳�ʽ	
}PTGrabImageMode;

// �������ģʽ
typedef enum tagPTTriggerMode
{
	ePTNotTrigger = -1,		// �Ǵ���ģʽ
	ePTSoftwareTrigger = 0,		// �������ģʽ 
	ePTHardwareTrigger = 1			// Ӳ������ģʽ	
} PTTriggerMode;

// ���ظ�ʽ
typedef enum tagPTPixelFormat
{
	ePTUnknown = -1,				// δ֪
	ePTGrey8 = 0,				// 8λ�Ҷ�ͼ��
	ePTRGB24 = 1,				// 24λ��ɫͼ��
	ePTRGBA32 = 2					// 32λ��ɫͼ��
} PTPixelFormat;

// ����ɼ��Ƿ�ɹ���״̬��־
typedef enum tagPTStatus
{
	ePTGrabOK = 0,			// �ɼ��ɹ�
	ePTGrabFailed = 1,			// �ɼ�ʧ��
	ePTGrabTimeout = 2,			// �ɼ���ʱ
} PTStatus;

// ����ɼ��̹߳���ģʽ
typedef enum tagPTGrabThreadStatus
{
	ePTIdle = -1,					// ����
	ePTSnap = 0,					// ��֡
	ePTGrab							// ����
} PTGrabThreadStatus;


// �ص���������������������ɼ���ɺ���øûص�������
// ����˵����
//		pOwner:		�����ö����ָ�룬��SetOwner���������pOwnerһ�£�
//		status:		�ɼ�״̬ö�٣��ɹ��������ʱ
//		pImage:		ͼ������ָ��
//		lWidth:		ͼ��Ŀ�
//		lHeight:	ͼ��ĸ�
//		pixelFormat:ͼ������ظ�ʽ
// ע��
//		����ɼ�ͼ��ɹ�����pImage��lWidth��lHeight��pixelFormatΪͼ����ص���Ч���ݣ�
//		����ɼ�ͼ�������pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��
//		����ɼ�ͼ��ʱ����pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��

typedef void (WINAPI *PTGrabbedCallback)(void* pOwner, PTStatus status, unsigned char* pImage
	, long lWidth, long lHeight, PTPixelFormat pixelFormat);
//
// FlyCapture2.0�ɼ���
//
class PointGray
{
public:
	PointGray();
	~PointGray();
};

