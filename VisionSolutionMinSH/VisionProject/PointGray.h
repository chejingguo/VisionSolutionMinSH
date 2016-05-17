#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlyCapture2.h"
#include  <memory>

using namespace FlyCapture2;

#pragma pack(push,8)

// 相机接口类型
typedef enum tagPTCameraType
{
	eInterfaceUnknow = 0,		    // 未知接口
	eIEEE1394 = 1,                  // 1394接口
	eGigE,						    // GigE接口
	eUSB2,						    // USB2.0接口
	eUSB3						    // USB3.0接口
}PTCameraType;

// 相机信息
typedef struct tagPTCameraInfo
{
	unsigned int m_uiIndex;         // 序号
	unsigned int m_uiSerialNum;     // 序列号
	PTCameraType m_CameraType;      // 相机类型
	IPAddress	 m_ipAddress;	    // IP地址
	IPAddress    m_subnetMask;      // 子网掩码
	IPAddress    m_defaultGateway;  // 默认网关
	char		 m_cModelName[512];	// 相机型号
	char		 m_cResolution[512];// 相机分辨率
	tagPTCameraInfo()
	{
		m_uiIndex = -1;
		m_uiSerialNum = 0;
		memset(m_cModelName, 0, 512);
		memset(m_cResolution, 0, 512);
	}
}PTCameraInfo, *pPTCameraInfo;

// Format7格式
typedef struct tagPTFormat7Info
{
	Mode         m_mode;			// 模式
	unsigned int m_uiStartX;        // 起始X
	unsigned int m_uiStartY;		// 起始Y
	unsigned int m_uiWidth;         // 宽度
	unsigned int m_uiHeight;		// 高度
	float        m_fBusSpeed;       // 数据率
	PixelFormat  m_pixelFormat;     // 像素格式

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

// 采集图像模式
typedef enum tagGrabImageMode
{
	eWaitEvent = 0,                 // 内部事件触发
	eGrabThread					    // 线程方式	
}PTGrabImageMode;

// 相机触发模式
typedef enum tagPTTriggerMode
{
	ePTNotTrigger = -1,		// 非触发模式
	ePTSoftwareTrigger = 0,		// 软件触发模式 
	ePTHardwareTrigger = 1			// 硬件触发模式	
} PTTriggerMode;

// 像素格式
typedef enum tagPTPixelFormat
{
	ePTUnknown = -1,				// 未知
	ePTGrey8 = 0,				// 8位灰度图像
	ePTRGB24 = 1,				// 24位彩色图像
	ePTRGBA32 = 2					// 32位彩色图像
} PTPixelFormat;

// 相机采集是否成功的状态标志
typedef enum tagPTStatus
{
	ePTGrabOK = 0,			// 采集成功
	ePTGrabFailed = 1,			// 采集失败
	ePTGrabTimeout = 2,			// 采集超时
} PTStatus;

// 相机采集线程工作模式
typedef enum tagPTGrabThreadStatus
{
	ePTIdle = -1,					// 空闲
	ePTSnap = 0,					// 单帧
	ePTGrab							// 连续
} PTGrabThreadStatus;


// 回调函数类型声明，当相机采集完成后调用该回调函数；
// 参数说明：
//		pOwner:		主调用对象的指针，与SetOwner的输入参数pOwner一致；
//		status:		采集状态枚举：成功、出错或超时
//		pImage:		图像数据指针
//		lWidth:		图像的宽
//		lHeight:	图像的高
//		pixelFormat:图像的像素格式
// 注：
//		如果采集图像成功，则pImage、lWidth、lHeight和pixelFormat为图像相关的有效数据；
//		如果采集图像出错，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；
//		如果采集图像超时，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；

typedef void (WINAPI *PTGrabbedCallback)(void* pOwner, PTStatus status, unsigned char* pImage
	, long lWidth, long lHeight, PTPixelFormat pixelFormat);
//
// FlyCapture2.0采集类
//
class PointGray
{
public:
	PointGray();
	~PointGray();
};

