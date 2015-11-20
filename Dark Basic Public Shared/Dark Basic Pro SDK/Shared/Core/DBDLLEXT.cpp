//
// EXT Specific Functions
//

// Common Includes
#include "windows.h"
#include "DBDLLCore.h"


// External Gloobal Data
extern GlobStruct g_Glob;

DARKSDK void EXT_CreateDisplay(DWORD dwDisplayType)
{
	// Create Display Area (last flag is locable bb - performance flag here)
	if(dwDisplayType==3)
		g_GFX_SetDisplayMode5(g_Glob.dwWindowWidth, g_Glob.dwWindowHeight, g_Glob.iScreenDepth, 0, 0, 1);
	else
		g_GFX_SetDisplayMode5(g_Glob.dwWindowWidth, g_Glob.dwWindowHeight, 0, 1, 0, 1);
}

DARKSDK void EXT_DeleteDisplay(void)
{
	// Free Display Elements
	g_GFX_RestoreDisplayMode();
}

DARKSDK void EXT_SetDefaultDisplayProperties(void)
{
	// Assign Ink Colour

	// Assign Fill Style

	// Assign Font Style
	#ifndef DARKSDK_COMPILE
	if(g_Text_SetCharacterSet)
	#endif
		g_Text_SetCharacterSet("Fixedsys", 0);
}

DARKSDK void EXT_ClearPrintArea(void)
{
	// Clear Display
	g_GFX_Clear( 0, 0, 0 );
}

DARKSDK void EXT_ClearSomeText(int LeftmostToClear, int RightmostToClear)
{
	// Work out Text Details
	SIZE Size;
	Size.cx=g_Text_GetWidth("_");
	Size.cy=g_Text_GetHeight("_");

	// Clear Area
	DWORD dwStoreColor = g_Glob.dwForeColor;
	g_Glob.dwForeColor = g_Glob.dwBackColor;
	g_Basic2D_Box(LeftmostToClear, g_Glob.iCursorY, RightmostToClear, g_Glob.iCursorY+Size.cy);
	g_Glob.dwForeColor = dwStoreColor;
}

DARKSDK void EXT_ShiftDisplayUp(int iShiftUp)
{
	// Shift Area Up The Display
	g_Basic2D_CopyArea(0, 0, g_dwScreenWidth, g_dwScreenHeight, 0, iShiftUp);
}

DARKSDK void EXT_PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn)
{
	if(pStr)
	{
		// Work out Text Details
		SIZE Size;
		Size.cx=g_Text_GetWidth(pStr);
		Size.cy=g_Text_GetHeight(pStr);
		if(Size.cy==0) Size.cy=g_Text_GetHeight(" ");

		// Write Text
		g_Text_Text(g_Glob.iCursorX, g_Glob.iCursorY, pStr);

		// Advance Cursor
		if(bIncludeCarriageReturn)
		{
			g_Glob.iCursorX=0;
			g_Glob.iCursorY+=Size.cy;

			// Scroll if reach bottom
			RECT rc;
			RECT winrc;
			GetClientRect(g_Glob.hWnd, &rc);
			GetWindowRect(g_Glob.hWnd, &winrc);
			int iTopDiff=(winrc.bottom-winrc.top)-rc.bottom;
			rc.top=0;
			rc.left=0;
			rc.right=g_Glob.dwWindowWidth;
			rc.bottom=g_Glob.dwWindowHeight-iTopDiff;
			if(g_Glob.iCursorY>rc.bottom-Size.cy)
			{
				// Shift Distance
				int iDist = g_Glob.iCursorY-(rc.bottom-Size.cy);

				// Shift Contents of display
				EXT_ShiftDisplayUp(iDist);

				// Clear Area
				DWORD dwStoreColor = g_Glob.dwForeColor;
				g_Glob.dwForeColor = g_Glob.dwBackColor;
				g_Basic2D_Box(0, rc.bottom-Size.cy, g_dwScreenWidth, g_dwScreenHeight);
				g_Glob.dwForeColor = dwStoreColor;

				// Restore Y Position
				g_Glob.iCursorY = (rc.bottom-Size.cy);
			}
		}
		else
		{
			g_Glob.iCursorX+=Size.cx;
		}
	}
}

DARKSDK SIZE EXT_GetTextSize(LPSTR pText)
{
	SIZE s;
	s.cx=g_Text_GetWidth(pText);
	s.cy=g_Text_GetHeight(pText);
	return s;
}
