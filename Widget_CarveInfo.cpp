#include "Widget_CarveInfo.h"
#include "glasswaredetectsystem.h"
extern GlasswareDetectSystem *pMainFrm;

Widget_CarveInfo::Widget_CarveInfo(QWidget *parent)
	:QWidget(parent)
{
	ui.setupUi(this);
	ui.widget_image->setWidgetName(tr("ImageSet"));
	toolButtonToCamera = new QToolButton(ui.widget_image);
	toolButtonToCamera->setMaximumHeight(25);
	toolButtonToCamera->setToolButtonStyle(Qt::ToolButtonIconOnly);
	toolButtonToCamera->setText(tr("CameraSet"));
	QPixmap pixmap(":/sysButton/arrow");
	toolButtonToCamera->setIcon(pixmap);
	ui.layoutImage->addWidget(ui.widget_image->widgetName, 0, Qt::AlignTop);
	ui.layoutImage->addStretch();
	ui.layoutImage->addWidget(toolButtonToCamera, 0, Qt::AlignTop);
	ui.layoutImage->setSpacing(0);
	ui.layoutImage->setContentsMargins(0, 0, 0, 0);

	ui.widget_camera->setWidgetName(tr("CameraSet"));
	toolButtonToImage = new QToolButton(ui.widget_camera);
	toolButtonToImage->setMaximumHeight(25);
	toolButtonToImage->setToolButtonStyle(Qt::ToolButtonIconOnly);
	toolButtonToImage->setText(tr("ImageSet"));
	QPixmap pixmap2(":/sysButton/arrowLeft");
	toolButtonToImage->setIcon(pixmap2);
	ui.layoutCamera->addWidget(ui.widget_camera->widgetName, 0, Qt::AlignTop);
	ui.layoutCamera->addStretch();
	ui.layoutCamera->addWidget(toolButtonToImage, 0, Qt::AlignTop);
	ui.layoutCamera->setSpacing(0);
	ui.layoutCamera->setContentsMargins(0, 0, 0, 0);

	ui.pushButton_setToCamera_2->setFocusPolicy(Qt::NoFocus);
	ui.pushButton_setToCamera->setFocusPolicy(Qt::NoFocus);
	ui.pushButton_copyROI->setFocusPolicy(Qt::NoFocus);
	ui.pushButton_save->setFocusPolicy(Qt::NoFocus);

	connect(toolButtonToCamera, SIGNAL(clicked()), this, SLOT(TrunCameraSet()));
	connect(toolButtonToImage, SIGNAL(clicked()), this, SLOT(TrunImageSet()));
	connect(ui.pushButton_setToCamera_2, SIGNAL(clicked()), ui.pushButton_setToCamera, SLOT(click()));
	connect(ui.pushButton_setToCamera_2, SIGNAL(clicked()), this, SLOT(slot_setToCamera()));
	ui.stackedWidget->setCurrentIndex(0);
	RepaintImage();
}

Widget_CarveInfo::~Widget_CarveInfo()
{
}
void Widget_CarveInfo::setImageHeight(int nHeight)
{ 
	axisY->setRange(0, nHeight);
	spLineSeries2->clear();
	spLineSeries3->clear();
	spLineSeries4->clear();
	spLineSeries2->append(140, 0);
	spLineSeries2->append(140, nHeight);
	spLineSeries3->append(170, 0);
	spLineSeries3->append(170, nHeight);
	spLineSeries4->append(200, 0);
	spLineSeries4->append(200, nHeight);
}
void Widget_CarveInfo::RepaintImage()
{
	//重绘曲线图
	ui.CharViewWidget->setRenderHint(QPainter::Antialiasing);
	//ui.CharViewWidget->chart()->setTheme(QChart::ChartThemeLight);
	//增加参考线
	spLineSeries1 = new QSplineSeries;
	spLineSeries2 = new QSplineSeries;
	spLineSeries3 = new QSplineSeries;
	spLineSeries4 = new QSplineSeries;
	spLineSeries2->setColor(Qt::green);
	spLineSeries3->setColor(Qt::green);
	spLineSeries4->setColor(Qt::green);
	
	spLineSeries1->setName(QString::fromLocal8Bit("平均灰度"));
	spLineSeries4->setName(QString::fromLocal8Bit("参考线"));
	axisX = new QValueAxis;
	axisX->setRange(110,230);
	axisX->setTickCount(5);
	axisX->setMinorTickCount(5);
	axisY = new QValueAxis;
	axisY->setRange(0, 8);
	//axisX->setTitleText("X Gray"); //设置X轴的标题
	axisX->setLabelFormat("%d");
	axisY->setLabelFormat("%d");
	ui.CharViewWidget->chart()->addSeries(spLineSeries1);
	ui.CharViewWidget->chart()->addSeries(spLineSeries2);
	ui.CharViewWidget->chart()->addSeries(spLineSeries3);
	ui.CharViewWidget->chart()->addSeries(spLineSeries4);
	ui.CharViewWidget->chart()->addAxis(axisX, Qt::AlignBottom);
	ui.CharViewWidget->chart()->addAxis(axisY, Qt::AlignLeft);
	spLineSeries1->attachAxis(axisX);
	spLineSeries1->attachAxis(axisY);
	spLineSeries2->attachAxis(axisX);
	spLineSeries2->attachAxis(axisY);
	spLineSeries3->attachAxis(axisX);
	spLineSeries3->attachAxis(axisY);
	spLineSeries4->attachAxis(axisX);
	spLineSeries4->attachAxis(axisY);
	ui.CharViewWidget->chart()->legend()->setAlignment(Qt::AlignBottom);
}
void Widget_CarveInfo::slot_setToCamera()
{
	ui.pushButton_setToCamera_2->setEnabled(false);
	QTimer::singleShot(2000,this,SLOT(slot_SetSaveStatus()));
}
void Widget_CarveInfo::slot_SetSaveStatus()
{
	ui.pushButton_setToCamera_2->setEnabled(true);
}

void Widget_CarveInfo::TrunCameraSet()
{
	ui.stackedWidget->setCurrentIndex(1);
}
void Widget_CarveInfo::TrunImageSet()
{
	int temp = pMainFrm->widget_carveSetting->pStackedCarve->currentIndex();
	if(pMainFrm->widget_carveSetting->listWidgetCarveImage[temp]->iIsTestGrey)
	{
		pMainFrm->widget_carveSetting->listWidgetCarveImage[temp]->slots_grey();
		ui.stackedWidget->setCurrentIndex(1);
	}else{
		ui.stackedWidget->setCurrentIndex(0);
	}
}