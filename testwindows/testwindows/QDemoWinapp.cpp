#include "QDemoWinapp.h"
#include "QMainframe.h"


QDemoWinapp::QDemoWinapp()
{
}


QDemoWinapp::~QDemoWinapp()
{
}
QDemoWinapp theApp;
BOOL QDemoWinapp::InitInstance()
{
	QMainframe* pMainWnd = new QMainframe();
	assert(pMainWnd);
	//QWinapp  ->m_pMainWnd
	m_pMainWnd = pMainWnd;
	//���ø��� QWnd CreateEx()
	BOOL bRet = pMainWnd->CreateEx(0, _T("www.itxueba.org"), _T("www.itxueba.org"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL);
	if (!bRet)
	{
		MessageBox(NULL, _T("��������ʧ�ܣ�"), _T("�������ڣ�"), 0);
		return FALSE;
	}

	pMainWnd->ShowWindow(SW_SHOW);
	pMainWnd->UpdateWindow();
	return TRUE;
}

BOOL QDemoWinapp::ExitInstance()
{
	if (m_pMainWnd)
	{
		delete (QWnd*)m_pMainWnd;
		m_pMainWnd = NULL;
	}
	return TRUE;
}
