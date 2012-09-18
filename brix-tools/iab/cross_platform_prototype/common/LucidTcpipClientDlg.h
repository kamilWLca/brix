

#pragma once
#include "LucidTcpipClientDlgRes.h"

// CLucidTcpipClientDlg dialog

class CLucidTcpipClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CLucidTcpipClientDlg)
	int m_iPort;
	CString m_csIpAddress;

public:
	//CLucidTcpipClientDlg(CWnd* pParent = NULL);   // standard constructor
	CLucidTcpipClientDlg();   // standard constructor
	virtual ~CLucidTcpipClientDlg();
	void SetPort(int iPort);
	void SetIpAddress(CString csIpAddress);
	int GetPort();
	CString GetIpAddress();

// Dialog Data
	//enum { IDD = IDD_LUCIDTCPIPCLIENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedRadioLocal();
//	afx_msg void OnBnClickedRadioRemote();
protected:
//	int m_iRadioIpAddress;
	CIPAddressCtrl m_ctlIpAddress;
	CEdit m_ctlRemotePort;
	CEdit m_ctlLocalPort;
	CEdit m_ctlAppServerURL;
	CEdit m_ctlAppServerUserName;
	CEdit m_ctlAppServerPassword;
};



