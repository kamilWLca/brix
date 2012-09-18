// Project.cpp: implementation of the Project class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DrawProg.h"
#include "Project.h"
#include "DrawProgView.h"
//#include "SODL.h" //@todo
//#include "GroupSetupDialog.h" //@todo
#include "FileOperations.h"
#include "BlockOperations.h" //@todo
//#include "Markup.h" //@todo
//#include "ExtDataFile.h" //@todo
//#include "BlockPortLabelDialog.h" //@todo
#include "GlobalFuncs_2.h"
#include "LucidEnums.h"
//#include "WidgetGroupManagerDlg.h" //@todo
#include <wx/msgdlg.h>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

//LucidTcpipClient tcpClient; //@todo


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Project::Project()
: m_bTransferAllHasOccurred(false)
{
	projectNum = -1;
	DEPNum = -1;
	m_bDefineMonitors = FALSE;
	flattened = new INXObjList;
	m_iHierIdCount = 0;
}

Project::Project(ProjectMetaData* _pProjMData, int _projectNum)
: m_bTransferAllHasOccurred(false)
{
	projectNum = _projectNum;
	pProjMData = _pProjMData;
	DEPNum = -1;
	m_bDefineMonitors = FALSE;
	flattened = new INXObjList;
}

Project::~Project()
{
	BlockOperations bo;
	bo.DeleteBlock(flattened);
	delete pProjMData;
}

DEP* Project::AddDEP() {
	DEPNum++;
	pDEP[DEPNum] = new DEP;
	INXString csProjectDir;

	pProjMData->getProjectDir(csProjectDir);
	pDEP[DEPNum]->projectDir = csProjectDir;
	pDEP[DEPNum]->projectNum = projectNum;
	return pDEP[DEPNum];
}

void Project::DeleteDEP(DEP* thisDEP) {
	bool shift = FALSE;

	for (int i=0; i<=DEPNum; i++) {
		if (thisDEP == pDEP[i]) {
			delete thisDEP;
			shift = TRUE;
		}
		// shift the project item pointers
		if (shift && i<DEPNum) {
			pDEP[i] = pDEP[i+1];
		}
	}
	DEPNum--;
}

// Deletes all the unused files that are no longer in the DEP
void Project::DeleteUnused(DEP* thisDEP) {
#ifdef TRY_EXCLUDE
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CFileFind finder;
	INXString fileName, depPath, csProjectDir;
	INXPOSITION pos;
	ConData* blob;
	bool fileExist = FALSE;
	CFileOperation fo;
	
	pProjMData->getProjectDir(csProjectDir);
	depPath = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(thisDEP->hItem) + thisDEP->depFilename + "\\";
	int bWorking = finder.FindFile(depPath + "*.prg");	
	while (bWorking)
	{
		fileExist = FALSE;
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		fileName.MakeReverse();
		fileName.Delete(0,4);
		fileName.MakeReverse();
		// check if the file exists in the DEP
		pos = thisDEP->condata->GetHeadPosition();
		while (pos) {
			blob = (ConData*) thisDEP->condata->GetNext(pos);
			if (blob->description == fileName) {
				fileExist = TRUE;
			}
		}
		if (!fileExist) {
		#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
			fo.Delete(depPath + fileName + ".prg");
			fo.Delete(depPath + fileName);
		#endif
		}
	}
#endif
}

/* @todo Somwhere in this shocking code there is something that idenfies if the bitmap file is one of a list of ones used to represent
x-ports and changes the function box display accordingly. Obviously this should be using the class name in IDF instead. This needs sorting out! */ 

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Adds a XPort to a DEP and a block port to its parent DEP
ConData* Project::AddXPort(DEP* pParentDEP, INXString type, INXString portLabel, INXPoint point, DEP* pDEP) {
	if (pParentDEP->AddBlockPort(type, portLabel, pDEP->depFilename)) {
		return pDEP->AddXPort(type, portLabel, point);
	}
	else {
		return NULL;
	}
}

// Adds a port to a block and a xport to its child DEP
void Project::AddBlockPort(ConData* blob, int iDataType, int iPortType, INXString portLabel, DEP* pDEP) {
	DEP* pChildDEP;
	INXString depPath, csProjectDir;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	HTREEITEM childItem;

	// get the type and point	
	INXPoint point = blob->CalculateXPortPosition(iPortType);
	INXString type = blob->BuildXPortString(iPortType,iDataType);

	pProjMData->getProjectDir(csProjectDir);
	if (pDEP->AddPort(blob, iDataType, iPortType, portLabel)) {

		// open subblock so that xport can be added. Doing it this way means don't have to save to 
		// a file. Can cause problems if save to a file and don't save the added port
		childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, pDEP->hItem);
		depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem);
		pFrame->m_wndProjectBar.m_cProjTree.hSelItem = childItem;
		CDrawProgView* pView = OpenView(csProjectDir + DEPDIR + depPath + blob->description + ".prg");
		pChildDEP = pView->pDEP;
		pChildDEP->AddXPort(type, portLabel, point);
		// set modified flag in doc
		//pView->pDoc->SetModifiedFlag(TRUE);
		pFrame->m_wndProjectBar.m_cProjTree.hSelItem = pDEP->hItem;
		// save both DEPs so they are consistent
		pDEP->SaveProg(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");
		pChildDEP->SaveProg(csProjectDir + DEPDIR + depPath + blob->description + ".prg");
		// open original DEP in window
		CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + 
			pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");	
		//OpenDEP(projectDir + DEPDIR + pDEP->depFilename + ".prg");
	}
}

// Sets the groupID in all the start ports and internal ports for the specified icon
void Project::AddIconToGroup(INXPOSITION selectedIcon, int groupID, DEP* vwDEP)
{
	ConData* blob;
	INXPOSITION pos;
	DEP* pChildDEP;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	HTREEITEM childItem;
	INXString depPath, csProjectDir;

	pProjMData->getProjectDir(csProjectDir);
	blob = (ConData*) vwDEP->condata->GetAt(selectedIcon);
	if (blob->m_iUserDefined) {
		// open userdefined block and add all the startports to the group
		pFrame->m_wndProjectBar.m_cProjTree.openProject = FALSE;
		childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, vwDEP->hItem);
		depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem);
		pFrame->m_wndProjectBar.m_cProjTree.hSelItem = childItem;
		pChildDEP = (OpenView(csProjectDir + DEPDIR + depPath + blob->description + ".prg"))->pDEP;
		pFrame->m_wndProjectBar.m_cProjTree.openProject = TRUE;
		pos = pChildDEP->condata->GetHeadPosition();
		while (pos) {
			AddIconToGroup(pos, groupID, pChildDEP);
			pChildDEP->condata->GetNext(pos);
		}
		pFrame->m_wndProjectBar.m_cProjTree.hSelItem = vwDEP->hItem;
		CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + 
			pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(vwDEP->hItem) + vwDEP->depFilename + ".prg");
	}
	else {
		vwDEP->AddIconToGroup(selectedIcon, groupID);
	}
}

// Assigns a hierarchical line ID to lines connected to userdefined blocks and lines connected to XINPUT ports to simplify mapping 
// in debug mode
void Project::AssignHierLineID()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXPOSITION pos, subpos;
	ConData *blob, *subblob, *othersubblob;
	long hierID = 0;
	INXObjList* tmpList;
	INXString filename, csProjectDir;
	HTREEITEM hChild, hItem;
	BlockOperations bo;
	UINT i;

	CanvasSupport cs;
	cs.SetDebugTrace(false);

	m_iHierIdCount = 0;
	cs.DebugTrace("sdg: assignhierlineid\n");
	pProjMData->getProjectDir(csProjectDir);
	pos = pDEP[0]->condata->GetHeadPosition();
	while (pos) {

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// For Each function-block in the top level dep
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		blob = (ConData*) pDEP[0]->condata->GetNext(pos);
		cs.DebugTrace("sdg: blob m_csIconType is: %s\n", blob->m_csIconType);


		// assign hierID to userdefined block and icon in sub block the input is connected to
		if (blob->m_iUserDefined) {

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// If the function-block is a sub-block (ie user-defined)
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// If the function-block is a sub-block (ie user-defined)
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			hItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, pDEP[0]->hItem);
			cs.DebugTrace("sdg: GetUserDefChildItem\n");

			filename = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + 
				blob->description + ".prg";
			cs.DebugTrace("sdg: Filename: %s\n", filename);

			tmpList = bo.LoadBlock(filename);
			cs.DebugTrace("sdg: LoadBlock\n");

			for (i=0; i<blob->inputport_num; i++) {

				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// For each input-port of the (top-level, user-defined) sub-block
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (blob->inputport[i]->line.exist) {
					hierID = m_iHierIdCount;
				}
				// Need to set unconnected ports to -1, because could have case where it is connected in sub-block
				// and has a value that was set in a previous call to this function
				else {
					hierID = -1;
				}
				
				blob->inputport[i]->line.hierID = hierID;
				subpos = tmpList->GetHeadPosition();

				while (subpos) {

					// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					// For each function-block in the sub-block
					// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

					subblob = (ConData*) tmpList->GetNext(subpos);
					cs.DebugTrace("sdg: input subblob m_csIconType is: %s\n", subblob->m_csIconType);
					// find icon port connected to XINPUT that has same description as user defined input

					for (UINT j=0; j<subblob->inputport_num; j++) {

						if (subblob->inputport[j]->line.exist) {
							othersubblob = bo.GetBlockIconFromID(subblob->inputport[j]->line.othericonid, tmpList);

							if (othersubblob->m_csIconType.Find("XINPUT") != -1 && 
								othersubblob->description == blob->inputport[i]->description) {
								subblob->inputport[j]->line.hierID = hierID;
								// This if block used to be outside the one its nested in
								// Need to check it still works with the debugger
								if (subblob->m_iUserDefined) {
									hChild = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(subblob, hItem);
									PropagateHierLineId(subblob, hChild, hierID, INPUTPORT, j);
								}
							}
						}
					}

				} // while (subpos)
				if (blob->inputport[i]->line.exist) {
					m_iHierIdCount++;
				}
			} // for (i=0; i<blob->inputport_num; i++)

			cs.DebugTrace("sdg: end input ports\n");

			for (i=0; i<blob->startport_num; i++) {
				if (blob->startport[i]->line.exist) {
					hierID = m_iHierIdCount;
				}
				// Need to set unconnected ports to -1, because could have case where it is connected in sub-block
				// and has a value that was set in a previous call to this function
				else {
					hierID = -1;
				}

				blob->startport[i]->line.hierID = hierID;
				subpos = tmpList->GetHeadPosition();

				while (subpos) {
					subblob = (ConData*) tmpList->GetNext(subpos);
					cs.DebugTrace("sdg: start subblob m_csIconType is: %s\n", subblob->m_csIconType);
					// find icon port connected to XINPUT that has same description as user defined input
					for (UINT j=0; j<subblob->startport_num; j++) {
						if (subblob->startport[j]->line.exist) {
							othersubblob = bo.GetBlockIconFromID(subblob->startport[j]->line.othericonid, tmpList);
							if (othersubblob->m_csIconType.Find("XSTART") != -1 && othersubblob->description == blob->startport[i]->description) {
								subblob->startport[j]->line.hierID = hierID;
								// This if block used to be outside the one its nested in
								// Need to check it still works with the debugger
								if (subblob->m_iUserDefined) {
									cs.DebugTrace("sdg: subblob block: %s\n", subblob->m_csBlockName);
									cs.DebugTrace("sdg: subblob GetUserDefChildItem\n");
									hChild = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(subblob, hItem);
									cs.DebugTrace("sdg: PropagateHierLineId\n");
									PropagateHierLineId(subblob, hChild, hierID, STARTPORT, j);
									cs.DebugTrace("sdg: end PropagateHierLineId\n");
								}
							}
						}
					}
				}
				if (blob->startport[i]->line.exist) {
					m_iHierIdCount++;
				}
			}
			// iterate through all sub-blocks and assign sub-blocks
			cs.DebugTrace("sdg: end start ports\n");
			bo.SaveBlock(filename, tmpList);
			bo.DeleteBlock(tmpList);
			AssignSubBlockHierLineId(hItem);
		} // if (blob->userDefined)

	}
}

// Same as the AssignHierLineId method except it recursively goes through all subsystem blocks
void Project::AssignSubBlockHierLineId(HTREEITEM hItem)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString csFileName, csProjectDir;
	INXObjList* pTmpList;
	BlockOperations bo;
	INXPOSITION pos;
	ConData *pBlob, *pOtherBlob;
	HTREEITEM hChildItem;
	long iHierId;

	pProjMData->getProjectDir(csProjectDir);
	csFileName = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + 
					(INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + ".prg";

	pTmpList = bo.LoadBlock(csFileName);
	pos = pTmpList->GetHeadPosition();
	while (pos) {
		pBlob = (ConData*) pTmpList->GetNext(pos);
		if (pBlob->m_iUserDefined) {
			hChildItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(pBlob, hItem);
			for (UINT i=0; i<pBlob->inputport_num; i++) {
				if (pBlob->inputport[i]->line.exist) {
					iHierId = m_iHierIdCount;
				}
				else {
					iHierId = -1;
				}

				pOtherBlob = bo.GetBlockIconFromID(pBlob->inputport[i]->line.othericonid, pTmpList);
				// if port is connected to XSTART assume the hierID has already been assigned
				if (pOtherBlob && pOtherBlob->m_csIconType.Find("XINPUT") == -1) {
					pBlob->inputport[i]->line.hierID = iHierId;
					PropagateHierLineId(pBlob, hChildItem, iHierId, INPUTPORT, i);
					if (pBlob->inputport[i]->line.exist) {
						m_iHierIdCount++;
					}
				}
			}
			for (UINT i=0; i<pBlob->startport_num; i++) {
				if (pBlob->startport[i]->line.exist) {
					iHierId = m_iHierIdCount;
				}
				else {
					iHierId = -1;
				}

				pOtherBlob = bo.GetBlockIconFromID(pBlob->startport[i]->line.othericonid, pTmpList);
				// if port is connected to XSTART assume the hierID has already been assigned
				if (pOtherBlob && pOtherBlob->m_csIconType.Find("XSTART") == -1) {
					pBlob->startport[i]->line.hierID = iHierId;
					PropagateHierLineId(pBlob, hChildItem, iHierId, STARTPORT, i);
					if (pBlob->startport[i]->line.exist) {
						m_iHierIdCount++;
					}
				}
			}
			AssignSubBlockHierLineId(hChildItem);
		}
	}
	bo.SaveBlock(csFileName, pTmpList);
	bo.DeleteBlock(pTmpList);
}

// Works together with AssignHierLineID and AssignSubBlockHierId to propagate the hierIds into lower level 
// subsystems. If an XINPUT port is connected to a subsystem then the hierId is propagated into the subsystem block.
void Project::PropagateHierLineId(ConData* blob, HTREEITEM hItem, long hierID, int portType, int iPortNum)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString filename, csProjectDir;
	INXObjList* tmpList;
	INXPOSITION subpos;
	ConData *subblob, *othersubblob;
	HTREEITEM hChild;
	BlockOperations bo;

	CanvasSupport cs;
	cs.SetDebugTrace(false);

	pProjMData->getProjectDir(csProjectDir);
	filename = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + 
		blob->description + ".prg";

	cs.DebugTrace("sdg: subblock Filename: %s\n", filename);
	tmpList = bo.LoadBlock(filename);
	cs.DebugTrace("sdg: subblock LoadBlock\n");

	if (portType == INPUTPORT) {
		subpos = tmpList->GetHeadPosition();
		while (subpos) {
			subblob = (ConData*) tmpList->GetNext(subpos);
			// find icon port connected to XINPUT that has same description as user defined input
			for (UINT j=0; j<subblob->inputport_num; j++) {
				if (subblob->inputport[j]->line.exist) {
					othersubblob = bo.GetBlockIconFromID(subblob->inputport[j]->line.othericonid, tmpList);
					if (othersubblob->m_csIconType.Find("XINPUT") != -1 && othersubblob->description == blob->inputport[iPortNum]->description) {
						subblob->inputport[j]->line.hierID = hierID;
						// This if block used to be outside the one its nested in
						// Need to check it still works with the debugger
						if (subblob->m_iUserDefined) {
							hChild = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(subblob, hItem);
							PropagateHierLineId(subblob, hChild, hierID, INPUTPORT, j);
						}
					}
				}
			}
		}
	}
	else if (portType == STARTPORT) {
		cs.DebugTrace("sdg: subblock start ports\n");
		subpos = tmpList->GetHeadPosition();
		while (subpos) {
			subblob = (ConData*) tmpList->GetNext(subpos);
			//cs.DebugTrace("sdg: subblock start subblob type is: %s\n", subblob->type);
			// find icon port connected to XINPUT that has same description as user defined input
			for (UINT j=0; j<subblob->startport_num; j++) {
				if (subblob->startport[j]->line.exist) {
					othersubblob = bo.GetBlockIconFromID(subblob->startport[j]->line.othericonid, tmpList);
					if (othersubblob->m_csIconType.Find("XSTART") != -1 && othersubblob->description == blob->startport[iPortNum]->description) {
						subblob->startport[j]->line.hierID = hierID;
						cs.DebugTrace("sdg: subblock port label is: %s\n", subblob->startport[j]->description);
						// This if block used to be outside the one its nested in
						// Need to check it still works with the debugger
						if (subblob->m_iUserDefined) {
							cs.DebugTrace("sdg: subblock subblob block: %s\n", subblob->m_csBlockName);
							cs.DebugTrace("sdg: subblock subblob GetUserDefChildItem\n");
							hChild = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(subblob, hItem);
							cs.DebugTrace("sdg: subblock PropagateHierLineId\n");
							PropagateHierLineId(subblob, hChild, hierID, STARTPORT, j);
							cs.DebugTrace("sdg: subblock end PropagateHierLineId\n");
						}
					}
				}
			}	
		}
	}
	cs.DebugTrace("sdg: end subblock start ports\n");
	bo.SaveBlock(filename, tmpList);
	bo.DeleteBlock(tmpList);
}

void Project::DebugStart(int mode) {
	INXString csProjectDir;

	pProjMData->getProjectDir(csProjectDir);
	// Create flattened list by writing out SODL
	AssignHierLineID();
	flattened = WriteSODL(csProjectDir + SODLDIR + SODLFILENAME);
	// reload DEP subblocks since IDs may have changed during flattening
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pDEP[0]->dbgMappedFlag = FALSE;
	//@todo is this reload why it's so slow? Perhaps do this more intellegently with dirty flags?
	for (int i=1; i<=DEPNum; i++) {
		pDEP[i]->condata->RemoveAll();
		pDEP[i]->LoadProg(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP[i]->hItem) + 
			pDEP[i]->depFilename + ".prg");
		pDEP[i]->dbgMappedFlag = FALSE;
	}
	debug.DebugStart(flattened,mode);
}

// delete the port from the block and delete the xport
void Project::DeleteBlockPort(INXPOSITION blockPos, int portNum, int portType, DEP* pDEP) {
	DEP* pChildDEP;
	ConData *block, *icon;
	INXString portLabel, depPath, csProjectDir;
	INXPOSITION pos, prevPos;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	HTREEITEM childItem;

	pProjMData->getProjectDir(csProjectDir);
	block = (ConData*) pDEP->condata->GetAt(blockPos);
	if (!block->m_iUserDefined) {
		wxMessageBox("Can only delete ports on a user defined block.");
		return;
	}

	// Delete block port
	portLabel = pDEP->DeletePort(block, portNum, portType);
	// load child dep in case it doesn't exist
	childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(block, pDEP->hItem);
	depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem);
	pFrame->m_wndProjectBar.m_cProjTree.hSelItem = childItem;
	CDrawProgView* pView = OpenView(csProjectDir + DEPDIR + depPath + block->description + ".prg");
	pChildDEP = pView->pDEP;
	// get xport to delete
	pos = pChildDEP->condata->GetHeadPosition();
	while (pos) {
		prevPos = pos;
		icon = (ConData*) pChildDEP->condata->GetNext(pos);
		if (icon->description == portLabel && (icon->m_csIconType.Find("XINPUT") != -1 ||
			icon->m_csIconType.Find("XOUTPUT") != -1 || icon->m_csIconType == "XSTART" || icon->m_csIconType == "XFINISH")) {
			pChildDEP->DeleteIcon(prevPos, 1);
			// set modified flag in doc
			//pView->pDoc->SetModifiedFlag(TRUE);
		}
	}
	// save both DEPs so they are consistent
	pDEP->SaveProg(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");
	pChildDEP->SaveProg(csProjectDir + DEPDIR + depPath + block->description + ".prg");
	// restore original DEP in view
	pFrame->m_wndProjectBar.m_cProjTree.hSelItem = pDEP->hItem;
	CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + 
		pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");		
}

// deletes a port from a block in a parent DEP
void Project::DeleteXPort(DEP* pParentDEP, INXString portLabel, INXString blockName) {
	INXPOSITION pos, prevPos;
	ConData* icon;
	UINT i;

	pos = pParentDEP->condata->GetHeadPosition();
	while (pos) {
		prevPos = pos;
		icon = (ConData*) pParentDEP->condata->GetNext(pos);
		// find the block and delete the correct port
		if (icon->description == blockName) {
			for (i=0; i<icon->inputport_num; i++) {
				if (icon->inputport[i]->description == portLabel) {
					pParentDEP->DeletePort(icon, i, INPUTPORT);
				}
			}
			for (i=0; i<icon->outputport_num; i++) {
				if (icon->outputport[i]->description == portLabel) {
					pParentDEP->DeletePort(icon, i, OUTPUTPORT);
				}
			}
			for (i=0; i<icon->startport_num; i++) {
				if (icon->startport[i]->description == portLabel) {
					pParentDEP->DeletePort(icon, i, STARTPORT);
				}
			}
			for (i=0; i<icon->finishport_num; i++) {
				if (icon->finishport[i]->description == portLabel) {
					pParentDEP->DeletePort(icon, i, FINISHPORT);
				}
			}
		}
	}
}

void Project::RenameBlockPort(INXPOSITION blockPos, int iPortNum, int iPortType, DEP* pDEP) {
	DEP* pChildDEP;
	ConData *block, *icon;
	INXString csOldPortLabel, csNewPortLabel, depPath, csProjectDir;
	INXPOSITION pos, prevPos;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	HTREEITEM childItem;

	pProjMData->getProjectDir(csProjectDir);
	block = (ConData*) pDEP->condata->GetAt(blockPos);
	if (!block->m_iUserDefined) {
		wxMessageBox("Can only rename ports on a user defined block.");
		return;
	}

	csOldPortLabel = pDEP->GetPortLabel(blockPos, iPortNum, iPortType);
	csNewPortLabel = csOldPortLabel;
	// Prompt user for new port name
	CBlockPortLabelDialog dialog(csOldPortLabel, block, pDEP);
	if (dialog.DoModal() == IDOK) {
		csNewPortLabel = dialog.GetPortLabel();
		csNewPortLabel.MakeLower();
	}

	// Rename block port
	pDEP->SetPortLabel(blockPos, iPortNum, iPortType, csNewPortLabel);
	// load child dep in case it doesn't exist
	childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(block, pDEP->hItem);
	depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem);
	pFrame->m_wndProjectBar.m_cProjTree.hSelItem = childItem;
	CDrawProgView* pView = OpenView(csProjectDir + DEPDIR + depPath + block->description + ".prg");
	pChildDEP = pView->pDEP;
	// get xport to delete
	pos = pChildDEP->condata->GetHeadPosition();
	while (pos) {
		prevPos = pos;
		icon = (ConData*) pChildDEP->condata->GetNext(pos);
		if (icon->description == csOldPortLabel && (icon->m_csIconType.Find("XINPUT") != -1 ||
			icon->m_csIconType.Find("XOUTPUT") != -1 || icon->m_csIconType == "XSTART" || icon->m_csIconType == "XFINISH")) {
				icon->description = csNewPortLabel;
		}
	}
	// save both DEPs so they are consistent
	pDEP->SaveProg(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");
	pChildDEP->SaveProg(csProjectDir + DEPDIR + depPath + block->description + ".prg");
	// restore original DEP in view
	pFrame->m_wndProjectBar.m_cProjTree.hSelItem = pDEP->hItem;
	CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(csProjectDir + DEPDIR + 
		pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + ".prg");	
}

void Project::RenameXport(INXPOSITION iconPos, DEP* pDEP, INXString &csNewPortLabel) 
{
	INXString csProjectDir, csOldPortLabel;
	ConData* icon;

	pProjMData->getProjectDir(csProjectDir);
	icon = (ConData*) pDEP->condata->GetAt(iconPos);
	if (icon->m_csIconType.Find("XINPUT") == -1 && icon->m_csIconType.Find("XOUTPUT") == -1 && 
		icon->m_csIconType != "XSTART" && icon->m_csIconType != "XFINISH") {
		wxMessageBox("Can only rename Xports.");
		return;
	}

	csOldPortLabel = icon->description;
	csNewPortLabel = icon->description;
	// Prompt user for new port name
	CBlockPortLabelDialog dialog(csOldPortLabel, icon, pDEP);
	if (dialog.DoModal() == IDOK) {
		csNewPortLabel = dialog.GetPortLabel();
		csNewPortLabel.MakeLower();
	}

	// Rename Xport
	icon->description = csNewPortLabel;
}

void Project::EditGroupSetup() {
	CGroupSetupDialog dialog(this);

	if (dialog.DoModal()) {
	}
}

/*
// returns a pointer to a DEP for a matching dep filename
DEP* Project::GetDEPPtr(INXString depFilename) {
	for (int i=0; i<=DEPNum; i++) {
		if (depFilename == pDEP[i]->depFilename) {
			return pDEP[i];
		}
	}
	return NULL;
}
*/

CDrawProgView* Project::OpenView(INXString depFilename) {
	CDocument* Subsystem = AfxGetApp( )->OpenDocumentFile(depFilename);		
	// Need to initialise undo for case when the view is already open
	POSITION pos = Subsystem->GetFirstViewPosition();
	CDrawProgView* pView = (CDrawProgView*) Subsystem->GetNextView(pos);
	//pView->initUndo();
	return pView;
}

// Resets all the values and events in all the DEPs loaded in
void Project::ResetDebug() {
	for (int i=0; i<=DEPNum; i++) {
		pDEP[i]->ResetAllDbgEvents();
		pDEP[i]->ResetAllDbgValues();
	}
}

// Saves all the DEP that is open in the application
#endif
void Project::SaveProjectDep() {
#ifdef TRY_EXCLUDE
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	MainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString csProjectDir;
	pProjMData->getProjectDir(csProjectDir);

	int response;
	int iRetest = 1;
	double dFreeDiskSpace;


	// test for sufficient disk space to save project, loop on retry, exit app on cancel
	while (iRetest > 0) {
		iRetest = 0;
		dFreeDiskSpace = pApp->MyGetFreeDiskSpaceMB();
		if (dFreeDiskSpace < DISK_SPACE_CRITICAL_MB) {
			response = wxMessageBox("Unable to save project, less than 10MB of free disk space remaining.\nCannot continue - Cancel will exit application.","",wxICON_EXCLAMATION|wxCANCEL,pFrame); //MB_RETRYCANCEL | MB_ICONWARNING);
			if (response == IDRETRY) {
				iRetest = 1;
			} else {
				//@todo - we could be a bit nicer about this - freeze project view instead of exiting app
//				pFrame->exiting = TRUE;
//				pFrame->SendMessage(WM_CLOSE);
				exit(1);
				return;
			}
		}
	}
	
	// changed the DEP, so if SODL was uptodate before, set flag to indicate dirty SODL
	pProjMData->setDirtySODLFlag();

	//DeleteUnused(pDEP[0]);
	for (int i=0; i<=DEPNum; i++) {
		pDEP[i]->SaveProg(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP[i]->hItem) + 
			pDEP[i]->depFilename + ".prg");
	}
#endif
}

// Saves all the open DEP and the project meta-data
void Project::SaveProject()
{
	SaveProjectDep();
	// Do this in calling routine, as otherwise pProjMData
	// will have beeen deleted, so lock can't be released
	// pProjMData->writeProjectFile();
}

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
INXObjList* Project::WriteSODL(INXString sodlfile) {

	SODL compile(this);

	// save the top level dep to a temp file so it can be loaded into the flattened list
	pDEP[0]->SaveProg(workDir + TEMPDIR + "temp");
	//compile.Copy2Flattened();
	compile.WriteSODL(sodlfile);

	// remove file used to warn IGB users that SODL is outofdate
	pProjMData->clearDirtySODLFlag();
	pProjMData->clearDirtyWidgetsFlag();

	return compile.flattened;
}

// method that adds widgets for a pasted list
void Project::addPastedGuiWidgets(HTREEITEM hItem, INXObjList* pasteList)
{
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM hUserDefItem;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	set<INXString> sWidgetGroupSet;
	vector<pair<INXString, INXString> > vWidgetGroupPairVec;
	INXString csWidgetGroupName;

	if (IsPastedGuiWidgetsInProject(pasteList, hItem)) {
		// Get a set of all the widget group names in the paste list
		pos = pasteList->GetHeadPosition();
		while(pos) {
			blob = (ConData *) (pasteList->GetNext(pos));
			if (blob->m_iUserDefined) {
				hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
				getWidgetGroupNames(hUserDefItem, sWidgetGroupSet);
			}
			else if (blob->isGuiWidget()) {
				blob->getScreenTag(csWidgetGroupName);
				sWidgetGroupSet.insert(csWidgetGroupName);
			}
		}
	
		// Prompt the user to use a different group name for each group name in the set
		setWidgetGroupNames(sWidgetGroupSet, vWidgetGroupPairVec);
	}

	// Add all the Gui widgets in the paste list to the project meta-data
	pos = pasteList->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (pasteList->GetNext(pos));
		if (blob->m_iUserDefined) {
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
			updateWidgetGroupNames(hUserDefItem, vWidgetGroupPairVec);
		}
		else if (blob->isGuiWidget()) {
			updateWidgetGroup(blob, vWidgetGroupPairVec);
		}
	}
}


// This method adds a widget to the project meta-data
void Project::addGuiWidget(ConData* blob) 
{
	INXString csWidgetTag = "", csScreenTag = "", csScreenTagDescr = "";
	UINT iTagNum = 0, iWidgetParamNum = 0;
	char szTagNum[8] = {'\0'};

	pProjMData->setDirtyWidgetsFlag();

	for (UINT i=1; i<blob->iParamNum; i++) {
		// get widget tag
		if (blob->iconParam[i]->dataType == 5) {
			csWidgetTag = blob->iconParam[i]->value;
			//csWidgetTag = "widget";
			iWidgetParamNum = i;
		}
		// get screen tag
		else if (blob->iconParam[i]->dataType == 4) {
			csScreenTag = blob->iconParam[i]->value;
			csScreenTagDescr = blob->iconParam[i]->description;
		}
	}

	// only add GUI widgets to project file
	if (csWidgetTag != "") {
		// Get number of widgets and then create a unique tag using this number
		// if this tag is not unique then increment the number
		pProjMData->getGuiWidgetVecSize(iTagNum);
		_itoa_s(iTagNum, szTagNum, 10);
		GuiWidget widget;
		// adding a new widget
		if (csWidgetTag == "widget") {
			widget.setWidgetTag((CString &)(csWidgetTag + szTagNum));
			widget.setScreenTag((CString &)csScreenTag);
		}
		// copying an existing widget
		else {
			widget.setWidgetTag((CString&)csWidgetTag);
			widget.setScreenTag((CString&)csScreenTag);
		}

		while (pProjMData->guiWidgetInVec(widget)) {
			csWidgetTag = "widget";
			iTagNum++;
			_itoa_s(iTagNum, szTagNum, 10);
			widget.setWidgetTag(csWidgetTag + szTagNum);
		}
		pProjMData->addGuiWidget(widget);
		// add default screen tag
		pProjMData->createNewScreenTag((CString&)csScreenTag, (CString&)csScreenTagDescr);
		if (csWidgetTag == "widget") {
			blob->iconParam[iWidgetParamNum]->value = csWidgetTag + szTagNum;
		}
		else {
			blob->iconParam[iWidgetParamNum]->value = csWidgetTag;
		}
	}
}

void Project::removeWidgetsInUserDefBlock(HTREEITEM hItem)
{
	INXString csProjectDir, csBlockFile;
	BlockOperations bo;
	INXObjList* encapsulated;
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM hUserDefItem;
	DEP* pLoadedDEP;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	// If DEP is in memory then need to remove widgets from this because the user may have added widgets
	// that have not been saved.
	pLoadedDEP = depIsLoaded(hItem);
	if (pLoadedDEP) {
		encapsulated = pLoadedDEP->condata;
	}
	else {
		pProjMData->getProjectDir(csProjectDir);
		csBlockFile = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + ".prg";
		encapsulated = bo.LoadBlock(csBlockFile);
	}
			
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_iUserDefined) {
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
			removeWidgetsInUserDefBlock(hUserDefItem);
		}
		else if (blob->isGuiWidget()) {
			removeGuiWidget(blob);
		}
	}

	// if the DEP is in memory don't do memory cleanup here
	if (!pLoadedDEP) {
		bo.DeleteBlock(encapsulated);
	}
}

void Project::removeGuiWidget(ConData* blob)
{
	INXString csWidgetTag, csScreenTag;

	pProjMData->setDirtyWidgetsFlag();

	blob->getWidgetTag(csWidgetTag);
	blob->getScreenTag(csScreenTag);
	GuiWidget widget((CString&)csWidgetTag, (CString&)csScreenTag);
	pProjMData->removeGuiWidget(widget);
}

// This method gets a set of unique widget group names in a subsystem
void Project::getWidgetGroupNames(HTREEITEM hItem, set<INXString> &sWidgetGroupSet)
{
	INXString csProjectDir, csBlockFile, csWidgetGroupName;
	BlockOperations bo;
	INXObjList* encapsulated;
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM hUserDefItem;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	pProjMData->getProjectDir(csProjectDir);
	csBlockFile = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + ".prg";
	encapsulated = bo.LoadBlock(csBlockFile);
			
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_iUserDefined) {
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
			getWidgetGroupNames(hUserDefItem, sWidgetGroupSet);
		}
		else if (blob->isGuiWidget()) {
			blob->getScreenTag(csWidgetGroupName);
			sWidgetGroupSet.insert(csWidgetGroupName);
		}
	}
	bo.DeleteBlock(encapsulated);
}

// This method prompts the user to set the set of widget group names in a subsystem
void Project::setWidgetGroupNames(set<INXString> sWidgetGroupSet, vector<pair<INXString, INXString> > &vWidgetGroupPairVec)
{
	pair<INXString, INXString> prWidgetGroupPair;
	INXString csWidgetGroupDesc;
	UINT iSize = sWidgetGroupSet.size();
	int count = 1;
	vector<INXString> vWidgetGroupNames;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	pProjMData->getScreenTags(vWidgetGroupNames);
	CWidgetGroupManagerDlg dlg(vWidgetGroupNames);

	set<INXString>::iterator it = sWidgetGroupSet.begin();
	while( it != sWidgetGroupSet.end() ){
		prWidgetGroupPair.first = (*it);
		dlg.setWidgetGroupName(*it);
		dlg.setDialogCountString("Dialog " + intToString(count) + " of " + intToString(iSize));
		if (dlg.DoModal()==IDOK) {
			prWidgetGroupPair.second = dlg.getWidgetGroupName();
			csWidgetGroupDesc = "Copy of " + (*it);
			if (!pProjMData->createNewScreenTag((CString&)prWidgetGroupPair.second, (CString&)csWidgetGroupDesc)) {
				// Add widget group to project explorer
				pFrame->m_wndProjectBar.m_cProjTree.addWidgetGroup(prWidgetGroupPair.second, pDEP[0]->hItem);
			}
		}
		else {
			prWidgetGroupPair.second = dlg.getWidgetGroupName();
			csWidgetGroupDesc = "Copy of " + (*it);
			pProjMData->createNewScreenTag((CString&)prWidgetGroupPair.second, (CString&)csWidgetGroupDesc);
		}
		vWidgetGroupPairVec.push_back(prWidgetGroupPair);
		it++;
		count++;
	}
}

// This method updates the widget group names in a subsystem
void Project::updateWidgetGroupNames(HTREEITEM hItem, vector<pair<INXString, INXString> > vWidgetGroupPairVec)
{
	INXString csProjectDir, csBlockFile, csWidgetGroupName;
	BlockOperations bo;
	INXObjList* encapsulated;
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM hUserDefItem;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	pProjMData->getProjectDir(csProjectDir);
	csBlockFile = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + ".prg";
	encapsulated = bo.LoadBlock(csBlockFile);
			
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_iUserDefined) {
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
			updateWidgetGroupNames(hUserDefItem, vWidgetGroupPairVec);
		}
		else if (blob->isGuiWidget()) {
			updateWidgetGroup(blob, vWidgetGroupPairVec);
		}
	}
	bo.SaveBlock(csBlockFile, encapsulated);
	bo.DeleteBlock(encapsulated);
}

// This method updates a widget group name in a single widget FB.
void Project::updateWidgetGroup(ConData* pBlob, vector<pair<INXString, INXString> > vWidgetGroupPairVec)
{
	INXString csWidgetGroupName;
	// Add Gui widgets for the case when the widgets are not already in the project
	if (vWidgetGroupPairVec.size() == 0) {
		addGuiWidget(pBlob);
	}
	// Add Gui widgets for the case when the widgets are already in the project
	else {
		pBlob->getScreenTag(csWidgetGroupName);
		for (UINT i=0; i<vWidgetGroupPairVec.size(); i++) {
			if (vWidgetGroupPairVec[i].first == csWidgetGroupName) {
				// widget group names are same so assign unique tag
				if (vWidgetGroupPairVec[i].first == vWidgetGroupPairVec[i].second) {
					addGuiWidget(pBlob);
				}
				else {
					pBlob->setScreenTag(vWidgetGroupPairVec[i].second);
					addGuiWidget(pBlob);
				}
			}
		}
	}
}

// This method returns true if any of the Gui widgets in the paste list are in the project meta-data
// otherwise it returns false
// This method can be called recursively. For subsystems pass it the list of ConData objects for the subsystem.
// hItem is for the subsystem.
bool Project::IsPastedGuiWidgetsInProject(INXObjList* pPasteList, HTREEITEM hItem)
{
	INXPOSITION pos;
	ConData* pBlob;
	bool bRet = false;
	GuiWidget xWidget;
	INXString csBlockFile, csProjectDir;
	BlockOperations bo;
	HTREEITEM hUserDefItem;
	INXObjList* pEncapsulated;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	pos = pPasteList->GetHeadPosition();
	while(pos) {
		pBlob = (ConData *) (pPasteList->GetNext(pos));
		if (pBlob->m_iUserDefined) {
			pProjMData->getProjectDir(csProjectDir);
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(pBlob, hItem);
			csBlockFile = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hUserDefItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hUserDefItem) + ".prg";
			pEncapsulated = bo.LoadBlock(csBlockFile);
			if (IsPastedGuiWidgetsInProject(pEncapsulated, hUserDefItem)) {
				bRet = true;
			}
			bo.DeleteBlock(pEncapsulated);
		}
		else if (pBlob->isGuiWidget()) {
			GetWidget(pBlob, xWidget);
			if (pProjMData->guiWidgetInVec(xWidget)) {
				bRet = true;
			}
		}
	}

	return bRet;
}

// This method returns a GuiWidget object for the specified ConData object
// The ConData object must be a GuiWidget
void Project::GetWidget(const ConData *pBlob, GuiWidget &xWidget)
{
	INXString csWidgetTag, csScreenTag;

	for (UINT i=1; i<pBlob->iParamNum; i++) {
		// get widget tag
		if (pBlob->iconParam[i]->dataType == 5) {
			csWidgetTag = pBlob->iconParam[i]->value;
		}
		// get screen tag
		else if (pBlob->iconParam[i]->dataType == 4) {
			csScreenTag = pBlob->iconParam[i]->value;
		}
	}

	xWidget.setWidgetTag((CString&)csWidgetTag);
	xWidget.setScreenTag((CString&)csScreenTag);
}

DEP* Project::depIsLoaded(HTREEITEM hItem)
{
	DEP* pLoadedDEP = NULL;

	for (int i=0; i<=DEPNum; i++) {
		if (pDEP[i]->hItem == hItem) {
			pLoadedDEP = pDEP[i];
		}
	}

	return pLoadedDEP;
}

void Project::closeUserDefDoc(HTREEITEM hItem)
{
	INXString csProjectDir, csBlockFile;
	BlockOperations bo;
	INXObjList* encapsulated;
	INXPOSITION pos;
	ConData* blob;
	HTREEITEM hUserDefItem;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	pProjMData->getProjectDir(csProjectDir);
	csBlockFile = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + ".prg";
	encapsulated = bo.LoadBlock(csBlockFile);
	
	// Find all the user defined blocks within an encapsualted block
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_iUserDefined) {
			hUserDefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
			closeUserDefDoc(hUserDefItem);
		}
	}
	bo.DeleteBlock(encapsulated);
	// if the user defined blocks are loaded then close the document/window
	if (depIsLoaded(hItem)) {
		// decrement dep counter, as this is used to save the project
		//DEPNum--;
		((CDrawProgApp*)AfxGetApp())->CloseUserDefDoc(hItem);
	}
}

#endif
bool Project::getDefineMonitors()
{
	return m_bDefineMonitors;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void Project::setDefineMonitors(bool bDefineMonitors)
{
	m_bDefineMonitors = bDefineMonitors;
}

// Method that runs the National Language Support tool
#endif
bool Project::RunNlsExec()
{
	INXString csProjectDir, csProjectName, csExec, csInstallDir, csParams;
	DrawProg *pApp = NULL;//@todo(CDrawProgApp *) AfxGetApp();
	CFileOperation fo;
	bool bRet = false;

	pProjMData->getProjectDir(csProjectDir);
	pProjMData->getProjectName(csProjectName);
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	pApp->GetInstallationBaseDir(csInstallDir);
#endif
	// ***REVISIT*** Should we save SODL first????

	csExec = csInstallDir + wxT("\\bin\\NationalLanguageSupport.exe");
	if (!fo.CheckPath(csExec)) {
		wxMessageBox(wxT("National Language Support not installed."));
	}
	else {
		// Run the NLS executable
		csParams = csProjectDir + wxT(" ") + csProjectName;
		//ShellExecute(NULL, "open", csExec, csParams, NULL, SW_HIDE);
		bRet = true;
	}

	return bRet;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Method that opens the nls.csv file in the application specified in tools.ini
void Project::OpenNlsFile()
{
	INXString csProjectDir, csNlsFilePathName, csToolExecPathName;
	CDrawProgApp *pApp = (CDrawProgApp *) AfxGetApp();
	CFileOperation fo;

	pProjMData->getProjectDir(csProjectDir);
	// Fire up Excel
	csNlsFilePathName = csProjectDir + NLSDIR + "nls.csv";
	csToolExecPathName = pApp->GetToolExecPath("nls");
	if (csToolExecPathName != "" && fo.CheckPath(csToolExecPathName)) {
		ShellExecute(NULL, "open", csToolExecPathName, csNlsFilePathName, NULL, SW_SHOW);
	}
	else {
		ShellExecute(NULL, "open", csNlsFilePathName, NULL, NULL, SW_SHOW);
	}
}

#endif
