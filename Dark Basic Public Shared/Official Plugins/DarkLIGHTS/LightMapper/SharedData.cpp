#include "SharedData.h"

SharedData::SharedData( )
{
	strcpy ( pStatus, "" );
	pMutex = CreateMutex( NULL, FALSE, NULL );
	pTextureLockMutex = CreateMutex( NULL, FALSE, NULL );
	bComplete = false;
	bTerminate = false;
	fPercentage = 0.0f;
}

SharedData::~SharedData( )
{
	CloseHandle( pMutex );
	CloseHandle( pTextureLockMutex );
}

bool SharedData::SetStatus( char *pMessage, float percent )
{
	DWORD dwResult = WaitForSingleObject( pMutex, 3000 );

	if ( dwResult == WAIT_OBJECT_0 )
	{
		strcpy_s( pStatus, 255, pMessage );
		fPercentage = percent;

		ReleaseMutex( pMutex );

		return true;
	}

	return false;
}

bool SharedData::GetStatus( char *pOutStr )
{
	DWORD dwResult = WaitForSingleObject( pMutex, 3000 );

	if ( dwResult == WAIT_OBJECT_0 )
	{
		strcpy_s( pOutStr, 255, pStatus );
		
		ReleaseMutex( pMutex );

		return true;
	}

	return false;
}

void SharedData::SetComplete( bool complete )
{
	bComplete = complete;
}

bool SharedData::GetComplete( )
{
	return bComplete;
}

float SharedData::GetPercent( )
{
	return fPercentage;
}

void SharedData::SetTerminate( bool terminate )
{
	bTerminate = terminate;
}

bool SharedData::GetTerminate( )
{
	return bTerminate;
}

bool SharedData::GrabTextureLockMutex( )
{
	DWORD dwResult = WaitForSingleObject( pTextureLockMutex, 5000 );

	if ( dwResult == WAIT_OBJECT_0 )
	{
		return true;
	}

	return false;
}

void SharedData::ReleaseTextureLockMutex( )
{
	ReleaseMutex( pTextureLockMutex );
}