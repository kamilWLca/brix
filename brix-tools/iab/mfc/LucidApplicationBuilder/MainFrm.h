// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__EC34D268_981B_11D3_A9AD_00001C21326E__INCLUDED_)
#define AFX_MAINFRM_H__EC34D268_981B_11D3_A9AD_00001C21326E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ProjectBar.h"
#include "LucidConstants.h"
#include "MyTreeCtrl.h"

#include "FunctionBlockBar.h"
#include "FunctionBlockTree.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CProjectBar m_wndProjectBar;
	//CProjectBar	m_wndProjectBar2;

	void updateFunctionBlockBar();

//	ConInfoBar m_wndConInfoDBar;
	//CMyTreeCtrl m_wndProjectBar.m_cProjTree;

	//FunctionBlockTree	m_cFuncBlockTree;
	FunctionBlockBar	m_cFuncBlockBar;

	bool exiting;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CComboBox	m_combo;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewProject();
	afx_msg void OnViewPalette();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPalette(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

	void setFrameCaption();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__EC34D268_981B_11D3_A9AD_00001C21326E__INCLUDED_)

	