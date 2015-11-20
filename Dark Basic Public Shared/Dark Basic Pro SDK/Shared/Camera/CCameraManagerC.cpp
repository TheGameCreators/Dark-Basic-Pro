#include "CCameraManagerc.h"
#include "CCameraDatac.h"

// 20120311 IanM - Use std::map instead of cData class.
//               - Removal of dead code.

CCameraManager::CCameraManager ( )
{
	
}

CCameraManager::~CCameraManager ( )
{
	for (mitStore p = m_List.begin(); p != m_List.end(); ++p)
	{
		delete p->second;
	}
	m_List.clear();
}

bool CCameraManager::Add ( tagCameraData* pData, int iID )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	mitStore p = m_List.find( iID );
	if (p != m_List.end())
	{
		delete p->second;
		m_List.erase( p );
	}

	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagCameraData* test = new tagCameraData;
	memcpy ( test, pData, sizeof ( tagCameraData ) );
	m_List.insert( std::make_pair( iID, test ) );
	///////////////////////////////////////

	return true;
}

bool CCameraManager::Delete ( int iID )
{
	mitStore p = m_List.find( iID );
	if (p != m_List.end())
	{
		delete p->second;
		m_List.erase( p );
	}

	return true;
}

tagCameraData* CCameraManager::GetData ( int iID )
{
	mitStore p = m_List.find( iID );
	if (p != m_List.end())
	{
		return p->second;
	}

	return 0;
}

int CCameraManager::GetNextID ( int iIDHave )
{
	mitStore p = m_List.upper_bound( iIDHave );
	if (p != m_List.end())
	{
		return p->first;
	}

	return -1;
}
