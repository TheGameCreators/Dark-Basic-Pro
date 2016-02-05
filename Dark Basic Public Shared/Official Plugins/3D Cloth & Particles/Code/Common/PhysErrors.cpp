#include "stdafx.h"
//#include "../DBProPhysicsMaster/stdafx.h"
//#include "Windows.h"
//#include "PhysErrors.h"
//#include "PhysStrings.h"


ePhysError lastError=PERR_AOK;

void reportError(ePhysError err, ePhysCommand cmd)
{
	if(__errorMessagesAvailable)
	{
		MessageBox(NULL, PERR_messages[err], PCMD_names[cmd], MB_OK);						
	}
	else
	{
		MessageBox(NULL, "A physics error occurred in the DBPro application.","Physics Error", MB_OK);
	}
#ifdef _DEBUG
	DebugBreak();
#else
	exit(EXIT_FAILURE);
#endif
}
