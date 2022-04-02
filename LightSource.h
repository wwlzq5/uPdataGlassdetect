#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#include "ui_LightSource.h"
#include <QPushButton>
#include <QSignalMapper>
#include <QList>
typedef unsigned int (*LED_Connect)(char*, int);
typedef unsigned int (*LED_DIST_New_ReadInfo)(unsigned int,char*);
typedef int (*LED_DIST_Disconnect)(unsigned int);
typedef int (*LED_DIST_New_ReadAll)(unsigned int,int,char*);
//typedef int (*LED_DIST_New_SetArea)(unsigned int,int,int,int);
typedef int (*LED_DIST_New_SetAll)(unsigned int,int,int*);
typedef int (*LED_DIST_New_SelectStyle)(unsigned int,int);
typedef int (*LED_DIST_New_SetLightTime)(unsigned int,int);
typedef int (*LED_DIST_New_SaveAll)(unsigned int);
class LightSource : public QWidget
{
	Q_OBJECT

public:
	LightSource(QWidget *parent = 0);
	~LightSource();
public:
	void initLight();
	void initConfig();
	void getLight();
	void getStyle();
	void closeEvent(QCloseEvent *event);
	void SaveParameter();
	void intoLightSource(int);
public slots:
	void slots_clickAccont(int);
	//void slot_setValue(int);
	void slot_radioButton(bool);
	void slot_setPushbotton(int);
	void slot_login();
	void slot_cancel();
private:
	Ui::Light_Source ui;
public:
	LED_Connect nLed_Connect;
	LED_DIST_New_ReadInfo nLed_readInfo;
	LED_DIST_Disconnect nled_disconnect;
	LED_DIST_New_ReadAll nled_readAll;
	//LED_DIST_New_SetArea nled_setArea;
	LED_DIST_New_SetAll nled_setAll;
	LED_DIST_New_SelectStyle nled_SelectStyle;
	LED_DIST_New_SetLightTime nled_SetLightTime;
	LED_DIST_New_SaveAll nled_SaveAll;
public:
	QSignalMapper* signal_mapper;
	QSignalMapper* signal_mapper2;
	QList<QPushButton*> nListButton;
	int nPositon;
	int nStyle;
	int nState;
	unsigned int nNetID;
	int nBrightness[60];
};
#endif // DEBUG
