#include "stdafx.h"
#include "../common/LabLgbBaseApp.h"
#include "../common/LucidConstants.h"
#include "../common/ProjectMetaData.h"
#include "../common/common.h" //'getlowercase'
#include "../LucidApplicationBuilder/FileOperations.h"
#include "../common/LucidEnums.h"
#include "../common/LucidTcpipClient.h"
#include "../common/TgtTransProgDlog.h"
#include "../common/GlobalFuncs_1.h"
#include "../common/GlobalFuncs_2.h"
#include "LgbIconDataStrucs.h"
#include "TransferErrorDlog.h"
#include "libxml/xmlreader.h"
#include "LucidRegAccess.h"
#include "../common/EHSParamsDialog.h"

#include <sys/stat.h>
#include <afxadv.h>
#include <cassert>
#include <curl/curl.h>

#define NUMBER_OF_ERROR_LINES_TO_LISTEN_FOR 3
#define EHSGRAPHICS_XCOORD "xcoord"
#define EHSGRAPHICS_YCOORD "ycoord"
#define EHSGRAPHICS_WIDTH "width"
#define EHSGRAPHICS_HEIGHT "height"
#define EHSGRAPHICS_RED "red"
#define EHSGRAPHICS_GREEN "green"
#define EHSGRAPHICS_BLUE "blue"
#define EHSGRAPHICS_ALPHA "alpha"
#define EHSGRAPHICS_HASFRAME "hasFrame"
#define EHSGRAPHICS_ZORDER "zorder"

CTgtTransProgDlog * CLabLgbBaseApp::c_pTgtTransProgressDlog;
CTransferErrorDlog * CLabLgbBaseApp::c_pTgtTransErrorDlog;

bool CLabLgbBaseApp::c_bSendAllToTargetHasOccurred;
Log CLabLgbBaseApp::c_Log;

CLabLgbBaseApp::CLabLgbBaseApp(void)
: m_pProjMetaData(NULL)
{
	m_pProject = NULL;
	c_bSendAllToTargetHasOccurred = FALSE;
	csCurrentTarget = REG_TARGET_LOCATION_DATA_LOCAL;
	isEHSLocalInstallPathKnown = false;

	InitialiseWinsock();

	// global init for libcurl, only needs to be run once
	curl_global_init(CURL_GLOBAL_ALL);


	// set the current working directory - needed by WINE to start EHS using shellExecute
	INXString baseDir;
	CLabLgbBaseApp *pApp = ( CLabLgbBaseApp * ) AfxGetApp();
	pApp->GetInstallationBaseDir(baseDir);	//base dir is something like C:/Program Files/inx/tools (whereever the iab exe is being run from)
	SetCurrentDirectory(baseDir);
//	AfxMessageBox("base dir:" + baseDir); 


	// determine which OS we are installed in - windows or linux
	INXString testPath = "/etc";	//this can be any file that is always found on a linux system
	isInstalledInWindows = false;			

	struct stat xFileInfo;
	// stat returns -1 if fails, 0 otherwise
	if (stat(testPath,&xFileInfo) == -1) {
		isInstalledInWindows = true;		
//		AfxMessageBox("Can't open file for reading :" + testPath + "\nRunning in Windows OS");
//	} else {
//		AfxMessageBox("Found file :" + testPath + "\nRunning in Linux OS");
	}
	//@todo - implement some code to distinguish between wine under Linux from wine under MAC OS
	// this means we have a three ways of running the tools:
	// 1. windows OS - IAB/IGB + EHS all run in windows env
	// 2. linux OS - IAB/IGB run in windows env in wine and EHS runs in linux
	// 3. MAC OS - IAB/IGB + EHS run in windows env in wine


	pEHSInitParams = new EHSInitParams;
	getEHSParamsFromFile();
}

CLabLgbBaseApp::~CLabLgbBaseApp(void)
{
	// Winsock dll cleanup
	WSACleanup();
}

/**
 * Return free disk space available to user, in MB
 *
 */
double CLabLgbBaseApp::MyGetFreeDiskSpaceMB()
{	
	ULARGE_INTEGER freeBytes;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytes;
	BOOL succ = false;

	INXString baseDir;
	GetInstallationBaseDir(baseDir);	//base dir is where the inx exe is being run from

	succ = GetDiskFreeSpaceEx(baseDir, &freeBytes, &totalBytes, &totalFreeBytes);
//	double dUserFreeSpace = 9.31322575e-10;	// convert bytes into gigabytes
	double dUserFreeSpace = 9.53674316e-7; // convert bytes to megabytes
	dUserFreeSpace *= (double) freeBytes.QuadPart;

/*
	INXString strValue,strInt, strDecimal;
	int decimal,sign;
	strValue = _fcvt(dUserFreeSpace,3,&decimal,&sign); 
	strInt = strValue.Left(decimal); // strInt contains 4
	strDecimal = strValue.Mid(decimal); // strDecimal contains 125

	INXString strFinalVal;
	strFinalVal.Format("%s.%s",strInt,strDecimal);
	AfxMessageBox("user free MB: " + strFinalVal);
*/
	return dUserFreeSpace;
}


void CLabLgbBaseApp::InitialiseWinsock()
{	
	ltsStatusType=LTS_STATUS_OK;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
		ltsStatusType=LTS_STATUS_FAIL;
	}
}


/*
 Parse the XML file using libXML2 that holds the information regarding initialisation params for EHS
 @todo - move this code to EHSParamDialog::init()
*/
void CLabLgbBaseApp::getEHSParamsFromFile()
{
	char *name = NULL, * value = NULL;
	xmlTextReaderPtr reader;
	int ret = 0;

	INXString baseDir;
	GetInstallationBaseDir(baseDir);	//base dir is where the inx exe is being run from

	INXString csEHSParamFilePath = baseDir;
	if (isInstalledInWindows) {
		csEHSParamFilePath += EHS_WIN_VAR_DIR;
	} else { /* assume windows, in which case we need to check EHS's install directory */
		struct stat xFileInfo2;
		if (stat(EHS_LINUX_OPT_INSTALL_DIR,&xFileInfo2) == 0) { /* is this the opt version */
			csEHSParamFilePath = EHS_LINUX_OPT_INSTALL_DIR;
		}else { /* this is a tools path installed version of EHS */
			csEHSParamFilePath += EHS_LINUX_VAR_DIR;
		}
	}

	// test if local install directory of EHS is where it is expected to be
	//@todo - log unknown local EHS path error in console
	struct stat xFileInfo;
	// stat returns -1 if fails, 0 otherwise
	if (stat(csEHSParamFilePath,&xFileInfo) == 0) {
		isEHSLocalInstallPathKnown = true;		

		csEHSParamFilePath += EHS_SYSDATA_DIR;
		csEHSParamFilePath += EHS_GRAPHICS_INFO_FILE;

		reader = xmlNewTextReaderFilename(csEHSParamFilePath);	
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


				if (strcmp(EHSGRAPHICS_XCOORD, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nXCoord = atoi(value);
				} else if (strcmp(EHSGRAPHICS_YCOORD, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nYCoord = atoi(value);
				} else if (strcmp(EHSGRAPHICS_WIDTH, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nWidth = atoi(value);
				} else if (strcmp(EHSGRAPHICS_HEIGHT, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nHeight = atoi(value);
				} else if (strcmp(EHSGRAPHICS_RED, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nColourRed = atoi(value);
				} else if (strcmp(EHSGRAPHICS_GREEN, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nColourGreen = atoi(value);
				} else if (strcmp(EHSGRAPHICS_BLUE, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nColourBlue = atoi(value);
				} else if (strcmp(EHSGRAPHICS_ALPHA, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nAlpha = atoi(value);
				} else if (strcmp(EHSGRAPHICS_HASFRAME, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nHasFrame = atoi(value);
				} else if (strcmp(EHSGRAPHICS_ZORDER, name) == 0) {
					ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
					value = (char*) xmlTextReaderConstValue(reader);
					pEHSInitParams->nZOrder = atoi(value);
				}

				// read next node
				ret = xmlTextReaderRead(reader);
			}
			xmlFreeTextReader(reader);

		} else {
			// write defaults to file
			//todo - we want to show this sort of message in the error console when it is implemented instead of the first thing the user sees when they start IAB
	//        AfxMessageBox("Unable to open file " + csEHSParamFilePath + ".\nUsing defaults for EHS initialisation parameters");

			FILE* pFile;
			errno_t error;

			error=fopen_s(&pFile,csEHSParamFilePath,"w");
			if (error == 0) {
				fprintf(pFile,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
				fprintf(pFile,"<graphics>\n");
				fprintf(pFile,"<initialPosition>\n");
				fprintf(pFile,"  <xcoord>%d</xcoord>\n",pEHSInitParams->nXCoord);
				fprintf(pFile,"  <ycoord>%d</ycoord>\n",pEHSInitParams->nYCoord);
				fprintf(pFile,"  <width>%d</width>\n",pEHSInitParams->nWidth);
				fprintf(pFile,"  <height>%d</height>\n",pEHSInitParams->nHeight);
				fprintf(pFile,"</initialPosition>\n");
				fprintf(pFile,"<colour>\n");
				fprintf(pFile,"  <red>%d</red>\n",pEHSInitParams->nColourRed);
				fprintf(pFile,"  <green>%d</green>\n",pEHSInitParams->nColourGreen);
				fprintf(pFile,"  <blue>%d</blue>\n",pEHSInitParams->nColourBlue);
				fprintf(pFile,"  <alpha>%d</alpha>\n",pEHSInitParams->nAlpha);
				fprintf(pFile,"</colour>\n");
				fprintf(pFile,"<hasFrame>%d</hasFrame>\n",pEHSInitParams->nHasFrame);
				fprintf(pFile,"<zorder>%d</zorder>\n",pEHSInitParams->nZOrder);
				fprintf(pFile,"</graphics>\n");
				fclose(pFile);
			}
				
		}
	}
}



BOOL CLabLgbBaseApp::CheckExecutableFolderContextIsOk() 
{

	BOOL retVal = false;

	INXString baseDir;
	GetInstallationBaseDir(baseDir);

	CFileOperation fo;

	// Check various 'child' folders are present, otherwise the app won't
	// find bitmaps etc and will fail.

	// For some reason, the const INXStrings defining these 
	// in LucidConstants.h haven't been set
	// at this point!

	int result = fo.CheckPath( baseDir + "\\BMP"  );
	if(result != PATH_IS_FOLDER) return false;

	result = fo.CheckPath( baseDir + "\\IDF"  );
	if(result != PATH_IS_FOLDER) return false;

	result = fo.CheckPath( baseDir + "\\temp"  );
	if(result != PATH_IS_FOLDER) return false;

	return true;

}


BOOL CLabLgbBaseApp::GetInstallationBaseDir( INXString &installationBaseDir ) 
{

	//TCHAR test1[PATHBUFFER_SIZE];

	installationBaseDir.Empty();
	
	// Don't use this call as it returns the working dir which may well have
	// sweet FA to do with where the executable is - which is what we want.
	// MR
	//GetCurrentDirectory( PATHBUFFER_SIZE, test1 );

	INXString choppable;
	GetExecutableName( choppable); // Should now have full path to LucidDiddlyDo.exe file.

	int dumPos = choppable.Find( "\\bin" );
	choppable = choppable.Left(dumPos);

	char zappedCopy[PATHBUFFER_SIZE];

	strcpy_s(zappedCopy, PATHBUFFER_SIZE, choppable );

	//getlowercase(zappedCopy, strlen(zappedCopy));

	installationBaseDir = zappedCopy;
	//remove intital quotes
	//installationBaseDir = installationBaseDir.Right(installationBaseDir.GetLength()-1);

	return TRUE;

}

void CLabLgbBaseApp::GetExecutableName(INXString & execName)
{

	LPTSTR test1 = GetCommandLine();

	execName = test1;

	// remove all leading quotes and white spaces:
	while( execName.FindOneOf(" \"") == 0 ){
		execName = execName.Right(execName.GetLength()-1);
	}

	// remove all trailing quotes and white spaces:
	execName.MakeReverse();
	while( execName.FindOneOf(" \"") == 0 ){
		execName = execName.Right(execName.GetLength()-1);
	}

	execName.MakeReverse();

	// Now detect whether there is just one arg, or more than one, and strip off the
	// trailing args if present.

	// There will be a /" /" present in the middle of the overall string if more than one arg
	int pos = execName.Find("\" \"");

	if(pos != -1){
		execName = execName.Left(pos);
	}

	return;
}


LucidErrEnum CLabLgbBaseApp::openDocInBackground(
				   const LucidManualsEnum manualTypeEnum )
{

	INXString appExecName = "";
	INXString docToBeOpened = "";

	if(manualTypeEnum == kLgbRefManual){

		docToBeOpened = LGB_REFMANUAL_FILENAME;

	}else if(manualTypeEnum == kLabUserGuide){ 

		docToBeOpened = LAB_USERGUIDE_FILENAME;

	}else if(manualTypeEnum == kLabRefManual){ 

		docToBeOpened = LAB_REFMANUAL_FILENAME;

	}else if(manualTypeEnum == kLgbLabTutorial){ 

		docToBeOpened = LGBLAB_TUTORIAL_FILENAME;

	} else {

		return kErr_ManualTypeUnknown;
	}

/*

	if(applicationEnum == kAppAcrobatReader){

		appExecName = ACROBAT_READER_EXENAME;

	}else{

		return kErr_HelperAppUnknown;

	}
*/


	INXString baseDir = "";


	GetInstallationBaseDir(baseDir);

	INXString fullManualFilePath = baseDir + MANUALSDIR + docToBeOpened;
	fullManualFilePath = cloneBackSlashes( fullManualFilePath );

	// Put proj file in quotes so a path with a spaces gets treated as a single arg.
	// fullProjFilePath = "\"" + fullProjFilePath + "\"";

	HINSTANCE hInst = ShellExecute(
		NULL, 
		"open", //appExecName, 
		//"C:\\ess2_source\\main_branch3\\dist\\manuals\\d.pdf", //
		fullManualFilePath, 
		NULL,
		NULL, 
		SW_SHOW );


	return kErr_NoErr;
}

void CLabLgbBaseApp::addProjectToMRUList(INXString csProjectPathName)
{
	// Remove .prg from MRU file list
	for(int i=0;i < this->m_pRecentFileList->GetSize();i++)	{
		INXString strFileName(this->m_pRecentFileList->m_arrNames[i]);
		if (strFileName.Right(4) != ".lpj") {
			this->m_pRecentFileList->Remove(i);
		}
	}
	// add project path name to MRU list
	this->m_pRecentFileList->Add(csProjectPathName);
}

LucidErrEnum CLabLgbBaseApp::getActiveImages(ProjectMetaData *pProjMetaData, std::set< INXString > &vImageNames )
{ 

	if( pProjMetaData->projectIsSet() ){


			// Don't read the proj file too often, or it's very likely that you'll get circular-call stack overflows.
			// Ensure that proj file has been read in the function tht called this .
			//pProjMetaData->readProjectFile();

			LucidErrEnum err;
			INXString layoutHostStub;
			INXString fileName;

			std::set<INXString> vLayoutNames;

			pProjMetaData->getActiveLayouts( vLayoutNames );

			std::set<INXString>::iterator it = vLayoutNames.begin();

			while(it != vLayoutNames.end() ){

				err = pProjMetaData->getProjectDir(fileName);
				fileName += GUIDIR + *it + ".gui";

				CFileOperation fo;
				int foErr = fo.CheckPath( fileName);
				assert( foErr != PATH_NOT_FOUND );
				assert( foErr != PATH_ERROR );

				LoadImageInfFromGuiFile( fileName, vImageNames );
				it++;

			} // for( int layout=0; layout< tags.size(); layout++)

			return kErr_NoErr;

	} else{  // project is set
	
		return kErr_ProjectFileNotRead;

	} // project isn't set.

}

LucidErrEnum CLabLgbBaseApp::getActiveFonts(ProjectMetaData *pProjMetaData, std::set< INXString > &vFontNames )
{ 
	if( pProjMetaData->projectIsSet() ){
			LucidErrEnum err;
			INXString layoutHostStub;
			INXString fileName;

			std::set<INXString> vLayoutNames;
			pProjMetaData->getActiveLayouts( vLayoutNames );

			// iterate through GUI layouts, parse each layout for fonts used
			std::set<INXString>::iterator it = vLayoutNames.begin();
			while(it != vLayoutNames.end() ){
				err = pProjMetaData->getProjectDir(fileName);
				fileName += GUIDIR + *it + ".gui";

				CFileOperation fo;
				int foErr = fo.CheckPath( fileName);
				assert( foErr != PATH_NOT_FOUND );
				assert( foErr != PATH_ERROR );

				LoadFontInfFromGuiFile( fileName, vFontNames );
				it++;
			}
			return kErr_NoErr;
	} else{  // project is set
	
		return kErr_ProjectFileNotRead;
	} // project isn't set.
}


bool gbUpdatedOnly;
int giFilesToBeTransferred;
INXString gcsProjDir;
Project *gpProject;
ProjectMetaData *gpProjMetaData;

void CLabLgbBaseApp::transferToTarget(const bool &updatedOnly) 
{
	gbUpdatedOnly = updatedOnly;
	gpProject = m_pProject;

	Sleep(50);

	giFilesToBeTransferred = copyTransferrablesToExports( m_pProjMetaData, updatedOnly, false );
/*
	char* res;
	sprintf(res, "%d", giFilesToBeTransferred);
	AfxMessageBox("nFilesToGo:" + INXString(res));
*/

	//LucidErrEnum err = m_pProjMetaData->getProjectDir(gcsProjDir);
	//assert( kErr_NoErr == err );
	gpProjMetaData = m_pProjMetaData;

	AfxBeginThread(transferToTargetThread, this);

}

UINT CLabLgbBaseApp::transferToTargetThread( LPVOID ptr ) 
{
	if (!transferExportsToTarget( gpProjMetaData, giFilesToBeTransferred )) {
		// LAB
		if (gpProject) {
			gpProject->m_bTransferAllHasOccurred = (gpProject->m_bTransferAllHasOccurred || !gbUpdatedOnly);
		}
		// LGB
		else {
			c_bSendAllToTargetHasOccurred = c_bSendAllToTargetHasOccurred || !gbUpdatedOnly;
		}
	}
	c_pTgtTransProgressDlog->ShowWindow( SW_HIDE );

	return 0;
}

UINT CLabLgbBaseApp::showTransferDialogThread( LPVOID ptr ) 
{
	INXString *cs = ( INXString * )ptr;
	c_pTgtTransProgressDlog->setPromptOverall( *cs );
	c_pTgtTransProgressDlog->ShowWindow( SW_NORMAL );

	//c_pTgtTransProgressDlog->setPromptOverallDirect( *cs );
	c_pTgtTransProgressDlog->Invalidate();
	c_pTgtTransProgressDlog->UpdateWindow();
	c_pTgtTransProgressDlog->RedrawWindow();
	delete cs;
	return 0;
}

int CLabLgbBaseApp::copyTransferrablesToExports( ProjectMetaData *pProjMetaData, 
												 const bool &updatedOnly, const bool &bAppUpload
												 ) 
{
	INXString csFileName, csFileExt;

	assert(pProjMetaData->isLocked() );

	pProjMetaData->readProjectFile();

	CFileOperation fo;
	INXString projDir;
	vector<ExtResourceFile> xResourceFilesVec;

	// 0th thing to do is to save all changed files
	// release lock for case when save is performed, because it needs lock
	pProjMetaData->releaseLock();
	SaveAllModified();
	if (!pProjMetaData->getLock()) {
		assert(1==0);
	}

	// 1st thing is to completely clear the 'exports' folder.
	pProjMetaData->getProjectDir( projDir );
	fo.DeleteFolderFiles( projDir + EXPORTDIR + "*.*" );

	std::vector< ProjectMetaData::TransferData_t > vTransferData;

	// Now get the meta list to see what has changed since the last transfer
	pProjMetaData->inflateTransferrableData( vTransferData, updatedOnly, bAppUpload );

	// Copy all files from their host name/location to their exports name / location.
	// Note that any 'gui' files need special translation because they need any refs to
	// png files (inside them) to be mapped to the targetfile name also.

	INXString csFullHostFileName;
	INXString csTargetFileName;
	
	for(size_t i=0;i<vTransferData.size(); i++)
	{
		if( vTransferData[i].type == LccPmdXfers::kLayouts ){

			//INXString csFullHostFileName = projDir + GUIDIR + vTransferData[i].csHostFileFullPath;
			translateGuiFile(	pProjMetaData, 
				vTransferData[i].csHostFileFolder + vTransferData[i].csHostFileFullName, 
								projDir + EXPORTDIR + vTransferData[i].csTargetFileName );

		} else {
			//@todo add some error trapping for missing files, currently application just terminates with an error
			fo.CopyFileGood( vTransferData[i].csHostFileFolder, vTransferData[i].csHostFileFullName,
								projDir + EXPORTDIR, vTransferData[i].csTargetFileName );
		}

	} // for(size_t i=0;i<vFullPaths.size(); i++)


	// Check copy to exports went OK.
	// Count files individually, as there may be a '.svn' file present, which would spoil
	// the 'assert' below.

	CFileFind ff;
	BOOL bRes;
	int nFiles = 0;

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.txt" );
	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.sdl" );
	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.xml" );
	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.gui" );
	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.png" );
	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.bdf" );

	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	bRes = ff.FindFile( projDir + EXPORTDIR + "*.csv" );

	while (bRes)
	{
		bRes = ff.FindNextFile();
		nFiles++;
	}

	pProjMetaData->getResourceFiles(xResourceFilesVec);
	for (UINT i=0; i<xResourceFilesVec.size(); i++) {
		xResourceFilesVec.at(i).getHostFileName(csFileName);
		if (fo.CheckPath(projDir + EXPORTDIR + csFileName)) {
			csFileExt = csFileName.Right(3);
			if (csFileExt != "txt" && csFileExt != "sdl"  && csFileExt != "xml" && csFileExt != "gui" && csFileExt != "png" && csFileExt != "bdf" && csFileExt != "csv") {
				nFiles++;
			}	
		}
	}	

	//const int EXCLUDE_PRESENTFOLDERAND_DOT = 2;
	assert( vTransferData.size()== nFiles );

	//pProjMetaData->releaseLock();

	return vTransferData.size();

}


void CLabLgbBaseApp::LoadImageInfFromGuiFile( const INXString &layoutName , std::set<INXString> &vNames)
{
	
/* Load background bitmaps from layout file */

	std::vector< LgbTextIconEssentialData_t > textIcons;
	std::vector< LgbPatchIconEssentialData_t > patchIcons;
	std::vector< LgbImageIconEssentialData_t > imageIcons;
	std::vector< LgbTextStyleEssentialData_t > vTextStyles;

	parseGuiFile( layoutName, textIcons, patchIcons, imageIcons, vTextStyles );

	for(size_t i=0;i<imageIcons.size();i++)
		vNames.insert( imageIcons[i].bitmapFileName );


}

/**
 * Retrieve a list of fonts used by the layout passed in
 */
void CLabLgbBaseApp::LoadFontInfFromGuiFile( const INXString &layoutName , std::set< INXString > &vFontNames)
{
	std::vector< LgbTextIconEssentialData_t > textIcons;
	std::vector< LgbPatchIconEssentialData_t > patchIcons;
	std::vector< LgbImageIconEssentialData_t > imageIcons;
	std::vector< LgbTextStyleEssentialData_t > vTextStyles;

	parseGuiFile( layoutName, textIcons, patchIcons, imageIcons, vTextStyles );

	for(size_t i=0;i<textIcons.size();i++)
		vFontNames.insert( textIcons[i].csFont + BDF_FILE_EXTENSION );
}

void CLabLgbBaseApp::translateGuiFile( 
								ProjectMetaData *pProjMetaData,
						const INXString &translatableFullPath,
						const INXString &translatedFullPath
						)
{

	CFileOperation fo;

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Check that temp file to send to target is either not present or, if it is present, is deletable.
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	int err = fo.CheckPath(translatedFullPath);

	if( err == PATH_IS_FILE ){

		BOOL canDelete = fo.CanDelete(translatedFullPath);

		if(!canDelete){

			AfxMessageBox( "temp file for tranlation of gui files can't be deleted!" );

		}else{

			fo.Delete( translatedFullPath );

		}

	}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// now open and parse the (gui) file.
	// The approach is to parse each line into tokens separated by spaces or tabs.
	// then for each token, if it ends in 'bmp', look-up the target file name and write
	// that instead of the token.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	

	std::vector< LgbTextIconEssentialData_t > tIcons;
	std::vector< LgbPatchIconEssentialData_t > pIcons;
	std::vector< LgbImageIconEssentialData_t> iIcons;
	std::vector< LgbTextStyleEssentialData_t > vTextStyles;

	parseGuiFile( translatableFullPath, tIcons, pIcons, iIcons, vTextStyles );

	INXString targetFileName;
	ofstream targetGuiFile( translatedFullPath );

	targetGuiFile << GUI_FILE_VERSION;

	for(size_t i=0;i<tIcons.size();i++){
		targetGuiFile << tIcons[i].basics.tag << ',';
		targetGuiFile << tIcons[i].basics.type << ',';
		targetGuiFile << tIcons[i].basics.xTopLft << ',';
		targetGuiFile << tIcons[i].basics.yTopLft << ',';
		targetGuiFile << tIcons[i].basics.width << ',';
		targetGuiFile << tIcons[i].basics.height << ',';
		targetGuiFile << tIcons[i].basics.zPos << ',';

		targetGuiFile << tIcons[i].fgAlpha << ',';
		targetGuiFile << tIcons[i].fgRed << ',';
		targetGuiFile << tIcons[i].fgGreen << ',';
		targetGuiFile << tIcons[i].fgBlue << ',';

		targetGuiFile << tIcons[i].bgAlpha << ',';
		targetGuiFile << tIcons[i].bgRed << ',';
		targetGuiFile << tIcons[i].bgGreen << ',';
		targetGuiFile << tIcons[i].bgBlue  << ',';

		pProjMetaData->getTargetFileNameForBdfHostFileName(tIcons[i].csFont + ".bdf", targetFileName);
		targetGuiFile << (CString)targetFileName << ',';
		targetGuiFile << tIcons[i].leftIndent << ',';
		targetGuiFile << tIcons[i].rightIndent << ',';
		targetGuiFile << tIcons[i].topIndent << ',';
		targetGuiFile << tIcons[i].bottomIndent << ',';
		targetGuiFile << tIcons[i].lineSpacing << '\n';
	}

	for(size_t i=0;i<pIcons.size();i++){
		targetGuiFile << pIcons[i].basics.tag << ',';
		targetGuiFile << pIcons[i].basics.type << ',';
		targetGuiFile << pIcons[i].basics.xTopLft << ',';
		targetGuiFile << pIcons[i].basics.yTopLft << ',';
		targetGuiFile << pIcons[i].basics.width << ',';
		targetGuiFile << pIcons[i].basics.height << ',';
		targetGuiFile << pIcons[i].basics.zPos << ',';
	
		targetGuiFile << pIcons[i].bgAlpha << ',';
		targetGuiFile << pIcons[i].bgRed << ',';
		targetGuiFile << pIcons[i].bgGreen << ',';
		targetGuiFile << pIcons[i].bgBlue  << '\n';
	}

	for(size_t i=0;i<iIcons.size();i++){
		targetGuiFile << iIcons[i].basics.tag << ',';
		targetGuiFile << iIcons[i].basics.type << ',';
		targetGuiFile << iIcons[i].basics.xTopLft << ',';
		targetGuiFile << iIcons[i].basics.yTopLft << ',';
		targetGuiFile << iIcons[i].basics.width << ',';
		targetGuiFile << iIcons[i].basics.height << ',';
		targetGuiFile << iIcons[i].basics.zPos << ',';

		targetGuiFile << iIcons[i].alpha << ',';

		LucidErrEnum err = 
			pProjMetaData->getTargetFileNameForPngHostFileName( iIcons[i].bitmapFileName, targetFileName );

		if(err != kErr_NoErr){
			INXString dum = "Transfer Error! Program will not run in EHS!\n";
			dum += "No target file name found for host bitmap file:\n";
			dum += iIcons[i].bitmapFileName;
			dum += "\nPlease report this bug to nCapsa";
			AfxMessageBox(dum);
		}

		targetGuiFile << (CString)targetFileName << '\n';
	}

	targetGuiFile.close();
}


LucidErrEnum CLabLgbBaseApp::transferExportsToTarget( 
						ProjectMetaData *pProjMetaData,
						const int &nFilesToGo
				) 
{
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;
	INXString csReply;
	INXString projDir;
	pProjMetaData->getProjectDir(projDir);

	c_pTgtTransProgressDlog->setReset();
	c_pTgtTransErrorDlog->clearErrors();

	if(nFilesToGo == 0){

		c_pTgtTransProgressDlog->setPromptOverall( "Zero Files to be Transferred!" );

		// Give the user a chance to see the message...
		Sleep(1000);
		c_pTgtTransProgressDlog->setReset();


	}else {


		int nFilesGone = 0;

		INXString csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
		c_pTgtTransProgressDlog->setPromptOverall( csDum );
		c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );



#ifdef _DEBUG
		const int BUFFSZ = 8192;
		char buff[BUFFSZ];
		sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nconnecting...\n" );
		OutputDebugString(buff);
#endif

		// connect to tcpip client
		if (( ltsStatusType = tcpClient.Connect()) != 0){

			AfxMessageBox("Unable to establish a TCPIP connection.");

			c_pTgtTransProgressDlog->setReset();

			// Give Windows a chance to pump the 'PostMessage'
			Sleep(50);

			return kErr_NoTcpConn;

		} else {


#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nConnected OK...\n" );
			OutputDebugString(buff);
#endif

			bool bGotConnection = true;

			int nLines = 0;
			int iCount = 0;

			INXString csDelim = "> \n";
			const bool bLeftOnly = true;

			INXString csReply = "";
			std::list<INXString> guiText;

			// Get any text / errors that are alreay in the pipeline from previous transfers.
			// If there are any errors, display them, then underwwrite them with a "Starting new transfer" line.

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nRead Previous Errors...\n" );
			OutputDebugString(buff);
#endif

			while( (csReply.Right(csDelim.GetLength()) != csDelim) && 
					(iCount<1) && !c_pTgtTransProgressDlog->m_bCancelled && 
					bGotConnection )  {

				ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
				TCHAR tbuff[BUFFSZ];
				_tcscpy_s( tbuff, BUFFSZ, _T(csReply) );
				_stprintf_s( tbuff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
				OutputDebugString(buff);
#endif

				iCount++;
			}

			nLines = parseLines( csReply, guiText );
			std::list<INXString> tmpList;


			bool bIntroLineAdded = false;

			std::list<INXString>::iterator it = guiText.begin();

			while(it != guiText.end() ){

				if( (*it).Left(3) == "**E"){
					if(!bIntroLineAdded){
						tmpList.push_back( "<ERRORS FROM PREVIOUS TRANSFER - START>" );
						bIntroLineAdded = true;
					}
					tmpList.push_back( *it );
				}
				it++;

			}


			if( tmpList.size() ){

				tmpList.push_back( "<ERRORS FROM PREVIOUS TRANSFER - FINISH>" );
				tmpList.push_back( " " );
				tmpList.push_back( "<STARTING NEW TRANSFER>" );

				c_pTgtTransErrorDlog->addErrors( tmpList );
				c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
			}


			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .dat files.
			CFileFind ff;

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing dat files...\n" );
			OutputDebugString(buff);
#endif

			BOOL bRes = ff.FindFile( projDir + EXPORTDIR + "*.txt" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{

				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				INXString csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( csDum );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );
				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );
				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

				//tmpList.push_back("Hong Kong Fuey 1");
				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );

				Sleep(250);

			}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .png files.

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing PNG files...\n" );
			OutputDebugString(buff);
#endif

			iCount = 0;
			csReply = "";
			bRes = ff.FindFile( projDir + EXPORTDIR + "*.png" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{

				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				INXString csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( ff.GetFileName() );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );

				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );
				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

			//	tmpList.push_back("Hong Kong Fuey 2");
				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );

				Sleep(250);
			}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .bdf files.

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing PNG files...\n" );
			OutputDebugString(buff);
#endif

			iCount = 0;
			csReply = "";
			bRes = ff.FindFile( projDir + EXPORTDIR + "*.bdf" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{

				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				INXString csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( ff.GetFileName() );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );

				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );
				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

			
				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );

				Sleep(250);
			}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .gui files.

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing GUI files...\n" );
			OutputDebugString(buff);
#endif

			iCount = 0;
			csReply = "";
			bRes = ff.FindFile( projDir + EXPORTDIR + "*.gui" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{

				csReply = "";
				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				INXString csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( ff.GetFileName() );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );
				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && 
						(iCount<100) && 
						!c_pTgtTransProgressDlog->m_bCancelled && 
						bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					int len = csReply.GetLength();
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );

				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

		//		tmpList.push_back("Hong Kong Fuey 3");
				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );
				Sleep(250);
			}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .csv files.

#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing PNG files...\n" );
			OutputDebugString(buff);
#endif

			iCount = 0;
			csReply = "";
			bRes = ff.FindFile( projDir + EXPORTDIR + "*.csv" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{

				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				INXString csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( (INXString)ff.GetFileName() );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );

				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );
				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

			//	tmpList.push_back("Hong Kong Fuey 2");
				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );

				Sleep(250);
			}

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any resource files.
#ifdef _DEBUG
			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing resource files...\n" );
			OutputDebugString(buff);
#endif
			INXString csFileName;
			vector<ExtResourceFile> xResourceFileVec;
			pProjMetaData->getResourceFiles(xResourceFileVec);
			
			for (UINT i=0; i<xResourceFileVec.size(); i++) {
				xResourceFileVec[i].getHostFileName(csFileName);
				bRes = ff.FindFile( projDir + EXPORTDIR + csFileName);
				while (bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
				{
					int iFileSize = FileSize( projDir + EXPORTDIR + csFileName);
					INXString csDum = csFileName + "(" + intToString(iFileSize) + " bytes)";

					c_pTgtTransProgressDlog->setPromptFile( csDum );
					ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + csFileName, csFileName, c_pTgtTransProgressDlog );
					if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

					while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
						ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
						sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
						OutputDebugString(buff);
#endif

						iCount++;
					}

					nLines = parseLines( csReply, guiText );
					std::list<INXString> tmpList;

					std::list<INXString>::iterator it = guiText.begin();
					while(it != guiText.end() ){
						if( (*it).Left(3) == "**E"){
							tmpList.push_back( *it );
						}
						it++;
					}

					//tmpList.push_back("Hong Kong Fuey 1");
					if( tmpList.size() ){
						c_pTgtTransErrorDlog->addErrors( tmpList );
						c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
					}

					nFilesGone++;
					csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
					c_pTgtTransProgressDlog->setPromptOverall( csDum );
					c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );

					Sleep(250);
					bRes = false;
				}
			}	

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Find any .sdl files.
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/

#ifdef _DEBUG

			sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nProcessing sdl file...\n" );
			OutputDebugString(buff);
#endif

			iCount = 0;
			csReply = "";
			bRes = ff.FindFile( projDir + EXPORTDIR + "*.sdl" );

			while ( bRes && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{
				csReply = "";
				bRes = ff.FindNextFile();

				int iFileSize = FileSize( projDir + EXPORTDIR + (INXString)ff.GetFileName() );
				csDum = (INXString)ff.GetFileName() + "(" + intToString(iFileSize) + " bytes)";

				c_pTgtTransProgressDlog->setPromptFile( ff.GetFileName() );
				ltsStatusType = tcpClient.SendFile(projDir + EXPORTDIR + (INXString)ff.GetFileName(), (INXString)ff.GetFileName(), c_pTgtTransProgressDlog );

				if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

				ltsStatusType = LTS_STATUS_TEMPORARY;

				while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
					ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
					int len = csReply.GetLength();
					sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
					OutputDebugString(buff);
#endif

					iCount++;
				}

				nLines = parseLines( csReply, guiText );

				std::list<INXString> tmpList;

				std::list<INXString>::iterator it = guiText.begin();
				while(it != guiText.end() ){
					if( (*it).Left(3) == "**E"){
						tmpList.push_back( *it );
					}
					it++;
				}

				if( tmpList.size() ){
					c_pTgtTransErrorDlog->addErrors( tmpList );
					c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
				}

				nFilesGone++;
				INXString csDum = intToString(nFilesGone) + "/" + intToString(nFilesToGo) + " Files Transferred";
				c_pTgtTransProgressDlog->setPromptOverall( csDum );
				c_pTgtTransProgressDlog->setProgbarOverall( 100 * nFilesGone / nFilesToGo );
				Sleep(150);
			}
			/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/

			iCount = 0;
			csReply = "";

			if( !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )
			{
				// Start app on ehs

				/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
				if (( tcpClient.SendText("R\n")) != 0)
				{
					AfxMessageBox("Unable to send Run command over TCP/IP connection.");
					tcpClient.DisConnect();

				} else {

#ifdef _DEBUG
					sprintf_s( buff, BUFFSZ, "%s", "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nSent R (for Run)...\n" );
					OutputDebugString(buff);
#endif

					if(ltsStatusType == LTS_STATUS_FAIL) bGotConnection = false;

					while( (csReply.Right(csDelim.GetLength()) != csDelim) && (iCount<100) && !c_pTgtTransProgressDlog->m_bCancelled && bGotConnection )  {
							ltsStatusType = tcpClient.ReceiveTextBlock( csReply, csDelim, bLeftOnly ); 

#ifdef _DEBUG
						sprintf_s( buff, BUFFSZ, "iCount = %d: csReply=:\n{%s}\n", iCount, csReply.Left(BUFFSZ-30));
						OutputDebugString(buff);
#endif
							iCount++;
					}

					nLines = parseLines( csReply, guiText );

					std::list<INXString> tmpList;
					std::list<INXString>::iterator it = guiText.begin();

					while(it != guiText.end() ){
						if( (*it).Left(3) == "**E"){
							tmpList.push_back( *it );
						}
						it++;
					}

					if( tmpList.size() ){
						c_pTgtTransErrorDlog->addErrors( tmpList );
						c_pTgtTransErrorDlog->ShowWindow( SW_NORMAL );
					}
				}

				/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/

			} // if( !c_pTgtTransProgDlog->m_bCancelled )

			tcpClient.DisConnect();

			c_pTgtTransProgressDlog->setProgbarOverall(0);
			c_pTgtTransProgressDlog->setPromptFile("");

		}

	} // if(nFilesToGo != 0)

	c_pTgtTransProgressDlog->setReset();

	return kErr_NoErr;

}

int CLabLgbBaseApp::getOpenDocCount(void)
{

	int retVal = 0;
	POSITION templatePos = GetFirstDocTemplatePosition();
	CDocTemplate *pTemplate = GetNextDocTemplate( templatePos );

	POSITION docPos = pTemplate->GetFirstDocPosition();

	while (docPos){
		pTemplate->GetNextDoc(docPos);
		retVal++; 
	}

	return retVal;
}



void CLabLgbBaseApp::writeSodl(void)
{
	// LAB's app provides an action for this, LGB doesn't / can't
}

void CLabLgbBaseApp::setProjMetaData(  ProjectMetaData *pVal ) 
{
	m_pProjMetaData = pVal;
}

void CLabLgbBaseApp::setProject(  Project *pProject ) 
{
	m_pProject = pProject;
}

BOOL CLabLgbBaseApp::InitInstance()
{
	return CWinApp::InitInstance();
}

void CLabLgbBaseApp::showTransferDialog( const bool & bVisible )
{
	if (bVisible)
		c_pTgtTransProgressDlog->ShowWindow( SW_NORMAL );
	else
		c_pTgtTransProgressDlog->ShowWindow( SW_HIDE );

}

// Method that loads a vector with a pair of tool id and executable path from the file, tools.ini
void CLabLgbBaseApp::LoadToolVec()
{
	INXString csInstallDir, csToolsFilePath, csToolExecPath;
	vector<INXString> vToolKeysVec;
	pair<INXString, INXString> prToolPair;

	GetInstallationBaseDir(csInstallDir);
	csToolsFilePath = csInstallDir + LUCID_EXECUTABLES_DIR + "\\" + TOOLSFILE;
	GetIniKeys("tools", csToolsFilePath, vToolKeysVec);

	for (size_t i=0; i<vToolKeysVec.size(); i++) {
		csToolExecPath = GetIniValue("tools", vToolKeysVec[i], csToolsFilePath);
		prToolPair.first = vToolKeysVec[i];
		prToolPair.second = csToolExecPath;
		m_vToolsPairVec.push_back(prToolPair);
	}
}

INXString CLabLgbBaseApp::GetToolExecPath(const INXString csTool)
{
	INXString csToolExecPath = "";

	for (size_t i=0; i<m_vToolsPairVec.size(); i++) {
		if (m_vToolsPairVec[i].first == csTool) {
			csToolExecPath = m_vToolsPairVec[i].second;
		}
	}

	return csToolExecPath;
}


bool CLabLgbBaseApp::isConnectionToEHS(void) {
	LucidTcpipClient tcpClient;
	LtsStatusType ltsStatusType;
	bool isEHSRunning = false;

	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) == 0) {
		// test connection by retrieving state
		if (( tcpClient.SendText("?S\n")) == 0)	{
			isEHSRunning = true;
		}
		// tidy up, close the port at this end
		tcpClient.DisConnect();
	}
	return isEHSRunning;
}

/*
 * Two modes of operation:
 * 1. Local Mode - if no tcpip connection will start a local version of EHS, if the local path is known else will report error
 * 2. Remote Mode - if no tcpip connection will report error as cannot start EHS remotely
 *
 * In both cases 1 and 2 will send command to restart EHS if there is a tcpip connection 
 * but not if about to send SODL (which restarts EHS anyway)
 *
 * Returns true if EHS started successfully or already running
 */
bool CLabLgbBaseApp::startEHS(bool aboutToSendAllSODL){
	bool success = false;
	bool ehsRunning = isConnectionToEHS();

	if (!ehsRunning) {
//		AfxMessageBox("EHS Not Runing"); 
		// only try and start EHS if locally hosted and know its local path
		if ((csCurrentTarget == REG_TARGET_LOCATION_DATA_LOCAL) && isEHSLocalInstallPathKnown) {			
//			HINSTANCE h = ShellExecute(NULL, "open", "/bin/bash ./workspace/INX/TARGET_TREES/ehs_env-linux_x86_gtk/bin/run_ehs.sh NO_RESTART", NULL, NULL, SW_SHOW);
//			HINSTANCE h = ShellExecute(NULL, "open", "/bin/bash \"./.wine/drive_c/Program Files/inx/tools/EHS/bin/run_ehs.sh\" NO_RESTART", NULL, NULL, SW_SHOW);
// alt. can escape spaces instead of quoting whole path...	HINSTANCE h = ShellExecute(NULL, "open", "/bin/bash ./.wine/drive_c/Program\ Files/inx/tools/EHS/bin/run_ehs.sh NO_RESTART", NULL, NULL, SW_SHOW);
			
			// use base dir to get install path 
			INXString baseDir;
			GetInstallationBaseDir(baseDir);	//base dir is something like C:/Program Files/inx/tools (whereever the iab exe is being run from)
//			AfxMessageBox("base dir:" + baseDir); 


			// if we are about to send SODL after start EHS then delete any old SODL first (don't care whether it exists or not try anyway)
			if (aboutToSendAllSODL) {
				INXString csSODLPath = baseDir;
				if (isInstalledInWindows) {
					csSODLPath = csSODLPath + EHS_WIN_VAR_DIR + EHS_APPDATA_DEFAULT_DIR + EHS_SODL_FILE;
				} else {
					struct stat xFileInfo2;
					if (stat(EHS_LINUX_OPT_INSTALL_DIR,&xFileInfo2) == 0) { /* is this the opt version */
/* do these mixed linux path specifiers work OK?*/
						csSODLPath = EHS_LINUX_OPT_INSTALL_DIR "/" EHS_APPDATA_DEFAULT_DIR EHS_SODL_FILE;
					}else { /* this is a tools path installed version of EHS */
						csSODLPath = csSODLPath + EHS_LINUX_VAR_DIR + EHS_APPDATA_DEFAULT_DIR + EHS_SODL_FILE;
					}
				}
				remove(csSODLPath);
			}
			/* construct path to run EHS from */

			// are we in linux or windows, construct path to start EHS accordingly
			HINSTANCE h;
			INXString csEHSExePath = "";
			INXString csWorkingPath = "";
			if (isInstalledInWindows) {
				// call to EHS.exe
				csEHSExePath = baseDir + EHS_WIN_VAR_DIR + EHS_BIN_DIR + "\\ehs.exe";
				//csEHSExePath = baseDir + EHS_WIN_VAR_DIR + EHS_BIN_DIR + "\\run_ehs.bat";
				csWorkingPath = baseDir + EHS_WIN_VAR_DIR + EHS_BIN_DIR;
				h = ShellExecute(NULL, "open", csEHSExePath, NULL, csWorkingPath, SW_HIDE);
			} else {

				// construct path to run_ehs.sh which we will pass to bash
				// it is relative to the current working directory, baseDir, which is .../inx/tools/
/*
 * note - old method was to try call to bash in usr directory but can't get shell execute to expand $HOME
 * this call works from cmd line:	/bin/bash $HOME"/.wine/drive_c/Program Files/inx/tools/EHS/ehs_env-linux_x86_gtk/bin/run_ehs.sh" NO_RESTART
 * but doesn't work in shell execute
 */
				struct stat buf;
				if (stat(EHS_LINUX_OPT_INSTALL_DIR "/" EHS_LINUX_OPT_RUNTARGET,&buf)==0) {/* try the /opt/ehs version first - normal installer!!! */
					h = ShellExecute(NULL, "open", EHS_LINUX_OPT_INSTALL_DIR "/" EHS_LINUX_OPT_RUNTARGET " NO_RESTART LIB_HOST", NULL, NULL,SW_SHOW);
				}
				else { /* this is for the installer versiom - this isn't normally used! */
					// construct the last part of the path
					char cPath[MAX_PATH] = EHS_LINUX_VAR_DIR;
					strcat_s(cPath, EHS_BIN_DIR);
					strcat_s(cPath, "\\run_ehs.sh");
					// replace all backslashes with forward slashes used by unix
					for (int i = 0; i < strlen(cPath); i++) {
						if (cPath[i] == '\\')
							cPath[i] = '/';
					}

					// full path we have just built:	csEHSExePath = "/bin/bash \"./EHS/ehs_env-linux_x86_gtk/bin/run_ehs.sh\" NO_RESTART";
					csEHSExePath = "/bin/bash \".";
					csEHSExePath = csEHSExePath + cPath + "\" NO_RESTART LIB_HOST";
					h = ShellExecute(NULL, "open", csEHSExePath, NULL, NULL,
							SW_SHOW);// SW_HIDE or SW_SHOW, neither show a dos box before EHS starts
					//				AfxMessageBox("path:" + csEHSExePath);
				}
			}

			if ((UINT)h <= 32) {
				AfxMessageBox("Failed to start EHS,\nerror code: " + (UINT)h);
			} else {
				//@todo what we need is a new variable to send the bash script that starts EHS telling it to not load old SODL when we are starting EHS locally from IAB/LGB

				// 10 tries to connect then give up
				for (UINT i=1; i < 5; i++) {
					Sleep(500);
					success = isConnectionToEHS();

					if (success) break;
				}
				if (!success) {
					AfxMessageBox("Failed to start EHS, unknown error");
				}
			}
		} else if (csCurrentTarget == REG_TARGET_LOCATION_DATA_LOCAL) {
			AfxMessageBox("EHS not installed in a known location and unable to connect to local EHS over TCPIP connection,\nEHS cannot be started or restarted.");
		} else {
			AfxMessageBox("Unable to connect to remote EHS over TCPIP connection,\nEHS cannot be started or restarted.");
		}
	} else {
		// EHS is already running
		success = true;
		// if we are not about to send SODL (not a full or partial transfer) then we need to send an explicit restart to EHS to reload existing SODL
		if (!aboutToSendAllSODL) {
			LucidTcpipClient tcpClient;
			LtsStatusType ltsStatusType;
			// connect to tcpip client
			if (( ltsStatusType = tcpClient.Connect()) == 0) {
				// - reload EHS
				if (( tcpClient.SendText("F\nR\n")) != 0)	{
					AfxMessageBox("Could not send command to restart.\nUnable to send command over TCPIP connection.");
				}
				// tidy up, close the port at this end
				tcpClient.DisConnect();
			}
		}
	}

	/* wine example of how to run a bash script directly from wine */
//	HINSTANCE h = ShellExecute(NULL, "open", "/bin/bash /home/bob/workspace/INX/TARGET_TREES/ehs_env-linux_x86_gtk/bin/run_ehs.sh NO_RESTART", NULL, NULL, SW_SHOW);

	/* wine example of how to run an exe */
//	HINSTANCE h = ShellExecute(NULL, "open", "ehs.exe", NULL, NULL, SW_SHOW);

	/* wine example of how to use winconsole cmd to start a console window (visible to user), execute a command and then close the console window */
//	HINSTANCE h = ShellExecute(NULL, "open", "wineconsole cmd /c ./workspace/INX/TARGET_TREES/ehs_env-linux_x86_gtk/bin/run_ehs.sh NO_RESTART", NULL, NULL, SW_SHOW);

	return success;
}

void CLabLgbBaseApp::openEHSInitParamsDialog(){
	//@todo - profiles need to be supported to show init params for different target types

	EHSInitParams * pParams = pEHSInitParams;
	EHSParamsDialog paramDlg(pParams);

	
	if (paramDlg.DoModal() == IDOK) {
		pParams->nXCoord = paramDlg.nXCoord;		
		pParams->nYCoord = paramDlg.nYCoord;
		pParams->nWidth = paramDlg.nWidth;
		pParams->nHeight = paramDlg.nHeight;
		pParams->nColourRed = paramDlg.nRed;
		pParams->nColourGreen = paramDlg.nGreen;
		pParams->nColourBlue = paramDlg.nBlue;
		pParams->nAlpha = paramDlg.nAlpha;
		pParams->nHasFrame = paramDlg.nHasFrame;
		pParams->nZOrder = paramDlg.nZOrder;

		INXString baseDir;
		GetInstallationBaseDir(baseDir);	//base dir is where the inx exe is being run from

		INXString csEHSParamFilePath = baseDir;
		if (isInstalledInWindows) {
			csEHSParamFilePath += EHS_WIN_VAR_DIR;
		} else {
			csEHSParamFilePath += EHS_LINUX_VAR_DIR;
		}
		csEHSParamFilePath += EHS_SYSDATA_DIR;
		csEHSParamFilePath += EHS_GRAPHICS_INFO_FILE;

		FILE* pFile;
		errno_t error;

		error=fopen_s(&pFile,csEHSParamFilePath,"w");
		if (error!=0) {             
	        AfxMessageBox("Unable to create file " + csEHSParamFilePath);
		} else {
			fprintf(pFile,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			fprintf(pFile,"<graphics>\n");
			fprintf(pFile,"<initialPosition>\n");
			fprintf(pFile,"  <xcoord>%d</xcoord>\n",pParams->nXCoord);
			fprintf(pFile,"  <ycoord>%d</ycoord>\n",pParams->nYCoord);
			fprintf(pFile,"  <width>%d</width>\n",pParams->nWidth);
			fprintf(pFile,"  <height>%d</height>\n",pParams->nHeight);
			fprintf(pFile,"</initialPosition>\n");
			fprintf(pFile,"<colour>\n");
			fprintf(pFile,"  <red>%d</red>\n",pParams->nColourRed);
			fprintf(pFile,"  <green>%d</green>\n",pParams->nColourGreen);
			fprintf(pFile,"  <blue>%d</blue>\n",pParams->nColourBlue);
			fprintf(pFile,"  <alpha>%d</alpha>\n",pParams->nAlpha);
			fprintf(pFile,"</colour>\n");
			fprintf(pFile,"<hasFrame>%d</hasFrame>\n",pParams->nHasFrame);
			fprintf(pFile,"<zorder>%d</zorder>\n",pParams->nZOrder);
			fprintf(pFile,"</graphics>\n");
			fclose(pFile);
		}

	}
}
