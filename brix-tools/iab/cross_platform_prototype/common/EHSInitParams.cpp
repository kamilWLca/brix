#include "stdafx.h"
#include "EHSInitParams.h"

#define EHS_CONFIG_DISPLAY_HEIGHT 768	/**< Height for the OSD part of the display */
#define EHS_CONFIG_DISPLAY_WIDTH 1024	/**< Width for the OSD part of the display */

EHSInitParams::EHSInitParams(void)
{
	//@todo - all these defaults should come from EHS, not here
	nXCoord = 10;
	nYCoord = 10;
	nWidth = EHS_CONFIG_DISPLAY_WIDTH;		//these are defined in the platform config header file
	nHeight = EHS_CONFIG_DISPLAY_HEIGHT;	//these are defined in the platform config header file
	nColourRed = 32; /* default to dark grey rather than black this avoids the invisibility problem with gstreamer setting up colour keying*/
	nColourGreen = 32;
	nColourBlue = 32;
	nAlpha = 255;							//always black and opaque defaults
	nHasFrame = 1;
	nZOrder = 1;							// default to normal z-order	
}

EHSInitParams::~EHSInitParams(void)
{
}
