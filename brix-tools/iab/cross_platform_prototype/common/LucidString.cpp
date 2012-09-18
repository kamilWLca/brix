// LucidString.cpp : implementation file
//

#include "LucidString.h"


// CLucidString
CLucidString::CLucidString()
{

}

CLucidString::~CLucidString()
{
}

double CLucidString::GetWidthAt(int nIndex)
{
	char c;
	double ret;

	// The following values were obtained by observing width of the characters in LAB
	c = this->GetAt(nIndex);
	switch (c) {
		case 'a' : ret = 5.4; break;
		case 'b' : ret = 5.4; break;
		case 'c' : ret = 5.4; break;
		case 'd' : ret = 5.4; break;
		case 'e' : ret = 5.4; break;
		case 'f' : ret = 4.5; break;
		case 'g' : ret = 5.4; break;
		case 'h' : ret = 5.4; break;
		case 'i' : ret = 2.5; break;
		case 'j' : ret = 2.5; break;
		case 'k' : ret = 5.4; break;
		case 'l' : ret = 2.5; break;
		case 'm' : ret = 8.5; break;
		case 'n' : ret = 5.4; break;
		case 'o' : ret = 5.4; break;
		case 'p' : ret = 5.4; break;
		case 'q' : ret = 5.4; break;
		case 'r' : ret = 4; break;
		case 's' : ret = 5.4; break;
		case 't' : ret = 3.5; break;
		case 'u' : ret = 5.4; break;
		case 'v' : ret = 6.5; break;
		case 'w' : ret = 7; break;
		case 'x' : ret = 5.4; break;
		case 'y' : ret = 6.5; break;
		case 'z' : ret = 5; break;

		case 'A' : ret = 7; break;
		case 'B' : ret = 6.9; break;
		case 'C' : ret = 7.5; break;
		case 'D' : ret = 7.5; break;
		case 'E' : ret = 7; break;
		case 'F' : ret = 7; break;
		case 'G' : ret = 7.5; break;
		case 'H' : ret = 7.5; break;
		case 'I' : ret = 3.5; break;
		case 'J' : ret = 5.8; break;
		case 'K' : ret = 7; break;
		case 'L' : ret = 5.6; break;
		case 'M' : ret = 7.5; break;
		case 'N' : ret = 7.5; break;
		case 'O' : ret = 7.5; break;
		case 'P' : ret = 7; break;
		case 'Q' : ret = 7.5; break;
		case 'R' : ret = 7.5; break;
		case 'S' : ret = 7; break;
		case 'T' : ret = 5.6; break;
		case 'U' : ret = 7.5; break;
		case 'V' : ret = 7; break;
		case 'W' : ret = 9.5; break;
		case 'X' : ret = 5.6; break;
		case 'Y' : ret = 7.5; break;
		case 'Z' : ret = 7; break;

		case '-' : ret = 3.5; break;
		case '(' : ret = 3.5; break;
		case ')' : ret = 3.5; break;
		case ' ' : ret = 2.5; break;

		default: ret = 5.4; break;
	}

	return ret;
}

CLucidString& CLucidString::operator=(char* lp)
{
      INXString::operator=(lp);
      return *this;
}
