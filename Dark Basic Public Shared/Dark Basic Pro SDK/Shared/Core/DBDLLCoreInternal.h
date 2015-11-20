//
// DBDLLCore Internal Header
//

// Common Includes
#include "DBDLLCore.h"

// Function Prototypes
DARKSDK void SetPrintCursor(int iX, int iY);
DARKSDK void PrintInteger(LONGLONG lValue, bool bIncludeCarriageReturn);
DARKSDK void PrintFloat(double dValue, bool bIncludeCarriageReturn);
DARKSDK void PrintString(LPSTR pString, bool bIncludeCarriageReturn);
DARKSDK void PrintNothing(void);
DARKSDK void InputSomething(LPSTR* pStr);
DARKSDK void InputInteger(LONGLONG* plValue);
DARKSDK void InputFloat(double* pdValue);
DARKSDK void InputString(LPSTR* pStr);
