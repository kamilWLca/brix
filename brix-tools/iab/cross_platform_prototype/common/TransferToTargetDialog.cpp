// TransferAppDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TransferToTargetDialog.h"
#include "../common/LabLgbBaseApp.h"
#include "../common/LucidConstants.h"
#include "../LucidApplicationBuilder/Project.h"
#include <cassert>

IMPLEMENT_DYNAMIC(CTransferToTargetDialog, CDialog)

CTransferToTargetDialog::CTransferToTargetDialog() : CDialog ( _T("IDD_LAB_TRANSFER_TARGET") )
,m_iRadioAll(1)
,m_pProject(NULL)
,m_pProjMetaData(NULL)
,m_bCurrentlyTransferring(false)
{
}

CTransferToTargetDialog::~CTransferToTargetDialog()
{
}

void CTransferToTargetDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,  ID_LAB_TRANSFER, m_ctlBtnTransfer );
	DDX_Control(pDX,  ID_LAB_DISMISS, m_ctlBtnDismiss );
	DDX_Control(pDX, IDC_LAB_PROGRESS2, m_ctl_ProgressBar);
	DDX_Control(pDX, IDC_EHS_ERROR_LIST, m_ctlListEhsError);
	DDX_Check(pDX, IDC_LAB_CHKBX_DISMISS_ON_COMP, m_bAutoDismissVal);
	DDX_Control(pDX, IDC_LAB_CHKBX_DISMISS_ON_COMP, m_ctlCheckDismiss);
	DDX_Control(pDX, IDC_LAB_TRANSFERRABLE_NAME_EDITRO, m_ctl_TransferFile);
	DDX_Control(pDX, IDC_RADIO_XFER_CHANGED, m_ctlRadioChanged );
	DDX_Control(pDX, IDC_RADIO_XFER_ALL, m_ctlRadioAll );

}


BEGIN_MESSAGE_MAP(CTransferToTargetDialog, CDialog)

	ON_BN_CLICKED(ID_LAB_TRANSFER, &CTransferToTargetDialog::OnBnClickedTransfer)
	ON_BN_CLICKED(ID_LAB_DISMISS, &CTransferToTargetDialog::OnBnClickedDismiss)
	ON_BN_CLICKED(IDC_RADIO_XFER_ALL, &CTransferToTargetDialog::OnBnClickedRadioXferAll)
	ON_BN_CLICKED(IDC_RADIO_XFER_CHANGED, &CTransferToTargetDialog::OnBnClickedRadioXferChanged)

	ON_MESSAGE( UWM_SET_PROGRESSBAR, OnSetProgressBar )
	ON_MESSAGE( UWM_ADD_ERRORLINE, OnAddErrorLine )
	ON_MESSAGE( UWM_CLR_ERRORLINE, OnClrErrorLines )
	ON_MESSAGE( UWM_SET_FILENAME, OnSetFileName )
	ON_MESSAGE( UWM_SET_TRNSFRBTN, OnSetTransferBtn )
	ON_MESSAGE( UWM_UPDATE_DISPLAY, OnUpdateDisplay )
	
END_MESSAGE_MAP()


// CTransferToTargetDialog message handlers

LRESULT CTransferToTargetDialog::OnSetProgressBar(WPARAM, LPARAM lParam)
{
	int *i = (int *)lParam;
	m_ctl_ProgressBar.SetPos( *i );
	m_ctlBtnTransfer.EnableWindow(false);

	UpdateData(FALSE);
	RedrawWindow();
	delete i;
	return 0;
}

LRESULT CTransferToTargetDialog::OnAddErrorLine(WPARAM, LPARAM lParam)
{
	CString *cs = (CString *)lParam;
	m_ctlListEhsError.AddString( *cs );

	CString csDum;
	for(int i=0;i<m_ctlListEhsError.GetCount();i++){
		m_ctlListEhsError.GetText( i, csDum );
	}

	UpdateData(FALSE);
	RedrawWindow();
	delete cs;
	return 0;
}

LRESULT CTransferToTargetDialog::OnClrErrorLines(WPARAM, LPARAM lParam)
{

	m_ctlListEhsError.ResetContent();
	m_ctlBtnTransfer.EnableWindow(false);

	UpdateData(FALSE);
	RedrawWindow();
	return 0;
}

LRESULT CTransferToTargetDialog::OnSetFileName(WPARAM, LPARAM lParam)
{
	CString *cs = (CString *)lParam;
	m_ctl_TransferFile.SetWindowText(*cs);
	m_ctlBtnTransfer.EnableWindow(false);

	UpdateData(FALSE);
	RedrawWindow();
	delete cs;
	return 0;
}

LRESULT CTransferToTargetDialog::OnUpdateDisplay(WPARAM, LPARAM lParam)
{

	UpdateData(FALSE);
	RedrawWindow();

	return 0;
}

LRESULT CTransferToTargetDialog::OnSetTransferBtn(WPARAM, LPARAM lParam)
{
	bool *val = (bool *)lParam;
	m_ctlBtnTransfer.EnableWindow(*val);

	UpdateData(FALSE);
	RedrawWindow();
	delete val;
	return 0;
}


BOOL CTransferToTargetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_bCurrentlyTransferring)
	{

		//m_ctlBtnTransfer.EnableWindow(false);
	}else{

		m_ctl_ProgressBar.SetRange(0, 100);
		m_ctl_ProgressBar.SetPos(0);

		m_ctlBtnDismiss.EnableWindow(true);
		m_ctlBtnTransfer.EnableWindow(true);

		m_iRadioAll = 1;
		m_iRadioChanged = 0;

		m_ctlListEhsError.ResetContent();

		m_ctlRadioChanged.SetCheck( !m_iRadioAll );
		m_ctlRadioAll.SetCheck( !m_iRadioChanged);
	}

	UpdateData(false);
	RedrawWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool  CTransferToTargetDialog::getSendChangedOnly(void) const
{
	return m_iRadioChanged;
}

CTransferToTargetDialog *gpDlog;
extern CString gcsProjDir;
int giNFilesToBeTransferred;
LucidErrEnum giTransferErr;

void CTransferToTargetDialog::transferThread(  )
{
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	//giTransferErr = pApp->transferExportsToTarget( gcsProjDir, gpDlog, giNFilesToBeTransferred );
}

UINT CTransferToTargetDialog::transferThread( LPVOID ptr )
{
     CTransferToTargetDialog *me = (CTransferToTargetDialog *)ptr;
     me->transferThread();
     return 0;
}

void CTransferToTargetDialog::OnBnClickedTransfer()
{	
	assert(m_pProjMetaData != NULL);

	if(!m_pProjMetaData->getLock() ){

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);

	} else {

	//	if( (m_pProject != NULL) && (m_pProjMetaData != NULL)  ){	
	//		m_pProject->
	//	} // if( (m_pProject != NULL) && (m_pProjMetaData != NULL)  )

		UpdateData();

		m_bCurrentlyTransferring = true;
		m_pProjMetaData->readProjectFile();

		m_ctlBtnTransfer.EnableWindow(false);

		m_ctlListEhsError.ResetContent();
		UpdateData(false);
		RedrawWindow();

		CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();

	//	int nFilesToBeTransferred =
	//		pApp->copyTransferrablesToExports( m_pProjMetaData, getSendChangedOnly() );

		CString projDir;
		LucidErrEnum err = m_pProjMetaData->getProjectDir(projDir);

		gcsProjDir = projDir;
		//giNFilesToBeTransferred = nFilesToBeTransferred;
		gpDlog = this;
		AfxBeginThread( transferThread, this );

		//pApp->transferExportsToTarget( projDir, this, nFilesToBeTransferred );

		// Write-out the latest transfer time.
		if( giTransferErr == kErr_NoErr ){

			m_pProjMetaData->updateTransferTime();
			m_pProjMetaData->writeProjectFile();
		}

		m_bCurrentlyTransferring = false;
		m_ctlBtnTransfer.EnableWindow(true);

		m_pProjMetaData->releaseLock();

		if ( m_ctlCheckDismiss.GetState() ) {
			OnOK();
		}

	}
}

void CTransferToTargetDialog::OnBnClickedDismiss()
{
	UpdateData();
	CDialog::OnCancel();
}

void CTransferToTargetDialog::setProject( Project *pProj )
{
	m_pProject = pProj;
}

void CTransferToTargetDialog::setProjectMetaData( ProjectMetaData *pMD )
{
	m_pProjMetaData = pMD;
}


void CTransferToTargetDialog::updateAndDisplay()
{
	PostMessage( UWM_UPDATE_DISPLAY, 0, (LPARAM)NULL );
}

void CTransferToTargetDialog::setTransferFileName(const CString & fileName)
{
	CString *cs = new CString(fileName);
	PostMessage( UWM_SET_FILENAME, 0, (LPARAM)cs );
}

void CTransferToTargetDialog::setProgressBar(const int & pos)
{
	int *i = new int(pos);
	PostMessage( UWM_SET_PROGRESSBAR, 0, (LPARAM)i );

}

void CTransferToTargetDialog::setTransferBtn(const bool &val )
{
	bool *v = new bool(val);
	PostMessage( UWM_SET_TRNSFRBTN, 0, (LPARAM)v );

}


void CTransferToTargetDialog::addEhsEchoLine(const CString & line)
{
	CString *cs = new CString(line);
	PostMessage( UWM_ADD_ERRORLINE, 0, (LPARAM)cs );
}

void CTransferToTargetDialog::clearEhsEchoes()
{
	CString *cs = NULL;
	PostMessage( UWM_CLR_ERRORLINE, 0, (LPARAM)cs );
}


void CTransferToTargetDialog::OnBnClickedRadioXferAll()
{
	m_iRadioAll = 1;
	m_iRadioChanged = 0;
	UpdateData(false);
}

void CTransferToTargetDialog::OnBnClickedRadioXferChanged()
{
	m_iRadioAll = 0;
	m_iRadioChanged = 1;
	UpdateData(false);
}
