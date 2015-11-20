#include "CCameraManager.h"

CCameraManager::CCameraManager ( )
{
	
}

CCameraManager::~CCameraManager ( )
{
	
}

bool CCameraManager::Add ( tagCameraData* pData, int iID )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagCameraData* ptr = NULL;
	ptr = ( tagCameraData* ) m_List.Get ( iID );
			
	if ( ptr != NULL )
		m_List.Delete ( iID );
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagCameraData* test;
	test = new tagCameraData;

	memset ( test,     0, sizeof ( test          ) );
	memcpy ( test, pData, sizeof ( tagCameraData ) );

	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	return true;
}

bool CCameraManager::Delete ( int iID )
{
	m_List.Delete ( iID );

	return true;
}

tagCameraData* CCameraManager::GetData ( int iID )
{		
	return ( tagCameraData* ) m_List.Get ( iID );
}

void CCameraManager::Update ( void )
{
	
}