#pragma once

#include "LucidConstants.h"

#include <fstream>
#include "Porting_Classes/INXString.h"
using namespace std;

class Log
{
public:
	Log(void);
public:
	~Log(void);

// methods
	bool Open(INXString csFileName);
	bool Close();
	void WriteLogMsg(const INXString csMsg);
	bool IsOpen();

// members
private:
	ofstream c_LogFile;
	bool m_bOpen;
	static FILE* c_pFile;

};
