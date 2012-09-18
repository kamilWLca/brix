// EditList.cpp: implementation of the EditList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawProg.h"
#include "EditList.h"
#include "MainFrm.h"
#include "FileOperations.h"
#include "BlockOperations.h"
#include "Porting_Classes/INXObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EditList::EditList()
{
	copySelList = new INXObjList;
	pasteOffset.SetPoint(20,20);
}

EditList::~EditList()
{
	delete copySelList;
}

// saves the copySelList
void EditList::SaveCopy(INXString Info) {
	ofstream datafile(Info);
	//put an error trap
	ConData *blob;

	datafile << "IconData" << endl;
	INXPOSITION pos;
	pos = copySelList->GetHeadPosition();
	
	if (!datafile.good()) {
		AfxMessageBox("File could not be written");
	}

	while (pos) { 		
		blob=(ConData *) (copySelList->GetNext(pos));
		datafile<<"BEGIN_BLOCK"<<endl;
		blob->Save(&datafile);
	} 

	datafile<<"END_OF_BLOCKS"<<endl;
	datafile.close();

}

// Loads the saved copySelList into the pasteList
INXObjList* EditList::LoadPaste(INXString Info) {
	ifstream datafile(Info);
	char type[256] = {'\0'};
	long int id;
	INXObjList* pasteList = NULL;

	datafile >> type;
	// if a file is empty don't load it
	if ((INXString)type == "") {
		return NULL;
	}
	datafile.close();

	datafile.open(Info);

	// store the id value
	id = ConData::uniqueidgenerator;

	while ((!datafile.eof()) && (!datafile.bad())) {
		datafile >> type;
		if (strcmp(type,"END_OF_BLOCKS")==0) break;
		else
		if (strcmp(type,"BEGIN_BLOCK")==0) {
			ConData *blob = new ConData;
			blob->Load(&datafile);
			if (!pasteList) {
				pasteList = new INXObjList;
			}
			pasteList->AddTail((INXObject*) blob);
		}
	}

	// restore the id value, since it will have been increased when creating flattened
	ConData::uniqueidgenerator = id;

	return pasteList;
}

// This function stores a copy of the selected icons when a copy or cut is performed
void EditList::BufferSelectedIcons(INXRect selectRect, Project* pProject, DEP* pDEP)
{
	ConData *icondata;
	ConData *otherIcon;
	INXPOSITION pos, otherPos;
	bool isConnected = FALSE;
	INXObjList* temp;
	CFileOperation fo;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	BlockOperations bo;
	UINT i;
	INXString csProjectDir;
	
	pProject->pProjMData->getProjectDir(csProjectDir);
	copySelList->RemoveAll();
	// the following algorithm is used because there isn't a copy constructor
	// copy the condata list to the flattened list
	// copy the selected icons from the flattened list to the copySelList
	temp = LoadTemp();
	bo.AssignIconLineEndPoints(temp);
	
	// if the icon is in the selection box add it to copySelList
	pos = temp->GetHeadPosition();
	while (pos) {
		icondata = (ConData *) (temp->GetNext(pos));
		//if (selectRect.PtInRect(icondata->GetIconCentre())) {
		if (icondata->selected) {
			// Copy user defined blocks to the temp directory
			if (icondata->m_iUserDefined) {
				fo.SetOverwriteMode(true); // set OverwriteMode flag
				fo.Copy(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename 
					+ "\\" + icondata->description + ".prg", workDir + TEMPDIR);
				fo.Copy(csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename 
					+ "\\" + icondata->description, workDir + TEMPDIR);
				//CopyInstance(icondata, pProject, pDEP);
			}
			copySelList->AddTail(icondata);
		}
		// if icon isn't in selection box then delete
		else {
			//delete icondata;
		}
	}
	
	//delete temp;

	// if an input on an icon in copySelList is not connected to another
	// icon in the list then disconnect it
	// remove all tags from ports in the copy list. Don't want same tags as this is confusing.
	pos = copySelList->GetHeadPosition();
	while (pos) {
		icondata = (ConData *) (copySelList->GetNext(pos));
		for (i=0; i<icondata->inputport_num; i++) {
			icondata->inputport[i]->tag = "";
			otherPos = copySelList->GetHeadPosition();
			isConnected = FALSE;
			while (otherPos) {
				otherIcon = (ConData *)(copySelList->GetNext(otherPos));
				if (icondata->inputport[i]->line.exist && icondata->inputport[i]->line.othericonid == otherIcon->identnum) {
					isConnected = TRUE;
				}
			}
			if (!isConnected) {
				icondata->inputport[i]->line.exist = 0;
			}
		}
		for (i=0; i<icondata->startport_num; i++) {
			icondata->startport[i]->tag = "";
			otherPos = copySelList->GetHeadPosition();
			isConnected = FALSE;
			while (otherPos) {
				otherIcon = (ConData *)(copySelList->GetNext(otherPos));
				if (icondata->startport[i]->line.exist && icondata->startport[i]->line.othericonid == otherIcon->identnum) {
					isConnected = TRUE;
				}
			}
			if (!isConnected) {
				icondata->startport[i]->line.exist = 0;
			}
		}
		for (i=0; i<icondata->outputport_num; i++) {
			icondata->outputport[i]->tag = "";
		}
		for (i=0; i<icondata->finishport_num; i++) {
			icondata->finishport[i]->tag = "";
		}
	}

	// save copySelList. This is used for test purposes
	SaveCopy((INXString)workDir + "\\tmpcopy.prg");
	//pView->pasteFlag = TRUE;
	// delete the temp list now the selected blocks have been saved
	bo.DeleteBlock(temp);
}

INXObjList* EditList::LoadTemp() {	
	ConData* blob;
	long int id = 0;
	INXObjList* temp = new INXObjList;

	// Copy the condata list to the flattened list
	// save the condata list and then load it back in to flattened
	// Alternatively could use a copy constructor
	temp->RemoveAll();
	//SaveProg(workDir + TEMPDIR + "temp");
	ifstream datafile(workDir + TEMPDIR + "temp");
	char type[256];

	while ((!datafile.eof()) && (!datafile.fail())) {
		datafile >> type;
		if (strcmp(type,"END_OF_BLOCKS")==0) break;
		else
		if (strcmp(type,"BEGIN_BLOCK")==0) 
		{
			blob = new ConData;
			
			if(blob)
			{	blob->Load(&datafile);
				temp->AddTail((INXObject*) blob);
				id = blob->identnum;
			}
			else
			{
				//delete blob;
				break;
			}
		}
	}
	
	// set the uniqueidgenerator to the identnum of the last icon
	// This is necessary as it is possible that this value may be greater than
	// the number of icons loaded, due to icons being deleted previously. This
	// prevents icon IDs being duplicated
	// Need to add 1, since uniqueidgenerator is incremented after a new icon is
	// instantiated.
	//id++;
	//ConData::uniqueidgenerator = id;
	return temp;
}

INXPoint EditList::getPasteOffset()
{
	return pasteOffset;
}

void EditList::setPasteOffset(INXPoint point)
{
	pasteOffset = point;
}

void EditList::incPasteOffset()
{
	pasteOffset.Offset(20,20);
}

void EditList::resetPasteOffset()
{
	pasteOffset.SetPoint(20,20);
}