#ifndef WIDGETIOSET_H
#define WIDGETIOSET_H

#include <QWidget>
#include <QMessageBox>
#include "ui_iowidget.h"

class IOtestWidget : public QWidget
{
	Q_OBJECT

public:
	IOtestWidget(QWidget *parent = 0);
	~IOtestWidget();
public slots:
	void slots_IOShow();
	void slots_IoSetPam();
	void slots_showPam(int,int);
signals:
	void showIocard();
	void modifyIoPam(int,int);
public:
	Ui::IOtest ui;
};

#endif