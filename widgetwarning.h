#ifndef Widget_Warning_H
#define Widget_Warning_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLayout>

class Widget_Warning : public QWidget
{
	Q_OBJECT

public:
	Widget_Warning(QWidget *parent = 0);
	~Widget_Warning();
	
	void Init();
 	bool IsShowWarning();
	void showWarnning(QString);
	void hideWarnning();
public slots:
	void slot_ShowMessage(int warningType, QString warningInfo);//warningType1:Ìß·ÏÂÊ±¨¾¯£º2PLC±¨¾¯
	void slots_HideWidget();
	void slots_StopAlert();
public:
	bool bIsShow;
	QDesktopWidget* desktopWidget;
	QLabel *labelWarningPic;
	QLabel *labelWarningInfo;
	QVBoxLayout *layoutMain;
	bool bHiding;
	int iWarningType;
	QTimer* m_delayTime;
};

#endif // Widget_Warning_H
