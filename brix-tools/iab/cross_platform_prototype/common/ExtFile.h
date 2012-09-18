/** @file ExtFile.h
 * 
 * @author: Matthew Rutherford
 * @version: $Revision: 577 $
 * @date: $Date$
 * 
 * Copyright (c) Ncapsa Ltd, 2007. All rights reserved.
 */

#pragma once

#include "LucidEnums.h"
#include "Porting_Classes/INXString.h"

//! The base class for representing external files in LAB and LGB projects
class ExtFile
{
public:
	ExtFile(void);
	ExtFile(const ExtFile &file);

	~ExtFile(void);

// Attributes

protected:
	INXString m_csHostFileName;	//!< The file name on the host (PC)
	INXString m_csDescription;	//!< User's description of the file
	INXString m_csFilePrefix;		//!< 
	INXString m_csFileExtension;

// Methods
public:
	bool operator==(const ExtFile &file) const;
	LucidErrEnum getHostFileName(INXString &csFileName) const;
	LucidErrEnum setHostFileName(const INXString &csFileName);

	LucidErrEnum getDescription(INXString &csDescr) const;
	LucidErrEnum setDescription(const INXString &csDescr);

};
