
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	PROCESSOR COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#include "stdafx.h"
#include "oscpuinfo.h"
#include "cpuusage.h"
#include "core.h"

CPUInfo	g_CPU;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK int   GetProcessorCount				( void );
DARKSDK DWORD GetProcessorVendor			( DWORD dwReturn, int iID );
DARKSDK int   GetProcessorTypeID			( int iID );
DARKSDK int   GetProcessorFamilyID			( int iID );
DARKSDK int   GetProcessorModelID			( int iID );
DARKSDK int   GetProcessorSteppingID		( int iID );
DARKSDK DWORD GetProcessorBrand				( DWORD dwReturn, int iID );
DARKSDK DWORD GetProcessorPlatform			( DWORD dwReturn, int iID );
DARKSDK DWORD GetProcessorName				( DWORD dwReturn, int iID );
DARKSDK int   GetProcessorFrequency			( int iID );
DARKSDK void  GetProcessorOnChipFeatures    ( int iID );
DARKSDK void  GetProcessorPowerManagement   ( int iID );
DARKSDK void  GetProcessorSupportedFeatures ( int iID );

DARKSDK int   GetProcessorUsage             ( int iID );

char* GetOS ( void );

/*
	// export names for string table -

	GET PROCESSOR COUNT[%L%?GetProcessorCount@@YAHXZ%
	GET PROCESSOR VENDOR$[%SL%?GetProcessorVendor@@YAKKH@Z%ID
	GET PROCESSOR TYPE ID[%LL%?GetProcessorTypeID@@YAHH@Z%ID
	GET PROCESSOR FAMILY ID[%LL%?GetProcessorFamilyID@@YAHH@Z%ID
	GET PROCESSOR MODEL ID[%LL%?GetProcessorModelID@@YAHH@Z%ID
	GET PROCESSOR STEPPING ID[%LL%?GetProcessorSteppingID@@YAHH@Z%ID
	GET PROCESSOR BRAND$[%SL%?GetProcessorBrand@@YAKKH@Z%ID
	GET PROCESSOR PLATFORM$[%SL%?GetProcessorPlatform@@YAKKH@Z%ID
	GET PROCESSOR NAME$[%SL%?GetProcessorName@@YAKKH@Z%ID
	GET PROCESSOR FREQUENCY[%LL%?GetProcessorFrequency@@YAHH@Z%ID
	GET PROCESSOR ON CHIP FEATURES%L%?GetProcessorOnChipFeatures@@YAXH@Z%ID
	GET PROCESSOR POWER MANAGEMENT%L%?GetProcessorPowerManagement@@YAXH@Z%ID
	GET PROCESSOR SUPPORTED FEATURES%L%?GetProcessorSupportedFeatures@@YAXH@Z%ID

	GET PROCESSOR USAGE[%LL%?GetProcessorUsage@@YAHH@Z%ID
*/

CCpuUsage g_ProcessorUsage;

int GetProcessorUsage ( int iID )
{
	return g_ProcessorUsage.GetCpuUsage ( );
}

int GetProcessorCount ( void )
{
	return 1;
}

DWORD GetProcessorVendor ( DWORD dwReturn, int iID )
{
	return ( DWORD ) SetupString ( g_CPU.GetVendorID ( ) );
}

int GetProcessorTypeID ( int iID )
{
	char* szString = g_CPU.GetTypeID ( );

	int iValue = atoi ( szString );

	SAFE_DELETE_ARRAY ( szString );

	return iValue;
	

	char* pString = SetupString ( g_CPU.GetTypeID ( ) );

	return atoi ( pString );
}

int GetProcessorFamilyID ( int iID )
{
	char* szString = g_CPU.GetFamilyID ( );

	int iValue = atoi ( szString );

	SAFE_DELETE_ARRAY ( szString );

	return iValue;

	char* pString = SetupString ( g_CPU.GetFamilyID ( ) );

	return atoi ( pString );
}

int GetProcessorModelID	( int iID )
{
	char* szString = g_CPU.GetModelID ( );

	int iValue = atoi ( szString );

	SAFE_DELETE_ARRAY ( szString );

	return iValue;

	char* pString = SetupString ( g_CPU.GetModelID ( ) );
	
	return atoi ( pString );
}

int GetProcessorSteppingID ( int iID )
{
	char* szString = g_CPU.GetSteppingCode ( );

	int iValue = atoi ( szString );

	SAFE_DELETE_ARRAY ( szString );

	return iValue;

	char* pString = SetupString ( g_CPU.GetSteppingCode ( ) );
	
	return atoi ( pString );
}

DWORD GetProcessorBrand ( DWORD dwReturn, int iID )
{
	return ( DWORD ) SetupString ( g_CPU.GetVendorString ( ) );
}

DWORD GetProcessorPlatform ( DWORD dwReturn, int iID )
{
	
	char* szString = GetOS ( );

	DWORD dwA = ( DWORD ) SetupString ( szString );

	SAFE_DELETE_ARRAY ( szString );

	return dwA;

	return ( DWORD ) SetupString ( GetOS ( ) );
}

DWORD GetProcessorName ( DWORD dwReturn, int iID )
{
	return ( DWORD ) SetupString ( g_CPU.GetExtendedProcessorName ( ) );
}

int GetProcessorFrequency ( int iID )
{
	int iMhz = g_CPU.GetProcessorClockFrequency ( );

	return iMhz;
}

void GetProcessorOnChipFeatures ( int iID )
{
	char szFeatureList [ 100 ] [ 256 ];
	char szTemp [ 256 ];
	int  iFeatureCount = 0;

	if ( g_CPU.DoesCPUSupportFeature ( L1CACHE_FEATURE ) )
	{
		itoa ( g_CPU.GetProcessorCacheXSize ( L1CACHE_FEATURE ), szTemp, 10 );

		strcpy ( szFeatureList [ iFeatureCount ], "L1 cache = " );
		strcat ( szFeatureList [ iFeatureCount ], szTemp );
		strcat ( szFeatureList [ iFeatureCount ], " kb" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( L2CACHE_FEATURE ) )
	{
		itoa ( g_CPU.GetProcessorCacheXSize ( L2CACHE_FEATURE ), szTemp, 10 );

		strcpy ( szFeatureList [ iFeatureCount ], "L2 cache = " );
		strcat ( szFeatureList [ iFeatureCount ], szTemp );
		strcat ( szFeatureList [ iFeatureCount ], " kb" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( L3CACHE_FEATURE ) )
	{
		itoa ( g_CPU.GetProcessorCacheXSize ( L3CACHE_FEATURE ), szTemp, 10 );

		strcpy ( szFeatureList [ iFeatureCount ], "L3 cache = " );
		strcat ( szFeatureList [ iFeatureCount ], szTemp );
		strcat ( szFeatureList [ iFeatureCount ], " kb" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( APIC_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "APIC present" );
		iFeatureCount++;
	}

	if ( !g_pGlob )
	{
		Error ( 1 );
		return;
	}
	
	g_pGlob->checklistexists     = true;
	g_pGlob->checklisthasvalues  = true;
	g_pGlob->checklisthasstrings = true;
	g_pGlob->checklistqty        = iFeatureCount;

	for ( int i = 0; i < iFeatureCount; i++ )
	{
		GlobExpandChecklist ( i, 256 );

		strcpy ( g_pGlob->checklist [ i ].string, szFeatureList [ i ] );
	}
}

void GetProcessorPowerManagement ( int iID )
{
	char szFeatureList [ 100 ] [ 256 ];
	int  iFeatureCount = 0;

	if ( g_CPU.DoesCPUSupportFeature ( THERMALMONITOR_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "Thermal monitor" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( TEMPSENSEDIODE_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "Temperature sensor" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( FREQUENCYID_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "Frequency ID" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( VOLTAGEID_FREQUENCY ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "Voltage ID" );
		iFeatureCount++;
	}

	if ( !g_pGlob )
	{
		Error ( 1 );
		return;
	}

	g_pGlob->checklistexists     = true;
	g_pGlob->checklisthasvalues  = true;
	g_pGlob->checklisthasstrings = true;
	g_pGlob->checklistqty        = iFeatureCount;

	for ( int i = 0; i < iFeatureCount; i++ )
	{
		GlobExpandChecklist ( i, 256 );

		strcpy ( g_pGlob->checklist [ i ].string, szFeatureList [ i ] );
	}
}

void GetProcessorSupportedFeatures ( int iID )
{
	char szFeatureList [ 100 ] [ 256 ];
	int  iFeatureCount = 0;

	if ( g_CPU.DoesCPUSupportFeature ( MMX_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "MMX" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( MMX_PLUS_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "MMX+" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( SSE_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "SSE" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( SSE_FP_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "SSE FP" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( SSE_MMX_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "SSE MMX" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( SSE2_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "SSE2" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( AMD_3DNOW_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "AMD 3D Now" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( AMD_3DNOW_PLUS_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "AMD 3D Now+" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( IA64_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "IA 64" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( MP_CAPABLE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "MP" );
		iFeatureCount++;
	}

	if ( g_CPU.DoesCPUSupportFeature ( SERIALNUMBER_FEATURE ) )
	{
		strcpy ( szFeatureList [ iFeatureCount ], "Serial" );
		iFeatureCount++;
	}

	if ( !g_pGlob )
	{
		Error ( 1 );
		return;
	}

	g_pGlob->checklistexists     = true;
	g_pGlob->checklisthasvalues  = true;
	g_pGlob->checklisthasstrings = true;
	g_pGlob->checklistqty        = iFeatureCount;

	for ( int i = 0; i < iFeatureCount; i++ )
	{
		GlobExpandChecklist ( i, 256 );

		strcpy ( g_pGlob->checklist [ i ].string, szFeatureList [ i ] );
	}
}

char* GetOS ( void )
{
	OSVERSIONINFOEX osvi;
	BOOL bIsWindows64Bit;
	BOOL bOsVersionInfoEx;
	char * szOperatingSystem = new char [256];

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	ZeroMemory (&osvi, sizeof (OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) {
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx ((OSVERSIONINFO *) &osvi)) return NULL;
	}

	switch (osvi.dwPlatformId) {
		case VER_PLATFORM_WIN32_NT:
			// Test for the product.
			if (osvi.dwMajorVersion <= 4) strcpy (szOperatingSystem, "Microsoft Windows NT ");
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) strcpy (szOperatingSystem, "Microsoft Windows 2000 ");
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) strcpy (szOperatingSystem, "Microsoft Windows XP ");


			// Display version, service pack (if any), and build number.
			if (osvi.dwMajorVersion <= 4)
			{
				
			} else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
				// Windows XP and .NET server.
				typedef BOOL (CALLBACK* LPFNPROC) (HANDLE, BOOL *);
				HINSTANCE hKernelDLL; 
				LPFNPROC DLLProc;
				
				// Load the Kernel32 DLL.
				hKernelDLL = LoadLibrary ("kernel32");
				if (hKernelDLL != NULL)  { 
					// Only XP and .NET Server support IsWOW64Process so... Load dynamically!
					DLLProc = (LPFNPROC) GetProcAddress (hKernelDLL, "IsWow64Process"); 
				 
					// If the function address is valid, call the function.
					if (DLLProc != NULL) (DLLProc) (GetCurrentProcess (), &bIsWindows64Bit);
					else bIsWindows64Bit = false;
				 
					// Free the DLL module.
					FreeLibrary (hKernelDLL); 
				} 

				FreeLibrary (hKernelDLL); 

 				// IsWow64Process ();
				if (bIsWindows64Bit) strcat (szOperatingSystem, "64-Bit ");
				else strcat (szOperatingSystem, "32-Bit ");
			} else { 
				// Windows 2000 and everything else.
//				sprintf (szOperatingSystem, "%s%s(Build %d)", szOperatingSystem, osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
			}
			break;

		case VER_PLATFORM_WIN32_WINDOWS:
			// Test for the product.
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
				strcpy (szOperatingSystem, "Microsoft Windows 95 ");
				if (osvi.szCSDVersion[1] == 'C') strcat (szOperatingSystem, "OSR 2.5 ");
				else if (osvi.szCSDVersion[1] == 'B') strcat (szOperatingSystem, "OSR 2 ");
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
				strcpy (szOperatingSystem, "Microsoft Windows 98 ");
				if (osvi.szCSDVersion[1] == 'A' ) strcat (szOperatingSystem, "SE ");
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
				strcpy (szOperatingSystem, "Microsoft Windows ME ");
			} 
			break;

		case VER_PLATFORM_WIN32s:
			strcpy (szOperatingSystem, "Microsoft Win32s ");
			break;

		//default:
		//	strcpy (szOperatingSystem, "Unknown Windows ");
		//	break;
	}

	return szOperatingSystem; 
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
