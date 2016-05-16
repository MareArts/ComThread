// SerialTestDlg.h : 헤더 파일
//

#pragma once

#include "CommThread.h"
#include "afxwin.h"

// CSerialTestDlg 대화 상자
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
// 생성입니다.
public:
	CSerialTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERIALTEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
