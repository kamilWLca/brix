#pragma once
#include "afxcmn.h"
#include "LucidConstants.h"
#include "ProjectMetaData.h"
#include "ComponentMenuItem.h"
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObject.h"

/////////////////////////////////////////////////////////////////////////////
// FunctionBlockTree dialog
/*****************************************************************/
// Class FunctionBlockTree handles the Dialog box containing the list of processing objects 
// There are three levels of tree to select in a hierachy - typically: Type, name, datatype
// THIS CLASS SHOULD BE RENAMED TO ObjectSelector
/*****************************************************************/


class FunctionBlockTree :	public CTreeCtrl
{
public:
	FunctionBlockTree(void);
	~FunctionBlockTree(void);

	UINT m_DragDropFormat;
	
	HTREEITEM firstlev[MAX_FIRST_LEV];
	HTREEITEM secondlev[MAX_SECOND_LEV];
	HTREEITEM thirdlev[MAX_THIRD_LEV];
	HTREEITEM forthlev[MAX_THIRD_LEV]; //@todo change this to 4th level.
	
	HTREEITEM handle;

// Implementation
	int m_iL2MenuSize;
	int m_iL3MenuSize;
	int m_iL4MenuSize;
	INXObjArray<INXString> m_csaL2IconName;
	INXObjArray<INXString> m_csaL2MenuName;
	INXObjArray<INXString> m_csaL3IconName;
	INXObjArray<INXString> m_csaL3MenuName;
	INXObjArray<INXString> m_csaL4IconName;
	INXObjArray<INXString> m_csaL4MenuName;


	
	INXPoint pos;
	int m_iLev1Ind, m_iLev2Ind, m_iLev3Ind, m_iLev4Ind; //@todo should be an array

// Dialog Data
	//{{AFX_DATA(FunctionBlockTree)
	//enum { IDD = IDD_CHOOSE };
	//CTreeCtrl	m_descript;
	//}}AFX_DATA
protected:
	void readIDF(INXString csIDFPath, INXString fileType);
	void FunctionBlockTree::readMenuMetaInfo();
	void FunctionBlockTree::readMenuItemsFromCDF(INXString *level1, INXString *level2, INXString *level3, INXString *level4, INXString csIDFPath);
	void FunctionBlockTree::addBitmapToImageList(unsigned int bitmapID);
	void FunctionBlockTree::outputNamePriority(ComponentMenuItem *item);
	void FunctionBlockTree::addMenuItem(INXString csIconName, INXString csUserdefined, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4);
	void FunctionBlockTree::addComponentsToTree(int currLevel, ComponentMenuItem *item, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4);

private:
	bool m_bLftBtnDown;
	//bool leafIsSelected();
	bool m_bDraggingIcon;
	set<INXString> m_sL2LibMenuNames;
	CImageList m_imageList; // image list used by the tree
	INXObjArray<INXObject *> m_rootMenuList; // list of ComponentMenuItems at root level of the menu tree
	INXString m_styleLookup[99];
	int m_maxStyle;
	INXObjArray<INXObject *> m_componentList; // list of m_ComponentOnTree_t to be placed on tree

	struct ComponentOnTree_t : public INXObject {
		INXString iconName;
		INXString menuNameL1;
		INXString menuNameL2;
		INXString menuNameL3;
		INXString menuNameL4;
		INXString userDefined;
	};

public:
	bool leafIsSelected(); 
	void loadContents(); 
	void ReadFile();
	int GetMItemSize(INXString csLevel);
	INXString GetIconName(int i, INXString csLevel);
	void getL2LibMenuNames(set<INXString> &sL2MenuNames); // this reads the first column (level 2) if menu items.
	void init(void);

	// Generated message map functions
	//{{AFX_MSG(FunctionBlockTree)
	//virtual BOOL OnInitDialog();
	//virtual void OnOK();
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint _point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint _point);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	HTREEITEM m_hItemClicked;
	HTREEITEM m_hPrevItemClicked;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


