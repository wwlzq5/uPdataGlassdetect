#include "LightSource.h"
#include <QLibrary>
#include <qDebug>
#include "glasswaredetectsystem.h"
extern GlasswareDetectSystem *pMainFrm;
LightSource::LightSource(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initLight();
	initConfig();
}

LightSource::~LightSource()
{

}
void LightSource::closeEvent(QCloseEvent *event)
{
	//关闭连接
	//slot_cancel();
}
void LightSource::initLight()
{
	signal_mapper = new QSignalMapper(this);
	for(int i=0;i<60;i++)
	{
		QPushButton *test = new QPushButton();
		connect(test,SIGNAL(clicked()),signal_mapper,SLOT(map()));
		signal_mapper->setMapping(test,i);
		test->setMinimumWidth(20);
		test->setMinimumHeight(150);
		test->setText(QString("%1").arg(i+1));
		test->setStyleSheet("background-color:gray;border:1px solid rgb(96,96,96)");
		ui.horizontalLayout->addWidget(test);
		ui.horizontalLayout->setSpacing(0);
		nBrightness[i] = 50;
		nListButton.push_back(test);
	}
	ui.checkBox->setStyleSheet("QCheckBox::indicator {width:15px;height:15px;border-radius:7px}""QCheckBox::indicator:checked {background-color:green;}""QCheckBox::indicator:unchecked {background-color:red;}");
	connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(slots_clickAccont(int)));
	connect(ui.horizontalSlider,SIGNAL(valueChanged(int)), ui.spinBox, SLOT(setValue(int)));
	connect(ui.spinBox, SIGNAL(valueChanged(int)), ui.horizontalSlider, SLOT(setValue(int)));
	connect(ui.pushButton_5,SIGNAL(clicked()),this,SLOT(slot_login()));
	connect(ui.pushButton_6,SIGNAL(clicked()),this,SLOT(slot_cancel()));
	connect(ui.radioButton,SIGNAL(toggled(bool)),this,SLOT(slot_radioButton(bool)));
	signal_mapper2 = new QSignalMapper(this);
	connect(ui.pushButton,SIGNAL(clicked()),signal_mapper2,SLOT(map()));
	connect(ui.pushButton_2,SIGNAL(clicked()),signal_mapper2,SLOT(map()));
	connect(ui.pushButton_3,SIGNAL(clicked()),signal_mapper2,SLOT(map()));
	connect(ui.pushButton_4,SIGNAL(clicked()),signal_mapper2,SLOT(map()));
	connect(ui.pushButton_7,SIGNAL(clicked()),signal_mapper2,SLOT(map()));
	signal_mapper2->setMapping(ui.pushButton,1);
	signal_mapper2->setMapping(ui.pushButton_2,2);
	signal_mapper2->setMapping(ui.pushButton_3,3);
	signal_mapper2->setMapping(ui.pushButton_4,4);
	signal_mapper2->setMapping(ui.pushButton_7,5);
	connect(signal_mapper2, SIGNAL(mapped(int)), this, SLOT(slot_setPushbotton(int)));
	ui.radioButton->setChecked(false);
	ui.pushButton_6->setVisible(false);
	ui.pushButton_3->setVisible(false);
}

void LightSource::initConfig()
{
	nPositon =0;
	nNetID = 0;
	nStyle = 0;
	nState = 0;
	setWindowIcon(QIcon("./Resources/LOGO.png"));
	setWindowTitle(tr("Light set"));
	char* szStr = "LED_DIST.dll";
	WCHAR szTest[256];
	memset(szTest,0,sizeof(szTest));
	MultiByteToWideChar(CP_ACP,0,szStr,strlen(szStr)+1,szTest,sizeof(szTest)/sizeof(szTest[0]));
	HMODULE hdll = LoadLibrary(szTest);
	nLed_Connect = (LED_Connect)GetProcAddress(hdll,"LED_DIST_Connect");
	nLed_readInfo = (LED_DIST_New_ReadInfo)GetProcAddress(hdll,"LED_DIST_New_ReadInfo");
	nled_disconnect = (LED_DIST_Disconnect)GetProcAddress(hdll,"LED_DIST_Disconnect");
	nled_readAll = (LED_DIST_New_ReadAll)GetProcAddress(hdll,"LED_DIST_New_ReadAll");
	//nled_setArea = (LED_DIST_New_SetArea)GetProcAddress(hdll,"LED_DIST_New_SetArea");
	nled_setAll = (LED_DIST_New_SetAll)GetProcAddress(hdll,"LED_DIST_New_SetAll");
	nled_SelectStyle = (LED_DIST_New_SelectStyle)GetProcAddress(hdll,"LED_DIST_New_SelectStyle");
	nled_SetLightTime = (LED_DIST_New_SetLightTime)GetProcAddress(hdll,"LED_DIST_New_SetLightTime");
	nled_SaveAll = (LED_DIST_New_SaveAll)GetProcAddress(hdll,"LED_DIST_New_SaveAll");
}
void LightSource::slot_login()
{
	QString str=ui.lineEdit_2->text(); //192.168.250.103
	QByteArray ba = str.toLatin1();
	char* ipAddress=ba.data();
	nNetID = nLed_Connect(ipAddress, 8234);
	if(nNetID == 0)
	{
		QMessageBox::information(this,tr("Infomation"),tr("login failed"));;
		return;
	}
	ui.pushButton_6->setVisible(true);
	ui.pushButton_5->setVisible(false);
	ui.checkBox->setChecked(true);
	nState = 1;
	getLight();
	getStyle();
	QMessageBox::information(this,tr("Infomation"),tr("login success"));
}
void LightSource::slot_cancel()
{
	nled_disconnect(nNetID);
	ui.pushButton_6->setVisible(false);
	ui.pushButton_5->setVisible(true);
	ui.checkBox->setChecked(false);
	nListButton[nPositon]->setStyleSheet("background-color:gray;border:1px solid rgb(96,96,96)");//background-color:green
	nState = 0;
}
void LightSource::slots_clickAccont(int i)
{
	//设置按钮样式
	if(nPositon != i)
	{
		if(nBrightness[nPositon]!=0)
		{
			nListButton[nPositon]->setStyleSheet("background-color:green");//background-color:green
		}else{
			nListButton[nPositon]->setStyleSheet("background-color:gray");//background-color:green
		}
		nPositon = i;
	}
	//nListButton[i]->setStyleSheet("border:4px solid rgb(0,0,255)");//background-color:green
	ui.spinBox->setValue(nBrightness[i]);
	if(nBrightness[i]!=0)
	{
		nListButton[i]->setStyleSheet("background-color:green;border:4px solid rgb(0,0,255)");
	}else{
		nListButton[i]->setStyleSheet("background-color:gray;border:4px solid rgb(0,0,255)");
	}
	//获取某个节点的配置

}
void LightSource::slot_radioButton(bool test)
{
	if(test)
	{
		nStyle = 1;
	}else{
		nStyle = 2;
	}
	getStyle();
}
void LightSource::slot_setPushbotton(int i)
{
	switch (i)
	{
	case 1://恢复默认配置
		if(ui.radioButton->isChecked())
		{
			for(int i=0;i<60;i++)
			{
				nBrightness[i]=50;
			}
		}else{
			for(int i=0;i<60;)
			{
				nBrightness[i] = 50;
				nBrightness[i+1] = 50;
				nBrightness[i+2] = 0;
				nBrightness[i+3] = 0;
				i+=4;
			}
		}
		getStyle();
	case 4:
		SaveParameter();
		break;
	case 5:
		nBrightness[nPositon] = ui.spinBox->text().toInt();//保存单区域的亮度等级
		if(nBrightness[nPositon]!=0)
		{
			nListButton[nPositon]->setStyleSheet("background-color:green;border:4px solid rgb(0,0,255)");
		}else{
			nListButton[nPositon]->setStyleSheet("background-color:gray;border:4px solid rgb(0,0,255)");
		}
		break;
	default:
		break;
	}
}
void LightSource::intoLightSource(int icarema)
{
	int normalCamera = pMainFrm->m_sCarvedCamInfo[icarema].m_iToRealCamera+1;
	int nTemp = 101;
	if(pMainFrm->m_sSystemInfo.iRealCamCount==12)
	{
		if(normalCamera == 1 || normalCamera == 2 || normalCamera == 7 || normalCamera == 8)
		{
			nTemp = 101;
		}
		if(normalCamera == 3 || normalCamera == 4 || normalCamera == 9 || normalCamera == 10)
		{
			nTemp = 102;
		}
		if(normalCamera == 5 || normalCamera == 6 || normalCamera == 11 || normalCamera == 12)
		{
			nTemp = 103;
		}
	}else if(pMainFrm->m_sSystemInfo.iRealCamCount == 15){
		if(normalCamera == 1 || normalCamera == 2 || normalCamera == 3 || normalCamera == 10 || normalCamera == 11)
		{
			nTemp = 101;
		}
		if(normalCamera == 4 || normalCamera == 5 || normalCamera == 6 || normalCamera == 12 || normalCamera == 13)
		{
			nTemp = 102;
		}
		if(normalCamera == 7 || normalCamera == 8 || normalCamera == 9 || normalCamera == 14 || normalCamera == 15)
		{
			nTemp = 103;
		}
	}else{
		if(normalCamera == 1 || normalCamera == 2 || normalCamera == 3 || normalCamera == 10 || normalCamera == 11 || normalCamera == 12)
		{
			nTemp = 101;
		}
		if(normalCamera == 4 || normalCamera == 5 || normalCamera == 6 || normalCamera == 13 || normalCamera == 14 || normalCamera == 15)
		{
			nTemp = 102;
		}
		if(normalCamera == 7 || normalCamera == 8 || normalCamera == 9 || normalCamera == 16 || normalCamera == 17 || normalCamera == 18)
		{
			nTemp = 103;
		}
	}
	if(pMainFrm->m_sSystemInfo.m_iSystemType == 3)
	{
		nTemp += 3;
	}
	ui.lineEdit_2->setText(QString("192.168.250.")+QString::number(nTemp));
}
void LightSource::SaveParameter()
{
	if(nState)
	{
		nled_setAll(nNetID,nStyle,nBrightness);//设置所有区域亮度等级
		nled_SetLightTime(nNetID,ui.lineEdit->text().toInt());//设置点亮时长
		nled_SelectStyle(nNetID,nStyle);//保存条纹样式
		nled_SaveAll(nNetID);//保存数据
	}
}
void LightSource::getStyle()
{
	//获取条纹样式
	char str[67];
	memset(str,0,67);
	if(67 == nled_readAll(nNetID,nStyle,str) && nState)
	{
		for(int i=4;i<64;i++)
		{
			nBrightness[i-4] = str[i];
		}
	}
	ui.spinBox->setValue(nBrightness[0]);
	for(int i=0;i<60;i++)
	{
		if(nBrightness[i]!=0)
		{
			nListButton[i]->setStyleSheet("background-color:green");
		}else{
			nListButton[i]->setStyleSheet("background-color:gray");
		}
	}
}
void LightSource::getLight()
{
	//获取光源样式
	char sTest[12];
	memset(sTest,0,12);
	if(12 == nLed_readInfo(nNetID,sTest) && nState)
	{
		nStyle = sTest[4];
		if(nStyle == 1)
		{
			ui.radioButton->setChecked(true);//样式1
		}else if(nStyle == 2)
		{
			ui.radioButton_2->setChecked(true);//样式2
		}
		if((int)sTest[6]<0)
		{
			ui.lineEdit->setText(QString::number((sTest[5]<<8) + sTest[6]+256));
		}else{
			ui.lineEdit->setText(QString::number((sTest[5]<<8) + sTest[6]));
		}
	}
}