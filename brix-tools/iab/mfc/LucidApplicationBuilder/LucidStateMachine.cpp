// LucidStateMachine.cpp: implementation of the CLucidStateMachine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawProg.h"
#include "condat.h"
#include "LucidStateMachine.h"
#include "LucidStateMachineDialog.h"
#include "Parameter.h"
#include "DEP.h"
#include "BlockOperations.h"
#include "sizecbar.h"
#include "Porting_Classes/INXObject.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CLucidStateMachine::CLucidStateMachine()
{

}

CLucidStateMachine::~CLucidStateMachine()
{

}

bool CLucidStateMachine::placeComponents(INXPoint point, DEP* pDEP, Project* pProject)
{
		
	LucidStateMachineDialog lsmdialog;
	int NumTransitionToEdit =8;
	INXString csStateEdit = "";
	INXString csBitmapNameAppend="";
	bool bTransitionAction = false;
	bool bEntryAction = false;
	INXString csProjectDir;

	pProject->pProjMData->getProjectDir(csProjectDir);
	if(lsmdialog.DoModal() == IDOK)
	{
		NumTransitionToEdit	= atoi(lsmdialog.m_csTransitionsTo);
		csStateEdit = lsmdialog.m_csState;
		bEntryAction = (lsmdialog.m_bEntryActionCheckBox != 0);
		bTransitionAction = (lsmdialog.m_bTransitionActionCheck != 0);


		if(lsmdialog.GetRadioCheck() == 0)
		{
			csBitmapNameAppend ="EVENT";
		}
		
	}
	else 
	{	return false;
	}


	if(NumTransitionToEdit>0)
	{

		INXString csBitmapName = "SFCBARGROUP8";
		INXString csIdfName = "";
		INXString blockName = "Logic";
		ConData* pLogicIconData;
		ConData* pSFCBarGroupIconData;
		BlockOperations bo;

		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;


		int nlogicPositionInc = 95;



		INXPoint pointBarGroup = point;
		pointBarGroup.y += 100;

		if((NumTransitionToEdit>8)&&(NumTransitionToEdit<13))
		{
			csBitmapName = "SFCBARGROUP12";
			//nlogicPositionInc = 75;
		}
		else if((NumTransitionToEdit>4)&&(NumTransitionToEdit<9))
		{
			csBitmapName = "SFCBARGROUP8";		
		}
		else if(NumTransitionToEdit==4)
		{
			csBitmapName = "SFCBARGROUP4";	
		}
		else if(NumTransitionToEdit==3)
		{
			csBitmapName = "SFCBARGROUP3";		
		}
		else if(NumTransitionToEdit==2)
		{
			csBitmapName = "SFCBARGROUP2";		
		}
		else if(NumTransitionToEdit==1)
		{
			csBitmapName = "SFCBAR";		
		}
		else
		{
			csBitmapName = "SFCBARGROUP12";	
			NumTransitionToEdit = 12;

		}
		
		csBitmapName += csBitmapNameAppend;

		INXPOSITION selected = pDEP->AddIcon(csBitmapName, csIdfName, pointBarGroup);  //create memory and set point
		pSFCBarGroupIconData = (ConData *)pDEP->condata->GetAt(selected);

		point.x = point.x +25;

		INXPoint pointFirstLogicIcon = point;
		
		
			
		for(int x=0; x<NumTransitionToEdit; x++)
		{
		

			if(csBitmapNameAppend =="")
			{
				csBitmapName = "CONDITIONLOGIC";

				selected = pDEP->AddIcon("CONDITIONLOGIC", "", point);  //create memory and set point
				pLogicIconData = (ConData *) pDEP->condata->GetAt(selected);
				// the class name for a new block is the block name
				if (blockName != "") {
					pLogicIconData->className = blockName;
				}
				// create an instance for encapsulated blocks
			//	if (csIdfName != "") {
					
					pDEP->CreateInstance(pLogicIconData,3);
			//	}

				pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(pLogicIconData,pProject, pDEP->hItem);

				

				pSFCBarGroupIconData->AddLine(x+1, STARTPORT, pLogicIconData,0, FINISHPORT);
				pSFCBarGroupIconData->AddLine(x, INPUTPORT, pLogicIconData,0, OUTPUTPORT);
		
				AddConditionLogicPortsBelow(pLogicIconData,  pDEP, pProject);
/*
				BlockOperations bo;
	
				INXString filename = pProject->projectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + "\\" + pLogicIconData->description+ ".prg";
				loaded = bo.NewBlock(filename);
				
				ConData *blob = new ConData;
				blob->init("XOUTPUTB","", pLogicIconData->CalculateXPortPosition(OUTPUTPORT) );
				blob->description = pLogicIconData->outputport[0]->description;
				loaded->AddTail((CObject*) blob);
			//	bo.SaveBlock(filename, loaded);
	
				
				ConData *blob2 = new ConData;
				blob2->init("XFINISH","", pLogicIconData->CalculateXPortPosition(FINISHPORT)  );
				blob2->description = pLogicIconData->finishport[0]->description;
				loaded->AddTail((CObject*) blob2);
				bo.SaveBlock(filename, loaded);
*/
			}


		
			if(bTransitionAction)
			{
				ConData* pTransitionIconData ;


				selected = pDEP->AddIcon("TRANSITIONACTION", "", (point+INXPoint(0,160)));  //create memory and set point
				pTransitionIconData = (ConData *) pDEP->condata->GetAt(selected);

				pTransitionIconData->className = "Transition_Action" ;
				pDEP->CreateInstance(pTransitionIconData,3);

				pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(pTransitionIconData,pProject, pDEP->hItem);

				pTransitionIconData->AddLine(0,STARTPORT, pSFCBarGroupIconData,x, FINISHPORT);




				AddActionPortsBelow(pTransitionIconData,  pDEP,  pProject);

/*
				INXString filename = pProject->projectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + "\\" + pTransitionIconData->description+ ".prg";
				loaded = bo.NewBlock(filename);
			
			
				ConData *blob = new ConData;
				blob->init("XSTART","", pTransitionIconData->CalculateXPortPosition(STARTPORT) );
				blob2->description = pLogicIconData->startport[0]->description;
				loaded->AddTail((CObject*) blob);
			
	
				
				ConData *blob2 = new ConData;
				blob2->init("XFINISH","",  pTransitionIconData->CalculateXPortPosition(FINISHPORT) );
				blob2->description = pLogicIconData->finishport[0]->description;
				loaded->AddTail((CObject*) blob2);
				bo.SaveBlock(
				, loaded);
*/


			}

			point.x = point.x + nlogicPositionInc;
			
		}
		
		pointBarGroup.y = pointBarGroup.y -260;
		pointBarGroup.x = pointBarGroup.x -30;

		csBitmapName = "STATE";
		
		
	
		ConData* pStateIconData ;


		selected = pDEP->AddIcon("STATE", "", pointBarGroup);  //create memory and set point
	
		pStateIconData = (ConData *) pDEP->condata->GetAt(selected);

	//	INXString e;
	//	e.Format("%d",sizeof(pStateIconData->iParamNum));
		//e.Format("%d",sizeof(pStateIconData->iconParam));
	//	AfxMessageBox(e);
		

		
		pStateIconData->iconParam[1]->value = /*"State:" +*/ csStateEdit;



		if(bEntryAction)
		{
			ConData* pEntryActionIconData ;

			selected = pDEP->AddIcon("ENTRYACTION", "", (pointBarGroup + INXPoint(10,100)));  //create memory and set point
			pEntryActionIconData = (ConData *) pDEP->condata->GetAt(selected);

			pEntryActionIconData->className = "Entry_Action" ;
			pDEP->CreateInstance(pEntryActionIconData,3);

			pFrame->m_wndProjectBar.m_cProjTree.AddItem2ProjectTree(pEntryActionIconData,pProject, pDEP->hItem);


			pEntryActionIconData->AddLine(0,STARTPORT, pStateIconData,0, FINISHPORT);
			pSFCBarGroupIconData->AddLine(0,STARTPORT, pEntryActionIconData,0, FINISHPORT);


			INXString filename = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + "\\" + pEntryActionIconData->description+ ".prg";
		
		

			AddActionPortsBelow( pEntryActionIconData,   pDEP,  pProject);
		
		

		}
		else
		{
			pSFCBarGroupIconData->AddLine(0,STARTPORT, pStateIconData,0, FINISHPORT);

		}

	}



	return true;
} 


void CLucidStateMachine::AddConditionLogicPortsBelow(ConData* pIconData,  DEP* pDEP, Project* pProject)
{


		BlockOperations bo;

		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		INXObjList* loaded;
		INXString csProjectDir;
		pProject->pProjMData->getProjectDir(csProjectDir);
		INXString path = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + "\\" + pIconData->description;
		INXString filename = path + ".prg";

		CreateDirectory(path,NULL);
//		CreateDirectory(workDir + USERDEFDIR + blockName, NULL);


		loaded = bo.NewBlock(filename);

		ConData *blob = new ConData;
		blob->init("XOUTPUTB","", pIconData->CalculateXPortPosition(OUTPUTPORT));
		blob->description = pIconData->outputport[0]->description;
		loaded->AddTail((INXObject*) blob);
		bo.SaveBlock(filename, loaded);

		
		ConData *blob2 = new ConData;
		blob2->init("XFINISH","", pIconData->CalculateXPortPosition(FINISHPORT));
		blob2->description = pIconData->finishport[0]->description;
		loaded->AddTail((INXObject*) blob2);
		bo.SaveBlock(filename, loaded);




}

void CLucidStateMachine::AddActionPortsBelow(ConData* pIconData,DEP* pDEP, Project* pProject)
{


		BlockOperations bo;

		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		INXObjList* loaded;
		INXString csProjectDir;
		pProject->pProjMData->getProjectDir(csProjectDir);
		INXString path = csProjectDir + DEPDIR + pFrame->m_wndProjectBar.m_cProjTree.GetDEPPath(pDEP->hItem) + pDEP->depFilename + "\\" + pIconData->description;
		INXString filename = path + ".prg";

		CreateDirectory(path,NULL);
		
		
		loaded = bo.NewBlock(filename);
	


		ConData *blob = new ConData;
		blob->init("XSTART","", pIconData->CalculateXPortPosition(STARTPORT));
		blob->description = pIconData->startport[0]->description;
		loaded->AddTail((INXObject*) blob);
		bo.SaveBlock(filename, loaded);
	
		ConData *blob2 = new ConData;
	//	loaded = bo.NewBlock(filename);
		blob2->init("XFINISH","",  pIconData->CalculateXPortPosition(FINISHPORT));
		blob2->description = pIconData->finishport[0]->description;
		loaded->AddTail((INXObject*) blob2);
		bo.SaveBlock(filename, loaded);


}