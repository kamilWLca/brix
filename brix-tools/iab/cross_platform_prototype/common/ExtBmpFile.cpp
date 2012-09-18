//#include "stdafx.h"
#include "../common/ExtBmpFile.h"

ExtBmpFile::ExtBmpFile(void)
{
	m_csFilePrefix = "b";
	m_csFileExtension = ".bmp";
}

ExtBmpFile::ExtBmpFile(const ExtBmpFile &file)
{
	m_csFilePrefix = "b";
	m_csFileExtension = ".bmp";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtBmpFile::~ExtBmpFile(void)
{
}
