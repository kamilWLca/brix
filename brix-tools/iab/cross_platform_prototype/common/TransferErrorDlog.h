#pragma once
#include "afxwin.h"
#include <list>
#include "HScrollListBox.h"
#include "ResizingDialog.h"
#include "../common/TransferErrorDlogRes.h"
#include "Porting_Classes/INXString.h"

#define UWM_ADD_ERRORS (WM_APP + 1)
#define UWM_CLEAR_ERRORS (WM_APP + 2)

// CTransferErrorDlog dialog

class CTransferErrorDlog : public CResizingDialog
{
	DECLARE_DYNAMIC(CTransferErrorDlog)

public:

	afx_msg LRESULT OnClearErrors(WPARAM, LPARAM);
	afx_msg LRESULT OnAddErrors(WPARAM, LPARAM);

	CTransferErrorDlog(	//std::list<INXString> &errorLineList,
						CWnd* pParent = NULL);   // standard constructor

	virtual ~CTransferErrorDlog();

// Dialog Data
	enum { IDD = IDD_TRANSFER_ERROR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	CHScrollListBox m_ctlListBox;
	virtual BOOL OnInitDialog();
	void addErrors( const std::list<INXString> &rINXStrings );
	void clearErrors( );

private:
	std::list<INXString> m_cErrorLineList;

	afx_msg void OnBnClickedOk();
};
