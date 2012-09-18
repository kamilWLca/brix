#pragma once
#include "ExtFile.h"
#include "Porting_Classes/INXString.h"

class ExtTransferableFile :	public ExtFile
{
public:
	ExtTransferableFile(void);
	ExtTransferableFile(const ExtFile &file);
	~ExtTransferableFile(void);

	LucidErrEnum getTargetFileName(INXString &csFileName) const;
	LucidErrEnum setTargetFileName(const int &FileStemNumber);
	LucidErrEnum setTargetFileName(const INXString &cStr1);

protected:
	INXString m_csTargetFileName;
};
