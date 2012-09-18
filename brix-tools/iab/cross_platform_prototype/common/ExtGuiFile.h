// ExtGuiFile.h: interface for the GuiFile class
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "ExtFile.h"
#include "../common/LucidTypedefs.h"
#include "LucidEnums.h"
#include "Porting_Classes/INXString.h"

class ExtGuiFile: public ExtFile
{

public:
	ExtGuiFile(void);
	ExtGuiFile(const INXString &hostFileName);
	ExtGuiFile(const ExtGuiFile &file);
	ExtGuiFile(const GuiFileEssentialData_t &fileData);


public:
	~ExtGuiFile(void);


// Attributes
protected:
	INXString m_csScreenTag;


// Methods
public:
	LucidErrEnum getScreenTag(INXString &tag) const;
	LucidErrEnum setScreenTag(const INXString &tag);


};
