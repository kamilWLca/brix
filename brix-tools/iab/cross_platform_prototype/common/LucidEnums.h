// LucidEnums.h: global enums
//
//////////////////////////////////////////////////////////////////////

#pragma once


enum LucidLGBScreenTagProcessOptions {
	kAdd,
	kRemove,
	kEditDescrip
};

enum LucidErrEnum {

	kErr_NoErr, 
	kErr_NoneSelected,

	kErr_InvalidFolderStructure, 
	kErr_InvalidProjectDir,
	kErr_DialogNotOk, 
	kErr_ProjectFileNotWritten, 
	kErr_ProjectFileNotRead,
	kErr_UndefinedFileType,
	kErr_FileInVec,

	kErr_FileNameLengthIsZero,
	kErr_NoComboBoxItemSelected,

	kErr_ProjectMetaData_GuiFileAlreadyPresent,
	kErr_ProjectMetaData_GuiFileNotAlreadyPresent,

	kErr_ProjectMetaData_PngFileAlreadyPresent,
	kErr_ProjectMetaData_PngFileNotAlreadyPresent,

	kErr_ProjectMetaData_BmpFileAlreadyPresent,
	kErr_ProjectMetaData_BmpFileNotAlreadyPresent,

	kErr_ProjectMetaData_BdfFileAlreadyPresent,
	kErr_ProjectMetaData_BdfFileNotAlreadyPresent,

	kErr_ProjectMetaData_ScreenTagAlreadyPresent,
	kErr_ProjectMetaData_ScreenTagNotAlreadyPresent,
	kErr_ProjectMetaData_ScreenTagGuiTagPairAlreadyPresent,
	kErr_ProjectMetaData_WidgetScreenPairAlreadyPresent,
	kErr_ProjectMetaData_WidgetIsNotPresent,
	kErr_NoWidgetTag,
	kErr_NoScreenTag,
	kErr_ManualTypeUnknown,
	kErr_HelperAppUnknown,
	kErr_ProjectMetaDataNoProjectSet,

	kErr_LgbDrawableAlreadyInList,

	kErr_TransferrableNotFound,
	kErr_TransferrableAlreadyExists,

	kErr_CouldNotCreateFolder,

	kErr_DidNotGetLock,

	kErr_NoTcpConn
	
};

enum LucidManualsEnum {
	kLgbRefManual,
	kLabUserGuide,
	kLabRefManual,
	kLgbLabTutorial
};


enum LucidHelperAppEnum {
	kAppAcrobatReader
};

enum ExtFileTypeEnum {kGui, kBmp, kData, kBdf};

enum LtsStatusType {
	LTS_STATUS_OK = 0,
	LTS_STATUS_FAIL = 1,
	LTS_STATUS_TEMPORARY = 2,
	LTS_STATUS_PERMANENT = 4,
	LTS_STATUS_TIMEOUT = 8,
	LTS_FILE_OPEN_FAILED = 16,
	LTS_FILE_CREATE_FAILED = 32
};

enum LgbRectSelTypeEnum {

	k_LgbDrawableOutside,
	k_LgbDrawableInterior,

	k_LgbDrawableLft,
	k_LgbDrawableRgt,
	k_LgbDrawableTop,
	k_LgbDrawableBtm,

	k_LgbDrawableTopLft,
	k_LgbDrawableTopRgt,
	k_LgbDrawableLwrLft,
	k_LgbDrawableLwrRgt,


	k_LgbDrawableError
};

enum LgbClassIDEnum {
	k_LgbDrawable,
	k_LgbResizedDrawable,
	k_LgbImageIcon,
	k_LgbTextIcon,
	k_LgbPatchIcon
};

enum LgbIconTypeEnum {
	k_LgbAllIconTypes,
	k_LgbBitmapOnly,
	k_LgbTextBoxOnly,
	k_LgbPatchOnly
};

enum LgbRectIntsecnEnum {
	k_LgbCompletelyWithin,
	k_LgbCompletelyWithout,
	k_LgbPartlyWithin
};

enum LucidFolderDescripEnum {
	k_
};

enum LabLineSegmentTypeEnum {
	k_LabNoLine,
	k_LabDiagonalLine,
	k_LabVerticalLine,
	k_LabHorizontalLine
};