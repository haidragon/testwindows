#include "QWinapp.h"
#include "Global.h"


QWinapp::QWinapp()
{
	m_pMainWnd = NULL;
	g_pWinApp = this;
}


QWinapp::~QWinapp()
{
}
BOOL QWinapp::InitInstance()
{
	return TRUE;
}

BOOL QWinapp::ExitInstance()
{
	return TRUE;
}

void QWinapp::run()
{
	MSG msg;
	while (::GetMessage(&msg, NULL, NULL, NULL))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}