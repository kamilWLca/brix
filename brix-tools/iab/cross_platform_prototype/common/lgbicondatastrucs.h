
#pragma once

typedef struct  {

	CString type;
	CString tag;
	int xTopLft;
	int yTopLft;
	int width;
	int height;
	int location;
	int zPos;

} LgbIconEssentialData_t;

typedef struct  {

	CString csClass;
	CString csFont;

	short alpha;
	short red;
	short green;
	short blue;

} LgbTextStyleEssentialData_t;

typedef struct  {

	LgbIconEssentialData_t basics;

	short fgAlpha;
	short bgAlpha;

	short	bgRed;
	short 	bgGreen;
	short 	bgBlue;
	short 	fgRed;
	short 	fgGreen;
	short 	fgBlue;

	CString csFont;
	int leftIndent;
	int rightIndent;
	int topIndent;
	int bottomIndent;
	int lineSpacing;

} LgbTextIconEssentialData_t;

typedef struct  {

	LgbIconEssentialData_t basics;

	short bgAlpha;

	short	bgRed;
	short 	bgGreen;
	short 	bgBlue;

} LgbPatchIconEssentialData_t;

typedef struct  {

	LgbIconEssentialData_t basics;

	short alpha;

	CString bitmapFileName;

	short lockAspect;

} LgbImageIconEssentialData_t;
