// Parameter.h: interface for the Parameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PARAMETER_H_INCLUDED_)
#define _PARAMETER_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "ParameterStore.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXObject.h"

#include <fstream>
using namespace std;


class ParameterStore;

class Parameter : public INXObject  
{
public:
	// constructors
	Parameter();

	Parameter(	INXString _name, 
				int _dataType, 
				INXString _minRange, 
				INXString _maxRange, 
				INXString defaultVal, 
				INXString _description, 
				INXObjArray<INXString>* _enumValArr, 
				INXObjArray<INXString>* _enumLabelArr);

	virtual ~Parameter();

	Parameter & operator=( const Parameter &rhs );	
	void copy( const ParameterStore *rhs );

	// methods
	void Save(ostream *);
	void Load(istream *);
	bool IsTextbox();

	INXString GetEnumLabel(INXString enumVal);
	INXString GetEnumVal(INXString enumLabel);
	
	// attributes
	INXString name;
	INXString description;
	int dataType;
	INXString minRange;
	INXString maxRange;
	INXString value;

	INXObjArray<INXString>* enumValArr;
	INXObjArray<INXString>* enumLabelArr;

};

#endif // !defined(AFX_PARAMETER_H__184F839C_89B7_45D7_AEE9_9102F8EE2472__INCLUDED_)
