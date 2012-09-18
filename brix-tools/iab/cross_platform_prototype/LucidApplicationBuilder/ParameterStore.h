// ParameterStore.h: interface for the ParameterStore class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
//#include "Parameter.h"
//using namespace std;
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXObject.h"
class Parameter;

class ParameterStore : public INXObject  
{
public:
	// constructors
	ParameterStore();

	bool IsTextbox();

	ParameterStore(	INXString _name, 
				int _dataType, 
				INXString _minRange, 
				INXString _maxRange, 
				INXString defaultVal, 
				INXString _description, 
				INXObjArray<INXString>* _enumValArr, 
				INXObjArray<INXString>* _enumLabelArr);

	virtual ~ParameterStore();
	
	void copy( const Parameter *rhs );

	// attributes
	INXString m_csName;
	INXString m_csDescription;
	int m_iDataType; //parameter data type @tod the types here need to be enumerated type 5 is for tags info
	INXString m_csMinRange;
	INXString m_csMaxRange;
	INXString m_csValue;
	INXObjArray<INXString> m_csaEnumValArr;
	INXObjArray<INXString> m_csaEnumLabelArr;

	INXString GetEnumLabel(INXString enumVal);
	INXString GetEnumVal(INXString enumLabel);

};
