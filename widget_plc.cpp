#include "widget_plc.h"
#include <QLayout>
#include <QGroupBox>
#include <QSettings>
#include <QTextCodec>
#include <QMouseEvent>
#include <QPainter>
#include <QGridLayout>
#include <QSignalMapper>
#include "glasswaredetectsystem.h"
extern GlasswareDetectSystem *pMainFrm;
#define CUSTOMALERT 3
Widget_PLC::Widget_PLC(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.SureButton,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonsure()));
	connect(ui.pushButton_save,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonsave()));
	connect(ui.pushButton_read,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonread()));
	connect(ui.checkBox,SIGNAL(clicked()),this,SLOT(slots_showPamSet()));
	connect(ui.checkBox_2,SIGNAL(clicked()),this,SLOT(slots_showPamSet()));
	m_pSocket = new QUdpSocket();
	m_pSocket->connectToHost("192.168.250.1", 9600);
	if (m_pSocket->state() == QAbstractSocket::ConnectedState || m_pSocket->waitForConnected(2000))
	{
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(slots_readFromPLC()));
	}
	QIntValidator* IntValidator = new QIntValidator;
	IntValidator->setRange(1, 60);
	//ui.lineEdit_2->setValidator(IntValidator);
	ui.lineEdit_3->setValidator(IntValidator);
	ui.lineEdit_4->setValidator(IntValidator);
	ui.lineEdit_5->setValidator(IntValidator);
	ui.lineEdit_6->setValidator(IntValidator);
	IntValidator->setRange(1,450);
	ui.lineEdit_1->setValidator(IntValidator);
	nSystemType = pMainFrm->m_sSystemInfo.m_iSystemType;
	m_CrashTimer = new QTimer(this);
	connect(m_CrashTimer,SIGNAL(timeout()),this,SLOT(slots_CrashTimeOut()));
	m_CrashTimer->start(10000);
	//获取PLC报警信息
	nErrorType = 0;
	nErrorCameraID = 0;
	QButtonGroup* test4=new QButtonGroup(this);
	test4->addButton(ui.radioButton_9);
	test4->addButton(ui.radioButton_10);
	QGridLayout *Contentlayout = new QGridLayout(ui.scrollAreaWidgetContents);
	/////////////////////
	if(nSystemType == 2)
	{
		nAlertDataList = new int[96];
		memset(nAlertDataList,0,96*sizeof(int));
		nCustomList = new int[CUSTOMALERT*2];
		memset(nCustomList,0,CUSTOMALERT*2*sizeof(int));

		for(int i=0;i<96;i++)
		{
			QCheckBox *checkBox = new QCheckBox(this);
			nlistCheckBox<<checkBox;
		}
		QSignalMapper* signalmapper = new QSignalMapper(this);//工具栏的信号管理
		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setText(QString::fromLocal8Bit("是否报警"));//勾选表示要报警
		Contentlayout->addWidget(checkBox,0,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(slots_modify1(int)));
		QCheckBox *checkBox2 = new QCheckBox(this);
		checkBox2->setText(QString::fromLocal8Bit("是否停输送线"));//勾选表示要停止输送线
		Contentlayout->addWidget(checkBox2,0,2,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		connect(checkBox2,SIGNAL(stateChanged(int)),this,SLOT(slots_modify2(int)));
		QCheckBox *checkBox3 = new QCheckBox(this);
		checkBox3->setText(QString::fromLocal8Bit("是否停理瓶器"));//勾选表示要停止理瓶器
		Contentlayout->addWidget(checkBox3,0,3,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		connect(checkBox3,SIGNAL(stateChanged(int)),this,SLOT(slots_modify3(int)));
		for (int i = 0;i < 32;i++)
		{
			QLabel * label = new QLabel(this);
			QString ErrorName;
			if(i<pMainFrm->m_vstrPLCInfoType.count())
			{
				ErrorName = pMainFrm->m_vstrPLCInfoType[i];
				label->setText(ErrorName);
			}else{
				ErrorName = "";
				label->setText("");
			}
			connect(nlistCheckBox[i], SIGNAL(stateChanged(int)), signalmapper, SLOT(map()));
			signalmapper->setMapping(nlistCheckBox[i], i);
			connect(nlistCheckBox[i+32], SIGNAL(stateChanged(int)), signalmapper, SLOT(map()));
			signalmapper->setMapping(nlistCheckBox[i+32], i+32);
			connect(nlistCheckBox[i+64], SIGNAL(stateChanged(int)), signalmapper, SLOT(map()));
			signalmapper->setMapping(nlistCheckBox[i+64], i+64);
			if(ErrorName != "")
			{
				Contentlayout->addWidget(label,i+1,0);
				Contentlayout->addWidget(nlistCheckBox[i],i+1,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
				Contentlayout->addWidget(nlistCheckBox[i+32],i+1,2,1,1,Qt::AlignLeft | Qt::AlignVCenter);
				Contentlayout->addWidget(nlistCheckBox[i+64],i+1,3,1,1,Qt::AlignLeft | Qt::AlignVCenter);
			}else{
				label->setVisible(false);
				nlistCheckBox[i]->setVisible(false);
				nlistCheckBox[i+32]->setVisible(false);
				nlistCheckBox[i+64]->setVisible(false);
			}
		}
		connect(signalmapper, SIGNAL(mapped(int)), this, SLOT(slots_clickBox(int)));
		//增加自定义的报警
		QSignalMapper* signalmapper1 = new QSignalMapper(this);//工具栏的信号管理
		QLabel *nLabel2 = new QLabel(this);
		nLabel2->setText(QString::fromLocal8Bit("是否停输送线"));//勾选表示要停止输送线
		ui.gridLayout_12->addWidget(nLabel2,0,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		QLabel *nLabel3 = new QLabel(this);
		nLabel3->setText(QString::fromLocal8Bit("是否停理瓶器"));//勾选表示要停止理瓶器
		ui.gridLayout_12->addWidget(nLabel3,0,2,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		QLabel *nLabel11 = new QLabel(this);
		nLabel11->setText(QString::fromLocal8Bit("前壁补踢报警"));//勾选表示要报警
		ui.gridLayout_12->addWidget(nLabel11,1,0);
		QLabel *nLabel12 = new QLabel(this);
		nLabel12->setText(QString::fromLocal8Bit("夹持补踢报警"));//勾选表示要报警
		ui.gridLayout_12->addWidget(nLabel12,2,0);
		QLabel *nLabel13 = new QLabel(this);
		nLabel13->setText(QString::fromLocal8Bit("后壁补踢报警"));//勾选表示要报警
		ui.gridLayout_12->addWidget(nLabel13,3,0);
		for(int i=0;i<CUSTOMALERT*2;i++)
		{
			QCheckBox *checkBox = new QCheckBox(this);
			nCustomAlert<<checkBox;
			connect(nCustomAlert[i], SIGNAL(stateChanged(int)), signalmapper1, SLOT(map()));
			signalmapper1->setMapping(nCustomAlert[i], i);
		}
		connect(signalmapper1, SIGNAL(mapped(int)), this, SLOT(slots_AutoAlert(int)));

		for(int i=0;i<CUSTOMALERT;i++)
		{
			ui.gridLayout_12->addWidget(nCustomAlert[i],i+1,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
			ui.gridLayout_12->addWidget(nCustomAlert[i+CUSTOMALERT],i+1,2,1,1,Qt::AlignLeft | Qt::AlignVCenter);
		}
	}
	ui.scrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
	ui.scrollArea->viewport()->setStyleSheet("background-color:transparent;");
	m_PlcPicture = new Widget_PLCPicture(ui.widget_3);
	ui.gridLayout_3->addWidget(m_PlcPicture);
	connect(m_PlcPicture,SIGNAL(showSetPam()),this,SLOT(slots_HidePicture()));
	connect(this,SIGNAL(signal_updatePLCInfo(WORD)),m_PlcPicture,SLOT(slots_updatePLCInfo(WORD)));
	if(nSystemType == 2)
	{
		m_zTimer = new QTimer(this);
		connect(m_zTimer,SIGNAL(timeout()),this,SLOT(slots_TimeOut()));
		m_zTimer->start(1000);
	}
	EnableCortol();
}
Widget_PLC::~Widget_PLC()
{
	delete m_pSocket;
}
void Widget_PLC::EnableCortol()
{
	ui.widget->setVisible(false);
	ui.widget_3->setVisible(true);
	ui.lineEdit_30->setVisible(false);
	ui.label_39->setVisible(false);
	ui.lineEdit_31->setVisible(false);
	ui.label_40->setVisible(false);
}
void Widget_PLC::slots_HidePicture()
{
	ui.groupBox_2->setVisible(false);
	ui.groupBox_4->setVisible(false);
	ui.widget->setVisible(true);
	ui.widget_3->setVisible(false);
}
void Widget_PLC::slots_intoWidget()
{
	QByteArray st;
	SendPLCMessage(87,st,1,2,266);//暂时获取界面显示的所有数据2*5+2*6+8*4+8*9+4+3*4+10*8 120+80+12
	ui.checkBox->setChecked(false);
	ui.checkBox_2->setChecked(false);
	ui.widget->setVisible(false);
	ui.widget_3->setVisible(true);
}
void Widget_PLC::slots_showPamSet()
{
	if(ui.checkBox->isChecked())
	{
		ui.groupBox_2->setVisible(true);
	}else{
		ui.groupBox_2->setVisible(false);
	}
	if(ui.checkBox_2->isChecked())
	{
		ui.groupBox_4->setVisible(true);
	}else{
		ui.groupBox_4->setVisible(false);
	}
}
void Widget_PLC::slots_modify1(int temp)
{
	if(temp == 2)
	{
		for (int i = 0;i < 32;i++)
		{
			nlistCheckBox[i]->setChecked(true);
		}
	}else if(temp == 0)
	{
		for (int i = 0;i < 32;i++)
		{
			nlistCheckBox[i]->setChecked(false);
		}
	}
}
void Widget_PLC::slots_modify2(int temp)
{
	if(temp == 2)
	{
		for (int i = 32;i < 64;i++)
		{
			nlistCheckBox[i]->setChecked(true);
		}
	}else if(temp == 0)
	{
		for (int i = 32;i < 64;i++)
		{
			nlistCheckBox[i]->setChecked(false);
		}
	}
}
void Widget_PLC::slots_modify3(int temp)
{
	if(temp == 2)
	{
		for (int i = 64;i < 96;i++)
		{
			nlistCheckBox[i]->setChecked(true);
		}
	}else if(temp == 0)
	{
		for (int i = 64;i < 96;i++)
		{
			nlistCheckBox[i]->setChecked(false);
		}
	}
}
void Widget_PLC::slots_AutoAlert(int temp)
{
	if(nCustomList[temp] == 1)
	{
		nCustomList[temp] = 0;
	}else{
		nCustomList[temp] = 1;
	}
}
void Widget_PLC::slots_clickBox(int mTemp)
{
	if(nAlertDataList[mTemp]==1)
	{
		nAlertDataList[mTemp]=0;
	}else{
		nAlertDataList[mTemp]=1;
	}
}
void Widget_PLC::slots_Pushbuttonread()
{
	QByteArray st;
	SendPLCMessage(20,st,1,1,40);
}
void Widget_PLC::slots_CrashTimeOut()
{
	QByteArray st;
	int zTest = 1;
	if(nErrorCameraID)
	{
		int test = 1;
		DataToByte(test,st);
	}else{
		int test = 0;
		DataToByte(test,st);
	}
	DataToByte(zTest,st);
	if(nSystemType == 1)
	{
		SendPLCMessage(500,st,2,1,8);
	}else if(nSystemType == 2){
		SendPLCMessage(504,st,2,1,8);
	}else if(nSystemType == 3){
		SendPLCMessage(508,st,2,1,8);
	}
}
void Widget_PLC::slots_TimeOut()
{
	//获取PLC的报警信息
	QByteArray st;
	SendPLCMessage(0,st,1,1,8);//读取报警数据
}
void Widget_PLC::SendDataToPLCHead(int address, QByteArray& st, int state,int id,int DataSize) //参数1为相机ID号，参数2为组装后的数据，参数3为读写状态,参数4为通道ID(可以为任意整数),参数5为数据大小
{
	QByteArray v_szTmp;
	v_szTmp.append(QChar(0x80).toLatin1());//ICF  display frame information
	v_szTmp.append(QChar(0x00).toLatin1());//RSV  reserved by system
	v_szTmp.append(QChar(0x02).toLatin1());//GCT  permissible number of gateways
	v_szTmp.append(QChar(0x00).toLatin1());//DNA  destination network address
	v_szTmp.append(QChar(0x01).toLatin1());//DA1  destination node address
	v_szTmp.append(QChar(0x00).toLatin1());//DA2  destination unit address
	v_szTmp.append(QChar(0x00).toLatin1());//SNA  source network address
	v_szTmp.append(QChar(0x02).toLatin1());//SA1  source node address
	v_szTmp.append(QChar(0x00).toLatin1());//SA2  source unit address
	v_szTmp.append(id);//SID  service id   预先计划赋值sendId
	v_szTmp.append(QChar(0x01).toLatin1());//MRC  main request code
	v_szTmp.append(state);//SRC  sub request code
	v_szTmp.append(QChar(0xB2).toLatin1());//H区都是B2,D区是82
	v_szTmp.append(address/256); //内存地址
	v_szTmp.append(address%256); //内存地址
	v_szTmp.append(QChar(0x00).toLatin1());
	v_szTmp.append(DataSize/2/256);//数据长度
	v_szTmp.append(DataSize/2%256);//数据长度
	v_szTmp.append(st);
	st = v_szTmp;
}
void Widget_PLC::SendPLCMessage(int address,QByteArray& send,int state,int id,int DataSize) //异步发送数据改变PLC参数
{
	SendDataToPLCHead(address,send,state,id,DataSize);
	if (m_pSocket->state() == QAbstractSocket::ConnectedState)
	{
		if (NULL != m_pSocket)
		{
			m_pSocket->write(send);
		}
	}
}
void Widget_PLC::slots_readFromPLC()
{
	QByteArray v_receive = m_pSocket->readAll();
	if (v_receive.size() == 280)//242+12+4+10+12
	{
		double v_douTemp = 0;
		int v_Itmp = 0;
		int v_bit = 14;
		int j=0;
		WORD v_Itmps=0;
		for (;v_bit<18;v_bit+=2)
		{
			v_Itmps=0;
			ByteToData(v_receive,v_bit,v_bit+1,v_Itmps);
			for(int i=0;i<CUSTOMALERT;i++)
			{
				if(v_Itmps >> i & 0x01)
				{
					nCustomAlert[j]->setChecked(true);
				}else{
					nCustomAlert[j]->setChecked(false);
				}
				j++;
			}
		}
		j=0;
		v_Itmps=0;
		ByteToData(v_receive,v_bit,v_bit+1,v_Itmps);//H89
		if(v_Itmps)
		{
			ui.radioButton->setChecked(true);
			ui.radioButton_3->setChecked(false);
		}else{
			ui.radioButton->setChecked(false);
			ui.radioButton_3->setChecked(true);
		}
		v_bit+=6;//24 
		for (;v_bit<36;v_bit+=2)//24+12
		{
			v_Itmps=0;
			ByteToData(v_receive,v_bit,v_bit+1,v_Itmps);
			for(int i=0;i<16;i++)
			{
				if(v_Itmps >> i & 0x01)
				{
					nlistCheckBox[j]->setChecked(true);
				}else{
					nlistCheckBox[j]->setChecked(false);
				}
				j++;
			}
		}
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp); //H98
		ui.lineEdit_21->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_1->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		if(v_Itmp == 0)
		{
			ui.radioButton_1->setChecked(true);
			ui.radioButton_2->setChecked(false);
		}else{
			ui.radioButton_1->setChecked(false);
			ui.radioButton_2->setChecked(true);
		}
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_2->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_3->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_4->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_5->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_6->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_7->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_9->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_10->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_11->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_12->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_14->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_15->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_17->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_19->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		if(v_Itmp == 0)
		{
			ui.radioButton_9->setChecked(false);
			ui.radioButton_10->setChecked(true);
		}else{
			ui.radioButton_9->setChecked(true);
			ui.radioButton_10->setChecked(false);
		}
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_22->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_23->setText(QString::number(v_Itmp*0.01,'f',2));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_24->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_25->setText(QString::number(v_douTemp,'f',2));//主动轮齿数应该没有小数
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_27->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);//输送线电机最大频率
		ui.lineEdit_26->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_28->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_29->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_30->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_31->setText(QString::number(v_douTemp,'f',2));//皮带节距
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_32->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_33->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_34->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_35->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_36->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_37->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_38->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_39->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.comboBox->setCurrentIndex(v_Itmp);
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.comboBox_2->setCurrentIndex(v_Itmp);
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.comboBox_3->setCurrentIndex(v_Itmp);
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_40->setText(QString::number(v_Itmp));
		v_bit+=4;
		QString VeSION;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		VeSION += QString::number(v_Itmp)+".";
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		VeSION += QString::number(v_Itmp)+".";
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		VeSION += QString::number(v_Itmp);
		ui.label_54->setText(QString::fromLocal8Bit("PLC版本:")+VeSION);
	}else if(v_receive.size() == 22)
	{
		WORD v_Itmp=0;
		int j=0;
		int m_byte=14;
		bool Asert = true;
		for (;m_byte<20;m_byte+=2)
		{
			ByteToData(v_receive,m_byte,m_byte+1,v_Itmp);
			for(int i=0;i<16;i++)
			{
				if(v_Itmp >> i & 0x01)
				{
					nErrorType = j;
					Asert = false;
					//pMainFrm->Logfile.write(QString("send %1").arg(j),AbnormityLog);
				}
				j++;
			}
		}
		if(Asert)
		{
			nErrorType = -1;
		}
		ByteToData(v_receive,m_byte,m_byte+1,v_Itmp);
		emit signal_updatePLCInfo(v_Itmp);
	}else if(v_receive.size() == 54)//14+40
	{
		double v_douTemp;
		int v_bit=14;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_8->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_13->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_16->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_18->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_20->setText(QString::number(v_douTemp,'f',2));
	}
}
void Widget_PLC::slots_Pushbuttonsure()
{
	QByteArray st;
	WORD TempData = 3;
	DataToByte(TempData,st);
	SendPLCMessage(90,st,2,1,2);//写入指令，命令下发完毕后再写一次
}
void Widget_PLC::SendCustomAlert(int temp,int nData)
{
	QByteArray st;
	WORD TempData=0;
	if(nData == 0)
	{
		TempData=0;
	}else{
		if(temp == 1)
		{
			TempData = 1;
		}else if(temp == 2)
		{
			TempData = 2;
		}else{
			TempData = 4;
		}
	}
	DataToByte(TempData,st);
	SendPLCMessage(2,st,2,1,2);
}
void Widget_PLC::slots_Pushbuttonsave()
{
	QByteArray st;
	WORD test = 1;
	WORD nData[6];
	memset(nData,0,sizeof(WORD)*6);
	for(int i=0;i<CUSTOMALERT*2;i++)
	{
		if(i<CUSTOMALERT)
		{
			if(nCustomList[i])
			{
				nData[0] += test<<i;
			}
		}else 
		{
			if(nCustomList[i])
			{
				nData[1] += test<<(i-CUSTOMALERT);
			}
		}
	}
	if(ui.radioButton->isChecked())
	{
		nData[2]=1;
	}
	for(int i=0;i<5;i++)
	{
		DataToByte(nData[i],st);
	}
	memset(nData,0,sizeof(WORD)*6);
	for(int i=0;i<96;i++)
	{
		if(i<32)
		{
			if(nAlertDataList[i])
			{
				if(i<16)
				{
					nData[0] += test<<i;
				}else{
					nData[1] += test<<(i-16);
				}
			}
		}else if(i>=32&&i<64)
		{
			if(nAlertDataList[i])
			{
				if(i<48)
				{
					nData[2] += test<<(i-32);
				}else{
					nData[3] += test<<(i-48);
				}
			}
		}else{
			if(nAlertDataList[i])
			{
				if(i<80)
				{
					nData[4] += test<<(i-64);
				}else{
					nData[5] += test<<(i-80);
				}
			}
		}
	}
	for(int i=0;i<6;i++)
	{
		DataToByte(nData[i],st);
	}
	int TempData = 0;
	TempData = ui.lineEdit_21->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_1->text().toInt();
	DataToByte(TempData,st);
	if(ui.radioButton_1->isChecked())
	{
		TempData = 0;
	}else{
		TempData = 1;
	}
	DataToByte(TempData,st);
	TempData = ui.lineEdit_2->text().toDouble()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_3->text().toDouble()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_4->text().toDouble()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_5->text().toDouble()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_6->text().toDouble()*100;
	DataToByte(TempData,st);
	double TempSpeed = ui.lineEdit_7->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_9->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_10->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_11->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_12->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_14->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_15->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_17->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_19->text().toDouble();
	DataToByte(TempSpeed,st);
	if(ui.radioButton_9->isChecked())
	{
		TempData = 1;
	}else{
		TempData = 0;
	}
	DataToByte(TempData,st);//8*9+9*4+2*6 72+36+12=120

	TempData = ui.lineEdit->text().toInt()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_22->text().toDouble()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_23->text().toDouble()*100;
	DataToByte(TempData,st);
	TempSpeed = ui.lineEdit_24->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_25->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_27->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_26->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_28->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_29->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_30->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_31->text().toDouble();
	DataToByte(TempSpeed,st);
	TempData = ui.lineEdit_32->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_33->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_34->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_35->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_36->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_37->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_38->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_39->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.comboBox->currentIndex();
	DataToByte(TempData,st);
	TempData = ui.comboBox_2->currentIndex();
	DataToByte(TempData,st);
	TempData = ui.comboBox_3->currentIndex();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_40->text().toInt();
	DataToByte(TempData,st);
	//总数
	QString PLCVersion = ui.label_54->text();
	QStringList list = PLCVersion.split(".");
	for(int i=0;i<list.count();i++)
	{
		TempData = list[i].toInt();
		DataToByte(TempData,st);
	}
	SendPLCMessage(87,st,2,1,266);//120+44+64=244+10
}
template<typename T>
void Widget_PLC::DataToByte(T& xx, QByteArray& st)
{
	//st.clear();
	char nChar = 0;
	char* f_pshort = reinterpret_cast<char*>(&xx);
	for (int i = 0; i < sizeof(T); ++i)
	{
		if (i % 2 == 0)
		{
			nChar = (char)(*(f_pshort + i + 1));
		}
		else
		{
			nChar = (char)(*(f_pshort + i - 1));
		}
		st.append(nChar);
	}
}
template<typename T>
void Widget_PLC::ByteToData(QByteArray& st, int nStart, int nEnd,T& xt)
{
	xt = 0;
	char* f_pshort = reinterpret_cast<char*>(&xt);
	for (int i = nStart, j = 0; i <= nEnd; ++i, ++j)
	{
		if (i % 2 == 0)
		{
			*(f_pshort + j) = st[i + 1];
		}
		else
		{
			*(f_pshort + j) = st[i - 1];
		}
	}
}