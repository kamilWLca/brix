//#include "stdafx.h"
#include "ExtBdfFile.h"

ExtBdfFile::ExtBdfFile(void)
{
	m_csFilePrefix = "f";
	m_csFileExtension = ".bdf";
}

ExtBdfFile::ExtBdfFile(const ExtBdfFile &file)
{
	m_csFilePrefix = "f";
	m_csFileExtension = ".bdf";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtBdfFile::~ExtBdfFile(void)
{
}
