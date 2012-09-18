#pragma once
//#include "Resource.h"

#include "afxwin.h"
#include <map>
#include <vector>
#include "ProjectMetaData.h"
#include "../LucidGUIBuilder/ScreenTagListBox.h"
#include "HScrollListBox.h"
#include "Porting_Classes/INXString.h"

//#define IDD_SCREEN_TAGS 50
using namespace std;

// CScreenTagDialog dialog

class CScreenTagDialog : public CDialog
{
	DECLARE_DYNAMIC(CScreenTagDialog)

public:

	CScreenTagDialog( ProjectMetaData *pPMD, CWnd* pParent = NULL );   // standard constructor
	virtual ~CScreenTagDialog();

// Dialog Data
	//enum { IDD = IDD_SCREEN_TAGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CEdit m_control_addTag;
	CEdit m_control_addDescription;

public:


	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
	virtual void OnOK();
public:



protected:
	ProjectMetaData *m_pProjectMetaData;
	map< INXString, ScreenMgrSupportData_t > m_TagDescrMap;
	afx_msg void OnStnClickedScrntagStaticTags();

public:
	int getTagList(vector<CString> &tagVector);
	afx_msg void OnBnClickedScrntagButtonAdd();
	CString formShowableLine(CString tag, CString descr);

protected:
	virtual void OnCancel();
public:
	//int getScreenTagMgrData(map<CString, ScreenMgrSupportData_t> &tagDescrMap);
	void addMainString(CString & cStr);
	void updateDisplayWithUnderlyingData();
	afx_msg void OnBnClickedScrntagButtonDelete();

protected:

	vector<TagAndDescriptionProcessable_t> m_vTagsToBeProcessed;
	CHScrollListBox m_ctlDescrList;
	CHScrollListBox m_ctlTagList;
	CListBox m_ctlTimesUsed;
	CButton m_ctlDeleteBtn;

public:
	afx_msg void OnLbnSelchangeTagList();
	afx_msg void OnBnClickedScrntagEditdescripBt();

protected:
	CButton m_ctlEditDescripBtn;
};



