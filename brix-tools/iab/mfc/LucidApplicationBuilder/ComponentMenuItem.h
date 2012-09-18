#pragma once

/////////////////////////////////////////////////////////////////////////////
// ComponentMenuItem
/*****************************************************************/
// Class ComponentMenuItem - holds the menu information items
// on the component menu tree.
// This structure is partly populated from dist/config/menu.xml
// with the remainder of the items coming from the component cdf/idf
// files as they are read in
//
// Note that it is not a requirement that every menu item is listed
// in menu.xml.
//
// Menu items are nested so we have an array in each class
/*****************************************************************/
#include "Porting_Classes/INXObjArray.h"
#include "Porting_Classes/INXString.h"
#include "Porting_Classes/INXObject.h"

class ComponentMenuItem : public INXObject
{
public:
	ComponentMenuItem(ComponentMenuItem * m_parent);
	~ComponentMenuItem(void);

	INXString m_name; // the menu item name
	INXObjArray<INXObject *> m_listOfSubMenuItems; // array of other sub menus of this menu
	ComponentMenuItem * m_parent; // parent of this menu item, null if root menu item

	int m_priority;	// this is a non-unique number that tells us where the menu item should appear in the list
					// - do not assume the numbers assigned to menu items will be consecutive or unique	
	INXString m_styleOnTree;  // refers the bitmap that will be displayed on the tree for each menu item
};
