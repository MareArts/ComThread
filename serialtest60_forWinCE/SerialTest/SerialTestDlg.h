// SerialTestDlg.h : ��� ����
//

#pragma once

#include "CommThread.h"
#include "afxwin.h"

// CSerialTestDlg ��ȭ ����
class CSerialTestDlg : public CDialog
{

public:
	CCommThread*		m_CommThread;
	UINT	m_uDataEndFlag;
	//UINT	m_uErrorCount;
	//UINT	m_uReceiveCount;
	CString	m_strReceive;
	CComboBox m_cBaudrate;

	CComboBox m_cPortNuber;
	int			m_iPortNumber;
	int			m_iBaudRate;
	UINT		m_exFlag;
	CWinThread *	m_hTxThread;
public:
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
// �����Դϴ�.
public:
	CSerialTestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SERIALTEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	
	CString m_strSendData;
	CEdit m_cReceiveStatus;

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonoPortOpen();
	afx_msg void OnBnClickedButtonSendData();
	afx_msg void OnBnClickedButtonClosePort();
	afx_msg void OnBnClickedButtonSend();
	UINT m_uErrorCount;
	UINT m_uReceiveCount;
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

UINT	TXThread(LPVOID lParam);
