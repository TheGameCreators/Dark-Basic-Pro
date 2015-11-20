//
// GDI Specific Functions
//

// Common Includes
#include "windows.h"
#include "DBDLLCore.h"

// External Gloobal Data
extern GlobStruct g_Glob;

DARKSDK void GDI_CreateDisplay(DWORD dwDisplayType)
{
	// Create Display Area
	g_hdcDisplay = CreateCompatibleDC(NULL);
	g_hDisplayBitmap = CreateCompatibleBitmap(g_hdcDisplay, g_dwScreenWidth, g_dwScreenHeight);

	// Select Default Colour Scheme
	SetTextColor(g_hdcDisplay, g_colFore);
	SetBkColor(g_hdcDisplay,g_colBack);

	// Create Brush
	g_hBrush = CreateSolidBrush(0);
}

DARKSDK void GDI_DeleteDisplay(void)
{
	// Free display resources
	if(g_hBrush)
	{
		DeleteObject(g_hBrush);
		g_hBrush=NULL;
	}
	if(g_hDisplayBitmap)
	{
		DeleteObject(g_hDisplayBitmap);
		g_hDisplayBitmap=NULL;
	}
	if(g_hdcDisplay)
	{
		DeleteDC(g_hdcDisplay);
		g_hdcDisplay=NULL;
	}
}

DARKSDK void GDI_SetDefaultDisplayProperties(void)
{
	// Nothing required (maybe ink, font, etc)
}

DARKSDK void GDI_ClearPrintArea(void)
{
	if(g_hdcDisplay)
	{
		// Select In Bitmap and Brush
		HGDIOBJ hdcOldBitmap = SelectObject(g_hdcDisplay, g_hDisplayBitmap);
		HGDIOBJ hOldBrush = SelectObject(g_hdcDisplay, g_hBrush);

		// Clear Device
		RECT rc;
		GetClientRect(g_Glob.hWnd, &rc);
		Rectangle(g_hdcDisplay, 0, 0, rc.right, rc.bottom);

		// Select Out the Objects and Delete
		SelectObject(g_hdcDisplay, hOldBrush);
		SelectObject(g_hdcDisplay, hdcOldBitmap);

		// Update Display
		InvalidateRect(g_Glob.hWnd, NULL, FALSE);
	}
}

DARKSDK void GDI_ClearSomeText(int LeftmostToClear, int RightmostToClear)
{
	if(g_hdcDisplay)
	{
		// Work out Text Details
		SIZE Size;
		DWORD dwLength = strlen(" ");
		GetTextExtentPoint32(g_hdcDisplay, " ", dwLength, &Size); 

		// Select In Bitmap and Brush
		HGDIOBJ hdcOldBitmap = SelectObject(g_hdcDisplay, g_hDisplayBitmap);
		HGDIOBJ hOldBrush = SelectObject(g_hdcDisplay, g_hBrush);

		// Clear Area
		Rectangle(g_hdcDisplay, LeftmostToClear, g_Glob.iCursorY, RightmostToClear, g_Glob.iCursorY+Size.cy);

		// Select Out the Objects and Delete
		SelectObject(g_hdcDisplay, hOldBrush);
		SelectObject(g_hdcDisplay, hdcOldBitmap);

		// Update Display
		InvalidateRect(g_Glob.hWnd, NULL, FALSE);
	}
}

DARKSDK void GDI_ShiftDisplayUp(int iShiftUp)
{
	if(g_hdcDisplay)
	{
		// Select In Bitmap
		HGDIOBJ hdcOldBitmap = SelectObject(g_hdcDisplay, g_hDisplayBitmap);

		// Shift Area Up The Display
		BitBlt(g_hdcDisplay, 0, 0, g_dwScreenWidth, g_dwScreenHeight, g_hdcDisplay, 0, iShiftUp, SRCCOPY);

		// Select Out the Object
		SelectObject(g_hdcDisplay, hdcOldBitmap);
	}
}

DARKSDK void GDI_PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn)
{
	if(g_hdcDisplay)
	{
		// Select in the Display Bitmap
		HGDIOBJ hdcOld = SelectObject(g_hdcDisplay, g_hDisplayBitmap);

		if(pStr)
		{
			// Work out Text Details
			SIZE Size;
			DWORD dwLength = strlen(pStr);
			GetTextExtentPoint32(g_hdcDisplay, pStr, dwLength, &Size); 

			// Write Text to HDC
			TextOut(g_hdcDisplay, g_Glob.iCursorX, g_Glob.iCursorY, pStr, dwLength);

			// Ensure Height is calculated
			if(Size.cy==0) { GetTextExtentPoint32(g_hdcDisplay, " ", 1, &Size); Size.cx=0; }

			// Advance Cursor
			if(bIncludeCarriageReturn)
			{
				g_Glob.iCursorX=0;
				g_Glob.iCursorY+=Size.cy;

				// Scroll if reach bottom
				RECT rc;
				GetClientRect(g_Glob.hWnd, &rc);
				if(g_Glob.iCursorY>rc.bottom-Size.cy)
				{
					// Shift Distance
					int iDist = g_Glob.iCursorY-(rc.bottom-Size.cy);

					// Shift Contents of display
					GDI_ShiftDisplayUp(iDist);

					// Select In Brush
					HGDIOBJ hOldBrush = SelectObject(g_hdcDisplay, g_hBrush);

					// Clear Area
					Rectangle(g_hdcDisplay, 0, rc.bottom-Size.cy, g_dwScreenWidth, g_dwScreenHeight);

					// Select Out the Brush
					SelectObject(g_hdcDisplay, hOldBrush);

					// Restore Y Position
					g_Glob.iCursorY = (rc.bottom-Size.cy);
				}
			}
			else
			{
				g_Glob.iCursorX+=Size.cx;
			}
		}

		// Select Out the Display Bitmap
		SelectObject(g_hdcDisplay, hdcOld);

		// Update Display
		InvalidateRect(g_Glob.hWnd, NULL, FALSE);
	}
}

DARKSDK SIZE GDI_GetTextSize(LPSTR pText)
{
	SIZE s;
	GetTextExtentPoint32(g_hdcDisplay, pText, strlen(pText), &s);
	return s;
}
