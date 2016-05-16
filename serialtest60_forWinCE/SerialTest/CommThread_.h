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
	//--------- ȯ�� ���� -----------------------------------------//
private:
	HANDLE				m_hComm;				// ��� ��Ʈ ���� �ڵ�
	CString				m_sPortName;			// ��Ʈ �̸� (COM1 ..)
	BOOL				m_bConnected;			// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	HANDLE				m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	WORD				m_wPortID;				// WM_COMM_READ�� �Բ� ������ �μ�.
	CQueue				m_QueueRead;			//ť����
	//CEvent *			m_RevThreadEvent;		//ATCommand Class�� CDMARevThread ������ SetEvent()������
	CCriticalSection	m_critic;				//Queue PutByte(), GetByte() ũ��Ƽ�ü��� 
	
// Operations
public:
	BOOL		OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );//��Ʈ ���� 
	void		ClosePort();							//��Ʈ �ݱ�
	DWORD		WriteComm(BYTE *pBuff, DWORD nToWrite);	//��Ʈ�� ������ ����
	DWORD		ReadComm(BYTE *pBuff, DWORD nToRead);	//��Ʈ���� ������ �о����
	BOOL		GetByte (BYTE* pData);					//���ۿ��� ������ �б�
	BOOL		PutByte (BYTE btData);					//���ۿ� ������ ����
	int			GetSize ();								//���ۿ� ������ ũ�� 
	HANDLE		GetHandle ();							//CCommThread Handle ���� �Ѱ���
	//CEvent *	GetEvent();								//m_RevThreadEvent ���� �Ѱ���
	CString		GetPortName();							//��Ʈ���� �Ѱ���
	BOOL		IsConnected();							//ThreadWatchComm ����
	HANDLE		GetThreadHandle();						//ThreadWatchComm handle���� �Ѱ���
	void		SetThreadHandle(HANDLE handle);			//ThreadWatchComm Handle�� setting
	void		SetCallerWnd(CWnd * pCallerWnd);
	CWnd *		m_pCallerWnd;
};

// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CCommThread* pComm);

