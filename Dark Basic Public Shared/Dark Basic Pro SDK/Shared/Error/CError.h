#ifndef _CERROR_H_
#define _CERROR_H_

#include <windows.h>   
#include <windowsx.h>
#include "cruntimeerrors.h"

// Handler Passed into DLL
extern CRuntimeErrorHandler* g_pErrorHandler;

void Error ( char* szMessage );
void Message ( int iID, char* szMessage, char* szTitle );

void			RunTimeError(DWORD dwErrorCode);
void			RunTimeWarning(DWORD dwErrorCode);
void			RunTimeSoftWarning ( DWORD dwErrorCode );
void			RunTimeError(DWORD dwErrorCode, LPSTR pStrClue);

#endif _CERROR_H_