// IconLines.h: interface for the IconLines class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ICONLINES_H_INCLUDED_)
#define _ICONLINES_H_INCLUDED_
/*
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000*/

#include <iostream>
#include <list>
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXPoint.h"
#include "Porting_Classes/INXSize.h"
#include "Porting_Classes/INXRect.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObject.h"
#include <wx/glcanvas.h>

using namespace std;

class IconLines : public INXObject
{
public:
	void Save(ostream * file) ;
	void Load(istream * file) ; 
	IconLines();
	virtual ~IconLines();
	void SetEndPoints(INXPoint *start, INXPoint *end,long int _othericon,int _portNum, int _portType) ;
	/*addLine is created only when new line is generated EXCLUDED temporarly*/
	void addNode(INXPoint _node);
	/*addLine is created only when new line is generated EXCLUDED temporarly*/
	void AddNode(INXPoint cpNodePt, int iNodeNum);
	/*addLine is created only when new line is generated EXCLUDED temporarly*/
	int AddDogLeg(int x_postion = 50,int ylevel=50); // makes a forward dogleg of the line
	//void editNode(INXPoint _node);
	void deleteNode();
	void DeleteNode(int iNodeNum);
	/*addLine is created only when new line is generated EXCLUDED temporarly*/
	void deleteAllNodes();
	//void Draw(CDC* theDC);
	//void DrawDC(CDC* theDC);
	void DrawGL();
	void DrawDCGL();
	//void Delete();
	//void Disconnect();
	////void setIDNum(long int _idNum);
	//void setPortType(int _portType);
	//void Move(INXPoint point);
	//void MoveOutNode(INXPoint point);
	void deleteNodeMemory();
	//void setShow(int _show);
	// Debug monitor
	//void setDbgMonitor(bool bDbgMonitorSel);
	//void toggleDbgMonitor();

	bool getDbgMonitor();

	void setDefineMonitor(bool bDefineMonitor);

	// RTA Trace
	//void setRtaTraceId(UINT iRtaTraceId);
	//UINT getRtaTraceId();
	// Selected Line Segments

	void SetSelectedSegment(int iSegmentNum);

	int GetSelectedSegment();

	void MoveSegment(INXPoint point);

	//int GetLineSegment(const INXPoint cpPoint);
	//bool IsOnSegment(const INXPoint cpPoint, const int iSegmentNum);
	
	bool IsSegmentsAligned(const int iSegmentNum1, const int iSegmentNum2);
	
	void DrawNode(INXPoint point);

	int exist;
	INXObjArray <INXPoint*>points;
	long int othericonid;
    int otherportno;
	INXPoint otherportpoint;
	long int idNum;
	int portType; // port type line is connected to
	int dataType;
	int nodeCount;
	INXString dbgValue;
	INXRect valueRect;
	int dbgEvent;
	int lineSelected;
	long hierID; // this is to simplify mapping process in debug mode
	int show; //hides lines when moving selection boxes
private:
	bool m_bDbgMonitorSel;
	bool m_bDefineMonitor;
	unsigned int m_iRtaTraceId;
	int m_iSelSegmentNum;
/*@todo OpenGL Line Functions */
private:
	//x,y co-ordinates need for 6 points to paint a line segment
	float px1, px2, px3, px4, px5, px6;
	float py1, py2, py3, py4, py5, py6;
	//seting colors
	float r, g, b;
	//array to store co-ordinates of a bend
	float curveCoord[24];

	float x, y;
private:
	void setGLPoint(INXPoint pnt);
	void setGLColor(float _r, float _g, float _b);
	void drawGLLine(INXPoint currentPnt,INXPoint nextPoint, INXPoint endPoint);
	void setPointForGLLine(std::list< INXPoint > _pointsList);
	void drawLineSeg(float px1, float py1, float px2, float py2, float px3, 
		float py3, float px4, float py4, float px5, float py5, float px6, float py6);
	void drawBend(float x, float y, int a);
};

#endif // !defined(AFX_ICONLINES_H__C1041E81_B35E_11D8_A9B0_00055DD37FD7__INCLUDED_)
