
// SerialAppDlg.h : ͷ�ļ�
//

#pragma once


// CSerialAppDlg �Ի���
class CSerialAppDlg : public CDialogEx
{
// ����
public:
	CSerialAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERIALAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboDevice();
	// ��ǰ����ʹ�õ����⴮�ھ��
	HANDLE m_hDevice;
	// ��ָ���Ĵ���
	HANDLE OpenComm(CString strCom);
	// �������⴮��Ӳ���豸������ֵ���豸����
	int SearchDevice(void);
	CComboBox m_DeviceComBox;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonSearchDevice();
	afx_msg void OnBnClickedButtonSend();
	// �������ݵı�־
	BOOL m_bDeviceBusy;
	afx_msg void OnBnClickedButtonReceive();
	// ���ڽ������ݵı�־
	BOOL m_bIsReceive;
};
