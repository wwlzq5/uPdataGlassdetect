#include "WidgetWarning.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QLayout>
#include <windows.h>



Widget_Warning::Widget_Warning(QWidget *parent)
	: QWidget(parent)
{
	setStyleSheet(QString::fromUtf8("background-color: rgb(138,171,140);"));
	bIsShow = true;
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	labelWarningPic = new QLabel();
	QPixmap pixmapWarning(":/sys/Alert");
	labelWarningPic->setPixmap(pixmapWarning);
	labelWarningPic->setFixedSize(pixmapWarning.size());
	labelWarningInfo = new QLabel();
	QFont font("宋体",20,QFont::DemiBold,false);
	labelWarningInfo->setFont(font);
	labelWarningInfo->setText(tr("Alert Info!"));
	labelWarningInfo->setAlignment(Qt::AlignCenter);
	Init();
}

Widget_Warning::~Widget_Warning()
{
	close();
}
void Widget_Warning::Init()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);//去掉标题栏
	desktopWidget = QApplication::desktop();
	setMinimumSize(400,300);
	QVBoxLayout *layoutInfo = new QVBoxLayout();
	layoutInfo->addWidget(labelWarningInfo,Qt::AlignCenter);
	
	QHBoxLayout *layoutBody = new QHBoxLayout();
	layoutBody->addWidget(labelWarningPic,Qt::AlignCenter);
	layoutBody->addLayout(layoutInfo,Qt::AlignCenter);
	layoutBody->setContentsMargins(10,0,10,0);
	layoutBody->setStretch(0,1);
	layoutBody->setStretch(1,1);

	QVBoxLayout *layoutMain = new QVBoxLayout(this);
	layoutMain->addLayout(layoutBody);
	layoutMain->setContentsMargins(0,0,0,0);
}
bool Widget_Warning::IsShowWarning()
{
	return bIsShow;
}
void Widget_Warning::slot_ShowMessage(int warningType, QString warningInfo)
{
	
}
void Widget_Warning::showWarnning(QString warningInfo)
{
	labelWarningInfo->setText(warningInfo);
	QDesktopWidget* desktopWidget = QApplication::desktop();
	move(desktopWidget->width()-400,desktopWidget->height()-300);
	show();
	raise();
}
void Widget_Warning::hideWarnning()
{
	hide();
}
void Widget_Warning::slots_HideWidget()
{
	hide();
}
void Widget_Warning::slots_StopAlert()
{
	/*bIsShow = false;
	m_delayTime->start();
	hide();*/
}
