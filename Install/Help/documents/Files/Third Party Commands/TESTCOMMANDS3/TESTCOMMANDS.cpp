//
// TESTCOMMANDS 2 - Shows use of core data in TPC DLL
//

// Extra Includes
#include "stdio.h"

// My Own Include
#include "TESTCOMMANDS.h"

// Include Core (optional)
#include "globstruct.h"
GlobStruct* g_pGlob = NULL;

//
// Constructors (optional)
//

void Constructor ( void )
{
	// Create memory here
}

void Destructor ( void )
{
	// Free memory here
}

void ReceiveCoreDataPtr ( LPVOID pCore )
{
	// Get Core Data Pointer here
	g_pGlob = (GlobStruct*)pCore;
}

//
// Standard DLL Main
//

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

//
// Simple Commands (core not needed)
//

void PrintText( LPSTR pString )
{
	if(pString)
	{
		MessageBox(NULL, pString, "", MB_OK);
	}
}

int GetValue( void )
{
	return 42;
}

//
// String Commands (core needed)
//

DWORD ReturnAFloat( int iValue )
{
	float fValue = ((float)iValue) + 0.1f;
	return *(DWORD*)&fValue;
}

DWORD ReverseString( DWORD pOldString, DWORD pStringIn )
{
	// Delete old string
	if(pOldString) g_pGlob->CreateDeleteString ( (DWORD*)&pOldString, 0 );

	// Return string pointer
	LPSTR pReturnString=NULL;

	// If input string valid
	if(pStringIn)
	{
		// Create a new string and copy input string to it
		DWORD dwSize=strlen( (LPSTR)pStringIn );
		g_pGlob->CreateDeleteString ( (DWORD*)&pReturnString, dwSize+1 );
		strcpy(pReturnString, (LPSTR)pStringIn);

		// Reverse the new string
		strrev(pReturnString);
	}

	// Return new string pointer
	return (DWORD)pReturnString;
}

void DisplayLargeNumbers( double dValueA, LONGLONG lValueB )
{
	// Make output
	LPSTR pOutput = new char[256];
	sprintf(pOutput, "Output - Double Float:%.16g Double Integer:%I64d", dValueA, lValueB);

	// Use Core PRINT to display output
	g_pGlob->PrintStringFunction ( pOutput, true );

	// Free output string
	if(pOutput)
	{
		delete pOutput;
		pOutput=NULL;
	}
}
