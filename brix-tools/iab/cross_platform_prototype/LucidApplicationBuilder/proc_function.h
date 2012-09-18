// proc_function.h: interface for the proc_function class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PROC_FUNCTION_H_INCLUDED_)
#define _PROC_FUNCTION_H_INCLUDED_
/*
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
*/
#include <fstream>
#include "Porting_Classes/INXObjArray.h"
using namespace std;
//#include <afxtempl.h>

/*
This is the information for a function:
NOTE: IT may be better to make all the ports a member of this class, rather than 
mapping the function arguments to ports using the inputs and outputs variables.???

  */
class Proc_function  
{
public:
	Proc_function();
	Proc_function(istream *);
	virtual ~Proc_function();
//data	
	char func_name[256];
	int start_trig;
	INXObjArray<int> complete; //index
	INXObjArray<int> outputs;
	INXObjArray<int> inputs;
};

#endif // !defined(AFX_PROC_FUNCTION_H__55451EA2_F88D_11D8_A9B1_00055DD37FD7__INCLUDED_)
