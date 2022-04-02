#ifndef STATETOOL_H
#define STATETOOL_H
#pragma once

#include <QSettings>
#include <QFontMetrics>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDir>
#include "common.h"
#include "setDebugNew.h"
//相机库
#include "DHGrabberForSG.h"
#pragma comment(lib,"DHGrabberForSG.lib")

#include "DHGrabberForMER.h"
#pragma comment(lib,"DHGrabberForMER.lib")
//算法库
#include "RotEmptyBottle.h"
#pragma comment(lib,"RotEmptyBottle.lib")

#include "alg_struct.h"

using namespace Alg;  // 使用命名空间 [2009-9-15-yangfs]

class StateTool
{

public:
	static bool WritePrivateProfileQString(QString strSectionName, QString strKeyName, QString strValue, QString strFileName);
	static bool writeInit(QString path, QString user_key, QString user_value);
	static bool readInit(QString path, QString user_key, QString &user_value);
	static bool updateText(QString text, int max_width, QString &elided_text);
	static QString getSkinName();

};

//错误信息类
class cErrorTypeInfo
{
public:
	int m_iErrorTypeCount;//总错误类型个数
	int iErrorCountByType[ERRORTYPE_MAX_COUNT];

	void operator = (const cErrorTypeInfo cError)
	{
		m_iErrorTypeCount = cError.m_iErrorTypeCount;
		for (int i=0; i<ERRORTYPE_MAX_COUNT; i++)
		{
			iErrorCountByType[i] = cError.iErrorCountByType[i];
		}
	}
public:
	cErrorTypeInfo()
	{
		m_iErrorTypeCount=0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorCountByType[i]=0;
		}
	}
	~cErrorTypeInfo()
	{

	}
	void Clear()
	{
//		m_iErrorTypeCount=0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorCountByType[i]=0;
		}
	}
	BOOL ErrorTypeJudge(int &iErrorType)
	{
		if (0<iErrorType&&iErrorType<(m_iErrorTypeCount+1))
		{
			return TRUE;
		}
		else
			return FALSE;
	}
};

// 机器信号 ：： 其中仅适用接口卡图像信号数m_iImageCount
class CMachineSignal  //根据具体现场的需求来定义机器信号内容
{
public:	
	int m_iSensorCount;	    // 传感器，帧计数 即到位信号计数 最大
	int m_iImageCount;		// 接口卡图像信号计数
	int m_iKickCount;       // 踢废计数
public:

	CMachineSignal()
	{
		m_iSensorCount = 0;       
		m_iImageCount  = 0;
		m_iKickCount = 0;       // 踢废计数
	}

	virtual ~CMachineSignal()
	{
	}

	CMachineSignal(const CMachineSignal &MachineSignal)
	{
		m_iSensorCount = MachineSignal.m_iSensorCount;
		m_iImageCount  = MachineSignal.m_iImageCount;
		m_iKickCount   = MachineSignal.m_iKickCount;
	}

	const CMachineSignal& operator=(const CMachineSignal &MachineSignal)
	{
		m_iSensorCount = MachineSignal.m_iSensorCount;
		m_iImageCount  = MachineSignal.m_iImageCount;
		m_iKickCount   = MachineSignal.m_iKickCount;
		return *this;
	}
};

//采集图像信息
class CGrabElement
{ 
public:
	CGrabElement()
	{
		nCamSN = -1;
		dCostTime = 0;
		bHaveImage = FALSE;
		nCheckRet = 0;
		nSignalNo = 0;
		myImage = NULL;
		initID = 0;
		nWidth = 0;
		nHeight = 0;
		SourceImage = NULL;
	}
	~CGrabElement()
	{
		//释放算法返回错误信息链表
		if (!cErrorParaList.isEmpty())
		{
			cErrorParaList.clear();
		}
	}
public:	
	QImage*				myImage;	        // 具体的图像信息  宽度、高度、数据区等
	s_AlgImageLocInfo   sImgLocInfo;		// 定位信息
	QList<s_ErrorPara>  cErrorParaList;		// 算法返回的错误信息链表
	QList<QRect>		cErrorRectList;		// 算法返回的错误矩形框链表
	int					nCamSN;				// 相机序号
	double				dCostTime;			// 检测耗时	
	BOOL				bHaveImage;			// 是否有图像
	int					nCheckRet;			// 检测结果 对应错误列表
	int                 nSignalNo;			// 接口卡信号个数 255
	int					initID;
	int					nMouldID;			// 瓶底模点
	QImage*				SourceImage;
	int					nWidth;
	int					nHeight;
};

class CDetectElement
{ 
public:
	CDetectElement()
	{
		iType = 0;
		bIsImageNormalCompelet = false;
		bIsImageStressCompelet = false;
		ImageNormal = NULL;
		ImageStress = NULL;
 		iCameraNormal = -1;
 		iCameraStress = -1;
 		iSignalNoNormal = -1;
 		iSignalNoStress = -1;

	}
	~CDetectElement()
	{
// 		delete ImageNormal;
// 		delete ImageStress;
	}
	void AddNormalImage(CGrabElement*	sImageNormal)
	{
		Q_ASSERT(sImageNormal->myImage->width()>0 );
		Q_ASSERT(sImageNormal->nSignalNo>=0 );
		Q_ASSERT(sImageNormal->nSignalNo<256 );

		ImageNormal = sImageNormal;
		iCameraNormal = sImageNormal->nCamSN;
		iSignalNoNormal = sImageNormal->nSignalNo;
		bIsImageNormalCompelet = true;
	}
	void AddStressImage(CGrabElement*	sImageStress)
	{
		Q_ASSERT(sImageStress->myImage->width()>0 );
		Q_ASSERT(sImageStress->nSignalNo>=0 );
		Q_ASSERT(sImageStress->nSignalNo<256 );

		ImageStress = sImageStress;
		iCameraStress = sImageStress->nCamSN;
		iSignalNoStress = sImageStress->nSignalNo;
		bIsImageStressCompelet = true;
	}
	bool IsImageNormalCompelet()
	{
		return bIsImageNormalCompelet;
	}
	bool IsImageStressCompelet()
	{
		return bIsImageStressCompelet;
	}
	void setType(int type)
	{
		iType = type;
	}
	int SignalNoNormal()
	{
		return iSignalNoNormal;
	}	
	int SignalNoStress()
	{
		return iSignalNoStress;
	}
	void copyDatato(CDetectElement &cDes)
	{
		Q_ASSERT(ImageNormal->myImage->width()>0 );
		Q_ASSERT(ImageStress->myImage->width()>0 );
		Q_ASSERT(ImageNormal->nSignalNo>=0 );
		Q_ASSERT(ImageStress->nSignalNo>=0 );
		Q_ASSERT(ImageNormal->nSignalNo<256 );
		Q_ASSERT(ImageStress->nSignalNo<256 );

		cDes.ImageNormal = ImageNormal;
		cDes.ImageStress = ImageStress;
		cDes.iType = iType;
		cDes.iCameraNormal = iCameraNormal;
		cDes.iCameraStress = iCameraStress;
		cDes.iSignalNoNormal = iSignalNoNormal;
		cDes.iSignalNoStress = iSignalNoStress;
		cDes.bIsImageNormalCompelet = bIsImageNormalCompelet;
		cDes.bIsImageStressCompelet = bIsImageStressCompelet;
		clear();
	}
	void clear()
	{
		iType = 0;
		bIsImageNormalCompelet = false;
		bIsImageStressCompelet = false;
		ImageNormal = NULL;
		ImageStress = NULL;
		iCameraNormal = -1;
		iCameraStress = -1;
		iSignalNoNormal = -1;
		iSignalNoStress = -1;

	}
public:	
	CGrabElement*	ImageNormal;	    // 图像
	CGrabElement*   ImageStress;		// 应力图像
	int				iType;				// 0:保存正常图像  1：保存应力图像
	int				iCameraNormal;
	int				iCameraStress;
	int				iSignalNoNormal;
	int				iSignalNoStress;
	bool			bIsImageNormalCompelet;
	bool			bIsImageStressCompelet;
};

//互斥伪实现消息等待
class GrabberEvent
{
public:
	GrabberEvent()
	{
		for (int i = 0;i<CAMERA_MAX_COUNT;i++)
		{
			iTimpCheck[i] = 0;
		}
	}

	void postMessage(int iCamera)
	{
		mutexM.lock();
		iTimpCheck[iCamera]++;
		waitConditionM.wakeOne();
		mutexM.unlock();
	}
	int waitMessage(unsigned long time = ULONG_MAX)
	{
		int bRtn = 0;
		mutexM.lock();
		for (int iCamera = 0;iCamera<CAMERA_MAX_COUNT;iCamera++)
		{
			if (iTimpCheck[iCamera]>0)
			{ 
				iTimpCheck[iCamera]--;
				bRtn = true;
				mutexM.unlock();
				return iCamera;
			}
		}
		bRtn = waitConditionM.wait(&mutexM,time);
		for (int iCamera = 0;iCamera<CAMERA_MAX_COUNT;iCamera++)
		{
			if (iTimpCheck[iCamera]>0)
			{
				iTimpCheck[iCamera]--;
				bRtn = true;
				mutexM.unlock();
				return iCamera;
			}
		}
		mutexM.unlock();
		return -1;
	}
private:
	QWaitCondition waitConditionM;
	QMutex mutexM;
	int iTimpCheck[CAMERA_MAX_COUNT];
};
//互斥伪实现消息等待
class IOCardEvent
{
public:
	IOCardEvent()
	{
		int bRtn = 0;
	}

	void postMessage()
	{
		mutexM.lock();
//		iTimpCheck[iCamera]++;
		waitConditionM.wakeOne();
		mutexM.unlock();
	}
	bool waitMessage(unsigned long time = ULONG_MAX)
	{
		int bRtn = 0;
		mutexM.lock();
		bRtn = waitConditionM.wait(&mutexM,time);
		mutexM.unlock();
		return bRtn;
	}
private:
	QWaitCondition waitConditionM;
	QMutex mutexM;
//	int iTimpCheck[CAMERA_MAX_COUNT];
};


//计时器类
class CSpendTime
{
public:
	LARGE_INTEGER litmp;
	LONGLONG Qpart1,Qpart2,Useingtime,Useingtime_us;
	double dfMinus,dfFreq,dfTime;

	virtual ~CSpendTime()
	{
	}
	CSpendTime()
	{
		Qpart1 = 0;
		Qpart2 = 0;
		Useingtime = 0;
		dfMinus = 0;
		dfFreq = 0;
		dfTime = 0;
		Useingtime_us = 0;
	}
	void StartSpeedTest()
	{
		//获得CPU计时器的时钟频率
		QueryPerformanceFrequency(&litmp);//取得高精度运行计数器的频率f,单位是每秒多少次（n/s）,
		dfFreq = (double)litmp.QuadPart;

		QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值
		Qpart1 = 0;
		Qpart1 = litmp.QuadPart; //开始计时
	}
	void StopSpeedTest()
	{

		QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值
		Qpart2 = litmp.QuadPart; //终止计时

		dfMinus = (double)(Qpart2 - Qpart1);//计算计数器值
		dfTime = (dfMinus / dfFreq)*1000;//获得对应时间,单位为ms,可以乘1000000精确到微秒级（us）
		Useingtime = dfTime*1000;
		Useingtime_us = dfTime*1000000;

	}
};
//对话框信息-提示对话框只能在主线程弹出，将弹出信息传递给主线程
typedef struct _MSGBoxInfo
{
	QString strMsgtitle;
	QString strMsgInfo;
}s_MSGBoxInfo;

#endif