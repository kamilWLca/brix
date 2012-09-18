#pragma once
#include "Porting_Classes/INXString.h"

//! Knows all about project- & installation-folders, and lpj-file format, for various revisions of code.
/*! Categorizes these according to a major and minor revision.
    Serves these up to any client who wants to know */
class RevisionGuru
{
public:

//! General holder for the names of folders
	struct FolderNames_t {
		INXString relPath[10];
		int nFolders; 
	};

//! General holder for the names of files
	struct FileNames_t {	
		INXString relPath[50];
		int nFiles;
	} ;

//! Packages a folder- an a file-holder
	struct FolderFileDef_t{	
		FolderNames_t folders; 
		FileNames_t files;
	} ;

	RevisionGuru(void);
	~RevisionGuru(void);

	//! Returns the full-path to all folders that should be under the project folder.
	/*! ie laods these into folderDef.  String could contain a slash ie ''DeviceData\SODL', 
		as well as just (say) 'DeviceData' */
	void getProjFolderStructure(	const int &majorRev, 
									const int &minorRev,
											FolderFileDef_t &folderDef );


};
