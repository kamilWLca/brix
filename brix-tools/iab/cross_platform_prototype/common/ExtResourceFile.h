#pragma once
#include "ExtTransferableFile.h"

class ExtResourceFile :	public ExtTransferableFile
{
public:
	ExtResourceFile(void);
	ExtResourceFile(const ExtResourceFile &file);
	~ExtResourceFile(void);

};
