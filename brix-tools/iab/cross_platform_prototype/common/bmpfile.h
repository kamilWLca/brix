//#include <windef.h>  
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXPoint.h"
#include "Porting_Classes/INXSize.h"
#include <wx/glcanvas.h>

#ifndef _BITMAP_H
#define _BITMAP_H

typedef unsigned char BYTE;
typedef long LONG;
typedef unsigned int UINT;
typedef unsigned long ULONG;

class Bitmap //  : public CObject
{

typedef struct inxBITMAPINFOHEADER{
        unsigned long      biSize;
        long       biWidth;
        long       biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;


public:
Bitmap();
~Bitmap(void);

INXSize Init(INXString FileName) ;
//int Draw(CDC* pDC, CPoint point);
int DrawGL(INXPoint point);
long  GetWidth(); 
long  GetHeight(); 

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
