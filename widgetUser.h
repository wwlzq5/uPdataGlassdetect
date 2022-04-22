#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include "ui_userwidget.h"
#include <QTime>
class UserWidget : public QWidget
{
	Q_OBJECT

public:
	UserWidget(QWidget *parent = 0);
	~UserWidget();
	void ShowInterfance();
	void initialUserInfo();
public:
	Ui::UserWidget ui;
	QList<QString> listUser;
	QStringList strUserList;
	QStringList strPasswordList;
	QList<int> nPermissionsList;
signals:
	void signal_LoginState(int nPerm,bool isUnlock);
private:
	QString strUserName;
	QString PassWord;
	QString strPassWordUser;
	QString strPassWordAdmin;
	bool isNewUserStatus;
	bool isOnlyChangePerission;

public:
	int nScreenCount;
	int nPermission;
	bool iUserPerm;
private:
	void initial();
private slots:
	void slots_login();
	void slots_changePassWrod();
	void slots_loginChangePassWrod();
	void slots_CancelchangePassWrod();
	void slots_NewUser();
	void slots_deleteUser();
	void slots_OnlyChangePermission_Checked(int);
	void slots_ShowCount(int,int);
};

#endif // USERWIDGET_H
