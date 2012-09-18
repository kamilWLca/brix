// Group.h: interface for the Group class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GROUP_H_INCLUDED_)
#define _GROUP_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "Porting_Classes/INXString.h"

class Group  
{
public:
	Group();
	virtual ~Group();
	
	// Attributes
	unsigned int ID;
	INXString Name;
	unsigned int Period;
	unsigned int Alloc;
};

#endif 
