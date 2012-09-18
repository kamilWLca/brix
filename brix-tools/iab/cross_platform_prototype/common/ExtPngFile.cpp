//#include "stdafx.h"
#include "../common/ExtPngFile.h"

ExtPngFile::ExtPngFile(void)
{
	m_csFilePrefix = "p";
	m_csFileExtension = ".png";
}

ExtPngFile::ExtPngFile(const ExtPngFile &file)
{
	m_csFilePrefix = "p";
	m_csFileExtension = ".png";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtPngFile::~ExtPngFile(void)
{
}
