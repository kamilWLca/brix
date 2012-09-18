//#pragma once
//#include "stdafx.h"

#include "LucidEnums.h"
#include "LucidConstants.h"
#include "RevisionGuru.h"
#include "../FileOperations.h"
#include "ProjFolderMinder.h"
#include <wx/msgdlg.h>
#include <wx/filename.h>

ProjFolderMinder::ProjFolderMinder()
{
}

void ProjFolderMinder::setProjectFolder( const INXString &projFolder )
{
	m_csProjDir = projFolder;
}


void ProjFolderMinder::setProjectRevision( const int &projMajorRev, const int &projMinorRev )
{
	m_iCodeMajorRev = projMajorRev;
	m_iCodeMinorRev = projMinorRev;
}


LucidErrEnum ProjFolderMinder::createProjectFolderStructure( )
{

	CFileOperation fo;

	// when overwriting existing project delete old one.
	if (fo.CheckPath(m_csProjDir)) {
		#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
			fo.Delete(m_csProjDir);
		#endif
	}

	wxFileName createDirectoryTemp(m_csProjDir);
		createDirectoryTemp.Mkdir(0x777);	
	// Now get list folders hat should be under proj folder.

	RevisionGuru cGuruInst;;

	RevisionGuru::FolderFileDef_t folderDef;
	cGuruInst.getProjFolderStructure( m_iCodeMajorRev, m_iCodeMinorRev, folderDef  );


	// Create directories

	for( int i=0; i<folderDef.folders.nFolders;i++ ){	
		wxFileName createDirectoryTemp(m_csProjDir + folderDef.folders.relPath[i] );
		createDirectoryTemp.Mkdir(0x777);	
	}

	return kErr_NoErr;
}

LucidErrEnum ProjFolderMinder::assessProjectFolderStructure( INXString &csMissingFolder )
{

	RevisionGuru cGuruInst;;

	RevisionGuru::FolderFileDef_t folderDef;
	cGuruInst.getProjFolderStructure( m_iCodeMajorRev, m_iCodeMinorRev, folderDef  );

	CFileOperation fo;

	// Check directories exist

	for( int i=0; i<folderDef.folders.nFolders;i++ ){
		if (!fo.CheckPath(m_csProjDir + folderDef.folders.relPath[i] )) {
			csMissingFolder = folderDef.folders.relPath[i];
			return kErr_InvalidFolderStructure;
		}
	}

	return kErr_NoErr;
}


LucidErrEnum ProjFolderMinder::moveSodl_1(void)
{

	CFileOperation fo;
	bool bSucceed;

	// Create a new sodl folder in the right place, if necessary
	if( PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + SODLDIR )){
		wxFileName createDirectoryTemp(m_csProjDir + SODLDIR );
		createDirectoryTemp.Mkdir(0x777);		
	}

	// An old 'exports' may contain  sodl file - if so, move it to the right place.
	if( PATH_IS_FILE == fo.CheckPath( m_csProjDir + wxT("\\exports\\t.sdl") )  ){
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
		bSucceed = fo.CopyFileGood( m_csProjDir + "\\exports\\", SODLFILENAME, 
										m_csProjDir + SODLDIR, SODLFILENAME );
#endif
	}

	// Now delete the old sodl folder
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
		bSucceed = fo.Delete( m_csProjDir + "\\SODL\\" );
#endif
	INXString okDum = "NB. Automatic Project-Folder Structure Update!\nSODL folder and file has been moved, from:\n";
	okDum+= m_csProjDir;
	okDum += wxT("\nto\n");
	okDum += m_csProjDir + DEVICEDATADIR;
	okDum += wxT(".\n\nNB. This internal format change requires no user action");

	wxMessageBox( okDum ); 

	return kErr_NoErr;
}


LucidErrEnum ProjFolderMinder::moveExports_1(void)
{

	CFileOperation fo;
	bool bSucceed;

	// Create a new exports folder in the right place, if necessary
	if( PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + EXPORTDIR )){
		wxFileName createDirectoryTemp(m_csProjDir + EXPORTDIR );
		createDirectoryTemp.Mkdir(0x777);
	}

	// An old 'exports' may contain  sodl file - if so, move it to the right place.
	if( PATH_IS_FILE == fo.CheckPath( m_csProjDir + wxT("\\exports\\t.sdl") )  ){
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
		bSucceed = fo.CopyFileGood( m_csProjDir + "\\exports\\", SODLFILENAME, 
										m_csProjDir + SODLDIR, SODLFILENAME );
#endif
	}

	// Now delete the old exports folder
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bSucceed = fo.Delete( m_csProjDir + "\\exports\\" );
#endif
	INXString okDum = "NB. Automatic Project-Folder Structure Update!\nexports folder and file has been moved, from:\n";
	okDum+= m_csProjDir;
	okDum += wxT("\nto\n");
	okDum += m_csProjDir + DDATADIR;
	okDum += wxT(".\n\nNB. This internal format change requires no user action");

	wxMessageBox( okDum ); 

	return kErr_NoErr;
}


LucidErrEnum ProjFolderMinder::createTemp(void)
{

	CFileOperation fo;

	// Create a new 'temp' folder in the right place, if necessary
	if( PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + TEMPDIR )){

	wxFileName createDirectoryTemp(m_csProjDir + TEMPDIR );
		if (createDirectoryTemp.Mkdir(0x777)) 
			return kErr_NoErr;
		else
			return kErr_CouldNotCreateFolder;

	}else{

		return kErr_NoErr;
	}
}

LucidErrEnum ProjFolderMinder::createLgbTemp(void)
{

	CFileOperation fo;

	// Create a new 'temp' folder in the right place, if necessary
	if( PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + LGB_TEMPDIR )){

	wxFileName createDirectoryTemp(m_csProjDir + LGB_TEMPDIR );
		if (createDirectoryTemp.Mkdir(0x777)) 
			return kErr_NoErr;
		else
			return kErr_CouldNotCreateFolder;

	}else{

		return kErr_NoErr;
	}
}

LucidErrEnum ProjFolderMinder::CreateNlsDir(void)
{
	CFileOperation fo;

	// Create a new 'NLS' folder in the right place, if necessary
	if (PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + NLSDIR)) {
		wxFileName createDirectoryTemp(m_csProjDir + NLSDIR);
		if (createDirectoryTemp.Mkdir(0x777)) {
			return kErr_NoErr;
		}
		else {
			return kErr_CouldNotCreateFolder;
		}

	} else {
		return kErr_NoErr;
	}
}

LucidErrEnum ProjFolderMinder::CreateResourceDir(void)
{
	CFileOperation fo;

	// Create a new 'NLS' folder in the right place, if necessary
	if (PATH_NOT_FOUND == fo.CheckPath( m_csProjDir + RESOURCEDIR)) {
		wxFileName createDirectoryTemp(m_csProjDir + RESOURCEDIR);
		if (createDirectoryTemp.Mkdir(0x777)) {
			return kErr_NoErr;
		}
		else {
			return kErr_CouldNotCreateFolder;
		}

	} else {
		return kErr_NoErr;
	}
}
