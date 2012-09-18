#include "stdafx.h"
#include"..\ASCShared\Trigs.h"

int Trigs::init()  {
	ok=0;
handle=ARCBOARD_Init();
if (handle!=AN_SUCCESS) {
	AfxMessageBox("Can't open IO card");
	return 0;
}


handle=PCIO8_Init(0x180);
if (handle!=AN_SUCCESS) {
	AfxMessageBox("Can't open IO card");
	return 0;
}
else {
	//int z=PCIO8_OutputsEnable(0x180,AN_ENABLE ); // Assume it works for now	
	write(0,0);
	write(1,0);
	write(2,0);
	write(3,0);
	write(4,0);
	write(5,0);
	write(6,0);
	write(7,0);
	ok=1;
	return 1;

}

return 0;
}
											
int Trigs::read() {
	if (!ok) return 0;
PCIO8_DigInputGroupRead (0x180,&sig_in);

return sig_in;
}

int Trigs::write(int num,int state) { // start bit 0
	if (!ok) return 0;
	char _state=(char)state;
	char bit=1;
	if (_state) _state=(char)255;
	bit=bit<<num;
	if (_state) sig_out=sig_out|(((bit)&_state));
	else sig_out=sig_out&(((255-bit)|_state));
	//sig_out=255-sig_out;
return PCIO8_DigOutputGroupWrite (0x180,255-sig_out);

}










int Trigs::setvec(char *vec) {
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


int Trigs::reset(int i) {
sig_in=0;
sig_out=0;
//for (i=0;i<256;i++) {
// sigs[i]=0;
//}
return 0;
}




