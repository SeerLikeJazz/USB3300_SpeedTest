
// SerialApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSerialAppApp:
// �йش����ʵ�֣������ SerialApp.cpp
//

class CSerialAppApp : public CWinApp
{
public:
	CSerialAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	// ����ϵͳʱ������
	void SetTimeResolution(void);
};

extern CSerialAppApp theApp;