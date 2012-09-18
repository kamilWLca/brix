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
#include "../LucidGUIBuilder/DrawGUI.h"
#include "libxml/xmlreader.h"
#include <fstream>
#include <stdio.h>
#include <cassert>
#include <algorithm>

#define RADIX_TEN 10 // for converting ints to strings
#define DUMMY_BUFFER_SIZE 50

#define EhsFopen(fname,fmode) fopen(fname,fmode) /**< Open file. */
#define EhsFclose(p) fclose(p) /**< Close file. */
#define EhsFremove(fname) remove(fname) /**< Remove file. */

#define LUCIDPROJECT_TRANSFERRABLES "Transferrables"
#define LUCIDPROJECT_DATAFILESET "DataFileSet"
#define LUCIDPROJECT_IMAGEFILESET "ImageFileSet"
#define LUCIDPROJECT_LAYOUTFILESET "LayoutFileSet"
#define LUCIDPROJECT_RESOURCEFILESET "ResourceFileSet"
#define LUCIDPROJECT_MISCFILESET "MiscFileSet"
#define LUCIDPROJECT_FONTFILESET "FontFileSet"
#define LUCIDPROJECT_LOCATION "Location"


using namespace std;

ProjectMetaData::ProjectMetaData(INXString projectDir)
: m_bProjectIsSet(false)
, m_csLucidVersion(_T(""))
, m_bLocked(false)
, m_iCodeMajorRev(CODE_FORMAT_MAJOR_REV)
, m_iCodeMinorRev(CODE_FORMAT_MINOR_REV)
, m_csProjectAppCanonicalName("")
, m_csProjectAppCommercialName("")
, m_csProjectAppVersion("")
, m_csProjectAppDescription("")
{
	m_csProjectDir = projectDir;
	ProjectMetaData();
}


ProjectMetaData::ProjectMetaData(void)
: m_bProjectIsSet(false)
, m_csLucidVersion(_T(""))
, m_bLocked(false)
, m_iCodeMajorRev(CODE_FORMAT_MAJOR_REV)
, m_iCodeMinorRev(CODE_FORMAT_MINOR_REV)
, m_csProjectAppCanonicalName("")
, m_csProjectAppCommercialName("")
, m_csProjectAppVersion("")
, m_csProjectAppDescription("")
{
	// set up vectors' initial size.
	m_vBmpFiles.reserve( PROJMETADATA_INITIAL_FILEVEC_SIZE );
	m_vDataFiles.reserve( PROJMETADATA_INITIAL_FILEVEC_SIZE );
	m_vGuiFiles.reserve( PROJMETADATA_INITIAL_FILEVEC_SIZE );
	m_vResourceFiles.reserve( PROJMETADATA_INITIAL_FILEVEC_SIZE );
    nBuildNo=0;
	setProjectToNull();
	isDirtySODL = false;
	isSODLCheckBeingPerformed = true;
}

ProjectMetaData::~ProjectMetaData(void)
{
}
/*
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
*/

/*
bool ProjectMetaData::checkProjectFileData(void)
{

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Check that placeholder.png is present:
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	bool bGotPlaceHolder = false;
	std::vector < ExtPngFile > pngFiles;
	getPngFiles( pngFiles );

	for(size_t i=0;i<pngFiles.size();i++){
		if(pngFiles[i].m_csHostFileName == "placeholder.png")
			bGotPlaceHolder = true;
	}

	// Add placeholder.png to project file, and copy it physically from instlln to project
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

		eGF.setHostFileName( tagDat.activeHostFilename );

		if( !fileInProject(eGF) ){

			GuiFileEssentialData_t essDat;
			essDat.fileDescr = GUI_LAYOUT_DEFAULT_DESCRIPTION;
			essDat.fileHostNameStub = tagDat.activeHostFilename;
			essDat.screenTag = tag;
			const bool addPhysically = true;

			err = addGuiFile( essData, addPhysically );
			assert(kErr_NoErr == err);
		}

	}





	return false;
}
*/
LucidErrEnum ProjectMetaData::getProjectName(INXString &csProjName) const
{
	csProjName = m_csProjectName;
	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getFullPathProjectFile(INXString &csProjDir) const
{
	csProjDir = m_csProjectDir + "\\" + m_csProjectName +".lpj";
	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getProjectDir(INXString &csProjDir) const
{
	csProjDir = m_csProjectDir;
	return kErr_NoErr;
}

void ProjectMetaData::setProjectName(INXString csProjName)
{
	m_csProjectName = csProjName;
}

void ProjectMetaData::setProjectDir(INXString csProjDir)
{
	m_csProjectDir = csProjDir;
}
/*
LucidErrEnum ProjectMetaData::readProjectFile()
{
	//LucidErrEnum err;

	if(!projectIsSet()){

		return kErr_ProjectMetaDataNoProjectSet;

	}else{

		INXString dummyCs = "";
		getFullPathProjectFile(dummyCs);
		LucidErrEnum err = readProjectFile( dummyCs );

			// It's an old project, so set its revision to the coded-in labels (ie consistent with this rev of the code).
		m_cProjFolderMinder.setProjectFolder( m_csProjectDir );
//		m_cProjFolderMinder.setProjectRevision( m_iCodeMajorRev, m_iCodeMinorRev );

		assert( kErr_NoErr == err);

	}

	return kErr_NoErr;
}
*/
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
/*
LucidErrEnum ProjectMetaData::readProjectFile( INXString csProjectPathName )
{

	// With this overload of readProjectFile, we are normally reading NOT the present
	// project, but another one - which is probably in the process of being created.  
	// Therefore it is best not to lock the proj file.
	// assert( m_bLocked );
	// assert(  isUpToDate() );

	ifstream projectfile(csProjectPathName);
	char type[WORK_DIR_SIZE] = {'\0'};
	CMarkup xml;
	INXString csXML, csWidgetTag, csScreenTag, csTmpProjectDir;
	Group groupObj;

	if (projectfile.fail()) {
		INXString dummy = "Unable to open project file.";
		dummy += csProjectPathName;
		AfxMessageBox(dummy);
		return kErr_ProjectFileNotRead;
	}
	
	// Before setting the project to null take a copy of the
	// project dir, so can set it again for the case
	// when reading undo files.
	csTmpProjectDir = m_csProjectDir;

	// This is required for the case when reading a project file,
	// and the class has already been set up.
	setProjectToNull();


	// load xml project file
	while ((!projectfile.eof()) && (!projectfile.bad())) {
		projectfile.getline(type,4095);
		csXML = csXML + type;
	}

	projectfile.close();

	m_bProjectIsSet = true;

	// Extract project data
	xml.SetDoc( csXML );

	xml.FindElem("LucidProject");
	xml.IntoElem();


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NB. We are now 'inside the Lucidproject tag.

	xml.FindElem("Version");
	m_csLucidVersion = xml.GetData();


	

	xml.FindElem("ProjectName");
	m_csProjectName = xml.GetData();

	xml.FindElem("LastTransferToTarget");
	INXString dummyTS = xml.GetData();

	// handle case where ts isn't present
	if( dummyTS == m_csProjectName)
		m_cLastTransferTime.setAncient();
	else
		m_cLastTransferTime.csParse( dummyTS );

	// Get group data
	xml.FindElem("GroupSet");
	xml.IntoElem();
	while (xml.FindElem("Group")) {
		xml.FindChildElem("ID");
		groupObj.ID = atoi(xml.GetChildData());
		xml.FindChildElem("Name");
		groupObj.Name = xml.GetChildData();
		xml.FindChildElem("Period");
		groupObj.Period = atoi(xml.GetChildData());
		xml.FindChildElem("Allocation");
		groupObj.Alloc = atoi(xml.GetChildData());
		m_vGroups.push_back(groupObj);
	}
	xml.OutOfElem();

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// External Files.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	xml.FindElem("ExternalFiles");
	xml.IntoElem();

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Get GUI files.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	xml.FindElem("GuiFileSet");
	xml.IntoElem();

	ExtGuiFile egf;
	while (xml.FindElem("GuiFile")) {

		xml.FindChildElem("HostFileName");
		egf.setHostFileName(xml.GetChildData());

		xml.FindChildElem("Description");
		egf.setDescription(xml.GetChildData());

		xml.FindChildElem("Screen");
		egf.setScreenTag(xml.GetChildData());

		m_vGuiFiles.push_back(egf);
	}
	xml.OutOfElem(); // GuiFileSet



	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// get PNG files

	xml.FindElem("PngFileSet");
	xml.IntoElem();

	ExtPngFile epf;
	while (xml.FindElem("PngFile")) {

		xml.FindChildElem("HostFileName");
		epf.setHostFileName(xml.GetChildData());

		xml.FindChildElem("Description");
		epf.setDescription(xml.GetChildData());

		xml.FindChildElem("TargetFileName");
		epf.setTargetFileName(xml.GetChildData());

		m_vPngFiles.push_back(epf);
	}
	xml.OutOfElem(); // PngFileSet

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// get BMP files

	xml.FindElem("BmpFileSet");
	xml.IntoElem();

	ExtBmpFile ebf;
	while (xml.FindElem("BmpFile")) {

		xml.FindChildElem("HostFileName");
		ebf.setHostFileName(xml.GetChildData());

		xml.FindChildElem("Description");
		ebf.setDescription(xml.GetChildData());

		xml.FindChildElem("TargetFileName");
		ebf.setTargetFileName(xml.GetChildData());

		m_vBmpFiles.push_back(ebf);
	}
	xml.OutOfElem(); // BmpFileSet

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// get Data files.

	xml.FindElem("DataFileSet");
	xml.IntoElem();

	ExtDataFile edf;
	while (xml.FindElem("DataFile")) {

		xml.FindChildElem("HostFileName");
		edf.setHostFileName(xml.GetChildData());

		xml.FindChildElem("Description");
		edf.setDescription(xml.GetChildData());

		xml.FindChildElem("TargetFileName");
		edf.setTargetFileName(xml.GetChildData());

		m_vDataFiles.push_back(edf);
	}

	xml.OutOfElem(); // DataFileSet

	xml.OutOfElem(); // External Files


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Get Screen Tags
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	TagProjMetaSupportData_t suppData;

	xml.FindElem("ScreenSet");
	xml.IntoElem();
	while (xml.FindElem("Screen")) {

		xml.FindChildElem("Tag");
		csScreenTag = xml.GetChildData();

		xml.FindChildElem("Description");
		suppData.tagDescr = xml.GetChildData();

		xml.FindChildElem("TargetFileName");
		suppData.tgtFilename = xml.GetChildData();

		xml.FindChildElem("ActiveHostLayout");

		// If reading an old proj file with no 'active layout field',
		// the active layout will still be got as the same as the targetfilename
		// ie field above.
		// So use the 1st layout as a default.
		INXString dummy = xml.GetChildData();	

		if(dummy.Right(3) == "gui"){

			ExtGuiFile egf = m_vGuiFiles.front();
			LucidErrEnum err = egf.getHostFileName( suppData.activeHostFilename );
			assert( kErr_NoErr == err );

		}else {

			suppData.activeHostFilename = dummy;
		}

		addScreenTag(csScreenTag, suppData);

	}

	xml.OutOfElem(); // ScreenSet


// Get Widgets
	xml.FindElem("WidgetSet");
	xml.IntoElem();

	while (xml.FindElem("Widget")) {
		xml.FindChildElem("WidgetTag");
		csWidgetTag = xml.GetChildData();
		xml.FindChildElem("ScreenTag");
		csScreenTag = xml.GetChildData();

		GuiWidget widget(csWidgetTag, csScreenTag);
		addGuiWidget(widget);
	}

	xml.OutOfElem(); // WidgetSet


// get external file name-counts.

	xml.FindElem("ExternalDataFileMetas");
	xml.IntoElem();

	xml.FindElem("NumberOfHighestGuiFile");
	INXString dum = xml.GetData();
	m_iLastGuiFileAddedKey = atoi(dum);

	xml.FindElem("NumberOfHighestPngFile");
	m_iLastPngFileAddedKey= atoi(xml.GetData());

	xml.FindElem("NumberOfHighestBmpFile");
	m_iLastBmpFileAddedKey= atoi(xml.GetData());

	xml.FindElem("NumberOfHighestDatFile");
	m_iLastDataFileAddedKey= atoi(xml.GetData());

	xml.OutOfElem(); // ExternalDataFileMetas


// Get Transferrables

	// 1st clear out previous contents
	m_cTransferables.clear();

	xml.FindElem("Transferrables");
	xml.IntoElem();

	LccPmdXfers::TypesEnum dummy = LccPmdXfers::kData; // LccPmdXfers::kData;

	INXString csHostFileName;
	INXString csTargetFileName;

	xml.FindElem("DataFileSet");
	while ( xml.FindChildElem("Location") )
	{
		csHostFileName = xml.GetChildData();
		m_cTransferables.addItem( dummy, csHostFileName );
	}

	xml.FindElem("ImageFileSet");
	while ( xml.FindChildElem("Location") )
	{
		csHostFileName = xml.GetChildData();
		//getTargetFileNameForPngHostFileName( csHostFileName, csTargetFileName );
		m_cTransferables.addItem( dummy, csHostFileName );
	}

	xml.FindElem("LayoutFileSet");
	while ( xml.FindChildElem("Location") )
	{
		csHostFileName = xml.GetChildData();
		//getTargetFileNameForGuiHostFileName( csHostFileName, csTargetFileName );
		m_cTransferables.addItem( dummy, csHostFileName );
	}

	xml.FindElem("MiscFileSet");
	while ( xml.FindChildElem("Location") )
	{
		csHostFileName = xml.GetChildData();
		//getTargetFileNameForDataHostFileName( csHostFileName, csTargetFileName );
		m_cTransferables.addItem( dummy, csHostFileName );
	}

	xml.OutOfElem(); // Transferrables
	
	// project root dir is no longer stored in project file so
	// if project file is an undo file the project dir is the project dir stored before
	// setting the project to null
	if (csProjectPathName.Find("temp\\lpj_undo") != -1) {
		m_csProjectDir = csTmpProjectDir;
	}
	// extract the project dir from the pathname and projectname
	// subtract 5 for .lpj and the '/' between the project dir and project file name
	else {
		int iLen = csProjectPathName.GetLength() - m_csProjectName.GetLength() - 5;
		m_csProjectDir = csProjectPathName.Left(iLen);
	}

	m_cProjFolderMinder.setProjectFolder( m_csProjectDir );
	getVersionInts();
	//m_cProjFolderMinder.setProjectRevision( m_iCodeMajorRev, m_iCodeMinorRev );

	updateTransferrables();

	return kErr_NoErr;

}
*/
LucidErrEnum ProjectMetaData::showNewProjectDialog()
{
	CProjectDialog projDlg;
	projDlg.SetIsWinEnv(((CLabLgbBaseApp *)AfxGetApp())->isInstalledInWindows);
	if (projDlg.DoModal()==IDOK) {

		setProjectToNull();

		// It's a new project, so set its revision to the coded-in labels (ie consistent with this rev of the code).
		m_iProjReadMajorRev = -1;
		m_iProjReadMinorRev = -1;

		m_csProjectName = projDlg.m_ProjectName;
		m_csProjectDir = projDlg.m_ProjectDir + "\\" + projDlg.m_ProjectName;
		m_csProjectAppCanonicalName = projDlg.m_ProjectName;
		m_csProjectAppCommercialName = projDlg.m_ProjectCommercialName;
		m_csProjectAppVersion = projDlg.m_ProjectVersion;
		m_csProjectAppDescription = projDlg.m_ProjectDescription;

		// set the folder minder with the current rev.
		initProjFolderMinder();

		//m_cProjFolderMinder.setProjectFolder( m_csProjectDir );
		//m_cProjFolderMinder.setProjectRevision( m_iCodeMajorRev, m_iCodeMinorRev );

		createFolderStructure();
		copyDummyBmp();
		createDefaultGroup();

		// Add the default widget-group.
		INXString tag = "default";
		INXString description = GUI_LAYOUT_DEFAULT_DESCRIPTION;
		createNewScreenTag(tag, description);
		
		if(!getLock()){

			AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
			return kErr_DidNotGetLock;

		} else {
			writeProjectFile();
			releaseLock();
		}

		m_bProjectIsSet = true;

		return kErr_NoErr;
	}
	return kErr_DialogNotOk;
}

LucidErrEnum ProjectMetaData::showOpenProjectDialog()
{
	CFileDialog dlg(1,"lpj","",OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,"Lucid Project Files (*.lpj)|*.lpj|");
	CFileOperation fo;
	//LucidErrEnum ret;

	// Set the initial directory to be the registry ProjectDir.
	// If this directory doesn't exist then use the registry ProjectRootDir.

	INXString csInitialDir = AfxGetApp()->GetProfileString(_T("Project"),_T("ProjectDir"),_T(DEFAULTPROJECTROOTDIR));
	if (!fo.CheckPath(csInitialDir)) {
		csInitialDir = AfxGetApp()->GetProfileString(_T("Project"),_T("ProjectRootDir"),_T(DEFAULTPROJECTROOTDIR));
	}

	dlg.m_ofn.lpstrInitialDir = csInitialDir.GetBuffer(4096);
	csInitialDir.ReleaseBuffer();

	int res = dlg.DoModal();

	if ( res==IDOK ) {

		setProjectToNull();

		INXString csProjectPathName = dlg.GetPathName();
		INXString csProjectFileName = dlg.GetFileName();
		// extract the project dir from the pathname and projectname
		// subtract 1 for the '/' between the project dir and project file name
		int iLen = csProjectPathName.GetLength() - csProjectFileName.GetLength() - 1;
		m_csProjectDir = csProjectPathName.Left(iLen);
		// By convention, chop off the '.lpj' extension.
		m_csProjectName = csProjectFileName.Left( csProjectFileName.GetLength() - 4);


//		m_csProjectName = dlg.m_ProjectName;
//		m_csProjectDir = dlg.m_ProjectDir + "\\" + dlg.m_ProjectName;

		m_bProjectIsSet = true;

		const LPCTSTR data_char = (LPCTSTR) m_csProjectDir;
		AfxGetApp()->WriteProfileString(_T("Project"),_T("ProjectDir"),m_csProjectDir);

		return kErr_NoErr;

	}else{
		return kErr_DialogNotOk;
	}
}

LucidErrEnum ProjectMetaData::showSaveProjectAsDialog()
{
	INXString csOldProjectRootDir, csOldProjectName, csProjectPathName;
	CProjectDialog projDlg;
	projDlg.SetIsWinEnv(((CLabLgbBaseApp *)AfxGetApp())->isInstalledInWindows);
	projDlg.m_ProjectName = m_csProjectAppCanonicalName;
	projDlg.m_ProjectCommercialName = m_csProjectAppCommercialName;
	projDlg.m_ProjectVersion = m_csProjectAppVersion;
	projDlg.m_ProjectDescription = m_csProjectAppDescription;

	if (projDlg.DoModal()==IDOK) {
		// read project file
		getFullPathProjectFile(csProjectPathName);
		// Do this read in calling routine so locking works Ok, b4 proj name has been changed.
		//readProjectFile(csProjectPathName);

		copyFolderTo((INXString)projDlg.m_ProjectDir, (INXString)projDlg.m_ProjectName);
		m_csProjectName = projDlg.m_ProjectName;
		m_csProjectDir = projDlg.m_ProjectDir + "\\" + projDlg.m_ProjectName;
		m_csProjectAppCanonicalName = projDlg.m_ProjectName;
		m_csProjectAppCommercialName = projDlg.m_ProjectCommercialName;
		m_csProjectAppVersion = projDlg.m_ProjectVersion;
		m_csProjectAppDescription = projDlg.m_ProjectDescription;
		writeProjectDescriptionFile();

		// Do this read in calling routine so locking works Ok, after proj name has been changed.
		//readProjectFile(csProjectPathName);
		//writeProjectFile();
		return kErr_NoErr;
	}
	return kErr_DialogNotOk;
}


/*
 * Deprecated, do not use.
 * Data Items removed from IAB
 */
LucidErrEnum ProjectMetaData::showAddFileDialog(ExtDataFile &file, bool &bExists)
{
	CFileOperation fo;
	INXString csProjectPathName;
	CFileDialog dlg(1,"txt","",OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,"Project Data Files (*.txt)|*.txt|");
		
	bExists = FALSE;
	if (dlg.DoModal()==IDOK) {

		if(!getLock()){

			AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
			return kErr_DidNotGetLock;

		} else {

			// read project file
			getFullPathProjectFile(csProjectPathName);
			readProjectFile(csProjectPathName);

			INXString csPathName = dlg.GetPathName();
			INXString csFileName = dlg.GetFileName();
			file.setHostFileName(csFileName);
			// Check if the file already exists
			if (fileInProject(file)) {
				if (AfxMessageBox("The file already exists.  Do you want to replace it?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
					fo.SetOverwriteMode(TRUE);
					fo.Copy(csPathName, m_csProjectDir + DDATADIR);
				}
				bExists = TRUE;

			} else {
				// copy the data file to the project directory
				fo.SetOverwriteMode(TRUE);
				fo.Copy(csPathName, m_csProjectDir + DDATADIR);
				addDataFile(file);
			}
			writeProjectFile();
			releaseLock();
			return kErr_NoErr;
		}
		return kErr_DialogNotOk;
	}
}

LucidErrEnum ProjectMetaData::showAddResourceDialog(ExtResourceFile &file, bool &bExists)
{
	CFileOperation fo;
	INXString csProjectPathName;
	CFileDialog dlg(1,"*","",OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,"Project Resource Files (*.*)|*.*|");
		
	bExists = FALSE;
	if (dlg.DoModal()==IDOK) {

		if(!getLock()){

			AfxMessageBox(PMD_LOCK_FAILURE_MESSAGE);
			return kErr_DidNotGetLock;

		} else {

			// read project file
			getFullPathProjectFile(csProjectPathName);
			readProjectFile(csProjectPathName);

			INXString csPathName = dlg.GetPathName();
			INXString csFileName = dlg.GetFileName();
			file.setHostFileName(csFileName);
			// Check if the file already exists
			if (fileInProject(file)) {
				if (AfxMessageBox("The file already exists.  Do you want to replace it?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
					fo.SetOverwriteMode(TRUE);
					fo.Copy(csPathName, m_csProjectDir + RESOURCEDIR);
				}
				bExists = TRUE;

			} else {
				// copy the data file to the project directory
				fo.SetOverwriteMode(TRUE);
				fo.Copy(csPathName, m_csProjectDir + RESOURCEDIR);
				addResourceFile(file);
			}

			writeProjectFile();
			releaseLock();

			return kErr_NoErr;
		}

		return kErr_DialogNotOk;

	}
}

// get a simple copy of vector of external files
LucidErrEnum ProjectMetaData::getGuiFiles(vector<ExtGuiFile> &guiFileVec) const
{
	guiFileVec = m_vGuiFiles;

	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getPngFiles(vector<ExtPngFile> &fileVec) const
{
	fileVec = m_vPngFiles;

	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getBmpFiles(vector<ExtBmpFile> &bmpFileVec) const
{
	bmpFileVec = m_vBmpFiles;

	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getDataFiles(vector<ExtDataFile> &dataFileVec) const
{
	dataFileVec = m_vDataFiles;
	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getBdfFiles(vector<ExtBdfFile> &bdfFileVec) const
{
	bdfFileVec = m_vBdfFiles;

	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getNlsFiles(vector<ExtNlsFile> &nlsFileVec) const
{
	nlsFileVec = m_vNlsFiles;

	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getResourceFiles(vector<ExtResourceFile> &xResourceFileVec) const
{
	xResourceFileVec = m_vResourceFiles;
	return kErr_NoErr;
}


bool ProjectMetaData::isLocked()
{
	return m_bLocked;
}

LucidErrEnum ProjectMetaData::getGroupVec(vector<Group> &groupVec)
{
	groupVec = m_vGroups;
	return kErr_NoErr;
}


bool ProjectMetaData::fileInProject(const ExtDataFile &file) const
{
	return (find(m_vDataFiles.begin(), m_vDataFiles.end(), file)
					!= m_vDataFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtGuiFile &file) const
{
	return (find(m_vGuiFiles.begin(), m_vGuiFiles.end(), file)
					!= m_vGuiFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtPngFile &file) const
{
	return (find(m_vPngFiles.begin(), m_vPngFiles.end(), file)
					!= m_vPngFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtBmpFile &file) const
{
	return (find(m_vBmpFiles.begin(), m_vBmpFiles.end(), file)
					!= m_vBmpFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtBdfFile &file) const
{
	return (find(m_vBdfFiles.begin(), m_vBdfFiles.end(), file)
					!= m_vBdfFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtNlsFile &file) const
{
	return (find(m_vNlsFiles.begin(), m_vNlsFiles.end(), file)
					!= m_vNlsFiles.end() );
}

bool ProjectMetaData::fileInProject(const ExtResourceFile &file) const
{
	return (find(m_vResourceFiles.begin(), m_vResourceFiles.end(), file)
					!= m_vResourceFiles.end() );
}


LucidErrEnum ProjectMetaData::removeAllGroups()
{
	m_vGroups.clear();
	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::addGroup(Group &group)
{
	m_vGroups.push_back(group);
	return kErr_NoErr;
}

LucidErrEnum ProjectMetaData::getGuiWidgets(vector<GuiWidget> &vWidgets)
{
	vWidgets = m_vGuiWidgets;
	return kErr_NoErr;
}

// This method only adds the widget if it doesn't already exist
LucidErrEnum ProjectMetaData::addGuiWidget(const GuiWidget &widget)
{
	LucidErrEnum ret = kErr_NoErr;

	ret = guiWidgetInVec(widget);
	if (!ret) {
		m_vGuiWidgets.push_back(widget);
	}

	return ret;
}

LucidErrEnum ProjectMetaData::guiWidgetInVec(const GuiWidget &widget)
{
	LucidErrEnum ret = kErr_NoErr;

	// Check if gui widget already exists
	for (UINT i=0; i<m_vGuiWidgets.size(); i++) {
		if (widget == m_vGuiWidgets.at(i)) {
			ret = kErr_ProjectMetaData_WidgetScreenPairAlreadyPresent;
		}
	}

	return ret;
}

LucidErrEnum ProjectMetaData::updateWidget(const GuiWidget &widget1, const GuiWidget &widget2)
{
	LucidErrEnum ret = kErr_ProjectMetaData_WidgetIsNotPresent;
	INXString csWidgetTag, csScreenTag;

	for (UINT i=0; i<m_vGuiWidgets.size(); i++) {

		if (widget1 == m_vGuiWidgets.at(i)) {
			widget2.getWidgetTag(csWidgetTag);
			widget2.getScreenTag(csScreenTag);
			m_vGuiWidgets.at(i).setWidgetTag(csWidgetTag);
			m_vGuiWidgets.at(i).setScreenTag(csScreenTag);
			ret = kErr_NoErr;
		}
	}	

	return ret;
}

LucidErrEnum ProjectMetaData::removeGuiWidget(const GuiWidget &widget)
{
	UINT index;
	bool remove = FALSE;

	for (UINT i=0; i<m_vGuiWidgets.size(); i++) {
		if (widget == m_vGuiWidgets.at(i)) {
			remove = TRUE;
			index = i;
		}
	}

	if (remove) {
		m_vGuiWidgets.erase(m_vGuiWidgets.begin()+index,m_vGuiWidgets.begin()+index+1);
		return kErr_NoErr;
	}
	else {
		return kErr_ProjectMetaData_WidgetIsNotPresent;
	}
}

LucidErrEnum ProjectMetaData::getGuiWidgetVecSize(UINT &size)
{
	size = (UINT)m_vGuiWidgets.size();
	return kErr_NoErr;
}


void ProjectMetaData::createDefaultGroup()
{
	Group groupObj;

	groupObj.ID = 1;
	groupObj.Name = "Default";
	groupObj.Period = DEFAULT_PERIOD;
	groupObj.Alloc = DEFAULT_ALLOC; // 90%
	m_vGroups.push_back(groupObj);
}

void ProjectMetaData::createFolderStructure()
{
	CFileOperation fo;


	// when overwriting existing project delete old one.
	if (fo.CheckPath(m_csProjectDir)) {
		fo.Delete(m_csProjectDir);
	}
	

	// create project folder
	CreateDirectory(m_csProjectDir, NULL);

	// create subdirectories
	LucidErrEnum err = m_cProjFolderMinder.createProjectFolderStructure();


	CreateDirectory(m_csProjectDir + DEPDIR + m_csProjectName, NULL);
	// create empty dep file, so when call OpenDocumentFile there is something to open
	ofstream outfile(m_csProjectDir + DEPDIR + m_csProjectName + ".prg");
	outfile << "";	
	outfile.close();
}


void ProjectMetaData::copyFolderTo(INXString csNewProjRootDir, INXString csNewProjName) 
{

	CFileOperation fo;
	INXString csNewProjectDir = csNewProjRootDir + "\\" + csNewProjName;

	// when overwriting existing project delete old one.
	if (fo.CheckPath(csNewProjectDir)) {
		fo.Delete(csNewProjectDir);
	}

	// Copy existing project to new project dir
	CreateDirectory(csNewProjectDir, NULL);
	fo.Copy(m_csProjectDir + DEPDIR, csNewProjectDir);
	fo.Copy(m_csProjectDir + "\\DeviceData", csNewProjectDir );
	fo.Copy(m_csProjectDir + EXPORTDIR, csNewProjectDir);
	//fo.Copy(m_csProjectDir + SODLDIR, csNewProjectDir);

	// Rename top level DEP
	fo.Rename(csNewProjectDir + DEPDIR + m_csProjectName, csNewProjectDir + DEPDIR + csNewProjName);
	fo.Rename(csNewProjectDir + DEPDIR + m_csProjectName + ".prg", csNewProjectDir + DEPDIR + csNewProjName + ".prg");

}


void ProjectMetaData::setProjectToNull(void)
{
	m_iLastGuiFileAddedKey = 0;
	m_iLastDataFileAddedKey = 0;
	m_iLastBmpFileAddedKey = 0;
	m_iLastPngFileAddedKey = 0;
	m_iLastBdfFileAddedKey = 0;

	m_csProjectName = "";
	m_csProjectDir = "";

	m_vGroups.clear();

	m_vBmpFiles.clear();
	m_vDataFiles.clear();
	m_vGuiFiles.clear();
	m_vPngFiles.clear();
	m_vBdfFiles.clear();
	m_vNlsFiles.clear();
	m_vResourceFiles.clear();

	m_mapScreenTagProjMetas.clear();
	m_vGuiWidgets.clear();

	m_cTransferables.clear();

	m_bProjectIsSet = false;

	/*
	if(m_pLock){

		if(m_pLock->Unlock(1) ){
			HANDLE hndl = HANDLE( *m_pMutex );
			ReleaseMutex( hndl );
		} else {
			assert(1==0);
		}
		delete m_pLock;
		m_pLock = NULL;
	}*/

//	if( m_pMutex){
//		delete m_pMutex;
//		m_pMutex = NULL;
//	}
}

LucidErrEnum ProjectMetaData::editScreenTagDescription(
	const INXString &tag,
	const INXString &tagDescrip)
{


	if(!screenTagInProject(tag)){
		return kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;
	}else{
		TagProjMetaSupportData_t dummy = m_mapScreenTagProjMetas[tag];
		dummy.tagDescr = tagDescrip;
		m_mapScreenTagProjMetas[tag] = dummy;

		return kErr_NoErr;
	}
}

LucidErrEnum ProjectMetaData::removeScreenTag(
	const INXString &tag )
{

	assert( tag.GetLength() >0 );

	ScreenProjMetaMap_t::iterator it;

	if( (it=m_mapScreenTagProjMetas.find(tag)) == m_mapScreenTagProjMetas.end()){

		// The tag is already in the set of screen tags ...
		return 	kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

	}else{

		// load these all into the screen tag map
		m_mapScreenTagProjMetas.erase(it);

		return kErr_NoErr;

	}

}



LucidErrEnum ProjectMetaData::getTargetFileNameForGuiHostFileName(
				const INXString &hostFileName, INXString &targetFileName ) const
{

	ExtGuiFile eGF;
	LucidErrEnum err = getGuiFileByName( hostFileName, eGF );

	if(err!=kErr_NoErr){

		return err;

	}else{ // if(err==kErr_NoErr

		INXString screenTag;
		err = eGF.getScreenTag(screenTag);

		if(err!=kErr_NoErr){

			return err;

		} else { // if(err==kErr_NoErr) 

			TagProjMetaSupportData_t tagData;

			err = getScreenTagMetas( screenTag, tagData);

			if (err != kErr_NoErr){
			
				return err;

			} else {

				targetFileName = tagData.tgtFilename;
				return kErr_NoErr;
			}

		} //if(err!=kErr_NoErr)

	} // if(err==kErr_NoErr

}

LucidErrEnum ProjectMetaData::getScreenTagMetas(
		const INXString &screenTag,
		TagProjMetaSupportData_t &tagData) const
{

	ScreenProjMetaMapPair_t *pPair = NULL;

	ScreenProjMetaMapConstIt_t it = m_mapScreenTagProjMetas.begin();

	while (it != m_mapScreenTagProjMetas.end() ){

		pPair = (ScreenProjMetaMapPair_t *) &(*it);

		if(pPair->first == screenTag){

			tagData = pPair->second;
			return kErr_NoErr;

		}
		it++;

	} // while (it != m_mapScreenTagProjMetas.end() )

	return kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

}


LucidErrEnum ProjectMetaData::getActiveLayouts(std::set< INXString > &vLayoutList )
{

	if( projectIsSet() ){

		vLayoutList.clear();
		LucidErrEnum err;
		INXString layoutHostStub;
		std::vector< INXString > widgetGrps;

		err = getScreenTags( widgetGrps );
		assert( kErr_NoErr == err );

		std::vector<INXString> vNames;

		for( int wg=0; wg<widgetGrps.size(); wg++ ){

			getActiveLayout( widgetGrps[wg], layoutHostStub );

			vLayoutList.insert( layoutHostStub );

		} // for( int layout=0; layout< tags.size(); layout++)

		return kErr_NoErr;

	} // project is set
	else{
		return kErr_ProjectFileNotRead;
	} // project isn't set

}

LucidErrEnum ProjectMetaData::getActiveLayout(
	const INXString &widgetGroup, INXString &layoutHostName )
{

	LucidErrEnum err;

	if( m_mapScreenTagProjMetas.find(widgetGroup) == m_mapScreenTagProjMetas.end()){

		// The tag is not in the set of screen tags ...
		return 	kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

	} else {

		ExtGuiFile egf;
		egf.setHostFileName( layoutHostName );
		err = getGuiFileByName( layoutHostName, egf );

		TagProjMetaSupportData_t dummy = m_mapScreenTagProjMetas[widgetGroup];
		layoutHostName = dummy.activeHostFilename;

		return kErr_NoErr;

	}
}


LucidErrEnum ProjectMetaData::setActiveLayout(
	const INXString &widgetGroup, const INXString &layoutHostName )
{

	LucidErrEnum err;

	if( m_mapScreenTagProjMetas.find(widgetGroup) == m_mapScreenTagProjMetas.end()){

		// The tag is not in the set of screen tags ...
		return 	kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

	} else {

		ExtGuiFile egf;
		egf.setHostFileName( layoutHostName );
		err = getGuiFileByName( layoutHostName, egf );

		if( kErr_NoErr != err ){

			return kErr_ProjectMetaData_GuiFileNotAlreadyPresent;

		} else {

			TagProjMetaSupportData_t dummy = m_mapScreenTagProjMetas[widgetGroup];
			dummy.activeHostFilename = layoutHostName;
			m_mapScreenTagProjMetas[widgetGroup] = dummy;

			return kErr_NoErr;
		}
	}
}

LucidErrEnum ProjectMetaData::updateScreenTag(
	const INXString &tag, const TagProjMetaSupportData_t &suppData )
{

	//assert( suppData.tagDescr.GetLength() >0 );
	assert( suppData.tgtFilename.GetLength() >0 );
	LucidErrEnum err;

	if( kErr_NoErr == (err=removeScreenTag( tag ))  ){

		err = addScreenTag( tag, suppData );

	}
	return err;

}

LucidErrEnum ProjectMetaData::addScreenTag(
	const INXString &tag, const TagProjMetaSupportData_t &suppData )
{

	//assert( suppData.tagDescr.GetLength() >0 );
	assert( suppData.tgtFilename.GetLength() >0 );

	if( m_mapScreenTagProjMetas.find(tag) != m_mapScreenTagProjMetas.end()){

		// The tag is already in the set of screen tags ...
		return 	kErr_ProjectMetaData_ScreenTagAlreadyPresent;

	}else{

		// load these all into the screen tag map
		m_mapScreenTagProjMetas[tag] = suppData;

		return kErr_NoErr;

	}

}


LucidErrEnum ProjectMetaData::createNewScreenTag(
	const INXString &tag, const INXString &descr )
{

	if( m_mapScreenTagProjMetas.find(tag) != m_mapScreenTagProjMetas.end()){

		// The tag is already in the set of screen tags ...
		return 	kErr_ProjectMetaData_ScreenTagAlreadyPresent;

	}else{

		INXString cStr1;  // dummy var to hold tgt file name...
		TagProjMetaSupportData_t tsd; // dummy var to hold Screen descrip and tgt file name

		// generate tgt file name

		// 1st the number -count part of the file.

		char buffer[100];
		sprintf_s(buffer, "%07d", m_iLastGuiFileAddedKey );
		cStr1 = buffer;
		m_iLastGuiFileAddedKey++;

		// Now build up to the actual target filename
		cStr1 = GUI_TARGETFILE_PREFIX + cStr1 + GUI_TARGETFILE_SUFFIX;


		// Add the default GUI file for this tag.
		// 1st construct the layout file name.

		GuiFileEssentialData_t essData;

		essData.fileDescr = descr;
		essData.fileHostNameStub = GUI_LAYOUT_DEFAULT_FILENAMEPREFIX + tag;
		essData.screenTag = tag;

		// Load these into the screen tag support data struc
		tsd.tagDescr = descr;
		tsd.tgtFilename = cStr1;
		tsd.activeHostFilename = essData.fileHostNameStub;

		// load these all into the screen tag map
		m_mapScreenTagProjMetas[tag] = tsd;

		LucidErrEnum err = addGuiFile(essData, true );
		assert( kErr_NoErr == err );

		return kErr_NoErr;

	}

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::addGuiFile(
	const GuiFileEssentialData_t &fileData, const bool &addFileSystemItem )
{
	LucidErrEnum ret = kErr_NoErr;

	// 'find' should get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.

	// 1st create a 'trial' file with the right data..
	ExtGuiFile dummyFile(fileData);

	// Now see if it's in the vector already.
//	if( find( m_vGuiFiles.begin(), m_vGuiFiles.end(), dummyFile ) !=
//		m_vGuiFiles.end() ){

	if(fileInProject( dummyFile )) {

			return kErr_ProjectMetaData_GuiFileAlreadyPresent;

	}else if( !screenTagInProject(fileData.screenTag) ){

	// The screen tag must already be present!
	// find should NOT return end()!
		return kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

	}else{
	
		m_vGuiFiles.push_back(dummyFile);

		// Create the layout physically.

		if( addFileSystemItem ){

			INXString fullPath;
			getProjectDir(fullPath); //kwhite added
			fullPath += GUIDIR;
			fullPath = fullPath + fileData.fileHostNameStub + ".gui";
			ofstream targetGuiFile( fullPath );
			targetGuiFile.close();

		}

		return kErr_NoErr;
	}
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::removeGuiFile(
	const ExtGuiFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtGuiFile>::iterator it;

	// 'find' should NOT get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.

	if(!fileInProject(file)){

		return kErr_ProjectMetaData_GuiFileNotAlreadyPresent;

	}else{

		it = find( m_vGuiFiles.begin(), m_vGuiFiles.end(), file );

		m_vGuiFiles.erase(it);
		
		return kErr_NoErr;

	}

}

LucidErrEnum ProjectMetaData::removeGuiFiles(const INXString &csScreenTag)
{
	ExtGuiFile *pGF;
	ExtGuiFile tmpGuiFile;
	INXString csTag, csDescr, csFileName;
	vector<ExtGuiFile> vTmpGuiFiles;
	vector<ExtGuiFile>::iterator it = m_vGuiFiles.begin();

	// find all files to erase
	while (it != m_vGuiFiles.end()) {

		pGF = &(*it);

		pGF->getScreenTag(csTag);
		pGF->getDescription(csDescr);
		pGF->getHostFileName(csFileName);

		if (csTag == csScreenTag) {
			tmpGuiFile.setDescription(csDescr);
			tmpGuiFile.setHostFileName(csFileName);
			tmpGuiFile.setScreenTag(csTag);
			vTmpGuiFiles.push_back(tmpGuiFile);
		}

		it++;

	} // while(it != m_vGuiFiles.end())

	// erase all files found
	vector<ExtGuiFile>::iterator it2 = vTmpGuiFiles.begin();
	while (it2 != vTmpGuiFiles.end()) {
		pGF = &(*it2);
		pGF->getScreenTag(csTag);
		pGF->getDescription(csDescr);
		pGF->getHostFileName(csFileName);
		tmpGuiFile.setDescription(csDescr);
		tmpGuiFile.setHostFileName(csFileName);
		tmpGuiFile.setScreenTag(csTag);

		m_vGuiFiles.erase(find( m_vGuiFiles.begin(), m_vGuiFiles.end(), tmpGuiFile ));
		it2++;
	}	

	return kErr_NoErr;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::addPngFile(
	const ExtPngFile &file )
{
	LucidErrEnum ret = kErr_NoErr;

	//Only add file if not already present

	if(!fileInProject(file))
	{

		INXString cStr1;  // dummy var to hold tgt file name...

		ExtPngFile dummyFile(file);
		dummyFile.setTargetFileName( m_iLastPngFileAddedKey );
		m_iLastPngFileAddedKey++;
		m_vPngFiles.push_back(dummyFile);
		
		return kErr_NoErr;
	}

	//assert(1==0);
	return kErr_ProjectMetaData_PngFileAlreadyPresent;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::addBmpFile(
	const ExtBmpFile &file )
{
	LucidErrEnum ret = kErr_NoErr;

	//Only add file if not already present

	if(!fileInProject(file))
	{

		INXString cStr1;  // dummy var to hold tgt file name...

		ExtBmpFile dummyFile(file);
		dummyFile.setTargetFileName( m_iLastBmpFileAddedKey );
		m_iLastBmpFileAddedKey++;
		m_vBmpFiles.push_back(dummyFile);
		
		return kErr_NoErr;
	}
	//assert(1==0);
	return kErr_ProjectMetaData_BmpFileAlreadyPresent;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::addBdfFile(
	const ExtBdfFile &file )
{
	LucidErrEnum ret = kErr_NoErr;

	//Only add file if not already present

	if(!fileInProject(file))
	{

		INXString cStr1;  // dummy var to hold tgt file name...

		ExtBdfFile dummyFile(file);
		// To Do: Set target filename
		dummyFile.setTargetFileName( m_iLastBdfFileAddedKey );
		m_iLastBdfFileAddedKey++;
		m_vBdfFiles.push_back(dummyFile);
		
		return kErr_NoErr;
	}
	//assert(1==0);
	return kErr_ProjectMetaData_BdfFileAlreadyPresent;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::removePngFile(
	const ExtPngFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtPngFile>::iterator it;

	// 'find' should NOT get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.
	assert( (it=find( m_vPngFiles.begin(),m_vPngFiles.end(),file)) !=
						m_vPngFiles.end() );

	m_vPngFiles.erase(it);

	return ret;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::removeBmpFile(
	const ExtBmpFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtBmpFile>::iterator it;

	// 'find' should NOT get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.
	assert( (it=find( m_vBmpFiles.begin(),m_vBmpFiles.end(),file)) !=
						m_vBmpFiles.end() );

	m_vBmpFiles.erase(it);

	return ret;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*
 * Deprecated, do not use.
 * Data Items removed from IAB
 */
LucidErrEnum ProjectMetaData::addDataFile(
						const ExtDataFile &file )
{
	LucidErrEnum ret = kErr_NoErr;

	// 'find' should get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.
	assert( find( m_vDataFiles.begin(), m_vDataFiles.end(), file ) ==
						m_vDataFiles.end() );

	INXString cStr1;  // dummy var to hold tgt file name...

	ExtDataFile dummyFile(file);
	dummyFile.setTargetFileName( m_iLastDataFileAddedKey );
	m_iLastDataFileAddedKey++;
	m_vDataFiles.push_back(dummyFile);

	return ret;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*
 * Deprecated, do not use.
 * Data Items removed from IAB
 */
LucidErrEnum ProjectMetaData::removeDataFile(
	const ExtDataFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtDataFile>::iterator it;

// 'find' should NOT get to the end without finding the file . otherwise file
// is already present in the vector.  Trying to add a file already present
// is an error.
	assert( (it=find(m_vDataFiles.begin(),m_vDataFiles.end(),file)) !=
						m_vDataFiles.end() );

	m_vDataFiles.erase(it);

	return ret;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::removeBdfFile(
	const ExtBdfFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtBdfFile>::iterator it;

	// 'find' should NOT get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.
	assert( (it=find( m_vBdfFiles.begin(),m_vBdfFiles.end(),file)) !=
						m_vBdfFiles.end() );

	m_vBdfFiles.erase(it);

	return ret;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::RemoveAllBdfFiles()
{
	LucidErrEnum ret = kErr_NoErr;

	m_iLastBdfFileAddedKey = 0;
	m_vBdfFiles.clear();

	return ret;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
LucidErrEnum ProjectMetaData::addResourceFile(
						const ExtResourceFile &file )
{
	LucidErrEnum ret = kErr_NoErr;

	// 'find' should get to the end without finding the file . otherwise file
	// is already present in the vector.  Trying to add a file already present
	// is an error.
	assert( find( m_vResourceFiles.begin(), m_vResourceFiles.end(), file ) ==
						m_vResourceFiles.end() );

	INXString csFileName;  // dummy var to hold tgt file name...

	ExtResourceFile dummyFile(file);
	
	dummyFile.getHostFileName(csFileName);
	dummyFile.setTargetFileName((CString)csFileName);
	m_vResourceFiles.push_back(dummyFile);
	
	//writeProjectFile();

	return ret;

}

LucidErrEnum ProjectMetaData::removeResourceFile(const ExtResourceFile &file )
{
	LucidErrEnum ret = kErr_NoErr;
	vector<ExtResourceFile>::iterator it;

	assert( (it=find(m_vResourceFiles.begin(),m_vResourceFiles.end(),file)) !=
						m_vResourceFiles.end() );

	m_vResourceFiles.erase(it);

	return ret;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

LucidErrEnum ProjectMetaData::getScreenTags(vector<INXString> &tags )
{
	tags.clear();

	ScreenProjMetaMapPair_t *pPr;

	ScreenProjMetaMapConstIt_t it = m_mapScreenTagProjMetas.begin();

	while(it != m_mapScreenTagProjMetas.end()){

		pPr = (ScreenProjMetaMapPair_t *) &(*it);
		tags.push_back(pPr->first);
		it++;

	} // while(it != m_mapScreenTagProjMetas.end())

/*
	tags.clear();
	tags.push_back(INXString("tag 1"));
	tags.push_back(INXString("tag 2"));
	tags.push_back(INXString("tag 31"));
*/
	return kErr_NoErr;

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

LucidErrEnum ProjectMetaData::getScreenTagMgrData(
		ScreenMgrDlogMap_t &tagData )
{

	INXString tag; // dummy variable
	ScreenMgrSupportData_t dummy; // dummy variable
	ScreenProjMetaMapPair_t *pPr; // dummy variable

	tagData.clear(); // clear out old stuff

	ScreenProjMetaMapIt_t it = m_mapScreenTagProjMetas.begin();

	// Loop thru all screen tags in project
	while(it != m_mapScreenTagProjMetas.end()){

		pPr = (ScreenProjMetaMapPair_t *) &(*it);

		tag = pPr->first;

		char dumBuf[99];
		dummy.tagDescr = pPr->second.tagDescr; // screen tag's decsrption
		_itoa_s(getScreenTagUsageInWidgets(tag), dumBuf, 99, 10); // how many widgets tag is used on
		dummy.useCount = dumBuf;
		tagData[tag] = dummy; // add into returnable

		it++;

	} // while(it != m_mapScreenTagProjMetas.end())

	return kErr_NoErr;

}

LucidErrEnum ProjectMetaData::getGuiFiles(const INXString &screenTag, vector<GuiFileMonty_t> &guiFileVec)
{
	guiFileVec.clear();

	INXString dummyStr;

	ExtGuiFile *pGF;
	GuiFileMonty_t guiFM;


	vector<ExtGuiFile>::iterator it = m_vGuiFiles.begin();

	// Loop thru each file in vector:
	while(it != m_vGuiFiles.end()){

		pGF = &(*it);

		LucidErrEnum err = pGF->getScreenTag(dummyStr) ;
		assert( err == kErr_NoErr );
		guiFM.tag = dummyStr;

		if( guiFM.tag == screenTag ){

			LucidErrEnum err = pGF->getHostFileName(dummyStr);
			assert(  err == kErr_NoErr );
			guiFM.fileHostNameStub = dummyStr;

			err = pGF->getDescription(dummyStr);
			assert( err == kErr_NoErr );
			guiFM.fileDescr = dummyStr;


			err = getScreenTagDescription(guiFM.tag, dummyStr);
			assert(  err == kErr_NoErr );
			guiFM.tagDescr = dummyStr;

			guiFileVec.push_back(guiFM);
		}

		it++;

	} // while(it != m_vGuiFiles.end())


	return kErr_NoErr;

}

LucidErrEnum ProjectMetaData::getGuiFiles(vector<GuiFileMonty_t> &guiFileVec)
{
	guiFileVec.clear();

	INXString dummyStr;

	ExtGuiFile *pGF;
	GuiFileMonty_t guiFM;

	vector<ExtGuiFile>::iterator it = m_vGuiFiles.begin();

	// Loop thru each file in vector:
	while(it != m_vGuiFiles.end()){

		pGF = &(*it);

		LucidErrEnum err = pGF->getHostFileName(dummyStr);

		assert(  err == kErr_NoErr );
		guiFM.fileHostNameStub = dummyStr;

		err = pGF->getDescription(dummyStr);
		assert( err == kErr_NoErr );
		guiFM.fileDescr = dummyStr;


		err = pGF->getScreenTag(dummyStr) ;
		assert( err == kErr_NoErr );
		guiFM.tag = dummyStr;

		err = getScreenTagDescription(guiFM.tag, dummyStr);
		assert(  err == kErr_NoErr );
		guiFM.tagDescr = dummyStr;

		guiFileVec.push_back(guiFM);

		it++;

	} // while(it != m_vGuiFiles.end())


	return kErr_NoErr;

}


LucidErrEnum ProjectMetaData::getScreenTagDescription(
	const INXString &tag, INXString &tagDescr )
{

	ScreenProjMetaMapPair_t *pPr = NULL;

	ScreenProjMetaMapConstIt_t it = m_mapScreenTagProjMetas.find(tag);

	if(it==m_mapScreenTagProjMetas.end()){

		return kErr_ProjectMetaData_ScreenTagNotAlreadyPresent;

	}else{

		pPr = (ScreenProjMetaMapPair_t *) &(*it);
		tagDescr= pPr->second.tagDescr;
		return kErr_NoErr;
	}

}

LucidErrEnum ProjectMetaData::getPngFileByName(const INXString & hostFileName, ExtPngFile &eBF ) const
{

	eBF.setHostFileName( hostFileName );

	if( !fileInProject(eBF) ){

		return kErr_ProjectMetaData_PngFileNotAlreadyPresent;

	}else{

		vector <ExtPngFile>::const_iterator it = find( m_vPngFiles.begin(), m_vPngFiles.end(), eBF );

		eBF = *it;

		return kErr_NoErr;
	}
}


LucidErrEnum ProjectMetaData::getBmpFileByName(const INXString & hostFileName, ExtBmpFile &eBF ) const
{

	eBF.setHostFileName( hostFileName );

	if( !fileInProject(eBF) ){

		return kErr_ProjectMetaData_BmpFileNotAlreadyPresent;

	}else{

		vector <ExtBmpFile>::const_iterator it = find( m_vBmpFiles.begin(), m_vBmpFiles.end(), eBF );

		eBF = *it;

		return kErr_NoErr;
	}
}



LucidErrEnum ProjectMetaData::getDataFileByName(const INXString & hostFileName, ExtDataFile &eF ) const
{

	eF.setHostFileName( hostFileName );

	if( !fileInProject(eF) ){

		return kErr_ProjectMetaData_GuiFileNotAlreadyPresent;

	}else{

		vector <ExtDataFile>::const_iterator it = find( m_vDataFiles.begin(), m_vDataFiles.end(), eF );

		eF = *it;

		return kErr_NoErr;
	}
}

LucidErrEnum ProjectMetaData::getResourceFileByName(const INXString & hostFileName, ExtResourceFile &eF ) const
{

	eF.setHostFileName( hostFileName );

	if( !fileInProject(eF) ){

		return kErr_ProjectMetaData_GuiFileNotAlreadyPresent;

	}else{

		vector <ExtResourceFile>::const_iterator it = find( m_vResourceFiles.begin(), m_vResourceFiles.end(), eF );

		eF = *it;

		return kErr_NoErr;
	}
}



LucidErrEnum ProjectMetaData::getGuiFileByName(const INXString & hostFileName, ExtGuiFile &eGF ) const
{

	eGF.setHostFileName( hostFileName );

	if( !fileInProject(eGF) ){

		return kErr_ProjectMetaData_GuiFileNotAlreadyPresent;

	}else{

		vector <ExtGuiFile>::const_iterator it = find( m_vGuiFiles.begin(), m_vGuiFiles.end(), eGF );

		eGF = *it;

		return kErr_NoErr;
	}
}

LucidErrEnum ProjectMetaData::getBdfFileByName(const INXString & hostFileName, ExtBdfFile &eBF ) const
{

	eBF.setHostFileName( hostFileName );

	if( !fileInProject(eBF) ){

		return kErr_ProjectMetaData_BdfFileNotAlreadyPresent;

	}else{

		vector <ExtBdfFile>::const_iterator it = find( m_vBdfFiles.begin(), m_vBdfFiles.end(), eBF );

		eBF = *it;

		return kErr_NoErr;
	}
}

LucidErrEnum ProjectMetaData::getNlsFileByName(const INXString & hostFileName, ExtNlsFile &eNF ) const
{

	eNF.setHostFileName( hostFileName );

	if( !fileInProject(eNF) ){

		return kErr_ProjectMetaData_BdfFileNotAlreadyPresent;

	}else{

		vector <ExtNlsFile>::const_iterator it = find( m_vNlsFiles.begin(), m_vNlsFiles.end(), eNF );

		eNF = *it;

		return kErr_NoErr;
	}
}

int ProjectMetaData::getScreenTagUsageInWidgets(const INXString & screenTag)
{

	int tagUsage = 0;

	INXString widgetScreenTag = "";

	vector<GuiWidget>::iterator it = m_vGuiWidgets.begin();

	while(it != m_vGuiWidgets.end()){

		LucidErrEnum err = (*it).getScreenTag(widgetScreenTag);

		assert( err == kErr_NoErr);

		if(widgetScreenTag == screenTag) tagUsage++;

		it++;

	} // while(it != m_mapScreenTagProjMetas.end())

	return tagUsage;

}

bool ProjectMetaData::projectIsSet(void)
{
	return m_bProjectIsSet;
}

void ProjectMetaData::testComponents(void)
{
	testAddScreenTag();



}


void ProjectMetaData::testAddScreenTag()
{
	LucidErrEnum err;
	m_mapScreenTagProjMetas.clear();

	err = createNewScreenTag("tag1", "Description1" );
	err = createNewScreenTag("tag2", "Description2" );
	err = createNewScreenTag( INXString("tag3"), INXString("Description3") );
}

bool ProjectMetaData::screenTagInProject(const INXString & tag)
{
	if(m_mapScreenTagProjMetas.size() == 0)return false;

	return( !(m_mapScreenTagProjMetas.find(tag) == 
						m_mapScreenTagProjMetas.end()) );

}

LucidErrEnum ProjectMetaData::initialiseProjectLgbBitmaps(void)
{
	INXString installDir;



	return LucidErrEnum();
}

LucidErrEnum ProjectMetaData::getTargetFileNameForDataHostFileName(
				const INXString &dataHostFileName, INXString &targetFileName ) const
{

	ExtDataFile eF;
	LucidErrEnum err = getDataFileByName( dataHostFileName, eF );

	if(err!=kErr_NoErr){

		return err;

	}else{ // if(err==kErr_NoErr

		err =  eF.getTargetFileName( targetFileName );

		return err;

	} // if(err==kErr_NoErr

}

LucidErrEnum ProjectMetaData::getTargetFileNameForBmpHostFileName(
				const INXString &bmpHostFileName, INXString &targetFileName ) const
{

	ExtBmpFile eBF;
	LucidErrEnum err = getBmpFileByName( bmpHostFileName, eBF );

	if(err!=kErr_NoErr){

		return err;

	}else{ // if(err==kErr_NoErr

		err =  eBF.getTargetFileName( targetFileName );

		return err;

	} // if(err==kErr_NoErr

}

LucidErrEnum ProjectMetaData::getTargetFileNameForPngHostFileName(
				const INXString &pngHostFileName, INXString &targetFileName ) const
{

	ExtPngFile eBF;
	LucidErrEnum err = getPngFileByName( pngHostFileName, eBF );

	if(err!=kErr_NoErr){

		return err;

	}else{ // if(err==kErr_NoErr

		err =  eBF.getTargetFileName( targetFileName );

		return err;

	} // if(err==kErr_NoErr

}

LucidErrEnum ProjectMetaData::getTargetFileNameForBdfHostFileName(
				const INXString &bdfHostFileName, INXString &targetFileName ) const
{

	ExtBdfFile eBF;
	LucidErrEnum err = getBdfFileByName( bdfHostFileName, eBF );

	if(err!=kErr_NoErr){

		return err;

	}else{ // if(err==kErr_NoErr

		err =  eBF.getTargetFileName( targetFileName );

		return err;

	} // if(err==kErr_NoErr

}

LucidErrEnum ProjectMetaData::getTargetFileNameForNlsHostFileName(
				const INXString &nlsHostFileName, INXString &targetFileName ) const
{

	ExtNlsFile eNF;
	LucidErrEnum err = getNlsFileByName( nlsHostFileName, eNF );

	if (err!=kErr_NoErr) {
	}
	else { // if(err==kErr_NoErr
		err =  eNF.getTargetFileName(targetFileName);
	} // if(err==kErr_NoErr

	return err;
}


bool ProjectMetaData::getRealLock(char* szFile, int iLineNum)
{
	//return true; //@todo these locks are causing problems
	CLabLgbBaseApp *pBaseApp = (CLabLgbBaseApp *) AfxGetApp();
	char szMsg[256] = {'\0'};

	sprintf(szMsg, "getLock() called by %s:%d\n", szFile, iLineNum);
	pBaseApp->c_Log.WriteLogMsg(szMsg);

	if(m_bLocked) {
		AfxMessageBox("LOCK: BLOCKED");
		assert( 1==0 );

	} else {

		//try to read the lock file
		//if exists then already a lock on this project, return error
		//else no lock, so create lock file
		INXString projLockFile;
		getProjectDir(projLockFile);
		projLockFile += PROJ_LOCK_FILE_NAME;

//		AfxMessageBox("create lock fopen: " + projLockFile);
		
		FILE * pFile = NULL;
		pFile = EhsFopen(projLockFile,"r");
		if (pFile != NULL) {
			EhsFclose(pFile);

			// failed to get lock - allow a manual override
			if (AfxMessageBox("PROJECT LOCKED BY ANOTHER APPLICATION\nDo you want to override the lock?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
				m_bLocked = true;
				return true;
			} else {
				m_bLocked = false;
				return false;
			}

//			AfxMessageBox("PROJECT LOCKED BY ANOTHER APPLICATION: BLOCKED");
		} else {
			pFile = EhsFopen(projLockFile,"w");
			if (pFile == NULL) {
				// not expecting this
				AfxMessageBox("Failed to create lock file for project in: " + projLockFile);
				m_bLocked = false;
				return false;
			} else {
				//success
				EhsFclose(pFile);
				m_bLocked = true;
				return true;
			}
		}



/*		INXString dum;
		getProjectDir(dum);
		dum += PMD_LOCK_FILE_NAME;

		m_hLockFileHandle = CreateFile(
			dum,						// open One.txt
			GENERIC_READ,				// open for reading
			FILE_SHARE_WRITE,			// nothing else can open the file
			NULL,						// no security
			OPEN_ALWAYS,				// existing or new file
			FILE_ATTRIBUTE_NORMAL,		// normal file
			NULL);						// no attr. template


		if ( m_hLockFileHandle == INVALID_HANDLE_VALUE ){
			AfxMessageBox("LOCK: INVALID HANDLE VALUE");
			m_bLocked = false;
			return false;
		} else {

			m_bLocked = true;
			return true;
		}
*/
	}
}

bool ProjectMetaData::releaseRealLock(char* szFile, int iLineNum)
{
	//return true;
	CLabLgbBaseApp *pBaseApp = (CLabLgbBaseApp *) AfxGetApp();
	char szMsg[256] = {'\0'};

	sprintf(szMsg, "releaseLock() called by %s:%d\n", szFile, iLineNum);
	pBaseApp->c_Log.WriteLogMsg(szMsg);

    assert(m_bLocked == true);

	//try to read the lock file
	//if exists then remove the lock
	//else no lock file to remove, warn user but continue
	INXString projLockFile;
	getProjectDir(projLockFile);
	projLockFile += PROJ_LOCK_FILE_NAME;

	DWORD err_code = 0;
	FILE * pFile = NULL;

//	AfxMessageBox("release lock fopen: " + projLockFile);

	pFile = EhsFopen(projLockFile,"r");
	if (pFile != NULL) {
		EhsFclose(pFile); 
		if( EhsFremove(projLockFile) != 0 ) {
			//
			// error removing lock - is the file still open?
			err_code = GetLastError();
			AfxMessageBox("Failed to remove lock file for project, check if another application is locking it");
			return false;
		} else {
			// success
//				AfxMessageBox("project unlocked");
			m_bLocked = false;
			return true;
		}
	} else {
		// not expecting this but not a critical error, continue
//			AfxMessageBox("Failed to find lock file for project");
		m_bLocked = false;
		return true;
	}

/*
	if( CloseHandle( m_hLockFileHandle ) ){
		m_bLocked = false;
		return true;
	}else
		return false;
*/
}

LccTimeStamp ProjectMetaData::getLastFileReadTime()
{
	return m_cLastFileReadTime;
}

LccTimeStamp ProjectMetaData::getLastTransferTime()
{
	return m_cLastTransferTime;
}

void ProjectMetaData::updateTransferTime()
{
	CTime dummy = CTime::GetCurrentTime();
	bool bSucceed = dummy.GetAsDBTIMESTAMP( m_cLastTransferTime );
	assert(bSucceed);
}

void ProjectMetaData::updateFileReadTime()
{
	CTime dummy = CTime::GetCurrentTime();
	bool bSucceed = dummy.GetAsDBTIMESTAMP( m_cLastFileReadTime );
	assert(bSucceed);
}

void ProjectMetaData::getListOfPreviousTransferredItems(LccPmdXfers &vAlreadyTransItems) {
	char *name = NULL, * value = NULL;
	xmlTextReaderPtr reader;
	int ret = 0;
	LccPmdXfers::TypesEnum foundType = LccPmdXfers::kNothing;

	INXString csProjectFilePath;
	getFullPathProjectFile(csProjectFilePath);


	reader = xmlNewTextReaderFilename(csProjectFilePath);	
	if (reader != NULL) {

		// read next node
		ret = xmlTextReaderRead(reader);

		while (ret == 1) {
			name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!
//			AfxMessageBox(name);

			// if this is a node end type, then skip this node
			if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
				ret = xmlTextReaderRead(reader);
				continue;
			}
			// if this is a #text type in an unexpected place, then skip this node
			if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
				ret = xmlTextReaderRead(reader);
				continue;
			}

			// assuming xml is structured so location tags come after appropriate "datafileset", "imagefileset", etc tag
			if (strcmp(LUCIDPROJECT_LOCATION, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					vAlreadyTransItems.addItem(foundType, value);
			}else if (strcmp(LUCIDPROJECT_DATAFILESET, name) == 0) {
				foundType = LccPmdXfers::kData;
			}else if (strcmp(LUCIDPROJECT_IMAGEFILESET, name) == 0) {
				foundType = LccPmdXfers::kImages;
			}else if (strcmp(LUCIDPROJECT_LAYOUTFILESET, name) == 0) {
				foundType = LccPmdXfers::kLayouts;
			}else if (strcmp(LUCIDPROJECT_RESOURCEFILESET, name) == 0) {
				foundType = LccPmdXfers::kResource;
			}else if (strcmp(LUCIDPROJECT_MISCFILESET, name) == 0) {
				foundType = LccPmdXfers::kMisc;
			}else if (strcmp(LUCIDPROJECT_FONTFILESET, name) == 0) {
				foundType = LccPmdXfers::kFonts;
			}

			// read next node
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
	}	
}

int ProjectMetaData::inflateTransferrableData( 
							std::vector< TransferData_t > &vTransferrablesData, 
							  const bool &updatedOnly, const bool &bAppUpload)
{
	LucidErrEnum err;
	INXString fullPath;
	INXString projDir, csInstallDir;
	INXString csDum;
	CTime cTime;
	BOOL bWorking;
	BOOL bSucceed;
	BOOL bIsAlreadyTransferred;
	LccTimeStamp timeStamp;
	TransferData_t transferData;
	LccPmdXfers::DataPair_t tripleData;
	LccPmdXfers::DataPair_t prevDataPair;
	CDrawGUIApp *pApp = ( CDrawGUIApp *  )AfxGetApp();

	//LucidErrEnum err = pProjMetaData->getProjectDir( projDir );
	//assert(kErr_NoErr == err);

	// Update the complete (meta) list of everything that might need to go across.
	updateTransferrables(bAppUpload);

	LccPmdXfers vAlreadyTransItems;

	if(updatedOnly) getListOfPreviousTransferredItems(vAlreadyTransItems);

	CFileFind finder;

	vTransferrablesData.clear();

	for(size_t i=0; i<m_cTransferables.getCount();i++){

		tripleData = m_cTransferables.getItem( i );

		transferData.type = tripleData.type;


		if(tripleData.type == LccPmdXfers::kData){

			transferData.csHostFileFolder = m_csProjectDir + DDATADIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			err = getTargetFileNameForDataHostFileName( tripleData.hostFileName, csDum );
			assert( kErr_NoErr==err);
			transferData.csTargetFileName = csDum;

		}else if( tripleData.type == LccPmdXfers::kImages){ 

			transferData.csHostFileFolder = m_csProjectDir + GUIDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			err = getTargetFileNameForPngHostFileName( tripleData.hostFileName, csDum );
			assert( kErr_NoErr==err);
			transferData.csTargetFileName = csDum;

		}else if( tripleData.type == LccPmdXfers::kLayouts) {

			transferData.csHostFileFolder = m_csProjectDir + GUIDIR;
			transferData.csHostFileStubName = tripleData.hostFileName;
			transferData.csHostFileFullName = transferData.csHostFileStubName + ".gui";

			err = getTargetFileNameForGuiHostFileName( tripleData.hostFileName, csDum );
			assert( kErr_NoErr==err);
			transferData.csTargetFileName = csDum;

		}else if(tripleData.type == LccPmdXfers::kResource) {

			transferData.csHostFileFolder = m_csProjectDir + RESOURCEDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";
			
			// Target name is same as host name for resource files
			//err = getTargetFileNameForDataHostFileName( tripleData.hostFileName, csDum );
			//assert( kErr_NoErr==err);
			transferData.csTargetFileName = tripleData.hostFileName;

		}else if(tripleData.type == LccPmdXfers::kMisc){

			transferData.csHostFileFolder = m_csProjectDir + SODLDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			transferData.csTargetFileName = SODLFILENAME;

		} else if(tripleData.type == LccPmdXfers::kDescription){

			transferData.csHostFileFolder = m_csProjectDir + APP_DESC_DIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			transferData.csTargetFileName = APP_DESC_FILE;
		}
		else if (tripleData.type == LccPmdXfers::kFonts) {
			pApp->GetInstallationBaseDir(csInstallDir);
			transferData.csHostFileFolder =  csInstallDir + FONTDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			err = getTargetFileNameForBdfHostFileName( tripleData.hostFileName, csDum );
			assert( kErr_NoErr==err);
			transferData.csTargetFileName = csDum;
		}
		else if (tripleData.type == LccPmdXfers::kNls) { 

			transferData.csHostFileFolder = m_csProjectDir + NLSDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";

			err = getTargetFileNameForNlsHostFileName( tripleData.hostFileName, csDum );
			assert( kErr_NoErr==err);
			transferData.csTargetFileName = csDum;
		}
		else if(tripleData.type == LccPmdXfers::kResource) {

			transferData.csHostFileFolder = m_csProjectDir + RESOURCEDIR;
			transferData.csHostFileFullName = tripleData.hostFileName;
			transferData.csHostFileStubName = "";
			
			// Target name is same as host name for resource files
			//err = getTargetFileNameForDataHostFileName( tripleData.hostFileName, csDum );
			//assert( kErr_NoErr==err);
			transferData.csTargetFileName = tripleData.hostFileName;
		}
		else {
			assert(1==0);
		}


		if(updatedOnly){

			bWorking = finder.FindFile( 
				transferData.csHostFileFolder + transferData.csHostFileFullName );

			while (bWorking) {

				bWorking = finder.FindNextFile();
				bSucceed = finder.GetLastWriteTime( cTime );
				assert(bSucceed);

				bSucceed = cTime.GetAsDBTIMESTAMP( timeStamp );
				assert(bSucceed);

				// test to see if this file has been transferred before, if not send it
				bIsAlreadyTransferred = false;
				for(size_t j=0; j<vAlreadyTransItems.getCount();j++){
					prevDataPair = vAlreadyTransItems.getItem(j);

					if (tripleData.type == prevDataPair.type) {
						if (strcmp(tripleData.hostFileName, prevDataPair.hostFileName) == 0){
							bIsAlreadyTransferred = true;
							break;
						}
					}
				}

				// we always need to send SODL to force EHS to reload the app
				if(!bIsAlreadyTransferred || timeStamp.isNewerThan( m_cLastTransferTime ) || (transferData.csTargetFileName == SODLFILENAME) ){
					vTransferrablesData.push_back( transferData );
				}

			} // while (bWorking)

		} // if(updatedOnly) 
		else {
			vTransferrablesData.push_back( transferData );

		} // if(!updatedOnly) 

	} // for(size_t i=0; i<m_vContents.size();i++)

	return 	vTransferrablesData.size();
}


void ProjectMetaData::updateTransferrables(const bool &bAppUpload)
{
	assert(m_bLocked);

	INXString csHostFileName;
	INXString csTgtFileName;
	
	m_cTransferables.clear();

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the sodl to the list - this file is ALWAYS present
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_cTransferables.addItem( LccPmdXfers::kMisc,  SODLFILENAME );

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the info.xml to the list - this file is ALWAYS present for APP UPLOAD
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (bAppUpload) {
		m_cTransferables.addItem( LccPmdXfers::kDescription,  APP_DESC_FILE );
	}


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the data files to the list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::vector< ExtDataFile > vDataFiles;

	LucidErrEnum err = getDataFiles( vDataFiles );
	assert(kErr_NoErr== err);

	//set<INXString>::iterator it = vDataFiles.begin();
	//while( it != vDataFiles.end() ){
	for(size_t i=0;i<vDataFiles.size(); i++){
		vDataFiles[i].getHostFileName( csHostFileName );
		//err = getTargetFileNameForDataHostFileName( csHostFileName, csTgtFileName );
		//assert(kErr_NoErr== err);

		m_cTransferables.addItem( LccPmdXfers::kData, csHostFileName );
		//it++;
	}


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the IMAGE files to the list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::set< INXString > vImageFiles;

	err = ((CLabLgbBaseApp *)AfxGetApp())->getActiveImages( this, vImageFiles );
	assert(kErr_NoErr== err);

	set<INXString>::iterator it = vImageFiles.begin();
	while( it != vImageFiles.end() ){
	//for(size_t i=0;i<vImageFiles.size(); i++)
		m_cTransferables.addItem( LccPmdXfers::kImages, *it );
		it++;
	}


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the LAYOUT files related to by the content-files in the layout list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Previously only sent layout files if there were widgets.
	// Now always send the layout files even if there are no widgets
	//if (m_vGuiWidgets.size() > 0) {
	std::set< INXString > vLayoutNames;
	getActiveLayouts( vLayoutNames );

	it = vLayoutNames.begin();
	while( it != vLayoutNames.end() ){
	//for(size_t i=0;i<vLayoutNames.size(); i++)
		m_cTransferables.addItem( LccPmdXfers::kLayouts,  *it );
		it++;
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the resource files to the list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::vector< ExtResourceFile > vResourceFiles;

	err = getResourceFiles( vResourceFiles );
	assert(kErr_NoErr== err);

	for(size_t i=0;i<vResourceFiles.size(); i++){
		vResourceFiles[i].getHostFileName( csHostFileName );

		m_cTransferables.addItem( LccPmdXfers::kResource, csHostFileName );
	}
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the bdf files to the list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	std::set< INXString > vFontNames;

	err = ((CLabLgbBaseApp *)AfxGetApp())->getActiveFonts( this, vFontNames );
	assert(kErr_NoErr== err);

	it = vFontNames.begin();
	while( it != vFontNames.end() ){
		m_cTransferables.addItem( LccPmdXfers::kFonts, *it );
		it++;
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add the nls files to the list
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::vector< ExtNlsFile > vNlsFiles;

	err = getNlsFiles( vNlsFiles );
	assert(kErr_NoErr== err);

	for (size_t i=0; i<vNlsFiles.size(); i++) {
		vNlsFiles[i].getHostFileName( csHostFileName );
		m_cTransferables.addItem( LccPmdXfers::kNls, csHostFileName );
	}
	
	int i=0;

}

/*
 * Called when DEP has been changed, making the SODL out of date
 * Creates a file used by IGB to warn user if SODL is out of date
 * note because this method is called so freq we are using a state flag as it is quicker
 * to check than testing for the existence of a file
 */
void ProjectMetaData::setDirtySODLFlag()
{
	if (!isDirtySODL) {
		isDirtySODL = true;
		// write file 
		INXString csProjectDir;
		getProjectDir(csProjectDir);
		INXString updateSODLFileName;
		updateSODLFileName = csProjectDir + UPDATE_SODL_FILE_NAME;
		FILE * pFile = NULL;

		pFile = EhsFopen(updateSODLFileName,"w");
		if (pFile == NULL) {
			// not expecting this
			AfxMessageBox("Failed to create file for project in: " + updateSODLFileName);
		} else {
			//success
			EhsFclose(pFile);
		}
	}
}

void ProjectMetaData::clearDirtySODLFlag()
{
	isDirtySODL = false;

	INXString csProjectDir;
	getProjectDir(csProjectDir);
	INXString updateSODLFileName;
	updateSODLFileName = csProjectDir + UPDATE_SODL_FILE_NAME;
	FILE * pFile = NULL;

	pFile = EhsFopen(updateSODLFileName,"r");
	if (pFile != NULL) {
		EhsFclose(pFile); 
		if( EhsFremove(updateSODLFileName) != 0 ) {
			//
			// error removing file - is it still open?
			AfxMessageBox("Failed to remove file: " + updateSODLFileName);
		} else {
			// success
		}
	}
}

/*
 * Note this is used by IGB to test for changes in IAB that have made the
 * the SODL out-of-date - so always check the file and never the state variable in IGB
 */
bool ProjectMetaData::checkDirtySODL()
{
	if (isSODLCheckBeingPerformed) {
		INXString csProjectDir;
		getProjectDir(csProjectDir);
		INXString updateSODLFileName;
		updateSODLFileName = csProjectDir + UPDATE_SODL_FILE_NAME;
		FILE * pFile = NULL;

		pFile = EhsFopen(updateSODLFileName,"r");
		if (pFile != NULL) {
			EhsFclose(pFile); 
			return true;
		}
	}
	return false;
}

/*
 * Called when Widgets have been changed, making the SODL out of date and potentially affecting IGB design
 * Creates a file used by IGB to warn user if Widgets out of date
 */
void ProjectMetaData::setDirtyWidgetsFlag()
{
	// write file 
	INXString csProjectDir;
	getProjectDir(csProjectDir);
	INXString updateWidgetsFileName;
	updateWidgetsFileName = csProjectDir + UPDATE_WIDGETS_FILE_NAME;
	FILE * pFile = NULL;

	pFile = EhsFopen(updateWidgetsFileName,"w");
	if (pFile == NULL) {
		// not expecting this
		AfxMessageBox("Failed to create file for project in: " + updateWidgetsFileName);
	} else {
		//success
		EhsFclose(pFile);
	}
}

void ProjectMetaData::clearDirtyWidgetsFlag()
{
	INXString csProjectDir;
	getProjectDir(csProjectDir);
	INXString updateWidgetsFileName;
	updateWidgetsFileName = csProjectDir + UPDATE_WIDGETS_FILE_NAME;
	FILE * pFile = NULL;

	pFile = EhsFopen(updateWidgetsFileName,"r");
	if (pFile != NULL) {
		EhsFclose(pFile); 
		if( EhsFremove(updateWidgetsFileName) != 0 ) {
			//
			// error removing file - is it still open?
			AfxMessageBox("Failed to remove file: " + updateWidgetsFileName);
		} else {
			// success
		}
	}
}

/*
 * Note this is used by IGB to test for changes in IAB that have made the
 * the SODL out-of-date - so always check the file and never the state variable in IGB
 */
bool ProjectMetaData::checkDirtyWidgets()
{
	if (isSODLCheckBeingPerformed) {
		INXString csProjectDir;
		getProjectDir(csProjectDir);
		INXString updateWidgetsFileName;
		updateWidgetsFileName = csProjectDir + UPDATE_WIDGETS_FILE_NAME;
		FILE * pFile = NULL;

		pFile = EhsFopen(updateWidgetsFileName,"r");
		if (pFile != NULL) {
			EhsFclose(pFile); 
			return true;
		}
	}
	return false;
}


/*
 * Note this is used by IGB to test for changes in IAB that have made the
 * the SODL out-of-date - so always check the file and never the state variable
 */
bool ProjectMetaData::checkNewerSODLAvailable()
{
	if (isSODLCheckBeingPerformed) {
		if (nBuildNo < readBuildNumFromFile()) {
			return true;
		}
	}
	return false;
}

void ProjectMetaData::setIsSODLCheckBeingPerformed(bool val)
{
	isSODLCheckBeingPerformed = val;
}

void ProjectMetaData::populateBuildNumFromFile(void){
	this->nBuildNo = readBuildNumFromFile();
}

/*
* reads build number from file
*/
int ProjectMetaData::readBuildNumFromFile(void)
{
	INXString csProjectDir;
	char lineFromFile[256];
	getProjectDir(csProjectDir);
	int num;
	ifstream read;
	read.open(csProjectDir+"\\build.num",ios_base::in);
	if ( read.is_open() ) {
		read.getline(lineFromFile,256);
		if (read.rdstate() & ifstream::failbit)
			num=1;
		else
			num=atoi(lineFromFile);
	}
	else {
		num=0;
		}
	read.close();
	return num;
}

int ProjectMetaData::getBuildNum() {
	return nBuildNo;
}