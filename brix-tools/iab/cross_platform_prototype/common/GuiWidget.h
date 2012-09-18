// GuiWidget.h: interface for the GuiWidget class
//
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "stdafx.h"
#include "LucidEnums.h"
#include "Porting_Classes/INXString.h"

class GuiWidget
{
public:
	GuiWidget(void);
	GuiWidget(const INXString &widgetTag, const INXString &screenTag );
	~GuiWidget(void);


	bool operator==(const GuiWidget &gw) const;


// Attributes
private:
	INXString m_csWidgetTag;
	INXString m_csScreenTag;

// Methods
public:
	LucidErrEnum getWidgetTag(INXString &csWidgetTag) const;
	LucidErrEnum getScreenTag(INXString &csScreenTag) const;
	LucidErrEnum setWidgetTag(const INXString &csWidgetTag);
	LucidErrEnum setScreenTag(const INXString &csScreenTag);


};
