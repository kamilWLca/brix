#pragma once

#include "Resource.h"
#include "Porting_Classes/INXString.h"
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
// CProjectDialog dialog

class CProjectDialog : public wxDialog
{
	//DECLARE_CLASS(CProjectDialog)

public:
	CProjectDialog(wxWindow* pParent = NULL);   // standard constructor
	virtual ~CProjectDialog();

// Dialog Data
	enum { IDD = IDD_PROJECTDIALOG };
private:
	wxTextCtrl *projectNameTxtBox;
	wxTextCtrl *projectRtDirTxtBox;
	wxTextCtrl *commercialNameBox;
	wxTextCtrl *versionTxtBox;
	wxTextCtrl *descriptionBox;

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnOk(wxCommandEvent& event);
	void OnFindProjDir(wxCommandEvent & event);
	DECLARE_EVENT_TABLE()
public:
	//virtual BOOL OnInitDialog();

	bool IsValidDir();
	bool IsValidName();
	bool ProjectExists();
	void SetIsWinEnv(bool isWinEnv);


	INXString m_ProjectName;
	INXString m_ProjectDir;
	//CEdit m_ProjectDirEdit;
	INXString m_ProjectCommercialName;
	INXString m_ProjectVersion;
	INXString m_ProjectDescription;

private:
	bool bIsWindowsEnv;

};
