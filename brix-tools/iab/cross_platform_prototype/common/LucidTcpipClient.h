/** @file LucidTcpipClient.h 
 * Implements Client Class TCPIP interface 
 * 
 * @author: Kieron White
 * @version: $Revision: 000 $
 * @date: $Date: 2007-03-29 $
 */
#pragma once
#include <fstream>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include "LucidEnums.h"
#include "LucidConstants.h"
#include "Porting_Classes/INXString.h"

#ifndef LUCIDTCPIPCLIENT_H
#define LUCIDTCPIPCLIENT_H

class TcpLogger
{
	char szFilename[256];	
	void LogAddMessage(INXString csLog);
	void LogRemoveLinesFromFrontOfFile(int number_of_lines_to_remove);
	void LogClear();
public:
	static long s_lLogCount;
	static int s_iRunOnce;
	TcpLogger();
	long Log(const char* pszFormat, ...);
	long LogNumberOfLines();
};

class TcpTestSuite
{
	TcpLogger logger;
	void LoggerTest1();
	void LoggerTest2();
	void LoggerTest3();
public:
	void RunTests();
	void RunATest();
};

class CTgtTransProgDlog;

class LucidTcpipClient
{
private:
	SOCKET s_Socket;
	SOCKET s_TempSocket; //needed by AcceptSocket
	SOCKADDR_IN s_Connection;
	TcpLogger logger;
	TcpTestSuite tests;
	LtsStatusType SendText(char *szText, int size); //handles binary data
	LtsStatusType SendData(INXString csSendBuffer);
	LtsStatusType SendData(const char *szSendBuffer, int size);
	LtsStatusType SendRawData(const char *szSendBuffer, int size);
	LtsStatusType RecvData(INXString &csRecvBuffer);
	LtsStatusType InitialiseWinsock();
	LtsStatusType CreateSocket();
	LtsStatusType ServerConnect();
	LtsStatusType SetSocketOptions();
	INXString csTempLineBuffer;
	INXString csTempBlockBuffer;
	static const char* szEscape; // represents escape character over TCP/IP
	static const char* szDisconnect; // TCP/IP disconnect signal sent over TCP/IP
public:	
	static int s_iPort;	
	static INXString s_csIpAddress;	
	LucidTcpipClient(void);
	~LucidTcpipClient(void);
	LtsStatusType Connect();
	LtsStatusType DisConnect();
	LtsStatusType SendFileContents(INXString csFilePath);
	LtsStatusType SendFileContents(INXString csFilePath, CTgtTransProgDlog *pDlog );
	
	LtsStatusType SendText(INXString csText);	
	LtsStatusType ReceiveTextLine(INXString &csReceiveText); // receives a line of text 
	LtsStatusType ReceiveTextBlock(INXString &csReceiveText); // receives a block of text i.e. includes line breaks


	//! Receives a delimited block of text i.e. includes line breaks - either to leftmost or rightmost limiter
	/*!
	\param csReceiveText Get's loaded with the text block
	\param csDelimToken The delimiter between blocks
	\param bGetLeftBlock If true, get only leftmost block.  Otherwise get all *complete* blocks.
	*/
	LtsStatusType ReceiveTextBlock( 
		INXString &csReceiveText, 
		INXString &csDelimToken,
		const bool &bGetLeftBlock ); 

	void ConfigDlg();
	void GetLastError();

	//Used for config dialog interface
	int GetPort();
	INXString GetIPAddress();
	void SetPort(int iPort);
	void SetIPAddress(INXString csIpAddress);

	//Helper functions
	int FileSize(INXString csFullPath);
	LtsStatusType WaitForServer(INXString &csReceive);

	LtsStatusType SendFile(INXString csFilePath, INXString csTargetFileName );
	LtsStatusType SendFile(INXString csFilePath, INXString csTargetFileName, CTgtTransProgDlog *pDlog );

	void InitTcpipClient();
};
#endif