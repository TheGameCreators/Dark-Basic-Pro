// include definition
#include "cerror.h"

#define DARKSDK __declspec ( dllexport )

#pragma comment ( lib, "user32.lib" )

#define DB_PRO 1

// Handler Passed into DLL

void Error ( char* szMessage )
{
		//extern HWND gHwnd;
		HWND mHwnd =  GetForegroundWindow ( );
		//HWND mHwnd =  gHwnd;
		ShowCursor ( true );
		//ShowWindow ( mHwnd, SW_HIDE );
		MessageBox ( NULL, szMessage, "DarkBASIC Pro Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TOPMOST );
		//PostQuitMessage ( 0 );
	
}

void Message ( int iID, char* szMessage, char* szTitle )
{
	MessageBox ( NULL, szMessage, szTitle, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_TOPMOST );
}

void RunTimeError ( DWORD dwErrorCode )
{
	// Assign Run Time Error To Global Error Handler
	
}

void RunTimeWarning ( DWORD dwErrorCode )
{
	// Assign Run Time Error To Global Error Handler
//	if(g_pErrorHandler) g_pErrorHandler->dwErrorCode = dwErrorCode;
}
