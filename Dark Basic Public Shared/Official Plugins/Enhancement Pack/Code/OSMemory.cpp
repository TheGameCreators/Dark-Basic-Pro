
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	MEMORY COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#include "stdafx.h"
#include <math.h>

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK int GetInstalledMemory   ( int iReturn );
DARKSDK int GetMemoryAvailable   ( int iReturn );
DARKSDK int GetMemoryPercentUsed ( void );
DARKSDK int GetMemoryPercentFree ( void );

/*
	// export names for string table -

	GET INSTALLED MEMORY[%LL%?GetInstalledMemory@@YAHH@Z%
	GET MEMORY AVAILABLE[%LL%?GetMemoryAvailable@@YAHH@Z%
	GET MEMORY PERCENT USED[%L%?GetMemoryPercentUsed@@YAHXZ%
	GET MEMORY PERCENT FREE[%L%?GetMemoryPercentFree@@YAHXZ%
*/

int GetInstalledMemory ( int iReturn )
{
	MEMORYSTATUS memoryStatus = { 0 };
	
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	
	::GlobalMemoryStatus ( &memoryStatus );
	
	if ( iReturn == 0 )
		return ( int ) ceil ( ( float ) memoryStatus.dwTotalPhys / 1024 );
	
	if ( iReturn == 1 )
		return ( int ) ceil ( ( float ) memoryStatus.dwTotalPhys / 1024 / 1024 );
	
	if ( iReturn == 2 )
		return ( int ) ceil ( ( float ) memoryStatus.dwTotalPhys / 1024 / 1024 / 1024 );
	
	return 0;
}

int GetMemoryAvailable ( int iReturn )
{
	MEMORYSTATUS memoryStatus = { 0 };
	
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	
	::GlobalMemoryStatus ( &memoryStatus );

	if ( iReturn == 0 )
		return ( int ) ceil ( ( float ) memoryStatus.dwAvailPhys / 1024 );
	
	if ( iReturn == 1 )
		return ( int ) ceil ( ( float ) memoryStatus.dwAvailPhys / 1024 / 1024 );
	
	if ( iReturn == 2 )
		return ( int ) ceil ( ( float ) memoryStatus.dwAvailPhys / 1024 / 1024 / 1024 );
	
	return 0;
}

int GetMemoryPercentUsed ( void )
{
	MEMORYSTATUS memoryStatus = { 0 };
	
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	
	::GlobalMemoryStatus ( &memoryStatus );

	return memoryStatus.dwMemoryLoad;
}

int GetMemoryPercentFree ( void )
{
	MEMORYSTATUS memoryStatus = { 0 };
	
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	
	::GlobalMemoryStatus ( &memoryStatus );

	return 100 - memoryStatus.dwMemoryLoad;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
