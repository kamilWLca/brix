#include "stdafx.h"
#include <winreg.h>

/**
 *  returns a registry value specified by the section path and key
 *  csSectionPath is the full section path e.g. "Software\\Lucid\\LucidApplicationBuilder\\Target"
 *  csKey is the key
 *  csDefault is the dafault value
 */

CString getLucidRegValue(CString csSectionPath, CString csKey, CString csDefault)
{
	HKEY myKey;
	DWORD varType = REG_SZ; //the value's a REG_SZ type
	DWORD buffSize = 1024;
	LPBYTE pszBuf=NULL;
	pszBuf = (LPBYTE)malloc(buffSize);
	CString retStr = "";

	pszBuf[0] = '\0';
	RegOpenKeyEx(HKEY_CURRENT_USER,csSectionPath, 0, KEY_READ, &myKey);
	RegQueryValueExA(myKey,csKey,NULL,&varType,pszBuf,&buffSize);
	if (pszBuf[0] == '\0') {
		retStr = csDefault;
	}
	else {
		retStr = (LPCSTR(pszBuf));
	}
	free(pszBuf);
	RegCloseKey(myKey);

	return retStr;
} 

void setLucidRegValue(const CString csSectionPath, const CString csKey, CString csValue)
{
	HKEY myKey;
	DWORD varType = REG_SZ; //the value's a REG_SZ type
	DWORD buffSize = 1024;
	DWORD dwDisp;

	char pBuf[1024] = {'\0'};
	const int nLen = csValue.GetLength();
	strcpy(pBuf, csValue.GetBuffer(nLen));

	// if the Section Path doesn't exist then create it
	if (RegOpenKeyEx(HKEY_CURRENT_USER,csSectionPath, 0, KEY_WRITE, &myKey)) {
		RegCreateKeyEx(HKEY_CURRENT_USER, csSectionPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &myKey, &dwDisp);
	}
	RegSetValueExA(myKey,csKey,0,varType,(const BYTE *)pBuf,sizeof(pBuf));
	RegCloseKey(myKey);
	csValue.ReleaseBuffer();
}
