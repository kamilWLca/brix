// DEP.cpp: implementation of the DEP class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawProg.h"
#include "condat.h"
#include "DEP.h"
#include "Project.h" //@todo 
//#include "IconParams.h" //@todo 
//#include "SODL.h" //@todo 
//#include "Encapsulation.h" //@todo 
//#include "TagDialog.h" //@todo 
#include "MainFrame.h"
#include "FileOperations.h" //@todo 
#include "BlockOperations.h" //@todo 
#include "GlobalFuncs_2.h" 
#include "Porting_Classes/INXObject.h"
#include "wx/wx.h"
#include <wx/msgdlg.h>
#include <cmath>
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
// global variable declared in CDrawProgApp
//extern char workDir[WORK_DIR_SIZE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DEP::DEP()
{
	condata = new INXObjList;
	//pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	depFilename = "";
	dbgMappedFlag = FALSE;
	boolTags = new INXObjArray<INXString>;
	intTags = new INXObjArray<INXString>;
	realTags = new INXObjArray<INXString>;
	stringTags = new INXObjArray<INXString>;
	eventTags = new INXObjArray<INXString>;

	m_CanvasSize.cx = 2000;
	m_CanvasSize.cy = 2000;
}

DEP::~DEP()
{
	ConData* blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();
	while (pos) {
		blob = (ConData*) (condata->GetNext(pos));
		if (blob) delete blob;
	}
	delete condata;
	delete boolTags;
	delete intTags;
	delete realTags;
	delete stringTags;
	delete eventTags;
}

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
int DEP::AddBlockPort(INXString type, INXString portLabel, INXString blockName) {
	int portType, dataType, ret;
	ConData* block;
	INXPOSITION pos;

	// get the port type
	if (type.Find("XINPUT") != -1) {
		portType = INPUTPORT;
	}
	else if (type.Find("XOUTPUT") != -1) {
		portType = OUTPUTPORT;
	}
	else if (type == "XSTART") {
		portType = STARTPORT;
	}
	else if (type == "XFINISH") {
		portType = FINISHPORT;
	}
	// get the data type
	if (type == "XINPUTB" || type == "XOUTPUTB") {
		dataType = BOOLEAN;
	}
	else if (type == "XINPUTI" || type == "XOUTPUTI") {
		dataType = INTEGER;
	}
	else if (type == "XINPUTR" || type == "XOUPUTR") {
		dataType = FLOAT;
	}
	else if (type == "XINPUTS" || type == "XOUTPUTS") {
		dataType = STRING;
	}
	// get the block and add the port
	pos = condata->GetHeadPosition();
	while (pos) {
		block = (ConData*) (condata->GetNext(pos));
		if (block->description == blockName) {
			ret = AddPort(block, dataType, portType, portLabel);
		}
	}

	return ret;
}


// This function adds an icon to the condata list
INXPOSITION  DEP::AddIcon(INXString csIconType, INXString csBlockName, INXPoint point, int iShow) {
	ConData *blobb = new ConData;
	long iId;

	blobb->init(csIconType, csBlockName, point, iShow);
	// ensure id is unique
	iId = blobb->identnum;
	while (!IsUniqueID(iId)) {
		iId++;
	}
	blobb->identnum = iId;
	ConData::uniqueidgenerator = iId;

	return AddIconObjectList(blobb);
}

/*
Adds an Icon Object to the Icon List and reurns a CObject POSITION structure
  */
INXPOSITION DEP::AddIconObjectList(ConData* blob) { //add all blobs using this function
	return condata->AddTail(blob);
}

// Sets the groupID in all the start ports and internal ports for the specified icon
void DEP::AddIconToGroup(INXPOSITION selectedIcon, int groupID) {
	ConData* blob;
	UINT i;

	blob = (ConData*) condata->GetAt(selectedIcon);
	// set the group ID in all the start ports
	for (i=0; i<blob->startport_num; i++) {
		blob->startport[i]->groupID = groupID;
	}
	// set the group ID in all the internal ports
	for (i=0; i<blob->internalport_num; i++) {
		blob->internalport[i]->groupID = groupID;
	}
}

/**
 * Returns true if both icons passed in are xports
 *
 */
#endif
bool areBothIconsXPorts(ConData *pFirstIcon, ConData *pSecondIcon)
{
	bool bAreBothIcons = false;
	bool bFirstIsXport = false;
	bool bSecondIsXport = false;

	if (pFirstIcon && pSecondIcon) {
		if (pFirstIcon->m_csIconType.Find("XINPUT") != -1 || pFirstIcon->m_csIconType.Find("XOUTPUT") != -1 || pFirstIcon->m_csIconType == wxT("XSTART") || pFirstIcon->m_csIconType == wxT("XFINISH")) {
			bFirstIsXport = true;
		}
		if (pSecondIcon->m_csIconType.Find("XINPUT") != -1 || pSecondIcon->m_csIconType.Find("XOUTPUT") != -1 || pSecondIcon->m_csIconType == wxT("XSTART") || pSecondIcon->m_csIconType == wxT("XFINISH")) {
			bSecondIsXport = true;
		}

		if (bFirstIsXport && bSecondIsXport) {
			bAreBothIcons = true;
		}
	}
	return bAreBothIcons;
}

/*
Attempt to add a line, once both ends have been identified by the user's mouse clicks.
First this method identifies if the connetion is valid, and then also identifies which icon
is the input and which is the output
return 1 if it was successful otherwise return 0.
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE_____BUT_USED_FOR_CREATING_NEW_LINE
int DEP::AddLine(INXPOSITION selectedIcon, int selectedPort, int selectedPortType, INXPOSITION selectedIcon2,int selectedPort2, int selectedPortType2) {
	ConData* iconIn;
	ConData* iconOut;
	int portIn, portOut, portInType, portOutType;
	int validConnect = 1;


	// Display warning if try connecting to an initialised startport
	if (selectedPortType == STARTPORT) {
		iconIn =(ConData*) condata->GetAt(selectedIcon);
		if (iconIn->startport[selectedPort]->initialise) {
			wxMessageBox("Illegal connection. Start port is already connected to initialiser.");
			depFSM.drawingline = 0;
			return 0;
		}
	}
	if (selectedPortType2 == STARTPORT) {
		iconOut =(ConData*) condata->GetAt(selectedIcon2);
		if (iconOut->startport[selectedPort2]->initialise) {
			wxMessageBox("Illegal connection. Start port is already connected to initialiser.");
			depFSM.drawingline = 0;
			return 0;
		}
	}

	if ((selectedPortType ==INPUTPORT && selectedPortType2 == OUTPUTPORT) ||
		(selectedPortType ==STARTPORT && selectedPortType2 == FINISHPORT))
	{
		portInType=selectedPortType;
		portOutType=selectedPortType2;
		portIn=selectedPort;
		portOut=selectedPort2;
		iconIn =(	ConData* ) condata->GetAt(selectedIcon);
		iconOut=(	ConData* ) condata->GetAt(selectedIcon2);
	}
	else if ((selectedPortType ==OUTPUTPORT && selectedPortType2 == INPUTPORT) ||
		(selectedPortType ==FINISHPORT && selectedPortType2 == STARTPORT))
	{
		portInType=selectedPortType2;
		portOutType=selectedPortType;
		portIn=selectedPort2;
		portOut=selectedPort;
		iconIn =(ConData* ) condata->GetAt(selectedIcon2);
		iconOut=(ConData* ) condata->GetAt(selectedIcon);
	}
	else
	{
		wxMessageBox("This is an illegal connection.");
		depFSM.drawingline = 0;
		return 0;
	}

	if (areBothIconsXPorts(iconIn, iconOut)) {
		wxMessageBox("Illegal connection. Start and end points are Xports.");
		depFSM.drawingline = 0;
		return 0;
	}


	// check data type for connections between input and output ports only
	if ((selectedPortType == INPUTPORT && selectedPortType2 == OUTPUTPORT) ||
		(selectedPortType == OUTPUTPORT && selectedPortType2 == INPUTPORT)) {
		validConnect = iconIn->CheckDatatypes(portIn,portInType,iconOut,portOut,portOutType);
	}
	// call the inout icon method that actually does the connection after error trapping
	if (validConnect != -1) {
		if (!iconIn->AddLine(portIn,portInType,iconOut,portOut,portOutType)) {
			depFSM.drawingline = 0;
			return 0;
		}
		// set status of output/finish port as connected
		iconOut->setOutputPortAsConnected(portOut,portOutType);
		if (portOutType == OUTPUTPORT) {
			iconOut->outputport[portOut]->line.othericonid = iconIn->identnum;
			iconOut->outputport[portOut]->line.otherportno = portIn;
		}
		else if (portOutType == FINISHPORT) {
			iconOut->finishport[portOut]->line.othericonid = iconIn->identnum;
			iconOut->finishport[portOut]->line.otherportno = portIn;
		}
	}
	else {
		depFSM.drawingline = 0;
		return 0;
	}
	depFSM.drawingline = 0;

	return 1;
}

// Adds 2 nodes to make the lines orthogonal
void DEP::AddNodes(INXPOSITION selectedIcon, int selectedPort, int selectedPortType, INXPOSITION selectedIcon2, int selectedPort2, int selectedPortType2) {
	ConData *icondata1, *icondata2;

	if (selectedPortType2 == INPUTPORT || selectedPortType == INPUTPORT || selectedPortType2 == STARTPORT ||
		selectedPortType == STARTPORT) {
		icondata2 = (ConData*) condata->GetAt(selectedIcon2);
		icondata1 = (ConData*) condata->GetAt(selectedIcon);
		icondata1->AddNodes(selectedPort, selectedPortType, icondata2, selectedPort2, selectedPortType2);
	}
}

// Function that adds a port to a block
#endif
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
int DEP::AddPort(ConData* blob, int iDataType, int iPortType, INXString portLabel) {
	INXPoint point;

	point = blob->GetIconPos();
	INXObjArray<INXString>* funcName = new INXObjArray<INXString>;
	INXObjArray<unsigned int>* funcArg = new INXObjArray<unsigned int>;

	if ((blob->inputport_num + blob->startport_num >= (MAXBLOCKPORTS))||(blob->outputport_num + blob->finishport_num >= (MAXBLOCKPORTS)))
	{
		wxMessageBox("No more ports can be added as you have the maximum.");
		return 0;
	}

	//@todo - add functionality to specify if xports are mandatory or not - currently defaults to "mandatoryFlag=0" for port constructor below

	// create instances of the port to add
	if (iPortType == INPUTPORT) {

		point = point + INXPoint(-6,30 + ((blob->startport_num-1)*15 + blob->inputport_num*15));
		blob->inputport[blob->inputport_num] = new Port(point, blob->inputport_num, iDataType, iPortType, portLabel, funcName, funcArg, 1, 0, 1, 0);
		blob->inputport_num++;
	}
	else if (iPortType == OUTPUTPORT) {

		point = point + INXPoint(ENCAPS_ICON_WIDTH,30 + ((blob->finishport_num-1)*15 + blob->outputport_num*15));
		blob->outputport[blob->outputport_num] = new Port(point, blob->outputport_num, iDataType, iPortType, portLabel, funcName, funcArg, 1, 0, 1, 0);
		blob->outputport_num++;
	}
	else if (iPortType == STARTPORT) {

		// add port
		point = point + INXPoint(-6,10 + (15 * blob->startport_num));
		blob->startport[blob->startport_num] = new Port(point, blob->startport_num, -1, iPortType, portLabel, funcName, funcArg, 1, 0, 1, 0);
		// shift input ports
		for (UINT i=0; i<blob->inputport_num; i++) {
			INXRect rect = blob->inputport[i]->rectangle;
			blob->inputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y+15,rect.BottomRight().x,rect.BottomRight().y+15);
			blob->inputport[i]->P.y = blob->inputport[i]->P.y + 15;
		}
		blob->startport_num++;
	}
	else if (iPortType == FINISHPORT) {

		// add port
		point = point + INXPoint(ENCAPS_ICON_WIDTH,10 + (15 * blob->finishport_num ));
		blob->finishport[blob->finishport_num] = new Port(point, blob->finishport_num, -1, iPortType, portLabel, funcName, funcArg, 1, 0, 1, 0);
		// shift output ports
		for (UINT i=0; i<blob->outputport_num; i++) {
			INXRect rect = blob->outputport[i]->rectangle;
			blob->outputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y+15,rect.BottomRight().x,rect.BottomRight().y+15);
			blob->outputport[i]->P.y = blob->outputport[i]->P.y + 15;
		}
		blob->finishport_num++;
	}

	blob->ResizeIcon();

	return 1;
	// Check if port is off the bottom off the icon abd grow if needed
/*
	INXPoint IconBottomRight =  blob->rectangle.BottomRight();

	if(blob->inputport_num>0)
	{	INXPoint InputPortPosition = (blob->inputport[(blob->inputport_num)-1])->rectangle.BottomRight();
		if(InputPortPosition.y > IconBottomRight.y )
		{	blob->ResizeIcon();
		}
	}
	if(blob->outputport_num>0)
	{	INXPoint OutputPortPosition = (blob->outputport[(blob->outputport_num)-1])->rectangle.BottomRight();
		if(OutputPortPosition.y > IconBottomRight.y )
		{	blob->ResizeIcon();
		}
	}
	if(blob->startport_num>0)
	{	INXPoint StartPortPosition = (blob->startport[(blob->startport_num)-1])->rectangle.BottomRight();
		if(StartPortPosition.y > IconBottomRight.y )
		{	blob->ResizeIcon();
		}
	}
	if(blob->finishport_num>0)
	{	INXPoint FinishPortPosition = (blob->finishport[(blob->finishport_num)-1])->rectangle.BottomRight();
		if(FinishPortPosition.y > IconBottomRight.y )
		{	blob->ResizeIcon();
		}
	}

*/

}

// Adds a tag to a port and adds a line if there is another tag with the same name
int DEP::AddTag(INXPOSITION iconPos, int portNum, int portType) {
	ConData *blob, *otherBlob;
	INXPOSITION pos, iconPos2;
	int dataType, ret;
	bool isUniqueOutTag = FALSE;

	blob = (ConData*) condata->GetAt(iconPos);
	// get data type
	if (portType == INPUTPORT) {
		dataType = blob->inputport[portNum]->datatype;
	}
	else if (portType == OUTPUTPORT) {
		dataType = blob->outputport[portNum]->datatype;
	}
	else if (portType == STARTPORT || portType == FINISHPORT) {
		dataType = 4;
	}
	InitTagLists();
	CTagDialog dialog(boolTags, intTags, realTags, stringTags, eventTags, dataType, portType, condata, 0);
	if (portType == INPUTPORT) {
		if (blob->inputport[portNum]->connected) {
			wxMessageBox("Input port is already connected.");
			ret = 0;
		}
		else if (dialog.ShowModal() == IDOK) {
			blob->inputport[portNum]->tag = dialog.tag;
			// Add line if an output has same tag
			pos = condata->GetHeadPosition();
			while (pos) {
				iconPos2 = pos;
				otherBlob = (ConData*) condata->GetNext(pos);
				for (UINT i=0; i<otherBlob->outputport_num; i++) {
					if (dialog.tag == otherBlob->outputport[i]->tag) {
						AddLine(iconPos, portNum, portType, iconPos2, i, OUTPUTPORT);
						AddNodes(iconPos, portNum, portType, iconPos2, i, OUTPUTPORT);
					}
				}
			}
			//AddTag2List(dialog.tag, dataType);
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	else if (portType == STARTPORT) {
		if (blob->startport[portNum]->connected) {
			wxMessageBox("Start port is already connected.");
			ret = 0;
		}
		else if (dialog.ShowModal() == IDOK) {
			blob->startport[portNum]->tag = dialog.tag;
			// Add line if an output has same tag
			pos = condata->GetHeadPosition();
			while (pos) {
				iconPos2 = pos;
				otherBlob = (ConData*) condata->GetNext(pos);
				for (UINT i=0; i<otherBlob->finishport_num; i++) {
					if (dialog.tag == otherBlob->finishport[i]->tag) {
						AddLine(iconPos, portNum, portType, iconPos2, i, FINISHPORT);
						AddNodes(iconPos, portNum, portType, iconPos2, i, FINISHPORT);
					}
				}
			}
			//AddTag2List(dialog.tag, dataType);
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	else if (portType == OUTPUTPORT) {
		if (dialog.ShowModal() == IDOK) {
			blob->outputport[portNum]->tag = dialog.tag;
			// Add lines to all inputs with the same tag
			pos = condata->GetHeadPosition();
			while (pos) {
				iconPos2 = pos;
				otherBlob = (ConData*) condata->GetNext(pos);
				for (UINT i=0; i<otherBlob->inputport_num; i++) {
					if (dialog.tag == otherBlob->inputport[i]->tag) {
						AddLine(iconPos, portNum, portType, iconPos2, i, INPUTPORT);
						AddNodes(iconPos, portNum, portType, iconPos2, i, INPUTPORT);
					}
				}
			}
			//AddTag2List(dialog.tag, dataType);
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	else if (portType == FINISHPORT) {
		if (dialog.ShowModal() == IDOK) {
			blob->finishport[portNum]->tag = dialog.tag;
			// Add lines to all start ports with the same tag
			pos = condata->GetHeadPosition();
			while (pos) {
				iconPos2 = pos;
				otherBlob = (ConData*) condata->GetNext(pos);
				for (UINT i=0; i<otherBlob->startport_num; i++) {
					if (dialog.tag == otherBlob->startport[i]->tag) {
						AddLine(iconPos, portNum, portType, iconPos2, i, STARTPORT);
						AddNodes(iconPos, portNum, portType, iconPos2, i, STARTPORT);
					}
				}
			}
			//AddTag2List(dialog.tag, dataType);
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	else {
		ret = 0;
	}

	return ret;
}
#endif

void DEP::AddTag2List(INXString tag, int dataType) {
	bool exists = FALSE;

	switch (dataType) {
	case 0 :
		{
			for (int i=0; i<boolTags->GetSize(); i++) {
				if (tag == boolTags->GetAt(i)) exists = TRUE;
			}
			if (!exists) boolTags->Add(tag);
			break;
		}
	case 1 :
		{
			for (int i=0; i<intTags->GetSize(); i++) {
				if (tag == intTags->GetAt(i)) exists = TRUE;
			}
			if (!exists) intTags->Add(tag);
			break;
		}
	case 2 :
		{
			for (int i=0; i<realTags->GetSize(); i++) {
				if (tag == realTags->GetAt(i)) exists = TRUE;
			}
			if (!exists) realTags->Add(tag);
			break;
		}
	case 3 :
		{
			for (int i=0; i<stringTags->GetSize(); i++) {
				if (tag == stringTags->GetAt(i)) exists = TRUE;
			}
			if (!exists) stringTags->Add(tag);
			break;
		}
	case 4 :
		{
			for (int i=0; i<eventTags->GetSize(); i++) {
				if (tag == eventTags->GetAt(i)) exists = TRUE;
			}
			if (!exists) eventTags->Add(tag);
			break;
		}
	}
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Function that adds a xport
ConData* DEP::AddXPort(INXString type, INXString portLabel, INXPoint point) {
	ConData* xport;
	INXPOSITION pos;

	pos = AddIcon(type, "", point);
	xport = (ConData*) condata->GetAt(pos);
	xport->description = portLabel;
	return xport;
}
#endif
/*
This updates the the line objects to point at the correct endpoint data objects,
which are actually the otherend's icon port point data attributes.
This makes moving objets and lines together easier - I think!
This function is necessary to call after a programme is loaded from file.
because line endpoints need to point to memeory locations,that are only defined when the icons are created.
Confused?? have a look at the endpoint stuff in class IconLines.
  */

void DEP::AssignIconLineEndPoints() {
	ConData* blob, *blob2;
	INXPOSITION pos;
	UINT i;

	pos = condata->GetHeadPosition();
	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		for (i=0;i<blob->inputport_num;i++) {
			// only add line if its not already connected
			if (blob->inputport[i]->line.exist && !blob->inputport[i]->connected) {
				blob2=GetIconFromID(blob->inputport[i]->line.othericonid);
				if (blob2) {
					blob->AddLine(i, INPUTPORT, blob2, blob->inputport[i]->line.otherportno, OUTPUTPORT);
					// set status of output/finish port as connected
	//				blob2->setOutputPortAsConnected(blob->inputport[i]->line.otherportno,OUTPUTPORT);
				}
			}
		}
		for (i=0;i<blob->startport_num;i++) {

			// for consistency and is required to check if a mandatory port has been connected up
			if (blob->startport[i]->initialise) {
				blob->startport[i]->connected = 1;
			}

			if (blob->startport[i]->line.exist && !blob->startport[i]->connected) {
				blob2=GetIconFromID(blob->startport[i]->line.othericonid);
				if (blob2) {
					blob->AddLine(i, STARTPORT, blob2, blob->startport[i]->line.otherportno, FINISHPORT);
					// set status of output/finish port as connected
	//				blob2->setOutputPortAsConnected(blob->startport[i]->line.otherportno,FINISHPORT);
				}
			}
		}
	}
}

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Function that checks the newID for the encapsulated icon does not already exist within the
// encapsulated block. If it does it increments newID until it is unique.
long int DEP::CheckNewID(INXObjList* encapsulated, long int id) {
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

// Function that connects up the encapsulated icon when using subset encapsulation
void DEP::ConnectEncapsulatedIcon(INXPOSITION encapsulatedPos, INXRect encapsulate, INXObjList* oldDEP)
{
	UINT i;
	INXPOSITION pos, rmPos, otherPos;
	ConData* blob;
	ConData* otherBlob;
	ConData* encapsulatedIcon;
	int startNum = 0, inNum = 0, finishNum = 0, outNum = 0;
	bool incPortNum = FALSE;
	BlockOperations bo;
	int otherPortNum;

	// pointer to the encapsulated icon
	encapsulatedIcon = (ConData*) condata->GetAt(encapsulatedPos);

	// check every icon to see if its centre is within the encapsulation box
	pos = condata->GetHeadPosition();
	while(pos) {
		rmPos = pos;
		blob = (ConData *) (condata->GetNext(pos));
		if (blob != encapsulatedIcon) {
		if (encapsulate.PtInRect(blob->GetIconCentre())) {
			//if (blob->getEncapsulated()) {
			// for each port check if the connected icon is in the encapsulation box
			// if its outside then the port must be connected to an xport so connect
			// encapsulated icon port to other port.
			for (i=0; i<blob->startport_num; i++) {
				if (blob->startport[i]->line.exist) {
					otherBlob = bo.GetBlockIconFromID(blob->startport[i]->line.othericonid, oldDEP);
					otherPortNum = blob->startport[i]->line.otherportno;
					if (!otherBlob->getEncapsulated() && !otherBlob->finishport[otherPortNum]->xportConnected) {
						otherBlob->finishport[otherPortNum]->xportConnected = 1;
						encapsulatedIcon->startport[startNum]->line.exist = 1;
						encapsulatedIcon->startport[startNum]->line.othericonid = blob->startport[i]->line.othericonid;
						encapsulatedIcon->startport[startNum]->line.otherportno = blob->startport[i]->line.otherportno;
						encapsulatedIcon->AddLine(startNum, STARTPORT, otherBlob, blob->startport[i]->line.otherportno, FINISHPORT);
						startNum++;
					}
				}
			}
			for (i=0; i<blob->inputport_num; i++) {
				if (blob->inputport[i]->line.exist) {
					otherBlob = bo.GetBlockIconFromID(blob->inputport[i]->line.othericonid, oldDEP);
					otherPortNum = blob->inputport[i]->line.otherportno;
					if (!otherBlob->getEncapsulated() && !otherBlob->outputport[otherPortNum]->xportConnected) {
						otherBlob->outputport[otherPortNum]->xportConnected = 1;
						encapsulatedIcon->inputport[inNum]->line.exist = 1;
						encapsulatedIcon->inputport[inNum]->line.othericonid = blob->inputport[i]->line.othericonid;
						encapsulatedIcon->inputport[inNum]->line.otherportno = blob->inputport[i]->line.otherportno;
						encapsulatedIcon->AddLine(inNum, INPUTPORT, otherBlob, blob->inputport[i]->line.otherportno, OUTPUTPORT);
						inNum++;
					}
				}
			}
			for (i=0; i<blob->finishport_num; i++) {
				incPortNum = FALSE;
				otherPos = condata->GetHeadPosition();
				while (otherPos) {
					otherBlob = (ConData*) (condata->GetNext(otherPos));
					for (UINT j=0; j<otherBlob->startport_num; j++) {
						if (otherBlob->startport[j]->line.exist &&
							(otherBlob->startport[j]->line.othericonid == blob->identnum) &&
							((UINT)otherBlob->startport[j]->line.otherportno == i)) {
							//if (!otherBlob->getEncapsulated()) {
							if (!encapsulate.PtInRect(otherBlob->GetIconCentre())) {
								otherBlob->startport[j]->line.othericonid = encapsulatedIcon->identnum;
								otherBlob->startport[j]->line.otherportno = finishNum;
								// First delete old line before connecting new line
								otherBlob->DeleteLine(j, STARTPORT);
								otherBlob->AddLine(j, STARTPORT, encapsulatedIcon, finishNum, FINISHPORT);
								incPortNum = TRUE;
								//finishNum++;
							}
						}
					}
				}
				// The port number is incremented once all the start ports have been connected to
				// the finish port on the encapsulated icon, but only if the finish port was connected up.
				if (incPortNum) {
					finishNum++;
				}
			}
			for (i=0; i<blob->outputport_num; i++) {
				incPortNum = FALSE;
				otherPos = condata->GetHeadPosition();
				while (otherPos) {
					otherBlob = (ConData*) (condata->GetNext(otherPos));
					for (UINT j=0; j<otherBlob->inputport_num; j++) {
						if (otherBlob->inputport[j]->line.exist &&
							(otherBlob->inputport[j]->line.othericonid == blob->identnum) &&
							((UINT)otherBlob->inputport[j]->line.otherportno == i)) {
							//if (!otherBlob->getEncapsulated()) {
							if (!encapsulate.PtInRect(otherBlob->GetIconCentre())) {
								otherBlob->inputport[j]->line.othericonid = encapsulatedIcon->identnum;
								otherBlob->inputport[j]->line.otherportno = outNum;
								// First delete old line before connecting new line
								otherBlob->DeleteLine(j, INPUTPORT);
								otherBlob->AddLine(j, INPUTPORT, encapsulatedIcon, outNum, OUTPUTPORT);
								incPortNum = TRUE;
								//outNum++;
							}
						}
					}
				}
				// The port number is incremented once all the input ports have been connected to
				// the output port on the encapsulated icon, but only if the output port was connected up.
				if (incPortNum) {
					outNum++;
				}
			}
			delete (ConData*) condata->GetAt(rmPos);
			condata->RemoveAt(rmPos);
		}
		}
	}
	// *** for some reasonwhen adding lines to the input and start ports of the encapsulated block the other
	// end does not seem to be connected to the other icon. Because moving the other icon does not move the
	// lines with it. However, saving the DEP and reloading it leads to expected behaviour.
	// disconnect all the input and start lines to the encapsulated icon
	for (i=0; i<encapsulatedIcon->startport_num; i++) {
		if (encapsulatedIcon->startport[i]->line.exist) {
			encapsulatedIcon->startport[i]->DisconnectLine();
		}
	}
	for (i=0; i<encapsulatedIcon->inputport_num; i++) {
		if (encapsulatedIcon->inputport[i]->line.exist) {
			encapsulatedIcon->inputport[i]->DisconnectLine();
		}
	}
	//reconnect all the input and start lines to the encapsulated icon
	// can use AssignIconLineEndPoints now as it checks if a line is already connected
	for (i=0; i<encapsulatedIcon->startport_num; i++) {
		if (encapsulatedIcon->startport[i]->line.exist) {
			otherBlob = GetIconFromID(encapsulatedIcon->startport[i]->line.othericonid);
			if (otherBlob) {
				encapsulatedIcon->AddLine(i, STARTPORT, otherBlob, encapsulatedIcon->startport[i]->line.otherportno, FINISHPORT);
			}
		}
	}
	for (i=0; i<encapsulatedIcon->inputport_num; i++) {
		if (encapsulatedIcon->inputport[i]->line.exist) {
			otherBlob = GetIconFromID(encapsulatedIcon->inputport[i]->line.othericonid);
			if (otherBlob) {
				encapsulatedIcon->AddLine(i, INPUTPORT, otherBlob, encapsulatedIcon->inputport[i]->line.otherportno, OUTPUTPORT);
			}
		}
	}
	ResetXportConnected();
}

// creates an instance of a user defined block
void DEP::CreateInstance(ConData* userDefBlob, int lib) {
	INXString className, instanceName, depPath, origName;
	INXPOSITION pos;
	ConData* blob;
	int instNum = 0;
	char szInstNum[8];
	CMainFrame* pFrame;

	origName = userDefBlob->description;
	// determine an instance number
	className = userDefBlob->className;
	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		if (blob->className == userDefBlob->className && blob->instNum > instNum) {
			instNum = blob->instNum;
		}
	}
	instNum++;

	// create unique instance name.
	_itoa_s(instNum, szInstNum, 10);
	instanceName = className + szInstNum;
	userDefBlob->instNum = instNum;
	userDefBlob->description = instanceName;
	pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//userDefBlob->hierarchyName = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(this->hItem) + depFilename + "\\";
	depPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hItem) + depFilename + "\\";

	// copy library component to correct DEP dir
	CFileOperation fo;
	fo.SetOverwriteMode(true); // set OverwriteMode flag
	if (lib == 3) {
	}
	// paste sub-block
	else if (lib == 2) {
		if (origName != instanceName) {
			fo.Delete(workDir + TEMPDIR + instanceName + ".prg");
			fo.Delete(workDir + TEMPDIR + instanceName);
		}
		//copy file
		fo.Rename(workDir + TEMPDIR + origName + ".prg", workDir + TEMPDIR + instanceName + ".prg");
		fo.Copy(workDir + TEMPDIR + instanceName + ".prg", projectDir + DEPDIR + depPath);
		fo.Rename(workDir + TEMPDIR + instanceName + ".prg", workDir + TEMPDIR + origName + ".prg");
		//copy dir
		fo.Rename(workDir + TEMPDIR + origName, workDir + TEMPDIR + instanceName);
		fo.Copy(workDir + TEMPDIR + instanceName, projectDir + DEPDIR + depPath);
		fo.Rename(workDir + TEMPDIR + instanceName, workDir + TEMPDIR + origName);
	}
	else if ((lib == 1) || (lib == 0)) {
		ifstream infile((INXString)workDir + USERDEFDIR + className + ".prg");
		ofstream outfile(projectDir + DEPDIR + depPath + instanceName + ".prg");
		outfile << infile.rdbuf();
		infile.close();
		outfile.close();
		// This tries to copy new blocks, but they don't exist so gives error
		if (!fo.Copy(workDir + USERDEFDIR + className, projectDir + DEPDIR + depPath)) {
			//fo.ShowError(); // if copy fails show error message
		}
		// if directory already exists delete it so can do rename
		fo.Delete(projectDir + DEPDIR + depPath + "\\" + instanceName);
		fo.Rename(projectDir + DEPDIR + depPath + "\\" + className, projectDir + DEPDIR + depPath + "\\" + instanceName);
		// if not library component then delete from userdefdir
		if (lib==0) {
			fo.Delete(workDir + USERDEFDIR + className + ".idf.ini");
			fo.Delete(workDir + USERDEFDIR + className + ".prg");
			fo.Delete(workDir + USERDEFDIR + className);
		}
	}
	// lib = 4 is for the case when a new user defined FB is created
	else if (lib == 4) {
		ifstream infile((INXString)workDir + USERDEFDIR + "NewComponent.prg");
		ofstream outfile(projectDir + DEPDIR + depPath + instanceName + ".prg");
		outfile << infile.rdbuf();
		infile.close();
		outfile.close();
		CreateDirectory(projectDir + DEPDIR + depPath + "\\" + instanceName, NULL);
		fo.Delete(workDir + USERDEFDIR + "NewComponent.prg");
	}
}

/*
This function must use the CObject List method to locate the object and delete it (deallocate memort etc)
It should first check all the other icon's input connections (othericon attribute) to see if theconnection the ouputs and see
Only delete output lines if bDelOutputs is TRUE
*/
void DEP::DeleteIcon(INXPOSITION number, bool bDelOutputs) { //delete actual blob using this
	ConData *blob, *blob2;
	INXPOSITION pos, selectedControl;
	long int othericonid = -1;
	bool userDefined = FALSE;
	//char szInstNum[8];
	UINT i;

	blob = (ConData *) condata->GetAt(number); // save the old pointer for deletion
	if (bDelOutputs) {
	// remove lines connected to outputs
	for (i=0; i<blob->outputport_num; i++) {
		pos = condata->GetHeadPosition();
		while (pos) {
			selectedControl = pos;
			blob2 = (ConData *) (condata->GetNext(pos));
			for (UINT j=0; j<blob2->inputport_num; j++) {
				othericonid = blob2->inputport[j]->line.othericonid;
				if (othericonid == blob->identnum) {
					blob2->inputport[j]->tag = "";
					DeleteLine(selectedControl, j, INPUTPORT);
				}
			}
		}
	}

	// remove lines connected to trigger outputs
	for (i=0; i<blob->finishport_num; i++) {
		pos = condata->GetHeadPosition();
		while (pos) {
			selectedControl = pos;
			blob2 = (ConData*) condata->GetNext(pos);
			for (UINT j=0; j<blob2->startport_num; j++) {
				if (blob2->startport[j]->line.othericonid == blob->identnum) {
					blob2->startport[j]->tag = "";
					DeleteLine(selectedControl, j, STARTPORT);
				}
			}
		}
	}


	}

	condata->RemoveAt(number);

	delete blob; // deletion avoids memory leak
}

/*
	Remove a line
*/
void DEP::DeleteLine(INXPOSITION selectedControl,int PortSelected, int portType) {
	ConData* selectedIcon;

	selectedIcon= (ConData*) condata->GetAt(selectedControl);
	selectedIcon->DeleteLine(PortSelected, portType);
}

INXString DEP::DeletePort(ConData* blob, int portNum, int portType) {
	INXRect rect;
	INXString portLabel = "";
	INXPOSITION pos;
	ConData* otherBlob;
	UINT i;

	switch (portType) {
	case 0:
		{
			portLabel = blob->inputport[portNum]->description;
			delete blob->inputport[portNum];
			blob->inputport_num--;
			// shift remaining input ports
			for (i=portNum; i<blob->inputport_num; i++) {
				blob->inputport[i] = blob->inputport[i+1];
				rect = blob->inputport[i]->rectangle;
				blob->inputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->inputport[i]->P.y = blob->inputport[i]->P.y - 15;
			}

			blob->ResizeIcon();

			break;
		}
	case 1:
		{
			portLabel = blob->outputport[portNum]->description;
			delete blob->outputport[portNum];
			blob->outputport_num--;
			// delete all lines connected to output port
			pos = condata->GetHeadPosition();
			while (pos) {
				otherBlob = (ConData*) condata->GetNext(pos);
				for (i=0; i<otherBlob->inputport_num; i++) {
					if (blob->identnum == otherBlob->inputport[i]->line.othericonid && otherBlob->inputport[i]->line.otherportno == portNum) {
						otherBlob->DeleteLine(i, INPUTPORT);
					}
				}
			}
			// shift remaining output ports
			for (i=portNum; i<blob->outputport_num; i++) {
				blob->outputport[i] = blob->outputport[i+1];

				rect = blob->outputport[i]->rectangle;
				blob->outputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->outputport[i]->P.y = blob->outputport[i]->P.y - 15;
				// change otherportno in lines connected to shifted output ports
				pos = condata->GetHeadPosition();
				while (pos) {
					otherBlob = (ConData*) condata->GetNext(pos);
					for (UINT j=0; j<otherBlob->inputport_num; j++) {
						if (blob->identnum == otherBlob->inputport[j]->line.othericonid && otherBlob->inputport[j]->line.otherportno == (int)i+1) {
							otherBlob->inputport[j]->line.otherportno = i;
						}
					}
				}
			}

			blob->ResizeIcon();
			break;
		}
	case 2:
		{
			portLabel = blob->startport[portNum]->description;
			delete blob->startport[portNum];
			blob->startport_num--;
			// shift remaining start ports
			for (i=portNum; i<blob->startport_num; i++) {
				blob->startport[i] = blob->startport[i+1];
				rect = blob->startport[i]->rectangle;
				blob->startport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->startport[i]->P.y = blob->startport[i]->P.y - 15;
			}
			// shift all input ports
			for (i=0; i<blob->inputport_num; i++) {
				rect = blob->inputport[i]->rectangle;
				blob->inputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->inputport[i]->P.y = blob->inputport[i]->P.y - 15;
			}
			blob->ResizeIcon();
			break;
		}
	case 3:
		{
			portLabel = blob->finishport[portNum]->description;
			delete blob->finishport[portNum];
			blob->finishport_num--;
			// delete all lines connected to finish port
			pos = condata->GetHeadPosition();
			while (pos) {
				otherBlob = (ConData*) condata->GetNext(pos);
				for (i=0; i<otherBlob->startport_num; i++) {
					if (blob->identnum == otherBlob->startport[i]->line.othericonid && otherBlob->startport[i]->line.otherportno == portNum) {
						otherBlob->DeleteLine(i, STARTPORT);
					}
				}
			}
			// shift remaining finish ports
			for (i=portNum; i<blob->finishport_num; i++) {
				blob->finishport[i] = blob->finishport[i+1];
				rect = blob->finishport[i]->rectangle;
				blob->finishport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->finishport[i]->P.y = blob->finishport[i]->P.y - 15;
				// change otherportno in lines connected to shifted finish ports
				pos = condata->GetHeadPosition();
				while (pos) {
					otherBlob = (ConData*) condata->GetNext(pos);
					for (UINT j=0; j<otherBlob->startport_num; j++) {
						if (blob->identnum == otherBlob->startport[j]->line.othericonid && otherBlob->startport[j]->line.otherportno == (int)i+1) {
							otherBlob->startport[j]->line.otherportno = i;
						}
					}
				}
			}
			// shift all output ports
			for (i=0; i<blob->outputport_num; i++) {
				rect = blob->outputport[i]->rectangle;
				blob->outputport[i]->rectangle=INXRect(rect.TopLeft().x,rect.TopLeft().y-15,rect.BottomRight().x,rect.BottomRight().y-15);
				blob->outputport[i]->P.y = blob->outputport[i]->P.y - 15;
			}
			blob->ResizeIcon();
			break;
		}
	default:
		wxMessageBox("Data type doesn't exist"); break;
	}
	return portLabel;
}

// deletes a tag and the associated line
void DEP::DeleteTag(INXPOSITION iconPos, int portNum, int portType) {
	ConData *blob, *otherBlob;
	INXPOSITION pos;

	blob = (ConData*) condata->GetAt(iconPos);
	//remove tag and delete line
	if (portType == INPUTPORT && blob->inputport[portNum]->tag != "") {
		blob->inputport[portNum]->tag = "";
		blob->DeleteLine(portNum, portType);
	}
	else if (portType == STARTPORT && blob->startport[portNum]->tag != "") {
		blob->startport[portNum]->tag = "";
		blob->DeleteLine(portNum, portType);
	}
	// remove all tags and delete all tagged lines connected to output port
	else if (portType == OUTPUTPORT && blob->outputport[portNum]->tag != "") {
		pos = condata->GetHeadPosition();
		while (pos) {
			otherBlob = (ConData*) condata->GetNext(pos);
			for (UINT i=0; i<otherBlob->inputport_num; i++) {
				if (blob->identnum == otherBlob->inputport[i]->line.othericonid && otherBlob->inputport[i]->tag ==
					blob->outputport[portNum]->tag && portNum == otherBlob->inputport[i]->line.otherportno) {
					otherBlob->inputport[i]->tag = "";
					otherBlob->DeleteLine(i, INPUTPORT);
				}
			}
		}
		//RemoveTag(blob->outputport[portNum]->tag, blob->outputport[portNum]->datatype);
		blob->outputport[portNum]->tag = "";
	}
	else if (portType == FINISHPORT && blob->finishport[portNum]->tag != "") {
		pos = condata->GetHeadPosition();
		while (pos) {
			otherBlob = (ConData*) condata->GetNext(pos);
			for (UINT i=0; i<otherBlob->startport_num; i++) {
				if (blob->identnum == otherBlob->startport[i]->line.othericonid && otherBlob->startport[i]->tag ==
					blob->finishport[portNum]->tag && portNum == otherBlob->startport[i]->line.otherportno) {
					otherBlob->startport[i]->tag = "";
					otherBlob->DeleteLine(i, STARTPORT);
				}
			}
		}
		//RemoveTag(blob->finishport[portNum]->tag, blob->finishport[portNum]->datatype);
		blob->finishport[portNum]->tag = "";
	}
}

// Draw the DEP
#endif

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void DEP::Draw(CDC * pDC) {
	Draw(pDC, false, 0); 
}

// Draw the DEP
void DEP::Draw(CDC * pDC, bool _onlyDrawAnim, int _toggleAnim) {
	INXPOSITION pos;
	ConData* icondata;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);

	if (!condata->IsEmpty()) {
		for (pos = condata->GetHeadPosition();pos !=NULL;) {
			icondata=((ConData *) condata->GetNext(pos));
			if (!pDC->IsPrinting()) {
				icondata->Draw(pDC, _onlyDrawAnim, _toggleAnim);
				//if (icondata->showdescript) icondata->DrawDescription(pDC);
			}
			// change the mapping mode to scale the printed output
			else {
				pDC->SetMapMode(MM_LOENGLISH);
				icondata->MinusYCoords();
				icondata->Draw(pDC, _onlyDrawAnim, _toggleAnim);
				icondata->MinusYCoords();
				pDC->SetMapMode(MM_TEXT);
			}
		}
		if (_onlyDrawAnim) {
			return;
		}

		// Once all the icons and ports have been drawn, draw the lines so that they are on top of icons
		if (pDC->IsPrinting()) {
			pDC->SetMapMode(MM_LOENGLISH);
		}
		UINT i;
		for (pos = condata->GetHeadPosition();pos !=NULL;) {
			icondata=((ConData *) condata->GetNext(pos));
			for (i=0; i<icondata->inputport_num; i++) {
				if (icondata->inputport[i]->tag == "") {
					icondata->inputport[i]->line.setDefineMonitor(pProject->getDefineMonitors());
					icondata->inputport[i]->line.Draw(pDC);
				}
			}
			for (i=0; i<icondata->startport_num; i++) {
				if (icondata->startport[i]->tag == "") {
					icondata->startport[i]->line.setDefineMonitor(pProject->getDefineMonitors());
					icondata->startport[i]->line.Draw(pDC);
				}
			}
		}
		if (pDC->IsPrinting()) {
			pDC->SetMapMode(MM_TEXT);
		}
	}
}
#endif

void DEP::DrawGL(){
	DrawGL(false,0);
}
void DEP::DrawGL(bool _onlyDrawAnim, int _toggleAnim){


	INXPOSITION pos;
	ConData* icondata;
	/* @todo ORIGINAL
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	*/
	//@todo
	//MainFrame* pFrame = NULL;
#ifdef TRY_EXCLUDE_PROJECT
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
#endif
	if (!condata->IsEmpty()) {
		for (pos = condata->GetHeadPosition();pos !=NULL;) {
			icondata=((ConData *) condata->GetNext(pos));

				icondata->DrawGL(_onlyDrawAnim, _toggleAnim);
		}
		if (_onlyDrawAnim) {
			return;
		}

		// Once all the icons and ports have been drawn, draw the lines so that they are on top of icons
		
		UINT i;
		for (pos = condata->GetHeadPosition();pos !=NULL;) {
			icondata=((ConData *) condata->GetNext(pos));
			for (i=0; i<icondata->inputport_num; i++) {
				if (icondata->inputport[i]->tag == wxT("")) {
				#ifdef TRY_EXCLUDE_PROJECT
					icondata->inputport[i]->line.setDefineMonitor(pProject->getDefineMonitors());
				#endif
					icondata->inputport[i]->line.DrawGL();
				}
			}
			for (i=0; i<icondata->startport_num; i++) {
				if (icondata->startport[i]->tag == wxT("")) {
				#ifdef TRY_EXCLUDE_PROJECT
					icondata->startport[i]->line.setDefineMonitor(pProject->getDefineMonitors());
				#endif
					icondata->startport[i]->line.DrawGL();
				}
			}
		}
	}
}

/*
This is called to bring up a parameters dialog box, if the user clicks on an icon
This function should call a parameter block that reads information from the specific icon
description file to generate a data types specific parameter input dialog box,
with descriptions and inout valiadation.
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
INT_PTR DEP::EditControl(INXPOSITION selected) {

	ConData *blob;
	INXString csOldInstName, csOldFuncName;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
	HTREEITEM hUserdefItem;

	blob = (ConData *) condata->GetAt(selected);
	IconParams dialog(blob, pProject);

	//set the parameters in the dialog box to those in the icon
	// Could do this in OnInitDialog function
	for (UINT i=1; i<blob->iParamNum; i++) {
		dialog.labels[i] = blob->iconParam[i]->name;
		dialog.values[i] = blob->iconParam[i]->value;
	}

	if (blob->m_iUserDefined) {
		csOldInstName = blob->description;
		csOldFuncName = blob->className;
		hUserdefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
	}

	// display dialog box
	INT_PTR returnable = dialog.DoModal();
	if ( returnable==IDOK ) {
		// Rename subsystem if the name has changed
		if (blob->m_iUserDefined && blob->className != csOldFuncName) {
			RenameSubsystem(csOldInstName, blob->description, hUserdefItem);
		}
		// update widget groups in project explorer incase any were updated by the user
		pFrame->m_wndProjectBar.m_cProjTree.updateWidgetGroups(hItem);

		if (blob->isGuiWidget()) {
			pProject->pProjMData->setDirtyWidgetsFlag(); 
		}
	}
	return returnable;
}

int DEP::GetFinishLineID(INXObjList* flattened, ConData* otherBlockIcon, ConData* flatBlob, int endNum, HTREEITEM hItem) {
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString blockFile;
	INXObjList* encapsulated;
	INXPOSITION blockPos, otherFlatPos;
	ConData *blockIcon, *otherFlatIcon;
	HTREEITEM childItem;
	BlockOperations bo;

	if (otherBlockIcon->m_iUserDefined) {
		// load block
		childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(otherBlockIcon, hItem);
		blockFile = projectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem) + "\\" +
			otherBlockIcon->description + ".prg";
		encapsulated = bo.LoadBlock(blockFile);
		blockPos = encapsulated->GetHeadPosition();
		while (blockPos) {
			blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
			if (blockIcon->m_csIconType == "XFINISH" && blockIcon->description == otherBlockIcon->finishport[endNum]->description) {
				otherBlockIcon = bo.GetBlockIconFromID(blockIcon->startport[0]->line.othericonid, encapsulated);
				endNum = blockIcon->startport[0]->line.otherportno;
			}
		}
	}

	if (otherBlockIcon->m_iUserDefined) {
		GetFinishLineID(flattened, otherBlockIcon, flatBlob, endNum, childItem);
	}
//	else if (otherBlockIcon->identnum == flatBlob->blockID) {
	else if (otherBlockIcon->identnum == flatBlob->identnum) {
		otherFlatPos = flattened->GetHeadPosition();
		while (otherFlatPos) {
			otherFlatIcon = (ConData *) (flattened->GetNext(otherFlatPos));
			for (UINT i=0; i<otherFlatIcon->startport_num; i++) {
				if (otherFlatIcon->startport[i]->line.othericonid == flatBlob->identnum) {
					return otherFlatIcon->startport[i]->line.idNum;
				}
			}
		}
	}
	return 0;
}


#endif
/*
Searches all icons and return an object pointer for a ConObject with matching identnum.
  */
ConData* DEP::GetIconFromID(long int id) {
	ConData* blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();

	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		if (blob->identnum==id) return blob;
	}
	return NULL;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// This function searches through all hierarchical levels of encapsulation to find the line ID
int DEP::GetInLineID(ConData* blob, ConData* flatBlob, int inNum) {
	char szInstNum[8];
	INXString instanceName, blockFile;
	INXObjList* encapsulated;
	ConData *blockIcon;
	ConData *otherBlockIcon;
	INXPOSITION blockPos, otherBlockPos;
	BlockOperations bo;

	// load block
	_itoa_s(blob->instNum, szInstNum, 10);
	if (!blob->instNum) {
		instanceName = blob->className;
	}
	else {
		instanceName = blob->className + "_" + blob->hierarchyName + szInstNum;
	}
	blockFile = projectDir + DEPDIR + instanceName + ".prg";
	encapsulated = bo.LoadBlock(blockFile);
	// check if the input port on the encapsulated icon matches the XINPUT in
	// the encapsulated block
	blockPos = encapsulated->GetHeadPosition();
	while (blockPos) {
		blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
		if (blockIcon->m_csIconType.Find("XINPUT") > -1 && blockIcon->description == blob->inputport[inNum]->description) {
			otherBlockPos = encapsulated->GetHeadPosition();
			while (otherBlockPos) {
				otherBlockIcon = (ConData *) (encapsulated->GetNext(otherBlockPos));
				for (UINT j=0; j<otherBlockIcon->inputport_num; j++) {
					if (otherBlockIcon->inputport[j]->line.exist &&
						otherBlockIcon->inputport[j]->line.othericonid == blockIcon->identnum) {
						if (otherBlockIcon->m_iUserDefined) {
							return GetInLineID(otherBlockIcon, flatBlob, j);
						}
						//else if (otherBlockIcon->identnum == flatBlob->blockID) {
						else if (otherBlockIcon->identnum == flatBlob->identnum) {
							return flatBlob->inputport[j]->line.idNum;
						}
					}
				}
			}
		}
	}
	return 0;
}

// Function that returns a pointer to a list of lines specified by its data type and id
INXObjList* DEP::GetLine(long lineID, int dataType) {
	ConData *blob;
	INXPOSITION pos;
	INXObjList *linesList = new INXObjList;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		// events
		if (dataType == 4) {
			for (UINT i=0; i<blob->startport_num; i++) {
				if (blob->startport[i]->line.idNum == lineID) {
					//return &blob->startport[i]->line;
					linesList->AddTail(&blob->startport[i]->line);
				}
			}
		}
		// data
		else {
			for (UINT i=0; i<blob->inputport_num; i++) {
				if (blob->inputport[i]->datatype == dataType && blob->inputport[i]->line.idNum == lineID) {
					//return &blob->inputport[i]->line;
					linesList->AddTail(&blob->inputport[i]->line);
				}
			}
		}
	}

	//return NULL;
	return linesList;
}

int DEP::GetOutLineID(INXObjList* flattened, ConData* otherBlockIcon, ConData* flatBlob, int outNum, HTREEITEM hItem) {
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString blockFile;
	INXObjList* encapsulated;
	INXPOSITION blockPos, otherFlatPos;
	ConData *blockIcon, *otherFlatIcon;
	HTREEITEM childItem;
	BlockOperations bo;

	if (otherBlockIcon->m_iUserDefined) {
		// load block
		childItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(otherBlockIcon, hItem);
		blockFile = projectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(childItem) + "\\" +
			otherBlockIcon->description + ".prg";
		encapsulated = bo.LoadBlock(blockFile);
		blockPos = encapsulated->GetHeadPosition();
		while (blockPos) {
			blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
			if (blockIcon->m_csIconType.Find("XOUTPUT") > -1 && blockIcon->description == otherBlockIcon->outputport[outNum]->description) {
				otherBlockIcon = bo.GetBlockIconFromID(blockIcon->inputport[0]->line.othericonid, encapsulated);
				outNum = blockIcon->inputport[0]->line.otherportno;
			}
		}
	}

	if (otherBlockIcon->m_iUserDefined) {
		GetOutLineID(flattened, otherBlockIcon, flatBlob, outNum, childItem);
	}
	//else if (otherBlockIcon->identnum == flatBlob->blockID) {
	else if (otherBlockIcon->identnum == flatBlob->identnum) {
		otherFlatPos = flattened->GetHeadPosition();
		while (otherFlatPos) {
			otherFlatIcon = (ConData *) (flattened->GetNext(otherFlatPos));
			for (UINT i=0; i<otherFlatIcon->inputport_num; i++) {
				if (otherFlatIcon->inputport[i]->line.othericonid == flatBlob->identnum) {
					return otherFlatIcon->inputport[i]->line.idNum;
				}
			}
		}
	}
	return 0;
}

// This function searches through all hierarchical levels of encapsulation to find the line ID
int DEP::GetStartLineID(ConData* blob, ConData* flatBlob, int startNum) {
	char szInstNum[8];
	INXString instanceName, blockFile;
	INXObjList* encapsulated;
	ConData *blockIcon;
	ConData *otherBlockIcon;
	INXPOSITION blockPos, otherBlockPos;
	BlockOperations bo;

	// load block
	_itoa_s(blob->instNum, szInstNum, 10);
	if (!blob->instNum) {
		instanceName = blob->className;
	}
	else {
		instanceName = blob->className + "_" + blob->hierarchyName + szInstNum;
	}
	blockFile = projectDir + DEPDIR + instanceName + ".prg";
	encapsulated = bo.LoadBlock(blockFile);
	// check if the start port on the encapsulated icon matches the XSTART in
	// the encapsulated block
	blockPos = encapsulated->GetHeadPosition();
	while (blockPos) {
		blockIcon = (ConData *) (encapsulated->GetNext(blockPos));
		if (blockIcon->m_csIconType == "XSTART" && blockIcon->description == blob->startport[startNum]->description) {
			otherBlockPos = encapsulated->GetHeadPosition();
			while (otherBlockPos) {
				otherBlockIcon = (ConData *) (encapsulated->GetNext(otherBlockPos));
				for (UINT j=0; j<otherBlockIcon->startport_num; j++) {
					if (otherBlockIcon->startport[j]->line.exist &&
						otherBlockIcon->startport[j]->line.othericonid == blockIcon->identnum) {
						if (otherBlockIcon->m_iUserDefined) {
							return GetStartLineID(otherBlockIcon, flatBlob, j);
						}
						//else if (otherBlockIcon->identnum == flatBlob->blockID) {
						else if (otherBlockIcon->identnum == flatBlob->identnum) {
							return flatBlob->startport[j]->line.idNum;
						}
					}
				}
			}
		}
	}
	return 0;
}

// highlight all icons in selected box
#endif
int DEP::HighlightSelected(INXRect selectRect) {
	ConData *icondata;
	INXPOSITION pos;
	int count = 0;

	pos = condata->GetHeadPosition();
	while (pos) {
		icondata = (ConData *) (condata->GetNext(pos));
		if (selectRect.PtInRect(icondata->GetIconCentre())) {
			icondata->selected = 1;
			count++;
		}
	}
	return count;
}

/////////////////////////////////////////////////////////////////////////////
// Test all the icons if a mouse click is on an icon
INXPOSITION DEP::Incontrol(INXPoint point) {

	ConData *icondata;
	INXPOSITION pos,retpos;
	for (pos = condata->GetHeadPosition();pos !=NULL;) {
		retpos=pos;
		icondata=((ConData *) condata->GetNext(pos));
		// *: The following should really check that the icon is displayed ( as in the following commented code)
		if (icondata->In(point)) {
			//icondata->show = false; //kwhite:canvassupport
			return retpos;
		}
		/*
		if (icondata->show==1) {
				if ..... return pos;
			}

		}
		  */
	}
  return pos;

}

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Method that highlights the specified FB
void DEP::setFBHighlight(INXPOSITION fbPos)
{
	if (fbPos) {
		ConData* pFB = (ConData *) condata->GetAt(fbPos);
		pFB->selected = 1;
	}
}
#endif
// This function is run after the DEP is loaded, and is used to populate the tag lists
void DEP::InitTagLists() {
	INXPOSITION pos;
	ConData* blob;

	// Remove every tag from the lists
	boolTags->RemoveAll();
	intTags->RemoveAll();
	realTags->RemoveAll();
	stringTags->RemoveAll();
	eventTags->RemoveAll();

	pos = condata->GetHeadPosition();
	while (pos) {
		UINT i;
		blob = (ConData*) (condata->GetNext(pos));
		for (i=0; i<blob->inputport_num; i++) {
			if (blob->inputport[i]->tag != wxT("")) {
				AddTag2List(blob->inputport[i]->tag, blob->inputport[i]->datatype);
			}
		}
		for (i=0; i<blob->outputport_num; i++) {
			if (blob->outputport[i]->tag != wxT("")) {
				AddTag2List(blob->outputport[i]->tag, blob->outputport[i]->datatype);
			}
		}
		for (i=0; i<blob->startport_num; i++) {
			if (blob->startport[i]->tag != wxT("")) {
				AddTag2List(blob->startport[i]->tag, EVENT);
			}
		}
		for (i=0; i<blob->finishport_num; i++) {
			if (blob->finishport[i]->tag != wxT("")) {
				AddTag2List(blob->finishport[i]->tag, EVENT);
			}
		}
	}
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
bool DEP::IsUniqueID(long id) {
	INXPOSITION pos;
	ConData* blob;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		if (blob->identnum == id) {
			return FALSE;
		}
	}

	return TRUE;
}

/* Search through a data file and add icons
*: Needs doing again - stream the class (see above)
*/
#endif
void DEP::LoadProg(INXString Info) {
	depFSM.enabledraw = 0;
	//ResizeParentToFit( );   // Default bShrinkOnly argument
	ifstream datafile((char*)Info);
	char type[256];
	long int id,nothingFOundCounter=0;
	bool bCanvasFlag = FALSE;

	while ((!datafile.eof()) && (!datafile.bad())) {

		datafile >> type;
		if (strcmp(type,"END_OF_BLOCKS")==0) break;
		else
		if (strcmp(type,"BEGIN_BLOCK")==0) {
			ConData *blob=new ConData;
			blob->Load(&datafile);
			condata->AddTail((INXObject*) blob);
			id = blob->identnum;
		} else
		if (strcmp(type,"CanvasSizeXY")==0) {
			INXSize temp;
			datafile >> temp.cx;
			datafile >> temp.cy;
			setCanvasSize(temp);
			bCanvasFlag = TRUE;
		}
		else {
			if (nothingFOundCounter++ > 1000000) break; // Nothing found problem with data but don't hang

		}
	}

	// Check for the case when the .prg file doesn't have a canvas size
	// This happens when using the SaveBlock method in the BlockOperations class
	// e.g. when performing encapsulation
	// It is possible that the encapsulated block is larger than the default canvas size
	// Therefore, extend the canvas if this is the case
	if (!bCanvasFlag) {
		initCanvasSize();
	}

	// set the uniqueidgenerator to the identnum of the last icon + 1
	// This is necessary as it is possible that this value may be greater than
	// the number of icons loaded, due to icons being deleted previously. This
	// prevents icon IDs being duplicated
	// Need to add 1, since uniqueidgenerator is incremented after a new icon is
	// instantiated.
	id++;
	uniqueidgenerator = id;

	AssignIconLineEndPoints(); // this function scans all the icons and updates the line endpoint  pointers to port points.
	depFSM.enabledraw = 1;
	//RedrawWindow();
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Maps the lineIDs in the flattened list to the lineIDs in condata
void DEP::MapLineID(INXObjList* flattened) {
	ConData *blob;
	ConData *flatBlob;
	ConData *otherBlockIcon;
	INXPOSITION pos, flatPos;
	INXString instanceName, blockFile;
	int finishLineID, outLineID;
	bool errorFlag = FALSE;

	// copy the line IDs from flattened list to condata list
	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		flatPos = flattened->GetHeadPosition();
		while(flatPos) {
			UINT i;
			flatBlob = (ConData *) (flattened->GetNext(flatPos));
			// if the icon is encapsulated then find the port in the flattened list that has the same hierID
			if (blob->m_iUserDefined) {
				for (i=0; i<blob->inputport_num; i++) {
					for (UINT j=0; j<flatBlob->inputport_num; j++) {
						if (blob->inputport[i]->line.hierID != -1 &&
							blob->inputport[i]->line.hierID == flatBlob->inputport[j]->line.hierID &&
							flatBlob->inputport[j]->line.exist) {
							blob->inputport[i]->line.idNum = flatBlob->inputport[j]->line.idNum;
						}
					}
				}
				for (i=0; i<blob->startport_num; i++) {
					for (UINT j=0; j<flatBlob->startport_num; j++) {
						if (blob->startport[i]->line.hierID != -1 &&
							blob->startport[i]->line.hierID == flatBlob->startport[j]->line.hierID &&
							flatBlob->startport[j]->line.exist) {
							blob->startport[i]->line.idNum = flatBlob->startport[j]->line.idNum;
						}
					}
				}
			}
			// if the icon is an XOUTPUT need to get the line ID from the icon in the
			// flattened list that is connected to the same icon the XOUTPUT is connected to
			else if (blob->m_csIconType.Find("XOUTPUT") > -1 && blob->inputport[0]->line.exist) {
				otherBlockIcon = GetIconFromID(blob->inputport[0]->line.othericonid);
				outLineID = GetOutLineID(flattened, otherBlockIcon, flatBlob, blob->inputport[0]->line.otherportno, hItem);
				if (outLineID) {
					//blob->inputport[0]->line.idNum = GetOutLineID(flattened, otherBlockIcon, flatBlob, blob->inputport[0]->line.otherportno);
					blob->inputport[0]->line.idNum = outLineID;
				}
			}
			// if the icon is an XFINISH need to get the line ID from the icon in the
			// flattened list that is connected to the same icon the XFINISH is connected to
			else if (blob->m_csIconType == "XFINISH" && blob->startport[0]->line.exist) {
				otherBlockIcon = GetIconFromID(blob->startport[0]->line.othericonid);
				// only assign the line ID if it is not 0
				finishLineID = GetFinishLineID(flattened, otherBlockIcon, flatBlob, blob->startport[0]->line.otherportno, hItem);
				if (finishLineID) {
					//blob->startport[0]->line.idNum = GetFinishLineID(flattened, otherBlockIcon, flatBlob, blob->startport[0]->line.otherportno);
					blob->startport[0]->line.idNum = finishLineID;
				}
			}
			// make sure its not an xport
			else if (blob->identnum == flatBlob->identnum && blob->m_csIconType.Find("XINPUT") == -1 && blob->m_csIconType.Find("XSTART") == -1) {
				// this is for debug purposes to make sure the icon ids are for the same icons
				if (blob->m_csIconType == flatBlob->m_csIconType) {
					for (i=0; i<blob->inputport_num; i++) {
						blob->inputport[i]->line.idNum = flatBlob->inputport[i]->line.idNum;
					}
					for (i=0; i<blob->startport_num; i++) {
						blob->startport[i]->line.idNum = flatBlob->startport[i]->line.idNum;
					}
				}
				else {
					if (!errorFlag) {
						errorFlag = TRUE;
						wxMessageBox("The mapping has failed. Debug may not work as expected.");
					}
					//wxMessageBox("The icons have same id but are not of same type");
				}
				// break out of while loop when line IDs have been copied
				break;
			}
		}
	}
}

#endif
//Test whether click is on a line - allow nodes etc. to be put on the line
// Check if point clicked is within a certain distance of the line.
int DEP::OnLine(INXPoint point, INXPOSITION* Icon, bool* nodeSel, int* portType, const bool &interrogateOnly ) {
	ConData *blob;
	INXPOSITION pos, retpos;
	INXPoint end, start;
	int endIndex, iLineSegNum;
	UINT i;
	LabLineSegmentTypeEnum eLineSegment = k_LabDiagonalLine;

	*nodeSel = FALSE;

	for (pos = condata->GetHeadPosition();pos !=NULL;) {
		retpos = pos;
		blob =((ConData *) condata->GetNext(pos));
		for (i=0; i<blob->inputport_num; i++) {
//@todo - refactor to remove duplicate code for input and start ports
			if (blob->inputport[i]->line.exist && blob->inputport[i]->tag == wxT("")) {
				endIndex = blob->inputport[i]->line.points.GetUpperBound();
				for (int j=0; j<endIndex; j++) {
					start = *((INXPoint*)blob->inputport[i]->line.points.GetAt(j));
					end = *((INXPoint*)blob->inputport[i]->line.points.GetAt(j+1));
					eLineSegment = IsOnLineSegment(point, start, end);

					// check if a node is selected and it is not the endpoint
					if (j!=endIndex-1 && OnNode(point, end)) {
						// set the selected node
						if(!interrogateOnly) blob->inputport[i]->line.nodeCount = j+1;
						*Icon = retpos;
						*nodeSel = TRUE;
						*portType = INPUTPORT;
						return i;
					}
					else if (eLineSegment == k_LabHorizontalLine || eLineSegment == k_LabVerticalLine) {
						if (!interrogateOnly) {
							bool alreadyAddedTwoPoints = false;
							iLineSegNum = j;
							// if the point clicked is above the line then the SnapToGrid function will
							// snap the point to the grid line above the drawn line. Therefore, force it here.
							if (start.y == end.y) {
								point.y = start.y;
							}
							// if connected to an output port add a dog-leg
							if (j == 0) {
								blob->inputport[i]->line.nodeCount = 0;
								blob->inputport[i]->line.addNode(SnapToGrid(point));
								blob->inputport[i]->line.nodeCount = 1;
								blob->inputport[i]->line.addNode(SnapToGrid(point));
								iLineSegNum = 2;
								alreadyAddedTwoPoints = true;
							}
							// if connected to an input port add a dog-leg
							if (j+1 == endIndex) {
								// if there is only a start and end point (ie a straight line)
								// then we will be here but have already added two points and 
								// are about to add another two - ensure these next two are displaced on x-axis from
								// first two - towards the output port
								if (alreadyAddedTwoPoints) {
									point.x = start.x + NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE;
								}
								blob->inputport[i]->line.nodeCount = j;
								blob->inputport[i]->line.addNode(SnapToGrid(point));
								blob->inputport[i]->line.nodeCount = j+1;
								blob->inputport[i]->line.addNode(SnapToGrid(point));
// 								iLineSegNum = j;  //code removed as selects the wrong segment, causing floating port problem							}
							}
							blob->inputport[i]->line.SetSelectedSegment(iLineSegNum);
						}
						*Icon = retpos;
						*portType = INPUTPORT;
						return i;
					}
					else if (eLineSegment == k_LabDiagonalLine) {
						if (!interrogateOnly) {
							//blob->inputport[i]->line.SetSelectedSegment(-1);
							blob->inputport[i]->line.SetSelectedSegment(j);
						}
						*Icon = retpos;
						*portType = INPUTPORT;
						return i;
					}
				}
			}
		}

		for (i=0; i<blob->startport_num; i++) {
			if (blob->startport[i]->line.exist && blob->startport[i]->tag == wxT("")) {
				endIndex = blob->startport[i]->line.points.GetUpperBound();
				for (int j=0; j<endIndex; j++) {
					start = *((INXPoint*)blob->startport[i]->line.points.GetAt(j));
					end = *((INXPoint*)blob->startport[i]->line.points.GetAt(j+1));
					eLineSegment = IsOnLineSegment(point, start, end);

					// check if a node is selected
					if (OnNode(point, end)) {
						// set the selected node
						if (!interrogateOnly) {
							blob->startport[i]->line.nodeCount = j+1;
						}
						*Icon = retpos;
						*nodeSel = TRUE;
						*portType = STARTPORT;
						return i;
					}
					else if (eLineSegment == k_LabHorizontalLine || eLineSegment == k_LabVerticalLine) {
						if (!interrogateOnly) {
							bool alreadyAddedTwoPoints = false;
							iLineSegNum = j;
							// if the point clicked is above the line then the SnapToGrid function will
							// snap the point to the grid line above the drawn line. Therefore, force it here.
							if (start.y == end.y) {
								point.y = start.y;
							}
							// if connected to an output port add a dog-leg
							if (j == 0) {
								blob->startport[i]->line.nodeCount = 0;
								blob->startport[i]->line.addNode(SnapToGrid(point));
								blob->startport[i]->line.nodeCount = 1;
								blob->startport[i]->line.addNode(SnapToGrid(point));
								iLineSegNum = 2;
								alreadyAddedTwoPoints = true;
							}
							// if connected to an input port add a dog-leg
							if (j+1 == endIndex) {
								// if there is only a start and end point (ie a straight line)
								// then we will be here but have already added two points and 
								// are about to add another two - ensure these next two are displaced on x-axis from
								// first two - towards the output port
								if (alreadyAddedTwoPoints) {
									point.x = start.x + NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE;
								}
								blob->startport[i]->line.nodeCount = j;
								blob->startport[i]->line.addNode(SnapToGrid(point));
								blob->startport[i]->line.nodeCount = j+1;
								blob->startport[i]->line.addNode(SnapToGrid(point));
// 								iLineSegNum = j;  //code removed as selects the wrong segment, causing floating port problem
							}
							blob->startport[i]->line.SetSelectedSegment(iLineSegNum);
						}
						*Icon = retpos;
						*portType = STARTPORT;
						return i;
					}
					else if (eLineSegment == k_LabDiagonalLine) {
						if (!interrogateOnly) {
							blob->startport[i]->line.SetSelectedSegment(-1);
						}
						*Icon = retpos;
						*portType = STARTPORT;
						return i;
					}
				}
			}
		}
	}
	return -1;

}

// Function that returns true if a point is close to a node.
bool DEP::OnNode(INXPoint point, INXPoint node) {
	int xdist,ydist;
	xdist=labs(point.x - node.x);
	ydist=labs(point.y - node.y);
	if ( xdist < NODECLOSENESS && ydist < NODECLOSENESS) {
		return  TRUE;
	}

	return FALSE;
}

// test for a mouse click is on a port and if so return a pointer to the icon and the
// relative port number
int DEP::OnConnect(INXPoint point,INXPOSITION* Icon,int * portType, int *portConnected) {
	//*input=-1;*output=-1;*conpoint=INXPoint(0,0);
	int portNo;
	ConData *icondata;
	INXPOSITION pos,retpos;
	for (pos = condata->GetHeadPosition();pos !=NULL;) {
		retpos=pos;
		icondata=((ConData *) condata->GetNext(pos));
		portNo=icondata->OnPort(point,portType,portConnected);
		if (portNo>-1) {
			*Icon=retpos;
			return portNo;
		}
	}
	return -1;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Function that removes a tag from a taglist
void DEP::RemoveTag(INXString tag, int dataType) {
	switch (dataType) {
	case 0 :
		{
			for (int i=0; i<boolTags->GetSize(); i++) {
				if (tag == boolTags->GetAt(i)) {
					boolTags->RemoveAt(i);
				}
			}
			break;
		}
	case 1 :
		{
			for (int i=0; i<intTags->GetSize(); i++) {
				if (tag == intTags->GetAt(i)) {
					intTags->RemoveAt(i);
				}
			}
			break;
		}
	case 2 :
		{
			for (int i=0; i<realTags->GetSize(); i++) {
				if (tag == realTags->GetAt(i)) {
					realTags->RemoveAt(i);
				}
			}
			break;
		}
	case 3 :
		{
			for (int i=0; i<stringTags->GetSize(); i++) {
				if (tag == stringTags->GetAt(i)) {
					stringTags->RemoveAt(i);
				}
			}
			break;
		}
	case 4 :
		{
			for (int i=0; i<eventTags->GetSize(); i++) {
				if (tag == boolTags->GetAt(i)) {
					eventTags->RemoveAt(i);
				}
			}
			break;
		}
	}
}

// Function that gives the icons in an encapsulated block unique IDs
void DEP::ReassignIconIDs(INXObjList* encapsulated)
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
		// excluding xports can cause a problem when pasting xports. Therefore remove the following 2 lines
		//if (blockIcon->m_csIconType.Find("XINPUT") == -1 && blockIcon->m_csIconType != "XSTART"
			//&& blockIcon->m_csIconType.Find("XOUTPUT") == -1 && blockIcon->m_csIconType != "XFINISH") {
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
		//}
	}
	ConData::uniqueidgenerator = newID;
}

void DEP::RenewPosition(INXPOSITION selected, INXPoint point, INXPoint firstpoint)
{
	INXPOSITION pos;
	ConData *icondata;
	ConData *otherIcon;
	UINT i;
	icondata=((ConData *) condata->GetAt(selected));
	icondata->RenewPosition(point, firstpoint);
	icondata->m_iShow = true; //kwhite:canvassupport

	//*** commented out but may need this
	//moveselected = NULL;//so it isn't highlighted anymore;

	// move lines connected to output ports
	INXPoint offsetpoint = firstpoint - point;
	// only reroute lines if component has been moved
	if ((offsetpoint.x == 0) && (offsetpoint.y == 0)) {
		return;
	}

	for (i=0; i<(int)icondata->outputport_num; i++) {
		pos = condata->GetHeadPosition();
		while (pos) {
			otherIcon = (ConData *) (condata->GetNext(pos));
			for (UINT j=0; j<otherIcon->inputport_num; j++) {
				if (icondata->identnum == otherIcon->inputport[j]->line.othericonid &&
					otherIcon->inputport[j]->line.otherportno == i && otherIcon->inputport[j]->line.exist) {
					otherIcon->inputport[j]->line.MoveOutNode(offsetpoint);
				}
			}
		}
	}
	// move lines connected to finish ports
	for (i=0; i<(int)icondata->finishport_num; i++) {
		pos = condata->GetHeadPosition();
		while (pos) {
			otherIcon = (ConData *) (condata->GetNext(pos));
			for (UINT j=0; j<otherIcon->startport_num; j++) {
				if (icondata->identnum == otherIcon->startport[j]->line.othericonid &&
					otherIcon->startport[j]->line.otherportno == i && otherIcon->startport[j]->line.exist) {
					otherIcon->startport[j]->line.MoveOutNode(offsetpoint);
				}
			}
		}
	}

	// for each input port and start port move x coord to be half way
	//
	//for (i=0; i<(int)icondata->inputport_num; i++) {
	//	otherIcon = GetIconFromID(icondata->inputport[i]->line.othericonid);
	//	otherPortNo = icondata->inputport[i]->line.otherportno);
	//	otherX = otherIcon->outputport[otherPortNo]->P.x;
	//	iconPortX = icondata->inputport[i]->P.x;
	//	newNodeX =
	//}
}

// Handle moving all objects selected or connected to selection
// Called when user lifts left mouse button to place a selected box at a new position
// or called to handle boundary hit event after inverting selection.
INXRect DEP::RenewSelectedPos(INXPoint newpoint, INXPoint firstpoint, INXRect selectRect)
{
	MoveObjectsWithinSelectedBox( newpoint, firstpoint) ;
	MoveExternallyConnectedLines( newpoint, firstpoint) ;
	/* @todo MIGRATION_ISSUES */
	//CPoint tempPnt = SnapToGrid((CPoint)newpoint);
	//INXPoint _newpoint(tempPnt.x, tempPnt.y); 
	selectRect.OffsetRect(newpoint - firstpoint);
	return selectRect;
}

void DEP::SnapIconToGrid(INXPOSITION selected)
{
	ConData* icondata = ((ConData *) condata->GetAt(selected));
	INXRect boundRect = icondata->GetBoundingRectangle();
	int iTop = boundRect.top;
	int iLeft = boundRect.left;
	INXPoint topLeft = boundRect.TopLeft();
	int iTopOffset = iTop % GRIDSIZE;
	int iLeftOffset = iLeft % GRIDSIZE;

	iTop = iTop - iTopOffset;
	iLeft = iLeft - iLeftOffset;
	INXPoint snappedTopLeft(iLeft, iTop);
	RenewPosition(selected, snappedTopLeft, topLeft);
}

// Handle moving lines outside of selection box but connected to it
// called when user lifts left mouse button to place a selected box at a new position
// or called when boundary hit event occurs after inverting selection.
void DEP::MoveExternallyConnectedLines(INXPoint newpoint, INXPoint firstpoint)
{
	ConData *icondata, *otherIcon;
	INXPOSITION pos1, pos2;

	// renew the position of every icon in the selected box
	pos1 = condata->GetHeadPosition();
	while (pos1) {
		icondata = (ConData *) (condata->GetNext(pos1));
		if (icondata->selected) {

			// move lines connected to output ports outside selection box (blue lines)
			INXPoint offsetpoint = firstpoint - newpoint;
			for (int j=0; j<(int)icondata->outputport_num; j++) {
				pos2 = condata->GetHeadPosition();
				while (pos2) {
					otherIcon = (ConData *) (condata->GetNext(pos2));
					if (!otherIcon->selected) {
						for (UINT k=0; k<otherIcon->inputport_num; k++) {
							if (icondata->identnum == otherIcon->inputport[k]->line.othericonid &&
								otherIcon->inputport[k]->line.otherportno == j && otherIcon->inputport[k]->line.exist) {
								otherIcon->inputport[k]->line.MoveOutNode(offsetpoint);
							}
						}
					}
				}
			}
			// move lines connected to finish ports outside selection box (black lines)
			for (int j=0; j<(int)icondata->finishport_num; j++) {
				pos2 = condata->GetHeadPosition();
				while (pos2) {
					otherIcon = (ConData *) (condata->GetNext(pos2));
					if (!otherIcon->selected) {
						for (UINT k=0; k<otherIcon->startport_num; k++) {
							if (icondata->identnum == otherIcon->startport[k]->line.othericonid &&
								otherIcon->startport[k]->line.otherportno == j && otherIcon->startport[k]->line.exist) {
								otherIcon->startport[k]->line.MoveOutNode(offsetpoint);
							}
						}
					}
				}
			}
		}
	}
}

// Handle moving lines, ports, icons within selection box
// called when user lifts left mouse button to place a selected box at a new position
// or called when boundary hit event occurs, just before moving inverted selection
void DEP::MoveObjectsWithinSelectedBox(INXPoint newpoint, INXPoint firstpoint)
{
	ConData *icondata, *otherIcon;
	INXPOSITION pos1;
	long othericonid;
	UINT i;

	pos1 = condata->GetHeadPosition();
	while (pos1) {
		icondata = (ConData *) (condata->GetNext(pos1));
		if ( icondata->selected )
		{
			// set lineSelected if line is in selection box
			for (i=0; i<icondata->inputport_num; i++) {
				if (icondata->inputport[i]->line.exist) {
					othericonid = icondata->inputport[i]->line.othericonid;
					otherIcon = GetIconFromID(othericonid);
					if (otherIcon->selected )
					{
						icondata->inputport[i]->line.lineSelected = 1;	// (blue line)
					}
				}
			}
			for (i=0; i<icondata->startport_num; i++) {
				if (icondata->startport[i]->line.exist) {
					othericonid = icondata->startport[i]->line.othericonid;
					otherIcon = GetIconFromID(othericonid);
					if ( otherIcon->selected)
					{
						icondata->startport[i]->line.lineSelected = 1;	// (black line)
					}
				}
			}
			icondata->RenewPosition(newpoint, firstpoint);

			// reset lineSelected if line is in selection box
			for (i=0; i<icondata->inputport_num; i++) {
				if (icondata->inputport[i]->line.exist) {
					othericonid = icondata->inputport[i]->line.othericonid;
					otherIcon = GetIconFromID(othericonid);
					if ( otherIcon->selected )
					{
						icondata->inputport[i]->line.lineSelected = 0;
					}
				}
			}
			for (i=0; i<icondata->startport_num; i++) {
				if (icondata->startport[i]->line.exist) {
					othericonid = icondata->startport[i]->line.othericonid;
					otherIcon = GetIconFromID(othericonid);
					if ( otherIcon->selected )
					{
						icondata->startport[i]->line.lineSelected = 0;
					}
				}
			}
		}
	}
}

//Function inverts selected flag for all objects
void DEP::InvertSelected()
{
	ConData *icondata;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();
	while (pos)
	{
		icondata = (ConData *) (condata->GetNext(pos));
		icondata->selected  = !icondata->selected;
	}
}



// Function that resets all line dbgEvents
void DEP::ResetAllDbgEvents() {
	ConData *blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		for (UINT i=0; i<blob->startport_num; i++) {
			blob->startport[i]->line.dbgEvent = 0;
		}
	}
}

// Function that resets all line dbgValues
void DEP::ResetAllDbgValues() {
	ConData *blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		for (UINT i=0; i<blob->inputport_num; i++) {
			blob->inputport[i]->line.dbgValue = "";
		}
	}
}

/* Iterate through all the icons and save to file
*: This could be converted to a streamble class etc.? and done automatically
*/
#endif
void DEP::SaveProg(INXString Info) {
	ofstream datafile((char *)Info);
	//put an error trap
	ConData *blob;
	INXString tmp;
	if (!datafile.good()) {
		tmp.Format(wxT("File %s could not be written"), (char*)Info);
		wxMessageBox(tmp);
	}

	datafile << "CanvasSizeXY" << endl;
	datafile << getCanvasSize().cx << "\t";
	datafile << getCanvasSize().cy << endl << endl;

	datafile << "IconData" << endl;
	INXPOSITION pos;
	pos = condata->GetHeadPosition();
	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		datafile<<"BEGIN_BLOCK"<<endl;
		blob->Save(&datafile);
	}
	datafile<<"END_OF_BLOCKS"<<endl;
	datafile.close();

}


// the flattened list has the line IDs. This list is only used to write out sodl. The condata
// list does not contain line IDs. The condata list is displayed. There are 2 choices for debug.
// Firstly, display flattened list. This is a bad idea as there is no hierarchy since encapsulated
// blocks have been flattened. Secondly, update condata with the line IDs. This is the option
// implemented here.
/*
void DEP::SetCondataLineID(CObList* flattened) {
	ConData *blob;
	ConData *flatBlob;
	ConData *otherBlockIcon;
	POSITION pos, flatPos;
	INXString instanceName, blockFile;
	int finishLineID, outLineID;

	// copy the line IDs from flattened list to condata list
	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		flatPos = flattened->GetHeadPosition();
		while(flatPos) {
			flatBlob = (ConData *) (flattened->GetNext(flatPos));
			// if the icon ids are the same copy the line ids
//			if ((instName == "temptop" && blob->identnum == flatBlob->identnum) ||
//				(instName != "temptop" && blob->identnum ==  flatBlob->blockID)) {
			if (blob->identnum == flatBlob->identnum) {
				// this is for debug purposes to make sure the icon ids are for the same icons
				if (blob->m_csIconType == flatBlob->m_csIconType) {
					for (UINT i=0; i<blob->inputport_num; i++) {
						blob->inputport[i]->line.idNum = flatBlob->inputport[i]->line.idNum;
					}
					for (i=0; i<blob->startport_num; i++) {
						blob->startport[i]->line.idNum = flatBlob->startport[i]->line.idNum;
					}
				}
				else {
					wxMessageBox("The icons have same id but are not of same type");
				}
				// break out of while loop when line IDs have been copied
				break;
			}
			// if the icon is encapsulated then need to get line ID from within this block
			else if (blob->m_iUserDefined) {
				for (UINT i=0; i<blob->inputport_num; i++) {
					blob->inputport[i]->line.idNum = GetInLineID(blob, flatBlob, i);
				}
				for (i=0; i<blob->startport_num; i++) {
					blob->startport[i]->line.idNum = GetStartLineID(blob, flatBlob, i);
				}
			}
			// if the icon is an XOUTPUT need to get the line ID from the icon in the
			// flattened list that is connected to the same icon the XOUTPUT is connected to
			else if (blob->m_csIconType.Find("XOUTPUT") > -1 && blob->inputport[0]->line.exist) {
				otherBlockIcon = GetIconFromID(blob->inputport[0]->line.othericonid);
				outLineID = GetOutLineID(flattened, otherBlockIcon, flatBlob, blob->inputport[0]->line.otherportno);
				if (outLineID) {
					blob->inputport[0]->line.idNum = GetOutLineID(flattened, otherBlockIcon, flatBlob, blob->inputport[0]->line.otherportno);
				}
			}
			// if the icon is an XFINISH need to get the line ID from the icon in the
			// flattened list that is connected to the same icon the XFINISH is connected to
			else if (blob->m_csIconType == "XFINISH" && blob->startport[0]->line.exist) {
				otherBlockIcon = GetIconFromID(blob->startport[0]->line.othericonid);
				// only assign the line ID if it is not 0
				finishLineID = GetFinishLineID(flattened, otherBlockIcon, flatBlob, blob->startport[0]->line.otherportno);
				if (finishLineID) {
					blob->startport[0]->line.idNum = GetFinishLineID(flattened, otherBlockIcon, flatBlob, blob->startport[0]->line.otherportno);
				}
			}
		}
	}
}
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Replace tag with a line
void DEP::ShowLine(INXPOSITION iconPos, int portNum, int portType) {
	ConData *blob;
	INXString tag;

	blob = (ConData*) condata->GetAt(iconPos);
	// remove tag so that line is drawn
	if (portType == INPUTPORT) {
		if (blob->inputport[portNum]->line.exist) {
			blob->inputport[portNum]->tag = "";
		}
		else {
			wxMessageBox("This port is not connected.");
		}
	}
	else if (portType == STARTPORT) {
		if (blob->startport[portNum]->line.exist) {
			blob->startport[portNum]->tag = "";
		}
		else {
			wxMessageBox("This port is not connected.");
		}
	}
	else if (portType == OUTPUTPORT) {
		if (IsOutputPortConnected(blob, portNum)) {
			blob->outputport[portNum]->tag = "";
			RemoveConnectedInputTags(blob, portNum);
		}
		else {
			wxMessageBox("This port is not connected.");
		}
	}
	else if (portType == FINISHPORT) {
		if (IsFinishPortConnected(blob, portNum)) {
			blob->finishport[portNum]->tag = "";
			RemoveConnectedStartTags(blob, portNum);
		}
		else {
			wxMessageBox("This port is not connected.");
		}
	}
}

bool DEP::IsOutputPortConnected(ConData *pBlob, int iPortNum)
{
	bool bIsConnected = FALSE;
	INXPOSITION pos;
	ConData* pOtherBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pOtherBlob = (ConData*) condata->GetNext(pos);
		for (UINT i=0; i<pOtherBlob->inputport_num; i++) {
			if (pBlob->identnum == pOtherBlob->inputport[i]->line.othericonid && pOtherBlob->inputport[i]->line.exist &&
				iPortNum == pOtherBlob->inputport[i]->line.otherportno) {
				bIsConnected = TRUE;
			}
		}
	}

	return bIsConnected;
}

bool DEP::IsFinishPortConnected(ConData *pBlob, int iPortNum)
{
	bool bIsConnected = FALSE;
	INXPOSITION pos;
	ConData* pOtherBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pOtherBlob = (ConData*) condata->GetNext(pos);
		for (UINT i=0; i<pOtherBlob->startport_num; i++) {
			if (pBlob->identnum == pOtherBlob->startport[i]->line.othericonid && pOtherBlob->startport[i]->line.exist &&
				iPortNum == pOtherBlob->startport[i]->line.otherportno) {
				bIsConnected = TRUE;
			}
		}
	}

	return bIsConnected;
}

void DEP::RemoveConnectedInputTags(ConData *pBlob, int iPortNum)
{
	INXPOSITION pos;
	ConData* pOtherBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pOtherBlob = (ConData*) condata->GetNext(pos);
		for (UINT i=0; i<pOtherBlob->inputport_num; i++) {
			if (pBlob->identnum == pOtherBlob->inputport[i]->line.othericonid && pOtherBlob->inputport[i]->line.exist &&
				iPortNum == pOtherBlob->inputport[i]->line.otherportno) {
				pOtherBlob->inputport[i]->tag = "";
			}
		}
	}
}

void DEP::RemoveConnectedStartTags(ConData *pBlob, int iPortNum)
{
	INXPOSITION pos;
	ConData* pOtherBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pOtherBlob = (ConData*) condata->GetNext(pos);
		for (UINT i=0; i<pOtherBlob->startport_num; i++) {
			if (pBlob->identnum == pOtherBlob->startport[i]->line.othericonid && pOtherBlob->startport[i]->line.exist &&
				iPortNum == pOtherBlob->startport[i]->line.otherportno) {
				pOtherBlob->startport[i]->tag = "";
			}
		}
	}
}

// Function that performs subset encapsulation
ConData* DEP::SubsetEncapsulate(INXRect encapsulate, int lib) {
	Encapsulation subBlock;
	INXPoint point;
	INXObjList *depList = NULL, *encapsulated = NULL;
	INXPOSITION encapsulatedPos, pos, rmPos;
	ConData *encapsulatedIcon, *blob;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	BlockOperations bo;

	SaveProg(workDir + TEMPDIR + "temp");
	depList = bo.LoadBlock(workDir + TEMPDIR + "temp");

	// @todo SDG: REVISIT. This is a quick fix for now - Don't allow Xports to be encapsulated.
	// It should be possible to do this, but isn't straight forward, and there are a lot of other
	// bugs, so this can wait.
	if (subBlock.HasXPorts(encapsulate, depList)) {
		wxMessageBox("Currently, it is not possible to encapsulate Xports. Please try again.");
	}
	else {
		encapsulated = subBlock.EncapsulateSubset(encapsulate, depList, hItem, lib);
	}

	if (encapsulated) {
		// update project view
		pos = encapsulated->GetHeadPosition();
		while(pos) {
			blob = (ConData*) (encapsulated->GetNext(pos));
			if (blob->m_iUserDefined) {
				pFrame->m_wndProjectBar.m_cProjTree.DelItemFromProjectTree(blob, hItem);
			}
		}

		// add encapsulated icon to condata. Position block at the top left coordinates of the
		// rectangle drawn
		point.x = encapsulate.TopLeft().x;
		point.y = encapsulate.TopLeft().y;
		point = SnapToGrid(point);
		// Add the appropriate encapsulated icon depending on the number of xports
		encapsulatedPos = AddIcon(subBlock.iconBitmap, subBlock.blockName, point);
		ConnectEncapsulatedIcon(encapsulatedPos, encapsulate, depList);
		encapsulatedIcon = (ConData *) condata->GetAt(encapsulatedPos);
		CreateInstance(encapsulatedIcon, lib);
		// tag the ports that were previously tagged
		TagPorts(encapsulatedIcon, encapsulated);
		// delete xports
		// don't delete function blocks, as they get deleted when deleting all function blocks in depList
		// remove xports from deplist so don't delete twice which would cause a crash
		pos = depList->GetHeadPosition();
		while (pos) {
			rmPos = pos;
			blob = (ConData*) (depList->GetNext(pos));
			if (blob->m_csIconType == "XFINISH" || blob->m_csIconType == "XSTART" || blob->m_csIconType.Find("XINPUT") != -1 ||
				blob->m_csIconType.Find("XOUTPUT") != -1) {
				depList->RemoveAt(rmPos);
			}
		}
		pos = encapsulated->GetHeadPosition();
		while(pos) {
			blob = (ConData*) (encapsulated->GetNext(pos));
			if (blob->m_csIconType == "XFINISH" || blob->m_csIconType == "XSTART" || blob->m_csIconType.Find("XINPUT") != -1 ||
				blob->m_csIconType.Find("XOUTPUT") != -1) {
				delete blob;
			}
		}

		delete encapsulated;
		bo.DeleteBlock(depList);
		return encapsulatedIcon;
	}

	bo.DeleteBlock(depList);
	return NULL;
}

// Replaces a line with a tag at the ports the line was connected to
void DEP::TagLine(INXPOSITION iconPos, int portNum, int portType) {
	ConData *blob, *otherBlob;
	long othericonid;
	int otherportno;
	int dataType;

	// set tag on ports
	blob = (ConData*) condata->GetAt(iconPos);
	// get data type
	if (portType == INPUTPORT) {
		dataType = blob->inputport[portNum]->datatype;
	}
	else if (portType == OUTPUTPORT) {
		dataType = blob->outputport[portNum]->datatype;
	}
	else if (portType == STARTPORT || portType == FINISHPORT) {
		dataType = EVENT;
	}
	InitTagLists();
	CTagDialog dialog(boolTags, intTags, realTags, stringTags, eventTags, dataType, portType, condata, 1);
	if (portType == INPUTPORT) {
		othericonid = blob->inputport[portNum]->line.othericonid;
		otherportno = blob->inputport[portNum]->line.otherportno;
		otherBlob = GetIconFromID(othericonid);
		// if output port is already tagged use same tag for input
		if (otherBlob->outputport[otherportno]->tag != "") {
			blob->inputport[portNum]->tag = otherBlob->outputport[otherportno]->tag;
		}
		else if (dialog.ShowModal() == IDOK) {
			blob->inputport[portNum]->tag = dialog.tag;
			otherBlob->outputport[otherportno]->tag = dialog.tag;
		}
	}
	else if (portType == STARTPORT) {
		othericonid = blob->startport[portNum]->line.othericonid;
		otherportno = blob->startport[portNum]->line.otherportno;
		otherBlob = GetIconFromID(othericonid);
		// if finish port is already tagged use same tag for start port
		if (otherBlob->finishport[otherportno]->tag != "") {
			blob->startport[portNum]->tag = otherBlob->finishport[otherportno]->tag;
		}
		else if (dialog.ShowModal() == IDOK) {
			blob->startport[portNum]->tag = dialog.tag;
			otherBlob->finishport[otherportno]->tag = dialog.tag;
		}
	}
}

// Tags the ports on an encapsulated icon if they were tagged previously
void DEP::TagPorts(ConData* icon, INXObjList* encapsulated) {
	INXPOSITION pos;
	ConData* blob;
	UINT i;

	// tag start ports
	for (UINT i=0; i<icon->startport_num; i++) {
		pos = encapsulated->GetHeadPosition();
		while (pos) {
			blob = (ConData*) encapsulated->GetNext(pos);
			if (blob->m_csIconType == "XSTART" && icon->startport[i]->description == blob->description) {
				icon->startport[i]->tag = blob->finishport[0]->tag;
			}
		}
	}
	// tag input ports
	for (i=0; i<icon->inputport_num; i++) {
		pos = encapsulated->GetHeadPosition();
		while (pos) {
			blob = (ConData*) encapsulated->GetNext(pos);
			if (blob->m_csIconType.Find("XINPUT") != -1 && icon->inputport[i]->description == blob->description) {
				icon->inputport[i]->tag = blob->outputport[0]->tag;
			}
		}
	}
	// tag finish ports
	for (i=0; i<icon->finishport_num; i++) {
		pos = encapsulated->GetHeadPosition();
		while (pos) {
			blob = (ConData*) encapsulated->GetNext(pos);
			if (blob->m_csIconType == "XFINISH" && icon->finishport[i]->description == blob->description) {
				icon->finishport[i]->tag = blob->startport[0]->tag;
			}
		}
	}
	// tag output ports
	for (i=0; i<icon->outputport_num; i++) {
		pos = encapsulated->GetHeadPosition();
		while (pos) {
			blob = (ConData*) encapsulated->GetNext(pos);
			if (blob->m_csIconType.Find("XOUTPUT") != -1 && icon->outputport[i]->description == blob->description) {
				icon->outputport[i]->tag = blob->inputport[0]->tag;
			}
		}
	}
}

// Function that performs topdown encapsulation
void DEP::TopdownEncapsulate() {
	Encapsulation subBlock;

	subBlock.EncapsulateBlock(condata, hItem);
}

void DEP::HideSelected(INXPOSITION m_ItemSelected)
{
	ConData *pCondata;

	if (m_ItemSelected!=NULL)
	{
		pCondata = (ConData*) condata->GetAt(m_ItemSelected);
		pCondata->m_iShow = false;
	}
}

void DEP::ShowSelected(INXPOSITION m_ItemSelected)
{
	ConData *pCondata;

	if (m_ItemSelected!=NULL)
	{
		pCondata = (ConData*) condata->GetAt(m_ItemSelected);
		pCondata->m_iShow = true;
	}
}

void DEP::HideSelectedPositions()
{
	ConData *icondata;
	INXPOSITION item;

	// renew the position of every icon in the selected box
	item = condata->GetHeadPosition();
	while (item)
	{
		icondata = (ConData *) (condata->GetNext(item));

		if ( icondata->selected )
		{
			//icondata->show = false;
			icondata->hideSelected();
		}
	}
}

void DEP::ShowSelectedPositions()
{
	ConData *icondata;
	INXPOSITION item;

	// renew the position of every icon in the selected box
	item = condata->GetHeadPosition();
	while (item)
	{
		icondata = (ConData *) (condata->GetNext(item));

		if ( icondata->selected )
		{
			icondata->showSelected();
		}
	}
}

#endif
//Handle canvas size being stretched beyond 2000,2000
void DEP::setCanvasSize(INXSize CanvasSize)
{
	m_CanvasSize = CanvasSize;
}

// Method that resizes the canvas if the application is bigger than the default canvas size
void DEP::initCanvasSize()
{
	ConData *pFB;
	INXPOSITION pos;

	// renew the position of every icon in the selected box
	pos = condata->GetHeadPosition();
	while (pos) {
		pFB = (ConData *) (condata->GetNext(pos));
		if (pFB->rectangle.bottom > m_CanvasSize.cy) {
			m_CanvasSize.cy = pFB->rectangle.bottom;
		}
		if (pFB->rectangle.right > m_CanvasSize.cx) {
			m_CanvasSize.cx = pFB->rectangle.right;
		}
	}
}

//Handle canvas size being stretched beyond 2000,2000
INXSize DEP::getCanvasSize()
{
	INXSize it;
	it.cx = 99;
	it.cy = 101;

	return (m_CanvasSize);
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void DEP::deleteConData()
{
	INXPOSITION pos;
	ConData* blob;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		delete blob;
	}
}

void DEP::setSaveSelectedFlag(bool flag)
{
	INXPOSITION pos;
	ConData* blob;

	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
		blob->saveSelectedFlag = flag;
	}
}

#endif
INXPoint DEP::getInitScrollPos(INXRect clientRect)
{
	INXPOSITION pos;
	ConData* blob;
	INXPoint top(-1,-1), left, scrollPos = top;
	INXPoint tmp;
	INXRect topRect, leftRect, intersectRect;

	// Check if the top most and the left most coordintes both fit in the client rect
	// If they do then the scroll position is this point. If not then the scroll position
	// is the left most icon.
	// To check if both coordinates fit in the client rect, topRect is the client Rect offset by the
	// top most coordinate, and leftRect is the client Rect offset by the left most coordinate. If
	// these 2 rects overlap then both coordinates fit.
	pos = condata->GetHeadPosition();
	while(pos) {
		blob = (ConData *) (condata->GetNext(pos));
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

	// Only check if rectangles overlap if there are any icons
	if (top.x != -1 && top.y != -1) {
		topRect = clientRect;
		topRect.OffsetRect(top);
		leftRect = clientRect;
		leftRect.OffsetRect(left);
		if (intersectRect.IntersectRect(topRect, leftRect)) {
			scrollPos.SetPoint(left.x - INIT_SCROLL_LEFT_MARGIN, top.y - INIT_SCROLL_TOP_MARGIN);
		}
		else {
			scrollPos.SetPoint(left.x - INIT_SCROLL_LEFT_MARGIN, left.y - INIT_SCROLL_TOP_MARGIN);
		}
	}

	// returns (-1,-1) if there are no icons
	return scrollPos;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void DEP::ResetXportConnected()
{
	ConData* blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();

	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		for (UINT i=0; i<blob->finishport_num; i++) {
			blob->finishport[i]->xportConnected = 0;
		}
		for (UINT i=0; i<blob->outputport_num; i++) {
			blob->outputport[i]->xportConnected = 0;
		}
	}
}

INXString DEP::GetPortLabel(INXPOSITION blockPos, int iPortNum, int iPortType)
{
	INXString csPortLabel;
	ConData* block;

	block = (ConData*) condata->GetAt(blockPos);

	switch (iPortType) {
	case INPUTPORT:
		{
			csPortLabel = block->inputport[iPortNum]->description;
			break;
		}
	case OUTPUTPORT:
		{
			csPortLabel = block->outputport[iPortNum]->description;
			break;
		}
	case STARTPORT:
		{
			csPortLabel = block->startport[iPortNum]->description;
			break;
		}
	case FINISHPORT:
		{
			csPortLabel = block->finishport[iPortNum]->description;
			break;
		}
	default:
		wxMessageBox("Data type doesn't exist"); break;
	}

	return csPortLabel;
}

void DEP::SetPortLabel(INXPOSITION blockPos, int iPortNum, int iPortType, INXString csPortLabel)
{
	ConData* block;

	block = (ConData*) condata->GetAt(blockPos);

	switch (iPortType) {
	case INPUTPORT:
		{
			block->inputport[iPortNum]->description = csPortLabel;
			break;
		}
	case OUTPUTPORT:
		{
			block->outputport[iPortNum]->description = csPortLabel;
			break;
		}
	case STARTPORT:
		{
			block->startport[iPortNum]->description = csPortLabel;
			break;
		}
	case FINISHPORT:
		{
			block->finishport[iPortNum]->description = csPortLabel;
			break;
		}
	default:
		wxMessageBox("Data type doesn't exist"); break;
	}
}

void DEP::SetPortLabel(INXString csBlockName, INXString csOldPortLabel, INXString csNewPortLabel)
{
	ConData* blob;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();

	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		if (blob->description == csBlockName) {

			for (UINT i=0; i<blob->inputport_num; i++) {
				if (blob->inputport[i]->description == csOldPortLabel) {
					blob->inputport[i]->description = csNewPortLabel;
				}
			}
			for (UINT i=0; i<blob->outputport_num; i++) {
				if (blob->outputport[i]->description == csOldPortLabel) {
					blob->outputport[i]->description = csNewPortLabel;
				}
			}
			for (UINT i=0; i<blob->startport_num; i++) {
				if (blob->startport[i]->description == csOldPortLabel) {
					blob->startport[i]->description = csNewPortLabel;
				}
			}
			for (UINT i=0; i<blob->finishport_num; i++) {
				if (blob->finishport[i]->description == csOldPortLabel) {
					blob->finishport[i]->description = csNewPortLabel;
				}
			}
		}
	}
}

void DEP::RenameSubsystem(INXString csOldInstName, INXString csNewInstName, HTREEITEM hUserdefItem)
{
	INXString csDepPath, csInstNum, csProjectDir;
	CFileOperation fo;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CDrawProgApp *pApp = ( CDrawProgApp *) AfxGetApp();
	INXPOSITION pos;
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hUserdefItem);

	pProject->pProjMData->getProjectDir(csProjectDir);

	// update filesystem
	csDepPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hUserdefItem);
	fo.Rename(csProjectDir + DEPDIR + csDepPath + csOldInstName, csProjectDir + DEPDIR + csDepPath + csNewInstName);
	fo.Rename(csProjectDir + DEPDIR + csDepPath + csOldInstName + ".prg", csProjectDir + DEPDIR + csDepPath + csNewInstName + ".prg");

	// update project window
	pFrame->m_wndProjectBar.m_cProjTree.SetItemText(hUserdefItem, csNewInstName);

	// update subsystem document window if its open
	pos = ((CDrawProgApp*)AfxGetApp())->pDocTemplate->GetFirstDocPosition();
	while (pos) {
		CDrawProgDoc* pDocmnt = (CDrawProgDoc*) ((CDrawProgApp*)AfxGetApp())->pDocTemplate->GetNextDoc(pos);
		if (pDocmnt->pDEP->depFilename == csOldInstName) {
			pDocmnt->SetTitle(csNewInstName);
			pDocmnt->SetPathName(csProjectDir + DEPDIR + csDepPath + csNewInstName + ".prg");
			pDocmnt->pDEP->depFilename = csNewInstName;
		}
	}
}

void DEP::AddToLibrary(INXPOSITION blockPos, INXString csMenuName)
{
	CFileOperation fo;
	HTREEITEM hUserdefItem;
	INXString csDepPath, csProjectDir;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
	ConData* blob = (ConData*) condata->GetAt(blockPos);
	Encapsulation subSystem;
	INXObjList *encapsulated;
	BlockOperations bo;
	int nIconNum;

	// Copy subset to userdefdir
	hUserdefItem = pFrame->m_wndProjectBar.m_cProjTree.GetUserDefChildItem(blob, hItem);
	csDepPath = pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(hUserdefItem);
	pProject->pProjMData->getProjectDir(csProjectDir);
	fo.SetOverwriteMode(TRUE);
	// delete dir. if it exists so can do a rename
	fo.Delete((INXString)workDir + USERDEFDIR + blob->className);
	fo.Copy(csProjectDir + DEPDIR + csDepPath + blob->description, (INXString)workDir + USERDEFDIR);
	fo.Rename((INXString)workDir + USERDEFDIR + blob->description, (INXString)workDir + USERDEFDIR + blob->className);
	// delete .prg if it exists so can do a rename
	fo.Delete((INXString)workDir + USERDEFDIR + blob->className + ".prg");
	fo.Copy(csProjectDir + DEPDIR + csDepPath + blob->description + ".prg", (INXString)workDir + USERDEFDIR);
	fo.Rename((INXString)workDir + USERDEFDIR + blob->description + ".prg", (INXString)workDir + USERDEFDIR + blob->className + ".prg");

	//write IDF
	encapsulated = bo.LoadBlock((INXString)workDir + USERDEFDIR + blob->className + ".prg");
	subSystem.SetEncapsulated(encapsulated);
	nIconNum = subSystem.SelectEncapsulateIcon();
	subSystem.WriteIDF(blob, nIconNum, csMenuName);
	delete encapsulated;
}

bool DEP::IsRectEmpty(INXRect selectRect)
{
	ConData* blob;
	INXPOSITION pos;
	bool bIsEmpty = TRUE;

	pos = condata->GetHeadPosition();

	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		if (selectRect.PtInRect(blob->GetIconCentre())) {
			bIsEmpty = FALSE;
		}
	}

	return bIsEmpty;
}

ConData* DEP::getIconAtPoint(INXPoint point)
{
	ConData* blob;
	ConData* ret = NULL;
	INXPOSITION pos;

	pos = condata->GetHeadPosition();

	while (pos) {
		blob=(ConData *) (condata->GetNext(pos));
		if (blob->rectangle.PtInRect(point)) {
			ret = blob;
		}
	}

	return ret;
}

INXPOSITION DEP::getPosFromIcon(ConData *pIcon)
{
	ConData* pBlob;
	INXPOSITION pos, oldpos, retpos;

	pos = condata->GetHeadPosition();

	while (pos) {
		oldpos = pos;
		pBlob = (ConData *) (condata->GetNext(pos));
		if (pBlob == pIcon) {
			retpos = oldpos;
		}
	}

	return retpos;
}


// Method that drags connections from one port to another by swapping ports
void DEP::dragConnections(INXPOSITION selectedIcon, int iSelectedPort, int iSelectedPortType, INXPOSITION selectedIcon2,int iSelectedPort2, int iSelectedPortType2)
{
	ConData *pFromIcon; // dragging connection from this icon
	ConData *pToIcon;	// dragging connection to this icon
	ConData *otherBlob; // this is the icon at the other end of the connection
	Port* pTmpPort;
	vector<IconLines*> linesVec;
	INXString csTmpDesc;
	long othericonid;

	pFromIcon = (ConData *) (condata->GetAt(selectedIcon));
	pToIcon = (ConData *) (condata->GetAt(selectedIcon2));



	switch (iSelectedPortType) {
	case 0 :	// input port
		{
			if (!pToIcon->inputport[iSelectedPort2]->connected) {
				if (pFromIcon->inputport[iSelectedPort]->datatype == pToIcon->inputport[iSelectedPort2]->datatype) {
					othericonid = pFromIcon->inputport[iSelectedPort]->line.othericonid;
					otherBlob = GetIconFromID(othericonid);
					if (areBothIconsXPorts(otherBlob, pToIcon)) {
						wxMessageBox("Cannot drag connections, because start and end points are Xports.");
					} else {
						pTmpPort = pFromIcon->inputport[iSelectedPort];
						pFromIcon->inputport[iSelectedPort] = pToIcon->inputport[iSelectedPort2];
						pToIcon->inputport[iSelectedPort2] = pTmpPort;
						swapPortData(pFromIcon->inputport[iSelectedPort], pToIcon->inputport[iSelectedPort2]);
					}
				}
				else {
					wxMessageBox("Cannot drag connections, because the datatypes do not match.");
				}
			}
			else {
				wxMessageBox("Cannot drag connections, because the target port is already connected.");
			}
			break;
		}
	case 1 :	// output port
		{
			getConnectedLines(pToIcon->identnum, iSelectedPort2, OUTPUTPORT, linesVec);
			if (!linesVec.size()) {
				if (pFromIcon->outputport[iSelectedPort]->datatype == pToIcon->outputport[iSelectedPort2]->datatype) {
					othericonid = pFromIcon->outputport[iSelectedPort]->line.othericonid;
					otherBlob = GetIconFromID(othericonid);
					if (areBothIconsXPorts(otherBlob, pToIcon)) {
						wxMessageBox("Cannot drag connections, because start and end points are Xports.");
					} else {
						pTmpPort = pFromIcon->outputport[iSelectedPort];
						pFromIcon->outputport[iSelectedPort] = pToIcon->outputport[iSelectedPort2];
						pToIcon->outputport[iSelectedPort2] = pTmpPort;
						swapPortData(pFromIcon->outputport[iSelectedPort], pToIcon->outputport[iSelectedPort2]);
						// reroute and update all lines connected to this output
						rerouteDraggedConnections(pFromIcon->identnum, pToIcon->identnum, iSelectedPort, iSelectedPort2, OUTPUTPORT);
					}
				}
				else {
					wxMessageBox("Cannot drag connections, because the datatypes do not match.");
				}
			}
			else {
				wxMessageBox("Cannot drag connections, because the target port is already connected.");
			}
			break;
		}
	case 2 :	// start port
		{
			if (!pToIcon->startport[iSelectedPort2]->connected && !pToIcon->startport[iSelectedPort2]->initialise) {
				othericonid = pFromIcon->startport[iSelectedPort]->line.othericonid;
				otherBlob = GetIconFromID(othericonid);
				if (areBothIconsXPorts(otherBlob, pToIcon)) {
					wxMessageBox("Cannot drag connections, because start and end points are Xports.");
				} else {
					pTmpPort = pFromIcon->startport[iSelectedPort];
					pFromIcon->startport[iSelectedPort] = pToIcon->startport[iSelectedPort2];
					pToIcon->startport[iSelectedPort2] = pTmpPort;
					swapPortData(pFromIcon->startport[iSelectedPort], pToIcon->startport[iSelectedPort2]);
				}
			}
			else {
				wxMessageBox("Cannot drag connections, because the target port is already connected or initialised.");
			}
			break;
		}
	case 3 :	// finish port
		{
			getConnectedLines(pToIcon->identnum, iSelectedPort2, FINISHPORT, linesVec);
			if (!linesVec.size()) {
				othericonid = pFromIcon->finishport[iSelectedPort]->line.othericonid;
				otherBlob = GetIconFromID(othericonid);
				if (areBothIconsXPorts(otherBlob, pToIcon)) {
					wxMessageBox("Cannot drag connections, because start and end points are Xports.");
				} else {
					pTmpPort = pFromIcon->finishport[iSelectedPort];
					pFromIcon->finishport[iSelectedPort] = pToIcon->finishport[iSelectedPort2];
					pToIcon->finishport[iSelectedPort2] = pTmpPort;
					swapPortData(pFromIcon->finishport[iSelectedPort], pToIcon->finishport[iSelectedPort2]);
					// reroute and update all lines connected to this output
					rerouteDraggedConnections(pFromIcon->identnum, pToIcon->identnum, iSelectedPort, iSelectedPort2, FINISHPORT);
				}
			}
			else {
				wxMessageBox("Cannot drag connections, because the target port is already connected.");
			}
			break;
		}
	}
}

// Method that swaps port data
void DEP::swapPortData(Port* pPort1, Port* pPort2)
{
	INXPoint port1OffsetPoint, port2OffsetPoint;
	INXString csTmpPortDesc;
	INXObjArray<unsigned int> *tmpFuncArg;
	INXObjArray<INXString> *tmpFuncName;
	bool tmpbPortVertical;
	long int tmplocation;
	int tmpxportConnected;
	int tmpatomicFlag;
	bool tmpmandatoryFlag;
	int tmpuserdefined;

	// Swap port positions
	port1OffsetPoint = pPort1->P - pPort2->P;
	port2OffsetPoint = pPort2->P - pPort1->P;
	pPort1->Move(port1OffsetPoint);
	pPort2->Move(port2OffsetPoint);

	// Swap port descriptions
	csTmpPortDesc = pPort1->description;
	pPort1->description = pPort2->description;
	pPort2->description = csTmpPortDesc;

	// Swap port functions
	tmpFuncArg = pPort1->funcArg;
	pPort1->funcArg = pPort2->funcArg;
	pPort2->funcArg = tmpFuncArg;
	tmpFuncName = pPort1->funcName;
	pPort1->funcName = pPort2->funcName;
	pPort2->funcName = tmpFuncName;

	// commented out attributes that are either already transferred in code above (eg. funcName) or need to stay with the port (eg. lineID)
/*
//	INXPoint P;   // position
//	int porttype;   //0 input, 1 output, 2 Trig input, 3 Trig output
	bool bPortVertical;
//	int datatype;   //0 bool, 1 int, 2 float, 3 string 
//	int portNum; // port number
//	INXString description;
//	INXObjArray<INXString>* funcName;
//	CUIntArray* funcArg;
//	int connected;
//	IconLines line; //this is the input line (one only allowed)
	long int location;
//	long int lineID; //this is for output ports. needed to write out SODL
	// Indicates if port is connected to a xport. Required for subset encapsulation.
	// Ensures that an output with multiple connections only has one xport.
	// Ensures inputs only have one connection
	int xportConnected;
	// Required for subset encapsulation. When an input is connected to an output that has a fanout 
	// greater than 1 only want to create 1 xport. Therefore, when it is created store its id in the
	// output port it is connected to.
	long xportID;
//	int initialise;
//	INXSize bitmapSize;
//	INXSize bitmHighSize;
//	Bitmap bitmap;
//	Bitmap bitmHighlight;
//	INXRect rectangle;
	int atomicFlag;
	bool mandatoryFlag;
//	INXString tag;
//	UINT groupID;
	int userdefined;
*/
	// Swap everything else that needs swapping
	tmpbPortVertical = pPort1->bPortVertical;
	pPort1->bPortVertical = pPort2->bPortVertical;
	pPort2->bPortVertical = tmpbPortVertical;

	tmplocation = pPort1->location;
	pPort1->location = pPort2->location;
	pPort2->location = tmplocation;

	tmpxportConnected = pPort1->xportConnected;
	pPort1->xportConnected = pPort2->xportConnected;
	pPort2->xportConnected = tmpxportConnected;

	tmpatomicFlag= pPort1->atomicFlag;
	pPort1->atomicFlag = pPort2->atomicFlag;
	pPort2->atomicFlag = tmpatomicFlag;

	tmpmandatoryFlag = pPort1->mandatoryFlag;
	pPort1->mandatoryFlag = pPort2->mandatoryFlag;
	pPort2->mandatoryFlag = tmpmandatoryFlag;

	tmpuserdefined = pPort1->userdefined;
	pPort1->userdefined = pPort2->userdefined;
	pPort2->userdefined = tmpuserdefined;
}

// method that gets a vector of lines connected to an output or finish port
void DEP::getConnectedLines(long iID, int iPortNum, int iPortType, vector<IconLines*> &linesVec)
{
	INXPOSITION pos, prevPos;
	ConData* pBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		prevPos = pos;
		pBlob = (ConData *) (condata->GetNext(pos));
		if (iPortType == OUTPUTPORT) {
			for (UINT i=0; i<pBlob->inputport_num; i++) {
				if (pBlob->inputport[i]->line.othericonid == iID && pBlob->inputport[i]->line.otherportno == iPortNum &&
					pBlob->inputport[i]->line.exist) {
					linesVec.push_back(&pBlob->inputport[i]->line);
				}
			}
		}
		else if (iPortType == FINISHPORT) {
			for (UINT i=0; i<pBlob->startport_num; i++) {
				if (pBlob->startport[i]->line.othericonid == iID && pBlob->startport[i]->line.otherportno == iPortNum &&
					pBlob->startport[i]->line.exist) {
					linesVec.push_back(&pBlob->startport[i]->line);
				}
			}
		}
	}
}

// Method that reroutes dragged outputs
// Has to search DEP for all inputs connected to the output
// Also updates 'othericonid' and 'otherportno' attributes in line object
void DEP::rerouteDraggedConnections(long iOldID, long iNewID, long iOldPortNum, long iNewPortNum, int iPortType)
{
	INXPOSITION pos;
	ConData* pBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (condata->GetNext(pos));
		if (iPortType == OUTPUTPORT) {
			for (UINT i=0; i<pBlob->inputport_num; i++) {
				if (pBlob->inputport[i]->line.othericonid == iOldID && pBlob->inputport[i]->line.otherportno == iOldPortNum) {
					pBlob->inputport[i]->line.othericonid = iNewID;
					pBlob->inputport[i]->line.otherportno = iNewPortNum;
					pBlob->inputport[i]->line.AddDogLeg(i*2+1, pBlob->rectangle.top);
				}
			}
		}
		else if (iPortType == FINISHPORT) {
			for (UINT i=0; i<pBlob->startport_num; i++) {
				if (pBlob->startport[i]->line.othericonid == iOldID && pBlob->startport[i]->line.otherportno == iOldPortNum) {
					pBlob->startport[i]->line.othericonid = iNewID;
					pBlob->startport[i]->line.otherportno = iNewPortNum;
					pBlob->startport[i]->line.AddDogLeg(i*2, pBlob->rectangle.top);
				}
			}
		}
	}
}

// Method that selects/clears all lines to be monitored during debug
// bSetClear is TRUE to select and FALSE to clear
void DEP::setAllMonitors(bool bSetClear)
{
	INXPOSITION pos;
	ConData* pBlob;
	TypeConversion tc;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);

	pos = condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (condata->GetNext(pos));
		for (UINT i=0; i<pBlob->inputport_num; i++) {
			if (pBlob->inputport[i]->line.exist) {
				pBlob->inputport[i]->line.setDbgMonitor(bSetClear);
				if (pProject->debug.debugMode != DBGSTOP) {
					/*
					pProject->debug.sendMonitor(bSetClear, pBlob->inputport[i]->line.idNum, tc.DataType2Char(pBlob->inputport[i]->datatype));
					*/
					if (!pProject->debug.sendMonitor(bSetClear, pBlob->inputport[i]->line.idNum, tc.DataType2Char(pBlob->inputport[i]->datatype))) {
							return;
					}
				}
			}
		}
		for (UINT i=0; i<pBlob->startport_num; i++) {
			if (pBlob->startport[i]->line.exist) {
				pBlob->startport[i]->line.setDbgMonitor(bSetClear);
				if (pProject->debug.debugMode != DBGSTOP) {
					/*pProject->debug.sendMonitor(pBlob->startport[i]->line.getDbgMonitor(), pBlob->startport[i]->line.idNum, "E");*/
					if (!pProject->debug.sendMonitor(pBlob->startport[i]->line.getDbgMonitor(), pBlob->startport[i]->line.idNum, "E")) {
							return;
					}
				}
			}
		}
	}
}

// Method that toggles all input lines connected to the same output
void DEP::toggleMonitoredLine(int iPortType, long iOtherIconId, int iOtherPortNo)
{
	INXPOSITION pos;
	ConData* pBlob;

	pos = condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (condata->GetNext(pos));
		if (iPortType==INPUTPORT) {
			for (UINT i=0; i<pBlob->inputport_num; i++) {
				if (pBlob->inputport[i]->line.exist && pBlob->inputport[i]->line.othericonid==iOtherIconId &&
					pBlob->inputport[i]->line.otherportno==iOtherPortNo) {
						pBlob->inputport[i]->line.toggleDbgMonitor();
				}
			}
		}
		else if (iPortType==STARTPORT) {
			for (UINT i=0; i<pBlob->startport_num; i++) {
				if (pBlob->startport[i]->line.exist && pBlob->startport[i]->line.othericonid==iOtherIconId &&
					pBlob->startport[i]->line.otherportno==iOtherPortNo) {
						pBlob->startport[i]->line.toggleDbgMonitor();
				}
			}
		}
	}
}

#endif
LabLineSegmentTypeEnum DEP::IsOnLineSegment(INXPoint point, INXPoint node1, INXPoint node2)
{
	int iXDist1, iYDist1;
	LabLineSegmentTypeEnum eRet = k_LabNoLine;
	double dDistance, dDivisor;

	iXDist1 = labs(point.x - node1.x);
	iYDist1 = labs(point.y - node1.y);

	dDivisor = sqrt((double)((node2.x - node1.x) * (node2.x - node1.x)) + ((node2.y - node1.y)*(node2.y - node1.y)));
	if (dDivisor == 0.0) {
		dDivisor = 0.001;
	}
	// calculate distance to a line
	// the following formula does not take into account the line endpoints
	dDistance = (abs(((node2.x - node1.x)*(node1.y - point.y)) - ((node1.x - point.x) * (node2.y - node1.y))))/dDivisor;

	// Check if its vertical segement
	if (iXDist1 < LINECLOSENESS && node1.x == node2.x && ((point.y > node1.y && point.y < node2.y) || (point.y < node1.y && point.y > node2.y))) {
		eRet = k_LabVerticalLine;
	}
	// Check if its horizontal segment
	else if (iYDist1 < LINECLOSENESS && node1.y == node2.y && ((point.x > node1.x && point.x < node2.x) || (point.x < node1.x && point.x > node2.x))) {
		eRet = k_LabHorizontalLine;
	}
	// Check if its diagonal line segment
	else if ((dDistance < LINECLOSENESS) && WithinEndpoints(point, node1, node2)) {
		eRet = k_LabDiagonalLine;
	}

	return eRet;
}

// Function that returns true if a point is within the line endpoints
bool DEP::WithinEndpoints(INXPoint point, INXPoint start, INXPoint end) {
	double minx, maxx, miny, maxy;

	// set the min and max x values
	if (end.x > start.x) {
		minx = start.x - LINECLOSENESS;
		maxx = end.x + LINECLOSENESS;
	}
	else {
		minx = end.x - LINECLOSENESS;
		maxx = start.x + LINECLOSENESS;
	}

	// set the min and max y values
	if (end.y > start.y) {
		miny = start.y - LINECLOSENESS;
		maxy = end.y + LINECLOSENESS;
	}
	else {
		miny = end.y - LINECLOSENESS;
		maxy = start.y + LINECLOSENESS;
	}

	if ((point.x > minx) && (point.x < maxx) && (point.y > miny) && (point.y < maxy)) {
		return TRUE;
	}

	return FALSE;
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void DEP::PropagateDebugDataDown(ConData* pSubsysIcon)
{
	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
	//HTREEITEM hParent = pFrame->m_wndProjectBar.m_cProjTree.GetParentItem(hItem);
	//DEP* pParentDep = NULL;
	//INXString csParentDepName = pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hParent);
	INXPOSITION pos;
	//ConData *pParentBlob = NULL, *pSubsysIcon = NULL;
	ConData *pBlob = NULL, *pOtherBlob = NULL;

	// Get Parent DEP
	/*
	for (int i=0; i<pProject->DEPNum; i++) {
		if (pProject->pDEP[i]->depFilename == csParentDepName) {
			pParentDep = pProject->pDEP[i];
		}
	}

	// Get subsystem icon in parent DEP that corresponds to this DEP
	parentPos = pParentDep->condata->GetHeadPosition();
	while (parentPos) {
		pParentBlob = (ConData *) (pParentDep->condata->GetNext(parentPos));
		if (pParentBlob->description == depFilename) {
			pSubsysIcon = pParentBlob;
		}
	}
	*/

	// Propagate debug values from subsystem icon onto lines connected to xPorts
	if (pSubsysIcon) {
		pos = condata->GetHeadPosition();
		while (pos) {
			pBlob = (ConData *) (condata->GetNext(pos));
			for (UINT i=0; i<pBlob->startport_num; i++) {
				if (pBlob->startport[i]->line.exist) {
					pOtherBlob = GetIconFromID(pBlob->startport[i]->line.othericonid);
					for (UINT j=0; j<pSubsysIcon->startport_num; j++) {
						if (pOtherBlob->description == pSubsysIcon->startport[j]->description) {
							pBlob->startport[i]->line.dbgEvent = pSubsysIcon->startport[j]->line.dbgEvent;
						}
					}
				}
			}
			for (UINT i=0; i<pBlob->inputport_num; i++) {
				if (pBlob->inputport[i]->line.exist) {
					pOtherBlob = GetIconFromID(pBlob->inputport[i]->line.othericonid);
					for (UINT j=0; j<pSubsysIcon->inputport_num; j++) {
						if (pOtherBlob->description == pSubsysIcon->inputport[j]->description) {
							pBlob->inputport[i]->line.dbgValue = pSubsysIcon->inputport[j]->line.dbgValue;
						}
					}
				}
			}
		}
	}
}

void DEP::PropagateDebugDataUp(HTREEITEM hSubsys)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	INXString csSubsysDepName = pFrame->m_wndProjectBar.m_cProjTree.GetItemText(hSubsys);
	Project* pProject = pFrame->m_wndProjectBar.m_cProjTree.GetProjectPtr(hItem);
	DEP* pSubsysDep = NULL;
	INXPOSITION pos, subsysPos;
	ConData *pBlob = NULL, *pSubsysBlob = NULL, *pChildBlob = NULL;
	int iPortNum;

	// Find the subsystem icon in the DEP
	pos = condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (condata->GetNext(pos));
		if (pBlob->description == csSubsysDepName) {
			pSubsysBlob = pBlob;
		}
	}

	if (!pSubsysBlob) {
		return;
	}

	// Get the DEP for the subsystem
	for (int i=0; i<=pProject->DEPNum; i++) {
		if (pProject->pDEP[i]->depFilename == csSubsysDepName) {
			pSubsysDep = pProject->pDEP[i];
		}
	}

	if (!pSubsysDep) {
		return;
	}

	// Propagate debug values from subsystem output Xports to subsystem outputs
	pos = condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (condata->GetNext(pos));
		for (UINT i=0; i<pBlob->startport_num; i++) {
			// check if a finish port is connected to the subsystem
			if (pBlob->startport[i]->line.exist && pBlob->startport[i]->line.othericonid == pSubsysBlob->identnum) {
				iPortNum = pBlob->startport[i]->line.otherportno;
				// find the corresponding line in the subsystem
				// Could optimise this by creating a temporary list of Xports to search
				subsysPos = pSubsysDep->condata->GetHeadPosition();
				while (subsysPos) {
					pChildBlob = (ConData *) (pSubsysDep->condata->GetNext(subsysPos));
					if (pChildBlob->m_csIconType == "XFINISH" && pChildBlob->startport[0]->line.exist &&
						pChildBlob->description == pSubsysBlob->finishport[iPortNum]->description) {
							pBlob->startport[i]->line.dbgEvent = pChildBlob->startport[0]->line.dbgEvent;
					}
				}
			}
		}
		for (UINT i=0; i<pBlob->inputport_num; i++) {
			// check if a finish port is connected to the subsystem
			if (pBlob->inputport[i]->line.exist && pBlob->inputport[i]->line.othericonid == pSubsysBlob->identnum) {
				iPortNum = pBlob->inputport[i]->line.otherportno;
				// find the corresponding line in the subsystem
				// Could optimise this by creating a temporary list of Xports to search
				subsysPos = pSubsysDep->condata->GetHeadPosition();
				while (subsysPos) {
					pChildBlob = (ConData *) (pSubsysDep->condata->GetNext(subsysPos));
					if (pChildBlob->m_csIconType.Find("XOUTPUT") != -1 && pChildBlob->inputport[0]->line.exist &&
						pChildBlob->description == pSubsysBlob->outputport[iPortNum]->description) {
							pBlob->inputport[i]->line.dbgValue = pChildBlob->inputport[0]->line.dbgValue;
					}
				}
			}
		}
	}
}

#endif
