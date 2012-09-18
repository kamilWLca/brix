// BlockOperations.cpp: implementation of the BlockOperations class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DrawProg.h"
#include "condat.h"
#include "BlockOperations.h"
#include <wx/msgdlg.h>
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BlockOperations::BlockOperations()
{

}

BlockOperations::~BlockOperations()
{

}

void BlockOperations::AssignIconLineEndPoints(INXObjList* temp) {
	ConData* blob, *blob2;
	INXPOSITION pos;
	
	pos = temp->GetHeadPosition();
	while (pos) { 
		blob = (ConData *) (temp->GetNext(pos));
		for (UINT i=0;i<blob->inputport_num;i++) {
			// only add line if its not already connected
			if (blob->inputport[i]->line.exist && !blob->inputport[i]->connected) {
				blob2 = GetBlockIconFromID(blob->inputport[i]->line.othericonid, temp);
				if (blob2) {
					blob->AddLine(i, INPUTPORT, blob2, blob->inputport[i]->line.otherportno, OUTPUTPORT);
				}
			}
		}
		for (UINT i=0;i<blob->startport_num;i++) {
			if (blob->startport[i]->line.exist && !blob->startport[i]->connected) {
				blob2 = GetBlockIconFromID(blob->startport[i]->line.othericonid, temp);
				if (blob2) blob->AddLine(i, STARTPORT, blob2, blob->startport[i]->line.otherportno, FINISHPORT);
			}
		}
	} 
}

/*
Searches all icons and return an object pointer for a ConObject with matching identnum.
  */
ConData* BlockOperations::GetBlockIconFromID(long id, INXObjList* encapsulated) {
	ConData* blob;
	INXPOSITION pos;

	pos = encapsulated->GetHeadPosition();
	while (pos) { 		
		blob=(ConData *) (encapsulated->GetNext(pos));
		if (blob->identnum==id) return blob;
	}
	return NULL;
}

// Loads an encapsulated block into a temporary list
INXObjList* BlockOperations::LoadBlock(INXString Info) {
	char *tempChar = (char*)Info.c_str();
	ifstream datafile(tempChar);
	INXObjList* encapsulated = new INXObjList;
	char type[256];
	long int id;

	// store the id value
	id = uniqueidgenerator;


	if(datafile.is_open())
	{
		int i = 0;
		while ((!datafile.eof()) && (!datafile.bad())) {
			i++;
			datafile >> type;
		//	TRACE(type);
			if (strcmp(type,"END_OF_BLOCKS")==0) break;
			else
			if (strcmp(type,"BEGIN_BLOCK")==0) {
				ConData *blob = new ConData;
				blob->Load(&datafile);
				encapsulated->AddTail((INXObject*) blob);
			}
			if(i>500)
			{
			//	TRACE("hang\n");

			}

		}
	}
	else
	{
		INXString a;
		a.Format(wxT("Can't open file: %s"),(char*)Info);
		wxMessageBox(a);

	}

	// restore the id value, since it will have been increased when creating flattened
	id++;
	uniqueidgenerator = id;

	AssignIconLineEndPoints(encapsulated);
	return encapsulated;
}

INXObjList* BlockOperations::NewBlock(INXString Info) {
	
	INXObjList* encapsulated = new INXObjList;

	uniqueidgenerator++;

	return encapsulated;
}

// Saves an encapsulated block
int BlockOperations::SaveBlock(INXString Info, INXObjList* encapsulated) {
	ofstream datafile(Info);
	//put an error trap
	ConData *blob;
	
	if (!datafile.good()) {
		wxMessageBox(wxT("File could not be written. Check the file does not already exist."));
		return 0;
	}
	
	datafile << "IconData" << endl;
	INXPOSITION pos;
	pos = encapsulated->GetHeadPosition();

	while (pos) { 		
		blob=(ConData *) (encapsulated->GetNext(pos));
		datafile<<"BEGIN_BLOCK"<<endl;
		blob->Save(&datafile);
	} 

	datafile<<"END_OF_BLOCKS"<<endl;
	datafile.close();
	// make the file read-only
	//_chmod(Info, _S_IREAD);
	return 1;

}

void BlockOperations::DeleteBlock(INXObjList* encapsulated) {
	ConData* blob;
	INXPOSITION pos;
	pos = encapsulated->GetHeadPosition();

	while (pos) { 		
		blob=(ConData *) (encapsulated->GetNext(pos));
		delete blob;
	}
	delete encapsulated;
}
