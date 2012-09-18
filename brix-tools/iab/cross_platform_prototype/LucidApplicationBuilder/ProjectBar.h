// ProjectBar.h: interface for the CProjectBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PROJECTBAR_H_INCLUDED_)
#define _PROJECTBAR_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
//#include "scbarg.h" @todo
//#include "MyTreeCtrl.h"

class CProjectBar //: public CSizingControlBarG  @todo
{
public:

	CProjectBar();
	virtual ~CProjectBar();
	int init(void);
#ifdef _UNUSED_FUNCTIONS_TO_LOAD_THE_FILE
	afx_msg void OnLButtonDblClk(UINT nFlags, INXPoint point);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
#endif
public:
	//CMyTreeCtrl m_cProjTree;
};

#endif 