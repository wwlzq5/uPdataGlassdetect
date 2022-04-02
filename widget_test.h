#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>
#include <QTimer>
#include "widget_errortype.h"
#include "widget_camera.h"
#include "ui_widget_test.h"
#include "camerasatuslabel.h"
#include "CIOCard.h"
#include "widget_IoSet.h"
enum StateEnum
{
	SENDDATA,
	GETSUCCESS,
	CLEAR,
	CONNECT,
	LOCKSCREEN,
	ALERT,
	SEVERS,//服务器
	IMAGE,
	NLEADING,
	NCLAMPING,
	NBACKING,
	MAININTERFACE,
	SYSTEMMODEADD,
	SYSTEMMODESELECT,
	SYSTEMMODEDELTE,
	ONLYSHOWSEVER,
	FRONTSTATE
};
enum UnitEnum
{
	LEADING, //前端
	CLAMPING,//夹持
	BACKING,//后端
};
struct MyStruct
{
	StateEnum nState;//发送状态标志位
	UnitEnum nUnit;//发送的设备标志位
	int nCount;//前端通过in0得到的过检总数
	int nFail;//前端通过out3得到的踢废总数
	char nTemp[256];//前端的多余变量
};
struct NingInfo
{
	int m_checkedNum;
	int m_passNum;
	int m_failureNum;
	int m_checkedNum2;
	NingInfo()
	{
		m_checkedNum = 0;
		m_passNum = 0;
		m_failureNum = 0;
		m_checkedNum2 = 0;
	}
};
class WidgetTest : public QWidget
{
	Q_OBJECT

public:

	WidgetTest(QWidget *parent = 0);
	~WidgetTest();
	Ui::WidgetTest ui;

	Widget_ErrorType *widget_ErrorType;
	Widget_Camera *widget_Camera;
public:
	void init();
	void initEquipAlarmTablewidget();
	void initWidgetName();
	bool leaveWidget();
	void updateIOCardParam();
	void getIOCardParam();
//	void InitIOCardInfo();
	void initInformation();
signals:
	void signals_changeStatisMode(int iStatisMode,int iMinute,int iStatisNumber);
	void signals_ReadDistance1();
	void signals_sendAlarm(int warningType, QString warningInfo);
	void signals_ShowWarning(int , QString );
	void signal_UsualSend(int);
	void signal_ioSetPam(int,int);
public slots:
	void slot_ConnectSever();
	void slot_readIoCard();
	void slots_IoSetPam(int,int);
	void slots_IoOpenPam();
	void slot_openPlcSet();
	void slots_intoWidget();
	void slots_ChoseCamera();
	void slots_ChoseErrorType();
	void slots_OKSave();
	void slots_Cancel();
	void slots_KickModeChanged(int iMode);

	void slots_SaveModeChanged(int index);
	void slots_readDelay();
	void slots_setToCard();
	void slots_setToFile();
	void slots_advance1();

	void slots_choseAllCamera();
	void slots_choseNoneCamera();
	void slots_choseAllErrorType();
	void slots_choseNoneErrorType();

	void slots_updateIOcardCounter();
	void slots_ifCheckShowImage();
	void slots_CameraOffAreet();
	void slots_EquipAlarmCheckBox(bool);
	void slots_EquipAlarmInfoShowbtn(bool);
	void slots_EquipAlarmSave();
	void slots_EquipAlarmClear();
	void slots_SetEquipAlarmSatus(int,bool);
	void slots_UpdateEquipRemainDays(int,int);
	
private:
	int iSaveMode;
	int iLastSaveMode;
	int iKickMode;
	int iLastKickMode;
	int iSaveImgCount;
	int iLastSaveImgCount;
	
	int iChoseCamera;
	int iChoseErrorType;
	
	int m_nDelay1;
	int m_nDelay2;
	int m_nDelay3;
	int m_nDelay4;
	int m_nDelay5;
	int m_nDelay6;
	int m_nKickDelay;
	int m_nKickWidth;
	int m_nSampleDelay;
	int m_nSampleWidth;

	QTimer* timerUpdateIOCardCounter;
	QTimer* CameraOffAreet;

	QList<CameraStatusLabel*> m_EquipAlarmStatusList;
	
public:
	QMutex nTestCounter;
	CIOCard* m_vIOCard;
	QTimer* nReadIOcard;
	//QWidget* nIOtest;
	IOtestWidget* nIotest;
	s_ConfigIOCardInfo m_sSystemInfo1;
public:
	NingInfo nInfo;
	//QConsole* nConsole;
	int iIOCardOffSet;
	int ifshowImage;
	int CameraOFF[CAMERA_MAX_COUNT];
};

#endif // TESTWIDGET_H