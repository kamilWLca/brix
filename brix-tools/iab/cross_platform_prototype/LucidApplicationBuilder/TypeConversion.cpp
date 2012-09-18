// TypeConversion.cpp: implementation of the TypeConversion class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "DrawProg.h"
#include "TypeConversion.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TypeConversion::TypeConversion()
{

}

TypeConversion::~TypeConversion()
{

}

// Function that converts the data type to a char
char TypeConversion::DataType2Char(int dataType) {

	char c;

	switch(dataType) {
		case 0:
			c = 'B'; break;
		case 1:
			c = 'I'; break;
		case 2:
			c = 'F'; break;
		case 3:
			c = 'S'; break;
		default:
			c = 'X'; break;
	}
	return c;
}

// Converts the data type to an int
int TypeConversion::DataType2Int(char dataType) {
	int i;

	if (dataType == 'B') {
		i = 0;
	}
	else if (dataType == 'I') {
		i = 1;
	}
	else if (dataType == 'F') {
		i = 2;
	}
	else if (dataType == 'R') {
		i = 2;
	}
	else if (dataType == 'S') {
		i = 3;
	}
	else if (dataType == 'E') {
		i = 4;
	}
	else {
		i = -1;
	}
	return i;
}

// Converts the data type to an int
int TypeConversion::DataType2Int(INXString dataType) {
	int i;

	if (dataType == wxT("B")) {
		i = 0;
	}
	else if (dataType == wxT("I")) {
		i = 1;
	}
	else if (dataType == wxT("F")) {
		i = 2;
	}
	else if (dataType == wxT("R")) {
		i = 2;
	}
	else if (dataType == wxT("S")) {
		i = 3;
	}
	else if (dataType == wxT("E")) {
		i = 4;
	}
	else {
		i = -1;
	}
	return i;
}
