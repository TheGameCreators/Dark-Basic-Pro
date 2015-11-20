//
// Global IDRs
//

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)
#include "resource.h"

//
// Common Macros
//

#if 0
#define SAFE_DELETE(x)	if(x) { delete x; x=NULL; }
#define SAFE_CLOSE(x)	if(x) { CloseHandle(x); x=NULL; }
#define SAFE_FREE(x)	if(x) { GlobalFree(x); x=NULL; }
#else
//# include "../../Dark Basic Public Shared/Dark Basic Pro SDK/Shared/global.h"
#include "DB3.h"
#endif
