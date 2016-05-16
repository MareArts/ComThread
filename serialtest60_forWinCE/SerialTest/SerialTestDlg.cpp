// SerialTestDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SerialTest.h"
#include "SerialTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSerialTestDlg ��ȭ ����

#define sendMsg		m_cReceiveStatus.ReplaceSel(L"SendMsg : ");
#define NextLine	m_cReceiveStatus.ReplaceSel(L"\r\n");
#define Receive		m_cReceiveStatus.ReplaceSel(L"RecvMsg : ");

CSerialTestDlg::CSerialTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialTestDlg::IDD, pParent)
	, m_strSendData(_T(""))
	, m_uErrorCount(0)
	, m_uReceiveCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEND_DATA, m_strSendData);
	DDX_Control(pDX, IDC_EDIT_RECEIVE_DATA, m_cReceiveStatus);
	DDX_Control(pDX, IDC_COMBO_PORT_NUMBER, m_cPortNuber);
	//	DDX_Text(pDX, IDC_EDIT3, m_uErrorCount);
	//	DDX_Text(pDX, IDC_EDIT1, m_uReceiveCount);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_cBaudrate);
}

BEGIN_MESSAGE_MAP(CSerialTestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTONO_PORT_OPEN, &CSerialTestDlg::OnBnClickedButtonoPortOpen)
	ON_BN_CLICKED(IDC_BUTTON_SEND_DATA, &CSerialTestDlg::OnBnClickedButtonSendData)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_PORT, &CSerialTestDlg::OnBnClickedButtonClosePort)
	ON_MESSAGE(WM_RECEIVE_DATA,OnReceiveData)
//	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSerialTestDlg::OnBnClickedButtonSend)
ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSerialTestDlg �޽��� ó����

BOOL CSerialTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_iBaudRate = 0;

	m_CommThread = NULL;
	m_CommThread = new CCommThread;
	ASSERT(m_CommThread != NULL);
	m_uDataEndFlag = 0;
	m_uErrorCount = 0;
	m_uReceiveCount = 0;

	m_CommThread->SetCallerWnd(this);
	m_strReceive =L"";
	
	m_cPortNuber.SetCurSel(0);
	m_cBaudrate.SetCurSel(0);
	MoveWindow(0,0,480,272);

	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSerialTestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SERIALTEST_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SERIALTEST_DIALOG));
	}
}
#endif


void CSerialTestDlg::OnDestroy()
{
	CDialog::OnDestroy();
	if(m_CommThread != NULL)
	{
		delete m_CommThread;
	}
	m_exFlag = 0;

	/*if(m_hTxThread != NULL)
	{
		delete m_hTxThread;
	}*/
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CSerialTestDlg::OnBnClickedButtonoPortOpen()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	CString strPort = L"";

	UpdateData(TRUE);
	m_iPortNumber = m_cPortNuber.GetCurSel();
	switch(m_iPortNumber)
	{
	case 0: strPort = "COM1:"; break;
	case 1: strPort = "COM2:"; break;
	case 2: strPort = "COM3:"; break;
	case 3: strPort = "COM4:"; break;
	case 4: strPort = "COM5:"; break;
	case 5: strPort = "COM6:"; break;
	case 6: strPort = "COM7:"; break;
	case 7: strPort = "COM8:"; break;
	case 8: strPort = "COM9:"; break;

	default: strPort = "COM1:"; break;
	}

	
	switch(m_cBaudrate.GetCurSel())
	{
	case 0: m_iBaudRate = 2400; break;
	case 1: m_iBaudRate = 4800; break;
	case 2: m_iBaudRate = 9600; break;
	case 3: m_iBaudRate = 14400; break;
	case 4: m_iBaudRate = 19200; break;
	case 5: m_iBaudRate = 38400; break;
	case 6: m_iBaudRate = 57600; break;
	case 7: m_iBaudRate = 115200; break;
	default: m_iBaudRate = 9600;break;

	}
		
	if(m_CommThread->IsConnected() == FALSE)
	{
		if(!m_CommThread->OpenPort(strPort,m_iBaudRate,8,0,0))
		{
			m_cReceiveStatus.ReplaceSel(L"��Ʈ���� ����");
			NextLine;
		}
		else
		{
			m_cReceiveStatus.ReplaceSel(L"��Ʈ���� ���� !!");
			NextLine;
			m_cBaudrate.EnableWindow(FALSE);
			m_cPortNuber.EnableWindow(FALSE);			
		}
	}
	else
	{
		m_cReceiveStatus.ReplaceSel(L"�̹� ��Ʈ�� ���� �ֽ��ϴ�.");
		NextLine;
	}
}

void CSerialTestDlg::OnBnClickedButtonSendData()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	int Len;
	Len = WideCharToMultiByte(CP_ACP, 0, m_strSendData, -1, NULL, 0, 0, 0);
	char* Data = new char[sizeof(char) * Len];
	WideCharToMultiByte(CP_ACP, 0, m_strSendData, -1, Data, Len, 0, 0);
	if(m_strSendData != _T(""))
	{
		m_CommThread->WriteComm((unsigned char*)(LPCTSTR)Data, Len);
	}
	if(m_strSendData != L"")
	{
		sendMsg
		m_cReceiveStatus.ReplaceSel(m_strSendData);
		m_strReceive = L"";
		NextLine

	}	
	UpdateData(FALSE);	
}

void CSerialTestDlg::OnBnClickedButtonClosePort()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_CommThread->IsConnected() == TRUE)
	{
		m_CommThread->ClosePort();
		m_cReceiveStatus.ReplaceSel(L"��Ʈ�ݱ� ���� !!");
		m_cBaudrate.EnableWindow(TRUE);
		m_cPortNuber.EnableWindow(TRUE);
		NextLine;
	}
}

LRESULT CSerialTestDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	UINT	uReceiveDataSize = 0;
	UINT	i = 0;
	CString strReceiveData;
	BYTE	aByte;
	
	uReceiveDataSize = m_CommThread->GetSize();
	for(i = 0; i < uReceiveDataSize; i++)
	{
		m_CommThread->GetByte(&aByte);
		strReceiveData.Format(L"%c",aByte);
		m_strReceive += strReceiveData;
	/*	if(strReceiveData == '$')
		{
			m_uDataEndFlag = 0;
			m_strReceive = L"";			
		}
		else if(strReceiveData == '@')
		{
			m_uDataEndFlag = 1;			
		}
		else if(strReceiveData != L"")
		{
			m_strReceive += strReceiveData;	
		}

		if(m_uDataEndFlag == 1)
		{
			if(m_strReceive != L"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz")
			{
				m_uErrorCount++;			
				//TRACE(L"m_uErrorCount :%d\r\n",m_uErrorCount);
			}
			m_uReceiveCount++;
			//TRACE(L"m_uReceiveCount :%d\r\n",m_uReceiveCount);
			m_uDataEndFlag = 0;
			m_cReceiveStatus.SetSel(-1,0);

			if(m_strReceive != L"")
			{
				//m_cReceiveStatus.ReplaceSel(m_strReceive);
				m_strReceive = L"";
			}
		}*/
	}	

	if(m_strReceive != L"")
	{
		Receive
		m_cReceiveStatus.ReplaceSel(m_strReceive);
		m_strReceive = L"";
		NextLine
	}

	UpdateData(FALSE);
	return TRUE;
}
void CSerialTestDlg::OnBnClickedButtonSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CHAR * sendmessage = "$ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz@";
	//m_CommThread->WriteComm((BYTE *)sendmessage,62);
	//m_hTxThread = AfxBeginThread(TXThread,this);
}

UINT	TXThread(LPVOID lParam)
{
	CSerialTestDlg * SerialTx = (CSerialTestDlg *)lParam; 
	SerialTx->m_exFlag = 1;
	CHAR * sendmessage = "$ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz@";
	
	while(SerialTx->m_exFlag)
	{
		SerialTx->m_CommThread->WriteComm((BYTE *)sendmessage,64);	
		Sleep(1500);
	}
	return FALSE;
}
HBRUSH CSerialTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.

	if(nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX)
	{
	  static CBrush s_brush(RGB(156, 255, 255));
	  pDC->SetBkMode(TRANSPARENT);  
	  pDC->SetTextColor(RGB(31, 0, 0));  //���ڻ�(�ϰ� ����)
	  pDC->SetBkColor(RGB(0, 0, 0));  //����
	  return s_brush;
	}
	else if(nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
	{
	  static CBrush s_brush(RGB(205, 255, 206));
	  pDC->SetBkMode(TRANSPARENT);  
	  pDC->SetTextColor(RGB(31, 0, 0));  //���ڻ�(�ϰ� ����)
	  pDC->SetBkColor(RGB(0, 0, 0));  //����
	  return s_brush;
	}

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}
