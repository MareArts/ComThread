#include "stdafx.h"
#include "CommThread.h"

void CQueue::Clear()
{
	m_iHead = m_iTail =0;
	memset(buff,0,BUFF_SIZE);
}

CQueue::CQueue()
{
	Clear();
}
int CQueue::GetSize()
{
	return (m_iHead - m_iTail + BUFF_SIZE) % BUFF_SIZE;
}
BOOL CQueue::PutByte(BYTE b)
{
	if(GetSize() == (BUFF_SIZE-1)) return FALSE;
	buff[m_iHead++] =b;
	m_iHead %= BUFF_SIZE;
	return TRUE;
}
BOOL CQueue::GetByte(BYTE *pb)
{
	if(GetSize() == 0) return FALSE;
	*pb = buff[m_iTail++];
	m_iTail %= BUFF_SIZE;
	return TRUE;
}

CCommThread::CCommThread()
{
	m_bConnected = FALSE;
	m_pCallerWnd = NULL;
	//m_RevThreadEvent = new CEvent(FALSE,FALSE,NULL,NULL);
	//ASSERT(m_RevThreadEvent != NULL);
}

CCommThread::~CCommThread()
{
	//m_RevThreadEvent = NULL;
	//delete m_RevThreadEvent;
}

void CCommThread::SetCallerWnd(CWnd * pCallerWnd)
{
	m_pCallerWnd = pCallerWnd;
	if(m_pCallerWnd == NULL)
	{
		AfxMessageBox(L"Can't Find m_pCallerWnd");
	}
}

// 포트열기
BOOL CCommThread::OpenPort(CString strPortName, 
						   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;

	m_sPortName = strPortName;
	
	m_hComm = CreateFile( m_sPortName, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING, 
		0, 
		NULL);

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"fail Port open");
		return FALSE;
	}

	SetCommMask( m_hComm, EV_RXCHAR);	
	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	
	PurgeComm( m_hComm,					
	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts( m_hComm, &timeouts);
	
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	
	dcb.BaudRate = dwBaud;
	dcb.ByteSize = byData;
	dcb.Parity = byParity;
	dcb.StopBits = byStop;
	
	dcb.fBinary = TRUE;				// 바이너리모드, EOF체크 안함
	dcb.fParity = FALSE;				// 패리티 체크의 사용 여부	
	dcb.fOutxCtsFlow = FALSE;			// CTS 출력 흐름제어 사용 여부
	dcb.fOutxDsrFlow = FALSE;			// DSR 출력 흐름제어 사용 여부
	dcb.fDtrControl = DTR_CONTROL_DISABLE;	// DTR 흐름제어 형태
	dcb.fDsrSensitivity = FALSE;			// DSR 감도
	dcb.fTXContinueOnXoff = FALSE;		// XOFF continues Tx 
	dcb.fOutX = FALSE;				// XON/XOFF 출력 흐름제어 사용 여부
	dcb.fInX = FALSE;				// XON/XOFF 입력 흐름제어 사용 여부
	dcb.fErrorChar = FALSE;			// Disable error replacement 
	dcb.fNull = FALSE;				// Disable null stripping 
	dcb.fRtsControl = RTS_CONTROL_DISABLE;	// RTS 흐름제어
	dcb.fAbortOnError = TRUE;			// 에러 무시
	

	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	m_bConnected = TRUE;
	m_hThreadWatchComm = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm, 
		this, 0, &dwThreadID);

	if (! m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	return TRUE;
}

// 포트 닫기
void CCommThread::ClosePort()
{
	m_bConnected = FALSE;
	SetCommMask( m_hComm, 0);
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	CloseHandle(m_hComm);
}
// 데이터 쓰기
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwErrorFlags;
	COMSTAT	comstat;

	if( !m_bConnected )		
	{
		return 0;
	}

	//TRACE(L"Write FIle %s \r\n", CString(pBuff));

	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, 0))
	{
		ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		//TRACE(L"ERROR~\r\n");
	}
	//TRACE(L"Written %d \r", dwWritten);
	return dwWritten;
}

// 데이터 읽기
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead, dwErrorFlags;
	COMSTAT comstat;
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	//TRACE(L"Read FIle %d \r\n",dwRead);
	if(dwRead > 0)
	{
		//TRACE(L"Read FIle %s \r\n", CString(pBuff));
		if( !ReadFile( m_hComm, pBuff, dwRead, &dwRead, 0) )
		{
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
		else 
		{
			//TRACE(L"Read FIle %s \r\n", CString(pBuff));
			//pBuff[dwRead] = 0;
		}
	}
	return dwRead;
}


// 포트감시를 위한 스레드
DWORD	ThreadWatchComm(CCommThread* pComm)
{
	DWORD           dwEvent;
	BOOL            bOk = TRUE;
	BYTE            buff[2048];     
	DWORD           dwRead; 	
	HANDLE			h_GetHandle;

	if( !SetCommMask( pComm->GetHandle(), EV_RXCHAR) )
	{
		bOk = FALSE;  
	}

	if( !bOk )
	{
		AfxMessageBox(_T("Error while creating ThreadWatchComm, ") + pComm->GetPortName());
		return FALSE;
	}

	h_GetHandle = pComm->GetHandle();
	ASSERT(h_GetHandle != NULL);
	while (pComm->IsConnected())
	{
		dwEvent = 0;
		WaitCommEvent( h_GetHandle, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwRead = pComm->ReadComm( buff, 2048); 
				if(BUFF_SIZE - pComm->GetSize() > (int)dwRead)
				{
					for( WORD i = 0; i < dwRead; i++ )
					{
						pComm->PutByte(buff[i]); 
					}
				}
				else
					AfxMessageBox(_T("Buffer Full"));
			}while(dwRead);
		}
		
		pComm->m_pCallerWnd->PostMessageW(WM_RECEIVE_DATA,NULL,NULL);
		Sleep(0);
	}
	pComm->SetThreadHandle(NULL);
	return FALSE;
}

BOOL CCommThread::GetByte (BYTE* pData)
{
	m_critic.Lock ();

	BOOL bRet = m_QueueRead.GetByte (pData);

	m_critic.Unlock ();

	return bRet;
}

BOOL CCommThread::PutByte (BYTE btData)
{
	m_critic.Lock ();

	int bRet = m_QueueRead.PutByte (btData);

	m_critic.Unlock ();

	return bRet;
}

int	CCommThread::GetSize ()
{
	int DataSize = m_QueueRead.GetSize();

	return DataSize;
}

HANDLE	CCommThread::GetHandle ()
{
	return this->m_hComm;
}

/*
CEvent * CCommThread::GetEvent()
{
	return this->m_RevThreadEvent;
}
*/

CString	CCommThread::GetPortName()
{
	return this->m_sPortName;
}

BOOL CCommThread::IsConnected()
{
	return this->m_bConnected;
}

HANDLE CCommThread::GetThreadHandle()
{
	return this->m_hThreadWatchComm;
}

void CCommThread::SetThreadHandle(HANDLE handle)
{
	m_hThreadWatchComm = handle;
}