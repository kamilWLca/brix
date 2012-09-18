// Coninfo.cpp: implementation of the Coninfo class.
//
//////////////////////////////////////////////////////////////////////


//#include "DrawProg.h"
#include "condat.h"
#include "LucidConstants.h" //@todo 
#include "FileOperations.h" //@todo 
#include "GlobalFuncs_2.h" //@todo 
//#include "libxml/xmlreader.h" //@todo 

#include <fstream>
using namespace std;
#include "string.h"
#include "Porting_Classes/INXObjArray.h"
#include <wx/msgdlg.h>
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
// global variable declared in CDrawProgApp
extern wxChar workDir[WORK_DIR_SIZE];

//main
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*

*: Is a serachable key to identify code areas where further attention required
This is used to define 
connections between icons, where one icon will need to be able to identify another.
This method is likely to cause some issues when subfunction blocks are introduced into programmes, because
Unique identifiers must be ensured. I guess this will be done by the subfunction loading programming checking all the loaded identities, and 
re-assignaing if necessary. It will also mean  that we can cross this bridge when we get to it. I haven't yet thought of a better solution! All comments welcome!
*/
//static long int uniqueidgenerator; // one copy for all instances
//This variable will be set to 0 when the programme is initialised, though we don't care what it it as long as it is unique.

/*
Constructor assigns defeualt values to attributes
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::copyDialogData( const ConData &sourceData )
{
	this->iParamNum = sourceData.iParamNum;

	for (UINT i=1; i<sourceData.iParamNum; i++) {
		if( (this->iconParam[i]==NULL) && (sourceData.iconParam[i] != NULL) )
			this->iconParam[i] = new Parameter;

		*(this->iconParam[i]) = *(sourceData.iconParam[i]); 
	}

	return;
}

#endif
ConData::ConData()
{
	m_csIconType = "_";
	description="";;
	optionstring="_";
	rectangle = INXRect(0,0,0,0); /// body part of icon
	identnum=uniqueidgenerator++; //assign a unique identifier. *: should we check that this is unique in some cache or something?? - this would 
	// be more robust
	m_iShow = 1;
	showdescript=0;
	selected = 0;
	instNum = 0;
	hierarchyName = "";
	m_iUserDefined = 0;
	blockID = -1;
	saveSelectedFlag = FALSE;
	encapsulated = FALSE;
	inputport_num = 0, outputport_num = 0, startport_num = 0, finishport_num = 0, internalport_num = 0, iParamNum = 0;

	for(size_t i=0;i<MAXPARAMS;i++)
		iconParam[i] = NULL;
}

/*
Initialise attributes from data held within the file located at the path 
"..\ICicons\[_type]". Adjust the position attributes to _point and indicate if the icon is visible with 'show'
  */
// 'block' is the IDF file name for an encapsulated icon. For an encapsulated icon, the variable 'type' is 
// 'ENCAPSULATE' (bitmap file name) which is different to the IDF file name. For all other icons it is the same. 
// Therefore, can't use 'type' to read the IDF for an encapsulated icon.
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::init(INXString csIconType, INXString csBlockName, INXPoint point, int iShow) {
	m_iShow = iShow;
	m_csIconType = csIconType;

	if(m_csIconType == "")
	{
		wxMessageBox("Icon type is empty");
	}

	m_csBlockName = csBlockName;
	if (m_csBlockName != "") {
		ReadIDFFile(m_csBlockName, point);
	}
	else {
		ReadIDFFile(m_csIconType, point);
	}
	
	initBmp(point);

	if(m_iUserDefined == 1)	
	{
		ResizeIcon();
		repositionVerticalPorts();
	}
}

#endif
void ConData::initBmp(INXPoint _point) 
{
	INXString bitmappath;
	CFileOperation fo;

	// don't assume bitmap is in a particular directory, check CDF dir first then IDF
	bitmappath = workDir + CDFDIR + m_csIconType + wxT(".bmp");
	if (!fo.CheckPath(bitmappath)) {
		bitmappath = workDir + BMPDIR + m_csIconType + wxT(".bmp");
		if (!fo.CheckPath(bitmappath)) {
			wxMessageBox(wxT("Unable to open ") + bitmappath);
			return;
		}
	}

	INXSize tempSize=bitmap.Init(bitmappath);
	rectangle=INXRect(_point.x,_point.y,_point.x+tempSize.cx,_point.y+tempSize.cy);
}

/*
Destroy the dynamically allocated data
  */
ConData::~ConData()
{
UINT i;
for (i=0;i<inputport_num;i++) {
	if (inputport[i]) {
		delete inputport[i];
	}
}
for (i=0;i<outputport_num;i++) 
	delete outputport[i];
for (i=0;i<startport_num;i++) {
	if (startport[i]) {
		delete startport[i];
	}
}
for (i=0;i<finishport_num;i++) 
	delete finishport[i];
for (i=0;i<internalport_num;i++) 
	delete internalport[i];
for (i=1;i<iParamNum; i++)
	delete iconParam[i];
}

/* Function for drawing the icon, including all the child port connections*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::Draw(CDC * theDC) {
	Draw(theDC, false, 0);
}

/* Function for drawing the icon, including all the child port connections*/
void ConData::Draw(CDC * theDC, bool _onlyDrawAnim, int _toggleAnim) {
	INXRect highlightRect;
	CPen highlightpen;
	COLORREF oldTxtColor;
	INXString csValue;

	highlightpen.CreatePen(PS_DASH,1,RGB(0,255,0));

	if (m_iShow) {
		if (!_onlyDrawAnim) {
			highlightRect.TopLeft().x = rectangle.TopLeft().x - 1;
			highlightRect.TopLeft().y = rectangle.TopLeft().y - 1;
			highlightRect.BottomRight().x = rectangle.BottomRight().x + 2;
			highlightRect.BottomRight().y = rectangle.BottomRight().y + 2;
			if (selected) {
				theDC->SelectObject(highlightpen);
				theDC->Rectangle(highlightRect);
			}

			/* example of code used to restore original pen
			CPen* originalPen;
			originalPen = theDC->GetCurrentPen();
			theDC->SelectObject(nullpen);
			theDC->SelectObject(originalPen);
			*/

				//BYTE* bitmapbuf=bitmap;
				//
				//if (theDC!=NULL) {
				//	UINT m_widthDW=0;
				//	INXRect clientRect;
				//	GetClientRect(clientRect);
					
					// Center It
			//@todo
			INXPoint _point(rectangle.TopLeft().x, rectangle.TopLeft().y);
			bitmap.Draw(theDC,_point);
			// define font for constants
			LOGFONT logFont;
			// pitch size is 8
			if (theDC->IsPrinting()) {
				logFont.lfHeight = -MulDiv(8, theDC->GetDeviceCaps(LOGPIXELSY), 432);
			}
			else {
				logFont.lfHeight = -MulDiv(8, theDC->GetDeviceCaps(LOGPIXELSY), 72);
			}
			logFont.lfWidth = 0;
			logFont.lfEscapement = 0;
			logFont.lfOrientation = 0;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfItalic = 0;
			logFont.lfUnderline = 0;
			logFont.lfStrikeOut = 0;
			logFont.lfCharSet = ANSI_CHARSET;
			logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			logFont.lfQuality = PROOF_QUALITY;
			logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
			strcpy_s(logFont.lfFaceName, "Arial");
			CFont font;
			font.CreateFontIndirect(&logFont);
			CFont* oldFont = theDC->SelectObject(&font);
			int oldBkMode = theDC->GetBkMode();
			theDC->SetBkMode(1);	// set to transparent background

			// display constant values
			if (m_csIconType.Find("const_") != -1 || m_csIconType == INT_COMP || m_csIconType == REAL_COMP || m_csIconType == INT_GREATER_THAN ||
				m_csIconType == REAL_GREATER_THAN || m_csIconType == INT_GREATER_THAN_EQUALS || m_csIconType == REAL_GREATER_THAN_EQUALS) {
				if (iconParam[1]->value == "_") {
					csValue = "";
				}
				else {
					csValue = iconParam[1]->value;
				}
				if (theDC->IsPrinting()) {

					theDC->TextOut(rectangle.TopLeft().x+15,rectangle.BottomRight().y+23,(CString&)csValue.Left(13));
				}
				else {
					theDC->TextOut(rectangle.TopLeft().x+15,rectangle.BottomRight().y-23,(CString&)csValue.Left(13));
				}
			}
			else if(m_csIconType == "STATE")
			{
				if (theDC->IsPrinting()) {
					theDC->TextOut(rectangle.TopLeft().x+20,rectangle.BottomRight().y+35,(CString&)iconParam[1]->value.Left(13));
				}
				else {
					theDC->TextOut(rectangle.TopLeft().x+20,rectangle.BottomRight().y-35,(CString&)iconParam[1]->value.Left(13));
				}
			}

			// display description (instance name)
			INXPoint point=rectangle.TopLeft();
			oldTxtColor = theDC->GetTextColor();
			theDC->SetTextColor(RGB(0,0,255));
			// printing uses th MM_LOENGLISH mapping mode
			
			if (theDC->IsPrinting()) {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-15,(CString&)description.Mid(15,15));
				}
			}
			else {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+15,(CString&)description.Mid(15,15));
				}
			}
			theDC->SetTextColor(oldTxtColor);

			// display title
			DrawTitle(theDC);
			theDC->SetBkMode(oldBkMode);
		}	

		for (UINT i=0;i<inputport_num;i++) ((Port*)(inputport[i]))->Draw(theDC, _onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<outputport_num;i++) ((Port*)(outputport[i]))->Draw(theDC, _onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<startport_num;i++) ((Port*)(startport[i]))->Draw(theDC, _onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<finishport_num;i++) ((Port*)(finishport[i]))->Draw(theDC, _onlyDrawAnim, _toggleAnim);
		
	}
}
#endif
void ConData::DrawGL(){
	DrawGL(false, 0);
}
void ConData::DrawGL(bool _onlyDrawAnim, int _toggleAnim){

	INXRect highlightRect;
	//CPen highlightpen;
	//COLORREF oldTxtColor;
	INXString csValue;
	//highlightpen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if (m_iShow) {
		if (!_onlyDrawAnim) {
			/*highlightRect.TopLeft().x = rectangle.TopLeft().x - 1;
			highlightRect.TopLeft().y = rectangle.TopLeft().y - 1;
			highlightRect.BottomRight().x = rectangle.BottomRight().x + 2;
			highlightRect.BottomRight().y = rectangle.BottomRight().y + 2;*/
			if (selected) {
				//theDC->SelectObject(highlightpen);
				//theDC->Rectangle(highlightRect);
			}
			/* example of code used to restore original pen
			CPen* originalPen;
			originalPen = theDC->GetCurrentPen();
			theDC->SelectObject(nullpen);
			theDC->SelectObject(originalPen);
			*/
				//BYTE* bitmapbuf=bitmap;
				//
				//if (theDC!=NULL) {
				//	UINT m_widthDW=0;
				//	INXRect clientRect;
				//	GetClientRect(clientRect);
					
					// Center It
			//@todo
			INXPoint _point(rectangle.TopLeft().x, rectangle.TopLeft().y);
			bitmap.DrawGL(_point);
			// define font for constants
			//LOGFONT logFont;
			// pitch size is 8
			/*if (theDC->IsPrinting()) {
				logFont.lfHeight = -MulDiv(8, theDC->GetDeviceCaps(LOGPIXELSY), 432);
			}
			else {
				logFont.lfHeight = -MulDiv(8, theDC->GetDeviceCaps(LOGPIXELSY), 72);
			}
			logFont.lfWidth = 0;
			logFont.lfEscapement = 0;
			logFont.lfOrientation = 0;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfItalic = 0;
			logFont.lfUnderline = 0;
			logFont.lfStrikeOut = 0;
			logFont.lfCharSet = ANSI_CHARSET;
			logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			logFont.lfQuality = PROOF_QUALITY;
			logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
			strcpy_s(logFont.lfFaceName, "Arial");
			CFont font;
			font.CreateFontIndirect(&logFont);
			CFont* oldFont = theDC->SelectObject(&font);
			int oldBkMode = theDC->GetBkMode();
			theDC->SetBkMode(1);	// set to transparent background
*/
/*			// display constant values
			if (m_csIconType.Find("const_") != -1 || m_csIconType == INT_COMP || m_csIconType == REAL_COMP || m_csIconType == INT_GREATER_THAN ||
				m_csIconType == REAL_GREATER_THAN || m_csIconType == INT_GREATER_THAN_EQUALS || m_csIconType == REAL_GREATER_THAN_EQUALS) {
				if (iconParam[1]->value == "_") {
					csValue = "";
				}
				else {
					csValue = iconParam[1]->value;
				}
				if (theDC->IsPrinting()) {

					theDC->TextOut(rectangle.TopLeft().x+15,rectangle.BottomRight().y+23,(CString&)csValue.Left(13));
				}
				else {
					theDC->TextOut(rectangle.TopLeft().x+15,rectangle.BottomRight().y-23,(CString&)csValue.Left(13));
				}
			}
			else if(m_csIconType == "STATE")
			{
				if (theDC->IsPrinting()) {
					theDC->TextOut(rectangle.TopLeft().x+20,rectangle.BottomRight().y+35,(CString&)iconParam[1]->value.Left(13));
				}
				else {
					theDC->TextOut(rectangle.TopLeft().x+20,rectangle.BottomRight().y-35,(CString&)iconParam[1]->value.Left(13));
				}
			}
			*/
			// display description (instance name)
			INXPoint point=rectangle.TopLeft();
			//oldTxtColor = theDC->GetTextColor();
			//theDC->SetTextColor(RGB(0,0,255));
			// printing uses th MM_LOENGLISH mapping mode
			/*
			if (theDC->IsPrinting()) {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-15,(CString&)description.Mid(15,15));
				}
			}
			else {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+15,(CString&)description.Mid(15,15));
				}
			}
			theDC->SetTextColor(oldTxtColor);

			// display title
			DrawTitle(theDC);
			theDC->SetBkMode(oldBkMode);
		*/
		}	
	// Disable drawing ports temporarly
		for (UINT i=0;i<inputport_num;i++) ((Port*)(inputport[i]))->DrawGL(_onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<outputport_num;i++) ((Port*)(outputport[i]))->DrawGL(_onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<startport_num;i++) ((Port*)(startport[i]))->DrawGL(_onlyDrawAnim, _toggleAnim);
		for (UINT i=0;i<finishport_num;i++) ((Port*)(finishport[i]))->DrawGL(_onlyDrawAnim, _toggleAnim);

	}
}

/*
Delete a line connected to an input port.
This function is called when the user clicks on an input port that is already connected
  */
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::DeleteLine(int port, int porttype){ // this isn't very nice, but anyway
	
	if       (porttype == INPUTPORT) inputport[port]->RemoveLine();
	else if  (porttype == STARTPORT) startport[port]->RemoveLine();
    //PD*: change how this is called 
}

#endif
/*
Adds a line (connection) to a input port of the icon
This function accepts a pointer to the icon at the other (far) end of the new connection. It also accepts the 
port identifier for the far icon to make the connection.
  */

int ConData::AddLine(int inPortNum, int inPortType, ConData* othericon,int otherPortNum, int otherPortType){ 	
	INXPoint *Otherend=othericon->GetPortPointPtr(otherPortNum, otherPortType);
	// Check if the port (this icon) is a data input
	if (inPortType == INPUTPORT ) {
		// Check if the icon is already connected to this input (only one allwed for inputs - no need to check for outputs)
		if (inputport[inPortNum]->connected) {
			wxMessageBox(wxT("Cannot connect more than one output to an input"));
			return 0;
		}
		// If OK connect data input using the Port method addline(....)
		inputport[inPortNum]->addLine(Otherend,othericon->identnum,otherPortNum, otherPortType);
	}
	else {
	//Check if the port is a trigger input (start)
		if  (inPortType == STARTPORT ) {
			if (startport[inPortNum]->connected) {
				wxMessageBox(wxT("Cannot connect more than one output to an input"));
				return 0;
			}
			startport[inPortNum]->addLine(Otherend,othericon->identnum,otherPortNum, otherPortType);
		}
	}

	return 1;
}


/*
Sets the connected flag on Output and Finish ports of a component
*/
void ConData::setOutputPortAsConnected(int portNum, int portType){ 	
	if (portType == OUTPUTPORT ) {
		outputport[portNum]->connected = 1;
	} else if  (portType == FINISHPORT ) {
		finishport[portNum]->connected = 1;
	}
}

// This function makes lines drawn by the user orthogonal by adding 2 nodes.
// For data lines nodes are added half way along the line.
// For event lines nodes are added 40% or 60% of the way along the line.
#ifdef OLDLINE
void ConData::AddNodes(int selectedPort, int selectedPortType, ConData* othericon, int otherPortNum, int otherPortType) {
	INXPoint portPoint1, portPoint2, node1, node2;
	/* Put nodes here and check for backward direction */ 
	portPoint2 = othericon->inputport[otherPortNum]->P;
	portPoint1 = outputport[selectedPort]->P;
	if (otherPortType == INPUTPORT) { 
		/* Adds a dogleg at the midpoint */
		node1.x = portPoint1.x + ((portPoint2.x - portPoint1.x)/2);
		node1.y = portPoint1.y;
		node2.x = node1.x;
		node2.y = portPoint2.y;
		othericon->inputport[otherPortNum]->line.addNode(node1);
		othericon->inputport[otherPortNum]->line.addNode(node2);
	}
	else if (selectedPortType == INPUTPORT) {
		node1.x = portPoint2.x + ((portPoint1.x - portPoint2.x)/2);
		node1.y = portPoint2.y;
		node2.x = node1.x;
		node2.y = portPoint1.y;
		inputport[selectedPort]->line.addNode(node1);
		inputport[selectedPort]->line.addNode(node2);
	}
	else if (otherPortType == STARTPORT) {
		portPoint2 = othericon->startport[otherPortNum]->P;
		portPoint1 = finishport[selectedPort]->P;
		// ensure event lines do not overlap data lines
		if (portPoint2.y > portPoint1.y) {
			node1.x = portPoint1.x + (long)((portPoint2.x - portPoint1.x)*0.6);
		}
		else {
			node1.x = portPoint1.x + (long)((portPoint2.x - portPoint1.x)*0.4);
		}
		node1.y = portPoint1.y;
		node2.x = node1.x;
		node2.y = portPoint2.y;
		othericon->startport[otherPortNum]->line.addNode(node1);
		othericon->startport[otherPortNum]->line.addNode(node2);
	}
	else if (selectedPortType == STARTPORT) {
		portPoint2 = othericon->finishport[otherPortNum]->P;
		portPoint1 = startport[selectedPort]->P;
		// ensure event lines do not overlap data lines
		if (portPoint1.y > portPoint2.y) {
			node1.x = portPoint2.x + (long)((portPoint1.x - portPoint2.x)*0.6);
		}
		else {
			node1.x = portPoint2.x + (long)((portPoint1.x - portPoint2.x)*0.4);
		}
		node1.y = portPoint2.y;
		node2.x = node1.x;
		node2.y = portPoint1.y;
		startport[selectedPort]->line.addNode(node1);
		startport[selectedPort]->line.addNode(node2);
	}
}


#else

void ConData::AddNodes(int selectedPort, int selectedPortType, ConData* othericon, int otherPortNum, int otherPortType) {
	//INXPoint ThisPoint1, OtherPoint2; //, node1, node2;
	/* Put nodes here and check for backward direction */ 
	//Port *portPoint2 = othericon->inputport[otherPortNum]->P;
	//portPoint1 = outputport[selectedPort]->P;
	int y_shift=-50; 
	y_shift =min(othericon->rectangle.top,rectangle.top); // if a loop backward is required go below either block 

	if (otherPortType == INPUTPORT) { 
		/* best guess to go over or under the function block (if it is directed righ to left */
		/* Adds a dogleg at the midpoint */
		othericon->inputport[otherPortNum]->line.AddDogLeg(otherPortNum*2+1,y_shift); // otherPortNum*3 is to stagger the lines
	}
	else if (selectedPortType == INPUTPORT) {
		inputport[selectedPort]->line.AddDogLeg(selectedPort*2+1,y_shift);
	}
	else if (otherPortType == STARTPORT) {
		othericon->startport[otherPortNum]->line.AddDogLeg(otherPortNum*2,y_shift); // dog leg 60% way along
	}
	else if (selectedPortType == STARTPORT) {
		startport[selectedPort]->line.AddDogLeg(selectedPort*2,y_shift);
	}
}
#endif

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::ReRouteAll() {
UINT i;
	for (i=0;i<inputport_num;i++) {
		((Port*)(inputport[i]))->line.AddDogLeg(i*2+1,rectangle.top);
		//if (*portType>-1) return i; //assume not on port returns -1
	}
	/*
	for (i=0;i<outputport_num;i++) {
		*portConnected=((Port*)(outputport[i]))->connected;
		*portType=((Port*)(outputport[i]))->OnPort(point);
		if (*portType>-1) return i;
	}
	*/
	for ( i=0;i<startport_num;i++) {
		((Port*)(startport[i]))->line.AddDogLeg(i*2,rectangle.top);
		//if (*portType>-1) return i;
	}/*
	for (i=0;i<finishport_num;i++) {
		*portConnected=((Port*)(finishport[i]))->connected;
		*portType=((Port*)(finishport[i]))->OnPort(point);
		if (*portType>-1) return i;
	}
	*/

}

/*
This function returns a type code for port data types:
This information is found from the port object and follows the code defined in Port.cpp
  */
int ConData::GetPortDataType(int portNum, int portType) {
	// now select the type of port
	if       (portType == INPUTPORT) return inputport[portNum]->datatype;//+rectangle.TopLeft();
	else if  (portType == STARTPORT) return startport[portNum]->datatype;//+rectangle.TopLeft();
	else if  (portType == OUTPUTPORT) return outputport[portNum]->datatype;//+rectangle.TopLeft();
	else if  (portType == FINISHPORT) return finishport[portNum]->datatype;//+rectangle.TopLeft();
	return 999; //this is an error condition
}


/* 

  Checks the data types of near and far ports. It is used to check before a new connection is made

  */
int  ConData::CheckDatatypes(int portIn, int portInType, ConData*iconOut,int portOut,int portOutType) {
	
	if  (GetPortDataType(portIn, portInType)!=iconOut->GetPortDataType(portOut,portOutType)) {
		wxMessageBox("Cannot connect different data types");
		return -1;	
	}
	return 1;
}

/*
Checks a screen x,y coordinate to see if it is near a port, by iterating through all the icons and all 
the ports locations within. Not a very efficient algorithm, but appears to be adequate for current requirements
A speed up version would just check the rectangle of the icons first and only check individual port locations 
once a candidate icon has been found
  */
#endif
int ConData::OnPort(INXPoint point,int * portType,int *portConnected) {
	//int portret=0;
	if (m_iShow) {
		UINT i;
		//point=point;//-rectangle.TopLeft();
	for (i=0;i<inputport_num;i++) {
		*portConnected=((Port*)(inputport[i]))->connected;
		*portType=((Port*)(inputport[i]))->OnPort(point);
		if (*portType>-1) return i; //assume not on port returns -1
	}
	for (i=0;i<outputport_num;i++) {
		*portConnected=((Port*)(outputport[i]))->connected;
		*portType=((Port*)(outputport[i]))->OnPort(point);
		if (*portType>-1) return i;
	}
	for ( i=0;i<startport_num;i++) {
		*portConnected=((Port*)(startport[i]))->connected;
		*portType=((Port*)(startport[i]))->OnPort(point);
		if (*portType>-1) return i;
	}
	for (i=0;i<finishport_num;i++) {
		*portConnected=((Port*)(finishport[i]))->connected;
		*portType=((Port*)(finishport[i]))->OnPort(point);
		if (*portType>-1) return i;
	}
  }

return -1;

}

/*
Returns a pointer to the coordinate object of a port
  */
INXPoint *ConData::GetPortPointPtr(int port, int portType){
	// now select the type of port
	if       (portType == INPUTPORT) return &inputport[port]->P;//+rectangle.TopLeft();
	else if  (portType == STARTPORT) return &startport[port]->P;//+rectangle.TopLeft();
	else if  ((portType == OUTPUTPORT)) return &outputport[port]->P;//+rectangle.TopLeft();
	else if  ((portType == FINISHPORT)) return &finishport[port]->P;//+rectangle.TopLeft();
	
	return NULL; //this is an error condition
}


/*
Returns the coordinates of a port as a new INXPoint object
  */
INXPoint ConData::GetPortPoint(int port, int portType){
	// now select the type of port
	if       (portType == INPUTPORT) return inputport[port]->P;//+rectangle.TopLeft();
	else if  (portType == STARTPORT) return startport[port]->P;//+rectangle.TopLeft();
	else if  (portType == OUTPUTPORT) return outputport[port]->P;//+rectangle.TopLeft();
	else if  (portType == FINISHPORT) return finishport[port]->P;//+rectangle.TopLeft();
	return INXPoint(0,0); //this is an error condition
}

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
/*
Allows the positions of an Icon to be moved by changing the icon location, and also updating 
all the children (i.e. ports and subsequently any line end points)
  */
void ConData::RenewPosition(INXPoint newpoint, INXPoint oldpoint) {
	INXPoint offsetpoint = oldpoint - newpoint;
	
	// only reroute lines if component has been moved
	if ((offsetpoint.x == 0) && (offsetpoint.y == 0)) {
		return;
	}

	rectangle.OffsetRect(newpoint - oldpoint);

	UINT i;
	for (i=0;i<inputport_num;i++) {
		((Port*)(inputport[i]))->Move(offsetpoint);
	}
	for (i=0;i<outputport_num;i++) {
		((Port*)(outputport[i]))->Move(offsetpoint);
	}

	for ( i=0;i<startport_num;i++) {
		((Port*)(startport[i]))->Move(offsetpoint);
	}
	for (i=0;i<finishport_num;i++) {
		((Port*)(finishport[i]))->Move(offsetpoint);
	}
}


// Legacy code for displaying text data on icons - code may come in useful again later.
void ConData::DrawFixed(CDC* theDC) {
//	if ((theDC!=NULL)) {
//	if (((fixeddata!="")&&(fixeddata!="_"))) {

		// loop through all port here
//					INXPoint point=rectangle.TopLeft();
//					theDC->SetTextColor(RGB(255,255,255));
//					theDC->SetBkColor(RGB(0,255,0));
//					theDC->TextOut(point.x+input[0].x-60,point.y+input[0].y-7,"               ");
//					theDC->TextOut(point.x+input[0].x-60,point.y+input[0].y-7,fixeddata.Left(10));
//					theDC->SetBkColor(RGB(255,255,255));
//	}
//	}
}

/* 
Displays any textual description data below the icon 
*/
void ConData::DrawDescription(CDC* theDC) {
	
	COLORREF oldTxtColor;

	// create a font for printing
	//if (theDC->IsPrinting()){
		LOGFONT logFont;
		logFont.lfHeight = (long)0.1;
		logFont.lfHeight = -MulDiv(8, theDC->GetDeviceCaps(LOGPIXELSY), 72);
		logFont.lfWidth = 0;
		logFont.lfEscapement = 0;
		logFont.lfOrientation = 0;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfItalic = 0;
		logFont.lfUnderline = 0;
		logFont.lfStrikeOut = 0;
		logFont.lfCharSet = ANSI_CHARSET;
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = PROOF_QUALITY;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		strcpy_s(logFont.lfFaceName, "Arial");
		CFont font;
		font.CreateFontIndirect(&logFont);
		CFont* oldFont = theDC->SelectObject(&font);	
	//}

	if ((theDC!=NULL)) {
		//if ((description!="")&&(description!="_")) {
			INXPoint point=rectangle.TopLeft();
			oldTxtColor = theDC->GetTextColor();
			// draws a line from text to icon
			//theDC->MoveTo(rectangle.BottomRight().x-35,rectangle.BottomRight().y-10);
			//theDC->LineTo(rectangle.BottomRight().x-25,rectangle.BottomRight().y+2);
			theDC->SetTextColor(RGB(0,0,255));
			// printing uses th MM_LOENGLISH mapping mode
			if (theDC->IsPrinting()) {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y-15,(CString&)description.Mid(15,15));
				}
			}
			else  {
				theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+1,(CString&)description.Left(15));
				if (description.GetLength()>15) {
					theDC->TextOut(rectangle.TopLeft().x+7,rectangle.BottomRight().y+15,(CString&)description.Mid(15,15));
				}
			}
			theDC->SetTextColor(oldTxtColor);
		//}
		//if (i==moveselected) {
		//theDC->SelectObject(greenpen);
		//	theDC->FrameRect(rectangle,&greenbrush);
		//}
	}
}	

/* 
 * Displays icon title, on icon, at bottom centre
 */
#endif
void ConData::DrawTitle() {
	
	//COLORREF oldTxtColor;
#ifdef _EXCLUDE_DC_
	if ((theDC!=NULL)) {

	// create a font for printing
	//if (theDC->IsPrinting()){
		LOGFONT logFont;
		logFont.lfHeight = (long)0.1;
		logFont.lfHeight = -MulDiv(12, theDC->GetDeviceCaps(LOGPIXELSY), 72); // font size
		logFont.lfWidth = 0;
		logFont.lfEscapement = 0;
		logFont.lfOrientation = 0;
		logFont.lfWeight = FW_BOLD;
		logFont.lfItalic = 0;
		logFont.lfUnderline = 0;
		logFont.lfStrikeOut = 0;
		logFont.lfCharSet = ANSI_CHARSET;
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = PROOF_QUALITY;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		strcpy_s(logFont.lfFaceName, "Arial");


		//@todo - doesn't restore font back to previous this call
		CFont font;
		font.CreateFontIndirect(&logFont);
		CFont* oldFont = theDC->SelectObject(&font);

		INXPoint point=rectangle.TopLeft();
		oldTxtColor = theDC->GetTextColor();



/*
		//@todo - only if the display name is set in the CDF, display at bottom of FB, raised as issue #526
		//if...
//		INXString cropDesc = className.Left(10); // limit name to n chrs
		INXString cropDesc = className;
		double nChrDisplaySize = 1.65; // factor to account for font size - for pt8=1, pt12~1.65
		int nDescLen = cropDesc.GetLength();
		int nPadding = 25;	// combined left and right padding for text
		double nStrOffset;

		// determine display width of string using LucidString method
		CLucidString clsCropDescript;
		clsCropDescript = cropDesc;
		nStrOffset = 0;
		for (int i=0; i<nDescLen; i++) {
			nStrOffset = nStrOffset + (clsCropDescript.GetWidthAt(i) * nChrDisplaySize);
			if (nStrOffset > (rectangle.Width() - nPadding)) {
				cropDesc = cropDesc.Left(i);
				break;
			}
		}

		if (theDC->IsPrinting()) {
			theDC->TextOut(rectangle.TopLeft().x+15, rectangle.BottomRight().y+23, cropDesc);
		} else {
			theDC->TextOut(rectangle.TopLeft().x + ((int) rectangle.Width() / 2) - ((int) nStrOffset / 2), rectangle.BottomRight().y-26, cropDesc);
		}
*/
		theDC->SetTextColor(oldTxtColor);
	}//exclude cDC from this function
#endif
}	

/*
Function for loading an icon's attributed from an ascii  file handle
*: This may be replaced by making the object streamable so that the attributes can be stored in raw binary.
This will be a much better way of ensuring consistency beween saved and loaded data.
The current ASCII method requires boring matching of the save and load ascii data handling routines, which will be avoided if all 
the icons, ports, and line objects are made streamable.
  */

int ConData::Load(istream* file)
{
	//INXPoint point=rectangle.TopLeft();
	char temp[1024];
	//uniqueidgenerator=1; //reset the global static id counter 
//	*file >> temp; //read the BEGIN_CONDATA MARKER
	*file >> temp;
	m_csIconType = (INXString)temp;
	if(m_csIconType == "") return 1; //file name empty
	*file >> temp;
	m_csBlockName = temp;
	if (m_csBlockName == "_") m_csBlockName.Append("");
	//init(type, block, INXPoint(0,0), 1); //PROBLEMHERE 1
	initBmp(INXPoint(0,0));
	*file >> identnum; //the counter will get incremented by the above and hence duplicates will be avoided.
	*file >> instNum;
	*file >> m_iUserDefined;
	*file >> selected;
	file->ignore(1,'\n');
	file->getline(temp,1023);
	description = temp;
	file->getline(temp,1023);
	optionstring = temp;
	file->getline(temp,1023);
	longDesc = temp;
	*file >> temp;
	className = temp;
	file->ignore(1,'\n');
	file->getline(temp,1023);
	//*file >> temp;
	hierarchyName = temp;
	*file >> rectangle.TopLeft().x>>rectangle.TopLeft().y>> rectangle.BottomRight().x>>rectangle.BottomRight().y;
	*file >> m_iShow;
	*file >> showdescript;
	inputport_num=0;outputport_num=0;startport_num=0;finishport_num=0;internalport_num=0;iParamNum=1;
	do {
		temp[0]=0;//incase filehandle fails
		*file >> temp;
		if (strcmp(temp,"parameter")==0) {
			iconParam[iParamNum]=new Parameter;
			iconParam[iParamNum]->Load(file);
			iParamNum++;
		}
		if (strcmp(temp,"inputport")==0) {
			inputport[inputport_num]=new Port(m_iUserDefined);
			inputport[inputport_num]->Load(file);inputport_num++;}
		if (strcmp(temp,"outputport")==0) {
			outputport[outputport_num]=new Port(m_iUserDefined);
			outputport[outputport_num]->Load(file);outputport_num++;}
		if (strcmp(temp,"startport")==0) {
			startport[startport_num]=new Port(m_iUserDefined);
			startport[startport_num]->Load(file);startport_num++;}
		if (strcmp(temp,"finishport")==0) {
			finishport[finishport_num]=new Port(m_iUserDefined);
			finishport[finishport_num]->Load(file);finishport_num++;}	
		if (strcmp(temp,"internalport")==0) {
			internalport[internalport_num]=new Port(m_iUserDefined);
			internalport[internalport_num]->Load(file);internalport_num++;}	
	} while (strcmp(temp,"END_BLOCK")&&file->good());
	
	if (description==wxT("_")) description.Append(wxT(""));  //put back to normal
	if (optionstring==wxT("_")) optionstring.Append(wxT(""));
	if (longDesc==wxT("_")) longDesc.Append(wxT(""));
	if (hierarchyName==wxT("_")) hierarchyName.Append(wxT(""));


//	repositionVerticalPorts();
	return 0;
}


/*
*: See above
  */
void ConData::Save(ostream* file)
{
	if (description==wxT("")) description.Append(wxT("_"));	//make sure no empty bits (spaces)
	if (optionstring==wxT("")) optionstring.Append(wxT("_"));
	if (longDesc==wxT("")) longDesc.Append(wxT("_"));
	if (m_csBlockName==wxT("")) m_csBlockName.Append(wxT("_"));
	if (hierarchyName==wxT("")) hierarchyName.Append(wxT("_"));
	*file << endl<<(char*)(m_csIconType)<<endl;
	*file << (char*)(m_csBlockName) << endl;
	// Only save selected when doing a copy or cut
	if (saveSelectedFlag) {
		*file << identnum<<"\t"<<instNum<<"\t"<<m_iUserDefined<<"\t"<<selected<<endl;
	}
	else {
		*file << identnum<<"\t"<<instNum<<"\t"<<m_iUserDefined<<"\t"<<0<<endl;
	}
	*file << (char*)(description)<<endl;
	*file << (char*)optionstring<<endl;
	*file << (char*)longDesc<<endl;
	*file << (char*)className<<endl;
	*file << (char*)hierarchyName<<endl;
	*file << rectangle.TopLeft().x<<"\t"<<rectangle.TopLeft().y<<"\t"<<rectangle.BottomRight().x<<"\t"<<rectangle.BottomRight().y<<endl;
	*file <<m_iShow<<"\t"<<showdescript<<endl;
	
	UINT i;
	for (i=1; i<iParamNum;i++) {*file << "parameter" << endl; iconParam[i]->Save(file);}
	for (i=0;i<inputport_num;i++) {*file << "inputport"<<endl;inputport[i]->Save(file);}
	for (i=0;i<outputport_num;i++) {*file << "outputport"   <<endl;outputport[i]->Save(file);}
	for (i=0;i<startport_num;i++) {*file << "startport"     <<endl;startport[i]->Save(file);}
	for (i=0;i<finishport_num;i++) {*file << "finishport"   <<endl;finishport[i]->Save(file);}
	for (i=0;i<internalport_num;i++) {*file << "internalport"   <<endl;internalport[i]->Save(file);}
	*file << "END_BLOCK"<<endl;
	if (description==wxT("_")) description.Append(wxT(""));  //put back to normal
	if (optionstring==wxT("_")) optionstring.Append(wxT(""));
	if (longDesc==wxT("_")) longDesc.Append(wxT(""));
	if (m_csBlockName == wxT("_")) m_csBlockName.Append(wxT(""));
	if (hierarchyName == wxT("_")) hierarchyName.Append(wxT(""));
}


#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE

void ConData::readFromIDFFile(INXString filepath, INXPoint point) {
	INXString portDesc, csPortType, xCoord, yCoord, csDataType, csAtomicFlag, csPortOrientation, csMandatoryFlag;
	INXString paramName, minRange, maxRange, defaultVal, paramDesc, csUserDef, enumVal, enumLabel;
	vector<INXString> vPortKeysVec;
	INXString port = "Port", param = "Parameter";
	INXString enumValKey = "", enumLabelKey = "";
	INXString portSection = "";
	INXString paramSection = "";
	int iEquals = 0, iPortNum = 1, iEnumNum = 0;
	char cPortNum[8], cParamNum[8], cEnumNum[8];
	int iPortType = -1, iDataType = -1, iAtomicFlag = 1, iMandatoryFlag = 0;
	UINT x,y;
	UINT funcInd, iFuncArg;
	bool portFlag = FALSE;
	bool bPortVertical = FALSE;

	// Extract Icon attributes from IDF file
	className = GetIniValue(SECTIONCLASS, KEYCLASS, filepath);
	//description = GetIniValue(SECTIONSDESC, KEYDESC, filepath);
	longDesc = GetIniValue(SECTIONLDESC, KEYDESC, filepath);
	csUserDef = GetIniValue(SECTIONICON, KEYUSERDEF, filepath);
	
	// Find out whether it is an encapsulated block
	if (csUserDef=="1") {
		m_iUserDefined = 1;
	}
	else {
		m_iUserDefined = 0;
	}

	// Construct which port to extract
	_itoa_s(iPortNum, cPortNum, 10);
	portSection = port + cPortNum;

	// Use the port type key to check that a port exists.
	csPortType = GetIniValue(portSection, KEYPORTTYPE, filepath);
	
	// while there are still ports 
	// Extract port data
	while (csPortType != "") {
		iAtomicFlag = 1;
		iMandatoryFlag = 0;
		
		bPortVertical = FALSE;

		// Initialise the function array index for the new port
		// and create new instances of funcName and funcArg arrays
		funcInd = 0;
		funcName = new INXObjArray<INXString>;
		funcArg = new INXObjArray<unsigned int>;
		
		vPortKeysVec.clear();
		GetIniKeys(portSection, filepath, vPortKeysVec);
		// for each port key check if it matches one of the known keys and get the
		// data for that key. If it doesn't match then it must be a function name.
		for (int i=0; i<vPortKeysVec.size(); i++) {
			if (vPortKeysVec[i].Find(KEYDESC) != -1) {
				portDesc = GetIniValue(portSection, KEYDESC, filepath);
				//wxMessageBox(descKey + "=" + portDesc);
			}
			else if (vPortKeysVec[i].Find(KEYPORTTYPE) != -1) {
				csPortType = GetIniValue(portSection, KEYPORTTYPE, filepath);
				//convert INXString to int
				iPortType = portType2Int(csPortType);
			}
			else if (vPortKeysVec[i].Find(KEYXCOORD) != -1) {
				xCoord = GetIniValue(portSection, KEYXCOORD, filepath);
				x = atoi(xCoord);
			}
			else if (vPortKeysVec[i].Find(KEYYCOORD) != -1) {
				yCoord = GetIniValue(portSection, KEYYCOORD, filepath);
				y = atoi(yCoord);
			}
			else if (vPortKeysVec[i].Find(KEYDATATYPE) != -1) {
				csDataType = GetIniValue(portSection, KEYDATATYPE, filepath);
				iDataType = dataType2Int(csDataType);
			}
			else if (vPortKeysVec[i].Find(KEYATOMICFLAG) != -1) {
				csAtomicFlag = GetIniValue(portSection, KEYATOMICFLAG, filepath);
				iAtomicFlag = atoi(csAtomicFlag);
			}
			else if (vPortKeysVec[i].Find(KEYPORTVERT) != -1) {
				csPortOrientation = GetIniValue(portSection, KEYPORTVERT, filepath);
				bPortVertical = (csPortOrientation == "1");
			}
			else if (vPortKeysVec[i].Find(KEYMANDATORYFLAG) != -1) {
				csMandatoryFlag = GetIniValue(portSection, KEYMANDATORYFLAG, filepath);
				iMandatoryFlag = atoi(csMandatoryFlag);
			}
			else {
				funcName->SetAtGrow(funcInd, vPortKeysVec[i]);
				//funcName->Add(funcNameKey);
				iFuncArg = atoi(GetIniValue(portSection, vPortKeysVec[i], filepath));
				funcArg->SetAtGrow(funcInd, (UINT)iFuncArg);
				//funcArg->Add((UINT)iFuncArg);
				funcInd++;
			}
		}

		// create instance of port
		if (iPortType==INPUTPORT) {
			inputport[inputport_num] = new Port(INXPoint(x,y)+point, inputport_num, iDataType, iPortType, portDesc, funcName, funcArg, 1, bPortVertical, m_iUserDefined, iMandatoryFlag);
			inputport_num++;
		}
		if (iPortType==OUTPUTPORT) {
			outputport[outputport_num] = new Port(INXPoint(x,y)+point, outputport_num, iDataType, iPortType, portDesc, funcName, funcArg, 1, bPortVertical, m_iUserDefined, iMandatoryFlag);
			outputport_num++;
		}
		if (iPortType==STARTPORT) {
			startport[startport_num] = new Port(INXPoint(x,y)+point, startport_num, iDataType, iPortType, portDesc, funcName, funcArg, iAtomicFlag, bPortVertical, m_iUserDefined, iMandatoryFlag);
			startport_num++;
		}
		if (iPortType==FINISHPORT) {
			finishport[finishport_num] = new Port(INXPoint(x,y)+point, finishport_num, iDataType, iPortType, portDesc, funcName, funcArg, 1, bPortVertical, m_iUserDefined, iMandatoryFlag);
			finishport_num++;
		}
		if (iPortType==INTERNALPORT) {
			internalport[internalport_num] = new Port(internalport_num, iPortType, funcName, funcArg, iAtomicFlag);
			internalport_num++;
		}

		// Extract next port if it exists
		iPortNum++;
		_itoa_s(iPortNum, cPortNum, 10);
		portSection = port + cPortNum;
		//bPortVertical = FALSE;
		//Get next port if it exists
		csPortType = GetIniValue(portSection, KEYPORTTYPE, filepath);
	
	}// end of while for port 
	

	// Extract parameter data
	// Construct which parameter to extract
	_itoa_s(iParamNum, cParamNum, 10);
	paramSection = param + cParamNum;

	// Use the paramName keyword to check that a port exists.
	paramName = GetIniValue(paramSection, KEYNAME, filepath);

	// while there are still parameters 
	while (paramName != "") {
		csDataType = GetIniValue(paramSection, KEYDATATYPE, filepath);
		iDataType = dataType2Int(csDataType);
		minRange = GetIniValue(paramSection, KEYMINRANGE, filepath);
		maxRange = GetIniValue(paramSection, KEYMAXRANGE, filepath);
		// Extract enumerated values and labels and add to an array
		INXObjArray<INXString>* enumValArr = new INXObjArray<INXString>;
		INXObjArray<INXString>* enumLabelArr = new INXObjArray<INXString>;
		if (iDataType == 1) {
			// Get values
			_itoa_s(iEnumNum, cEnumNum, 10);
			enumValKey = KEYENUMVAL + cEnumNum;
			enumVal = GetIniValue(paramSection, enumValKey, filepath);
			while (enumVal != "") {
				enumValArr->Add(enumVal);
				// Extract next value if it exists
				iEnumNum++;
			    _itoa_s(iEnumNum, cEnumNum, 10);
				enumValKey = KEYENUMVAL + cEnumNum;
				enumVal = GetIniValue(paramSection, enumValKey, filepath);
			}
			// Get Labels
			iEnumNum = 0;
			_itoa_s(iEnumNum, cEnumNum, 10);
			enumLabelKey = KEYENUMLABEL + cEnumNum;
			enumLabel = GetIniValue(paramSection, enumLabelKey, filepath);
			while (enumLabel != "") {
				enumLabelArr->Add(enumLabel);
				// Extract next value if it exists
				iEnumNum++;
			    _itoa_s(iEnumNum, cEnumNum, 10);
				enumLabelKey = KEYENUMLABEL + cEnumNum;
				enumLabel = GetIniValue(paramSection, enumLabelKey, filepath);
			}
		}
		defaultVal = GetIniValue(paramSection, KEYDEFAULT, filepath);
		paramDesc = GetIniValue(paramSection, KEYDESC, filepath);

		// create instance of Parameter
		iconParam[iParamNum] = new Parameter(paramName, iDataType, minRange, maxRange, defaultVal, paramDesc, enumValArr, enumLabelArr);

		// Extract next parameter if it exists
		iParamNum++;
		_itoa_s(iParamNum, cParamNum, 10);
		paramSection = param + cParamNum;
	
		paramName = GetIniValue(paramSection, KEYNAME, filepath);

	}
}

/* @todo move code to utils package
 takes a filename and removes the extension - everything after and including the first "." encountered
*/
#endif
static void removeExtensionFromFilename(char * trimmedStr, const char *toTrimStr) {
	int myStrLen = 0, lenA = 0, lenB = 0;
	const char * strB = NULL;

	lenA = strlen(toTrimStr);
	strB = strstr(toTrimStr,".");
	lenB = strlen(strB);
	myStrLen = lenA - lenB;
	strncpy(trimmedStr, toTrimStr, myStrLen);
	trimmedStr[myStrLen] = '\0';
}



struct definedFunctions {
	char * funcName;
	int maxInputArg;
	int maxOutputArg;
	int maxFinishArg;
};


/*
	build a list of unique function names and record max of argument number for each port type
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::growFunctionArray(struct definedFunctions *funcs, const char *funcName, int iPortType, int iFuncArg){
	bool isNew = true;
	int i=0;

	// if the name of the function has already been recorded, then update the number of arguments if the current argument number is greater than that encountered before
	while(funcs[i].funcName) {
		if (strcmp(funcs[i].funcName, funcName) == 0) {
			if (iPortType==INPUTPORT) {
				if (funcs[i].maxInputArg < iFuncArg) {
					funcs[i].maxInputArg = iFuncArg;
				}
			} else if (iPortType==OUTPUTPORT) {
				if (funcs[i].maxOutputArg < iFuncArg) {
					funcs[i].maxOutputArg = iFuncArg;
				}
			} else if (iPortType==FINISHPORT) {
				if (funcs[i].maxFinishArg < iFuncArg) {
					funcs[i].maxFinishArg = iFuncArg;
				}
			}
			isNew = false;
			break;
		}
		i++;
	}
	// else it is a new function, so add its name to the array and record the argument number
	if (isNew) {
		funcs[i].funcName = (char *) malloc(strlen(funcName)+2); // need the terminator too?
		strcpy(funcs[i].funcName,funcName);
		if (iPortType==INPUTPORT) {
			funcs[i].maxInputArg = iFuncArg;
		} else if (iPortType==OUTPUTPORT) {
			funcs[i].maxOutputArg = iFuncArg;
		} else if (iPortType==FINISHPORT) {
			funcs[i].maxFinishArg = iFuncArg;
		}
	}
}

/*
	need to correctly assign funcArg numbers for each function to the ports
	where for a given function, input ports are numbered first, then output, then finish
*/
void ConData::updateFunctionArg(struct definedFunctions *funcs){
	int i = 0, j = 0, k = 0;

	for (i=0; i<outputport_num; i++) {
		// offset the arg number of the output ports by the max argument number of the input ports for a given function
		j = 0;
		while(funcs[j].funcName) {
			INXString temp(funcs[j].funcName);
			for (int k=0; k<outputport[i]->funcName->GetSize(); k++) {
				if (strcmp(temp, outputport[i]->funcName->GetAt(k)) == 0) {
					outputport[i]->funcArg->SetAt(k, outputport[i]->funcArg->GetAt(k) + funcs[j].maxInputArg);			
				}
			}
			j++;
		}
	}
	for (i=0; i<finishport_num; i++) {
		// offset the arg number of the finish ports by the sum of the max argument number of the input and output ports for a given function
		j = 0;
		while(funcs[j].funcName) {
			INXString temp(funcs[j].funcName);
			for (int k=0; k<finishport[i]->funcName->GetSize(); k++) {
				if (strcmp(temp, finishport[i]->funcName->GetAt(k)) == 0) {
					finishport[i]->funcArg->SetAt(k, finishport[i]->funcArg->GetAt(k) + funcs[j].maxInputArg + funcs[j].maxOutputArg);			
				}
			}
			j++;
		}
	}
}


void ConData::readFromCDFFile(INXString filepath, INXPoint point) {
	INXString csDataType;
	INXString portDesc;
	INXString paramName, minRange, maxRange, defaultVal, paramDesc, csUserDef;
	int iPortType = -1, iDataType = -1, iAtomicFlag = 1, iMandatoryFlag = 0;
	UINT x,y;
	UINT funcInd, iFuncArg;
	bool portFlag = FALSE;
	bool bPortVertical = FALSE;
	int ind = 0;
	int numInArgs = 0;
	int i = 0, j = 0, k = 0;

	struct definedFunctions funcs[MAXFUNCS] = {0};
//	funcs = (struct definedFunctions *) malloc(sizeof(funcs));

	// @todo "user defined" - not implemented as not generated by eclipse pluggin
	//csUserDef = GetIniValue(SECTIONICON, KEYUSERDEF, filepath);
	//
	//// Find out whether it is an encapsulated block
	//if (csUserDef=="1") {
	//	m_iUserDefined = 1;
	//}
	//else {
	//	m_iUserDefined = 0;
	//}


	char *name = NULL, * value = NULL, * cTemp = NULL;
	xmlTextReaderPtr reader;
	int ret = 0;
	int len;
	char trimmedStr[256];

    reader = xmlNewTextReaderFilename(filepath);
    if (reader != NULL) {

		// read next node
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
			name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!

			// if this is a node end type, then skip this node
			if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
				ret = xmlTextReaderRead(reader);
				continue;
			}
			// if this is a #text type in an unexpected place, then skip this node
			if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
				ret = xmlTextReaderRead(reader);
				continue;
			}


			if ((strcmp(NODE_ICON_ELEMENT, name) == 0)) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);

				// trim of .bmp - it was implied in old IDF format so will remain implied here
				removeExtensionFromFilename(trimmedStr,value);
				m_csIconType = trimmedStr;
			} else if ((strcmp(NODE_CLASS_ELEMENT, name) == 0)) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);

				className = value;
			// @todo module tag not used
			//} else if ((strcmp(NODE_MODULE_ELEMENT, name) == 0)) {
			//	ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
			//	value = (char*) xmlTextReaderConstValue(reader);

			//	description = value;
			// @todo description tag not used
			//} else if ((strcmp(NODE_SDESC_ELEMENT, name) == 0)) {
			//	ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
			//	value = (char*) xmlTextReaderConstValue(reader);

			//	description = value;
			} else if ((strcmp(NODE_LDESC_ELEMENT, name) == 0)) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);

				longDesc = value;
			} else if ((strcmp(NODE_PARAM_ELEMENT, name) == 0)) {
				INXObjArray<INXString>* enumValArr = new INXObjArray<INXString>;
				INXObjArray<INXString>* enumLabelArr = new INXObjArray<INXString>;

				// read next node
				ret = xmlTextReaderRead(reader);

				while (ret == 1) {
					
					// DEBUG ONLY
//					int temp = xmlTextReaderNodeType(reader);

					name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!

					// if this is a node end type, then skip this node
					if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
						if ((strcmp(NODE_PARAM_ELEMENT, name) == 0)) {
							break;
						} else {
							ret = xmlTextReaderRead(reader);
							continue;
						}
					}
					// if this is a #text type in an unexpected place, then skip this node
					if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
						ret = xmlTextReaderRead(reader);
						continue;
					}

					if ((strcmp(NODE_PARAM_NAME_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						paramName = value;
					} else if ((strcmp(NODE_PARAM_DATATYPE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						csDataType = value;
						iDataType = dataType2Int(csDataType);
					} else if ((strcmp(NODE_PARAM_DEFAULTVALUE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						defaultVal = value;
					} else if ((strcmp(NODE_PARAM_MINVALUE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						minRange = value;
					} else if ((strcmp(NODE_PARAM_MAXVALUE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						maxRange = value;
					} else if ((strcmp(NODE_PARAM_DESCRIPTION_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						paramDesc = value;
					} else if ((strcmp(NODE_PARAM_DISPLAYONWIDGET_ELEMENT, name) == 0)) {

						// read next node
						ret = xmlTextReaderRead(reader);

						while (ret == 1) {
							name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!

							// if this is a node end type, then skip this node
							if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
								if ((strcmp(NODE_PARAM_DISPLAYONWIDGET_ELEMENT, name) == 0)) {
									break;
								} else {
									ret = xmlTextReaderRead(reader);
									continue;
								}
							}
							// if this is a #text type in an unexpected place, then skip this node
							int temp = xmlTextReaderNodeType(reader);
							if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
								ret = xmlTextReaderRead(reader);
								continue;
							}


							// @todo display coords on widget not implemented yet
//							if ((strcmp(NODE_PARAM_DISPLAYONWIDGET_X_ELEMENT, name) == 0)) {
//								ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
//								value = (char*) xmlTextReaderConstValue(reader);
//								 = atoi(value);
//							} else if ((strcmp(NODE_PARAM_DISPLAYONWIDGET_Y_ELEMENT, name) == 0)) {
//								ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
//								value = (char*) xmlTextReaderConstValue(reader);
//								 = atoi(value);
//							}


							// read next node
							ret = xmlTextReaderRead(reader);
						}

					} else if ((strcmp(NODE_PARAM_LISTPLACE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						ind = atoi(value);
					// @todo argument placement not implemented yet
//					} else if ((strcmp(NODE_PARAM_ARGPLACE_ELEMENT, name) == 0)) {
//						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
//						value = (char*) xmlTextReaderConstValue(reader);
//						 = value;
					} 

					// read next node
					ret = xmlTextReaderRead(reader);
				}

				// create instance of Parameter
				iconParam[ind] = new Parameter(paramName, iDataType, minRange, maxRange, defaultVal, paramDesc, enumValArr, enumLabelArr);
				// note - this code ensures that the array index is pointing to the last empty entry, regardless of the order that
				// the parameters are presented
				// seems to be necessary for the parameters to be stored correctly
				ind++;
				if (ind > iParamNum) {
					iParamNum = ind;
				}

			} else if ((strcmp(NODE_PORT_ELEMENT, name) == 0)) {
				iAtomicFlag = 1;				
				iMandatoryFlag = 0;				
				bPortVertical = FALSE;

				// Initialise the function array index for the new port
				// and create new instances of funcName and funcArg arrays
				funcInd = 0;
				funcName = new INXObjArray<INXString>;
				funcArg = new INXObjArray<unsigned int>;

				// read next node
				ret = xmlTextReaderRead(reader);

				while (ret == 1) {
					name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!

					// if this is a node end type, then skip this node
					if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
						if ((strcmp(NODE_PORT_ELEMENT, name) == 0)) {
							break;
						} else {
							ret = xmlTextReaderRead(reader);
							continue;
						}
					}
					// if this is a #text type in an unexpected place, then skip this node
					int temp = xmlTextReaderNodeType(reader);
					if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
						ret = xmlTextReaderRead(reader);
						continue;
					}

					if ((strcmp(NODE_PORT_PORTTYPE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						iPortType = portType2Int(value);
					} else if ((strcmp(NODE_PORT_DATATYPE_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						iDataType = dataType2Int(value);
					} else if ((strcmp(NODE_PORT_DESCRIPTION_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);

						// strip out any text in square brackets - this is a unique identifier that used by ICB but not needed in IAB
						//note - we are cheating, just assuming that square brackets are at end of string and only keeping chars before '['
						cTemp = strstr(value,"[");
						if (cTemp) {
							len = strlen(cTemp);
							value[strlen(value) - len] = '\0';
						}
						portDesc = value;
					} else if ((strcmp(NODE_PORT_XCOORD_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						x = atoi(value);
					} else if ((strcmp(NODE_PORT_YCOORD_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						y = atoi(value);
					} else if ((strcmp(NODE_PORT_FUNCTIONNAME_ELEMENT, name) == 0)) {
						value = (char*) xmlTextReaderGetAttribute(reader, (const xmlChar *)"argument");
						iFuncArg = atoi(value);
						funcArg->SetAtGrow(funcInd, (UINT)iFuncArg);

						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						funcName->SetAtGrow(funcInd, (LPCTSTR) value);

						growFunctionArray(funcs, value, iPortType, iFuncArg);
						funcInd++;
					// @todo wcet not implemented yet
//					} else if ((strcmp(NODE_PORT_WCET_ELEMENT, name) == 0)) {
//						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
//						value = (char*) xmlTextReaderConstValue(reader);
//						= value;
					} else if ((strcmp(NODE_PORT_ATOMICFLAG_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						iAtomicFlag = atoi(value);
					} else if ((strcmp(NODE_PORT_MANDATORYFLAG_ELEMENT, name) == 0)) {
						ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
						value = (char*) xmlTextReaderConstValue(reader);
						iMandatoryFlag = atoi(value);
					} 
				


					
					
					// read next node
					ret = xmlTextReaderRead(reader);
				}

				// create instance of port
				// @todo - we really shouldn't fully instantiate the port at this point as we don't know the true value of funcArg array at this point. Should place the values in a temporary variable to make this clear in the code and only place values in funcArg when we know the correct values

				if (iPortType==INPUTPORT) {
					inputport[inputport_num] = new Port(INXPoint(x-NODE_PORT_CDF_DRAW_LEFT_OFFSET,y)+point, inputport_num, iDataType, iPortType, portDesc, funcName, funcArg, 1, bPortVertical, m_iUserDefined, iMandatoryFlag);
					inputport_num++;
					numInArgs = funcInd;
				}
				if (iPortType==OUTPUTPORT) {
					outputport[outputport_num] = new Port(INXPoint(x+NODE_PORT_CDF_DRAW_RIGHT_OFFSET,y)+point, outputport_num, iDataType, iPortType, portDesc, funcName, funcArg, 1, bPortVertical, m_iUserDefined, iMandatoryFlag);
					outputport_num++;
				}
				if (iPortType==STARTPORT) {
					startport[startport_num] = new Port(INXPoint(x-NODE_PORT_CDF_DRAW_LEFT_OFFSET,y)+point, startport_num, iDataType, iPortType, portDesc, funcName, funcArg,  iAtomicFlag, bPortVertical, m_iUserDefined, iMandatoryFlag);
					startport_num++;
				}
				if (iPortType==FINISHPORT) {
					finishport[finishport_num] = new Port(INXPoint(x+NODE_PORT_CDF_DRAW_RIGHT_OFFSET,y)+point, finishport_num, iDataType, iPortType, portDesc, funcName, funcArg,  1, bPortVertical, m_iUserDefined, iMandatoryFlag);
					finishport_num++;
				}
				if (iPortType==INTERNALPORT) {
					internalport[internalport_num] = new Port(internalport_num, iPortType, funcName, funcArg, iAtomicFlag);
					internalport_num++;
				}

			}


			// read next node
            ret = xmlTextReaderRead(reader);
		}
        xmlFreeTextReader(reader);

		updateFunctionArg(funcs);
	} else {
        printf("Unable to open %s\n", filepath);
    }
}


// Function that reads an icon's description file (IDF) to establish the graphics
// and port geometry.
void ConData::ReadIDFFile(INXString csIconType, INXPoint point) {
	INXString filepath;
	CFileOperation fo;
	int pos = 0;

	inputport_num = 0;
	outputport_num = 0;
	startport_num = 0;
	finishport_num = 0;
	internalport_num = 0;
	iParamNum = 1;




	pos = csIconType.Find(FILE_TYPE_CDF);
	if (pos == -1) {
		// Library IDF is now in the 'userdefined' directory
		filepath = workDir + IDFDIR + csIconType + ".idf.ini";
		if (!fo.CheckPath(filepath)) {
			filepath = workDir + USERDEFDIR + csIconType + ".idf.ini";
			if (!fo.CheckPath(filepath)) {
				wxMessageBox("No IDF defined for Component: " + csIconType);
				return;
			}
		}

		readFromIDFFile(filepath, point);
	} else {
		// Library CDF
		filepath = workDir + CDFDIR + csIconType;
		if (!fo.CheckPath(filepath)) {
// @todo used defined components not implemented yet for CDF file types
//			filepath = workDir + USERDEFDIR + csIconType + ".idf.ini";
//			if (!fo.CheckPath(filepath)) {
				wxMessageBox("No CDF defined for Component: " + csIconType);
				return;
//			}
		}
		
		readFromCDFFile(filepath, point);
	}
}


// Function that converts a port type INXString to an int
int ConData::portType2Int(INXString str) {
	if (str == "InputPort") {
		return INPUTPORT;
	}
	else if (str == "OutputPort") {
		return OUTPUTPORT;
	}
	else if (str == "StartPort") {
		return STARTPORT;
	}
	else if (str == "FinishPort") {
		return FINISHPORT;
	}
	else if (str == "InternalPort") {
		return INTERNALPORT;
	}
	else {
		return -1;
	}
}

//Function that converts a data type INXString to an int
int ConData::dataType2Int(INXString str)
{
	if (str == "B") {
		return 0;
	}
	else if (str == "I") {
		return 1;
	}
	else if (str == "F") {
		return 2;
	}
	else if (str == "S") {
		return 3;
	}
	else if (str == "ST") {
		return 4;
	}
	else if (str == "WT") {
		return 5;
	}
	else {
		return -1;
	}
}

#endif
/*
Tests a coordinate to see if it is within the icons's boundary
This is used to select icons, for instance to bring up configuration dialog boxes.
It could also be used to speed up the port test discussed above
  */
int ConData::In(INXPoint point)  //Is this point in the icon area
{
	int retval=0;
	if (m_iShow==1) {
			if ( (point.x > (rectangle.TopLeft().x+5)) &&
				(point.y > (rectangle.TopLeft().y+5)) &&
				(point.x < (rectangle.BottomRight().x-5)) &&
				(point.y < (rectangle.BottomRight().y-5)) )  {

				retval=1;
			}
	}

	return retval;
		
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
void ConData::LoadNewBMP(INXString csIconType) {
	INXString bitmappath = csIconType + ".bmp";	
	bitmappath = workDir + BMPDIR + bitmappath;
	INXSize tempSize = bitmap.Init(bitmappath);
	INXPoint _point = rectangle.TopLeft();
	rectangle = INXRect(_point.x,_point.y,_point.x+tempSize.cx,_point.y+tempSize.cy);
}

void ConData::ResizeIcon()
{
	char szPortNum[3];

	int p;

	if(  (CountNonVerticalPorts(INPUTPORT) + CountNonVerticalPorts(STARTPORT)) > (CountNonVerticalPorts(OUTPUTPORT) + CountNonVerticalPorts(FINISHPORT))	)
	{
		p= (int)(((CountNonVerticalPorts(INPUTPORT)+1 + CountNonVerticalPorts(STARTPORT) + 1)/2));
	}
	else
	{
		p = (int)(((CountNonVerticalPorts(OUTPUTPORT)+1 + CountNonVerticalPorts(FINISHPORT) + 1)/2));
	}

	if(p>10)
	{	p=10;
	}
	if(p<1)
	{	p=1;
	}

	_itoa_s(p,szPortNum,10);
	LoadNewBMP("ENCAPSULATE" + (INXString)szPortNum );
	m_csIconType = ("ENCAPSULATE" + (INXString)szPortNum );
	repositionVerticalPorts(); //make sure vertical ports stay level with the bottom.
}

void ConData::repositionVerticalPorts()
{
	int rh = rectangle.BottomRight().y - rectangle.TopLeft().y;

	if((bitmap.GetHeight() != (rh-1)))
	{
		int rectbr = rectangle.BottomRight().y;
		int bgh = bitmap.GetHeight();

		rectangle.bottom = rectangle.bottom + bgh - rh - 1;

	}

	INXPoint p = rectangle.BottomRight();

	UINT i;
	for (i=0; i<outputport_num; i++) 
	{
		if(outputport[i]->bPortVertical==true)
		{
			outputport[i]->Move(INXPoint(0, outputport[i]->rectangle.top-p.y ));
		}

	}
	for (i=0; i<finishport_num; i++)
	{
		if(finishport[i]->bPortVertical==true)
		{
			finishport[i]->Move(INXPoint(0, finishport[i]->rectangle.top-p.y ));
		}

	}

}

INXPoint ConData::CalculateXPortPosition(int iPortType)
{
	if(iPortType == INPUTPORT)
	{
		return INXPoint(0,50 + (100 * (inputport_num)));
	}
	else if(iPortType == OUTPUTPORT)
	{
		return INXPoint(200,(70+(100 * outputport_num)));
	}
	else if(iPortType == STARTPORT)
	{
		return INXPoint(0,100 * (startport_num));
	}
	else if(iPortType == FINISHPORT)
	{
		return INXPoint(200,100 * (finishport_num));
	}
	else
	{
		return INXPoint(0,0);
	}
		
}

INXString ConData::BuildXPortString(int iPortType,int iDataType)
{

	if(iPortType == STARTPORT)
	{
		return "XSTART";
	}
	else if(iPortType == FINISHPORT)
	{
		return  "XFINISH";
	}
	else if(iPortType == INPUTPORT)
	{

		if(iDataType == BOOLEAN)
		{
			return "XINPUTB";
		}
		else if(iDataType == INTEGER)
		{
			return "XINPUTI";
		}
		else if(iDataType == FLOAT)
		{
			return "XINPUTR";
		}
		else if(iDataType == STRING)
		{
			return "XINPUTS";
		}
	}
	else if(iPortType == OUTPUTPORT)
	{
	
		if(iDataType == BOOLEAN)
		{
			return "XOUTPUTB";
		}
		else if(iDataType == INTEGER)
		{
			return "XOUTPUTI";
		}
		else if(iDataType == FLOAT)
		{
			return "XOUTPUTR";
		}
		else if(iDataType == STRING)
		{
			return "XOUTPUTS";
		}
	}
	
	return "";
}

#endif
INXPoint ConData::GetIconCentre()
{
	return rectangle.CenterPoint();
	
}

INXPoint ConData::GetIconPos()
{
	return rectangle.TopLeft();
}

INXPoint ConData::GetIconBottomRight()
{
	return rectangle.BottomRight();
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE

void ConData::MinusYCoords()
{
	
	rectangle = INXRect(GetIconPos().x, -GetIconPos().y,GetIconBottomRight().x, -GetIconBottomRight().y);

}

INXRect ConData::GetBoundingRectangle()
{
	return rectangle;
}

int ConData::CountNonVerticalPorts(int PortType)
{

	int count = 0;

	if(PortType == INPUTPORT)
	{
		for(UINT i=0;i<inputport_num;i++)
		{
			if(inputport[i]->bPortVertical != true)
			{	
				count++;
			}
		}
		return count;
	}
	else if(PortType == OUTPUTPORT)
	{
		for(UINT i=0;i<outputport_num;i++)
		{
			if(outputport[i]->bPortVertical != true)
			{	
				count++;
			}
		}
		return count;
	}
	else if(PortType == STARTPORT)
	{	
		for(UINT i=0;i<startport_num;i++)
		{
			if(startport[i]->bPortVertical != true)
			{	
				count++;
			}
		}
		return count;
	}
	else if(PortType == FINISHPORT)
	{
		for(UINT i=0;i<finishport_num;i++)
		{
			if(finishport[i]->bPortVertical != true)
			{	
				count++;
			}
		}
		return count;
	}
	else 
	{
		return 0;
	}
	


}

bool ConData::isGuiWidget()
{
	bool ret = FALSE;

	for (UINT i=1; i<iParamNum; i++) {
		// check if there is widget tag
		if (iconParam[i]->dataType == 5) {
			ret = TRUE;
		}
	}

	return ret;
}

LucidErrEnum ConData::getWidgetTag(INXString &csWidgetTag)
{
	LucidErrEnum ret = kErr_NoWidgetTag;

	for (UINT i=1; i<iParamNum; i++) {
		// get widget tag
		if (iconParam[i]->dataType == 5) {
			csWidgetTag = iconParam[i]->value;
			ret = kErr_NoErr;
		}
	}

	return ret;
}

// Method that gets a widget group name (formerly known as a screen tag)
LucidErrEnum ConData::getScreenTag(INXString &csScreenTag)
{
	LucidErrEnum ret = kErr_NoScreenTag;

	for (UINT i=1; i<iParamNum; i++) {
		// get screen tag
		if (iconParam[i]->dataType == 4) {
			csScreenTag = iconParam[i]->value;
			ret = kErr_NoErr;
		}
	}

	return ret;
}

// Method that sets a widget group name (formerly known as a screen tag)
LucidErrEnum ConData::setScreenTag(INXString csScreenTag)
{
	LucidErrEnum ret = kErr_NoScreenTag;

	for (UINT i=1; i<iParamNum; i++) {
		// set screen tag
		if (iconParam[i]->dataType == 4) {
			iconParam[i]->value = csScreenTag;
			ret = kErr_NoErr;
		}
	}

	return ret;
}

void ConData::setEncapsulated(bool encaps)
{
	encapsulated = encaps;
}

bool ConData::getEncapsulated()
{
	return encapsulated;
}

void ConData::hideSelected()
{
	m_iShow = 0;
	for (UINT i=0; i<inputport_num; i++) {
		inputport[i]->line.setShow(0);
	}
	for (UINT i=0; i<startport_num; i++) {
		startport[i]->line.setShow(0);
	}
}

void ConData::showSelected()
{
	m_iShow = 1;
	for (UINT i=0; i<inputport_num; i++) {
		inputport[i]->line.setShow(1);
	}
	for (UINT i=0; i<startport_num; i++) {
		startport[i]->line.setShow(1);
	}
}

// Method that returns the number of ports for the specified datatype.
// Can only be used with input and out ports, and not start or finish ports
int ConData::getPortCount(int iPortType, int iDataType)
{
	int iPortCount = 0;

	if (iPortType == INPUTPORT) {
		for (UINT i=0; i<inputport_num; i++) {
			if (inputport[i]->datatype == iDataType) {
				iPortCount++;
			}
		}
	}
	else if (iPortType == OUTPUTPORT) {
		for (UINT i=0; i<outputport_num; i++) {
			if (outputport[i]->datatype == iDataType) {
				iPortCount++;
			}
		}
	}

	return iPortCount;
}

#endif
