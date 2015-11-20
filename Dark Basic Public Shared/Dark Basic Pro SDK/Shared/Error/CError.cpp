// include definition
#include "cerror.h"
#include ".\..\core\globstruct.h"
#pragma comment ( lib, "user32.lib" )
#define DB_PRO 1

// Handler Passed into DLL
CRuntimeErrorHandler* g_pErrorHandler = NULL;

void Error ( char* szMessage )
{
	#if DB_PRO
		if(g_pErrorHandler)
			if(g_pErrorHandler->dwErrorCode==0)
				RunTimeError(RUNTIMEERROR_GENERICERROR);
	#else
		//extern HWND gHwnd;
		HWND mHwnd =  GetForegroundWindow ( );
		//HWND mHwnd =  gHwnd;
		ShowCursor ( true );
		//ShowWindow ( mHwnd, SW_HIDE );
		MessageBox ( NULL, szMessage, "DarkBASIC Pro Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TOPMOST );
		//PostQuitMessage ( 0 );
	#endif
}

void Message ( int iID, char* szMessage, char* szTitle )
{
	MessageBox ( NULL, szMessage, szTitle, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_TOPMOST );
}

void RunTimeError ( DWORD dwErrorCode )
{
	// Assign Run Time Error To Global Error Handler
	if(g_pErrorHandler) g_pErrorHandler->dwErrorCode = dwErrorCode;
}

void RunTimeWarning ( DWORD dwErrorCode )
{
	// Assign Run Time Error To Global Error Handler
//	if(g_pErrorHandler) g_pErrorHandler->dwErrorCode = dwErrorCode;
}

void RunTimeSoftWarning ( DWORD dwErrorCode )
{
}

void RunTimeError(DWORD dwErrorCode, LPSTR pExtraErrorString)
{
	// lee - 180407 - now would it not be a good idea to know what the
	// data was that caused the runtime error?
	if ( pExtraErrorString )
	{
		// leechange - u71 - 061108 - instead of two message boxes
		// MessageBox ( NULL, pExtraErrorString, "Error", MB_OK );
		// we, piggy back error message in the EXEunpack string of the globstruct
		DWORD dwUsedChars = 0;
		if ( g_pGlob )
			if ( g_pGlob->pEXEUnpackDirectory )
				dwUsedChars = strlen ( g_pGlob->pEXEUnpackDirectory );
		if ( dwUsedChars > 0 )
		{
			// hack the error string into the large pEXEUnpackDirectory string path
			DWORD dwCanUse = _MAX_PATH - 3 - dwUsedChars;
			char pSecretErrorString [ _MAX_PATH ];
			strcpy ( pSecretErrorString, pExtraErrorString );
			pSecretErrorString [ dwCanUse ] = 0; // cut short in case too long
			DWORD dwSecretLength = strlen ( pSecretErrorString ) + 1;
			memcpy ( g_pGlob->pEXEUnpackDirectory + dwUsedChars + 1, pSecretErrorString, dwSecretLength );
		}
	}
	RunTimeError ( dwErrorCode );
}

void LastSystemError()
{
	LPVOID lpBuffer;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError ( ), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR)&lpBuffer, 0, NULL)) return;
	MessageBox ( NULL, (LPCTSTR)lpBuffer, "System Error", MB_OK );
	LocalFree ( lpBuffer );
}
