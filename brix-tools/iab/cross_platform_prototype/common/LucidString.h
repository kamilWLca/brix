#pragma once
#include "Porting_Classes/INXString.h"
// CLucidString

class CLucidString : public INXString
{

public:
	CLucidString();
	virtual ~CLucidString();

	double GetWidthAt(int nIndex);
	CLucidString& operator=(char* lp);
};


