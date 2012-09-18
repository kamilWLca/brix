#pragma once
#include "ExtTransferableFile.h"

class ExtNlsFile :	public ExtTransferableFile
{
public:
	ExtNlsFile(void);
	ExtNlsFile(const ExtNlsFile &file);
	~ExtNlsFile(void);
};
