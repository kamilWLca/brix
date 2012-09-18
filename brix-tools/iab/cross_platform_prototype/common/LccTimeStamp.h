#pragma once

//#include "stdafx.h"
//#include <oledb.h>
#include "Porting_Classes/INXString.h"
#include "GlobalFuncs_2.h"
#define LCCTIMESTAMP_ANCIENTDATE "0000-00-00 00:00:00:000000000"

class LccTimeStamp:public DBTIMESTAMP
{
public:
	LccTimeStamp(void);
public:
	~LccTimeStamp(void);

	bool isNewerThan( const DBTIMESTAMP &ts2 );

	LccTimeStamp& operator=(const DBTIMESTAMP &timeStamp);

	INXString csPrint( );//const DBTIMESTAMP &ts );

	void setAncient();

	void csParse( INXString &cs );

};
