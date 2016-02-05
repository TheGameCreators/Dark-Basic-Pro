
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	INTERNET COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
//#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
//#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <winsock2.h>
#include "core.h"

#pragma comment ( lib, "Ws2_32.lib" )

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK DWORD GetWebClient ( void );
DARKSDK DWORD GetIPAddress ( void );

/*
	// export names for string table -

	GET WEB CLIENT$[%S%?GetWebClient@@YAKXZ%
	GET IP ADDRESS$[%S%?GetIPAddress@@YAKXZ%
*/

DWORD GetWebClient ( void )
{
	char	  szBuffer [ 256 ];
	HINSTANCE hBrowser = LoadLibrary ( "shdocvw.dll" );

	if ( hBrowser )
	{
		HRESULT				hr             = S_OK;
		DLLGETVERSIONPROC	pDllGetVersion = ( DLLGETVERSIONPROC ) GetProcAddress ( hBrowser, "DllGetVersion" );

		if ( pDllGetVersion )
		{
			DLLVERSIONINFO dvi;

			ZeroMemory ( &dvi, sizeof ( dvi ) );

			dvi.cbSize = sizeof ( dvi );

			hr = ( *pDllGetVersion ) ( &dvi );

			if ( SUCCEEDED ( hr ) )
				sprintf ( szBuffer, "Internet Explorer Version: %ld.%ld, Build Number: %ld",dvi.dwMajorVersion, dvi.dwMinorVersion, dvi.dwBuildNumber );
		}
	}
	else
	{
		Error ( 8 );
		return 0;
	}

	FreeLibrary ( hBrowser );

	return ( DWORD ) SetupString ( szBuffer );
}

DWORD GetIPAddress ( void )
{
	WORD		wVersionRequested;
	PHOSTENT	hostinfo;
    WSADATA		wsaData;
    char		szName [ 255 ] = "";
	char*		IP             = NULL;
    
	wVersionRequested = MAKEWORD ( 2, 0 );
	
	if ( WSAStartup ( wVersionRequested, &wsaData ) == 0 )
	{
		if ( gethostname ( szName, sizeof ( szName ) ) == 0 )
		{
			if ( ( hostinfo = gethostbyname ( szName ) ) != NULL )
			{
				IP = inet_ntoa ( *( struct in_addr* ) *hostinfo->h_addr_list );
				
				strcpy ( szName, IP );
			}
		}
		
		WSACleanup ( );
	}
	else
	{
		Error ( 9 );
		return 0;
	}
	
	return ( DWORD ) SetupString ( szName );
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
