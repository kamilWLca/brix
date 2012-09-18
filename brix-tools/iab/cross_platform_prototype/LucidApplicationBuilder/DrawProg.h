#ifndef _DRAWPROG_H
#define _DRAWPROG_H

#include <wx/app.h>
#include <wx/event.h>
#include <wx/docview.h>
#include "MainFrame.h"
#include "Resource.h"
#include "LucidConstants.h"
#include "ProjectMetaData.h"
#include "MyTreeCtrl.h"
#include "DEP.h"
#include "Project.h"

class DrawProg: public wxApp
{
public:
	DrawProg(void);
	bool OnInit();
	~DrawProg(void);

	wxDocument* OpenProject(ProjectMetaData* pProjMData);
	wxDocument* OpenDocumentFile(INXString lpszFileName);
	bool SetWorkDirGlobal(wxChar * workDir);
	bool CheckExecutableFolderContextIsOk();
	bool GetInstallationBaseDir(INXString &installationBaseDir);
	void GetExecutableName(INXString & execName);
	void OnSaveProject(wxCommandEvent& event);
	void OnNewProject(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
public:
	MainFrame* m_frame;
	DEP *pDEP;
private:
	MyTreeCtrl m_cProjTree;
private:
	void displayView(Project *proj, INXString doc_file);

};

#endif



