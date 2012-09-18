//#include "stdafx.h"
#include "LucidConstants.h"
#include "RevisionGuru.h"
#include <cassert>

RevisionGuru::RevisionGuru(void)
{ }

RevisionGuru::~RevisionGuru(void)
{ }

void RevisionGuru::getProjFolderStructure(	const int &majorRev, 
								const int &minorRev,
								RevisionGuru::FolderFileDef_t &folderDef  )
{

	int index=0;

	if(majorRev ==1){

		if( (minorRev==2) || (minorRev==3) ){

			// Folders

			folderDef.folders.relPath[index++] =	DEPDIR;

			folderDef.folders.relPath[index++] =	DEVICEDATADIR;
			folderDef.folders.relPath[index++] =	DDATADIR;
			folderDef.folders.relPath[index++] =	GUIDIR;
			folderDef.folders.relPath[index++] =	SODLDIR;

			folderDef.folders.relPath[index++] =	EXPORTDIR;

			folderDef.folders.relPath[index++] =	TEMPDIR;
			folderDef.folders.relPath[index++] =	LGB_TEMPDIR;

			folderDef.folders.nFolders = index;

			// Files
			folderDef.files.nFiles = 0;
		}
		else if (minorRev == 4) {
			// Folders

			folderDef.folders.relPath[index++] =	DEPDIR;

			folderDef.folders.relPath[index++] =	DEVICEDATADIR;
			folderDef.folders.relPath[index++] =	DDATADIR;
			folderDef.folders.relPath[index++] =	GUIDIR;
			folderDef.folders.relPath[index++] =	NLSDIR;
			folderDef.folders.relPath[index++] =	RESOURCEDIR;
			folderDef.folders.relPath[index++] =	SODLDIR;

			folderDef.folders.relPath[index++] =	EXPORTDIR;

			folderDef.folders.relPath[index++] =	TEMPDIR;
			folderDef.folders.relPath[index++] =	LGB_TEMPDIR;

			folderDef.folders.nFolders = index;

			// Files
			folderDef.files.nFiles = 0;
		}
	} else {

		//assert(1==0);
	}
}
