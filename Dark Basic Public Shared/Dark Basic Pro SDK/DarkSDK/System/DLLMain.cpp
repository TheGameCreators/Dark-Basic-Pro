#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

BOOL WINAPI DLLMain ( HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
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