#pragma once
#include "TgtTransProgDlogRes.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ProjectMetaData.h"
#include "Porting_Classes/INXString.h"

#define UWM_SET_FILE_PROGBAR (WM_APP + 1)
#define UWM_SET_OVERALL_PROGBAR (WM_APP + 2)
#define UWM_SET_FILE_PROMPT (WM_APP + 3)
#define UWM_SET_OVERALL_PROMPT (WM_APP + 4)
#define UWM_SET_RESET (WM_APP + 5)
#define UWM_SET_CANCEL_BTN (WM_APP + 6)



class CTgtTransProgDlog : public CDialog
{
	DECLARE_DYNAMIC(CTgtTransProgDlog)

public:
	afx_msg LRESULT OnSetProgbarOverall(WPARAM, LPARAM);
	afx_msg LRESULT OnSetProgbarFile(WPARAM, LPARAM);
	afx_msg LRESULT OnSetPromptOverall(WPARAM, LPARAM);
	afx_msg LRESULT OnSetPromptFile(WPARAM, LPARAM);
	afx_msg LRESULT OnSetReset(WPARAM, LPARAM);
	afx_msg LRESULT OnSetCancelBtn(WPARAM, LPARAM);

public:
	//CTgtTransProgDlog(CWnd* pParent = NULL);   // standard constructor
	CTgtTransProgDlog();  
	virtual ~CTgtTransProgDlog();


private:
	//m_bCancelled; //! If true, means that user has pressed the cancel button
	//void update();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void setPromptOverall(const INXString & fileName);
	void setPromptOverallDirect(const INXString & fileName);

	void setProgbarOverall(const int & pos);

	void setPromptFile(const INXString & fileName);
	void setProgbarFile(const int & pos);

	void setReset();
	void setCancelBtn( const bool &bEnabled );

	CButton m_ctlBtnCancel;
	CProgressCtrl m_ctlProgbarFile;
	CProgressCtrl m_ctlProgbarOverall;
	CStatic m_ctlStaticFilePrompt;
	CStatic m_ctlStaticOverallPrompt;	

	afx_msg void OnBnClickedButtonCancel();

public:
	virtual BOOL OnInitDialog();
	// !flags whether a user cancel-button press is pending
	bool m_bCancelled;
};
