// DrawProgDoc.cpp : implementation of the CDrawProgDoc class
//

/* 
Draw prog data handling class

  */

#include "stdafx.h"
#include "DrawProg.h"
#include "condat.h"
#include "DrawProgView.h"
#include "DrawProgDoc.h"

//#include "ncftp_functions.h"
//_CRTIMP int * __cdecl _errno(void);
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];


/////////////////////////////////////////////////////////////////////////////
// CDrawProgDoc construction/destruction

CDrawProgDoc::CDrawProgDoc(){
	// TODO: add one-time construction code here
	closing = FALSE;
	pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pProject = NULL;
	pDEP = NULL;
}

CDrawProgDoc::~CDrawProgDoc()
{
	// delete project from project tree
	if (pProject) {
		/*
		if (pDEP->depFilename == pProject->projectName) {
			pFrame->m_wndProjectBar.m_cProjTree.DeleteItem(pDEP->hItem);
			//pFrame->m_wndProjectBar.m_cProjTree.DeleteTreeItem(pProject->projectNum, 0, 0);
			int projectNum = pProject->projectNum;
			pFrame->m_wndProjectBar.m_cProjTree.DeleteProject(projectNum);
		}
		else {
			pProject->DeleteDEP(pDEP);
		}*/
		pProject->DeleteDEP(pDEP);
	}
}

/*
BOOL CDrawProgDoc::OnNewDocument()
{
	INXString filename;
	//return FALSE;

	if (!CDocument::OnNewDocument())
		return FALSE;

	bool projectSpace = FALSE;
	
	// set the project number. There are two approaches. 1. When a project is deleted shift the projects.
	// 2. the approach used is to find a project number that is avaialable.
	for (int i=0; i<MAX_PROJECTS; i++) {
		if (pFrame->m_wndProjectBar.m_cProjTree.pProject[i] == NULL) {
			pProject = pFrame->m_wndProjectBar.m_cProjTree.AddProject(i);
			pProject->projectNum = i;
			i = MAX_PROJECTS;
			projectSpace = TRUE;
			// initialise the block count array
			for (int j=0; j<MAX_HIER_LEVELS; j++) {
				pFrame->m_wndProjectBar.m_cProjTree.blockNumArr[pProject->projectNum][j] = 0;
			}
		}
	}
	if (!projectSpace) {
		AfxMessageBox("Unable to create new project. DrawProg contains the maximum projects allowed.");
		return FALSE;
	}
	
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	INXString strPath = "";
	INXString strYourCaption(_T("Project Creator"));
	INXString strYourTitle(_T("Select a new project directory."));

	CPathDialog dlg(strYourCaption, strYourTitle, strPath);
	if(dlg.DoModal()==IDOK)
	{
		pProject->projectDir = dlg.GetPathName();
		// Extract project name. This is the same as the actual project directory
		filename = pProject->projectDir;
		filename.MakeReverse();
		filename = filename.SpanExcluding("\\");
		filename.MakeReverse();
		pProject->projectName = filename;

		CreateDirectory(pProject->projectDir + "\\DEP", NULL);
		CreateDirectory(pProject->projectDir + "\\SODL", NULL);
		CreateDirectory(pProject->projectDir + "\\DeviceData", NULL);
		CreateDirectory(pProject->projectDir + "\\DeviceData\\GUI", NULL);
		CreateDirectory(pProject->projectDir + "\\DeviceData\\data", NULL);
		SetTitle(pProject->projectName);

		// populate the treectrl with the new project name
		pFrame->m_wndProjectBar.m_cProjTree.projTreeItem[pProject->projectNum][0][0] = pFrame->m_wndProjectBar.m_cProjTree.InsertItem(_T(pProject->projectName));
		
		// set project data for this project item
		pDEP = pProject->AddDEP();
		pDEP->depFilename = filename;

		POSITION pos = GetFirstViewPosition();
		CDrawProgView* pView = (CDrawProgView*) GetNextView(pos);
		pView->pProject = pProject;
		pView->pDEP = pDEP;
		pView->initUndo();

		return TRUE;
	}

	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CDrawProgDoc serialization

void CDrawProgDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CDrawProgDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	INXString filename;
	int len;
	int response;

	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	double dFreeDiskSpace = pApp->MyGetFreeDiskSpaceMB();
	if (dFreeDiskSpace < DISK_SPACE_WARNING_MB) {
		response = AfxMessageBox("Warning, less than 50MB of free disk space remaining.\nYou may not be able save your work.\nContinue?",MB_YESNO | MB_ICONWARNING);
		if (response == IDNO) {
			return FALSE;
		}
	} else if (dFreeDiskSpace < DISK_SPACE_CRITICAL_MB) {
		AfxMessageBox("Unable to open project, less than 10MB of free disk space remaining.",MB_ICONWARNING);
		return FALSE;
	}



	// Extract filename
	filename = (INXString)lpszPathName;
	//filename = filename.SpanExcluding(".");
	filename.MakeReverse();
	filename = filename.SpanExcluding("\\");
	filename.MakeReverse();
	// remove .prg to get filename
	len = filename.GetLength() - 4;
	filename = filename.Left(len);

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	POSITION pos = GetFirstViewPosition();
	CDrawProgView* pView = (CDrawProgView*) GetNextView(pos);
	
	if (pFrame->m_wndProjectBar.m_cProjTree.openProject) {
		pProject = pFrame->m_wndProjectBar.m_cProjTree.openProj;
	}
	else {
		pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(pFrame->m_wndProjectBar.m_cProjTree.hSelItem);
	}

	// Check Folder Structure
	INXString csProjDir;

	pProject->pProjMData->initProjFolderMinder(); 

	if(pProject->pProjMData->folderStructureNotOk()){
		AfxMessageBox("Unable to open this project, because the directory structure is invalid");
		return kErr_InvalidFolderStructure;
	}



	// Create DEP, load and initialise
	pDEP = pProject->AddDEP();
	pDEP->LoadProg((INXString)lpszPathName);
	pDEP->InitTagLists();
	pDEP->depFilename = filename;
	// Populate project tree if opening a project. Uses the loaded DEP to populate the tree
	if (pFrame->m_wndProjectBar.m_cProjTree.openProject) {
		pFrame->m_wndProjectBar.m_cProjTree.PopulateProjectTree(pDEP->condata, pProject);
	}
	pDEP->hItem = pFrame->m_wndProjectBar.m_cProjTree.hSelItem;

	// Set the project and DEP pointers in the view
	pView->pProject = pProject;
	pView->pDEP = pDEP;
	pView->RedrawWindow();

	// Need to initialise undo for case when the view is already open
	pView->initUndo();

	return TRUE;
}

BOOL CDrawProgDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	OnFileSave();
	return 1;//CDocument::OnSaveDocument(lpszPathName);
}

/*
BOOL CDrawProgDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	//lpszPathName = projectDir + "\\DEP\\" + projectName + ".prg"; 
	INXString sdlFile = lpszPathName;
	INXString filename = lpszPathName;
	// Remove .prg
	sdlFile = sdlFile.SpanExcluding(".");
	sdlFile = sdlFile + ".sdl";
	// TODO: Add your specialized code here and/or call the base class
	
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CDrawProgView *pView = (CDrawProgView *) pChild->GetActiveView();
	
	// Originally the above code was used to point to the view to save.
	// However, when saving multiple views it only saved the active view to all files.
	// The following code saves the correct view to the correct file
	POSITION pos = GetFirstViewPosition();
	CDrawProgView* pView = (CDrawProgView*) GetNextView(pos);
	
	pView->oldDepFilename = pView->depFilename;
	// Extract filename
	filename = filename.SpanExcluding(".");
	filename.MakeReverse();
	filename = filename.SpanExcluding("\\");
	filename.MakeReverse();
	pView->depFilename = filename;	

	//AfxMessageBox(lpszPathName);
	//pView->SaveProg(lpszPathName);
	pView->SaveProg(projectDir + "\\DEP\\" + projectName + ".prg");
	pView->WriteSDL((LPCTSTR)sdlFile);

	//runftp();
	
	int Code;                                                        
    // attach FCE                                                 
    Code = fceAttach(1, 0); // KEY_CODE = 0                          
    if(Code<0) AfxMessageBox("Cannot Attach");
	Code = fceConnect(0,(LPSTR)"192.168.254.176",               
    (LPSTR)"sam", (LPSTR)"x");    
    if(Code<0) AfxMessageBox("Cannot connect");;                                      
    // change to proper directory                                 
    Code = fceSetServerDir(0, (LPSTR)"/home/sam");          
    if(Code<0) AfxMessageBox("Cannot set server dir.");                                      
    // set to ASCII xfer mode                                      
    fceSetMode(0,'A');                                               
    // upload the file                                          
    Code = fcePutFile(0,(LPSTR)"t.sdl");                      
    if(Code<0) AfxMessageBox("Couldn't upload file");                                      
    // QUIT                                                       
    fceClose(0);                                                     
    fceRelease();
	

	//pFrame->MDINext();
	return 1;//CDocument::OnSaveDocument(lpszPathName);
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDrawProgDoc diagnostics

#ifdef _DEBUG
void CDrawProgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDrawProgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDrawProgDoc::OnFileSave() 
{
	INXString depFilename, csProjectDir;
	INXString sdlFile;

	pProject->pProjMData->getProjectDir(csProjectDir);
	if (csProjectDir == (INXString)workDir + USERDEFDIR) {
		AfxMessageBox("Saving a temporary copy to temporary.dep. To create a reusable subsystem use the pink file save button on the toolbar");
			POSITION pos = GetFirstViewPosition();
			CDrawProgView* pView = (CDrawProgView*) GetNextView(pos);
			pDEP->SaveProg("temporary.dep");
			pView->SetWindowText("temporay.dep");
			//pView->depFilename="temporay.dep";
			pDEP->depFilename = "temporay.dep";
		return;
	}
	
	// if a library component
	if (csProjectDir == (INXString)workDir + USERDEFDIR) {
		depFilename = csProjectDir + "\\" + pDEP->depFilename + ".prg"; 
		sdlFile = csProjectDir + "\\t.sdl";
	}
	else
	{
		depFilename = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + "\\" +
			pDEP->depFilename + ".prg"; 
		sdlFile = csProjectDir + "\\SODL\\t.sdl";
	}

	// TODO: Add your specialized code here and/or call the base class
	// Originally the above code was used to point to the view to save.
	// However, when saving multiple views it only saved the active view to all files.
	// The following code saves the correct view to the correct file
	POSITION pos = GetFirstViewPosition();
	CDrawProgView* pView = (CDrawProgView*) GetNextView(pos);
	
	if (pDEP) {
		pDEP->SaveProg(depFilename);

		SetModifiedFlag(FALSE);
	}
}

void CDrawProgDoc::OnFileSaveAs() 
{
	//OnFileSave();
	CDocument::OnFileSaveAs();
}

// disable file save as menu item
void CDrawProgDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	INXString csProjectDir;
	pProject->pProjMData->getProjectDir(csProjectDir);
	pCmdUI->Enable(csProjectDir == (INXString)workDir + USERDEFDIR);	
}

void CDrawProgDoc::OnUpdateNewsubsystem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}


void CDrawProgDoc::OnNewsubsystem() 
{
	INXString csProjectDir;
	pProject->pProjMData->getProjectDir(csProjectDir);
	csProjectDir == (INXString)workDir + USERDEFDIR;
	//InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

}

void CDrawProgDoc::OnCloseDocument() 
{
	INXString csProjectName;
	/* brings up an assertion when a sub-block is modified
	if (IsModified()) {
		INXString message = "Save changes to " + pDEP->depFilename + ".prg?";
		int response = AfxMessageBox(message ,MB_YESNO);
		if (response == IDYES) {
			OnFileSave();
		}
	}
	*/

	// if exiting application then don't close project here
	if (!((CMainFrame*)AfxGetApp()->m_pMainWnd)->exiting) {
		if (pDEP && pProject) {
			// if closing the project top level view close the project
			pProject->pProjMData->getProjectName(csProjectName);
			if (pDEP->depFilename == csProjectName && !closing) {
				// closing project so stop debugger if its running
				if (pProject->debug.debugMode != DBGSTOP) {
					pProject->debug.DebugStop();
					pProject->debug.setView(NULL);
				}
				closing = TRUE;
				pFrame->m_wndProjectBar.m_cProjTree.CloseProject(pDEP->hItem, pProject);

				return;
			}
		}
	}
	else {
		// exiting app so stop debugger if its running
		if (pProject->debug.debugMode != DBGSTOP) {
			pProject->debug.DebugStop();
			pProject->debug.setView(NULL);
		}
		// Clean up project when closing app. 
		// Used to be done when saving project. However, project is saved after each action now.
		// Don't want to delete unused when saving project, because can't do undo for encapsulated blocks.
		pProject->DeleteUnused(pProject->pDEP[0]);
	}

	CDocument::OnCloseDocument();
}

BOOL CDrawProgDoc::SaveModified() 
{
	// TODO: Add your specialized code here and/or call the base class	
	// Don't prompt user to save as should be saved already
	SetModifiedFlag(FALSE);
	return CDocument::SaveModified();
	//return TRUE;
}

// OnFileSave is protected so call it from this function which is public
void CDrawProgDoc::SaveUserDefDoc() {
	OnFileSave();
}

void CDrawProgDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModified());		
}

void CDrawProgDoc::OnUpdateSaveProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(((CDrawProgApp*)AfxGetApp())->IsModified(pProject));			
}
