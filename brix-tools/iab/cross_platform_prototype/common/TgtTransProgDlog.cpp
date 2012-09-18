// TransferAppDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TgtTransProgDlog.h"
#include "../common/LabLgbBaseApp.h"
#include "../common/LucidConstants.h"
#include "../LucidApplicationBuilder/Project.h"
#include <cassert>

IMPLEMENT_DYNAMIC(CTgtTransProgDlog, CDialog)

CTgtTransProgDlog::CTgtTransProgDlog() : 
CDialog ( _T("IDD_TARGET_TRANSFER_PROGESS") )
, m_bCancelled(false)
{
}

CTgtTransProgDlog::~CTgtTransProgDlog()
{
}

void CTgtTransProgDlog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CANCEL1003, m_ctlBtnCancel);
	DDX_Control(pDX, IDC_PROGBAR_FILE, m_ctlProgbarFile);
	DDX_Control(pDX, IDC_PROGBAR_OVERALL, m_ctlProgbarOverall);
	DDX_Control(pDX, IDC_STATIC_FILE_PROGRESS, m_ctlStaticFilePrompt);
	DDX_Control(pDX, IDC_STATIC_OVERALL_PROGRESS, m_ctlStaticOverallPrompt);
}

BEGIN_MESSAGE_MAP(CTgtTransProgDlog, CDialog)

	ON_BN_CLICKED(IDC_BUTTON_CANCEL1003, &CTgtTransProgDlog::OnBnClickedButtonCancel)

	ON_MESSAGE( UWM_SET_FILE_PROGBAR, OnSetProgbarFile )
	ON_MESSAGE( UWM_SET_OVERALL_PROGBAR, OnSetProgbarOverall )
	ON_MESSAGE( UWM_SET_FILE_PROMPT, OnSetPromptFile )
	ON_MESSAGE( UWM_SET_OVERALL_PROMPT, OnSetPromptOverall )
	ON_MESSAGE( UWM_SET_RESET, OnSetReset )
	ON_MESSAGE( UWM_SET_CANCEL_BTN, OnSetCancelBtn )

END_MESSAGE_MAP()

LRESULT CTgtTransProgDlog::OnSetProgbarOverall(WPARAM, LPARAM lParam)
{
	int *i = (int *)lParam;
	m_ctlProgbarOverall.SetPos( *i );

	UpdateData(FALSE);
	RedrawWindow();
	delete i;
	return 0;
}

LRESULT CTgtTransProgDlog::OnSetPromptOverall(WPARAM, LPARAM lParam)
{
	INXString *cs = (INXString *)lParam;
	m_ctlStaticOverallPrompt.SetWindowTextA( *cs );

	UpdateData(FALSE);
	RedrawWindow();
	delete cs;
	return 0;
}

LRESULT CTgtTransProgDlog::OnSetProgbarFile(WPARAM, LPARAM lParam)
{
	int *i = (int *)lParam;
	m_ctlProgbarFile.SetPos( *i );

	UpdateData(FALSE);
	RedrawWindow();
	delete i;
	return 0;
}

LRESULT CTgtTransProgDlog::OnSetPromptFile(WPARAM, LPARAM lParam)
{
	INXString *cs = (INXString *)lParam;
	m_ctlStaticFilePrompt.SetWindowTextA( *cs );

	UpdateData(FALSE);
	RedrawWindow();
	delete cs;
	return 0;
}

LRESULT CTgtTransProgDlog::OnSetReset(WPARAM, LPARAM lParam)
{
	m_ctlStaticFilePrompt.SetWindowTextA( "" );
	m_ctlStaticOverallPrompt.SetWindowTextA( "" );
	m_ctlProgbarFile.SetPos(0);
	m_ctlProgbarOverall.SetPos(0);
	m_ctlBtnCancel.EnableWindow( true );

	m_bCancelled = false;

	UpdateData(FALSE);
	RedrawWindow();

	return 0;
}

LRESULT CTgtTransProgDlog::OnSetCancelBtn(WPARAM, LPARAM lParam)
{
	bool *pVal = (bool *)lParam;
	m_bCancelled = *pVal;

	UpdateData(FALSE);
	RedrawWindow();
	delete pVal;
	return 0;
}


void CTgtTransProgDlog::OnBnClickedButtonCancel()
{
	m_bCancelled = true;
	m_ctlBtnCancel.EnableWindow( m_bCancelled );
	UpdateData(FALSE);
}

BOOL CTgtTransProgDlog::OnInitDialog()
{
	CDialog::OnInitDialog();
	setReset();
	return TRUE;  // return TRUE unless you set the focus to a control

}

void CTgtTransProgDlog::setProgbarOverall(const int & pos)
{
	int *i = new int(pos);
	PostMessage( UWM_SET_OVERALL_PROGBAR, 0, (LPARAM)i );
}

void CTgtTransProgDlog::setProgbarFile(const int & pos)
{
	int *i = new int(pos);
	PostMessage( UWM_SET_FILE_PROGBAR, 0, (LPARAM)i );
}

void CTgtTransProgDlog::setPromptOverallDirect(const INXString & textVal)
{
	m_ctlStaticOverallPrompt.SetWindowText( textVal );
	UpdateData(false);
	RedrawWindow();
}

void CTgtTransProgDlog::setPromptOverall(const INXString & fileName)
{
	INXString *cs = new INXString(fileName);
	PostMessage( UWM_SET_OVERALL_PROMPT, 0, (LPARAM)cs );
}

void CTgtTransProgDlog::setPromptFile(const INXString & fileName)
{
	INXString *cs = new INXString(fileName);
	PostMessage( UWM_SET_FILE_PROMPT, 0, (LPARAM)cs );
}

void CTgtTransProgDlog::setReset()
{
	PostMessage( UWM_SET_RESET );
}

void CTgtTransProgDlog::setCancelBtn( const bool &bEnabled )
{
	bool *v = new bool(bEnabled);
	PostMessage( UWM_SET_CANCEL_BTN, 0, (LPARAM)v );
}
