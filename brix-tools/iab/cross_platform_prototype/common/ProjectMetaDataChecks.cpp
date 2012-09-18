#include "stdafx.h"
#include "ProjectMetaData.h"

#include "../LucidApplicationBuilder/ProjectDialog.h"

#include "Markup.h"
#include "LucidConstants.h"
#include "LucidEnums.h"
#include "../LucidApplicationBuilder/FileOperations.h"
#include "ExtGuiFile.h"
#include "ExtBmpFile.h"
#include "ExtDataFile.h"
#include "LabLgbBaseApp.h"
#include "LucidTypedefs.h"
#include "LccPmdXfers.h"
#include "GlobalFuncs_2.h"
#include <fstream>
#include <stdio.h>
#include <cassert>
#include <algorithm>

#define RADIX_TEN 10 // for converting ints to strings
#define DUMMY_BUFFER_SIZE 50

using namespace std;

bool ProjectMetaData::checkProjectFileData(void)
{

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Check that placeholder.png is present:
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	bool bGotPlaceHolder = false;
	INXString csDum;
	std::vector < ExtPngFile > pngFiles;
	getPngFiles( pngFiles );

	for(size_t i=0;i<pngFiles.size();i++){
		pngFiles[i].getHostFileName( csDum);
		if(csDum == ICON_PLACEHOLDER_FILE ) 
			bGotPlaceHolder = true;
	}

	// Add placeholder.png to project file, and copy it physically from installation to project
	if(!bGotPlaceHolder){
		copyDummyBmp();
	}


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Check that 'active' png's have been set for each screen tag
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Get list of all screen tags.

	std::vector < INXString > tags;
	getScreenTags( tags );

	// For each tag, get active layout name, and check it is present.

	TagProjMetaSupportData_t tagDat;
	LucidErrEnum err;
	ExtGuiFile eGF;

	for(size_t i=0;i<tags.size(); i++){

		err = getScreenTagMetas( tags[i], tagDat );
		assert(kErr_NoErr == err);

		if( tagDat.activeHostFilename == "" ){
			tagDat.activeHostFilename = GUI_LAYOUT_DEFAULT_FILENAMEPREFIX + tags[i];
		}

		updateScreenTag( tags[i], tagDat );

		eGF.setHostFileName( tagDat.activeHostFilename );

		if( !fileInProject(eGF) ){

			GuiFileEssentialData_t essDat;
			essDat.fileDescr = GUI_LAYOUT_DEFAULT_DESCRIPTION;
			essDat.fileHostNameStub = tagDat.activeHostFilename;
			essDat.screenTag = tags[i];
			const bool addPhysically = true;

			err = addGuiFile( essDat, addPhysically );
			assert(kErr_NoErr == err);
		}

	}

	return false;
}
/*

void ProjectMetaData::initProjFolderMinder()
{
	m_cProjFolderMinder.setProjectFolder(m_csProjectDir);
	m_cProjFolderMinder.setProjectRevision( m_iCodeMajorRev, m_iCodeMinorRev );
}

INXString ProjectMetaData::getVersionString(void)
{
	return intToString(m_iCodeMajorRev) + "." + intToString(m_iCodeMinorRev);
}

void ProjectMetaData::getVersionInts(void)
{
	assert( m_csLucidVersion.GetLength() >=3 ); // 'i.j' min )

	int dotPos = m_csLucidVersion.Find(".");
	assert(dotPos > 0);

	m_iProjReadMajorRev = _ttoi( m_csLucidVersion.Left(dotPos) );
	m_iProjReadMinorRev = _ttoi( m_csLucidVersion.Right(m_csLucidVersion.GetLength() - dotPos -1 ) );

}
*/

LucidErrEnum ProjectMetaData::folderStructureNotOk() //INXString csProjectDir
{

	bool keepGoing = true;
	CFileOperation fo;
	INXString csMissingFolder;

	LucidErrEnum returnErr = kErr_InvalidFolderStructure;

	// Check sub-directories, and fix them if they are fixable.
	while( keepGoing ){

		returnErr = m_cProjFolderMinder.assessProjectFolderStructure( csMissingFolder );

		if( kErr_NoErr!=returnErr ){

			if( SODLDIR == csMissingFolder ){

				if(kErr_NoErr !=m_cProjFolderMinder.moveSodl_1() ){

					INXString failDum = "NB. Automatic Update attempted to move SODL folder\nfrom:\n";
					failDum+= m_csProjectDir;
					failDum += "\nto\n";
					failDum += m_csProjectDir + SODLDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;

				}

			} else if( EXPORTDIR == csMissingFolder ){

				if(kErr_NoErr !=m_cProjFolderMinder.moveExports_1() ){

					INXString failDum = "NB. Automatic Update attempted to move exports folder\nfrom:\n";
					failDum+= m_csProjectDir;
					failDum += "\nto\n";
					failDum += m_csProjectDir + EXPORTDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;

				}

			} else if( TEMPDIR == csMissingFolder ){

				if(kErr_NoErr !=m_cProjFolderMinder.createTemp() ){

					INXString failDum = "NB. Automatic Update attempted to move exports folder\nfrom:\n";
					failDum+= m_csProjectDir;
					failDum += "\nto\n";
					failDum += m_csProjectDir + EXPORTDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;

				}

			} else if( LGB_TEMPDIR == csMissingFolder ){

				if(kErr_NoErr !=m_cProjFolderMinder.createLgbTemp() ){

					INXString failDum = "NB. Automatic Update attempted to move exports folder\nfrom:\n";
					failDum+= m_csProjectDir;
					failDum += "\nto\n";
					failDum += m_csProjectDir + EXPORTDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;

				}
			} else if( NLSDIR == csMissingFolder ){
				if (kErr_NoErr != m_cProjFolderMinder.CreateNlsDir()) {

					INXString failDum = "NB. Automatic Update attempted to create NLS folder in:\n";
					failDum += m_csProjectDir + NLSDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;
				}				
			} else if( RESOURCEDIR == csMissingFolder ){
				if (kErr_NoErr != m_cProjFolderMinder.CreateResourceDir()) {

					INXString failDum = "NB. Automatic Update attempted to create Resource folder in:\n";
					failDum += m_csProjectDir + RESOURCEDIR;
					failDum += ".\nThis action failed:\nSeek support from nCapsa";

					AfxMessageBox( failDum ); 

					keepGoing = false;
				}
			} else {

				// we don't know how to fix the folders - better to bomb out than enter an endless loop.
				AfxMessageBox("Project has an outdated folder structure,\nattempts to fix which have failed.\nSeek Support from nCapsa");
				assert(1==0);
			}

		} else {// if(err)

			// err==kErr_NoErr, so we're ok to leave loop
			keepGoing = false;
		}

	} // while(kErr_NoErr!=err)

	// Copy the placeholder image into place
	copyDummyBmp();

	return returnErr;

}


void ProjectMetaData::copyDummyBmp()
{
	// Now copy the dummy.bmp file from the instllation dir to the DeviceData\GUI dir
	// as this will be needed when the user starts using LGB.
	CFileOperation fileOp;

	try {

		INXString installnDir;
		CLabLgbBaseApp *baseApp = (CLabLgbBaseApp *) AfxGetApp();
		baseApp->GetInstallationBaseDir(installnDir);

		INXString sourceDir = installnDir + IMAGEDIR;
		INXString sourceFile = ICON_PLACEHOLDER_FILE;

		if( fileOp.CheckPath( sourceDir + sourceFile) != PATH_IS_FILE  ){
			AfxMessageBox( "Problem with provision of installation 'placeholder' file - seek assistance!" );
			assert(1==0);
		}

		INXString destDir;
		getProjectDir(destDir);
		destDir += GUIDIR;
		INXString destFile = ICON_PLACEHOLDER_FILE;


		if( fileOp.CheckPath( destDir + destFile) != PATH_IS_FILE  ){
			fileOp.CopyFileGood(sourceDir, sourceFile, destDir, destFile );
		}

	}catch ( CFExeption(sError)  ){

		// For some reason, this catch doesn' work, as the handling of the exception's
		// INXString thows its own exception.

		AfxMessageBox( "Problem with 'placeholder' bitmap - seek assistance!" );
		assert(1==0);
	}

	ExtPngFile pngFile;
	pngFile.setHostFileName(ICON_PLACEHOLDER_FILE);
	addPngFile( pngFile );

}

