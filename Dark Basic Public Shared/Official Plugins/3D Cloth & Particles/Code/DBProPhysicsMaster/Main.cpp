#include "stdafx.h"

#pragma comment( exestr, "gU%B2V^4ug65h6Jh78V8NB9£9yFn4j*y4GNmjg3)BM45d6f%7Fgkj8hK6{mB54Y:@K5M~8bdsF3ds3oi5U5jNB" __TIMESTAMP__ )

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}



DLLEXPORT void Constructor ( void )
{
	loadLocalisationStrings();

	physics = new PhysicsManager();	
	if(!physics) 
	{
		MessageBox(0, "Out of memory. Can't initialise.", "Physics Plugin", MB_OK);
		#ifdef _DEBUG
		DebugBreak();
		#endif
		exit(EXIT_FAILURE);
	}
}

DLLEXPORT void Destructor ( void )
{	
	for(int i=0;i<__numErrorMsgs;i++)
	{
		SAFEDELETE_ARRAY(PERR_messages[i]);
	}

	//for(int i=0;i<__numCommandNames;i++)
	for(i=0;i<__numCommandNames;i++)
	{
		SAFEDELETE_ARRAY(PCMD_names[i]);		
	}

	SAFEDELETE_ARRAY(PERR_messages);
	SAFEDELETE_ARRAY(PCMD_names);
	SAFEDELETE(physics);
}

DLLEXPORT void ReceiveCoreDataPtr ( LPVOID pCore )
{		
	if(physics)
	{
		physics->DBPro_globalPtr = (GlobStruct*)pCore;
		physics->getDLLHandle();
	}
}

DLLEXPORT int GetAssociatedDLLs(void)
{
	// return a code indicating which DLLs are required
	// 1 = Basic3D (and friends)
	return 1;
}

 


