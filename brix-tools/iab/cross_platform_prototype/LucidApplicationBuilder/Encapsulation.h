// Encapsulation.h: interface for the Encapsulation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ENCAPSULATION_H_INCLUDED_)
#define _ENCAPSULATION_H_INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
*/
#include "Encapsulate.h"
#include "condat.h"
#include "Porting_Classes/INXObjArray.h"
//#include "mainFrm.h"


class Encapsulation  
{
public:
	Encapsulation();
	virtual ~Encapsulation();

	// Attributes
	INXString blockName;
	INXString iconBitmap;
	//CMainFrame *pFrame;	
	INXObjList* encapsulated;
	HTREEITEM hItem;

	// Methods
	void EncapsulateBlock(INXObjList* _encapsulated, HTREEITEM _hItem);
	INXObjList* EncapsulateSubset(INXRect encapsulate, INXObjList* depList, HTREEITEM _hItem, int nLib);
	int SelectEncapsulateIcon();
	int SaveEncapsulated(Encapsulate* dialog, int iconNum, int nLib);
	void WriteIDF(Encapsulate *dialog, int iconNum);
	void WriteIDF(const ConData *block, const int nIconNum, const INXString csL2MenuName);
	int GetFirstInPortYCoord();
	int GetFirstOutPortYCoord();
	INXString IconNum2Bitmap(int iconNum);
	ConData* GetIconFromID(long int id, INXObjList* list);
	ConData* AddXPort(INXString type, INXPoint point);
	bool IsUniqueSubsetID(long id);
	//void EditEncapsulation(POSITION selectedIcon);
	//void SaveSubBlocks(CObList* subBlock, INXString hierarchyName);
	void repositionEncapsulated();
	void setLineSelected(ConData* blob);
	void ResetXportConnected(INXObjList* depList);
	void SetEncapsulated(INXObjList* _encapsulated);
	bool IsLibComponentUnique(const INXString csLibName);
	bool HasXPorts(INXRect xEncapsulate, INXObjList* xDepList);
};

#endif // !defined(AFX_ENCAPSULATION_H__5B8F7D0E_9346_4C0F_BBDC_9BAF4988CF61__INCLUDED_)
