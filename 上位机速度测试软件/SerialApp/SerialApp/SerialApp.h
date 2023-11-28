
// SerialApp.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSerialAppApp:
// 有关此类的实现，请参阅 SerialApp.cpp
//

class CSerialAppApp : public CWinApp
{
public:
	CSerialAppApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	// 调整系统时间粒度
	void SetTimeResolution(void);
};

extern CSerialAppApp theApp;