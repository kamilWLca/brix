//#include "stdafx.h"
#include "LccPmdXfers.h"
#include "LucidEnums.h"
#include "../common/ProjectMetaData.h"
#include "../common/LccTimeStamp.h"

//#include "atltime.h"

#include <cassert>
#include <vector>

LccPmdXfers::LccPmdXfers()
{
}

LccPmdXfers::~LccPmdXfers()
{
}



int LccPmdXfers::getCount( const LccPmdXfers::TypesEnum &type )
{
	int count = 0;

	for(size_t i=0; i<m_vContents.size();i++){
		if(m_vContents[i].type == type)
			count++;

	} // for(size_t i=0; i<m_vContents.size();i++)

	return count;
}

int LccPmdXfers::getCount( )
{
	return m_vContents.size();
}

LccPmdXfers::DataPair_t LccPmdXfers::getItem( const int &index )
{
	assert( m_vContents.size() > index );
	return m_vContents[index];
}

int LccPmdXfers::getItems( const LccPmdXfers::TypesEnum &type, std::vector<DataPair_t> &items )
{
	items.clear();
	size_t i = 0;

	for(size_t i=0; i<m_vContents.size();i++){
		if(m_vContents[i].type == type){
			items.push_back( m_vContents[i] );
		}
	} // for(size_t i=0; i<m_vContents.size();i++)

	return 	items.size();
}
/*
int LccPmdXfers::getOverallIndex( const LccPmdXfers::TypesEnum type ,  const INXString &value )
{
	for(size_t i=0; i<m_vContents.size();i++){
		if(m_vContents[i].type == type){
			if(m_vContents[i].location == value ){
				return i;
			}
		}
	} // for(size_t i=0; i<m_vContents.size();i++)

	return 	-1;
}
*/
/*
LucidErrEnum LccPmdXfers::removeItem( const LccPmdXfers::TypesEnum &type ,  INXString value )
{
	int overallIndex = getOverallIndex( type, value );

	if( -1 == overallIndex){
		assert( 1==0 );			// catch this big time in debug mode
		return kErr_TransferrableNotFound;
	}else{
		std::vector<Pair_t> tmp;

		for(size_t i=0; i<overallIndex; i++ )
			tmp.push_back( m_vContents[i] );

		for(size_t i=overallIndex+1; i<m_vContents.size(); i++ )
			tmp.push_back( m_vContents[i] );

		m_vContents.clear();
		m_vContents = tmp;

		return kErr_NoErr;
	}
}
*/
LucidErrEnum LccPmdXfers::addItem( 
						const TypesEnum &type ,  
						const INXString &hostFileName )
{
	DataPair_t dum;
	dum.hostFileName = hostFileName;
	dum.type = type;
	m_vContents.push_back( dum );

	return kErr_NoErr;
}
/*
	int overallIndex = getOverallIndex( type, value );

	if( -1 != overallIndex){
		assert( 1==0 ); // catch this big time in debug mode
		return kErr_TransferrableAlreadyExists;
	}else{
		DataPair_t dum;
		dum.hostLocation = hostLocation;
		dum.targetFileName = tgtFileName;
		dum.type = type;
		m_vContents.push_back( dum );
		return kErr_NoErr;
	}
}
*/
void LccPmdXfers::clear( const LccPmdXfers::TypesEnum &type )
{
	std::vector<DataPair_t> tmp;

	for(size_t i=0; i<m_vContents.size();i++){
		if(m_vContents[i].type != type){
			tmp.push_back( m_vContents[i]);
		}
	} // for(size_t i=0; i<m_vContents.size();i++)

	m_vContents.clear();
	m_vContents = tmp;
}

void LccPmdXfers::clear()
{
	m_vContents.clear();
}

