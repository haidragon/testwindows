#pragma once
#include "QWinapp.h"
class QDemoWinapp:
	public QWinapp
{
public:
	QDemoWinapp();
	~QDemoWinapp();
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
};

extern QDemoWinapp theApp;