
// SerialAppDlg.h : 头文件
//

#pragma once


// CSerialAppDlg 对话框
class CSerialAppDlg : public CDialogEx
{
// 构造
public:
	CSerialAppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERIALAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboDevice();
	// 当前正在使用的虚拟串口句柄
	HANDLE m_hDevice;
	// 打开指定的串口
	HANDLE OpenComm(CString strCom);
	// 搜索虚拟串口硬件设备。返回值：设备数量
	int SearchDevice(void);
	CComboBox m_DeviceComBox;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonSearchDevice();
	afx_msg void OnBnClickedButtonSend();
	// 传输数据的标志
	BOOL m_bDeviceBusy;
	afx_msg void OnBnClickedButtonReceive();
	// 正在接收数据的标志
	BOOL m_bIsReceive;
};
