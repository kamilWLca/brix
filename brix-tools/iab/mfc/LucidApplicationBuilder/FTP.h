// FTP.h: interface for the FTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTP_H__CA923845_B70C_4564_8D23_FA4EEB745A61__INCLUDED_)
#define AFX_FTP_H__CA923845_B70C_4564_8D23_FA4EEB745A61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxinet.h>
#include "Porting_Classes/INXObject.h"
class FTP : public INXObject  
{
public:
	FTP();
	virtual ~FTP();

	// methods
	CFtpConnection* ftpConnect(LPCSTR server, LPCSTR username, LPCSTR password);
	int ftpPut(LPCSTR localFile, LPCSTR remoteFile, CFtpConnection* pConn, int transferType);
	int ftpGet(LPCSTR remoteFile, LPCSTR localFile, CFtpConnection* pConn);
	int ftpSetServerDir(LPCSTR serverDir, CFtpConnection* pConn);
	void ftpClose();
	void ftpOpen();
	int ftpRename(LPCSTR existingFile, LPCSTR newFile, CFtpConnection* pConn);

	// attributes
	CInternetSession* session;
};

#endif // !defined(AFX_FTP_H__CA923845_B70C_4564_8D23_FA4EEB745A61__INCLUDED_)
