// Debugger.cpp: implementation of the Debugger class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DrawProg.h"
#include "Debugger.h"
#include "GlobalFuncs_2.h"
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
#include "../common/General_Utilities/GlobalFuncs_3.h"
#endif
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
#include "ForceValueDialog.h"
#include "GotoTimeDialog.h"
#endif
// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

//void DebugThread(void * data);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Debugger::Debugger()
{
	debugMode = DBGSTOP;
	dbgFinished = TRUE;
	lock = FALSE;
	dbgDataRdy = FALSE;
	prevEndTime = 0;
	dbgHistory = FALSE;
	dbgHistTimeFlag1 = FALSE;
	dbgHistTimeFlag2 = FALSE;
	m_bWriteRtaTrace = FALSE;
	clearAll=FALSE;
}

Debugger::~Debugger()
{
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
// separate thread used to read debug trace messages
void DebugThread(void * data) {
	Debugger * dbg = (Debugger *) data;
	INXString csTraceMsg;

	while (dbg->debugMode != DBGSTOP) {
		dbg->tcpClient.ReceiveTextBlock(csTraceMsg);
		dbg->displayTraceMsg(csTraceMsg);
		if (dbg->getWriteRtaTrace()) {
			dbg->m_RtaTraceSupport.rtaTraceLogFlush();
			dbg->setWriteRtaTrace(FALSE);
		}
		Sleep(DEBUG_SLEEP);
	}
	dbg->dbgFinished = TRUE;
	if (dbg->pView) {
		dbg->pView->pProject->ResetDebug();
		dbg->pView->RedrawWindow();
	}
}

void Debugger::DbgHistStart() {
	dbgHistory = TRUE;
	iDbgHistFileNum = 0;
	char szDbgHistFileNum[128];
	MSG message;

	// map the line IDs in the SODL to the lines in the view
	//*** do we need to do this, since should not be able to do history if not done debug
	//pProject->WriteSODL(pProject->projectDir + FTPSODL);
	//pDEP->SetCondataLineID();

	pView->pProject->ResetDebug();

	while (dbgHistory) {
		// Display message if reach end of debug files,
		// and start at debug0
		if (iDbgHistFileNum > iDbgFileNum) {
			AfxMessageBox("You have reached the end of the recorded data. Returning to the start.");
			iDbgHistFileNum = 0;
			pView->pProject->ResetDebug();
		}
		_itoa_s(iDbgHistFileNum,szDbgHistFileNum,10);
		LoadDbgHistFile(workDir + DEBUGDIR + DEBUGFILE + szDbgHistFileNum + ".txt", iDbgHistFileNum);
		iDbgHistFileNum++;
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	// reset all debug data and events when leaving debug history mode
	pView->pProject->ResetDebug();
	pView->RedrawWindow();
}

void Debugger::DbgHistTime() {
	CGotoTimeDialog dialog;
	ifstream dbgfile;
	long iMaxEndTime;
	char szMaxEndTime[10];
	char unused[256] = {'\0'};
	bool isEmpty = FALSE;
	bool foundMaxTime = FALSE;
	int iFileNum = iDbgFileNum;
	char szFileNum[128];

	// get the max debug history time
	while (!foundMaxTime) {
		_itoa_s(iFileNum,szFileNum,10);
		INXString filename = workDir + DEBUGDIR + DEBUGFILE + szFileNum + ".txt";
		dbgfile.open(filename);

		isEmpty = FALSE;
		dbgfile >> unused;
		// if a file is empty don't load it
		if ((INXString)unused == "") {
			isEmpty = TRUE;
		}
		dbgfile.close();

		dbgfile.open(workDir + DEBUGDIR + DEBUGFILE + szFileNum + ".txt");
		// read the file
		while ((!dbgfile.eof()) && (!dbgfile.bad()) && !isEmpty) {
			dbgfile >> unused;
			dbgfile >> iMaxEndTime;
			dbgfile.getline(unused, 255);
			foundMaxTime = TRUE;
		}
		dbgfile.close();
		iFileNum--;
		// if all the files are empty then display a message
		if (iFileNum<0 && !foundMaxTime) {
			AfxMessageBox("No data was recorded during debug.");
			return;
		}
	}

	bool timeOK = FALSE;
	while (!timeOK) {
		if (dialog.DoModal()==IDOK) {
			_itoa_s(iMaxEndTime, szMaxEndTime, 10);
			if (dialog.m_DbgHistTime > iMaxEndTime) {
				AfxMessageBox("The maximum time allowed is " + (INXString)szMaxEndTime);
			}
			else {
				dbgHistTime = dialog.m_DbgHistTime;
				// set a flag to indicate to keep reading data until the specified time is reached
				dbgHistTimeFlag1 = TRUE;
				dbgHistTimeFlag2 = TRUE;
				waitNxtEvnt = TRUE;
				timeOK = TRUE;
			}
		}
	}
}

void Debugger::DebugForce(INXPOSITION selectedIcon, int selectedPortType, int selectedPort) {
}

/*
void Debugger::DebugForce(POSITION selectedIcon, int selectedPortType, int selectedPort) {
	ConData* blob;
	int iDataType;
	FTPOptions options;

	// Menu should be disabled when not in debug mode
	if (debugMode != DBGPAUSE) {
		AfxMessageBox("Warning: Can only force data and events in debug pause mode.";
		return;
	}

	ofstream forcefile(workDir + DEBUGDIR + "insert.txt";

	blob = (ConData*) pView->pDEP->condata->GetAt(selectedIcon);
	// write out data type folowed by line ID followed by value.
	if (selectedPortType == STARTPORT) {
		forcefile << "E" << "\t";
		forcefile << blob->startport[selectedPort]->line.idNum << endl;
	}
	else if (selectedPortType == INPUTPORT) {
		iDataType = blob->inputport[selectedPort]->datatype;
		CForceValueDialog dialog(iDataType);
		if (dialog.DoModal()==IDOK) {
			forcefile << converter.DataType2Char(iDataType) << "\t";
			forcefile << blob->inputport[selectedPort]->line.idNum << "\t";
			// if the line clicked is connected to an input port then prompt the user for the value
			forcefile << dialog.m_ForceValue << endl;
		}
	}

	forcefile.close();

	if (!ftpDebug.ftpPut(workDir + DEBUGDIR + "insert.txt","insert.txt",pFtpConn,FTP_TRANSFER_TYPE_ASCII)) {
			AfxMessageBox("Unable to transfer insert.txt.";
	}
}
*/

void Debugger::DebugPause()
{
	// Stop app running on ehs
	if (( tcpClient.SendText("P\n")) != 0)	{
		AfxMessageBox("Unable to send command over TCPIP connection.");
		tcpClient.DisConnect();
		return;
	}

	debugMode = DBGPAUSE;
}

void Debugger::DebugRestart()
{
	if (debugMode == DBGSTEP) {
		if (( tcpClient.SendText("P\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}

	if (debugMode == DBGPAUSE || debugMode == DBGSTEP) {
		if (( tcpClient.SendText("C\n")) != 0)	{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}

	debugMode = DBGSTART;
}

/* Note the mode flag was intended to allow debugger to be different at start up 
it is actually not needed as this function doesn't do any EHS control other than set the montitors
*/

void Debugger::DebugStart(INXObjList *flattened,int mode)
{
	LtsStatusType ltsStatusType;
	char szDebugStartCommand[10];
	// connect to tcpip client
	if (( ltsStatusType = tcpClient.Connect()) != 0){
		AfxMessageBox("Unable to establish a TCPIP connection.");
	} else {
		// Start debug on ehs
		if (mode == 1 ) if (( tcpClient.SendText("F\n")) != 0); //relaod and get ready
		if (( tcpClient.SendText("=+\n")) != 0)
		{
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			delete pView->dbgMsg;
			return;
		}
		pView->pProject->setDefineMonitors(TRUE);
		debugMode = DBGSTART;
		dbgFinished = FALSE;
		// Do mapping
		pView->pDEP->MapLineID(flattened);
		pView->pDEP->dbgMappedFlag = TRUE;
		pView->RedrawWindow();
		//if (mode == 0 ) 
			sendAllMonitors(); //don't do this in the other case as it is assumed that this is already done before the app is reloaded
		//if (mode) tcpClient.SendText("l\n");//reload app and run
		// read debug files using a separate thread
		if (mode == 1 ) {
				Sleep(200);// why not
				tcpClient.SendText("R\n"); //relaod and get ready
				Sleep(200);// why not
		}
		AfxBeginThread((AFX_THREADPROC)DebugThread, this);
	}
	delete pView->dbgMsg;
}

void Debugger::DebugStep()
{
	// Send step command
	if (( tcpClient.SendText("S\n")) != 0)	{
		AfxMessageBox("Unable to send command over TCPIP connection.");
		tcpClient.DisConnect();
		return;
	}
	debugMode = DBGSTEP;
	clearAll=TRUE;
}

void Debugger::DebugStop()
{
	// Stop debug on ehs
	// In DBGSTEP send P,=-,C
	// In DBGSTART send =-
	// In DBGPAUSE send =-,C
	clearAll=TRUE;
	if (debugMode == DBGSTEP) {
		if (( tcpClient.SendText("P\n")) != 0)	{
			debugMode = DBGSTOP;
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}
	if (( tcpClient.SendText("=-\n")) != 0)	{
		debugMode = DBGSTOP;
		AfxMessageBox("Unable to send command over TCPIP connection.");
		tcpClient.DisConnect();
		return;
	}
	if (debugMode == DBGPAUSE || debugMode == DBGSTEP) {
		if (( tcpClient.SendText("C\n")) != 0)	{
			debugMode = DBGSTOP;
			AfxMessageBox("Unable to send command over TCPIP connection.");
			tcpClient.DisConnect();
			return;
		}
	}

	ExitDebugger();
	tcpClient.DisConnect();
}

#endif
void Debugger::DestroyView() {
	//pView = NULL;
	//if (debugMode != DBGSTOP) {
	//	debugMode = DBGSTOP;
	//}
}
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
/*
void Debugger::FtpDbgCtrlFile(INXString filename) {
	char timeBuf[9];
	char dateBuf[9];
	FTPOptions options;
	ofstream ctrlfile(workDir + DEBUGDIR + filename);

	// write timestamp to debug control file
	_strdate_s(dateBuf);
	_strtime_s(timeBuf);
	timestamp = (INXString)(dateBuf) + "_" + (INXString)(timeBuf);
	ctrlfile << timestamp;
	ctrlfile.close();

	// ftp control file
	// set server directory
	//if (!ftpDebug.ftpSetServerDir((LPCSTR)options.m_serverDir, pFtpConn))
	//{
	//	AfxMessageBox("Cannot set server directory.";
	//}
	//else
	//{
		if (!ftpDebug.ftpPut(workDir + DEBUGDIR + filename,filename,pFtpConn,FTP_TRANSFER_TYPE_ASCII))
		{
			AfxMessageBox("Unable to transfer " + filename);
		}
	//}
}
*/

/*
int Debugger::GetEHSState()
{
	FTPOptions options;
	ifstream statefile;
	int iState;
	char szState[16] = {'\0'};
	char szTimeStamp[20] = {'\0'};

	//ftp get debug file
	//pConn = ftpDebug.ftpConnect((LPCSTR)options.m_server, (LPCSTR)options.m_username, (LPCSTR)options.m_password);
	// set server directory
	//if (!ftpDebug.ftpSetServerDir((LPCSTR)options.m_serverDir, pFtpConn))
	//{
	//	AfxMessageBox("Cannot set server directory.";
	//}
	//else
	//{
		if (!ftpDebug.ftpGet("state.txt",workDir + DEBUGDIR + "state.txt",pFtpConn))
		{
			AfxMessageBox("Unable to establish a connection. Check the EHS is running.";
			iState = -1;
		}
		else {
			statefile.open(workDir + DEBUGDIR + "state.txt";
			statefile >> szState;
			statefile >> szTimeStamp;
			statefile.close();
			if (strcmp(szState, "NORMAL") == 0) {
				iState = 0;
			}
			else if (strcmp(szState, "DEBUG") == 0) {
				iState = 1;
			}
			else if (strcmp(szState, "PAUSE") == 0) {
				iState = 2;
			}
			else {
				//AfxMessageBox("Unrecognizable State";
				iState = -1;
			}
		}
	//}

	return iState;
}
*/

// Load in a debug file in debug history mode
void Debugger::LoadDbgHistFile(INXString filename, int dbgFileNum) {
	ifstream dbgfile(filename);
	long startTime, endTime;
	char szDataType;
	int iDataType;
	char szValue[256] = {'\0'};
	long lineID;
	IconLines *line;
	bool isEmpty = FALSE;
	MSG message;
	INXObjList *lineList;
	INXPOSITION pos;

	dbgfile >> szValue;
	// if a file is empty don't load it
	if ((INXString)szValue == "") {
		isEmpty = TRUE;
	}
	dbgfile.close();

	dbgfile.open(filename);
	// read the file
	while ((!dbgfile.eof()) && (!dbgfile.bad()) && !isEmpty) {
		if (waitNxtEvnt) {
			pView->pDEP->ResetAllDbgEvents();
			dbgfile >> startTime;
			dbgfile >> endTime;
			dbgfile >> szDataType;
			dbgfile >> lineID;
			// get the value if its not an event
			if (szDataType != 'E') {
				if (szDataType == 'S') {
					dbgfile.ignore(1,'\t');
					dbgfile.getline(szValue,255);
				}
				else {
					dbgfile >> szValue;
				}
			}
			else if (dbgHistory) {
				// this flag exits from the file so can start reading data from debug0. this
				// is necessary if the user wants to go to a time that is smaller than the current
				// time
				if (dbgHistTimeFlag1) {
					dbgHistTimeFlag1 = FALSE;
					iDbgHistFileNum = -1;
					pView->pDEP->ResetAllDbgValues();
					break;
				}
				if (dbgHistTimeFlag2) {
					// if reach the go to time specified by user then stop loading data and return
					// to debug history step mode
					if (endTime >= dbgHistTime) {
						waitNxtEvnt = FALSE;
						dbgHistTimeFlag2 = FALSE;
					}
				}
				// debug history step mode set the flag to false. This is set to true when the
				// user chooses to step
				else {
					waitNxtEvnt = FALSE;
				}
			}
			// convert the data value to the right data type
			// and store it in the right line
			iDataType = converter.DataType2Int(szDataType);
			// get a list of lines that match the lineID
			lineList = pView->pDEP->GetLine(lineID, iDataType);
			pos = lineList->GetHeadPosition();
			while (pos) {
				line = (IconLines*) (lineList->GetNext(pos));
				if (line != NULL) {
					if (iDataType == 4) {
						line->dbgEvent = 1;
					}
					else {
						line->dbgValue = szValue;
					}
				}
			}
			delete lineList;
			//Sleep(endTime - prevEndTime);
			// don't redraw when finding a time specified by user
			if (!dbgHistTimeFlag2) {
				pView->RedrawWindow();
			}
		}
		//prevEndTime = endTime;
		// check if there are any other messages on the message queue.
		// if don't do this crashes when try and stop debug.
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	dbgfile.close();
	// ensure that the total sleep time is the same as the file write time to prevent
	// the DEP getting files before they exist.
	//fileEndTime = (dbgFileNum + 1) * 2000;
	//Sleep(fileEndTime - prevEndTime);
	//prevEndTime = fileEndTime;

}

// Display the trace messages
void Debugger::displayTraceMsg(INXString csTraceMsg)
{
	int curPos = 0;
	INXString csToken, csSeq, csID, csMsgType, csDataType, csValue, csTime;
	int iDataType;
	IconLines *line;
	INXObjList *lineList;
	INXPOSITION pos;
	bool redrawFlag = FALSE;
	int iRmCharPos;
	UINT iTime;

	// Stop RTA-Trace support if it isn't running
	// This will stop RTA support before the process has started.
	// Therefore need a flag that is set when RTA-Trace is started.
	/*
	if (!m_RtaTraceSupport.isRtaTraceRunning()) {
		m_RtaTraceSupport.setRtaTrace(FALSE);
	}
	*/

	// if the first character isn't '#' then ignore token
	if (csTraceMsg.GetAt(0) == '#') {
		csToken = "#";
	}
	else {
		csToken = csTraceMsg.Tokenize("#",curPos);
	}

	// tokenise trace message
	while (csToken != "") {
		csToken = Tokenize(csTraceMsg, curPos);
		if (csToken != "") {
			csSeq = ""; csID = ""; csMsgType = ""; csDataType = ""; csValue = "";

			csSeq = csToken;
			csID = Tokenize(csTraceMsg, curPos);
			csMsgType = Tokenize(csTraceMsg, curPos);
			if (csMsgType == EVENT_MSG_TYPE) {
				if (debugMode == DBGSTEP) {
					debugMode = DBGPAUSE;
				}
				if (pView && clearAll) { // only remove events if flag is set - the function may have a flag to clear only last set..
					pView->pDEP->ResetAllDbgEvents();
				}
				csTime = Tokenize(csTraceMsg, curPos);
				csDataType = "E";
			}
			else if (csMsgType == DATA_MSG_TYPE) {
				csTime = Tokenize(csTraceMsg, curPos);
				csDataType = Tokenize(csTraceMsg, curPos);
				csValue = Tokenize(csTraceMsg, curPos);
				// Strip off unwanted characters
				iRmCharPos = csValue.Find('>');
				if (iRmCharPos != -1) {
					csValue = csValue.Left(iRmCharPos);
				}
				iRmCharPos = csValue.Find('\n');
				if (iRmCharPos != -1) {
					csValue = csValue.Left(iRmCharPos);
				}
			}
			else if (csMsgType == START_FUNC_MSG_TYPE) {
				csTime = Tokenize(csTraceMsg, curPos);
				csDataType = "E";
			}
			else if (csMsgType == FINISH_FUNC_MSG_TYPE) {
				csTime = Tokenize(csTraceMsg, curPos);
				csDataType = "E";
			}

			// remove \n
			iRmCharPos = csTime.Find('\n');
			if (iRmCharPos != -1) {
				csTime = csTime.Left(iRmCharPos);
			}

			// EHS time is in us. RTA Trace seems to display in 10us
			if (m_RtaTraceSupport.getRtaTrace()) {
				sscanf(csTime, "%u", &iTime);
				iTime = iTime / 10;
				csTime = intToString(iTime);
			}

			// convert the data value to the right data type
			// and store it in the right line
			iDataType = converter.DataType2Int(csDataType);
			//iID = atoi(csID);
			// get a list of lines that match the lineID
			//if (pView && debugMode != DBGPAUSE) {
			if (pView) {
			lineList = pView->pDEP->GetLine((long)atoi(csID), iDataType);
			pos = lineList->GetHeadPosition();
			while (pos) {
				line = (IconLines*) (lineList->GetNext(pos));
				if (line != NULL) {
					if (iDataType == 4) {
						if (csMsgType == EVENT_MSG_TYPE) {
							line->dbgEvent = 1;
						}
						if (m_RtaTraceSupport.getRtaTrace()) {
							if (csMsgType == EVENT_MSG_TYPE) {
								m_RtaTraceSupport.rtaTraceActivate(line->getRtaTraceId(), csTime);
							}
							else if (csMsgType == START_FUNC_MSG_TYPE) {
								m_RtaTraceSupport.rtaTraceStart(line->getRtaTraceId(), csTime);
							}
							else if (csMsgType == FINISH_FUNC_MSG_TYPE) {
								m_RtaTraceSupport.rtaTraceEnd(line->getRtaTraceId(), csTime);
							}
						}
					}
					else {
						line->dbgValue = csValue;
						if (m_RtaTraceSupport.getRtaTrace()) {
							if (csDataType == "B") {
								if (csValue == "T") {
									csValue = "1";
								}
								else if (csValue == "F") {
									csValue = "0";
								}
							}

							// 0xA000 is an RTA-Trace number that indicates Tracepoint data
							if (csDataType == "R") {
								m_RtaTraceSupport.rtaTraceLogDataFloat(line->getRtaTraceId(), (0xA000 | line->getRtaTraceId()), csValue, csTime);
							}
							else if (csDataType == "S") {
								m_RtaTraceSupport.rtaTraceLogDataString(line->getRtaTraceId(), (0xA000 | line->getRtaTraceId()), csValue, csTime);
							}
							else {
								m_RtaTraceSupport.rtaTraceLogData(line->getRtaTraceId(), (0xA000 | line->getRtaTraceId()), csValue, csTime);
							}
						}
						//pView->InvalidateRect(line->valueRect, FALSE);
					}
				}
			}
			delete lineList;
			redrawFlag = TRUE;
			}
		}
	}

	if (redrawFlag && pView) {
		pView->RedrawWindow();
	}
}

INXString Debugger::Tokenize(INXString csTraceMsg, int &curPos)
{
	if (curPos < 0) {
		return "";
	}
	else {
		return csTraceMsg.Tokenize("#",curPos);
	}
}

void Debugger::setView(CDrawProgView *_pView)
{
	pView = _pView;
	m_RtaTraceSupport.setView(_pView);
}

/*
bool Debugger::TestTimeStamp()
{
	FTPOptions options;
	ifstream statefile;
	char szTimeStamp[20] = {'\0'};

	if (!ftpDebug.ftpGet("state.txt",workDir + DEBUGDIR + "state.txt",pFtpConn))
	{
		AfxMessageBox("Unable to establish a connection. Check the EHS is running.";
	}
	else {
		statefile.open(workDir + DEBUGDIR + "state.txt";
		statefile >> szTimeStamp;
		statefile >> szTimeStamp;
		statefile.close();
		return (strcmp(szTimeStamp, timestamp) == 0);
	}

	return FALSE;
}
*/

// Method that sends all the lines to monitor
void Debugger::sendAllMonitors()
{
	INXPOSITION pos;
	ConData* pBlob;
	INXString csTcpStr;
	TypeConversion tc;
	clearAll=FALSE; // don't clear from now on
	pos = pView->pDEP->condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (pView->pDEP->condata->GetNext(pos));
		for (UINT i=0; i<pBlob->inputport_num; i++) {
			if (pBlob->inputport[i]->line.getDbgMonitor() && pBlob->inputport[i]->line.exist) {
				if ((tcpClient.SendText("=M+" + intToString(pBlob->inputport[i]->line.idNum) + "," +
					tc.DataType2Char(pBlob->inputport[i]->datatype) + "\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
			// This is required for case when do an undo. In this case all monitors must be resent
			else if (!pBlob->inputport[i]->line.getDbgMonitor() && pBlob->inputport[i]->line.exist) {
				if ((tcpClient.SendText("=M-" + intToString(pBlob->inputport[i]->line.idNum) + "," +
					tc.DataType2Char(pBlob->inputport[i]->datatype) + "\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
		}
		for (UINT i=0; i<pBlob->startport_num; i++) {
			if (pBlob->startport[i]->line.getDbgMonitor() && pBlob->startport[i]->line.exist) {
				if ((tcpClient.SendText("=M+" + intToString(pBlob->startport[i]->line.idNum) + ",E\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
			// This is required for case when do an undo. In this case all monitors must be resent
			else if (!pBlob->startport[i]->line.getDbgMonitor() && pBlob->startport[i]->line.exist) {
				if ((tcpClient.SendText("=M-" + intToString(pBlob->startport[i]->line.idNum) + ",E\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
		}
	}
}

// Method that sends a monitor command to the EHS for a line specified by its ID and type
// bOnOff determines whether the monitor is turned on or off
// Returns true if the send was successful, otherwise it returns false
bool Debugger::sendMonitor(bool bOnOff, long iId, INXString csType)
{
	INXString csTcpCmd;
	clearAll=TRUE;
	if (bOnOff) {
		csTcpCmd = "=M+" + intToString(iId) + "," + csType + "\n";
	}
	else {
		csTcpCmd = "=M-" + intToString(iId) + "," + csType + "\n";
	}

	if ((tcpClient.SendText(csTcpCmd)) != 0) {
		ExitDebugger();
		AfxMessageBox("Unable to send command over TCPIP connection.");
		tcpClient.DisConnect();
		return false;
	}

	return true;
}

// This method clears all the monitored lines in EHS.
void Debugger::ClearAllMonitors()
{
	INXPOSITION pos;
	ConData* pBlob;
	INXString csTcpStr;
	TypeConversion tc;
	clearAll=TRUE;
	pos = pView->pDEP->condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (pView->pDEP->condata->GetNext(pos));
		for (UINT i=0; i<pBlob->inputport_num; i++) {
			if (pBlob->inputport[i]->line.getDbgMonitor() && pBlob->inputport[i]->line.exist) {
				if ((tcpClient.SendText("=M-" + intToString(pBlob->inputport[i]->line.idNum) + "," +
					tc.DataType2Char(pBlob->inputport[i]->datatype) + "\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
		}
		for (UINT i=0; i<pBlob->startport_num; i++) {
			if (pBlob->startport[i]->line.getDbgMonitor() && pBlob->startport[i]->line.exist) {
				if ((tcpClient.SendText("=M-" + intToString(pBlob->startport[i]->line.idNum) + ",E\n")) != 0) {
					AfxMessageBox("Unable to send command over TCPIP connection.");
					tcpClient.DisConnect();
					return;
				}
			}
		}
	}
}

void Debugger::setWriteRtaTrace(bool bWrite)
{
	m_bWriteRtaTrace = bWrite;
}

bool Debugger::getWriteRtaTrace()
{
	return m_bWriteRtaTrace;
}

void Debugger::ExitDebugger()
{
	clearAll=FALSE;
	pView->pProject->setDefineMonitors(FALSE);
	pView->setStatusBarText("Line Editing");
	m_RtaTraceSupport.setRtaTrace(FALSE);
	m_RtaTraceSupport.terminateRtaTraceProc();
	pView->stopTraceTimer();

	debugMode = DBGSTOP;

}

int Debugger::ReStartInDebug(void)
{
	
	return 0;
}

#endif