
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	CORE SET UP COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// SOUND DLL
// LPDIRECTSOUND8 GetSoundInterface ( void )

/*
*/

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "globstruct.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "core.h"
#include <stdio.h>

#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// GLOBALS /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

char					g_szErrorList [ 256 ] [ 256 ];
sSound					g_SoundList   [ 256 ];

bool					g_bErrorFile = false;
GlobStruct*				g_pGlobal    = NULL;
LPDIRECTSOUND8			g_pSound     = NULL;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK void ReceiveCoreDataPtr   ( LPVOID pCore );
DARKSDK int  GetAssociatedDLLs    ( void );
DARKSDK void Destructor           ( void );

		void LoadSystemDLL        ( void );
		void LoadSoundDLL         ( void );
		void SetupErrorCodes      ( void );

void ReceiveCoreDataPtr ( LPVOID pCore )
{
	// get the global structure pointer from DB Pro

	// store the pointer
	g_pGlobal = ( GlobStruct* ) pCore;

	// set up error codes
	SetupErrorCodes ( );

	// load the sound dll
	LoadSoundDLL ( );
}

int GetAssociatedDLLs ( void )
{
	// return associated DLLs - in this case we depend on the sound DLL
	return 2;
}

void Destructor ( void )
{
	// clean up
}

void LoadSoundDLL ( void )
{
	// load the sound DLL and extract the sound interface function

	// set up the function pointer type definition
	typedef LPDIRECTSOUND8 ( *GetSoundPFN ) ( void );

	// declare the function pointer
	GetSoundPFN pGetSound = NULL;

	// display error message if there is a problem with the global structure
	if ( !g_pGlobal )
	{
		Error ( 1 );
		return;
	}

	// attempt to set the sound function pointer
	pGetSound = ( GetSoundPFN ) GetProcAddress ( g_pGlobal->g_Sound, "?GetSoundInterface@@YAPAUIDirectSound8@@XZ" );

	// fail if pointer is invalid
	if ( !pGetSound )
	{
		Error ( 3 );
		return;
	}

	// store the sound interface
	g_pSound = pGetSound ( );

	// make sure all is okay
	if ( !g_pSound )
	{
		Error ( 2 );
		return;
	}

	// clear the sound list
	memset ( g_SoundList, 0, sizeof ( g_SoundList ) );

	// go through all items in list and reset values
	for ( int i = 0; i < 256; i++ )
	{
		g_SoundList [ i ].pBuffer		= NULL;
		g_SoundList [ i ].bPlaying      = false;
		g_SoundList [ i ].bPaused       = false;
		g_SoundList [ i ].bStopped      = false;
		g_SoundList [ i ].bLooping      = false;
		g_SoundList [ i ].dwPosition    = 0;
	}
}

void SetupErrorCodes ( void )
{
	// set up error codes for when things fail

	char  szPath [ 255 ] = "";
	char  szDir  [ 255 ] = "";
	HKEY  hKey;
	DWORD dwBufLen;

	// open the registry key
	RegOpenKeyEx    ( HKEY_LOCAL_MACHINE, "SOFTWARE\\Dark Basic\\Dark Basic Pro", 0, KEY_QUERY_VALUE, &hKey );
	RegQueryValueEx ( hKey, "install-path", NULL, NULL, ( LPBYTE ) szPath, &dwBufLen );
	RegCloseKey     ( hKey );

	// append our path
	strcat ( szPath, "\\compiler\\plugins-licensed\\" );

	GetCurrentDirectory ( 255, szDir );
	SetCurrentDirectory ( szPath );

	// clear out the error table
	memset ( g_szErrorList, 0, sizeof ( g_szErrorList ) );

	// attempt to open the "loc" file
	FILE* fp = fopen ( "EnhancementsOV.loc", "rt" );

	// if the file is valid we should be running from DB Pro
	if ( fp )
	{
		// read in each line of the file
		char szTemp [ 256 ];
		int  iIndex = 0;
		
		while ( fgets ( szTemp, sizeof ( szTemp ), fp ) )
		{
			char* token = strtok ( szTemp, "\n" );

			while ( token )
			{
				strcpy ( g_szErrorList [ iIndex++ ], token );
				token = strtok ( 0, "\n" );
			}
		}

		// close the file
		fclose ( fp );

		// save state as we have the error file loaded
		g_bErrorFile = true;
	}

	// restory directory
	SetCurrentDirectory ( szDir );
}

void Error ( int iID )
{
	if ( g_bErrorFile )
		MessageBox ( NULL, g_szErrorList [ iID ], g_szErrorList [ 0 ], MB_ICONERROR | MB_OK );
	else
	{
		char szNum [ 3 ];

		itoa ( iID, szNum, 10 );

		MessageBox ( NULL, szNum, "Enhancements - Ogg Vorbis - Runtime Error", MB_ICONERROR | MB_OK );
	}

	PostQuitMessage ( 0 );
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
