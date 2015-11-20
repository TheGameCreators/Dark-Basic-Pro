// include definition
#include "csoundmanagerc.h"

#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }

CSDKSoundManager::CSDKSoundManager ( )
{
	
}

CSDKSoundManager::~CSDKSoundManager ( )
{
}

bool CSDKSoundManager::Add ( sSoundData* pData, int iID )
{
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	sSoundData* ptr = NULL;
	ptr = ( sSoundData* ) m_List.Get ( iID );
			
	if ( ptr != NULL )
		m_List.Delete ( iID );
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	sSoundData* test;
	test = new sSoundData;

	memset ( test,     0, sizeof ( test       ) );
	memcpy ( test, pData, sizeof ( sSoundData ) );

	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////


	return true;
}

bool CSDKSoundManager::Delete ( int iID )
{
	sSoundData* ptr = NULL;
	ptr = this->GetData ( iID );

	if(ptr)
	{
		SAFE_DELETE ( ptr->pSound );
		SAFE_RELEASE ( ptr->pDSBuffer3D );
		delete ptr;
	}

	m_List.Delete ( iID );

	return true;
}

sSoundData* CSDKSoundManager::GetData ( int iID )
{		
	return ( sSoundData* ) m_List.Get ( iID );
}

void CSDKSoundManager::ShutDown ( void )
{
	link* check = m_List.m_start;

	for ( int temp = 0; temp < m_List.m_count; temp++ )
	{
		sSoundData* ptr = NULL;
		ptr = this->GetData ( check->id );
		if ( ptr == NULL ) continue;

		SAFE_DELETE ( ptr->pSound );
		SAFE_RELEASE ( ptr->pDSBuffer3D );

		delete ptr;

		check = check->next;
	}
}

void CSDKSoundManager::Update ( void )
{
	link* check = m_List.m_start;

	for ( int temp = 0; temp < m_List.m_count; temp++ )
	{
		sSoundData* ptr = NULL;
		ptr = this->GetData ( check->id );
		if ( ptr == NULL ) continue;
		
		check = check->next;
	}
}