//
// DBDLLCore Header
//

#ifndef DBDLLCORE_H
#define DBDLLCORE_H

// Common Includes
#include "windows.h"

// Custom Includes
#include "globstruct.h"
#include "..\error\cerror.h"
#include "macros.h"

// Additional CoreHeader Includes
#include "DBDLLExtCalls.h"

#include "..\global.h"

#include <DB3Math.h>

// mike - 100405
#define DBPRO_GLOBAL 

// Global Core Functions
extern DARKSDK void InternalClearWindowsEntry(void);
extern DARKSDK DWORD InternalProcessMessages();

// Global Internal Data
extern HWND				g_hWnd						;
extern LPSTR			g_pVarSpace					;

// Global Performance Flag Vars
extern bool				g_bProcessorFriendly		;
extern bool				g_bUseExternalDisplayLayer	;

// Global Display Vars
extern HBITMAP			g_hDisplayBitmap			;
extern HDC				g_hdcDisplay				;
extern int				g_iX						;
extern int				g_iY						;
extern COLORREF			g_colFore					;
extern COLORREF			g_colBack					;
extern HBRUSH			g_hBrush					;
extern DWORD			g_dwScreenWidth				;
extern DWORD			g_dwScreenHeight			;
extern DWORD			g_dwWindowWidth				;
extern DWORD			g_dwWindowHeight			;

// Global Input Vars
extern DWORD			g_dwWindowsTextEntrySize	;
extern LPSTR			g_pWindowsTextEntry			;
extern DWORD			g_dwWindowsTextEntryPos		;
extern unsigned char	g_cKeyPressed				;
extern int				g_iEntryCursorState			;

// Global Data Vars
extern LPSTR			g_pDataLabelPtr				;
extern LPSTR			g_pDataLabelEnd				;

#endif
