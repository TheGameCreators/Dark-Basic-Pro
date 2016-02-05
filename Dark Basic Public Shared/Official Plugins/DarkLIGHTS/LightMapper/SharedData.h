#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <windows.h>
#include <process.h>

class SharedData
{
	
private:

	char pStatus[256];
	float fPercentage;
	bool bComplete;
	bool bTerminate;
	HANDLE pMutex;

	HANDLE pTextureLockMutex;

public:

	SharedData( );
	~SharedData( );

	bool SetStatus( char* pMessage, float percent );
	void SetComplete( bool complete );
	bool GetStatus( char* pOutStr );
	float GetPercent( );
	bool GetComplete( );

	void SetTerminate( bool terminate );
	bool GetTerminate( );

	bool GrabTextureLockMutex( ); //waits thread until available
	void ReleaseTextureLockMutex( );
};

#endif