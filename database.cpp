#include "database.h"
#include <Qdebug>
DataBase::DataBase(int temp)
{
	cameraCount = temp;
	createConnection();
	createTable();
	memset(&m_MyData,0,sizeof(TemporaryData));
}
//建立一个数据库连接
bool DataBase::createConnection()
{
	//QStringList strlist = QSqlDatabase::drivers();
	//以后就可以用"sqlite1"与数据库进行连接了
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "sqlite1");
	db.setDatabaseName(".//Report.db");
	if( !db.open())
	{
		return false;
	}
	return true;
}

//创建数据库表
bool DataBase::createTable()
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	QString sql("create table Report(id varchar,AllCount int,TickCount int,type int,failCount int");
	for(int i=1;i<=cameraCount;i++)
	{
		sql+=QString(",camera%1 int").arg(i);
	}
	sql+=")";
	bool success = query.exec(sql);
	if(success)
	{
		return true;
	}
	else
	{
		/*QString sqlDelete("drop table Report");
		query.exec(sqlDelete);
		query.exec(sql);*/
		return false;
	}
}

//向数据库中插入记录
bool DataBase::insert(QString id,TemporaryData temp)
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	QString sql="";
	for(int i=1;i<50;i++)
	{
		if(temp.m_CameraTypeCount[i] == 0)
		{
			continue;
		}
		sql +="insert into Report values(";
		sql += id;
		sql+=QString(",%1").arg(temp.m_AllCount);
		sql+=QString(",%1").arg(temp.m_FailCount);
		sql+=QString(",%1").arg(i);
		sql+=QString(",%1").arg(temp.m_CameraTypeCount[i]);
		for(int j=1; j <= cameraCount; j++)
		{
			sql+=QString(",%1").arg(temp.m_TypeCount[j][i]);
		}
		sql+=")";
		query.exec(sql);
		sql.clear();
	}
	return true;
}

//查询所有信息
QList<SeleteData> DataBase::queryAll(QString id)
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	query.exec(QString("select * from Report where id like '%1%'").arg(id));//20200921____
	//query.exec(QString("select * from Report where id like '%1__00'").arg(id));//20200921__00

	QSqlRecord rec = query.record();
	QList<SeleteData> m_SeleteData;
	while(query.next())
	{
		SeleteData m_data;
		m_data.id = query.value(0).toLongLong();
		m_data.m_AllCount = query.value(1).toInt();
		m_data.m_FailCount = query.value(2).toInt();
		m_data.m_Type = query.value(3).toInt();
		m_data.m_CameraTypeCount = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_data.m_TypeCount[i] = query.value(i+5).toInt();
		}
		m_SeleteData<<m_data;
		m_data.SeleteDatClear();
	}
	return m_SeleteData;
	/*
	m_MyData.TemporaryDataClear();
	int temp=1;
	while(query.next())
	{
		m_MyData.id[temp] = query.value(0).toLongLong();
		m_MyData.m_AllCount = query.value(1).toInt();
		m_MyData.m_FailCount = query.value(2).toInt();
		m_MyData.m_Type[temp] = query.value(3).toInt();
		m_MyData.m_CameraTypeCount[temp] = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_MyData.m_TypeCount[i][temp] = query.value(i+5).toInt();
		}
		temp++;
	}
	return m_MyData;
	*/
}

QList<SeleteData> DataBase::queryAll(QString startTime,QString endTime)
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	query.exec(QString("select * from Report where id > %2 and id <= %3 ").arg(startTime).arg(endTime));//20200921____

	QSqlRecord rec = query.record();
	QList<SeleteData> m_SeleteData;
	while(query.next())
	{
		SeleteData m_data;
		m_data.id = query.value(0).toLongLong();
		m_data.m_AllCount = query.value(1).toInt();
		m_data.m_FailCount = query.value(2).toInt();
		m_data.m_Type = query.value(3).toInt();
		m_data.m_CameraTypeCount = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_data.m_TypeCount[i] = query.value(i+5).toInt();
		}
		m_SeleteData<<m_data;
		m_data.SeleteDatClear();
	}
	return m_SeleteData;
}

//根据ID删除记录
bool DataBase::deleteById(int id)
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	query.prepare(QString("delete from Report where id = %1").arg(id));
	if(!query.exec())
	{
		return false;
	}
	return true;
}

bool DataBase::deleteFromDate(QString dateId)
{
	QString pData = dateId + "0000";
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	query.prepare(QString("delete from Report where id in ( select id from Report where id < %1 )").arg(pData));
	if(!query.exec())
	{
		return false;
	}
	return true;
}

//根据ID更新记录
bool DataBase::updateById(int id)
{
	//query.exec();
	return true;
}

//排序
QList<SeleteData> DataBase::sortById(QString id)
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	bool success=query.exec(QString("select * from Report where id = %1 order by failCount desc ").arg(id));  //降序
	QSqlRecord rec = query.record();
	QList<SeleteData> m_SeleteData;
	while(query.next())
	{
		SeleteData m_data;
		m_data.id = query.value(0).toLongLong();
		m_data.m_AllCount = query.value(1).toInt();
		m_data.m_FailCount = query.value(2).toInt();
		m_data.m_Type = query.value(3).toInt();
		m_data.m_CameraTypeCount = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_data.m_TypeCount[i] = query.value(i+5).toInt();
		}
		m_SeleteData<<m_data;
		m_data.SeleteDatClear();
	}
	return m_SeleteData;
	/*
	memset(&m_MyData,0,sizeof(TemporaryData));
	int temp=1;
	while(query.next())
	{
		m_MyData.id[temp] = query.value(0).toLongLong();
		m_MyData.m_AllCount = query.value(1).toInt();
		m_MyData.m_FailCount = query.value(2).toInt();
		m_MyData.m_Type[temp] = query.value(3).toInt();
		m_MyData.m_CameraTypeCount[temp] = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_MyData.m_TypeCount[i][temp] = query.value(i+5).toInt();
		}
		temp++;
	}
	return m_MyData;
	*/
}

QList<SeleteData> DataBase::queryAllByOrder( QString id )
{	//查询当天所有数据 按时间和不同缺陷的缺陷数 升序 查询
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	query.exec(QString("select * from Report where id like '%1%' order by id,failCount").arg(id));//20200921____
	//query.exec(QString("select * from Report where id like '%1__00'").arg(id));//20200921__00

	QSqlRecord rec = query.record();
	QList<SeleteData> m_SeleteData;
	while(query.next())
	{
		SeleteData m_data;
		m_data.id = query.value(0).toLongLong();
		m_data.m_AllCount = query.value(1).toInt();
		m_data.m_FailCount = query.value(2).toInt();
		m_data.m_Type = query.value(3).toInt();
		m_data.m_CameraTypeCount = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_data.m_TypeCount[i] = query.value(i+5).toInt();
		}
		m_SeleteData<<m_data;
		m_data.SeleteDatClear();
	}
	return m_SeleteData;
}

QList<SeleteData> DataBase::queryOnce( QString id )
{
	QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
	QSqlQuery query(db);
	bool success=query.exec(QString("select * from Report where id = %1 order by failCount").arg(id));  //缺陷数升序
	QSqlRecord rec = query.record();
	QList<SeleteData> m_SeleteData;
	while(query.next())
	{
		SeleteData m_data;
		m_data.id = query.value(0).toLongLong();
		m_data.m_AllCount = query.value(1).toInt();
		m_data.m_FailCount = query.value(2).toInt();
		m_data.m_Type = query.value(3).toInt();
		m_data.m_CameraTypeCount = query.value(4).toInt();
		for(int i = 0;i<cameraCount;i++)
		{
			m_data.m_TypeCount[i] = query.value(i+5).toInt();
		}
		m_SeleteData<<m_data;
		m_data.SeleteDatClear();
	}
	return m_SeleteData;
}
