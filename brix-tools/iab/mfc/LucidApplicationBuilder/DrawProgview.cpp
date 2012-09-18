// DrawProgView.cpp : implementation of the CDrawProgView class
//

/* 

  This class contains a CObject typed list of all icons in the display.Currently all other displayed data
  is a child of an icon. All programme data is held in this view, rather than the Document Class, because the data is spcific to this view.
  	Displaying manipulating etc is all done via access through icon objects (ConDat)
  Temporary attributes are provided for storing information such as user connection actions etc. to interpret 
  what a GUI user is doing.
  */

#include "stdafx.h"
#include "Resource.h"
#include "DrawProg.h"
#include "MainFrm.h"
#include "DrawProgDoc.h"
#include "DrawProgView.h"
#include "bmpfile.h"
#include "condat.h"
#include "IconData.h"
#include "Tags.h"
#include "delims.h"
#include "Datsrc.h"
#include "config.h"
#include "LucidConstants.h"
#include "colourKey.h"
#include "AddPortDialog.h"
#include "Encapsulation.h"
#include "BlockNameDialog.h"
#include "PortLabelDialog.h"
#include "TagDialog.h"
#include "PrintData.h"
#include "ZoomDialog.h"
#include "ChildFrm.h"
#include "SelectGroupDialog.h"
#include "LucidStateMachine.h"
#include "RJMFileFind2.h"
#include "ExportDialog.h"
#include "FileOperations.h"
#include "ProjectDialog.h"
#include "LucidTcpipClient.h"
#include "LucidEnums.h"
#include "LucidTcpipClient.h"
#include "BlockOperations.h"
#include "TransferToTargetDialog.h"
#include "MenuNameDialog.h"
#include "GlobalFuncs_2.h"
#include "ProjectDescriptionDialog.h"
#include "LucidRegAccess.h"
#include <afxtempl.h>
#include <fstream>
using namespace std;
#include <sys/stat.h>
//#include <sys/types.h>
#include <io.h>
#include <math.h>
#include <curl/curl.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>

//include openGL tools and wxWidgets tools
#include "wx/wx.h"
#include <wx/glcanvas.h>

#define CURL_WRITE_BUFFER_SIZE 12000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];
//bool CDrawProgView::c_bSendAllToTargetHasOccurred;


/*@todo wx seperate window setup*/
#define testingGLRending
#ifdef testingGLRending
DEP *glDEP;
CDC *glTempDC;
void Initialize(){
	glOrtho(0,500,500,0,0.0f,100.0f);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
}
class wxGLCanvasSubClass: public wxGLCanvas {
        void Render();
public:
    wxGLCanvasSubClass(wxFrame* parent);
    void Paintit(wxPaintEvent& event);
protected:
    DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(wxGLCanvasSubClass, wxGLCanvas)
    EVT_PAINT    (wxGLCanvasSubClass::Paintit)
END_EVENT_TABLE()
 
wxGLCanvasSubClass::wxGLCanvasSubClass(wxFrame *parent)
:wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas")){
  //  int argc = 1;
    //char* argv[1] = { wxString((wxTheApp->argv)[0]).char_str() };
}

void wxGLCanvasSubClass::Paintit(wxPaintEvent& WXUNUSED(event)){
    Render();
}
 
void wxGLCanvasSubClass::Render(){
    SetCurrent();
    wxPaintDC(this);
	Initialize();
	//set background
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDEP->DrawGL(glTempDC);
	glFlush();
    SwapBuffers();
}
 
class MyApp: public wxApp
{
    virtual bool OnInit();
    wxGLCanvas * MyGLCanvas;
};
IMPLEMENT_APP_NO_MAIN(MyApp)

bool MyApp::OnInit(){
    wxFrame *frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello GL World"), wxPoint(50,50), wxSize(500,500));
     MyGLCanvas = new wxGLCanvasSubClass(frame);
    frame->Show(TRUE);
    return TRUE;
}
#endif
// CDrawProgDoc

IMPLEMENT_DYNCREATE(CDrawProgDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawProgDoc, CDocument)
	//{{AFX_MSG_MAP(CDrawProgDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBSYSTEM, OnUpdateNewsubsystem)
	ON_COMMAND(ID_NEWSUBSYSTEM, OnNewsubsystem)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_SAVE_PROJECT, OnUpdateSaveProject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CDrawProgView::bHasConnectionToEHS = false;


/////////////////////////////////////////////////////////////////////////////
// CDrawProgView

IMPLEMENT_DYNCREATE(CDrawProgView, CScrollView)

BEGIN_MESSAGE_MAP(CDrawProgView, CScrollView)
	//{{AFX_MSG_MAP(CDrawProgView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_DELETE_LINE, OnDeleteLine)
	ON_COMMAND(ID_DELETE_NODE, OnDeleteNode)
	ON_COMMAND(ID_STRAIGHTEN, OnStraighten)
	ON_COMMAND(ID_LINE_REROUTE, OnReRoute)
	ON_COMMAND(ID_LINE_ADDNODE, OnAddNode)
	ON_COMMAND(ID_ICON_REROUTEALL, OnReRouteAll)
	ON_COMMAND(ID_ICON_SHOW_DESC, OnIconShowDesc)
	ON_COMMAND(ID_SUBSET, OnSubset)
//	ON_COMMAND(ID_TOPDOWN, OnTopdown) //dk: removed from IAB as doesn't work (doesn't create a subsystem), and deemed unnecessary as can use create subsystem
//	ON_COMMAND(ID_FTP_OPTIONS, OnFtpOptions)
//	ON_COMMAND(ID_FTP_SODL, OnFtpSodl)
	ON_COMMAND(ID_PORT_INITIALISE, OnPortInitialise)
	ON_COMMAND(ID_KEY, OnKey)
	ON_COMMAND(ID_ICON_CREATEINSTANCE, OnCreateInstance)
	ON_COMMAND(ID_UP_LEVEL, OnUpLevel)
	ON_COMMAND(ID_DEBUG_START, OnDebugStart)
	ON_COMMAND(ID_DEBUG_STOP, OnDebugStop)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_DEBUG_PAUSE, OnDebugPause)
	ON_COMMAND(ID_DEBUG_STEP, OnDebugStep)
	ON_COMMAND(ID_DEBUG_FORCE, OnDebugForce)
	ON_COMMAND(ID_DBGHIST_START, OnDbgHistStart)
	ON_COMMAND(ID_DBGHIST_STEP, OnDbgHistStep)
	ON_COMMAND(ID_DBGHIST_TIME, OnDbgHistTime)
	ON_COMMAND(ID_DBGHIST_STOP, OnDbgHistStop)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STOP, OnUpdateDebugStop)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_FORCE, OnUpdateDebugForce)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_PAUSE, OnUpdateDebugPause)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEP, OnUpdateDebugStep)
	ON_UPDATE_COMMAND_UI(ID_DBGHIST_START, OnUpdateDbgHistStart)
	ON_UPDATE_COMMAND_UI(ID_DBGHIST_STEP, OnUpdateDbgHistStep)
	ON_UPDATE_COMMAND_UI(ID_DBGHIST_STOP, OnUpdateDbgHistStop)
	ON_UPDATE_COMMAND_UI(ID_DBGHIST_TIME, OnUpdateDbgHistTime)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_START, OnUpdateDebugStart)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_DEBUG_RESTART, OnDebugRestart)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_RESTART, OnUpdateDebugRestart)
	ON_WM_TIMER()
	ON_WM_TIMER(TIMER_IDS_DRAWANIMATEDSTUFF,OnTimer)
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI(ID_UP_LEVEL, OnUpdateUpLevel)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_FTP_SODL, OnUpdateFtpSodl)
	ON_COMMAND(ID_ADD_PORT, OnAddPort)
	ON_UPDATE_COMMAND_UI(ID_ADD_PORT, OnUpdateAddPort)
	ON_COMMAND(ID_DELETE_PORT, OnDeletePort)
	ON_COMMAND(ID_TAG_LINE, OnTagLine)
	ON_COMMAND(ID_SHOW_LINE, OnShowLine)
	ON_COMMAND(ID_DELETE_TAG, OnDeleteTag)
	ON_COMMAND(ID_ADD_TAG, OnAddTag)
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_COMMAND(ID_WRITE_SODL, OnWriteSodl)
	ON_UPDATE_COMMAND_UI(ID_WRITE_SODL, OnUpdateWriteSodl)
	ON_COMMAND(ID_GROUP_SETUP, OnGroupSetup)
	ON_COMMAND(ID_ADD_ICON_TO_GROUP, OnAddIconToGroup)
	ON_COMMAND(ID_ADD_PORT_TO_GROUP, OnAddPortToGroup)
	ON_COMMAND(ID_ADD_TO_GROUP, OnAddToGroup)
	ON_COMMAND(ID_SAVE_PROJECT, OnSaveProject)
//	ON_COMMAND(ID_LIBRARY, OnLibrary) //dk: removed from IAB, works but deemed unnecessary as can use create subsystem
	ON_COMMAND(ID_IMPORT_LIB, OnImportLib)
	ON_COMMAND(ID_EXPORT_LIB, OnExportLib)
	ON_COMMAND(ID_CLOSE_PROJECT, OnCloseProject)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_PROJECT, OnUpdateCloseProject)
	ON_COMMAND(ID_SELECT_ALL, OnSelectAll)
	//ON_WM_LBUTTONDBLCLK() //@todo unused and interferes with INXPoint
	ON_UPDATE_COMMAND_UI(ID_GROUP_SETUP, OnUpdateGroupSetup)
//	ON_COMMAND(ID_FTP_EHS, OnFtpEhs)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_IMPORT_DATA_FILE, &CDrawProgView::OnImportDataFile)
	ON_COMMAND(ID_SAVE_PROJECT_AS, &CDrawProgView::OnSaveProjectAs)
	ON_COMMAND(ID_EDIT_PROJECT_DESC, &CDrawProgView::OnEditProjectDesc)
	ON_COMMAND(ID_EDIT_DELETE, &CDrawProgView::OnEditDelete)
	ON_WM_SIZE()
	//ON_WM_MOUSEWHEEL() @todo is not used and doesnt allow to use INXPoint
	ON_COMMAND(ID_TRANSFER_OPTIONS, &CDrawProgView::OnTransferOptions)
	ON_COMMAND(ID_ZOOM_IN, &CDrawProgView::OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CDrawProgView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CDrawProgView::OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CDrawProgView::OnUpdateZoomOut)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_LIB, &CDrawProgView::OnUpdateImportLib)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_LIB, &CDrawProgView::OnUpdateExportLib)
	ON_UPDATE_COMMAND_UI(ID_TRANSFER_MANAGER, &CDrawProgView::OnUpdateTransferManager)
	ON_UPDATE_COMMAND_UI(ID_LAUNCH_TRANSFER, &CDrawProgView::OnUpdateLaunchTransfer)
	ON_COMMAND(ID_VIEW_REFRESH, &CDrawProgView::OnViewRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, &CDrawProgView::OnUpdateViewRefresh)
	ON_COMMAND(ID_RENAME_PORT, &CDrawProgView::OnRenamePort)
	ON_COMMAND(ID_RENAME_XPORT, &CDrawProgView::OnRenameXport)
	ON_COMMAND(ID_ADD_TO_LIBRARY, &CDrawProgView::OnAddToLibrary)
	ON_UPDATE_COMMAND_UI(ID_ADD_TO_GROUP, &CDrawProgView::OnUpdateAddToGroup)
	ON_UPDATE_COMMAND_UI(ID_LAB_SENDCHANGEDDATA, &CDrawProgView::OnUpdateTransferChanged)
	
	ON_COMMAND(ID_LAB_TRANSFERALLDATA, &CDrawProgView::OnLabTransferAll)
	ON_COMMAND(ID_LAB_SENDCHANGEDDATA, &CDrawProgView::OnLabTransferChanged)
	ON_COMMAND(ID_DEFINE_MONITORS, &CDrawProgView::OnDefineMonitors)
	ON_COMMAND(ID_OPTIONS_SELECTALLMONITORS, &CDrawProgView::OnSelectAllMonitors)
	ON_COMMAND(ID_OPTIONS_CLEARALLMONITORS, &CDrawProgView::OnClearAllMonitors)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SELECTALLMONITORS, &CDrawProgView::OnUpdateSelectAllMonitors)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_CLEARALLMONITORS, &CDrawProgView::OnUpdateClearAllMonitors)
	ON_UPDATE_COMMAND_UI(ID_DEFINE_MONITORS, &CDrawProgView::OnUpdateDefineMonitors)
	ON_COMMAND(ID_TRACE, &CDrawProgView::OnTrace)
	ON_UPDATE_COMMAND_UI(ID_TRACE, &CDrawProgView::OnUpdateTrace)
	ON_UPDATE_COMMAND_UI(ID_LAB_TRANSFERALLDATA, &CDrawProgView::OnUpdateTransferAll)
	ON_COMMAND(ID_NATIONAL_LANGUAGE_SUPPORT, &CDrawProgView::OnNationalLanguageSupport)
	ON_COMMAND(ID_FILE_IMPORTRESOURCEFILE, &CDrawProgView::OnImportResourceFile)
	ON_COMMAND(ID_DEBUG_STARTWITHRESTART, &CDrawProgView::OnDebugStartwithrestart)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STARTWITHRESTART, &CDrawProgView::OnUpdateDebugStartwithrestart)

	ON_COMMAND(ID_LAB_EHS_START, &CDrawProgView::OnEHSStartRestart)
	ON_COMMAND(ID_LAB_EHS_STOP, &CDrawProgView::OnEHSStop)
	ON_COMMAND(ID_LAB_EHS_TOPMOST, &CDrawProgView::OnEHSTopmost)
	ON_COMMAND(ID_LAB_EHS_BOTTOMMOST, &CDrawProgView::OnEHSBottommost)
	ON_COMMAND(ID_LAB_EHS_NORMALZORDER, &CDrawProgView::OnEHSNormalZOrder)
	ON_COMMAND(ID_LAB_EHS_INIT_PARAMS, &CDrawProgView::OnEHSInitParams)
	ON_COMMAND(ID_EHS_MANAGEEHSLOCALLY, &CDrawProgView::OnEHSManageLocally)
	ON_COMMAND(ID_EHS_MANAGEEHSREMOTELY, &CDrawProgView::OnEHSManageRemotely)
	ON_UPDATE_COMMAND_UI(ID_EHS_MANAGEEHSLOCALLY, &CDrawProgView::OnUpdateEHSManageLocally)
	ON_UPDATE_COMMAND_UI(ID_EHS_MANAGEEHSREMOTELY, &CDrawProgView::OnUpdateEHSManageRemotely)
	ON_UPDATE_COMMAND_UI(ID_LAB_EHS_START, &CDrawProgView::OnUpdateEHSStartRestart)
	ON_UPDATE_COMMAND_UI(ID_LAB_EHS_STOP, &CDrawProgView::OnUpdateEHSStop)
	ON_UPDATE_COMMAND_UI(ID_LAB_EHS_INIT_PARAMS, &CDrawProgView::OnUpdateEHSInitParams)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LOCALHOST, &CDrawProgView::OnUpdateLocalHost)
	ON_CBN_SELCHANGE(ID_EHS_HOST_COMBO, &CDrawProgView::OnSelChangeEHSHostCombo)
END_MESSAGE_MAP()

//CRITICAL_SECTION m_cs;

/////////////////////////////////////////////////////////////////////////////
// CDrawProgView construction/destruction
void CDrawProgView::OnAddNode() {
	int iSegmentNum = -1;
	ConData* blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
	if (selectedPortType == INPUTPORT) {
		iSegmentNum = blob->inputport[selectedPort]->line.GetLineSegment(SnapToGrid(m_cpRButtonDown));
	}
	else if (selectedPortType == STARTPORT) {
		iSegmentNum = blob->startport[selectedPort]->line.GetLineSegment(SnapToGrid(m_cpRButtonDown));
	}
	if (iSegmentNum >= 0) {
		if (selectedPortType == INPUTPORT) {
			blob->inputport[selectedPort]->line.AddNode(SnapToGrid(m_cpRButtonDown), iSegmentNum + 1);
		}
		else if (selectedPortType == STARTPORT) {
			blob->startport[selectedPort]->line.AddNode(SnapToGrid(m_cpRButtonDown), iSegmentNum + 1);
		}
		RedrawWindow();
		SaveUndo();
		pProject->SaveProjectDep();
	}
}


CDrawProgView::CDrawProgView()
{
	// TODO: add construction code here
	pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pProject = NULL;
	pDEP = NULL;	

	INXString format = AfxGetApp()->GetProfileString("DragDrop", "Clipformat", "Common");
	if (format == "Private")
		m_DragDropFormat = ::RegisterClipboardFormat("InterfaceClipboardFormat");
	else
	{
		if (format != "Common")
			AfxMessageBox("Please specify a valid clipformat!", MB_ICONSTOP);
		m_DragDropFormat = CF_TEXT;
	}

	init();
}

CDrawProgView::~CDrawProgView()
{
	/*if (pProject) {
		OnDbgHistStop();
		while (!pProject->debug.dbgFinished) {
			Sleep(50);
		}
	}*/
}

void CDrawProgView::init() {

	//c_bSendAllToTargetHasOccurred = false;

	//kwhite:canvassupport replaced following with above
	xViewSize = 2000;
	yViewSize = 2000;
	viewSize = INXSize(xViewSize, yViewSize);

	SetScrollSizes(MM_TEXT, INXSize(0,0));
	scale = 100;
	running = 0;
	moveselected = NULL;
	m_iLftBtnDownState = 0;
	enableSubset = 0;
	showfixed = 1;
	pasteFlag = FALSE;
	copyFlag = FALSE;
	lock = FALSE;
	lib = -1;
	//create pen types
	redpen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	greenpen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	bluepen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	blackpen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	nullpen.CreatePen(PS_SOLID,2,RGB(255,255,255)); //for deleting
	dashkpen.CreatePen(PS_DASH,1,RGB(0,0,0));
	greenbrush.CreateSolidBrush( RGB(0,255,0) );
}

// initialise undo/redo function
void CDrawProgView::initUndo() {
	undoFlag = FALSE;
	redoFlag = FALSE;
	wrapFlag = FALSE;
	iUndoFileNum = 0;
	maxUndoFileNum = 0;
	iUndoFlagNum = 1;

	// remove all the undo files
	int filenum = 1;
	char szUndoFileNum[MAX_UNDO_FILE_NUM];
	_itoa_s(filenum, szUndoFileNum, 10);
	while (remove(workDir + TEMPDIR + "undo" + szUndoFileNum + ".prg") != -1) {
		remove(workDir + TEMPDIR + "lpj_undo" + szUndoFileNum);
		if (filenum >= (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1) {
			filenum = 0;
		}
		else {
			filenum++;
		}
		_itoa_s(filenum, szUndoFileNum, 10);
	}
	
	// save the initial DEP
	SaveUndo();
	undoFlag = FALSE;
}

/*
This function is run before a window is displayed, and allows configurations to be made before display
  */
BOOL CDrawProgView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	//SetScrollSizes( MM_TEXT, INXSize(2000,1000));
  	//SetScrollSizes( MM_LOENGLISH, INXSize(2000,1000));
  //

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawProgView This funtion iterates through all the icons and runs there display functions
// by passing the View screen handle pDC.
//  
/*@todo test the only onece*/

void CDrawProgView::OnDraw(CDC* dc)
{
	// Set Frame Caption
	// Want to make frame caption is set for active view
	// Not sure where the best place is to do this
	// Need to cater for the case when 2 projects are open and 1 is closed
	pFrame->setFrameCaption();
	bool tempAnimFlag = onlyDrawAnimatedStuff;
	onlyDrawAnimatedStuff = false; // turn this flag off ASAP, to minimise collision of onDraw called for everything
//	otherDrawEvent = false;

	if (!pDEP->depFSM.enabledraw) return; 

	// prevent screen flickering by using double buffering
	if ((pProject->debug.debugMode == DBGSTART) || (m_iLftBtnDownState == 5) || (m_iLftBtnDownState == 4)) {
		CMemDC pDC(dc);
		CDrawProgDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDC->Rectangle(encapsulate);
		pDC->Rectangle(selectRect);
		pDEP->Draw(pDC);
	} else {
		if (!tempAnimFlag) {
			dc->Rectangle(encapsulate);
			if (m_iLftBtnDownState != 6) //kwhite:canvassupport 
			{
				dc->Rectangle(selectRect);
			}
		}
/*
		if (tempAnimFlag) {
			TRACE(_T("OnDraw - anim true\n"));
		} else {
			TRACE(_T("OnDraw - anim false\n"));
		}
*/
		pDEP->Draw(dc,tempAnimFlag,toggleAnimation);
	}
	glDEP = pDEP; //temporary testing of openGL 
	glTempDC = dc;
	/*
	if (pDC->IsPrinting()) {
		pDC->SetMapMode(MM_LOENGLISH);
		pDC->Rectangle(20, -20, 220, -220);
	}
	else {
		pDC->Rectangle(20, 20, 220, 220);
	}	
	*/
	// The following is an example of a dashed line. In print preview this causes a debug assertion failure
	/*
	INXPoint start, end;
	start.x = 20;
	start.y = 20;
	end.x = 220;
	end.y = 20;

	pDC->SelectObject(dashkpen);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	*/

	// TODO: add draw code for native data here
	//CDC *theDC = GetDC();
	// The following line was in the original. Was removed as it prevented printing
	/*CDC* theDC=pDC;*/
	
	// The following line was in the original. Was removed as it prevented printing
	/*ReleaseDC(theDC);	*/


}

/////////////////////////////////////////////////////////////////////////////
// CDrawProgView printing

BOOL CDrawProgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_lpUserData = new CPrintData;

	// Get the whole view area
	INXRect ViewExtent = GetViewExtent();
	// Save the reference point for the whole document
	((CPrintData*)(pInfo->m_lpUserData))->m_ViewRefPoint = INXPoint(ViewExtent.left, ViewExtent.top);
	// Get the name of the view file and save it
	((CPrintData*)(pInfo->m_lpUserData))->m_ViewTitle = pDEP->depFilename;
	// Calculate how many printed page widths of PAGE_WIDTH units are required to 
	// accomodate the width of the view
	if (((CDrawProgApp*)AfxGetApp())->GetOrientation() == 0) {
		((CPrintData*)(pInfo->m_lpUserData))->m_nWidths = (UINT)ceil(((double)(ViewExtent.Width()))/PAGE_WIDTH);
		// Calculate how many printed page lengths of PAGE_LENGTH units are required to
		// accomodate the length of the view
		((CPrintData*)(pInfo->m_lpUserData))->m_nLengths = (UINT)ceil(((double)(ViewExtent.Height()))/PAGE_LENGTH);
	}
	else if (((CDrawProgApp*)AfxGetApp())->GetOrientation() == 1) {
		((CPrintData*)(pInfo->m_lpUserData))->m_nWidths = (UINT)ceil(((double)(ViewExtent.Width()))/PAGE_LENGTH);
		// Calculate how many printed page lengths of PAGE_LENGTH units are required to
		// accomodate the length of the view
		((CPrintData*)(pInfo->m_lpUserData))->m_nLengths = (UINT)ceil(((double)(ViewExtent.Height()))/PAGE_WIDTH);
	}
	// Set first page number as 1 and
	// set the last page number as the total number of pages
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(((CPrintData*)(pInfo->m_lpUserData))->m_nWidths * ((CPrintData*)(pInfo->m_lpUserData))->m_nLengths);
	
	if (!DoPreparePrinting(pInfo))
	{
		delete (CPrintData*)(pInfo->m_lpUserData);
		return FALSE;
	}

	return TRUE;
}

void CDrawProgView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDrawProgView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	// Delete print data object
	delete (CPrintData*)(pInfo->m_lpUserData);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawProgView diagnostics

#ifdef _DEBUG
void CDrawProgView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDrawProgView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDrawProgDoc* CDrawProgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawProgDoc)));
	return (CDrawProgDoc*)m_pDocument;
}
#endif //_DEBUG

// Called when an icon is selected to be moved it is drawn as a rectangle outline
void CDrawProgView::SetToMovingRectangle(INXPOSITION selected) {
		ConData *icondata;

		icondata = ((ConData *) pDEP->condata->GetAt(selected));
		RectangleSite.CopyRect(icondata->GetBoundingRectangle());
		// Make sure bounding rectangle is snapped to grid before moving
		//RectangleSite.SetRect(pDEP->SnapToGrid(RectangleSite.TopLeft()),pDEP->SnapToGrid(RectangleSite.BottomRight()));
		// Adjust size of RectangleSite according to scale
		/*double deltaHeight = ((RectangleSite.Height()/2)*(100-scale))/100;
		double deltaWidth = ((RectangleSite.Width()/2)*(100-scale))/100;
		RectangleSite.DeflateRect((int)deltaWidth, (int)deltaHeight);*/
		//RectangleSite.OffsetRect(-RectangleSite.TopLeft()); //make into 0 offset box
}

INXRect CDrawProgView::GetViewExtent() {
	INXRect ViewExtent(0,0,1,1); // Initial view extent
	INXRect ElementBound(0,0,0,0); // Space for element bounding rectangle
	INXPOSITION pos;
	ConData* blob;

	//CClientDC dc(this); 
	//dc.SetMapMode(MM_LOENGLISH);
	pos = pDEP->condata->GetHeadPosition();
	while (pos) {
		blob = (ConData*) pDEP->condata->GetNext(pos);
		//dc.LPtoDP(blob->rectangle);
		ElementBound = blob->GetBoundingRectangle();
		// Make coordinates of view extent the outer limits

		ViewExtent.UnionRect(ViewExtent, ElementBound);

	}
	ViewExtent.NormalizeRect();
	//dc.SetMapMode(MM_TEXT);
	//ReleaseDC(&dc);
	return ViewExtent;
}

void CDrawProgView::ResetScrollSizes() {
	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	INXSize vwSize = viewSize;
	aDC.LPtoDP((LPPOINT)vwSize);
	SetScrollSizes(MM_TEXT, vwSize);
}

/*  Called after the user has dragged-and-dropped a component from 
the component palette onto the view, to do the data processing / adding
to the project etc. */
ConData* CDrawProgView::processComponentDrop(
			INXPoint point, INXString &csIconType, INXString &csBlock )
{
	INXString csBlockName = "";// This is only set if the icon as a 
	ConData* blob = NULL;
	CPortLabelDialog portDlg(pDEP);
	INXString parentName;
	int dataType = -1, portType = -1;
	
	// block is defined if it is an encapsulated icon
	// get block name for new blocks
	if (csBlock == "NewComponent") {
		CBlockNameDialog blockDlg;

		// SDG: This is a fudge for now. Needs fixing properly.
		// Makes User Components appear at top of tree in component bar
		csBlock = "aaaNewComponent";
		if (blockDlg.DoModal() == IDOK) {
			csBlockName = blockDlg.m_BlockName;
		}
		else {
			return NULL;
		}
	}
	else if((csBlock == "State Machine Group")||(csBlock == "StateMachineGroup")||( csBlock == "Wizard"))
	{
		if(CLucidStateMachine::placeComponents(point,pDEP,pProject))
		{
			SaveUndo();
			RedrawWindow();	
		}
		return NULL;
	}

	// if the icon is an xport add the port to the block in the parent view if it exists
	if (csIconType.Find("XINPUT") != -1 || csIconType.Find("XOUTPUT") != -1 || csIconType == "XSTART" || 
			csIconType == "XFINISH") {

		// only allow xports to be added to subsytems
		if (pFrame->m_wndProjectBar.m_cProjTree.IsSubsytem(pProject, pDEP)) {
			if (portDlg.DoModal() == IDOK) {
				return pFrame->m_wndProjectBar.m_cProjTree.AddXPort(csIconType, portDlg.m_PortLabel.MakeLower(), point, pProject, pDEP);
				//((CDrawProgApp*)AfxGetApp())->AddBlockPort(parentName, pDEP->depFilename, dataType, portType, portDlg.m_PortLabel);
				// don't run rest of function as DeleteBlockPort runs DeleteIcon
				// this should return the xport added, not NULL
			}
			return NULL;
		} else {
			AfxMessageBox("Can only add External Ports to Subsytems");
			return NULL;
		}
	}

	INXPOSITION selected = pDEP->AddIcon(csIconType, csBlock, point);  //create memory and set point
	blob = (ConData *) pDEP->condata->GetAt(selected);

	// the class name for a new block is the block name
	if (csBlockName != "") {
		blob->className = csBlockName;
		// create an instance name for this new block
		//CreateDirectory(workDir + USERDEFDIR + blockName, NULL);
		pDEP->CreateInstance(blob, 4); // THIS Will delete IDF
	}
	// create an instance for library components
	if (csBlock != "" && csBlockName == "") {
		//system("copy " + (INXString)workDir + USERDEFDIR + block + ".prg " + projectDir + DEPDIR + block + ".prg");
		pDEP->CreateInstance(blob, 1);
	}
	if((blob->className == "TRANSITIONACTION")||(blob->className  == "ENTRYACTION"))
	{

			CBlockNameDialog blockDlg;

			if (blockDlg.DoModal() == IDOK) {
				blob->description = blockDlg.m_BlockName;

				CLucidStateMachine::AddActionPortsBelow(blob,pDEP,pProject);
				return blob;

			}
			else {
				return NULL;
			}

			/*				CLucidStateMachine::AddActionPortsBelow(blob,pDEP,pProject);
			return blob;
*/

	}
	if(blob->className  == "CONDITIONLOGIC")	
	{
		CBlockNameDialog blockDlg;

		if (blockDlg.DoModal() == IDOK) {
			blob->description = blockDlg.m_BlockName;

			CLucidStateMachine::AddConditionLogicPortsBelow(blob,pDEP,pProject);
			return blob;

		}
		else {
			return NULL;
		}

	}
	return blob;
}

// removes highlighting from icons
void CDrawProgView::RemoveHighlight()
{
	INXPOSITION pos;
	ConData* blob;

	pos = pDEP->condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (pDEP->condata->GetNext(pos));
		blob->selected = 0;
	}
	RedrawWindow();
}

// save DEP and project file so can perform an undo
// project file needs to be undone since it contains a list of all the gui widgets.

void CDrawProgView::SaveUndo() {
	char szUndoFileNum[MAX_UNDO_FILE_NUM];
	
	undoFlag = TRUE;
	redoFlag = FALSE;

	_itoa_s(iUndoFileNum, szUndoFileNum, 10);

	pDEP->SaveProg(workDir + TEMPDIR + "undo" + szUndoFileNum + ".prg");
	pProject->pProjMData->writeProjectFileAs(workDir + TEMPDIR + "lpj_undo" + szUndoFileNum);

	if (iUndoFileNum >= (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1) {
		iUndoFileNum = 0;
		wrapFlag = TRUE;
	}
	else {
		iUndoFileNum++;
	}
	// Need to move the file num at which the undo flag becomes false
	// When file num wraps around need to save the filenum at which the undo function
	// becomes disabled should the user do enough undos
	if (wrapFlag && iUndoFileNum==iUndoFlagNum) {
		if (iUndoFlagNum >= (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1) {
			iUndoFlagNum = 0;
		}
		else {
			iUndoFlagNum++;
		}
	}
	maxUndoFileNum = iUndoFileNum;
}

// Saves the hierarchy name for each encapsulated block.These are used by flattening process when
// writing SODL
// This function is required because cannot use the function in myTreeCtrl for the top level as
// it saves group data
void CDrawProgView::SaveHierName() {
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM childItem;
	INXString csProjectDir;

	pProject->pProjMData->getProjectDir(csProjectDir);
	pos = pDEP->condata->GetHeadPosition();
	while (pos) {
		blob = (ConData*) pDEP->condata->GetNext(pos);
		if (blob->m_iUserDefined) {
			blob->hierarchyName = pDEP->depFilename + "\\";
			childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, pDEP->hItem);
			pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(childItem, csProjectDir, pProject);
		}
	}
	pDEP->SaveProg(csProjectDir + DEPDIR + pDEP->depFilename + ".prg");
}

/* 
This is called when a user clicks a port to start making a connection.
It is called after the icon and selected port has been identified with ::OnPort(..)
It stores a copy of this information for when the other end of the line has been selected
  */
void CDrawProgView::StartLine(INXPOSITION selectedIcon, int PortSelected, int portType) {
	ConData* icon;

	icon = (ConData*)(pDEP->condata->GetAt(selectedIcon));
	selectedControl = selectedIcon;
	selectedPort = PortSelected;
	selectedPortType = portType;
	INXPoint conpoint=icon->GetPortPoint(PortSelected, portType);
	conpoint.x = (conpoint.x * scale)/100;
	conpoint.y = (conpoint.y * scale)/100;
	
	// set some flags identifying the user's activity
	lastlinepoint = conpoint;
	lastmovingline = conpoint;
	pDEP->depFSM.drawingline = 1;
}

// returns true if library component imported already exists in userdefined directory
bool CDrawProgView::LibExist(INXString libName) {
	CFileFind finder;
	INXString fileName, compName;

	int bWorking = finder.FindFile(workDir + USERDEFDIR + "*.prg");	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == libName) {
			return TRUE;
		}
	}

	return FALSE;
}

// Sets the unique icon id to be greater than the largest id in the current view
void CDrawProgView::SetIconID() {
	ConData* blob;
	INXPOSITION pos;
	long id = 0;

	pos = pDEP->condata->GetHeadPosition();
	while (pos) {
		blob = (ConData*) pDEP->condata->GetNext(pos);
		if (blob->identnum > id) {
			id = blob->identnum;
		}
	}
	id++;
	ConData::uniqueidgenerator = id;
}

// returns true if an xport label in list already exists in the DEP
bool CDrawProgView::XportNameExist(INXObjList* list) {
	INXPOSITION depPos, listPos;
	ConData *depBlob, *listBlob;

	listPos = list->GetHeadPosition();
	while (listPos) {
		listBlob = (ConData*)(list->GetNext(listPos));
		if (listBlob->m_csIconType.Find("XINPUT") != -1 || listBlob->m_csIconType.Find("XOUTPUT") != -1 || 
			listBlob->m_csIconType == "XSTART" || listBlob->m_csIconType == "XFINISH") {
			depPos = pDEP->condata->GetHeadPosition();
			while (depPos) {
				depBlob = (ConData*)(pDEP->condata->GetNext(depPos));
				if (depBlob->m_csIconType == listBlob->m_csIconType && depBlob->description == listBlob->description) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
void CDrawProgView::SubsetEncapsulate() 
{

		ConData* encapsulatedIcon;

		if (selectRect.IsRectEmpty()) {
			AfxMessageBox("First select the icons you wish to encapsulate.");
			return;
		}
		if (pDEP->IsRectEmpty(selectRect)) {
			if (AfxMessageBox("You haven't selected any icons. Do you want to create an empty subsytem?",MB_YESNO|MB_ICONEXCLAMATION)==IDNO) {
				return;
			}
		}

		selectRect.NormalizeRect();
		encapsulatedIcon = pDEP->SubsetEncapsulate(selectRect, lib);
		if (encapsulatedIcon) {
			if (encapsulatedIcon->m_iUserDefined) {
				// add this instance to the project tree
				pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(encapsulatedIcon, pProject, pDEP->hItem);
			}
			
			// only update function block bar if encapsualting a library component
			if (lib) {
				pFrame->updateFunctionBlockBar();
			}

			SaveUndo();
		}
		else {
			RemoveHighlight();
		}
		selectRect.SetRectEmpty();
		pProject->SaveProjectDep();
		RedrawWindow();
}

// Method that sets the text in the status bar
void CDrawProgView::setStatusBarText(INXString csText)
{
	CChildFrame* viewFrame = (CChildFrame*)GetParentFrame();
	viewFrame->m_StatusBar.GetStatusBarCtrl().SetText(csText, 1, 0);
}

// Method that toggles lines when define monitoring is turned on
void CDrawProgView::toggleMonitoredLine(ConData *pBlob)
{
	TypeConversion tc;
	INXString csRtaFilePath = (INXString)"localhost.Generic-Debugger:" + workDir + TRACEDIR + TRACE_CONFIG_FILE;

	if (selectedPortType == INPUTPORT) {
		//pBlob->inputport[selectedPort]->line.toggleDbgMonitor();
		pDEP->toggleMonitoredLine(INPUTPORT, pBlob->inputport[selectedPort]->line.othericonid, pBlob->inputport[selectedPort]->line.otherportno);
		if (pProject->debug.debugMode != DBGSTOP) {
			pProject->debug.sendMonitor(pBlob->inputport[selectedPort]->line.getDbgMonitor(),
			pBlob->inputport[selectedPort]->line.idNum, tc.DataType2Char(pBlob->inputport[selectedPort]->datatype));
			if (pProject->debug.m_RtaTraceSupport.getRtaTrace()) {
				pProject->debug.m_RtaTraceSupport.terminateRtaTraceProc();
				pProject->debug.m_RtaTraceSupport.createRtaConfigFile();
				ShellExecute(NULL, "open", RTA_TRACE_PATH, csRtaFilePath, NULL, SW_SHOWNORMAL);
			}
		}
	}
	else if (selectedPortType == STARTPORT) {
		//pBlob->startport[selectedPort]->line.toggleDbgMonitor();
		pDEP->toggleMonitoredLine(STARTPORT, pBlob->startport[selectedPort]->line.othericonid, pBlob->startport[selectedPort]->line.otherportno);
		if (pProject->debug.debugMode != DBGSTOP) {
			pProject->debug.sendMonitor(pBlob->startport[selectedPort]->line.getDbgMonitor(),
			pBlob->startport[selectedPort]->line.idNum, "E");
			if (pProject->debug.m_RtaTraceSupport.getRtaTrace()) {
				pProject->debug.m_RtaTraceSupport.terminateRtaTraceProc();
				pProject->debug.m_RtaTraceSupport.createRtaConfigFile();
				ShellExecute(NULL, "open", RTA_TRACE_PATH, csRtaFilePath, NULL, SW_SHOWNORMAL);
			}
		}
	}
}

void CDrawProgView::startTraceTimer()
{
	SetTimer(TRACE_TIMER_ID, TRACE_PERIOD, 0);
}

void CDrawProgView::stopTraceTimer()
{
	KillTimer(TRACE_TIMER_ID);
}

/* 
*/
void CDrawProgView::OnLButtonDown(UINT nFlags, CPoint _point) 
{	// @todo
	INXPoint point(_point.x, _point.y);
	// kwhite:canvassupport
	// case 1: ?											
	// case 2: capture selection of function or encap block
	// case 3: capture line selection						
	// case 4: rubber band for encapsulation block			
	// case 5: creation of rubber band						
	// case 6: Multiple objects selected with rubberband

	running=1;
	int portConnected=0;
	int portType=0;
	pDoc->SetModifiedFlag();
	//point+=GetScrollPosition( ) ;
	INXPOSITION selectedIcon;
	int PortSelected;
	ConData* blob;
	TypeConversion tc;

	//point.x = (point.x * 100)/scale;
	//point.y = (point.y * 100)/scale;
	CClientDC aDC(this); // create device context
	OnPrepareDC(&aDC); // adjust origin
	aDC.DPtoLP((LPPOINT)point); // convert point to Logical
	if (!selectRect.PtInRect(point)) {
		selectRect.SetRectEmpty();
	}

	if (pDEP->depFSM.drawingline!=1){
		//LineSelected=online(point); // todo
		PortSelected = pDEP->OnConnect(point,&selectedIcon,&portType,&portConnected); // return -ve if an already connected input for deletion

		// if within selection box
		if (selectRect.PtInRect(point)) {
			m_iLftBtnDownState = 6;
			oldpoint = SnapToGrid(point); 
			firstpoint = SnapToGrid(point);		
		}
		else if ((PortSelected>-1)) {
			RemoveHighlight();
			StartLine(selectedIcon,PortSelected,portType);
		}
		else { // if not a port then check on a line
			RemoveHighlight();
			// if the point is on a line then return the port number the line is connected to
			bool INTERROGATE_ONLY = false;
			// when defining monitors don't add nodes
			if (pProject->getDefineMonitors()) {
				INTERROGATE_ONLY = true;
			}
			
			selectedPort = pDEP->OnLine(point, &nodeIcon, &nodeSelected, &selectedPortType, INTERROGATE_ONLY);
			// if on a node
			if (nodeSelected) {
				blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
				if (pProject->getDefineMonitors()) {
					toggleMonitoredLine(blob);
				}
				else {
					if (selectedPortType == INPUTPORT) {
						blob->inputport[selectedPort]->line.editNode(point);
					}
					else if (selectedPortType == STARTPORT) {
						blob->startport[selectedPort]->line.editNode(point);
					}
				}
				m_iLftBtnDownState = 3;
			}
			// if on a line
			else if (selectedPort > -1) {
				/* @todo PP: Create the variables for this function
				blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
				// Record Position for move
				m_iLftBtnDownState=XXX;
				oldpoint = pDEP->SnapToGrid(point); 
				firstpoint = pDEP->SnapToGrid(point);
				SetToMovingLine(selectedLine);
				moveselectedLine = selectedLine;
				*/
				// remove the following when above is done...
				blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
				if (pProject->getDefineMonitors()) {
					toggleMonitoredLine(blob);
				}
				else {
					// SDG: If left click on a line drag the segment for Gila.
					// Prior to Gila a node was added
					m_iLftBtnDownState = 8;  // set the mode that a line segment is being moved
				}
			}
			else { // if not a line/node then check on an icon
				selectedControl = pDEP->Incontrol(point);
				if (selectedControl) {
					pDEP->setFBHighlight(selectedControl);
					m_iLftBtnDownState=2;
					oldpoint = SnapToGrid(point); 
					firstpoint = SnapToGrid(point);
					SetToMovingRectangle(selectedControl);
					moveselected = selectedControl;
				}
				// if on background allow user to select several icons
				else {
					m_iLftBtnDownState = 5;
					selectRect.TopLeft() = point;
					selectRect.BottomRight() = point;
					//removeHighlight();
				}
			}
		}
	}	
	//kwhite:canvassupport
	//cs.SetDebugTrace(true);
	if (cs.ValidLeftButtonEvent(m_iLftBtnDownState) || pDEP->depFSM.drawingline || m_iLftBtnDownState==5)
	{
		SaveUndo();					//must come first.
		pProject->SaveProjectDep();

		undoFlag = TRUE;
		SetCapture();				//prevent mouse events disapearing if pointer past boundary	
		if(m_iLftBtnDownState == 2)			//Single item selected
		{
			cs.SetTempStore(point);	//Needed to remember first point before mouse move.
									//Used by MouseUp when calling ConData::RenewPosition(..,oldpoint)
			INXPOSITION hideSelected;
			hideSelected = pDEP->Incontrol(point);
			pDEP->setFBHighlight(hideSelected);
			pDEP->HideSelected(hideSelected);
		}
		if(m_iLftBtnDownState == 6)			//Multiple items selected
		{
			cs.SetTempStore(point);	//Needed to remember first point before mouse move.
									//Used by MouseUp when calling ConData::RenewPosition(..,oldpoint)
			pDEP->HideSelectedPositions();
		}
		cs.DebugTrace("kwhite:canvassupport:OnLButtonDown:SetCapture,SaveUndo,HideControl\n");		
	}
	CScrollView::OnLButtonDown(nFlags, (CPoint)point);
	running=0;
}

/*
	Get the event of the user releasing the mouse button which may mean any of the following, 
	depending on the user's previous activity. . e.g. finished drawing a line
*/
void CDrawProgView::OnLButtonUp(UINT nFlags, CPoint _point) 
{
	INXPoint point(_point.x, _point.y);
	running=1;
	//point+=GetScrollPosition( ) ;
	int selectedPort2;
	INXPOSITION selectedControl2;
	int selectedPortType2 = 0;
	int selectedPortConnected2 = 0;
	int iSelSegNum = -1;
	ConData* pBlob;

	//point.x = (point.x * 100)/scale;
	//point.y = (point.y * 100)/scale;
	CClientDC aDC(this); // create device context
	OnPrepareDC(&aDC); // adjust origin
	aDC.DPtoLP((LPPOINT)point); // convert point to Logical

	switch (m_iLftBtnDownState) {

	case 1:

		break;

	case 2:// this moves an icon around if not drawing a line - 
		{
		cs.SetDebugTrace(false);
		if (pDEP->depFSM.drawingline) break;
		if(cs.TopLeftBoundaryHit())
		{
			cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:cs.TopLeftBoundaryHit\n");
			pDEP->RenewPosition(moveselected, SnapToGrid(point), SnapToGrid(cs.GetTempStore()));
			cs.SetTopLeftBoundaryHit(false);
		}
		else
		{
			cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:!cs.TopLeftBoundaryHit()\n");
			pDEP->RenewPosition(moveselected, SnapToGrid(point), SnapToGrid(firstpoint));
		}
		pDEP->SnapIconToGrid(moveselected);
		pDEP->ShowSelected(moveselected); //Im relying on moveselected 
		m_iLftBtnDownState=0;
		SaveUndo();
		undoFlag = TRUE; //kwhite:canvassupport added
		copyFlag = TRUE;
		break;
		}
	case 3:
		SaveUndo();
		break;		
	case 4:
		{
			// This used to be encapsulation
		}
		break;
	case 5:
		{
			selectRect.NormalizeRect();
			// if there is at least 1 icon slected then display cut and copy buttons and menu items
			if (pDEP->HighlightSelected(selectRect)) {
				copyFlag = TRUE;
			}
			else {
				copyFlag = FALSE;
			}
		}
		break;
	case 6:
		{
			if(cs.TopLeftBoundaryHit())
			{
				cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:if MoveObjects() Y %d\n", SnapToGrid(point).y, cs.GetTempStore().y);
				pDEP->RenewSelectedPos(SnapToGrid(point), SnapToGrid(cs.GetTempStore()), selectRect);				
				selectRect = (selectRect)+(oldpoint-firstpoint);
				cs.SetTopLeftBoundaryHit(false);
			}
			else
			{
				cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:            Y %d\n", SnapToGrid(point).y, firstpoint.y);
				selectRect = pDEP->RenewSelectedPos(SnapToGrid(point), SnapToGrid(firstpoint), selectRect);					
			}
			pDEP->ShowSelectedPositions();	
			SaveUndo();
			undoFlag = TRUE; //kwhite:canvassupport added
		}
		break;
	case 7:// this moves an Line around if not drawing a line - 
		{
		cs.SetDebugTrace(false);
		if (pDEP->depFSM.drawingline) break;
		if(cs.TopLeftBoundaryHit())
		{
			cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:cs.TopLeftBoundaryHit\n");
			pDEP->RenewPosition(moveselected, SnapToGrid(point), cs.GetTempStore());
			cs.SetTopLeftBoundaryHit(false);
		}
		else
		{
			cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:!cs.TopLeftBoundaryHit()\n");
			pDEP->RenewPosition(moveselected, SnapToGrid(point), firstpoint);
		}
		/* @todo PP: Create this function for moving a line
		pDEP->ShowSelected(lineselected); //Im relying on moveselected 
		*/
		m_iLftBtnDownState=0;
		SaveUndo();
		undoFlag = TRUE; //kwhite:canvassupport added
		copyFlag = TRUE;
		break;
		}
	case 8: // line segements
		{
		//TO DO: check if segments are aligned. If they are delete node.
		pBlob = (ConData*) pDEP->condata->GetAt(nodeIcon);
		if (selectedPortType == INPUTPORT) {
			iSelSegNum = pBlob->inputport[selectedPort]->line.GetSelectedSegment();
			if (iSelSegNum >= 0) {
				if (iSelSegNum+2 <= pBlob->inputport[selectedPort]->line.points.GetUpperBound()) {
					if (pBlob->inputport[selectedPort]->line.IsSegmentsAligned(iSelSegNum, iSelSegNum+2)) {
						pBlob->inputport[selectedPort]->line.DeleteNode(iSelSegNum+1);
						pBlob->inputport[selectedPort]->line.DeleteNode(iSelSegNum+1);
					}
				}
				if (iSelSegNum >= 2) {
					if (pBlob->inputport[selectedPort]->line.IsSegmentsAligned(iSelSegNum, iSelSegNum-2)) {
						pBlob->inputport[selectedPort]->line.DeleteNode(iSelSegNum-1);
						pBlob->inputport[selectedPort]->line.DeleteNode(iSelSegNum-1);
					}
				}
			}
		}
		else if (selectedPortType == STARTPORT) {
			iSelSegNum = pBlob->startport[selectedPort]->line.GetSelectedSegment();
			if (iSelSegNum >= 0) {
				if (iSelSegNum+2 <= pBlob->startport[selectedPort]->line.points.GetUpperBound()) {
					if (pBlob->startport[selectedPort]->line.IsSegmentsAligned(iSelSegNum, iSelSegNum+2)) {
						pBlob->startport[selectedPort]->line.DeleteNode(iSelSegNum+1);
						pBlob->startport[selectedPort]->line.DeleteNode(iSelSegNum+1);
					}
				}
				if (iSelSegNum >= 2) {
					if (pBlob->startport[selectedPort]->line.IsSegmentsAligned(iSelSegNum, iSelSegNum-2)) {
						pBlob->startport[selectedPort]->line.DeleteNode(iSelSegNum-1);
						pBlob->startport[selectedPort]->line.DeleteNode(iSelSegNum-1);
					}
				}
			}		}
		
		SaveUndo();
		break;	
		}
	}

	if (pDEP->depFSM.drawingline) { //if drawing a line find the other end and test if it is another port
		CClientDC dc(this);
		// to change the colour for a drawn line the pen needs to be set in IconLines::Draw
		dc.SelectObject(blackpen);
		//dc.MoveTo(lastlinepoint+GetScrollPosition( ) );
		//dc.LineTo(lastmovingline+GetScrollPosition( ) );
		selectedPort2 = pDEP->OnConnect(point,&selectedControl2,&selectedPortType2,&selectedPortConnected2);
		if (selectedPort2 > -1) {
			// check if connection dragging or drawing
			// connection dragging
			if (selectedPortType == selectedPortType2) {
				pDEP->dragConnections(selectedControl, selectedPort, selectedPortType, selectedControl2, selectedPort2, selectedPortType2);
				SaveUndo();
			}
			// connection drawing
			else {
				// Only add nodes and save undo file, if the line was added successfully
				if (pDEP->AddLine(selectedControl, selectedPort, selectedPortType, selectedControl2, selectedPort2, selectedPortType2)) {
					pDEP->AddNodes(selectedControl, selectedPort, selectedPortType, selectedControl2, selectedPort2, selectedPortType2);
					SaveUndo();
				}
			}
		}
		pDEP->depFSM.drawingline = 0;
		ReleaseDC(&dc);
	}
	
	if (this == GetCapture())	//kwhite:canvassupport - prevent mouse events disapearing if pointer past boundary
	{
		cs.DebugTrace("kwhite:canvassupport:OnLButtonUp:ReleaseCapture,SetMoveObjects(false)\n");
		ReleaseCapture();		//release the mouse capture otherwise other apps wont work
	}

	CScrollView::OnLButtonUp(nFlags, point);
	m_iLftBtnDownState=0;		
	running = 0;
	pProject->SaveProjectDep();
	RedrawWindow();
}

/*
	When the right button is clicked on an icon then bring up a popup menu
*/
void CDrawProgView::OnRButtonDown(UINT nFlags, CPoint _point) 
{
	INXPoint point(_point.x, _point.y);
	CMenu iconMenu;
	ConData* blob;
	int portConnected=0;
	INXPoint menuPoint = point;
	//point.x = (point.x * 100)/scale;
	//point.y = (point.y * 100)/scale;
	CClientDC aDC(this); // create device context
	OnPrepareDC(&aDC); // adjust origin
	aDC.DPtoLP((LPPOINT)point); // convert point to Logical
	m_cpRButtonDown = point;
	
	ClientToScreen((LPPOINT)menuPoint);	// Convert to screen coordinates
	RemoveHighlight();
	// TODO: Add your message handler code here and/or call default
	pDoc->SetModifiedFlag();
	//point+=GetScrollPosition( ) ;
	running=1;
	selectRect.SetRectEmpty();

	selectedPort = pDEP->OnConnect(point,&selectedControl,&selectedPortType,&portConnected);
	// load the port menu if an unconnected start port is selected
	if (selectedPort>-1) {
		blob = (ConData*) pDEP->condata->GetAt(selectedControl);
		/*@todo MIGRATION_ISSUES // iconMenu.LoadMenu
		iconMenu.LoadMenu(IDR_ICON_MENU); // Load the icon menu
		*/
		// Check initialise item
		if (selectedPortType == STARTPORT && !blob->startport[selectedPort]->line.exist) {
			iconMenu.GetSubMenu(7)->CheckMenuItem(ID_PORT_INITIALISE,(blob->startport[selectedPort]->initialise==1?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
			iconMenu.GetSubMenu(8)->CheckMenuItem(ID_PORT_INITIALISE,(blob->startport[selectedPort]->initialise==1?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
		}

		// Enable Tag menu items appropriately
		switch (selectedPortType) {
			case STARTPORT: 
			{
				iconMenu.GetSubMenu(7)->EnableMenuItem(ID_ADD_TAG, blob->startport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(7)->EnableMenuItem(ID_DELETE_TAG, blob->startport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(7)->EnableMenuItem(ID_SHOW_LINE, !(blob->startport[selectedPort]->tag != "" &&
					blob->startport[selectedPort]->line.exist));
				iconMenu.GetSubMenu(8)->EnableMenuItem(ID_ADD_TAG, blob->startport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(8)->EnableMenuItem(ID_DELETE_TAG, blob->startport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(8)->EnableMenuItem(ID_SHOW_LINE, !(blob->startport[selectedPort]->tag != "" &&
					blob->startport[selectedPort]->line.exist));
				break;
			}
			case INPUTPORT:
			{
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_ADD_TAG, blob->inputport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_DELETE_TAG, blob->inputport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_SHOW_LINE, !(blob->inputport[selectedPort]->tag != "" &&
					blob->inputport[selectedPort]->line.exist));
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_ADD_TAG, blob->inputport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_DELETE_TAG, blob->inputport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_SHOW_LINE, !(blob->inputport[selectedPort]->tag != "" &&
					blob->inputport[selectedPort]->line.exist));
				break;
			}
			case OUTPUTPORT:
			{
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_ADD_TAG, blob->outputport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_DELETE_TAG, blob->outputport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_SHOW_LINE, !(blob->outputport[selectedPort]->tag != "" &&
					pDEP->IsOutputPortConnected(blob, selectedPort)));
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_ADD_TAG, blob->outputport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_DELETE_TAG, blob->outputport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_SHOW_LINE, !(blob->outputport[selectedPort]->tag != "" &&
					pDEP->IsOutputPortConnected(blob, selectedPort)));
				break;
			}
			case FINISHPORT:
			{
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_ADD_TAG, blob->finishport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_DELETE_TAG, blob->finishport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(1)->EnableMenuItem(ID_SHOW_LINE, !(blob->finishport[selectedPort]->tag != "" &&
					pDEP->IsFinishPortConnected(blob, selectedPort)));
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_ADD_TAG, blob->finishport[selectedPort]->tag != "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_DELETE_TAG, blob->finishport[selectedPort]->tag == "");
				iconMenu.GetSubMenu(6)->EnableMenuItem(ID_SHOW_LINE, !(blob->finishport[selectedPort]->tag != "" &&
					pDEP->IsFinishPortConnected(blob, selectedPort)));
				break;
			}			
		}


		// display port menu pop-up
		if (blob->m_iUserDefined && selectedPortType != STARTPORT) {
			iconMenu.GetSubMenu(6)->EnableMenuItem(ID_ADD_PORT_TO_GROUP, 1);
			iconMenu.GetSubMenu(6)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		}
		else if (!blob->m_iUserDefined && selectedPortType == STARTPORT) {
			iconMenu.GetSubMenu(7)->EnableMenuItem(ID_ADD_PORT_TO_GROUP, 1);
			iconMenu.GetSubMenu(7)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		}
		else if (blob->m_iUserDefined && selectedPortType == STARTPORT) {
			iconMenu.GetSubMenu(8)->EnableMenuItem(ID_ADD_PORT_TO_GROUP, 1);
			iconMenu.GetSubMenu(8)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		}
		else {
			iconMenu.GetSubMenu(1)->EnableMenuItem(ID_ADD_PORT_TO_GROUP, 1);
			iconMenu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		}
	}
	else {

		const bool INTERROGATE_ONLY = true;
		selectedPort = pDEP->OnLine(point, &nodeIcon, &nodeSelected, &selectedPortType, INTERROGATE_ONLY );
		selectedControl = pDEP->Incontrol(point);
		if (nodeSelected) {
			/*@todo MIGRATION_ISSUES // iconMenu.LoadMenu
			iconMenu.LoadMenu(IDR_ICON_MENU); // Load the icon menu
	*/
			// display node menu pop-up
			iconMenu.GetSubMenu(3)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		}
		else if (selectedPort > -1) {
				/*@todo MIGRATION_ISSUES // iconMenu.LoadMenu
			iconMenu.LoadMenu(IDR_ICON_MENU); // Load the icon menu
			*/
			// display line menu pop-up
			iconMenu.GetSubMenu(2)->EnableMenuItem(ID_DEBUG_FORCE, 1);
			iconMenu.GetSubMenu(2)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
		} else if (selectedControl) {
			// highlight icon
			pDEP->setFBHighlight(selectedControl);
			RedrawWindow();

			blob = (ConData *) pDEP->condata->GetAt(selectedControl);
		/*@todo MIGRATION_ISSUES  //	iconMenu.LoadMenu
			iconMenu.LoadMenu(IDR_ICON_MENU); // Load the icon menu
			*/
			// Check show description item
			iconMenu.CheckMenuItem(ID_ICON_SHOW_DESC,(blob->showdescript==1?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
			// display icon menu pop-up
			if (blob->m_iUserDefined) {
				iconMenu.GetSubMenu(4)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
			}
			else if (blob->m_csIconType.Find("XINPUT") != -1 || blob->m_csIconType.Find("XOUTPUT") != -1 || 
				blob->m_csIconType == "XSTART" || blob->m_csIconType == "XFINISH") {
				iconMenu.GetSubMenu(5)->EnableMenuItem(ID_ADD_ICON_TO_GROUP, 1);
				iconMenu.GetSubMenu(5)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
			}
			else {
				iconMenu.GetSubMenu(5)->EnableMenuItem(ID_ADD_ICON_TO_GROUP, 1);
				iconMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, menuPoint.x, menuPoint.y, this);
			}

		} else {

			// Superseded by the dockable, semi-permanent ConInfoBar, which allows picking of
			// components.
			/*
			point = pDEP->SnapToGrid(point);
			readProjectFile();

			//This is where the component-palette comes up.
			blob = SelectIcon(point);
			//blob = SelectIcon(menuPoint);
			if (blob) {
				if (blob->userDefined) {
					// add this instance to the project tree
					pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(blob, pProject, pDEP->hItem);
					// Make sure gui widget tags are unique
					// add any gui widgets to the project meta data
					HTREEITEM hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, pDEP->hItem);
					pProject->uniquifyWidgetsInUserDefBlock(hUserDefItem);
				}
				// Add Gui widgets to project file
				else if (blob->isGuiWidget()) {
					pProject->addGuiWidget(blob);
				}
				// if adding xport then don't allow undo
				if (!(blob->m_csIconType.Find("XINPUT") != -1 || blob->m_csIconType.Find("XOUTPUT") != -1 || blob->m_csIconType == "XSTART" 
					|| blob->m_csIconType == "XFINISH")) {
					SaveUndo();
				}
				pProject->SaveProject();
				RedrawWindow();
			}
			*/

		}
	}
	
	CScrollView::OnRButtonDown(nFlags, point);
	running=0;
}


/* 
Used to draw the trace line when new lines are beig drawn, or icons are being moved
  */
void CDrawProgView::OnMouseMove(UINT nFlags, CPoint _point) 
{
	INXPoint point(_point.x, _point.y);
	// kwhite:canvassupport
	// case 1: ?											
	// case 2: capture selection of function or encap block
	// case 3: capture line selection						
	// case 4: rubber band for encapsulation block			
	// case 5: creation of rubber band						
	// case 6: Multiple objects selected with rubberband
	// case 8: Dragging line segments
	// or Drawing line
	
	CScrollView::OnMouseMove(nFlags, (INXPoint)point);

	char buffx[200];
	sprintf_s(buffx, 200, "%d= m_iLftBtnDownState\n", m_iLftBtnDownState );
	TRACE( buffx );

	if(pDEP->depFSM.drawingline)
		sprintf_s(buffx, 200, "pDEP->depFSM.drawingline==true\n" );
	else
		sprintf_s(buffx, 200, "pDEP->depFSM.drawingline==false\n" );

	TRACE( buffx );

	if (!running) {

	INXPoint linePoint = point;
	INXSize topleftpnt(GetScrollPosition( ).x, GetScrollPosition( ).y);

	linePoint+=topleftpnt ;
	Sleep(20);
	running=1;
	CClientDC dc(this); // create device context
	OnPrepareDC(&dc); // adjust origin
	dc.DPtoLP((LPPOINT)point); // convert point to Logical



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// When the user is not 'doing anything' ie not drawing a line, dragging a connection etc
	// adjust cursor according to what it is over

	if( m_iLftBtnDownState == 0 && !pDEP->depFSM.drawingline ){ 

		//if (pDEP->depFSM.drawingline)
		INXPoint pnt = point;

		// Over a port?
		int iTmpPortConnected=0;
		int iTmpPortType=0;
		INXPOSITION cTmpSelectedThing;

		int iTmpPortSelected = 0;

		iTmpPortSelected = pDEP->OnConnect(pnt,&cTmpSelectedThing,&iTmpPortType,&iTmpPortConnected);

		if (iTmpPortSelected >= 0) {
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABPORT) );
		} else {
			cTmpSelectedThing = pDEP->Incontrol(pnt);

			bool bTmpNodeSelected;
			int iTmpSelectedPortType;

			const bool INTERROGATE_ONLY = true;
			iTmpPortSelected = pDEP->OnLine(pnt, &cTmpSelectedThing, &bTmpNodeSelected, &iTmpSelectedPortType, INTERROGATE_ONLY);

			// Over a line?
			if(iTmpPortSelected >= 0) {
				if (bTmpNodeSelected) {
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABNODE) );
				} else {
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABLINE) );
				}
			//Over a f-block?
			} else if (cTmpSelectedThing) {
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABBLOCK) );
			} else {
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW) );
			}
		}
	} else if (m_iLftBtnDownState == 0 && pDEP->depFSM.drawingline) { 
		INXPoint pnt = point;

		// Over a port?
		int iTmpPortConnected=0;
		int iTmpPortType=0;
		INXPOSITION cTmpSelectedThing;

		int iTmpPortSelected = 0;

		iTmpPortSelected = pDEP->OnConnect(pnt,&cTmpSelectedThing,&iTmpPortType,&iTmpPortConnected);

		if( iTmpPortSelected >=0 ){

			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABPORT) );

		} else{

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW) );

		}
	}

	switch (m_iLftBtnDownState) 
	{	// horrible code - sorry 
		// lbutton down describes what activity is being done 
		// 0 i think is nothing, 
		// 2 is moving an icon. drawing a line is seperately indicated for some reason in  'drawingline'
	case 0: 
	{

		if( !pDEP->depFSM.drawingline ){ 

			//if (pDEP->depFSM.drawingline)
			INXPoint pnt = point;

			// Over a port?
			int iTmpPortConnected = 0;
			int iTmpPortType = 0;
			INXPOSITION cTmpSelectedThing;

			int iTmpPortSelected = 0;

			iTmpPortSelected = pDEP->OnConnect(pnt,&cTmpSelectedThing,&iTmpPortType,&iTmpPortConnected);

			if ( iTmpPortSelected >= 0 ) {

				::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABPORT) );

			} else {
				cTmpSelectedThing = pDEP->Incontrol(pnt);

				bool bTmpNodeSelected;
				int iTmpSelectedPortType;

				const bool INTERROGATE_ONLY = true;
				iTmpPortSelected = pDEP->OnLine(pnt, &cTmpSelectedThing, &bTmpNodeSelected, &iTmpSelectedPortType, INTERROGATE_ONLY);

				// Over a line?
				if(iTmpPortSelected >= 0) {
					if (bTmpNodeSelected) {
						::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABNODE) );
					} else {
						::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABLINE) );
					}
				//Over a f-block?
				} else if (cTmpSelectedThing) {
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABBLOCK) );
				} else {
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW) );
				}
			}

		} 	else if( pDEP->depFSM.drawingline ) { 

			//if (pDEP->depFSM.drawingline)
			INXPoint pnt = point;

			// Over a port?
			int iTmpPortConnected=0;
			int iTmpPortType=0;
			INXPOSITION cTmpSelectedThing;

			int iTmpPortSelected = 0;

			iTmpPortSelected = pDEP->OnConnect(pnt,&cTmpSelectedThing,&iTmpPortType,&iTmpPortConnected);

			if( iTmpPortSelected >=0 ){

				::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABPORT) );

			} else{

				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW) );

			}
		}

		break;
	}
	case 2:
	{
		//CClientDC dc(this); //this is the client area only so compensate for scroll offset
		dc.SelectObject(nullpen);
		INXRect extra(2,2,2,2);INXPoint bacabit(1,1);
		INXRect dooby = (RectangleSite)+(oldpoint-firstpoint);
		dooby+=extra;
 		dc.Rectangle(dooby);
		InvalidateRect(dooby,FALSE);
		//oldpoint=point-bacabit;
		dc.SelectObject(dashkpen);
		dooby = RectangleSite+(point-firstpoint);
		dc.Rectangle(dooby);
		//ReleaseDC(&dc); //kwhite this is badly managed
		oldpoint = point;		
		if (cs.StretchCanvas(dooby, viewSize)) 
		{
			SetScrollSizes(MM_TEXT, viewSize);
			pDEP->setCanvasSize(viewSize);
		}
		SetPointToTestAgainst(dooby); //Needed for boundary check on item rather than mouse cursor
		break;
	}
	// update view with line node being dragged	
	case 3:
		{
		if (!pProject->getDefineMonitors()) {
			ConData* blob;

			blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
			// snap nodes to grid.
			point = SnapToGrid(point);
			if (selectedPortType == INPUTPORT) {
				blob->inputport[selectedPort]->line.editNode(point);
			}
			else if (selectedPortType == STARTPORT) {
				blob->startport[selectedPort]->line.editNode(point);
			}

			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABNODE) );

			RedrawWindow();
		}
		break; 
		}
	// update view with encapsulation box being dragged
	case 4:
		{
		encapsulate.BottomRight() = point;
		dc.Rectangle(encapsulate);
		RedrawWindow();
		break; 
		}
	// update view with selection box being dragged
	case 5:
		{
		selectRect.BottomRight() = point;
		//kwhite:canvassupport - remmed following, this is confusing it is drawn in OnDraw
		//kwhite:canvassupport - dc.Rectangle(selectRect); 
		RedrawWindow();
		break; 
		}
	case 6:
	{
		//CClientDC dc(this); //this is the client area only so compensate for scroll offset
		dc.SelectObject(nullpen);
		INXRect extra(2,2,2,2);
		INXRect dooby = (selectRect)+(oldpoint-firstpoint);
		dooby+=extra;
 		dc.Rectangle(dooby); 
		InvalidateRect(dooby,FALSE);
		dc.SelectObject(dashkpen);
		dooby = selectRect+(point-firstpoint);
		dc.Rectangle(dooby);
		//ReleaseDC(&dc); //kwhite this is badly managed
		oldpoint = point;
		if (cs.StretchCanvas(dooby, viewSize)) 
		{
			SetScrollSizes(MM_TEXT, viewSize);
			pDEP->setCanvasSize(viewSize);
		}
		SetPointToTestAgainst(dooby); //Needed for boundary check on items rather than mouse curser
		break;
	}
	// update view with line segement being dragged	
	case 8:
		{
		if (!pProject->getDefineMonitors()) {
			ConData* blob;

			blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
			// snap nodes to grid.
			point = SnapToGrid(point);
			if (selectedPortType == INPUTPORT) {
				blob->inputport[selectedPort]->line.MoveSegment(point);
			}
			else if (selectedPortType == STARTPORT) {
				blob->startport[selectedPort]->line.MoveSegment(point);
			}

			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_LABLINE) );

			RedrawWindow();
		}
		break; 
		}

	}
		// line drawing
		if (pDEP->depFSM.drawingline) {
			INXRect invalided;
			CClientDC dc(this);	//kwhite this is badly managed
			
			cs.DebugTrace("drawing line\n");
			dc.MoveTo(lastlinepoint-topleftpnt);
			dc.SelectObject(nullpen);
			dc.LineTo(lastmovingline-topleftpnt);
			dc.MoveTo(lastlinepoint-topleftpnt);
			if (lastlinepoint.x>lastmovingline.x) {
				invalided.TopLeft().x = lastmovingline.x;
				invalided.BottomRight().x = lastlinepoint.x;
			}
			else {
				invalided.BottomRight().x=lastmovingline.x;invalided.TopLeft().x=lastlinepoint.x;}
			if (lastlinepoint.y>lastmovingline.y) {
				invalided.TopLeft().y=lastmovingline.y;invalided.BottomRight().y=lastlinepoint.y;}
			else {
				invalided.BottomRight().y=lastmovingline.y;invalided.TopLeft().y=lastlinepoint.y;}	
			
			InvalidateRect(invalided,FALSE);
			// When drawing a line the colour is defined here
			dc.SelectObject(greenpen);
			dc.LineTo(linePoint-topleftpnt);
			lastmovingline=linePoint;
			ReleaseDC(&dc);
		}

	}	
	running=0;

	cs.SetDebugTrace(false);

	if (cs.ValidLeftButtonEvent(m_iLftBtnDownState) || pDEP->depFSM.drawingline || m_iLftBtnDownState==5)
	{
		//Mouse over point  -  needs to look at the logical area
		CClientDC dc(this); // create device context
		OnPrepareDC(&dc);   // adjust origin
		dc.LPtoDP((LPPOINT)point);  // convert point to Logical - confused but it works.
		
		// When line hits (0,0) don't shift rest of diagram, because there aren't any
		// icons above or to the left of this point
		if (pDEP->depFSM.drawingline || m_iLftBtnDownState==5) {
			if (cs.NearTopLeftBoundary(point)) {
				INXPoint ScrollPos = cs.AdjustPositionOfScrollUpwards();
				ScrollToPosition(ScrollPos);				
				RedrawWindow();
				Sleep(10);	
			}
		}
		else {
			if (cs.NearTopLeftBoundary(cs.GetPointToTestAgainst())) {
				INXPoint adjust;
				adjust = cs.AdjustPositionOfObjects(firstpoint);

				//Ensure items dont dissappear from the bottom/right of the screen
				cs.StretchCanvas(adjust - firstpoint, viewSize);
				SetScrollSizes(MM_TEXT, viewSize);
				pDEP->setCanvasSize(viewSize);
			
				//Move currently selected objects first, and externally connected lines
				pDEP->MoveObjectsWithinSelectedBox(adjust, firstpoint);
				pDEP->MoveExternallyConnectedLines( adjust, firstpoint);

				//Now we can move other objects, in essence we are moving everything
				pDEP->InvertSelected();									//flip selection
				pDEP->MoveObjectsWithinSelectedBox(adjust, firstpoint);
				pDEP->MoveExternallyConnectedLines( adjust, firstpoint);
				pDEP->InvertSelected();									//flip selection back		
		
				adjust = cs.AdjustPositionOfObjects(cs.GetTempStore()); //alter stored point to reflect scrolling effect
				cs.SetTempStore(adjust);
				cs.SetTopLeftBoundaryHit(true);
				RedrawWindow();	
				Sleep(10);	
			}
		}	//INXPoint 

		if (cs.NearBottomRightBoundary(point))
		{
			//INXString tmp;	
			//UINT nScrollCode = 0;
			INXPoint ScrollPosition = cs.AdjustPositionOfScrollDownwards();
			ScrollToPosition(ScrollPosition);				
			RedrawWindow();
			Sleep(10);				
		}
		ReleaseDC(&dc);
	}

}

void CDrawProgView::SetPointToTestAgainst(INXRect rect) 
{
	INXPoint temp;
	temp.x = rect.left;
	temp.y = rect.top;

	//must take account of scroll position
	INXSize topleft = GetScrollPosition( ); 
	temp.x = temp.x - topleft.cx;
	temp.y = temp.y - topleft.cy;

	//record in canvas object for boundary hit test
	cs.SetPointToTestAgainst(temp);
}

// this assigns the view to a document for file handling
void CDrawProgView::OnInitialUpdate() 
{
	// Set up the scrollbars
	ResetScrollSizes(); 
	INXRect clientRect;
	this->GetClientRect(clientRect);
	INXPoint point = pDEP->getInitScrollPos(clientRect);
	this->ScrollToPosition(point);
	cs.SetScrollPosition(point);

	CScrollView::OnInitialUpdate();

	//m_DT.Register(this);
	
	// TODO: Add your specialized code here and/or call the base class
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	onlyDrawAnimatedStuff = false;
//	otherDrawEvent = false;
	toggleAnimation = 0;

	// note - it is currently not possible to start IAB in debug mode, but this is future proofing
	if (pProject->debug.debugMode == DBGSTOP) {
		SetTimer(TIMER_IDS_DRAWANIMATEDSTUFF,TIMER_WAITMS_DRAWANIMATEDSTUFF,NULL); //fire a timer to animate certain parts of the DEP eg. unconnected mandatory ports
	}
	//todo - this blocks any other process from connecting to EHS, suggest use watchdog pattern where EHS creates a file and watchdog checks for presence of this file
//	AfxBeginThread(checkForEHSConnection, this);
}

void CDrawProgView::ViewInstance(INXPOSITION selectedIcon) {
	ConData* selectedBlob = NULL;
	HTREEITEM childItem;
	INXString depPath, csProjectDir;

	// open the instance dep
	selectedBlob = (ConData*) pDEP->condata->GetAt(selectedIcon);
	pFrame->m_wndProjectBar.m_cProjTree.openProject = FALSE;
	childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(selectedBlob, pDEP->hItem);
	depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem);
	pFrame->m_wndProjectBar.m_cProjTree.hSelItem = childItem;
	pProject->pProjMData->getProjectDir(csProjectDir);
	CDocument* Subsystem=AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + depPath +
		"\\" + selectedBlob->description + ".prg");
//@todo there is a crash here that occurs when sub systems are opened (reports that the path doesn't exist - this coid be when two projects are open at the same time.
	// Propagate debug data
	POSITION pos = Subsystem->GetFirstViewPosition();
	CDrawProgView* pView = (CDrawProgView*) Subsystem->GetNextView(pos);
	if (pView->pProject->debug.debugMode != DBGSTOP) {
		pView->pDEP->ResetAllDbgEvents();
		// To Do: Pass the method select
		pView->pDEP->PropagateDebugDataDown(selectedBlob);
	}
	// Need to initialise undo for case when the view is already open
	// This is done in OnActivateView
	/*
		POSITION pos = Subsystem->GetFirstViewPosition();
		CDrawProgView* pView = (CDrawProgView*) Subsystem->GetNextView(pos);
		pView->initUndo();
	*/
}

void CDrawProgView::OnDelete() 
{
	//ShellExecute(NULL, "open", "ruby", "C:\\work\\webTV\\EHS\\iplayer.dl http://www.bbc.co.uk/iplayer/page/item/b00b24mk.shtml?src=ip_potpw", NULL, SW_HIDE);
	if( !pProject->pProjMData->getLock() ) {

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{

		ConData* blob;
		bool undo = TRUE;
		
		readProjectFile();
		// don't allow an undo when deleting a Xport
		blob = (ConData*) pDEP->condata->GetAt(selectedControl);
		if (blob->m_csIconType.Find("XINPUT") != -1 || blob->m_csIconType.Find("XOUTPUT") != -1 || blob->m_csIconType == "XSTART" || 
			blob->m_csIconType == "XFINISH") {
			undo = FALSE;
		}
		pFrame->m_wndProjectBar.m_cProjTree.DeleteIcon(selectedControl, pProject, pDEP, 1);
		RedrawWindow();
		if (undo) {
			SaveUndo();
		}
		else {
			initUndo();
		}
		pProject->SaveProject();
		pProject->pProjMData->writeProjectFile();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnProperties() 
{

	if( !pProject->pProjMData->getLock() ) {

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);

	}else{

		ConData* blob;
		
		readProjectFile();
		blob = (ConData*) pDEP->condata->GetAt(selectedControl);
		INT_PTR retVal = pDEP->EditControl(selectedControl);

		// remove highlight

		blob->selected = 0;
		if( retVal == IDOK)
			pProject->SaveProject();
		RedrawWindow();

		if( retVal == IDOK)
			pProject->pProjMData->writeProjectFile();

		pProject->pProjMData->releaseLock();

	}
}

void CDrawProgView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and / or call the base class	
	/*if (pDC->IsPrinting()) {
		pDC->SetMapMode(MM_TEXT);
	}*/
	CScrollView::OnPrepareDC(pDC, pInfo);

	CDrawProgDoc* pDoc = GetDocument();
	pDC->SetMapMode(MM_ANISOTROPIC);
	//pDC->SetMapMode(MM_TEXT);

	//kwhite:canvassupport pDEP initiated by GetDocument()
	//INXSize vwSize = pDEP->getCanvasSize();
	viewSize = pDEP->getCanvasSize();
	
	// set this if using MM_LOENGLISH
	//vwSize.cy = -vwSize.cy;
	//pDC->SetWindowExt(vwSize);
	pDC->SetWindowExt(viewSize);

	// Get the number of pixels per inch in x and y
	int xLogPixels = pDC->GetDeviceCaps(LOGPIXELSX);
	int yLogPixels = pDC->GetDeviceCaps(LOGPIXELSY);
	
	// Calculate the viewport extent in x and y
	//long xExtent = (long)vwSize.cx*scale*xLogPixels/100L;
	//long yExtent = (long)vwSize.cy*scale*yLogPixels/100L;
	//long xExtent = (long)(vwSize.cx*scale)/100;
	//long yExtent = (long)(vwSize.cy*scale)/100;
	long xExtent = (long)(viewSize.cx*scale)/100;
	long yExtent = (long)(viewSize.cy*scale)/100;

	//pDC->SetViewportExt((int)xExtent, (int)-yExtent);	
	pDC->SetViewportExt((int)xExtent, (int)yExtent);	
}

void CDrawProgView::OnDeleteLine() 
{

	pDEP->DeleteLine(nodeIcon, selectedPort, selectedPortType);
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnDeleteNode() 
{
	int iSegmentNum = -1;
	ConData* blob;

	blob = (ConData*) pDEP->condata->GetAt(nodeIcon);

	if (selectedPortType == INPUTPORT) {
		iSegmentNum = blob->inputport[selectedPort]->line.GetLineSegment(SnapToGrid(m_cpRButtonDown));
	}
	else if (selectedPortType == STARTPORT) {
		iSegmentNum = blob->startport[selectedPort]->line.GetLineSegment(SnapToGrid(m_cpRButtonDown));
	}

	if (iSegmentNum >= 0) {
		if (selectedPortType == INPUTPORT) {
			blob->inputport[selectedPort]->line.DeleteNode(iSegmentNum);
		}
		else if (selectedPortType == STARTPORT) {
			blob->startport[selectedPort]->line.DeleteNode(iSegmentNum);
		}
		RedrawWindow();
		SaveUndo();
		pProject->SaveProjectDep();
	}
}

void CDrawProgView::OnStraighten() 
{
	ConData* blob;

	blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
	if (selectedPortType == INPUTPORT) {
		blob->inputport[selectedPort]->line.deleteAllNodes();
	}
	else if (selectedPortType == STARTPORT) {
		blob->startport[selectedPort]->line.deleteAllNodes();
	}		
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

// This function reroutes all lines of the seldcted icon. 
void CDrawProgView::OnReRouteAll() {
	ConData* blob;
	blob = (ConData*) pDEP->condata->GetAt(selectedControl);// assume selected is OK!
	blob->ReRouteAll();
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep(); // why do we need these?
}


// This function reroutes the slected line - 
// THe port number dependent placement is stil l messy (in allr respects) and spread out over the classes this will need reviewing 
// at the next code review. @todo it would be best if this were to be moved into the condata class as the 'reroute all' is there too
// and this knows about port numbers, which can then finally be passed to the line class dogleg function.
void CDrawProgView::OnReRoute()  
{
	ConData* blob;
	blob = (ConData*) pDEP->condata->GetAt(nodeIcon);
	int y_shift=blob->rectangle.top; // if a loop backward is required go below either block 
	if (selectedPortType == INPUTPORT) {
		blob->inputport[selectedPort]->line.AddDogLeg(blob->inputport[selectedPort]->portNum*2+1,y_shift);
	}
	else if (selectedPortType == STARTPORT) {
		blob->startport[selectedPort]->line.AddDogLeg(blob->startport[selectedPort]->portNum*2,y_shift);
	}
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnIconShowDesc() 
{
	ConData* blob;
	
	blob = (ConData*) pDEP->condata->GetAt(selectedControl);
	if (blob->showdescript == 0) {
		CDC *theDC = GetDC();
		blob->DrawDescription(theDC);
		blob->showdescript = 1;
	}
	else if (blob->showdescript == 1) {
		blob->showdescript = 0;
	}
	// remove highlight
	blob->selected = 0;
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnSubset() 
{
	if( !pProject->pProjMData->getLock() ) {

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{
		lib = 0;
		SubsetEncapsulate();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnTopdown() 
{
	if( !pProject->pProjMData->getLock() ) {
		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{

		pDEP->TopdownEncapsulate();
		pFrame->updateFunctionBlockBar();
		pProject->pProjMData->releaseLock();
	}
}



void CDrawProgView::OnPortInitialise() 
{
	ConData* blob;

	blob = (ConData*) pDEP->condata->GetAt(selectedControl);
	if (!(selectedPortType == STARTPORT && !blob->startport[selectedPort]->line.exist)) {
		AfxMessageBox("Can only initialise unconnected start ports.");
		return;
	}

	if (blob->startport[selectedPort]->initialise) {
		blob->startport[selectedPort]->initialise = 0;
		blob->startport[selectedPort]->connected = 0;
	}
	// set the start port to initialised
	else {
		blob->startport[selectedPort]->initialise = 1;
		blob->startport[selectedPort]->connected = 1;
	}
	blob->startport[selectedPort]->init();
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
	
}

void CDrawProgView::OnKey() 
{
	colourKeyDialog = new ColourKey;

   //Check if new succeeded and we got a valid pointer to a dialog object
	if(colourKeyDialog != NULL)
	{
		// Create modeless dialog box
		BOOL ret = colourKeyDialog->Create(IDD_COLOUR_KEY,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Dialog");
		//dialog->ShowWindow(SW_SHOW);
	}
	else
		AfxMessageBox("Error Creating Dialog Object");
	
	/**** how is dialog deleted? ***/
	//delete dialog;	
}

void CDrawProgView::OnCreateInstance() 
{
	ConData* blob;

	blob = (ConData*) pDEP->condata->GetAt(selectedControl);
	if (blob->m_iUserDefined)
	{
		ViewInstance(selectedControl);
	}
	else
	{
		AfxMessageBox("Can only view user defined blocks.");
	}
	// remove highlight
	blob->selected = 0;

	RedrawWindow();	
}

void CDrawProgView::OnUpLevel()
{
	pFrame->m_wndProjectBar.m_cProjTree.OpenParent(pDEP->hItem, pProject);
}

void CDrawProgView::OnDebugStart() 
{	
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	bool ehsRunning = pApp->isConnectionToEHS();
	if (!ehsRunning) {
		ehsRunning = pApp->startEHS(false);
	}

	// if EHS started successfully or already running, start debug
	if (ehsRunning) {
		// display loading debug message
		dbgMsg = new CDbgMsgDialog;
	   //Check if new succeeded and we got a valid pointer to a dialog object
		if(dbgMsg != NULL)
		{
			// Create modeless dialog box
			BOOL ret = dbgMsg->Create(IDD_DEBUG_MSG,this);
			if(!ret)   //Create failed.
				AfxMessageBox("Error creating Dialog");
			//dbgMsg->ShowWindow(SW_SHOW);
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		
		pProject->debug.setView(this);
		pProject->DebugStart(0); //switch to debug without restart
	//	if (dbgMsg) delete dbgMsg;
	}
}

void CDrawProgView::OnDebugRestart() 
{
	pProject->debug.DebugRestart();
}

void CDrawProgView::OnDebugStop() 
{
	pProject->debug.DebugStop();
}

void CDrawProgView::OnEHSInitParams(){
	((CLabLgbBaseApp *) AfxGetApp())->openEHSInitParamsDialog();	
}



void CDrawProgView::OnEHSStartRestart(){
	((CLabLgbBaseApp *) AfxGetApp())->startEHS(false);	
}

/**
 *
 * Exports transferables to export directory
 * Transferables are sodl, info.xml, images, fonts etc
 *
 */
bool CDrawProgView::appUploader_ExportTransferables(){
	bool success = false;

	INXString infoFilePath;
	pProject->pProjMData->getProjectDir(infoFilePath);
	infoFilePath += APP_DESC_DIR;
	infoFilePath += APP_DESC_FILE;

	// make sure the info.xml file has been created before we proceed
	CFileOperation fo;
	int foErr = fo.CheckPath( infoFilePath);
	if (foErr != PATH_IS_FILE) {
		AfxMessageBox("Project Description not found:" + infoFilePath);
	} else {
		CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
		pApp->setProjMetaData( pProject->pProjMData );
		pApp->setProject(pProject);

		if( !pProject->pProjMData->getLock() ){
			AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
		}else{
			pApp->copyTransferrablesToExports(pProject->pProjMData, false, true);
			// Write-out the latest transfer time.
			pProject->pProjMData->updateTransferTime();
			pProject->pProjMData->writeProjectFile();
			pProject->pProjMData->releaseLock();
			success = true;
		}
	}
	return success;
}

/**
 *
 * Tars up the files in the export directory
 *
 */
bool CDrawProgView::appUploader_TarExportFiles(){
	bool success = false;

	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;
  
	struct archive *a;
	struct archive_entry *entry;
	struct stat st;
	INXString exportPath;
	pProject->pProjMData->getProjectDir(exportPath);
	exportPath += EXPORTDIR;
	INXString filePath;
	INXString name;

	INXString tarFilePath;
	pProject->pProjMData->getProjectDir(tarFilePath);
	tarFilePath += APP_TAR_FILE;

	a = archive_write_new();
	archive_write_set_compression_gzip(a);
	archive_write_set_format_pax_restricted(a); // tar format that uses pax extensions only when absolutely necessary. Most of the time, it will write plain ustar entries
	archive_write_open_filename(a, tarFilePath);

	// write each file in the export directory to the archive
	CFileFind hFile;
	BOOL bFound = hFile.FindFile( exportPath + _T("*.*") );
	success = true;
	while ( bFound && success )   
	{
		bFound = hFile.FindNextFile();
		name = hFile.GetFileName();
		if( (! hFile.IsDots()) && (! hFile.IsDirectory() ))		
		{
			name = hFile.GetFileName();
			filePath = hFile.GetFilePath();
			stat(filePath, &st);
			entry = archive_entry_new();
			archive_entry_set_pathname(entry, name);
			archive_entry_set_size(entry, st.st_size);
			archive_entry_set_filetype(entry, AE_IFREG);
			archive_entry_set_perm(entry, 0644);
			archive_write_header(a, entry);

			success = false;
			pFile = fopen ( filePath , "rb" );
			if (pFile != NULL) {
				// obtain file size:
				fseek (pFile , 0 , SEEK_END);
				lSize = ftell (pFile);
				rewind (pFile);

				// allocate memory to contain the whole file:
				buffer = (char*) malloc (sizeof(char)*lSize);
				if (buffer != NULL) {
					// copy the file into the buffer:
					result = fread (buffer,1,lSize,pFile);
					if (result == lSize) {
						archive_write_data(a, buffer, result);
						success = true;
						//@todo - log success and failure of archiving each file
					}
				}
				fclose(pFile);
				free(buffer);
			}

			archive_entry_free(entry);
		}
	}

	int arcClsErr = archive_write_close(a);
	int arcFreeErr = archive_write_free(a); // archive_write_finish() in libarchive2.x and 3.x

	// if files archived successfully and close and free were successfull
	if (success && (arcClsErr != 0 || arcFreeErr != 0)) {
		success = false;
	}
	return success;
}

void CDrawProgView::OnEHSStop(){
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {

		if (( tcpClient.SendText("E\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
		}
		// tidy up, close the port at this end
		tcpClient.DisConnect();
	}
}

/* - don't need to send params via tcpip, now update graphics.nfo
void CDrawProgView::OnEHSUpdate(){
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {
		EHSInitParams * pParams = ((CDrawProgApp *) AfxGetApp())->pEHSInitParams;
		char cRGB[10];
		sprintf_s(cRGB,"%3d%3d%3d",pParams->nColourRed,pParams->nColourGreen,pParams->nColourBlue);


//		if (( tcpClient.SendText("UX100\nUY200\nUW600\nUH300\nUC200057220\nUA150\nUF1\nUZ1\n")) != 0)	{
		if (( tcpClient.SendText("UX" + intToString(pParams->nXCoord) + "\n"
									"UY" + intToString(pParams->nYCoord) + "\n"
									"UW" + intToString(pParams->nWidth) + "\n"
									"UH" + intToString(pParams->nHeight) + "\n"
									"UC" + cRGB + "\n"
									"UA" + intToString(pParams->nAlpha) + "\n"
									"UF" + intToString(pParams->nHasFrame) + "\n"
									"UZ" + intToString(pParams->nZOrder) + "\n"
									)) != 0)	{

			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}
}
*/

void switchTarget(int iSelInd) {
	LucidTcpipClient tcpClient;

	INXString csLocalIpAddress = DEFAULT_TARGET_LOCAL_IP;
	INXString csLocalPort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCAL_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));
	INXString csRemoteIpAddress = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),_T((CString)DEFAULT_TARGET_REMOTE_IP));
	INXString csRemotePort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));

	if (iSelInd == TARGET_COMBO_INDEX_LOCAL) {
		// local target selected
		tcpClient.SetIPAddress((CString)csLocalIpAddress);
		tcpClient.SetPort(atoi(csLocalPort));
		setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T(REG_TARGET_LOCATION_DATA_LOCAL));
		((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget = REG_TARGET_LOCATION_DATA_LOCAL;
	}
	else if (iSelInd == TARGET_COMBO_INDEX_REMOTE) {
		// remote target selected
		tcpClient.SetIPAddress((CString)csRemoteIpAddress);
		tcpClient.SetPort(atoi(csRemotePort));
		setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T(REG_TARGET_LOCATION_DATA_REMOTE));
		((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget = REG_TARGET_LOCATION_DATA_REMOTE;
	} 
	else {
		// app server target selected
		setLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T(REG_TARGET_LOCATION_DATA_APPSERVER));
		((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget = REG_TARGET_LOCATION_DATA_APPSERVER;
	}
}

void CDrawProgView::OnSelChangeEHSHostCombo() {
	int selInd = pFrame->m_combo.GetCurSel();
	switchTarget(selInd);
}

void CDrawProgView::OnEHSManageLocally() {
	pFrame->m_combo.SetCurSel(TARGET_COMBO_INDEX_LOCAL);
	switchTarget(TARGET_COMBO_INDEX_LOCAL);
}

void CDrawProgView::OnEHSManageRemotely() {
	//@todo validate remote IP address before allow user to switch to managing EHS remotely
	pFrame->m_combo.SetCurSel(TARGET_COMBO_INDEX_REMOTE);
	switchTarget(TARGET_COMBO_INDEX_REMOTE);
}



void CDrawProgView::OnUpdateEHSManageLocally(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget != REG_TARGET_LOCATION_DATA_LOCAL);
}

void CDrawProgView::OnUpdateEHSManageRemotely(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget != REG_TARGET_LOCATION_DATA_REMOTE);
}

void CDrawProgView::OnUpdateEHSStartRestart(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget != REG_TARGET_LOCATION_DATA_APPSERVER);
}

void CDrawProgView::OnUpdateEHSStop(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget != REG_TARGET_LOCATION_DATA_APPSERVER);
}

void CDrawProgView::OnUpdateEHSInitParams(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget == REG_TARGET_LOCATION_DATA_LOCAL) && (((CLabLgbBaseApp *) AfxGetApp())->isEHSLocalInstallPathKnown));
}

void CDrawProgView::OnEHSTopmost(){
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {

		if (( tcpClient.SendText("Z2\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}
}

void CDrawProgView::OnEHSBottommost(){
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {

		if (( tcpClient.SendText("Z0\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}
}

void CDrawProgView::OnEHSNormalZOrder(){
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {

		if (( tcpClient.SendText("Z1\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}
}

void CDrawProgView::OnUpdateLocalHost(CCmdUI *pCmdUI)
{
/* test to add random text to status bar
    pCmdUI->Enable(); 
    INXString strPage;
    strPage.Format(_T("Page %d"), 12); 
    pCmdUI->SetText(strPage); 
*/
}

void CDrawProgView::OnDebugStartwithrestart()
{
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	bool ehsRunning = pApp->isConnectionToEHS();
	if (!ehsRunning) {
		ehsRunning = pApp->startEHS(false);
	}

	// if EHS started successfully or already running, start debug
	if (ehsRunning) {

		/* We need to restart -> set to debug -> send monitors -> run
		'L' -> 'X' - > send stuff() -> 'R' 
		*/
		/* @todo we don't do any explicit state change checking here that current SODL and target is coherent */
	/*	pProject->debug.tcpClient.SendText("Lt.sdl\n"); //@todo shouldn't access the tcpip bits like this really!
		pProject->debug.tcpClient.SendText("Lt.sdl\n"); //@todo shouldn't access the tcpip bits like this really!
		OnDebugStart(); // just it it up then run
		*/
		// display loading debug message
		dbgMsg = new CDbgMsgDialog;
	   //Check if new succeeded and we got a valid pointer to a dialog object
		if(dbgMsg != NULL)
		{
			// Create modeless dialog box
			BOOL ret = dbgMsg->Create(IDD_DEBUG_MSG,this);
			if(!ret)   //Create failed.
				AfxMessageBox("Error creating Dialog");
			//dbgMsg->ShowWindow(SW_SHOW);
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		
		pProject->debug.setView(this);
		pProject->DebugStart(1); //switch to debug without restart
	}	
}


void CDrawProgView::OnEditCut() 
{
	ConData *icondata;
	INXPOSITION pos, delPos;
	bool undo = TRUE;

	if( !pProject->pProjMData->getLock() ) {
		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{		
		readProjectFile();
		// keep a copy of the selected box to be deleted
		pDEP->setSaveSelectedFlag(TRUE);
		pDEP->SaveProg(workDir + TEMPDIR + "temp");
		pDEP->setSaveSelectedFlag(FALSE);
		// save project because may have changed sub-blocks but not saved, and want to paste changes
		pProject->SaveProject();
		edit.BufferSelectedIcons(selectRect, pProject, pDEP);
		pasteFlag = TRUE;

		// delete every icon in the selected box
		pos = pDEP->condata->GetHeadPosition();
		while (pos) {
			delPos = pos;
			icondata = (ConData *) (pDEP->condata->GetNext(pos));
			//if (selectRect.PtInRect(icondata->GetIconCentre())) {
			if (icondata->selected) {
				if (icondata->m_csIconType.Find("XINPUT") != -1 || icondata->m_csIconType.Find("XOUTPUT") != -1 || icondata->m_csIconType == "XSTART" || 
					icondata->m_csIconType == "XFINISH") {
					undo = FALSE;
				}
				pFrame->m_wndProjectBar.m_cProjTree.DeleteIcon(delPos, pProject, pDEP, 1);
				if (!undo) {
					//@todo - currently can't undo an edit-cut if the selection contains a subsystem
					initUndo();
				}
			}
		}
		selectRect.BottomRight() = selectRect.TopLeft();
		RedrawWindow();
		copyFlag = FALSE;
		if (undo) {
			SaveUndo();
		}
		pProject->SaveProject();
		edit.resetPasteOffset();
		pProject->pProjMData->writeProjectFile();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnEditCopy() 
{

	if( !pProject->pProjMData->getLock() ) {
		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{
		readProjectFile();
		// if an icon is selected then want to be able to copy
		// Therefore, need to save the fact that it is selected
		// However, only want to do this when copying and not when saving app.
		pDEP->setSaveSelectedFlag(TRUE);
		pDEP->SaveProg(workDir + TEMPDIR + "temp");
		pDEP->setSaveSelectedFlag(FALSE);
		// save project because may have changed sub-blocks but not saved, and want to paste changes
		pProject->SaveProject();
		edit.BufferSelectedIcons(selectRect, pProject, pDEP);
		pasteFlag = TRUE;
		edit.resetPasteOffset();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnEditPaste() 
{
	ConData* icondata;
	INXPOSITION pos;
	INXObjList* pasteList;
	INXPoint old(0,0);
	INXPoint topLeft, bottomRight, ScrollPosition;
	HTREEITEM hParent;
	INXString depPath, csProjectDir;
	DEP* pParentDEP;
	INXRect clientRect;

	if( !pProject->pProjMData->getLock() ) {
		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{

		readProjectFile();
		pProject->pProjMData->getProjectDir(csProjectDir);
		//SaveCopy("tmpcopy.prg");
		RemoveHighlight();
		pasteList = edit.LoadPaste((INXString)workDir + "\\tmpcopy.prg");
		if (pasteList == NULL) {
			pProject->pProjMData->releaseLock();
			return;
		}
		if (XportNameExist(pasteList)) {
			AfxMessageBox("Cannot paste the icons, because one of the Xport labels already exists.");
			pProject->pProjMData->releaseLock();
			return;
		}
		pDEP->ReassignIconIDs(pasteList);

		// initialise the selection box coordinates
		icondata = (ConData*)(pasteList->GetHead());
		topLeft = icondata->GetIconPos();
		bottomRight = icondata->GetIconBottomRight();

		pos = pasteList->GetHeadPosition();
		while (pos) {
			icondata = (ConData*)(pasteList->GetNext(pos));
			// highlight all the icons pasted so that they can be moved
			icondata->selected = 1;
			// adjust selection box coordinates to fit around the pasted icons
			if (icondata->GetIconPos().x < topLeft.x) {
				topLeft.x = icondata->GetIconPos().x;
			}
			if (icondata->GetIconPos().y < topLeft.y) {
				topLeft.y = icondata->GetIconPos().y;
			}
			if (icondata->GetIconBottomRight().x > bottomRight.x) {
				bottomRight.x = icondata->GetIconBottomRight().x;
			}
			if (icondata->GetIconBottomRight().y > bottomRight.y) {
				bottomRight.y = icondata->GetIconBottomRight().y;
			}
			//icondata->RenewPosition(offset, old, firstpoint);
			// add user defined blocks to project tree
			if (icondata->m_iUserDefined) {
				pDEP->CreateInstance(icondata, 2);
				pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(icondata, pProject, pDEP->hItem);
			}
			// if we are pasting an xport, test if it is being pasted into a subsystem first
			if (icondata->m_csIconType.Find("XINPUT") != -1 || icondata->m_csIconType.Find("XOUTPUT") != -1 || 
				icondata->m_csIconType == "XSTART" || icondata->m_csIconType == "XFINISH") {
				hParent = pFrame->m_wndProjectBar.m_cProjTree.GetParentItem(pDEP->hItem);

				// can only add xports if DEP has a parent (is a subsystem), add ports to parent component and add xport to this DEP
				if ((INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hParent) != APP) {
					depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hParent);
					pFrame->m_wndProjectBar.m_cProjTree.hSelItem = hParent;
					CDrawProgView* pView = pProject->OpenView(csProjectDir + DEPDIR + depPath + 
						(INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hParent) + ".prg");
					pParentDEP = pView->pDEP;
					pParentDEP->AddBlockPort(icondata->m_csIconType, icondata->description, pDEP->depFilename);
					// set modified flag in doc
					pView->pDoc->SetModifiedFlag(TRUE);
					pFrame->m_wndProjectBar.m_cProjTree.hSelItem = pDEP->hItem;
					CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + 
						pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");	
					pDEP->condata->AddTail(icondata);
				}
			} else {
				pDEP->condata->AddTail(icondata);
			}
		}
		pProject->addPastedGuiWidgets(pDEP->hItem, pasteList);
		
		selectRect.TopLeft() = topLeft;
		selectRect.BottomRight() = bottomRight;
		selectRect.InflateRect(10,10,10,10);
		// offset for pasted icons
		//selectRect.OffsetRect(offset);

		//pDEP->condata->AddTail(pasteList);
		pDEP->AssignIconLineEndPoints(); 	
		//pasteList->RemoveAll();
		delete pasteList;

		// check that the pasted icons are inside the client rect otherwise paste them in 
		// the top left corner of the client rect
		ScrollPosition = this->GetScrollPosition();
		this->GetClientRect(clientRect);
		clientRect.MoveToXY(ScrollPosition);
		if (!clientRect.PtInRect(topLeft)) {
			edit.setPasteOffset(ScrollPosition);
			firstpoint = topLeft - INXPoint(20,30);
		}
		else {
			firstpoint = old;
		}

		selectRect = pDEP->RenewSelectedPos(edit.getPasteOffset(), firstpoint, selectRect);
		// if pasted icons are near bottom right then increase canvas and adjust scroll position
		if (cs.StretchCanvas(selectRect, viewSize)) {
			SetScrollSizes(MM_TEXT, viewSize);
			pDEP->setCanvasSize(viewSize);
		}
		if (cs.NearBottomRightBoundary(selectRect.BottomRight())) {
			cs.AdjustPositionOfScrollDownwards();
			ScrollPosition = cs.AdjustPositionOfScrollDownwards();
			ScrollToPosition(ScrollPosition);	
		}
		edit.incPasteOffset();
		RedrawWindow();
		// icons are still highlighted so allow cut and copy
		copyFlag = TRUE;
		SaveUndo();
		pProject->SaveProject();

		pProject->pProjMData->writeProjectFile();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnDebugPause() 
{
	pProject->debug.DebugPause();	
}

void CDrawProgView::OnDebugStep() 
{
	pProject->debug.DebugStep();
}

void CDrawProgView::OnDebugForce() 
{
	pProject->debug.DebugForce(nodeIcon, selectedPortType, selectedPort);
}

void CDrawProgView::OnDbgHistStart() 
{
	pProject->debug.DbgHistStart();
}

void CDrawProgView::OnDbgHistStep() 
{
	pProject->debug.waitNxtEvnt = TRUE;
}

void CDrawProgView::OnDbgHistTime() 
{
	pProject->debug.DbgHistTime();
}

void CDrawProgView::OnDbgHistStop() 
{
	pProject->debug.dbgHistory = FALSE;	
	pProject->debug.waitNxtEvnt = TRUE;
}

void CDrawProgView::OnUpdateDebugStartwithrestart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((pProject->debug.debugMode == DBGSTOP) && !pProject->debug.dbgHistory && !lock);
}


void CDrawProgView::OnUpdateDebugStart(CCmdUI* pCmdUI) 
{	
	// debugger doesn't work in ehs so disable in dep
	//pCmdUI->Enable(TRUE);
	pCmdUI->Enable((pProject->debug.debugMode == DBGSTOP) && !pProject->debug.dbgHistory && !lock);
}

void CDrawProgView::OnUpdateDebugStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((pProject->debug.debugMode != DBGSTOP) && !pProject->debug.dbgHistory && !lock);
}

void CDrawProgView::OnUpdateDebugForce(CCmdUI* pCmdUI) 
{
	// Enable force item when in pause state
	pCmdUI->Enable(FALSE);
	//pCmdUI->Enable((pProject->debug.debugMode == DBGPAUSE) && !pProject->debug.dbgHistory);	
}

void CDrawProgView::OnUpdateDebugPause(CCmdUI* pCmdUI) 
{
	// Enable pause item when in start state and step state
	// Want to be able to pause stepping.
	pCmdUI->Enable(((pProject->debug.debugMode == DBGSTART) || (pProject->debug.debugMode == DBGSTEP)) && !pProject->debug.dbgHistory && !lock);
}

void CDrawProgView::OnUpdateDebugStep(CCmdUI* pCmdUI) 
{
	// Enable step item when in pause state
	pCmdUI->Enable((pProject->debug.debugMode == DBGPAUSE) && !pProject->debug.dbgHistory);
	// Keep button depressed until event received
	pCmdUI->SetCheck((pProject->debug.debugMode == DBGSTEP));
}

void CDrawProgView::OnUpdateDebugRestart(CCmdUI* pCmdUI) 
{
	// Enable restart item when in pause state and step state
	pCmdUI->Enable(((pProject->debug.debugMode == DBGPAUSE) || (pProject->debug.debugMode == DBGSTEP)) && !pProject->debug.dbgHistory);
}

void CDrawProgView::OnUpdateDbgHistStart(CCmdUI* pCmdUI) 
{
	// Enable debug history start item when debug data is available
	pCmdUI->Enable(((pProject->debug.debugMode == DBGPAUSE) || ((pProject->debug.debugMode == DBGSTOP) && 
		pProject->debug.dbgDataRdy)) && !pProject->debug.dbgHistory);
}

void CDrawProgView::OnUpdateDbgHistStep(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pProject->debug.dbgHistory);
}

void CDrawProgView::OnUpdateDbgHistStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pProject->debug.dbgHistory);
}

void CDrawProgView::OnUpdateDbgHistTime(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pProject->debug.dbgHistory);
}

void CDrawProgView::OnDestroy() 
{
	CScrollView::OnDestroy();

	if (pProject) {
		pProject->debug.DestroyView();
	}
}

void CDrawProgView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(pasteFlag);
}

void CDrawProgView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(copyFlag);
}

void CDrawProgView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(copyFlag);
}

void CDrawProgView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_IDS_DRAWANIMATEDSTUFF) {
//		KillTimer(TIMER_IDS_DRAWANIMATEDSTUFF);

		//@todo - various performance improvements: currently uses brute force to check every port and only highlight those in state of mandatory & no connection
		// Suggestion is to have a global reference count maintained by the ports that turns the animation timer on/off when > 0 and = 0 respectively
		// As a bootstrap to the reference count could check all ports on a slow timer and kill the animation timer if no ports require highlighting
		onlyDrawAnimatedStuff = false;
		
		// only do this if not debugging, dragging or selecting rectangle etc
		if ((pProject->debug.debugMode == DBGSTOP) && (m_iLftBtnDownState == 0)) {
			onlyDrawAnimatedStuff = true;
			if (toggleAnimation == 0) {
				toggleAnimation = 1;
			} else {
				toggleAnimation = 0;
			}
			// redraw ports - flag to prevent background from being erased
			RECT clientRect;
			GetClientRect(&clientRect);
			InvalidateRect((INXRect) clientRect,false);


			HICON hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), getConnectionState() ? MAKEINTRESOURCE(IDI_LED_ON) : MAKEINTRESOURCE(IDI_LED_OFF), IMAGE_ICON, 16, 16, LR_SHARED);
			if (getConnectionState() && !toggleAnimation) {
				hIcon = NULL;
			}

			//todo - should poll both local and remote not one or the other
			if (((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget == REG_TARGET_LOCATION_DATA_LOCAL) {
				pFrame->m_wndStatusBar.GetStatusBarCtrl().SetIcon(pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_LOCALHOST), hIcon);
				pFrame->m_wndStatusBar.GetStatusBarCtrl().SetIcon(pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_NUM), NULL);
			} else if (((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget == REG_TARGET_LOCATION_DATA_REMOTE) {
				pFrame->m_wndStatusBar.GetStatusBarCtrl().SetIcon(pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_NUM), hIcon);
				pFrame->m_wndStatusBar.GetStatusBarCtrl().SetIcon(pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_LOCALHOST), NULL);
			}
			DestroyIcon(hIcon);


		}
	} else {
		pProject->debug.setWriteRtaTrace(TRUE);
	}
	CScrollView::OnTimer(nIDEvent);
}

bool CDrawProgView::getConnectionState() {
	return bHasConnectionToEHS;
}

UINT CDrawProgView::checkForEHSConnection( LPVOID ptr ) 
{
	//todo - polling of connection should be in a separate thread
	// need to poll local and remote independent of setting csCurrentTarget
	// problem - blocks process if remote IP not found as timeout is too long
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;

	while (1) {
		// connect to tcpip client
		if (( ltsStatusType = tcpClient.Connect()) != 0){
			bHasConnectionToEHS = false;
			//todo - why does this test never fail in linux env?
		} else {
			// test connection by retrieving state
			if (( tcpClient.SendText("?S\n")) != 0)	{
				bHasConnectionToEHS = false;
			} else {
				bHasConnectionToEHS = true;
			}
		}
		Sleep(2000);		
	}
	return 0;
}



BOOL CDrawProgView::OnEraseBkgnd(CDC* pDC) 
{
	if ((pProject->debug.debugMode == DBGSTART) || (m_iLftBtnDownState == 5) || (m_iLftBtnDownState == 4)) {
		return FALSE;
	}
	else {
		return CScrollView::OnEraseBkgnd(pDC);
	}
}

void CDrawProgView::OnUpdateUpLevel(CCmdUI* pCmdUI) 
{
	HTREEITEM hParent = pFrame->m_wndProjectBar.m_cProjTree.GetParentItem(pDEP->hItem);
	pCmdUI->Enable((INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hParent) != APP);
}

void CDrawProgView::OnEditUndo() 
{
	if(!pProject->pProjMData->getLock() ){
		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
	}else{

		char szUndoFileNum[MAX_UNDO_FILE_NUM];
		int iTmp;

		// delete all condata objects
		pDEP->deleteConData();
		pDEP->condata->RemoveAll();
		// decrement the undo file num pointer
		if (iUndoFileNum == 0) {
			iUndoFileNum = (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1;
		}
		else {
			iUndoFileNum--;
		}
		// Load the previous DEP and project file
		if (iUndoFileNum == 0) {
			iTmp = (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1;
		}
		else {
			iTmp = iUndoFileNum-1;
		}
		_itoa_s(iTmp, szUndoFileNum, 10);
		pDEP->LoadProg(workDir + TEMPDIR + "undo" + szUndoFileNum + ".prg");
		//pProject->pProjMData->setProjectToNull();
		pProject->pProjMData->readProjectFile(workDir + TEMPDIR + "lpj_undo" + szUndoFileNum);

		if (iUndoFileNum == iUndoFlagNum) {
			undoFlag = FALSE;
		}
		// remove selection box for the case when doing a group move
		selectRect.BottomRight() = selectRect.TopLeft();
		copyFlag = FALSE;
		
		// update project navigator
		pFrame->m_wndProjectBar.m_cProjTree.UpdateTree(pProject, pDEP);
		pProject->SaveProject();
		pProject->pProjMData->writeProjectFile();

		// remap DEP and resend all monitors in debug mode
		if (pProject->debug.debugMode != DBGSTOP) {
			pDEP->MapLineID(pProject->flattened);
			pProject->debug.sendAllMonitors();
		}

		RedrawWindow();
		redoFlag = TRUE;
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(undoFlag);	
}

void CDrawProgView::OnEditRedo() 
{	
	if(!pProject->pProjMData->getLock() ){
		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
	}else{

		char szUndoFileNum[MAX_UNDO_FILE_NUM];

		// delete all condata objects
		pDEP->deleteConData();
		pDEP->condata->RemoveAll();
		// Load the DEP and project file prior to undo
		_itoa_s(iUndoFileNum, szUndoFileNum, 10);
		pDEP->LoadProg(workDir + TEMPDIR + "undo" + szUndoFileNum + ".prg");
		pProject->pProjMData->readProjectFile(workDir + TEMPDIR + "lpj_undo" + szUndoFileNum);
		if (iUndoFileNum >= (int)pow(10.0, MAX_UNDO_FILE_NUM-1)-1) {
			iUndoFileNum = 0;
		}
		else {
			iUndoFileNum++;
		}
		if (iUndoFileNum == maxUndoFileNum) {
			redoFlag = FALSE;
		}

		// update project navigator
		pFrame->m_wndProjectBar.m_cProjTree.UpdateTree(pProject, pDEP);
		pProject->pProjMData->writeProjectFile();

		// remap DEP in debug mode
		if (pProject->debug.debugMode != DBGSTOP) {
			pDEP->MapLineID(pProject->flattened);
			pProject->debug.sendAllMonitors();
		}

		RedrawWindow();
		undoFlag = TRUE;
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(redoFlag);	
}

void CDrawProgView::OnUpdateFtpSodl(CCmdUI* pCmdUI) 
{
	INXString csProjectName;
	pProject->pProjMData->getProjectName(csProjectName);
	pCmdUI->Enable((csProjectName == pDEP->depFilename) && (pProject->debug.debugMode == DBGSTOP));	
}

void CDrawProgView::OnAddPort() 
{
	ConData* blob;
	INXString portLabel;
	INXString portType, dataType;
	int iPortType, iDataType;
	INXPoint point;
	INXString blockFile;

	blob = (ConData*) pDEP->condata->GetAt(selectedControl);
	CAddPortDialog dialog(blob);
	if (blob->m_iUserDefined) {
		if (dialog.DoModal() == IDOK) {
			portLabel = dialog.m_PortLabel;
			portLabel.MakeLower();
			portType = dialog.portType;
			dataType = dialog.dataType;
			iPortType = blob->portType2Int(portType);
			// convert data type to an int
			if (dataType == "Boolean") {
				iDataType = 0;
			}
			else if (dataType == "Integer") {
				iDataType = 1;
			}
			else if (dataType == "Real") {
				iDataType = 2;
			}
			else if (dataType == "String") {
				iDataType = 3;
			}
			else {
				iDataType = -1;
			}
			pFrame->m_wndProjectBar.m_cProjTree.openProject = FALSE;
			pProject->AddBlockPort(blob, iDataType, iPortType, portLabel, pDEP);
			pFrame->m_wndProjectBar.m_cProjTree.openProject = TRUE;
			blob->selected = 0;
			RedrawWindow();
			pProject->SaveProjectDep();
			//@todo - currently can't undo addition of ports to a subsystem
			// don't allow undo of ports because can't undo them in the other block
			initUndo();
		}
	}
	else {
		AfxMessageBox("Can only add ports to user defined blocks.");
	}	
}

void CDrawProgView::OnUpdateAddPort(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(FALSE);	
}

void CDrawProgView::OnDeletePort() 
{
	if (AfxMessageBox("Deleting a port will also change the subsystem and the action cannot be undone. Do you want to continue?",MB_OKCANCEL|MB_ICONEXCLAMATION)==IDOK) {
		pFrame->m_wndProjectBar.m_cProjTree.DeleteBlockPort(selectedControl, selectedPort, selectedPortType, pProject, pDEP);
		//SaveUndo();
		pProject->SaveProjectDep();
		//@todo - currently can't undo a delete port on a subsystem but have added a confimation dialog
		// Can't undo delete port currently, because it requires udating 2 deps (.prg)
		initUndo();
	}
}

void CDrawProgView::OnTagLine() 
{
	pDEP->TagLine(nodeIcon, selectedPort, selectedPortType);
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnShowLine() 
{
	pDEP->ShowLine(selectedControl, selectedPort, selectedPortType);
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnDeleteTag() 
{
	pDEP->DeleteTag(selectedControl, selectedPort, selectedPortType);
	RedrawWindow();
	SaveUndo();
	pProject->SaveProjectDep();
}

void CDrawProgView::OnAddTag() 
{
	if (pDEP->AddTag(selectedControl, selectedPort, selectedPortType)) {
		RedrawWindow();
		SaveUndo();
		pProject->SaveProjectDep();
	}
}

void CDrawProgView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	int xOrg, yOrg;

	// Output the file name
	pDC->SetTextAlign(TA_CENTER);
	pDC->TextOut(pInfo->m_rectDraw.right/2, -20, (CString)((CPrintData*)(pInfo->m_lpUserData))->m_ViewTitle);
	pDC->SetTextAlign(TA_LEFT);

	// Calculate the origin point for the current page
	if (((CDrawProgApp*)AfxGetApp())->GetOrientation() == 0) {
		xOrg = (int)(((CPrintData*)(pInfo->m_lpUserData))->m_ViewRefPoint.x + PAGE_WIDTH*((pInfo->m_nCurPage - 1)%
		(((CPrintData*)(pInfo->m_lpUserData))->m_nWidths)));
		yOrg = (int)(((CPrintData*)(pInfo->m_lpUserData))->m_ViewRefPoint.y - PAGE_LENGTH*((pInfo->m_nCurPage - 1)/
		(((CPrintData*)(pInfo->m_lpUserData))->m_nWidths)));
	}
	else if (((CDrawProgApp*)AfxGetApp())->GetOrientation() == 1) {
		xOrg = (int)(((CPrintData*)(pInfo->m_lpUserData))->m_ViewRefPoint.x + PAGE_LENGTH*((pInfo->m_nCurPage - 1)%
		(((CPrintData*)(pInfo->m_lpUserData))->m_nLengths)));
		yOrg = (int)(((CPrintData*)(pInfo->m_lpUserData))->m_ViewRefPoint.y - PAGE_WIDTH*((pInfo->m_nCurPage - 1)/
		(((CPrintData*)(pInfo->m_lpUserData))->m_nLengths)));
	}

	// Calculate offsets to center drawing area on page as positive values
	//int xOffset = (int)(pInfo->m_rectDraw.right - PAGE_WIDTH)/2;
	//int yOffset = -(int)(pInfo->m_rectDraw.bottom + PAGE_LENGTH)/2;
	int xOffset = 0; int yOffset = 0;

	// Change window origin to correspond to current page & save old origin
	INXPoint oldOrg = pDC->SetWindowOrg(xOrg-xOffset, yOrg+yOffset);

	// Define a clip rectangle the size of the printed area
	//pDC->IntersectClipRect(xOrg,yOrg,xOrg+(int)PAGE_WIDTH,yOrg+(int)PAGE_LENGTH);
	pDC->IntersectClipRect(xOrg,yOrg,xOrg+pInfo->m_rectDraw.right,yOrg+pInfo->m_rectDraw.bottom);
	
	OnDraw(pDC); // Draw the whole view
	pDC->SelectClipRgn(NULL); // Remove the clip rectangle
	pDC->SetWindowOrg(oldOrg); // Restore old window origin
	//CScrollView::OnPrint(pDC, pInfo);
}

void CDrawProgView::OnViewZoom() 
{
	CZoomDialog dialog;

	// Initialise the zoom dialog box
	dialog.iScale = scale;
	if (dialog.DoModal() == IDOK) {
		scale = dialog.iScale;
		RedrawZoom();
	}
}

void CDrawProgView::OnWriteSodl() 
{
	if(!pProject->pProjMData->getLock() ){
		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
	}else{
		INXString csProjectDir;
		pProject->pProjMData->getProjectDir( csProjectDir );

		//AfxMessageBox( "Get ready to call SaveProject" );
		pProject->SaveProject();

		//AfxMessageBox( "Get ready to call m_cProjTree.SaveHierName" );
		pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(pProject->pDEP[0]->hItem, csProjectDir, pProject);

		//AfxMessageBox( "Get ready to call WriteSODL" );
		pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnUpdateWriteSodl(CCmdUI* pCmdUI) 
{
	INXString csProjectName;
	pProject->pProjMData->getProjectName(csProjectName);
	//pCmdUI->Enable((csProjectName == pDEP->depFilename) && (pProject->debug.debugMode == DBGSTOP));	
	pCmdUI->Enable(pProject->debug.debugMode == DBGSTOP);	
}

void CDrawProgView::OnGroupSetup() 
{
	pProject->EditGroupSetup();	
}

void CDrawProgView::OnAddIconToGroup() 
{
	CSelectGroupDialog dialog(pProject);
	int index;
	vector<Group> vGroups;

	if (dialog.DoModal()==IDOK) {
		// get the corresponding group ID
		pProject->pProjMData->getGroupVec(vGroups);
		for (UINT i=0; i<vGroups.size(); i++) {
			if (dialog.selectedGroup == vGroups.at(i).Name) {
				index = i;
			}
		}
		// set the groupID in all the icon start ports
		pProject->AddIconToGroup(selectedControl, vGroups.at(index).ID, pDEP);
	}
	// remove highlight
	ConData* blob = (ConData*) pDEP->condata->GetAt(selectedControl);
	blob->selected = 0;
	RedrawWindow();
}

void CDrawProgView::OnAddPortToGroup() 
{
	CSelectGroupDialog dialog(pProject);
	ConData* blob;
	vector<Group> vGroups;

	// only set the group ID in start ports
	if (selectedPortType == STARTPORT) {
		if (dialog.DoModal()==IDOK) {
			// get the corresponding group ID
			pProject->pProjMData->getGroupVec(vGroups);
			for (UINT i=0; i<vGroups.size(); i++) {
				if (dialog.selectedGroup == vGroups.at(i).Name) {
					// set the groupID in the start port selected
					blob = (ConData*) pDEP->condata->GetAt(selectedControl);
					blob->startport[selectedPort]->groupID = vGroups.at(i).ID;
				}
			}
		}
	}
	else {
		AfxMessageBox("Can only add start ports to a group.");
	}
}

// Add selected icons to a group
void CDrawProgView::OnAddToGroup() 
{
	INXPOSITION pos, selectedPos;	
	CSelectGroupDialog dialog(pProject);
	int selectedCount = 0;
	ConData* icondata;
	vector<Group> vGroups;

	// check if any icons have been selected
	pos = pDEP->condata->GetHeadPosition();
	while (pos) {
		icondata = (ConData *) (pDEP->condata->GetNext(pos));
		if (selectRect.PtInRect(icondata->GetIconCentre())) {
			selectedCount++;
		}
	}
	if (selectedCount) {
		if (dialog.DoModal()==IDOK) {
			// get the corresponding group ID
			pProject->pProjMData->getGroupVec(vGroups);
			for (UINT i=0; i<vGroups.size(); i++) {
				if (dialog.selectedGroup == vGroups.at(i).Name) {
					// set the groupID in all the selected icons
					pos = pDEP->condata->GetHeadPosition();
					while (pos) {
						selectedPos = pos;
						icondata = (ConData *) (pDEP->condata->GetNext(pos));
						if (selectRect.PtInRect(icondata->GetIconCentre())) {
							pProject->AddIconToGroup(selectedPos, vGroups.at(i).ID, pDEP);
						}
						// remove highlight
						icondata->selected = 0;
					}
				}
			}
		}
	}
	else {
		AfxMessageBox("First select the icons you want to add to the group.");
	}

	selectRect.SetRectEmpty();
	RedrawWindow();
}

void CDrawProgView::OnSaveProject() 
{
	// read project file first to avoid overwriting changes made by LGB
	if(!pProject->pProjMData->getLock() ){

		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );

	}else{
		readProjectFile();
		pProject->SaveProject();
		// don't allow undo when save project, because it removes user defined blocks that have been deleted
		undoFlag = FALSE;
		pProject->pProjMData->releaseLock();
	}
	/*@todo MIGRATION_ISSUES temporary testing of the 
	 openGL view using Save button */
		int temp = 0;
		wxApp* glApp = new MyApp(); 
		wxApp::SetInstance(glApp);
		wxEntryStart(temp, 0);
		wxTheApp->OnInit();
		wxTheApp->OnRun();
		wxTheApp->OnExit();
		wxEntryCleanup();
}

void CDrawProgView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	INXString csRtaFilePath = (INXString)"localhost.Generic-Debugger:" + workDir + TRACEDIR + TRACE_CONFIG_FILE;
	bool bChangeView = FALSE;

	if (bActivate) {
		if (pProject->debug.debugMode != DBGSTOP) {
				if (pProject->debug.pView != this) {
					bChangeView = TRUE;
				}		
			// Only map lines once. Not every time view is activated.
			// Can't do this in OnInitialUpdate because view might not have been opened in debug mode
			if (!pDEP->dbgMappedFlag) {
				pDEP->dbgMappedFlag = TRUE;
				pFrame->m_wndProjectBar.m_cProjTree.OpenDebugDEP(this);
			}
			else {
				pProject->debug.setView(this);
			}
		}

		// Only send monitors and restart trace if a new view is opened
		if (bChangeView) {
			// don't want EHS to send trace data for monitored lines that are in a previous view
			// because event lines may not be displayed
			pProject->debug.ClearAllMonitors();
			pProject->debug.sendAllMonitors();
			if (pProject->debug.m_RtaTraceSupport.getRtaTrace()) {
				pProject->debug.m_RtaTraceSupport.terminateRtaTraceProc();
				pProject->debug.m_RtaTraceSupport.createRtaConfigFile();
				ShellExecute(NULL, "open", RTA_TRACE_PATH, csRtaFilePath, NULL, SW_SHOWNORMAL);
			}
		}

		CChildFrame* viewFrame = (CChildFrame*)GetParentFrame();
		if (pProject->getDefineMonitors()) {
			viewFrame->m_StatusBar.GetStatusBarCtrl().SetText("Define Monitoring", 1, 0);
		}
		else {
			viewFrame->m_StatusBar.GetStatusBarCtrl().SetText("Line Editing", 1, 0);
		}

		// set the unique icon id for the current view
		SetIconID();
		//@todo - undo is initialised when view is first activated which has an unwanted consequence of clearing the cache when LAB moves to background and then foreground
		// REVISIT
		// Don't allow an undo when a view is first activated
		// This isn't ideal because it means if LAB moves to background and then foreground
		// undo is initialised. It is the only way I could think of initialising undo when changing the view
		initUndo();
	}

	// update target location from registry
	INXString csLocation = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T((CString)DEFAULT_TARGET_LOCATION));
	((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget = csLocation;
	// update target combo box with remote ip address
	INXString csRemoteIP = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),_T((CString)DEFAULT_TARGET_REMOTE_IP));
	pFrame->m_combo.DeleteString(TARGET_COMBO_INDEX_REMOTE);
	pFrame->m_combo.InsertString(TARGET_COMBO_INDEX_REMOTE,csRemoteIP);

	if (csLocation == REG_TARGET_LOCATION_DATA_APPSERVER) {
		pFrame->m_combo.SetCurSel(TARGET_COMBO_INDEX_APPSERVER);
	} else 	if (csLocation == REG_TARGET_LOCATION_DATA_REMOTE) {
		pFrame->m_combo.SetCurSel(TARGET_COMBO_INDEX_REMOTE);
	} else {
		pFrame->m_combo.SetCurSel(TARGET_COMBO_INDEX_LOCAL);
	}

	// update status bar with remote ip address
	pFrame->m_wndStatusBar.GetStatusBarCtrl().SetText(TARGET_DISPLAY_NAME_LOCAL, pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_CAPS),SBT_NOBORDERS);
	pFrame->m_wndStatusBar.GetStatusBarCtrl().SetText(csRemoteIP, pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_SCRL),SBT_NOBORDERS);


	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CDrawProgView::OnLibrary() 
{
	if(!pProject->pProjMData->getLock() ){

		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
	}else{

		lib = 1;
		SubsetEncapsulate();
		pProject->pProjMData->releaseLock();
	}
}

void CDrawProgView::OnImportLib() 
{
	// rArchive has been removed due to license issues
	//rArchive ra;
	CFileOperation fo;
	INXString m_File_Root = workDir + EXPORTDIR;
	CFileFind finder;
	INXString fileName, compName;
	
	// Create a directory for the archived files to be written to
	fo.SetOverwriteMode(true); // set OverwriteMode flag
	fo.Delete(m_File_Root);
	CreateDirectory(m_File_Root, NULL);
	CFileDialog box(1,"r","archive.r",OFN_FILEMUSTEXIST,"Archived Library Files (*.r)|*.r|");
	if (box.DoModal()==IDOK) {
		//extract archive
		/*
		if (!ra.Open(box.GetPathName())) {
			MessageBox(_T("Failed to open import library file."));
			return;	
		}
		ra.SetRoot(m_File_Root);
		if (!ra.ResetPointer()) MessageBox(_T("Pointer reset failed."));
		while (ra.WriteNextFile() == TRUE) {
		}
		*/

		// Copy the library components to the userdefined directory unless they already exist in which
		// case prompt the user
		int bWorking = finder.FindFile(workDir + EXPORTDIR + "*.prg");	
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			fileName = finder.GetFileName();
			compName = fileName;
			compName.MakeReverse();
			compName.Delete(0,4);
			compName.MakeReverse();
			if (LibExist(fileName)) {
				if ((AfxMessageBox("The library component " + compName + " already exists. Do you want to overwrite it?",
					MB_YESNO))==IDYES) {
					fo.Copy((INXString)workDir + EXPORTDIR + fileName, (INXString)workDir + USERDEFDIR);
					fo.Copy((INXString)workDir + EXPORTDIR + compName, (INXString)workDir + USERDEFDIR);
					fo.Copy((INXString)workDir + EXPORTDIR + compName + ".idf.ini", (INXString)workDir + USERDEFDIR);
				}
			}
			else {
				fo.Copy((INXString)workDir + EXPORTDIR + fileName, (INXString)workDir + USERDEFDIR);
				fo.Copy((INXString)workDir + EXPORTDIR + compName, (INXString)workDir + USERDEFDIR);
				fo.Copy((INXString)workDir + EXPORTDIR + compName + ".idf.ini", (INXString)workDir + USERDEFDIR);
			}
		}
	}
}

void CDrawProgView::OnExportLib() 
{
	RJMFileFind2 ff;
	INXObjArray<INXString> csa;
	INXString m_File_Root = workDir + EXPORTDIR;
	INXString csProjectDir;
	pProject->pProjMData->getProjectDir(csProjectDir);
	INXString m_Archive_File_Name = csProjectDir + EXPORTDIR + "archive.r";
	CExportDialog dialog;

	if (dialog.DoModal() == IDOK) {
		ff.FindFiles(true,m_File_Root,_T("*.*"),&csa);
	
		// Open archive
		// rArchive has been rmoved due to license issues
		/*
		rArchive ra;
		if (!ra.Open(m_Archive_File_Name,true)) {
			MessageBox(_T("Failed to open export library file."));
			return;
		};

		ra.SetRoot(m_File_Root);
		*/

		// Add files to archive
		/*
		INXString tmp;
		for (int c=0;c<csa.GetSize();c++) {
			tmp = csa.GetAt(c);
			tmp = tmp.Right(tmp.GetLength()-m_File_Root.GetLength());
			if (!ra.AddToArchive(tmp)) MessageBox(_T("Failed to add file " + tmp));
		};
		ra.Close();
		*/
		MessageBox(_T("Export library file created in " + csProjectDir + EXPORTDIR));
	}
}

void CDrawProgView::OnCloseProject() 
{
	// Save the proj file before losing the proj, 
	// bcos otherwise/ afterwards, pProjMData==NULL and the prog crashes.
	if( !pProject->pProjMData->getLock() ) {
		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
	}else{
		pProject->pProjMData->readProjectFile();
		pProject->pProjMData->writeProjectFile();
		pProject->pProjMData->releaseLock();
	}

	pDoc->OnCloseDocument();
}

void CDrawProgView::OnUpdateCloseProject(CCmdUI* pCmdUI) 
{
	INXString csProjectName;
	pProject->pProjMData->getProjectName(csProjectName);
	pCmdUI->Enable(csProjectName == pDEP->depFilename);	
}

void CDrawProgView::OnSelectAll() 
{
	INXRect ElementBound; 
	INXPOSITION pos;
	ConData* blob;

	pos = pDEP->condata->GetHeadPosition();
	if (pos) {
		blob = (ConData*) pDEP->condata->GetNext(pos);
		selectRect = blob->GetBoundingRectangle();
	}
	while (pos) {
		blob = (ConData*) pDEP->condata->GetNext(pos);
		//dc.LPtoDP(blob->rectangle);
		ElementBound = blob->GetBoundingRectangle();
		// Make coordinates of view extent the outer limits

		selectRect.UnionRect(selectRect, ElementBound);

	}
	selectRect.NormalizeRect();
	selectRect.InflateRect(10,10,10,10);
	if (pDEP->HighlightSelected(selectRect)) {
		copyFlag = TRUE;
	}
	else {
		copyFlag = FALSE;
	}
	RedrawWindow();
}

void CDrawProgView::OnLButtonDblClk(UINT nFlags, INXPoint _point) 
{
	INXPoint point(_point.x, _point.y);
	// TODO: Add your message handler code here and/or call default
	selectRect.SetRectEmpty();
	CClientDC aDC(this); // create device context
	OnPrepareDC(&aDC); // adjust origin
	aDC.DPtoLP((LPPOINT)point); // convert point to Logical
	//point.x = (point.x * 100)/scale;
	//point.y = (point.y * 100)/scale;

	CDrawProgApp *pApp = (CDrawProgApp *) AfxGetApp();

//	if(!pProject->pProjMData->getLock()){

//		AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );

///	}else{

	//	pProject->pProjMData->readProjectFile();

		selectedControl = pDEP->Incontrol(point);
		if (selectedControl) {
			ConData* blob;
			blob = (ConData*) pDEP->condata->GetAt(selectedControl);
			pDEP->setFBHighlight(selectedControl);
			if (blob->m_csBlockName == "") {
				OnProperties();
			}
			else {
				OnCreateInstance();
			}
		}

//		pProject->pProjMData->writeProjectFile();
//		pProject->pProjMData->releaseLock();

//	} // if(!m_ProjectMetaData.getLock())
	
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CDrawProgView::OnUpdateGroupSetup(CCmdUI* pCmdUI) 
{
	// disable group setup
	//pCmdUI->Enable(FALSE);	
}

void CDrawProgView::OnImportDataFile()
{
/* dk - Data Items removed from IAB
	ExtDataFile dataFile;
	bool bExists;

	if (!pProject->pProjMData->showAddFileDialog(dataFile, bExists)) {
		// showAddfiledialog saves the pmd file!
		if (!bExists) {
			pFrame->m_wndProjectBar.m_cProjTree.AddDataFile(dataFile, pDEP->hItem);
		}
	}
*/
}

void CDrawProgView::OnEditProjectDesc()
{
	ProjectDescriptionDialog projDlg(pProject->pProjMData);

	if (projDlg.DoModal()==IDOK) {
		pProject->pProjMData->writeProjectDescriptionFile();
		pFrame->m_wndProjectBar.m_cProjTree.UpdateProjectDescrptionOnTree(pProject, pDEP);
	}
}

void CDrawProgView::OnSaveProjectAs()
{

	if( !pProject->pProjMData->getLock() ) {

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);

	}else{

		readProjectFile();
		pProject->pProjMData->releaseLock();

		INXString csOldProjectDir, csOldProjectName;
		INXString csNewProjectDir, csNewProjectName, csProjectPathName;
		POSITION pos;
		CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();

		pProject->pProjMData->getProjectDir(csOldProjectDir);
		pProject->pProjMData->getProjectName(csOldProjectName);

		if (!pProject->pProjMData->showSaveProjectAsDialog()) {

			pProject->pProjMData->getProjectDir(csNewProjectDir);
			pProject->pProjMData->getProjectName(csNewProjectName);

			pProject->pProjMData->getLock();
			pProject->pProjMData->writeProjectFile();

			pos = ((CDrawProgApp*)AfxGetApp())->pDocTemplate->GetFirstDocPosition();	
			// The user could do a save as from a sub view therfore:
			// find the top doc object in the project as it needs its name changing
			while (pos) {
				CDrawProgDoc* pDocmnt = (CDrawProgDoc*) ((CDrawProgApp*)AfxGetApp())->pDocTemplate->GetNextDoc(pos);
				if (pDocmnt->pDEP->depFilename == csOldProjectName) {
					// Rename project in project window
					pFrame->m_wndProjectBar.m_cProjTree.SaveProjectAs(csNewProjectName, pDocmnt->pDEP->hItem);
					// Update DEP
					pDocmnt->pDEP->depFilename = csNewProjectName;
					pDocmnt->pDEP->projectDir = csNewProjectDir;
					// update title with new name
					pDocmnt->SetTitle(csNewProjectName);
					pDocmnt->SetPathName(csNewProjectDir + DEPDIR + csNewProjectName + ".prg");
					pProject->pProjMData->getFullPathProjectFile(csProjectPathName);
					pApp->addProjectToMRUList(csProjectPathName);
					pFrame->setFrameCaption();
				}
				// Ensure that all open docs have their pathnames updated.
				// This prevents a new doc being opened.
				else if (pDocmnt->pProject == this->pProject) {
					pDocmnt->SetPathName(csNewProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDocmnt->pDEP->hItem) + pDocmnt->pDEP->depFilename + ".prg");
				}

			}
			pProject->pProjMData->releaseLock();
		}
	}
}

BOOL CDrawProgView::OnMouseWheel(UINT nFlags, short zDelta, INXPoint pt)
{
	// kwhite:canvassupport - SetScrollPosition changed by MouseWheel
	// ScrollPosition is used by NearBottomRightBoundary check
	INXSize topleft = GetScrollPosition( );
	cs.SetScrollPosition(topleft);
	
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CDrawProgView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{	
	// kwhite:canvassupport - SetScrollPosition changed by normal scrolling
	// ScrollPosition is used by NearBottomRightBoundary check
	INXSize topleft = GetScrollPosition( );
	cs.SetScrollPosition(topleft);
	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

void CDrawProgView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// kwhite:canvassupport SetBoundary to accommodate new change in size
	INXPoint max;
	max.x = cx;
	max.y = cy;
	cs.SetMaxBoundary( max);
}

void CDrawProgView::OnEditDelete() 
{
	ConData *icondata;
	INXPOSITION pos, delPos;
	bool undo = TRUE;
	
	if( !pProject->pProjMData->getLock() ) {

		AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);

	}else{

		pProject->pProjMData->readProjectFile();
		// delete every icon in the selected box
		pos = pDEP->condata->GetHeadPosition();
		while (pos) {
			delPos = pos;
			icondata = (ConData *) (pDEP->condata->GetNext(pos));
			//if (selectRect.PtInRect(icondata->GetIconCentre())) {
			if (icondata->selected) {
				if (icondata->m_csIconType.Find("XINPUT") != -1 || icondata->m_csIconType.Find("XOUTPUT") != -1 || icondata->m_csIconType == "XSTART" || 
					icondata->m_csIconType == "XFINISH") {
					undo = FALSE;
				}
				pFrame->m_wndProjectBar.m_cProjTree.DeleteIcon(delPos, pProject, pDEP, 1);
				if (!undo) {
					//@todo - currently can't undo an edit-delete if the selection contains a subsystem
					initUndo();
				}
			}
		}
		selectRect.BottomRight() = selectRect.TopLeft();
		RedrawWindow();
		if (undo) {
			SaveUndo();
		}
		pProject->SaveProject();
		pProject->pProjMData->writeProjectFile();
		pProject->pProjMData->releaseLock();
	}

}

void CDrawProgView::OnTransferOptions()
{

	LucidTcpipClient tcpipClient;
	tcpipClient.ConfigDlg();

	// set the 'all has been transferred already' flag to false, for this new target.
	pProject->m_bTransferAllHasOccurred = false;
	// update the ip address for the target
	int selInd = pFrame->m_combo.GetCurSel();
	INXString csRemoteIP = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),_T((CString)DEFAULT_TARGET_REMOTE_IP));
	pFrame->m_combo.DeleteString(TARGET_COMBO_INDEX_REMOTE);
	pFrame->m_combo.InsertString(TARGET_COMBO_INDEX_REMOTE,csRemoteIP);
	pFrame->m_combo.SetCurSel(selInd);
	// update status bar with remote ip address
	pFrame->m_wndStatusBar.GetStatusBarCtrl().SetText(csRemoteIP, pFrame->m_wndStatusBar.CommandToIndex(ID_INDICATOR_SCRL),SBT_NOBORDERS);
}

void CDrawProgView::OnZoomIn()
{
	scale = scale+ZOOM_DELTA;
	RedrawZoom();
}

void CDrawProgView::RedrawZoom()
{
	char szScale[MAX_ZOOM_CHAR] = {'\0'};

	// Get the frame window for this view
	CChildFrame* viewFrame = (CChildFrame*)GetParentFrame();
	// Write the zoom value to the status bar
	_itoa_s(scale, szScale, 10);
	viewFrame->m_StatusBar.GetStatusBarCtrl().SetText("Zoom: " + (INXString)szScale + "%", 0, 0);
		
	// Adjust scrolling to the new scale
	ResetScrollSizes();
	RedrawWindow();
}

void CDrawProgView::OnZoomOut()
{
	scale = scale-ZOOM_DELTA;
	RedrawZoom();
}

void CDrawProgView::OnUpdateZoomIn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(scale<MAX_ZOOM);
}

void CDrawProgView::OnUpdateZoomOut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(scale>MIN_ZOOM);
}

/*
void CDrawProgView::OnTransferAll()
{
	CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();
	INXString csProjectDir;

	// Write out SODL

// The following line overwrites the contents of the project as seen by LGB.
// This is a problem, as often around this point the user has added some image files to project.
// There doesn't seem to be any particlar need to write the project right now (does there)
// ( MSR 19Sept07 ).
	//pProject->SaveProject();  

	pProject->pProjMData->getProjectDir(csProjectDir);
	// Transfer Manager can be called anywhere in the application hierarchy
	// Need to save hierarchy names starting at the top level of the project
	pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(pProject->pDEP[0]->hItem, csProjectDir, pProject);
	pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	// PD says that there shouldn't be 2 places where u find sodl.
	//pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	pApp->setProjectMetaData(pProject->pProjMData);
	//pApp->setProject(pProject);
	pApp->transferToTarget( changedOnly );
}
*/
/*

void CDrawProgView::OnTransferManager()
{
	CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();
	INXString csProjectDir;

	// Write out SODL

// The following line overwrites the contents of the project as seen by LGB.
// This is a problem, as often around this point the user has added some image files to project.
// There doesn't seem to be any particlar need to write the project right now (does there)
// ( MSR 19Sept07 ).
	//pProject->SaveProject();  

	pProject->pProjMData->getProjectDir(csProjectDir);
	// Transfer Manager can be called anywhere in the application hierarchy
	// Need to save hierarchy names starting at the top level of the project
	pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(pProject->pDEP[0]->hItem, csProjectDir, pProject);
	pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	// PD says that there shouldn't be 2 places where u find sodl.
	//pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	pApp->c_pTransferToTargetDialog->setProjectMetaData(pProject->pProjMData);
	pApp->c_pTransferToTargetDialog->setProject(pProject);
	pApp->c_pTransferToTargetDialog->ShowWindow(SW_SHOWNORMAL);
}

void CDrawProgView::OnLaunchTransfer()
{
	CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();
	INXString csProjectDir;

// Write out SODL
// The following line overwrites the contents of the project as seen by LGB.
// This is a problem, as often around this point the user has added some image files to project.
// There doesn't seem to be any particlar need to write the project right now (does there)
// ( MSR 19Sept07 ).
//pProject->SaveProject(); 
 
	pProject->pProjMData->getProjectDir(csProjectDir);
	// Launch Transfer can be invoked anywhere in the application hierarchy
	// Need to save hierarchy names starting at the top level of the project
	pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(pProject->pDEP[0]->hItem, csProjectDir, pProject);
	pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	pApp->c_pTransferToTargetDialog->setProjectMetaData( pProject->pProjMData );
	pApp->c_pTransferToTargetDialog->setProject(pProject);
	pApp->c_pTransferToTargetDialog->ShowWindow(SW_SHOWNORMAL);
	pApp->c_pTransferToTargetDialog->OnBnClickedTransfer();

}
*/
void CDrawProgView::readProjectFile()
{
	INXString csProjectPathName;

	pProject->pProjMData->getFullPathProjectFile(csProjectPathName);
	pProject->pProjMData->readProjectFile((CString)csProjectPathName);
}

// disable import and export library functions, because decompressing is not working for import library
void CDrawProgView::OnUpdateImportLib(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CDrawProgView::OnUpdateExportLib(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CDrawProgView::OnUpdateTransferManager(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->debug.debugMode == DBGSTOP);
}

void CDrawProgView::OnUpdateLaunchTransfer(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->debug.debugMode == DBGSTOP);
}

void CDrawProgView::OnViewRefresh()
{
	INXRect clientRect;

	this->GetClientRect(clientRect);
	INXPoint point = pDEP->getInitScrollPos(clientRect);
	this->ScrollToPosition(point);
	cs.SetScrollPosition(point);
}

void CDrawProgView::OnUpdateViewRefresh(CCmdUI *pCmdUI)
{
//	TRACE(_T("OnUpdateViewRefresh\n"));
	onlyDrawAnimatedStuff = false; // turn this flag off to ensure that normal calls to paint repaint whole window

	pCmdUI->Enable(scale == 100);
}

void CDrawProgView::setLftBtnDownState(const int &val)
{
	m_iLftBtnDownState = val;
}
DROPEFFECT CDrawProgView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, INXPoint point)
{
	//AfxMessageBox("DragOver");

	return CScrollView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CDrawProgView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, INXPoint point)
{
	//AfxMessageBox("Drop");

	return CScrollView::OnDrop(pDataObject, dropEffect, point);
}

void CDrawProgView::OnRenamePort()
{
	pFrame->m_wndProjectBar.m_cProjTree.RenameBlockPort(selectedControl, selectedPort, selectedPortType, pProject, pDEP);
	pProject->SaveProjectDep();
	//@todo - currently can't undo a rename port of subsystem
	// Can't undo rename port currently, because it requires udating 2 deps (.prg)
	initUndo();
	RedrawWindow();
}

void CDrawProgView::OnRenameXport()
{
	pFrame->m_wndProjectBar.m_cProjTree.RenameXport(selectedControl, pProject, pDEP);
	pProject->SaveProjectDep();
	//@todo - currently can't undo a rename port of subsystem
	// Can't undo rename xport currently, because it requires udating 2 deps (.prg)
	initUndo();
	RedrawWindow();
}

void CDrawProgView::OnAddToLibrary()
{
	Encapsulation subsystem;
	CMenuNameDialog dlg;
	INXString csMenuName;
	ConData* blob = (ConData*) pDEP->condata->GetAt(selectedControl);

	if (!blob->m_iUserDefined) {
		AfxMessageBox("Can only add user defined blocks to a library.");
		return;
	}
	else if (!subsystem.IsLibComponentUnique(blob->className)) {
		if (AfxMessageBox("A library component with this name already exists. Do you want to replace it?",MB_YESNO|MB_ICONEXCLAMATION)==IDNO) {
			return;
		}
	}
	
	// Get menu name
	if (dlg.DoModal()==IDOK) {
		csMenuName = dlg.getMenuName();
	}
	else {
		return;
	}

	pDEP->AddToLibrary(selectedControl, csMenuName);
	// Update the function block chooser
	pFrame->updateFunctionBlockBar();
}

void CDrawProgView::OnUpdateTransferChanged(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((((CLabLgbBaseApp *) AfxGetApp())->csCurrentTarget != REG_TARGET_LOCATION_DATA_APPSERVER) && pProject->m_bTransferAllHasOccurred && (pProject->debug.debugMode == DBGSTOP));
}

void CDrawProgView::OnUpdateAddToGroup(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

/*
void CDrawProgView::OnLabTransferAll()
{
	CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();
	INXString csProjectDir;

	pProject->pProjMData->getProjectDir(csProjectDir);
	// Transfer Manager can be called anywhere in the application hierarchy
	// Need to save hierarchy names starting at the top level of the project
	pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(pProject->pDEP[0]->hItem, csProjectDir, pProject);
	pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	const bool bUpdatedOnly = false;
	pApp->transferToTarget( pProject->pProjMData, bUpdatedOnly );

}

void CDrawProgView::OnLabTransferChanged()
{
	pProject->pProjMData->getProjectDir(csProjectDir);

	// Transfer Manager can be called anywhere in the application hierarchy
	// Need to save hierarchy names starting at the top level of the project
	pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(
			pProject->pDEP[0]->hItem, csProjectDir, pProject);

	pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);

	const bool bUpdatedOnly = true;
	pApp->transferToTarget( pProject->pProjMData, bUpdatedOnly );
}

*/

void CDrawProgView::OnLabTransferAll()
{ 
	transferToTarget( false );
}


void CDrawProgView::OnLabTransferChanged()
{ 
	transferToTarget( true );
}



/* CALLBACK FUNCTION that writes to our data buffer
 *
 * This leaves the string null terminates at each chunk of data
 * 
 */
size_t AppUpload_write_data(void *ptr, size_t size, size_t nmemb, void *userdata) {
	size_t amount = size * nmemb;
	char * buffer = (char *)userdata;

	strncpy(buffer, (char *)ptr,amount>CURL_WRITE_BUFFER_SIZE?CURL_WRITE_BUFFER_SIZE:amount );
	buffer[amount]='\0'; // terminate if pre-gunged
	return amount;
}


void CDrawProgView::uploadAppToServer()
{
	INXString tarFilePath;
	pProject->pProjMData->getProjectDir(tarFilePath);
	tarFilePath += APP_TAR_FILE;

	bool success = false;
	success = appUploader_ExportTransferables();
	if (!success) return;
	//@todo log progress
	success = appUploader_TarExportFiles();
	if (!success) return;
	//@todo log progress

	// 3. upload the app tar file
	// find the app tar file in the main project dir
	CFileOperation fo;
	int foErr = fo.CheckPath( tarFilePath);
	if (foErr != PATH_IS_FILE) {
		AfxMessageBox("App tar file not found:" + tarFilePath);
	} else {
		// perform file upload

		//@todo - replace with the nonblocking version and a progress bar or callback checked from a separate thread
		AfxMessageBox("Uploading commencing... Click OK to continue");

		CURL *curl;
		CURLcode res;

		INXString message;
		long lResponseCode;
//		char * pCharURLEncodedUserName;
//		char * pCharUserName;
		INXString csUserName = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_USERNAME_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_USERNAME));
//		pCharUserName = (char *) (LPCTSTR) csUserName;
		INXString csURL = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_URL_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_URL));
		INXString csPassword = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_APPSERVER_PASSWORD_VALUE),_T((CString)DEFAULT_TARGET_APPSERVER_PASSWORD));

		struct curl_httppost *formpost=NULL;
		struct curl_httppost *lastptr=NULL;
		struct curl_slist *headerlist=NULL;
		static const char buf[] = "Expect:";

		curl = curl_easy_init();
		// initalize custom header list (stating that Expect: 100-continue is not wanted 
		headerlist = curl_slist_append(headerlist, buf);

		if(curl) {
//don't need this, url encoded in form?			pCharURLEncodedUserName = curl_easy_escape(curl, pCharUserName, 0);

			/* Fill in the file upload field. This makes libcurl load data from
				 the given file name when curl_easy_perform() is called. */ 
			curl_formadd(&formpost,
				   &lastptr,
				   CURLFORM_COPYNAME, "file",
				   CURLFORM_FILE, tarFilePath,
				   CURLFORM_END);

			/* Fill in the username field */ 
			curl_formadd(&formpost,
				   &lastptr,
				   CURLFORM_COPYNAME, "username",
				   CURLFORM_COPYCONTENTS, csUserName,
				   CURLFORM_END);

			/* Fill in the user password field */ 
			curl_formadd(&formpost,
				   &lastptr,
				   CURLFORM_COPYNAME, "password",
				   CURLFORM_COPYCONTENTS, csPassword,
				   CURLFORM_END);

			/* what URL that receives this POST */ 
			curl_easy_setopt(curl, CURLOPT_URL, csURL);
			/* disable 100-continue header if explicitly requested */ 
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

			res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, AppUpload_write_data);
			if (res != CURLE_OK) {
				AfxMessageBox("libcurl failed to register callback");
			} else {
				char * buffer = new char[CURL_WRITE_BUFFER_SIZE];
				buffer[0] = '\0';
				res = curl_easy_setopt(curl, CURLOPT_WRITEDATA,(void*) buffer); //We'll write XML into a buffer directly
				if (res != CURLE_OK) {
					AfxMessageBox("libcurl to register writedata");
				} else {						

					res = curl_easy_perform(curl);
					if (res == CURLE_OK) {
						curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lResponseCode);
						//@todo - check response code from server

						// check buffer to see if any error xml
						if (buffer != NULL) {
							if (buffer[0] == '\0') {
								AfxMessageBox("App successfully uploaded");
							} else {
								message = buffer;
								//@todo - extract error message from xml 
								AfxMessageBox("App uploading failed ...\n" + message);
							}
						} else {
							AfxMessageBox("App successfully uploaded");
						}
					} else if (res == CURLE_COULDNT_RESOLVE_HOST) {
						AfxMessageBox("App failed to upload, couldn't resolve host.\nCheck the URL of the App Server");
					} else {
						AfxMessageBox("App failed to upload, libcurl error code: " + res);
					}
				}
				free(buffer);
			}

			/* then cleanup the formpost chain */ 
			curl_formfree(formpost);

			/* free slist */ 
			curl_slist_free_all (headerlist);

			/* always cleanup */ 
			curl_easy_cleanup(curl);
		}
	}

}


void CDrawProgView::transferToTarget(const bool &bUpdatedOnly )
{
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();

	// first check which target - if it is App server upload app via http post form, if it is EHS local or remote se tcpip
	INXString csLocation = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T((CString)DEFAULT_TARGET_LOCATION));

	if (csLocation == REG_TARGET_LOCATION_DATA_APPSERVER) {
		uploadAppToServer();
	} 
	// start EHS - if starts successfully or is already running, send SODL (which also reloads app in EHS)
	else if (pApp->startEHS(true)) {

		// read project file first to avoid overwriting changes made by LGB
		pApp->c_pTgtTransProgressDlog->ShowWindow(SW_NORMAL);
		INXString *pcsDum = new INXString("Writing SODL ...");
		pApp->c_pTgtTransProgressDlog->SendMessage( UWM_SET_OVERALL_PROMPT, 0, (LPARAM)pcsDum  );

		Sleep(500);

		pApp->setProjMetaData( pProject->pProjMData );
		pApp->setProject(pProject);

		INXString csProjectDir;
		pProject->pProjMData->getProjectDir(csProjectDir);

		// Transfer Manager can be called anywhere in the application hierarchy
		// Need to save hierarchy names starting at the top level of the project
		pFrame->m_wndProjectBar.m_cProjTree.SaveHierName(
				pProject->pDEP[0]->hItem, csProjectDir, pProject);

		pProject->WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);



		pcsDum = new INXString("Tabulating Transferrables ...");
		pApp->c_pTgtTransProgressDlog->SendMessage( UWM_SET_OVERALL_PROMPT, 0, (LPARAM)pcsDum  );

		if( !pProject->pProjMData->getLock() ){

			AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );

		}else{

			pApp->transferToTarget( bUpdatedOnly );
			//pProject->m_bTransferAllHasOccurred = pProject->m_bTransferAllHasOccurred || (!bUpdatedOnly && pApp->getTransferSuccess());

			// Write-out the latest transfer time.
			pProject->pProjMData->updateTransferTime();
			pProject->pProjMData->writeProjectFile();

			pProject->pProjMData->releaseLock();

		}
	}	
}

void CDrawProgView::OnDefineMonitors()
{
	// Get the frame window for this view
	CChildFrame* viewFrame = (CChildFrame*)GetParentFrame();
	if (pProject->getDefineMonitors()) {
		pProject->setDefineMonitors(FALSE);
		viewFrame->m_StatusBar.GetStatusBarCtrl().SetText("Line Editing", 1, 0);
	}
	else {
		pProject->setDefineMonitors(TRUE);
		viewFrame->m_StatusBar.GetStatusBarCtrl().SetText("Define Monitoring", 1, 0);
	}
	RedrawWindow();
}

void CDrawProgView::OnSelectAllMonitors()
{
	pDEP->setAllMonitors(TRUE);
	SaveUndo();					//must come first.
	pProject->SaveProjectDep();
	RedrawWindow();
}

void CDrawProgView::OnClearAllMonitors()
{
	pDEP->setAllMonitors(FALSE);
	SaveUndo();					//must come first.
	pProject->SaveProjectDep();
	RedrawWindow();
}

void CDrawProgView::OnUpdateSelectAllMonitors(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->getDefineMonitors());	
}

void CDrawProgView::OnUpdateClearAllMonitors(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->getDefineMonitors());
}

void CDrawProgView::OnUpdateDefineMonitors(CCmdUI *pCmdUI)
{
	if (pProject->getDefineMonitors()) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
	pCmdUI->Enable(pProject->debug.debugMode == DBGSTOP);
}

void CDrawProgView::OnTrace()
{
	INXString csRtaFilePath = (INXString)"localhost.Generic-Debugger:" + workDir + TRACEDIR + TRACE_CONFIG_FILE;
	CFileOperation fo;

	// Check that RTA-Trace is installed
	if (!fo.CheckPath(RTA_TRACE_PATH)) {
		AfxMessageBox("RTA Trace is not installed");
		return;
	}
	if (!pProject->debug.m_RtaTraceSupport.getRtaTrace()) {
		if (pProject->debug.m_RtaTraceSupport.createRtaConfigFile()) {
			// Launch RTA-TRACE
			ShellExecute(NULL, "open", RTA_TRACE_PATH, csRtaFilePath, NULL, SW_SHOWNORMAL);
			pProject->debug.m_RtaTraceSupport.setRtaTrace(TRUE);
			startTraceTimer();
		}
	}
	else {
		pProject->debug.m_RtaTraceSupport.terminateRtaTraceProc();
		stopTraceTimer();
		pProject->debug.m_RtaTraceSupport.setRtaTrace(FALSE);
	}
}

void CDrawProgView::OnUpdateTrace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->debug.debugMode != DBGSTOP);
	pCmdUI->SetCheck(pProject->debug.m_RtaTraceSupport.getRtaTrace());
}

void CDrawProgView::OnUpdateTransferAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(pProject->debug.debugMode == DBGSTOP);
}

void CDrawProgView::OnNationalLanguageSupport()
{
	if (pProject->RunNlsExec()) {
		pProject->OpenNlsFile();
	}
}

void CDrawProgView::OnImportResourceFile()
{
	ExtResourceFile xResourceFile;
	bool bExists;

	if (!pProject->pProjMData->showAddResourceDialog(xResourceFile, bExists)) {
		if (!bExists) {
			pFrame->m_wndProjectBar.m_cProjTree.AddResourceFile(xResourceFile, pDEP->hItem);
		}
	}
}


