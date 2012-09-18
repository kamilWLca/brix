// ScreenTagDescripEditor.cpp : implementation file
//

#include "stdafx.h"
#include "../common/LucidTypedefs.h"
#include "ScreenTagDescripEditor.h"
#include "ScreenTagDescripEditorRes.h"


// CScreenTagDescripEditor dialog

IMPLEMENT_DYNAMIC(CScreenTagDescripEditor, CDialog)

CScreenTagDescripEditor::CScreenTagDescripEditor(
	TagAndDescription_t &tagAndDescrip, CWnd* pParent /*=NULL*/)
	: CDialog(_T("IDD_EDIT_SCRNTAG_DESCRIP") , pParent), m_tTagAndDescrip(tagAndDescrip)
	
{
	//m_ctlTagEdit.SetWindowText( tagAndDescrip.tagDescr);
	//m_ctlTagDescrip.SetWindowText( tagAndDescrip.tagDescr);
}

CScreenTagDescripEditor::~CScreenTagDescripEditor()
{
}

void CScreenTagDescripEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DESCRIP, m_ctlTagDescrip);
	DDX_Control(pDX, IDC_TAG_READONLY, m_ctlTagEdit);
}


BEGIN_MESSAGE_MAP(CScreenTagDescripEditor, CDialog)
END_MESSAGE_MAP()


// CScreenTagDescripEditor message handlers

BOOL CScreenTagDescripEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlTagEdit.SetWindowText( m_tTagAndDescrip.tag);
	m_ctlTagDescrip.SetWindowText( m_tTagAndDescrip.tagDescr);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScreenTagDescripEditor::OnOK()
{
	UpdateData();

	m_ctlTagEdit.GetWindowText( m_tTagAndDescrip.tag );
	m_ctlTagDescrip.GetWindowText( m_tTagAndDescrip.tagDescr );

	CDialog::OnOK();
}

CString CScreenTagDescripEditor::getTagDescription(void)
{
	return m_tTagAndDescrip.tagDescr;
}
