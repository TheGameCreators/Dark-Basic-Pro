// TESTCOMMANDS.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#define MYCOMMAND __declspec ( dllexport )

MYCOMMAND void PrintText( LPSTR pString )
{
	if(pString)
	{
		MessageBox(NULL, pString, "", MB_OK);
	}
}

MYCOMMAND int GetValue( void )
{
	return 42;
}
