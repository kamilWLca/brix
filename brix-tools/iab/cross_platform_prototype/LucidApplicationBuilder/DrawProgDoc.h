// DrawProgDoc.h : interface of the CDrawProgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_DRAWPROGDOC_H_INCLUDED_)
#define _DRAWPROGDOC_H_INCLUDED_
/*
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
*/
#include "MainFrame.h"
#include "condat.h"
//#include "ProjectChooser.h"
//#include "PathDialog.h" //@todo
//#include "Project.h"
#include "DEP.h"
#include <wx/docview.h>

/*

Handles the GUI selection of reading and writing files - The actual objects that are read or written to files
are all contained in the associated CDrawProgView object.

*/
class DrawProgDoc : public wxDocument
{
protected: // create from serialization only
	DrawProgDoc();

// Attributes
public:
	MainFrame* pFrame;
	bool closing;
	Project* pProject;
	DEP* pDEP;
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Operations
	void SaveUserDefDoc();
#endif
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawProgDoc)
	public:
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	virtual void Serialize(CArchive& ar);
#endif
	virtual bool OnOpenDocument(const wxString& filename);
	virtual bool OnCloseDocument();
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE	
protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
#endif
// Implementation
public:
	virtual ~DrawProgDoc();
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawProgDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewsubsystem(CCmdUI* pCmdUI);
	afx_msg void OnNewsubsystem();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveProject(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWPROGDOC_H__EC34D26C_981B_11D3_A9AD_00001C21326E__INCLUDED_)
