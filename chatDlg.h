
// chatDlg.h : ͷ�ļ�
//
#define WM_RECVDATA WM_USER+1
#pragma once

//���崫�ݲ����ṹ��05
struct Recvparam
{
	SOCKET sock;
	HWND  hd;
};
// CchatDlg �Ի���
class CchatDlg : public CDialogEx
{
// ����
public:
	CchatDlg(CWnd* pParent = NULL);	// ��׼���캯��
	bool InitSocket();//02
	static DWORD WINAPI RecvProc(LPVOID p);
// �Ի�������
	enum { IDD = IDD_CHAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
//��Ա����01
private:
	SOCKET m_socket;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnRecvdata(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonSend();
};
