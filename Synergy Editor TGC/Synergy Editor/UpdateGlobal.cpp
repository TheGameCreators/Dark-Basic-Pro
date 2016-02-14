#include "StdAfx.h"
#include "UpdateGlobal.h"
#include "Utilities.h"

bool UpdateGlobal::Switch(CString newLocation, CString updateLocation, bool WaitForReboot)
{
	int type = (WaitForReboot) ? MOVEFILE_DELAY_UNTIL_REBOOT : MOVEFILE_COPY_ALLOWED;

	CString backupFile = newLocation + _T(".bak");

	Utilities::EnsurePathExists(Utilities::ExtractPath(newLocation));

	DeleteFile(backupFile);
	if(Utilities::CheckFileExists(newLocation))
	{
		if (!MoveFileEx(newLocation, backupFile, type)) 
		{
			return false;
		}
	}

	bool bMoveOK = (MoveFileEx(updateLocation, newLocation, type) == TRUE);

	return bMoveOK;	
}
