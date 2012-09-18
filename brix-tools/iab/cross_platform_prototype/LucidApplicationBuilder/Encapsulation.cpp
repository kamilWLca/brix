// Encapsulation.cpp: implementation of the Encapsulation class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DrawProg.h"
#include "Encapsulation.h"
#include "TypeConversion.h" //@todo
#include "FileOperations.h"
#include "BlockOperations.h"
#include "GlobalFuncs_2.h" //@todo
#include "Porting_Classes/INXRect.h"
#include <wx/msgdlg.h>
#include "MFCstruct.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif*/
/*
This module is for generating IDF from sub systems - not the dialog box (it appears!).
*/
// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Encapsulation::Encapsulation()
{
	iconBitmap = "";
	blockName = "";
	hItem = NULL;
	//pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
}

Encapsulation::~Encapsulation()
{

}

// function that performs top-down encapsulation
void Encapsulation::EncapsulateBlock(INXObjList* _encapsulated, HTREEITEM _hItem)
{
	hItem = _hItem;
	encapsulated = _encapsulated;
	Encapsulate *dialog = new Encapsulate(encapsulated);
	//INXString blockFile;
	int iconNum;

	// find an appropriate encapsulation icon
	iconNum = SelectEncapsulateIcon();
	// save the IDF and DEP if an appropriate icon is available
	if (iconNum > 0) {
		SaveEncapsulated(dialog, iconNum, 0); // Need to check if nLib is 0 or 1
	}
	else if (iconNum == -1) {
		wxMessageBox("WARNING: Too many ports. Cannot encapsulate this block.");
	}

	delete dialog;
}

// Function that selects the most appropriate encapsulation icon depending on the number of xports
int Encapsulation::SelectEncapsulateIcon() {
	INXPOSITION pos;
	ConData* blob;
	UINT inNum = 0, outNum = 0, startNum = 0, finishNum = 0;
	int iconNum;

	// Get the number of xports
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType.Find("XINPUT") != -1) {
			inNum++;
		}
		else if (blob->m_csIconType.Find("XOUTPUT") != -1) {
			outNum++;
		}
		else if (blob->m_csIconType == "XSTART") {
			startNum++;
		}
		else if (blob->m_csIconType == "XFINISH") {
			finishNum++;
		}
	}

	// Select the most appropriate icon
	if (inNum<=1 && outNum<=1 && startNum<=1 && finishNum<=1) {
		iconNum = 1;
	}
	else if (inNum<=2 && outNum<=2 && startNum<=2 && finishNum<=2) {
		iconNum = 2;
	}
	else if (inNum<=3 && outNum<=3 && startNum<=3 && finishNum<=3) {
		iconNum = 3;
	}
	else if (inNum<=4 && outNum<=4 && startNum<=4 && finishNum<=4) {
		iconNum = 4;
	}
	else if (inNum<=5 && outNum<=5 && startNum<=5 && finishNum<=5) {
		iconNum = 5;
	}
	else if (inNum<=6 && outNum<=6 && startNum<=6 && finishNum<=6) {
		iconNum = 6;
	}
	else if (inNum<=7 && outNum<=7 && startNum<=7 && finishNum<=7) {
		iconNum = 7;
	}
	else if (inNum<=8 && outNum<=8 && startNum<=8 && finishNum<=8) {
		iconNum = 8;
	}
	else if (inNum<=9 && outNum<=9 && startNum<=9 && finishNum<=9) {
		iconNum = 9;
	}
	else if (inNum<=10 && outNum<=10 && startNum<=10 && finishNum<=10) {
		iconNum = 10;
	}
	else {
		iconNum = -1;
	}

	return iconNum;
}

// Function that writes out idf and DEP if block name does not already exist
// returns 0 if cancelled and 1 if OK.
int Encapsulation::SaveEncapsulated(Encapsulate* dialog, int iconNum, int nLib)
{
	MainFrame* pFrame = NULL;/*@todo (CMainFrame*)AfxGetApp()->m_pMainWnd;*/
	INXPOSITION pos;
	ConData* blob;
	bool nameOK = FALSE;
	bool errorFlag = FALSE;
	bool bIsUnique = TRUE;
	INXString blockFile, depPath, csProjectDir;
	UINT inNum = 0, outNum = 0, startNum = 0, finishNum = 0;
	char csPortNum[8];
	Project* pProject;
	BlockOperations bo;
	UINT i;

	// Get the number of xports
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType.Find("XINPUT") != -1) {
			inNum++;
		}
		else if (blob->m_csIconType.Find("XOUTPUT") != -1) {
			outNum++;
		}
		else if (blob->m_csIconType == "XSTART") {
			startNum++;
		}
		else if (blob->m_csIconType == "XFINISH") {
			finishNum++;
		}
	}

	// Set the arrays with the correct number of xport labels
	for (i=1; i<=inNum; i++) {
		_itoa_s(i, csPortNum, 10); 
		dialog->inLabels[i] = "Input" + (INXString)csPortNum;
		dialog->inNames[i] = "i" + (INXString)csPortNum;
	}
	for (i=1; i<=outNum; i++) {
		_itoa_s(i, csPortNum, 10); 
		dialog->outLabels[i] = "Output" + (INXString)csPortNum;
		dialog->outNames[i] = "o" + (INXString)csPortNum;
	}
	for (i=1; i<=startNum; i++) {
		_itoa_s(i, csPortNum, 10); 
		dialog->startLabels[i] = "Start" + (INXString)csPortNum;
		dialog->startNames[i] = "s" + (INXString)csPortNum;
	}
	for (i=1; i<=finishNum; i++) {
		_itoa_s(i, csPortNum, 10); 
		dialog->finishLabels[i] = "Finish" + (INXString)csPortNum;
		dialog->finishNames[i] = "f" + (INXString)csPortNum;
	}
	// set the dialog member variables to the array values
	//dialog->getArray();

	
	// Don't save until the block name and ports are valid
	while (!nameOK) {
		errorFlag = FALSE;
		if (dialog->ShowModal()==IDOK) {
			if (dialog->m_BlockName == "") {
				wxMessageBox("WARNING: Enter a Block Name.");
				errorFlag = TRUE;
			}
			else if (pFrame->m_wndProjectBar.m_cProjTree.NameExist(dialog->m_BlockName, hItem)) {
				wxMessageBox("WARNING: Block Name already exists. Choose a different name.");
				errorFlag = TRUE;
			}
			else if (dialog->m_BlockName.Find(" ") != -1) {
				wxMessageBox("WARNING: Block Name is not allowed to contain spaces.");
				errorFlag = TRUE;
			}
			else {//filename OK
				// check if a library component is unique
				if (nLib==1) {
					if (!IsLibComponentUnique(dialog->m_BlockName)) {
						if (wxMessageBox("A library component with this name already exists. Do you want to replace it?",MB_YESNO|MB_ICONEXCLAMATION)==IDNO) {
							bIsUnique = FALSE;
						}
					}
				}
				if (bIsUnique) {
					WriteIDF(dialog, iconNum);
					// reposition encapsulated DEP, so its in top left corner
					repositionEncapsulated();
					// save encapsulated DEP in user defined directory
					blockFile = workDir + USERDEFDIR + dialog->m_BlockName + ".prg";
					if (!bo.SaveBlock(blockFile, encapsulated)) {
						return 0;
					}
					CFileOperation fo;   
					fo.SetOverwriteMode(true); // set OverwriteMode flag
					fo.Delete(workDir + USERDEFDIR + dialog->m_BlockName);
					CreateDirectory(workDir + USERDEFDIR + dialog->m_BlockName, NULL);
					// copy any sub-blocks
					pos = encapsulated->GetHeadPosition();
					while(pos) {
						blob = (ConData*) (encapsulated->GetNext(pos));
						if (blob->m_iUserDefined) {
							pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
							pProject->pProjMData->getProjectDir(csProjectDir);
							depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + (INXString)pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hItem) + "\\";
							if (!fo.Copy(csProjectDir + DEPDIR + depPath + blob->description + ".prg",
								workDir + USERDEFDIR + dialog->m_BlockName)) {
								fo.ShowError(); // if copy fails show error message
							}
							if (!fo.Copy(csProjectDir + DEPDIR + depPath + blob->description,
								workDir + USERDEFDIR + dialog->m_BlockName)) {
								fo.ShowError(); // if copy fails show error message
							}
						}
					}
					errorFlag = FALSE;
				}
				else {
					errorFlag = TRUE;
				}
			}
		}
		else {
			return 0;
		}
		if (!errorFlag) {
			nameOK = TRUE;
		}

	}
	return 1;
}
/*
void Encapsulation::SaveSubBlocks(CObList* subBlock, INXString hierarchyName) {
	CObList* tmpList = new CObList;
	ConData* blob;
	POSITION pos;
	INXString newHierName;

	pos = subBlock->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (subBlock->GetNext(pos));
		if (blob->m_iUserDefined) {
			CreateDirectory(workDir + USERDEFDIR + hierarchyName, NULL);		
			ifstream infile(projectDir + DEPDIR + blob->hierarchyName + blob->description + ".prg");
			ofstream outfile(workDir + USERDEFDIR + hierarchyName + "\\" + blob->description + ".prg");
			outfile << infile.rdbuf();	
			infile.close();
			outfile.close();
			tmpList = LoadBlock(projectDir + DEPDIR + blob->hierarchyName + blob->description + ".prg");
			newHierName = hierarchyName + "\\" + blob->description;
			SaveSubBlocks(tmpList, newHierName);
		}
	}		
	delete tmpList;
}
*/

// Function that writes out IDF for an encapsulated block
void Encapsulation::WriteIDF(Encapsulate *dialog, int iconNum) {
	INXString idfFile = workDir + USERDEFDIR + dialog->m_BlockName + ".idf.ini";
	ofstream datafile(idfFile);
	INXPOSITION pos;
	ConData *blob;
	UINT inNum = 1, outNum = 1, startNum = 1, finishNum = 1, portNum = 1;
	char csPortNum[8];
	INXString bitmap;
	TypeConversion converter;
	int iInFirstYCoord, iOutFirstYCoord;

	if (!datafile.good()) {
		wxMessageBox("File could not be written");
	}
	
	// select the most appropriate encapsulation icon depending on the number of xports
	//iconNum = selectEncapsulateIcon(encapsulated);
	bitmap = IconNum2Bitmap(iconNum);

	datafile << "[Icon]" << endl;
	//datafile << "graphic filename=ENCAPSULATE" << endl;
	datafile << "graphic filename=" << (char*)bitmap << endl;
	datafile << "user defined=1" << endl;
	datafile << endl;
	datafile << "[Class]" << endl;
	datafile << "class name=" << (char*)dialog->m_BlockName << endl;
	datafile << endl;
	datafile << "[MenuLevel1]" << endl;
	datafile << "level1 menu=User Components" << endl;
	datafile << endl;
	if (dialog->getMenuName() == "") {
		datafile << "[MenuLevel2]" << endl;
		datafile << "level2 menu=" << (char*)dialog->m_BlockName << endl;
		datafile << endl;
	}
	else {
		datafile << "[MenuLevel2]" << endl;
		datafile << "level2 menu=" << (char*)dialog->getMenuName() << endl;
		datafile << endl;
		datafile << "[MenuLevel3]" << endl;
		datafile << "level3 menu=" << (char*)dialog->m_BlockName << endl;
		datafile << endl;
	}
	datafile << "[Short Description]" << endl;
	datafile << "description=" << endl;
	datafile << endl;
	datafile << "[Long Description]" << endl;
	datafile << "description=" << (char*)dialog->m_LongDescription << endl;
	datafile << endl;

	// write out ports. The xports in the diagram should have the same description as in the idf
	// to match them up when flattening.
	iInFirstYCoord = GetFirstInPortYCoord();
	iOutFirstYCoord = GetFirstOutPortYCoord();
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType.Find("XINPUT") != -1) {
			_itoa_s(portNum, csPortNum, 10);
			datafile << "[Port" << csPortNum << "]" << endl;
			//itoa(inNum, csPortNum, 10); 
			blob->description = dialog->inNames[inNum];
			datafile << "description=" << (char*)dialog->inNames[inNum] << endl;
			datafile << "port type=InputPort" << endl;
			datafile << "x coordinate=-6" << endl;
			datafile << "y coordinate=" << (iInFirstYCoord + ((inNum - 1) * 15)) << endl;
			datafile << "data type=" << converter.DataType2Char(blob->outputport[0]->datatype) << endl;
			datafile << endl;
			inNum++;
			portNum++;
		}
		else if (blob->m_csIconType.Find("XOUTPUT") != -1) {
			_itoa_s(portNum, csPortNum, 10);
			datafile << "[Port" << csPortNum << "]" << endl;
			//itoa(outNum, csPortNum, 10);
			blob->description = dialog->outNames[outNum];
			datafile << "description=" << (char*)dialog->outNames[outNum] << endl;
			datafile << "port type=OutputPort" << endl;
			datafile << "x coordinate=86" << endl;
			datafile << "y coordinate=" << (iOutFirstYCoord + ((outNum - 1) * 15)) << endl;
			datafile << "data type=" << converter.DataType2Char(blob->inputport[0]->datatype) << endl;
			datafile << endl;
			outNum++;
			portNum++;
		}
		else if (blob->m_csIconType == "XSTART") {
			_itoa_s(portNum, csPortNum, 10);
			datafile << "[Port" << csPortNum << "]" << endl;
			//itoa(startNum, csPortNum, 10);
			blob->description = dialog->startNames[startNum];
			datafile << "description=" << (char*)dialog->startNames[startNum] << endl;
			datafile << "port type=StartPort" << endl;
			datafile << "x coordinate=-6" << endl;
			datafile << "y coordinate=" << (10 + ((startNum - 1) * 15)) << endl;
			datafile << endl;
			startNum++;
			portNum++;
		}
		else if (blob->m_csIconType == "XFINISH") {
			_itoa_s(portNum, csPortNum, 10);
			datafile << "[Port" << csPortNum << "]" << endl;
			//itoa(finishNum, csPortNum, 10);
			blob->description = dialog->finishNames[finishNum];
			datafile << "description=" << (char*)dialog->finishNames[finishNum] << endl;
			datafile << "port type=FinishPort" << endl;
			datafile << "x coordinate=86" << endl;
			datafile << "y coordinate=" << (10 + ((finishNum - 1) * 15)) << endl;
			datafile << endl;
			finishNum++;
			portNum++;
		}
	}
	datafile.close();

}

// Function that writes out IDF for a subsystem that is being added to the library
void Encapsulation::WriteIDF(const ConData *block, const int nIconNum, INXString csL2MenuName) {
	INXString idfFile = workDir + USERDEFDIR + block->className + ".idf.ini";
	ofstream datafile(idfFile);
	UINT nInNum = 1, nOutNum = 1, nStartNum = 1, nFinishNum = 1, nPortNum = 1;
	INXString bitmap;
	TypeConversion converter;
	int nInFirstYCoord, nOutFirstYCoord;

	if (!datafile.good()) {
		wxMessageBox("File could not be written");
	}
	
	// select the most appropriate encapsulation icon depending on the number of xports
	//iconNum = selectEncapsulateIcon(encapsulated);
	bitmap = IconNum2Bitmap(nIconNum);

	datafile << "[Icon]" << endl;
	//datafile << "graphic filename=ENCAPSULATE" << endl;
	datafile << "graphic filename=" <<(char*)bitmap << endl;
	datafile << "user defined=1" << endl;
	datafile << endl;
	datafile << "[Class]" << endl;
	datafile << "class name=" << (char*)block->className << endl;
	datafile << endl;
	datafile << "[MenuLevel1]" << endl;
	datafile << "level1 menu=User Components" << endl;
	datafile << endl;
	if (csL2MenuName == "") {
		datafile << "[MenuLevel2]" << endl;
		datafile << "level2 menu=" << (char*)block->className << endl;
		datafile << endl;
	}
	else {
		datafile << "[MenuLevel2]" << endl;
		datafile << "level2 menu=" << (char*)csL2MenuName << endl;
		datafile << endl;
		datafile << "[MenuLevel3]" << endl;
		datafile << "level3 menu=" << (char*)block->className << endl;
		datafile << endl;
	}
	datafile << "[Short Description]" << endl;
	datafile << "description=" << (char*)block->description << endl;
	datafile << endl;
	datafile << "[Long Description]" << endl;
	datafile << "description=" << (char*)block->longDesc << endl;
	datafile << endl;

	// write out ports. The xports in the diagram should have the same description as in the idf
	// to match them up when flattening.
	nInFirstYCoord = GetFirstInPortYCoord();
	nOutFirstYCoord = GetFirstOutPortYCoord();
	for (UINT i=0; i<block->inputport_num; i++) {
		datafile << "[Port" << (char*)intToString(nPortNum) << "]" << endl;
		datafile << "description=" << (char*)block->inputport[i]->description << endl;
		datafile << "port type=InputPort" << endl;
		datafile << "x coordinate=-6" << endl;
		datafile << "y coordinate=" << (nInFirstYCoord + ((nInNum - 1) * 15)) << endl;
		datafile << "data type=" << converter.DataType2Char(block->inputport[i]->datatype) << endl;
		datafile << endl;
		nInNum++;
		nPortNum++;
	}
	for (UINT i=0; i<block->outputport_num; i++) {
		datafile << "[Port" << (char*)intToString(nPortNum) << "]" << endl;
		datafile << "description=" << (char*)block->outputport[i]->description << endl;
		datafile << "port type=OutputPort" << endl;
		datafile << "x coordinate=86" << endl;
		datafile << "y coordinate=" << (nOutFirstYCoord + ((nOutNum - 1) * 15)) << endl;
		datafile << "data type=" << converter.DataType2Char(block->outputport[i]->datatype) << endl;
		datafile << endl;
		nOutNum++;
		nPortNum++;
	}
	for (UINT i=0; i<block->startport_num; i++) {
		datafile << "[Port" << (char*)intToString(nPortNum) << "]" << endl;
		datafile << "description=" << (char*)block->startport[i]->description << endl;
		datafile << "port type=StartPort" << endl;
		datafile << "x coordinate=-6" << endl;
		datafile << "y coordinate=" << (10 + ((nStartNum - 1) * 15)) << endl;
		datafile << endl;
		nStartNum++;
		nPortNum++;
	}
	for (UINT i=0; i<block->finishport_num; i++) {
		datafile << "[Port" << (char*)intToString(nPortNum) << "]" << endl;
		datafile << "description=" << (char*)block->finishport[i]->description << endl;
		datafile << "port type=FinishPort" << endl;
		datafile << "x coordinate=86" << endl;
		datafile << "y coordinate=" << (10 + ((nFinishNum - 1) * 15)) << endl;
		datafile << endl;
		nFinishNum++;
		nPortNum++;
	}

	datafile.close();

}

// Function that returns the y coordinate of the first input port
int Encapsulation::GetFirstInPortYCoord() {
	INXPOSITION pos;
	ConData* blob;
	UINT startNum = 0;

	// Get the number of xstart ports
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType == "XSTART") {
			startNum++;
		}
	}

	return ((startNum + 1) * 15);
}

// Function that returns the y coordinate of the first input port
int Encapsulation::GetFirstOutPortYCoord() {
	INXPOSITION pos;
	ConData* blob;
	UINT finishNum = 0;

	// Get the number of xfinish ports
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType == "XFINISH") {
			finishNum++;
		}
	}

	return ((finishNum + 1) * 15);
}

// function that converts the integer value for the encapsulated icon to the bitmap filename string
INXString Encapsulation::IconNum2Bitmap(int iconNum) {

	INXString bitmap;

	switch(iconNum) {
		case -1:
			bitmap = ""; break;
		case 1:
			bitmap = "ENCAPSULATE1"; break;
		case 2:
			bitmap = "ENCAPSULATE2"; break;
		case 3:
			bitmap = "ENCAPSULATE3"; break;
		case 4:
			bitmap = "ENCAPSULATE4"; break;
		case 5:
			bitmap = "ENCAPSULATE5"; break;
		case 6:
			bitmap = "ENCAPSULATE6"; break;
		case 7:
			bitmap = "ENCAPSULATE7"; break;
		case 8:
			bitmap = "ENCAPSULATE8"; break;
		case 9:
			bitmap = "ENCAPSULATE9"; break;
		case 10:
			bitmap = "ENCAPSULATE10"; break;
		default:
			bitmap = ""; break;
	}
	return bitmap;
}

// function that encapsulates a subset of a block.
// Several steps are required to perform subsetting.
// The first step requires connecting xports to the subsetted block and saving this as
// a separate block.
// The second step involves adding the encapsulate icon
// The third step involves connecting up the encapsulate icon and removing the icons that
// have been encapsulated. This step is performed by the connectEncapsulatedIcon function.
INXObjList* Encapsulation::EncapsulateSubset(INXRect encapsulate, INXObjList* depList, HTREEITEM _hItem, int nLib) {
	INXPOSITION pos, otherPos;
	ConData* blob;
	ConData* otherBlob;
	ConData* xportBlob;
	INXPoint point;
	int inPortNum = 0, outPortNum = 0;
	int iconNum, saveOK, otherPortNum;
	UINT i;
	BlockOperations bo;
	
	hItem = _hItem;
	encapsulated = new INXObjList;
	Encapsulate *dialog = new Encapsulate(encapsulated);
	// check every icon to see if its centre is within the encapsulation box
	pos = depList->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (depList->GetNext(pos));

		if (encapsulate.PtInRect(blob->GetIconCentre())) {
			encapsulated->AddTail(blob);
			// set attribute in condata, that is used by DEP::ConnectEncapsulatedIcon
			// Previously ConnectEncapsulatedIcon method checked if the icon centre was in the selection box.
			// However, now icons are repositioned this approach cannot be used.
			blob->setEncapsulated(TRUE);
			// for each port check if the connected icon is in the encapsulation box
			// if its outside connect the port to a xport.
			for (i=0; i<blob->startport_num; i++) {
				if (blob->startport[i]->line.exist) {
					otherBlob = GetIconFromID(blob->startport[i]->line.othericonid, depList);
					if (!encapsulate.PtInRect(otherBlob->GetIconCentre())) {
						// Check if an xport has already been created for this port i.e. fanout
						otherPortNum = blob->startport[i]->line.otherportno;
						if (!otherBlob->finishport[otherPortNum]->xportConnected) {
							otherBlob->finishport[otherPortNum]->xportConnected = 1;
							// Add XStart XPort
							point.x = 0;
							point.y = 100 * inPortNum;
							xportBlob = AddXPort("XSTART", point);
							//blob->startport[i]->tag = "";
							// set the tag on xstart
							xportBlob->finishport[0]->tag = blob->startport[i]->tag;
							otherBlob->finishport[otherPortNum]->xportID = xportBlob->identnum;
							inPortNum++;
						}
						else {
							xportBlob = GetIconFromID(otherBlob->finishport[otherPortNum]->xportID, encapsulated);
						}
						blob->startport[i]->line.othericonid = xportBlob->identnum;
						blob->startport[i]->line.otherportno = 0;
						// add straight line
						blob->DeleteLine(i, STARTPORT);
						blob->AddLine(i, STARTPORT, xportBlob, 0, FINISHPORT);
						//blob->AddNodes(i, STARTPORT, xportBlob, 0, FINISHPORT);
					}
				}
			}
			for (i=0; i<blob->inputport_num; i++) {
				if (blob->inputport[i]->line.exist) {
					otherBlob = GetIconFromID(blob->inputport[i]->line.othericonid, depList);
					if (!encapsulate.PtInRect(otherBlob->GetIconCentre())) {
						// Check if an xport has already been created for this port i.e. fanout
						otherPortNum = blob->inputport[i]->line.otherportno;
						if (!otherBlob->outputport[otherPortNum]->xportConnected) {
							otherBlob->outputport[otherPortNum]->xportConnected = 1;
							// Add XInput XPort
							point.x = 0;
							point.y = 100 * inPortNum;
							// add the XINPUT of the correct data type
							switch(blob->inputport[i]->datatype) {
								case 0:
									xportBlob = AddXPort("XINPUTB", point); break;
								case 1:
									xportBlob = AddXPort("XINPUTI", point); break;
								case 2:
									xportBlob = AddXPort("XINPUTR", point); break;
								case 3:
									xportBlob = AddXPort("XINPUTS", point); break;
								default:
									wxMessageBox("Data type doesn't exist"); break;
							}
							//blob->inputport[i]->tag = "";
							// set the tag on xinput
							xportBlob->outputport[0]->tag = blob->inputport[i]->tag;
							otherBlob->outputport[otherPortNum]->xportID = xportBlob->identnum;
							inPortNum++;
						}
						else {
							xportBlob = GetIconFromID(otherBlob->outputport[otherPortNum]->xportID, encapsulated);
						}
						blob->inputport[i]->line.othericonid = xportBlob->identnum;
						blob->inputport[i]->line.otherportno = 0;
						// add straight line
						blob->DeleteLine(i, INPUTPORT);
						blob->AddLine(i, INPUTPORT, xportBlob, 0, OUTPUTPORT);
						//blob->AddNodes(i, INPUTPORT, xportBlob, 0, OUTPUTPORT);
					}
				}
			}
			for (i=0; i<blob->finishport_num; i++) {
				otherPos = depList->GetHeadPosition();
				// check all the start ports on all the icons to see if they are connected
				// to a finish port on blob. If this is the case add a XFINISH port.
				while (otherPos) {
					otherBlob = (ConData*) (depList->GetNext(otherPos));
					for (UINT j=0; j<otherBlob->startport_num; j++) {
						if (otherBlob->startport[j]->line.exist &&
							(otherBlob->startport[j]->line.othericonid == blob->identnum) &&
							((UINT)otherBlob->startport[j]->line.otherportno == i)) {
							if (!encapsulate.PtInRect(otherBlob->GetIconCentre()) && 
								!blob->finishport[i]->xportConnected) {
								point.x = blob->finishport[i]->P.x + 100;
								point.y = 100 * outPortNum;
								xportBlob = AddXPort("XFINISH", point);
								//xportBlob->startport[0]->line.exist = 1;
								xportBlob->startport[0]->line.othericonid = blob->identnum;
								xportBlob->startport[0]->line.otherportno = i;
								//blob->finishport[i]->tag = "";
								// set the tag on xfinish
								xportBlob->startport[0]->tag = blob->finishport[i]->tag;
								blob->finishport[i]->xportConnected = 1;
								//xlob->DeleteLine(i, STARTPORT);
								xportBlob->AddLine(0, STARTPORT, blob, i, FINISHPORT);
								//xportBlob->AddNodes(0, STARTPORT, blob, i, FINISHPORT);
								outPortNum++;
							}
						}
					}
				}
			}
			for (i=0; i<blob->outputport_num; i++) {
				otherPos = depList->GetHeadPosition();
				// check all the input ports on all the icons to see if they are connected
				// to an output port on blob. If this is the case add a XOUTPUT port.
				while (otherPos) {
					otherBlob = (ConData*) (depList->GetNext(otherPos));
					for (UINT j=0; j<otherBlob->inputport_num; j++) {
						if (otherBlob->inputport[j]->line.exist &&
							(otherBlob->inputport[j]->line.othericonid == blob->identnum) &&
							((UINT)otherBlob->inputport[j]->line.otherportno == i)) {
							if (!encapsulate.PtInRect(otherBlob->GetIconCentre()) &&
								!blob->outputport[i]->xportConnected) {
								point.x = blob->outputport[i]->P.x + 100;
								point.y = 100 * outPortNum;
								// Add the XOUTPUT of the correct data type
								switch(blob->outputport[i]->datatype) {
									case 0:
										xportBlob = AddXPort("XOUTPUTB", point); break;
									case 1:
										xportBlob = AddXPort("XOUTPUTI", point); break;
									case 2:
										xportBlob = AddXPort("XOUTPUTR", point); break;
									case 3:
										xportBlob = AddXPort("XOUTPUTS", point); break;
									default:
										wxMessageBox("Data type doesn't exist"); break;
								}
								//xportBlob->inputport[0]->line.exist = 1;
								xportBlob->inputport[0]->line.othericonid = blob->identnum;
								xportBlob->inputport[0]->line.otherportno = i;
								// remove labels from ports connected to xoutput
								//blob->outputport[i]->tag = "";
								// set the tag on xoutput
								xportBlob->inputport[0]->tag = blob->outputport[i]->tag;
								blob->outputport[i]->xportConnected = 1;
								xportBlob->AddLine(0, INPUTPORT, blob, i, OUTPUTPORT);
								//xportBlob->AddNodes(0, INPUTPORT, blob, i, OUTPUTPORT);
								outPortNum++;
							}
						}
					}
				}
			}
		}
	}
	// find an appropriate encapsulation icon
	iconNum = SelectEncapsulateIcon();
	// save the IDF and DEP if an appropriate icon is available
	if (iconNum > 0) {
		saveOK = SaveEncapsulated(dialog, iconNum, nLib);
	}
	else if (iconNum == -1) {
		wxMessageBox("WARNING: Too many ports. Cannot encapsulate this block.");
		saveOK = 0;
	}

	// Do the following if OK was pressed on the dialog box
	if (saveOK) {
		blockName = dialog->m_BlockName;
		iconBitmap = IconNum2Bitmap(iconNum);
		ResetXportConnected(depList);
		delete dialog;
		//delete encapsulated;
		return encapsulated;
	}

	delete dialog;
	// delete xports
	// don't delete function blocks, as they get deleted when deleting all function blocks in depList
	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData*) (encapsulated->GetNext(pos));
		if (blob->m_csIconType == "XFINISH" || blob->m_csIconType == "XSTART" || blob->m_csIconType.Find("XINPUT") != -1 || 
			blob->m_csIconType.Find("XOUTPUT") != -1) {
			delete blob;
		}
	}
	delete encapsulated;
	return NULL;
}

/*
Searches all icons and return an object pointer for a ConObject with matching identnum.
  */
ConData* Encapsulation::GetIconFromID(long int id, INXObjList* list) {
	ConData* blob;
	INXPOSITION pos;

	pos = list->GetHeadPosition();
	while (pos) { 		
		blob=(ConData *) (list->GetNext(pos));
		if (blob->identnum==id) return blob;
	}
	return NULL;
}

// This function adds a new icon to the encapsulated list
ConData* Encapsulation::AddXPort(INXString type, INXPoint point) {
	ConData *blobb = new ConData;
	long id;

	blobb->init(type, "", point, 1);
	// ensure id is unique
	id = blobb->identnum;
	while (!IsUniqueSubsetID(id)) {
		id++;
	}
	blobb->identnum = id;
	ConData::uniqueidgenerator = id;

	encapsulated->AddTail(blobb);

	return blobb;
}

bool Encapsulation::IsUniqueSubsetID(long id) {
	INXPOSITION pos;
	ConData* blob;

	pos = encapsulated->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->identnum == id) {
			return FALSE;
		}
	}

	return TRUE;
}


/*
void Encapsulation::EditEncapsulation(POSITION selectedIcon) 
{
	CObList* encapsulated = new CObList;
	ConData* blob = (ConData*) condata.GetAt(selectedIcon);
	bool nameOK = FALSE;
	bool errorFlag = FALSE;
	int iconNum;
	char csPortNum[8];
	INXString blockFile;
	
	blockFile = projectDir + DEPDIR + blob->block + ".prg";
	encapsulated = LoadBlock(blockFile);
	iconNum = selectEncapsulateIcon(encapsulated);
	Encapsulate *dialog = new Encapsulate(encapsulated);

	dialog->m_BlockName = blob->block;
	for (UINT i=1; i<=blob->inputport_num; i++) {
		itoa(i, csPortNum, 10); 
		dialog->inLabels[i] = "Input" + (INXString)csPortNum;
		dialog->inNames[i] = blob->inputport[i-1]->description;
	}
	for (i=1; i<=blob->outputport_num; i++) {
		itoa(i, csPortNum, 10); 
		dialog->outLabels[i] = "Output" + (INXString)csPortNum;
		dialog->outNames[i] = blob->outputport[i-1]->description;
	}
	for (i=1; i<=blob->startport_num; i++) {
		itoa(i, csPortNum, 10); 
		dialog->startLabels[i] = "Start" + (INXString)csPortNum;
		dialog->startNames[i] = blob->startport[i-1]->description;
	}
	for (i=1; i<=blob->finishport_num; i++) {
		itoa(i, csPortNum, 10); 
		dialog->finishLabels[i] = "Finish" + (INXString)csPortNum;
		dialog->finishNames[i] = blob->finishport[i-1]->description;
	}	

	// Don't save until the block name and ports are valid
	while (!nameOK) {
		errorFlag = FALSE;
		if (dialog->DoModal()==IDOK) {
			if (dialog->m_BlockName == "") {
				wxMessageBox("WARNING: Enter a Block Name.");
				errorFlag = TRUE;
			}
			//else if (isNameExist(dialog->m_BlockName, encapsulated)) {
			//	wxMessageBox("WARNING: Block Name already exists. Choose a different name.");
			//	errorFlag = TRUE;
			//}
			else {
				writeIDF(dialog, encapsulated, iconNum);
				blockFile = workDir + USERDEFDIR + dialog->m_BlockName + ".prg";
				SaveBlock(blockFile, encapsulated);
				blockFile = projectDir + DEPDIR + dialog->m_BlockName + ".prg";
				SaveBlock(blockFile, encapsulated);
				for (i=1; i<=blob->inputport_num; i++) {
					blob->inputport[i-1]->description = dialog->inNames[i];
				}
				for (i=1; i<=blob->outputport_num; i++) {
					blob->outputport[i-1]->description = dialog->outNames[i];
				}
				for (i=1; i<=blob->startport_num; i++) {
					blob->startport[i-1]->description = dialog->startNames[i];
				}
				for (i=1; i<=blob->finishport_num; i++) {
					blob->finishport[i-1]->description = dialog->finishNames[i];
				}				
				errorFlag = FALSE;
			}
		}
		if (!errorFlag) {
			nameOK = TRUE;
		}

	}

	delete dialog;
	delete encapsulated;
}
*/

void Encapsulation::repositionEncapsulated()
{
	ConData *blob;
	INXPOSITION pos;
	INXPoint tmp, top(-1, -1), left, topLeft;
	INXPoint newpoint, oldpoint;

	// get top left point of current encapsulated DEP
	pos = encapsulated->GetHeadPosition();
	while (pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType != "XFINISH" && blob->m_csIconType != "XSTART" && blob->m_csIconType.Find("XINPUT") == -1 && 
			blob->m_csIconType.Find("XOUTPUT") == -1) {
			tmp = blob->GetIconPos();
			// initialise top and left to coordinates of first icon
			if (top.x == -1 && top.y == -1) {
				top = tmp;
				left = tmp;
			}
			// get top most and left most coordinates
			else {
				if (tmp.y < top.y) {
					top = tmp;
				}
				if (tmp.x < left.x) {
					left = tmp;
				}
			}
		}
	}

	// reposition every icon
	pos = encapsulated->GetHeadPosition();
	while (pos) {
		blob = (ConData *) (encapsulated->GetNext(pos));
		if (blob->m_csIconType != "XSTART" && blob->m_csIconType.Find("XINPUT") == -1) {
			oldpoint = blob->GetIconPos();
			newpoint = oldpoint;
			if (blob->m_csIconType != "XFINISH" && blob->m_csIconType.Find("XOUTPUT") == -1) {
				newpoint.Offset(-(left.x - ENCAPS_XPOS), -(top.y - ENCAPS_YPOS));
			}
			// Only reposition x coordinate for XOutput and XFinish ports
			else {
				newpoint.Offset(-(left.x - ENCAPS_XPOS), 0);
			}

			setLineSelected(blob);
			blob->RenewPosition(newpoint, oldpoint);
		}
	}
}

void Encapsulation::setLineSelected(ConData *blob)
{
	ConData* otherIcon;
	long othericonid;
	UINT i;

	// set lineSelected if line is in selection box 
	for (i=0; i<blob->inputport_num; i++) {
		if (blob->inputport[i]->line.exist) {
			othericonid = blob->inputport[i]->line.othericonid;
			otherIcon = GetIconFromID(othericonid, encapsulated);
			if (otherIcon->getEncapsulated()) {  
				blob->inputport[i]->line.lineSelected = 1;
			}
		}
	}
	for (i=0; i<blob->startport_num; i++) {
		if (blob->startport[i]->line.exist) {
			othericonid = blob->startport[i]->line.othericonid;
			otherIcon = GetIconFromID(othericonid, encapsulated);
			if (otherIcon->getEncapsulated()) {  
				blob->startport[i]->line.lineSelected = 1;				
			}
		}
	}		
}

void Encapsulation::ResetXportConnected(INXObjList* depList) 
{
	ConData* blob;
	INXPOSITION pos;

	pos = depList->GetHeadPosition();

	while (pos) { 		
		blob = (ConData *) (depList->GetNext(pos));
		for (UINT i=0; i<blob->finishport_num; i++) {
			blob->finishport[i]->xportConnected = 0;
		}
		for (UINT i=0; i<blob->outputport_num; i++) {
			blob->outputport[i]->xportConnected = 0;
		}
	}
}

void Encapsulation::SetEncapsulated(INXObjList *_encapsulated)
{
	encapsulated = _encapsulated;
}

bool Encapsulation::IsLibComponentUnique(const INXString csLibName)
{
	CFileOperation fo;

	// IDF for library components used to be saved in IDF dir.
	if (fo.CheckPath((INXString)workDir + IDFDIR + csLibName + ".idf.ini")) {
		return false;
	}
	// IDF for library components is now saved in userdefined dir.
	else if (fo.CheckPath((INXString)workDir + USERDEFDIR + csLibName + ".idf.ini")) {
		return false;
	}
	else {
		return true;
	}
}

bool Encapsulation::HasXPorts(INXRect xEncapsulate, INXObjList* xDepList)
{
	ConData* xBlob;
	INXPOSITION xPos;
	bool bRet = false;

	xPos = xDepList->GetHeadPosition();

	while (xPos) { 		
		xBlob = (ConData *) (xDepList->GetNext(xPos));
		if (xEncapsulate.PtInRect(xBlob->GetIconCentre())) {
			if (xBlob->m_csIconType == "XFINISH" || xBlob->m_csIconType == "XSTART" || xBlob->m_csIconType.Find("XINPUT") != -1 || 
				xBlob->m_csIconType.Find("XOUTPUT") != -1) {
					bRet = true;
			}
		}
	}

	return bRet;
}
