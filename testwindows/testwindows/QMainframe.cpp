#include "QMainframe.h"


//�ȳ�ʼ������
QMainframe::QMainframe()
{
}


QMainframe::~QMainframe()
{
}

// msg 
LRESULT QMainframe::OnClose(WPARAM wParam, LPARAM lParam)
{
	return DestroyWindow();
}

LRESULT QMainframe::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return TRUE;
}

LRESULT QMainframe::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}