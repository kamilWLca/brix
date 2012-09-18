//#include "stdafx.h"
#include "ExtResourceFile.h"

ExtResourceFile::ExtResourceFile(void)
{
	m_csFilePrefix = "d";
	m_csFileExtension = ".txt";
}

ExtResourceFile::ExtResourceFile(const ExtResourceFile &file)
{
	m_csFilePrefix = "d";
	m_csFileExtension = ".txt";
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
	file.getTargetFileName(m_csTargetFileName);
}

ExtResourceFile::~ExtResourceFile(void)
{
}
