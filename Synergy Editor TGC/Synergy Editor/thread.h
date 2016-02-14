#ifndef THREAD_H
#define THREAD_H

#include <windows.h>
#include <process.h>

class Thread
{

private:

	HANDLE pThread;
	UINT iThreadID;

	static unsigned __stdcall EntryPoint( void *pParams )
    {
		Thread* pThis = (Thread*)pParams;
		
		return pThis->Run();
    }

protected:

	//this is the function that will get called when the thread is run, must be filled out in a sub-class
	virtual unsigned Run( ) = 0;
	
public:

	Thread( )
	{
		pThread = 0;
		iThreadID = 0;
	}

	virtual ~Thread( )
	{
		if ( pThread )
		{
			CloseHandle( pThread );
		}
	}

	//start the thread
	virtual void Start( )
	{
		if ( IsRunning() ) return;
		
		pThread = (HANDLE)_beginthreadex( NULL, 0, EntryPoint, (void*) this, 0, &iThreadID );
	}

	//call this to wait for this thread to finish (returns immediately if the thread has stopped or was never started)
	void Join( )
	{
		if ( pThread ) WaitForSingleObject( pThread, INFINITE );
	}

	//call this to determine if the thread is still running (returns immediately)
	bool IsRunning( )
	{
		bool bRunning = false;
		
		if ( pThread ) bRunning = (WaitForSingleObject( pThread, 0 ) != WAIT_OBJECT_0);
		
		return bRunning;
	}

	//end the thread prematurely
	void Terminate( )
	{
		if ( pThread ) 
		{
			//TerminateThread( pThread, 0 ); //Vista doesn't approve of forced termination, sub-classes should overload this entire function and include a termination code path
			CloseHandle( pThread );
		}

		pThread = 0;
		iThreadID = 0;
	}
};

#endif