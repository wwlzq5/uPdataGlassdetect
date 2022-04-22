#ifndef WIDGET_CARVEINFO_H
#define WIDGET_CARVEINFO_H

#include <QWidget>
#include <QToolButton>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE
#include "ui_Widget_CarveInfo.h"
#include <QValueAxis>
class Widget_CarveInfo : public QWidget
{
	Q_OBJECT

public:
	Widget_CarveInfo(QWidget *parent = 0);
	~Widget_CarveInfo();
	void RepaintImage();
private slots:
	//void slots_craveImg();					//ºÙ«–
//	void slots_ShowActiveImage(int);		//œ‘ æ
signals:
	void signals_craveImg();					//ºÙ«–
public slots:
	void TrunCameraSet();
	void TrunImageSet();
	void slot_setToCamera();
	void slot_SetSaveStatus();
private:
	QToolButton *toolButtonToCamera;
	QToolButton *toolButtonToImage;
public:
	QSplineSeries *spLineSeries1;
	QSplineSeries *spLineSeries2;
	QSplineSeries *spLineSeries3;
	QSplineSeries *spLineSeries4;
	QValueAxis *axisX;
	QValueAxis *axisY;
public:
	void setImageHeight(int);
public:
	Ui::Widget_CarveInfo ui;
};

#endif // WIDGET_CARVEINFO_H
