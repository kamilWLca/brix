#include "FunctionBlockTree.h"
#include "DrawProg.h"
#include <wx/icon.h>
#include <wx/imaglist.h> 
#include <wx/image.h>
#include "wx/log.h"

#include "./res/icon1.xpm"
#include "./res/icon2.xpm"
#include "./res/icon3.xpm"
#include "./res/icon4.xpm"
#include "./res/icon5.xpm"

#include <fstream>
#include "LucidConstants.h"
#include "GlobalFuncs_2.h"
#include "libxml/xmlreader.h"
#include <sstream>
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <wx/dir.h>

#define BITMAP_HEIGHT 16
#define BITMAP_WIDTH 6
#define INITIAL_SIZE_IMAGE_LIST 1
#define MAX_SIZE_IMAGE_LIST 4



/////////////////////////////////////////////////////////////////////////////
// FunctionBlockTree dialog
//static INXPRECT position;
//static int first=0;
// global variable declared in CDrawProgApp
extern wxChar workDir[WORK_DIR_SIZE];


IMPLEMENT_DYNAMIC_CLASS(FunctionBlockTree, wxTreeCtrl)

FunctionBlockTree::FunctionBlockTree(void){
}
FunctionBlockTree::FunctionBlockTree(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
					   long style): wxTreeCtrl(parent, id, pos, size, style){
	init() ;
}

FunctionBlockTree::~FunctionBlockTree(void){
}

static void trimString(char * trimmedStr, const char *toTrimStr) {
	const char * endPos = NULL;
	int i = 0, myStrLen = 0;

	while (((*toTrimStr == ' ') || (*toTrimStr == '\t') || (*toTrimStr == '\n') || (*toTrimStr == '\r')) && (*toTrimStr != '\0')) { toTrimStr++;}
	myStrLen = strlen(toTrimStr);
	endPos = toTrimStr + myStrLen - 1;
	while ((*endPos == ' ') || (*endPos == '\t') || (*endPos == '\n') || (*endPos == '\r')) { endPos--;i++;}
	myStrLen = myStrLen - i;
	strncpy(trimmedStr, toTrimStr, myStrLen);
	trimmedStr[myStrLen] = '\0';
}



//iAB

void FunctionBlockTree::init() 
{
	int i;
	//	CDialog::OnInitDialog();
	//@todo these are done in the mainframe class
	//if (first) MoveWindow(position);
	//Get window position, and then move it to the position the mouse was clicked
	//position = new INXRect; /* @todo MIGRATION_ISSUE*/
	//GetWindowRect(position);
	//MoveWindow(this->pos.x, this->pos.y, position->right - position->left, position->bottom - position->top);
	//SetBackGroundColor(wxColor(205,220,255));
	//SetTextColor(RGB(10,0,30)); @todo do it in the wxWidgets
	//SetLineColor(RGB(128,30,30)); @todo do it in the wxWidgets
	//delete position;

	// to create icons for the tree - first create an image list and then associate it with the tree
#ifdef EXCLUDE_TEMPORARLY
	m_imageList.Create (BITMAP_WIDTH, BITMAP_HEIGHT, ILC_MASK|ILC_COLOR32, INITIAL_SIZE_IMAGE_LIST, MAX_SIZE_IMAGE_LIST);

	// Add the bitmaps to the ImageList
	addBitmapToImageList(IDB_TREEICON_COMPONENT_DEFAULT);
	addBitmapToImageList(IDB_TREEICON_DEFAULT);	
	addBitmapToImageList(IDB_TREEICON_A1);
	addBitmapToImageList(IDB_TREEICON_B1);
	addBitmapToImageList(IDB_TREEICON_C1);
	addBitmapToImageList(IDB_TREEICON_D1);
	addBitmapToImageList(IDB_TREEICON_E1);
	addBitmapToImageList(IDB_TREEICON_A2);
	addBitmapToImageList(IDB_TREEICON_B2);
	addBitmapToImageList(IDB_TREEICON_C2);
	addBitmapToImageList(IDB_TREEICON_D2);
	addBitmapToImageList(IDB_TREEICON_E2);

	SetImageList( &m_imageList, LVSIL_NORMAL );	// LVSIL_NORMAL - expects max 16x16 pixel bitmap
//	ASSERT(GetImageList(LVSIL_NORMAL) == &m_imageList);
#endif
	i = 0;
	m_styleLookup[i++] = "COMPONENT DEFAULT";
	m_styleLookup[i++] = "DEFAULT";
	m_styleLookup[i++] = "A1";
	m_styleLookup[i++] = "B1";
	m_styleLookup[i++] = "C1";
	m_styleLookup[i++] = "D1";
	m_styleLookup[i++] = "E1";
	m_styleLookup[i++] = "A2";
	m_styleLookup[i++] = "B2";
	m_styleLookup[i++] = "C2";
	m_styleLookup[i++] = "D2";
	m_styleLookup[i++] = "E2";
	m_maxStyle = i;

	// read in the priority settings and style for menu items
//	readStyleMetaInfo();	//@todo - in future should read the style meta inf in from the menu.xml file and use this to populate the styleOnTree property of each menu item
							// at the moment are simply assuming that the Style tag for each menu item holds this information
	readMenuMetaInfo();	

	//@todo - to change GUI tree based on a change of profile or addition of a component, need to delete the GUI tree and call this method again

	loadContents();

}

void FunctionBlockTree::readIDF(INXString csIDFPath, INXString fileType)
{
	
	INXString csFileName, csFilePath;
	//finder.FindFirst(csIDFPath + "*" + fileType);
	int bWorking = 0;
	wxDir finder(csIDFPath);
	if(finder.IsOpened()){
		finder.FindFirst(csIDFPath,"*" + fileType);
		wxString nameOfFile;
		bWorking = finder.GetFirst(&nameOfFile,"*" + fileType);
	}


	INXString csLevel1;
	INXString csLevel2;
	INXString csLevel3;
	INXString csLevel4;
	INXString csIconName;
	INXString csUserdefined;

	m_csaL2IconName.SetSize(MAX_SECOND_LEV);
	m_csaL2MenuName.SetSize(MAX_SECOND_LEV);
	m_csaL3IconName.SetSize(MAX_THIRD_LEV);
	m_csaL3MenuName.SetSize(MAX_THIRD_LEV);
	m_csaL4IconName.SetSize(MAX_THIRD_LEV); //@todo these should use a level four value too!
	m_csaL4MenuName.SetSize(MAX_THIRD_LEV);



	// read in ini files and construct menu tree
	while (bWorking)
	{
		bWorking = finder.GetNext(&csFileName); //bWorking = finder.FindNextFile(); //original 
		//csFileName = finder.GetFileName();// original
		csFilePath = csIDFPath + csFileName;
		
		if (fileType == FILE_TYPE_CDF) {
			// Extract the menu items from the ini file
			readMenuItemsFromCDF(&csLevel1,&csLevel2,&csLevel3,&csLevel4,csFilePath);
			// note - change from IDF method - use actual filename at this point as icon name, later will retrieve file and parse to get icon name 
			csIconName = csFileName;
			//readUserDefinedFromCDF(&csUserdefined,csFilePath); //@todo ?is this defined in the xml when creating user defined components? - if so we need to extract it here
			csUserdefined = "";
		} else {
			// Extract the menu items from the ini file
			csLevel1 = GetIniValue(SECTIONL1, KEYL1, csFilePath);
			csLevel2 = GetIniValue(SECTIONL2, KEYL2, csFilePath);
			csLevel3 = GetIniValue(SECTIONL3, KEYL3, csFilePath);
			csLevel4 = GetIniValue(SECTIONL4, KEYL4, csFilePath);
			// Extract icon name from the ini file
			csIconName = GetIniValue(SECTIONICON, KEYICON, csFilePath);
			// Extract user defined
			csUserdefined = GetIniValue("Icon", "user defined", csFilePath);
		}

		// add it to the collection of components to be added to the tree
		ComponentOnTree_t * comp = new ComponentOnTree_t();
		comp->iconName = csIconName;
		comp->menuNameL1 = csLevel1;
		comp->menuNameL2 = csLevel2;
		comp->menuNameL3 = csLevel3;
		comp->menuNameL4 = csLevel4;
		comp->userDefined = csUserdefined;
		m_componentList.Add(comp);
	}
}

/*
 Parse the XML file using libXML2 that holds the information regarding menu item styles
*/
void FunctionBlockTree::readMenuMetaInfo()
{
	char *name = NULL, * value = NULL;
	xmlTextReaderPtr reader;
	int ret = 0, i;
	ComponentMenuItem * thisMenuItem = NULL;
	ComponentMenuItem * parentMenuItem = NULL;
	bool processingStyles = false, itemAdded = false, noMoreChildren = false;

	INXString menuConfigPath = workDir + MENUCONFIGFILE;
    reader = xmlNewTextReaderFilename(menuConfigPath);
    if (reader != NULL) {

		// read next node
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
			name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!

			// if this is a node end type, then skip this node
			if (xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
				// if we are processing a menu item this signifies no more children,
				if (strcmp(COMPONENTMENU_ITEM_ELEMENT, name) == 0 && thisMenuItem != NULL) {
					// if previous menu item hasn't been added to the list by the time we get to the next item, add it at the end
					if (!itemAdded) {
						if (parentMenuItem == NULL) {
							m_rootMenuList.Add(thisMenuItem);
						} else {
							parentMenuItem->m_listOfSubMenuItems.Add(thisMenuItem);
						}
						itemAdded = true;
					}

					// if we are processing a menu item this signifies no more children,
					// so can change parent pointer to be this nodes parent, ready to accept next sibling
					parentMenuItem = thisMenuItem->m_parent;
					thisMenuItem = parentMenuItem;
				}
				ret = xmlTextReaderRead(reader);
				continue;
			}
			// if this is a #text type in an unexpected place, then skip this node
			if ((xmlTextReaderNodeType(reader) == NODE_TYPE_STARTTEXT_ELEMENT) || (xmlTextReaderNodeType(reader) == NODE_TYPE_ENDTEXT_ELEMENT)) {
				ret = xmlTextReaderRead(reader);
				continue;
			}


			if (strcmp(COMPONENTMENU_STYLE_ELEMENT, name) == 0) {
				//@todo - inefficient as parses all style tags before reaches menu items, can't get methods to work to skip this part of the subtree
				//ret = xmlTextReaderNextSibling(reader); //doesn't work, returns error!
				//xmlTextReaderReadInnerXml(reader);
				processingStyles = true;
			} else if (strcmp(COMPONENTMENU_ITEM_ELEMENT, name) == 0) {
				processingStyles = false;

				// initialise new menu item
				// everytime we find a menu item in xml, create a ComponentMenuItem and add it to its parent list
				// if no current menu item, means we add it to the root list
				noMoreChildren = false;
				itemAdded = false;
				thisMenuItem = new ComponentMenuItem(parentMenuItem);

			} else if (!processingStyles && strcmp(COMPONENTMENU_ITEM_NAME_ELEMENT, name) == 0) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);
				thisMenuItem->m_name = value;
			} else if (!processingStyles && strcmp(COMPONENTMENU_ITEM_PRIORITY_ELEMENT, name) == 0) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);
				thisMenuItem->m_priority = atoi(value);

				// insert new menu item according to its priority
				if (parentMenuItem == NULL) {
					for (i=0; i<m_rootMenuList.GetCount(); i++) {
						if (thisMenuItem->m_priority < ((ComponentMenuItem*) m_rootMenuList.GetAt(i))->m_priority) {
							break;
						}
					}
					m_rootMenuList.InsertAt(i,thisMenuItem);					
				} else {
					for (i=0; i<parentMenuItem->m_listOfSubMenuItems.GetCount(); i++) {
						if (thisMenuItem->m_priority < ((ComponentMenuItem*) parentMenuItem->m_listOfSubMenuItems.GetAt(i))->m_priority) {
							break;
						}
					}
					parentMenuItem->m_listOfSubMenuItems.InsertAt(i,thisMenuItem);					
				}
				itemAdded = true;
				parentMenuItem = thisMenuItem;
			} else if (!processingStyles && strcmp(COMPONENTMENU_ITEM_STYLE_ELEMENT, name) == 0) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);
				thisMenuItem->m_styleOnTree = value;
			}

			// read next node
            ret = xmlTextReaderRead(reader);
		}
        xmlFreeTextReader(reader);

		//dk debug only
//		for (i=0; i<m_rootMenuList.GetCount(); i++) {
//			outputNamePriority((ComponentMenuItem*) m_rootMenuList.GetAt(i));
//		}


	} else {
        wxMessageBox("Unable to open file " + menuConfigPath);
    }
}

void FunctionBlockTree::outputNamePriority(ComponentMenuItem *item) {
	if (item == NULL) return;

	std::string s;
	std::stringstream out;
	out << item->m_priority;
	s = out.str();
//	TRACE(_T("Name: " + item->m_name + ", Priority: " +  s.c_str() + "\n")); //mfc

	int i;
	for (i=0; i<item->m_listOfSubMenuItems.GetCount(); i++) {
		outputNamePriority((ComponentMenuItem*) item->m_listOfSubMenuItems.GetAt(i));
	}
}

void FunctionBlockTree::readMenuItemsFromCDF(INXString *level1, INXString *level2, INXString *level3, INXString *level4, INXString csIDFPath)
{
	char target[256];
	char *name = NULL, * value = NULL;
	xmlTextReaderPtr reader;
	int levelCnt = 0;
	bool isMenuParsed = false;
	int ret = 0;

	*level1 = "";
	*level2 = "";
	*level3 = "";
	*level4 = "";

    reader = xmlNewTextReaderFilename(csIDFPath);
    if (reader != NULL) {

		// read next node
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {

			// if this is a node end type, then stop processing
			if (isMenuParsed && xmlTextReaderNodeType(reader) == NODE_TYPE_END_ELEMENT) {
				break;
			}

			name = (char*) xmlTextReaderConstName(reader); // This is dealoocated with reader  -don't deallocate!
			if ((strcmp(NODE_MENU_ELEMENT, name) == 0)) {
				ret = xmlTextReaderRead(reader);	// note - once find node, need to read again to access #text element
				value = (char*) xmlTextReaderConstValue(reader);
				//value = (char*) xmlTextReaderGetAttribute(reader, (const xmlChar *)"name");

				trimString(target,value);

				isMenuParsed = true;
				levelCnt++;
				if (levelCnt == 1) {
					*level1 = (INXString) target;
				} else if (levelCnt == 2) {
					*level2 = (INXString) target;
				} else if (levelCnt == 3) {
					*level3 = (INXString) target;
				} else if (levelCnt == 4) {
					*level4 = (INXString) target;
				} else {
					// only expecting 4 menu levels
					wxMessageBox("menuTag: exceeded 4 menu items");
				}
			}

			// read next node
            ret = xmlTextReaderRead(reader);
		}
        xmlFreeTextReader(reader);
	} else {
        wxMessageBox("Unable to open " + csIDFPath);
    }
}

void FunctionBlockTree::addBitmapToImageList(unsigned int bitmapID) {	
	wxBitmap theBitmap; // bitmap witch loads 32bits bitmap

/*@todo MIGRATION_ISSUE	
	theBitmap.LoadBitmap(bitmapID); */
	//theBitmap.LoadOEMBitmap(1);
#ifdef EXCLUDE_TEMPORARLY
	theBitmap.LoadBitmap(bitmapID);
	m_imageList.Add(&theBitmap);
#endif
}	

#ifdef _EXCLUDE_AS_UNUSED_IN_WX_WIDG_FOR_NOW
void FunctionBlockTree::addMenuItem(INXString csIconName, INXString csUserdefined, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4) {
	int csImgIndL1 = 1;	// 1 is default style for non-component menu items, 0 is style for components
	int csImgIndL2 = 1;
	int csImgIndL3 = 1;
	int csImgIndL4 = 1;
	INXString lev1Item;
	INXString lev2Item;
	INXString lev3Item;
	//INXString lev4Item;
	int iTmpL1Ind = 0;
	int iTmpL2Ind = 0;
	int iTmpL3Ind = 0;
	bool bLev1Flag = FALSE;
	bool bLev2Flag = FALSE;
	bool bLev3Flag = FALSE;
	int i;


	// lookup style
	for (i=0; i<m_maxStyle;i++) {
		if (strcmp(m_styleLookup[i], csStyle1) == 0) {
			csImgIndL1 = i;
		}
		if (strcmp(m_styleLookup[i], csStyle2) == 0) {
			csImgIndL2 = i;
		}
		if (strcmp(m_styleLookup[i], csStyle3) == 0) {
			csImgIndL3 = i;
		}
		if (strcmp(m_styleLookup[i], csStyle4) == 0) {
			csImgIndL4 = i;
		}
	}

	// use component bitmap for leaves of tree
	if (strcmp(csLevel2,"") == 0) {
		csImgIndL1 = 0;
	} else if (strcmp(csLevel3,"") == 0) {
		csImgIndL2 = 0;
	} else if (strcmp(csLevel4,"") == 0) {
		csImgIndL3 = 0;
	} else {
		csImgIndL4 = 0;
	}

	// add level 1 item to menu if it doesn't exist. We search through each level to see...
	for (i=0; i<m_iLev1Ind; i++) { // search the level 1 sapce for an existing parent
		if(firstlev[i].IsOk())
		lev1Item = GetItemText(firstlev[i]);
		// if it exists use this index for level 2
		if (lev1Item == csLevel1) {
			bLev1Flag = TRUE;
			iTmpL1Ind = i;//m_iLev1Ind;
			//m_iLev1Ind = i;
		}
	}
	if (!bLev1Flag) {

		firstlev[m_iLev1Ind]= AppendItem(rootId,csLevel1);// just text for now, csImgIndL1, csImgIndL1);
		// apprently we dont support these: m_csaL1IconName.SetAtGrow(m_iLev1Ind, csIconName);

		iTmpL1Ind=m_iLev1Ind; 
		m_iLev1Ind++;
	}

	// add level 2 item to menu if it doesn't exist
	if (csLevel2 != "") {
	for (i=0; i<m_iLev2Ind; i++) {
		// for each level 2 item check its parent is the same as the current item's level 1 branch; 
		// i.e check if we already have an element with the same parent. 
		if (GetItemParent(secondlev[i]) == firstlev[iTmpL1Ind]) { // found another entry with the same parent.
			if(secondlev[i].IsOk())
			lev2Item = GetItemText(secondlev[i]);
			if (lev2Item == csLevel2) { // check that the current level entry isn't the same.
				bLev2Flag = TRUE;
				iTmpL2Ind = i;//m_iLev2Ind;
				//m_iLev2Ind = i; // store the index that a common parent brnach has been identified.
			}
		}
	}

	if (!bLev2Flag) { //if the branch doesn't exist in this level then make it, or if the next level up is not clear the make it again!
	secondlev[m_iLev2Ind]= AppendItem(firstlev[iTmpL1Ind],csLevel2); //@todo
#ifdef _EXCLUDE_TEMPORARLY_
		secondlev[m_iLev2Ind]=InsertItem(csLevel2, csImgIndL2, csImgIndL2, firstlev[iTmpL1Ind]);
#endif
		if (csLevel1 == "User Components") {
				m_sL2LibMenuNames.insert(csLevel2);
			}
		m_csaL2IconName.SetAtGrow(m_iLev2Ind, csIconName);
		iTmpL2Ind=m_iLev2Ind; 
		m_iLev2Ind++;
		
	}
	}
	bLev2Flag = FALSE;

// add level 3 item to menu if it doesn't exi
	if (csLevel3 != "") {
		for (i=0; i<m_iLev3Ind; i++) {
			// for each level 2 item check its parent is the same as the level 1 item
			if (GetItemParent(thirdlev[i]) == secondlev[iTmpL2Ind]) {
				//for (int j=0; j<m_iLev2Ind; j++) { // need to loop again to see if there are any upper levels with the same tag - this is all rubbish and needs to be re-written as this could fail quite easily too
				//	if (GetParentItem(secondlev[j]) == firstlev[iTmpL1Ind] ) {		// all is the same!		
				if(secondlev[i].IsOk())		
				lev3Item = GetItemText(thirdlev[i]);
						if (lev3Item == csLevel3) {
							bLev3Flag = TRUE;
							iTmpL3Ind = i; //sort out using this in the following
							//iTmpL2Ind = i;
							//iTmpL1Ind = j;
						}
				//	}
				//}
			}
		}
		if (!bLev3Flag ) { //if the branch doesn't exist in this level then make it, or if the next level up is not clear the make it again!
#ifdef _EXCLUDE_TEMPORARLY_			
			thirdlev[m_iLev3Ind]=InsertItem(csLevel3, csImgIndL3, csImgIndL3, secondlev[iTmpL2Ind]);
#endif			
			thirdlev[m_iLev3Ind]= AppendItem(secondlev[iTmpL2Ind],csLevel3 );
			/*
			if (csLevel1 == "User Components") {
				m_sL2LibMenuNames.insert(csLevel3); // shouldn;t be needed
			}
			*/
			if (csUserdefined == "1") { // We need this apparently because defining the icon name tel
				m_csaL3MenuName.SetAtGrow(m_iLev3Ind, csLevel3);
				/* We don't need this because libraries are always anchored at User Components..
				if (csLevel1 == "User Components") {
				m_sL2LibMenuNames.insert(csLevel2);
				}
				*/
			}

			m_csaL3IconName.SetAtGrow(m_iLev3Ind, csIconName);
			iTmpL3Ind=m_iLev3Ind; //for the benefit of others 
			m_iLev3Ind++;
		}
	}


	// add level 4 item to menu @todo this is silly!
	if (csLevel4 != "") {
#ifdef _EXCLUDE_TEMPORARLY_
		forthlev[m_iLev4Ind] = InsertItem(csLevel4, csImgIndL4, csImgIndL4 , thirdlev[iTmpL3Ind]); 
#endif
		forthlev[m_iLev4Ind] = AppendItem(thirdlev[iTmpL3Ind],csLevel4); 
		// add icon name to level4 array
		m_csaL4IconName.SetAtGrow(m_iLev4Ind, csIconName);
		if (csUserdefined == "1") { // We need this apparently because defining the icon name tel
			m_csaL4MenuName.SetAtGrow(m_iLev4Ind, csLevel4);
			/* We don't need this because libraries are always anchored at User Components..
			if (csLevel1 == "User Components") {
				m_sL2LibMenuNames.insert(csLevel2);
			}
			*/
		}
		
		//	m_csaL4MenuName.SetAtGrow(m_iLev4Ind, "");
		//}
		m_iLev4Ind++; // we do this here because it isn't done in the following stuff.
	}
	/* We are notmusing icons here - some colour might be nice though!
	    add icon name and menu name to separate level4 arrays
	else { // yes we should be shot for allowing this code!!
		m_csaL4IconName.SetAtGrow(m_iLev3Ind, csIconName);
		if (csUserdefined == "1") {
			m_csaL3MenuName.SetAtGrow(m_iLev3Ind, csLevel3);
		}
		else {
			m_csaL3MenuName.SetAtGrow(m_iLev3Ind, "");
		}
	}
	*/
		
	bLev1Flag = FALSE;

	bLev2Flag = FALSE;

	bLev3Flag = FALSE;

	m_iL4MenuSize = m_iLev4Ind;
	m_iL3MenuSize = m_iLev3Ind;
	m_iL2MenuSize = m_iLev2Ind;
}


void FunctionBlockTree::addComponentsToTree(int currLevel, ComponentMenuItem *item, INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, INXString csStyle1, INXString csStyle2, INXString csStyle3, INXString csStyle4) {
	int i;
	bool isMatch;

	if (currLevel == 1) {
		csLevel1 = item->m_name;
		csStyle1 = item->m_styleOnTree;
	} else if (currLevel == 2) {
		csLevel2 = item->m_name;
		csStyle2 = item->m_styleOnTree;
	} else if (currLevel == 3) {
		csLevel3 = item->m_name;
		csStyle3 = item->m_styleOnTree;
	} else if (currLevel == 4) {
		csLevel4 = item->m_name;
		csStyle4 = item->m_styleOnTree;
	}

	// if no children, then find all components that match this branch of the tree
	// and populate the GUI tree with them
	if (item->m_listOfSubMenuItems.IsEmpty()) {
		for (i=0; i<m_componentList.GetCount();) {
			ComponentOnTree_t* comp = (ComponentOnTree_t*) m_componentList.GetAt(i);

			// if we are only at level 1 for example, want to find all components with this as the level 1 menu name
			// if we are at level 2, want to find all components with matching level 1 and level 2 names etc
			isMatch = false;
			if (currLevel == 1) {
				if (strcmp(csLevel1, comp->menuNameL1) == 0) {
					isMatch = true;
				}
			} else if (currLevel == 2) {
				if (strcmp(csLevel1, comp->menuNameL1) == 0 && strcmp(csLevel2, comp->menuNameL2) == 0) {
					isMatch = true;
				}
			} else if (currLevel == 3) {
				if (strcmp(csLevel1, comp->menuNameL1) == 0 && strcmp(csLevel2, comp->menuNameL2) == 0 && strcmp(csLevel3, comp->menuNameL3) == 0) {
					isMatch = true;
				}
			} else if (currLevel == 4) {
				if (strcmp(csLevel1, comp->menuNameL1) == 0 && strcmp(csLevel2, comp->menuNameL2) == 0 && strcmp(csLevel3, comp->menuNameL3) == 0 && strcmp(csLevel4, comp->menuNameL4) == 0) {
					isMatch = true;
				}
			}

			if (isMatch) {
				addMenuItem(comp->iconName, comp->userDefined, comp->menuNameL1, comp->menuNameL2, comp->menuNameL3, comp->menuNameL4, csStyle1, csStyle2, csStyle3, csStyle4);
				m_componentList.RemoveAt(i);			
			} else {
				i++;
			}

		}
		return;
	}

	std::string s;
	std::stringstream out;
	out << item->m_priority;
	s = out.str();
//	TRACE(_T("Name: " + item->m_name + ", Priority: " +  s.c_str() + "\n"));//mfc

	currLevel++;
	for (i=0; i<item->m_listOfSubMenuItems.GetCount(); i++) {
		addComponentsToTree(currLevel, (ComponentMenuItem*) item->m_listOfSubMenuItems.GetAt(i), csLevel1, csLevel2, csLevel3, csLevel4, csStyle1, csStyle2, csStyle3, csStyle4);
	}
}

bool FunctionBlockTree::leafIsSelected() 
{
	// TODO: Add extra validation here
	//handle=GetSelectedItem();
	if (!m_hItemClicked.IsOk()){ // original if (m_hItemClicked == NULL){
			//AfxMessageBox("Must select from the end of a menu branch");
			return false;
	}
		
	if ( m_hItemClicked > 0) { // original if ( GetChildItem(m_hItemClicked)>0) {
		//AfxMessageBox("Must select from the end of a menu branch");
		return false;
	}
	//if (!first) position=new INXRect;
	//GetWindowRect(position);
	//first=1;
	return true;
}

#endif
void FunctionBlockTree::addItemsToTree(INXString csLevel1, INXString csLevel2, INXString csLevel3, INXString csLevel4, int it){
	if(levelOne.IsEmpty()){
		levelOne.Add(AppendItem(rootId,csLevel1));
		if(csLevel2 != ""){
			if(levelTwo.IsEmpty()){
				levelTwo.Add(AppendItem(levelOne.GetAt(levelOne.GetUpperBound()),csLevel2));
				if(csLevel3 != ""){
					if(levelThree.IsEmpty()){
						levelThree.Add(AppendItem(levelTwo.GetAt(levelTwo.GetUpperBound()),csLevel3));
						if(csLevel4 != ""){
							if(levelFour.IsEmpty()){
								levelFour.Add(AppendItem(levelThree.GetAt(levelThree.GetUpperBound()),csLevel4));
							}
						}
					}
				}
			}
		}
	}else{
		wxString tempStrIdL1 =  GetItemText(levelOne.GetAt(levelOne.GetUpperBound()));
		if(tempStrIdL1 == csLevel1 && csLevel1 != ""){
			levelTwo.Add(AppendItem(levelOne.GetAt(levelOne.GetUpperBound()),csLevel2));
			wxString tempStrIdL2 =  GetItemText(levelTwo.GetAt(levelTwo.GetUpperBound()));
			if(tempStrIdL2 == csLevel2 && csLevel2 != ""){
				levelThree.Add(AppendItem(levelTwo.GetAt(levelTwo.GetUpperBound()),csLevel3));	
				wxString tempStrIdL3 = GetItemText(levelThree.GetAt(levelThree.GetUpperBound()));
				if(tempStrIdL3 == csLevel3 && csLevel3 != ""){
					levelFour.Add(AppendItem(levelThree.GetAt(levelThree.GetUpperBound()),csLevel4));	
				}else if(tempStrIdL3 != ""){
					levelThree.Add(AppendItem(levelTwo.GetAt(levelTwo.GetUpperBound()),csLevel3));
				}			
			}else if(tempStrIdL2 != ""){
				levelTwo.Add(AppendItem(levelOne.GetAt(levelOne.GetUpperBound()),csLevel2));
			}
		}else if(tempStrIdL1 != ""){
			levelOne.Add(AppendItem(rootId,csLevel1));
		}	
	}
}

void FunctionBlockTree::loadContents() 
{	
	int i;
	DeleteAllItems();
	m_iLev1Ind = 0;
	m_iLev2Ind = 0;
	m_iLev3Ind = 0;
	m_iLev4Ind = 0;

//	readIDF(workDir + USERDEFDIR, FILE_TYPE_IDF); //@todo folder doesn't exist in the dist folder
//	readIDF(workDir + USERDEFDIR, FILE_TYPE_CDF); //@todo - handle user defined components for CDF files
#ifdef _EXCLUDE_TEMPORARLY_
	readIDF(workDir + IDFDIR, FILE_TYPE_IDF); //@todo excluding  temporarly 
#endif
	readIDF(workDir + CDFDIR, FILE_TYPE_CDF);
 
	//@todo - insert profile code here... as need to remove items from list of component m_componentList before addMenuItem called
	for (i=0; i<m_rootMenuList.GetCount(); i++) {
		//ComponentMenuItem* comp = (ComponentMenuItem*) m_rootMenuList.GetAt(i);
	//AppendItem(levelOne.GetAt(0),"test");
	}
	rootId =  AddRoot(wxT("ROOT"), -1, -1,
                                  new MyTreeItemData(wxT("Root item")));
	
    
	for (i=0; i<m_componentList.GetCount(); i++) {
		ComponentOnTree_t* comp = (ComponentOnTree_t*) m_componentList.GetAt(i);	
		addItemsToTree(comp->menuNameL1, comp->menuNameL2, comp->menuNameL3, comp->menuNameL4,i);
	}
#ifdef _EXCLUDE_TEMPORARLY_
	// traverse config menu item tree - it is already in priority order - and add components that match menu entries
	for (i=0; i<m_rootMenuList.GetCount(); i++) {
		addComponentsToTree(1, (ComponentMenuItem*) m_rootMenuList.GetAt(i), "", "", "", "", "", "", "", "");
	}
	

	// add any components that are remaining to the tree - no priority specified so just in the order read in
	for (i=0; i<m_componentList.GetCount(); i++) {
		ComponentOnTree_t* comp = (ComponentOnTree_t*) m_componentList.GetAt(i);
		// no styles specified, use defaults
		addMenuItem(comp->iconName, comp->userDefined, comp->menuNameL1, comp->menuNameL2, comp->menuNameL3, comp->menuNameL4, "", "", "", "");
	}
#endif
}

#ifdef _EXCLUDE_AS_UNUSED_IN_WX_WIDG_FOR_NOW

int FunctionBlockTree::GetMItemSize(INXString csLevel){
	int iMenuItemSize = -1;

	if (csLevel == "L2") {
		iMenuItemSize = m_iL2MenuSize;
	}
	else if (csLevel == "L3") {
		iMenuItemSize = m_iL3MenuSize;
	}
	else if (csLevel == "L4") {
		iMenuItemSize = m_iL4MenuSize;
	}

	return iMenuItemSize;
}

INXString FunctionBlockTree::GetIconName(int i, INXString csLevel)
{
	INXString csIconName = "";

	if (csLevel == "L2") {
		csIconName = m_csaL2IconName.GetAt(i);
	}
	else if (csLevel == "L3") {
		csIconName = m_csaL2IconName.GetAt(i);
	}
	else if (csLevel == "L4") {
		csIconName = m_csaL3IconName.GetAt(i);
	}

	return csIconName;
}

void FunctionBlockTree::getL2LibMenuNames(set<INXString> &sL2MenuNames)
{
	sL2MenuNames = m_sL2LibMenuNames;
}
#endif


