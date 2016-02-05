#ifndef INTERPROCESSCOMMUNICATION_H__
#define INTERPROCESSCOMMUNICATION_H__

#include <windows.h>
#include <tchar.h>

struct tInterData
{
	DWORD ServerPID;
	DWORD ClientPID;
};

class cIPC
{
	public:
		cIPC  ( LPCTSTR SharedName, DWORD Size, BOOL bHandlesInheritable = FALSE );
		~cIPC ( );

		void   SendBuffer			( LPVOID Buffer, DWORD dwOffset, DWORD Size );
		void   ReceiveBuffer		( LPVOID Buffer, DWORD dwOffset, DWORD Size );

	public:

		HANDLE 			m_hDataEvent;

	private:

		DWORD  			LastError;
		bool   			m_bIsServer;
		tInterData*		m_ipcd;
		DWORD			m_nSize;
		DWORD			m_PID;
		HANDLE 			m_hFileMap;
		HANDLE 			m_hDataMutex;
		LPVOID 			m_lpMem;
		LPVOID 			m_lpMappedViewOfFile;
		TCHAR			m_szFileMapName [ 80 ];
		TCHAR  			m_szMutexName   [ 80 ];
		TCHAR  			m_szEventName   [ 80 ];
		BOOL   			m_bHandlesInheritable;
};

#endif