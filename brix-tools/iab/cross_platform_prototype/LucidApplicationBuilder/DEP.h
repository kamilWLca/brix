// DEP.h: interface for the DEP class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(_DEP_H_INCLUDED_)
#define _DEP_H_INCLUDED_



#include "Porting_Classes/INXPoint.h"

#include "condat.h"
//#include "MainFrame.h"
#include "FSM.h" //@todo 
#include "LucidEnums.h" //@todo 
#include <vector> // stl class
#include "Porting_Classes/INXObjList.h"
#include "Porting_Classes/INXRect.h"
#include "Porting_Classes/INXString.h"



class DEP  
{
private:
	INXSize m_CanvasSize; //Handle canvas size being stretched beyond 2000,2000

public:
	DEP(); ////
	virtual ~DEP(); ////

	// Attributes
	INXObjList* condata;  // declare a list type to point to ConDat objects
	INXString depFilename;
	//HTREEITEM hItem;
	INXString projectDir;
	int projectNum;
	bool dbgMappedFlag;
		
	INXObjArray<INXString> *intTags, *boolTags, *realTags, *stringTags, *eventTags;

	//CMainFrame *pFrame;	
	FSM depFSM;

	// Methods
	
	//int AddBlockPort(INXString type, INXString portLabel, INXString blockName); 
	//INXPOSITION AddIcon(INXString csIconType, INXString csBlockName, INXPoint point, int iShow=1); // adds a new icon 
	//INXPOSITION AddIconObjectList(ConData* blob); //add all blobs using this function
	//void AddIconToGroup(INXPOSITION selectedIcon, int groupID);
	
	/*addLine is created only when new line is generated EXCLUDED temporarly
	int AddLine(INXPOSITION selectedIcon, int selectedPort, int selectedPortType, INXPOSITION selectedIcon2,int selectedPort2, int selectedPortType2) ;
	*/
	/* addNodes same as AddLine
	void AddNodes(INXPOSITION selectedIcon, int selectedPort, int selectedPortType, INXPOSITION selectedIcon2, int selectedPort2, int selectedPortType2);
	*/
	//int AddPort(ConData* blob, int iDataType, int iPortType, INXString portLabel);
	//int AddTag(INXPOSITION iconPos, int portNum, int portType);
	void AddTag2List(INXString tag, int dataType);
	//ConData* AddXPort(INXString type, INXString portLabel, INXPoint point);

	void AssignIconLineEndPoints();////

	//long CheckNewID(INXObjList* encapsulated, long int id);
	//void ConnectEncapsulatedIcon(INXPOSITION encapsulatedPos, INXRect selectedRect, INXObjList* oldDEP);
	//void CreateInstance(ConData* userDefBlob, int lib);
	//void DeleteIcon(INXPOSITION number, bool bDelOutputs); // 
	//void DeleteLine(INXPOSITION selectedControl,int PortSelected, int portType); 
	//INXString DeletePort(ConData* blob, int portNum, int portType); 
	//void DeleteTag(INXPOSITION iconPos, int portNum, int portType); 
//	void Draw(CDC * pDC);
//	void Draw(CDC * pDC, bool _onlyDrawAnim, int _toggleAnim);
	
	void DrawGL();////
	
	void DrawGL(bool _onlyDrawAnim, int _toggleAnim);////
	
	//INT_PTR EditControl(INXPOSITION selected); //default is that control is assigned if neq 0
	//int GetFinishLineID(INXObjList* flattened, ConData* otherBlockIcon, ConData* flatBlob, int endNum, HTREEITEM hItem);
	
	ConData* GetIconFromID(long int id);//// 
	
	//int GetInLineID(ConData* blob, ConData* flatBlob, int inNum);
	//INXObjList* GetLine(long lineID, int dataType);
	//int GetOutLineID(INXObjList* flattened, ConData* otherBlockIcon, ConData* flatBlob, int outNum, HTREEITEM hItem);
	//int GetStartLineID(ConData* blob, ConData* flatBlob, int startNum);
	
	int HighlightSelected(INXRect selectRect); ////
	
	INXPOSITION Incontrol(INXPoint point);////

	//void setFBHighlight(INXPOSITION fbPos); 
	
	void InitTagLists();////
	
	//bool IsUniqueID(long id);

	void LoadProg(INXString Info);////

	//int OnLine(INXPoint point, POSITION* Icon, bool* nodeSel, int* portType);
	//If interrogateOnly is true, nothing is changed in DEP, ie node count, etc

	int OnLine(INXPoint point, INXPOSITION* Icon, bool* nodeSel, int* portType, const bool &interrogateOnly );////
	
	bool OnNode(INXPoint point, INXPoint node);////
	
	int OnConnect(INXPoint point,INXPOSITION* Icon,int * portType, int *portConnected) ;////
	
	LabLineSegmentTypeEnum IsOnLineSegment(INXPoint point, INXPoint node1, INXPoint node2);////
	
	//void ReassignIconIDs(INXObjList* encapsulated);
	//void RemoveTag(INXString tag, int dataType);
	//void RenewPosition(INXPOSITION selected, INXPoint point, INXPoint firstpoint);
	//INXRect RenewSelectedPos(INXPoint newpoint, INXPoint firstpoint, INXRect selectRect);
	//void SnapIconToGrid(INXPOSITION selected);

	void SaveProg(INXString Info);////

	//void SetCondataLineID(CObList* flattened);
	//void ShowLine(INXPOSITION iconPos, int portNum, int portType);
	//bool IsOutputPortConnected(ConData* pBlob, int iPortNum);
	//bool IsFinishPortConnected(ConData* pBlob, int iPortNum);
	//void RemoveConnectedInputTags(ConData* pBlob, int iPortNum);
	//void RemoveConnectedStartTags(ConData* pBlob, int iPortNum);
	//ConData* SubsetEncapsulate(INXRect encapsulate, int lib);
	//void TagLine(INXPOSITION iconPos, int portNum, int portType);
	//void TagPorts(ConData* encapsulatedIcon, INXObjList* encapsulated);
	//void TopdownEncapsulate();

	bool WithinEndpoints(INXPoint point, INXPoint start, INXPoint end);////

	//void SetView(CDrawProgView* _pView);

	//kwhite:canvassupport
	//void HideSelected(INXPOSITION m_ItemSelected);
	//void ShowSelected(INXPOSITION m_ItemSelected);
	//void HideSelectedPositions();
	//void ShowSelectedPositions();	
	//void InvertSelected();
	//void MoveObjectsWithinSelectedBox(INXPoint newpoint, INXPoint firstpoint);
	//void MoveExternallyConnectedLines(INXPoint newpoint, INXPoint firstpoint);

	//Handle canvas size being stretched beyond 2000,2000
	void setCanvasSize(INXSize CanvasSize);////

	void initCanvasSize();
	
	INXSize getCanvasSize();////

	INXPoint getInitScrollPos(INXRect clientRect);////
	
	//void deleteConData();
	//void setSaveSelectedFlag(bool flag);

	//void ResetXportConnected();
	//INXString GetPortLabel(INXPOSITION blockPos, int iPortNum, int iPortType);
	//void SetPortLabel(INXPOSITION blockPos, int iPortNum, int iPortType, INXString csPortLabel);
	//void SetPortLabel(INXString csBlockName, INXString csOldPortLabel, INXString csNewPortLabel);

	//void RenameSubsystem(INXString csOldInstName, INXString csNewInstName, HTREEITEM hUserdefItem);
	//void AddToLibrary(INXPOSITION blockPos, INXString csMenuName);
	//bool IsRectEmpty(INXRect selectRect);
	//ConData* getIconAtPoint(INXPoint point);

	// FB substitution methods
	//INXPOSITION getPosFromIcon(ConData* pIcon);

	// Connections
	//void dragConnections(INXPOSITION selectedIcon, int iSelectedPort, int iSelectedPortType, INXPOSITION selectedIcon2,int iSelectedPort2, int iSelectedPortType2);
	//void swapPortData(Port* pPort1, Port* pPort2);
	//void getConnectedLines(long iID, int iPortNum, int iPortType, vector<IconLines*> &linesVec);
	//void rerouteDraggedConnections(long iOldID, long iNewID, long iOldPortNum, long iNewPortNum, int iPortType);

	// Define Monitoring
	//void setAllMonitors(bool bSetClear);
	//void toggleMonitoredLine(int iPortType, long iOtherIconId, int iOtherPortNo);

	// Debug
	//void ResetAllDbgEvents();
	//void ResetAllDbgValues();
	//void MapLineID(INXObjList* flattened);
	//void PropagateDebugDataDown(ConData* pSubsysIcon);
//	void PropagateDebugDataUp(HTREEITEM hSubsys);

};

#endif // !defined(AFX_DEP_H__F9664C86_F68B_414F_9898_39AA1958B8EA__INCLUDED_)
