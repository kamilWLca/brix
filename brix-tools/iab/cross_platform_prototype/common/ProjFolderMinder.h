#pragma once
//#include "stdafx.h"

#include "LucidEnums.h"
#include "Porting_Classes/INXString.h"
class ProjectMetaData;

//!author: M Rutherford.  Copyright nCapsa ltd 2007.
//! A utility class, designed as a member of ProjectMetaData, used to manage projct folders
/*! Must be initialised with project folder, under which it will inspect/create etc the folders,
and with the major and minor revisions of the folders to be inspected.
NB. These would normally be the present *code's* major and minor revisions, although they
might be the project data's revisions, if he project were old, as in the following scenario:

	Open an old project file (say 1.1).
	Inspect the proj folders against 1.1 format, to check that what is expected to be there is there.
	Assuming it is all there, move / delete / create the folders of the project to render it up-to-date
	with the code's revision.
*/
class ProjFolderMinder
{

public:
	ProjFolderMinder();

	void setProjectFolder( const INXString &projFolder );
	void setProjectRevision( const int &projMajorRev, const int &projMinorRev );

	LucidErrEnum assessProjectFolderStructure( INXString &csMissingFolder );
	LucidErrEnum createProjectFolderStructure( );
	LucidErrEnum moveSodl_1(void);
	LucidErrEnum moveExports_1(void);
	LucidErrEnum createTemp(void);
	LucidErrEnum createLgbTemp(void);
	LucidErrEnum CreateNlsDir(void);
	LucidErrEnum CreateResourceDir(void);

private:
	INXString m_csProjDir;
	int m_iCodeMajorRev;
	int m_iCodeMinorRev; 

};
