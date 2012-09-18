#ifndef _INXRECT_H
#define _INXRECT_H

#pragma once
#include <wx/gdicmn.h>
#include <atltypes.h>

#include "INXPoint.h"
#include "INXSize.h"


#define LEGACYINX

class INXRect: public wxRect{

#ifdef LEGACYINX
	POINT _point;
	RECT _rect;
#endif

public:
	int left;
	int right;
	int top;
	int bottom;

public:
operator CRect(){
CRect _crect(left, top, right, bottom);
return _crect;
};
operator CPoint(){
CPoint _cpoint(x,y);
return _cpoint;
};
operator LPPOINT(){
	_point.x = x;
	_point.y = y;
return &_point;
};
operator LPRECT(){
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
	return &_rect;
};

// constructors
INXRect(){
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	left = x;
	top = y;
	right = width + x;
	bottom = height + y;
#ifdef LEGACYINX
	_point.x = 0;
	_point.y = 0;
	_rect.left = 0;
	_rect.top = 0;
	_rect.right = 0;
	_rect.bottom = 0;
#endif
};
/* @todo check if the left, right, top and bottom, are the same as x,y,width ahd height of wxWidgets*/
INXRect(int _left, int _top, int _right, int _bottom):
	wxRect(_left, _top, _right - _left,  _bottom - _top){
	left = _left;
	top = _top;
	right = _right;
	bottom = _bottom;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
}
INXRect(INXPoint pos, INXSize size){
	left = pos.x;
	top = pos.y;
	right = size.cx;
	bottom = size.cy;
	x = left;
	y = top;
	width = right - x;
	height = bottom - y;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
}
INXRect(RECT &rect){
	x = rect.left;
	y = rect.top;
	width = rect.right - x;
	height = rect.bottom - y;
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
};
INXRect(wxRect &rect){
	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	left = x;
	top = y;
	right = width + x;
	bottom = height + y;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
};
INXRect(LPCRECT lpRect){
	left = lpRect->left;
	top = lpRect->top;
	right = lpRect->right;
	bottom = lpRect->bottom;
	x = left;
	y = top;
	width = right - x;
	height = bottom - y;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
};
INXRect(INXPoint &_topLeft, INXPoint &_bottomRight){
	left = _topLeft.x;
	top = _topLeft.y;
	right = _bottomRight.x;
	bottom = _bottomRight.y;
	x = left;
	y = top;
	width = right + x;
	height = bottom + y;
#ifdef LEGACYINX
	_point.x = left;
	_point.y = top;
	_rect.left = left;
	_rect.top = top;
	_rect.right = right;
	_rect.bottom = bottom;
#endif
};
//operators

INXPoint& BottomRight(){
		return *(INXPoint *)this;
};
INXPoint CenterPoint(){
	INXPoint tempPoint(x + (width/2),y + (height/2));
	return tempPoint;
};
void CopyRect(wxRect &rect){
	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	left = x;
	top = y;
	right = width + x;
	bottom = height + y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
void DeflateRect(int _x, int _y){
left += _x;
right -= _x;
top += _y;
bottom -= _y;
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
void DeflateRect(int _left, int _top, int _right, int _bottom){
left += _left;
right -= _right;
top += _top;
bottom -= _bottom;
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
bool EqualRect(wxRect &rect){
bool temp = false;
if(*this == rect) temp = true;
return temp;
};
void InflateRect(int _x, int _y){
left -= _x;
right += _x;
top -= _y;
bottom += _y;
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
void InflateRect(int _left, int _top, int _right, int _bottom){
left -= _left;
right += _right;
top -= _top;
bottom += _bottom;
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
int Height(){
return height;
};
bool IntersectRect(wxRect rect1, wxRect rect2){
bool temp = false;
if(rect1.Intersects(rect2)){
temp = true;
*this = rect1.Intersect(rect2);
}
left = x;
top = y;
right = width + x;
bottom = height + y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
return temp;
};
bool IsRectEmpty(){
bool temp = false;
if ((left >= right) && (top >= bottom))
temp = true;
return temp;
};
void MoveToXY(int _x, int _y){
x = _x;
y = _y;
left = x;
top = y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
#endif
};
void MoveToXY(INXPoint pnt){
x = pnt.x;
y = pnt.y;
left = x;
top = y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
#endif
};
void NormalizeRect(){
int temp;
if (left > right){
temp = left;
left = right;
right = temp;
}
if (top > bottom){
temp = top;
top = bottom;
bottom = temp;
}
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
void OffsetRect(int _x, int _y ){
x += _x;
y += _y;
left = x;
top = y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
#endif
};
void OffsetRect(INXPoint &pnt){
x += pnt.x;
y += pnt.y;
left = x;
top = y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
#endif
};
bool PtInRect(INXPoint pnt){
bool temp = false;
if ((pnt.x >= left )&&(pnt.x < right )&&
        (pnt.y >= top)&&(pnt.y < bottom))temp = true;
return temp;
};
void SetRect(int _left, int _top, int _right, int _bottom){
left = _left;
top = _top;
right = _right;
bottom = _bottom;
x = _left;
y = _top;
width = _right - x;
height = _bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
void SetRectEmpty(){
left = 0;
top = 0;
right = 0;
bottom = 0;
x = 0;
y = 0;
width = 0;
height = 0;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
};
INXSize Size(){
INXSize tempSize(width,height);
return tempSize;
};
INXPoint& TopLeft(){
return *((INXPoint *)this + 1);
};
int Width(){
return width;
};
bool UnionRect(INXRect rect1,INXRect rect2){
left = min(rect1.left, rect2.left);
top = min(rect1.top, rect2.top);
right = max(rect1.right, rect2.right);
bottom = max(rect1.bottom, rect2.bottom);
x = left;
y = top;
width = right - x;
height = bottom - y;
#ifdef LEGACYINX
_point.x = left;
_point.y = top;
_rect.left = left;
_rect.top = top;
_rect.right = right;
_rect.bottom = bottom;
#endif
return true;
};

INXRect operator - (const INXPoint &pnt){
INXRect tempRect(x,y,width,height);
tempRect.x -= pnt.x;
tempRect.y -= pnt.y;
return tempRect;
};
INXRect operator + (const INXPoint &pnt){
INXRect tempRect(x,y,width,height);
tempRect.x += pnt.x;
tempRect.y += pnt.y;
return tempRect;
};
};

#endif