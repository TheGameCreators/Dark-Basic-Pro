

#ifndef _CPUINFO_H_
#define _CPUINFO_H_

#define STORE_CLASSICAL_NAME(x)		sprintf (ChipID.ProcessorName, x)
#define STORE_TLBCACHE_INFO(x,y)	x = (x < y) ? y : x
#define VENDOR_STRING_LENGTH		(12 + 1)
#define CHIPNAME_STRING_LENGTH		(48 + 1)
#define SERIALNUMBER_STRING_LENGTH	(29 + 1)
#define TLBCACHE_INFO_UNITS			(15)
#define CLASSICAL_CPU_FREQ_LOOP		10000000
#define RDTSC_INSTRUCTION			_asm _emit 0x0f _asm _emit 0x31
#define	CPUSPEED_I32TO64(x, y)		(((__int64) x << 32) + y)

#define CPUID_AWARE_COMPILER

#ifdef CPUID_AWARE_COMPILER
	#define CPUID_INSTRUCTION		cpuid
#else
	#define CPUID_INSTRUCTION		_asm _emit 0x0f _asm _emit 0xa2
#endif

#define MMX_FEATURE					0x00000001
#define MMX_PLUS_FEATURE			0x00000002
#define SSE_FEATURE					0x00000004
#define SSE2_FEATURE				0x00000008
#define AMD_3DNOW_FEATURE			0x00000010
#define AMD_3DNOW_PLUS_FEATURE		0x00000020
#define IA64_FEATURE				0x00000040
#define MP_CAPABLE					0x00000080
#define HYPERTHREAD_FEATURE			0x00000100
#define SERIALNUMBER_FEATURE		0x00000200
#define APIC_FEATURE				0x00000400
#define SSE_FP_FEATURE				0x00000800
#define SSE_MMX_FEATURE				0x00001000
#define CMOV_FEATURE				0x00002000
#define MTRR_FEATURE				0x00004000
#define L1CACHE_FEATURE				0x00008000
#define L2CACHE_FEATURE				0x00010000
#define L3CACHE_FEATURE				0x00020000
#define ACPI_FEATURE				0x00040000
#define THERMALMONITOR_FEATURE		0x00080000
#define TEMPSENSEDIODE_FEATURE		0x00100000
#define FREQUENCYID_FEATURE			0x00200000
#define VOLTAGEID_FREQUENCY			0x00400000
#include "stdafx.h"
//#include <windows.h>

#include <memory.h>
//#include <stdlib.h>
#include <stdio.h>

typedef	void (*DELAY_FUNC)(unsigned int uiMS);

class CPUSpeed
{
	public:
		CPUSpeed  ( );
		~CPUSpeed ( );
		
		int CPUSpeedInMHz;

		__int64 __cdecl GetCyclesDifference ( DELAY_FUNC, unsigned int );
		
	private:
		static void Delay         ( unsigned int );
		static void DelayOverhead ( unsigned int );
};

class CPUInfo
{
	public:
		CPUInfo  ( );
		~CPUInfo ( );

		char* GetVendorString                 ( void );
		char* GetVendorID                     ( void );
		char* GetTypeID                       ( void );
		char* GetFamilyID                     ( void );
		char* GetModelID                      ( void );
		char* GetSteppingCode                 ( void );
		char* GetExtendedProcessorName        ( void );
		char* GetProcessorSerialNumber        ( void );
		int   GetLogicalProcessorsPerPhysical ( void );
		int   GetProcessorClockFrequency      ( void );
		int   GetProcessorAPICID              ( void );
		int   GetProcessorCacheXSize          ( DWORD );
		bool  DoesCPUSupportFeature           ( DWORD );

		bool __cdecl DoesCPUSupportCPUID ( void );

	private:
		struct ID
		{
			int  Type;
			int  Family;
			int  Model;
			int  Revision;
			int  ExtendedFamily;
			int  ExtendedModel;
			char ProcessorName [ CHIPNAME_STRING_LENGTH     ];
			char Vendor        [ VENDOR_STRING_LENGTH       ];
			char SerialNumber  [ SERIALNUMBER_STRING_LENGTH ];
		};

		struct CPUPowerManagement
		{
			bool HasVoltageID;
			bool HasFrequencyID;
			bool HasTempSenseDiode;
		};

		struct CPUExtendedFeatures
		{
			bool				Has3DNow;
			bool				Has3DNowPlus;
			bool				SupportsMP;
			bool				HasMMXPlus;
			bool				HasSSEMMX;
			bool				SupportsHyperthreading;
			int					LogicalProcessorsPerPhysical;
			int					APIC_ID;
			CPUPowerManagement	PowerManagement;
		};	
		
		struct CPUFeatures
		{
			bool HasFPU;
			bool HasTSC;
			bool HasMMX;
			bool HasSSE;
			bool HasSSEFP;
			bool HasSSE2;
			bool HasIA64;
			bool HasAPIC;
			bool HasCMOV;
			bool HasMTRR;
			bool HasACPI;
			bool HasSerial;
			bool HasThermal;
			int CPUSpeed;
			int L1CacheSize;
			int L2CacheSize;
			int L3CacheSize;
			CPUExtendedFeatures ExtendedFeatures;
		};
    
		enum Manufacturer
		{
			AMD, Intel, NSC, UMC, Cyrix, NexGen, IDT, Rise, Transmeta, UnknownManufacturer
		};

		Manufacturer	ChipManufacturer;
		CPUFeatures		Features;
		CPUSpeed*		Speed;
		ID				ChipID;

		bool __cdecl RetrieveCPUFeatures              ( void );
		bool __cdecl RetrieveCPUIdentity              ( void );
		bool __cdecl RetrieveCPUCacheDetails          ( void );
		bool __cdecl RetrieveClassicalCPUCacheDetails ( void );
		bool __cdecl RetrieveCPUClockSpeed            ( void );
		bool __cdecl RetrieveClassicalCPUClockSpeed   ( void );
		bool __cdecl RetrieveCPUExtendedLevelSupport  ( int  );
		bool __cdecl RetrieveExtendedCPUFeatures      ( void );
		bool __cdecl RetrieveProcessorSerialNumber    ( void );
		bool __cdecl RetrieveCPUPowerManagement       ( void );
		bool __cdecl RetrieveClassicalCPUIdentity     ( void );
		bool __cdecl RetrieveExtendedCPUIdentity      ( void );
};

#endif