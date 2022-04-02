#include "widget_Report.h"
#include <QDebug>
#include "glasswaredetectsystem.h"
extern GlasswareDetectSystem *pMainFrm;
WidgetReport::WidgetReport(QWidget *parent,int temp)
	: QWidget(parent),m_CameraCount(temp)
{
	ui.setupUi(this);
	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows); 
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection); 
	ui.tableView->verticalHeader()->setVisible(false);	
	ui.widget->setWidgetName(tr("Hourly Defect Count Plot"));

	if(pMainFrm->sLanguage == 0)
	{
		ui.calendarWidget->setLocale(QLocale::Chinese);
	}

	QToolButton *prevBtn=ui.calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_prevmonth"));
	QToolButton *nextBtn=ui.calendarWidget->findChild<QToolButton*>(QLatin1String("qt_calendar_nextmonth"));
	prevBtn->setIcon(QIcon(":/calendar/prev"));
	nextBtn->setIcon(QIcon(":/calendar/next"));

	m_modelCount = new QStandardItemModel(this);
	ui.tableView->setModel(m_modelCount);
	QStringList headerList;
	headerList<<tr("DateTime");
	headerList<<tr("AllCheck");
	headerList<<tr("AllReject");
	headerList<<tr("Reject Rate");
	for (int i = 1;i <= m_CameraCount;i++)
	{
		headerList <<QObject::tr("Camera%1").arg(i);
	}
	m_modelCount->setHorizontalHeaderLabels(headerList);

	pieImage = new PieView();
	pieImage->setSize(350);
	pieImage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pieImage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
	pieImage->setModel(model);
	m_pielayout = new QVBoxLayout(this);
	m_pielayout->addWidget(pieImage);
	m_pielayout->setMargin(4);
	ui.widget_pie->setLayout(m_pielayout);
	m_database = new DataBase(m_CameraCount);
	//删除三个月以前的数据
	QDate m_CurrentDate=QDate::currentDate();
	QDate m_LastDate=m_CurrentDate.addMonths(-6);
	QString dateStr = m_LastDate.toString(Qt::ISODate);
	dateStr.replace("-", "");
	m_database->deleteFromDate(dateStr);

	//测试代码
	connect(ui.tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(slots_ShowPie(QModelIndex)));
	connect(ui.pushButton_check,SIGNAL(clicked()),this,SLOT(slots_check()));
	connect(ui.Btn_Export,SIGNAL(clicked()),this,SLOT(slots_Export()));
	memset(&m_temp,0,sizeof(TemporaryData));
	ExportThread=new ExportExcelThread(this);
	ExportThread->m_database = m_database;

}
void WidgetReport::slots_check()
{
	QDate dateSelecte = ui.calendarWidget->selectedDate();
	QString temp = dateSelecte.toString(Qt::ISODate);
	temp.replace("-", "");
	QList<SeleteData> m_temp = m_database->queryAll(temp);
	//TemporaryData m_temp = m_database-> queryAll(temp);
	insertReport(temp,m_temp);
	m_lastTime = temp;
	m_lastDataCount = m_temp.count();
}


void WidgetReport::slots_Export()
{
	if (ExportThread->isRunning())
	{
		return;
	}
	//QMessageBox::information(NULL, QString::fromLocal8Bit("注意"), QString::fromLocal8Bit("保存excel时间较长请耐心等待约10秒左右"), QMessageBox::Yes, QMessageBox::Yes);
	ExportThread->m_Date = ui.calendarWidget->selectedDate();
	ExportThread->InitShiftTime(pMainFrm->m_sSystemInfo.shift1,pMainFrm->m_sSystemInfo.shift2,pMainFrm->m_sSystemInfo.shift3);
	ExportThread->start();
	QMessageBox::information(NULL, QString::fromLocal8Bit("注意"), QString::fromLocal8Bit("保存excel时间较长请耐心等待约10秒左右"), QMessageBox::Yes, QMessageBox::Yes);
}

void WidgetReport::resizeEvent(QResizeEvent *event)
{
	pieImage->setSize(ui.widget->height() - 30 );
}

void WidgetReport::clearReport()
{
	while(m_modelCount->rowCount()>0 )
	{
		QList<QStandardItem*> listItem = m_modelCount->takeRow(0);
		qDeleteAll(listItem);
		listItem.clear();
	}
}
void WidgetReport::drawPieImage()
{
	int sum=0;
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
	QFont itemFont("微软雅黑",10);
	itemFont.setBold(true);

	model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
	int remp =m_dateList.size();
	for(int i = 0;i < remp;i++)
	{
		sum += m_dateList.at(i).m_Count;
	}
	int m_failCount = m_dateList.at(0).m_failCount;
	m_failCount = sum < m_failCount ? sum : m_failCount;
	int m_AllCount = m_dateList.at(0).m_AllCheckCount;
	if(remp <= 10)
	{
		int t_fail = 0;
		for(int i = 0;i < remp;i++)
		{
			int t_data = 0 ;
			if (i != (remp-1))
			{
				t_data = m_dateList.at(i).m_Count;
// 				if (sum != 0)
// 				{
// 					t_data = m_failCount * t_data  / sum ;
// 				}
				t_fail += t_data;
			}
			else
			{
				t_data = m_failCount - t_fail ;
			}
			model->insertRows(i, 1, QModelIndex());
			model->setData(model->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType.at(m_dateList[i].m_type)+":" 
							+ QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			model->setData(model->index(i, 1, QModelIndex()), QString::number(t_data));
			model->setData(model->index(i, 0, QModelIndex()), color[i], Qt::DecorationRole);
			model->item(i,0)->setFont(itemFont);
		}
	}else{
		int t_fail = 0;
		for(int i = 0;i < 9;i++)
		{
			int t_data = m_dateList.at(i).m_Count;
			if (sum != 0)
			{
				t_data = m_failCount * t_data  / sum ;
			}
			t_fail += t_data;

			model->insertRows(i, 1, QModelIndex());
			model->setData(model->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType.at(m_dateList[i].m_type)+":" 
							+ QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			model->setData(model->index(i, 1, QModelIndex()), QString::number(t_data));
			model->setData(model->index(i, 0, QModelIndex()), color[i], Qt::DecorationRole);
			model->item(i,0)->setFont(itemFont);
		}
		t_fail = m_failCount - t_fail ;
		model->insertRows(9, 1, QModelIndex());
		model->setData(model->index(9, 0, QModelIndex()), tr("Other:") + QString::number(t_fail)+"("+QString::number((double)t_fail/m_AllCount*100,'f',2)+"%)");
		model->setData(model->index(9, 1, QModelIndex()), QString::number(t_fail));
		model->setData(model->index(9, 0, QModelIndex()), color[9], Qt::DecorationRole);
		model->item(9,0)->setFont(itemFont);
	}
}
void WidgetReport::slots_ShowPie(QModelIndex modelIndex)
{
	m_dateList.clear();
	int iListNo = modelIndex.row();
	QModelIndex index = m_modelCount->index(iListNo,0);
	QString name = m_modelCount->data(index).toString();
	QList<SeleteData> m_temp = m_database->sortById(name);
	for(int i = 0; i < m_temp.count();i++)
	{
		TData temp;
		temp.m_type = m_temp[i].m_Type;
		temp.m_Count = m_temp[i].m_CameraTypeCount;
		temp.m_failCount = m_temp[i].m_FailCount;
		temp.m_AllCheckCount = m_temp[i].m_AllCount;
		m_dateList.push_back(temp);
	}
	/*
	TemporaryData m_temp = m_database->sortById(name);
	for(int i = 1; i < 50;i++)
	{
		if(m_temp.m_Type[i] != 0)
		{
			TData temp;
			temp.m_type = m_temp.m_Type[i];
			temp.m_Count = m_temp.m_CameraTypeCount[i];
			m_dateList.push_back(temp);
		}
	}
	*/
	drawPieImage();
}
void WidgetReport::insertReport(QString id,QList<SeleteData> m_datalist)
{
	if(m_lastTime != id || m_lastDataCount != m_datalist.count())
	{
		//清除上一次饼图显示
		model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
		pieImage->update();//刷新
		clearReport();
		m_insertData.clear();
		long long m_lastdataId=0;
		QList<int> m_FailCountList;
		int tmpFailcount=0;
		QList<QList<int>> m_Countlist;//存放相同ID 每个相机检测的缺陷数
		int ps=0;
		QList<QList<int>> m_CameraData; //半个小时 每个相机的检测不合格总数
		for (int i=0;i<m_datalist.count();i++)
		{	
			if (m_lastdataId != m_datalist[i].id )
			{
				if(ps != 0)
				{
					QList<int> m_tempList;
					for(int m=0;m<m_CameraCount;m++)
					{
						int m_tempData=0;
						for (int j=0;j<m_Countlist.count();j++)
						{
							m_tempData += m_Countlist[j][m];
						}
						m_tempList << m_tempData;
					}
					m_CameraData << m_tempList;
					
					m_Countlist.clear();
				}
				ps++;
				m_lastdataId = m_datalist[i].id;
			}

			QList<int> m_Count;
			for(int j=0;j<m_CameraCount;j++)
			{	//保存每一条数据 每一个相机的数据
				m_Count << m_datalist[i].m_TypeCount[j];
			}
			m_Countlist<<m_Count;

			if(i == (m_datalist.count() -1))
			{//最后一组
				QList<int> m_tempList;
				for(int m=0;m<m_CameraCount;m++)
				{
					int m_tempData=0;
					for (int j=0;j<m_Countlist.count();j++)
					{
						m_tempData += m_Countlist[j][m];
					}
					m_tempList << m_tempData;
				}
				m_CameraData << m_tempList;
				m_Countlist.clear();
			}
		}

		m_lastdataId = 0;
		ps=0;
		for (int i=0 ; i<m_datalist.count();i++)
		{
			if (m_lastdataId != m_datalist[i].id )
			{
				if (ps != 0)
				{
					m_FailCountList<<tmpFailcount;
					tmpFailcount = 0;
				}
				else
					ps++;
				tmpFailcount = m_datalist[i].m_CameraTypeCount;
				m_lastdataId = m_datalist[i].id;
			}
			else
			{
				tmpFailcount += m_datalist[i].m_CameraTypeCount;
			}
			if (i == (m_datalist.count()-1))
			{
				m_FailCountList<<tmpFailcount;
			}
			
		}

		int k=0;
		m_lastdataId=0;
		for (int i=0;i<m_datalist.count();i++)
		{	
			if (m_lastdataId != m_datalist[i].id )
			{
				m_insertData.clear();
				m_FailCountList[k] = m_FailCountList[k] < m_datalist[i].m_FailCount ? m_FailCountList[k] : m_datalist[i].m_FailCount;
				QStandardItem* t_temp=new QStandardItem(QString::number(m_datalist[i].id));
				m_insertData<<t_temp;
				t_temp =new QStandardItem(QString::number(m_datalist[i].m_AllCount));
				m_insertData<<t_temp;
				t_temp =new QStandardItem(QString::number(m_FailCountList[k]));
				m_insertData<<t_temp;
				t_temp =new QStandardItem(QString::number((double)m_FailCountList[k]/m_datalist[i].m_AllCount*100,'f',2)+"%");
				m_insertData<<t_temp;
				for(int j=0;j<m_CameraCount;j++)
				{
					t_temp =new QStandardItem(QString::number(m_CameraData[k][j]));
					m_insertData<<t_temp;
				}
				int iRow = m_modelCount->rowCount();
				m_modelCount->insertRow(iRow,m_insertData);

				k++;
				m_lastdataId = m_datalist[i].id;
			}
		}

			
		/*
		for(int i=1;i<50;i++)
		{
			if(m_data.m_Type[i] == 0)
			{
				continue;
			}
			//qDeleteAll(m_insertData.begin(), m_insertData.end());
			m_insertData.clear();
			QString aa = QString::number(m_data.id[i]);
			QStandardItem* t_temp=new QStandardItem(QString::number(m_data.id[i]));
			m_insertData<<t_temp;
			t_temp =new QStandardItem(QString::number(m_data.m_AllCount));
			m_insertData<<t_temp;
			t_temp =new QStandardItem(QString::number(m_data.m_FailCount));
			m_insertData<<t_temp;
// 			t_temp =new QStandardItem(QString::number(m_data.m_CameraTypeCount[i]));
// 			m_insertData<<t_temp;
			for(int j=0;j<m_CameraCount;j++)
			{
				t_temp =new QStandardItem(QString::number(m_data.m_TypeCount[j][i]));
				m_insertData<<t_temp;
			}
			int iRow = m_modelCount->rowCount();
			m_modelCount->insertRow(iRow,m_insertData);
		}
		*/
	}
}
WidgetReport::~WidgetReport()
{

}

ExportExcelThread::ExportExcelThread( QObject *parent )
	:QThread(parent)
{
	curShift=-1;
	iShowAllnum=false;
}

ExportExcelThread::~ExportExcelThread()
{

}

void ExportExcelThread::run()
{
	QDate pDate = m_Date;
	QString temp = pDate.toString(Qt::ISODate);
	temp += " Report.xls";
	QString fileName = pMainFrm->m_sConfigInfo.m_strAppPath +"CountInfo/Report/"+temp ;
	QFileInfo fileInfo(fileName);
	QDir fileDir(fileInfo.absolutePath());
	if (!fileDir.exists())
	{
		fileDir.mkdir(fileInfo.absolutePath());
	}
	QFile m_file(fileName);
	if (m_file.exists())
	{
		m_file.remove();
	}
	ExcelObject *m_ExcelObj=new ExcelObject();
	if (!m_ExcelObj->init())
	{
		QMessageBox::warning(NULL,tr("Error"),tr("Failed to create Excel object, please install Microsoft Excel."),QMessageBox::Apply);
		return;
	}
	QString DateStr = pDate.toString(Qt::ISODate).replace("-","");
	int row =1;
	for (int i=0;i<24; i ++ )
	{
		for (int j=0;j<2;j++)
		{
			QString pTimestr=QString("%1").arg(i,2,10,QChar('0'));
			switch (j)
			{
			case 0:
				pTimestr += "00";
				break;
			case 1:
				pTimestr += "30";
				break;
			default:
				break;
			}
			pTimestr = DateStr + pTimestr;	
			QList<SeleteData> pdatas = m_database->queryOnce(pTimestr);
			if (pdatas.isEmpty())
			{
				continue;
			}
			int TempCameraCount[30];//每个相机检测缺陷数
			int ErrorAllCount =0;	//总缺陷数
			memset(TempCameraCount,0,30*sizeof(int));
			for (int k=0;k<pdatas.count();k++)
			{
				for (int pIndex=0;pIndex<pMainFrm->m_sSystemInfo.iCamCount;pIndex++)
				{
					TempCameraCount[pIndex] += pdatas[k].m_TypeCount[pIndex];
				}
				ErrorAllCount += pdatas[k].m_CameraTypeCount;
			}
			int iTempbody = 0, iTempfinish = 0, iTempBottom=0;
			for(int k=0;k<pMainFrm->m_sSystemInfo.iCamCount;k++)
			{
				if(0 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
				{
					iTempbody += TempCameraCount[k];
				}
				else if(1 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
				{
					iTempfinish += TempCameraCount[k];
				}		
				else if(2 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
				{
					iTempBottom += TempCameraCount[k];
				}			
			}
			//报表总览
			int m_Failcount= ErrorAllCount < pdatas[0].m_FailCount ? ErrorAllCount : pdatas[0].m_FailCount;
			QDateTime time = QDateTime::fromString(pTimestr, "yyyyMMddhhmm");  
			m_ExcelObj->SetGeneralInfo(row,QString::fromLocal8Bit("时间：%1\n检测总数：%2   不合格数：%3   不合格率：%4\n瓶身剔废：%5   瓶口剔废：%6   瓶底剔废：%7")
				.arg(time.toString("yyyy-MM-dd hh:mm:ss"))
				.arg(pdatas[0].m_AllCount)
				.arg(m_Failcount)
				.arg(QString::number((double)m_Failcount * 100 / pdatas[0].m_AllCount,'f',2) + "%")
				.arg(iTempbody)
				.arg(iTempfinish)
				.arg(iTempBottom));
			//标题
			m_ExcelObj->SetTatilRow(row+1);
			//每种缺陷
			for (int k=0;k<pdatas.count();k++)
			{
				QStringList dataList;
				dataList<<pMainFrm->m_sErrorInfo.m_vstrErrorType.at(pdatas[k].m_Type)
					<<QString::number(pdatas[k].m_CameraTypeCount)
					<<QString::number((double)pdatas[k].m_CameraTypeCount * 100 / ErrorAllCount , 'f',2) + "%";
				for (int pIndex =0 ;pIndex<pMainFrm->m_sSystemInfo.iCamCount ; pIndex++)
				{
					dataList<<QString::number(pdatas[k].m_TypeCount[pIndex]);
				}

				m_ExcelObj->SetRowData(row+2+k,dataList);
			}
			//合计
			QStringList dataList;
			dataList<<tr("SUM")
				<<QString::number(ErrorAllCount)
				<<QString("");
			for (int pIndex =0 ;pIndex<pMainFrm->m_sSystemInfo.iCamCount ; pIndex++)
			{
				dataList<<QString::number(TempCameraCount[pIndex]);
			}
			m_ExcelObj->SetRowData(row+2+pdatas.count(),dataList);

			m_ExcelObj->SetBolder(row,row+pdatas.count()+2);

			row = row + pdatas.count() +4;
		}
	}

	if (iShowAllnum)
	{
		QString shiftStr;
		QTime m_Time;
		if (curShift == 0)
		{		
			shiftStr=tr("shift1");
			m_Time = m_shifts[0];
		}
		else if (curShift==1)
		{
			shiftStr=tr("shift2");
			m_Time = m_shifts[1];
		}
		else
		{
			shiftStr=tr("shift3");
			m_Time = m_shifts[2];
		}
		QDateTime curTime = QDateTime::currentDateTime();
		//curTime.addSecs(30 * 60);
		QString m_Datestr = m_Date.toString(Qt::ISODate);
		m_Datestr.replace("-", "");//20201028
		QDateTime m_Datetime = QDateTime::fromString(m_Datestr+m_Time.toString("hhmm"),"yyyyMMddhhmm");
		int m_Allcount = 0;
		int m_FailCount = 0 ;
		int m_CameraTypeCount[50] ={0};
		int m_TypeCount[30][50] = {0};
		m_Datetime = m_Datetime.addSecs(30 * 60);
		while(m_Datetime < curTime)
		{
			QString pDataTimestr = m_Datetime.toString("yyyyMMddhhmm");
			QList<SeleteData> pdatas = m_database->queryOnce(pDataTimestr);
			if (pdatas.isEmpty())
			{
				m_Datetime = m_Datetime.addSecs(30 * 60);
				continue;
			}
			m_Allcount += pdatas[0].m_AllCount;
			m_FailCount += pdatas[0].m_FailCount;
			for (int i=0;i<pdatas.count();i++)
			{
				int pType = pdatas[i].m_Type;
				m_CameraTypeCount[pType] += pdatas[i].m_CameraTypeCount;
				for (int pIndex=0;pIndex<pMainFrm->m_sSystemInfo.iCamCount;pIndex++)
				{
					m_TypeCount[pIndex][pType] += pdatas[i].m_TypeCount[pIndex];
				}
			}
			m_Datetime = m_Datetime.addSecs(30 * 60);
		}
		if (m_Allcount != 0 )
		{
			int TempCameraCount[30]={0};//每个相机检测缺陷数
			int ErrorAllCount =0;	//总缺陷数
			int iTempbody = 0, iTempfinish = 0, iTempBottom=0;
			for(int k=0;k<pMainFrm->m_sSystemInfo.iCamCount;k++)
			{
				for (int j= 0 ; j<50 ;j++)
				{
					TempCameraCount[k] += m_TypeCount[k][j];
					if(0 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
					{
						iTempbody += m_TypeCount[k][j];
					}
					else if(1 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
					{
						iTempfinish += m_TypeCount[k][j];
					}		
					else if(2 == pMainFrm->m_sCarvedCamInfo[k].m_iImageType)
					{
						iTempBottom += m_TypeCount[k][j];
					}	
				}		
			}
			for (int j= 0 ; j<50 ;j++)
			{
				ErrorAllCount += m_CameraTypeCount[j];
			}
			m_ExcelObj->SetGeneralInfo(row,QString::fromLocal8Bit("班次：%1\n检测总数：%2   不合格数：%3   不合格率：%4\n瓶身剔废：%5   瓶口剔废：%6   瓶底剔废：%7")
				.arg(shiftStr)
				.arg(m_Allcount)
				.arg(m_FailCount)
				.arg(QString::number((double)m_FailCount * 100 / m_Allcount,'f',2) + "%")
				.arg(iTempbody)
				.arg(iTempfinish)
				.arg(iTempBottom));
			//标题
			m_ExcelObj->SetTatilRow(row+1);
			//每种缺陷
			int tmpRow = 0;
			for (int k=0;k<50;k++)
			{
				if (m_CameraTypeCount[k] == 0)
				{
					continue;
				}
				tmpRow += 1;
				QStringList dataList;
				dataList<<pMainFrm->m_sErrorInfo.m_vstrErrorType.at(k)
					<<QString::number(m_CameraTypeCount[k])
					<<QString::number((double)m_CameraTypeCount[k] * 100 / ErrorAllCount , 'f',2) + "%";
				for (int pIndex =0 ;pIndex<pMainFrm->m_sSystemInfo.iCamCount ; pIndex++)
				{
					dataList<<QString::number(m_TypeCount[pIndex][k]);
				}
				m_ExcelObj->SetRowData(row+1+tmpRow,dataList);
			}
			//合计
			QStringList dataList;
			dataList<<tr("SUM")
				<<QString::number(ErrorAllCount)
				<<QString("");
			for (int pIndex =0 ;pIndex<pMainFrm->m_sSystemInfo.iCamCount ; pIndex++)
			{
				dataList<<QString::number(TempCameraCount[pIndex]);
			}
			m_ExcelObj->SetRowData(row+2+tmpRow,dataList);

			m_ExcelObj->SetBolder(row,row+tmpRow+2);

		}
		iShowAllnum = false;
	}

	if (m_ExcelObj->m_Version.toFloat() > 11)
	{ //xls是excel2003及以前版本生成的文件格式，而xlsx是excel2007及以后版本生成的文件格式。
		fileName = fileName + "x";
	}
	m_ExcelObj->SaveAs(fileName);
	delete m_ExcelObj;
	m_ExcelObj = NULL;

	QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
}

void ExportExcelThread::InitShiftTime(QTime pShift1,QTime pShift2,QTime pShift3)
{
	m_shifts.clear();
	m_shifts<<pShift1<<pShift2<<pShift3;
	qSort(m_shifts);
	QTime curTime=QTime::currentTime();
	if (curTime > m_shifts[0] && curTime <= m_shifts[1] )
		curShift = 0 ;
	else if(curTime > m_shifts[1] && curTime <= m_shifts[2])
		curShift = 1 ;
	else
		curShift = 2 ;

	QDate dateSelecte = QDate::currentDate();
	if (m_Date == dateSelecte )
	{
		iShowAllnum =true;
	}
}

