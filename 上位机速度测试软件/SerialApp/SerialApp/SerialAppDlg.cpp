
// SerialAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SerialApp.h"
#include "SerialAppDlg.h"
#include "afxdialogex.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CSerialAppDlg �Ի���




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


// CSerialAppDlg ��Ϣ�������

BOOL CSerialAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SearchDevice();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSerialAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSerialAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//// ���Ĵ����豸
void CSerialAppDlg::OnCbnSelchangeComboDevice()
{
	//// ���Ĵ����豸
	CloseHandle(m_hDevice);
	m_hDevice = NULL;
	int nSelDev = m_DeviceComBox.GetCurSel();
	CString strComm;
	m_DeviceComBox.GetLBText(nSelDev, strComm);
	m_hDevice = OpenComm(strComm);
}


// ��ָ���Ĵ���
HANDLE CSerialAppDlg::OpenComm(CString strCom)
{
	HANDLE hCom = NULL;
	strCom = "\\\\.\\" + strCom;		//// ΢��Ԥ����ı�׼�豸��ֻ���С�COM1��-��COM9����COM10�����ϴ��ж˿�Ӧ��ʹ��"\\.\COM10"�����Ĵ���ר������
	//// 1. �򿪴���
	hCom = CreateFile(strCom,		// COM������
		GENERIC_READ|GENERIC_WRITE, // �������д
		0,							// ��ռ��ʽ
		NULL,						// ���ð�ȫ�����Խṹ��ȱʡֵΪNULL��
		OPEN_EXISTING,				// �򿪶����Ǵ���
		0,							// ͬ����ʽ
		NULL);
	if (hCom == (HANDLE)-1)
	{
		// ErrorMessage(strCom+_T(": �򿪴���ʧ��!"));
		return hCom = NULL;
	}
	//// 2. ���ô���
	SetupComm(hCom, 102400, 524288);		//���(PC-->USB)��������С100KB������(USB-->PC)�������Ĵ�С��512KB
	COMMTIMEOUTS TimeOuts;					//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = 10;
	TimeOuts.ReadTotalTimeoutMultiplier = 10;
	TimeOuts.ReadTotalTimeoutConstant = 10;	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 10;
	TimeOuts.WriteTotalTimeoutConstant = 10;
	SetCommTimeouts(hCom, &TimeOuts);			//���ó�ʱ
	DCB dcb; GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200;						//������Ϊ115200
	dcb.ByteSize = 8;							//ÿ���ֽ���8λ
	dcb.Parity = NOPARITY;						//����żУ��λ
	dcb.StopBits = ONESTOPBIT;					//1��ֹͣλ
	SetCommState(hCom, &dcb);
	PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); //��ɾ����롢���������
	return hCom;
}


// ��������Ӳ���豸������ֵ���豸����
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
	//// �رմ���
	CloseHandle(m_hDevice);

	CDialogEx::OnClose();
}

//// ��������Ӳ���豸
void CSerialAppDlg::OnBnClickedButtonSearchDevice()
{
	SearchDevice();
}

int Data[104857600];

//// �����������ݵ�����
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
			dwTransNum += dwRet;		//// �ۼӴ��͵�������
		}
		int nTime = timeGetTime() - nLastTime;
		nTime = (nTime==0) ? 1 : nTime;
		CString strText;
		strText.Format("���������ٶȣ�%.2f KB/s", ((float)dwTransNum)/(nTime));
		SetDlgItemText(IDC_STATIC_TEXT, strText);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		m_bDeviceBusy = FALSE;
	}
}

//// �����⴮�ڽ�����������
void CSerialAppDlg::OnBnClickedButtonReceive()
{
	int nLastTime = timeGetTime();
	DWORD dwRet = 0;
	DWORD dwTransNum = 0;

	if (!m_bIsReceive)
	{
		m_bIsReceive = TRUE;
		GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(FALSE);
		UINT uInstruct = 0x55;			// �������ݴ����ָ��
		WriteFile(m_hDevice, (LPVOID) &uInstruct, sizeof(UINT), &dwRet, NULL);
		for (int i=0; i<10; i++)
		{
			if (!ReadFile(m_hDevice, (LPVOID) &Data, 102400, &dwRet, NULL))
				break;
			dwTransNum += dwRet;		//// �ۼӽ��յ�������
		}
		uInstruct = 0xAA;				// ��ֹ���ݴ����ָ��
		WriteFile(m_hDevice, (LPVOID) &uInstruct, sizeof(UINT), &dwRet, NULL);
		int nTime = timeGetTime() - nLastTime;
		nTime = (nTime==0) ? 1 : nTime;
		CString strText;
		strText.Format("���������ٶȣ�%.2f KB/s", ((float)dwTransNum)/(nTime));

		// ��STM32������ͣ���������͵Ķ�������Ҳ������������Ӱ��STM32��ѭ������
		COMMTIMEOUTS TimeOut = {100,100,100,100,100};
		SetCommTimeouts(m_hDevice, &TimeOut);
		ReadFile(m_hDevice, (LPVOID) &Data, 102400, &dwRet, NULL);

		SetDlgItemText(IDC_STATIC_TEXT, strText);
		GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(TRUE);
		m_bIsReceive = FALSE;
	}
}
