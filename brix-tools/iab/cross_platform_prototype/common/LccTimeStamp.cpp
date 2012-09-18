//#include "stdafx.h"
#include "LccTimeStamp.h"
#include <cassert>

LccTimeStamp::LccTimeStamp(void)
{
}


LccTimeStamp::~LccTimeStamp(void)
{
}

LccTimeStamp& LccTimeStamp::operator=(const DBTIMESTAMP &timeStamp)
{
	DBTIMESTAMP::operator=( timeStamp );
	return *this;
}

void LccTimeStamp::csParse( INXString &cs )
{
	//DBTIMESTAMP ts ;
	memset( this, 0, sizeof(DBTIMESTAMP) ); 

    const size_t newsize = 100;
    char buff[newsize];

	int i;
	for(i=0;i<cs.GetLength();i++)
		buff[i] = (char) cs.GetAt(i);

	buff[i+1] = '\0';

	sscanf( buff,
		//(char *)lptStr, 
		//dum,
		"%04d-%02d-%02d %02d:%02d:%02d:%09d", 
		&year, &month, &day, &hour, &minute, &second, &fraction );
		//&ts.year, &ts.month, &ts.day, &ts.hour, &ts.minute, &ts.second, &ts.fraction );

	return;
}

void LccTimeStamp::setAncient()
{
	INXString dum( LCCTIMESTAMP_ANCIENTDATE );
	csParse( dum );
}


INXString LccTimeStamp::csPrint( )
							  //const DBTIMESTAMP &ts )
{
	char buff[100];
	sprintf(buff,"%04d-%02d-%02d %02d:%02d:%02d:%03d",
		year, month, day, hour, minute, second, fraction/1000000 );

	return INXString(buff);
}

bool LccTimeStamp::isNewerThan( const DBTIMESTAMP &ts2 )
{
	if( year < ts2.year)
		return false;
	else if ( year > ts2.year)
		return true;
	else
	{
		// years are equal
		if( month < ts2.month )
			return false;
		else if ( month > ts2.month )
			return true;
		else {
			// months are equal

			if( day < ts2.day )
				return false;
			else if ( day > ts2.day )
				return true;
			else {
				// days are equal

				if( hour < ts2.hour )
					return false;
				else if ( hour > ts2.hour )
					return true;
				else {
					// hour are equal

					if( minute < ts2.minute )
						return false;
					else if ( minute > ts2.minute )
						return true;
					else {
						// minute are equal

						if( second < ts2.second )
							return false;
						else if ( second > ts2.second )
							return true;
						else
						{
							// second are equal

							if( fraction < ts2.fraction )
								return false;
							else if ( fraction > ts2.fraction )
								return true;
							else{
								// cor blimey, this was hell of a coincidence!
								// Phone Jeremy Paxman!
								//assert(1==0);
								// Sometimes this happens because the fraction is 0.
								// then, if you've saved something (like  gui file, on prompt) just before
								// this point, the times do come out equal.
								return true;
							}

						} // second are equal

					} // minute are equal

				} // hours are equal

			} // days are equal

		} // month == ts2.month

	} // year == ts2.year
}
