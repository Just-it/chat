
// chatDlg.h : 头文件
//
#define WM_RECVDATA WM_USER+1
#pragma once

//定义传递参数结构体05
struct Recvparam
{
	SOCKET sock;
	HWND  hd;
};
// CchatDlg 对话框
class CchatDlg : public CDialogEx
{
// 构造
public:
	CchatDlg(CWnd* pParent = NULL);	// 标准构造函数
	bool InitSocket();//02
	static DWORD WINAPI RecvProc(LPVOID p);
// 对话框数据
	enum { IDD = IDD_CHAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
//成员变量01
private:
	SOCKET m_socket;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnRecvdata(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonSend();
};
