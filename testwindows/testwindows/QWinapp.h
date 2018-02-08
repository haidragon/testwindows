#pragma once
#include "stdafx.h"

class QWinapp
{
public:
	QWinapp();
	~QWinapp();
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	virtual void run();

public:
	LPVOID m_pMainWnd;
};

