// port.h: icon port information
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PORT_H_INCLUDED_)
#define _PORT_H_INCLUDED_
/*
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000*/

#include "bmpfile.h" //@todo
#include "IconLines.h"
#include "LucidConstants.h" //@todo
#include "LucidString.h" //@todo
#include "Porting_Classes/INXRect.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXObject.h"

class Port  : public INXObject
{
public:
	Port(int _userdefined); 
	//Port(INXPoint _P, unsigned int _portNum, int _dataType, int _portType, INXString _description, INXObjArray<INXString>* _funcName, INXObjArray<unsigned int>* _funcArg, int _atomicFlag, bool bVerticalIn, int _userdefined, int _mandatoryFlag);
	//Port(UINT _portNum, int _portType, INXObjArray<INXString>* _funcName, INXObjArray<unsigned int>* _funcArg, int _atomicFlag);
	virtual ~Port();
	INXPoint P;   // position
	int porttype;   //0 input, 1 output, 2 Trig input, 3 Trig output
	bool bPortVertical;
	int datatype;   //0 bool, 1 int, 2 float, 3 string 
	int portNum; // port number
	INXString description;
	INXObjArray<INXString>* funcName;
	INXObjArray<unsigned int>* funcArg;
	int connected;
	IconLines line; //this is the input line (one only allowed)
	long int location;
	long int lineID; //this is for output ports. needed to write out SODL
	// Indicates if port is connected to a xport. Required for subset encapsulation.
	// Ensures that an output with multiple connections only has one xport.
	// Ensures inputs only have one connection
	int xportConnected;
	// Required for subset encapsulation. When an input is connected to an output that has a fanout 
	// greater than 1 only want to create 1 xport. Therefore, when it is created store its id in the
	// output port it is connected to.
	long xportID;
	int initialise;
	INXSize bitmapSize;
	INXSize bitmHighSize;
	Bitmap bitmap;
	Bitmap bitmHighlight;
	INXRect rectangle;
	int atomicFlag;
	bool mandatoryFlag;
	INXString tag;
	unsigned int  groupID;
	int userdefined;
	//nb - DEP::swapPortData() copies all the data from one por to another SO if add more attributes here, be sure to include it in this method - there is a bug raised to improve this architecture

// Methods
	void init();
	int OnPort(INXPoint);  //accepts coordinated and returns 0 for not close amd then an int> 0 for closeness
	int addLine(INXPoint *point,long int othericon,int otherPortNum, int otherPortType); // adds a line to inputs only otherwise return an error -1
	//int RemoveLine();
	//int DisconnectLine();
	//void Draw(CDC* theDC);
	//void Draw(CDC* theDC, bool _onlyDrawAnim, int _toggleAnim);
	void DrawGL();
	void DrawGL( bool _onlyDrawAnim, int _toggleAnim);
	//int Move(INXPoint point);
	//void setLineID(long int _lineID);
	void Save(ostream *);
	void Load(istream *);
	//CObject* OtherIconID;

private:
	INXRect GetPortBitmapArea();
	void drawGLPort(INXRect _rect, int portState);
	void setGLPortColor(float red, float green, float blue);
private:
	float r, g, b;
};

#endif // !defined(AFX_PORT_H__9E27DF60_B038_11D8_A9B0_00055DD37FD7__INCLUDED_)
