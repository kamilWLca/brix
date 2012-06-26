// IconLines.cpp: implementation of the IconLines class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "DrawProg.h"
#include "IconLines.h"
#include <list>
#include <cmath>
#include "../common/LucidConstants.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IconLines::IconLines()
{
	exist = 0;
	othericonid = -1;
	otherportno = 0;
	idNum = 0;
	portType = -1;
	nodeCount = 0;
	dbgValue = "";
	valueRect = (0,0,0,0);
	dbgEvent = 0;
	lineSelected = 0;
	hierID = -1;
	show = 1;
	m_bDbgMonitorSel = FALSE;
	m_bDefineMonitor = FALSE;
	m_iRtaTraceId = 0;
	m_iSelSegmentNum = -1;
}

IconLines::~IconLines()
{
	deleteNodeMemory();
}


/*
The endpoints of lines are pointers to the CPoint position data attributes of the icon ports, 
and not actually copied in the lines data attributes. Doing this by reference allows icons to be moved 
and the endpoints of lines to be automatically updated. For bent lines the intermediate position will be fixed and are held only in the lines object.
  */
void IconLines::SetEndPoints(CPoint *start, CPoint *end,long int _othericon,int _portNum, int _portType) {
	CPoint *tmp;
	tmp = start;
	int nopoints=points.GetUpperBound();
	if (nopoints<1) {
	points.SetAtGrow(0,(CObject*) start) ;//new CPoint(start)); ..insert incase any mid points exist
	points.Add((CObject*) end) ;//new CPoint(end));
	}
	else if (nopoints<2) {
	//delete points[0]; // remove memory? Memory leak
    //if (points[0]!=NULL) {
	//   delete points[0];
	//}
	points[0]=((CObject*) start) ;//new CPoint(start));
	points.Add((CObject*) end) ;//new CPoint(end));
	}
	else {
	points[0]=((CObject*) start) ;//new CPoint(start));
	//points[nopoints]=((CObject*) end) ; // original code
	points.Add((CObject*) end) ;//new CPoint(end));
	}

	othericonid=_othericon;
	otherportno=_portNum;
	exist=1;

}

/*  Adds a dog leg to a line. It first clears any previous nodes and adds either two for a forward connection
    or adds a 4 nodes for a backward path that will travel underneath both function blocks.
	x_position is a relative index that is related to the port number, so that this function can attempt 
	to calculate a non overlapping vertical path for the dog leg.
	y_level is the y level of the bottom of the function blocks. This is used as the basis for backward travelling
	connections to avoid the bottom of the function block.
	*/

#ifdef BENDUNDER
int IconLines::AddDogLeg(int x_position,int ylevel) // makes a forward dogleg of the line
{
	int x_dogleg=0;
	int endnodenum = points.GetUpperBound();
	if (endnodenum<1) return 0; // Need to have endpoints already defined
	//CPoint dognode1,dognode2,dognode3,dognode4; 
	deleteAllNodes(); //Remove intermediate nodes (straighten)
	int end_x =((CPoint*)(points[1]))->x;
	int end_y=((CPoint*)(points[1]))->y;
	/* forward and down path */
	if ((((CPoint*)(points[0]))->x<((CPoint*)(points[1]))->x) && ((((CPoint*)(points[0]))->y>((CPoint*)(points[1]))->y)) ) // The input port is right of the output port
	{	
		x_dogleg=min ((end_x-((CPoint*)(points[0]))->x)/2+(x_position*5)+((CPoint*)(points[0]))->x,end_x-10);//try to do staggered positions, but if too close to inout port then do 10 pixels back 
		addNode(CPoint(x_dogleg, ((CPoint*)(points[0]))->y));
		addNode(CPoint(x_dogleg, end_y));
	}
	/* forward and up path */
	else if ((((CPoint*)(points[0]))->x<((CPoint*)(points[1]))->x) && ((((CPoint*)(points[0]))->y<=((CPoint*)(points[1]))->y)) )
	{ // here we will do the dog legs backwards from the halfway point to get proper nesting
		x_dogleg=max ((end_x-((CPoint*)(points[0]))->x)/2-(x_position*5)+((CPoint*)(points[0]))->x,((CPoint*)(points[0]))->x+10);//try to do staggered positions, but if too close to inout port then do 10 pixels back 
		addNode(CPoint(x_dogleg, ((CPoint*)(points[0]))->y));
		addNode(CPoint(x_dogleg, end_y));
	}
	else { // the input port is behind - will draw a fixed loop over the top
		addNode(CPoint(  ((CPoint*)(points[0]))->x + max(5,40-x_position*5), ((CPoint*)(points[0]))->y));
		addNode(CPoint(  ((CPoint*)(points[0]))->x + max(5,40-x_position*5), 40+ylevel-x_position*5)); // @todo something better than this
		addNode(CPoint(end_x-max(5,40-x_position*5), ylevel+40-x_position*5));
		addNode(CPoint(end_x-max(5,40-x_position*5), end_y));
	}
	return 0;
}
#else // over the top
int IconLines::AddDogLeg(int x_position,int ylevel) // makes a forward dogleg of the line. Ylevel is a top of block clearence value
{
	int x_dogleg=0;
	int endnodenum = points.GetUpperBound();
	x_position++; //make this start at 1 to simplify sums
	if (endnodenum<1) return 0; // Need to have endpoints already defined
	//CPoint dognode1,dognode2,dognode3,dognode4; 
	deleteAllNodes(); //Remove intermediate nodes (straighten)
	int end_x =((CPoint*)(points[1]))->x;
	int end_y=((CPoint*)(points[1]))->y;
	/* forward and down path */
	if ((((CPoint*)(points[0]))->x<((CPoint*)(points[1]))->x) && ((((CPoint*)(points[0]))->y>((CPoint*)(points[1]))->y)) ) // The input port is right of the output port
	{	
		x_dogleg=end_x-(x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE);//try to do staggered positions, but if too close to inout port then do 10 pixels back 
		addNode(CPoint(x_dogleg, ((CPoint*)(points[0]))->y));
		addNode(CPoint(x_dogleg, end_y));
	}
	/* forward and up path */
	else if ((((CPoint*)(points[0]))->x<((CPoint*)(points[1]))->x) && ((((CPoint*)(points[0]))->y<=((CPoint*)(points[1]))->y)) )
	{ // here we will do the dog legs backwards from the halfway point to get proper nesting
		x_dogleg= end_x-(x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE);//try to do staggered positions, but if too close to inout port then do 10 pixels back 
		addNode(CPoint(x_dogleg, ((CPoint*)(points[0]))->y));
		addNode(CPoint(x_dogleg, end_y));
	}
	else { // the input port is behind - will draw a fixed loop underneath
		addNode(CPoint(  ((CPoint*)(points[0]))->x + max(5,x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE), ((CPoint*)(points[0]))->y));
		addNode(CPoint(  ((CPoint*)(points[0]))->x + max(5,x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE), ylevel-x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE)); // @todo something better than this
		addNode(CPoint(end_x-max(5,x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE), ylevel-x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE));
		addNode(CPoint(end_x-max(5,x_position*NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE), end_y));
	}
	return 0;
}
#endif

/*
void IconLines::AddBackwardogLeg(int x_postion ) // makes a Backward dogleg of the line
{

}
*/

// Method that adds a node point to the array of points at the index specified by iNodeNum
void IconLines::AddNode(CPoint cpNodePt, int iNodeNum) {
	// Don't allow nodes to be connected to ports
	if (iNodeNum <= 0 || iNodeNum >= points.GetUpperBound()) {
		return;
	}
	points.InsertAt(iNodeNum, (CObject*) new CPoint(cpNodePt));
}

// Function that adds a node to the array of points
void IconLines::addNode(CPoint _node) {
	nodeCount++;
	points.InsertAt(nodeCount, (CObject*) new CPoint(_node));
}

// Function that edits a node
void IconLines::editNode(CPoint _node) {
	CPoint *temp;
	temp=(CPoint*)points.GetAt(nodeCount);
	*temp=_node;
}

// Function to delete a node. The node count should be set first
void IconLines::deleteNode() {
	delete (CPoint*)points.GetAt(nodeCount);
	points.RemoveAt(nodeCount);
}

// Method that deletes the specified node
void IconLines::DeleteNode(int iNodeNum) {
	int temp = points.GetUpperBound();
	
	// Don't allow points connected to ports to be deleted
	if (iNodeNum <= 0 || iNodeNum >= points.GetUpperBound()) {
		return;
	}
	delete (CPoint*)points.GetAt(iNodeNum);
	points.RemoveAt(iNodeNum);
}

// Function to delete all nodes. Used to return to a straight line
void IconLines::deleteAllNodes() {
	int nodeNum = points.GetUpperBound() - 1;

	for (int i=nodeNum; i>0; i--) {
		delete (CPoint*)points.GetAt(i);
		points.RemoveAt(i);
	}
	nodeCount = 0;
}

void IconLines::Delete(){
	int sublinnum = points.GetUpperBound(); // do an error check here
	if (sublinnum<0) return;
	deleteNodeMemory();
	points.RemoveAll();
	exist=0;
	nodeCount = 0;
	othericonid = -1;
}

void IconLines::deleteNodeMemory()
{
	for (int i=1; i<points.GetSize()-1; i++) {
		delete (CPoint*)points.GetAt(i);
	}	
}

void IconLines::Disconnect(){
	int sublinnum=points.GetUpperBound(); // do an error check here
	if (sublinnum<0) return;
	deleteNodeMemory();
	points.RemoveAll();
}

// function to set the ID. Required for writing out SODL
void IconLines::setIDNum(long int _idNum) {
	idNum = _idNum;
}

void IconLines::setPortType(int _portType) {
	portType = _portType;
}

void IconLines::Save(ostream * file) {
	*file << "BEGIN_LINE" << endl;
	*file << exist << "\t" << othericonid<< "\t" << otherportno << "\t" << portType << "\t" << hierID << "\t" << m_bDbgMonitorSel << endl;
	for (int i =1;i<(points.GetUpperBound());i++) { //only save the points of non - endpoints
		*file << ((CPoint *)( points[i]))->x << "\t" << ((CPoint *)( points[i]))->y <<endl;
	}
	*file << "END_LINE" << endl;
}

void IconLines::Load(istream * file) {
	char temp[256];
	CPoint ReadPoint;
	nodeCount = 0;
	CString csTmpStr;

	file->ignore(1,'\n');
	file->getline(temp,254); // BEGIN_LINE 
	file->getline(temp,254);
	// Use sscanf instead of the line below to make the read backward compatible
	//*file >>  temp >> exist >> othericonid >> otherportno >> portType >> hierID >> m_bDbgMonitorSel;
	sscanf(temp, "%d%d%d%d%d%d", &exist, &othericonid, &otherportno, &portType, &hierID, &m_bDbgMonitorSel);

	if (!exist) return;
	//points.Add(NULL); //add NULL start point
	do {
		*file >> temp;
		if (strcmp(temp,"END_LINE")==0) break; // Sorry but it's short
		ReadPoint.x=atoi(temp);
		*file >> ReadPoint.y;
		nodeCount++;
		points.SetAtGrow(nodeCount, (CObject*) new CPoint(ReadPoint));
	} while (1);
	//points.Add(NULL); // add NULL end point
}

// routine to draw a set of points as a ziggy line
// routine to draw a set of points as a ziggy line
void IconLines::Draw(CDC* theDC) {
	CPoint begin,end;
	CPoint p0, p1, p2; // Pts that form each respective corner
	int iLineSeg = 0; 

	CPen bluepen, redpen, greenpen, yellowpen, dashpen, dashorangepen, blackpen, purplepen, dashpurplepen;
	bluepen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	redpen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	greenpen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	yellowpen.CreatePen(PS_SOLID,1,RGB(225,225,0));
	dashpen.CreatePen(PS_SOLID,1,RGB(128,128,128)); //dashpen.CreatePen(PS_DASH,1,RGB(0,0,0));
	dashorangepen.CreatePen(PS_DASH,2,RGB(255,128,0));
	blackpen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	purplepen.CreatePen(PS_SOLID,2,RGB(128,0,255));
	dashpurplepen.CreatePen(PS_DASH,2,RGB(128,0,255));

	//COLORREF colour;

	if (!exist) return;

	int nLineSegs = points.GetUpperBound(); // do an error check here

	if ( nLineSegs < 0 ) return;		
	
	// Control lines are drawn with a dashed line
	// *** For some reason print preview comes up with a debug assertion failure when using
	// SelectObject. Also, does not print out dashed lines. For the time being only have dashed 
	// lines and coloured lines in the normal view mode.

	CPen* originalPen;
	originalPen = theDC->GetCurrentPen();

	if (show) {
		if (portType == STARTPORT) {
			if (!theDC->IsPrinting() && dbgEvent && m_bDbgMonitorSel) {
				theDC->SelectObject(dashorangepen);
			}
			else {
				if (!theDC->IsPrinting() && m_bDbgMonitorSel && m_bDefineMonitor) {
					theDC->SelectObject(dashpurplepen);
				}
				else {
					theDC->SelectObject(dashpen);
				}
			}
		}
		else {
			if (!theDC->IsPrinting() && m_bDbgMonitorSel && m_bDefineMonitor) {
				theDC->SelectObject(purplepen);
			}
			else {
				switch(dataType) {
				case 0: theDC->SelectObject(yellowpen); break;
				case 1: theDC->SelectObject(bluepen); break;		
				case 2:	theDC->SelectObject(greenpen); break;
				case 3:	theDC->SelectObject(redpen); break;
				default: theDC->SelectObject(blackpen); break;	
				}
			}
		}

		p0 = (CPoint) *( (CPoint *) points.GetAt(0) );
		p1 = (CPoint) *( (CPoint *) points.GetAt(1) );


		if(nLineSegs > 1)
		{
			// Need at least 2 segs to form a corner!			
			
			// list to store points in.  Easiest to store them first, then draw them after
			std::list< CPoint > pntList;

			pntList.clear();

			float v0[2], v1[2];					// line segment vectors (x,y)
			float v0Mag, v1Mag;					//line seg lengths
			float fCosTheta;					//cosine of angle between segs
			float NEARLY_RIGHT_ANGLE = 80.0;	// Chamfer corners only of near a right-angle

			float PI = 4.0 * atan2( 1.0, 1.0 );
			float fLimitCos = cos( NEARLY_RIGHT_ANGLE * PI / 180 ); 
			// angles not nearly right have bigger cos than this

			int CHAMFER_LENGTH = 5;  // Do not chamfer very short lines

			p2 = (CPoint) *( (CPoint *) points.GetAt(2) );

			pntList.push_back( p0 );
//if (portType == STARTPORT) {
			// Loop through corners
			while ( iLineSeg <= nLineSegs-2 )
			{

				// Get x,y vector of each line seg
				v0[0] = p1.x  - p0.x;
				v1[0] = p2.x  - p1.x;

				v0[1] = p1.y  - p0.y;
				v1[1] = p2.y  - p1.y;

				// Calc magnitudes (line seg lengths!)
				v0Mag = sqrt( v0[0] * v0[0] + v0[1] * v0[1] );
				v1Mag = sqrt( v1[0] * v1[0] + v1[1] * v1[1] );

				// Normalise to unit length
				v0[0] /= v0Mag;
				v0[1] /= v0Mag;

				v1[0] /= v1Mag;
				v1[1] /= v1Mag;

				// Form dot product to find (cos of) theta
				fCosTheta = v0[0] * v1[0] + v0[1] * v1[1];


				// Add chamfer if appropriate
				bool bNearRtAngle = fabs( fCosTheta ) < fLimitCos;
				bool bLinesLongEnuff = ( (v0Mag > 2 * CHAMFER_LENGTH ) && (v1Mag > 2 * CHAMFER_LENGTH )   );
				bool bDoChamfer = bNearRtAngle && bLinesLongEnuff;
		

				if( bDoChamfer ){

					CPoint p1Before, p1After;

					// go back along 1st seg from corner, forward along 2nd seg
					p1Before.x = p1.x - CHAMFER_LENGTH*v0[0];
					p1Before.y = p1.y - CHAMFER_LENGTH*v0[1];

					p1After.x = p1.x + CHAMFER_LENGTH*v1[0];
					p1After.y = p1.y + CHAMFER_LENGTH*v1[1];

					pntList.push_back( p1Before );
					pntList.push_back( p1After );

				} else {

					pntList.push_back( p1 );
				}

				iLineSeg++;

				// Don't access past end of 'points' contents at end of loop
				if( iLineSeg <= nLineSegs-2 )
				{
					p0 = p1;
					p1 = p2;
					p2 = (CPoint) *( (CPoint *) points.GetAt(iLineSeg + 2) );

				}

			} // while ( iLineSeg < nLineSegs-2 )

			// add final point
			pntList.push_back( p2 );

			std::list< CPoint >::iterator it = pntList.begin();


			// reflect points on line in y-axis for printing
			if (theDC->IsPrinting()) {
				it->y = -1 * (it->y);
			}

			// Draw the lines themselves
			theDC->MoveTo( *it );
			while(++it !=  pntList.end() )
			{
				// reflect points on line in y-axis for printing
				if (theDC->IsPrinting()) {
					it->y = -1 * (it->y);
				}
				theDC->LineTo( *it );
			}

		} else { // if !( nLineSegs > 1)
			
			if (theDC->IsPrinting()) {
				p0.y = -p0.y;
				p1.y = -p1.y;
			}
			theDC->MoveTo( p0 );
			theDC->LineTo( p1 );
		} //( nLineSegs > 1)

	} // if (show) 

	// DrawNodes on straight lines
	for (iLineSeg=0; iLineSeg<nLineSegs-2; iLineSeg++) {
		p0 = (CPoint) *((CPoint *) points.GetAt(iLineSeg));
		p1 = (CPoint) *((CPoint *) points.GetAt(iLineSeg+1));
		p2 = (CPoint) *((CPoint *) points.GetAt(iLineSeg+2));
		if ((p0.x == p1.x && p1.x == p2.x) || (p0.y == p1.y && p1.y == p2.y)) {
			DrawNode(theDC, p1);
		}
	}

	// restore original pen
	theDC->SelectObject(originalPen);
			
}

// this is called for all the lines connected as inputs to a function block
/*
void IconLines::Move(CPoint point) {
	CPoint node;
	int nodenum = points.GetUpperBound();

	int oldNodeCount = nodeCount;
	for (int i=1; i<nodenum; i++) {
		// if a line is in the selection box then offset all the nodes
		if (lineSelected) {
			node = *((CPoint*)(points.GetAt(i)));
			node = node - point;
			nodeCount = i;
			editNode(node);
		}
		// if a line isn't in the selection box then offset all the x coords / 2
		// offset all the y coords except the one closest to the other end
		else {
			node = *((CPoint*)(points.GetAt(i)));
			node.x = node.x - point.x/2;
			if (i>1) {
				node.y = node.y - point.y;
			}
			nodeCount = i;
			editNode(node);
		}
	}

	nodeCount = oldNodeCount;
}
*/

void IconLines::Move(CPoint relative_point) {
	CPoint *node,*nodeInputPort,*nodeOutputPort,*nodeNext,*nodeNext2; //create a new node position
	int min_y_position=0;
	int mostRightNodePosition=0;
	int nodenum = points.GetUpperBound();
	int oldNodeCount = nodeCount;
	nodeOutputPort = ((CPoint*)(points.GetAt(0)));						// this is the output port
	if (nodenum>0) nodeNext = ((CPoint*)(points.GetAt(nodenum-1)));  // this is the next node from the output port
	nodeInputPort = ((CPoint*)(points.GetAt(nodenum)));						// this is the inout port
	
	if (lineSelected) { // if this is a whole line shift just move all the points.
		for (int i=1; i<nodenum; i++) {
			node = ((CPoint*)(points.GetAt(i)));
			*node = *node - relative_point;
			nodeCount = i;
			//editNode(node);
		}
		return ;
	}
	//if this is just one end moving do the same as Move out..... 
	for (int i=0; i<=nodenum; i++) { // find the max y position in case a double dog leg is needed.
		node = ((CPoint*)(points.GetAt(i)));
		if (node->y > min_y_position)  min_y_position=node->y;
		if ((i<nodenum)&&(node->x>mostRightNodePosition)) mostRightNodePosition=node->x;
		}
	if ( nodenum < 2 )  { // This is a straight line - leave as a straight line, [perhaps if the shift makes the direction go backwards add a dogleg?]
	// do nothing
	}
	else if ( nodenum < 3 )  { // there is no dogleg so do nothing to intermediate nodes
	 // do nothing let the ends move - this was a messy connection anyway
	}
	else /* if ( nodenum < 4 ) */{ // this has a dogleg
 // single dog leg move just the first nodes
		if (((nodeOutputPort->x ) > nodeInputPort->x-10) /*&&  // this will tangle if it's a new 'slide past', add a dogleg
			((nodeOutputPort->x ) <= nodeInputPort->x)*/ ) { // was the line alreadty travelling backwards
			AddDogLeg(2,min_y_position-40); //make a dog leg - overlapping lines may occur as no port number info is available.
		}
		else { // simple shift without tangling - so just adjust orthogonal part relative to ouput ports.
			if (((nodeInputPort->x - relative_point.x)< (mostRightNodePosition+10)) //&&//{ // dog leg is closest to this so move the dog leg nodes
				/*((nodeOutputPort->x ) > nodeInputPort->x) */ ) {
				nodeNext2 = ((CPoint*)(points.GetAt(nodenum-2))); // we will need the next node in the dogleg to update the x position 	
				nodeNext->x = max(nodeNext->x - relative_point.x,nodeOutputPort->x); // move the first dogleg node as is, unless past ither end 
				nodeNext->y = nodeNext->y - relative_point.y;
				nodeNext2->x=max(nodeNext2->x - relative_point.x,nodeOutputPort->x); 
			} 
			else { // dog leg is closer to the other end so leave it where it is
				 // just need to change the first nodes y position
				nodeNext->y = nodeNext->y - relative_point.y;
				//nodeNext->x = nodeNext->x - relative_point.x; 
			}
		}
	
	}
	nodeCount = points.GetUpperBound(); // update with the possibly new data
}

/* This is called for all the lines connected as outputs to a function block
i.e. This function is called if the other end of the line is being moved 
   (as a result of a function block being moved)
   This block should shift the nodes of the line to maintain as much of a sensible structure as possible
   The inout is the relative point of the shift.
   */

void IconLines::MoveOutNode(CPoint relative_point) {
	CPoint *node,*nodeInputPort,*nodeOutputPort,*nodeNext,*nodeNext2; //create a new node position
	int min_y_position=0;
	int mostLeftNodePosition=1000000;
	int nodenum = points.GetUpperBound();
	int oldNodeCount = nodeCount;
	nodeOutputPort = ((CPoint*)(points.GetAt(0)));						// this is the output port
	if (nodenum>0) nodeNext = ((CPoint*)(points.GetAt(1)));  // this is the next node from the output port
	nodeInputPort = ((CPoint*)(points.GetAt(nodenum)));						// this is the inout port
	for (int i=0; i<=nodenum; i++) { // find the max y position in case a double dog leg is needed.
		node = ((CPoint*)(points.GetAt(i)));
		if (node->y > min_y_position)  min_y_position=node->y;
		if ((i>0)&&(node->x<mostLeftNodePosition)) mostLeftNodePosition=node->x;
		}
	if ( nodenum < 2 )  { // This is a straight line - leave as a straight line, [perhaps if the shift makes the direction go backwards add a dogleg?]
	// do nothing
	}
	else if ( nodenum < 3 )  { // there is no dogleg so do nothing to intermediate nodes
	 // do nothing let the ends move - this was a messy connection anyway
	}
	else /* if ( nodenum < 4 ) */{ // this has a dogleg
 // single dog leg move just the first nodes
		if (((nodeOutputPort->x - relative_point.x) > nodeInputPort->x-10) /* && //{  // this will tangle, add a dogleg
			((nodeOutputPort->x ) <= nodeInputPort->x) */) { // was the line alreadty travelling backwards
			AddDogLeg(1,min_y_position-40); //make a dog leg - overlapping lines may occur as no port number info is available.
		}
		else { // simple shift without tangling - so just adjust orthogonal part relative to ouput ports.
			if (((nodeOutputPort->x )> (mostLeftNodePosition+10)) //&& //{ // dog leg is closest to this so move the dog leg nodes
				/*((nodeOutputPort->x ) > nodeInputPort->x) */) {
				nodeNext2 = ((CPoint*)(points.GetAt(2))); // we will need the next node in the dogleg to update the x position 	
				nodeNext->x = max(nodeNext->x - relative_point.x,nodeInputPort->x); // move the first dogleg node as is, unless past other end 
				nodeNext->y = nodeNext->y - relative_point.y;
				nodeNext2->x=max(nodeNext2->x - relative_point.x,nodeInputPort->x); 
			} 
			else { // dog leg is closer to the other end so leave it where it is
				 // just need to change the first nodes y position
				nodeNext->y = nodeNext->y - relative_point.y;
				//nodeNext->x = nodeNext->x - relative_point.x; 
			}
		}
	
	}
	nodeCount = points.GetUpperBound(); // update with the possibly new data
}

void IconLines::setShow(int _show)
{
	show = _show;
}

/*
void IconLines::Move(CPoint point) {
	CPoint node1, node2, outport, inport;
	int nodenum = points.GetUpperBound();
	// only move nodes if there are at least 2
	if (nodenum < 3) {
		return;
	}

	int oldNodeCount = nodeCount;
	
	outport = *((CPoint*)(points.GetAt(0)));
	inport = *((CPoint*)(points.GetAt(nodenum)));
	node1 = *((CPoint*)(points.GetAt(1)));
	node2 = *((CPoint*)(points.GetAt(nodenum-1)));
	node2.y = node2.y - point.y;
	
	// recalculate x coords
	if (portType == INPUTPORT) {
		node1.x = node1.x - point.x/2;
		node2.x = node2.x - point.x/2;
	}
	// if have more than two nodes can cause strange behaviour, so only adjust the 
	// nodes if they have the same x coords
	else if (portType == STARTPORT && node2.x == node1.x) {
		if (node2.y > node1.y) {
			node1.x = outport.x + (long)((inport.x - outport.x) * 0.6);
			node2.x = node1.x;
		}
		else {
			node1.x = outport.x + (long)((inport.x - outport.x) * 0.4);
			node2.x = node1.x;
		}
	}
	
	// offset x and y coord of node closest to input port
	// update points array with offset node
	nodeCount = nodenum - 1;
	editNode(node2);
	
	// offset x coord of node closest to output port
	// x coords of these points should be same
	// update points array with offset node
	nodeCount = 1;
	editNode(node1);

	nodeCount = oldNodeCount;
}

void IconLines::MoveOutNode(CPoint point) {
	CPoint node1, node2, inport, outport;
	int nodenum = points.GetUpperBound();
	// only move nodes if there are at least 2
	if (nodenum < 3) {
		return;
	}

	int oldNodeCount = nodeCount;
	
	// offset x and y coord of node closest to input port
	outport = *((CPoint*)(points.GetAt(0)));
	inport = *((CPoint*)(points.GetAt(nodenum)));
	node1 = *((CPoint*)(points.GetAt(1)));
	node2 = *((CPoint*)(points.GetAt(nodenum - 1)));
	node1.y = node1.y - point.y;
	
	// recalculate x coords
	if (portType == INPUTPORT) {
		node1.x = node1.x - point.x/2;
		node2.x = node2.x - point.x/2;
	}
	// if have more than two nodes can cause strange behaviour, so only adjust the 
	// nodes if they have the same x coords
	else if (portType == STARTPORT && node2.x == node1.x) {
		if (node2.y > node1.y) {
			node1.x = outport.x + (long)((inport.x - outport.x) * 0.6);
			node2.x = node1.x;
		}
		else {
			node1.x = outport.x + (long)((inport.x - outport.x) * 0.4);
			node2.x = node1.x;
		}
	}

	// update points array with offset node
	nodeCount = 1;
	editNode(node1);
	
	// offset x coord of node closest to input port
	// x coords of these points should be same
	// update points array with offset node
	nodeCount = nodenum - 1;
	editNode(node2);

	nodeCount = oldNodeCount;
}*/

// Debug Monitor
void IconLines::setDbgMonitor(bool bDbgMonitorSel)
{
	m_bDbgMonitorSel = bDbgMonitorSel;
}

void IconLines::toggleDbgMonitor()
{
	if (m_bDbgMonitorSel) {
		m_bDbgMonitorSel = FALSE;
	}
	else {
		m_bDbgMonitorSel = TRUE;
	}
}

bool IconLines::getDbgMonitor()
{
	return m_bDbgMonitorSel;
}

void IconLines::setDefineMonitor(bool bDefineMonitor)
{
	m_bDefineMonitor = bDefineMonitor;
}

// RTA Trace
void IconLines::setRtaTraceId(UINT iRtaTraceId)
{
	m_iRtaTraceId = iRtaTraceId;
}

UINT IconLines::getRtaTraceId()
{
	return m_iRtaTraceId;
}

void IconLines::MoveSegment(CPoint point)
{
	// Make sure selected Line segment has been set
	if (m_iSelSegmentNum >= 0)
	{
		CPoint *temp1, *temp2;
		temp1 = (CPoint*)points.GetAt(m_iSelSegmentNum);
		temp2 = (CPoint*)points.GetAt(m_iSelSegmentNum+1);
		// move vertical segment
		if (temp1->x == temp2->x) {
			temp1->x = point.x;
			temp2->x = point.x;
		}
		// move horizontal segment
		else if (temp1->y == temp2->y) {
			temp1->y = point.y;
			temp2->y = point.y;
		}
	}
}

void IconLines::SetSelectedSegment(int iSegmentNum)
{
	m_iSelSegmentNum = iSegmentNum;
}

int IconLines::GetSelectedSegment()
{
	return m_iSelSegmentNum;
}

// Method that returns the line segment the specified point is on.
// Returns -1 if the point is not on any segment
int IconLines::GetLineSegment(const CPoint cpPoint)
{
	int iSegmentNum = -1;

	for (int j=0; j<points.GetUpperBound()-1; j++) {
		if (IsOnSegment(cpPoint, j)) {
			iSegmentNum = j;
		}
	}

	return iSegmentNum;
}

// Method that returns True if a point is on the specified segment
// Otherwise returns false
bool IconLines::IsOnSegment(const CPoint cpPoint, const int iSegmentNum)
{
	bool bSuccess = false;
	CPoint cpStart, cpEnd;
	double dMinx, dMaxx, dMiny, dMaxy;

	cpStart = *((CPoint*)points.GetAt(iSegmentNum));
	cpEnd = *((CPoint*)points.GetAt(iSegmentNum+1));
	
	// set the min and max x values
	if (cpEnd.x > cpStart.x) {
		dMinx = cpStart.x - LINECLOSENESS;
		dMaxx = cpEnd.x + LINECLOSENESS;
	}
	else {
		dMinx = cpEnd.x - LINECLOSENESS;
		dMaxx = cpStart.x + LINECLOSENESS;
	}
	
	// set the min and max y values
	if (cpEnd.y > cpStart.y) {
		dMiny = cpStart.y - LINECLOSENESS;
		dMaxy = cpEnd.y + LINECLOSENESS;
	}
	else {
		dMiny = cpEnd.y - LINECLOSENESS;
		dMaxy = cpStart.y + LINECLOSENESS;
	}

	if ((cpPoint.x > dMinx) && (cpPoint.x < dMaxx) && (cpPoint.y > dMiny) && (cpPoint.y < dMaxy)) {
		bSuccess = true;
	}

	return bSuccess;
}

// Method that returns true if the 2 specified segments are aligned
bool IconLines::IsSegmentsAligned(const int iSegmentNum1, const int iSegmentNum2)
{
	CPoint cpSeg1StartPt, cpSeg1EndPt, cpSeg2StartPt, cpSeg2EndPt;

	if (iSegmentNum1 >= points.GetUpperBound() || iSegmentNum2 >= points.GetUpperBound()) {
		return false;
	}
	cpSeg1StartPt = *((CPoint*)points.GetAt(iSegmentNum1));
	cpSeg1EndPt = *((CPoint*)points.GetAt(iSegmentNum1 + 1));
	cpSeg2StartPt = *((CPoint*)points.GetAt(iSegmentNum2));
	cpSeg2EndPt = *((CPoint*)points.GetAt(iSegmentNum2 + 1));

	return ((cpSeg1StartPt.x == cpSeg1EndPt.x && cpSeg1StartPt.x == cpSeg2StartPt.x && cpSeg1StartPt.x == cpSeg2EndPt.x) ||
		(cpSeg1StartPt.y == cpSeg1EndPt.y && cpSeg1StartPt.y == cpSeg2StartPt.y && cpSeg1StartPt.y == cpSeg2EndPt.y));

}

void IconLines::DrawNode(CDC *pDC, CPoint point)
{
	CPoint tmpPoint = point;

	// Draw an 'X'
	tmpPoint.Offset(-5,-5);
	pDC->MoveTo(tmpPoint);
	tmpPoint.Offset(10,10);
	pDC->LineTo(tmpPoint);
	tmpPoint.Offset(-10,0);
	pDC->MoveTo(tmpPoint);
	tmpPoint.Offset(10,-10);
	pDC->LineTo(tmpPoint);
	pDC->MoveTo(point);
}