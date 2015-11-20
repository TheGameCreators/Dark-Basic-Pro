/*
**************** DLL Notes *************************

Usage          - There will be 2 DLL's, one for debug mode
               - which contains all of the functions, help
			   - and examples. The debug mode DLL will only
			   - be used within the confines of DB Pro. And
			   - we will have a release mode DLL which will
			   - just contain the functions, this will be
			   - used with final created executable files
Description    - contained in description.txt
Help files     - contained in help.txt
Examples       - contained in examples.txt
Function names - contained in function.txt

*****************************************************
*/

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <stdlib.h>

BOOL WINAPI DLLMain ( HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{

		}
        break;

        case DLL_THREAD_ATTACH:
		{

		}
        break;

        case DLL_THREAD_DETACH:
		{

		}
        break;

        case DLL_PROCESS_DETACH:
		{

		}
        break;
    }

	return true;
}