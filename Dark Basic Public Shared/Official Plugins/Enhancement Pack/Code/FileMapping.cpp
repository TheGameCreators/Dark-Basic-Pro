
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

#include "core.h"

#include <vector>

#include "ipc.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// GLOBALS /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

struct sFileMap
{
	//HANDLE hCreate;
	//HANDLE hOpen;
	//VOID*  pData;

	cIPC*  pIPC;
	char   szName [ 256 ];

	sFileMap ( )
	{
		pIPC = NULL;

		strcpy ( szName, "" );

		//hCreate = NULL;
		//hOpen   = NULL;
		//pData   = NULL;	
	}

	
};

sFileMap g_FileMap [ 256 ];

#define FILEMAP_ID_DWORD		0
#define FILEMAP_ID_STRING		1
#define FILEMAP_ID_FLOAT		2

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK void  CreateFileMap    ( int iID, DWORD dwName, DWORD dwSize );
DARKSDK void  OpenFileMap      ( int iID, DWORD dwName );
DARKSDK void  CloseFileMap     ( int iID );
DARKSDK void  DestroyFileMap   ( int iID );

DARKSDK DWORD GetFileMapDWORD  ( int iID, DWORD dwOffset );
DARKSDK DWORD GetFileMapString ( DWORD dwReturn, int iID, DWORD dwOffset );
DARKSDK DWORD GetFileMapFloat  ( int iID, DWORD dwOffset );

DARKSDK void  SetFileMapDWORD  ( int iID, DWORD dwOffset, DWORD dwValue  );
DARKSDK void  SetFileMapString ( int iID, DWORD dwOffset, DWORD dwString );
DARKSDK void  SetFileMapFloat  ( int iID, DWORD dwOffset, float fValue );

DARKSDK void  SetEventAndWait  ( int iID );
/*
	// export names for string table -

	CREATE FILE MAP%LSL%?CreateFileMap@@YAXHKK@Z%
	OPEN FILE MAP%LS%?OpenFileMap@@YAXHK@Z%
	CLOSE FILE MAP%L%?CloseFileMap@@YAXH@Z%
	DESTROY FILE MAP%L%?DestroyFileMap@@YAXH@Z%

  	SET FILE MAP DWORD%LLL%?SetFileMapDWORD@@YAXHKK@Z%
	SET FILE MAP STRING$%LLS%?SetFileMapString@@YAXHKK@Z%
	SET FILE MAP FLOAT%LLF%?SetFileMapFloat@@YAXHKM@Z%

	GET FILE MAP DWORD[%LLL%?GetFileMapDWORD@@YAKHK@Z%
	GET FILE MAP STRING$[%SLL%?GetFileMapString@@YAKKHK@Z%
	GET FILE MAP FLOAT[%FLL%?GetFileMapFloat@@YAKHK@Z%
*/

bool CheckFileMapID ( int iID )
{
	if ( iID < 0 || iID > 255 )
	{
		Error ( 7 );
		return false;
	}

	return true;
}

void CreateFileMap ( int iID, DWORD dwName, DWORD dwSize )
{
	if ( !CheckFileMapID ( iID ) )
		return;

	SAFE_DELETE ( g_FileMap [ iID ].pIPC );

	g_FileMap [ iID ].pIPC = new cIPC ( ( char* ) dwName, dwSize );

	strcpy ( g_FileMap [ iID ].szName, ( char* ) dwName );

	/*
	if ( g_FileMap [ iID ].hCreate )
	{
		Error ( 15 );
		return;
	}

	g_FileMap [ iID ].hCreate = CreateFileMapping ( ( HANDLE ) 0xFFFFFFFF, NULL, PAGE_READWRITE, 0, dwSize, ( char* ) dwName );
	*/
}

void OpenFileMap ( int iID, DWORD dwName )
{
	if ( !CheckFileMapID ( iID ) )
		return;

	SAFE_DELETE ( g_FileMap [ iID ].pIPC );

	g_FileMap [ iID ].pIPC = new cIPC ( ( char* ) dwName, 0 );

	strcpy ( g_FileMap [ iID ].szName, ( char* ) dwName );

	/*
	g_FileMap [ iID ].hOpen = OpenFileMapping ( FILE_MAP_ALL_ACCESS, TRUE, ( char* ) dwName );
	g_FileMap [ iID ].pData = MapViewOfFile   ( g_FileMap [ iID ].hOpen, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

	strcpy ( g_FileMap [ iID ].szName, ( char* ) dwName );

	CloseFileMap ( iID );
	*/
}

void CloseFileMap ( int iID )
{
	if ( !CheckFileMapID ( iID ) )
		return;

	/*
	UnmapViewOfFile ( g_FileMap [ iID ].pData );

	g_FileMap [ iID ].hCreate = NULL;
	g_FileMap [ iID ].hOpen   = NULL;
	g_FileMap [ iID ].pData   = NULL;
	*/
}

void DestroyFileMap ( int iID )
{
	if ( !CheckFileMapID ( iID ) )
		return;

	SAFE_DELETE ( g_FileMap [ iID ].pIPC );

	strcpy ( g_FileMap [ iID ].szName, "" );

	/*
	for ( int i = 0; i < 256; i++ )
	{
		CloseHandle ( g_FileMap [ i ].hCreate );
		CloseHandle ( g_FileMap [ i ].hOpen   );
	}
	*/
}

DWORD GetFileMapDWORD ( int iID, DWORD dwOffset )
{
	if ( !g_FileMap [ iID ].pIPC )
		return 0;

	DWORD dwValue = 0;

	g_FileMap [ iID ].pIPC->ReceiveBuffer ( &dwValue, dwOffset, sizeof ( dwValue ) );

	return dwValue;

	/*
	g_FileMap [ iID ].hOpen = OpenFileMapping ( FILE_MAP_ALL_ACCESS, TRUE, ( char* ) g_FileMap [ iID ].szName );
	g_FileMap [ iID ].pData = MapViewOfFile   ( g_FileMap [ iID ].hOpen, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

	BYTE* pData   = ( BYTE* ) g_FileMap [ iID ].pData;
	DWORD dwValue = 0;

	if ( pData )
	{
		memcpy ( &dwValue, &pData [ dwOffset ], sizeof ( DWORD ) );

		UnmapViewOfFile ( g_FileMap [ iID ].pData );

		return dwValue;
	}

	return 0;
	*/
}

DWORD GetFileMapString ( DWORD dwReturn, int iID, DWORD dwOffset )
{
	if ( !g_FileMap [ iID ].pIPC )
		return 0;

	char szString [ 256 ] = "";

	g_FileMap [ iID ].pIPC->ReceiveBuffer ( &szString, dwOffset, sizeof ( szString ) );

	DWORD dwSize        = strlen ( szString );
	char* pReturnString	= NULL;	

	if ( dwReturn )
		g_pGlob->CreateDeleteString ( ( DWORD* ) &dwReturn, 0 );

	g_pGlob->CreateDeleteString ( ( DWORD* ) &pReturnString, dwSize + 1 );
		
	strcpy ( pReturnString, szString );

	return ( DWORD ) pReturnString;

	/*
	g_FileMap [ iID ].hOpen = OpenFileMapping ( FILE_MAP_ALL_ACCESS, TRUE, ( char* ) g_FileMap [ iID ].szName );
	g_FileMap [ iID ].pData = MapViewOfFile   ( g_FileMap [ iID ].hOpen, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

	BYTE* pA = ( BYTE* ) g_FileMap [ iID ].pData;
	BYTE* pB = ( BYTE* ) pA [ dwOffset ];

	if ( pA )
	{
		char* pszString = ( char* ) &pA [ dwOffset ];
		DWORD dwSize        = strlen ( pszString );
		char* pReturnString	= NULL;	

		if ( dwReturn )
			g_pGlob->CreateDeleteString ( ( DWORD* ) &dwReturn, 0 );

		g_pGlob->CreateDeleteString ( ( DWORD* ) &pReturnString, dwSize + 1 );
		
		strcpy ( pReturnString, pszString );

		UnmapViewOfFile ( g_FileMap [ iID ].pData );

		return ( DWORD ) pReturnString;
	}

	return 0;
	*/
}

DWORD GetFileMapFloat ( int iID, DWORD dwOffset )
{
	if ( !g_FileMap [ iID ].pIPC )
		return 0;

	float fValue = 0;

	g_FileMap [ iID ].pIPC->ReceiveBuffer ( &fValue, dwOffset, sizeof ( fValue ) );

	return *( DWORD* ) &fValue;

	/*
	if ( !CheckFileMapID ( iID ) )
		return 0;

	char* pA = ( char* ) g_FileMap [ iID ].pData;
	char* pB = ( char* ) pA [ dwOffset ];

	float fValue = *( float* ) pB;

	return *( DWORD* ) &fValue;
	*/
}

void SetFileMapDWORD ( int iID, DWORD dwOffset, DWORD dwValue )
{
	if ( !g_FileMap [ iID ].pIPC )
		return;

	g_FileMap [ iID ].pIPC->SendBuffer ( &dwValue, dwOffset, sizeof ( dwValue ) );

	/*
	g_FileMap [ iID ].hOpen = OpenFileMapping ( FILE_MAP_ALL_ACCESS, TRUE, ( char* ) g_FileMap [ iID ].szName );
	g_FileMap [ iID ].pData = MapViewOfFile   ( g_FileMap [ iID ].hOpen, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

	BYTE* pData   = ( BYTE* ) g_FileMap [ iID ].pData;
	
	if ( pData )
	{
		//if ( dwOffset == 16 )
		//	Sleep ( 1000 );
		//	MessageBox ( NULL, "info", "info", MB_OK );

		memcpy ( &pData [ dwOffset ], &dwValue, sizeof ( DWORD ) );

		//FlushViewOfFile(  g_FileMap [ iID ].pData, 0 );
		//UnmapViewOfFile ( g_FileMap [ iID ].pData );
	}
	*/
}

void SetFileMapString ( int iID, DWORD dwOffset, DWORD dwString )
{
	if ( !g_FileMap [ iID ].pIPC )
		return;

	char* pString         = ( char* ) dwString;
	char  szBlank [ 255 ] = "";

	g_FileMap [ iID ].pIPC->SendBuffer ( szBlank, dwOffset, sizeof ( szBlank ) );
	g_FileMap [ iID ].pIPC->SendBuffer ( pString, dwOffset, strlen ( pString ) * sizeof ( char ) );

	/*
	g_FileMap [ iID ].hOpen = OpenFileMapping ( FILE_MAP_ALL_ACCESS, TRUE, ( char* ) g_FileMap [ iID ].szName );
	g_FileMap [ iID ].pData = MapViewOfFile   ( g_FileMap [ iID ].hOpen, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

	BYTE* pData   = ( BYTE* ) g_FileMap [ iID ].pData;
	
	if ( pData )
	{
		char* pString = ( char* ) dwString;

		//char szEmpty [ 255 ];

		//strcpy ( szEmpty, "" );
		//memcpy ( &pData [ dwOffset ], szEmpty, strlen ( szEmpty ) * sizeof ( char ) );

		for ( int i = dwOffset; i < dwOffset + 255; i++ )
				pData [ i ] = 0;

		memcpy ( &pData [ dwOffset ], pString, strlen ( pString ) * sizeof ( char ) );

		//memcpy ( &pData [ dwOffset ], &dwValue, sizeof ( DWORD ) );

		//FlushViewOfFile(  g_FileMap [ iID ].pData, 0 );

		//UnmapViewOfFile ( g_FileMap [ iID ].pData );

	}
	*/
}

void SetFileMapFloat ( int iID, DWORD dwOffset, float fValue )
{
	if ( !g_FileMap [ iID ].pIPC )
		return;

	g_FileMap [ iID ].pIPC->SendBuffer ( &fValue, dwOffset, sizeof ( fValue ) );

	/*
	if ( !CheckFileMapID ( iID ) )
		return;

	char* pA = ( char* ) g_FileMap [ iID ].pData;
	char* pB = ( char* ) pA [ dwOffset ];

	float* pData = ( float* ) pB;

	*pData = fValue;
	*/
}

void SetEventAndWait ( int iID )
{
	// only if filemap valid
	if ( !g_FileMap [ iID ].pIPC )
		return;

	// use this event handle
	HANDLE hEvent = g_FileMap [ iID ].pIPC->m_hDataEvent;
	if ( !hEvent ) return;

	// Wait for response (normally this event is set, but is reset when requesting update)
	DWORD dwTimeOutDelay=5000;
	ResetEvent ( hEvent );
	if ( WaitForSingleObject ( hEvent, dwTimeOutDelay )==WAIT_TIMEOUT )
	{
		// Five seconds up and no signal, interface must be elsewhere
	}
}
