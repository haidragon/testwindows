#include "QWinapp.h"
#include "Global.h"

//��QWinappΪ��ʼ������ QDemoWinapp�̳��� 
//��QWndΪ�������ڻ���  QMainframe �̳��� 
int WINAPI _tWinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPTSTR lpCmdLine, IN int nShowCmd)
{
//#include ".\global.h"
//	QWinapp* g_pWinApp = NULL;
//	QWinapp* GlbGetApp()
//	{
//		return g_pWinApp;
//	}

	//ͨ������QWinappָ������������ʼ��
	//���������QMainframe�������ڶ��ﵽ��ʼ��
	QWinapp* pWinapp = GlbGetApp();
	assert(pWinapp);
	//class QDemoWinapp :
	//	public QWinapp
	pWinapp->InitInstance();//QDemoWinapp->InitInstance();
	pWinapp->run();
	pWinapp->ExitInstance();
	return TRUE;
}