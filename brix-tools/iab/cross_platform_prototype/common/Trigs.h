#ifndef __TRIGS
#define __TRIGS

//#include <classlib\thread.h>
#include <afx.h>
//#include "setups.h"
#include "../ASCShared/arcboard.h"
#include <math.h>

class Trigs {
public:
	int ok;
int handle;
unsigned char sig_in;
unsigned char sig_out;


int init()  ;										
int read() ;
int write(int num,int state) ;
int setvec(char *vec) ;
int reset(int i) ;
};
#endif