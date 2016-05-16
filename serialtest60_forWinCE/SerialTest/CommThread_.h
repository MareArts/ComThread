#define BUFF_SIZE			4192

#define WM_RECEIVE_DATA					(WM_APP + 1)

class CQueue
{
public:
	BYTE buff[BUFF_SIZE];
	int m_iHead, m_iTail;
	CQueue();
	void Clear();
	int GetSize();
	BOOL PutByte(BYTE b);
	BOOL GetByte(BYTE *pb);
};

class	CCommThread
{
public:

	CCommThread();
	~CCommThread();
	//--------- 환경 변수 -----------------------------------------//
private:
	HANDLE				m_hComm;				// 통신 포트 파일 핸들
	CString				m_sPortName;			// 포트 이름 (COM1 ..)
	BOOL				m_bConnected;			// 포트가 열렸는지 유무를 나타냄.
	HANDLE				m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	WORD				m_wPortID;				// WM_COMM_READ와 함께 보내는 인수.
	CQueue				m_QueueRead;			//큐버퍼
	//CEvent *			m_RevThreadEvent;		//ATCommand Class의 CDMARevThread 스레드 SetEvent()시켠줌
	CCriticalSection	m_critic;				//Queue PutByte(), GetByte() 크리티컬섹션 
	
// Operations
public:
	BOOL		OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );//포트 열기 
	void		ClosePort();							//포트 닫기
	DWORD		WriteComm(BYTE *pBuff, DWORD nToWrite);	//포트에 데이터 쓰기
	DWORD		ReadComm(BYTE *pBuff, DWORD nToRead);	//포트에서 데이터 읽어오기
	BOOL		GetByte (BYTE* pData);					//버퍼에서 데이터 읽기
	BOOL		PutByte (BYTE btData);					//버퍼에 데이터 쓰기
	int			GetSize ();								//버퍼에 쓰여진 크기 
	HANDLE		GetHandle ();							//CCommThread Handle 값을 넘겨줌
	//CEvent *	GetEvent();								//m_RevThreadEvent 값을 넘겨줌
	CString		GetPortName();							//포트값을 넘겨줌
	BOOL		IsConnected();							//ThreadWatchComm 생성
	HANDLE		GetThreadHandle();						//ThreadWatchComm handle값을 넘겨줌
	void		SetThreadHandle(HANDLE handle);			//ThreadWatchComm Handle값 setting
	void		SetCallerWnd(CWnd * pCallerWnd);
	CWnd *		m_pCallerWnd;
};

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);

