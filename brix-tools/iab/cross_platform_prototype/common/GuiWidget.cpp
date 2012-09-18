//#include "stdafx.h"
#include "GuiWidget.h"

GuiWidget::GuiWidget(void)
{
}

GuiWidget::GuiWidget(const INXString &widgetTag, const INXString &screenTag ):
m_csScreenTag(screenTag), m_csWidgetTag(widgetTag)
{
}

GuiWidget::~GuiWidget(void)
{
}

LucidErrEnum GuiWidget::getWidgetTag(INXString &csWidgetTag) const
{
	csWidgetTag = m_csWidgetTag;
	return kErr_NoErr;
}

LucidErrEnum GuiWidget::getScreenTag(INXString &csScreenTag) const
{
	csScreenTag = m_csScreenTag;
	return kErr_NoErr;
}

LucidErrEnum GuiWidget::setWidgetTag(const INXString &csWidgetTag)
{
	m_csWidgetTag = csWidgetTag;
	return kErr_NoErr;
}

LucidErrEnum GuiWidget::setScreenTag(const INXString &csScreenTag)
{
	m_csScreenTag = csScreenTag;
	return kErr_NoErr;
}

bool GuiWidget::operator==(const GuiWidget &gw) const
{
	INXString csWidgetTag, csScreenTag;

	bool b1 = (m_csWidgetTag==gw.m_csWidgetTag);
	bool b2 = (m_csScreenTag==gw.m_csScreenTag);

	return b1&&b2;
}