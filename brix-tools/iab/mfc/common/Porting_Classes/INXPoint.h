#ifndef _INXPOINT_H
#define _INXPOINT_H

#pragma once
#include <wx/gdicmn.h>
#include <atltypes.h>
#include "INXSize.h"

#define LEGACYINX
class INXPoint: public wxPoint{

public:
	// @todo using temporary CPoint conversion function, to test it with the mfc code 
#ifdef LEGACYINX
	POINT _point;
#endif
	operator CPoint(){
		CPoint _cpoint(x,y);
		return _cpoint;
	};
	operator POINT(){
		POINT _cpoint;
		_cpoint.x = x;
		_cpoint.y = y;
		return _cpoint;
	};
	
	operator LPPOINT(){
		_point.x = x;
		_point.y = y;
		return &_point;
	};
	// constructors
	INXPoint(): wxPoint(){
		x = 0;
		y = 0;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	}
	INXPoint(int _x, int _y):wxPoint(_x,_y){
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	}

	INXPoint(const INXPoint &pnt){
		x = pnt.x;
		y = pnt.y;	
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	INXPoint(const CPoint &pnt){
		x = pnt.x;
		y = pnt.y;	
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	INXPoint (wxPoint &pnt){
		x = pnt.x;
		y = pnt.y;	 
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	INXPoint (wxSize &sz){
		x = sz.x;
		y = sz.y;	
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	void Offset(int xOff, int yOff){
		x = x + xOff;
		y = y + yOff;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	void SetPoint(int setX, int setY){
		x = setX;
		y = setY;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	INXPoint operator - (const wxPoint &pnt){
		INXPoint tempPoint(x,y);
		tempPoint.x -= pnt.x;
		tempPoint.y -= pnt.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
		return tempPoint;
	};
	INXPoint operator + (const wxPoint &pnt){
		INXPoint tempPoint(x,y);
		tempPoint.x += pnt.x;
		tempPoint.y += pnt.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
		return tempPoint;
	};
	INXPoint operator - (const wxSize &sz){
		INXPoint tempPoint(x,y);
		tempPoint.x -= sz.x;
		tempPoint.y -= sz.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
		return tempPoint;
	};
	

	INXPoint operator + (const wxSize &sz){
		INXPoint tempPoint(x,y);
		tempPoint.x += sz.x;
		tempPoint.y += sz.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
		return tempPoint;
	};

	void operator +=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	void operator +=(const wxSize &sz){
		x += sz.x;
		y += sz.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	void operator -=(const wxPoint &pnt){
		x += pnt.x;
		y += pnt.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif	
	};
	void operator -=(const wxSize &sz){
		x += sz.x;
		y += sz.y;
		#ifdef LEGACYINX
			_point.x = x;
			_point.y = y;
		#endif		
	};
};
#endif