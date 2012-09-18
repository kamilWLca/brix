#ifndef __TRIGS
#define __TRIGS

//#include <classlib\thread.h>
#include <afx.h>
//#include "setups.h"
#include "arcom.h"
#include "math.h"

class Trigs {
public:
int handle;
char sig_in;
char sig_out;


int init()  {
	
handle=COMMON_boardinit(0x180,ID_PCIO8);
if (handle<0) {
	AfxMessageBox("Can't open IO card");
	return 0;
}

else {
	DIGITAL_group_enable(handle); // Assume it works for now	
	//write(0,0);
	//write(1,0);
	//write(2,0);
	//write(3,0);
	//write(4,0);
	//write(5,0);
	//write(6,0);
	//write(7,0);
	return 1;

}
	
}
											
int read() {
sig_in=DIGITAL_read(handle,0);

return sig_in;
}

int write(int num,int state) { // start bit 0
	char _state=(char)state;
	char bit=1;
	if (_state) _state=255;
	bit=bit<<num;
	if (_state) sig_out=sig_out|(((bit)&_state));
	else sig_out=sig_out&(((255-bit)|_state));
	//sig_out=255-sig_out;
return DIGITAL_write(handle,0,255-sig_out);

}










int setvec(char *vec) {
	if (sig_in&1) vec[0]=0;
	else vec[0]=1;
	if (sig_in&2) vec[1]=0;
	else vec[1]=1;
	if (sig_in&4) vec[2]=0;
	else vec[2]=1;
	if (sig_in&8) vec[3]=0;
	else vec[3]=1;
	if (sig_in&16) vec[4]=0;
	else vec[4]=1;
	if (sig_in&32) vec[5]=0;
	else vec[5]=1;
	if (sig_in&64) vec[6]=0;
	else vec[6]=1;
	if (sig_in&128) vec[7]=0;
	else vec[7]=1;	  
return sig_in;

}


int reset(int i) {
sig_in=0;
sig_out=0;
//for (i=0;i<256;i++) {
// sigs[i]=0;
//}
return 0;
}

};



class KadekTrigs : public Trigs  {

int get_cam_trigs() {	
return 0;
}

int reset_cam_trigs() {
return 0;
}

int get_label_trig()  {
return 0;
}

int reset_label_trig()  {
return 0;
}

};




#endif