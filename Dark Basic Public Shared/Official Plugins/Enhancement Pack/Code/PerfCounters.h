#include <windows.h>
#include <stdio.h>
#include <comdef.h>	// for using bstr_t class

#define TOTALBYTES    100*1024
#define BYTEINCREMENT 10*1024

template <class T>
class CPerfCounters
{
public:
	CPerfCounters()
	{
	}
	~CPerfCounters()
	{
	}

	T GetCounterValue(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, DWORD dwCounterIndex, LPCTSTR pInstanceName = NULL)
	{
		QueryPerformanceData(pPerfData, dwObjectIndex, dwCounterIndex);

	    PPERF_OBJECT_TYPE pPerfObj = NULL;
		T lnValue = {0};

		// Get the first object type.
		pPerfObj = FirstObject( *pPerfData );

		// Look for the given object index

		for( DWORD i=0; i < (*pPerfData)->NumObjectTypes; i++ )
		{

			if (pPerfObj->ObjectNameTitleIndex == dwObjectIndex)
			{
				lnValue = GetCounterValue(pPerfObj, dwCounterIndex, pInstanceName);
				break;
			}

			pPerfObj = NextObject( pPerfObj );
		}
		return lnValue;
	}

protected:

	class CBuffer
	{
	public:
		CBuffer(UINT Size)
		{
			m_Size = Size;
			m_pBuffer = (LPBYTE) malloc( Size*sizeof(BYTE) );
		}
		~CBuffer()
		{
			free(m_pBuffer);
		}
		void *Realloc(UINT Size)
		{
			m_Size = Size;
			m_pBuffer = (LPBYTE) realloc( m_pBuffer, Size );
			return m_pBuffer;
		}

		void Reset()
		{
			memset(m_pBuffer,NULL,m_Size);
		}
		operator LPBYTE ()
		{
			return m_pBuffer;
		}

		UINT GetSize()
		{
			return m_Size;
		}
	public:
		LPBYTE m_pBuffer;
	private:
		UINT m_Size;
	};

	//
	//	The performance data is accessed through the registry key 
	//	HKEY_PEFORMANCE_DATA.
	//	However, although we use the registry to collect performance data, 
	//	the data is not stored in the registry database.
	//	Instead, calling the registry functions with the HKEY_PEFORMANCE_DATA key 
	//	causes the system to collect the data from the appropriate system 
	//	object managers.
	//
	//	QueryPerformanceData allocates memory block for getting the
	//	performance data.
	//
	//
	void QueryPerformanceData(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, DWORD dwCounterIndex)
	{
		//
		// Since i want to use the same allocated area for each query,
		// i declare CBuffer as static.
		// The allocated is changed only when RegQueryValueEx return ERROR_MORE_DATA
		//
		static CBuffer Buffer(TOTALBYTES);

		DWORD BufferSize = Buffer.GetSize();
		LONG lRes;

		char keyName[8];
		sprintf(keyName,"%d %d",dwObjectIndex, dwCounterIndex);

		Buffer.Reset();
		while( (lRes = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
								   keyName,
								   NULL,
								   NULL,
								   Buffer,
								   &BufferSize )) == ERROR_MORE_DATA )
		{
			// Get a buffer that is big enough.

			BufferSize += BYTEINCREMENT;
			Buffer.Realloc(BufferSize);
		}
		*pPerfData = (PPERF_DATA_BLOCK) Buffer.m_pBuffer;
	}

	//
	//	GetCounterValue gets performance object structure
	//	and returns the value of given counter index .
	//	This functions iterates through the counters of the input object
	//	structure and looks for the given counter index.
	//
	//	For objects that have instances, this function returns the counter value
	//	of the instance pInstanceName.
	//
	T GetCounterValue(PPERF_OBJECT_TYPE pPerfObj, DWORD dwCounterIndex, LPCTSTR pInstanceName)
	{
		PPERF_COUNTER_DEFINITION pPerfCntr = NULL;
		PPERF_INSTANCE_DEFINITION pPerfInst = NULL;
		PPERF_COUNTER_BLOCK pCounterBlock = NULL;

		// Get the first counter.

		pPerfCntr = FirstCounter( pPerfObj );

		// Look for the index of '% Total processor time'

		for( DWORD j=0; j < pPerfObj->NumCounters; j++ )
		{
			if (pPerfCntr->CounterNameTitleIndex == dwCounterIndex)
				break;

			// Get the next counter.

			pPerfCntr = NextCounter( pPerfCntr );
		}

		if( pPerfObj->NumInstances == PERF_NO_INSTANCES )		
		{
			pCounterBlock = (PPERF_COUNTER_BLOCK) ((LPBYTE) pPerfObj + pPerfObj->DefinitionLength);
		}
		else
		{
			pPerfInst = FirstInstance( pPerfObj );
		
			// Look for instance pInstanceName
			_bstr_t bstrInstance;
			_bstr_t bstrInputInstance = pInstanceName;
			for( int k=0; k < pPerfObj->NumInstances; k++ )
			{
				bstrInstance = (wchar_t *)((PBYTE)pPerfInst + pPerfInst->NameOffset);
				if (!stricmp((LPCTSTR)bstrInstance, (LPCTSTR)bstrInputInstance))
				{
					pCounterBlock = (PPERF_COUNTER_BLOCK) ((LPBYTE) pPerfInst + pPerfInst->ByteLength);
					break;
				}
				
				// Get the next instance.

				pPerfInst = NextInstance( pPerfInst );
			}
		}

		if (pCounterBlock)
		{
			T *lnValue = NULL;
			lnValue = (T*)((LPBYTE) pCounterBlock + pPerfCntr->CounterOffset);
			return *lnValue;
		}
		return -1;
	}


	/*****************************************************************
	 *                                                               *
	 * Functions used to navigate through the performance data.      *
	 *                                                               *
	 *****************************************************************/

	PPERF_OBJECT_TYPE FirstObject( PPERF_DATA_BLOCK PerfData )
	{
		return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + PerfData->HeaderLength) );
	}

	PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj )
	{
		return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + PerfObj->TotalByteLength) );
	}

	PPERF_COUNTER_DEFINITION FirstCounter( PPERF_OBJECT_TYPE PerfObj )
	{
		return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + PerfObj->HeaderLength) );
	}

	PPERF_COUNTER_DEFINITION NextCounter( PPERF_COUNTER_DEFINITION PerfCntr )
	{
		return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + PerfCntr->ByteLength) );
	}

	PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj )
	{
		return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + PerfObj->DefinitionLength) );
	}

	PPERF_INSTANCE_DEFINITION NextInstance( PPERF_INSTANCE_DEFINITION PerfInst )
	{
		PPERF_COUNTER_BLOCK PerfCntrBlk;

		PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + PerfInst->ByteLength);

		return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + PerfCntrBlk->ByteLength) );
	}
};