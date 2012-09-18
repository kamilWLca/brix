
#include <wx/msgdlg.h>
#include <errno.h>
#include <stdio.h>


#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define BI_RGB 0L

#include "bmpfile.h" 

Bitmap::Bitmap(void)
{
	data = NULL;
}

Bitmap::~Bitmap(void)
{
	if (data) {
		delete data;
	}
}

INXSize Bitmap::Init(INXString FileName) {
		//int x=bmpWidth[i];
		//int y=bmpHieght[i];
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//bmiHeader.biWidth = x;
		//bmiHeader.biHeight = y;
		bmiHeader.biPlanes = 1;
		bmiHeader.biBitCount = 24;
		bmiHeader.biCompression = BI_RGB;
		bmiHeader.biSizeImage = 0;
		bmiHeader.biXPelsPerMeter = 0;
		bmiHeader.biYPelsPerMeter = 0;
		bmiHeader.biClrUsed = 0;
		bmiHeader.biClrImportant = 0;
		BYTE* bitmapbuf=LoadBMP(FileName);
		if (!bitmapbuf) return INXSize(-1,-1);
		data = MakeDwordAlignedBuf(bitmapbuf);
		// set up a DIB 
		//int a=((CRect*) bitmapRect[i])->TopLeft().x;
		//int b=((CRect*) bitmapRect[i])->TopLeft().y;
		//	int c=((CRect*) bitmapRect[i])->Width();
		//	int d=((CRect*) bitmapRect[i])->Height()
		delete [] bitmapbuf;
		return INXSize(bmiHeader.biWidth-1,bmiHeader.biHeight-1);
}

int Bitmap::DrawGL(INXPoint point) {
	//temporary draw of the GL rectangles as the function blocks
	drawGLRect((float)point.x,(float)point.y,(float)GetWidth(),(float)GetHeight());
	return 0;
}
void Bitmap::drawGLRect(float x, float y, float width, float height){
		glBegin(GL_POLYGON);
		glColor3f(0.7, 0.5, 1); // light purple
		glVertex2f(x, y);
		glVertex2f(x, y + height);
		glVertex2f(x + width, y + height);
		glVertex2f(x + width, y);
		glEnd();
};
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
int Bitmap::Draw(CPoint point) {
	//BYTE *tmp=NULL;
	// unsigned int-align for display
	UINT parma=bmiHeader.biWidth;
	UINT parmb;
	if (!theDC->IsPrinting()) {
		parmb=bmiHeader.biHeight;
	}
	else {
		parmb=-bmiHeader.biHeight;
	}
	UINT left =point.x;//topleft.x;//(clientRect.Width() - x) / 2;
	UINT top = point.y;//topleft.y;//(clientRect.Height() - y) / 2;
	int liness = StretchDIBits(theDC->m_hDC,
		left,top ,
		parma,
		parmb,
		0,0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		data,
		(LPBITMAPINFO)&bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);
//	if (tmp) delete [] tmp; // done drawing bitmap
	return 0;
}

#endif
LONG Bitmap::GetWidth(){

	return bmiHeader.biWidth;
}

LONG Bitmap::GetHeight(){

	return bmiHeader.biHeight;
}

BYTE * Bitmap::MakeDwordAlignedBuf(BYTE *dataBuf)		// bytes!!!
									 //UINT widthPix,				// pixels!!
									 //UINT height, UINT *uiOutWidthBytes
									
{
	////////////////////////////////////////////////////////////
	// what's going on here? this certainly means trouble 
	if (dataBuf==NULL)
		return NULL;
	unsigned int widthPix=bmiHeader.biWidth;
	unsigned int height=bmiHeader.biHeight;


	////////////////////////////////////////////////////////////
	// how big is the smallest unsigned int-aligned buffer that we can use?
	unsigned int uiWidthBytes;
	uiWidthBytes = WIDTHBYTES(widthPix * 24);

	unsigned long dwNewsize=(unsigned long)((unsigned long)uiWidthBytes * 
							(unsigned long)height);
	BYTE *pNew;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	pNew=(BYTE *)new BYTE[dwNewsize];
	if (pNew==NULL) {
		return NULL;
	}
	
	////////////////////////////////////////////////////////////
	// copy row-by-row
	UINT uiInWidthBytes = widthPix * 3;
	UINT uiCount;
	for (uiCount=0;uiCount < height;uiCount++) {
		BYTE * bpInAdd;
		BYTE * bpOutAdd;
		ULONG lInOff;
		ULONG lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	return pNew;
}


BYTE* Bitmap::LoadBMP(INXString fileName)
//UINT *width, UINT *height,BYTE* planes,BYTE *bp
{
	typedef struct 
	{
		BYTE m1,m2;		//Ascii 2-byte "BM"
		long filesize;	//Total length of file 4-byte
		short res1,res2;//Reserved 4-byte
		long pixoff;	//pixel-offset 4-byte
		long bmisize;   //size of data header 4-byte (40)                
    	UINT w;			//width of bitmap 4-byte
		UINT h;			//height of bitmap 4-byte
		BYTE planes;	//# of colour planes 2-byte (1)
		BYTE bp;		//# of bits per pixel 2-byte (1..32)
		long compression;	//compression 4-byte 
		unsigned long sizeimage; //stored pixel data 4-byte
		long xscale, yscale;	//width,height of resolution in pixels per meter
		long colors;			//# of colours used 4-byte
		long impcol;			//# of important colours 4-byte
	}BITMAP_DATA;
	
	BITMAP_DATA b;	//scope local
	b.w = 0;		//initialise width and height
	b.h = 0;
	
	long m_bytesRead=0;
	BYTE* outBuf;
	int pixelSize = 0;
	
	// for safety
	//*width=0; *height=0;
	FILE *fp;
	
	fp = fopen(fileName,"rb");

	if (fp == NULL) {
		wxMessageBox(wxT("Can't open file for reading :\n")+fileName);
		return NULL;
	} else {
	    long rc;
		rc=fread((BYTE  *)&(b.m1),1,1,fp); m_bytesRead+=1;
		if (rc==-1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((BYTE  *)&(b.m2),1,1,fp); m_bytesRead+=1;
		if (rc==-1) wxMessageBox(wxT("Read Error!"));
		if ((b.m1!='B') || (b.m2!='M')) {
			wxMessageBox(wxT("Not a valid BMP File"));
			fclose(fp);
			return NULL;
        }
        
 		rc=fread((long  *)&(b.filesize),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((int  *)&(b.res1),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((int  *)&(b.res2),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.pixoff),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.bmisize),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&b.w,4,1,fp);	 m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}
		rc=fread((long  *)&b.h,4,1,fp); m_bytesRead+=4;

		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}
		rc=fread((short  *)&b.planes,2,1,fp); m_bytesRead+=2;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}


		rc=fread((short  *)&b.bp,2,1,fp); m_bytesRead+=2;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&b.compression,4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.sizeimage),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.xscale),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}
		rc=fread((long  *)&(b.yscale),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.colors),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		rc=fread((long  *)&(b.impcol),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {wxMessageBox(wxT("Read Error!")); fclose(fp); return NULL;}

		if (b.compression!=BI_RGB) {
	    	wxMessageBox(wxT("This is a compressed file."));
	    	fclose(fp);
	    	return NULL;
	    }

		if (b.colors == 0) {
			b.colors = 1 << b.bp;
		}


		// read colormap

		RGBQUAD *colormap = NULL;

		switch (b.bp) {
		case 24:
			break;
			// read pallete 
		case 1:
		case 4:
		case 8:
			colormap = new RGBQUAD[b.colors];
			if (colormap==NULL) {
				fclose(fp);
				wxMessageBox(wxT("Out of memory"));
				return NULL;
			}

			int i;
			for (i=0;i<b.colors;i++) {
				BYTE r,g,b, dummy;

				rc=fread((BYTE *)&(b),1,1,fp);
				m_bytesRead++;
				if (rc!=1) {
					wxMessageBox(wxT("Read Error!"));
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				rc=fread((BYTE  *)&(g),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					wxMessageBox(wxT("Read Error!"));
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				rc=fread((BYTE  *)&(r),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					wxMessageBox(wxT("Read Error!"));
					delete [] colormap;
					fclose(fp);
					return NULL;
				}


				rc=fread((BYTE  *)&(dummy),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					wxMessageBox(wxT("Read Error!"));
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				colormap[i].rgbRed=b;
				colormap[i].rgbGreen=g;
				colormap[i].rgbBlue=r;
			}
			break;
		}


		if ((long)m_bytesRead>b.pixoff) {
			fclose(fp);
			wxMessageBox(wxT("Corrupt palette"));
			delete [] colormap;
			fclose(fp);
			return NULL;
		}

		while ((long)m_bytesRead<b.pixoff) {
			char dummy;
			fread(&dummy,1,1,fp);
			m_bytesRead++;
		}

		// set the output params
		//*width=w;
		//*height=h;

		if (b.bp==24) {
			pixelSize = 3;
		} else if (b.bp==32) {
			pixelSize = 4;
		} else {
			wxMessageBox(wxT("Error - Bitmap not 24 or 32 bit"));
		}
		// alloc our buffer
		long row_size = b.w * pixelSize;
		long bufsize = (long)b.w * pixelSize * (long)b.h;

		outBuf=(BYTE *) new BYTE [bufsize];
		if (outBuf==NULL) {
			wxMessageBox(wxT("Memory alloc Failed"));
		} else {

			//	read it

			unsigned long row=0;
			long rowOffset=0;

			// read rows in reverse order
			for (row=0;row<b.h;row++) {

				// which row are we working on?
				rowOffset=(long unsigned)row*b.w*3; // width * 3, not row_size;

				if ((b.bp==24) || (b.bp==32)) {

					for (UINT col=0;col<b.w;col++) {
						long offset = col * 3; // 3, not pixelSize;
						unsigned char pixel[3];	// note - only allocating for 24bit rgb, ignores alpha channel in 32bit

						if (fread((void  *)(pixel),1,pixelSize,fp)==pixelSize) {
							if ( row == 100 ) {
								row = 100;
							}
							printf("pixel 0 1 2 3 %c %c %c %c\n", pixel[0], pixel[1], pixel[2], pixel[3]);
						
							*(outBuf + rowOffset + offset + 0)=pixel[0];
							*(outBuf + rowOffset + offset + 1)=pixel[1];
							*(outBuf + rowOffset + offset + 2)=pixel[2];
						}
				
					}

					m_bytesRead+=row_size;
					
					// read unsigned int padding
					while ((m_bytesRead-b.pixoff)&pixelSize) {
						char dummy;
						if (fread(&dummy,1,1,fp)!=1) {
							wxMessageBox(wxT("Read Error1"));
							delete [] outBuf;
							fclose(fp);
							return NULL;
						}

						m_bytesRead++;
					}
 
					
				} else {	// 1, 4, or 8 bit image

					// pixels are packed as 1 , 4 or 8 bit vals. 

					int bit_count = 0;
					UINT mask = (1 << b.bp) - 1;

					BYTE inbyte=0;

					for (UINT col=0;col<b.w;col++) {
						
						int pix=0;

						// if we need another byte
						if (bit_count <= 0) {
							bit_count = 8;
							if (fread(&inbyte,1,1,fp)!=1) {
								wxMessageBox(wxT("Read Error2"));
								delete [] outBuf;
								delete [] colormap;
								fclose(fp);
								return NULL;
							}
							m_bytesRead++;
						}

						// keep track of where we are in the bytes
						bit_count -= b.bp;
						pix = ( inbyte >> bit_count) & mask;
///*8 bit adjustment
						*(outBuf + rowOffset + col * 3 + 2) = colormap[pix].rgbBlue;
						*(outBuf + rowOffset + col * 3 + 1) = colormap[pix].rgbGreen;
						*(outBuf + rowOffset + col * 3 + 0) = colormap[pix].rgbRed;
//*/

					}

					// read unsigned int padding
					while ((m_bytesRead-b.pixoff)&3) {
						char dummy;
						if (fread(&dummy,1,1,fp)!=1) {
							wxMessageBox(wxT("Read Error3"));
							delete [] outBuf;
							if (colormap)
								delete [] colormap;
							fclose(fp);
							return NULL;
						}
						m_bytesRead++;
					}
				}
			}
		
		}

		if (colormap) {
			delete [] colormap;
		}

		fclose(fp);

		bmiHeader.biWidth = b.w;
		bmiHeader.biHeight = b.h;

    }
	return outBuf;
}
