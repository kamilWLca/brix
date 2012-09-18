#pragma once
#include "ExtTransferableFile.h"

class ExtDataFile :	public ExtTransferableFile
{
public:
	ExtDataFile(void);
	ExtDataFile(const ExtDataFile &file);
	~ExtDataFile(void);

};
