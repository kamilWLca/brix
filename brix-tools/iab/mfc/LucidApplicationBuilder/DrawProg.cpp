// DrawProg.cpp : Defines the class behaviors for the application.
//

/* Windows application window */

//#include "Resource.h"
#include "stdafx.h"
#include "DrawProg.h"
//#include "..\common\version.h"  Not now used - this was for kieron's old script.

#include "MainFrm.h"
#include "ChildFrm.h"
#include "DrawProgDoc.h"
#include "DrawProgView.h"
#include <direct.h>
#include "ProjectDialog.h"
#include "FileOperations.h"
#include "ScreenTagDialog.h"
#include "../common/General_Utilities/GlobalFuncs_3.h"
#include "LucidConstants.h"
#include "common.h"
#include "Resource.h"
#include <cassert>
#include "./revision.h"
#include "Markup.h"
#include "LucidTcpipClient.h"
#include "../common/TransferErrorDlog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "config.h"

// global variable that stores the working directory
char workDir[WORK_DIR_SIZE];

/////////////////////////////////////////////////////////////////////////////
// CDrawProgApp
//CTransferToTargetDialog * CDrawProgApp::c_pTransferToTargetDialog;

BEGIN_MESSAGE_MAP(CDrawProgApp, CWinApp)
	//{{AFX_MSG_MAP(CDrawProgApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_RELOAD_EHS, OnUpdateReloadEhs)
	//ON_COMMAND(ID_RELOAD_EHS, OnReloadEhs)
	ON_COMMAND(ID_NEW_PROJECT, OnNewProject)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	//ON_COMMAND(ID_FILE_OPEN, OnOpenProject)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_OPEN_PROJECT, &CDrawProgApp::OnOpenProject)
	ON_COMMAND(ID_HELP_LAB_USERGUIDE, &CDrawProgApp::OnHelpLabUserGuide)
	ON_COMMAND(ID_HELP_LAB_REFMANUAL, &CDrawProgApp::OnHelpLabRefManual)
	ON_COMMAND(ID_HELP_TUTORIALS, &CDrawProgApp::OnHelpTutorials)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawProgApp construction



BOOL CDrawProgApp::SetWorkDirGlobal(char * workDir)
{

	if(!CheckExecutableFolderContextIsOk()){
		AfxMessageBox("The executable is NOT in a valid location: the app won't work. Seek support");
		return FALSE;
	}

	INXString baseDir;
	GetInstallationBaseDir(baseDir);
	strcpy_s( workDir, WORK_DIR_SIZE, baseDir );
	return TRUE;

}

CDrawProgApp::CDrawProgApp()
{
	SetWorkDirGlobal(workDir);
}

CDrawProgApp::~CDrawProgApp()
{
	delete c_pTgtTransErrorDlog;
	delete c_pTgtTransProgressDlog;
	if (c_Log.IsOpen()) {
		c_Log.Close();
	}

}
/////////////////////////////////////////////////////////////////////////////
// The one and only CDrawProgApp object

CDrawProgApp theApp;
long int ConData::uniqueidgenerator=0;

/////////////////////////////////////////////////////////////////////////////
// CDrawProgApp initialization

BOOL CDrawProgApp::InitInstance()
{

	//m_pMutex = new CMutex( 0, TEXT( MUTEX_LABEL ), 0 );
	//assert(m_pMutex != NULL);

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.

#ifndef _AFXDLL
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//TCHAR* psAppName = _T("LucidGUIBuilder");
	//TCHAR* psWindowTitle = _T("C:\\temp");
	//HWND hWnd = getTitledApplicationWindow( psAppName, psWindowTitle );

	//::SetForegroundWindow( hWnd);

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Lucid"));

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	AfxOleInit();

	//CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_DRAWPRTYPE,
		RUNTIME_CLASS(CDrawProgDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDrawProgView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	//m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// DON'T display a new MDI child window during startup unless passed as a argument!!!
	if (cmdInfo.m_strFileName == "") {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->setFrameCaption();

	c_pTgtTransProgressDlog = new CTgtTransProgDlog();
	c_pTgtTransErrorDlog = new CTransferErrorDlog();

	assert( NULL != c_pTgtTransProgressDlog );
	assert( NULL != c_pTgtTransErrorDlog);

	c_pTgtTransProgressDlog->Create( _T("IDD_TARGET_TRANSFER_PROGRESS") );	//CTransferAppDialog::IDD );
	c_pTgtTransErrorDlog->Create( IDD_TRANSFER_ERROR );				//CTransferAppDialog::IDD );
	
	LucidTcpipClient tcpClient;
	// open log file
	c_Log.Open(LAB_LOG_FILE);

	// Load tools.ini
	LoadToolVec();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDrawProgApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CDrawProgApp::CloseProject(Project* pProject) {
	POSITION pos;

	pos = pDocTemplate->GetFirstDocPosition();
	
	while (pos) {
		CDrawProgDoc* pDoc = (CDrawProgDoc*) pDocTemplate->GetNextDoc(pos);
		if (pDoc->pProject == pProject) {
			pDoc->OnCloseDocument();
			pMainFrame->setFrameCaption();
		}
	}
}

// When delete a user defined block need to close the doc/view if it is open
void CDrawProgApp::CloseUserDefDoc(HTREEITEM hItem) {
	POSITION pos;

	pos = pDocTemplate->GetFirstDocPosition();
	
	// find the doc object that corresponds to the view to which the xport is added
	while (pos) {
		CDrawProgDoc* pDoc = (CDrawProgDoc*) pDocTemplate->GetNextDoc(pos);
		if (pDoc->pDEP->hItem == hItem) {
			// Save the user defined block in case an undo is performed. Need ports to match up.
			// SDG: Don't think this is needed anymore, because project is saved after every action
			//pDoc->SaveUserDefDoc();
			pDoc->OnCloseDocument();
		}
	}
}

int CDrawProgApp::GetOrientation() {
    PRINTDLG pd;
    pd.lStructSize = (DWORD)sizeof(PRINTDLG) ;
	BOOL bRet = GetPrinterDeviceDefaults(&pd) ;
    if (bRet) {
		DEVMODE *pDevMode = (DEVMODE*)::GlobalLock(m_hDevMode) ;
		if (pDevMode->dmOrientation == DMORIENT_PORTRAIT) {
			return 0;
		}
		else if (pDevMode->dmOrientation == DMORIENT_LANDSCAPE) {
			return 1;
		}
		::GlobalUnlock(m_hDevMode) ;
	}
	return -1;
}
// returns true if any document in the specified project has been modified
bool CDrawProgApp::IsModified(Project* pProject) {
	POSITION pos;

	pos = pDocTemplate->GetFirstDocPosition();
	
	// find the doc object that corresponds to the view to which the xport is added
	while (pos) {
		CDrawProgDoc* pDoc = (CDrawProgDoc*) pDocTemplate->GetNextDoc(pos);
		if (pDoc->pProject == pProject && pDoc->IsModified()) {
			return TRUE;
		}
	}

	return FALSE;
}

// Always enable the reload ehs buttton
void CDrawProgApp::OnUpdateReloadEhs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

/*
void CDrawProgApp::AddXPort(INXString projectName, INXString xport, INXString portLabel, INXPoint point ) {
	POSITION pos, viewPos, iconPos;
	ConData* blob;

	pos = pDocTemplate->GetFirstDocPosition();
	
	// find the doc object that corresponds to the view to which the xport is added
	while (pos) {
		CDrawProgDoc* pDoc = (CDrawProgDoc*) pDocTemplate->GetNextDoc(pos);
		if (pDoc->pProject->projectName == projectName) {
			viewPos = pDoc->GetFirstViewPosition();
			CDrawProgView* pView = (CDrawProgView*) pDoc->GetNextView(viewPos);
			iconPos = pView->pDEP->AddIcon(xport, "", point);
			blob = (ConData*) pView->pDEP->condata->GetAt(iconPos);
			blob->description = portLabel;
		}
	}
}
*/

void CDrawProgApp::OnNewProject() 
{
	ProjectMetaData* pProjMData = new ProjectMetaData;
	if (!pProjMData->showNewProjectDialog()) {

		if(!pProjMData->getLock()){
			AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
		}else{
			OpenProject(pProjMData);
			pProjMData->releaseLock();
		}
	}
	else {
		delete pProjMData;
	}
}

void CDrawProgApp::OnOpenProject()
{

	ProjectMetaData* pProjMData = new ProjectMetaData;
	LucidErrEnum err = pProjMData->showOpenProjectDialog();

	if (kErr_NoErr == err) {

		if(!pProjMData->getLock()){

			AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );
		}else{

			LucidErrEnum err = pProjMData->readProjectFile();




			OpenProject(pProjMData);
			pProjMData->releaseLock();
		}

	} else if (err == kErr_InvalidProjectDir) {

		AfxMessageBox("The project file contains\nan invalid project directory\n and/or an invalid project name.");
		delete pProjMData;

	} else {

		delete pProjMData;
	}
}

CDocument* CDrawProgApp::OpenProject(ProjectMetaData* pProjMData)
{
	INXString csProjectDir, csProjectName, csProjectPathName;
	Project* pProject;
	CFileOperation fo;
	bool bLockReleased = false;

	pProjMData->getProjectName(csProjectName);
	pProjMData->getProjectDir(csProjectDir);
	csProjectPathName = csProjectDir + "\\" + csProjectName + PROJECTEXT;
	ifstream projectfile(csProjectPathName);

	// set the project number. There are two approaches. 1. When a project is deleted shift the projects.
	// 2. the approach used is to find a project number that is avaialable.
	bool projectSpace = FALSE;

	for (int i=0; i<MAX_PROJECTS; i++) {
		if (!pMainFrame->m_wndProjectBar.m_cProjTree.pProject[i]) {
			pProject = pMainFrame->m_wndProjectBar.m_cProjTree.AddProject(i, pProjMData);
			i = MAX_PROJECTS;
			projectSpace = TRUE;
		}
	}
	if (!projectSpace) {
		AfxMessageBox("Unable to open the project. DrawProg contains the maximum projects allowed.");
		return NULL;
	}
	
	// Create nls.csv if it doesn't exist
	if (pProject->pProjMData->isLocked()) {
		pProject->pProjMData->releaseLock();
		bLockReleased = true;
	}
	if (!fo.CheckPath(csProjectDir + NLSDIR + NLSFILE)) {
		pProject->RunNlsExec();
		if (!pProject->pProjMData->getLock()) {
			AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
		}
		else {
			pProject->pProjMData->readProjectFile();
			pProject->pProjMData->releaseLock();
		}
	}
	if (bLockReleased) {	
		if (!pProject->pProjMData->getLock()) {
			assert(1==0);
		}
	}

	// Load the DEP
	// The following 2 lines are a fudge so that CDrawProgDoc can get the Project pointer in the case when
	// a project is freshly opened. Need to find a better way.
	pMainFrame->m_wndProjectBar.m_cProjTree.openProject = TRUE;
	pMainFrame->m_wndProjectBar.m_cProjTree.openProj = pProject;
	CDocument* Subsystem = OpenDocumentFile(csProjectDir + "\\" + DEPDIR + csProjectName + ".prg");
	pMainFrame->m_wndProjectBar.m_cProjTree.openProject = FALSE;
	// Add project to MRU file list
	for(int i=0;i < this->m_pRecentFileList->GetSize();i++)
	{
		INXString strFileName(this->m_pRecentFileList->m_arrNames[i]);
	}
	
	this->m_pRecentFileList->Add(csProjectPathName);
	//this->m_pRecentFileList->WriteList();
	for(int i=0;i < this->m_pRecentFileList->GetSize();i++)
	{
		INXString strFileName(this->m_pRecentFileList->m_arrNames[i]);
	}
	return Subsystem;
}

CDocument* CDrawProgApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	INXString ext = (INXString)lpszFileName;
	ext = ext.Right(4);
	// This function is called by MRU file list which is now a project file
	if (ext == ".prg") {
		CDocument* subsys = CWinApp::OpenDocumentFile(lpszFileName);
		// Remove .prg from MRU file list
		for(int i=0;i < this->m_pRecentFileList->GetSize();i++)
		{
			INXString strFileName(this->m_pRecentFileList->m_arrNames[i]);
			if (strFileName.Right(4) == ".prg") {
				this->m_pRecentFileList->Remove(i);
			}
		}
		return subsys;
	}
	else if (ext == ".lpj") {

		INXString pathName = (INXString)lpszFileName + "\\..";
		ProjectMetaData* pProjMData = new ProjectMetaData((CString)pathName);

		if(!pProjMData->getLock()){

			AfxMessageBox( PMD_LOCK_FAILURE_MESSAGE );

		}else{
			LucidErrEnum err = pProjMData->readProjectFile((INXString)lpszFileName);
			pProjMData->releaseLock();

			if (kErr_NoErr == err) {
				return OpenProject(pProjMData);
			}
			else {
#ifdef _DEBUG
				AfxMessageBox("OpenDocumentFile() - readProjectFile returned an error value");
#endif
			}

		}

	}
	return NULL;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetDlgItemTextA( ID_VERSION_DLG, LAB_REVISION_ID );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDrawProgApp::OnHelpLabUserGuide()
{
	openDocInBackground( kLabUserGuide );
}

void CDrawProgApp::OnHelpLabRefManual()
{
	openDocInBackground( kLabRefManual );
}

void CDrawProgApp::OnHelpTutorials()
{
	openDocInBackground( kLgbLabTutorial );
}

BOOL CDrawProgApp::OnIdle(LONG lCount)
{
	Sleep(50);
	// Force idle to update toolbar ie force lCount to -1. Required for debugger step mode.
    return CWinApp::OnIdle(-1);
}


