#ifndef THREAD_H
#define THREAD_H

#include <windows.h>
#include <process.h>

class Thread
{

private:

	//to avoid casting from MyThread* to void* to Thread*
	struct sThisStruct
	{
		Thread* pThis;
	};

	HANDLE pThread;
	UINT iThreadID;

	static unsigned __stdcall EntryPoint( void *pParams )
    {
		sThisStruct *pTS = (sThisStruct*) pParams;
		Thread* pThis = pTS->pThis;
		delete pTS;

		return pThis->Run();
    }

protected:

	virtual unsigned Run( ) = 0;
	
public:

	Thread( )
	{
		pThread = 0;
		iThreadID = 0;
	}

	virtual ~Thread( )
	{
		CloseHandle( pThread );
	}

	void Start( )
	{
		sThisStruct *pTS = new sThisStruct();
		pTS->pThis = this;

		pThread = (HANDLE)_beginthreadex( NULL, 0, EntryPoint, (void*) pTS, 0, &iThreadID );
	}

	void Join( )
	{
		WaitForSingleObject( pThread, INFINITE );
	}
};

#endif