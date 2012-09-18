//#include "stdafx.h"
#include "ExtNlsFile.h"

ExtNlsFile::ExtNlsFile(void)
{
	m_csFilePrefix = "n";
	m_csFileExtension = ".csv";
}

ExtNlsFile::ExtNlsFile(const ExtNlsFile &file)
{
	m_csFilePrefix = "n";
	m_csFileExtension = ".csv";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtNlsFile::~ExtNlsFile(void)
{
}
