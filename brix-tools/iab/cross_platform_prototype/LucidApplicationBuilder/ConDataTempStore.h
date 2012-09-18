// ConDataTempStore.h: interface for the Coninfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ParameterStore.h"

#include "../common/LucidEnums.h"
#include "../common/LucidConstants.h"
#include "condat.h"
#include "Porting_Classes/INXSize.h"
#include "Porting_Classes/INXString.h"

class ConDataTempStore  
{

public:
	ConDataTempStore();
	~ConDataTempStore();

	int m_iParamNum;
	int m_iUserDefined;

	INXString	m_csType; //graphic filename
	INXString m_csBlock; // block name for encapsulated blocks. graphic filename is encapsulate
	INXString m_csDescription;
	INXString m_csLongDesc;
	INXString m_csClassName;
	INXString m_csOptionstring;
	INXSize m_cSize;

	int m_iInstNum;
	INXString m_csHierarchyName;
	bool m_bSaveSelectedFlag;
	INXRect m_cRectangle; /// body part of icon

	ParameterStore* m_pParamStoreArr[MAXPARAMS];

	void loadFromProject( const ConData &source );
	void commitToProject( ConData *dest );

};