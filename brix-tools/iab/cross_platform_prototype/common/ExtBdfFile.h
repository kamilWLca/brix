#pragma once
#include "ExtTransferableFile.h"

#define BDF_FILE_EXTENSION ".bdf"

class ExtBdfFile :	public ExtTransferableFile
{
public:
	ExtBdfFile(void);
	ExtBdfFile(const ExtBdfFile &file);
	~ExtBdfFile(void);
};
