#ifndef EXCELOBJECT_H
#define EXCELOBJECT_H

#include <QAxObject>
#include <QColor>
#include <QDir>
#include "Windows.h"

class ExcelObject : public QObject
{
	Q_OBJECT

public:
	ExcelObject(QObject *parent=0);
	~ExcelObject();

	bool init();
	void SetGeneralInfo(int row,QString strInfo);
	void SetTatilRow(int row);
	void SetRowData(int row,QStringList pdatalist);
	void SetBolder(int startRow,int endRow);

	void SaveAs(QString fileName);

	QString m_Version;

private:
	QAxObject *excel;
	QAxObject *workbooks;
	QAxObject *workbook ;
	QAxObject *worksheet;
};

#endif // EXCELOBJECT_H
