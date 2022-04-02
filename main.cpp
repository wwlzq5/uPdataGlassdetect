#include <QApplication>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QTranslator>
#include <QSettings>
#include <QTextCodec>
//QT系统类型
#define DAHENGBLPKP_QT			//QT系统时定义该类型，否则注释掉
#include "glasswaredetectsystem.h"
//QT检测内存泄漏头文件
#include "setDebugNew.h"
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC 

#include "winuser.h"
#include "tlhelp32.h"
#include "tchar.h"
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")
static long  __stdcall CrashInfocallback(_EXCEPTION_POINTERS *pexcp);
//关闭同名死进程
void KillSameDeathProcess(QString exestr)
{
	/*HANDLE handle;
	HANDLE handle1;
	handle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 *info; 

	info=new PROCESSENTRY32;  
	info->dwSize=sizeof(PROCESSENTRY32); 

	Process32First(handle,info); 

	do
	{
		QString str = QString::fromWCharArray(info->szExeFile);
		if(exestr==str) 		
		{ 
			handle1=OpenProcess(PROCESS_TERMINATE,FALSE,info->th32ProcessID);
			TerminateProcess(handle1,0); 			
		}
	}while (Process32Next(handle,info)!=FALSE); 
	CloseHandle(handle); */
}

//仅运行一次
bool OnlyRunOnce(QString qstr,QString qstrext)
{
	HANDLE hMapping = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 16, qstr.utf16());
	if (hMapping == NULL)
	{
		return false;
	}
	else
	{
		if(GetLastError()==ERROR_ALREADY_EXISTS)
		{
			HWND hRecv = NULL;
			hRecv = ::FindWindow(NULL,qstr.utf16());
			//hRecv = ::FindWindow(NULL,QString("GlassDetectSystem").utf16());
			if (hRecv != NULL)
			{

				//Qt 无法获取该标记，原因未知，禁止最小化
				//if (::IsIconic(hRecv))
				//	::ShowWindow(hRecv,SW_SHOWNORMAL);
				//::ShowWindow(hRecv,SW_MAXIMIZE);
				//::SetForegroundWindow(hRecv);
				::BringWindowToTop(hRecv);
 				return false;
			}
			else
			{
				KillSameDeathProcess(qstrext);
			}
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashInfocallback);
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	QApplication a(argc, argv);
	//设置固定风格，不跟随系统变化而变化
	//QApplication::setStyle("plastique");
	//防止图标不显示
	QApplication::addLibraryPath("./QtPlugins");
	//执行文件名称+扩展名
	QString exepath = argv[0];
	QString exenameext = exepath.right(exepath.length()-exepath.lastIndexOf("\\")-1);
	QString apppath = exepath.left(exepath.lastIndexOf("\\")+1);
	QString exename = exenameext.left(exenameext.lastIndexOf("."));
	//只运行一个实例
	if (!OnlyRunOnce(exename,exenameext))
	{
//		QMessageBox::information(NULL,QObject::tr("System Prompted"),QObject::tr("Can not run multiple instances！"));//系统提示：系统已经运行，无法重复打开！	
		return 0;
	}
	QTranslator translator;
	QSettings setTranslation(".\\Config\\Config.ini",QSettings::IniFormat);
	setTranslation.setIniCodec(QTextCodec::codecForName("GBK"));

	int iLanguage = setTranslation.value("/system/Language",0).toInt();//	m_sErrorInfo.m_iErrorTypeCount = erroriniset.value(strSession,0).toInt();
	if (0 == iLanguage)
	{
 		translator.load(".\\glasswaredetectsystem_zh.qm");
 		a.installTranslator(&translator);
	}

	a.addLibraryPath("plugins/codecs/");
	//加载QSS样式表
	QFile qss(".\\GlasswareDetect.qss");
	qss.open(QFile::ReadOnly);
 	qApp->setStyleSheet(qss.readAll());
	qss.close();

	QString strLoading = ":/loading/loading";
	QSplashScreen spLoading(QPixmap(strLoading.toLocal8Bit().constData()));
	spLoading.show();

	GlasswareDetectSystem w;
	w.SetLanguage(iLanguage);
	//加密狗
	w.Init();
	bool bReturn = w.CheckLicense();
	if(!bReturn)
		return false;
	w.showNormal();
	w.raise();//放到最上
	w.activateWindow();
	spLoading.finish(&w);
	int nResult = a.exec();
	return nResult; 
}
long  __stdcall CrashInfocallback( _EXCEPTION_POINTERS *pexcp)
{
	HANDLE hDumpFile = ::CreateFile(
		L"MEMORY.DMP",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if( hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pexcp;
		dumpInfo.ThreadId = ::GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;
		::MiniDumpWriteDump(
			::GetCurrentProcess(),
			::GetCurrentProcessId(),
			hDumpFile,
			MiniDumpNormal,
			&dumpInfo,
			NULL,
			NULL
			);
	}
	return 0;
}