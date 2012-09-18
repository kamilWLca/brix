#ifndef _INXSIZE_H
#define _INXSIZE_H


#include <wx/gdicmn.h> 
#include "INXPoint.h"

class INXSize: public wxSize{

public:
	int cx, cy;
	// constructors
	INXSize(): wxSize(){
		x = 1;
		y = 1;
		cx = x;
		cy = y;
	};
	INXSize(int width, int height):wxSize(width ,height){
		cx = x;
		cy = y;	
	}	
	INXSize (wxSize &sz){
		x = sz.x;
		y = sz.y;
		cx = x;
		cy = y;
	};
	INXSize (wxPoint &pnt){
		x = pnt.x;
		y = pnt.y;	
		cx = x;
		cy = y;	
	};
	void operator +=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;
		cx = x;
		cy = y;
	};
	void operator +=(const wxSize &sz){
		x += sz.x;
		y += sz.y;
		cx = x;
		cy = y;	
	};
	void operator -=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;
		cx = x;
		cy = y;	
	};
	void operator -=(const wxSize &sz){
		x += sz.x;
		y += sz.y;
		cx = x;
		cy = y;	
	};
};
#endif