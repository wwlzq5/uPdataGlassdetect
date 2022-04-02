#ifndef GLASSWAREDETECTSYSTEM_H
#define GLASSWAREDETECTSYSTEM_H

#include <QMainWindow>
#include <qlabel.h>

#include <QWidget>
#include <QDesktopWidget>
#include <QTranslator>
#include <QIcon>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include <QTcpSocket>
#include "widget_title.h"
#include "widget_image.h"
#include "Widget_CarveSetting.h"
#include "widget_Management.h"
#include "widget_test.h"
#include "LightSource.h"
#include "widgetwarning.h"
#include "widgetUser.h"
#include "widget_plc.h"

#include <time.h>
#include <tlhelp32.h>
#include <QProcess>
#include "CombineResult.h"
#include "ConfigInfo.h"
#include "myQueue.h"
#include "clogFile.h"
#include "DetectThread.h"
#include "CIOCard.h"
#include "cmyerrorlist.h"
#include "stateTool.h"
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )

struct ImageSave
{
	QImage* pThat;
	QImage m_Picture;
};
struct MyErrorType
{
	int id;
	int nType;
	int nErrorArea;
};

class GlasswareDetectSystem : public QDialog
{
	Q_OBJECT

public:
	GlasswareDetectSystem(QWidget *parent = 0);
	~GlasswareDetectSystem();
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
public:
	void Init();
	void InitImage();//初始化图像
	void CloseCam();//关闭相机
	void InitIOCard();
	void ReleaseIOCard();
	void ReleaseImage();
	void ReleaseAll();//释放资源
	void GrabCallBack(const s_GBSIGNALINFO *SigInfo);//采集回调函数
	QString getVersion(QString strFullName);
	void StartDetectThread();	//开启检测线程
	void initDetectThread();	//开启检测线程
	void ShowCheckSet(int nCamIdx = 0,int signalNumber = 0);
	bool changeLanguage(int nLangIdx);
	void InitCamImage(int iCameraNo);
	void StartCamGrab();
	void CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight);
	bool RoAngle(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle);
	void InitLastData();
	bool CheckLicense();
	void MonitorLicense();
	void showAllert();
	static DWORD WINAPI SendDetect(void*);
	DWORD GetProcessIdFromName(const char*processName);
	void loginState(int nPerm,bool isUnlock);
	void initSocket();
signals:
	void signals_intoManagementWidget();
	void signals_intoTestWidget();
	void signals_intoPLCWidget();
	void signals_clear();
	void signals_HideWarning(int);
	void signals_ConnectSever();
public slots:
	void slots_turnPage(int current_page, int iPara = 0);
	void slots_MessageBoxMainThread(s_MSGBoxInfo msgbox);				//子线程弹出对话框
	void slots_OnBtnStar();
	void slots_OnExit(bool ifyanz=false);
	void slots_UpdateCoderNumber();
	void slots_updateCameraState(int nCam,int nMode = 0);
	void slots_SetCameraStatus(int nCam,int mode);
	void directoryChanged(QString path);
	void onServerDataReady();
	void slot_SockScreen();
	void slots_loginState(int,bool);
	void slots_ConnectServer();
	void slots_SocketStataChanged( QAbstractSocket::SocketState socketState);
public:
	//初始化
	void Initialize();
	void InitParameter();//初始化参数
	void ReadIniInformation();
	void LoadParameterAndCam();//加载参数初始化相机
	void InitGrabCard(s_GBINITSTRUCT struGrabCardPara,int index);//初始化采集卡
	void InitCam();//初始化相机
	void SetCarvedCamInfo();
	void SetCombineInfo();
	void InitCheckSet();
	void ReadCorveConfig();
	void initInterface();	//初始化界面
	int ReadImageSignal(int nImageNum,int camera=0);
	void SetLanguage(int pLang);
	bool SendDataToSever(int,StateEnum,QByteArray,bool);
public:
	QStackedWidget *statked_widget;
	WidgetTitle *title_widget;				//标题栏 
	WidgetCarveSetting *widget_carveSetting;			//剪切设置
	WidgetManagement *widget_Management;			//品种设置
	WidgetTest *test_widget;
	Widget_PLC * plc_widget;
	QWidget *widget_alg;
	LightSource* nLightSource;
	Widget_Warning *nWidgetWarning;
	QString skin_name;						//主窗口背景图片的名称
	QPixmap skin;
	UserWidget *nUserWidget;
	POINT gcPosition;
	//状态栏控件
	QList<CameraStatusLabel *> cameraStatus_list;
	QTimer *timerUpdateCoder;
	QTimer *nSockScreen;
	QTimer *nConnectTimer;
	QVector<QString> m_vstrPLCInfoType;		//错误类型
	QLabel *labelCoder;
	QWidget *stateBar;
	CLogFile Logfile;
public:
	e_CurrentMainPage m_eCurrentMainPage;
	e_CurrentMainPage m_eLastMainPage;
	int iLastPage;
	s_Permission sPermission;
	//配置信息结构体
	s_ConfigInfo m_sConfigInfo;
	s_SystemInfo m_sSystemInfo;
	s_ErrorInfo m_sErrorInfo;
	s_GBINITSTRUCT struGrabCardPara[CAMERA_MAX_COUNT];
	//运行信息结构体
	s_RunningInfo m_sRunningInfo;
	//设备维护报警结构体
	s_RuntimeInfo m_sRuntimeInfo;
	//连接服务器
	QTcpSocket * m_tcpSocket;
	//相机结构体
	s_RealCamInfo m_sRealCamInfo[CAMERA_MAX_COUNT];
	s_CarvedCamInfo m_sCarvedCamInfo[CAMERA_MAX_COUNT];
	CMyQueue nQueue[CAMERA_MAX_COUNT];	
	CDetectElement m_detectElement[CAMERA_MAX_COUNT];
	QMutex mutexDetectElement[CAMERA_MAX_COUNT];
	QMutex m_mutexmCarve[CAMERA_MAX_COUNT];
	CIOCard *m_vIOCard[IOCard_MAX_COUNT];		//IO卡队列
	CMyErrorList m_ErrorList;

	//图像综合相关参数
	CCombineRlt m_cCombine;
	QVector<QRgb> m_vcolorTable;				//生成灰度颜色表
	//算法使用的检测列表
	s_InputCheckerAry CherkerAry;
	//检测线程相关参数
	DetectThread *pdetthread[CAMERA_MAX_COUNT];
	
	BOOL m_bIsThreadDead;			//杀死线程，关闭窗口结束线程
	CBottleCheck m_cBottleCheck[CAMERA_MAX_COUNT];	// 算法类对象 [10/26/2010 GZ]
	CBottleModel m_cBottleModel;// 模板设置对话框 [10/26/2010 GZ]
	CBottleCheck m_cBottleRotate[CAMERA_MAX_COUNT];	
	CBottleCheck m_cBottleStress[CAMERA_MAX_COUNT];	
	CBottleCheck m_cBottleRotateCarve;
	QString sVersion;
	QString SaveDataPath;
	int sLanguage;
public:
	ImageSave m_SavePicture[CAMERA_MAX_COUNT];
	int surplusDays;
	HANDLE pHandles[CAMERA_MAX_COUNT];
	MyErrorType nSendData[256];
public:
	//涉及网络通信的变量
	QMutex nSocketMutex;
	int nCountNumber;
	int nLastKick;
	int nLastCheckNum;
	int nLastFailedNum;
	bool n_NetConnectState;
	char* m_ptr;
	int* nIOCard;
	QByteArray m_buffer;
	//启动外部程序
	QProcess *nSeverExe;
};
#endif // GLASSWAREDETECTSYSTEM_H

