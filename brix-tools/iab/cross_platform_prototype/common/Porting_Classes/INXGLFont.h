#ifndef _INXGLFONT_H
#define _INXGLFONT_H

#include "wx/wx.h"
#include <wx/glcanvas.h>
#include <FTGL/ftgl.h>

#define ARIAL_FONT "C:\\Windows\\Fonts\\arial.ttf"

class INXGLFont{
private:
	float red, green, blue, _x, _y;
	int size;
	char const* fontType;
	FTPixmapFont* font;
	int w_win, h_win;
public:
	INXGLFont(){
		red = 0;
		green = 0;
		blue = 0;
		size = 10;
		_x = 0;
		_y = 0;
		fontType = ARIAL_FONT;
	};
	void setFontColor(float r, float g, float b){
		red = r;
		green = g;
		blue = b;
	};
	void setFontSize(int _size){
		size = _size;
	};
	void setFontType(char const* _fontType){
		fontType = _fontType;
	};
	void textOut(float x, float y, char const* text){
		setUpFonts();  
		glColor3f(red, green, blue);
		glRasterPos2i(x, y);
		font->Render(text);
		delete font;
	};
private:
	void setUpFonts(){
		font = new FTPixmapFont(fontType);
		if(font->Error()){
			fprintf(stderr, "Failed to open font %s", fontType);
			exit(1);
		}
		font->FaceSize(size);
	};
};

#endif