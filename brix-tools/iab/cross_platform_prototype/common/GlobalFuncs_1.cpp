#pragma once

#include "GlobalFuncs_1.h"
#include "GlobalFuncs_2.h"
#include "stdafx.h"
#include <fstream>
#include <cassert>

#include "../common/LgbIconDataStrucs.h"

//#include "../LucidGUIBuilder/LgbTextIcon.h"
//#include "../LucidGUIBuilder/LgbImageIcon.h"
//#include "../LucidGUIBuilder/LgbColorsDef.h"

#include "LucidConstants.h"

#define BUFFSIZE 50
#define RADIX 10

#include <vector>

#include "../common/LgbIconDataStrucs.h"

extern INXString intToString(const int val);

bool cleanupGuiLayoutBasics( LgbIconEssentialData_t &rEssDat, INXString &csWarnings  );

void parseGuiFile(
				const INXString &csFileName, 
				std::vector< LgbTextIconEssentialData_t > &textIcons, 
				std::vector< LgbPatchIconEssentialData_t > &patchIcons, 
				std::vector< LgbImageIconEssentialData_t > &imageIcons,
				std::vector< LgbTextStyleEssentialData_t > &vTextStyles)
{
	
/* Load background bitmaps from layout file */

	char seps[]   = ",\t\n\r";
	char sepsWithSpace[]   = " ,\t\n\r";


	INXString dummy;

	char fileLine[STD_LINE_LENGTH];
	char versionTagLabel[STD_FIELD_LENGTH];
	char versionStr[STD_FIELD_LENGTH];
	
	char * tokenPtr[24];

	char *token1 = NULL;
	char *next_token = NULL;
	char *pDummyChar = NULL;
	
	bool newFormat;

	// read first line to see if format is 'new' or not.
	bool gettingVersion = true;
	bool keepGoing = true;
	bool isComment = false;

	std::ifstream datafile( csFileName );
	int count = 0;
	int nTokens;
	int nonCommentLineNumber = 0;

	LgbTextIconEssentialData_t textIconDat;
	LgbPatchIconEssentialData_t patchIconDat;
	LgbImageIconEssentialData_t imageIconDat;
	LgbTextStyleEssentialData_t textStyleDat;

	LgbIconEssentialData_t basicDat;

	while ((!datafile.eof()) && (!datafile.bad())) {	

		// Get the first non-comment line, and determine the version.
		// Treat this differently from the rest of the file, as the format
		// maybe prehistoric (no version 1st-line) or may be modern.
		// If non-prehistoric, we can rely on comma-separators being present, but not otherwise.

		while( gettingVersion ){

			datafile.getline(fileLine, STD_LINE_LENGTH);
			if(fileLine[0] != '#' ) {

				if(strstr( fileLine, "Version")==fileLine){

					token1 = strtok_s( fileLine, sepsWithSpace, &next_token );	
					strcpy_s(versionTagLabel, STD_FIELD_LENGTH, token1 );

					token1 = strtok_s( NULL, sepsWithSpace, &next_token);
					strcpy_s(versionStr, STD_FIELD_LENGTH, token1 );

					newFormat = true;

				} else { //if(strstr( fileLine, "Version")==fileLine)

					strcpy_s(versionTagLabel, STD_FIELD_LENGTH, "Pre-Historic" );
					strcpy_s(versionStr, STD_FIELD_LENGTH, "0.0.0" );
					newFormat = false;
				} // if(strstr( fileLine, "Version")!=fileLine)

				gettingVersion = false;

			} // if(fileLine[0] != '#' )

		} // while( gettingVersion )

		datafile.getline(fileLine, STD_LINE_LENGTH);

		count = 0;
		token1 = strtok_s( fileLine, seps, &next_token );	
		tokenPtr[count++] = token1;

		while ( token1 != NULL )
		{
			token1 = strtok_s( NULL, seps, &next_token);	
			tokenPtr[count++] = token1;
		}

		nTokens = count - 1;

		if( tokenPtr[0] != NULL ){

			if( (tokenPtr[0])[0] != '#' )
			{
				
				if(newFormat){

				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// NEW FORMAT
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

					dummy = tokenPtr[0];
					basicDat.tag = dummy.TrimLeft().TrimRight();
	

					dummy = tokenPtr[1];
					basicDat.type = dummy.TrimLeft().TrimRight(); 
					
					if (basicDat.type == "TextStyle") {
						textStyleDat.csClass = basicDat.tag;
						textStyleDat.csFont = tokenPtr[2];
						textStyleDat.alpha = atoi(tokenPtr[3]);
						textStyleDat.red = atoi(tokenPtr[4]);
						textStyleDat.green = atoi(tokenPtr[5]);
						textStyleDat.blue = atoi(tokenPtr[6]);
					}
					else {
						basicDat.xTopLft = atoi( tokenPtr[2] );
						basicDat.yTopLft = atoi( tokenPtr[3] );
						basicDat.width = atoi( tokenPtr[4] );				
						basicDat.height = atoi( tokenPtr[5] );

						basicDat.zPos = atoi( tokenPtr[6] );

						INXString csWarnings;
						if(cleanupGuiLayoutBasics( basicDat, csWarnings  ))
							AfxMessageBox(csWarnings);
					}



					if ( (basicDat.type == "GUI_Bitmap") ||  (basicDat.type == "GUI_Image") ){

						imageIconDat.basics = basicDat;

						imageIconDat.alpha= atoi( tokenPtr[7] );

					// strip-off any leading or trailing spaces..
						dummy =  tokenPtr[8];
						imageIconDat.bitmapFileName = dummy.TrimLeft().TrimRight(); 
						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

						// new item added to GUI file for images, set to 0 for old versions
						imageIconDat.lockAspect = 0;
						if (nTokens > 9) {
							imageIconDat.lockAspect = atoi( tokenPtr[9] );
						}


					} else if ((basicDat.type == "GUI_TextBox") || (basicDat.type == "GUI_TextBox1") || (basicDat.type == "GUI_TextBox2")) {

						textIconDat.basics = basicDat;

						textIconDat.fgAlpha  = atoi( tokenPtr[7] );
						textIconDat.fgRed  = atoi( tokenPtr[8] );
						textIconDat.fgGreen  = atoi( tokenPtr[9] );
						textIconDat.fgBlue  = atoi( tokenPtr[10] );

						textIconDat.bgAlpha  = atoi( tokenPtr[11] );
						textIconDat.bgRed  = atoi( tokenPtr[12] );
						textIconDat.bgGreen  = atoi( tokenPtr[13] );
						textIconDat.bgBlue  = atoi( tokenPtr[14] );
						
						
						if ((basicDat.type == "GUI_TextBox2")) {
							textIconDat.csFont = tokenPtr[15];
							textIconDat.leftIndent = atoi(tokenPtr[16]);
							textIconDat.rightIndent = atoi(tokenPtr[17]);
							textIconDat.topIndent = atoi(tokenPtr[18]);
							textIconDat.bottomIndent = atoi(tokenPtr[19]);
							textIconDat.lineSpacing = atoi(tokenPtr[20]);
							//textIconDat.fonttype  = atoi( tokenPtr[15] );
							//textIconDat.fonsize  = atoi( tokenPtr[16] );
						}
						else {
							textIconDat.basics.type = "GUI_TextBox2";
							textIconDat.csFont = "lsr";
							textIconDat.leftIndent = 10;
							textIconDat.rightIndent = 10;
							textIconDat.topIndent = 10;
							textIconDat.bottomIndent = 10;
							textIconDat.lineSpacing = 12;
						}

					} else if ( basicDat.type == "GUI_Patch" ){

						patchIconDat.basics = basicDat;

						patchIconDat.bgAlpha  = atoi( tokenPtr[7] );
						patchIconDat.bgRed  = atoi( tokenPtr[8] );
						patchIconDat.bgGreen  = atoi( tokenPtr[9] );
						patchIconDat.bgBlue  = atoi( tokenPtr[10] );

					} 
					else if (basicDat.type == "TextStyle") {
					}
					else {
						assert(1==0);
					}

				} else {

				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// PREVIOUS FORMAT
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

					dummy = tokenPtr[0];
					basicDat.tag = dummy.TrimLeft().TrimRight(); 

					dummy = tokenPtr[1];
					basicDat.type = dummy.TrimLeft().TrimRight();
					

					basicDat.xTopLft = atoi( tokenPtr[2] );
					basicDat.yTopLft = atoi( tokenPtr[3] );
					basicDat.width = atoi( tokenPtr[4] );				
					basicDat.height = atoi( tokenPtr[5] );
					basicDat.location = atoi( tokenPtr[6] );

					INXString csWarnings;
					if(cleanupGuiLayoutBasics( basicDat, csWarnings  ))
						AfxMessageBox(csWarnings);

					if ( (basicDat.type == "GUI_Bitmap") ||  (basicDat.type == "GUI_Image") ){

						imageIconDat.basics = basicDat;

					// strip-off any leading or trailing spaces..
						dummy =  tokenPtr[8];
						dummy = dummy.TrimLeft().TrimRight(); 
						
						if(dummy == "dummy.bmp")
							dummy=ICON_PLACEHOLDER_FILE;

						imageIconDat.bitmapFileName = dummy;

						basicDat.zPos = atoi( tokenPtr[9]);
						imageIconDat.alpha = 255;

					} else {

						textIconDat.basics = basicDat;

						textIconDat.bgRed  = atoi( tokenPtr[8] );
						textIconDat.bgGreen  = atoi( tokenPtr[9] );
						textIconDat.bgBlue  = atoi( tokenPtr[10] );
						textIconDat.fgRed  = atoi( tokenPtr[11] );
						textIconDat.fgGreen  = atoi( tokenPtr[12] );
						textIconDat.fgBlue  = atoi( tokenPtr[13] );
						textIconDat.basics.zPos = atoi( tokenPtr[14] );

						textIconDat.fgAlpha = 255;
						textIconDat.bgAlpha = 255;
					}
				} 

				if (basicDat.type=="GUI_Bitmap") {

					INXString projDir = "";
					imageIcons.push_back( imageIconDat );

				} else if (basicDat.type=="GUI_TextBox" || basicDat.type=="GUI_TextBox2") {

					textIcons.push_back(  textIconDat );

				} else if (basicDat.type=="GUI_Patch") {

					patchIcons.push_back(  patchIconDat );
				}
				else if (basicDat.type == "TextStyle") {
					vTextStyles.push_back(textStyleDat);
				}
				else{


					// unknown type!
					assert(1==0);

				}

			} 

		}  // if( token1!= NULL )
		
	}

	datafile.close();

}

bool cleanupGuiLayoutBasics( LgbIconEssentialData_t &rEssDat, INXString &csWarnings  )
{
	bool warningsExist = false;

	if( rEssDat.width < 0 ){
		warningsExist = true;
		csWarnings += rEssDat.tag;
		csWarnings += " width set from ";
		csWarnings += intToString(rEssDat.width);
		csWarnings += " to ";
		csWarnings += intToString(50);
		csWarnings += "\n";
		rEssDat.width = 50;
	}

	if( rEssDat.height < 0 ){
		warningsExist = true;
		csWarnings += rEssDat.tag;
		csWarnings += " height set from ";
		csWarnings += intToString(rEssDat.height);
		csWarnings += " to ";
		csWarnings += intToString(50);
		csWarnings += "\n";
		rEssDat.height = 50;
	}

	if(warningsExist)
		csWarnings = "GUI-layout post-processing message:\n" + csWarnings;

	return warningsExist;

}





