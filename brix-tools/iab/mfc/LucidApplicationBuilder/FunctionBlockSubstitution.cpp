#include "stdafx.h"
#include "FunctionBlockSubstitution.h"

FunctionBlockSubstitution::FunctionBlockSubstitution(void)
{
	pInitDraggee = new ConData;
	m_Droppee = NULL;
	m_Draggee = NULL;
}

FunctionBlockSubstitution::~FunctionBlockSubstitution(void)
{
	delete pInitDraggee;
}

void FunctionBlockSubstitution::createInitDraggee(INXString type, INXString block)
{
	INXPoint point(0,0);

	pInitDraggee->init(type, block, point, 1);
	m_Draggee = pInitDraggee;
}

void FunctionBlockSubstitution::setDraggee(ConData* draggee)
{
	m_Draggee = draggee;
}

void FunctionBlockSubstitution::setDroppee(ConData* droppee)
{
	m_Droppee = droppee;
}

ConData* FunctionBlockSubstitution::getDroppee()
{
	return m_Droppee;
}

void FunctionBlockSubstitution::setDEP(DEP *_pDEP)
{
	pDEP = _pDEP;
}

// Function that returns true if the draggee has a greater number of input and output ports
// for each data type, and start and finish ports, than the droppee
bool FunctionBlockSubstitution::isReplaceable(ConData *droppee)
{
	return (m_Draggee->startport_num >= droppee->startport_num &&
		m_Draggee->finishport_num >= droppee->finishport_num &&
		m_Draggee->getPortCount(INPUTPORT, INTEGER) >= droppee->getPortCount(INPUTPORT, INTEGER) &&
		m_Draggee->getPortCount(INPUTPORT, FLOAT) >= droppee->getPortCount(INPUTPORT, FLOAT) &&
		m_Draggee->getPortCount(INPUTPORT, STRING) >= droppee->getPortCount(INPUTPORT, STRING) &&
		m_Draggee->getPortCount(INPUTPORT, BOOLEAN) >= droppee->getPortCount(INPUTPORT, BOOLEAN) &&
		m_Draggee->getPortCount(OUTPUTPORT, INTEGER) >= droppee->getPortCount(OUTPUTPORT, INTEGER) &&
		m_Draggee->getPortCount(OUTPUTPORT, FLOAT) >= droppee->getPortCount(OUTPUTPORT, FLOAT) &&
		m_Draggee->getPortCount(OUTPUTPORT, STRING) >= droppee->getPortCount(OUTPUTPORT, STRING) &&
		m_Draggee->getPortCount(OUTPUTPORT, BOOLEAN) >= droppee->getPortCount(OUTPUTPORT, BOOLEAN));
}


// Method that points the dragee to the ports on the droppee.
// This way all existing connections are maintained
void FunctionBlockSubstitution::connectDraggee()
{
	UINT j=0;
	INXString csPortDesc;
	INXPoint newPoint;
	Port* tmpPort;
	INXObjArray<unsigned int> *tmpDraggeeFuncArg, *tmpDroppeeFuncArg;
	INXObjArray<INXString>* tmpDraggeeFuncName, *tmpDroppeeFuncName;

	for (UINT i=0; i<m_Draggee->startport_num; i++) {
		if (i<m_Droppee->startport_num) {
			csPortDesc = m_Draggee->startport[i]->description;
			newPoint = m_Draggee->startport[i]->P;
			tmpPort = m_Draggee->startport[i];
			tmpDraggeeFuncName = m_Draggee->startport[i]->funcName;
			tmpDroppeeFuncName = m_Droppee->startport[i]->funcName;			
			m_Draggee->startport[i] = m_Droppee->startport[i];
			m_Draggee->startport[i]->funcName = tmpDraggeeFuncName;
			m_Draggee->startport[i]->description = csPortDesc;
			m_Draggee->startport[i]->Move(m_Droppee->startport[i]->P - newPoint);
			m_Droppee->startport[i] = tmpPort;
			m_Droppee->startport[i]->funcName = tmpDroppeeFuncName;
		}
	}
	
	for (UINT i=0; i<m_Draggee->inputport_num; i++) {
		if (j<m_Droppee->inputport_num && m_Draggee->inputport[i]->datatype == m_Droppee->inputport[j]->datatype) {
			csPortDesc = m_Draggee->inputport[i]->description;
			newPoint = m_Draggee->inputport[i]->P;
			tmpPort = m_Draggee->inputport[i];
			tmpDraggeeFuncArg = m_Draggee->inputport[i]->funcArg;
			tmpDroppeeFuncArg = m_Droppee->inputport[j]->funcArg;
			tmpDraggeeFuncName = m_Draggee->inputport[i]->funcName;
			tmpDroppeeFuncName = m_Droppee->inputport[j]->funcName;			
			m_Draggee->inputport[i] = m_Droppee->inputport[j];
			m_Draggee->inputport[i]->funcArg = tmpDraggeeFuncArg;
			m_Draggee->inputport[i]->funcName = tmpDraggeeFuncName;
			m_Draggee->inputport[i]->description = csPortDesc;
			m_Draggee->inputport[i]->Move(m_Droppee->inputport[j]->P - newPoint);
			m_Droppee->inputport[j] = tmpPort;
			m_Droppee->inputport[j]->funcArg = tmpDroppeeFuncArg;
			m_Droppee->inputport[j]->funcName = tmpDroppeeFuncName;
			j++;
		}
	}

	for (UINT i=0; i<m_Draggee->finishport_num; i++) {
		if (i<m_Droppee->finishport_num) {
			csPortDesc = m_Draggee->finishport[i]->description;
			newPoint = m_Draggee->finishport[i]->P;
			tmpPort = m_Draggee->finishport[i];
			tmpDraggeeFuncArg = m_Draggee->finishport[i]->funcArg;
			tmpDroppeeFuncArg = m_Droppee->finishport[i]->funcArg;
			tmpDraggeeFuncName = m_Draggee->finishport[i]->funcName;
			tmpDroppeeFuncName = m_Droppee->finishport[i]->funcName;
			m_Draggee->finishport[i] = m_Droppee->finishport[i];
			m_Draggee->finishport[i]->funcArg = tmpDraggeeFuncArg;
			m_Draggee->finishport[i]->funcName = tmpDraggeeFuncName;
			m_Draggee->finishport[i]->description = csPortDesc;
			m_Draggee->finishport[i]->Move(m_Droppee->finishport[i]->P - newPoint);
			m_Droppee->finishport[i] = tmpPort;
			m_Droppee->finishport[i]->funcArg = tmpDroppeeFuncArg;
			m_Droppee->finishport[i]->funcName = tmpDroppeeFuncName;
		}
	}

	j=0;
	for (UINT i=0; i<m_Draggee->outputport_num; i++) {
		if (j<m_Droppee->outputport_num && m_Draggee->outputport[i]->datatype == m_Droppee->outputport[j]->datatype) {
			csPortDesc = m_Draggee->outputport[i]->description;
			newPoint = m_Draggee->outputport[i]->P;
			tmpPort = m_Draggee->outputport[i];
			tmpDraggeeFuncArg = m_Draggee->outputport[i]->funcArg;
			tmpDroppeeFuncArg = m_Droppee->outputport[j]->funcArg;
			tmpDraggeeFuncName = m_Draggee->outputport[i]->funcName;
			tmpDroppeeFuncName = m_Droppee->outputport[j]->funcName;
			m_Draggee->outputport[i] = m_Droppee->outputport[j];
			m_Draggee->outputport[i]->funcArg = tmpDraggeeFuncArg;
			m_Draggee->outputport[i]->funcName = tmpDraggeeFuncName;
			m_Draggee->outputport[i]->description = csPortDesc;
			m_Draggee->outputport[i]->Move(m_Droppee->outputport[j]->P - newPoint);
			m_Droppee->outputport[j] = tmpPort;
			m_Droppee->outputport[j]->funcArg = tmpDroppeeFuncArg;
			m_Droppee->outputport[j]->funcName = tmpDroppeeFuncName;

			reassignOtherPortNo(j, i, m_Droppee->identnum);
			j++;
		}
	}
}

// method that reassigns the attribute 'othericonid' associated with line objects connected to outputs
void FunctionBlockSubstitution::reassignOtherPortNo(int iOldPortNum, int iNewPortNum, long iFBID)
{
	INXPOSITION pos;
	ConData* pBlob;

	pos = pDEP->condata->GetHeadPosition();
	while (pos) {
		pBlob = (ConData *) (pDEP->condata->GetNext(pos));
		for (UINT j=0; j<pBlob->inputport_num; j++) {
			if (pBlob != m_Droppee) {
				if (pBlob->inputport[j]->line.othericonid == iFBID && pBlob->inputport[j]->line.otherportno == iOldPortNum) {
					pBlob->inputport[j]->line.otherportno = iNewPortNum;
				}
			}
		}
	}
}

// Method that returns true if the draggee and droppee have identical parameter names
bool FunctionBlockSubstitution::isSameParamNames()
{
	bool bRet = TRUE;

	if (m_Draggee->iParamNum == m_Droppee->iParamNum) {
		for (UINT i=1; i<m_Draggee->iParamNum; i++) {
			if (m_Draggee->iconParam[i]->name != m_Droppee->iconParam[i]->name) {
				bRet = FALSE;
			}
		}
	}
	else {
		bRet = FALSE;
	}

	return bRet;
}

// Method that copies the parameter values if the parameter names are identical
// returns 1 if successful and 0 otherwise
int FunctionBlockSubstitution::copyParamValues()
{
	INXString csTmpParamVal;

	if (isSameParamNames()) {
		for (UINT i=1; i<m_Draggee->iParamNum; i++) {
			// Can't just assign droppee parameters to dragee
			// Need to swap parameter values especially for widgets, because want to delete 
			// draggee tags from project meta-data and keep droppee tags when the droppee is deleted.
			csTmpParamVal = m_Draggee->iconParam[i]->value;
			m_Draggee->iconParam[i]->value = m_Droppee->iconParam[i]->value;
			m_Droppee->iconParam[i]->value = csTmpParamVal;
		}
		return 1;
	}
	else {
		return 0;
	}
}

// Returns true if the user has modified the droppee parameters
bool FunctionBlockSubstitution::isParamValsModified()
{
	ConData* pBlob = new ConData;
	INXPoint point(0,0);
	bool bRet = FALSE;

	// Check if the parameters in Droppee differ from the defaults
	// The defaults can be obtained by reading the IDF into a temporary condat object
	if (!m_Droppee->m_iUserDefined) {
		pBlob->init(m_Droppee->m_csIconType, "", point, 1);
		for (UINT i=1; i<m_Droppee->iParamNum; i++) {
			if ((CString)pBlob->iconParam[i]->value = "") {
				pBlob->iconParam[i]->value = "_";
			}
			if (m_Droppee->iconParam[i]->value != pBlob->iconParam[i]->value) {
				bRet = TRUE;
			}
		}
	}

	delete pBlob;
	return bRet;
}