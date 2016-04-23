
// chatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "chat.h"
#include "chatDlg.h"
#include "afxdialogex.h"

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


// CchatDlg �Ի���



CchatDlg::CchatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CchatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CchatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CchatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVDATA, &CchatDlg::OnRecvdata)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CchatDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

//��������ʼ���׽���03
bool CchatDlg::InitSocket()
{
	WSADATA wsdata;
	int err;
	//�����׽��ֿ�
	err = WSAStartup(MAKEWORD(2,2),&wsdata);
	if (0!=err)
	{
		MessageBox(_T("WSAStartup() error!"));
		return false;
	}
	//�����׽���
	m_socket = socket(AF_INET,SOCK_DGRAM,0);
	if (INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("�����׽���ʧ�ܣ�"));
		WSACleanup();
		return false;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//���׽�ֹ
	err = bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		closesocket(m_socket);
		MessageBox(_T("��ʧ�ܣ�"));
		return false;
	}
	return true;
}
//��������
DWORD WINAPI CchatDlg::RecvProc(LPVOID p)
{
	SOCKET sock = ((struct Recvparam*)p)->sock;
	HWND hwnd = ((struct Recvparam*)p)->hd;
	delete p;
	SOCKADDR_IN addrfrom;
	int len = sizeof(SOCKADDR);
	char recvBuf[200];
	char sendBuf[200];
	int retval;
	while (true)
	{
		memset(recvBuf,'\0',sizeof(recvBuf));
		//��������
		retval = recvfrom(sock,recvBuf,200,0,(SOCKADDR*)&addrfrom,&len);
		if (retval == SOCKET_ERROR)
			break;
		sprintf_s(sendBuf,"%s ˵: %s",inet_ntoa(addrfrom.sin_addr),recvBuf);
		::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)sendBuf);
	}
	return 0;
}
// CchatDlg ��Ϣ�������

BOOL CchatDlg::OnInitDialog()
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
	InitSocket();
	struct Recvparam* pRecvparam = new struct Recvparam;
	pRecvparam->sock = m_socket;
	pRecvparam->hd = m_hWnd;
	HANDLE hthread;
	hthread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvparam,0,NULL);
	CloseHandle(hthread);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CchatDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CchatDlg::OnPaint()
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
HCURSOR CchatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CchatDlg::OnRecvdata(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	CString str = A2W((char*)lParam);
	CString strTemp;
	GetDlgItemText(IDC_EDIT_RECV,strTemp);
	str += "\r\n";
	str += strTemp;
	SetDlgItemText(IDC_EDIT_RECV,str);
	return 0;
}


void CchatDlg::OnBnClickedButtonSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_SEND))->GetAddress(dwIP);

	SOCKADDR_IN addrto;
	addrto.sin_addr.S_un.S_addr = htonl(dwIP);
	addrto.sin_family = AF_INET;
	addrto.sin_port = htons(6000);

	CString strSend;
	GetDlgItemText(IDC_EDIT_SEND,strSend);
	USES_CONVERSION;
	char *sendbuff = W2A(strSend);
	int len = strlen(sendbuff);
	sendto(m_socket,sendbuff,len,0,(SOCKADDR*)&addrto,sizeof(SOCKADDR));
	SetDlgItemText(IDC_EDIT_SEND,_T(""));
}
