#ifndef _INXSTRING_H
#define _INXSTRING_H

#include <wx/string.h>
#include <algorithm>
using namespace std;

class INXString: public wxString{

public:
	operator char*(){
		return (char *)c_str();
	} 

public:
	// constructors
	INXString(): wxString(){
	};
	INXString(wxString &str):wxString(str){
	};
	INXString(const char *s): wxString(s){	
	};
	//copy constructor
	INXString(const char &c){
		append(c);
	};

	void AppendChar(char c){
		Append((wxChar)c);
	};
	void Delete(unsigned int _start, unsigned int _end){
		erase(_start,_end);
	};
	int GetLength() const{
		return (int)Len();	
	};
//	bool IsEmpty(){} // is inherited
//	void Empty(){} // is inherited
	char GetAt(unsigned int index) const{
		return GetChar(index);
	};
	void SetAt(unsigned int index, char c){
		SetChar(index, c);
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
	};
/*	void Format(char *s, ... ){} use Printf(char *s, ...) instead 
	that is inherited from parent */
	INXString TrimLeft(){
		 Trim(false);
		return (INXString)c_str();
	};
	INXString TrimRight(){
		Trim(true);
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
		return (int)wxString::Find((wxChar *)(char *)s);
	};
	int Finds( const char* s, int nStart)const {
		return (int)wxStringBase::find((wxChar *)s, nStart);	
	};
/*	int Find(INXString str){
		return (int)wxString::Find((wxChar *)(char *)str);
	};
	int Find(INXString str, int nStart){
		return (int)find((wxChar *)(char *)str, nStart);
	};*/
	int Find(char c, int nStart){
		return (int)find(c, nStart);
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
	void ReleaseBuffer(int nNewLength){
		UngetWriteBuf(nNewLength); 	
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
	/*INXString Tokenize(char* str, int pos){
		//CString _str;
		//_str = _str.Tokenize(str, pos);
		return NULL;
	};
	// Extraction 

	INXString SpanExcluding(const char* str){
		//CString _str = c_str();
		//_str = _str.SpanExcluding(str);
		return NULL;
	};*/
};
#endif