
#include "oscpuinfo.h"

CPUInfo::CPUInfo ( )
{
	if ( DoesCPUSupportCPUID ( ) )
	{
		RetrieveCPUIdentity ( );
		RetrieveCPUFeatures ( );

		if ( !RetrieveCPUClockSpeed ( ) )
			RetrieveClassicalCPUClockSpeed ( );

		if ( !RetrieveCPUCacheDetails ( ) )
			RetrieveClassicalCPUCacheDetails ( );

		if ( !RetrieveExtendedCPUIdentity ( ) )
			RetrieveClassicalCPUIdentity ( );
		
		RetrieveExtendedCPUFeatures   ( );
		RetrieveProcessorSerialNumber ( );
	}
}

CPUInfo::~CPUInfo ( )
{
}

char* CPUInfo::GetVendorString ( void )
{
	return ChipID.Vendor;
}

char* CPUInfo::GetVendorID ( void )
{
	switch ( ChipManufacturer )
	{
		case Intel:
			return "Intel Corporation";

		case AMD:
			return "Advanced Micro Devices";

		/*
		case NSC:
			return "National Semiconductor";
*/
  
		//case Cyrix:
		//	return "Cyrix Corp., VIA Inc.";

			/*
		case NexGen:
			return "NexGen Inc., Advanced Micro Devices";

		case IDT:
			return "IDT\\Centaur, Via Inc.";

		case UMC:
			return "United Microelectronics Corp.";

		case Rise:
			return "Rise";

		case Transmeta:
			return "Transmeta";
			*/

		default:
			return "Unknown Manufacturer";
	}
}

char* CPUInfo::GetTypeID ( void )
{
	char* szTypeID = new char [ 32 ];

	itoa ( ChipID.Type, szTypeID, 10 );

	return szTypeID;
}

char* CPUInfo::GetFamilyID ( void )
{
	char* szFamilyID = new char [ 32 ];

	itoa ( ChipID.Family, szFamilyID, 10 );

	return szFamilyID;
}

char* CPUInfo::GetModelID ( void )
{
	char* szModelID = new char [ 32 ];

	itoa ( ChipID.Model, szModelID, 10 );

	return szModelID;
}

char* CPUInfo::GetSteppingCode ( void )
{
	char* szSteppingCode = new char [ 32 ];

	itoa ( ChipID.Revision, szSteppingCode, 10 );

	return szSteppingCode;
}

char* CPUInfo::GetExtendedProcessorName ( void )
{
	return ChipID.ProcessorName;
}

char* CPUInfo::GetProcessorSerialNumber ( void )
{
	return ChipID.SerialNumber;
}

int CPUInfo::GetLogicalProcessorsPerPhysical ( void )
{
	return Features.ExtendedFeatures.LogicalProcessorsPerPhysical;
}

int CPUInfo::GetProcessorClockFrequency ( void )
{
	if ( Speed != NULL )
		return Speed->CPUSpeedInMHz;
	else 
		return -1;
}

int CPUInfo::GetProcessorAPICID ( void )
{
	return Features.ExtendedFeatures.APIC_ID;
}

int CPUInfo::GetProcessorCacheXSize ( DWORD dwCacheID )
{
	switch ( dwCacheID )
	{
		case L1CACHE_FEATURE:
			return Features.L1CacheSize;

		case L2CACHE_FEATURE:
			return Features.L2CacheSize;

		case L3CACHE_FEATURE:
			return Features.L3CacheSize;
	}

	return -1;
}

bool CPUInfo::DoesCPUSupportFeature ( DWORD dwFeature )
{
	bool bHasFeature = false;
	
	if ( ( ( dwFeature & MMX_FEATURE            ) != 0 ) && Features.HasMMX                                             ) bHasFeature = true;
	if ( ( ( dwFeature & MMX_PLUS_FEATURE       ) != 0 ) && Features.ExtendedFeatures.HasMMXPlus                        ) bHasFeature = true;
	if ( ( ( dwFeature & SSE_FEATURE            ) != 0 ) && Features.HasSSE                                             ) bHasFeature = true;
	if ( ( ( dwFeature & SSE_FP_FEATURE         ) != 0 ) && Features.HasSSEFP                                           ) bHasFeature = true;
	if ( ( ( dwFeature & SSE_MMX_FEATURE        ) != 0 ) && Features.ExtendedFeatures.HasSSEMMX                         ) bHasFeature = true;
	if ( ( ( dwFeature & SSE2_FEATURE           ) != 0 ) && Features.HasSSE2                                            ) bHasFeature = true;
	if ( ( ( dwFeature & AMD_3DNOW_FEATURE      ) != 0 ) && Features.ExtendedFeatures.Has3DNow                          ) bHasFeature = true;
	if ( ( ( dwFeature & AMD_3DNOW_PLUS_FEATURE ) != 0 ) && Features.ExtendedFeatures.Has3DNowPlus                      ) bHasFeature = true;
	if ( ( ( dwFeature & IA64_FEATURE           ) != 0 ) && Features.HasIA64                                            ) bHasFeature = true;
	if ( ( ( dwFeature & MP_CAPABLE             ) != 0 ) && Features.ExtendedFeatures.SupportsMP                        ) bHasFeature = true;
	if ( ( ( dwFeature & SERIALNUMBER_FEATURE   ) != 0 ) && Features.HasSerial                                          ) bHasFeature = true;
	if ( ( ( dwFeature & APIC_FEATURE           ) != 0 ) && Features.HasAPIC                                            ) bHasFeature = true;
	if ( ( ( dwFeature & CMOV_FEATURE           ) != 0 ) && Features.HasCMOV                                            ) bHasFeature = true;
	if ( ( ( dwFeature & MTRR_FEATURE           ) != 0 ) && Features.HasMTRR                                            ) bHasFeature = true;
	if ( ( ( dwFeature & L1CACHE_FEATURE        ) != 0 ) && ( Features.L1CacheSize != -1 )                              ) bHasFeature = true;
	if ( ( ( dwFeature & L2CACHE_FEATURE        ) != 0 ) && ( Features.L2CacheSize != -1 )                              ) bHasFeature = true;
	if ( ( ( dwFeature & L3CACHE_FEATURE        ) != 0 ) && ( Features.L3CacheSize != -1 )                              ) bHasFeature = true;
	if ( ( ( dwFeature & ACPI_FEATURE           ) != 0 ) && Features.HasACPI                                            ) bHasFeature = true;
	if ( ( ( dwFeature & THERMALMONITOR_FEATURE ) != 0 ) && Features.HasThermal                                         ) bHasFeature = true;
	if ( ( ( dwFeature & TEMPSENSEDIODE_FEATURE ) != 0 ) && Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode ) bHasFeature = true;
	if ( ( ( dwFeature & FREQUENCYID_FEATURE    ) != 0 ) && Features.ExtendedFeatures.PowerManagement.HasFrequencyID    ) bHasFeature = true;
	if ( ( ( dwFeature & VOLTAGEID_FREQUENCY    ) != 0 ) && Features.ExtendedFeatures.PowerManagement.HasVoltageID      ) bHasFeature = true;

	return bHasFeature;
}

bool __cdecl CPUInfo::DoesCPUSupportCPUID ( void )
{
	int CPUIDPresent = 0;

	// Use SEH to determine CPUID presence
    __try
	{
        _asm
		{

 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
            mov eax, 0
			CPUID_INSTRUCTION

			pop edx
			pop ecx
			pop ebx
			pop eax
        }
    }
	__except ( 1 )
	{
        CPUIDPresent = false;
		return false;
    }

	return ( CPUIDPresent == 0 ) ? true : false;
}

bool __cdecl CPUInfo::RetrieveCPUFeatures ( void )
{
	int CPUFeatures = 0;
	int CPUAdvanced = 0;

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
			; eax = 1 --> eax: CPU ID - bits 31..16 - unused, bits 15..12 - type, bits 11..8 - family, bits 7..4 - model, bits 3..0 - mask revision
			;			  ebx: 31..24 - default APIC ID, 23..16 - logical processsor ID, 15..8 - CFLUSH chunk size , 7..0 - brand ID
			;			  edx: CPU feature flags
			mov eax,1
			CPUID_INSTRUCTION
			mov CPUFeatures, edx
			mov CPUAdvanced, ebx

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	Features.HasFPU     = ( ( CPUFeatures & 0x00000001 ) != 0 );		// FPU Present             --> Bit 0
	Features.HasTSC     = ( ( CPUFeatures & 0x00000010 ) != 0 );		// TSC Present             --> Bit 4
	Features.HasAPIC    = ( ( CPUFeatures & 0x00000200 ) != 0 );		// APIC Present            --> Bit 9
	Features.HasMTRR    = ( ( CPUFeatures & 0x00001000 ) != 0 );		// MTRR Present            --> Bit 12
	Features.HasCMOV    = ( ( CPUFeatures & 0x00008000 ) != 0 );		// CMOV Present            --> Bit 15
	Features.HasSerial  = ( ( CPUFeatures & 0x00040000 ) != 0 );		// Serial Present          --> Bit 18
	Features.HasACPI    = ( ( CPUFeatures & 0x00400000 ) != 0 );		// ACPI Capable            --> Bit 22
    Features.HasMMX     = ( ( CPUFeatures & 0x00800000 ) != 0 );		// MMX Present             --> Bit 23
	Features.HasSSE     = ( ( CPUFeatures & 0x02000000 ) != 0 );		// SSE Present             --> Bit 25
	Features.HasSSE2    = ( ( CPUFeatures & 0x04000000 ) != 0 );		// SSE2 Present            --> Bit 26
	Features.HasThermal = ( ( CPUFeatures & 0x20000000 ) != 0 );		// Thermal Monitor Present --> Bit 29
	Features.HasIA64    = ( ( CPUFeatures & 0x40000000 ) != 0 );		// IA64 Present            --> Bit 30

	if ( Features.HasSSE )
	{
		__try
		{
			// perform orps xmm0, xmm0
			_asm
			{
				_emit 0x0f
	    		_emit 0x56
	    		_emit 0xc0	
			}

			// SSE FP capable processor
			Features.HasSSEFP = true;
	    }
	    __except ( 1 )
		{
	    	// bad instruction - processor or OS cannot handle SSE FP
			Features.HasSSEFP = false;
		}
	}
	else
	{
		// set the advanced SSE capabilities to not available
		Features.HasSSEFP = false;
	}

	// retrieve Intel specific extended features
	if ( ChipManufacturer == Intel )
	{
		// Intel specific: Hyperthreading --> Bit 28
		Features.ExtendedFeatures.SupportsHyperthreading       = ( ( CPUFeatures & 0x10000000 ) != 0 );
		Features.ExtendedFeatures.LogicalProcessorsPerPhysical = ( Features.ExtendedFeatures.SupportsHyperthreading ) ? ( ( CPUAdvanced & 0x00FF0000 ) >> 16 ) : 1;
		
		if ( ( Features.ExtendedFeatures.SupportsHyperthreading ) && ( Features.HasAPIC ) )
		{
			// retrieve APIC information if there is one present.
			Features.ExtendedFeatures.APIC_ID = ( ( CPUAdvanced & 0xFF000000 ) >> 24 );
		}
	}

	return true;
}

bool __cdecl CPUInfo::RetrieveCPUIdentity ( void )
{
	int CPUVendor [ 3 ];
	int CPUSignature;

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>>
			; eax = 0 --> eax: maximum value of CPUID instruction.
			;			  ebx: part 1 of 3; CPU signature.
			;			  edx: part 2 of 3; CPU signature.
			;			  ecx: part 3 of 3; CPU signature.
			mov eax, 0
			CPUID_INSTRUCTION
			mov CPUVendor [ 0 * TYPE int ], ebx
			mov CPUVendor [ 1 * TYPE int ], edx
			mov CPUVendor [ 2 * TYPE int ], ecx

			; <<CPUID>> 
			; eax = 1 --> eax: CPU ID - bits 31..16 - unused, bits 15..12 - type, bits 11..8 - family, bits 7..4 - model, bits 3..0 - mask revision
			;			  ebx: 31..24 - default APIC ID, 23..16 - logical processsor ID, 15..8 - CFLUSH chunk size , 7..0 - brand ID
			;			  edx: CPU feature flags
			mov eax,1
			CPUID_INSTRUCTION
			mov CPUSignature, eax

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	// process the returned information.
	memcpy ( ChipID.Vendor,             & ( CPUVendor [ 0 ] ), sizeof ( int ) );
	memcpy ( & ( ChipID.Vendor [ 4 ] ), & ( CPUVendor [ 1 ] ), sizeof ( int ) );
	memcpy ( & ( ChipID.Vendor [ 8 ] ), & ( CPUVendor [ 2 ] ), sizeof ( int ) );
	
	ChipID.Vendor [ 12 ] = '\0';

	// attempt to retrieve the manufacturer from the vendor string.
	if      ( strcmp ( ChipID.Vendor, "GenuineIntel" ) == 0 )	ChipManufacturer = Intel;				// Intel Corp.
	//else if ( strcmp ( ChipID.Vendor, "UMC UMC UMC " ) == 0 )	ChipManufacturer = UMC;					// United Microelectronics Corp.
	else if ( strcmp ( ChipID.Vendor, "AuthenticAMD" ) == 0 )	ChipManufacturer = AMD;					// Advanced Micro Devices
	//else if ( strcmp ( ChipID.Vendor, "AMD ISBETTER" ) == 0 )	ChipManufacturer = AMD;					// Advanced Micro Devices (1994)
	//else if ( strcmp ( ChipID.Vendor, "Cyrix" ) == 0 )	ChipManufacturer = Cyrix;				// Cyrix Corp., VIA Inc.
	//else if ( strcmp ( ChipID.Vendor, "NexGenDriven" ) == 0 )	ChipManufacturer = NexGen;				// NexGen Inc. (now AMD)
	//else if ( strcmp ( ChipID.Vendor, "CentaurHauls" ) == 0 )	ChipManufacturer = IDT;					// IDT/Centaur (now VIA)
	//else if ( strcmp ( ChipID.Vendor, "RiseRiseRise" ) == 0 )	ChipManufacturer = Rise;				// Rise
	//else if ( strcmp ( ChipID.Vendor, "GenuineTMx86" ) == 0 )	ChipManufacturer = Transmeta;			// Transmeta
	//else if ( strcmp ( ChipID.Vendor, "TransmetaCPU" ) == 0 )	ChipManufacturer = Transmeta;			// Transmeta
	//else if ( strcmp ( ChipID.Vendor, "Geode By NSC" ) == 0 )	ChipManufacturer = NSC;					// National Semiconductor
	else													    ChipManufacturer = UnknownManufacturer;	// Unknown manufacturer
	
	// retrieve the family of CPU present.
	ChipID.ExtendedFamily =	( ( CPUSignature & 0x0FF00000 ) >> 20 );	// Bits 27..20 Used
	ChipID.ExtendedModel  =	( ( CPUSignature & 0x000F0000 ) >> 16 );	// Bits 19..16 Used
	ChipID.Type           =	( ( CPUSignature & 0x0000F000 ) >> 12 );	// Bits 15..12 Used
	ChipID.Family         =	( ( CPUSignature & 0x00000F00 ) >>  8 );	// Bits 11..8 Used
	ChipID.Model          =	( ( CPUSignature & 0x000000F0 ) >>  4 );	// Bits 7..4 Used
	ChipID.Revision       =	( ( CPUSignature & 0x0000000F ) >>  0 );	// Bits 3..0 Used

	return true;
}

bool __cdecl CPUInfo::RetrieveCPUCacheDetails ( void )
{
	int L1Cache [ 4 ] = { 0, 0, 0, 0 };
	int L2Cache [ 4 ] = { 0, 0, 0, 0 };

	if ( RetrieveCPUExtendedLevelSupport ( 0x80000005 ) )
	{
		// L1 cache
		__try
		{
			_asm
			{
				; we must push/pop the registers <<CPUID>> writes to, as the
				; optimiser doesn't know about <<CPUID>>, and so doesn't expect
				; these registers to change.
				push eax
				push ebx
				push ecx
				push edx

				; <<CPUID>>
				; eax = 0x80000005 --> eax: L1 cache information - Part 1 of 4.
				;					   ebx: L1 cache information - Part 2 of 4.
				;					   edx: L1 cache information - Part 3 of 4.
				;			 		   ecx: L1 cache information - Part 4 of 4.
				mov eax, 0x80000005
				CPUID_INSTRUCTION
				mov L1Cache [ 0 * TYPE int ], eax
				mov L1Cache [ 1 * TYPE int ], ebx
				mov L1Cache [ 2 * TYPE int ], ecx
				mov L1Cache [ 3 * TYPE int ], edx

				pop edx
				pop ecx
				pop ebx
				pop eax
			}
		}
		__except ( 1 )
		{
			return false;
		}

		// save the L1 data cache size ( in KB ) from ecx: bits 31..24 as well as data cache size from edx: bits 31..24.
		Features.L1CacheSize  = ( ( L1Cache [ 2 ] & 0xFF000000 ) >> 24 );
		Features.L1CacheSize += ( ( L1Cache [ 3 ] & 0xFF000000 ) >> 24 );
	}
	else
	{
		Features.L1CacheSize = -1;
	}

	if ( RetrieveCPUExtendedLevelSupport ( 0x80000006 ) )
	{
		__try
		{
			_asm
			{
 				; we must push/pop the registers <<CPUID>> writes to, as the
				; optimiser doesn't know about <<CPUID>>, and so doesn't expect
				; these registers to change.
				push eax
				push ebx
				push ecx
				push edx

				; <<CPUID>>
				; eax = 0x80000006 --> eax: L2 cache information - Part 1 of 4.
				;					   ebx: L2 cache information - Part 2 of 4.
				;					   edx: L2 cache information - Part 3 of 4.
				;			 		   ecx: L2 cache information - Part 4 of 4.
				mov eax, 0x80000006
				CPUID_INSTRUCTION
				mov L2Cache [ 0 * TYPE int ], eax
				mov L2Cache [ 1 * TYPE int ], ebx
				mov L2Cache [ 2 * TYPE int ], ecx
				mov L2Cache [ 3 * TYPE int ], edx

				pop edx
				pop ecx
				pop ebx
				pop eax
			}
		}
		__except ( 1 )
		{
			return false;
		}

		// save the L2 unified cache size ( in KB ) from ecx: bits 31..16.
		Features.L2CacheSize = ( ( L2Cache [ 2 ] & 0xFFFF0000 ) >> 16 );
	}
	else 
	{
		Features.L2CacheSize = -1;
	}
	
	// define L3 as being not present as we cannot test for it.
	Features.L3CacheSize = -1;

	// return failure if we cannot detect either cache with this method
	return ( ( Features.L1CacheSize == -1 ) && ( Features.L2CacheSize == -1 ) ) ? false : true;
}

bool __cdecl CPUInfo::RetrieveClassicalCPUCacheDetails ( void )
{
	int TLBCode = -1, TLBData = -1, L1Code = -1, L1Data = -1, L1Trace = -1, L2Unified = -1, L3Unified = -1;
	int TLBCacheData [ 4 ] = { 0, 0, 0, 0 };
	int TLBPassCounter = 0;
	int TLBCacheUnit = 0;

	do
	{
		__try
		{
			_asm
			{
 				; we must push/pop the registers <<CPUID>> writes to, as the
				; optimiser doesn't know about <<CPUID>>, and so doesn't expect
				; these registers to change.
				push eax
				push ebx
				push ecx
				push edx

				; <<CPUID>>
				; eax = 2 --> eax: TLB and cache information - Part 1 of 4.
				;			  ebx: TLB and cache information - Part 2 of 4.
				;			  ecx: TLB and cache information - Part 3 of 4.
				;			  edx: TLB and cache information - Part 4 of 4.
				mov eax, 2
				CPUID_INSTRUCTION
				mov TLBCacheData [ 0 * TYPE int ], eax
				mov TLBCacheData [ 1 * TYPE int ], ebx
				mov TLBCacheData [ 2 * TYPE int ], ecx
				mov TLBCacheData [ 3 * TYPE int ], edx

				pop edx
				pop ecx
				pop ebx
				pop eax
			}
		}
		__except ( 1 )
		{
			return false;
		}

		int bob = ( ( TLBCacheData [ 0 ] & 0x00FF0000 ) >> 16 );

		// process the returned TLB and cache information
		for ( int nCounter = 0; nCounter < TLBCACHE_INFO_UNITS; nCounter++ )
		{
			switch ( nCounter )
			{
				// eax: bits 8..15 : bits 16..23 : bits 24..31
				case 0: TLBCacheUnit = ( ( TLBCacheData [ 0 ] & 0x0000FF00 ) >>  8 ); break;
				case 1: TLBCacheUnit = ( ( TLBCacheData [ 0 ] & 0x00FF0000 ) >> 16 ); break;
				case 2: TLBCacheUnit = ( ( TLBCacheData [ 0 ] & 0xFF000000 ) >> 24 ); break;

				// ebx: bits 0..7 : bits 8..15 : bits 16..23 : bits 24..31
				case 3: TLBCacheUnit = ( ( TLBCacheData [ 1 ] & 0x000000FF ) >>  0 ); break;
				case 4: TLBCacheUnit = ( ( TLBCacheData [ 1 ] & 0x0000FF00 ) >>  8 ); break;
				case 5: TLBCacheUnit = ( ( TLBCacheData [ 1 ] & 0x00FF0000 ) >> 16 ); break;
				case 6: TLBCacheUnit = ( ( TLBCacheData [ 1 ] & 0xFF000000 ) >> 24 ); break;

				// ecx: bits 0..7 : bits 8..15 : bits 16..23 : bits 24..31
				case  7: TLBCacheUnit = ( ( TLBCacheData [ 2 ] & 0x000000FF ) >>  0 ); break;
				case  8: TLBCacheUnit = ( ( TLBCacheData [ 2 ] & 0x0000FF00 ) >>  8 ); break;
				case  9: TLBCacheUnit = ( ( TLBCacheData [ 2 ] & 0x00FF0000 ) >> 16 ); break;
				case 10: TLBCacheUnit = ( ( TLBCacheData [ 2 ] & 0xFF000000 ) >> 24 ); break;

				// edx: bits 0..7 : bits 8..15 : bits 16..23 : bits 24..31
				case 11: TLBCacheUnit = ( ( TLBCacheData [ 3 ] & 0x000000FF ) >>  0 ); break;
				case 12: TLBCacheUnit = ( ( TLBCacheData [ 3 ] & 0x0000FF00 ) >>  8 ); break;
				case 13: TLBCacheUnit = ( ( TLBCacheData [ 3 ] & 0x00FF0000 ) >> 16 ); break;
				case 14: TLBCacheUnit = ( ( TLBCacheData [ 3 ] & 0xFF000000 ) >> 24 ); break;

				default: return false;
			}

			// mow process the resulting unit to see what it means....
			switch ( TLBCacheUnit )
			{
				case 0x00: break;
				case 0x01: STORE_TLBCACHE_INFO ( TLBCode,   4      ); break;
				case 0x02: STORE_TLBCACHE_INFO ( TLBCode,   4096   ); break;
				case 0x03: STORE_TLBCACHE_INFO ( TLBData,   4      ); break;
				case 0x04: STORE_TLBCACHE_INFO ( TLBData,   4096   ); break;
				case 0x06: STORE_TLBCACHE_INFO ( L1Code,    8      ); break;
				case 0x08: STORE_TLBCACHE_INFO ( L1Code,    16     ); break;
				case 0x0a: STORE_TLBCACHE_INFO ( L1Data,    8      ); break;
				case 0x0c: STORE_TLBCACHE_INFO ( L1Data,    16     ); break;
				case 0x10: STORE_TLBCACHE_INFO ( L1Data,    16     ); break;
				case 0x15: STORE_TLBCACHE_INFO ( L1Code,    16     ); break;
				case 0x1a: STORE_TLBCACHE_INFO ( L2Unified, 96     ); break;
				case 0x22: STORE_TLBCACHE_INFO ( L3Unified, 512    ); break;
				case 0x23: STORE_TLBCACHE_INFO ( L3Unified, 1024   ); break;
				case 0x25: STORE_TLBCACHE_INFO ( L3Unified, 2048   ); break;
				case 0x29: STORE_TLBCACHE_INFO ( L3Unified, 4096   ); break;
				case 0x39: STORE_TLBCACHE_INFO ( L2Unified, 128    ); break;
				case 0x3c: STORE_TLBCACHE_INFO ( L2Unified, 256    ); break;
				case 0x40: STORE_TLBCACHE_INFO ( L2Unified, 0      ); break;
				case 0x41: STORE_TLBCACHE_INFO ( L2Unified, 128    ); break;
				case 0x42: STORE_TLBCACHE_INFO ( L2Unified, 256    ); break;
				case 0x43: STORE_TLBCACHE_INFO ( L2Unified, 512    ); break;
				case 0x44: STORE_TLBCACHE_INFO ( L2Unified, 1024   ); break;
				case 0x45: STORE_TLBCACHE_INFO ( L2Unified, 2048   ); break;
				case 0x50: STORE_TLBCACHE_INFO ( TLBCode,   4096   ); break;
				case 0x51: STORE_TLBCACHE_INFO ( TLBCode,   4096   ); break;
				case 0x52: STORE_TLBCACHE_INFO ( TLBCode,   4096   ); break;
				case 0x5b: STORE_TLBCACHE_INFO ( TLBData,   4096   ); break;
				case 0x5c: STORE_TLBCACHE_INFO ( TLBData,   4096   ); break;
				case 0x5d: STORE_TLBCACHE_INFO ( TLBData,   4096   ); break;
				case 0x66: STORE_TLBCACHE_INFO ( L1Data,    8      ); break;
				case 0x67: STORE_TLBCACHE_INFO ( L1Data,    16     ); break;
				case 0x68: STORE_TLBCACHE_INFO ( L1Data,    32     ); break;
				case 0x70: STORE_TLBCACHE_INFO ( L1Trace,   12     ); break;
				case 0x71: STORE_TLBCACHE_INFO ( L1Trace,   16     ); break;
				case 0x72: STORE_TLBCACHE_INFO ( L1Trace,   32     ); break;
				case 0x77: STORE_TLBCACHE_INFO ( L1Code,    16     ); break;
				case 0x79: STORE_TLBCACHE_INFO ( L2Unified, 128    ); break;
				case 0x7a: STORE_TLBCACHE_INFO ( L2Unified, 256    ); break;
				case 0x7b: STORE_TLBCACHE_INFO ( L2Unified, 512    ); break;
				case 0x7c: STORE_TLBCACHE_INFO ( L2Unified, 1024   ); break;
				case 0x7e: STORE_TLBCACHE_INFO ( L2Unified, 256    ); break;
				case 0x81: STORE_TLBCACHE_INFO ( L2Unified, 128    ); break;
				case 0x82: STORE_TLBCACHE_INFO ( L2Unified, 256    ); break;
				case 0x83: STORE_TLBCACHE_INFO ( L2Unified, 512    ); break;
				case 0x84: STORE_TLBCACHE_INFO ( L2Unified, 1024   ); break;
				case 0x85: STORE_TLBCACHE_INFO ( L2Unified, 2048   ); break;
				case 0x88: STORE_TLBCACHE_INFO ( L3Unified, 2048   ); break;
				case 0x89: STORE_TLBCACHE_INFO ( L3Unified, 4096   ); break;
				case 0x8a: STORE_TLBCACHE_INFO ( L3Unified, 8192   ); break;
				case 0x8d: STORE_TLBCACHE_INFO ( L3Unified, 3096   ); break;
				case 0x90: STORE_TLBCACHE_INFO ( TLBCode,   262144 ); break;
				case 0x96: STORE_TLBCACHE_INFO ( TLBCode,   262144 ); break;
				case 0x9b: STORE_TLBCACHE_INFO ( TLBCode,   262144 ); break;
				
				default: return false;
			}
		}

		// increment the TLB pass counter
		TLBPassCounter ++;
	
	} while ( ( TLBCacheData [ 0 ] & 0x000000FF ) > TLBPassCounter );

	// we now have the maximum TLB, L1, L2, and L3 sizes
	if      ( ( L1Code == -1 ) && ( L1Data == -1 ) && ( L1Trace == -1 ) )	Features.L1CacheSize = -1;
	else if ( ( L1Code == -1 ) && ( L1Data == -1 ) && ( L1Trace != -1 ) )	Features.L1CacheSize = L1Trace;
	else if ( ( L1Code != -1 ) && ( L1Data == -1 ) )						Features.L1CacheSize = L1Code;
	else if ( ( L1Code == -1 ) && ( L1Data != -1 ) )						Features.L1CacheSize = L1Data;
	else if ( ( L1Code != -1 ) && ( L1Data != -1 ) )						Features.L1CacheSize = L1Code + L1Data;
	else																	Features.L1CacheSize = -1;
	
	if ( L2Unified == -1 )
		Features.L2CacheSize = -1;
	else
		Features.L2CacheSize = L2Unified;
	
	if ( L3Unified == -1 )
		Features.L3CacheSize = -1;
	else
		Features.L3CacheSize = L3Unified;

	return true;
}

bool __cdecl CPUInfo::RetrieveCPUClockSpeed ( void )
{
	// first of all we check to see if the RDTSC (0x0F, 0x31) instruction is supported
	if ( !Features.HasTSC )
		return false;

	// get the clock speed
	Speed = new CPUSpeed ( );

	if ( Speed == NULL )
		return false;

	return true;
}

bool __cdecl CPUInfo::RetrieveClassicalCPUClockSpeed ( void )
{
	LARGE_INTEGER	liStart,
					liEnd,
					liCountsPerSecond;
	double			dFrequency,
					dDifference;

	// attempt to get a starting tick count
	QueryPerformanceCounter ( &liStart );

	__try
	{
		_asm
		{
			mov eax, 0x80000000
			mov ebx, CLASSICAL_CPU_FREQ_LOOP
			Timer_Loop: 
			bsf ecx,eax
			dec ebx
			jnz Timer_Loop
		}	
	}
	__except ( 1 )
	{
		return false;
	}

	// attempt to get a starting tick count
	QueryPerformanceCounter ( &liEnd );

	// get the difference in seconds
	QueryPerformanceFrequency ( &liCountsPerSecond );
	
	dDifference = ( ( ( double ) liEnd.QuadPart - ( double ) liStart.QuadPart ) / ( double ) liCountsPerSecond.QuadPart );

	// calculate the clock speed
	if ( ChipID.Family == 3 )
	{
		// 80386 processors - 115 cycles
		dFrequency = ( ( ( CLASSICAL_CPU_FREQ_LOOP * 115 ) / dDifference ) / 1048576 );
	}
	else if ( ChipID.Family == 4 )
	{
		// 80486 processors - 47 cycles
		dFrequency = ( ( ( CLASSICAL_CPU_FREQ_LOOP * 47 ) / dDifference ) / 1048576 );
	}
	else if ( ChipID.Family == 5 )
	{
		// pentium processors - 43 cycles
		dFrequency = ( ( ( CLASSICAL_CPU_FREQ_LOOP * 43 ) / dDifference ) / 1048576 );
	}
	
	// save the clock speed
	Features.CPUSpeed = ( int ) dFrequency;

	return true;
}

bool __cdecl CPUInfo::RetrieveCPUExtendedLevelSupport ( int CPULevelToCheck )
{
	int MaxCPUExtendedLevel = 0;

	// the extended CPUID is supported by various vendors starting with the following CPU models: 
	//
	//		Manufacturer & Chip Name			|		Family		 Model		Revision
	//
	//		AMD K6, K6-2						|		   5		   6			x		
	//		Cyrix GXm, Cyrix III "Joshua"		|		   5		   4			x
	//		IDT C6-2							|		   5		   8			x
	//		VIA Cyrix III						|		   6		   5			x
	//		Transmeta Crusoe					|		   5		   x			x
	//		Intel Pentium 4						|		   f		   x			x
	//

	if ( ChipManufacturer == AMD )
	{
		if ( ChipID.Family < 5 )
			return false;

		if ( ( ChipID.Family == 5 ) && ( ChipID.Model < 6 ) )
			return false;
	}
	else if ( ChipManufacturer == Cyrix )
	{
		if ( ChipID.Family < 5 )
			return false;

		if ( ( ChipID.Family == 5 ) && ( ChipID.Model < 4 ) )
			return false;

		if ( ( ChipID.Family == 6 ) && ( ChipID.Model < 5 ) )
			return false;
	}
	else if ( ChipManufacturer == IDT )
	{
		if ( ChipID.Family < 5 )
			return false;

		if ( ( ChipID.Family == 5 ) && ( ChipID.Model < 8 ) )
			return false;
	}
	else if ( ChipManufacturer == Transmeta )
	{
		if ( ChipID.Family < 5 )
			return false;
	}
	else if ( ChipManufacturer == Intel )
	{
		if ( ChipID.Family < 0xf )
			return false;
	}
		
	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
			; eax = 0x80000000 --> eax: maximum supported extended level
			mov eax,0x80000000
			CPUID_INSTRUCTION
			mov MaxCPUExtendedLevel, eax

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	int nLevelWanted = ( CPULevelToCheck     & 0x7FFFFFFF );
	int nLevelReturn = ( MaxCPUExtendedLevel & 0x7FFFFFFF );

	if ( nLevelWanted > nLevelReturn )
		return false;

	return true;
}

bool __cdecl CPUInfo::RetrieveExtendedCPUFeatures ( void )
{
	int CPUExtendedFeatures = 0;

	// check that we are not using an Intel processor as it does not support this
	if ( ChipManufacturer == Intel )
		return false;

	// check to see if what we are about to do is supported
	if ( !RetrieveCPUExtendedLevelSupport ( 0x80000001 ) )
		return false;

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
			; eax = 0x80000001 --> eax: CPU ID - bits 31..16 - unused, bits 15..12 - type, bits 11..8 - family, bits 7..4 - model, bits 3..0 - mask revision
			;					   ebx: 31..24 - default APIC ID, 23..16 - logical processsor ID, 15..8 - CFLUSH chunk size , 7..0 - brand ID
			;					   edx: CPU feature flags
			mov eax,0x80000001
			CPUID_INSTRUCTION
			mov CPUExtendedFeatures, edx

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	// retrieve the extended features of CPU present
	Features.ExtendedFeatures.Has3DNow     = ( ( CPUExtendedFeatures & 0x80000000 ) != 0 );	// 3DNow Present   --> Bit 31
	Features.ExtendedFeatures.Has3DNowPlus = ( ( CPUExtendedFeatures & 0x40000000 ) != 0 );	// 3DNow+ Present  --> Bit 30
	Features.ExtendedFeatures.HasSSEMMX    = ( ( CPUExtendedFeatures & 0x00400000 ) != 0 );	// SSE MMX Present --> Bit 22
	Features.ExtendedFeatures.SupportsMP   = ( ( CPUExtendedFeatures & 0x00080000 ) != 0 );	// MP Capable      --> Bit 19
	
	// retrieve AMD specific extended features
	if ( ChipManufacturer == AMD )
	{
		// AMD specific: MMX-SSE --> Bit 22
		Features.ExtendedFeatures.HasMMXPlus = ( ( CPUExtendedFeatures & 0x00400000 ) != 0 );
	}

	// retrieve Cyrix specific extended features
	if ( ChipManufacturer == Cyrix )
	{
		// Cyrix specific: Extended MMX --> Bit 24
		Features.ExtendedFeatures.HasMMXPlus = ( ( CPUExtendedFeatures & 0x01000000 ) != 0 );
	}

	return true;
}

bool __cdecl CPUInfo::RetrieveProcessorSerialNumber ( void )
{
	int SerialNumber [ 3 ];

	if ( !Features.HasSerial )
		return false;

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>>
			; eax = 3 --> ebx: top 32 bits are the processor signature bits --> NB: Transmeta only ?!?
			;			  ecx: middle 32 bits are the processor signature bits
			;			  edx: bottom 32 bits are the processor signature bits
			mov eax, 3
			CPUID_INSTRUCTION
			mov SerialNumber [ 0 * TYPE int ], ebx
			mov SerialNumber [ 1 * TYPE int ], ecx
			mov SerialNumber [ 2 * TYPE int ], edx

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	sprintf (
				ChipID.SerialNumber, "%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x",
				( ( SerialNumber [ 0 ] & 0xff000000) >> 24 ),
				( ( SerialNumber [ 0 ] & 0x00ff0000) >> 16 ),
				( ( SerialNumber [ 0 ] & 0x0000ff00) >>  8 ),
				( ( SerialNumber [ 0 ] & 0x000000ff) >>  0 ),
				( ( SerialNumber [ 1 ] & 0xff000000) >> 24 ),
				( ( SerialNumber [ 1 ] & 0x00ff0000) >> 16 ),
				( ( SerialNumber [ 1 ] & 0x0000ff00) >>  8 ),
				( ( SerialNumber [ 1 ] & 0x000000ff) >>  0 ),
				( ( SerialNumber [ 2 ] & 0xff000000) >> 24 ),
				( ( SerialNumber [ 2 ] & 0x00ff0000) >> 16 ),
				( ( SerialNumber [ 2 ] & 0x0000ff00) >>  8 ),
				( ( SerialNumber [ 2 ] & 0x000000ff) >>  0 )
			);

	return true;
}

bool __cdecl CPUInfo::RetrieveCPUPowerManagement ( void )
{	
	int CPUPowerManagement = 0;

	if ( !RetrieveCPUExtendedLevelSupport ( 0x80000007 ) )
	{
		Features.ExtendedFeatures.PowerManagement.HasFrequencyID    = false;
		Features.ExtendedFeatures.PowerManagement.HasVoltageID      = false;
		Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode = false;
		return false;
	}

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
			; eax = 0x80000007 --> edx: get processor power management
			mov eax,0x80000007
			CPUID_INSTRUCTION
			mov CPUPowerManagement, edx
			
			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	Features.ExtendedFeatures.PowerManagement.HasTempSenseDiode = ( ( CPUPowerManagement & 0x00000001 ) != 0 );
	Features.ExtendedFeatures.PowerManagement.HasFrequencyID    = ( ( CPUPowerManagement & 0x00000002 ) != 0 );
	Features.ExtendedFeatures.PowerManagement.HasVoltageID      = ( ( CPUPowerManagement & 0x00000004 ) != 0 );
	
	return true;
}

bool __cdecl CPUInfo::RetrieveExtendedCPUIdentity ( void )
{
	int ProcessorNameStartPos = 0;
	int CPUExtendedIdentity [ 12 ];

	if ( !RetrieveCPUExtendedLevelSupport ( 0x80000002 ) ) return false;
	if ( !RetrieveCPUExtendedLevelSupport ( 0x80000003 ) ) return false;
	if ( !RetrieveCPUExtendedLevelSupport ( 0x80000004 ) ) return false;

	__try
	{
		_asm
		{
 			; we must push/pop the registers <<CPUID>> writes to, as the
			; optimiser doesn't know about <<CPUID>>, and so doesn't expect
			; these registers to change.
			push eax
			push ebx
			push ecx
			push edx

			; <<CPUID>> 
			; eax = 0x80000002 --> eax, ebx, ecx, edx: get processor name string (part 1)
			mov eax,0x80000002
			CPUID_INSTRUCTION
			mov CPUExtendedIdentity[0 * TYPE int], eax
			mov CPUExtendedIdentity[1 * TYPE int], ebx
			mov CPUExtendedIdentity[2 * TYPE int], ecx
			mov CPUExtendedIdentity[3 * TYPE int], edx

			; <<CPUID>> 
			; eax = 0x80000003 --> eax, ebx, ecx, edx: get processor name string (part 2)
			mov eax,0x80000003
			CPUID_INSTRUCTION
			mov CPUExtendedIdentity[4 * TYPE int], eax
			mov CPUExtendedIdentity[5 * TYPE int], ebx
			mov CPUExtendedIdentity[6 * TYPE int], ecx
			mov CPUExtendedIdentity[7 * TYPE int], edx

			; <<CPUID>> 
			; eax = 0x80000004 --> eax, ebx, ecx, edx: get processor name string (part 3)
			mov eax,0x80000004
			CPUID_INSTRUCTION
			mov CPUExtendedIdentity[8 * TYPE int], eax
			mov CPUExtendedIdentity[9 * TYPE int], ebx
			mov CPUExtendedIdentity[10 * TYPE int], ecx
			mov CPUExtendedIdentity[11 * TYPE int], edx

			pop edx
			pop ecx
			pop ebx
			pop eax
		}
	}
	__except ( 1 )
	{
		return false;
	}

	memcpy (ChipID.ProcessorName, &(CPUExtendedIdentity[0]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[4]), &(CPUExtendedIdentity[1]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[8]), &(CPUExtendedIdentity[2]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[12]), &(CPUExtendedIdentity[3]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[16]), &(CPUExtendedIdentity[4]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[20]), &(CPUExtendedIdentity[5]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[24]), &(CPUExtendedIdentity[6]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[28]), &(CPUExtendedIdentity[7]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[32]), &(CPUExtendedIdentity[8]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[36]), &(CPUExtendedIdentity[9]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[40]), &(CPUExtendedIdentity[10]), sizeof (int));
	memcpy (&(ChipID.ProcessorName[44]), &(CPUExtendedIdentity[11]), sizeof (int));
	ChipID.ProcessorName[48] = '\0';

	// Because some manufacturers (<cough>Intel</cough>) have leading white space - we have to post-process the name.
	if (ChipManufacturer == Intel) {
		for (int nCounter = 0; nCounter < CHIPNAME_STRING_LENGTH; nCounter ++) {
			// There will either be NULL (\0) or spaces ( ) as the leading characters.
			if ((ChipID.ProcessorName[nCounter] != '\0') && (ChipID.ProcessorName[nCounter] != ' ')) {
				// We have found the starting position of the name.
				ProcessorNameStartPos = nCounter;
				
				// Terminate the loop.
				break;
			}
		}

		// Check to see if there is any white space at the start.
		if (ProcessorNameStartPos == 0) return true;

		// Now move the name forward so that there is no white space.
		memmove (ChipID.ProcessorName, &(ChipID.ProcessorName[ProcessorNameStartPos]), (CHIPNAME_STRING_LENGTH - ProcessorNameStartPos));
	}

	return true;
}

bool _cdecl CPUInfo::RetrieveClassicalCPUIdentity ()
{
	// Start by decided which manufacturer we are using....
	switch (ChipManufacturer) {
		case Intel:
			// Check the family / model / revision to determine the CPU ID.
			switch (ChipID.Family) {
				/*
				case 3:
					sprintf (ChipID.ProcessorName, "Newer i80386 family"); 
					break;
				case 4:
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("i80486DX-25/33"); break;
						case 1: STORE_CLASSICAL_NAME ("i80486DX-50"); break;
						case 2: STORE_CLASSICAL_NAME ("i80486SX"); break;
						case 3: STORE_CLASSICAL_NAME ("i80486DX2"); break;
						case 4: STORE_CLASSICAL_NAME ("i80486SL"); break;
						case 5: STORE_CLASSICAL_NAME ("i80486SX2"); break;
						case 7: STORE_CLASSICAL_NAME ("i80486DX2 WriteBack"); break;
						case 8: STORE_CLASSICAL_NAME ("i80486DX4"); break;
						case 9: STORE_CLASSICAL_NAME ("i80486DX4 WriteBack"); break;
						//default: STORE_CLASSICAL_NAME ("Unknown 80486 family"); return false;
					}
					break;
					
				case 5:
					
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("P5 A-Step"); break;
						case 1: STORE_CLASSICAL_NAME ("P5"); break;
						case 2: STORE_CLASSICAL_NAME ("P54C"); break;
						case 3: STORE_CLASSICAL_NAME ("P24T OverDrive"); break;
						case 4: STORE_CLASSICAL_NAME ("P55C"); break;
						case 7: STORE_CLASSICAL_NAME ("P54C"); break;
						case 8: STORE_CLASSICAL_NAME ("P55C (0.25µm)"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Pentium® family"); return false;
					}
					
					break;
					*/

				case 6:
					switch (ChipID.Model) {
						/*
						case 0: STORE_CLASSICAL_NAME ("P6 A-Step"); break;
						case 1: STORE_CLASSICAL_NAME ("P6"); break;
						case 3: STORE_CLASSICAL_NAME ("Pentium® II (0.28 µm)"); break;
						case 5: STORE_CLASSICAL_NAME ("Pentium® II (0.25 µm)"); break;
						case 6: STORE_CLASSICAL_NAME ("Pentium® II With On-Die L2 Cache"); break;
						case 7: STORE_CLASSICAL_NAME ("Pentium® III (0.25 µm)"); break;
						case 8: STORE_CLASSICAL_NAME ("Pentium® III (0.18 µm) With 256 KB On-Die L2 Cache "); break;
						case 0xa: STORE_CLASSICAL_NAME ("Pentium® III (0.18 µm) With 1 Or 2 MB On-Die L2 Cache "); break;
						case 0xb: STORE_CLASSICAL_NAME ("Pentium® III (0.13 µm) With 256 Or 512 KB On-Die L2 Cache "); break;
						default: STORE_CLASSICAL_NAME ("Unknown P6 family"); return false;
						*/

						case 0: STORE_CLASSICAL_NAME ("P6"); break;
						//case 1: STORE_CLASSICAL_NAME ("P6"); break;
						case 3: STORE_CLASSICAL_NAME ("Pentium® II"); break;
						case 5: STORE_CLASSICAL_NAME ("Pentium® II"); break;
						case 6: STORE_CLASSICAL_NAME ("Pentium® II"); break;
						case 7: STORE_CLASSICAL_NAME ("Pentium® III"); break;
						case 8: STORE_CLASSICAL_NAME ("Pentium® III"); break;
						case 0xa: STORE_CLASSICAL_NAME ("Pentium® III"); break;
						case 0xb: STORE_CLASSICAL_NAME ("Pentium® III"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Pentium 3 family"); return false;
					}
					break;
					/*
				case 7:
					STORE_CLASSICAL_NAME ("Intel Merced (IA-64)");
					break;
					*/
				case 0xf:
					// Check the extended family bits...
					switch (ChipID.ExtendedFamily) {
						case 0:
							switch (ChipID.Model) {
								case 0: STORE_CLASSICAL_NAME ("Pentium® IV"); break;
								case 1: STORE_CLASSICAL_NAME ("Pentium® IV"); break;
								case 2: STORE_CLASSICAL_NAME ("Pentium® IV"); break;
								default: STORE_CLASSICAL_NAME ("Unknown Pentium 4 family"); return false;
							}
							break;
							/*
						case 1:
							STORE_CLASSICAL_NAME ("Intel McKinley (IA-64)");
							break;
							*/
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown Intel family");
					return false;
			}
			break;

		case AMD:
			// Check the family / model / revision to determine the CPU ID.
			switch (ChipID.Family) {
				case 4:
					/*
					switch (ChipID.Model) {
						case 3: STORE_CLASSICAL_NAME ("80486DX2"); break;
						case 7: STORE_CLASSICAL_NAME ("80486DX2 WriteBack"); break;
						case 8: STORE_CLASSICAL_NAME ("80486DX4"); break;
						case 9: STORE_CLASSICAL_NAME ("80486DX4 WriteBack"); break;
						case 0xe: STORE_CLASSICAL_NAME ("5x86"); break;
						case 0xf: STORE_CLASSICAL_NAME ("5x86WB"); break;
						default: STORE_CLASSICAL_NAME ("Unknown 80486 family"); return false;
					}
					break;
					*/
				case 5:
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("SSA5"); break;
						case 1: STORE_CLASSICAL_NAME ("5k86"); break;
						case 2: STORE_CLASSICAL_NAME ("5k86"); break;
						case 3: STORE_CLASSICAL_NAME ("5k86"); break;
						case 6: STORE_CLASSICAL_NAME ("K6"); break;
						case 7: STORE_CLASSICAL_NAME ("K6"); break;
						case 8: STORE_CLASSICAL_NAME ("K6-2"); break;
						case 9: STORE_CLASSICAL_NAME ("K6-III"); break;
						case 0xd: STORE_CLASSICAL_NAME ("K6-2+"); break;
						default: STORE_CLASSICAL_NAME ("Unknown 80586 family"); return false;
					}
					break;
				case 6:
					switch (ChipID.Model) {
						case 1: STORE_CLASSICAL_NAME ("Athlon"); break;
						case 2: STORE_CLASSICAL_NAME ("Athlon"); break;
						case 3: STORE_CLASSICAL_NAME ("Duron ( SF core )"); break;
						case 4: STORE_CLASSICAL_NAME ("Athlon ( Thunderbird core )"); break;
						case 6: STORE_CLASSICAL_NAME ("Athlon ( Palomino core )"); break;
						case 7: STORE_CLASSICAL_NAME ("Duron ( Morgan core )"); break;
						case 8: 
							if (Features.ExtendedFeatures.SupportsMP)
								STORE_CLASSICAL_NAME ("Athlon MP"); 
							else STORE_CLASSICAL_NAME ("Athlon XP");
							break;
						default: STORE_CLASSICAL_NAME ("Unknown K7 family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown AMD family");
					return false;
			}
			break;

			/*
		case Transmeta:
			switch (ChipID.Family) {	
				case 5:
					switch (ChipID.Model) {
						case 4: STORE_CLASSICAL_NAME ("Crusoe TM3x00 and TM5x00"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Crusoe family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown Transmeta family");
					return false;
			}
			break;

		case Rise:
			switch (ChipID.Family) {	
				case 5:
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("mP6 (0.25 µm)"); break;
						case 2: STORE_CLASSICAL_NAME ("mP6 (0.18 µm)"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Rise family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown Rise family");
					return false;
			}
			break;

		case UMC:
			switch (ChipID.Family) {	
				case 4:
					switch (ChipID.Model) {
						case 1: STORE_CLASSICAL_NAME ("U5D"); break;
						case 2: STORE_CLASSICAL_NAME ("U5S"); break;
						default: STORE_CLASSICAL_NAME ("Unknown UMC family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown UMC family");
					return false;
			}
			break;

		case IDT:
			switch (ChipID.Family) {	
				case 5:
					switch (ChipID.Model) {
						case 4: STORE_CLASSICAL_NAME ("C6"); break;
						case 8: STORE_CLASSICAL_NAME ("C2"); break;
						case 9: STORE_CLASSICAL_NAME ("C3"); break;
						default: STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family"); return false;
					}
					break;
				case 6:
					switch (ChipID.Model) {
						case 6: STORE_CLASSICAL_NAME ("VIA Cyrix III - Samuel"); break;
						default: STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown IDT\\Centaur family");
					return false;
			}
			break;
			*/

			/*
		case Cyrix:
			switch (ChipID.Family) {	
				case 4:
					switch (ChipID.Model) {
						case 4: STORE_CLASSICAL_NAME ("MediaGX GX, GXm"); break;
						case 9: STORE_CLASSICAL_NAME ("5x86"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Cx5x86 family"); return false;
					}
					break;
				case 5:
					switch (ChipID.Model) {
						case 2: STORE_CLASSICAL_NAME ("Cx6x86"); break;
						case 4: STORE_CLASSICAL_NAME ("MediaGX GXm"); break;
						default: STORE_CLASSICAL_NAME ("Unknown Cx6x86 family"); return false;
					}
					break;
				case 6:
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("6x86MX"); break;
						case 5: STORE_CLASSICAL_NAME ("Cyrix M2 Core"); break;
						case 6: STORE_CLASSICAL_NAME ("WinChip C5A Core"); break;
						case 7: STORE_CLASSICAL_NAME ("WinChip C5B\\C5C Core"); break;
						case 8: STORE_CLASSICAL_NAME ("WinChip C5C-T Core"); break;
						default: STORE_CLASSICAL_NAME ("Unknown 6x86MX\\Cyrix III family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown Cyrix family");
					return false;
			}
			break;
			*/

			/*
		case NexGen:
			switch (ChipID.Family) {	
				case 5:
					switch (ChipID.Model) {
						case 0: STORE_CLASSICAL_NAME ("Nx586 or Nx586FPU"); break;
						default: STORE_CLASSICAL_NAME ("Unknown NexGen family"); return false;
					}
					break;
				default:
					STORE_CLASSICAL_NAME ("Unknown NexGen family");
					return false;
			}
			break;

		case NSC:
			STORE_CLASSICAL_NAME ("Cx486SLC \\ DLC \\ Cx486S A-Step");
			break;
			*/

		default:
			// We cannot identify the processor.
			STORE_CLASSICAL_NAME ("Unknown family");
			return false;
	}

	return true;
}

// --------------------------------------------------------
//
//         Constructor Functions - CPUSpeed Class
//
// --------------------------------------------------------

CPUSpeed::CPUSpeed ()
{
	unsigned int uiRepetitions = 1;
	unsigned int uiMSecPerRepetition = 50;
	__int64	i64Total = 0, i64Overhead = 0;

	for (unsigned int nCounter = 0; nCounter < uiRepetitions; nCounter ++) {
		i64Total += GetCyclesDifference (CPUSpeed::Delay, uiMSecPerRepetition);
		i64Overhead += GetCyclesDifference (CPUSpeed::DelayOverhead, uiMSecPerRepetition);
	}

	// Calculate the MHz speed.
	i64Total -= i64Overhead;
	i64Total /= uiRepetitions;
	i64Total /= uiMSecPerRepetition;
	i64Total /= 1000;

	// Save the CPU speed.
	CPUSpeedInMHz = (int) i64Total;
}

CPUSpeed::~CPUSpeed ()
{
}

__int64	__cdecl CPUSpeed::GetCyclesDifference (DELAY_FUNC DelayFunction, unsigned int uiParameter)
{
	unsigned int edx1, eax1;
	unsigned int edx2, eax2;
		
	// Calculate the frequency of the CPU instructions.
	__try {
		_asm {
			push uiParameter		; push parameter param
			mov ebx, DelayFunction	; store func in ebx

			RDTSC_INSTRUCTION

			mov esi, eax			; esi = eax
			mov edi, edx			; edi = edx

			call ebx				; call the delay functions

			RDTSC_INSTRUCTION

			pop ebx

			mov edx2, edx			; edx2 = edx
			mov eax2, eax			; eax2 = eax

			mov edx1, edi			; edx2 = edi
			mov eax1, esi			; eax2 = esi
		}
	}

	// A generic catch-all just to be sure...
	__except (1) {
		return -1;
	}

	return (CPUSPEED_I32TO64 (edx2, eax2) - CPUSPEED_I32TO64 (edx1, eax1));
}

void CPUSpeed::Delay (unsigned int uiMS)
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);

	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart < x);
}

void CPUSpeed::DelayOverhead (unsigned int uiMS)
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);
	
	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart == x);
}