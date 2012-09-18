//#include "stdafx.h"
#include "ExtDataFile.h"

ExtDataFile::ExtDataFile(void)
{
	m_csFilePrefix = "d";
	m_csFileExtension = ".txt";
}

ExtDataFile::ExtDataFile(const ExtDataFile &file)
{
	m_csFilePrefix = "d";
	m_csFileExtension = ".txt";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtDataFile::~ExtDataFile(void)
{
}
