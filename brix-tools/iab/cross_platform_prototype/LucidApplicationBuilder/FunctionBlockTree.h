#ifndef _FUNCTIONBLOCKTREE_H
#define _FUNCTIONBLOCKTREE_H

#include <wx/treectrl.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
//#include "LucidConstants.h"
#include "ProjectMetaData.h"
#include "ComponentMenuItem.h"
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObject.h"

class FunctionBlockTree:public wxTreeCtrl{
	#define MAX_FIRST_LEV 256
	#define MAX_SECOND_LEV 4096
	#define MAX_THIRD_LEV 64000
	enum
    {
        TreeCtrlIcon_File,
        TreeCtrlIcon_FileSelected,
        TreeCtrlIcon_Folder,
        TreeCtrlIcon_FolderSelected,
        TreeCtrlIcon_FolderOpened
    };
public:
	FunctionBlockTree(void);
	FunctionBlockTree(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
					   long style);
public:
	~FunctionBlockTree(void);

	INXObjArray<const wxTreeItemId> levelOne;
	INXObjArray<const wxTreeItemId> levelTwo;
	INXObjArray<const wxTreeItemId> levelThree;
	INXObjArray<const wxTreeItemId> levelFour;

	unsigned int m_DragDropFormat;
	wxTreeItemId rootId;
	wxTreeItemId firstlev[MAX_FIRST_LEV];
	wxTreeItemId secondlev[MAX_SECOND_LEV];
	wxTreeItemId thirdlev[MAX_THIRD_LEV];
	wxTreeItemId forthlev[MAX_THIRD_LEV]; //@todo change this to 4th level.
	
	wxTreeItemId handle;

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
	protected:
	void readIDF(INXString csIDFPath, INXString fileType);
	void readMenuMetaInfo();
	void readMenuItemsFromCDF(INXString *level1, INXString *level2, INXString *level3, INXString *level4, INXString csIDFPath);
	void addBitmapToImageList(unsigned int bitmapID);
	void outputNamePriority(ComponentMenuItem *item);
	void addMenuItem(INXString csIconName, INXString csUserdefined, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4);
	void addComponentsToTree(int currLevel, ComponentMenuItem *item, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4);
	void addItemsToTree(INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, int it);
private:
	bool m_bLftBtnDown;
	//bool leafIsSelected();
	bool m_bDraggingIcon;
	set<INXString> m_sL2LibMenuNames;
	wxImageList m_imageList; // image list used by the tree
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


public:
	 void AddTestItemsToTree(size_t numChildren, size_t depth);
	 void CreateImageList(int size = 16);

	 wxTreeItemId m_hItemClicked;
	 wxTreeItemId m_hPrevItemClicked;
private:
	 void AddItemsRecursively(const wxTreeItemId& idParent, size_t nChildren,
		 size_t depth, size_t folder);
	 int          m_imageSize;               // current size of images
	 bool         m_reverseSort;             // flag for OnCompareItems
     wxTreeItemId m_lastItem,                // for OnEnsureVisible()
                 m_draggedItem;             // item being dragged r
	 DECLARE_DYNAMIC_CLASS(MyTreeCtrl)
};

class MyTreeItemData : public wxTreeItemData
{
public:
    MyTreeItemData(const wxString& desc) : m_desc(desc) { }
    const wxChar *GetDesc() const { return m_desc.c_str(); }

private:
    wxString m_desc;
};

#endif //_FUNCTIONBLOCKTREE_H
