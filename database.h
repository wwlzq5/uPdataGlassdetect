#ifndef DATABASE_H
#define DATABASE_H
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qsqldriver.h"
#include "qsqlrecord.h"
struct TemporaryData{
	long long id[50];
	int m_CameraTypeCount[50]; //某种缺陷的踢废总数
	int m_AllCount; //当前过检数目
	int m_FailCount; //当前踢废数目
	int m_Type[50]; //错误类型
	int m_TypeCount[30][50]; //每个相机对应的一种缺陷踢废数
	void TemporaryDataClear()
	{
		memset(this,0,sizeof(TemporaryData));
	}
};
struct SeleteData{
	long long id;
	int m_CameraTypeCount; //某种缺陷的踢废总数
	int m_AllCount; //当前过检数目
	int m_FailCount; //当前踢废数目
	int m_Type; //错误类型
	int m_TypeCount[30]; //每个相机对应的一种缺陷踢废数
	void SeleteDatClear()
	{
		memset(this,0,sizeof(SeleteData));
	}
};
class DataBase
{
public:
	DataBase(int);

	enum QueryFlags{
		ByHalfanHour = 0 ,
		ByanHour
	};

public:
	bool createConnection();  //创建一个连接
	bool createTable();       //创建数据库表
	bool insert(QString,TemporaryData);            //出入数据
	QList<SeleteData> queryAll(QString id);          //查询一天所有信息
	QList<SeleteData> queryAllByOrder(QString id);          //查询一天所有信息 降序
	QList<SeleteData> queryAll(QString startTime,QString endTime);          //查询所有信息
	QList<SeleteData> queryOnce(QString id);          //查询一次记录数据
	bool updateById(int id);  //更新
	bool deleteById(int id);  //删除
	bool deleteFromDate(QString dateId);  //删除指定日期之前的数据记录
	QList<SeleteData> sortById(QString);          //排序
	TemporaryData m_MyData;
public:
	int cameraCount;
};
#endif