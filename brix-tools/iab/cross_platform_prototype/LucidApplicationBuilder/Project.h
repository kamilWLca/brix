// Project.h: interface for the Project class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PROJECT_H_INCLUDED_)
#define _PROJECT_H_INCLUDED_

/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "DEP.h"
#include "LucidConstants.h"
#include "Group.h"
#include "Debugger.h"
#include "ProjectMetaData.h" //@todo
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjList.h"

class Project  
{
public:
	Project();
	Project(ProjectMetaData* _pProjMData, int _projectNum);
	virtual ~Project();
public:
	// Attributes
	ProjectMetaData* pProjMData;
	int projectNum;
	int DEPNum;
	DEP *pDEP[MAX_HIER_LEVELS * MAX_BLOCKS];
	INXObjList* flattened;
	Debugger debug;
private:
	bool m_bDefineMonitors;

public:
	// Methods
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void AddBlockPort(ConData* blob, int iDataType, int iPortType, INXString portLabel, DEP* pDEP);
	#endif
	DEP* AddDEP();
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void AddIconToGroup(INXPOSITION selectedIcon, int groupID, DEP* vwDEP);
	ConData* AddXPort(DEP* pParentDEP, INXString type, INXString portLabel, INXPoint point, DEP* pDEP);
	void AssignHierLineID();
	void PropagateHierLineId(ConData* blob, HTREEITEM hItem, long hierID, int portType, int iPortNum);
	void AssignSubBlockHierLineId(HTREEITEM hItem);
	void DebugStart(int mode);
	#endif
	void DeleteDEP(DEP* theDEP);
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void DeleteBlockPort(INXPOSITION blockPos, int portNum, int portType, DEP* pDEP);
	#endif
	void DeleteUnused(DEP* thisDEP);
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
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
	#endif
	void SaveProjectDep();
	void SaveProject();
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
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
	#endif
	// Define Monitors
	bool getDefineMonitors();
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE	
	void setDefineMonitors(bool bDefineMonitors);
	#endif
	// National Language Support
	bool RunNlsExec();
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void OpenNlsFile();
	#endif

public:
	bool m_bTransferAllHasOccurred;

private:
	long m_iHierIdCount;
};

#endif 
