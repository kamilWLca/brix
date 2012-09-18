#if !defined(_TAGDIALOG_H_INCLUDED_)
#define _TAGDIALOG_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 */
// TagDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTagDialog dialog
#include "Porting_Classes/INXObjList.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObjArray.h"
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/combobox.h>



class CTagDialog : public wxDialog
{
// Construction
public:
	CTagDialog(wxWindow* pParent = NULL);   // standard constructor
	CTagDialog(INXObjArray<INXString>* _boolTags, INXObjArray<INXString>* _intTags, INXObjArray<INXString>* _realTags, INXObjArray<INXString>* _stringTags, 
		INXObjArray<INXString>* _eventTags, int _dataType, int _portType, INXObjList* _condata, int _tagLine, wxWindow* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CTagDialog)
	enum { IDD = IDD_TAG_LINE };
	wxComboBox	m_Tag;
	//}}AFX_DATA

	// Attributes
	INXString tag;
	INXObjArray<INXString>* boolTags;
	INXObjArray<INXString>* intTags;
	INXObjArray<INXString>* realTags;
	INXObjArray<INXString>* stringTags;
	INXObjArray<INXString>* eventTags;
	int dataType;
	int portType;
	INXObjList* condata;
	int tagLine;

	// Methods
	void AddTag2List();
	bool IsTagUnique();
	bool IsTagNew();
	bool IsUniqueOutTag();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagDialog)
	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTagDialog)
	virtual bool OnInitDialog();
	void OnOK(wxCommandEvent & event);
	DECLARE_EVENT_TABLE()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
