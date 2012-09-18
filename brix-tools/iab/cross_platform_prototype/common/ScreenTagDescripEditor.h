#pragma once
#include "afxwin.h"
#include "../common/LucidTypedefs.h"
//#include "Resource.h"


// CScreenTagDescripEditor dialog

class CScreenTagDescripEditor : public CDialog
{
	DECLARE_DYNAMIC(CScreenTagDescripEditor)

public:
CScreenTagDescripEditor::CScreenTagDescripEditor(
	TagAndDescription_t &tagAndDescrip, CWnd* pParent =NULL );

	virtual ~CScreenTagDescripEditor();

// Dialog Data
	//enum { IDD = IDD_EDIT_SCRNTAG_DESCRIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	CEdit m_ctlTagDescrip;
	CEdit m_ctlTagEdit;
	TagAndDescription_t m_tTagAndDescrip;

public:
	CString getTagDescription(void);
};
