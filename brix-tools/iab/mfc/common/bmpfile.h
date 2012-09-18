#include <windef.h>  
#include "Porting_Classes/INXString.h"
#include <wx/glcanvas.h>

#ifndef BITMAP
#define BITMAP
class Bitmap //  : public CObject
{

public:
Bitmap();
~Bitmap(void);

CSize Init(INXString FileName) ;
int Draw(CDC* pDC, CPoint point);
int DrawGL(CDC* pDC, CPoint point);
LONG  GetWidth(); 
LONG  GetHeight(); 

private:
BYTE * MakeDwordAlignedBuf(BYTE *dataBuf);
									 //UINT widthPix,				// pixels!!
									 //UINT height,
									 //UINT *uiOutWidthBytes)	;	// bytes!!!;

BYTE * LoadBMP(INXString fileName);  //UINT *width, UINT *height,BYTE* planes,BYTE *bp
//openGL draw temporary function block
void drawGLRect(float x, float y, float width, float height);


private:
	BITMAPINFOHEADER bmiHeader;
	BYTE * data;

};

#endif