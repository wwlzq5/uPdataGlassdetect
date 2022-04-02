#include "widget_IoSet.h"
#include <QSettings>
#include "glasswaredetectsystem.h"
extern GlasswareDetectSystem *pMainFrm;

IOtestWidget::IOtestWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon("./Resources/LOGO.png"));
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(slots_IOShow()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(slots_IoSetPam()));
}
IOtestWidget::~IOtestWidget()
{
	//close();
}
void IOtestWidget::slots_IOShow()
{
	emit showIocard();
}
void IOtestWidget::slots_IoSetPam()
{
	if(pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
		int tempWidth = ui.lineEdit->text().toInt();
		int tempDelay = ui.lineEdit_2->text().toInt();
		emit modifyIoPam(tempWidth,tempDelay);
	}
}
void IOtestWidget::slots_showPam(int pam1,int pam2)
{
	ui.lineEdit->setText(QString::number(pam1));
	ui.lineEdit_2->setText(QString::number(pam2));
}