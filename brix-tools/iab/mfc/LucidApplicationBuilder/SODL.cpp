// SODL.cpp: implementation of the SODL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawProg.h"
#include "SODL.h"
#include "BlockOperations.h"
#include <Windows.h>
#include <cassert>
#include "Porting_Classes/INXObject.h"
#include "Porting_Classes/INXObjArray.h"

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

SODL::SODL()
{
}

SODL::SODL(Project* _pProject)
{
	pProject = _pProject;
	flattened = pProject->flattened;
}

SODL::~SODL()
{
}



// Writes version information into the SODL header 
void SODL::WriteVersionInformation(ofstream *datafile)
{
	 SYSTEMTIME st;
	 //unsigned long bn;
	 INXString csProjectName;
	pProject->pProjMData->getProjectName(csProjectName);
     GetSystemTime(&st);
     //printf("Year:%d\nMonth:%d\nDate:%d\nHour:%d\nMin:%d\nSecond:% d\n" ,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	 pProject->pProjMData->nBuildNo=UpdateBuildNum(); 
	 *datafile << "#V:2.0.0\n"; // @todo we need to read this from the Application specific data or a targetting file perhaps if we support multiple versions?
	 *datafile << "#B:" << pProject->pProjMData->nBuildNo << "\n";
	 *datafile << "#D:" << "Y" << st.wYear << "M" << st.wMonth << "d" << st.wDay<< "h" <<st.wHour << "m" <<st.wMinute << "s" << st.wSecond << "\n";
	 *datafile << "#N:" << (CString)csProjectName << "\n";
}


void SODL::WriteSODL(INXString sodlfile) {
	ofstream datafile(sodlfile);
	ConData *blob;
	INXPOSITION pos;
	INXString funcName;
	int funcArg = -1;
	int startFunc = 0;
	UINT i;
	UINT j;
	UINT funcInPortNum = 0, funcOutPortNum = 0, funcFinPortNum = 0;
	// *** lineID is an array of unsigned ints. However, it needs to store the value -1 if
	// a port is not connected. Could use a CArray of type int, but this gives a smaller range
	// of line IDs than using a CUIntArray. Using such an array means -1 is stored as 4294967295.
	// This value should never be reached.
	INXObjArray<unsigned int> lineType;
	CArray<long,long> lineID;
	vector<Group> vGroups;
	TagProjMetaSupportData_t tagData;
	INXString csTargetFileName = "", csMessage = "";

	if (!datafile.good()) {
		AfxMessageBox("File could not be written");
	}
	
	//AfxMessageBox( "Get ready to call Copy2Flattened" );
	// Flatten encapsulated blocks
	Copy2Flattened();

	//AfxMessageBox( "Get ready to call Flatten" );
	Flatten();
	//SaveProg2("tmp.prg");	
	// 1. Assign a unique incremental integer (within its data type) to each line
	//AfxMessageBox( "Get ready to call AssignLineIDNum" );
	AssignLineIDNum();
	// First write the number of groups to the sodl file.
	// Turn off scheduling
	//datafile << 0 << endl;

	//AfxMessageBox( "Get ready to call generate SODL proper" );
	//@todo - add the following datafile << "hashmark" << Project << endl;
/************* The following is where everything is written out. This should be a new function *******************/	
	// Write out the parameters for each group
	WriteVersionInformation(&datafile);
/************ Now add the group data **********************/
	pProject->pProjMData->getGroupVec(vGroups);
	datafile << vGroups.size() << endl;
	

	for (i=0; i<vGroups.size(); i++) {
		datafile << vGroups.at(i).ID << '\t';
		datafile << vGroups.at(i).Period << '\t';
		datafile << vGroups.at(i).Alloc << endl;
	}

	/***** Now add the programme body *******************/
	// 2. For each icon
	pos = flattened->GetHeadPosition();

	while(pos) {

		blob = (ConData *) (flattened->GetNext(pos));

		// check that a startport is connected or an internalport exists or it is the new style constant without a startport
		bool writeObject = FALSE;
		for (i=0; i<blob->startport_num; i++) {
			if (blob->startport[i]->line.exist || blob->startport[i]->initialise) {
				writeObject = true;
			}
		}

		if (blob->internalport_num > 0) {
			writeObject = true;
		}

		if (writeObject) {

			// 3. Write the tag and class name.
			datafile << "BEGIN ";
			datafile << (CString)blob->className << endl;

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Write the parameter string.
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (blob->iParamNum == 1) {
				datafile << NOPARAMS;

			} else {
				for (i=1; i<blob->iParamNum; i++) {
					// if the parameter is an empty string and there is only 1 parameter 
					// then write out "" else write out "NULL"
					if (blob->iconParam[i]->value == "_") {
						if (blob->iParamNum == 2) {
							datafile << "" << " ";
						}
						else {
							datafile << "NULL" << " ";
						}
					}
					// for string constants don't append a space
					else if (blob->m_csIconType == "const_s") {
							datafile << (CString)blob->iconParam[i]->value;
					}
					// for gui components prepend %%%_
					else if (i==1 && (blob->isGuiWidget())) {
						datafile << "%%%_" << (CString)blob->iconParam[i]->value << " ";
					}
					// for screen tags write out the target filename
					else if (blob->iconParam[i]->dataType == 4) {
						LucidErrEnum err = pProject->pProjMData->getScreenTagMetas((CString &)blob->iconParam[i]->value, tagData);
						assert (err == kErr_NoErr);
						datafile << (CString)tagData.tgtFilename << " ";
					}
					// write out target filename for data files
					else if (blob->m_csIconType.Find("file_ro") != -1 && blob->iconParam[i]->name == "File name") {
						if (pProject->pProjMData->getTargetFileNameForDataHostFileName(blob->iconParam[i]->value, csTargetFileName)) {
							csMessage.Format("File \"%s\" does not exist in the project. Your application may not work as expected.", blob->iconParam[i]->value);
							// Don't display message because IPlayer demo runs a script which relies on host filenames
							//AfxMessageBox(csMessage);
							datafile << (CString)blob->iconParam[i]->value << " ";
						}
						else {
							datafile << (CString)csTargetFileName << " ";
						}
					}
					else {
						datafile << (CString)blob->iconParam[i]->value << " ";
					}

				}

			} // (blob->iParamNum != 1)

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// <ENDOF> Write the parameter string.
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


			datafile << endl;
			// 5. Write the tag to begin listing functions


			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// For each start trigger port
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			for (i=0; i<blob->startport_num; i++) {

				funcInPortNum = 0; funcOutPortNum = 0, funcFinPortNum = 0;
				// if the start port is initialised SODL requires a start function
				if (blob->startport[i]->initialise) {
					startFunc = 1;
				}
				// if the start port is connected or initialised
				if (blob->startport[i]->line.exist || blob->startport[i]->initialise) {
					// 7.1 Write its function name. Is there only 1 function?
					funcName = blob->startport[i]->funcName->GetAt(0);
					datafile << (CString)funcName << '\t';
					// Write out atomic flag
					//datafile << blob->startport[i]->atomicFlag << '\t';
					datafile << 1 << '\t';
					// Write out group ID for start port
					datafile << blob->startport[i]->groupID << '\t';
					// Write out start trigger line ID
					datafile << blob->startport[i]->line.idNum << '\t';
					// 7.2 Search through all other ports to find any other references
					// to the function name
					lineID.RemoveAll();
					lineType.RemoveAll();
					for (j=0; j<blob->inputport_num; j++) {
						for (int k=0; k<blob->inputport[j]->funcName->GetSize(); k++) {
							if (blob->inputport[j]->funcName->GetAt(k) == funcName) {
								funcInPortNum++;
								funcArg = blob->inputport[j]->funcArg->GetAt(k);
								//store the line id and type in a temp. array
								// check if a line is connected
								if (blob->inputport[j]->line.exist) {
									lineID.SetAtGrow(funcArg, blob->inputport[j]->line.idNum);
								}
								else {
									lineID.SetAtGrow(funcArg, -1);		
								}
								lineType.SetAtGrow(funcArg, blob->inputport[j]->datatype);
							}
						}
					}

					// write out inputs
					datafile << funcInPortNum << " ";
					for (j=1; j<=funcInPortNum; j++) {
						datafile << convert.DataType2Char(lineType.GetAt(j)) << " ";
						// Originally wrote out a * for uncoonected ports. Now writes out -1.
						/*if (lineID.GetAt(j) == -1) {
							datafile << "* ";
						}
						else {*/
							datafile << lineID.GetAt(j) << " ";
						//}
					}

					for (j=0; j<blob->outputport_num; j++) {
						for (int k=0; k<blob->outputport[j]->funcName->GetSize(); k++) {
							if (blob->outputport[j]->funcName->GetAt(k) == funcName) {
								funcOutPortNum++;
								funcArg = blob->outputport[j]->funcArg->GetAt(k);
								//store the line id and type in a temp. array
								// check if a line is connected
								if (blob->outputport[j]->lineID > -1) {
									lineID.SetAtGrow(funcArg, blob->outputport[j]->lineID);
								}
								else {
									lineID.SetAtGrow(funcArg, -1);
								}
								lineType.SetAtGrow(funcArg, blob->outputport[j]->datatype);
							}
						}
					}
					
					// write out outputs
					datafile << '\t' << funcOutPortNum << " ";
					for (j=(funcInPortNum + 1); j<=(funcInPortNum + funcOutPortNum); j++) {
						datafile << convert.DataType2Char(lineType.GetAt(j)) << " ";
						datafile << lineID.GetAt(j) << " ";
					}
					
					for (j=0; j<blob->finishport_num; j++) {
						for (int k=0; k<blob->finishport[j]->funcName->GetSize(); k++) {
							if (blob->finishport[j]->funcName->GetAt(k) == funcName) {
								funcFinPortNum++;
								funcArg = blob->finishport[j]->funcArg->GetAt(k);
								//store the line id and type in a temp. array
								// check if a line is connected
								if (blob->finishport[j]->lineID > -1) {
									lineID.SetAtGrow(funcArg, blob->finishport[j]->lineID);
								}
								else {
									lineID.SetAtGrow(funcArg, -1);
								}
							}
						}
					}
					// write out finish ports
					datafile << '\t' << funcFinPortNum << " ";
					// Only write out the finish line numbers if there is at least one connection
					if (funcFinPortNum > 0) {
						for (int m=(funcInPortNum + funcOutPortNum + 1); m<lineID.GetSize(); m++) {
							datafile << lineID.GetAt(m) << " ";
						}
					}
					
					datafile << endl;
				}
			}
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// <ENDOF> For each start trigger port
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// For each internal trigger port
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			for (i=0; i<blob->internalport_num; i++) {
				funcInPortNum = 0; funcOutPortNum = 0, funcFinPortNum = 0;
				// ***REVISIT. Write its function name. Is there only 1 function?
				funcName = blob->internalport[i]->funcName->GetAt(0);
				datafile << (CString)funcName << '\t';
				// Write out atomic flag
				//datafile << blob->internalport[i]->atomicFlag << '\t';
				datafile << 1 << '\t';
				// Write out group ID for internal port
				datafile << blob->internalport[i]->groupID << '\t';
				// The internal trigger does not have a line ID so write out 0 if it is the
				// Start function and -1 otherwise.
				if (funcName == START) {
					datafile << 0 << '\t';
				}
				else {
					datafile << -1 << '\t';
				}
				// 7.2 Search through all other ports to find any other references
				// to the function name
				lineID.RemoveAll();
				lineType.RemoveAll();
				for (j=0; j<blob->inputport_num; j++) {
					for (int k=0; k<blob->inputport[j]->funcName->GetSize(); k++) {
						if (blob->inputport[j]->funcName->GetAt(k) == funcName) {
							funcInPortNum++;
							funcArg = blob->inputport[j]->funcArg->GetAt(k);
							//store the line id and type in a temp. array
							// check if a line is connected
							if (blob->inputport[j]->line.exist) {
								lineID.SetAtGrow(funcArg, blob->inputport[j]->line.idNum);
							}
							else {
								lineID.SetAtGrow(funcArg, -1);		
							}
								lineType.SetAtGrow(funcArg, blob->inputport[j]->datatype);
						}
					}
				}
				// write out inputs
				datafile << funcInPortNum << " ";
				for (j=1; j<=funcInPortNum; j++) {
					datafile << convert.DataType2Char(lineType.GetAt(j)) << " ";
					datafile << lineID.GetAt(j) << " ";
				}

				for (j=0; j<blob->outputport_num; j++) {
					for (int k=0; k<blob->outputport[j]->funcName->GetSize(); k++) {
						if (blob->outputport[j]->funcName->GetAt(k) == funcName) {
							funcOutPortNum++;
							funcArg = blob->outputport[j]->funcArg->GetAt(k);
							//store the line id and type in a temp. array
							// check if a line is connected
							if (blob->outputport[j]->lineID > -1) {
								lineID.SetAtGrow(funcArg, blob->outputport[j]->lineID);
							}
							else {
								lineID.SetAtGrow(funcArg, -1);
							}
							lineType.SetAtGrow(funcArg, blob->outputport[j]->datatype);
						}
					}
				}
					
				// write out outputs
				datafile << '\t' << funcOutPortNum << " ";
				for (j=(funcInPortNum + 1); j<=(funcInPortNum + funcOutPortNum); j++) {
					int tmp = lineType.GetAt(j);
					datafile << convert.DataType2Char(lineType.GetAt(j)) << " ";
					datafile << lineID.GetAt(j) << " ";
				}
					
				for (j=0; j<blob->finishport_num; j++) {
					for (int k=0; k<blob->finishport[j]->funcName->GetSize(); k++) {
						if (blob->finishport[j]->funcName->GetAt(k) == funcName) {
							funcFinPortNum++;
							funcArg = blob->finishport[j]->funcArg->GetAt(k);
							//store the line id and type in a temp. array
							// check if a line is connected
							if (blob->finishport[j]->lineID > -1) {
									lineID.SetAtGrow(funcArg, blob->finishport[j]->lineID);
							}
							else {
								lineID.SetAtGrow(funcArg, -1);
							}
						}
					}
				}
				// write out finish ports
				datafile << '\t' << funcFinPortNum << " ";
				for (int m=(funcInPortNum + funcOutPortNum + 1); m<lineID.GetSize(); m++) {
					datafile << lineID.GetAt(m) << " ";
				}		
				datafile << endl;
			}
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// <ENDOF> For each internal trigger port
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


			// 8. Write out end of icon tags
			datafile << "END" << endl;
			//datafile << endl;

		} //if (writeObject)
	}
	// Always Write out start function -- even if no other functions need initialising.
	//if (startFunc) {
		datafile << "BEGIN Start" << endl;
		datafile << "*" << endl;
		datafile << "Run_Start	1	1	0	0 	0 	1 1" << endl;
		datafile << "END" << endl;
	//}
	datafile.close();
}

void SODL::Copy2Flattened() {	
	ConData* blob;
	long int id = 0;
	INXPOSITION pos;
		char * type =new char[256];
		int blankcount=0;

	// Copy the condata list to the flattened list
	// save the condata list and then load it back in to flattened
	// Alternatively could use a copy constructor
	pos = flattened->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (flattened->GetNext(pos));
		delete blob;
	}
	flattened->RemoveAll();
	// need to do save in view class
	//SaveProg(workDir + TEMPDIR + "temp");
	ifstream datafile(workDir + TEMPDIR + "temp");


	while ((!datafile.eof()) && (!datafile.bad()) && (blankcount<1000000)) {
		datafile >> type;
		if (strcmp(type,"END_OF_BLOCKS")==0)
			break;
		else
			if (strcmp(type,"BEGIN_BLOCK")==0) {
				blob = new ConData;
				blob->Load(&datafile);
				flattened->AddTail((INXObject*) blob);
				id = blob->identnum;
			} else {
				blankcount++;
			}
	}
	
	// set the uniqueidgenerator to the identnum of the last icon
	// This is necessary as it is possible that this value may be greater than
	// the number of icons loaded, due to icons being deleted previously. This
	// prevents icon IDs being duplicated
	// Need to add 1, since uniqueidgenerator is incremented after a new icon is
	// instantiated.
	id++;
	ConData::uniqueidgenerator = id;
	delete type;
}

// Function that flattens all encapsulated icons.
void SODL::Flatten()
{
	ConData *blob, *blockIcon, *blockOtherIcon, *blobOtherIcon;
	INXObjList* encapsulated;
	INXPOSITION pos, rmpos, otherPos, blockPos;
	int otherportno;
	INXString blockFile, csProjectDir;
	bool allFlattened = FALSE;
	bool flattenFlag = FALSE;
	BlockOperations bo;
	UINT i;
	
	pProject->pProjMData->getProjectDir(csProjectDir);

	// This first loop is used to catch the case when an encapsulated icon that contains other 
	// encapsulated blocks is the last element in the list.
	// If it didn't exist then the icon would be flattened but the encapsulated icons within would not be
	// because GetNext(pos) would return null.
	while (!allFlattened) {

		flattenFlag = FALSE;
		// Find encapsulated blocks and flatten
		pos = flattened->GetHeadPosition();
		while(pos) {

			rmpos = pos;
			blob = (ConData *) (flattened->GetNext(pos));

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (blob->m_iUserDefined == 1) {
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

				flattenFlag = TRUE;
				blockFile = csProjectDir + DEPDIR + blob->hierarchyName + blob->description + ".prg";
				encapsulated = bo.LoadBlock(blockFile);
				// Give the block icons unique IDs
				//AfxMessageBox("About to call ReassignIconIDs ( from within 'Flatten' )");
				ReassignIconIDs(encapsulated);
				bo.SaveBlock(blockFile, encapsulated);

				// Hook up the encapsulated block
				// This is done in 2 passes for the case when a subsystem input is connected to an output on the same
				// subsystem. The 2 pass process takes care of the case when the output xport is in the list before
				// the FB connected to the input xport.
				// The first pass basically hooks up all the FBs.
				// The second pass hooks up all the output xports.
				blockPos = encapsulated->GetHeadPosition();
				while(blockPos) {
					blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
					// find the ports connected to a XINPUT
					if (blockIcon->m_csIconType.Find("XINPUT") == -1 && blockIcon->m_csIconType != "XSTART" && 
						blockIcon->m_csIconType.Find("XOUTPUT") == -1 && blockIcon->m_csIconType != "XFINISH") {
						for (i=0; i<blockIcon->inputport_num; i++) {
							// check the input is connected
							if (blockIcon->inputport[i]->line.exist) {

								blockOtherIcon = 
									bo.GetBlockIconFromID(blockIcon->inputport[i]->line.othericonid, encapsulated);

								// if an encapsulated icon is connected to an XINPUT then connect it to the icon
								// the XINPUT is connected to in the level above
								if (blockOtherIcon->m_csIconType.Find("XINPUT") != -1) {
									for (UINT j=0; j<blob->inputport_num; j++) {
										if (blockOtherIcon->description == blob->inputport[j]->description) { 
											if (blob->inputport[j]->line.exist) {
												blockIcon->inputport[i]->line.othericonid = blob->inputport[j]->line.othericonid;
												blockIcon->inputport[i]->line.otherportno = blob->inputport[j]->line.otherportno;
											}
											// if XINPUT on the encapsulated block is unconnected then the corresponding
											// port on the block should be unconnected
											else {
												blockIcon->inputport[i]->line.exist = 0;
											}
										}
									}
								}
							}
						} // for (i=0; i<blockIcon->inputport_num; i++) 

						// find the ports connected to a XSTART
						for (i=0; i<blockIcon->startport_num; i++) {
							// check the input is connected
							if (blockIcon->startport[i]->line.exist) {
								blockOtherIcon = bo.GetBlockIconFromID(blockIcon->startport[i]->line.othericonid, encapsulated);
								// if an encapsulated icon is connected to a XSTART then connect it to the icon
								// the XSTART is connected to in the level above
								if (blockOtherIcon->m_csIconType == "XSTART") {
									for (UINT j=0; j<blob->startport_num; j++) {
										if (blockOtherIcon->description == blob->startport[j]->description) { 
											if (blob->startport[j]->line.exist) {
												blockIcon->startport[i]->line.othericonid = blob->startport[j]->line.othericonid;
												blockIcon->startport[i]->line.otherportno = blob->startport[j]->line.otherportno;
											}
											// case when start port is initialised
											else if (blob->startport[j]->initialise) {
												blockIcon->startport[i]->initialise = 1;
												blockIcon->startport[i]->line.exist = 0;
												//blockIcon->startport[i]->line.othericonid = -1;
											}
											// if XSTART on the encapsulated block is unconnected then the corresponding
											// port on the block should be unconnected
											else {
												blockIcon->startport[i]->line.exist = 0;
											}
										}
									}
								}
							}
						}

						flattened->AddTail(blockIcon);
					} // if (blockIcon->m_csIconType.Find("XINPUT") == -1 && blockIcon->m_csIconType != "XSTART")

				} // while(blockPos) 

				blockPos = encapsulated->GetHeadPosition();
				while(blockPos) {
					blockIcon = (ConData *) (encapsulated->GetNext(blockPos));

					// find the ports connected to a XOUTPUT
					if (blockIcon->m_csIconType.Find("XOUTPUT") != -1) {
						blockOtherIcon = bo.GetBlockIconFromID(blockIcon->inputport[0]->line.othericonid, encapsulated);
						// find the input port in flattened connected to this XOUTPUT
						otherPos = flattened->GetHeadPosition();
						while(otherPos) {
							blobOtherIcon = (ConData *) (flattened->GetNext(otherPos));
							if (blobOtherIcon != blob) {
								for (i=0; i<blobOtherIcon->inputport_num; i++) {
									if (blobOtherIcon->inputport[i]->line.exist) {
										otherportno = blobOtherIcon->inputport[i]->line.otherportno;
										// if XOUTPUT is not connected then remove line from blobOtherIcon
										if (blobOtherIcon->inputport[i]->line.othericonid == blob->identnum &&
											blockIcon->description == blob->outputport[otherportno]->description &&
											!blockIcon->inputport[0]->line.exist) {
											blobOtherIcon->inputport[i]->line.exist = 0;
										}
										// if the input port in the flattened list is connected and
										// the icon it is connected to is the encapsulated block and
										// the port it is connected to is this XOUTPUT then connect it
										// to the icon in the encapsulated block
										else if ((blobOtherIcon->inputport[i]->line.othericonid == blob->identnum) &&
											(blockIcon->description == blob->outputport[otherportno]->description)) {
											blobOtherIcon->inputport[i]->line.othericonid = blockIcon->inputport[0]->line.othericonid;
											blobOtherIcon->inputport[i]->line.otherportno = blockIcon->inputport[0]->line.otherportno;
										}
									}
								}
							}
						}
					}

					// find the ports connected to a XFINISH
					if (blockIcon->m_csIconType == "XFINISH") {
						blockOtherIcon = bo.GetBlockIconFromID(blockIcon->startport[0]->line.othericonid, encapsulated);
						// find the start port in flattened connected to this XFINISH
						otherPos = flattened->GetHeadPosition();
						while(otherPos) {
							blobOtherIcon = (ConData *) (flattened->GetNext(otherPos));
							if (blobOtherIcon != blob) {
								for (i=0; i<blobOtherIcon->startport_num; i++) {
									if (blobOtherIcon->startport[i]->line.exist) {
										otherportno = blobOtherIcon->startport[i]->line.otherportno;
										// if XFINISH is not connected then remove line from blobOtherIcon
										if (blobOtherIcon->startport[i]->line.othericonid == blob->identnum &&
											blockIcon->description == blob->finishport[otherportno]->description &&
											!blockIcon->startport[0]->line.exist && !blockIcon->startport[0]->initialise) {
											blobOtherIcon->startport[i]->line.exist = 0;
										}
										// if the input port in the flattened list is connected and
										// the icon it is connected to is the encapsulated block and
										// the port it is connected to is this XOUTPUT then connect it
										// to the icon in the encapsulated block
										if ((blobOtherIcon->startport[i]->line.othericonid == blob->identnum) &&
											(blockIcon->description == blob->finishport[otherportno]->description)) {
											if (blockIcon->startport[0]->initialise) {
												blobOtherIcon->startport[i]->initialise = 1;
												blobOtherIcon->startport[i]->line.exist = 0;
												blobOtherIcon->startport[i]->line.othericonid = -1;
											}
											else {
												blobOtherIcon->startport[i]->line.othericonid = blockIcon->startport[0]->line.othericonid;
												blobOtherIcon->startport[i]->line.otherportno = blockIcon->startport[0]->line.otherportno;
											}
										}
									}
								}
							}
						}
					} // if (blockIcon->m_csIconType == "XFINISH") 
				} // while(blockPos) 

				// destroy xports
				blockPos = encapsulated->GetHeadPosition();
				while(blockPos) {
					blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
					if (blockIcon->m_csIconType.Find("XINPUT") != -1 || blockIcon->m_csIconType.Find("XOUTPUT") != -1 || 
						blockIcon->m_csIconType == "XSTART" || blockIcon->m_csIconType == "XFINISH") {
						delete blockIcon;
					}
				}
				// remove encapsulated block from flattened
				delete blob;
				flattened->RemoveAt(rmpos);
				// destroy encapsulated icon
				// causes a crash
				//delete blob;
				// destroy list of encapsulated icons
				delete encapsulated;

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			} // 		if (blob->m_iUserDefined == 1) 
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		}

		if (!flattenFlag) {
			allFlattened = TRUE;
		}

	} // while (!allFlattened) 
}

// Function that gives the icons in an encapsulated block unique IDs
void SODL::ReassignIconIDs(INXObjList* encapsulated)
{
	long int newID;
	INXPOSITION blockPos, otherPos;
	ConData* blockIcon;
	ConData* otherIcon;
	UINT i;
	
	// Get the current icon ID and increment
	newID = ConData::uniqueidgenerator;
	newID++;
	newID = CheckNewID(encapsulated, newID);
	
	// for each icon in the encapsulated block that is not an xport find the ports connected 
	// to the outputs and update the othericonid attribute in their line object.
	// Then update the ID for the icon
	blockPos = encapsulated->GetHeadPosition();
	while(blockPos) {
		blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
		// if assign new id to xports then don't need checkNewID function
		if (blockIcon->m_csIconType.Find("XINPUT") == -1 && blockIcon->m_csIconType != "XSTART"
			&& blockIcon->m_csIconType.Find("XOUTPUT") == -1 && blockIcon->m_csIconType != "XFINISH") {
			otherPos = encapsulated->GetHeadPosition();
			while(otherPos) {
				otherIcon = (ConData *) (encapsulated->GetNext(otherPos));
				for (i=0; i<otherIcon->inputport_num; i++) {
				// if the input port is connected to blockIcon then change othericonid
					if (otherIcon->inputport[i]->line.exist && 
						(otherIcon->inputport[i]->line.othericonid == blockIcon->identnum)) {
						otherIcon->inputport[i]->line.othericonid = newID;
					}
				}
				for (i=0; i<otherIcon->startport_num; i++) {
				// if the input port is connected to blockIcon then change othericonid
					if (otherIcon->startport[i]->line.exist && 
						(otherIcon->startport[i]->line.othericonid == blockIcon->identnum)) {
						otherIcon->startport[i]->line.othericonid = newID;
					}
				}
			}
			// The following is done for debug mode, so that when view a block it is possible
			// to map the flattened list onto the block.
			//blockIcon->blockID = blockIcon->identnum;
			blockIcon->identnum = newID;
			newID++;
			newID = CheckNewID(encapsulated, newID);
		}
	}
	ConData::uniqueidgenerator = newID;
}

// Function that checks the newID for the encapsulated icon does not already exist within the
// encapsulated block. If it does it increments newID until it is unique.
long SODL::CheckNewID(INXObjList* encapsulated, long int id) {
	INXPOSITION blockPos;
	ConData* blockIcon;
	bool uniqueID = FALSE;
	bool flag = FALSE;
	
	while (!uniqueID) {
		flag = FALSE;
		blockPos = encapsulated->GetHeadPosition();
		while(blockPos) {
			blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
			if (blockIcon->identnum == id) {
				id++;
				flag = TRUE;
			}
		}
		if (!flag) {
			uniqueID = TRUE;
		}
	}

	return id;
}

// Function that assigns a unique incremental integer (within its data type) to each line
void SODL::AssignLineIDNum() {
	ConData *blob;
	INXPOSITION pos;
	// eventLineInd is 2 initially, as 0 is reserved for Run_Start start trigger, and
	// 1 is reserved for Run_Start finish trigger
	long int boolLineInd = 1, intLineInd = 1, floatLineInd = 1, strLineInd = 1, eventLineInd = 2;
	long int otherPortLineID = -1;
	UINT i;

	// initialise line ID in output and finish ports
	InitLineID();
	
	pos = flattened->GetHeadPosition();
	// for every icon
	while (pos) {
		blob = (ConData *) (flattened->GetNext(pos));

		// for each input port
		for (i=0;i<blob->inputport_num;i++) {
			if (blob->inputport[i]->line.exist) {
				if (blob->inputport[i]->datatype == BOOLEAN) {
					// check whether other end is already connected.
					// If this is the case assign it the same line ID
					otherPortLineID = GetOtherPortLineID(blob, INPUTPORT, i);
					if (otherPortLineID > -1) {
						blob->inputport[i]->line.setIDNum(otherPortLineID);
					}
					else {
						blob->inputport[i]->line.setIDNum(boolLineInd);
						// assign line ID to port at other end
						AssignLineID2OtherPort(blob, INPUTPORT, i, boolLineInd);
						boolLineInd++;
					}
				}
				else if (blob->inputport[i]->datatype == INTEGER) {
					otherPortLineID = GetOtherPortLineID(blob, INPUTPORT, i);
					if (otherPortLineID > -1) {
						blob->inputport[i]->line.setIDNum(otherPortLineID);
					}
					else {
						blob->inputport[i]->line.setIDNum(intLineInd);
						AssignLineID2OtherPort(blob, INPUTPORT, i, intLineInd);
						intLineInd++;
					}
				}
				else if (blob->inputport[i]->datatype == FLOAT) {
					otherPortLineID = GetOtherPortLineID(blob, INPUTPORT, i);
					if (otherPortLineID > -1) {
						blob->inputport[i]->line.setIDNum(otherPortLineID);
					}
					else {
						blob->inputport[i]->line.setIDNum(floatLineInd);
						AssignLineID2OtherPort(blob, INPUTPORT, i, floatLineInd);
						floatLineInd++;
					}
				}
				else if (blob->inputport[i]->datatype == STRING) {
					otherPortLineID = GetOtherPortLineID(blob, INPUTPORT, i);
					if (otherPortLineID > -1) {
						blob->inputport[i]->line.setIDNum(otherPortLineID);
					}
					else {
						blob->inputport[i]->line.setIDNum(strLineInd);
						AssignLineID2OtherPort(blob, INPUTPORT, i, strLineInd);
						strLineInd++;
					}
				}
			}
		}
		// for each start port
		// control can have same line numbers as data as there is a separate
		// table for control
		for (i=0;i<blob->startport_num;i++) {
			if (blob->startport[i]->line.exist) {
				// check whether other end is already connected.
				// If this is the case assign it the same line ID
				if ((blob->identnum == 143 && i==2) || (blob->identnum == 174 && i==0)) {
					otherPortLineID = GetOtherPortLineID(blob, STARTPORT, i);
				}
				otherPortLineID = GetOtherPortLineID(blob, STARTPORT, i);
				if (otherPortLineID > -1) {
					blob->startport[i]->line.setIDNum(otherPortLineID);
				}
				else {
					blob->startport[i]->line.setIDNum(eventLineInd);
					AssignLineID2OtherPort(blob, STARTPORT, i, eventLineInd);
					eventLineInd++;
				}
			}
			// if the startport is initialised then set the line ID to 1
			// Note that line.exist is 0, since a line does not exist on the diagram
			else if (blob->startport[i]->initialise) {
				blob->startport[i]->line.setIDNum(1);
			}
		}
	}
}

// function that initialises the line ID in the output and finish ports
void SODL::InitLineID() {
	ConData *blob;
	INXPOSITION pos;
	UINT i;

	pos = flattened->GetHeadPosition();
	while (pos) {
		blob = (ConData *) (flattened->GetNext(pos));
		for (i=0; i<blob->outputport_num; i++) {
			blob->outputport[i]->setLineID(-1);
		}
		for (i=0; i<blob->finishport_num; i++) {
			blob->finishport[i]->setLineID(-1);
		}
	}
}

// Function that gets the line ID from an output port. This is used to check whether an output
// is connected
long SODL::GetOtherPortLineID(ConData *blob, int portType, int portNum) {
	long othericonid = -1;
	long otherlineid = -1;
	int otherportno = 0;
	ConData *blob2;
	
	if (portType == INPUTPORT) {
		othericonid = blob->inputport[portNum]->line.othericonid;
		otherportno = blob->inputport[portNum]->line.otherportno;
	}
	else if (portType == STARTPORT) {
		othericonid = blob->startport[portNum]->line.othericonid;
		otherportno = blob->startport[portNum]->line.otherportno;
	}

	// kwhite -handle null object - see similar function assignlineid2otherport
	if ( (blob2 = GetFlatIconFromID(othericonid))==NULL)
	{
		//Error occurs in both this function and AssignLineID2OtherPort
		return -1;
	}
		
	if (portType ==INPUTPORT) {
		otherlineid = blob2->outputport[otherportno]->lineID;
	}
	else if (portType == STARTPORT) {
		otherlineid = blob2->finishport[otherportno]->lineID;
	}
	
	return otherlineid;
}

/*
Searches all icons and return an object pointer for a ConObject with matching identnum.
  */
ConData* SODL::GetFlatIconFromID(long id) {
	ConData* blob;
	INXPOSITION pos;

	pos = flattened->GetHeadPosition();

	while (pos) { 		
		blob=(ConData *) (flattened->GetNext(pos));
		if (blob->identnum==id) 
			return blob;
	}
	return NULL;
}

// This function assigns a line ID to the ouput or finish port
void SODL::AssignLineID2OtherPort(ConData *blob, int portType, int portNum, long lineID) {
	long othericonid = -1;
	int otherportno = 0;
	static int stop_checking = 0; //This is messy but prevents too many user prompts.
	int response;

	ConData *blob2;
	
	if (portType == INPUTPORT) {
		othericonid = blob->inputport[portNum]->line.othericonid;
		otherportno = blob->inputport[portNum]->line.otherportno;
	}
	else if (portType == STARTPORT) {
		othericonid = blob->startport[portNum]->line.othericonid;
		otherportno = blob->startport[portNum]->line.otherportno;
	}

	//	kwhite - Handle null object if widget file corrupt or missing
	if ( (blob2 = GetFlatIconFromID(othericonid)) == NULL)
	{
		INXString message;
		message.Format("Missing component linked to %s ID %d.\nThe Application may not work as expected.\nPlease seek assistance from nCapsa.\nContinue?", blob->description, blob->identnum);
		if(stop_checking==0)
			response = AfxMessageBox(message ,MB_YESNO);

		if (response == IDNO) 
			stop_checking = 1;	
		
		return;
	}

	// set the line id in the appropriate output port
	if (portType == INPUTPORT) {
		blob2->outputport[otherportno]->setLineID(lineID);
	}
	else if (portType == STARTPORT) {
		blob2->finishport[otherportno]->setLineID(lineID);
	}

}


// reads/incrementsstores the build number to persistant storage. 
int SODL::UpdateBuildNum(void)
{
	INXString csProjectDir;
	char lineFromFile[256];
	pProject->pProjMData->getProjectDir(csProjectDir);
	int num;
	ifstream read;
	read.open((CString)csProjectDir+"\\build.num",ios_base::in);
	if ( read.is_open() ) {
		read.getline(lineFromFile,256);
		if (read.rdstate() & ifstream::failbit)
			num=1;
		else
			num=atoi(lineFromFile);
	}
	else {
		num=0;
		}
	read.close();
	ofstream write(csProjectDir+"\\build.num",ios_base::out); //apend
	num++;
	write << num;
	write.close();
	
	return num;
}
