#include "QWinapp.h"
#include "Global.h"

//类QWinapp为初始化基类 QDemoWinapp继承它 
//类QWnd为创建窗口基类  QMainframe 继承它 
int WINAPI _tWinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPTSTR lpCmdLine, IN int nShowCmd)
{
//#include ".\global.h"
//	QWinapp* g_pWinApp = NULL;
//	QWinapp* GlbGetApp()
//	{
//		return g_pWinApp;
//	}

	//通过父类QWinapp指针调用派生类初始化
	//派生类调用QMainframe创建窗口而达到初始化
	QWinapp* pWinapp = GlbGetApp();
	assert(pWinapp);
	//class QDemoWinapp :
	//	public QWinapp
	pWinapp->InitInstance();//QDemoWinapp->InitInstance();
	pWinapp->run();
	pWinapp->ExitInstance();
	return TRUE;
}