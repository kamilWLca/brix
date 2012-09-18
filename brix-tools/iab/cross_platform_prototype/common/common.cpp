#include "stdafx.h"
#include "common.h"


void getlowercase(char *str, int size)
{
	// Typical use getlowercase(test1, strlen(test1));

	int index = 0;
	while(index < size)
	{
		if(str[index] >= 65 && str[index] <= 90)
			str[index] += 32;
		index++;
	}
}