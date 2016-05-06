// scPTGrab2.cpp: implementation of the scPTGrab2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "svPTGrab2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static VideoMode g_fvmFormatVideoMode[8][8] = 
{
	{
		VIDEOMODE_160x120YUV444,
			VIDEOMODE_320x240YUV422 ,
			VIDEOMODE_640x480YUV411,
			VIDEOMODE_640x480YUV422,
			VIDEOMODE_640x480RGB,
			VIDEOMODE_640x480Y8,
			VIDEOMODE_640x480Y16,
	},
	{
			VIDEOMODE_800x600YUV422 ,
				VIDEOMODE_800x600RGB,
				VIDEOMODE_800x600Y8,		
				VIDEOMODE_1024x768YUV422,
				VIDEOMODE_1024x768RGB,
				VIDEOMODE_1024x768Y8,
				VIDEOMODE_800x600Y16,
				VIDEOMODE_1024x768Y16
		},
		{
				VIDEOMODE_1280x960YUV422 ,
					VIDEOMODE_1280x960RGB,
					VIDEOMODE_1280x960Y8,		
					VIDEOMODE_1600x1200YUV422,
					VIDEOMODE_1600x1200RGB,
					VIDEOMODE_1600x1200Y8,
					VIDEOMODE_1280x960Y16,
					VIDEOMODE_1600x1200Y16
			},
			{
					VIDEOMODE_FORCE_32BITS
				},
				{
						VIDEOMODE_FORCE_32BITS,
					},
					{
							VIDEOMODE_FORCE_32BITS,
						},
						{
								VIDEOMODE_FORCE_32BITS,
							},
							{
									VIDEOMODE_FORMAT7,
								},
								
};

static FrameRate g_frFrameRate[] = 
{
		FRAMERATE_1_875,				// 1.875 fps. (Frames per second)
		FRAMERATE_3_75,					// 3.75 fps.
		FRAMERATE_7_5,					// 7.5 fps.
		FRAMERATE_15,					// 15 fps.
		FRAMERATE_30,					// 30 fps.
		FRAMERATE_60,					// 60 fps.
		FRAMERATE_120,					// 120 fps.
		FRAMERATE_240,					// 240 fps.
		FRAMERATE_FORMAT7               // Format7
};

static float g_fFrameRate[] = 
{
		1.875,
		3.75,
		7.5,
		15.0,
		30.0,
		60.0,
		120.0,
		240.0,
		-1
};

std::auto_ptr<BusManager> scPTGrab2::m_AutoBusManager = std::auto_ptr<BusManager>(NULL);
//DoubleCrane scPTGrab2::m_DoubleCrane;
Mat         scPTGrab2::m_srcMat;

scPTGrab2::scPTGrab2(TNetWork* net, PTGrabImageMode grabImageMode)
{
	m_grabImageMode				= grabImageMode;  
	m_pCamera					= NULL;	
	m_bOnline					= false;
	m_pMainFrm					= NULL;
	m_funcCallback				= NULL;
	m_bSendCallbackEnable		= true;	
	m_pGrabImage				= NULL;
	m_lWidth					= 0;
	m_lHeight					= 0;
	m_bIsGrabbing				= false;
	m_bIsSnap					= false;
	mLhreadStatus				= ePTIdle;
	m_bAcquireSuceess			= false;
	m_bFreezeStop				= false;
	m_hEventSnapped				= NULL;
	m_nTriggerMode				= ePTNotTrigger;

	// 线程采集图像相关
	m_pWinThread				= NULL;													
	m_hEventToRun				= NULL;											
	m_hEventExit				= NULL;			  										
	m_bTerminate				= true;												
	m_bHaveStopped				= true;	
	
	m_cameraType				= eInterfaceUnknow;

	InitializeCriticalSection(&m_criticalSection); 	// 临界区

	m_bSendGrabTragger          = false;
	nBytes                      = 0; 

	//image process
	m_pWinProcessThread = NULL;
	pNet = net;
	//start stop control
	m_pWinStartStopThread = NULL;

	//
	m_bIsStartSuccess = false;

	//
	iIndexImage = 0;

}

scPTGrab2::~scPTGrab2()
{
	if (eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		Freeze();
		CloseHandle(m_hEventSnapped);
		m_hEventSnapped = NULL;
	}
	else // 线程采集方式
	{
		TerminateGrabThread();
	}

	// 关闭触发
	CloseTriggerMode();
	
	// 断开连接
	if (m_pCamera)
	{
		m_pCamera->SetCallback(NULL, NULL);
		m_pCamera->StopCapture();
		m_pCamera->Disconnect();	
		delete m_pCamera;
		m_pCamera = NULL;
	}
	m_bOnline = false;
	
	// 清空
	if (m_pGrabImage != NULL)
	{
		delete [] m_pGrabImage;
		m_pGrabImage = NULL;
		
		m_lWidth = 0;
		m_lHeight = 0;
	}
	
	DeleteCriticalSection(&m_criticalSection); 
}

// 获取相机的相关信息
bool scPTGrab2::GetCameraInfo(CameraInfo* pCamInfo)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	error = m_pCamera->GetCameraInfo(pCamInfo);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	return true;
}

// 获取相机的序列号
bool scPTGrab2::GetCameraSerialNumber(unsigned int* pSerialNumber)
{
	CameraInfo camInfo;
	if (!GetCameraInfo(&camInfo))
	{
		return false;
	}

	*pSerialNumber = camInfo.serialNumber;

	return true;
}

// 获取相机的索引
bool scPTGrab2::GetCameraIndex(unsigned int* pCamIndex)
{	
	unsigned int uiCnt;
	unsigned int uiSerialNum;

	if (!GetCameraSerialNumber(&uiSerialNum))
	{
		return false;
	}

	if (!GetBusCameraCount(&uiCnt))
	{
		return false;
	}

	PTCameraInfo *pCamInfo = new PTCameraInfo[uiCnt];
	if (!EnumerateBusCameras(pCamInfo, &uiCnt))
	{
		delete []pCamInfo;
	
		return false;
	}

	for (int j=0; j<uiCnt; j++)
	{
		if (pCamInfo[j].m_uiSerialNum == uiSerialNum)
		{
			*pCamIndex = pCamInfo[j].m_uiIndex;

			delete []pCamInfo;

			return true;
		}
	}

	delete []pCamInfo;

	return false;
}

void scPTGrab2::CameraInit()
{
	Property cameraProperty;
	cameraProperty.type=SHUTTER;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.absValue=9;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);

	//
	cameraProperty.type=GAIN;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.absValue=4;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);

    cameraProperty.type=AUTO_EXPOSURE;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);

	cameraProperty.type=FRAME_RATE;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);

	
	cameraProperty.type=SHARPNESS;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);

	cameraProperty.type=BRIGHTNESS;
	m_pCamera->GetProperty(&cameraProperty);
	cameraProperty.absControl=true;
	cameraProperty.onOff=true;
	cameraProperty.autoManualMode=false;
	m_pCamera->SetProperty(&cameraProperty);
}
// 设置回调函数参数
void scPTGrab2::SetOwner(void* pOwner, PTGrabbedCallback funcCallback)
{
	m_pMainFrm = pOwner;
	m_funcCallback = funcCallback;
}

// 设置是否调用回调函数
void scPTGrab2::SetSendCallbackEnable(bool bEnable)
{
	m_bSendCallbackEnable = bEnable;
}

// 获取是否调用回调函数
bool scPTGrab2::GetSendCallbackEnable()
{
	return m_bSendCallbackEnable;
}

// 采集完成调用回调函数
void scPTGrab2::SendCallback(PTStatus status)
{
	// 调用回调函数
	if( (m_funcCallback != NULL) && (m_pGrabImage != NULL) )
	{
		if (ePTGrabOK == status)
		{
			m_funcCallback(m_pMainFrm, status, m_pGrabImage, m_lWidth*nBytes, m_lHeight, m_pixelFormat);		
		}
		else
		{
			m_funcCallback(m_pMainFrm, status, NULL, NULL, NULL, ePTUnknown);		
		}
	}
}



// 根据相机GUID初始化相机
bool scPTGrab2::InitFromGuid(PGRGuid *pGuid)
{
	CString strRecordInfo;

	m_bOnline = false;
	
	Error error;
	
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}

	// 获取相机类型
	m_cameraType = GetCameraTypeFromGuid(pGuid);

	if (eGigE == m_cameraType) // GigE Camera
	{
		m_pCamera = new GigECamera;
	}
	else if (eIEEE1394 == m_cameraType || eUSB2 == m_cameraType 
		|| eUSB3 == m_cameraType) // 1394/USB Camera
	{
		m_pCamera = new Camera;
	}
	else // Unknown Camera
	{
		return false;
	}
	
	// 连接
	error = m_pCamera->Connect(pGuid);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	else
	{
		// 关闭触发
		CloseTriggerMode();
		
		// 试采集图像一幅图像确定图像尺寸，并分配内存
		EnterCriticalSection(&m_criticalSection);
		// 开始采集
		Error error;
		error = m_pCamera->StartCapture(NULL, NULL);
		if (error != PGRERROR_OK)
		{
			LeaveCriticalSection(&m_criticalSection);
			
			return false;
		}
		// 获取图像
		if (ePTGrabOK != AcquireImage())
		{
			LeaveCriticalSection(&m_criticalSection);
			
			return false;
		}
		// 停止采集
		m_pCamera->StopCapture();
		
		LeaveCriticalSection(&m_criticalSection);
		
		// 采集初始化
		if (eWaitEvent == m_grabImageMode) // 事件采集方式
		{
			m_hEventSnapped = CreateEvent(NULL,true,false,L"");
		}
		else // 线程采集方式
		{
			if (!CreateGrabThread())
			{
				return false;
			}
		}
		
		m_bOnline = true;
		
		return true;
	}
}
												
// 根据相机序列号初始化相机
bool scPTGrab2::InitFromSerial(unsigned int uiSerialNum) 
{
	PGRGuid guid;

	GetCameraGuidFromSerialNum(uiSerialNum, &guid);

	return (InitFromGuid(&guid));
}

// 根据索引号初始化相机
bool scPTGrab2::InitFromIndex(unsigned int nCamIndex) 
{
	PGRGuid guid;

	GetCameraGuidFromIndex(nCamIndex, &guid);
	
	return (InitFromGuid(&guid));
}

// 开始连续采集（支持FreeRun、触发两种模式）
// 调用该连续采集函数，没有等待立即返回
bool scPTGrab2::Grab()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		Stop();
		//m_bFreezeStop	= false;
		mLhreadStatus = ePTGrab;
		m_bIsGrabbing = true;
		m_bIsSnap = false;
		
		Start();
	}
	else // 线程采集方式
	{
		Stop();
                //m_bFreezeStop	= false;
		Start();
	
		if (!m_bIsGrabbing)
		{
			m_bAcquireSuceess = false;
			mLhreadStatus = ePTGrab;
			m_bIsGrabbing = true;
			SetEvent(m_hEventToRun);
		}
	}

	return true;
}	

// 结束连续采集	
bool scPTGrab2::Freeze() 
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		m_bFreezeStop	= true;
		Stop();
		mLhreadStatus	= ePTIdle;
		m_bIsGrabbing	= false;
		m_bIsSnap		= true;
		m_bFreezeStop   = false;
	}
	else // 线程采集方式
	{
		m_bFreezeStop = true;
		
		Stop();
	
		if (m_bIsGrabbing || !m_bHaveStopped)
		{
			mLhreadStatus = ePTIdle;
			m_bIsGrabbing = false;
			SetEvent(m_hEventToRun);
			int i = 0;
			while (!m_bHaveStopped && i < 500)
			{
				Sleep(2);
				i++;	
			}
		}

		m_bFreezeStop = false;
	}

	return true;
}

// 获取是否处于连续采集状态
bool scPTGrab2::IsGrabbing()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	return m_bIsGrabbing;
}

// 采集单帧图像（只支持FreeRun模式）。
// 如果bWaitSnapped=true, 等待直到采集完成才返回；
// 如果bWaitSnapped=false, 调用后不等待采集完直接返回；
bool scPTGrab2::Snap(bool bWaitSnapped)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		Stop();
		
		if (bWaitSnapped)
		{
			ResetEvent(m_hEventSnapped); // 重置单帧采集完成事件
		}
		
		mLhreadStatus = ePTSnap;
		m_bIsGrabbing = false;
		m_bIsSnap = true;
		
		Start();
		if (bWaitSnapped)
		{
			// 等待单帧采集完成事件，1秒钟之内采集不完退出等待，返回false
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hEventSnapped, 1000))
			{
				return false;
			}	

			return IsAcquireSuccess();
		}	

	}
	else
	{
		// 如果采集线程处于连续采集状态，则返回false
		if (m_bIsGrabbing || !m_bHaveStopped)
		{
			return false;
		}
		
		//add by he 
		//	Stop();	
		
		if (!Start())
		{
			Stop();
			Start();
		}
			
		m_bAcquireSuceess = false;
		mLhreadStatus = ePTSnap;
		if (bWaitSnapped)
		{
			ResetEvent(m_hEventSnapped);		// 重置单帧采集完成事件
		}
		
		SetEvent(m_hEventToRun);				// 发送运行采集事件
		
		if (bWaitSnapped)
		{
			// 等待单帧采集完成事件，1秒钟之内采集不完退出等待，返回false
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hEventSnapped, 1000))
			{
				return false;
			}
		
			return IsAcquireSuccess();
		}

	}

	return true;
}

// 开始
bool scPTGrab2::Start()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		Error error;
		error = m_pCamera->StartCapture(PTImageEventCallback, this);
		if (error != PGRERROR_OK)
		{
			return false;
		}
	}
	else // 线程采集方式
	{
		Error error;
		// 需先将回调函数设为空
		m_pCamera->SetCallback(NULL, NULL);
		error = m_pCamera->StartCapture(NULL, NULL);
		if (error != PGRERROR_OK)
		{
			return false;
		}
	}

	return true;
}
// eWaitEvent采集模式下的内部采集触发回调函数
void scPTGrab2::PTImageEventCallback(Image *pImage, const void *pCallbackData)
{
	scPTGrab2 *pThis = (scPTGrab2*)pCallbackData;

	pThis->m_bAcquireSuceess = false;

	if (!pImage || !pImage->GetData())
	{
		Sleep(2);

		//return;
		AfxMessageBox(L"image is empty！");
	}
	
	
	if (ePTGrab == pThis->mLhreadStatus) // 连续采集
	{
		if (pThis->m_bIsGrabbing)
		{
			PTStatus status = ePTGrabOK;
			
			Mat image(pImage->GetRows(), pImage->GetCols(), CV_8UC1, pImage->GetData());
			
			Size size = Size(700, 476);
			Rect roiRect = Rect(54, 70, size.width, size.height);
			//scPTGrab2::m_srcMat = scPTGrab2::m_srcMat(roiRect);
			
//			Size size = Size(700, 476);
//			Rect rect = Rect(54, 70, image.size().width, image.size().height-140);

			cvtColor(image(roiRect), m_srcMat, CV_GRAY2RGB);
			//cvtColor(image, m_srcMat, CV_GRAY2RGB);
			
			//CLOG::Out0(L"grab", L"hello %d", TDetectData::iIndex);

			SetEvent(pThis->m_hEventToProcess);
			/*
			pThis->m_ImageRaw.DeepCopy(pImage);

			if (!pThis->TransferData())
			{
				status = ePTGrabFailed;
			}
			else
			{
				pThis->m_bAcquireSuceess = true;
			}
			*/	
			if (pThis->m_bSendCallbackEnable)
			{
				pThis->SendCallback(status);
			}
		}
		else
		{
			AfxMessageBox(L"image is not grabbing！");
		}

	}
	else if (ePTSnap == pThis->mLhreadStatus) // 单帧采集
	{
		if (pThis->m_bIsSnap)
		{
			PTStatus status = ePTGrabOK;

			pThis->m_ImageRaw.DeepCopy(pImage);

			if (!pThis->TransferData())
			{
				status = ePTGrabFailed;
			}
			else
			{
				pThis->m_bAcquireSuceess = true;
			}

			if (pThis->m_bSendCallbackEnable)
			{
				pThis->SendCallback(status);
			}

			SetEvent(pThis->m_hEventSnapped);

			pThis->m_bIsSnap = false;
		}
	}

	Sleep(2);

	return;
}
// 图像数据格式转换：
// 首先分配图像内存，
// 需要考虑图像的宽高、格式。
// 目前支持三种PGR的像素格式：FLYCAPTURE_MONO8、FLYCAPTURE_RAW8和FLYCAPTURE_RGB8
// eGrey8：对应于PGR的FLYCAPTURE_MONO8、FLYCAPTURE_RAW8；
// eRGB24：对应于FLYCAPTURE_RGB8，但需要将R-G-B排列的数据转换为B-G-R排列的数据
bool scPTGrab2::TransferData()
{
	if (NULL == m_ImageRaw.GetData())
	{
		return false;
	}

	// 获取图像像素格式
	PTPixelFormat pixelFormat;	// 像素格式
	//	int nBytes;					// 每个像素占用的字节数
	switch (m_ImageRaw.GetPixelFormat())
	{
	case PIXEL_FORMAT_MONO8:
	case PIXEL_FORMAT_RAW8:
	{
							  pixelFormat = ePTGrey8;
							  nBytes = 1;
	}
		break;
	case PIXEL_FORMAT_RGB8:
	{
							  pixelFormat = ePTRGB24;
							  nBytes = 3;
	}
		break;
	default:
		return false;
	}

	// 获取图像的宽高
	LONG lWidth = m_ImageRaw.GetCols();
	LONG lHeight = m_ImageRaw.GetRows();

	// 分配图像数据内存
	if (NULL == m_pGrabImage)
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;
		m_pixelFormat = pixelFormat;
		m_pGrabImage = new unsigned char[m_lWidth * m_lHeight * nBytes];
	}
	else
	{
		if (m_lWidth != lWidth || m_lHeight != lHeight || m_pixelFormat != pixelFormat)
		{
			delete[] m_pGrabImage;
			m_pGrabImage = NULL;

			m_lWidth = lWidth;
			m_lHeight = lHeight;
			m_pixelFormat = pixelFormat;
			m_pGrabImage = new unsigned char[m_lWidth * m_lHeight * nBytes];
		}
	}

	// 判断内存分配是否成功
	if (NULL == m_pGrabImage)
	{
		return false;
	}

	// 拷贝图像内存
	memcpy(m_pGrabImage, m_ImageRaw.GetData(), m_lWidth * m_lHeight * nBytes);

	// 如果PGR像素格式是FLYCAPTURE_RGB8，则需要将R-G-B排列的数据转换为B-G-R排列的数据
	if (PIXEL_FORMAT_RGB8 == m_ImageRaw.GetPixelFormat()) // RGB转为BGR
	{
		for (int j = 0; j<lHeight; j++)
		for (int i = 0; i<lWidth * 3; i += 3)
		{
			BYTE byTemp;
			byTemp = m_pGrabImage[j*lWidth * 3 + i];
			m_pGrabImage[j*lWidth * 3 + i] = m_pGrabImage[j*lWidth * 3 + i + 2];
			m_pGrabImage[j*lWidth * 3 + i + 2] = byTemp;
		}
	}

	return true;
}
// 停止												
bool scPTGrab2::Stop()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	error = m_pCamera->StopCapture();
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	return true;
}



// 判断相机是否支持指定视频格式
// GigE相机只支持Format7模式
bool scPTGrab2::IsVideoModeSupported(VideoMode vmVideoMode)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (eInterfaceUnknow == m_cameraType) // Unknown Camera
	{
		return false;
	}
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		if (vmVideoMode != VIDEOMODE_FORMAT7)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	Camera *pCamera = (Camera*)m_pCamera;

	if ( vmVideoMode == VIDEOMODE_640x480RGB
		|| vmVideoMode == VIDEOMODE_640x480Y8
		|| vmVideoMode == VIDEOMODE_800x600RGB
		|| vmVideoMode == VIDEOMODE_800x600Y8
		|| vmVideoMode == VIDEOMODE_1024x768RGB
		|| vmVideoMode == VIDEOMODE_1024x768Y8
		|| vmVideoMode == VIDEOMODE_1280x960RGB
		|| vmVideoMode == VIDEOMODE_1280x960Y8
		|| vmVideoMode == VIDEOMODE_1600x1200Y8
		|| vmVideoMode == VIDEOMODE_1600x1200Y8)
	{
		Error error;
		bool bSupported = false;
		for (int j = 0; j < sizeof(g_frFrameRate)/sizeof(g_frFrameRate[0])-1; j++)
		{
			error = pCamera->GetVideoModeAndFrameRateInfo(vmVideoMode, g_frFrameRate[j], &bSupported);
			if ((error == PGRERROR_OK) && bSupported)
			{
				break;
			}
		}
		
		return bSupported;
	}
	else if (VIDEOMODE_FORMAT7 == vmVideoMode) //	默认支持Fmt7
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 判断相机是否支持指定帧率模式
// GigE相机只支持Format7模式
bool scPTGrab2::IsFrameRateSupported(FrameRate frFrameRate)						
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (eInterfaceUnknow == m_cameraType) // Unknown Camera
	{
		return false;
	}	
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		if (frFrameRate != FRAMERATE_FORMAT7)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	Camera *pCamera = (Camera*)m_pCamera;
	// query current video mode
	VideoMode vmVideoMode;
	if (!GetCurVideoMode(&vmVideoMode))
	{
		return false;
	}
		
	// is supported ?
	Error error;
	bool bSupported = false;
	error = pCamera->GetVideoModeAndFrameRateInfo(vmVideoMode, frFrameRate, &bSupported);
	if (error != PGRERROR_OK) return false;
	
	return bSupported;
}

// 设置视频模式
// 设置除Format7之外的其它视频模式
bool scPTGrab2::SetCurVideoMode(VideoMode vmVideoMode)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (FRAMERATE_FORMAT7 == vmVideoMode)
	{
		return false;
	}

	if (eGigE == m_cameraType) // GigE Camera
	{
		return false;
	}

	if (!IsVideoModeSupported(vmVideoMode))
	{
		return false;
	}
	
	bool bIsGrabbing = IsGrabbing();
	Freeze();
			
	//设置寄存器
	unsigned long ulFormat, ulMode;
	if (!ConvertVideoModeToULongs(vmVideoMode, &ulFormat, &ulMode))
	{
		return false;
	}
	ulFormat = ulFormat<<29;
	ulMode = ulMode<<29;
	
	Error error;
	error = m_pCamera->WriteRegister(0X608, ulFormat);
	if (error != PGRERROR_OK) return false;
	
	error = m_pCamera->WriteRegister(0X604, ulMode);
	if (error != PGRERROR_OK) return false;			
	
	if (bIsGrabbing)
	{
		Grab();
	}

	return true;
}

// 获取当前视频模式
bool scPTGrab2::GetCurVideoMode(VideoMode* pvmVideoMode)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (eInterfaceUnknow == m_cameraType) // Unknown Camera
	{
		return false;
	}
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		*pvmVideoMode = VIDEOMODE_FORMAT7;

		return true;
	}
	
	Camera *pCamera = (Camera*)m_pCamera;

	if (NULL == pvmVideoMode)
	{
		return false;
	}	

	Error error;
	VideoMode vmTemp;
	FrameRate frTemp;
	error = pCamera->GetVideoModeAndFrameRate(&vmTemp, &frTemp);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	else
	{
		*pvmVideoMode = vmTemp;
		
		return true;
	}
}

// 设置帧率模式
// 设置除Format7之外的其它帧率模式
bool scPTGrab2::SetCurFrameRate(FrameRate frFrameRate)						
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (FRAMERATE_FORMAT7 == frFrameRate)
	{
		return false;
	}

	if (eGigE == m_cameraType) // GigE Camera
	{
		return false;
	}

	if (!IsFrameRateSupported(frFrameRate))
	{
		return false;
	}
	
	unsigned long ulFrameRate;
	if (!ConvertFrameRateToULong(frFrameRate, &ulFrameRate))
	{
		return false;
	}
	
	ulFrameRate = ulFrameRate<<29;
	
	Error
		error = m_pCamera->WriteRegister(0X600, ulFrameRate);
	if (error != PGRERROR_OK) return false;
	
	return true;
}

// 获取帧率模式
bool scPTGrab2::GetCurFrameRate(FrameRate* pfrFrameRate)							
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (NULL == pfrFrameRate)
	{
		return false;
	}	
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		*pfrFrameRate = FRAMERATE_FORMAT7;
		
		return true;
	}
	
	Error error;
	unsigned int ulValue = 0;	
	error = m_pCamera->ReadRegister(0x600, &ulValue);
	if (error != PGRERROR_OK) return false;
	
	ulValue = ulValue>>29;	
	*pfrFrameRate = g_frFrameRate[ulValue];
	
	return true;
}

// 获取在当前视频模式下，支持的最大帧率
bool scPTGrab2::GetMaxFrameRate(FrameRate* pfrFrameRate)						
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	if (NULL == pfrFrameRate)
	{
		return false;
	}	
	
	if (eInterfaceUnknow == m_cameraType || eGigE == m_cameraType) // Unknown or GigE Camera
	{
		return false;
	}	

	Camera *pCamera = (Camera*)m_pCamera;
	
	VideoMode vmCurVideoMode;
	if (!GetCurVideoMode(&vmCurVideoMode))
	{
		return false;
	}
		
	// 查询可用的最大帧率，如果有则返回true，否则返回false
	Error error;
	bool bSupported;
	int nIndex = 0;
	int nFrameRateCount = sizeof(g_frFrameRate)/sizeof(g_frFrameRate[0]);
	for (nIndex = nFrameRateCount - 1; nIndex >= 0; nIndex--)
	{
		error = pCamera->GetVideoModeAndFrameRateInfo(vmCurVideoMode, g_frFrameRate[nIndex], &bSupported);
		if (error != PGRERROR_OK) return false;
		if (bSupported)
		{
			*pfrFrameRate = g_frFrameRate[nIndex];
			return true;
		}
	}
	
	return false;
}

// 设置帧率
bool scPTGrab2::SetCurFrameRate(float fFrameRate)					              
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	Property cameraProperty;
	cameraProperty.type = FRAME_RATE;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	
	// 设置
	cameraProperty.absValue = fFrameRate;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	cameraProperty.autoManualMode = false; // 手动设置模式
	error = m_pCamera->SetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}	
	
	return true;
}

// 获取当前帧率
bool scPTGrab2::GetCurFrameRate(float *pfFrameRate)					           
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	Property cameraProperty;
	cameraProperty.type = FRAME_RATE;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	*pfFrameRate = cameraProperty.absValue;
	
	return true;
}

// 获取可用的帧率范围
bool scPTGrab2::GetFrameRateRange(float *fMinFrameRate, float *fMaxFrameRate)	                      
{
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = FRAME_RATE;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	*fMinFrameRate = cameraPropertyInfo.absMin;
	*fMaxFrameRate = cameraPropertyInfo.absMax;
	
	return true;
}

// 设置曝光时间，单位ms
bool scPTGrab2::SetCurShutter(float fShutter)									
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	// 设置
	cameraProperty.absValue = fShutter;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	cameraProperty.autoManualMode = false; // 手动设置模式
	error = m_pCamera->SetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}	

	return true;
}

// 设置曝光时间，单位ms
bool scPTGrab2::GetCurShutter(float* pfShutter)										
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	*pfShutter = cameraProperty.absValue;
	
	return true;
}

// 获取可用的曝光时间范围，单位ms
bool scPTGrab2::GetShutterRange(float *pfMinShutter, float *pfMaxShutter)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = SHUTTER;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	*pfMinShutter = cameraPropertyInfo.absMin;
	*pfMaxShutter = cameraPropertyInfo.absMax;

	return true;
}

// 设置增益，单位db
bool scPTGrab2::SetCurGain(float fGain)												
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	// 设置
	cameraProperty.absValue = fGain;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	cameraProperty.autoManualMode = false; // 手动设置模式
	error = m_pCamera->SetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}	
	
	return true;
}

// 获取增益，单位db
bool scPTGrab2::GetCurGain(float* pfGain)											
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	*pfGain = cameraProperty.absValue;

	return true;
}

// 获取可用的增益范围，单位db
bool scPTGrab2::GetGainRange(float *pfMinGain, float *pfMaxGain)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = GAIN;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	*pfMinGain = cameraPropertyInfo.absMin;

	*pfMaxGain = cameraPropertyInfo.absMax;
	
	return true;
}

// Poll the camera to make sure the camera is actually in trigger mode
// before we start it (avoids timeouts due to the trigger not being armed)
bool scPTGrab2::CheckTriggerReady()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	const unsigned int k_softwareTrigger = 0x62C;
    Error error;
    unsigned int regVal = 0;
	
    do 
    {
        error = m_pCamera->ReadRegister( k_softwareTrigger, &regVal );
        if (error != PGRERROR_OK)
        {
			return false;
        }
		
    } while ( (regVal >> 31) != 0 );
	
	return true;
}                           

// Check that the camera actually supports the PGR SOFT_ASYNCLRIGGER or SOFTWARELRIGGER
// method of software triggering
bool scPTGrab2::CheckSoftwareTriggerPresence()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	const unsigned int kLriggerInq = 0x530;
	
	Error error;
	unsigned int regVal = 0;
	
	error = m_pCamera->ReadRegister( kLriggerInq, &regVal );
	
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	if( ( regVal & 0x10000 ) != 0x10000 )
	{
		return false;
	}
	
	return true;
}

// 设置相机的触发模式(软触发、硬触发、或是非触发)。
bool scPTGrab2::SetTriggerMode(PTTriggerMode mode, unsigned long ulTimeout )    
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	if (ePTNotTrigger == mode)
	{
		return CloseTriggerMode();
	}
	
	Error   error;	
	TriggerMode triggerMode;
    error = m_pCamera->GetTriggerMode( &triggerMode );
    if (error != PGRERROR_OK)
    {
		CloseTriggerMode();

        return false;
    }
	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;
	triggerMode.polarity=1;
	if (ePTSoftwareTrigger == mode) // 软触发
	{
		triggerMode.source = 7;
	}
	else
	{
		triggerMode.source = 0;
	}
	error = m_pCamera->SetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
	{
		CloseTriggerMode();

		return false;
	}
	m_nTriggerMode = mode;
	
	bool retVal = CheckTriggerReady();
	if( !retVal )
	{
		CloseTriggerMode();

		return false;
	}

	FC2Config config;
    error = m_pCamera->GetConfiguration( &config );
    if (error != PGRERROR_OK)
    {
		CloseTriggerMode();

        return false;
    } 

	// 设置采集延时
    config.grabTimeout = ulTimeout;
    error = m_pCamera->SetConfiguration( &config );
    if (error != PGRERROR_OK)
    {
		CloseTriggerMode();

        return false;
    } 
	
//	Start();

	return true;
}   

// 获取当前的触发模式           
PTTriggerMode scPTGrab2::GetTriggerMode()											
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return ePTNotTrigger;
	}

	TriggerMode triggerMode;
	Error error;
    error = m_pCamera->GetTriggerMode( &triggerMode );
    if (error != PGRERROR_OK)
    {
        return ePTNotTrigger;
    }

	if (triggerMode.onOff)
	{
		if (7 == triggerMode.source)
		{
			return ePTSoftwareTrigger;
		}
		else
		{
			return ePTHardwareTrigger;
		}
	}

	return ePTNotTrigger;
}

// 关闭触发模式
bool scPTGrab2::CloseTriggerMode()											
{
	if (!m_pCamera)
	{
		return false;
	}

	Error error;
	TriggerMode triggerMode;
	error = m_pCamera->GetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	triggerMode.onOff = false;    
    error = m_pCamera->SetTriggerMode( &triggerMode );
    if (error != PGRERROR_OK)
    {
        return false;
    }

	m_nTriggerMode = ePTNotTrigger;

//	m_pCamera->StopCapture();

	return true;
}

// 是否以触发方式（软触发或硬触发）采集图像
bool scPTGrab2::IsTrigger()														
{
	if ((ePTSoftwareTrigger == m_nTriggerMode) || (ePTHardwareTrigger == m_nTriggerMode))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 用触发方式采集图像：软触发
bool scPTGrab2::SendSoftTrigger()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;	
	if (m_nTriggerMode == ePTSoftwareTrigger)
	{
		if(CheckSoftwareTriggerPresence())
		{
			if(CheckTriggerReady())
			{
				error = m_pCamera->WriteRegister(0x62C, 0x80000000);
				if (error != PGRERROR_OK)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}	

// 获取图像的像素格式
PTPixelFormat scPTGrab2::GetImagePixelFormat()									
{
	return m_pixelFormat;
}

// 获取图像的宽					
long scPTGrab2::GetImageWidth()												
{
	return 	m_lWidth;	
}

// 获取图像的高									
long scPTGrab2::GetImageHeight()												
{
	return m_lHeight;	
}

// 获取存储图像所需的内存大小（字节）
long scPTGrab2::GetImageDataSize()											
{
	int nBytes = 1;
	
	switch(m_pixelFormat)
	{
	case ePTGrey8 :
		nBytes = 1;
		break;
	case ePTRGB24 :
		nBytes = 3;
		break;
	case ePTRGBA32 :
		nBytes = 4;
		break;
	default :
		nBytes = 1;
	}
	
	return m_lWidth * m_lHeight * nBytes;	
}

// 获取图像数据，要求外部已分配好内存
// 内存的大小由GetImageDataSize()获得；
bool scPTGrab2::GetImageData(unsigned char* pImage)						
{
	if (NULL == pImage)
	{
		return false;
	}
	
	EnterCriticalSection(&m_criticalSection);
	if (!m_bAcquireSuceess||
		 m_pGrabImage == NULL || m_lWidth <= 0 || m_lHeight <= 0)
	{
		LeaveCriticalSection(&m_criticalSection);
	
		return false;
	}
	
	memcpy(pImage, m_pGrabImage, GetImageDataSize());	
	
	LeaveCriticalSection(&m_criticalSection);

	return true;	
}
	
// 将指定视频模式转为在相机寄存器中相应的索引值
bool  scPTGrab2::ConvertVideoModeToULongs(VideoMode vmVideoMode, unsigned long* pulFormat, unsigned long* pulMode)
{	
	if (NULL == pulFormat || NULL == pulMode)
	{
		return false;
	}
	
	unsigned long ulFormat = 0;
	unsigned long ulMode =0;
	for (ulFormat = 0; ulFormat < 8; ulFormat++)
	{
		for (ulMode = 0; ulMode < 8; ulMode++)
		{
			if (vmVideoMode == g_fvmFormatVideoMode[ulFormat][ulMode])
			{
				*pulFormat = ulFormat;
				*pulMode = ulMode;
			
				return true;
			}
		}
	}
	
	return false;
}

// 将指定帧率模式转为在相机寄存器中相应的索引值
bool scPTGrab2::ConvertFrameRateToULong(FrameRate frFrameRate, unsigned long* pulFrameRate)
{
	if (NULL == pulFrameRate)
	{
		return false;
	}
	
	switch(frFrameRate)
	{
		// 1.875 fps. (Frames per second)
	   case FRAMERATE_1_875:
		   *pulFrameRate = 0;
		   break;
		   // 3.75 fps.
	   case FRAMERATE_3_75:
		   *pulFrameRate = 1;
		   break;
		   // 7.5 fps.
	   case FRAMERATE_7_5:
		   *pulFrameRate = 2;
		   break;
		   // 15 fps.
	   case FRAMERATE_15:
		   *pulFrameRate = 3;
		   break;
		   // 30 fps.
	   case FRAMERATE_30:
		   *pulFrameRate = 4;
		   break;
		   // 60 fps.
	   case FRAMERATE_60:
		   *pulFrameRate = 5;
		   break;
		   // 120 fps.
	   case FRAMERATE_120:
		   *pulFrameRate = 6;
		   break;
		   // 240 fps.
	   case FRAMERATE_240:
		   *pulFrameRate = 7;
		   break;
	   case FRAMERATE_FORMAT7:
		   *pulFrameRate = 8;
		   break;
	   default:
		   return false;
	}
	
	return true;
}

// 根据当前视频模式充值当前帧率模式
bool scPTGrab2::ResetCurFrameRateByVideoMode()
{
	FrameRate frFrameRate;
	if (!GetCurFrameRate(&frFrameRate))
	{
		return false;
	}
	if (IsFrameRateSupported(frFrameRate))
	{
		return true;
	}
	
	if (!GetMaxFrameRate(&frFrameRate))
	{
		return false;
	}
	
	return SetCurFrameRate(frFrameRate);
}

// 根据当前视频模式重置曝光时间参数
bool scPTGrab2::ResetCurShutterByFrameRate()
{
	float fMaxShutter;
	float fCurShutter;
	float fMinShutter;
	if (!GetShutterRange(&fMinShutter, &fMaxShutter))
	{
		return false;
	}
	
	if (!GetCurShutter(&fCurShutter))
	{
		return false;
	}
	
	if (fCurShutter > fMaxShutter)
	{
		SetCurShutter(fMaxShutter);
	}
	
	return true;
}

// 获取当前图像（灰度）
// bool scPTGrab2::GetImage(scImageGray& image, long alignModulus /* = 1 */)
// {
// 	EnterCriticalSection(&m_criticalSection);
// 	
// 	if (m_pixelFormat != ePTGrey8
// 		|| !m_bAcquireSuceess 
// 		|| m_pGrabImage == NULL || m_lWidth <= 0 || m_lHeight <= 0)
// 	{
// 		LeaveCriticalSection(&m_criticalSection);
// 	
// 		return false;
// 	}
// 	image.CreateImageBuffer(m_lWidth, m_lHeight, alignModulus);
// 	
// 	unsigned char* pDst;
// 	unsigned char* pSrc = m_pGrabImage;
// 	int nBytes = sizeof(unsigned char);
// 	int i = 0;
// 	for (i = 0; i < m_lHeight; i++)
// 	{
// 		pDst = image.PointToRow(i);
// 		memcpy(pDst, pSrc, m_lWidth * nBytes);
// 		
// 		pSrc += m_lWidth * nBytes;
// 	}
// 	
// 	LeaveCriticalSection(&m_criticalSection);
// 	
// 	return true;
// }
// 
// // 获取当前图像（彩色）
// bool scPTGrab2::GetImage(scImageRGB& image, long alignModulus /* = 1 */)
// {
// 	EnterCriticalSection(&m_criticalSection);
// 	
// 	if (m_pixelFormat != ePTRGB24
// 		|| !m_bAcquireSuceess 
// 		|| m_pGrabImage == NULL || m_lWidth <= 0 || m_lHeight <= 0)
// 	{
// 		LeaveCriticalSection(&m_criticalSection);
// 	
// 		return false;
// 	}
// 	image.CreateImageBuffer(m_lWidth, m_lHeight, alignModulus);
// 	
// 	unsigned char* pDst;
// 	unsigned char* pSrc = m_pGrabImage;
// 	int nBytes = 3 * sizeof(unsigned char);
// 	int i = 0;
// 	for (i = 0; i < m_lHeight; i++)
// 	{
// 		pDst = image.PointToRow(i);
// 		memcpy(pDst, pSrc, m_lWidth * nBytes);
// 		
// 		pSrc += m_lWidth * nBytes;
// 	}
// 	
// 	LeaveCriticalSection(&m_criticalSection);
// 	
// 	return true;
// }

// 设置采集延时，单位：ms
bool scPTGrab2::SetGrabTimeout(unsigned long ulTimeout)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	config.grabTimeout = ulTimeout;
	
	error = m_pCamera->SetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	return true;
}	

//  将当前参数保存到相机中
//  lChannel:通道号
bool scPTGrab2::SaveToCamera(long lChannel)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	error = m_pCamera->SaveToMemoryChannel(lChannel);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	return true;
}

// 设置异步和同步总线速度
bool scPTGrab2::SetBusSpeed(BusSpeed  asyncBusSpeed, BusSpeed  isochBusSpeed)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	config.asyncBusSpeed = asyncBusSpeed;
	config.isochBusSpeed = isochBusSpeed;
	
	error = m_pCamera->SetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	return true;
}

// 获取异步和同步总线速度
bool scPTGrab2::GetBusSpeed( BusSpeed* pAsyncBusSpeed, BusSpeed* pIsochBusSpeed)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	*pAsyncBusSpeed = config.asyncBusSpeed;
	*pIsochBusSpeed = config.isochBusSpeed;	
	
	return true;
}

// 获取相机类型
PTCameraType scPTGrab2::GetCameraType()
{
	return m_cameraType;
}

// 写寄存器
// uiAdd:地址；uiValue：写入值
bool scPTGrab2::WriteRegisiter(unsigned int uiAdd, unsigned int uiValue)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	error = m_pCamera->WriteRegister(uiAdd, uiValue);
	if (error != PGRERROR_OK)
	{
		return false;
	}

	return true;
}

// 读寄存器
// uiAdd:地址；uiValue：读入值
bool scPTGrab2::ReadRegisiter(unsigned int uiAdd, unsigned int *uiValue)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	Error error;
	error = m_pCamera->ReadRegister(uiAdd, uiValue);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	return true;
}

// 相机是否连接
bool scPTGrab2::IsOnline()
{
	return (m_bOnline && m_pCamera->IsConnected());
}

void scPTGrab2::DetectImage()
{
	iIndexImage++;//
	/*
	Size size = Size(700, 476);
	Rect roiRect = Rect(54, 70, size.width, size.height);
	scPTGrab2::m_srcMat = scPTGrab2::m_srcMat(roiRect);
	*/
	m_DoubleCrane.imageProcess(scPTGrab2::m_srcMat);

//	CLOG::Out0(L"grab1", L"hello %d", TDetectData::iIndex);

	detectInfo.clear();
	for (int i = 0; i < m_DoubleCrane.contNumber; i++)
	{
		temDetInfo.iIndex = i;
		temDetInfo.itype = 0;
		temDetInfo.x = m_DoubleCrane.objectInfoVect[i].objLoca.locationCenter.x;
		temDetInfo.y = m_DoubleCrane.objectInfoVect[i].objLoca.locationCenter.y;
		temDetInfo.rz = m_DoubleCrane.objectInfoVect[i].objLoca.locationAngle;

		detectInfo.push_back(temDetInfo);
		detectInfoUI.push_back(temDetInfo);
	}
	if (detectInfo.size() == 0)
	{
		sendInfo.Format(L"%d;%d;%.1f;%.1f;%.1f;$", iIndexImage, 0, 0.0, 0.0, 0.0);
	}
	else
	{
		infoHeader.Format(L"%d;%d;", iIndexImage, detectInfo.size());

		for (int i = 0; i < detectInfo.size(); i++)
		{
			oneInfo.Format(L"%.1f;%.1f;%.1f;", detectInfo[i].x, detectInfo[i].y, detectInfo[i].rz);
			infoContent = infoContent + oneInfo;
		}
		sendInfo = infoHeader + infoContent + L"$";
	}
	pNet->Send(sendInfo);

	//CLOG::Out0(L"grab2", L"hello %d\r\n", TDetectData::iIndex);
	infoHeader.Empty();
	infoContent.Empty();
	oneInfo.Empty();
	sendInfo.Empty();

	TDetectData::iIndex++;
}

UINT scPTGrab2::ProceesThread(LPVOID lpParam)
{
	scPTGrab2 *pOwner = (scPTGrab2*)lpParam;

	while (true)
	{
		WaitForSingleObject(pOwner->m_hEventToProcess, INFINITE);

		pOwner->DetectImage();

		SetEvent(pOwner->m_hEventToUpdateUI);
		ResetEvent(pOwner->m_hEventToProcess);
	}

	return 0;
}
// 采集线程函数
// 只有当采集模式为eGrabThread时，才会创建采集线程
UINT scPTGrab2::GrabThread(LPVOID lpParam)
{ 
	scPTGrab2 *pOwner=(scPTGrab2*)lpParam;
	
	if (NULL == pOwner)
	{
		return 0;
	}
	
	for( ; ; )
	{
		WaitForSingleObject(pOwner->m_hEventToRun, INFINITE);	// 当收到了运行采集循环的事件，包括单帧采集和连续采集
		if (pOwner->m_bTerminate)								// 如果结束线程标志为真，则退出总体循环
		{
			break;
		}
		
		switch(pOwner->mLhreadStatus)
		{
		case ePTSnap:
			{
				PTStatus status = pOwner->AcquireImage();		// 采集获取一幅图像
				if (pOwner->m_bSendCallbackEnable)
				{
					pOwner->SendCallback(status);				// 调用SendCallback(true)函数
				}
				
				ResetEvent(pOwner->m_hEventToRun);				// 允许继续进行采集
				SetEvent(pOwner->m_hEventSnapped);				// 通知调用线程停止等待
			}
			break;
		case ePTGrab:
			{
				pOwner->m_bHaveStopped = !pOwner->m_bIsGrabbing;// 标记连续采集开始
				
				while (pOwner->m_bIsGrabbing)					// 是否允许连续采集
				{
					PTStatus status = pOwner->AcquireImage();	// 采集获取一幅图像
//					pOwner->TransferData();
					if (pOwner->m_bSendCallbackEnable)
					{
						pOwner->SendCallback(status);			// 调用SendCallback(status)函数
					}
					Sleep(2);
					
				}
				
				pOwner->m_bHaveStopped = true;					// 标记连续采集已经停止了
				ResetEvent(pOwner->m_hEventToRun);				// 允许继续进行采集
				
			}
			break;
		}	
	}
	
	//发送"采集显示线程结束消息"
	SetEvent(pOwner->m_hEventExit);	
	
	return 0;
}

// 创建采集线程
// 只有当采集模式为eGrabThread时，才会创建采集线程
bool scPTGrab2::CreateGrabThread()
{	
	if (!m_bTerminate)
	{
		return false;
	}
	
	m_bTerminate = false;
	m_hEventSnapped = CreateEvent(NULL,true,false,L"");
	m_hEventToRun = CreateEvent(NULL,true,false,L"");
	m_hEventExit = CreateEvent(NULL,true,false,L"");
	
	//创建采集线程
   	m_pWinThread = AfxBeginThread(GrabThread,this,0,0,0,NULL); 
	
    if (NULL == m_pWinThread) return false;	
	
	return true;
}
bool scPTGrab2::CreateProcessThread()
{

	m_hEventToProcess = CreateEvent(NULL, true, false, L"");
	m_hEventToUpdateUI = CreateEvent(NULL, true, false, L"");

	//创建处理线程
	m_pWinProcessThread = AfxBeginThread(ProceesThread, this, 0, 0, 0, NULL);

	if (NULL == m_pWinProcessThread) return false;

	return true;
}

bool scPTGrab2::CreateStartStopThread()
{
	m_pWinStartStopThread = AfxBeginThread(StartStopThread, this, 0, 0, 0, NULL);

	if (NULL == m_pWinProcessThread) return false;
	return true;
}
UINT scPTGrab2::StartStopThread(LPVOID lpParam)
{
	scPTGrab2 *pGrab = (scPTGrab2*)lpParam;
	CString                  SReceiveInfo;
	CtrlCommand              CameraCMD = CtrlCommand::READY;

	while (true)
	{
		WaitForSingleObject(CStaticClass::m_hEventRecieve, INFINITE);

		if (CStaticClass::m_csReceiveInfo == L"con0")
		{
			CameraCMD = CtrlCommand::RESET;
		}
		else if (CStaticClass::m_csReceiveInfo == L"con1")
		{
			CameraCMD = CtrlCommand::STARTHARD;
		}
		else if (CStaticClass::m_csReceiveInfo == L"con2")
		{
			CameraCMD = CtrlCommand::STOP;
		}
		else
		{
			CameraCMD = CtrlCommand::READY;
		}

		switch (CameraCMD)
		{
		case RESET:
		{
						  pGrab->StartGrab();
						  break;
		}
		case STARTGRAB:
		{
						  pGrab->StartGrab();
						  break;
		}
		case STARTSNAP:
		{
						  pGrab->StartSnap();
						  break;
		}
		case STARTHARD:
		{
						  pGrab->StartHardGrab();
						  break;
		}
		case STARTSOFT:
		{
						  pGrab->StartSoftGrab();
						  break;
		}
		case STOP:
		{
					 pGrab->StopGrab();
					 break;
		}
		default:
			break;
		}
		CStaticClass::m_bControl = false;
		CStaticClass::m_csReceiveInfo.Empty();
		ResetEvent(CStaticClass::m_hEventRecieve);
	}

	return 0;
}
void scPTGrab2::StopGrab()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return ;
	}
	else
	{
		Stop();
	}

}
void  scPTGrab2::StartGrab()
{
	if (!InitFromIndex(0))
	{
		m_bIsStartSuccess = false;;
		CStaticClass::m_comErrorCode.errorType = 0;
		CStaticClass::m_comErrorCode.errorLabel = 'ei';
		CStaticClass::m_comErrorCode.errorNo = '0';
	}
	if (m_bIsGrabbing)
	{
		CloseTriggerMode();
		Freeze();
	}
	if (!Grab())
	{
		m_bIsStartSuccess = false;
	}
	else
	{
		//start success
		m_bIsStartSuccess = true;
	}

	if (m_bIsStartSuccess)
	{
		pNet->Send(L"normal");
	}
	else
	{
		pNet->Send(L"error");
	}
}
void  scPTGrab2::StartSnap()
{
	if (!m_bOnline)
	{
		m_bIsStartSuccess = false;
	}

	if (m_bIsGrabbing)
	{
		Freeze();
		CloseTriggerMode();
	}

	if (!SnapImage())
	{
		m_bIsStartSuccess = false;
	}
}
void  scPTGrab2::StartSoftGrab()
{
	if (m_bSoftTraggerOn == false)
	{
		m_bSoftTraggerOn = true;

		// 如果相机正在连续采集，则暂停连续采集
		if (IsGrabbing())
		{
			Freeze();
		}

		// 如果相机处于触发模式，则关闭触发模式
		if (m_nTriggerMode != ePTNotTrigger)
		{
			CloseTriggerMode();
		}
		// 设置相机为软触发模式
		if (!SetTriggerMode(ePTSoftwareTrigger))
		{
			return;
		}
		else
		{
			// 开始软触发采集
			Grab();
		}
		m_bSendGrabTragger = false;

	}
	else
	{
		m_bSendGrabTragger = false;
		SendSoftTrigger();
	}
}
void  scPTGrab2::StartHardGrab()
{
	if (!InitFromIndex(0))
	{
		m_bIsStartSuccess = false;;
		CStaticClass::m_comErrorCode.errorType = 0;
		CStaticClass::m_comErrorCode.errorLabel = 'ei';
		CStaticClass::m_comErrorCode.errorNo = '0';
	}

	if (IsGrabbing())
	{
		Freeze();
	}

	// 如果相机处于触发模式，则关闭触发模式
	if (m_nTriggerMode != ePTNotTrigger)
	{
		CloseTriggerMode();
	}

	// 设置相机为硬触发模式
	if (!SetTriggerMode(ePTHardwareTrigger))
	{
		m_bIsStartSuccess = false;
	}

	// 开始硬触发采集
	if (!Grab())
	{
		m_bIsStartSuccess = false;
	}
	else
	{
		//start success
		m_bIsStartSuccess = true;
	}
}
// 终止采集线程
// 只有当采集模式为eGrabThread时，才会终止采集线程
bool scPTGrab2::TerminateGrabThread()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (m_bTerminate)
	{
		return true;
	}
	
	if (m_bIsGrabbing)
	{
		Freeze();
	}
	
	//关闭采集线程
	m_bTerminate = true;
	SetEvent(m_hEventToRun);
	
	if (WAIT_TIMEOUT == WaitForSingleObject(m_hEventExit, 1000))
	{
		Sleep(500);
	}	
	
	if (m_pWinThread != NULL)
	{
		m_pWinThread = NULL;
	}	
	
	CloseHandle(m_hEventExit);
	m_hEventExit = NULL;

	CloseHandle(m_hEventSnapped);
	m_hEventSnapped = NULL;

	CloseHandle(m_hEventToRun);
	m_hEventToRun = NULL;
	
	return true;
}

// 从相机采集一幅图像，并进行相应的转换
// 采集图像返回两种标志结果：相机采集是否成功，图像采集是否成功
// 当从相机采集一幅图像成功时，并成功进行相应格式转换后，则返回ePTGrabOK，图像采集成功
// 当从相机采集一幅图像成功时，但未成功进行相应格式转换后，则返回ePTGrabFailed，图像采集失败
// 当从相机采集一幅图像超时时，不进行格式转换，则返回ePTGrabTimeout，图像采集失败
// 当从相机采集一幅图像出错时，如果是调用了Freeze,则根据原来图像采集状态返回ePTGrabOK或ePTGrabFailed；否则返回ePTGrabFailed，图像采集失败
PTStatus scPTGrab2::AcquireImage()
{
	EnterCriticalSection(&m_criticalSection);
	bool bAquireSuceess = m_bAcquireSuceess;
	m_bAcquireSuceess = false;

	bool IsSend = false;
	if (m_bSendGrabTragger)
	{
		IsSend = SendSoftTrigger();
	}

	Error	error;
	
	// 从相机采集图像
	error = m_pCamera->RetrieveBuffer(&m_ImageRaw);
	if (error == PGRERROR_OK)
	{
		m_bAcquireSuceess = TransferData();
		LeaveCriticalSection(&m_criticalSection);
		
		// 相机采集成功；图像获取是否成功由m_bAcquireSuceess决定
		return m_bAcquireSuceess ? ePTGrabOK : ePTGrabFailed;
	}
	else if(error == PGRERROR_TIMEOUT)
	{
		m_bAcquireSuceess = false;
		LeaveCriticalSection(&m_criticalSection);
		
		// 相机采集超时也算成功；图像获取失败
		return ePTGrabTimeout;
	}
	else
	{
		if (m_bFreezeStop)
		{
			m_bAcquireSuceess = bAquireSuceess;
		}
		else
		{
			m_bAcquireSuceess = false;
		}
		
		LeaveCriticalSection(&m_criticalSection);
			
		return m_bAcquireSuceess ? ePTGrabOK : ePTGrabFailed;
	}
}

// 重新扫描总线，当前相机连接会被中断
bool scPTGrab2::RescanBus()
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	Error error;
	error = pBusManager->RescanBus();
	if (error != PGRERROR_OK)
	{
		return false;
	}

	Sleep(1000);

	return true;
}																

// 获取总线上相机的数量
bool scPTGrab2::GetBusCameraCount(unsigned int* pCamCount)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	Error error;
	error = pBusManager->GetNumOfCameras(pCamCount);


	if (*pCamCount <= 0)
	{
		pBusManager->ForceAllIPAddressesAutomatically();
		pBusManager->RescanBus();
		error = pBusManager->GetNumOfCameras(pCamCount);
	}


	if (error != PGRERROR_OK)
	{
		return false;
	}
	else
	{
		return true;
	}
}	

// 获取总线上各相机的GUID值
// pGuid：必须分配内存；pSize:返回相机个数
bool scPTGrab2::EnumerateBusCameras(PGRGuid* pGuid, unsigned int* pSize)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	unsigned int uCameraNum;
	if (!GetBusCameraCount(&uCameraNum) || uCameraNum <= 0)
	{
		return false;
	}

	if (!pGuid || !pSize)
	{
		return false;
	}

	*pSize = uCameraNum;

	Error error;
	for (unsigned int i=0; i<uCameraNum; i++)
	{
		PGRGuid guid;
		error = pBusManager->GetCameraFromIndex(i, &guid);
		if (error != PGRERROR_OK)
		{
			pGuid[i] = guid;
		}
		else
		{
			pGuid[i] = guid;
		}
	}

	return true;
}	

// 获取总线上各相机的序列号
// pSerialInfo：必须分配内存；pSize:返回相机个数
bool scPTGrab2::EnumerateBusCameras(unsigned int* pSerialInfo, unsigned int* pSize)
{	
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	
	unsigned int uCameraNum;
	if (!GetBusCameraCount(&uCameraNum) || uCameraNum <= 0)
	{
		return false;
	}
	
	if (!pSerialInfo || !pSize)
	{
		return false;
	}
	
	*pSize = uCameraNum;

	Error error;
	for (unsigned int i=0; i<uCameraNum; i++)
	{
		unsigned int uSerialNum;
		PGRGuid guid;
		error = pBusManager->GetCameraSerialNumberFromIndex(i, &uSerialNum);
		if (error == PGRERROR_OK)
		{
			pSerialInfo[i] = uSerialNum;
		}
		else
		{
			pSerialInfo[i] = 0;
		}
	}
	
	return true;
}

// 获取总线上各相机的信息(序列号，相机类型等)
// pCameraInfo：必须分配内存；pSize:返回相机个数
bool scPTGrab2::EnumerateBusCameras(PTCameraInfo* pCameraInfo, unsigned int* pSize)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	unsigned int uCameraNum;
	PGRGuid guid;
	GigECamera gigECamera;
	Camera camera;

	if (!GetBusCameraCount(&uCameraNum) || uCameraNum <= 0)
	{
		return false;
	}
	
	if (!pCameraInfo || !pSize)
	{
		return false;
	}
	
	*pSize = uCameraNum;

	Error error;
	for (unsigned int i=0; i<uCameraNum; i++)
	{
		pCameraInfo[i].m_uiIndex = i;
		unsigned int uiSerialNum;
		error = pBusManager->GetCameraSerialNumberFromIndex(i, &uiSerialNum);
		if (error == PGRERROR_OK)
		{
			pBusManager->GetCameraFromSerialNumber(uiSerialNum, &guid);

			pCameraInfo[i].m_uiSerialNum = uiSerialNum;
			pCameraInfo[i].m_CameraType = GetCameraTypeFromSerialNum(uiSerialNum);
			
			// GigE Info
			if (eGigE == pCameraInfo[i].m_CameraType) // GigE Camera
			{
				if (gigECamera.Connect(&guid) == PGRERROR_OK)
				{
					CameraInfo tmpCamInfo;
					gigECamera.GetCameraInfo(&tmpCamInfo);
					strcpy_s(pCameraInfo[i].m_cModelName, tmpCamInfo.modelName);
					strcpy_s(pCameraInfo[i].m_cResolution, tmpCamInfo.sensorResolution);
				}
				GetGigECameraIPAddressByIndex(i, pCameraInfo[i].m_ipAddress, pCameraInfo[i].m_subnetMask, pCameraInfo[i].m_defaultGateway);
			}
			else
			{	
				if (camera.Connect(&guid) == PGRERROR_OK)
				{
					CameraInfo tmpCamInfo;
					camera.GetCameraInfo(&tmpCamInfo);
					strcpy_s(pCameraInfo[i].m_cModelName, tmpCamInfo.modelName);
					strcpy_s(pCameraInfo[i].m_cResolution,  tmpCamInfo.sensorResolution);
				}
			}
		}
		else
		{
			pCameraInfo[i].m_uiSerialNum = 0;
			pCameraInfo[i].m_CameraType = eInterfaceUnknow;
		}
		
	}
	
	return true;
}

// 根据索引获取相机的GUID
bool scPTGrab2::GetCameraGuidFromIndex(int iIndex, PGRGuid *pGuid)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	unsigned int uCameraNum;
	if (!GetBusCameraCount(&uCameraNum) || uCameraNum <= 0)
	{
		return false;
	}

	Error error;
	error = pBusManager->GetCameraFromIndex(iIndex, pGuid);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	else
	{
		return true;
	}

}

// 根据序列号获取相机的GUID
bool scPTGrab2::GetCameraGuidFromSerialNum(unsigned int uiSerialNum, PGRGuid *pGuid)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	unsigned int uCameraNum;
	if (!GetBusCameraCount(&uCameraNum) || uCameraNum <= 0)
	{
		return false;
	}
	
	Error error;
	error = pBusManager->GetCameraFromSerialNumber(uiSerialNum, pGuid);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	else
	{
		return true;
	}

}

// 根据GUID获取相机的类型
PTCameraType scPTGrab2::GetCameraTypeFromGuid(PGRGuid *pGuid)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	PTCameraType cameraType(eInterfaceUnknow);
	Error error;
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(pGuid, &interfaceType);
	if (error != PGRERROR_OK)
	{
		return eInterfaceUnknow;
	}
	
	if (INTERFACE_GIGE == interfaceType) // GigE Camera
	{
		cameraType = eGigE;
	}
	else if (INTERFACE_IEEE1394 == interfaceType) // 1394 Camera
	{
		cameraType = eIEEE1394;
	}
	else if (INTERFACE_USB2 == interfaceType) // USB2.0 Camera
	{
		cameraType = eUSB2;
	}
	else if (INTERFACE_USB3 == interfaceType) // USB3.0 Camera
	{
		cameraType = eUSB3;
	}

	return cameraType;
}

// 根据索引获取相机的类型
PTCameraType scPTGrab2::GetCameraTypeFromIndex(unsigned int uiIndex)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	PGRGuid guid;
	if (!GetCameraGuidFromIndex(uiIndex, &guid))
	{
		return eInterfaceUnknow;
	}
	
	PTCameraType cameraType(eInterfaceUnknow);
	Error error;	
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(&guid, &interfaceType);
	if (INTERFACE_GIGE == interfaceType) // GigE Camera
	{
		cameraType = eGigE;
	}
	else if (INTERFACE_IEEE1394 == interfaceType) // 1394 Camera
	{
		cameraType = eIEEE1394;
	}
	else if (INTERFACE_USB2 == interfaceType) // USB2.0 Camera
	{
		cameraType = eUSB2;
	}
	else if (INTERFACE_USB3 == interfaceType) // USB3.0 Camera
	{
		cameraType = eUSB3;
	}
	
	return cameraType;
}

// 根据序列号获取相机的类型
PTCameraType scPTGrab2::GetCameraTypeFromSerialNum(unsigned int uiSerialNum)
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();

	PGRGuid guid;
	if (!GetCameraGuidFromSerialNum(uiSerialNum, &guid))
	{
		return eInterfaceUnknow;
	}
	
	PTCameraType cameraType(eInterfaceUnknow);
	Error error;	
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(&guid, &interfaceType);
	if (INTERFACE_GIGE == interfaceType) // GigE Camera
	{
		cameraType = eGigE;
	}
	else if (INTERFACE_IEEE1394 == interfaceType) // 1394 Camera
	{
		cameraType = eIEEE1394;
	}
	else if (INTERFACE_USB2 == interfaceType) // USB2.0 Camera
	{
		cameraType = eUSB2;
	}
	else if (INTERFACE_USB3 == interfaceType) // USB3.0 Camera
	{
		cameraType = eUSB3;
	}

	return cameraType;
}

// 根据GUID设置GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::SetGigECameraIPAddressByGuid(PGRGuid *pGuid, IPAddress ipAddress, 
								   IPAddress subnetMask, IPAddress defaultGateway)
{
	CameraInfo camInfo[128];
	memset(camInfo, 0, 128*sizeof(CameraInfo));
	unsigned int uiCameraCnt;
	Error error;
	uiCameraCnt = 128;
	error = BusManager::DiscoverGigECameras(camInfo, &uiCameraCnt);
	if (error != PGRERROR_OK || uiCameraCnt <= 0)
	{
		return false;
	}

	for (int j=0; j<uiCameraCnt; j++)
	{
		PGRGuid tmpGuid;
		if (GetCameraGuidFromSerialNum(camInfo[j].serialNumber, &tmpGuid))
		{
			if (tmpGuid == *pGuid)
			{
				MACAddress macAddress;
				macAddress = camInfo[j].macAddress;
				error = BusManager::ForceIPAddressToCamera(macAddress, ipAddress, subnetMask, defaultGateway);
				if (error == PGRERROR_OK)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return false;
}

// 根据序列号设置GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::SetGigECameraIPAddressBySerialNum(unsigned int uiSerialNum, IPAddress ipAddress, 
											  IPAddress subnetMask, IPAddress defaultGateway)
{
	PGRGuid guid;
	if (!GetCameraGuidFromSerialNum(uiSerialNum, &guid))
	{
		return false;
	}

	return SetGigECameraIPAddressByGuid(&guid, ipAddress, subnetMask, defaultGateway);
}

// 根据索引设置GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::SetGigECameraIPAddressByIndex(unsigned int uiIndex, IPAddress ipAddress, 
										  IPAddress subnetMask, IPAddress defaultGateway)
{
	PGRGuid guid;
	if (!GetCameraGuidFromIndex(uiIndex, &guid))
	{
		return false;
	}
	
	return SetGigECameraIPAddressByGuid(&guid, ipAddress, subnetMask, defaultGateway);	
}

// 根据GUID获取GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::GetGigECameraIPAddressByGuid(PGRGuid *pGuid, IPAddress &ipAddress, 
										 IPAddress &subnetMask, IPAddress &defaultGateway)
{
	CameraInfo camInfo[128];
	memset(camInfo, 0, 128*sizeof(CameraInfo));
	unsigned int uiCameraCnt;
	Error error;
	uiCameraCnt = 128;
	error = BusManager::DiscoverGigECameras(camInfo, &uiCameraCnt);
	if (error != PGRERROR_OK || uiCameraCnt <= 0)
	{
		return false;
	}
	
	for (int j=0; j<uiCameraCnt; j++)
	{
		PGRGuid tmpGuid;
		if (GetCameraGuidFromSerialNum(camInfo[j].serialNumber, &tmpGuid))
		{
			if (tmpGuid == *pGuid)
			{
				ipAddress = camInfo[j].ipAddress;
				subnetMask = camInfo[j].subnetMask;
				defaultGateway = camInfo[j].defaultGateway;

				return true;
			}
		}
		
	}

	return false;
}

// 根据索引获取GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::GetGigECameraIPAddressByIndex(unsigned int uiIndex, IPAddress &ipAddress, 
										  IPAddress &subnetMask, IPAddress &defaultGateway)
{	
	PGRGuid guid;
	if (!GetCameraGuidFromIndex(uiIndex, &guid))
	{
		return false;
	}
	
	return GetGigECameraIPAddressByGuid(&guid, ipAddress, subnetMask, defaultGateway);		
}

// 根据序列号获取GigE相机的IP、子网掩码和默认网关
bool scPTGrab2::GetGigECameraIPAddressBySerialNum(unsigned int uiSerialNum, IPAddress &ipAddress, 
											  IPAddress &subnetMask, IPAddress &defaultGateway)
{
	PGRGuid guid;
	if (!GetCameraGuidFromSerialNum(uiSerialNum, &guid))
	{
		return false;
	}
	
	return GetGigECameraIPAddressByGuid(&guid, ipAddress, subnetMask, defaultGateway);			
}


// 重新设置总线上GigECamera相机IP
bool scPTGrab2::AutoForceAllGigECamerasIP()
{
	if (m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();	

	Error error;

	// 强制设置IP
	error = pBusManager->ForceAllIPAddressesAutomatically();
	if (error != PGRERROR_OK)
	{
		return false;
	}

	// 设置完成后，等待5s
	Sleep(5000);

	// 重新扫描总线
	error = pBusManager->RescanBus();
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	return true;
}	

// 获取Format7参数
bool scPTGrab2::GetFormat7Config(PTFormat7Info &format7Info)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (eInterfaceUnknow == m_cameraType) // Unknown Camera
	{
		return false;
	}	
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		GigECamera *pCamera = (GigECamera*)m_pCamera;
		
		Mode mode;
		Error error;
		error = pCamera->GetGigEImagingMode(&mode);
		if (error != PGRERROR_OK)
		{
			return false;
		}
		format7Info.m_mode = mode;

		GigEImageSettings imageSettings;
		error = pCamera->GetGigEImageSettings( &imageSettings);
		if (error != PGRERROR_OK)
		{
			return false;
		}
		
		format7Info.m_uiStartX = imageSettings.offsetX;
		format7Info.m_uiStartY = imageSettings.offsetY;
		format7Info.m_uiWidth = imageSettings.width;
		format7Info.m_uiHeight = imageSettings.height;
		format7Info.m_pixelFormat = imageSettings.pixelFormat;	
	}
	else // IEEE1394或USB
	{
		Camera *pCamera = (Camera*)m_pCamera;	
		
		Error error;
		Format7ImageSettings formatSettings;
		unsigned int uiPacketSize;
		float fBusSpeed;
		error = pCamera->GetFormat7Configuration(&formatSettings, &uiPacketSize, &fBusSpeed);
		if (error != PGRERROR_OK)
		{
			return false;
		}

		format7Info.m_fBusSpeed = fBusSpeed;
		format7Info.m_uiStartX = formatSettings.offsetX;
		format7Info.m_uiStartY = formatSettings.offsetY;
		format7Info.m_uiWidth = formatSettings.width;
		format7Info.m_uiHeight = formatSettings.height;
		format7Info.m_mode = formatSettings.mode;
		format7Info.m_pixelFormat = formatSettings.pixelFormat;
	}
	
	return true;
}

// 设置Format7参数
bool scPTGrab2::SetFormat7Config(PTFormat7Info format7Info)
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	if (eInterfaceUnknow == m_cameraType) // Unknown Camera
	{
		return false;
	}	
	
	if (eGigE == m_cameraType) // GigE Camera
	{
		GigECamera *pCamera = (GigECamera*)m_pCamera;
		
		Error error;
		Mode mode = format7Info.m_mode;
		bool bSupported;
		error = pCamera->QueryGigEImagingMode(mode, &bSupported);
		if (error != PGRERROR_OK || !bSupported)
		{
			return false;
		}

		error = error = pCamera->SetGigEImagingMode(mode);
		if (error != PGRERROR_OK)
		{
			return false;
		}

		GigEImageSettings imageSettings;
		imageSettings.offsetX = format7Info.m_uiStartX;
		imageSettings.offsetY = format7Info.m_uiStartY;
		imageSettings.width = format7Info.m_uiWidth;
		imageSettings.height = format7Info.m_uiHeight;
		imageSettings.pixelFormat = format7Info.m_pixelFormat;
		
		error = pCamera->SetGigEImageSettings( &imageSettings );
		if (error != PGRERROR_OK)
		{
			return false;
		}
	}
	else // 1394/USB
	{
		Camera *pCamera = (Camera*)m_pCamera;	
		
		Error error;	
		Format7ImageSettings formatSettings;	
		formatSettings.offsetX = format7Info.m_uiStartX;
		formatSettings.offsetY = format7Info.m_uiStartY;
		formatSettings.width = format7Info.m_uiWidth;
		formatSettings.height = format7Info.m_uiHeight;
		formatSettings.pixelFormat = format7Info.m_pixelFormat;
		formatSettings.mode = format7Info.m_mode;
		
		error = pCamera->SetFormat7Configuration(&formatSettings, format7Info.m_fBusSpeed);
		if (error != PGRERROR_OK)
		{
			return false;
		}
		
	}
	
	return true;
}

// 采集单帧图像；
// 该采集函数不能与他采集函数同时调用；
// 该采集函数直到采集完成后退出；
bool scPTGrab2::SnapImage()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	m_pCamera->SetCallback(NULL, NULL);	 // 需先将回调函数设为空

	m_pCamera->StartCapture(NULL, NULL);
	
	PTStatus status = AcquireImage();
	
	if (m_bSendCallbackEnable)
	{
		SendCallback(status);
	}
	
	m_pCamera->StopCapture();
	
	return IsAcquireSuccess();
}

// 利用软触发方式，采集单帧图像
// 发送完软触发信号后，直到采集单帧图像完成后，该函数才返回
// 调用该函数，需要将相机设置为软触发模式
// 该函数不依赖于采集线程
bool scPTGrab2::TriggerSnap()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}
	
	m_pCamera->StopCapture();

	m_pCamera->SetCallback(NULL, NULL); // 需先将函数设为空

	m_pCamera->StartCapture(NULL, NULL);
	
	if (!SendSoftTrigger())
	{
		return false;
	}
	
	PTStatus status = AcquireImage();
	
	if (m_bSendCallbackEnable)
	{
		SendCallback(status);
	}
	
	m_pCamera->StopCapture();
	
	return IsAcquireSuccess();
}

// 图像是否采集成功
bool scPTGrab2::IsAcquireSuccess()
{
	return m_bAcquireSuceess;
}

//设置状态，设置brightness、exposure、shutter、gain、framerate为手动状态
bool scPTGrab2::SetStatusManual()
{
	if (!m_bOnline || !m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	
	unsigned int uiValue = 0XC2000000;	
	
	//设置Brightness为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0X800, uiValue);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	//设置exposure为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0X804,uiValue);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	//设置shutter时间为Abs_Value手动方式	
	error = m_pCamera->WriteRegister(0X81C,uiValue);
	if (error != PGRERROR_OK)
	{
		return false;
	}
	
	//设置gain为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0X820,uiValue);	
	if (error != PGRERROR_OK)
	{
		return false;
	}

	return true;
}                      
												
bool scPTGrab2::Destroy()
{
	TerminateGrabThread();
	
	// 清空
	if (m_pGrabImage != NULL)
	{
		delete [] m_pGrabImage;
		m_pGrabImage = NULL;
		
		m_lWidth = 0;
		m_lHeight = 0;
	}
	
	m_bOnline = false;
	
	return true;
}