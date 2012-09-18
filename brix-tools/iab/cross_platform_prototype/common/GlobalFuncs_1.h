#pragma once
#include "stdafx.h"

#include "../common/LgbIconDataStrucs.h"
#include "Porting_Classes/INXString.h"

extern bool cleanupGuiLayoutBasics( LgbIconEssentialData_t &rEssDat, INXString &csWarnings  );

extern void parseGuiFile(
				const INXString &csFileName, 
				std::vector< LgbTextIconEssentialData_t > &textIcons, 
				std::vector< LgbPatchIconEssentialData_t > &patchIcons, 
				std::vector< LgbImageIconEssentialData_t > &imageIcons,
				std::vector< LgbTextStyleEssentialData_t > &vTextStyles);

