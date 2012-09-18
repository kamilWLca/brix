// Debugger.h: interface for the Debugger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DEBUGGER_H_INCLUDED_)
#define _DEBUGGER_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "DEP.h"
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
#include "TypeConversion.h"
#include "LucidTcpipClient.h"
#include "RtaTraceSupport.h"
#endif
#include "Porting_Classes/INXString.h"

class CDrawProgView;

class Debugger  
{
public:
	Debugger();
	virtual ~Debugger();
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	// Methods
	void DebugForce(INXPOSITION selectedIcon, int selectedPortType, int selectedPort);
	void DbgHistStart();
	void DbgHistTime();
	void DebugPause();
	void DebugRestart();
	void DebugStart(INXObjList* flattened,int mode);
	void DebugStep();
	void DebugStop();
#endif
	void DestroyView();
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	void FtpDbgCtrlFile(INXString filename);
	int GetEHSState();
	void LoadDbgHistFile(INXString filename, int dbgFileNum);
	void LoadDebugFile(INXString filename, int dbgFileNum);
	void displayTraceMsg(INXString csTraceMsg);
	INXString Tokenize(INXString csTraceMsg, int &curPos);
	void setView(CDrawProgView* _pView);
	void ExitDebugger();
	// Monitors
	void sendAllMonitors();
	bool sendMonitor(bool bOnOff, long iId, INXString csType);
	void ClearAllMonitors();
	// RTA Trace
	void setWriteRtaTrace(bool bWrite);
	bool getWriteRtaTrace();
#endif
	// debug variables
	char debugMode;
	bool dbgFinished;
	bool lock;
	int iDbgFileNum;
	bool dbgDataRdy;
	UINT timer;
	int prevEndTime;
	INXString timestamp;
	bool TestTimeStamp();
	bool dbgHistory;
	int iDbgHistFileNum;
	bool waitNxtEvnt;
	bool dbgHistTimeFlag1;
	bool dbgHistTimeFlag2;
	bool clearAll;
	long dbgHistTime;
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	LucidTcpipClient tcpClient;
#endif
	CDrawProgView* pView;
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	TypeConversion converter;
	//trace 
	RtaTraceSupport m_RtaTraceSupport;
#endif
private:
	bool m_bWriteRtaTrace;
	int ReStartInDebug(void);
};

#endif // !defined(AFX_DEBUGGER_H__43BFF4E6_8560_4AE7_87C3_EEAA637C44FF__INCLUDED_)
