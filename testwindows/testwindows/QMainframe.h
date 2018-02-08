#pragma once
#include "QWnd.h"
class QMainframe :
	public QWnd
{
public:
	QMainframe();
	virtual ~QMainframe();
public:
	//message handle
	virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
};

