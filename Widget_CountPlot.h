#ifndef WIDGET_COUNTPLOT_H
#define WIDGET_COUNTPLOT_H

#include <QWidget>
#include "ui_Widget_CountPlot.h"
#include "qcustomplot.h"
#include <QList>
#include <QColor>
#include "database.h"
#include "widget_pie.h"
#include <QMenu>

class CustomBars :public QCPBars
{
public:
	explicit CustomBars(QCPAxis *keyAxis , QCPAxis *valueAxis);

	enum CustomBarsFlags{
		CustomBars_NULL = 0,
		CustomBars_Rate
	};

	Qt::Alignment textAlignment() const { return mTextAlignment;}
	double spacing() const {return mSpacing;}
	QFont font() const {return mFont;}

	void setTextAlignment(Qt::Alignment alignment);
	void setSpacing(double spacing);
	void setFont(const QFont &font);

	void setTextSuffix(QString Suffix);

protected:
	Qt::Alignment mTextAlignment;
	double mSpacing;
	QFont mFont;
	CustomBarsFlags m_Flags;
	QString m_TextSuffix;

	virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
};


class Widget_CountPlot : public QWidget
{
	Q_OBJECT

public:
	Widget_CountPlot(int CameraCount,QWidget *parent = NULL);
	~Widget_CountPlot();

private slots:
	void Table1CheckedSlot(bool isChecked);
	void Table2CheckedSlot(bool isChecked);
	void SearchButtonSlot();
	void PushBtnSaveSlot();
	void PushBtnCancelSlot();
	void slots_saveTable();
	void Slots_Show_ContextMenu(QMouseEvent *e);

private:
	void ShowNumberTable(QString startTime,QString endTime,int pflag,QList<SeleteData> m_datalist);
	void ShowRateTable(QString startTime,QString endTime,int pflag,QList<SeleteData> m_datalist);
	void ShowShiftNumberTable(QString shift1_time,QString shift2_time,QString shift3_time,QList<SeleteData> m_datalist);
	void ShowShiftRateTable(QString shift1_time,QString shift2_time,QString shift3_time,QList<SeleteData> m_datalist);
	void ShowPieView(QList<SeleteData> m_datalist);
	QTime intTime(QTime ptime,int flag);  //时间取整

private:
	DataBase *m_database;
	int m_CameraCount;
	int m_SearchFlag;
	QTime Shift1,Shift2,Shift3;
	QMenu *contextMenu;
	QAction *saveAction;
	PieView *mWidget_pie;
	QStandardItemModel *model;

private:
	Ui::Widget_CountPlot ui;
};

#endif // WIDGET_COUNTPLOT_H
