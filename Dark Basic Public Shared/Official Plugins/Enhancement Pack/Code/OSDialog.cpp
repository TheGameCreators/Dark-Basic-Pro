
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	DIALOG COMMANDS
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
#include <commdlg.h>
#include "core.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK DWORD OpenFileDialog ( DWORD dwReturn, DWORD dwDir, DWORD dwFilter, DWORD dwTitle );
DARKSDK DWORD SaveFileDialog ( DWORD dwReturn, DWORD dwDir, DWORD dwFilter, DWORD dwTitle );

/*
	// export names for string table -

		OPEN FILE DIALOG[%SSSS%?OpenFileDialog@@YAKKKKK@Z%Directory, Filter, Title
		SAVE FILE DIALOG[%SSSS%?SaveFileDialog@@YAKKKKK@Z%Directory, Filter, Title
*/

DWORD OpenFileDialog ( DWORD dwReturn, DWORD dwDir, DWORD dwFilter, DWORD dwTitle )
{
	OPENFILENAME	ofn;
	char			szFile [ 260 ];
	
	memset ( szFile,  0, sizeof ( szFile ) );
	memset ( &ofn,    0, sizeof ( ofn    ) );

	ofn.lStructSize     = sizeof ( ofn );
	ofn.hwndOwner       = NULL;
	ofn.lpstrFile       = szFile;
	ofn.nMaxFile        = sizeof ( szFile );
	ofn.lpstrFilter     = ( char* ) dwFilter;
	ofn.nFilterIndex    = 1;
	ofn.lpstrTitle      = ( char* ) dwTitle;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = ( char* ) dwDir;
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	GetOpenFileName ( &ofn );

	return ( DWORD ) SetupString ( ofn.lpstrFile );
}

DWORD SaveFileDialog ( DWORD dwReturn, DWORD dwDir, DWORD dwFilter, DWORD dwTitle )
{
	OPENFILENAME	ofn;
	char			szFile [ 260 ];
	
	memset ( szFile,  0, sizeof ( szFile ) );
	memset ( &ofn,    0, sizeof ( ofn    ) );

	ofn.lStructSize     = sizeof ( ofn );
	ofn.hwndOwner       = NULL;
	ofn.lpstrFile       = szFile;
	ofn.nMaxFile        = sizeof ( szFile );
	ofn.lpstrFilter     = ( char* ) dwFilter;
	ofn.nFilterIndex    = 1;
	ofn.lpstrTitle      = ( char* ) dwTitle;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = ( char* ) dwDir;
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	GetSaveFileName ( &ofn );

	return ( DWORD ) SetupString ( ofn.lpstrFile );
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
