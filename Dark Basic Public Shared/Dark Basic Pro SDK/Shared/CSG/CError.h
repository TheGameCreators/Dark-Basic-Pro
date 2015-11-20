#ifndef _CERROR_H_
#define _CERROR_H_

#include <windows.h>   
#include <windowsx.h>


#define DARKSDK __declspec ( dllexport )

// Handler Passed into DLL


void Error ( char* szMessage );
void Message ( int iID, char* szMessage, char* szTitle );

void			RunTimeError(DWORD dwErrorCode);
void			RunTimeWarning(DWORD dwErrorCode);

#endif _CERROR_H_