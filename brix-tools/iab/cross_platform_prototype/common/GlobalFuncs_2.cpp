
//#include "stdafx.h"
#include <fstream>
//#include <oledb.h>
#include <cassert>
#include "Porting_Classes/INXPoint.h"
#include "LucidConstants.h"
#include <stdio.h>
#include <stdlib.h>


#define BUFFSIZE 50
#define RADIX 10

#include <vector>
#include <list>



typedef struct tagDBTIMESTAMP{
    short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
    unsigned long fraction;
    }DBTIMESTAMP;

// yyyy-mm-dd HH:MM:SS:mmm

DBTIMESTAMP parseTimeStamp( INXString &cs )
{
	DBTIMESTAMP ts ;
	memset( &ts, 0, sizeof(DBTIMESTAMP) ); 

	INXString cs2;
    const size_t newsize = 100;
    char buff[newsize];
int i;
	for(i=0;i<cs.GetLength();i++)
		buff[i] = (char) cs.GetAt(i);

	buff[i+1] = '\0';

	sscanf( buff,"%4d-%2d-%2d %2d:%2d:%2d:%9d", &ts.year, &ts.month, &ts.day, &ts.hour, &ts.minute, &ts.second, &ts.fraction );

	return ts;
}

INXString csPrintTimeStamp( const DBTIMESTAMP &ts )
{
	char buff[100];
	sprintf(buff, "%4d-%2d-%2d %2d:%2d:%2d:%3d",
		ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, ts.fraction/1000000 );

	return INXString(buff);
}

bool firstStampIsNewer( const DBTIMESTAMP &ts1,  const DBTIMESTAMP &ts2 )
{
	if( ts1.year < ts2.year)
		return false;
	else if ( ts1.year > ts2.year)
		return true;
	else
	{
		// years are equal
		if( ts1.month < ts2.month )
			return false;
		else if ( ts1.month > ts2.month )
			return true;
		else {
			// months are equal

			if( ts1.day < ts2.day )
				return false;
			else if ( ts1.day > ts2.day )
				return true;
			else {
				// days are equal

				if( ts1.hour < ts2.hour )
					return false;
				else if ( ts1.hour > ts2.hour )
					return true;
				else {
					// hour are equal

					if( ts1.minute < ts2.minute )
						return false;
					else if ( ts1.minute > ts2.minute )
						return true;
					else {
						// minute are equal

						if( ts1.second < ts2.second )
							return false;
						else if ( ts1.second > ts2.second )
							return true;
						else
						{
							// second are equal

							if( ts1.fraction < ts2.fraction )
								return false;
							else if ( ts1.fraction > ts2.fraction )
								return true;
							else{
								// cor blimey, this was hell of a coincidence!
								// Phone Jeremy Paxman!
								assert(1==0);
								return true;
							}

						} // second are equal

					} // minute are equal

				} // hours are equal

			} // days are equal

		} // ts1.month == ts2.month

	} // ts1.year == ts2.year
}


/**
 *	Helper functions:
 *  FileSize()	Obtaining the file size
 *	SendFile()  Makes it easier for user to pass file to be sent and target file name 
 */
int FileSize(const INXString &csFilePath)
{
	long begin,end;
	std::ifstream myfile(csFilePath,std::ios::in);
	if(!myfile) return -1;
	begin = myfile.tellg();
	myfile.seekg(0, std::ios::end);
	end = myfile.tellg();
	myfile.close();
	return (end-begin);
}



INXString intToString(const int val)
{
	//char buff[BUFFSIZE];
	//itoa( val, buff, RADIX);
	INXString buff;
	buff = buff.Format(wxT("%d"),val);
		
	return	INXString(buff);

} 

void parseSodlWidgetData( const INXString &fileLine, INXString &widgetTag, INXString &targetFileName )
{

	// A GUI tag is preceded by %%%_ which needs to be removed from the GUI file
	widgetTag = fileLine;
	widgetTag.Delete(0, 4);
	// Now that the %%%_ has gone, store value for target file name also.
	targetFileName = widgetTag;

	// where is space between widget tag and target file name
	int posnOfSeparatorSpace = widgetTag.Find(INXString(" "));

	// get the characters of the widget tag (ie up to, not including, the space).
	widgetTag = widgetTag.Left(posnOfSeparatorSpace);

	// Now get the target file name.

	int length = targetFileName.GetLength();
	targetFileName = targetFileName.Right(length-posnOfSeparatorSpace-1);

	return;

}

int parseLines( INXString &csTextBlock, std::list<INXString> &rLines )
{

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Split-up the received text into new-line terminated pieces.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// 1st of all, chop-off any \n's at the beginning of the string, as these are effectively 0-length
	// lines, which are not worth processing further.

	rLines.clear();

	int charPos = csTextBlock.Find('\n');
	int len = csTextBlock.GetLength();
	while(charPos == 0){
		csTextBlock = csTextBlock.Right(len-1);
		charPos = csTextBlock.Find('\n');
		len = csTextBlock.GetLength();
	}


	charPos = csTextBlock.Find('\n');
	len = csTextBlock.GetLength();

	if ( charPos == -1 ) {
		// There is no '\n' at all.  Treat the string as just one line.
		rLines.push_back(csTextBlock);
		csTextBlock = "";

	} else {

		while( charPos != -1 ){

			// The \n is in the middle somewhere - get the left-most substring
			rLines.push_back(csTextBlock.Left(charPos+1) );
			csTextBlock = csTextBlock.Right(csTextBlock.GetLength() - 1 - charPos);

			charPos = csTextBlock.Find('\n');
			len = csTextBlock.GetLength();

		} // while( charPos != -1 )

	} // if(charPos != -1)

	len = csTextBlock.GetLength();

	if ( csTextBlock.GetLength() > 0 ) {
		// Pushthe last non- \n terminated string
		rLines.push_back(csTextBlock);
		csTextBlock = "";

	}
	return rLines.size();

}

INXString cloneBackSlashes( const INXString &csText )
{
	INXString csRetval = "";

	for(int i=0;i<csText.GetLength();i++)
	{
		csRetval.AppendChar(((INXString)csText)[i] );
		if( ((INXString)csText)[i] == '\\'){
			csRetval.AppendChar( '\\' );
		}
	}
	return csRetval;
}

// Function that gets the data from an ini file by specifying the section and key
INXString GetIniValue(INXString csSection, INXString csKey, INXString csIniFileName) 
{
	char szDestBuff[256]={'\0'};
	char* pDestBuff = &szDestBuff[0];
	INXString csDestBuff = "";
	char szDefault[]={""};

//	GetPrivateProfileString(csSection, csKey, szDefault, szDestBuff, sizeof(szDestBuff), csIniFileName);

	csDestBuff = pDestBuff;
	return csDestBuff;
}

// Function that gets the data from an ini file by specifying the section and key
void GetIniKeys(INXString csSection, INXString csIniFileName, std::vector<INXString> &vKeysVec) 
{
	char szDestBuff[32767]={'\0'};
	char* pDestBuff = &szDestBuff[0];
	char szDefault[]={"NULL"};
	bool endOfBuffer = FALSE;
	int i = 0, j = 0, iEquals;
	char szTmpBuff[256]={'\0'};
	INXString csTmpBuff, csToolKey;

//	GetPrivateProfileSection(csSection, pDestBuff, sizeof(szDestBuff), csIniFileName);

	// Extracts strings from szDestBuff and puts them in an array
	while (!endOfBuffer) {
		// if '\0' then end of string so add to array
		if (szDestBuff[i] == '\0') {
			szTmpBuff[j] = szDestBuff[i];
			csTmpBuff = (INXString)szTmpBuff;
			// Only want to add key to vector
			iEquals = csTmpBuff.Find('=');
			csToolKey = (INXString)csTmpBuff.Left(iEquals);
			vKeysVec.push_back(csToolKey);
			j = 0;
		}
		else {
			szTmpBuff[j] = szDestBuff[i];
			j++;
		}
		// if there are 2 consecutive '\0' then its the end of the buffer
		if (szDestBuff[i] == '\0' && szDestBuff[i+1] == '\0') {
			endOfBuffer = TRUE;
		}
		i++;
	}
}

// Function that snaps a point to a grid
INXPoint SnapToGrid(INXPoint point) {
	INXPoint ret;

	ldiv_t blob = ldiv(point.x,GRIDSIZE);
	ret.x = blob.quot*GRIDSIZE;
	blob = ldiv(point.y,GRIDSIZE);
	ret.y = blob.quot*GRIDSIZE;

	return ret;
}
