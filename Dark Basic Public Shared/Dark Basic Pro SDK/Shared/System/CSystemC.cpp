
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


#include "csystemc.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <assert.h>
#pragma warning( push, 3 )
#pragma warning(disable:4786 4788)
#include <vector>
#pragma warning( pop )
using namespace std;
#include "dxdiaginfo.h"
#include "psapi.h"

// openGL headers for vid mem available command
//#include <GL/gl.h>
//#include <GL/glut.h> 
//#include <GL/glext.h>

#include "D3d9.h"
#include "DXGI.h"
#define _Field_size_(...)
#define _Field_size_bytes_(...)
#define _In_reads_bytes_opt_(...)
#define _Out_writes_bytes_all_opt_(...)
#define _Field_size_bytes_part_(...)
#define _In_range_(...)
#define _Out_writes_bytes_(...)
#define _Check_return_
#define _Inout_
#define _In_
#define _Out_
#define NTDDI_VERSION NTDDI_WIN7
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "d3dkmt.h"
#pragma comment(lib, "gdi32.lib")  // Doesn't do much, since it doesn't have the exports anyway...
#pragma comment(lib, "advapi32.lib")
typedef NTSTATUS (APIENTRY *PD3DKMTQueryStatistics)(_In_ CONST D3DKMT_QUERYSTATISTICS*);
typedef NTSTATUS (APIENTRY *PD3DKMTOpenAdapterFromDeviceName)(_Inout_ D3DKMT_OPENADAPTERFROMDEVICENAME*);


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DLL COntrol Structures
typedef DWORD (*MYPROC) (char*); 
DBPRO_GLOBAL HINSTANCE hDLLModule[256];

// Global Shared Data Pointer (passed in from core)
DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;
DBPRO_GLOBAL PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;

// setup typedefs to make function pointers readable
typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN ) ( void );
GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;	// get pointer to D3D device
IDirect3DDevice9*				m_pD3D = NULL;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( void )
{
	for(int dllid=0; dllid<=255; dllid++)
		hDLLModule[dllid]=NULL;
}

DARKSDK void Destructor ( void )
{
	// Free a DLLs
	for(int dllid=0; dllid<=255; dllid++)
	{
		if(hDLLModule[dllid])
		{
			FreeLibrary(hDLLModule[dllid]);
			hDLLModule[dllid]=NULL;
		}
	}
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
	g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;

	// setup function pointers
	g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_pGlob->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	if(g_GFX_GetDirect3DDevice) m_pD3D = g_GFX_GetDirect3DDevice ( ); else m_pD3D=NULL;
}

DARKSDK void RefreshD3D ( int iMode )
{
	// mike - 230406 - updated this function so D3D is reset

	// mike - 240604 - no need for this now

	/*
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		//Destructor();
	}
	if(iMode==1)
	{
		Constructor ( );
		PassCoreData ( g_pGlob );
	}
	*/
}

DARKSDK void* GetGlobalStructure ( void )
{
	// MIKE - 050104 - function to allow expansion packs to get the global structure
	return g_pGlob;
}

//
// Internal Commands
//

DARKSDK bool CallExist(HINSTANCE hDLLModule, char* DecoratedName)
{
	FARPROC fpAddress = (FARPROC)GetProcAddress(hDLLModule, DecoratedName);
	if(fpAddress)
		return true;
	else
		return false;
}

DARKSDK bool Call(HINSTANCE hDLLModule, char* DecoratedName, DWORD* pDataAddress, int paramnum, DWORD* ReturnData)
{
	FARPROC fpAddress = (FARPROC)GetProcAddress(hDLLModule, DecoratedName);
	if(fpAddress)
	{
		// Store ESP
		DWORD dwStore;
		__asm mov         dword ptr dwStore, esp

		// Params Pushed Onto Stack (backwards)
		for(int p=1; p<=paramnum; p++)
		{
			DWORD pDataItem = *((pDataAddress+paramnum)-p);
			__asm mov         eax,dword ptr pDataItem
			__asm push        eax
		}

		// Call DLL Function
		__asm call        dword ptr fpAddress
		__asm add         esp,4
		__asm mov         ecx,dword ptr ReturnData
		__asm mov         dword ptr [ecx],eax

		// Restore ESP
		__asm mov         esp, dwStore

		// Success
		return true;
	}
	else
		return false;
}

DARKSDK DWORD CallDLL_Param( int dllid, LPSTR pDLLFunction, int paramnum, DWORD* pDataAddress )
{
	DWORD ReturnData=0;
	if(dllid>=1 && dllid<=255)
	{
		if(hDLLModule[dllid])
		{
			// Call DLL-Function (CALL DLL)
			if(!Call(hDLLModule[dllid], pDLLFunction, pDataAddress, paramnum, &ReturnData))
			{
				// Fail
				RunTimeError(RUNTIMEERROR_SYSDLLCALLFAILED);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_SYSDLLNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_SYSDLLINDEXINVALID);

	return ReturnData;
}

//
// Commands
//

// System Expressions

DARKSDK int TMEMAvailable(void)
{
	MEMORYSTATUS memoryStatus = { 0 };
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	::GlobalMemoryStatus ( &memoryStatus );
	
	// return mb of total memory installed
	return ( int ) ceil ( ( double ) memoryStatus.dwTotalPhys / 1024 / 1024 );
}

// Define a function pointer to the Direct3DCreate9Ex function.
typedef HRESULT (WINAPI *LPDIRECT3DCREATE9EX)( UINT, void **);
HMODULE g_libHandle = NULL;
LPDIRECT3DCREATE9EX g_Direct3DCreate9ExPtr = NULL;

void FreeD3D9Ex ( void )
{
    // Free the library.
	if ( g_libHandle )
	{
	    FreeLibrary( g_libHandle );
		g_libHandle = NULL;
	}
}

HRESULT CheckD3D9Ex( void )
{
    HRESULT hr = E_FAIL;

    // Manually load the d3d9.dll library.
    g_libHandle = LoadLibrary("d3d9.dll");
    if(g_libHandle != NULL)
    {
        // Obtain the address of the Direct3DCreate9Ex function. 
        g_Direct3DCreate9ExPtr = (LPDIRECT3DCREATE9EX)GetProcAddress( g_libHandle, "Direct3DCreate9Ex" );
        if ( g_Direct3DCreate9ExPtr != NULL)
        {
            // Direct3DCreate9Ex is supported.
            hr = S_OK;
        }
        else
        {
            // Direct3DCreate9Ex is not supported on this
            // operating system.
            hr = ERROR_NOT_SUPPORTED;
			FreeD3D9Ex();
        }
    }
    return hr;
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

bool IsWindows8OrGreater ( void )
{
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   PGPI pGPI;
   BOOL bOsVersionInfoEx;
   DWORD dwType;

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
   if( ! bOsVersionInfoEx ) return false;

   // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
   pGNSI = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
   if(NULL != pGNSI)
	   pGNSI(&si);
   else 
	   GetSystemInfo(&si);

   if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
   {
      if ( osvi.dwMajorVersion == 6 )
      {
         if ( osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion >= 2 )
		 {
			 // Windows 8 or above
			 return true;
		 }
	  }
   }

   // otherwise NOT Windows 8
   return false;
}

DARKSDK int DMEMAvailable(void)
{
	static int Memory = 0;
	HANDLE ProcessHandle = GetCurrentProcess();
	LONGLONG dedicatedBytesUsed = 0;
	LONGLONG sharedBytesUsed = 0;
	LONGLONG committedBytesUsed = 0;
	HMODULE gdi32Handle;
	PFND3DKMT_QUERYSTATISTICS queryD3DKMTStatistics;
        
	if (gdi32Handle = LoadLibrary(TEXT("gdi32.dll")))
		queryD3DKMTStatistics = (PFND3DKMT_QUERYSTATISTICS)GetProcAddress(gdi32Handle, "D3DKMTQueryStatistics");
        
	if (queryD3DKMTStatistics && CheckD3D9Ex()==S_OK )
	{
		D3DKMT_QUERYSTATISTICS queryStatistics;
		IDirect3D9Ex* pDX = NULL;
		//Direct3DCreate9Ex ( D3D_SDK_VERSION, &pDX ); // 221214 - not supported in XP
		g_Direct3DCreate9ExPtr ( D3D_SDK_VERSION, (void**)&pDX );
		if ( pDX ) 
		{
			if ( pDX )
			{
				memset(&queryStatistics, 0, sizeof(D3DKMT_QUERYSTATISTICS));
				queryStatistics.Type = D3DKMT_QUERYSTATISTICS_PROCESS;
				pDX->GetAdapterLUID(0,&queryStatistics.AdapterLuid);
				queryStatistics.hProcess = ProcessHandle;
				if (queryD3DKMTStatistics(&queryStatistics)==0) 
				{
					committedBytesUsed = queryStatistics.QueryResult.ProcessInformation.SystemMemory.BytesAllocated;
				}
				memset(&queryStatistics, 0, sizeof(D3DKMT_QUERYSTATISTICS));
				queryStatistics.Type = D3DKMT_QUERYSTATISTICS_ADAPTER;
				pDX->GetAdapterLUID(0,&queryStatistics.AdapterLuid);
				if (queryD3DKMTStatistics(&queryStatistics)==0) 
				{
					ULONG i;
					ULONG segmentCount = queryStatistics.QueryResult.AdapterInformation.NbSegments;
					for (i = 0; i < segmentCount; i++) 
					{
						memset(&queryStatistics, 0, sizeof(D3DKMT_QUERYSTATISTICS));
						queryStatistics.Type = D3DKMT_QUERYSTATISTICS_SEGMENT;
						pDX->GetAdapterLUID(0,&queryStatistics.AdapterLuid);
						queryStatistics.QuerySegment.SegmentId = i;
						if (queryD3DKMTStatistics(&queryStatistics)==0) 
						{
							// Detect Windows OS
							bool aperture = false;
							if ( IsWindows8OrGreater() )
							{
								// Windows 8 and above is aperture = queryStatistics.QueryResult.SegmentInformation.Aperture;
								aperture = queryStatistics.QueryResult.SegmentInformation.Aperture;
							}
							else
							{
								// Windows 7
								aperture = queryStatistics.QueryResult.SegmentInformationV1.Aperture;
							}
                        
							memset(&queryStatistics, 0, sizeof(D3DKMT_QUERYSTATISTICS));
							queryStatistics.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT;
							pDX->GetAdapterLUID(0,&queryStatistics.AdapterLuid);
							queryStatistics.hProcess = ProcessHandle;
							queryStatistics.QueryProcessSegment.SegmentId = i;
							if (queryD3DKMTStatistics(&queryStatistics)==0)
							{
								if (aperture)
									sharedBytesUsed += queryStatistics.QueryResult
																		.ProcessSegmentInformation
																		.BytesCommitted;
								else
									dedicatedBytesUsed += queryStatistics.QueryResult
																			.ProcessSegmentInformation
																			.BytesCommitted;
							}
						}
					}
				}

				// free DX9Ex when done
				pDX->Release();
			}
		}
	}
	FreeD3D9Ex();
        
	// free GDI DLL
	FreeLibrary(gdi32Handle);

	// Pass dedicated memory used back to DBP
	Memory = dedicatedBytesUsed / 1024 / 1024;
	return Memory;
}

DARKSDK int SMEMAvailable(void)
{
	// mike - 240604 - no need for this now
//	MEMORYSTATUS memstatus;
//	memstatus.dwLength = sizeof(MEMORYSTATUS);
//	GlobalMemoryStatus(&memstatus);
//	return (int)memstatus.dwAvailPageFile;

	// mike - 240604 - new code for getting available system memory

		MEMORYSTATUS memoryStatus = { 0 };
	
	memoryStatus.dwLength = sizeof ( MEMORYSTATUS );
	
	::GlobalMemoryStatus ( &memoryStatus );

	// return mb of total memory available
	return ( int ) ceil ( ( double ) memoryStatus.dwAvailPhys / 1024 / 1024 );
}

DARKSDK int SMEMAvailable( int iMode )
{
	if ( iMode==2 )
	{
		// U75 - 050510 - returns Virtual Size (most apps will crash if this goes above 2GB!)
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		DWORDLONG dwVMSize1 = statex.ullTotalVirtual / 1024;
		DWORDLONG dwVMSize2 = statex.ullAvailVirtual / 1024;
		int dwVMSize = (int)(dwVMSize1-dwVMSize2);
		return dwVMSize;
	}
	else
	{
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;
		DWORD dwProcessID = GetCurrentProcessId();
		memset ( &pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS) );
		pmc.cb = sizeof ( PROCESS_MEMORY_COUNTERS );
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID );
		if (NULL == hProcess)
			return 0;
		else
		{
			GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc) );
			CloseHandle( hProcess );
			return pmc.PagefileUsage / 1024;
		}
	}
}

// global activity

DARKSDK void DisableEscapeKey(void)
{
	g_pGlob->bEscapeKeyEnabled=false;
}

DARKSDK void EnableEscapeKey(void)
{
	g_pGlob->bEscapeKeyEnabled=true;
}

DARKSDK void DisableSystemKeys(void)
{
	UINT nPreviousState;
	SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, TRUE, &nPreviousState, 0);
	g_pGlob->bSystemKeyEnabled=false;
}

DARKSDK void EnableSystemKeys(void)
{
	UINT nPreviousState;
	SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, FALSE, &nPreviousState, 0); 
	g_pGlob->bSystemKeyEnabled=true;
}

DARKSDK void ExitPrompt(DWORD pString, DWORD pString2)
{
	LPSTR pReturnString=NULL;
	if(pString)
	{
		DWORD dwSize=strlen((LPSTR)pString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		g_pGlob->pExitPromptString = pReturnString;
		strcpy(pReturnString, (LPSTR)pString);
	}
	if(pString2)
	{
		DWORD dwSize=strlen((LPSTR)pString2);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		g_pGlob->pExitPromptString2 = pReturnString;
		strcpy(pReturnString, (LPSTR)pString2);
	}
}

// checklist commands

DARKSDK void EmptyChecklist( void )
{
	g_pGlob->checklistexists = false;
	g_pGlob->checklisthasvalues = false;
	g_pGlob->checklisthasstrings = false;
	g_pGlob->checklistqty = 0;
	for(DWORD ci=0; ci<g_pGlob->dwChecklistArraySize; ci++)
	{
		g_pGlob->checklist[ci].valuea=-1;
		g_pGlob->checklist[ci].valueb=-1;
		g_pGlob->checklist[ci].valuec=-1;
		g_pGlob->checklist[ci].valued=-1;
		strcpy(g_pGlob->checklist[ci].string, "");
	}
}

DARKSDK int ChecklistQuantity( void )
{
	return g_pGlob->checklistqty;
}

DARKSDK bool CheckListForValue ( int iIndex )
{
	if(g_pGlob->checklistexists==false)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTNOTEXIST);
		return false;
	}
	if(iIndex<1 || iIndex>(int)g_pGlob->dwChecklistArraySize)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTILLEGALNUMBER);
		return false;
	}
	if(g_pGlob->checklisthasvalues==false)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTONLYSTRINGS);
		return false;
	}
	if(iIndex>g_pGlob->checklistqty)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTNUMBERWRONG);
		return false;
	}
	return true;
}

DARKSDK int ChecklistValueA( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return g_pGlob->checklist[iIndex-1].valuea;
}

DARKSDK int ChecklistValueB( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return g_pGlob->checklist[iIndex-1].valueb;
}

DARKSDK int ChecklistValueC( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return g_pGlob->checklist[iIndex-1].valuec;
}

DARKSDK int ChecklistValueD( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return g_pGlob->checklist[iIndex-1].valued;
}

DARKSDK DWORD ChecklistFValueA ( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return *(DWORD*)&g_pGlob->checklist[iIndex-1].fvaluea;
}

DARKSDK DWORD ChecklistFValueB ( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return *(DWORD*)&g_pGlob->checklist[iIndex-1].fvalueb;
}

DARKSDK DWORD ChecklistFValueC ( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return *(DWORD*)&g_pGlob->checklist[iIndex-1].fvaluec;
}

DARKSDK DWORD ChecklistFValueD ( int iIndex )
{
	if ( !CheckListForValue ( iIndex ) ) return 0;
	return *(DWORD*)&g_pGlob->checklist[iIndex-1].fvalued;
}

DARKSDK DWORD ChecklistString( DWORD pDestStr, int iIndex )
{
	if(g_pGlob->checklistexists==false)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTNOTEXIST);
		return NULL;
	}
	if(iIndex<1 || iIndex>(int)g_pGlob->dwChecklistArraySize)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTILLEGALNUMBER);
		return NULL;
	}
	if(g_pGlob->checklisthasstrings==false)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTONLYVALUES);
		return NULL;
	}
	if(iIndex>g_pGlob->checklistqty)
	{
		RunTimeError(RUNTIMEERROR_CHECKLISTNUMBERWRONG);
		return NULL;
	}

	// Free old string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

	// Return string
	LPSTR pReturnString=NULL;
	if(g_pGlob->checklist[iIndex-1].string)
	{
		DWORD dwSize=strlen(g_pGlob->checklist[iIndex-1].string);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, g_pGlob->checklist[iIndex-1].string);
	}
	return (DWORD)pReturnString;
}

// DLL Commands

DARKSDK void DLLLoadCore( DWORD pDLLFile, int dllid )
{
	// Load a DLL (LOAD DLL)
	if(dllid>=1 && dllid<=255)
	{
		if(hDLLModule[dllid]==NULL)
		{
			hDLLModule[dllid] = LoadLibrary((LPSTR)pDLLFile);
			if(!hDLLModule[dllid])
			{
				RunTimeSoftWarning(RUNTIMEERROR_SYSCOULDNOTLOADDLL);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_SYSDLLALREADYEXISTS);
	}
	else
		RunTimeError(RUNTIMEERROR_SYSDLLINDEXINVALID);
}

DARKSDK void DLLLoad( DWORD pDLLFile, int dllid )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)pDLLFile);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	DLLLoadCore( (DWORD)VirtualFilename, dllid );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void DLLDelete( int dllid )
{
	// Free a DLL (DELETE DLL)
	if(dllid>=1 && dllid<=255)
	{
		if(hDLLModule[dllid])
		{
			FreeLibrary(hDLLModule[dllid]);
			hDLLModule[dllid]=NULL;
		}
		else
			RunTimeError(RUNTIMEERROR_SYSDLLNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_SYSDLLINDEXINVALID);
}

DARKSDK int DLLExist( int dllid )
{
	int iRes=0;
	if(dllid>=1 && dllid<=255)
	{
		if(hDLLModule[dllid]) iRes=1;
	}
	else
		RunTimeError(RUNTIMEERROR_SYSDLLINDEXINVALID);

	return iRes;
}

DARKSDK int DLLCallExist( int dllid, DWORD pDLLFunction )
{
	int iRes=0;
	if(dllid>=1 && dllid<=255)
	{
		if(hDLLModule[dllid])
		{
			// Call DLL-Function (CALL DLL)
			if(CallExist(hDLLModule[dllid], (LPSTR)pDLLFunction)) iRes=1;
		}
	}
	else
		RunTimeError(RUNTIMEERROR_SYSDLLINDEXINVALID);

	return iRes;
}

DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction )
{
	DWORD Data[1]; Data[0]=0;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 0, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1 )
{
	DWORD Data[2]; Data[0]=P1;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 1, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2 )
{
	DWORD Data[3]; Data[0]=P1; Data[1]=P2;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 2, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3 )
{
	DWORD Data[4]; Data[0]=P1; Data[1]=P2; Data[2]=P3;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 3, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4 )
{
	DWORD Data[5]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 4, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5 )
{
	DWORD Data[6]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 5, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6 )
{
	DWORD Data[7]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5; Data[5]=P6;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 6, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7 )
{
	DWORD Data[8]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5; Data[5]=P6; Data[6]=P7;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 7, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8 )
{
	DWORD Data[9]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5; Data[5]=P6; Data[6]=P7; Data[7]=P8;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 8, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 )
{
	DWORD Data[10]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5; Data[5]=P6; Data[6]=P7; Data[7]=P8; Data[8]=P9;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 9, Data );
}
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 )
{
	DWORD Data[10]; Data[0]=P1; Data[1]=P2; Data[2]=P3; Data[3]=P4; Data[4]=P5; Data[5]=P6; Data[6]=P7; Data[7]=P8; Data[8]=P9; Data[9]=P10;
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, 10, Data );
}
DARKSDK DWORD CallDLLX( int dllid, DWORD pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS )
{
	return CallDLL_Param( dllid, (LPSTR)pDLLFunction, (int)dwNumberOfDWORDS, (DWORD*)pDataPtr );
}

DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction )
{
	CallDLL( dllid, pDLLFunction );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1 )
{
	CallDLL( dllid, pDLLFunction, P1 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2 )
{
	CallDLL( dllid, pDLLFunction, P1,P2 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5,P6 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5,P6,P7 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5,P6,P7,P8 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5,P6,P7,P8,P9 );
}
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 )
{
	CallDLL( dllid, pDLLFunction, P1,P2,P3,P4,P5,P6,P7,P8,P9,P10 );
}
DARKSDK void _CallDLLX( int dllid, DWORD pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS )
{
	CallDLLX( dllid, pDLLFunction, pDataPtr, dwNumberOfDWORDS );
}

//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorSystem ( void )
{
	Constructor ( );
}

void DestructorSystem ( void )
{
	Destructor ( );
}

void SetErrorHandlerSystem ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataSystem( LPVOID pGlobPtr )
{
	PassCoreData( pGlobPtr );
}

void RefreshD3DSystem ( int iMode )
{
	RefreshD3D ( iMode );
}

int dbSystemTMEMAvailable (void)
{
	return TMEMAvailable ( );
}

int dbSystemDMEMAvailable(void)
{
	return DMEMAvailable ( );
}

int dbSystemSMEMAvailable(void)
{
	return SMEMAvailable ( );
}

void dbDisableEscapeKey(void)
{
	DisableEscapeKey ( );
}

void dbEnableEscapeKey(void)
{
	EnableEscapeKey ( );
}

void dbDisableSystemKeys(void)
{
	DisableSystemKeys ( );
}

void dbEnableSystemKeys(void)
{
	EnableSystemKeys ( );
}

void dbExitPrompt(char* pString, char* pString2)
{
	ExitPrompt( ( DWORD ) pString, ( DWORD ) pString2);
}

void dbEmptyChecklist( void )
{
	EmptyChecklist ( );
}

int dbChecklistQuantity( void )
{
	return ChecklistQuantity ( );
}

int dbChecklistValueA( int iIndex )
{
	return ChecklistValueA(  iIndex );
}

int dbChecklistValueB( int iIndex )
{
	return ChecklistValueB(  iIndex );
}

int dbChecklistValueC( int iIndex )
{
	return ChecklistValueC(  iIndex );
}

int dbChecklistValueD( int iIndex )
{
	return ChecklistValueD(  iIndex );
}

float dbChecklistFValueA( int iIndex )
{
	DWORD dwReturn = ChecklistFValueA(  iIndex );
	
	return *( float* ) &dwReturn;
}

float dbChecklistFValueB( int iIndex )
{
	DWORD dwReturn = ChecklistFValueB(  iIndex );
	
	return *( float* ) &dwReturn;
}

float dbChecklistFValueC( int iIndex )
{
	DWORD dwReturn = ChecklistFValueC(  iIndex );
	
	return *( float* ) &dwReturn;
}

float dbChecklistFValueD( int iIndex )
{
	DWORD dwReturn = ChecklistFValueD(  iIndex );
	
	return *( float* ) &dwReturn;
}

char* dbChecklistString( int iIndex )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = ChecklistString ( NULL, iIndex );

	szReturn = ( char* ) dwReturn;

	return szReturn;

	//return ChecklistString(  pDestStr,  iIndex );
}

void dbDLLLoad( DWORD pDLLFile, int dllid )
{
	DLLLoad(  pDLLFile,  dllid );
}

void dbDLLDelete( int dllid )
{
	DLLDelete(  dllid );
}

int dbDLLExist( int dllid )
{
	return DLLExist( dllid );
}

int dbDLLCallExist( int dllid, char* pDLLFunction )
{
	return DLLCallExist(  dllid,  ( DWORD ) pDLLFunction );
}

DWORD dbCallDLL( int dllid, char* pDLLFunction)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5, P6);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5, P6, P7);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8)
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5, P6, P7, P8);
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 )
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5, P6, P7, P8, P9 );
}

DWORD dbCallDLL( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 )
{
	return CallDLL(  dllid,  ( DWORD ) pDLLFunction,  P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
}

DWORD dbCallDLLX( int dllid, char* pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS )
{
	return CallDLLX(  dllid,  ( DWORD ) pDLLFunction,  pDataPtr,  dwNumberOfDWORDS );
}

void* dbGetGlobalStructure ( void )
{
	return GetGlobalStructure ( );
}

void dbLoadDLL ( char* pDLLFile, int dllid )
{
	dbDLLLoad ( ( DWORD ) pDLLFile, dllid );
}

// lee - 300706 - GDK fixes
//void dbLoadDLL ( DWORD pDLLFile, int dllid ) { dbDLLLoad ( pDLLFile, dllid ); }
void dbDeleteDLL ( int dllid ) { dbDLLDelete ( dllid ); }
int	dbExistDLL ( int dllid ) { return dbDLLExist ( dllid ); }
//int	dbDLLCallExist ( int dllid, char* pDLLFunction ) { return dbDLLCallExist ( dllid, (DWORD)pDLLFunction ); }
int	dbSystemTmemAvailable ( void ) { return dbSystemTMEMAvailable (); }
int	dbSystemDmemAvailable ( void ) { return dbSystemDMEMAvailable (); }
int	dbSystemSmemAvailable ( void ) { return dbSystemSMEMAvailable (); }
void dbDisableEscapekey(void)
{
	DisableEscapeKey ( );
}
void dbEnableEscapekey(void)
{
	EnableEscapeKey ( );
}

#endif


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////