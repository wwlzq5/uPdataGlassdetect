#ifndef WIDGET_REPORT_H
#define WIDGET_REPORT_H
#include "ui_Report.h"
#include "qstandarditemmodel.h"
#include "widget_pie.h"
#include <QList>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QAxObject>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QThread>
#include "database.h"
#include "ExcelObject.h"

struct TData{
	int m_type;
	int m_Count;
	int m_failCount;
	int m_AllCheckCount;
};

class ExportExcelThread :public QThread
{
	Q_OBJECT
public:
	ExportExcelThread(QObject *parent = 0);
	~ExportExcelThread();

	void run();
	void InitShiftTime(QTime pShift1,QTime pShift2,QTime pShift3);

public:
	QDate m_Date;
	DataBase* m_database;

private:
	QList<QTime> m_shifts;
	int curShift;
	bool iShowAllnum;
};

class WidgetReport : public QWidget
{
	Q_OBJECT

public:
	WidgetReport(QWidget *parent = 0,int temp =0);
	~WidgetReport();
public:
	QStandardItemModel *m_modelCount;
	Ui::widget_Report ui;
	PieView *pieImage;
	QStandardItemModel *model;
	QVBoxLayout *m_pielayout;
	DataBase* m_database;
	ExportExcelThread *ExportThread;

public:
	void clearReport();
	void insertReport(QString,QList<SeleteData>);
	void drawPieImage();

public:
	QList<TData> m_dateList;
	QList<QStandardItem*> m_insertData;
	int m_CameraCount;
	TemporaryData m_temp;
	QString m_lastTime;
	int m_lastDataCount;
public slots:
	void slots_ShowPie(QModelIndex);
	void slots_check();
	void slots_Export();

protected:
	void resizeEvent(QResizeEvent *event);

private:
};


#endif // COUNTSET_H