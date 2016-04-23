
// chatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "chat.h"
#include "chatDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CchatDlg 对话框



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

//创建并初始化套接字03
bool CchatDlg::InitSocket()
{
	WSADATA wsdata;
	int err;
	//加载套接字库
	err = WSAStartup(MAKEWORD(2,2),&wsdata);
	if (0!=err)
	{
		MessageBox(_T("WSAStartup() error!"));
		return false;
	}
	//创建套接字
	m_socket = socket(AF_INET,SOCK_DGRAM,0);
	if (INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("创建套接字失败！"));
		WSACleanup();
		return false;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//绑定套截止
	err = bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		closesocket(m_socket);
		MessageBox(_T("绑定失败！"));
		return false;
	}
	return true;
}
//接受数据
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
		//接受数据
		retval = recvfrom(sock,recvBuf,200,0,(SOCKADDR*)&addrfrom,&len);
		if (retval == SOCKET_ERROR)
			break;
		sprintf_s(sendBuf,"%s 说: %s",inet_ntoa(addrfrom.sin_addr),recvBuf);
		::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)sendBuf);
	}
	return 0;
}
// CchatDlg 消息处理程序

BOOL CchatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitSocket();
	struct Recvparam* pRecvparam = new struct Recvparam;
	pRecvparam->sock = m_socket;
	pRecvparam->hd = m_hWnd;
	HANDLE hthread;
	hthread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvparam,0,NULL);
	CloseHandle(hthread);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CchatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	// TODO: 在此添加控件通知处理程序代码
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
