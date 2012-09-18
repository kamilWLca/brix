/** @file LucidTcpipClient.cpp 
 * Implements Client Class TCPIP interface 
 * 
 * @author: Kieron White
 * @version: $Revision: 000 $
 * @date: $Date: 2007-03-29 $
 * 
 */
#include "stdafx.h" 
#include "LucidTcpipClient.h"
#include "LucidTcpipClientDlg.h"
#include "TgtTransProgDlog.h"
#include "LucidConstants.h"
#include "LucidRegAccess.h"
#include "assert.h"
#include <string>
#include <stdio.h>



int LucidTcpipClient::s_iPort;
INXString LucidTcpipClient::s_csIpAddress;
long TcpLogger::s_lLogCount = 0;
int TcpLogger::s_iRunOnce = 1;

const char* LucidTcpipClient::szEscape = "\033E"; // represents escape character over TCP/IP
const char* LucidTcpipClient::szDisconnect = "\033D"; // TCP/IP disconnect signal sent over TCP/IP


LucidTcpipClient::LucidTcpipClient(void)
{
	// Enable or disable to run Unit Test
	tests.RunATest();
	//tests.RunTests();
	csTempLineBuffer = "";
	csTempBlockBuffer = "";
	InitTcpipClient();
}

/**
 * Destructor
 *
 * @description:
 * Ensure disconnection takes place as object is destroyed
 *
 * @param: none
 * @return LtsStatusType
 */
LucidTcpipClient::~LucidTcpipClient(void)
{
	this->DisConnect();
}

void LucidTcpipClient::InitTcpipClient()
{
	// Get the IP Address and Port from the registry
	INXString csPort;
	INXString csLocation = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCATION_VALUE),_T((CString)DEFAULT_TARGET_LOCATION));

	if (csLocation == REG_TARGET_LOCATION_DATA_REMOTE) {
		s_csIpAddress = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_IP_VALUE),_T((CString)DEFAULT_TARGET_REMOTE_IP));
		csPort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_REMOTE_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));
	}
	else {
		s_csIpAddress = DEFAULT_TARGET_LOCAL_IP;
		csPort = getLucidRegValue(_T(REG_TARGET_KEY),_T(REG_TARGET_LOCAL_PORT_VALUE),_T((CString)DEFAULT_TARGET_PORT));
	}
	s_iPort = atoi(csPort);
}

/***********************************************************************
 ***		(private) Standard Target TCPIP functions 				 ***
 ***********************************************************************/

/* - moved to init of application, as WSAStartup and WSACleanup should only be called once for lifetime of app
LtsStatusType LucidTcpipClient::InitialiseWinsock()
{	
	LtsStatusType ltsStatusType=LTS_STATUS_OK;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
		ltsStatusType=LTS_STATUS_FAIL;
	}
	return ltsStatusType;
}
*/

LtsStatusType LucidTcpipClient::CreateSocket()
{
	LtsStatusType ltsStatusType=LTS_STATUS_OK;
	s_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (s_Socket == INVALID_SOCKET) {
		int ErrNum = WSAGetLastError();
		ltsStatusType = LTS_STATUS_FAIL; //& LTS_STATUS_PERMANENT;
	}
	s_TempSocket = s_Socket;
	return ltsStatusType;
}

LtsStatusType LucidTcpipClient::ServerConnect()
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	s_Connection.sin_family = AF_INET;
	s_Connection.sin_addr.s_addr = inet_addr( this->s_csIpAddress );
	s_Connection.sin_port = htons( this->s_iPort );
	if (connect( s_Socket, (SOCKADDR*) & s_Connection, sizeof(s_Connection)) == SOCKET_ERROR) {
		int ErrNum = WSAGetLastError();
		ltsStatusType = LTS_STATUS_FAIL;
	}
	return ltsStatusType;
}

LtsStatusType LucidTcpipClient::SetSocketOptions()
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	int timeout = 50;
	int result = setsockopt(s_Socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(timeout));
	if (result == SOCKET_ERROR)
	{
		int ErrNum = WSAGetLastError();		
		ltsStatusType = LTS_STATUS_FAIL;
	}
	return ltsStatusType;
}

/***********************************************************************
 ***		(private) Specialisized Target TCPIP functions      	 ***
 ***********************************************************************/


/**
 * Cater for standard calls from SendText
 */
LtsStatusType LucidTcpipClient::SendData(INXString csSendBuffer)
{
	return SendData((const char *)csSendBuffer,csSendBuffer.GetLength());
}

/**
 * Send outgoing data. Expand all instances of <Esc> to <Esc>E. 
 * If outgoing data contains no <esc>, send it as-is
 * If it contains <esc>, copy everything up to the <esc> into a buffer,
 * put <esc>E into the buffer, then copy up to the next <esc>, ...
 * finally send, and free the buffer. This is extremely efficient where
 * outgoing text doesn't contain escape characters.
 * @test Possible test cases for this are:
 * pData = "T<esc>A<esc><esc><esc>D<esc>", nLength = 8
 * This sends "T<esc>EA<esc>E<esc>E<esc>ED<esc>E", nLen = 13
 *
 * pData = "abcdefgh", nLength = 8
 * This sends "abcdefgh", nLength = 8
 *
 * pData = "abcd<esc>efg", nLength = 8
 * This sends "abcd<esc>Eefg", nLength = 9
 * @param[in] pData pointer to outgoing data
 * @param[in] nLength size of data to send. If it's a null terminated string,
 * remember to add 1 for the null.
 * @return status
 */
LtsStatusType LucidTcpipClient::SendData(const char* pData, int nLength)
{
	/* points to the next occurrence of an <esc> in the input data */
	const char* pNext;
	/* points to the data to send out - initially the same as the input data.
	 * Won't break the const-ness of pData - pAllocatedData is only written
	 * to if it is changed to point to newly allocated memory */
	char* pAllocatedData = const_cast<char*>(pData); 
	/* if string is broken into fragments to allow insertion of <esc>E, this
	 * points to the next place where an insertion will occur */
	char* pInsertPoint = NULL;
	LtsStatusType ret;

	while (pNext = (const char*)memchr(pData,'\033',nLength))
	{
		if (!pInsertPoint)
		{
			/* input data isn't going to be sent untranslated, therefore
			 * allocate space to send it - worst case is 2 * length of input data */
			pAllocatedData = (char*)malloc(2*nLength);
			/* start inserting at start of freshly allocated memory */
			pInsertPoint = pAllocatedData;
		}
		if (pNext > pData)
		{
			/* send only the first part of pData up until the <esc> character */
			int nAmountToSend = (pNext-pData);
			memcpy(pInsertPoint,pData,nAmountToSend);
			pInsertPoint += nAmountToSend;
			nLength -= nAmountToSend;
		}
		/* else: there is nothing to send before the escape */
		nLength--; /* adjustment for the esc char we don't send */
		memcpy(pInsertPoint,LucidTcpipClient::szEscape,strlen(LucidTcpipClient::szEscape));
		pInsertPoint += strlen(LucidTcpipClient::szEscape);
		pData = pNext+1; /* continue at character after the escape */
	}

	if (pInsertPoint)
	{
		if (nLength > 0)
		{
			/* copy the remainder of the input data to the allocated buffer.
			 * This input data is untranslated as it contains no escape chars
			 */
			memcpy(pInsertPoint,pData,nLength);
			pInsertPoint += nLength;
		}
		/* we've had to insert characters, therefore, recalculate nLength */
		nLength = (int)(pInsertPoint - pAllocatedData);
	}
	ret = SendRawData(pAllocatedData, nLength);
	if (pInsertPoint) /* we needed to allocate memory, so free it */
	{
		free(pAllocatedData);
	}

	return ret;
}

/**
 * Send untranslated binary data. <esc> characters are not expanded to
 * <esc>E - this allows us to send the <esc>D message to disconnect.
 */
LtsStatusType LucidTcpipClient::SendRawData(const char *szSendBuffer, int size)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	int result = send(s_Socket, szSendBuffer,size, 0);
	if (result == SOCKET_ERROR)
	{
		int ErrNum = WSAGetLastError();	
		ltsStatusType = LTS_STATUS_FAIL;
	}
	return ltsStatusType;
}

/**
 * Cater for calls made from ReceiveTextLine and ReceiveTextBlock
 */
LtsStatusType LucidTcpipClient::RecvData(INXString &csRecvBuffer)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;

	char *szReceiveBuffer;
	szReceiveBuffer = new char[TCPIP_BUFFER_SIZE];
	assert(szReceiveBuffer != 0);

	memset( szReceiveBuffer, 0, TCPIP_BUFFER_SIZE);	
	int recvBufferLength = TCPIP_BUFFER_SIZE;
	int result = recv( s_Socket, szReceiveBuffer, recvBufferLength, 0);
	assert(recvBufferLength >= result);
	if (result == SOCKET_ERROR)
	{
		int ErrNum = WSAGetLastError();
		if (ErrNum == WSAETIMEDOUT) 
		{
			//ltsStatusType = LTS_STATUS_TIMEOUT & LTS_STATUS_TEMPORARY;
			ltsStatusType = LTS_STATUS_TIMEOUT;
		} 
		else
		{
			ltsStatusType = LTS_STATUS_FAIL;
		}
	}

	if (result>0)
	{
		logger.Log("***RecvData size %d %s\n", result, szReceiveBuffer);		
		csRecvBuffer.Format("%s", szReceiveBuffer);
		long cs_size = csRecvBuffer.GetLength();
		long sz_size = TCPIP_BUFFER_SIZE;	

		assert( cs_size <= sz_size );//ensure nothings lost in INXString copy
	}

	delete szReceiveBuffer;
	return ltsStatusType;

}

/***********************************************************************
 ***		(public) Specialisized Target TCPIP functions      	 ***
 ***********************************************************************/

/**
 * Lucid Target Client Connect
 *
 * @description:
 * This function is called for client to create connection to the server.  
 * It should only be called once, after connection finsihed call DisConnect 
 *
 * @param: none
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::Connect()
{
	LtsStatusType ltsStatusType=LTS_STATUS_OK;
//	ltsStatusType = InitialiseWinsock();
	if (ltsStatusType == 0) ltsStatusType = CreateSocket();
	if (ltsStatusType == 0) ltsStatusType = ServerConnect();
	if (ltsStatusType == 0) ltsStatusType = SetSocketOptions();
	return ltsStatusType;
}

LtsStatusType LucidTcpipClient::DisConnect()
{
	this->SendRawData(LucidTcpipClient::szDisconnect,strlen(LucidTcpipClient::szDisconnect));
	Sleep(200);
	LtsStatusType ltsStatusType = LTS_STATUS_OK;

	// recommended pattern for gracefull socket closing:
	// 1. call shutdown for send operations
	// 2. call recv until function completes with success and indicates 0 bytes received, if SOCKET_ERROR was received, no gracefull disconnection possible
	shutdown(s_Socket, 1); // shutdown send operations
	//loop 10 times and call receive every 100ms, if recv returns error or 0 bytes received, exit loop and proceed to closesocket
	int ret;
	int size = 1000;
	char *szRecvBuffer = new char[size];
	for (UINT i=1; i < 10; i++) {
		Sleep(100);
		ret = recv(s_Socket, szRecvBuffer, size, 0);

		if (ret == SOCKET_ERROR) break; // non-gracefull shutdown
		if (ret == 0) break;	// gracefull shutdown
	}
	

	closesocket(s_Socket);
	s_TempSocket = s_Socket;
	return ltsStatusType;
}

/**
 * Lucid Target Client SendFileContents
 *
 * @description:
 * Please call the following before calling this: SendText("L myfile.sdl 1052\n");
 * Then we send contents of a file to server
 *
 * @param:
 * - csFilePath � full file path of file to be sent to server
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::SendFileContents( INXString csFilePath, CTgtTransProgDlog *pDlog )
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	char pszFullContent[65535];	

	INXString csFullContent;
	int iFileSize = this->FileSize(csFilePath);
	int iSizeRemaining = iFileSize;
	const int arraysize = sizeof(pszFullContent);

	ifstream fin(csFilePath, ios::binary|ios::in);	
	if (!fin) {
		//ltsStatusType = LTS_FILE_OPEN_FAILED & LTS_STATUS_PERMANENT;
		ltsStatusType = LTS_FILE_OPEN_FAILED;
		return ltsStatusType;
	}

	if(fin.is_open())
	{			
		while ( (iSizeRemaining > 0)  &&  ( !pDlog->m_bCancelled    ) ) {

			memset(pszFullContent, 0, arraysize);	

			if (iSizeRemaining >= arraysize)
			{ //break it up if too large for array

				fin.read(pszFullContent, arraysize);
				this->SendText(pszFullContent, arraysize);


			} else { //send normally

				fin.read( pszFullContent, iSizeRemaining );
				this->SendText( pszFullContent, iSizeRemaining );
			}
				
			iSizeRemaining -= arraysize;

			int iDum = (int) (100.0 * ( 1.0 - (float( iSizeRemaining) ) / iFileSize) );
			pDlog->setProgbarFile( iDum );
		}		
	}

	fin.close();
	TRACE("Tcpip SendFileContents %s\n", csFilePath);
	return ltsStatusType;
}

/**
 * Lucid Target Client SendFileContents
 *
 * @description:
 * Please call the following before calling this: SendText("L myfile.sdl 1052\n");
 * Then we send contents of a file to server
 *
 * @param:
 * - csFilePath � full file path of file to be sent to server
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::SendFileContents(INXString csFilePath)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	char szFullContent[65535];	

	INXString csFullContent;
	int filesize = this->FileSize(csFilePath);
	const int arraysize = sizeof(szFullContent);

	ifstream fin(csFilePath, ios::binary|ios::in);	
	if (!fin) {
		//ltsStatusType = LTS_FILE_OPEN_FAILED & LTS_STATUS_PERMANENT;
		ltsStatusType = LTS_FILE_OPEN_FAILED;
		return ltsStatusType;
	}

	if(fin.is_open())
	{			
		while (filesize > 0) {
			memset(szFullContent, 0, arraysize);	

			if (filesize >= arraysize)
			{ //break it up if too large for array
				fin.read(szFullContent, arraysize);
				this->SendText(szFullContent, arraysize);
			}
			else
			{ //send normally
				fin.read(szFullContent, filesize);
				this->SendText(szFullContent, filesize);
			}
			filesize -= arraysize;
		}		
	}

	fin.close();
	TRACE("Tcpip SendFileContents %s\n", csFilePath);
	return ltsStatusType;
}

/**
 * Lucid Target Client SendText
 *
 * @description:
 * Sends text to server used for sending text from the higher level.
 * The character <esc> is always substituted with <esc>E.
 *
 * @param:
 * - csCommand � text to send to server
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::SendText(INXString csText)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	ltsStatusType = this->SendData(csText);
	logger.Log("SendText %s", csText.GetBuffer(csText.GetLength()));
	return ltsStatusType;
}

/**
 * Handle binary data properly
 *
 * Lucid Target Client SendText(char array, size of array)
 *
 * @description:
 * Used by FileContentsSend
 *
 * @param:
 * - szText � text to send to server
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::SendText(char *szText, int size)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	ltsStatusType = this->SendData(szText, size);
	logger.Log("***SendText size %d: Possibly binary data %s\n", size, szText);
	return ltsStatusType;
}

/**
 * Lucid Target Client ReceiveTextLine
 * 
 * @description:
 * This function is needed to view any data sent from the server to a client
 * It reads a line of text
 *
 * @output param:
 * - szReceiveText
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::ReceiveTextLine(INXString &csReceiveText)
{
	LtsStatusType retVal = LTS_STATUS_OK;
	//csReceiveText = "";
	LtsStatusType ltsStatusType = this->RecvData(csReceiveText);

	if(retVal != LTS_STATUS_OK)

		retVal = ltsStatusType;
	else{

		//retVal = LTS_STATUS_TIMEOUT;


		// SDG: store messages not terminated by a newline char, and concatenate them to the next 
		// received buffer
		csReceiveText = csTempLineBuffer + csReceiveText;

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Split-up the received text into new-line terminated pieces.
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		//1st of all, chop-off any \n's at the beginning of the string, as these are effectively 0-length
		//lines, which are not worth processing further.

		int charPos = csReceiveText.Find('\n');
		int len = csReceiveText.GetLength();
		while(charPos == 0){
			csReceiveText = csReceiveText.Right(len-1);
			charPos = csReceiveText.Find('\n');
			len = csReceiveText.GetLength();
		}


		charPos = csReceiveText.Find('\n');
		len = csReceiveText.GetLength();

		if ( charPos == -1 ) {
			// There is no '\n' at all.
			csTempLineBuffer = csReceiveText;
			csReceiveText = "";
			retVal = LTS_STATUS_OK;
		} 
		else {
			// There is at least 1 '\n' present ...
			if ( charPos!=len-1 ) {
				// the \n is in the middle somewhere - get the left-most substring
				csTempLineBuffer = csReceiveText.Right(csReceiveText.GetLength() - 1 - charPos);
				csReceiveText = csReceiveText.Left( charPos );
				retVal = LTS_STATUS_TEMPORARY;
			}
			else {
				//There is just a single '\n' at the end of the string
				csTempLineBuffer = "";
				retVal = LTS_STATUS_OK;
			}
		} 		

		return retVal;

	}
}

/**
 * Lucid Target Client ReceiveTextBlock
 * 
 * @description:
 * This function is needed to view any data sent from the server to a client 
 * It reads a block of text
 *
 * @output param:
 * - szReceiveText
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::ReceiveTextBlock(
		INXString &csReceiveText, 
		INXString &csDelimToken, 
		const bool &bGetFirstBlockOnly )
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	csReceiveText = "";
	ltsStatusType = this->RecvData(csReceiveText);	

	csReceiveText = csTempBlockBuffer + csReceiveText;
	int charPos;

	if( bGetFirstBlockOnly ){

		charPos = csReceiveText.Find( csDelimToken );
		int i=0;

	}else {

		int lastPos = csReceiveText.Find( csDelimToken, 0 );

		if( lastPos != -1){

			//int subStrLen = csDelimToken.GetLength();

			while( lastPos != -1 ){
				charPos = lastPos;
				lastPos = csReceiveText.Find( csDelimToken, lastPos+1 ); // find next occurrence of substring
			}

		} // if( lastPos != -1)
		else{
			charPos = lastPos;
		}

	}

	if (charPos != -1) {
		int len = csReceiveText.GetLength();
		csTempBlockBuffer	= csReceiveText.Right(len - charPos - csDelimToken.GetLength() );
		csReceiveText		= csReceiveText.Left(charPos + csDelimToken.GetLength() );

	} else {
		csTempBlockBuffer = csReceiveText;
		csReceiveText = "";
	}

	return ltsStatusType;
}


/**
 * Lucid Target Client ReceiveTextBlock
 * 
 * @description:
 * This function is needed to view any data sent from the server to a client 
 * It reads a block of text
 *
 * @output param:
 * - szReceiveText
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::ReceiveTextBlock(INXString &csReceiveText)
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	csReceiveText = "";
	ltsStatusType = this->RecvData(csReceiveText);	

	csReceiveText = csTempBlockBuffer + csReceiveText;
	int charPos = csReceiveText.ReverseFind('\n');
	if (charPos != -1) {
		int len = csReceiveText.GetLength();
		csTempBlockBuffer = csReceiveText.Right(len - 1 - charPos);
		csReceiveText = csReceiveText.Left(charPos);
	}
	else {
		csTempBlockBuffer = csReceiveText;
		csReceiveText = "";
	}

	return ltsStatusType;
}

/**
 * Used for dialog interface:
 *	ConfigDlg() 
 *	GetPort()
 *	GetIPAddress()
 *	SetPort(int iPort)
 *	SetIPAddress(INXString csIpAddress)
 */
void LucidTcpipClient::ConfigDlg()
{
	CLucidTcpipClientDlg dlg;
	//dlg.SetPort(this->GetPort());
	//dlg.SetIpAddress(this->GetIPAddress());
	if (dlg.DoModal()==IDOK)
	{
		this->SetPort(dlg.GetPort());
		this->SetIPAddress(dlg.GetIpAddress());
	}
}

//Used for dialog interface
int LucidTcpipClient::GetPort()
{ 	return this->s_iPort;
}

INXString LucidTcpipClient::GetIPAddress()
{ 	return this->s_csIpAddress;
}

void LucidTcpipClient::SetPort(int iPort)
{	s_iPort = iPort;
}

void LucidTcpipClient::SetIPAddress(INXString csIpAddress)
{	s_csIpAddress = csIpAddress;
}

/**
 *	Helper functions:
 *  FileSize()	Obtaining the file size
 *	SendFile()  Makes it easier for user to pass file to be sent and target file name 
 */
int LucidTcpipClient::FileSize(INXString csFilePath)
{
	long begin,end;
	ifstream myfile(csFilePath,ios::in);	
	if(!myfile) return -1;
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();
	return (end-begin);
}

LtsStatusType LucidTcpipClient::WaitForServer( INXString &csReceive )
{
	//Recv timeout 50ms. We will wait 500ms (10*50)
	int timeout = 10; 

	LtsStatusType ltsStatus = LTS_STATUS_OK;

	do {

		ltsStatus = this->ReceiveTextLine(csReceive);
		
		//if (csReceive != "") {
		if( csReceive.Left(2) == "> " ) {

			TRACE("WaitForServer >\n");
			return ltsStatus;

		}else if( csReceive.Left(5) == "**Err") {

			TRACE("Error >\n");
			return ltsStatus;

		} else if (csReceive != "") {

			logger.Log(csReceive);
			//return LTS_STATUS_OK;
		}

	} while(timeout--);

	logger.Log("Connection to server timed out\n");
	return LTS_STATUS_TIMEOUT;
}

/**
 * Lucid Target Client SendFile 
 *
 * @description:
 * Replaces SentText and old SendFile see SendFileContents
 *
 * @param:
 * - csFilePath � full file path of file to be sent to server
 *
 * @return LtsStatusType
 */
LtsStatusType LucidTcpipClient::SendFile(INXString csFilePath, INXString csTargetFileName )
{
	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	INXString command;
	command.Format("L %s %d\n", csTargetFileName, this->FileSize(csFilePath));	
	if (strstr((CString)csTargetFileName, "\\")) 
	{
		AfxMessageBox("SendFile: Target file name without path expected");
		return LTS_STATUS_FAIL;
	}
	if ( (ltsStatusType = this->SendText(command)) != 0)
	{
		AfxMessageBox("Unable to send command over TCPIP connection.");
		this->DisConnect();
		return ltsStatusType;
	}
	if ( (ltsStatusType = this->SendFileContents(csFilePath)) !=0)
	{
		AfxMessageBox("Failed to transfer data file " + csFilePath + ". Stopping application transfer.");
		this->DisConnect();
		return ltsStatusType;
	}
	return ltsStatusType;
}
/**
 * Lucid Target Client SendFile 
 *
 * @description:
 * Replaces SentText and old SendFile see SendFileContents
 *
 * @param:
 * - csFilePath � full file path of file to be sent to server
 *
 * @return LtsStatusType
 */

LtsStatusType LucidTcpipClient::SendFile(INXString csFilePath, INXString csTargetFileName, CTgtTransProgDlog *pDlog )
{
	pDlog->setProgbarFile(0);

	LtsStatusType ltsStatusType = LTS_STATUS_OK;
	INXString command;
	command.Format("L %s %d\n", csTargetFileName, this->FileSize(csFilePath));	
	if (strstr((CString)csTargetFileName, "\\")) 
	{
		AfxMessageBox("SendFile: Target file name without path expected");
		return LTS_STATUS_FAIL;
	}
	if ( (ltsStatusType = this->SendText(command)) != 0)
	{
		AfxMessageBox("Unable to send command over TCPIP connection.");
		this->DisConnect();
		return ltsStatusType;
	}
	if ( (ltsStatusType = this->SendFileContents(csFilePath, pDlog)) !=0)
	{
		AfxMessageBox("Failed to transfer data file " + csFilePath + ". Stopping application transfer.");
		this->DisConnect();
		return ltsStatusType;
	}
	return ltsStatusType;
}

/**********************************************************************
 * Tcp Logger Class
 *
 * @description:
 * This class is used to log all outbound tcp traffic for diagnostics
 * This class is used to log an error conditions
 * This class is used to send any logged messages to the output debug window
 *
 */
TcpLogger::TcpLogger()
{
#ifdef TCP_LOGGER
	strcpy(szFilename, "client_tcp.log");
	if(s_iRunOnce) {
		LogClear();
		s_iRunOnce = 0;
	}
#endif
}

void TcpLogger::LogAddMessage(INXString csLog)
{
#ifdef TCP_LOGGER
	FILE *fp;
	if((fp = fopen(szFilename,"a+")) == NULL) //a+ = append read/write text
	{
		if((fp = fopen(szFilename,"w")) == NULL) //w create it
		{
			//AfxMessageBox("TcpClient Log file could not be created");
		}else{
			fputs(csLog.GetBuffer(csLog.GetLength()), fp);
			fclose(fp);
		}
	}
	else
	{		
		fputs(csLog.GetBuffer(csLog.GetLength()),fp);
		fclose(fp);
	}
#endif
}

void TcpLogger::LogRemoveLinesFromFrontOfFile(int number_of_lines_to_remove)
{
	//Function renames log.dat to .tmp, then we recreate .dat file without the first line
#ifdef TCP_LOGGER

	FILE *from,*to;
	char szFilenameTemp[]="client_tcp.tmp";
	rename(szFilename, szFilenameTemp);	
	char ch=0;
	if((from=fopen(szFilenameTemp,"r"))==NULL) { //r = read text file
		//AfxMessageBox("LogRemoveFirstLine: could not open client tcp log file\n");
	}
	else
	{
		if((to=fopen(szFilename,"w"))==NULL) //w = write text file
			AfxMessageBox("LogRemoveFirstLine: could not create tcp log file\n");
		else
		{
			//ignore number_of_lines_to_remove
			while(number_of_lines_to_remove-- > 0) 
			{
				int giveup_no_newlines = 10000;
				while((ch = fgetc(from)) != 10)
				{
					//Just in case newlines not added by mistake cater for this.
					if(giveup_no_newlines-- <= 0) {
						number_of_lines_to_remove=0;
						break;										
					}
				}
			}
			//write rest of file
			while((ch = fgetc(from)) != EOF) fputc(ch, to);
			fclose(from);
			fclose(to);
		}		
	}
	remove(szFilenameTemp); 
#endif
}

void TcpLogger::LogClear()
{
#ifdef TCP_LOGGER
	FILE *fp;
	fp=fopen(szFilename,"w");
	fclose(fp);
#endif
}

long TcpLogger::LogNumberOfLines()
{
#ifdef TCP_LOGGER
	FILE *fp;
	char ch=0;
	long count=0L;
	
	if((fp=fopen(szFilename,"r"))==NULL) //r = read text file
		AfxMessageBox("LogNumberOfLines: could not open\n");
	else
	{
		while((ch = fgetc(fp)) != EOF)
		{
			if (ch==10)count++; //newline
		}
		fclose(fp);
	}
	return count;
#else
	return 0;
#endif
}

/**
 * Tcp Logger Log
 *
 * @description:
 * Main entry for adding messeages to the log file
 *
 * Example usage logger.Log("test %s\n", var1);
 * Please ensure you always add a \n. Unless explicitly not required.
 *
 * @param: any string arguments
 *
 * @return Current Log count (needed for verification in test suite)
 */
long TcpLogger::Log(const char* pszFormat, ...)
{
#ifdef TCP_LOGGER

	va_list args;
    int     len;
    char    *buffer;

    // retrieve the variable arguments
    va_start( args, pszFormat );
    len = _vscprintf( pszFormat, args ) // _vscprintf doesn't count
                                + 1; // terminating '\0'

	buffer = (char*)malloc( len * sizeof(char) );
    vsprintf( buffer, pszFormat, args ); // C4996
    
	// Append timestamp
	CTime tNow;
	INXString csTime;

	tNow = CTime::GetCurrentTime();
	csTime = tNow.Format( "%d/%m/%y %H:%M:%S ");

    INXString csBuffer;
	csBuffer.Format("%s%s", csTime, buffer);
    // Note: vsprintf is deprecated; consider using vsprintf_s instead

	//if( strstr(csBuffer, "\n")==NULL )
	//{
	//	TRACE("Warning: unless otherwise required a newline '\\n' is expected in following\n");
	//}
	TRACE("%s", csBuffer);					// Show to debug output window

	/* @todo do we want to make logging an option? The following function is implemented if so.
	 *
	 LogAddMessage(csBuffer);			// Add to file
	*/

	// Remove excess in log file 
	s_lLogCount++;
	if(s_lLogCount > 500)
	{
		long lRemove = 100;
		LogRemoveLinesFromFrontOfFile(lRemove);
		s_lLogCount = s_lLogCount - lRemove;
	}
	free( buffer );
	return s_lLogCount;
#else
	return 0;
#endif
}

/**********************************************************************
 * Tcp Test Suite Class
 *
 * @description:
 * So far this consists of testing logger to ensure robustness
 *
 */
void TcpTestSuite::RunTests()
{
	AfxMessageBox("All Tcp Tests about to start", MB_ICONINFORMATION);
	LoggerTest1();
	Sleep(500);
	LoggerTest2();
	Sleep(500);
	LoggerTest3();
	Sleep(500);
	AfxMessageBox("All Tcp Tests finished", MB_ICONINFORMATION);
}

void TcpTestSuite::RunATest()
{
	logger.Log("Logger started\n");	
}

void TcpTestSuite::LoggerTest1()
{
	//Test1. force logger to create, append and remove lines
#ifdef TCP_LOGGER

	int Number_Of_Lines_To_Create = 500;
	char test[]={
		"1 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 2 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 3 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 4 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 5 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 6 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 7 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 8 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 9 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 0 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 11 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 12 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890 \
		 13 abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890\0"};
	for(int x=0; x<Number_Of_Lines_To_Create;x++) 
	{
		logger.Log("Row %d: %s\n", x, test);
		Sleep(10);
	}

	// Check number of lines reduces as expected
	long test1 = logger.Log("test1\n");
	long verify_test1 = logger.LogNumberOfLines();
	assert(test1 == verify_test1);
#endif
}

void TcpTestSuite::LoggerTest2()
{
#ifdef TCP_LOGGER

	//Test2. force logger to create, append and remove lines
	int Number_Of_Lines_To_Create = 500;
	int test[]={	1,0,1,2,3,4,5,6,7,8,9,0,43,44,45,46,47,48,49,50, \
					2,0,1,2,3,4,5,6,7,8,9,0,43,44,45,46,47,48,49,50}; 


	for(int x=0; x<Number_Of_Lines_To_Create;x++) {
		logger.Log("%s possible binary data! Size %d\n", test, x);		
		Sleep(10);
	}

	// Check number of lines reduces as expected
	long test2 = logger.Log("test2\n");
	long verify_test2 = logger.LogNumberOfLines();
	assert(test2 == verify_test2);
#endif
}

void TcpTestSuite::LoggerTest3()
{
#ifdef TCP_LOGGER

	//Test3. force logger to create, append and remove lines
	int Number_Of_Lines_To_Create = 500;
	char test[]={
		"abcdefghijklmnopqurstxyz1234567890abcdefghijklmnopqurstxyz1234567890\0"};
		 
	for(int x=0; x<Number_Of_Lines_To_Create;x++) {
		logger.Log("Row %d:%s\n", x, test);
		Sleep(10);
	}
	// Check number of lines reduces as expected
	long test3 = logger.Log("test3\n");
	long verify_test3 = logger.LogNumberOfLines();
	assert(test3 == verify_test3);
#endif
}
