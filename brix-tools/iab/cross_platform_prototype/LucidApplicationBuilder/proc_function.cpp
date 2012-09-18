// Proc_function.cpp: implementation of the Proc_function class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DrawProg.h"
#include "proc_function.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




Proc_function::Proc_function()
{

}

//NOTE: Reads in stored data for Icon function data.
Proc_function::Proc_function(istream* file)
{
    int num, data;
	int i;
	*file >> start_trig; //read in the port number for a function's start trigger
	*file >> num;        //read in the number of complete ports (this will always be one - New requirement!)
	for (i=0;i<num;i++) {
		*file >> data;
		complete.SetAtGrow(i,data);
	}
	*file >> num;        // read in the number of data input ports
	for (i=0;i<num;i++) { 
		*file >> data;
		inputs.SetAtGrow(i,data);
	}
	*file >> num;		// read in the number of data output ports
	for (i=0;i<num;i++) {
		*file >> data;
		outputs.SetAtGrow(i,data);
	}
}


Proc_function::~Proc_function()
{
 // should clear away all the dynamically allocated stuff completes,inputs,outputs
}
