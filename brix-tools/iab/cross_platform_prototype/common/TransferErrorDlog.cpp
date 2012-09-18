// C:\ess2_source\main_branch3\tools\common\TransferErrorDlog.cpp : implementation file
//

#include "stdafx.h"
#include "../common/TransferErrorDlog.h"
#include "../common/TransferErrorDlogRes.h"

// CTransferErrorDlog dialog

IMPLEMENT_DYNAMIC(CTransferErrorDlog, CResizingDialog)

CTransferErrorDlog::CTransferErrorDlog(CWnd* pParent /*=NULL*/)
			: CResizingDialog (CTransferErrorDlog::IDD, pParent)
{

}

CTransferErrorDlog::~CTransferErrorDlog()
{
}

void CTransferErrorDlog::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctlListBox);
}


BEGIN_MESSAGE_MAP(CTransferErrorDlog, CResizingDialog)
	ON_MESSAGE( UWM_ADD_ERRORS, OnAddErrors )
	ON_MESSAGE( UWM_CLEAR_ERRORS, OnClearErrors )
	ON_BN_CLICKED(IDOK1, &CTransferErrorDlog::OnBnClickedOk)
END_MESSAGE_MAP()


// CTransferErrorDlog message handlers

BOOL CTransferErrorDlog::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	// sizing controls
    AddControl(IDC_LIST1,sizeResize,sizeResize);
    AddControl(IDOK1,sizeRelative,sizeRepos);

	std::list<INXString>::iterator it = m_cErrorLineList.begin();

	while(it != m_cErrorLineList.end() ){
		m_ctlListBox.AddString( *it);
		it++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// Handler called when dialog is resized. 
// SDG: For some reason it is not called.
void CTransferErrorDlog::OnSize(UINT nType, int cx, int cy)
{
	CResizingDialog::OnSize(nType, cx, cy);
    // Resize the list box to fit in the entire client area.
    m_ctlListBox.SetWindowPos(NULL, 
                           0, 0,
                           cx, cy-2,
                           SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

}

LRESULT CTransferErrorDlog::OnAddErrors(WPARAM, LPARAM lParam)
{
	std::list<INXString> *cs = (std::list<INXString> *)lParam;
	std::list<INXString>::iterator it = (*cs).begin();

	while( it!= (*cs).end() ){

		m_ctlListBox.AddString( *it );
		it++;
	}

	UpdateData(FALSE);
	RedrawWindow();
	return 0;
}


void CTransferErrorDlog::addErrors(const std::list<INXString> &errorLineList )
{
	std::list<INXString> *cs = new std::list<INXString>;
	*cs = errorLineList;
	PostMessage( UWM_ADD_ERRORS, 0, (LPARAM)cs );
}


LRESULT CTransferErrorDlog::OnClearErrors(WPARAM, LPARAM lParam)
{
	m_ctlListBox.ResetContent();
	UpdateData(FALSE);
	RedrawWindow();
	return 0;
}


void CTransferErrorDlog::clearErrors( )
{
	LPARAM dummy = NULL;
	PostMessage( UWM_CLEAR_ERRORS, 0, dummy );
}

void CTransferErrorDlog::OnBnClickedOk()
{
	CResizingDialog::OnOK();
}
