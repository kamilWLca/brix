#ifndef _INXRECT_H
#define _INXRECT_H

#include <wx/gdicmn.h>
#include "INXPoint.h"
#include "INXSize.h"

#ifndef max
  #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

class INXRect: public wxRect{
typedef struct tagINXRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} INXRECT; 
typedef const INXRECT *INXPRECT;

public:
	int left;
	int right;
	int top;
	int bottom;
public:
	INXRect(){
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		left = x;
		top = y;
		right = width + x;
		bottom = height + y;
	};
	// constructors
	/* @todo check if the left, right, top and bottom, are the same as x,y,width ahd height of wxWidgets*/
	INXRect(int _left, int _top, int _right, int _bottom):
		wxRect(_left, _top, _right - _left,  _bottom - _top){
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	};
	INXRect(INXPoint pos, INXSize size){
		left = pos.x;
		top = pos.y;
		right = size.cx;
		bottom = size.cy;
		x = left;
		y = top;
		width = right - x;
		height = bottom - y;
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
	};
	INXRect(INXPoint &_topLeft, INXPoint &_bottomRight){
		left = _topLeft.x;
		top = _topLeft.y;
		right = _bottomRight.x;
		bottom = _bottomRight.y;
		x = left;
		y = top;
		width = right - x;
		height = bottom - y;
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
	};
	void MoveToXY(INXPoint pnt){
		x = pnt.x;
		y = pnt.y;
		left = x;
		top = y;
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
	};
	void OffsetRect(int _x, int _y ){
		x += _x;
		y += _y;
		left = x;
		top = y;
	};
	void OffsetRect(INXPoint &pnt){
		x += pnt.x;
		y += pnt.y;
		left = x;
		top = y;
	};

	bool PtInRect(wxPoint pnt){
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