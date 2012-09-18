/** @file ExtTransferableFile.h
 * 
 * @author: Matthew Rutherford
 * @version: $Revision: 577 $
 * @date: $Date$
 * 
 * Copyright (c) Ncapsa Ltd, 2007. All rights reserved.
 */
#pragma once
#include "ExtTransferableFile.h"

class ExtPngFile :	public ExtTransferableFile
{
public:
	ExtPngFile(void);
	ExtPngFile(const ExtPngFile &file);
	~ExtPngFile(void);

};
