#include "MyTreeCtrl.h"


IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxTreeCtrl)

MyTreeCtrl::MyTreeCtrl()
{
	openProject = true;
	for (int i=0; i<MAX_PROJECTS; i++) {
		pProject[i] = NULL;
	}
}

MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
					   long style): wxTreeCtrl(parent, id, pos, size, style){

}

MyTreeCtrl::~MyTreeCtrl(void)
{
	for (int i=0; i<MAX_PROJECTS; i++) {
		DeleteProject(i);
	}
}

Project* MyTreeCtrl::AddProject(int projectNum, ProjectMetaData* pProjMData) {
	return pProject[projectNum] = new Project(pProjMData, projectNum);
}
void MyTreeCtrl::DeleteProject(int projectNum) {

	if (projectNum >= 0 && projectNum < MAX_PROJECTS) {
		if (pProject[projectNum]) {
			delete pProject[projectNum];
			pProject[projectNum] = NULL;
		}
	}
}
