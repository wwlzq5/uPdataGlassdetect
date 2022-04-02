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

	void initLogFile();			//��ʼ��Log�ļ�
	void ReadLogFile();			//��ʼ����������ʱ��
	void InitRemainDays();		//��ʼ��ʣ������
	void ResetLogFile(int pIndex = -1);		//�豸��ɱ������������ñ�����ʱ��
	void EquipExitLogFile();	//����˳���¼���������ʱ��


signals:
	void SendAlarms(int ,bool);		//���ͱ��� �������ţ��Ƿ񱨾���
	void SendRemainDays(int,int); //����ʣ������

protected slots:
	void Slots_timer();

private:
	static QScopedPointer<EquipRuntime> self;
	QString LogFileName;

	int saveInterval;
	QDateTime startTime;
	QTimer *m_Timer;
	QList<int> m_CumulTime;		//Сʱ ��ʹ���ۼ�ʱ��

	QList<bool> m_AlarmsSatus;
};

#endif // EQUIPRUNTIME_H
