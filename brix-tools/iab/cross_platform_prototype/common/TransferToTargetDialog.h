#pragma once
#include "TransferToTargetDialogRes.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ProjectMetaData.h"

#define UWM_SET_PROGRESSBAR (WM_APP + 1)
#define UWM_ADD_ERRORLINE (WM_APP + 2)
#define UWM_CLR_ERRORLINE (WM_APP + 3)
#define UWM_SET_FILENAME (WM_APP + 4)
#define UWM_SET_TRNSFRBTN (WM_APP + 5)
#define UWM_UPDATE_DISPLAY (WM_APP + 6)

class Project;
// CTransferToTargetDialog dialog

class CTransferToTargetDialog : public CDialog
{
	DECLARE_DYNAMIC(CTransferToTargetDialog)

public:
	//CTransferToTargetDialog(CWnd* pParent = NULL);   // standard constructor
	CTransferToTargetDialog();  
	virtual ~CTransferToTargetDialog();

	static UINT transferThread( LPVOID ptr );

	void transferThread( );

	afx_msg LRESULT OnSetProgressBar(WPARAM, LPARAM);
	afx_msg LRESULT OnAddErrorLine(WPARAM, LPARAM);
	afx_msg LRESULT OnClrErrorLines(WPARAM, LPARAM);
	afx_msg LRESULT OnSetFileName(WPARAM, LPARAM);
	afx_msg LRESULT OnSetTransferBtn(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateDisplay(WPARAM, LPARAM);
	

	bool  getSendChangedOnly(void) const;

// Dialog Data
	//enum { IDD = IDD_LAB_TRANSFER_TARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CButton m_ctlBtnTransfer;
	CButton m_ctlBtnDismiss;
	CButton m_ctlCheckDismiss;

	CProgressCtrl m_ctl_ProgressBar;

	CButton m_ctlRadioChanged;
	CButton m_ctlRadioAll;

	int m_iRadioAll;
	int m_iRadioChanged;

	//CButton m_ctl_CheckboxSodl;
	//CButton m_ctl_CheckboxData;
	//CButton m_ctl_CheckboxBitmap;
	//CButton m_ctl_CheckboxExports;
	CListBox m_ctlListEhsError;

	BOOL m_bAutoDismissVal;

	ProjectMetaData *m_pProjMetaData;
	Project *m_pProject;

	afx_msg void OnBnClickedTransfer();
	afx_msg void OnBnClickedDismiss();
	virtual BOOL OnInitDialog();

	//void appendError(const CString & csErr);
	void updateAndDisplay(void);
	void setTransferFileName(const CString & fileName);
	void setProgressBar(const int & pos);
	void setProjectMetaData( ProjectMetaData *pMD );
	void setProject( Project *pMD );
	void setTransferBtn(const bool & val );

	void addEhsEchoLine(const CString & line);
	void clearEhsEchoes();


public:
	CEdit m_ctl_TransferFile;
public:
	//afx_msg void OnBnClickedLabCheckboxExported();

public:
	afx_msg void OnBnClickedRadioXferAll();
	afx_msg void OnBnClickedRadioXferChanged();
	bool m_bCurrentlyTransferring;
};
