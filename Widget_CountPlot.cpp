#include "Widget_CountPlot.h"
#include "glasswaredetectsystem.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QFileInfo>

extern GlasswareDetectSystem *pMainFrm;

CustomBars::CustomBars(QCPAxis *keyAxis , QCPAxis *valueAxis)
	:QCPBars(keyAxis,valueAxis),
	mTextAlignment(Qt::AlignCenter),
	mSpacing(5),
	mFont(QFont(QLatin1String("san serif"),9)),
	m_TextSuffix("")
{

}

void CustomBars::setTextAlignment(Qt::Alignment alignment)
{
	mTextAlignment = alignment;
}

void CustomBars::setSpacing(double spacing)
{
	mSpacing = spacing;
}

void CustomBars::setFont(const QFont &font)
{
	mFont = font;
}

void CustomBars::setTextSuffix(QString Suffix)
{
	m_TextSuffix = Suffix;
}

void CustomBars::draw(QCPPainter *painter) Q_DECL_OVERRIDE
{
	if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
	if (mDataContainer->isEmpty()) return;

	QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
	getVisibleDataBounds(visibleBegin, visibleEnd);

	// loop over and draw segments of unselected/selected data:
	QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
	getDataSegments(selectedSegments, unselectedSegments);
	allSegments << unselectedSegments << selectedSegments;
	for (int i=0; i<allSegments.size(); ++i)
	{
		bool isSelectedSegment = i >= unselectedSegments.size();
		QCPBarsDataContainer::const_iterator begin = visibleBegin;
		QCPBarsDataContainer::const_iterator end = visibleEnd;
		mDataContainer->limitIteratorsToDataRange(begin, end, allSegments.at(i));
		if (begin == end)
			continue;

		for (QCPBarsDataContainer::const_iterator it=begin; it!=end; ++it)
		{
			// check data validity if flag set:
#ifdef QCUSTOMPLOT_CHECK_DATA
			if (QCP::isInvalidData(it->key, it->value))
				qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "of drawn range invalid." << "Plottable name:" << name();
#endif
			// draw bar:
			if (isSelectedSegment && mSelectionDecorator)
			{
				mSelectionDecorator->applyBrush(painter);
				mSelectionDecorator->applyPen(painter);
			} else
			{
				painter->setBrush(mBrush);
				painter->setPen(mPen);
			}
			applyDefaultAntialiasingHint(painter);
			QRectF barRect=getBarRect(it->key, it->value);
			painter->drawPolygon(getBarRect(it->key, it->value));

			//add text
			// 计算文字的位置
			painter->setFont(mFont);                     // 设置字体
			QString text = QString::number(it->value);   // 取得当前value轴的值，保留两位精度
			text += m_TextSuffix;

			QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // 计算文字所占用的大小

			if (mKeyAxis.data()->orientation() == Qt::Horizontal) 
			{    // 当key轴为水平轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atTop)     // 上轴，移动文字到柱状图下面
					textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
				else                                                   // 下轴，移动文字到柱状图上面
					textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
				textRect.setWidth(barRect.width());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}
			else 
			{                                                  // 当key轴为竖直轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atLeft)   // 左轴，移动文字到柱状图右边
					textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
				else                                                  // 右轴，移动文字到柱状图左边
					textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
				textRect.setHeight(barRect.height());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}
		}
	}

	// draw other selection decoration that isn't just line/scatter pens and brushes:
	if (mSelectionDecorator)
		mSelectionDecorator->drawDecoration(painter, selection());
}

Widget_CountPlot::Widget_CountPlot(int CameraCount,QWidget *parent)
	: m_CameraCount(CameraCount),QWidget(parent)
{
	ui.setupUi(this);

	ui.widget_shift->setWidgetName(tr("Select Shift"));
	ui.widget_table->setWidgetName(tr("Select Table"));
	ui.widget_ShiftSet->setWidgetName(tr("Search Set"));
	ui.widget_pie->setWidgetName(tr("Shift Defect Count Plot"));

	//set calendar picture
	QToolButton *prevBtn=ui.calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_prevmonth"));
	QToolButton *nextBtn=ui.calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_nextmonth"));
	prevBtn->setIcon(QIcon(":/calendar/prev"));
	nextBtn->setIcon(QIcon(":/calendar/next"));
	//set calendar Language
	if(pMainFrm->sLanguage == 0)
	{
		ui.calendarWidget->setLocale(QLocale::Chinese);
	}
	Shift1 = pMainFrm->m_sSystemInfo.SearchShift1;
	Shift2 = pMainFrm->m_sSystemInfo.SearchShift2;
	Shift3 = pMainFrm->m_sSystemInfo.SearchShift3;
	m_SearchFlag = pMainFrm->m_sSystemInfo.iSearchTimeFlag;
	ui.timeEdit_1->setTime(Shift1);
	ui.timeEdit_2->setTime(Shift2);
	ui.timeEdit_3->setTime(Shift3);
	if (m_SearchFlag == DataBase::ByanHour)
	{
		ui.radioBtn_anHour->setChecked(true);
	}
	else
	{
		ui.radioBtn_Half->setChecked(true);
	}

	//init database
	m_database =new DataBase(m_CameraCount);

	QLinearGradient gradient(0, 0, 0, 400);
	gradient.setColorAt(0, QColor(90, 90, 90));
	gradient.setColorAt(0.38, QColor(105, 105, 105));
	gradient.setColorAt(1, QColor(70, 70, 70));
	ui.CustomPlot->setBackground(QBrush(gradient));

	mWidget_pie =new PieView();
	mWidget_pie->setSize(150);
	mWidget_pie->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mWidget_pie->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	model = new QStandardItemModel(10, 2, this);
	model->setHeaderData(0, Qt::Horizontal, tr("Error1"));
	model->setHeaderData(1, Qt::Horizontal, tr("Error2"));
	model->setHeaderData(2, Qt::Horizontal, tr("Error3"));
	model->setHeaderData(3, Qt::Horizontal, tr("Error4"));
	model->setHeaderData(4, Qt::Horizontal, tr("Error5"));
	model->setHeaderData(5, Qt::Horizontal, tr("Error6"));
	model->setHeaderData(6, Qt::Horizontal, tr("Error7"));
	model->setHeaderData(7, Qt::Horizontal, tr("Error8"));
	model->setHeaderData(8, Qt::Horizontal, tr("Error9"));
	model->setHeaderData(9, Qt::Horizontal, tr("Error10"));
	mWidget_pie->setModel(model);
	QVBoxLayout *m_pielayout = new QVBoxLayout(this);
	m_pielayout->addWidget(mWidget_pie);
	m_pielayout->setMargin(3);
	ui.widget_2->setLayout(m_pielayout);

	//connect(ui.radioButton_table1,SIGNAL(clicked(bool)),this,SLOT(Table1CheckedSlot(bool)));
	//connect(ui.radioButton_table2,SIGNAL(clicked(bool)),this,SLOT(Table2CheckedSlot(bool)));
	
	connect(ui.pushButton_Save,SIGNAL(clicked()),this,SLOT(PushBtnSaveSlot()));
	connect(ui.pushButton_Cancel,SIGNAL(clicked()),this,SLOT(PushBtnCancelSlot()));

	connect(ui.pushButton_Search,SIGNAL(clicked()),this,SLOT(SearchButtonSlot()));

	contextMenu = new QMenu();
	saveAction = new QAction(tr("Save Table"),this);
	connect(saveAction,SIGNAL(triggered()),this,SLOT(slots_saveTable()));
	connect(ui.CustomPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(Slots_Show_ContextMenu(QMouseEvent*)));
}

Widget_CountPlot::~Widget_CountPlot()
{
}

void Widget_CountPlot::Table1CheckedSlot(bool isChecked)
{
	if (isChecked)
	{
		//ShowNumberTable();
	}
}

void Widget_CountPlot::Table2CheckedSlot(bool isChecked)
{
	if (isChecked)
	{
		//ShowRateTable();
	}
}

void Widget_CountPlot::SearchButtonSlot()
{
	QString startTime,endTime;
	QDate dateSelecte = ui.calendarWidget->selectedDate();
	QString temp = dateSelecte.toString(Qt::ISODate);
	temp.replace("-", "");//20201028
	QString shiftStr;
	if (ui.radioButton_Shift1->isChecked())
	{		
		startTime = temp + Shift1.toString("hhmm");
		endTime = temp + Shift2.toString("hhmm");
		shiftStr=tr("shift1");
	}
	else if (ui.radioButton_Shift2->isChecked())
	{
		startTime = temp + Shift2.toString("hhmm");
		endTime = temp + Shift3.toString("hhmm");
		shiftStr=tr("shift2");
	}
	else if(ui.radioButton_Shift3->isChecked())
	{
		startTime = temp + Shift3.toString("hhmm");
		temp = dateSelecte.addDays(1).toString(Qt::ISODate).replace("-","");
		endTime = temp + Shift1.toString("hhmm");
		shiftStr=tr("shift3");
	}
	else
	{
		startTime = temp + Shift1.toString("hhmm");
		temp = dateSelecte.addDays(1).toString(Qt::ISODate).replace("-","");
		endTime = temp + Shift1.toString("hhmm");
		shiftStr=tr("AllShift");
	}
	QList<SeleteData> m_temp = m_database->queryAll(startTime,endTime);
	if (ui.radioButton_table1->isChecked())
	{
		if (ui.radioButton_ShiftAll->isChecked())
		{
			QString shift1time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift1.toString("hhmm");
			QString shift2time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift2.toString("hhmm");
			QString shift3time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift3.toString("hhmm");
			ShowShiftNumberTable(shift1time,shift2time,shift3time,m_temp);
		}
		else
		{
			ShowNumberTable(startTime,endTime,m_SearchFlag,m_temp);
		}
		
	}
	else if(ui.radioButton_table2->isChecked())
	{
		if (ui.radioButton_ShiftAll->isChecked())
		{
			QString shift1time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift1.toString("hhmm");
			QString shift2time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift2.toString("hhmm");
			QString shift3time = dateSelecte.toString(Qt::ISODate).replace("-","") + Shift3.toString("hhmm");
			ShowShiftRateTable(shift1time,shift2time,shift3time,m_temp);
		}
		else
		{
			ShowRateTable(startTime,endTime,m_SearchFlag,m_temp);
		}
	}
	else
	{
		ui.widget_pie->setWidgetName(tr("Shift Defect Count Plot  ") + dateSelecte.toString(Qt::ISODate) +" "+ shiftStr);
		ShowPieView(m_temp);
	}
}

void Widget_CountPlot::PushBtnSaveSlot()
{
	Shift1 = ui.timeEdit_1->time();
	Shift2 = ui.timeEdit_2->time();
	Shift3 = ui.timeEdit_3->time();
	if (ui.radioBtn_anHour->isChecked())
	{
		m_SearchFlag = DataBase::ByanHour;
	}
	else if (ui.radioBtn_Half->isChecked())
	{
		m_SearchFlag = DataBase::ByHalfanHour;
	}
	Shift1 = intTime(Shift1,m_SearchFlag);
	Shift2 = intTime(Shift2,m_SearchFlag);
	Shift3 = intTime(Shift3,m_SearchFlag);
	ui.timeEdit_1->setTime(Shift1);
	ui.timeEdit_2->setTime(Shift2);
	ui.timeEdit_3->setTime(Shift3);

	pMainFrm->m_sSystemInfo.SearchShift1 = Shift1;
	pMainFrm->m_sSystemInfo.SearchShift2 = Shift2;
	pMainFrm->m_sSystemInfo.SearchShift3 = Shift3;
	pMainFrm->m_sSystemInfo.iSearchTimeFlag = m_SearchFlag;

	QSettings saveSearchSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	saveSearchSet.setIniCodec(QTextCodec::codecForName("GBK"));
	saveSearchSet.beginGroup("system");
	saveSearchSet.setValue(QString("Searchshift1"),Shift1.hour()*10000+Shift1.minute()*100+Shift1.second());
	saveSearchSet.setValue(QString("Searchshift2"),Shift2.hour()*10000+Shift2.minute()*100+Shift2.second());
	saveSearchSet.setValue(QString("Searchshift3"),Shift3.hour()*10000+Shift3.minute()*100+Shift3.second());
	saveSearchSet.setValue(QString("SearchTimeFlags"),m_SearchFlag);
	saveSearchSet.endGroup();
}

void Widget_CountPlot::PushBtnCancelSlot()
{
	ui.timeEdit_1->setTime(Shift1);
	ui.timeEdit_2->setTime(Shift2);
	ui.timeEdit_3->setTime(Shift3);
	if (m_SearchFlag == DataBase::ByanHour)
	{
		ui.radioBtn_anHour->setChecked(true);
	}
	else
	{
		ui.radioBtn_Half->setChecked(true);
	}
}

void Widget_CountPlot::slots_saveTable()
{
	QString m_SavePath = QFileDialog::getSaveFileName(this,tr("Save Table"),
										QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
										tr("Images (*.pdf *.bmp *.png *.jpg)"));
	if (m_SavePath == "")
	{
		return;
	}
	QFileInfo SavePathInfo(m_SavePath);
	if (SavePathInfo.suffix() == "pdf")
	{
		ui.CustomPlot->savePdf(m_SavePath);
	}
	else if( SavePathInfo.suffix() == "bmp")
	{
		ui.CustomPlot->saveBmp(m_SavePath);
	}
	else if (SavePathInfo.suffix() == "png")
	{
		ui.CustomPlot->savePng(m_SavePath);
	}
	else if (SavePathInfo.suffix() == "jpg")
	{
		ui.CustomPlot->saveJpg(m_SavePath);
	}
}

void Widget_CountPlot::Slots_Show_ContextMenu(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		contextMenu->clear(); //清除原有菜单
		QPoint point = e->pos(); //得到窗口坐标
		contextMenu->addAction(saveAction);

		contextMenu->exec(QCursor::pos());

		e->accept();
	}
	
}

void Widget_CountPlot::ShowNumberTable(QString startTime,QString endTime,int pflag,QList<SeleteData> m_datalist)
{
	QDateTime startDateT = QDateTime::fromString(startTime,"yyyyMMddhhmm");
	QDateTime endDateT = QDateTime::fromString(endTime,"yyyyMMddhhmm");
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();
	// create empty bar chart objects:
	CustomBars *fossil = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	CustomBars *nuclear = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	nuclear->setAntialiased(false);
	fossil->setAntialiased(false);
	// set names and colors:
	QList<QColor> colors;
	colors<<QColor(0, 0, 255) << QColor(250, 0, 0);

	fossil->setName(tr("All Detect Number"));
	fossil->setPen(QPen(colors[0]));
	colors[0].setAlpha(180);
	fossil->setBrush(colors[0]);
	nuclear->setName(tr("Failure Number"));
	nuclear->setPen(QPen(colors[1]));
	colors[1].setAlpha(180);
	nuclear->setBrush(colors[1]);

	//设置并排显示
	QCPBarsGroup *group = new QCPBarsGroup(ui.CustomPlot);  
	QList<QCPBars*> bars;
	bars <<fossil <<nuclear ;// << regen;
	foreach (QCPBars *bar, bars) {
		// 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
		bar->setWidthType(QCPBars::wtPlotCoords);
		bar->setWidth(bar->width() / bars.size()); // 设置柱状图的宽度大小
		group->append(bar);  // 将柱状图加入柱状图分组中
	}
	group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
	group->setSpacing(2);     // 设置较小的间距值，这样看起来更紧凑

	int startH = startDateT.time().hour();
	int endH = endDateT.time().hour();
	int startM = startDateT.time().minute();
	int endM = endDateT.time().minute();
	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	int m_range = 1;
	if (pflag == DataBase::ByanHour)
	{
		if (startH < endH )
		{
			if (startM > 0 )
			{
				startH += 1;
			}
			if (endM > 0 )
			{
				endH += 1;
			}
		}
		else
		{
			if (startM > 0 )
			{
				startH += 1;
			}
			if (endM > 0 )
			{
				endH += 1;
			}
			endH += 24;
		}
		int tmp = endH - startH;
		m_range = tmp +1;
		if ( tmp > 0 )
		{
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*60*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
			}
		}
		else
			return;
	}
	else //half an hour
	{
		int tmp=0;
		if (startH < endH)
		{
			tmp = (endH - startH)*2;
		}
		else
		{
			tmp = (endH + 24 - startH)*2;
		}

		if (startM > 0 && startM <= 30)
		{
			tmp -= 1;
		}
		else if(startM > 30 )
		{
			tmp -= 2;
		}
		if (endM > 0 && endM <= 30 )
		{
			tmp +=1;
		}
		else if(endM > 30)
		{
			tmp +=2;
		}
		m_range = tmp +1;

		if ( tmp > 0 )
		{
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*30*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
			}			
		}
		else
			return;
	}

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	//ui.CustomPlot->xAxis->setTickLabelRotation(-30);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, 4);
	ui.CustomPlot->xAxis->setRange(0, m_range);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Time(Hour)"));

	// Add data:
	QVector<double> fossilData, nuclearData;
	LONGLONG lastId=0;
	for (int i=0;i<m_datalist.count();i++)
	{
		if(m_datalist[i].id != lastId)
		{
			fossilData << m_datalist[i].m_AllCount;
			nuclearData << m_datalist[i].m_FailCount;
			lastId = m_datalist[i].id;
		}
	}
	//判断 获取数据 是否正确
	if (labels.count() != fossilData.count() )
	{
		QMessageBox::information(this,tr("Search Error"),tr("Search history Data Error!"),QMessageBox::Ok);
		return;
	}
	int MaxNumber=0;
	for(int i=0;i<fossilData.count();i++)
	{
		 MaxNumber = MaxNumber > fossilData[i] ? MaxNumber : fossilData[i];
	}
	if (MaxNumber >= 10000)
	{
		MaxNumber +=10000;
	}
	else if (MaxNumber <10000 && MaxNumber >=1000)
	{
		MaxNumber += 1000;
	}
	else if (MaxNumber <1000 && MaxNumber >= 100)
	{
		MaxNumber+=100;
	}
	else if (MaxNumber <100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}

	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Failure Number Table(Ps)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	//set data
	fossil->setData(ticks, fossilData);
	nuclear->setData(ticks, nuclearData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();
}

void Widget_CountPlot::ShowRateTable(QString startTime,QString endTime,int pflag,QList<SeleteData> m_datalist)
{
	QDateTime startDateT = QDateTime::fromString(startTime,"yyyyMMddhhmm");
	QDateTime endDateT = QDateTime::fromString(endTime,"yyyyMMddhhmm");
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();

	CustomBars *regen = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	regen->setAntialiased(false); // gives more crisp, pixel aligned bar borders
	regen->setName(tr("Failure Rate"));
	regen->setPen(QPen(QColor(0, 168, 140).lighter(130)));
	regen->setBrush(QColor(0, 168, 140));
	regen->setTextSuffix("%");

	int startH = startDateT.time().hour();
	int endH = endDateT.time().hour();
	int startM = startDateT.time().minute();
	int endM = endDateT.time().minute();
	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	int m_range = 1;
	if (pflag == DataBase::ByanHour)
	{
		if (startH < endH )
		{
			if (startM > 0 )
			{
				startH += 1;
			}
			if (endM > 0 )
			{
				endH += 1;
			}
		}
		else
		{
			if (startM > 0 )
			{
				startH += 1;
			}
			if (endM > 0 )
			{
				endH += 1;
			}
			endH += 24;
		}
		int tmp = endH - startH;
		m_range = tmp +1;
		if ( tmp > 0 )
		{
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*60*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
			}
		}
		else
			return;
	}
	else //half an hour
	{
		int tmp=0;
		if (startH < endH)
		{
			tmp = (endH - startH)*2;
		}
		else
		{
			tmp = (endH + 24 - startH)*2;
		}

		if (startM > 0 && startM <= 30)
		{
			tmp -= 1;
		}
		else if(startM > 30 )
		{
			tmp -= 2;
		}
		if (endM > 0 && endM <= 30 )
		{
			tmp +=1;
		}
		else if(endM > 30)
		{
			tmp +=2;
		}
		m_range = tmp +1;

		if ( tmp > 0 )
		{
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*30*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
			}			
		}
		else
			return;
	}
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	//ui.CustomPlot->xAxis->setTickLabelRotation(-30);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, 4);
	ui.CustomPlot->xAxis->setRange(0, m_range);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Time(Hour)"));

	// Add data:
	QVector<double>  regenData;
	LONGLONG lastId=0;
	for (int i=0;i<m_datalist.count();i++)
	{
		if(m_datalist[i].id != lastId)
		{
			double rate = (double)m_datalist[i].m_FailCount / m_datalist[i].m_AllCount * 100;
			rate = QString::number(rate ,'f',2).toDouble();
			regenData << rate;
			lastId = m_datalist[i].id;
		}
	}
	//判断 获取数据 是否正确
	if (labels.count() != regenData.count() )
	{
		QMessageBox::information(this,tr("Search Error"),tr("Search history Data Error!"),QMessageBox::Ok);
		return;
	}
	int MaxNumber=0;
	for(int i=0;i<regenData.count();i++)
	{
		MaxNumber = MaxNumber > regenData[i] ? MaxNumber : regenData[i];
	}
	if (MaxNumber <= 100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}

	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Failure Rate Table(%)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	regen->setData(ticks, regenData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();
}

void Widget_CountPlot::ShowShiftNumberTable(QString shift1_time,QString shift2_time,QString shift3_time,QList<SeleteData> m_datalist)
{
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();
	// create empty bar chart objects:
	CustomBars *fossil = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	CustomBars *nuclear = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	nuclear->setAntialiased(false);
	fossil->setAntialiased(false);
	// set names and colors:
	QList<QColor> colors;
	colors<<QColor(0, 0, 255) << QColor(250, 0, 0);

	fossil->setName(tr("Single Shift All Detect Number"));
	fossil->setPen(QPen(colors[0]));
	colors[0].setAlpha(180);
	fossil->setBrush(colors[0]);
	nuclear->setName(tr("Single Shift Failure Number"));
	nuclear->setPen(QPen(colors[1]));
	colors[1].setAlpha(180);
	nuclear->setBrush(colors[1]);

	//设置并排显示
	QCPBarsGroup *group = new QCPBarsGroup(ui.CustomPlot);  
	QList<QCPBars*> bars;
	bars <<fossil <<nuclear ;// << regen;
	foreach (QCPBars *bar, bars) {
		// 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
		bar->setWidthType(QCPBars::wtPlotCoords);
		bar->setWidth(bar->width() / bars.size()); // 设置柱状图的宽度大小
		group->append(bar);  // 将柱状图加入柱状图分组中
	}
	group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
	group->setSpacing(2);     // 设置较小的间距值，这样看起来更紧凑

	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	ticks<<1<<2<<3;
	labels<<tr("Shift1")<<tr("Shift2")<<tr("Shift3");
	
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	ui.CustomPlot->xAxis->setTickLabelRotation(0);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, 4);
	ui.CustomPlot->xAxis->setRange(0, 4);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Shift(1,2,3)"));

	// Add data:
	QVector<double> fossilData, nuclearData;
	LONGLONG shift1Id = shift1_time.toLongLong();
	LONGLONG shift2Id = shift2_time.toLongLong();
	LONGLONG shift3Id = shift3_time.toLongLong();
	int Shift1CheckNum=0,Shift2CheckNum=0,Shift3CheckNum=0;
	int Shift1FailureNum=0,Shift2FailureNum=0,Shift3FailureNum=0;
	LONGLONG lastId=0;
	for (int i = 0; i<m_datalist.count();i++)
	{
		if (lastId != m_datalist[i].id)
		{
			if (m_datalist[i].id > shift1Id && m_datalist[i].id <= shift2Id)
			{
				Shift1CheckNum += m_datalist[i].m_AllCount;
				Shift1FailureNum += m_datalist[i].m_FailCount;
			}
			else if (m_datalist[i].id > shift2Id && m_datalist[i].id <= shift3Id)
			{
				Shift2CheckNum += m_datalist[i].m_AllCount;
				Shift2FailureNum += m_datalist[i].m_FailCount;
			}
			else
			{
				Shift3CheckNum += m_datalist[i].m_AllCount;
				Shift3FailureNum += m_datalist[i].m_FailCount;
			}
			lastId = m_datalist[i].id;
		}
	}
	fossilData<<Shift1CheckNum<<Shift2CheckNum<<Shift3CheckNum;
	nuclearData<<Shift1FailureNum<<Shift2FailureNum<<Shift3FailureNum;

	int MaxNumber=0;
	for(int i=0;i<fossilData.count();i++)
	{
		MaxNumber = MaxNumber > fossilData[i] ? MaxNumber : fossilData[i];
	}
	if (MaxNumber >= 10000)
	{
		MaxNumber +=10000;
	}
	else if (MaxNumber <10000 && MaxNumber >=1000)
	{
		MaxNumber += 1000;
	}
	else if (MaxNumber <1000 && MaxNumber >= 100)
	{
		MaxNumber+=100;
	}
	else if (MaxNumber <100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}
	
	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Shift Failure Number Table(Ps)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	//setdata
	fossil->setData(ticks, fossilData);
	nuclear->setData(ticks, nuclearData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();
}

void Widget_CountPlot::ShowShiftRateTable(QString shift1_time,QString shift2_time,QString shift3_time,QList<SeleteData> m_datalist)
{
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();

	CustomBars *regen = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	regen->setAntialiased(false); // gives more crisp, pixel aligned bar borders
	regen->setName(tr("Single Shift Failure Rate"));
	regen->setPen(QPen(QColor(0, 168, 140).lighter(130)));
	regen->setBrush(QColor(0, 168, 140));
	regen->setTextSuffix("%");

	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	ticks<<1<<2<<3;
	labels<<tr("Shift1")<<tr("Shift2")<<tr("Shift3");

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	ui.CustomPlot->xAxis->setTickLabelRotation(0);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, 4);
	ui.CustomPlot->xAxis->setRange(0, 4);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Shift(1,2,3)"));

	// Add data:
	QVector<double>  regenData;
	LONGLONG shift1Id = shift1_time.toLongLong();
	LONGLONG shift2Id = shift2_time.toLongLong();
	LONGLONG shift3Id = shift3_time.toLongLong();
	int Shift1CheckNum=0,Shift2CheckNum=0,Shift3CheckNum=0;
	int Shift1FailureNum=0,Shift2FailureNum=0,Shift3FailureNum=0;
	LONGLONG lastId=0;
	for (int i = 0; i<m_datalist.count();i++)
	{
		if (lastId != m_datalist[i].id)
		{
			if (m_datalist[i].id > shift1Id && m_datalist[i].id <= shift2Id)
			{
				Shift1CheckNum += m_datalist[i].m_AllCount;
				Shift1FailureNum += m_datalist[i].m_FailCount;
			}
			else if (m_datalist[i].id > shift2Id && m_datalist[i].id <= shift3Id)
			{
				Shift2CheckNum += m_datalist[i].m_AllCount;
				Shift2FailureNum += m_datalist[i].m_FailCount;
			}
			else
			{
				Shift3CheckNum += m_datalist[i].m_AllCount;
				Shift3FailureNum += m_datalist[i].m_FailCount;
			}
			lastId = m_datalist[i].id;
		}
	}
	double shift1Rate = (double) Shift1FailureNum / Shift1CheckNum * 100;
	double shift2Rate = (double) Shift2FailureNum / Shift2CheckNum * 100;
	double shift3Rate = (double) Shift3FailureNum / Shift3CheckNum * 100;
	shift1Rate = QString::number(shift1Rate ,'f',2).toDouble();
	shift2Rate = QString::number(shift2Rate ,'f',2).toDouble();
	shift3Rate = QString::number(shift3Rate ,'f',2).toDouble();
	regenData<<shift1Rate<<shift2Rate<<shift3Rate;

	int MaxNumber=0;
	for(int i=0;i<regenData.count();i++)
	{
		MaxNumber = MaxNumber > regenData[i] ? MaxNumber : regenData[i];
	}
	if (MaxNumber <= 100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}

	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Shift Failure Rate Table(%)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	regen->setData(ticks, regenData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();
}

void Widget_CountPlot::ShowPieView(QList<SeleteData> m_datalist)
{
	if (m_datalist.count() ==  0 )
	{
		QMessageBox::warning(this,tr("Shift Failure count"),tr("Search Date error!"),QMessageBox::Ok);
		return;
	}
	int m_TypeCount[ERRORTYPE_MAX_COUNT]={0};
	int TmpTypeCount[ERRORTYPE_MAX_COUNT]= {0};
	int m_FailureNum = 0; 
	int m_AllCount = 0;
	int sum = 0;
	LONGLONG lastId = 0 ;
	QList<int> SubSums;
	QList<int> Failures;
	int subSum=0;
	for (int i=0;i<m_datalist.count();i++)
	{
		if (lastId != m_datalist[i].id)
		{
			if (i!=0)//跳过第一次数据
			{
				SubSums << subSum;//保存同一ID的总缺陷数
				subSum =0;
			}
			Failures<<m_datalist[i].m_FailCount;
			lastId = m_datalist[i].id;
		}
		subSum += m_datalist[i].m_CameraTypeCount;
		//如果是 最后一条数据
		if (i==m_datalist.count()-1 )
		{
			SubSums << subSum;
			subSum =0;
		}
	}
	lastId = 0 ;
	int ps=0;
	for (int i=0;i<m_datalist.count();i++)
	{
		if (lastId != m_datalist[i].id)
		{
			m_FailureNum += m_datalist[i].m_FailCount;
			m_AllCount += m_datalist[i].m_AllCount;
			lastId = m_datalist[i].id;
			ps++;
		}
		m_TypeCount[m_datalist[i].m_Type] += m_datalist[i].m_CameraTypeCount * ((double)Failures[ps-1] / SubSums[ps-1]);//分别乘各自时段的比例
		TmpTypeCount[m_datalist[i].m_Type] += m_datalist[i].m_CameraTypeCount * ((double)Failures[ps-1] / SubSums[ps-1]);
		sum += m_datalist[i].m_CameraTypeCount * ((double)Failures[ps-1] / SubSums[ps-1]);
	}
	for(int i=0;i<ERRORTYPE_MAX_COUNT-1;i++)
	{
		int MaxType=i;
		for(int j=i+1 ; j< ERRORTYPE_MAX_COUNT;j++)
		{
			if (TmpTypeCount[MaxType] < TmpTypeCount[j])
			{
				MaxType =j;
			}
		}
		if(MaxType != i)
		{
			int tmp = TmpTypeCount[i];
			TmpTypeCount[i] = TmpTypeCount[MaxType];
			TmpTypeCount[MaxType] = tmp;
		}
	}
	QList<int> TypeOrder;
	for(int i=0;i<ERRORTYPE_MAX_COUNT-1;i++)
	{	
		if (TmpTypeCount[i] !=0)
		{
			if (i!=0 && TmpTypeCount[i] == TmpTypeCount[i-1])
			{
				continue;
			}
			for(int j=0; j< ERRORTYPE_MAX_COUNT;j++)
			{
				if ( TmpTypeCount[i] == m_TypeCount[j]  )
				{
					TypeOrder<<j;
				}
			}
		}
	}
	QColor color[10];
	color[0] = QColor(153,230,0);
	color[1] = QColor(174,77,102);
	color[2] = QColor(128,255,128);
	color[3] = QColor(128,255,255);
	color[4] = QColor(0,128,255);
	color[5] = QColor(255,255,128);
	color[6] = QColor(0,248,128);
	color[7] = QColor(255,0,213);
	color[8] = QColor(35,0,255);
	color[9] = QColor(255,129,0);
	QFont itemFont("微软雅黑",9);
	itemFont.setBold(true);

	model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
	int remp =TypeOrder.count();
	if (remp <= 10)
	{
		int t_fail =0;
		for (int i=0;i<remp;i++)
		{
			int t_data =0;
			if ( i != remp -1)
			{
				t_data = m_TypeCount[TypeOrder[i]];
				//if(sum != 0)
				//{
				//	t_data = m_FailureNum * t_data / sum ;
				//}
				t_fail += t_data;
			}
			else
			{
				t_data = m_FailureNum - t_fail;
			}
			model->insertRows(i, 1, QModelIndex());
			model->setData(model->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType.at(TypeOrder[i])+":" 
				+ QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			model->setData(model->index(i, 1, QModelIndex()), QString::number(t_data));
			model->setData(model->index(i, 0, QModelIndex()), color[i], Qt::DecorationRole);
			model->item(i,0)->setFont(itemFont);
		}
	}
	else
	{
		int t_fail =0;
		for (int i=0;i<9;i++)
		{
			int t_data = m_TypeCount[TypeOrder[i]];
			//if(sum != 0)
			//{
			//	t_data = m_FailureNum * t_data / sum ;
			//}
			t_fail += t_data;
			
			model->insertRows(i, 1, QModelIndex());
			model->setData(model->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType.at(TypeOrder[i])+":" 
				+ QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			model->setData(model->index(i, 1, QModelIndex()), QString::number(t_data));
			model->setData(model->index(i, 0, QModelIndex()), color[i], Qt::DecorationRole);
			model->item(i,0)->setFont(itemFont);
		}
		t_fail = m_FailureNum - t_fail ;
		model->insertRows(9, 1, QModelIndex());
		model->setData(model->index(9, 0, QModelIndex()), tr("Other:") + QString::number(t_fail)+"("+QString::number((double)t_fail/m_AllCount*100,'f',2)+"%)");
		model->setData(model->index(9, 1, QModelIndex()), QString::number(t_fail));
		model->setData(model->index(9, 0, QModelIndex()), color[9], Qt::DecorationRole);
		model->item(9,0)->setFont(itemFont);
	}
}

QTime Widget_CountPlot::intTime(QTime ptime,int flag)
{
	int phour = ptime.hour();
	int pminute = ptime.minute();
	if(flag == DataBase::ByanHour)
	{
		if (pminute >0)
		{
			if (phour != 23)
			{
				phour += 1;
			}
			pminute = 0;
		}
	}
	else
	{
		if (pminute > 0 && pminute < 30)
		{
			pminute = 30;
		}
		else if ( pminute > 30 )
		{
			if (phour != 23)
			{
				phour += 1;
				pminute = 0;
			}	
			else
			{
				pminute = 30;
			}
		}
	}
	QString timeStr = QString("%1:%2").arg(phour).arg(pminute);

	return QTime::fromString(timeStr,"h:m");
}
