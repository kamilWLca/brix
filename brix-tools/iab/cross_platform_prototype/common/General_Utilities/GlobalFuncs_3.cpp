#pragma once
#include "stdafx.h"
#include <psapi.h>
#include <cassert>

////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 and Visual Studio .NET
// Runs in Windows XP and probably Windows 2000 too.
//
// lp -- list processes from command line.
// Illustrates how to use the various iterator classes in EnunmProc.h/cpp
//
#include "stdafx.h"
#include "EnumProc.h"

// check for switch: / or -
inline BOOL isswitch(TCHAR c) { return c==L'/' || c==L'-'; }

//////////////////
// Main entry point
//
HWND getTitledApplicationWindow( TCHAR* psAppName, TCHAR* psWindowTitle, HANDLE &hProcess )
{
	
	int count=0;
	BOOL bFirstModule = TRUE;
	CProcessIterator itp;
	for (DWORD pid=itp.First(); pid; pid=itp.Next()) {
		// Note: first module in CProcessModuleIterator is EXE for this process
		//
		TCHAR modname[_MAX_PATH];
		CProcessModuleIterator itm(pid);
		HMODULE hModule = itm.First(); // .EXE
		if (hModule) {
			GetModuleBaseName(itm.GetProcessHandle(),
				hModule, modname, _MAX_PATH);

			// Iterate over all top-level windows in process
			//
			BOOL bFirstWindow = TRUE;
			CMainWindowIterator itw(pid);
			for (HWND hwnd = itw.First(); hwnd; hwnd=itw.Next()) {

				if (bFirstWindow) {

					bFirstWindow = FALSE;
					char classname[256],title[256];
					GetClassName(hwnd,classname,sizeof(classname));
					GetWindowText(hwnd,title,sizeof(title));
					if(strstr( modname, psAppName) == modname) {
						if(strstr( title, psWindowTitle) == title) {
							hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
							return hwnd;
						}
					}
				} 
			}
			bFirstWindow || count++;
		}
	}
	return NULL;
}
