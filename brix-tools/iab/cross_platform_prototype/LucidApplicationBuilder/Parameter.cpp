// Parameter.cpp: implementation of the Parameter class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "DrawProg.h"
#include "Parameter.h"
#include "ParameterStore.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Parameter::Parameter()
{
	name = "";
	dataType = -1;
	minRange = "";
	maxRange = "";
	value = "";
	description = "";
	enumValArr = new INXObjArray<INXString>;
	enumLabelArr = new INXObjArray<INXString>;
}

Parameter::Parameter(
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
	enumValArr = _enumValArr;
	enumLabelArr = _enumLabelArr;
}

Parameter & Parameter::operator=( const Parameter &rhs )
{
	name			= rhs.name;
	dataType		= rhs.dataType;
	minRange		= rhs.minRange;
	maxRange		= rhs.maxRange;
	value			= rhs.value;
	description		= rhs.description;
	for(size_t i=0;i<rhs.enumValArr->GetCount(); i++)
	enumValArr		= rhs.enumValArr;
	enumLabelArr	= rhs.enumLabelArr;

	return *this;
}

Parameter::~Parameter()
{
	delete enumValArr;
	delete enumLabelArr;
}

void Parameter::Save(ostream * file) {
	INXString enumLabel;
	// avoid saving empty strings. causes problems
	if (name == wxT("")) {name.Append(wxT("_"));}
	if (minRange == wxT("")) {minRange.Append(wxT("_"));}
	if (maxRange == wxT("")) {maxRange.Append(wxT("_"));}
	if (value == wxT("")) {value.Append(wxT("_"));}
	if (description == wxT("")) {description.Append(wxT("_"));}

	*file << (char*)name << "\t"<< dataType << "\t"<< (char*)minRange << "\t"<< (char*)maxRange << "\t" << (char*)value << "\n";
	for (int i=0; i<enumValArr->GetSize(); i++) {
		*file << (char*)enumValArr->GetAt(i) << "\t";
	}
	*file << "EndOfValues" << endl;
	for (int i=0; i<enumLabelArr->GetSize(); i++) {
		*file << (char*)enumLabelArr->GetAt(i) << "\t";
	}
	*file << "EndOfLabels" << endl;
	*file << (char*)description << endl;
	*file << endl;
}

void Parameter::copy( const ParameterStore *rhs )
{
	name = rhs->m_csName;
	dataType = rhs->m_iDataType;
	minRange = rhs->m_csMinRange;
	maxRange = rhs->m_csMaxRange;
	value = rhs->m_csValue;
	description = rhs->m_csDescription;

	enumValArr->RemoveAll();
	enumLabelArr->RemoveAll();

	for(size_t i=0;i<rhs->m_csaEnumValArr.GetCount(); i++)
		enumValArr->SetAtGrow(i,  rhs->m_csaEnumValArr.GetAt(i) );

	for(size_t i=0;i<rhs->m_csaEnumLabelArr.GetCount(); i++)
		enumLabelArr->SetAtGrow(i,  rhs->m_csaEnumLabelArr.GetAt(i) );

	return;
}

void Parameter::Load(istream * file) {
	char temp[255];

	file->ignore(1,'\n');
	file->getline(temp,1023,'\t');
	name = temp;
	*file >> dataType; 
	*file >> temp;
	minRange = temp;
	*file >> temp;
	maxRange = temp;
	file->ignore(1,'\t');
	file->getline(temp,1023);
	value = temp;
	// load drop-down values
	*file >> temp;
	while (strcmp(temp,"EndOfValues")) {
		enumValArr->Add(temp);
		*file >> temp;
	}
	// load drop-down labels
	*file >> temp;
	while (strcmp(temp,"EndOfLabels")) {
		enumLabelArr->Add(temp);
		*file >> temp;
	}
	// cannot use >> operator to retrieve description as it may contain whitespaces
	// Use getline. However need to ignore end of line character
	file->ignore(1,'\n');
	file->getline(temp,254);
	description = temp;
	
	// restore empty strings
	if (name == wxT("_")) {name.Append(wxT(""));}
	if (minRange == wxT("_")) {minRange.Append(wxT(""));}
	if (maxRange == wxT("_")) {maxRange.Append(wxT(""));}
	// don't restore default value
	//if (value == "�$") {value = "";}
	if (description == wxT("_")) {description = "";}
	// load blank line
	//*file >> temp;
}

// Function that determines whether a parameter is displayed in a textbox or a drop-down
bool Parameter::IsTextbox() {
	// screen tag
	if (dataType == 4) {
		return FALSE;
	}
	else if (enumValArr->GetSize() == 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

// Function that returns an enumerated Label for a given enumerated value
INXString Parameter::GetEnumLabel(INXString enumVal) {
	INXString ret = "";

	for (int i=0; i<enumValArr->GetSize(); i++) {
		if (enumValArr->GetAt(i) == enumVal) {
			// avoid overflow error
			if (enumLabelArr->GetSize() >= i+1) {
				ret = enumLabelArr->GetAt(i);
			}
		}
	}
	return ret;
}

// Function that returns an enumerated value for a given enumerated label
INXString Parameter::GetEnumVal(INXString enumLabel) {
	INXString ret = "";

	for (int i=0; i<enumLabelArr->GetSize(); i++) {
		if (enumLabelArr->GetAt(i) == enumLabel) {
			// avoid overflow error
			if (enumValArr->GetSize() >= i+1) {
				ret = enumValArr->GetAt(i);
			}
		}
	}
	return ret;
}
