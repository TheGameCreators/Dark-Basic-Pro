//
// DBDLLEXT Header
//
#include "DBDLLCore.h"

// Create Function
void EXT_CreateDisplay(DWORD dwDisplayType);
void EXT_DeleteDisplay(void);
void EXT_SetDefaultDisplayProperties(void);

// Clear Functions
void EXT_ClearPrintArea(void);
void EXT_ClearSomeText(int LeftmostToClear, int RightmostToClear);

// Page Scroll Function
void EXT_ShiftDisplayUp(int iShiftUp);

// Text Print Function
void EXT_PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn);

// Text Width & Height Function
SIZE EXT_GetTextSize(LPSTR pText);