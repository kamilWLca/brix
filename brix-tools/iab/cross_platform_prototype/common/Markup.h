// Markup.h: interface for the CMarkup class.
//
// CMarkup Release 6.5 Lite
// Copyright (C) 1999-2003 First Objective Software, Inc. All rights reserved
// This entire notice must be retained in this source code
// Redistributing this source code requires written permission
// This software is provided "as is", with no warranty.
// Latest fixes enhancements and documentation at www.firstobject.com

#if !defined(_MARKUP_H_INCLUDED_)
#define _MARKUP_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
*/

#ifdef _DEBUG
#define _DS(i) (i?&((char*)m_csDoc)[m_aPos[i].nStartL]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjArray.h"

class CMarkup  
{
public:
	CMarkup() { SetDoc( NULL ); };
	CMarkup( char* szDoc ) { SetDoc( szDoc ); };
	CMarkup( const CMarkup& markup ) { *this = markup; };
	void operator=( const CMarkup& markup );
	virtual ~CMarkup() {};

	// Create
	INXString GetDoc() const { return m_csDoc; };
	bool AddElem( char* szName, char* szData=NULL ) { return x_AddElem(szName,szData,false,false); };
	bool AddChildElem( char* szName, char* szData=NULL ) { return x_AddElem(szName,szData,false,true); };
	bool AddAttrib( char* szAttrib, char* szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool AddChildAttrib( char* szAttrib, char* szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool SetAttrib( char* szAttrib, char* szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool SetChildAttrib( char* szAttrib, char* szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };

	// Navigate
	bool SetDoc( char* szDoc );
	bool IsWellFormed();
	bool FindElem( char* szName=NULL );
	bool FindChildElem( char* szName=NULL );
	bool IntoElem();
	bool OutOfElem();
	void ResetChildPos() { x_SetPos(m_iPosParent,m_iPos,0); };
	void ResetMainPos() { x_SetPos(m_iPosParent,0,0); };
	void ResetPos() { x_SetPos(0,0,0); };
	INXString GetTagName() const;
	INXString GetChildTagName() const { return x_GetTagName(m_iPosChild); };
	INXString GetData() const { return x_GetData(m_iPos); };
	INXString GetChildData() const { return x_GetData(m_iPosChild); };
	INXString GetAttrib( char* szAttrib ) const { return x_GetAttrib(m_iPos,szAttrib); };
	INXString GetChildAttrib( char* szAttrib ) const { return x_GetAttrib(m_iPosChild,szAttrib); };
	INXString GetError() const { return m_csError; };

	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
	};

protected:

#ifdef _DEBUG
	char* m_pMainDS;
	char* m_pChildDS;
#endif

	INXString m_csDoc;
	INXString m_csError;

	struct ElemPos
	{
		ElemPos() { Clear(); };
		ElemPos( const ElemPos& pos ) { *this = pos; };
		bool IsEmptyElement() const { return (nStartR == nEndL + 1); };
		void Clear()
		{
			nStartL=0; nStartR=0; nEndL=0; nEndR=0; nReserved=0;
			iElemParent=0; iElemChild=0; iElemNext=0;
		};
		void AdjustStart( int n ) { nStartL+=n; nStartR+=n; };
		void AdjustEnd( int n ) { nEndL+=n; nEndR+=n; };
		int nStartL;
		int nStartR;
		int nEndL;
		int nEndR;
		int nReserved;
		int iElemParent;
		int iElemChild;
		int iElemNext;
	};

	INXObjArray< ElemPos> m_aPos;
	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_nNodeType;

	struct TokenPos
	{
		TokenPos( char* sz ) { Clear(); szDoc = sz; };
		bool IsValid() const { return (nL <= nR); };
		void Clear() { nL=0; nR=-1; nNext=0; bIsString=false; };
		bool Match( char* szName )
		{
			int nLen = nR - nL + 1;
			return ( (strncmp( &szDoc[nL], szName, nLen ) == 0)
				&& ( szName[nLen] == _T('\0') || strchr(" =/[",szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		char* szDoc;
		bool bIsString;
	};

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};

	int x_GetFreePos();
	int x_ReleasePos();
	int x_ParseElem( int iPos );
	int x_ParseError( char* szError, char* szName = NULL );
	static bool x_FindChar( char* szDoc, int& nChar, char c );
	static bool x_FindAny( char* szDoc, int& nChar );
	static bool x_FindToken( TokenPos& token );
	INXString x_GetToken( const TokenPos& token ) const;
	int x_FindElem( int iPosParent, int iPos, char* szPath );
	INXString x_GetTagName( int iPos ) const;
	INXString x_GetData( int iPos ) const;
	INXString x_GetAttrib( int iPos, char* szAttrib ) const;
	bool x_AddElem( char* szName, char* szValue, bool bInsert, bool bAddChild );
	bool x_FindAttrib( TokenPos& token, char* szAttrib=NULL ) const;
	bool x_SetAttrib( int iPos, char* szAttrib, char* szValue );
	void x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags );
	int x_ParseNode( TokenPos& token );
	void x_DocChange( int nLeft, int nReplace, const INXString& csInsert );
	void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
	INXString x_TextToDoc( char* szText, bool bAttrib = false ) const;
	INXString x_TextFromDoc( int nLeft, int nRight ) const;
};

#endif // !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
