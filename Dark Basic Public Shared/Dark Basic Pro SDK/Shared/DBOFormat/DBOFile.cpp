
//
// DBOFile Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBOFILE HEADER ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "DBOFile.h"
#include "DBOBlock.h"
#include "direct.h"

DARKSDK_DLL int LoadDBOEx ( LPSTR pFilename, sObject** ppObject )
{
	DWORD dwBlockSize = 0;
	DWORD* pDBOBlock = NULL;

	if ( !DBOLoadBlockFile ( pFilename, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		return -1;

	if ( !ConstructObject ( ppObject, (LPSTR*)&pDBOBlock ) )
		return -2;

	return 1;
}

DARKSDK_DLL int SaveDBOEx ( LPSTR pFilename, sObject* pObject )
{
	// DBOBlock ptr
	DWORD dwBlockSize = 0;
	DWORD* pDBOBlock = NULL;

	// convert pObject to DBOBlock
	if ( !DBOConvertObjectToBlock ( pObject, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		return -2;
		
	// save DBOBlock to file
	if ( !DBOSaveBlockFile ( pFilename, (DWORD)pDBOBlock, dwBlockSize ) )
		return -3;

	// free block when done
	SAFE_DELETE(pDBOBlock);

	// okay
	return 1;
}