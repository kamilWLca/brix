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

class ExtBmpFile :	public ExtTransferableFile
{
public:
	ExtBmpFile(void);
	ExtBmpFile(const ExtBmpFile &file);
	~ExtBmpFile(void);

};
