//#include "stdafx.h"
#include "../common/LucidTypedefs.h"
#include "ExtGuiFile.h"

ExtGuiFile::ExtGuiFile(void)
{
	m_csFilePrefix = "g";
	m_csFileExtension = ".gui";
}

ExtGuiFile::ExtGuiFile(const ExtGuiFile &egf)
{
	*this = egf;
}

ExtGuiFile::ExtGuiFile(const INXString &hostFileName)
{
	m_csHostFileName = hostFileName;
}

ExtGuiFile::ExtGuiFile(const GuiFileEssentialData_t &fileData)
{
	m_csHostFileName = fileData.fileHostNameStub;
	m_csScreenTag    = fileData.screenTag;
	m_csDescription  = fileData.fileDescr;
}

ExtGuiFile::~ExtGuiFile(void)
{
}

LucidErrEnum ExtGuiFile::getScreenTag(INXString &tag) const
{
	tag = m_csScreenTag;
	return kErr_NoErr;
}

LucidErrEnum ExtGuiFile::setScreenTag(const INXString &tag)
{
	m_csScreenTag = tag;
	return kErr_NoErr;
}
