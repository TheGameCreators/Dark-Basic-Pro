//
// Standard parser Header For All Classes
//

// Standard Includes
#include "windows.h"
#include "macros.h"

// Custom Includes
#include "StatementList.h"
#include "ASMWriter.h"
#include "DBMWriter.h"
#include "Error.h"

// Parser Defines
#define DBMPLACEMENT_TOP		1
#define DBMPLACEMENT_MIDDLE		2
#define DBMPLACEMENT_BOTTOM		3

// External Class Pointers
extern CError*				g_pErrorReport;
extern CASMWriter*			g_pASMWriter;
extern CDBMWriter*			g_pDBMWriter;
extern CStatementList*		g_pStatementList;
