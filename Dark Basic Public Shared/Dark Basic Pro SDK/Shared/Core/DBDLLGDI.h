//
// DBDLLGDI Header
//
#include "DBDLLCore.h"

// Create Function
void GDI_CreateDisplay(DWORD dwDisplayType);
void GDI_DeleteDisplay(void);
void GDI_SetDefaultDisplayProperties(void);

// Clear Functions
void GDI_ClearPrintArea(void);
void GDI_ClearSomeText(int LeftmostToClear, int RightmostToClear);

// Page Scroll Function
void GDI_ShiftDisplayUp(int iShiftUp);

// Text Print Function
void GDI_PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn);

// Text Width & Height Function
SIZE GDI_GetTextSize(LPSTR pText);