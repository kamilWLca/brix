//#include "stdafx.h"
#include "LucidConstants.h"
#include "ExtTransferableFile.h"
#define sprintf_s(buffer, buffer_size, stringbuffer, number) (sprintf(buffer, stringbuffer, number))

ExtTransferableFile::ExtTransferableFile(void){
	m_csTargetFileName = "";
}

ExtTransferableFile::ExtTransferableFile(const ExtFile &file)
{
	file.getDescription(m_csDescription);
	file.getHostFileName(m_csHostFileName);
}

ExtTransferableFile::~ExtTransferableFile(void)
{ }

LucidErrEnum ExtTransferableFile::getTargetFileName(INXString &csFileName) const
{
	csFileName = m_csTargetFileName;
	return kErr_NoErr;
}

LucidErrEnum ExtTransferableFile::setTargetFileName(const int &fileStemNumber)
{

	INXString cStr1;
	char buffer[FILEBUFFER_SIZE];
	// 1st the number -count part of the file.
	sprintf_s( buffer, FILEBUFFER_SIZE, "%07d", fileStemNumber  );
	cStr1 = buffer;

	// Now build up to the actual target filename
	cStr1 = m_csFilePrefix + cStr1 + m_csFileExtension;
	m_csTargetFileName = cStr1;
	return kErr_NoErr;
}

LucidErrEnum ExtTransferableFile::setTargetFileName(const INXString &cStr1)
{
	m_csTargetFileName = cStr1;
	return kErr_NoErr;
}
