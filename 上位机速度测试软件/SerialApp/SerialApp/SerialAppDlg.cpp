
// SerialAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SerialApp.h"
#include "SerialAppDlg.h"
#include "afxdialogex.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialAppDlg 对话框




CSerialAppDlg::CSerialAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialAppDlg::IDD, pParent)
	, m_hDevice(NULL)
	, m_bDeviceBusy(FALSE)
	, m_bIsReceive(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_DeviceComBox);
}

BEGIN_MESSAGE_MAP(CSerialAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CSerialAppDlg::OnCbnSelchangeComboDevice)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_DEVICE, &CSerialAppDlg::OnBnClickedButtonSearchDevice)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSerialAppDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_RECEIVE, &CSerialAppDlg::OnBnClickedButtonReceive)
END_MESSAGE_MAP()


// CSerialAppDlg 消息处理程序

BOOL CSerialAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SearchDevice();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSerialAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSerialAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSerialAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//// 更改串口设备
void CSerialAppDlg::OnCbnSelchangeComboDevice()
{
	//// 更改串口设备
	CloseHandle(m_hDevice);
	m_hDevice = NULL;
	int nSelDev = m_DeviceComBox.GetCurSel();
	CString strComm;
	m_DeviceComBox.GetLBText(nSelDev, strComm);
	m_hDevice = OpenComm(strComm);
}


// 打开指定的串口
HANDLE CSerialAppDlg::OpenComm(CString strCom)
{
	HANDLE hCom = NULL;
	strCom = "\\\\.\\" + strCom;		//// 微软预定义的标准设备中只含有“COM1”-“COM9”，COM10及以上串行端口应当使用"\\.\COM10"这样的串口专用名称
	//// 1. 打开串口
	hCom = CreateFile(strCom,		// COM口名称
		GENERIC_READ|GENERIC_WRITE, // 允许读和写
		0,							// 独占方式
		NULL,						// 引用安全性属性结构，缺省值为NULL；
		OPEN_EXISTING,				// 打开而不是创建
		0,							// 同步方式
		NULL);
	if (hCom == (HANDLE)-1)
	{
		// ErrorMessage(strCom+_T(": 打开串口失败!"));
		return hCom = NULL;
	}
	//// 2. 设置串口
	SetupComm(hCom, 102400, 524288);		//输出(PC-->USB)缓冲区大小100KB，输入(USB-->PC)缓冲区的大小是512KB
	COMMTIMEOUTS TimeOuts;					//设定读超时
	TimeOuts.ReadIntervalTimeout = 10;
	TimeOuts.ReadTotalTimeoutMultiplier = 10;
	TimeOuts.ReadTotalTimeoutConstant = 10;	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 10;
	TimeOuts.WriteTotalTimeoutConstant = 10;
	SetCommTimeouts(hCom, &TimeOuts);			//设置超时
	DCB dcb; GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200;						//波特率为115200
	dcb.ByteSize = 8;							//每个字节有8位
	dcb.Parity = NOPARITY;						//无奇偶校验位
	dcb.StopBits = ONESTOPBIT;					//1个停止位
	SetCommState(hCom, &dcb);
	PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); //清干净输入、输出缓冲区
	return hCom;
}


// 搜索串口硬件设备。返回值：设备数量
int CSerialAppDlg::SearchDevice(void)
{
	if (m_hDevice!=NULL)
	{
		CloseHandle(m_hDevice);
		m_hDevice = NULL;
	}
	//CStringArray ary;
	m_DeviceComBox.ResetContent();
	int nDeviceNum = 0;
	HKEY hkey;
	LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T( "HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hkey);
	if (lRes == ERROR_SUCCESS)
	{ 
		TCHAR tchKey[MAX_PATH];
		TCHAR tchValue[20];
		DWORD dwIndex = 0;
		DWORD dwType = REG_SZ;
		while(lRes == ERROR_SUCCESS)
		{
			DWORD dwCount = MAX_PATH;
			DWORD dwVCount = 20;
			lRes = RegEnumValue(hkey, dwIndex++, tchKey, &dwCount, NULL, &dwType, (LPBYTE)tchValue, &dwVCount);
			if(lRes == ERROR_SUCCESS)
			{
				if((dwVCount > 0) && (dwCount > 0))
				{
					m_DeviceComBox.AddString(tchValue);
					nDeviceNum++;
				}
			}
		}
	}
	RegCloseKey(hkey);

	if (nDeviceNum > 0)
	{
		m_DeviceComBox.SetCurSel(0);
		CString strComm;
		m_DeviceComBox.GetLBText(0, strComm);
		m_hDevice = OpenComm(strComm);
	}
	return nDeviceNum;
}


void CSerialAppDlg::OnClose()
{
	//// 关闭串口
	CloseHandle(m_hDevice);

	CDialogEx::OnClose();
}

//// 重新搜索硬件设备
void CSerialAppDlg::OnBnClickedButtonSearchDevice()
{
	SearchDevice();
}

int Data[104857600];

//// 发送批量数据到串口
void CSerialAppDlg::OnBnClickedButtonSend()
{
	int nLastTime = timeGetTime();
	DWORD dwRet = 0;
	DWORD dwTransNum = 0;

	for (int i=0; i<102400; i++)
		Data[i] = 0xbbccddee;

	if (!m_bDeviceBusy)
	{
		m_bDeviceBusy = TRUE;
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		for (int i=0; i<10; i++)
		{
			if (!WriteFile(m_hDevice, (LPVOID) &Data, 102400, &dwRet, NULL))
				break;
			dwTransNum += dwRet;		//// 累加传送的数据量
		}
		int nTime = timeGetTime() - nLastTime;
		nTime = (nTime==0) ? 1 : nTime;
		CString strText;
		strText.Format("发送数据速度：%.2f KB/s", ((float)dwTransNum)/(nTime));
		SetDlgItemText(IDC_STATIC_TEXT, strText);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		m_bDeviceBusy = FALSE;
	}
}

//// 从虚拟串口接收批量数据
void CSerialAppDlg::OnBnClickedButtonReceive()
{
	int nLastTime = timeGetTime();
	DWORD dwRet = 0;
	DWORD dwTransNum = 0;

	if (!m_bIsReceive)
	{
		m_bIsReceive = TRUE;
		GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(FALSE);
		UINT uInstruct = 0x55;			// 启动数据传输的指令
		WriteFile(m_hDevice, (LPVOID) &uInstruct, sizeof(UINT), &dwRet, NULL);
		for (int i=0; i<10; i++)
		{
			if (!ReadFile(m_hDevice, (LPVOID) &Data, 102400, &dwRet, NULL))
				break;
			dwTransNum += dwRet;		//// 累加接收的数据量
		}
		uInstruct = 0xAA;				// 终止数据传输的指令
		WriteFile(m_hDevice, (LPVOID) &uInstruct, sizeof(UINT), &dwRet, NULL);
		int nTime = timeGetTime() - nLastTime;
		nTime = (nTime==0) ? 1 : nTime;
		CString strText;
		strText.Format("接收数据速度：%.2f KB/s", ((float)dwTransNum)/(nTime));

		// 把STM32来不及停下来而发送的多余数据也读回来，以免影响STM32主循环运行
		COMMTIMEOUTS TimeOut = {100,100,100,100,100};
		SetCommTimeouts(m_hDevice, &TimeOut);
		ReadFile(m_hDevice, (LPVOID) &Data, 102400, &dwRet, NULL);

		SetDlgItemText(IDC_STATIC_TEXT, strText);
		GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(TRUE);
		m_bIsReceive = FALSE;
	}
}
