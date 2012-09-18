// Coninfo.cpp: implementation of the Coninfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "DrawProg.h"
#include "ConDataTempStore.h"
#include "LucidConstants.h"
#include "FileOperations.h"

#include <fstream>
using namespace std;
#include "string.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// global variable declared in CDrawProgApp
extern char workDir[WORK_DIR_SIZE];

//main
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*

*: Is a serachable key to identify code areas where further attention required
This is used to define 
connections between icons, where one icon will need to be able to identify another.
This method is likely to cause some issues when subfunction blocks are introduced into programmes, because
Unique identifiers must be ensured. I guess this will be done by the subfunction loading programming checking all the loaded identities, and 
re-assignaing if necessary. It will also mean  that we can cross this bridge when we get to it. I haven't yet thought of a better solution! All comments welcome!
*/
//static long int uniqueidgenerator; // one copy for all instances
//This variable will be set to 0 when the programme is initialised, though we don't care what it it as long as it is unique.

/*
Constructor assigns defeualt values to attributes
*/

void ConDataTempStore::loadFromProject( const ConData &sourceData )
{
	m_iParamNum = sourceData.iParamNum;
	m_iUserDefined = sourceData.m_iUserDefined;

	m_csType				= sourceData.m_csIconType;
	m_csBlock				= sourceData.m_csBlockName;
	m_csDescription				= sourceData.description;
	m_csLongDesc				= sourceData.longDesc;
	m_csClassName				= sourceData.className;
	m_csOptionstring				= sourceData.optionstring;
	m_cSize				= sourceData.size;

	m_iInstNum				= sourceData.instNum;
	m_csHierarchyName				= sourceData.hierarchyName;
	m_bSaveSelectedFlag				= sourceData.saveSelectedFlag;
	m_cRectangle				= sourceData.rectangle; 


	for (UINT i=1; i<sourceData.iParamNum; i++) {
		if( (m_pParamStoreArr[i]==NULL) && (sourceData.iconParam[i] != NULL) )
			m_pParamStoreArr[i] = new ParameterStore;

		if( sourceData.iconParam[i] != NULL) 
			(m_pParamStoreArr[i])->copy(sourceData.iconParam[i]);
	}

	return;
}

ConDataTempStore::ConDataTempStore( )
{
	for(size_t i=0;i<MAXPARAMS;i++)
		m_pParamStoreArr[i] = NULL;
}

ConDataTempStore::~ConDataTempStore()
{
	for(size_t i=0;i<MAXPARAMS;i++)
		if( m_pParamStoreArr[i] != NULL )
			delete m_pParamStoreArr[i];
}

void ConDataTempStore::commitToProject( ConData *sourceData )
{
	//m_iParamNum = sourceData.iParamNum;
	//m_iUserDefined = sourceData.userDefined;

	//m_csType				= sourceData.type;
	//m_csBlock				= sourceData.block;
	sourceData->description = m_csDescription;
	sourceData->longDesc = m_csLongDesc;
	sourceData->className = m_csClassName;
	sourceData->optionstring = m_csOptionstring;
	//m_cSize				= sourceData.size;

	sourceData->instNum = m_iInstNum;
	sourceData->hierarchyName = m_csHierarchyName;
	sourceData->saveSelectedFlag = m_bSaveSelectedFlag;
	sourceData->rectangle = m_cRectangle;


	for (UINT i=1; i<sourceData->iParamNum; i++) {
		if( sourceData->iconParam[i] != NULL) 
			(sourceData->iconParam[i])->copy(m_pParamStoreArr[i]);
	}

	return;
}