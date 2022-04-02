#ifndef EQUIPRUNTIME_H
#define EQUIPRUNTIME_H

#include <QObject>
#include <QDateTime>
#include <QTimer>

class EquipRuntime : public QObject
{
	Q_OBJECT

public:
	static EquipRuntime *Instance();
	EquipRuntime(QObject *parent=NULL);
	~EquipRuntime();

	void start();
	void stop();

	void initLogFile();			//初始化Log文件
	void ReadLogFile();			//初始化各报警项时间
	void InitRemainDays();		//初始化剩余天数
	void ResetLogFile(int pIndex = -1);		//设备完成保养，重新设置报警项时间
	void EquipExitLogFile();	//软件退出记录软件已运行时间


signals:
	void SendAlarms(int ,bool);		//发送报警 （报警号，是否报警）
	void SendRemainDays(int,int); //更新剩余天数

protected slots:
	void Slots_timer();

private:
	static QScopedPointer<EquipRuntime> self;
	QString LogFileName;

	int saveInterval;
	QDateTime startTime;
	QTimer *m_Timer;
	QList<int> m_CumulTime;		//小时 已使用累计时间

	QList<bool> m_AlarmsSatus;
};

#endif // EQUIPRUNTIME_H
