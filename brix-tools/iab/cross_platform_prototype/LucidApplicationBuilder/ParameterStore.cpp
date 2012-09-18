// ParameterStore.cpp: implementation of the ParameterStore class.
//
//////////////////////////////////////////////////////////////////////

//#include "DrawProg.h"

#include "ParameterStore.h"
#include "Parameter.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParameterStore::ParameterStore()
{
}
/*
ParameterStore::ParameterStore(
					 INXString _name, 
					 int _dataType, 
					 INXString _minRange, 
					 INXString _maxRange, 
					 INXString _defaultVal, 
					 INXString _description, 
					 INXObjArray<INXString>* _enumValArr, 
					 INXObjArray<INXString>* _enumLabelArr
					 )
{
	name = _name;
	dataType = _dataType;
	minRange = _minRange;
	maxRange = _maxRange;
	value = _defaultVal;
	description = _description;

	for(size_t i=0;i<_enumValArr->GetCount(); i++)
		enumValArr.SetAtGrow(i,  (INXString)_enumValArr->GetAt(i) );

	for(size_t i=0;i<_enumLabelArr->GetCount(); i++)
		enumLabelArr.SetAtGrow(i,  (INXString)_enumLabelArr->GetAt(i) );

}
*/

void ParameterStore::copy( const Parameter *rhs )
{
	m_csName			= rhs->name;
	m_iDataType			= rhs->dataType;
	m_csMinRange		= rhs->minRange;
	m_csMaxRange		= rhs->maxRange;
	m_csValue			= rhs->value;
	m_csDescription		= rhs->description;

	m_csaEnumValArr.RemoveAll();
	m_csaEnumLabelArr.RemoveAll();

	for(size_t i=0;i<rhs->enumValArr->GetCount(); i++)
		m_csaEnumValArr.SetAtGrow(i,  (INXString)rhs->enumValArr->GetAt(i) );

	for(size_t i=0;i<rhs->enumLabelArr->GetCount(); i++)
		m_csaEnumLabelArr.SetAtGrow(i,  (INXString)rhs->enumLabelArr->GetAt(i) );

}

ParameterStore::~ParameterStore()
{
}

// Function that determines whether a parameter is displayed in a textbox or a drop-down
bool ParameterStore::IsTextbox() {
	// screen tag
	if (m_iDataType == 4) {
		return FALSE;
	}
	else if (m_csaEnumValArr.GetSize() == 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

// Function that returns an enumerated Label for a given enumerated value
INXString ParameterStore::GetEnumLabel(INXString enumVal) {
	INXString ret = "";

	for (int i=0; i<m_csaEnumValArr.GetSize(); i++) {
		if (m_csaEnumValArr.GetAt(i) == enumVal) {
			// avoid overflow error
			if (m_csaEnumValArr.GetSize() >= i+1) {
				ret = m_csaEnumValArr.GetAt(i);
			}
		}
	}
	return ret;
}

// Function that returns an enumerated value for a given enumerated label
INXString ParameterStore::GetEnumVal(INXString enumLabel) {
	INXString ret = "";

	for (int i=0; i<m_csaEnumLabelArr.GetSize(); i++) {
		if (m_csaEnumLabelArr.GetAt(i) == enumLabel) {
			// avoid overflow error
			if (m_csaEnumLabelArr.GetSize() >= i+1) {
				ret = m_csaEnumLabelArr.GetAt(i);
			}
		}
	}
	return ret;
}