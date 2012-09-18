// Project.h: interface for the Project class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT_H__BAE59696_6D9D_4162_AF44_02C956BA846F__INCLUDED_)
#define AFX_PROJECT_H__BAE59696_6D9D_4162_AF44_02C956BA846F__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DEP.h"
#include "LucidConstants.h"
#include "Group.h"
#include "Debugger.h"
#include "ProjectMetaData.h"
#include "Porting_Classes/INXString.h"

class Project  
{
public:
	Project();
	Project(ProjectMetaData* _pProjMData, int _projectNum);
	virtual ~Project();

	// Attributes
	ProjectMetaData* pProjMData;
	int projectNum;
	int DEPNum;
	DEP* pDEP[MAX_HIER_LEVELS * MAX_BLOCKS];
	INXObjList* flattened;
	Debugger debug;
private:
	bool m_bDefineMonitors;

public:
	// Methods
	void AddBlockPort(ConData* blob, int iDataType, int iPortType, INXString portLabel, DEP* pDEP);
	DEP* AddDEP();
	void AddIconToGroup(INXPOSITION selectedIcon, int groupID, DEP* vwDEP);
	ConData* AddXPort(DEP* pParentDEP, INXString type, INXString portLabel, INXPoint point, DEP* pDEP);
	void AssignHierLineID();
	void PropagateHierLineId(ConData* blob, HTREEITEM hItem, long hierID, int portType, int iPortNum);
	void AssignSubBlockHierLineId(HTREEITEM hItem);
	void DebugStart(int mode);
	void DeleteDEP(DEP* theDEP);
	void DeleteBlockPort(INXPOSITION blockPos, int portNum, int portType, DEP* pDEP);
	void DeleteUnused(DEP* thisDEP);
	void DeleteXPort(DEP* pParentDEP, INXString portLabel, INXString blockName);
	void EditGroupSetup();
	//DEP* GetDEPPtr(INXString depFilename);
	int LoadProg(INXString Info);
	CDrawProgView* OpenView(INXString depFilename);
	void RenameBlockPort(INXPOSITION blockPos, int iPortNum, int iPortType, DEP* pDEP);
	void RenameXport(INXPOSITION iconPos, DEP* pDEP, INXString &csNewPortLabel);
	void ResetDebug();
	void SaveHierName();
	void SaveProg(INXString Info);
	void SaveProjectDep();
	void SaveProject();
	INXObjList* WriteSODL(INXString sodlfile);

	// Tag Management
	void addGuiWidget(ConData* icon);
	void removeGuiWidget(ConData* icon);
	void addPastedGuiWidgets(HTREEITEM hItem, INXObjList* pasteList);
	void removeWidgetsInUserDefBlock(HTREEITEM hItem);
	void getWidgetGroupNames(HTREEITEM hItem, set<INXString> &sWidgetGroupSet); 
	void setWidgetGroupNames(set<INXString> sWidgetGroupSet, vector<pair<INXString, INXString> > &vWidgetGroupPairVec);
	void updateWidgetGroupNames(HTREEITEM hItem, vector<pair<INXString, INXString> > vWidgetGroupPairVec);
	void updateWidgetGroup(ConData* pBlob, vector<pair<INXString, INXString> > vWidgetGroupPairVec);
	bool IsPastedGuiWidgetsInProject(INXObjList* pPasteList, HTREEITEM hItem);
	void GetWidget(const ConData* pBlob, GuiWidget &xWidget);

	DEP* depIsLoaded(HTREEITEM hItem);
	void closeUserDefDoc(HTREEITEM hItem);

	// Define Monitors
	bool getDefineMonitors();
	void setDefineMonitors(bool bDefineMonitors);

	// National Language Support
	bool RunNlsExec();
	void OpenNlsFile();
	

public:
	bool m_bTransferAllHasOccurred;

private:
	long m_iHierIdCount;
};

#endif 