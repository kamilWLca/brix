/** \file	FileOperations.h                   
			Project: FopDemo\n
			Project type: MFC App\n
			Author: Vinnichenko Alexey\n
			E-mail: subj@mail.ru\n
			Description: Declaration of CFileOperation class and CFileExeption class.
*/ 
#ifndef _FILEOPERATIONS_H
#define _FILEOPERATIONS_H


#define PATH_ERROR			-1
#define PATH_NOT_FOUND		0
#define PATH_IS_FILE		1
#define PATH_IS_FOLDER		2

#include "Porting_Classes/INXString.h"
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
class CFExeption
{
public:
	CFExeption(unsigned int dwErrCode);
	CFExeption(INXString sErrText);
	INXString GetErrorText() {return m_sError;}
	unsigned int GetErrorCode() {return m_dwError;}

private:
	INXString m_sError;
	unsigned int m_dwError;
};
#endif

//*****************************************************************************************************

class CFileOperation
{
public:
	CFileOperation(); // constructor
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bool Delete(INXString sPathName); // delete file or folder
	bool DeleteFolderFiles(INXString sPathName); // delete file-contents of a folder
	bool Copy(INXString sSource, INXString sDest); // copy file or folder
	bool CopyFileGood(const INXString &sSourceFolder, 
				  const INXString &sSourceFile,
				  const INXString &sDestFolder, 
				  const INXString &sDestFile );

	bool Replace(INXString sSource, INXString sDest); // move file or folder
	bool Rename(INXString sSource, INXString sDest); // rename file or folder
	
	INXString GetErrorString() {return m_sError;} // return error description
	unsigned int GetErrorCode() {return m_dwError;} // return error code
	void ShowError() // show error message
		{MessageBox(NULL, m_sError, _T("Error"), MB_OK | MB_ICONERROR);}
	void SetAskIfReadOnly(bool bAsk = true) // sets behavior with readonly files(folders)
		{m_bAskIfReadOnly = bAsk;}
	bool IsAskIfReadOnly() // return current behavior with readonly files(folders)
		{return m_bAskIfReadOnly;}
	bool CanDelete(INXString sPathName); // check attributes
	void SetOverwriteMode(bool bOverwrite = false) // sets overwrite mode on/off
		{m_bOverwriteMode = bOverwrite;}
	bool IsOverwriteMode() {return m_bOverwriteMode;} // return current overwrite mode
	#endif
	int CheckPath(INXString sPath);
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bool IsAborted() {return m_bAborted;}
	INXString getExecutablePath(LPCTSTR lpFileName);
	#endif
protected:
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void DoDelete(INXString sPathName);
	void DoCopy(INXString sSource, INXString sDest, bool bDelteAfterCopy = false);
	void DoFileCopy(INXString sSourceFile, INXString sDestFile, bool bDeleteAfterCopy=false, const bool bUpdateAfterCopy=true );
	void DoFolderCopy(INXString sSourceFolder, INXString sDestFolder, bool bDelteAfterCopy = false);
	void DoRename(INXString sSource, INXString sDest);
	bool IsFileExist(INXString sPathName);
	void PreparePath(INXString &sPath);
	#endif
	void Initialize();
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void CheckSelfRecursion(INXString sSource, INXString sDest);
	bool CheckSelfCopy(INXString sSource, INXString sDest);
	INXString ChangeFileName(INXString sFileName);
	INXString ParseFolderName(INXString sPathName);
	#endif
private:
	INXString m_sError;
	unsigned int m_dwError;
	bool m_bAskIfReadOnly;
	bool m_bOverwriteMode;
	bool m_bAborted;
	int m_iRecursionLimit;
	#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void getStatus( const INXString &csFileName, CFileStatus & status );
	DBTIMESTAMP getLastUpdate( const INXString &csFileName );
	#endif
};


//*****************************************************************************************************

#endif //#ifndef _FILEOPERATIONS_H
