//
// DBDLLGDI Header
//
#include "DBDLLCore.h"

// Create/Delete Display
void CreateDisplay(DWORD dwDisplayMode);
void DeleteDisplay(void);
void SetDefaultDisplayProperties(void);

// CLear Functions
void ClearPrintArea(void);
void ClearSomeText(int LeftmostToClear, int RightmostToClear);

// Page Scroll Function
void ShiftDisplayUp(int iShiftUp);

// Text Print Function
void PrintSomething(LPSTR pStr, bool bIncludeCarriageReturn);

// Text Width & Height Function
SIZE GetTextSize(LPSTR pText);