#pragma once
#include "LucidEnums.h"
//#include "stdafx.h"
#include "Porting_Classes/INXString.h"
#include <vector>

class ProjectMetaData;

class LccPmdXfers 
{
public:
	LccPmdXfers(void);
	~LccPmdXfers(void);

	enum TypesEnum { 
		kNothing=0,
		kLayouts=1,
		kImages=2,
		kData=4,
		kMisc=8,
		kFonts=16,
		kNls=32,
		kResource=64,
		kDescription=128
	};

	struct DataPair_t {
		TypesEnum type;
		INXString hostFileName;
	};

public:
	int getCount( const TypesEnum &type );
	int getCount();

	LucidErrEnum addItem( const TypesEnum &type ,  const INXString &hostFileName );

	//LucidErrEnum removeItem( const TypesEnum &type ,  INXString value );

	int getItems( const TypesEnum &type, std::vector<DataPair_t> &items );

	DataPair_t getItem( const int &index );
	//int inflateTransferrableData( 
	//						std::vector< LccPmdXfers::DataPair_t  > &vFullPaths, 
	//						  const bool &updatedOnly,
	//						  ProjectMetaData *pProjMetaData );

	void clear();
	void clear( const TypesEnum &type );

private:
	std::vector< DataPair_t > m_vContents;
	//int getOverallIndex( const TypesEnum type ,  const INXString &value );
	//LucidErrEnum getItem( const TypesEnum &type , const int &index, INXString &value );



	//bool alreadyExists( const TypesEnum &type ,  INXString value );


};
