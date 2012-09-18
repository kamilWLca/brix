#ifndef _INXSTRING_H
#define _INXSTRING_H

#pragma once
#include <afx.h> /* @todo temporarly include of mfc lib*/
#include <wx/string.h>
#include <algorithm>

using namespace std;

#define LEGACYINX

class INXString: public wxString{

#ifdef LEGACYINX
	CString _string;
#endif
public:
	/*@todo Legacy code needed to compile code with mfc functions*/
	operator CString&(){	
		return _string;
	};
	operator CString(){	
		CString str = (char *)c_str();
		return str;
	};
	operator char *()const{
		char *_cstr = (char *)c_str();
		return _cstr;
	};
	
public:
	// constructors
	INXString(): wxString(){
	};
	INXString(wxString &str):wxString(str){
		#ifdef LEGACYINX
			_string = str.c_str();
		#endif
	};
	INXString(const char *s): wxString(s){
		#ifdef LEGACYINX
			_string = s;
		#endif		
	};
	//copy constructor
	INXString(const char &c){
		#ifdef LEGACYINX
			_string = c;
		#endif
		append(c);
	};
	INXString(const CString &str){
		#ifdef LEGACYINX
			_string = str;
		#endif
		append(str);
	};	
	void AppendChar(char c){
		Append((wxChar)c);
		#ifdef LEGACYINX
			_string.AppendChar(c);
		#endif
	};
	void Delete(unsigned int _start, unsigned int _end){
		erase(_start,_end);
		#ifdef LEGACYINX
			_string = c_str();
		#endif
	};
	size_t GetLength() const{
		return Len();	
	};
//	bool IsEmpty(){} // is inherited
//	void Empty(){} // is inherited
	char GetAt(unsigned int index) const{
		return GetChar(index);
	};
	void SetAt(unsigned int index, char c){
		SetChar(index, c);
		#ifdef LEGACYINX
			_string = c_str();
		#endif
	};
	// Comparision
	int CompareNoCase(const char *s){ // case insensetive
		return  CmpNoCase(s);
	};
	// Other Conversions 
//	void MakeUpper(){} inherits from parent
//	void MakeLower(){} inherits from parent
	void MakeReverse(){
		reverse(begin(), end());
		#ifdef LEGACYINX
			_string = c_str();
		#endif
	};
/*	void Format(char *s, ... ){} use Printf(char *s, ...) instead 
	that is inherited from parent */
	INXString TrimLeft(){
		 Trim(false);
		 #ifdef LEGACYINX
			_string = c_str();
		#endif
			return (INXString)c_str();
	};
	INXString TrimRight(){
		 Trim(true);
		 #ifdef LEGACYINX
			_string = c_str();
		#endif
			return (INXString)c_str();
	};
	// Searching 
	int Find(const unsigned char c){
		return (int)wxString::Find((wxChar)c);
	}; 
	int Find(const char c){
		return (int)wxString::Find((wxChar)c); 
	}; 
	int Find(const char *s){
		return (int)wxString::Find((wxChar *)s);
	};
	int Find(INXString str){
		return (int)wxString::Find((wxChar)(char *)str);
	};
	int Find(INXString str, int nStart){
		return (int)find((char *)str, nStart);
	};
	int Find(unsigned char c, int nStart){
		return (int)find(c, nStart);
	};
	int Find(char *s, int nStart){
		return (int) find(s, nStart);
	};
	int ReverseFind(char c){
		return (int) find_last_of(c, size() - 1);		
	};
	int FindOneOf(char *s){
		return (int) find_first_of(s); 
	};
	//	Buffer Access 
	char* GetBuffer(const int nMinBufLength){
		return GetWriteBuf(nMinBufLength);
	};
	void ReleaseBuffer(int nNewLength = -1){
		if(nNewLength == -1){
			nNewLength = StringLength((char *)c_str());
		}
		SetLength(nNewLength);
		#ifdef LEGACYINX
			_string = c_str();
		#endif
	};
	void SetLength(int _length){
		resize(_length);
	};
	int StringLength(const char *s){
		if(s == NULL){
			return(0);
		}return(int(strlen(s)));
	};
	/* @todo using mfc CString to creatre the method */
	INXString Tokenize(char* str, int pos){
		CString _str;
		_str = _str.Tokenize(str, pos);
		return (INXString)_str;
	};
	// Extraction 
	/* @todo using mfc CString to creatre the method*/
	INXString SpanExcluding(const char* str){
		CString _str = c_str();
		_str = _str.SpanExcluding(str);
		return (INXString)_str;
	};
};
#endif