// LucidTcpipClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LucidTcpipClientDlg.h"
#include "LucidConstants.h"
#include "LucidRegAccess.h"
#include "GlobalFuncs_2.h"

// CLucidTcpipClientDlg dialog

IMPLEMENT_DYNAMIC(CLucidTcpipClientDlg, CDialog)


//CLucidTcpipClientDlg::CLucidTcpipClientDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CLucidTcpipClientDlg::IDD, pParent)
CLucidTcpipClientDlg::CLucidTcpipClientDlg() : CDialog(_T("IDD_LUCIDTCPIPCLIENT_DLG"))
{
//
}

CLucidTcpipClientDlg::~CLucidTcpipClientDlg()
{
}

void CLucidTcpipClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

//	DDX_Radio(pDX, IDC_RADIO_LOCAL, m_iRadioIpAddress);
	DDX_Control(pDX, IDC_LTCLIENT_IPADDRESS, m_ctlIpAddress);
	DDX_Control(pDX, IDC_LTCLIENT_LOCAL_PORT, m_ctlLocalPort);
	DDX_Control(pDX, IDC_LTCLIENT_REMOTE_PORT, m_ctlRemotePort);
	DDX_Control(pDX, IDC_LTCLIENT_URL, m_ctlAppServerURL);
	DDX_Control(pDX, IDC_LTCLIENT_USERNAME, m_ctlAppServerUserName);
	DDX_Control(pDX, IDC_LTCLIENT_PASSWORD, m_ctlAppServerPassword);
}


BEGIN_MESSAGE_MAP(CLucidTcpipClientDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLucidTcpipClientDlg::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_RADIO_LOCAL, &CLucidTcpipClientDlg::OnBnClickedRadioLocal)
//	ON_BN_CLICKED(IDC_RADIO_REMOTE, &CLucidTcpipClientDlg::OnBnClickedRadioRemote)
END_MESSAGE_MAP()


// CLucidTcpipClientDlg message handlers

void CLucidTcpipClientDlg::OnBnClickedOk()
{
	CString csIpAddress, csLocalPort, csRemotePort, csURL, csUserName, csPassword;

	// TODO: Add your control notification handler code here
	this->GetDlgItemTextA(IDC_LTCLIENT_IPADDRESS, csIpAddress);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),csIpAddress);

	this->GetDlgItemTextA(IDC_LTCLIENT_LOCAL_PORT, csLocalPort);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCAL_PORT_VALUE),csLocalPort);
	this->GetDlgItemTextA(IDC_LTCLIENT_REMOTE_PORT, csRemotePort);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_PORT_VALUE),csRemotePort);

	this->GetDlgItemTextA(IDC_LTCLIENT_URL, csURL);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_URL_VALUE),csURL);
	this->GetDlgItemTextA(IDC_LTCLIENT_USERNAME, csUserName);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_USERNAME_VALUE),csUserName);

	//@todo - encrypt app server password
	this->GetDlgItemTextA(IDC_LTCLIENT_PASSWORD, csPassword);
	setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_PASSWORD_VALUE),csPassword);


/* this logic is has been moved to 'switch to managing local/remote EHS' buttons on toolbar
	if (!m_iRadioIpAddress) {
		csIpAddress = DEFAULT_TARGET_LOCAL_IP;
		setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION),_T(REG_TARGET_LOCATION_DATA_LOCAL));
		this->SetPort(this->GetDlgItemInt(IDC_LTCLIENT_LOCAL_PORT));
	}
	else {
		setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION),_T(REG_TARGET_LOCATION_DATA_REMOTE));
		this->SetPort(this->GetDlgItemInt(IDC_LTCLIENT_REMOTE_PORT));
	}

	this->SetIpAddress(csIpAddress);
*/
	OnOK();
}

void CLucidTcpipClientDlg::SetPort(int iPort)
{
	m_iPort = iPort;
}

void CLucidTcpipClientDlg::SetIpAddress(CString csIpAddress)
{
	m_csIpAddress = csIpAddress;
}

int CLucidTcpipClientDlg::GetPort()
{
	return m_iPort;
}

CString CLucidTcpipClientDlg::GetIpAddress()
{
	return m_csIpAddress;
}

BOOL CLucidTcpipClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString csIpAddress = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),_T((CString)DEFAULT_TARGET_REMOTE_IP));
//	CString csLocation = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION),_T(DEFAULT_TARGET_LOCATION));
	CString csLocalPort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCAL_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));
	CString csRemotePort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));
	CString csURL = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_URL_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_URL));
	CString csUserName = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_USERNAME_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_USERNAME));
	CString csPassword = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_PASSWORD_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_PASSWORD));

/*
	if (csLocation == REG_TARGET_LOCATION_DATA_REMOTE) {
		m_csIpAddress = csIpAddress;
		m_iRadioIpAddress = 1;
		m_ctlIpAddress.EnableWindow(true);
		m_ctlRemotePort.EnableWindow(true);
		m_ctlLocalPort.EnableWindow(false);
		m_iPort = atoi(csRemotePort);
	}
	else {
		m_csIpAddress = DEFAULT_TARGET_LOCAL_IP;
		m_iRadioIpAddress = 0;
		m_ctlIpAddress.EnableWindow(false);
		m_ctlRemotePort.EnableWindow(false);
		m_ctlLocalPort.EnableWindow(true);
		m_iPort = atoi(csLocalPort);
	}
*/
	this->SetDlgItemTextA(IDC_LTCLIENT_LOCAL_PORT,csLocalPort);
	this->SetDlgItemTextA(IDC_LTCLIENT_REMOTE_PORT,csRemotePort);
	this->SetDlgItemTextA(IDC_LTCLIENT_IPADDRESS,csIpAddress);
	this->SetDlgItemTextA(IDC_LTCLIENT_URL,csURL);
	this->SetDlgItemTextA(IDC_LTCLIENT_USERNAME,csUserName);
	this->SetDlgItemTextA(IDC_LTCLIENT_PASSWORD,csPassword);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

/*
void CLucidTcpipClientDlg::OnBnClickedRadioLocal()
{
	m_iRadioIpAddress = 0;
	m_ctlIpAddress.EnableWindow(false);
	m_ctlRemotePort.EnableWindow(false);
	m_ctlLocalPort.EnableWindow(true);
}

void CLucidTcpipClientDlg::OnBnClickedRadioRemote()
{
	m_iRadioIpAddress = 1;
	m_ctlIpAddress.EnableWindow(true);
	m_ctlRemotePort.EnableWindow(true);
	m_ctlLocalPort.EnableWindow(false);
}
*/