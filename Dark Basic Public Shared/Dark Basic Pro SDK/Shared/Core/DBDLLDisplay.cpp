//
// Display Related Functions
//

// Common Includes
#include "windows.h"
#include "DBDLLGDI.h"
#include "DBDLLEXT.h"
#include "DBDLLCore.h"

// Global data to store system colors
DBPRO_GLOBAL bool bColoursCaptured=false;
DBPRO_GLOBAL DWORD aDefaultSystemColors[30];

DARKSDK void CaptureSystemColors ( void )
{
	// capture all system colors
	bColoursCaptured=true;
	for ( int i=0; i<29; i++)
		aDefaultSystemColors[i] = GetSysColor(i); 
}

DARKSDK void RestoreSystemColors ( void )
{
	// element array
	if ( bColoursCaptured==true )
	{
		int aiSysElements[30];
		for ( int i=0; i<29; i++)
			aiSysElements[i]=i;

		// restore all system colors
		SetSysColors(28, aiSysElements, aDefaultSystemColors); 

		// reset flag
		bColoursCaptured=false;
	}
}

DARKSDK void CreateDisplay(DWORD dwDisplayType)
{
	if(g_bUseExternalDisplayLayer)
		EXT_CreateDisplay(dwDisplayType);
	else
		GDI_CreateDisplay(dwDisplayType);

	CaptureSystemColors();
}

DARKSDK void DeleteDisplay(void)
{
	RestoreSystemColors();

	if(g_bUseExternalDisplayLayer)
		EXT_DeleteDisplay();
	else
		GDI_DeleteDisplay();
}

DARKSDK void SetDefaultDisplayProperties(void)
{
	if(g_bUseExternalDisplayLayer)
		EXT_SetDefaultDisplayProperties();
	else
		GDI_SetDefaultDisplayProperties();
}

DARKSDK void ClearPrintArea(void)
{
	if(g_bUseExternalDisplayLayer)
		EXT_ClearPrintArea();
	else
		GDI_ClearPrintArea();
}

DARKSDK void ClearSomeText(int LeftmostToClear, int RightmostToClear)
{
	if(g_bUseExternalDisplayLayer)
		EXT_ClearSomeText(LeftmostToClear, RightmostToClear);
	else
		GDI_ClearSomeText(LeftmostToClear, RightmostToClear);
}

DARKSDK void ShiftDisplayUp(int iShiftUp)
{
	if(g_bUseExternalDisplayLayer)
		EXT_ShiftDisplayUp(iShiftUp);
	else
		GDI_ShiftDisplayUp(iShiftUp);
}

DARKSDK void PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn)
{
	if(g_bUseExternalDisplayLayer)
		EXT_PrintSomething(pStr, bIncludeCarriageReturn);
	else
		GDI_PrintSomething(pStr, bIncludeCarriageReturn);
}

DARKSDK SIZE GetTextSize(LPSTR pText)
{
	if(g_bUseExternalDisplayLayer)
		return EXT_GetTextSize(pText);
	else
		return GDI_GetTextSize(pText);
}
