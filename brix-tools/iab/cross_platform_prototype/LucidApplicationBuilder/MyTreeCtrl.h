#ifndef _MYTREECTRL_H
#define _MYTREECTRL_H

#include "Condat.h"
#include "DEP.h"
#include "Project.h"
#include "Porting_Classes/INXObjList.h"

#include "wx/treectrl.h"

class MyTreeCtrl:public wxTreeCtrl
{
public:
	enum
    {
        TreeCtrlIcon_File,
        TreeCtrlIcon_FileSelected,
        TreeCtrlIcon_Folder,
        TreeCtrlIcon_FolderSelected,
        TreeCtrlIcon_FolderOpened
    };

	MyTreeCtrl();
	MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
					   long style);
	virtual ~MyTreeCtrl();
	Project *pProject[MAX_PROJECTS];
	bool openProject;
	Project *openProj;
	Project* AddProject(int projectNum, ProjectMetaData* pProjMData); 
	void DeleteProject(int projectNum);

	 DECLARE_DYNAMIC_CLASS(MyTreeCtrl)
};


#endif //_MYTREECTRL_H