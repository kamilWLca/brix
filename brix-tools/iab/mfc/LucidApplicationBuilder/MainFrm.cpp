// MainFrm.cpp : implementation of the CMainFrame class
//

/* 

	Main window object

  */

#include "stdafx.h"
#include "DrawProg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define TOOLBAR_32BitRGBA // NOTE - need to change DrawProg.rc IDR_MAINFRAME bmp accordingly

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_COMMAND(ID_VIEW_COMPONENTPALETTE, OnViewPalette)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMPONENTPALETTE, OnUpdateViewPalette)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_LOCALHOST,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	exiting = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(true);

	EnableDocking( CBRS_ALIGN_TOP);
	EnableDocking( CBRS_ALIGN_RIGHT);
	EnableDocking( CBRS_ALIGN_LEFT);
	EnableDocking( CBRS_ALIGN_BOTTOM);

 UINT assignedToolbarID = 3344500;
 DWORD style = WS_CHILD | WS_VISIBLE | CBRS_TOP |
  CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
 if( !m_wndToolBar.CreateEx( this, TBSTYLE_FLAT, style, INXRect(1, 1, 1, 1), assignedToolbarID) )
  return -1;
 if( !m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
  return -1;

/*
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
*/

/* old toolbar method for 4bit colour toolbar
HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(),
    MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_BITMAP,
    0,0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
CBitmap bm;
bm.Attach(hBitmap);
*/

// toolbar bitmap is comprosed of a strip of 16wx15h images for each button - for example if there are 30 buttons on the toolbar, the toolbar bitmap will be 30x16=480 pixels wide
//standard image size for mfc toolbar is 16wx15h
//int toolButtonHeight = 15;
//int toolButtonWidth = 16;
int toolButtonHeight = 24;
int toolButtonWidth = 24;
m_wndToolBar.SetSizes(INXSize(toolButtonWidth + 7, toolButtonHeight + 6), INXSize(toolButtonWidth, toolButtonHeight)); // NOTE - will fail unless button is bigger than image, +7 pixels w and +6 pixels h

#ifndef TOOLBAR_32BitRGBA

// use IDR_MAINFRAME Toolbar_24bit.bmp or Toolbar_16bit.bmp in DrawProg.rc

// create a toolbar using the imagelist
CImageList *m_imagelist = new CImageList;
HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
   MAKEINTRESOURCE(IDR_MAINFRAME),
   IMAGE_BITMAP,
   0,0, // cx, cy
   LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS );
CBitmap bm;
bm.Attach(hbm);

m_imagelist->Create(toolButtonWidth,toolButtonHeight,ILC_COLOR24|ILC_MASK,1,1); //24bit colour with colour mask
//m_imagelist->Create(toolButtonWidth,toolButtonHeight,ILC_COLOR16|ILC_MASK,1,1); //16bit colour with colour mask
m_imagelist->Add(&bm, RGB(215,215,215)); // back colour of Toolbar_24bit.bmp and Toolbar_16bit.bmp to use as colour mask
m_wndToolBar.GetToolBarCtrl().SetImageList(m_imagelist);
//m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(m_imagelist);  // don't need this as mfc greys out buttons by default but if you want to design the images for disabled buttons, create an image list and add it here

#else 

// use IDR_MAINFRAME Toolbar_32bit.bmp in DrawProg.rc

// - this only works for machines with 32-bit colour depth, otherwise get black background
// but still have a problem where there is a dark background colour to each button in wine
// this may be the way the image is created in GIMP, some problem with the alpha channel
CImageList *m_imagelist = new CImageList;
HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
   MAKEINTRESOURCE(IDR_MAINFRAME),
   IMAGE_BITMAP,
   0,0, // cx, cy
   LR_CREATEDIBSECTION);
CBitmap bm;
bm.Attach(hbm);

m_imagelist->Create(toolButtonWidth,toolButtonHeight,ILC_COLOR32,1,1); // no colour mask with alpha transparency
m_imagelist->Add(&bm, (CBitmap*)NULL);
m_wndToolBar.GetToolBarCtrl().SetImageList(m_imagelist);
//@todo - create a diabled image list. When using 32bit RGBA need a disabled image list, as mfc can't create greyed out icons as no colour mask
//m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(m_imagelist);

#endif //TOOLBAR_32BitRGBA

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	// set width of pane and remove border
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_LOCALHOST),ID_INDICATOR_LOCALHOST, SBPS_NOBORDERS, 14);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_NUM),ID_INDICATOR_NUM, SBPS_NOBORDERS, 14);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_CAPS),ID_INDICATOR_CAPS, SBPS_NOBORDERS, 84);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SCRL),ID_INDICATOR_SCRL, SBPS_NOBORDERS, 84);


	INXRect rect;
	int nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_EHS_HOST_COMBO);
	m_wndToolBar.SetButtonInfo(nIndex, ID_EHS_HOST_COMBO, TBBS_SEPARATOR, 110); // this sets the width and makes it a separator type - may need to change back to button type if want to work with larger toolbar buttons 32x32
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, (LPRECT)rect);
	rect.top = 4; // added y-offset for 24x24 toolbar
	rect.bottom = rect.top + 250 /*drop height*/;
	if(!m_combo.Create(CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VISIBLE |
		WS_TABSTOP | WS_VSCROLL, rect, &m_wndToolBar, ID_EHS_HOST_COMBO))
	{
		TRACE(_T("Failed to create combo-box\n"));
		return FALSE;
	}
	m_combo.AddString(TARGET_DISPLAY_NAME_LOCAL);
	m_combo.AddString(TARGET_DISPLAY_NAME_REMOTE);
	m_combo.AddString(TARGET_DISPLAY_NAME_APPSERVER);
	CFont *m_Font = m_wndStatusBar.GetFont();
	m_combo.SetFont(m_Font);




	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	
	// create project bar

	if (!m_wndProjectBar.Create(_T(""), this, 123))
	{
		TRACE0("Failed to create project bar\n");
		return -1;		// fail to create
	}

	m_wndProjectBar.SetBarStyle(m_wndProjectBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndProjectBar.SetSCBStyle(m_wndProjectBar.GetSCBStyle() | SCBS_SIZECHILD);

	m_wndProjectBar.SetWindowTextA( _T("Project Bar") );

	m_wndProjectBar.EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndProjectBar, AFX_IDW_DOCKBAR_RIGHT);

	m_wndProjectBar.init();


	if (!m_cFuncBlockBar.Create("FuncBlockBar", this, 123 ))
	{
		TRACE0("Failed to create project FncBlkBar\n");
		return -1;		// fail to create
	}

	m_cFuncBlockBar.SetBarStyle(m_cFuncBlockBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_cFuncBlockBar.SetSCBStyle(m_cFuncBlockBar.GetSCBStyle() |SCBS_SIZECHILD);

	m_cFuncBlockBar.EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_cFuncBlockBar, AFX_IDW_DOCKBAR_LEFT);

	m_cFuncBlockBar.init();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// DONT do the std MFC thing of updating the frame title to the name
	// of the child-window's file, each time child is clicked.
	cs.style &= ~ FWS_ADDTOTITLE;
	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnViewProject() 
{	
	ShowControlBar(&m_wndProjectBar, !m_wndProjectBar.IsVisible(), FALSE);
}

void CMainFrame::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndProjectBar.IsVisible());
}
void CMainFrame::OnViewPalette() 
{	
	ShowControlBar(&m_cFuncBlockBar, !m_cFuncBlockBar.IsVisible(), FALSE);
}

void CMainFrame::OnUpdateViewPalette(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_cFuncBlockBar.IsVisible());
}

void  CMainFrame::updateFunctionBlockBar()
{
	m_cFuncBlockBar.loadContents();
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	exiting = TRUE;
	CMDIFrameWnd::OnClose();
}

/*
BOOL CMainFrame::CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIFrameWnd::CreateClient(lpCreateStruct, pWindowMenu);
}
*/

//BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
//{
	/*
	if (!m_wndSplitterLR.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitterLR.CreateView(0, 1, RUNTIME_CLASS(CRightView), INXSize(0, 0), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	m_wndSplitterLR.SetColumnInfo(0, 200, 0);

	if (!m_wndSplitterUD.CreateStatic(&m_wndSplitterLR, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitterLR.IdFromRowCol(0, 0)))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitterUD.CreateView(0, 0, RUNTIME_CLASS(CLeftView), INXSize(0, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	if (!m_wndSplitterUD.CreateView(1, 0, RUNTIME_CLASS(CProjectBar), INXSize(0, 0), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}

	return TRUE;*/
//	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
//}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	CMDIFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::setFrameCaption()
{
	CWnd * m_pMainWnd = AfxGetApp()->m_pMainWnd;
  
	CMDIFrameWnd* pMainWnd = (CMDIFrameWnd*)AfxGetMainWnd();
   // Get the active MDI child window
	CMDIChildWnd* pChild = (CMDIChildWnd*)pMainWnd->MDIGetActive();
   // Get the active view attached to the active MDI child window.
	CView* pOldActiveView = NULL;
	CDrawProgView* pView = NULL;
	if (pChild) {
		pOldActiveView = pChild->GetActiveView();
		pView = (CDrawProgView*)pOldActiveView;
	}

	if(pView) {
		INXString csProjectDir, csProjectName;
		pView->pProject->pProjMData->getProjectDir(csProjectDir);
		pView->pProject->pProjMData->getProjectName(csProjectName);	
		m_pMainWnd->SetWindowTextA(LAB_APP_NAME + csProjectDir + ":" + csProjectName);

	} else {
		m_pMainWnd->SetWindowTextA(LAB_APP_NAME + NO_PROJECT_WARNING);
	}
	//CDialog::OnOK();
}

			