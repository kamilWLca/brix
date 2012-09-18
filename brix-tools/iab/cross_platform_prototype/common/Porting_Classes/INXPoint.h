#ifndef _INXPOINT_H
#define _INXPOINT_H

#include <wx/gdicmn.h>
#include "INXSize.h"

class INXPoint: public wxPoint{
public:

	// constructors
	INXPoint(): wxPoint(){
		x = 0;
		y = 0;
	}
	INXPoint(int _x, int _y):wxPoint(_x,_y){
	
	}

	INXPoint(const INXPoint &pnt){
		x = pnt.x;
		y = pnt.y;	
	};

	INXPoint (wxPoint &pnt){
		x = pnt.x;
		y = pnt.y;	 	
	};
	INXPoint (wxSize &sz){
		x = sz.x;
		y = sz.y;		
	};
	void Offset(int xOff, int yOff){
		x = x + xOff;
		y = y + yOff;	
	};
	void SetPoint(int setX, int setY){
		x = setX;
		y = setY;	
	};
	INXPoint operator - (const wxPoint &pnt){
		INXPoint tempPoint(x,y);
		tempPoint.x -= pnt.x;
		tempPoint.y -= pnt.y;
		return tempPoint;
	};
	INXPoint operator + (const wxPoint &pnt){
		INXPoint tempPoint(x,y);
		tempPoint.x += pnt.x;
		tempPoint.y += pnt.y;
		return tempPoint;
	};
	INXPoint operator - (const wxSize &sz){
		INXPoint tempPoint(x,y);
		tempPoint.x -= sz.x;
		tempPoint.y -= sz.y;	
		return tempPoint;
	};
	

	INXPoint operator + (const wxSize &sz){
		INXPoint tempPoint(x,y);
		tempPoint.x += sz.x;
		tempPoint.y += sz.y;	
		return tempPoint;
	};

	void operator +=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;
	};
	void operator +=(const wxSize &sz){
		x += sz.x;
		y += sz.y;
	};
	void operator -=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;	
	};
	void operator -=(const wxSize &sz){
		x += sz.x;
		y += sz.y;	
	};
};
#endif