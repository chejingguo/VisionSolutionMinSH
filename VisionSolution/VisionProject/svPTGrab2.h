// scPTGrab.h: interface for the scPTGrab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_scPTGrab_H__A02A8D75_C6EE_4704_A17F_9B0A27B3447D__INCLUDED_)
#define AFX_scPTGrab_H__A02A8D75_C6EE_4704_A17F_9B0A27B3447D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlyCapture2.h"
#include  <memory>
using namespace FlyCapture2;

#pragma pack(push,8)


//
#include"DoubleCrane.h"
#include"NetWork.h"
//
#include"DetectData.h"
#include"log.h"
struct detInfo
{
	int   iIndex;
	int   itype;
	float x;
	float y;
	float rz;
};


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
}PTCameraInfo, *pPTCameraInfo ;

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
		m_mode			= MODE_0;		
		m_uiStartX		= 0;       
		m_uiStartY		= 0;		
		m_uiWidth		= 0;        
		m_uiHeight		= 0;		
		m_fBusSpeed		= 100.0;       
		m_pixelFormat	= PIXEL_FORMAT_MONO8;   
	}
}PTFormat7Info, *pPTFormat7Info ;

// 采集图像模式
typedef enum tagGrabImageMode
{
	eWaitEvent = 0,                 // 内部事件触发
	eGrabThread					    // 线程方式	
}PTGrabImageMode;

// 相机触发模式
typedef enum tagPTTriggerMode
{
	ePTNotTrigger		= -1,		// 非触发模式
	ePTSoftwareTrigger	= 0,		// 软件触发模式 
	ePTHardwareTrigger	= 1			// 硬件触发模式	
} PTTriggerMode;	

// 像素格式
typedef enum tagPTPixelFormat
{
	ePTUnknown	= -1,				// 未知
	ePTGrey8	= 0,				// 8位灰度图像
	ePTRGB24	= 1,				// 24位彩色图像
	ePTRGBA32	= 2					// 32位彩色图像
} PTPixelFormat;

// 相机采集是否成功的状态标志
typedef enum tagPTStatus
{
	ePTGrabOK		= 0,			// 采集成功
	ePTGrabFailed	= 1,			// 采集失败
	ePTGrabTimeout	= 2,			// 采集超时
} PTStatus;

// 相机采集线程工作模式
typedef enum tagPTGrabThreadStatus
{
	ePTIdle = -1,					// 空闲
	ePTSnap = 0,					// 单帧
	ePTGrab							// 连续
} PTGrabThreadStatus;

// 控制命令
typedef enum tagCtrlCommand
{
	READY = 0,	   // 
	STARTGRAB = 1, //
	STARTSNAP,
	STARTHARD,
	STARTSOFT,
	STOP,
	RESET

}CtrlCommand;
// 控制命令

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
										 ,long lWidth, long lHeight, PTPixelFormat pixelFormat);
//
// FlyCapture2.0采集类
//
class scPTGrab2  
{
public:
	// 构造函数，需设置采集模式，如果grabImageMode为eWaitEvent，内部不用创建线程；
	// 如果grabImageMode为eGrabThread，内部会创建线程来采集图像；
	scPTGrab2(TNetWork* net=NULL,PTGrabImageMode grabImageMode = eGrabThread);
	virtual ~scPTGrab2();

public:	
	////////////////////////////////////设置回调函数////////////////////////////////
	void SetOwner(void* pOwner, PTGrabbedCallback funcCallback);							// 设置调用者和回调函数
	void SetSendCallbackEnable(bool bEnable);												// 设置是否调用回调函数
	bool GetSendCallbackEnable();															// 获取是否调用回调函数
	
	////////////////////////////////////相机初始化/////////////////////////////////
	bool InitFromSerial(unsigned int uiSerialNum);											// 根据相机序列号初始化相机
	bool InitFromIndex(unsigned int nCamIndex);												// 根据索引号初始化相机
	bool InitFromGuid(PGRGuid *pGuid);														// 根据相机Guid初始化相机
	bool IsOnline();																		// 是否连接相机
	bool Destroy();																			// 断开相机连接
	void CameraInit();
	////////////////////////////////////采集操作///////////////////////////////////
	bool Grab();						
	bool Freeze();																			// 结束连续采集，需要已经创建采集线程
	bool IsGrabbing();																		// 获取是否处于连续采集状态
	bool SetGrabTimeout(unsigned long ulTimeout);											// 设置采集延时
	// 采集单帧图像（只支持FreeRun模式）。注：需要已经创建采集线程；
	// 如果bWaitSnapped=true, 等待直到采集完成才返回；
	// 如果bWaitSnapped=false, 调用后不等待采集完直接返回；
	bool Snap(bool bWaitSnapped = false);
	bool IsAcquireSuccess();																// 图像是否采集成功
	bool Start();																			// 控制相机开始采集图像，不需要创建采集线程
	bool Stop();																			// 控制相机停止采集图像，不需要创建采集线程
	//////////////////////////////////////////////////////////////////////////
	// 采集单帧图像；
	// 该采集函数不能与上述其他采集函数同时调用；
	// 该采集函数直到采集完成后退出；
	// 该函数不依赖于采集线程；
	bool SnapImage();																		// 直接采集一帧图像

	/////////////////////////////////触发设置//////////////////////////////////////
	// 设置相机的触发模式(软触发、硬触发、或是非触发)。注：当设置相机为软触发和硬触发时，需要已经创建采集线程
	bool SetTriggerMode(PTTriggerMode mode, unsigned long ulTimeout = 2000);                   
	PTTriggerMode  GetTriggerMode();														// 获取当前的触发模式	
	bool CloseTriggerMode();																// 关闭触发模式
	bool IsTrigger();																		// 是否以触发方式（软触发或硬触发）采集图像
	// 发送软触发信号采集单帧图像，利用连续采集获取单帧图像
	// 发送完软触发信号后，该函数立即返回
	// 采集一帧图像成功后，调用回调函数
	bool SendSoftTrigger();																	// 发送软触发信号
	// 发送软触发信号采集单帧图像，利用连续采集获取单帧图像
	// 发送完软触发信号后，该函数立即返回
	// 采集一帧图像成功后，调用回调函数
	bool TriggerSnap();																		// 触发采集一帧图像
	bool CheckTriggerReady();																// 查询并等待，直到相机可以进行触发
	bool CheckSoftwareTriggerPresence(); 													// 查询相机是否有软触发采集

	///////////////////////////////相机图像属性相关////////////////////////////////
	PTPixelFormat GetImagePixelFormat();													// 获取图像的像素格式
	long GetImageWidth();																	// 获取图像的宽
	long GetImageHeight();																	// 获取图像的高
	long GetImageDataSize();																// 获取存储图像所需的内存大小（字节）	
	bool GetImageData(unsigned char* pImage);												// 获取图像数据，要求外部已分配好内存， 内存的大小可由GetImageDataSize()获得；
// 	bool GetImage(scImageGray& image, long alignModulus = 1);								// 获取灰度图像，如果像素格式为ePTRGB24则返回false
// 	bool GetImage(scImageRGB& image, long alignModulus = 1);								// 获取RGB图像，如果像素格式为ePTGrey8则返回false
	
	/////////////////////////////////1394相机普通视频模式设置//////////////////////
	bool IsVideoModeSupported(VideoMode vmVideoMode);										// 查询1394相机是否支持给定的视频模式
	bool IsFrameRateSupported(FrameRate frFrameRate);										// 查询1394相机在当前视频模式下，是否支持给定的帧率
	bool SetCurVideoMode(VideoMode vmVideoMode);											// 设置1394相机视频模式：只支持Y8和RGB模式；可能需要改变当前帧率
	bool GetCurVideoMode(VideoMode* pvmVideoMode);											// 获取1394相机视频模式
	bool SetCurFrameRate(FrameRate frFrameRate);											// 设置1394相机帧率，可能需要改变最大最小曝光时间
	bool GetCurFrameRate(FrameRate* pfrFrameRate);											// 获取1394相机当前的帧率
	bool GetMaxFrameRate(FrameRate* pfrFrameRate);											// 获取1394相机在当前视频模式下，支持的最大帧率
	
	///////////////////////////////Format7视频模式设置/////////////////////////////
	bool GetFormat7Config(PTFormat7Info &format7Info);										// 获取Format7模式下的参数					
	bool SetFormat7Config(PTFormat7Info format7Info);										// 设置Format7模式下的参数

	/////////////////////////////////相机属性相关//////////////////////////////////
	// 帧率设置
	bool SetCurFrameRate(float fFrameRate);													// 设置帧率，可能需要改变最大最小曝光时间
	bool GetCurFrameRate(float *pfFrameRate);												// 获取当前帧率
	bool GetFrameRateRange(float *fMinFrameRate, float *fMaxFrameRate);						// 获取相机帧率范围
	// 曝光时间设置
	bool SetCurShutter(float fShutter);														// 设置曝光时间
	bool GetCurShutter(float* pfShutter);													// 获取当前的曝光时间
	bool GetShutterRange(float *pfMinShutter, float *pfMaxShutter);							// 获取曝光时间范围
	// 增益设置
	bool SetCurGain(float fGain);															// 设置增益
	bool GetCurGain(float* pfGain);															// 获取当前增益
	bool GetGainRange(float *pfMinGain, float *pfMaxGain);									// 获取增益范围
	
public:	
	////////////////////////////////////获取相机信息///////////////////////////////
	bool GetCameraInfo(CameraInfo* pCamInfo);												// 获取相机的相关信息
	bool GetCameraSerialNumber(unsigned int* pSerialNumber);								// 获取相机的序列号
	bool GetCameraIndex(unsigned int* pCamIndex);											// 获取相机索引值

	///////////////////////////////////相机参数////////////////////////////////////
	bool SaveToCamera(long lChannel = 1);													// 保存到相机中
	bool SetBusSpeed(BusSpeed  asyncBusSpeed, BusSpeed  isochBusSpeed);						// 设置异步和同步总线速度
	bool GetBusSpeed(BusSpeed* pAsyncBusSpeed, BusSpeed* pIsochBusSpeed);					// 获取异步和同步总线速度
	bool WriteRegisiter(unsigned int uiAdd, unsigned int uiValue);							// 写寄存器
	bool ReadRegisiter(unsigned int uiAdd, unsigned int *uiValue);							// 读寄存器			
	PTCameraType  GetCameraType();															// 获得相机种类,1394、USB或GigE
	// 设置brightness、exposure、shutter、gain为手动状态，一般为内部调用
	bool SetStatusManual();                        

	//////////////////////////////////////公共函数//////////////////////////////////
	static bool RescanBus();																// 重新扫描总线，当前相机连接会被中断
	static bool GetBusCameraCount(unsigned int* pCamCount);									// 获取总线上相机的数量
	static bool EnumerateBusCameras(PGRGuid* pGuid, unsigned int* pSize);					// 枚举总线上的可用相机及其Guid
	static bool EnumerateBusCameras(unsigned int* pSerialInfo, unsigned int* pSize);		// 枚举总线上的可用相机及其序列号
	static bool EnumerateBusCameras(PTCameraInfo* pCameraInfo, unsigned int* pSize);		// 枚举总线上的可用相机及其信息
	static bool GetCameraGuidFromIndex(int iIndex, PGRGuid *pGuid);							// 获得相机的Guid(通过序号)
	static bool GetCameraGuidFromSerialNum(unsigned int pSerialNum, PGRGuid *pGuid);	    // 获得相机的Guid(通过序列号)
	static PTCameraType GetCameraTypeFromGuid(PGRGuid *pGuid);								// 判断相机类型(通过Guid)
	static PTCameraType GetCameraTypeFromIndex(unsigned int uiIndex);						// 判断相机类型(通过序号)
	static PTCameraType GetCameraTypeFromSerialNum(unsigned int uiSerialNum);				// 判断相机类型(通过序列号)
	static bool GetGigECameraIPAddressByGuid(PGRGuid *pGuid, IPAddress &ipAddress, 
		IPAddress &subnetMask, IPAddress &defaultGateway);									// 获得GigE相机IP地址(通过Guid)
	static bool GetGigECameraIPAddressByIndex(unsigned int uiIndex, IPAddress &ipAddress, 
		IPAddress &subnetMask, IPAddress &defaultGateway);									// 获得GigE相机IP地址(通过序号)
	static bool GetGigECameraIPAddressBySerialNum(unsigned int uiSerialNum, IPAddress &ipAddress, 
		IPAddress &subnetMask, IPAddress &defaultGateway);									// 获得GigE相机IP地址(通过序列号)
	static bool SetGigECameraIPAddressByGuid(PGRGuid *pGuid, IPAddress ipAddress, 
		IPAddress subnetMask, IPAddress defaultGateway);									// 设置GigE相机IP地址(通过Guid)
	static bool SetGigECameraIPAddressByIndex(unsigned int uiIndex, IPAddress ipAddress, 
		IPAddress subnetMask, IPAddress defaultGateway);									// 设置GigE相机IP地址(通过序号)
	static bool SetGigECameraIPAddressBySerialNum(unsigned int uiSerialNum, IPAddress ipAddress, 
		IPAddress subnetMask, IPAddress defaultGateway);									// 强制设置所有GigE相机IP地址
	static bool AutoForceAllGigECamerasIP();													// 重新设置总线上GigECamera相机IP

public:
	///////////////////////////////内部调用函数(采集相关)//////////////////////////
	bool TransferData();
	bool CreateGrabThread();																// 创建采集线程，一个采集对象只能有一个采集线程	
	bool TerminateGrabThread();																// 结束采集线程
	PTStatus AcquireImage();																// 从相机采集一幅图像，并进行相应的转换
	void SendCallback(PTStatus	status);													// 采集完成调用回调函数
    static void PTImageEventCallback(Image *pImage, const void *pCallbackData);				// 内部回调函数  
	static unsigned int GrabThread(LPVOID lpParam);	

	///////////////////////////////内部调用函数(1394视频模式相关)//////////////////
	// 将指定视频模式转为在相机寄存器中相应的索引值
	bool ConvertVideoModeToULongs(VideoMode vmVideoMode, unsigned long* pulFormat 
		,unsigned long* pulMode);														
	// 将指定帧率模式转为在相机寄存器中相应的索引值   
	bool ConvertFrameRateToULong(FrameRate frFrameRate
								  ,unsigned long* pulFrameRate);	
	bool ResetCurFrameRateByVideoMode();													// 根据相机的当前视频模式调整当前帧率
	bool ResetCurShutterByFrameRate();														// 根据相机的当前帧率调整曝光时间

public:
	CameraBase			*m_pCamera;															// 相机采集基类
	PTCameraType         m_cameraType;														// 相机种类，1394或GigE
	bool				 m_bOnline;															// 相机是否连接
	PTGrabImageMode      m_grabImageMode;													// 采集图像方式
	bool	             m_bIsGrabbing;														// 是否正在连续采集图像
	bool                 m_bIsSnap;															// 是否正在单帧采集图像
	PTGrabThreadStatus	 mLhreadStatus;													// 采集线程所处的工作模式
	HANDLE				 m_hEventSnapped;	
	bool                 m_bAcquireSuceess;                                     
	bool				 m_bFreezeStop;														// 是否为FreezeStop
	Image                m_ImageRaw;														// 相机采集原图像
	unsigned char*       m_pGrabImage;														// 采集到的图像数据
	long	             m_lWidth;															// 图像的宽
	long	             m_lHeight;															// 图像的高
	PTPixelFormat		 m_pixelFormat;														// 图像的像素格式				
	void*	             m_pMainFrm;														// 回调函数主窗口指针
	PTGrabbedCallback	 m_funcCallback;													// 回调函数	
	bool				 m_bSendCallbackEnable;												// 是否允许回调	
	PTTriggerMode		 m_nTriggerMode;													// 触发模式
	CRITICAL_SECTION	 m_criticalSection;													// 临界区，控制图像数据的读写	

	///////////////////////////线程方式采集////////////////////////////////////////
	CWinThread*	         m_pWinThread;														// 连续采集线程
	HANDLE				 m_hEventToRun;														// 采集线程处于暂停时，发出消息开始采集事件句柄
	HANDLE				 m_hEventExit;			  											// 子线程通知主线程已结束事件句柄
	bool				 m_bTerminate;														// 退出实时显示标志
	bool				 m_bHaveStopped;													// 是否已经退出实时采集显示

	bool                 m_bSendGrabTragger;
	int                  nBytes;					// 每个像素占用的字节数

	//image procees
	void DetectImage();
	CString infoHeader;
	CString infoContent;
	CString oneInfo;
	CString sendInfo;

	DoubleCrane m_DoubleCrane;
	static Mat         m_srcMat;

	HANDLE			   m_hEventToProcess;
	HANDLE			   m_hEventToUpdateUI;
	bool               CreateProcessThread();
	CWinThread*        m_pWinProcessThread;

	static UINT ProceesThread(LPVOID lpParam);
	vector<detInfo>	   detectInfo;
	vector<detInfo>	   detectInfoUI;
	detInfo            temDetInfo;
	TNetWork*          pNet;
	int                iIndexImage;
	//start,stop control
	bool               CreateStartStopThread();
	CWinThread*        m_pWinStartStopThread;
	static UINT StartStopThread(LPVOID lpParam);

	bool   m_bIsStartSuccess;
	bool   m_bSoftTraggerOn;
	void   StartGrab();
	void   StartSnap();
	void   StopGrab();
	void   StartSoftGrab();
	void   StartHardGrab();

public:
	static std::auto_ptr<BusManager>	m_AutoBusManager;									// 总线管理类
};

#pragma pack(pop)

#endif // !defined(AFX_scPTGrab_H__A02A8D75_C6EE_4704_A17F_9B0A27B3447D__INCLUDED_)
