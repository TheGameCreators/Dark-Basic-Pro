//
// Header File
//

// Common Includes
#include "windows.h"

// Define macro
#define MYCOMMAND __declspec ( dllexport )

// Constructor Functions (optional)
MYCOMMAND void Constructor ( void );
MYCOMMAND void Destructor ( void );
MYCOMMAND void ReceiveCoreDataPtr ( LPVOID pCore );

// Do not decorate DLL-function-names
extern "C"
{
	// My Commands and Functions
	MYCOMMAND void		PrintText				( LPSTR pString );
	MYCOMMAND int		GetValue				( void );

	MYCOMMAND DWORD		ReturnAFloat			( int iValue );
	MYCOMMAND DWORD		ReverseString			( DWORD pOldString, DWORD pStringIn );
	MYCOMMAND void		DisplayLargeNumbers		( double dValueA, LONGLONG lValueB );
}
