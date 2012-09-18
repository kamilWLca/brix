// DrawProg.h : main header file for the DRAWPROG application
//


#if !defined(AFX_DRAWPROG_H__EC34D264_981B_11D3_A9AD_00001C21326E__INCLUDED_)
#define AFX_DRAWPROG_H__EC34D264_981B_11D3_A9AD_00001C21326E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Project.h"
#include "DrawProgView.h"
#include "../common/LabLgbBaseApp.h"
//#include "TransferAppDialog.h"
#include "TgtTransProgDlog.h"
#include "Porting_Classes/INXString.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawProgApp:
// See DrawProg.cpp for the implementation of this class
//
/*@todo wxWidgets ******/
#include "wx/wx.h"
class INXDrawProgApp : public wxApp
{
public:
    bool OnInit();
};

class CDrawProgApp : public CLabLgbBaseApp
{
public:
	CDrawProgApp();
	virtual ~CDrawProgApp();
	
	//attributes
	CMultiDocTemplate* pDocTemplate;
	CMainFrame* pMainFrame;
	
	// Methods
	CDocument* OpenProject(ProjectMetaData* pProjMData);
	void CloseProject(Project* pProject);
	void CloseUserDefDoc(HTREEITEM hItem);
	bool IsModified(Project* pProject);
	int GetOrientation();
	void CreateProjectFile(INXString ProjectDir, INXString ProjectName);
	//void addProjectToMRUList(INXString csProjectPathName); Now moved to parent class so LGB can use it.
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawProgApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDrawProgApp)
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateReloadEhs(CCmdUI* pCmdUI);
	//afx_msg void OnReloadEhs();
	afx_msg void OnNewProject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnOpenProject();
	afx_msg void OnHelpLabUserGuide();
	afx_msg void OnHelpLabRefManual();

	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	BOOL SetWorkDirGlobal(char * workDir);
	
public:
	//static CTransferToTargetDialog *c_pTransferToTargetDialog;
public:
	afx_msg void OnHelpTutorials();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWPROG_H__EC34D264_981B_11D3_A9AD_00001C21326E__INCLUDED_)

