/** @file ProjectMetaData.h
 * A Proxy for representing / managing /reading and writing the contents of the project file.
 * 
 * The project meta data lists such details as the project name and folder, lists the GUI, data
 * and bitmap files, the 'screen tags', etc etc.
 *
 * @author: Matthew Rutherford, Sam Gardner
 * @version: $Revision$
 * @date: $Date$
 * 
 * Copyright (c) Ncapsa Ltd, 2007. All rights reserved.
 */
#ifndef _PMD_H
#define _PMD_H

#pragma once
#include <wx/window.h>
// Prefixes for the target filenames of Gui files.
#define GUI_TARGETFILE_PREFIX  "g" 
//#define BMP_TARGETFILE_PREFIX  "b"
//#define DATA_TARGETFILE_PREFIX "d"

#define GUI_TARGETFILE_SUFFIX ".gui"

#define PROJMETADATA_INITIAL_FILEVEC_SIZE 30
#define UPDATE_SODL_FILE_NAME "\\UpdateSODL.nfo" 
#define UPDATE_WIDGETS_FILE_NAME "\\UpdateWidgets.nfo" 


#include <vector> // stl class
#include <set> // stl class
#include <map>	// stl class

#include "LucidEnums.h" // error codes

#include "ExtGuiFile.h"  // GUI (as processed by LGB) file
#include "ExtDataFile.h" // miscellaneous file
#include "ExtBmpFile.h" // GUI's bitmap file
#include "ExtPngFile.h" // GUI's bitmap file
#include "ExtBdfFile.h" // BDF Font file
#include "ExtNlsFile.h" // nls.csv
#include "ExtResourceFile.h" //@todo

#include "GuiWidget.h" // reprsentation of GUI-widget meta data

#include "LucidTypedefs.h" //@todo // definitions useful for brevity
#include "../Group.h"
#include "LccTimeStamp.h" //@todo
#include "LccPmdXfers.h" //@todo
//#include "afxmt.h" //@todo
#include "ProjFolderMinder.h"//@todo
//#include "Markup.h" //@todo
#include "Porting_Classes/INXString.h"

using namespace std;

class ProjectMetaData
{
public:
	ProjectMetaData(void);////
	ProjectMetaData(INXString projectDir);////
public:
	~ProjectMetaData(void);////

	struct TransferData_t {
		INXString csHostFileStubName;
		INXString csHostFileFullName;
		INXString csHostFileFolder;
		INXString csTargetFileName;

		LccPmdXfers::TypesEnum type;
	};

	LucidErrEnum updateScreenTag(
						const INXString					&tag, 
						const TagProjMetaSupportData_t	&suppData );////

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bool checkProjectFileData( void );

	int inflateTransferrableData( 
							std::vector< TransferData_t > &vTransferrablesData, 
							  const bool &updatedOnly, const bool &bAppUpload);

	bool isUpToDate();
	void updateTransferTime();
#endif
	unsigned long  nBuildNo; //  @todo this is on the wrong place! this is incremented for each build of SODL to provide a UID. This should be private and the update function should be in this class.


#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
private:	
	LccTimeStamp getLastTransferTime();
	void getListOfPreviousTransferredItems(LccPmdXfers &vAlreadyTransItems);


	LccTimeStamp getLastFileReadTime();
	void updateFileReadTime();
#endif

// Attributes
private:

	// CMutex *m_pMutex;

//	HANDLE m_hLockFileHandle;

	INXString m_csProjectDir;		// The folder in which the project file sits
	INXString m_csProjectName;	// the name of the project itself - always the same as the folder it sits in.
	

	/* this is too specific and complex - Needs to be made generic - and it doesn't work properly!*/
	vector<ExtGuiFile> m_vGuiFiles;		// Container for in-project guiFile metadata
	vector<ExtBmpFile> m_vBmpFiles;		// Container for in-project bitmap File metadata
	vector<ExtPngFile> m_vPngFiles;		// Container for in-project png File metadata
	vector<ExtDataFile> m_vDataFiles;	// Container for in-project text file metadata
	vector<ExtResourceFile> m_vResourceFiles;	// Container for in-project resource file metadata
	vector<ExtBdfFile> m_vBdfFiles;	// Container for in-project bdf font file metadata
	vector<ExtNlsFile> m_vNlsFiles; // Container for nls.csv

	LccPmdXfers m_cTransferables;

	LccTimeStamp m_cLastTransferTime;
	LccTimeStamp m_cLastFileReadTime;

 // screentag data as stored in project file, ie tag, descrip and target filename
	ScreenProjMetaMap_t m_mapScreenTagProjMetas;

	vector<Group> m_vGroups;

 // Records the essential data about which widgets use which sceen tags.
	vector<GuiWidget> m_vGuiWidgets;


// These are used in the counting-up of target file names as they are required.
// They record which integers have been used so far in creating 'unique' target file names.
// Note that a gui file doesn't have its own target file name.  Instead, it has a screen tag,
// and the screen tag has its own target file name.
	int m_iLastGuiFileAddedKey;
	int m_iLastBmpFileAddedKey;
	int m_iLastPngFileAddedKey;
	int m_iLastDataFileAddedKey;
	int m_iLastBdfFileAddedKey;

	bool isDirtySODL; // used to hold current state of SODL, true if DEP has been updated but SODL not written
	bool isSODLCheckBeingPerformed; // set to false if user has chosen to ignore dirty SODL warning

// Methods
public:
// Zero-out project data and 'offically' set project to 'not set'

	void setProjectToNull(); ////

	bool getRealLock(char* szFile, int iLineNum);////
	bool releaseRealLock(char* szFile, int iLineNum);////
	bool isLocked();////

	// Use these overloads when the project name has changed - eg when doing a 'save project as'
	//bool getLock(const INXString &lockFile );

	//bool releaseLock(const INXString &lockFile );
	LucidErrEnum readProjectFile(INXString csProjectPathName);
	LucidErrEnum readProjectFile();
	LucidErrEnum readProjectDescriptionFile();
	LucidErrEnum writeProjectFileAs(INXString csProjectPathName);
// Loads the 
	LucidErrEnum getProjectName(INXString &csProjName) const;////
	//LucidErrEnum getProjectRootDir(INXString &csProjRootDir) const;
	LucidErrEnum getProjectDir(INXString &csProjDir) const; ////
	LucidErrEnum getFullPathProjectFile(INXString &csProjDir) const;////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidErrEnum writeProjectFile();
	
	LucidErrEnum writeProjectDescriptionFile();
#endif
	LucidErrEnum showNewProjectDialog();////
	LucidErrEnum showOpenProjectDialog(wxWindow *parent);////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidErrEnum showSaveProjectAsDialog();
	LucidErrEnum showAddFileDialog(ExtDataFile &file, bool &bExists);
	LucidErrEnum showAddResourceDialog(ExtResourceFile &file, bool &bExists);

	LucidErrEnum getGroupVec(vector<Group> &groupVec);
#endif
// Returns true if given file is in the project, false otherwise
	bool fileInProject(const ExtDataFile &file) const;////
	bool fileInProject(const ExtGuiFile &file)  const;////
	bool fileInProject(const ExtBmpFile &file)  const;
	bool fileInProject(const ExtPngFile &file)  const;////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bool fileInProject(const ExtBdfFile &file)  const;
	bool fileInProject(const ExtNlsFile &file)  const;
	bool fileInProject(const ExtResourceFile &file) const;

// Returns true if project folder structure is OK, false otherwise
	LucidErrEnum folderStructureNotOk(); //INXString csProjectDir
#endif

	void setDirtySODLFlag();////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void clearDirtySODLFlag();
	bool checkDirtySODL();
	void setDirtyWidgetsFlag();
	void clearDirtyWidgetsFlag();
	bool checkDirtyWidgets();
	bool checkNewerSODLAvailable();
	void setIsSODLCheckBeingPerformed(bool val);
#endif
	int readBuildNumFromFile(void);////
	void populateBuildNumFromFile(void);////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	int getBuildNum(void);


// -------------------------------------------
// External data files
// -------------------------------------------

// Gui Files.

// add a single gui file: returns error if file already present
	LucidErrEnum addGuiFile( const GuiFileEssentialData_t &fileData, const bool &addFileSystemItem );

// remove a single gui file: returns error if file already present
	LucidErrEnum removeGuiFile(	const ExtGuiFile &file );

// remove all gui files associated with a screen
	LucidErrEnum removeGuiFiles(const INXString &screenTag);
#endif
// Inflate a vector of full gui file data, eg filename, file descrip, tag, tag descrip that match 'screenTag'
	LucidErrEnum getGuiFiles( vector<GuiFileMonty_t> &guiFileVec);////
	LucidErrEnum getGuiFiles(vector<ExtGuiFile> &guiFileVec) const;
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// 
	LucidErrEnum getGuiFiles(const INXString &screenTag, vector<GuiFileMonty_t> &guiFileVec);

// get a simple copy of vector of external files
	LucidErrEnum getGuiFiles(vector<ExtGuiFile> &guiFileVec) const;
	LucidErrEnum getBmpFiles(vector<ExtBmpFile> &bmpFileVec) const;
#endif
	LucidErrEnum getPngFiles(vector<ExtPngFile> &pngFileVec) const;////
	LucidErrEnum getDataFiles(vector<ExtDataFile> &dataFileVec) const;////
	LucidErrEnum getBdfFiles(vector<ExtBdfFile> &bdfFileVec) const;
	LucidErrEnum getNlsFiles(vector<ExtNlsFile> &nlsFileVec) const;////
	LucidErrEnum getResourceFiles(vector<ExtResourceFile> &xResourceFileVec) const;////

// PNG files.
	LucidErrEnum addPngFile( const ExtPngFile &file );////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidErrEnum removePngFile(	const ExtPngFile &file );

// Bmp files.
	LucidErrEnum addBmpFile( const ExtBmpFile &file );
	LucidErrEnum removeBmpFile(	const ExtBmpFile &file );

// Data files.
	LucidErrEnum addDataFile( const ExtDataFile &file );
	LucidErrEnum removeDataFile( const ExtDataFile &file );

// BDF files.
	LucidErrEnum addBdfFile( const ExtBdfFile &file );
	LucidErrEnum removeBdfFile( const ExtBdfFile &file );
	LucidErrEnum RemoveAllBdfFiles();

// Resource files.
	LucidErrEnum addResourceFile( const ExtResourceFile &file );
	LucidErrEnum removeResourceFile( const ExtResourceFile &file );

//--------------------------------------------
// Screen tags.

	LucidErrEnum getTargetFileNameForDataHostFileName(
				const INXString &hostFileName, INXString &targetFileName ) const;

	LucidErrEnum getTargetFileNameForGuiHostFileName(
				const INXString &hostFileName, INXString &targetFileName ) const;

	LucidErrEnum getTargetFileNameForPngHostFileName(
				const INXString &fileName, INXString &targetFileName ) const;

	LucidErrEnum getTargetFileNameForBmpHostFileName(
				const INXString &bmpFileName, INXString &targetFileName ) const;

	LucidErrEnum getTargetFileNameForBdfHostFileName(
				const INXString &bdfFileName, INXString &targetFileName ) const;

	LucidErrEnum getTargetFileNameForNlsHostFileName(
				const INXString &nlsFileName, INXString &targetFileName ) const;
#endif
	LucidErrEnum getScreenTagMetas(
								const INXString &screenTag,
								TagProjMetaSupportData_t &tagData) const;////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidErrEnum editScreenTagDescription(
		const INXString &tag,
		const INXString &tagDescrip);


// Adds a single screen tag.  generates target file name for tag and loads it into
// screenTag.  Returns error if tag is already present.
	LucidErrEnum createNewScreenTag( 
		const INXString &tag, const INXString &description );

#endif
// Adds a single screen tag.  DOESN'T generate target file name for tag.
// Assumes that the screen-tag has been read-in from a project file, so
// DOESN'T create a new target-filename for the screen tag.
// Returns error if tag is already present.
	LucidErrEnum addScreenTag( 
		const INXString &tag, const TagProjMetaSupportData_t &supportData );////

#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Sets the name of the 'active' host layout file for a screen tag.
// Returns error if tag is already present, or if layout doesn't belong to the screenTag.
	LucidErrEnum setActiveLayout(
		const INXString &widgetGroup, const INXString &layoutHostName );

#endif
// Gets the name of the 'active' host layout file for a screen tag.
// Returns error if tag is already present, or if layout doesn't belong to the screenTag.
	LucidErrEnum getActiveLayout(
		const INXString &widgetGroup, INXString &layoutHostName );////

// Gets the name of the 'active' host layout file for a screen tag.
// Returns error if tag is already present, or if layout doesn't belong to the screenTag.
	LucidErrEnum getActiveLayouts(
		std::set<INXString> &vLayoutNames );////


	LucidErrEnum removeScreenTag(const INXString &tag );////

// Get a list of simple tags.
	LucidErrEnum getScreenTags( vector<INXString> &tags );////
	LucidErrEnum addGuiWidget(const GuiWidget &widget);////
	LucidErrEnum guiWidgetInVec(const GuiWidget &widget);////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// Gets a list of screntags, descriptions and the number of GuiWidgets the tag
// is used on.
	LucidErrEnum getScreenTagMgrData(map<INXString, ScreenMgrSupportData_t> &tagsAndDescrs);

	//LucidErrEnum getScreenTags(vector<int> &tagsAndDescrs);

//--------------------------------------------
// Groups.

	LucidErrEnum addGroup(Group &group);
	LucidErrEnum removeAllGroups();

	
//--------------------------------------------
// Widget tags.

	LucidErrEnum addGuiWidget(const GuiWidget &widget);
	LucidErrEnum removeGuiWidget(const GuiWidget &widget);
	LucidErrEnum getGuiWidgets(vector<GuiWidget> &vWidgets);
	LucidErrEnum updateWidget(const GuiWidget &widgetToBeUpdated, const GuiWidget &copyableWidget);
	LucidErrEnum getScreenTagDescription(
							const INXString &tag, INXString &tagDescr );
	LucidErrEnum getGuiWidgetVecSize(UINT &size);


protected:

	void setProjectName(INXString csProjName);
	void createDefaultGroup();
	void createFolderStructure();
	void copyDummyBmp();
	void copyFolderTo(INXString csNewProjRootDir, INXString csNewProjName);

// Get the description of a given tag.  Return error if tag not present.

public:
	void setProjectDir(INXString csProjDir);

// Return how many GuiWidgets a given tag i used on.
	int getScreenTagUsageInWidgets(const INXString & screenTag);

	int writeProjectFileForFirstTime(void);
	//void initialise(void);
#endif
public:
	bool projectIsSet(void);////


protected:

// if true, indiactes tht the instance does actually point to a project
	bool m_bProjectIsSet;
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
public:
	void testComponents(void);
	void testAddScreenTag(void);
#endif
protected:
	INXString m_csLucidVersion;

public:
	INXString m_csProjectAppCanonicalName;
	INXString m_csProjectAppCommercialName;
	INXString m_csProjectAppVersion;
	INXString m_csProjectAppDescription;

public:
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	bool screenTagInProject(const INXString & tag);

	LucidErrEnum getDataFileByName(const INXString & hostFileName, ExtDataFile &eGF ) const;
	LucidErrEnum getResourceFileByName(const INXString & hostFileName, ExtResourceFile &eGF ) const;
#endif
	LucidErrEnum getGuiFileByName(const INXString & hostFileName, ExtGuiFile &eGF ) const;////
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidErrEnum getPngFileByName(const INXString & hostFileName, ExtPngFile &eBF ) const;
	LucidErrEnum getBmpFileByName(const INXString & hostFileName, ExtBmpFile &eBF ) const;
	LucidErrEnum getBdfFileByName(const INXString & hostFileName, ExtBdfFile &eBF ) const;
	LucidErrEnum getNlsFileByName(const INXString & hostFileName, ExtNlsFile &eNF ) const;

	// Copy 'dummy.bmp' from LAB installation dir\BMP to projectDir\DeviceData\GUI 
	LucidErrEnum initialiseProjectLgbBitmaps(void);
#endif
	void initProjFolderMinder();////
	void updateTransferrables(const bool &bAppUpload);////

private:
	bool m_bLocked;
	ProjFolderMinder m_cProjFolderMinder;

	const int m_iCodeMajorRev;
	const int m_iCodeMinorRev;

	int m_iProjReadMajorRev;
	int m_iProjReadMinorRev;

	void getVersionInts(void);////
	INXString getVersionString(void);////

};
#endif
