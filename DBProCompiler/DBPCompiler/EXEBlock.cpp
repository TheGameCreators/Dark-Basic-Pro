//#include "stdafx.h"
// EXEBlock.cpp: implementation of the CEXEBlock class.
//#define INITGUID
#include "Error.h"
#include "EXEBlock.h"
#include "direct.h"
#include "..\DBPCompilerEXE\resource.h"
#include ".\..\..\Dark Basic Public Shared\Dark basic Pro SDK\Shared\Core\globstruct.h"

// Prototype for associated GETDXVER.CPP (or empty true in MAIN.CPP if compiler)
HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

// Internal Function Pointers For Core Management
GDI_RetVoidParamVoidPFN				g_CORE_Program;
GDI_RetVoidParamLPVOID				g_CORE_PassCmdLinePtr;
GDI_RetVoidParamLPVOID				g_CORE_PassErrorPtr;
GDI_RetVoidParamLPVOID				g_CORE_PassEscapePtr;
GDI_RetVoidParamLPVOID				g_CORE_PassBreakOutPtr;
GDI_RetVoidParamLPVOIDDWORD			g_CORE_PassStructurePatterns;
GDI_RetVoidParamLPSTR2				g_CORE_PassDataPtrs;
GDI_RetDWORDParamDWORD4HINSTLPSTRPFN	g_CORE_InitDisplay;
GDI_RetVoidParamVoidPFN				g_CORE_PassDLLs;
GDI_RetVoidParamVoidPFN				g_CORE_ConstructDLLs;
GDI_RetDWORDParamVoidPFN			g_CORE_GetGlobPtr;

GDI_RetDWORDParamVoidPFN			g_CORE_CloseDisplay;
GDI_RetDWORDParamDWORDPFN			g_CORE_CreateVarSpace;
GDI_RetVoidParamVoidPFN				g_CORE_DeleteVarSpace;
GDI_RetDWORDParamDWORDPFN			g_CORE_CreateDataSpace;
GDI_RetVoidParamVoidPFN				g_CORE_DeleteDataSpace;
GDI_RetVoidParamDWORDPTRPFN			g_CORE_DeleteVarItem;
GDI_RetVoidParamDWORDPTRPFN			g_CORE_UnDim;
GDI_RetVoidParamVoidPFN				g_CORE_SyncRefresh;
GDI_RetIntParamVoidPFN				g_CORE_GetSecurityCode;
GDI_RetVoidParamVoidPFN				g_CORE_WipeSecurityCode;

// Internal Function Pointers For Transforms Management
DLL_Constructor						g_Transforms_Constructor;
DLL_Destructor						g_Transforms_Destructor;
DLL_Update							g_Transforms_Update;

// Internal Function Pointers For Sprite Management
SPRITES_RetVoidParamHINSTANCE2PFN	g_Sprites_Constructor;
SPRITES_RetVoidParamVoidPFN			g_Sprites_Destructor;
SPRITES_RetVoidParamVoidPFN			g_Sprites_Update;

// Internal Function Pointers For Image Management
IMAGE_RetVoidParamVoidPFN			g_Image_Constructor;				// constructor
IMAGE_RetVoidParamVoidPFN			g_Image_Destructor;					// destructor

// Global DLL Storage
HINSTANCE							hDLLMod[256];
bool								bDLLTPC[256];

// Global Variables
bool								g_bSuccessfulDLLLinks			= false;
DWORD								g_dwEscapeValueMem				= 0;
DWORD								g_dwBreakOutPosition			= 0;
LPSTR								g_pVarSpaceAddressInUse			= NULL;
DWORD								g_dwVarSpaceSizeInUse			= 0;
bool								g_bIsInternalDebugger			= false;
PROCESS_INFORMATION					g_InternalDebuggerProcessInfo;
HANDLE								g_hLastGFXPointer				= NULL;

// Store DirectX version for globstruct transfer (so core knows what we have - 080306)
DWORD								g_dwDirectXVersion				= 0;
TCHAR								g_strDirectXVersion[10]			= { 0 };

// Global Shared Data Pointer (passed in from core)
GlobStruct*							g_pGlob							= NULL;

// If linked from DarkEXE (temp loading... window)
extern HWND g_hTempWindow;
extern HWND g_igLoader_HWND;

#ifdef DEMOPROTECTEDMODE
bool WriteStringToRegistry(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	DWORD dwDisposition;
	char ObjectType[256];
	strcpy(ObjectType,"Num");
	Status = RegCreateKeyEx(HKEY_CURRENT_USER, PerfmonNamesKey, 0L, ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKeyNames, &dwDisposition);
	if(dwDisposition==REG_OPENED_EXISTING_KEY)
	{
		RegCloseKey(hKeyNames);
		Status = RegOpenKeyEx(HKEY_CURRENT_USER, PerfmonNamesKey, 0L, KEY_WRITE, &hKeyNames);
	}
    if(Status==ERROR_SUCCESS)
	{
        Status = RegSetValueEx(hKeyNames, valuekey, 0, REG_SZ, (LPBYTE)string, (strlen(string)+1)*sizeof(char));
	}
	RegCloseKey(hKeyNames);
	hKeyNames=0;
	return true;
}
void ReadStringFromRegistry(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	char ObjectType[256];
	DWORD Datavalue = 0;

	strcpy(string,"");
	strcpy(ObjectType,"Num");
	Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PerfmonNamesKey, 0L, KEY_READ, &hKeyNames);
    if(Status==ERROR_SUCCESS)
	{
		DWORD Type=REG_SZ;
		DWORD Size=256;
		Status = RegQueryValueEx(hKeyNames, valuekey, NULL, &Type, NULL, &Size);
		if(Size<255)
			RegQueryValueEx(hKeyNames, valuekey, NULL, &Type, (LPBYTE)string, &Size);

		RegCloseKey(hKeyNames);
	}
}
#endif

CEXEBlock::CEXEBlock()
{
	// Reset flags
	m_bEXEBlockPresent=false;

	// Settings
	m_dwInitialDisplayMode=1;
	m_dwInitialDisplayWidth=0;
	m_dwInitialDisplayHeight=0;
	m_dwInitialDisplayDepth=0;
	m_pInitialAppName=NULL;

	m_pOriginalFolderName=new char[_MAX_PATH+1];
	strcpy(m_pOriginalFolderName,"");
	m_pUnpackFolderName=new char[_MAX_PATH+1];
	strcpy(m_pUnpackFolderName,"");
	m_pAbsoluteAppFile=new char[_MAX_PATH+1];
	strcpy(m_pAbsoluteAppFile,"");

	// DLL Data
	m_dwNumberOfDLLs=0;
	m_pDLLIndexArray=NULL;
	m_pDLLFilenameArray=NULL;
	m_pDLLLoadedAlreadyArray=NULL;

	// MCB Reference Data
	m_dwNumberOfReferences=0;
	m_pRefArray=NULL;
	m_pRefTypeArray=NULL;
	m_pRefIndexArray=NULL;

	// Clear runtime error dword
	m_dwRuntimeErrorDWORD=0;
	m_dwRuntimeErrorLineDWORD=0;

	// Runtime string array database
	m_dwNumberOfRuntimeErrorStrings=0;
	m_pRuntimeErrorStringsArray=NULL;

	// Machine Code Block (MCB)
	m_dwSizeOfMCB=0;
	m_pMachineCodeBlock=NULL;
	m_dwStartOfMiniMC=0;

	// Commands Data
	m_dwNumberOfCommands=0;
	m_pCommandDLLIdArray=NULL;
	m_pCommandDLLCallArray=NULL;

	// Strings Data
	m_dwNumberOfStrings=0;
	m_pStringsArray=NULL;

	// Data Statements Data
	m_dwNumberOfDataItems=0;
	m_pDataArray=NULL;
	m_pDataStringsArray=NULL;

	// Variable Space Data
	m_dwVariableSpaceSize=0;
	m_pVariableSpace=NULL;

	// Data Space Data
	m_dwDataSpaceSize=0;
	m_pDataSpace=NULL;

	// Record Dynamic Variables for auto-freeing
	m_dwDynamicVarsQuantity=0;
	m_pDynamicVarsArray=NULL;
	m_pDynamicVarsArrayType=NULL;

	// Record UserTypeStringPatterns - reactivated for U71 (store structure types)
	m_dwUsertypeStringPatternQuantity=0;
	m_pUsertypeStringPatternArray=NULL;
}

CEXEBlock::~CEXEBlock()
{
	Clear();
}

void CEXEBlock::Clear(void)
{
	// Release appname
	SAFE_DELETE(m_pInitialAppName);

	// Release exefile ptrs
	SAFE_DELETE(m_pOriginalFolderName);
	SAFE_DELETE(m_pUnpackFolderName);
	SAFE_DELETE(m_pAbsoluteAppFile);

	// Release DLLs Data
	if ( m_pDLLFilenameArray ) DeleteArrayContents(m_pDLLFilenameArray,m_dwNumberOfDLLs);
	SAFE_DELETE(m_pDLLFilenameArray);
	SAFE_DELETE(m_pDLLIndexArray);
	SAFE_DELETE(m_pDLLLoadedAlreadyArray);

	// Release Ref Data
	SAFE_DELETE(m_pRefArray);
	SAFE_DELETE(m_pRefIndexArray);
	SAFE_DELETE(m_pRefTypeArray);

	// Release MCB Data 9leeadd - 090305 - DEP release)
	VirtualFree ( m_pMachineCodeBlock, 0, MEM_DECOMMIT | MEM_RELEASE );
	m_pMachineCodeBlock = NULL;

	// Release Runtime Error Strings Database
	if ( m_pRuntimeErrorStringsArray ) DeleteArrayContents(m_pRuntimeErrorStringsArray,m_dwNumberOfRuntimeErrorStrings);
	SAFE_DELETE(m_pRuntimeErrorStringsArray);

	// Release Commands Data
	if ( m_pCommandDLLCallArray ) DeleteArrayContents(m_pCommandDLLCallArray,m_dwNumberOfCommands);
	SAFE_DELETE(m_pCommandDLLCallArray);
	SAFE_DELETE(m_pCommandDLLIdArray);

	// Release Strings Data
	if ( m_pStringsArray ) DeleteArrayContents(m_pStringsArray,m_dwNumberOfStrings);
	SAFE_DELETE(m_pStringsArray);

	// Release Data Data
	SAFE_DELETE(m_pDataArray);
	if ( m_pDataStringsArray) DeleteArrayContents(m_pDataStringsArray,m_dwNumberOfDataItems);
	SAFE_DELETE(m_pDataStringsArray);

	// Release Dynamic Variable Offset Array
	SAFE_DELETE(m_pDynamicVarsArray);
	SAFE_DELETE(m_pDynamicVarsArrayType);

	// Release Structure Pattern Array
	SAFE_DELETE(m_pUsertypeStringPatternArray);

	// Reset all values
	m_bEXEBlockPresent=false;
	m_dwNumberOfDLLs=0;
	m_dwNumberOfReferences=0;
	m_dwSizeOfMCB=0;
	m_dwNumberOfCommands=0;
	m_dwNumberOfStrings=0;
	m_dwVariableSpaceSize=0;
	m_dwDataSpaceSize=0;
}

DWORD* CEXEBlock::CreateArray(DWORD dwCount,DWORD dwType)
{
	// leeadd - 090305 - DEP or regular flavour
	DWORD* pArray = NULL;
	if ( dwType==PAGE_EXECUTE_READWRITE )
	{
		// data block used to execute code
		pArray = (DWORD*) VirtualAlloc ( NULL, dwCount*sizeof(DWORD), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	}
	else
	{
		// regular data block
		pArray = new DWORD[dwCount];
		for(DWORD i=0; i<dwCount; i++)
			*(pArray+i)=NULL;
	}

	// return ptr
	return pArray;
}

DWORD* CEXEBlock::CreateArray(DWORD dwCount)
{
	return CreateArray(dwCount,0);
}

bool CEXEBlock::RecreateArray(DWORD** pArray, DWORD dwCount, DWORD NewCount)
{
	if(pArray)
	{
		DWORD* pTempArray = new DWORD[NewCount];
		if(pTempArray)
		{
			// Clear New
			for(DWORD i=0; i<NewCount; i++)
				*(pTempArray+i)=NULL;

			// Copy Old to New
			memcpy(pTempArray, *pArray, dwCount*sizeof(DWORD));

			// Delete Old (pointer array only)
			delete *pArray;

			// Switch Pointers
			*pArray=pTempArray;

			return true;
		}
		else
		{
			// EXEBlock shared - silent fail
			return false;
		}
	}

	// Soft fail
	return false;
}

void CEXEBlock::DeleteArrayContents(DWORD* pArray, DWORD dwCount)
{
	if(pArray)
	{
		for(DWORD i=0; i<dwCount; i++)
		{
			if(*(pArray+i))
			{
				delete (DWORD*)*(pArray+i);
				*(pArray+i)=NULL;
			}
		}
	}
}

bool CEXEBlock::FileExists(LPSTR pFilename)
{
	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Close File
		SAFE_CLOSE(hFile);
		return true;
	}
	// soft fail
	return false;
}

bool CEXEBlock::Save(char* lpFilename)
{
	HANDLE hFile = CreateFile(lpFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Settings
		SaveValue(hFile, &m_dwInitialDisplayMode);
		SaveValue(hFile, &m_dwInitialDisplayWidth);
		SaveValue(hFile, &m_dwInitialDisplayHeight);
		SaveValue(hFile, &m_dwInitialDisplayDepth);

		// Save AppName String
		DWORD dwLength=256;
		SaveValueArrayBytes(hFile, (DWORD**)&m_pInitialAppName, &dwLength);

		// DLL Data
		SaveValue(hFile, &m_dwNumberOfDLLs);
		SaveValueArray(hFile, &m_pDLLIndexArray, &m_dwNumberOfDLLs);
		SaveStringArray(hFile, &m_pDLLFilenameArray, &m_dwNumberOfDLLs);
		SaveValueArray(hFile, &m_pDLLLoadedAlreadyArray, &m_dwNumberOfDLLs);

		// MCB Reference Data
		SaveValue(hFile, &m_dwNumberOfReferences);
		SaveValueArray(hFile, &m_pRefArray, &m_dwNumberOfReferences);
		SaveValueArray(hFile, &m_pRefTypeArray, &m_dwNumberOfReferences);
		SaveValueArray(hFile, &m_pRefIndexArray, &m_dwNumberOfReferences);

		// Runtime Error String Database
		SaveValue(hFile, &m_dwNumberOfRuntimeErrorStrings);
		SaveStringArray(hFile, &m_pRuntimeErrorStringsArray, &m_dwNumberOfRuntimeErrorStrings);

		// Machine Code Block (MCB)
		SaveValue(hFile, &m_dwSizeOfMCB);
		SaveValueArrayBytes(hFile, &m_pMachineCodeBlock, &m_dwSizeOfMCB);

		// Commands Data
		SaveValue(hFile, &m_dwNumberOfCommands);
		SaveValueArray(hFile, &m_pCommandDLLIdArray, &m_dwNumberOfCommands);
		SaveStringArray(hFile, &m_pCommandDLLCallArray, &m_dwNumberOfCommands);

		// Strings Data
		SaveValue(hFile, &m_dwNumberOfStrings);
		SaveStringArray(hFile, &m_pStringsArray, &m_dwNumberOfStrings);

		// Data Data
		SaveValue(hFile, &m_dwNumberOfDataItems);
		SaveBlock(hFile, &m_pDataArray, m_dwNumberOfDataItems*10);
		SaveStringArray(hFile, &m_pDataStringsArray, &m_dwNumberOfDataItems);

		// Variable Space Data
		SaveValue(hFile, &m_dwVariableSpaceSize);

		// Data Space Data
		SaveValue(hFile, &m_dwDataSpaceSize);

		// Dynamic Variable Offset Data
		SaveValue(hFile, &m_dwDynamicVarsQuantity);
		SaveValueArray(hFile, &m_pDynamicVarsArray, &m_dwDynamicVarsQuantity);
		SaveValueArray(hFile, &m_pDynamicVarsArrayType, &m_dwDynamicVarsQuantity);

		// Usertype String Patterns - reactivated for U71 (store structure types)
		SaveValue(hFile, &m_dwUsertypeStringPatternQuantity);
		SaveBlock(hFile, &m_pUsertypeStringPatternArray, m_dwUsertypeStringPatternQuantity);

		// Close file
		CloseHandle(hFile);
		return true;
	}
	else
	{
		// EXEBlock shared - silent fail
		return false;
	}
}

bool CEXEBlock::SaveValue(HANDLE hFile, DWORD* Value)
{
	DWORD bytes=0;
	WriteFile(hFile, Value, 4, &bytes, NULL);
	if(bytes==0)
	{
		// EXEBlock shared - silent fail
		return false;
	}
	else
		return true;
}

bool CEXEBlock::SaveBlock(HANDLE hFile, LPSTR* pMem, DWORD dwSize)
{
	DWORD bytes=0;
	WriteFile(hFile, *pMem, dwSize, &bytes, NULL);
	if(bytes==0)
	{
		// EXEBlock shared - silent fail
		return false;
	}
	else
		return true;
}

bool CEXEBlock::SaveValueArray(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*pArray && *Count>0)
	{
		WriteFile(hFile, *pArray, (*Count)*sizeof(DWORD), &bytes, NULL);
		if(bytes==0) bResult=false;
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::SaveValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*pArray && *Count>0)
	{
		WriteFile(hFile, *pArray, (*Count), &bytes, NULL);
		if(bytes==0) bResult=false;
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::SaveStringArray(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*pArray && *Count>0)
	{
		for(DWORD index=0; index<*Count; index++)
		{
			char* pStr = (char*)*(*pArray+index);
			DWORD length = 0;
			if(pStr) length = strlen(pStr);
			WriteFile(hFile, &length, 4, &bytes, NULL);
			if(bytes==0) bResult=false;
			if(pStr)
			{
				// Write number of bytes in string
				if(length>0)
				{
					// Write string if has a length
					WriteFile(hFile, pStr, length, &bytes, NULL);
					if(bytes==0) bResult=false;
				}
			}
		}
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::StartInfo(LPSTR pUnpackFolderName, DWORD dwEncryptionKey)
{
	// Set Unpack Folder (and copy to global data)
	strcpy(m_pUnpackFolderName, pUnpackFolderName);
	m_dwEncryptionKey = dwEncryptionKey;

	// Complete
	return true;
}

bool CEXEBlock::Load(char* lpFilename)
{
	// Load EXE Filedata
	HANDLE hFile = CreateFile(lpFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Settings
		LoadValue(hFile, &m_dwInitialDisplayMode);
		LoadValue(hFile, &m_dwInitialDisplayWidth);
		LoadValue(hFile, &m_dwInitialDisplayHeight);
		LoadValue(hFile, &m_dwInitialDisplayDepth);

		// AppName String
		DWORD dwLength=256;
		LoadValueArrayBytes(hFile, (DWORD**)&m_pInitialAppName, &dwLength);

		// DLL Data
		LoadValue(hFile, &m_dwNumberOfDLLs);
		LoadValueArray(hFile, &m_pDLLIndexArray, &m_dwNumberOfDLLs);
		LoadStringArray(hFile, &m_pDLLFilenameArray, &m_dwNumberOfDLLs);
		LoadValueArray(hFile, &m_pDLLLoadedAlreadyArray, &m_dwNumberOfDLLs);
		ZeroMemory(m_pDLLLoadedAlreadyArray, sizeof(DWORD)*m_dwNumberOfDLLs);
		
		// MCB Reference Data
		LoadValue(hFile, &m_dwNumberOfReferences);
		LoadValueArray(hFile, &m_pRefArray, &m_dwNumberOfReferences);
		LoadValueArray(hFile, &m_pRefTypeArray, &m_dwNumberOfReferences);
		LoadValueArray(hFile, &m_pRefIndexArray, &m_dwNumberOfReferences);

		// Runtime Error String Database
		LoadValue(hFile, &m_dwNumberOfRuntimeErrorStrings);
		LoadStringArray(hFile, &m_pRuntimeErrorStringsArray, &m_dwNumberOfRuntimeErrorStrings);

		// Machine Code Block (MCB)
		// leeadd - 090305 - added flag for DEP protection allowance
		LoadValue(hFile, &m_dwSizeOfMCB);
		LoadValueArrayBytes(hFile, &m_pMachineCodeBlock, &m_dwSizeOfMCB, PAGE_EXECUTE_READWRITE);

		// Commands Data
		LoadValue(hFile, &m_dwNumberOfCommands);
		LoadValueArray(hFile, &m_pCommandDLLIdArray, &m_dwNumberOfCommands);
		LoadStringArray(hFile, &m_pCommandDLLCallArray, &m_dwNumberOfCommands);

		// Strings Data
		LoadValue(hFile, &m_dwNumberOfStrings);
		LoadStringArray(hFile, &m_pStringsArray, &m_dwNumberOfStrings);

		// Data Data
		LoadValue(hFile, &m_dwNumberOfDataItems);
		LoadBlock(hFile, &m_pDataArray, m_dwNumberOfDataItems*10);
		LoadStringArray(hFile, &m_pDataStringsArray, &m_dwNumberOfDataItems);

		// Variable Space Data
		LoadValue(hFile, &m_dwVariableSpaceSize);

		// Data Space Data
		LoadValue(hFile, &m_dwDataSpaceSize);

		// Dynamic Variable Offset Data
		LoadValue(hFile, &m_dwDynamicVarsQuantity);
		LoadValueArray(hFile, &m_pDynamicVarsArray, &m_dwDynamicVarsQuantity);
		LoadValueArray(hFile, &m_pDynamicVarsArrayType, &m_dwDynamicVarsQuantity);

		// Usertype String Patterns - reactivated for U71 (store structure types)
		LoadValue(hFile, &m_dwUsertypeStringPatternQuantity);
		LoadBlock(hFile, &m_pUsertypeStringPatternArray, m_dwUsertypeStringPatternQuantity);

		// Close file
		CloseHandle(hFile);
		return true;
	}
	else
	{
		// EXEBlock shared - silent fail
		return false;
	}
}

bool CEXEBlock::LoadValue(HANDLE hFile, DWORD* Value)
{
	DWORD bytes=0;
	ReadFile(hFile, Value, 4, &bytes, NULL);
	if(bytes==0)
	{
		// EXEBlock shared - silent fail
		return false;
	}
	else
		return true;
}

bool CEXEBlock::LoadBlock(HANDLE hFile, LPSTR* pMem, DWORD dwSize)
{
	DWORD bytes=0;
	*pMem = new char[dwSize+1];
	ReadFile(hFile, *pMem, dwSize, &bytes, NULL);
	if(bytes==0)
	{
		// EXEBlock shared - silent fail
		return false;
	}
	else
		return true;
}

bool CEXEBlock::LoadValueArray(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*Count>0)
	{
		// Create Array 
		*pArray = CreateArray(*Count,0);

		// Read data into Array
		ReadFile(hFile, *pArray, (*Count)*sizeof(DWORD), &bytes, NULL);
		if(bytes==0) bResult=false;
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::LoadValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count, DWORD dwType)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*Count>0)
	{
		// Create Array 
		*pArray = CreateArray(*Count,dwType);

		// Read data into Array
		ReadFile(hFile, *pArray, (*Count), &bytes, NULL);
		if(bytes==0) bResult=false;
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::LoadValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	return LoadValueArrayBytes(hFile, pArray, Count, 0);
}


bool CEXEBlock::LoadStringArray(HANDLE hFile, DWORD** pArray, DWORD* Count)
{
	DWORD bytes=0;
	bool bResult=true;
	if(*Count>0)
	{
		// Create Array 
		*pArray = CreateArray(*Count,0);

		// Read strings into Array of strings
		for(DWORD index=0; index<*Count; index++)
		{
			// Read length of string
			DWORD length = 0;
			ReadFile(hFile, &length, 4, &bytes, NULL);
			char* pStr = new char[length+1];
			if(length>0)
			{
				ReadFile(hFile, pStr, length, &bytes, NULL);
				if(bytes==0) bResult=false;
			}
			pStr[length]=0;
			*(*pArray+index) = (DWORD)pStr;
		}
	}
	else
		bResult=false;

	return bResult;
}

bool CEXEBlock::Init(HINSTANCE hInstance, bool bResult, LPSTR* pReturnError, LPSTR pCmdLine)
{
	bResult=InitDebug(hInstance, NULL, NULL, NULL, bResult, pReturnError, pCmdLine, false);
	return bResult;
}

bool CEXEBlock::InitMini(LPVOID pDHookS, LPVOID pDHookJ, LPVOID pDHookR, bool bResult, LPSTR* pReturnError)
{
	bResult=InitDebug(NULL, pDHookS, pDHookJ, pDHookR, bResult, pReturnError, NULL, true);
	return bResult;
}

bool CEXEBlock::CheckIfGotLatestDirectX ( bool bSilent )
{
	// Check if got Vista, which means DX9 is already there (as 9.0L)
	#ifndef NOGETDXVERSIONCODE
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (::GetVersionEx (&versionInfo))
	{
		if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if (versionInfo.dwMajorVersion >= 6)
			{
				// yes we have it by virtue of Vista!
				return true;
			}
		}
	}

	// LATEST DIRECTX EXE NEEDS..
	LPSTR pDXRequired = "DirectX 9.0c (October 2006) or later";
	DWORD dwRequiresVersion = 0x00090003;

	// lee - 270206 - u60 - new check, right out of DXSKDEC2005
    TCHAR strResult[128];
    HRESULT hr = GetDXVersion( &g_dwDirectXVersion, g_strDirectXVersion, 10 );
    if( SUCCEEDED(hr) )
    {
        if( g_dwDirectXVersion > 0 )
			wsprintf ( strResult, "DirectX %s installed (%d). Requires %s.", g_strDirectXVersion, g_dwDirectXVersion, pDXRequired );
        else
            wsprintf ( strResult, "DirectX not installed" );
    }
    else
        wsprintf ( strResult, "Unknown version of DirectX installed" );

	if ( g_dwDirectXVersion < dwRequiresVersion && bSilent==false )
	{
		// fail immediately if not got dx!
		MessageBox(NULL, strResult, "DirectX Error", MB_OK | MB_ICONERROR);
		return false;
	}
	#endif

	// yes we have it!
	return true;
}

bool CEXEBlock::InitDebug(HINSTANCE hInstance, LPVOID pDHookS, LPVOID pDHookJ, LPVOID pDHookR, bool bResult, LPSTR* pReturnError, LPSTR pCmdLine, bool bMiniInit)
{
	static const char *const pCoreName = "dbprocore.dll";

	// [EXE] Ensure dbprocore.dll is always present
	if(m_dwNumberOfDLLs==0)
	{
		// Make Core Entry
		m_dwNumberOfDLLs=1;
		m_pDLLIndexArray = new DWORD[1];
		m_pDLLIndexArray[0]=1;
		m_pDLLLoadedAlreadyArray = new DWORD[1];
		m_pDLLLoadedAlreadyArray[0] = 0;

		m_pDLLFilenameArray = new DWORD[1];
		m_pDLLFilenameArray[0] = (DWORD)new char[strlen(pCoreName)+1];
		strcpy((LPSTR)m_pDLLFilenameArray[0], pCoreName);
	}

	// [EXE] - Detect if using Basic3D.DLL and if so, check for DX9!
	bool bBasic3DIsUsedSoWeNeedDirectXCheck = false;
	if ( m_dwNumberOfDLLs>0 )
	{
		for(DWORD dll=0; dll<m_dwNumberOfDLLs; dll++)
		{
			int dllindex=m_pDLLIndexArray[dll];
			if(dllindex<=255)
			{
				// Load the DLL into memory
				LPSTR pDLLName = (LPSTR)m_pDLLFilenameArray[dll];
				if(stricmp(pDLLName, "DBProBasic3DDebug.dll")==NULL)
				{
					// DX used, so we make sure we have DirectX
					bBasic3DIsUsedSoWeNeedDirectXCheck = true;
					break;
				}
			}
		}
	}

	// leemove - 010306 - u60 - DirectX Check
	// leemovedagain - 221008 - u71 - moved to AFTER above check, as we only 
	// need to verify we have DirectX of ANY version if Basic3D employed
	bool bDirectXIsUpToDateFlag = false;
	if ( bBasic3DIsUsedSoWeNeedDirectXCheck==true )
	{
		// uses DirectX, so error if not up to date
		bDirectXIsUpToDateFlag = CheckIfGotLatestDirectX(false);
		if ( bDirectXIsUpToDateFlag==false )
			bResult=false;
	}
	else
	{
		// does not use DirectX, but we want to store the result so use silent detect
		bDirectXIsUpToDateFlag = CheckIfGotLatestDirectX(true);
	}

	// [EXE] Dependent DLL Linkage Info
	HINSTANCE hCoreDLL = NULL;

	// [EXE] Switch to TEMP Folder (that holds all exe-linked files)
	getcwd(m_pOriginalFolderName, _MAX_PATH);
	_chdir(m_pUnpackFolderName);

	// [EXE] Dynamically load all DLLs
	if(bResult==true)
	{
		if(bMiniInit==false) ZeroMemory(hDLLMod, sizeof(HINSTANCE)*256);
		if(bMiniInit==false) ZeroMemory(bDLLTPC, sizeof(bool)*256);
		for(DWORD dll=0; dll<m_dwNumberOfDLLs; dll++)
		{
			int dllindex=m_pDLLIndexArray[dll];
			if(dllindex<=255)
			{
				// Skip if already loaded
				if(m_pDLLLoadedAlreadyArray[dll]==0)
				{
					// Set flag to loaded
					m_pDLLLoadedAlreadyArray[dll]=1;

					// Load the DLL into memory
					LPSTR pDLLName = (LPSTR)m_pDLLFilenameArray[dll];
					if(stricmp(pDLLName, "EXE")==NULL)
					{
						// Module is part of the EXE (functionptrs passed into init above)
						hDLLMod[dllindex]=0;
					}
					else
					{
						// Module can be in several places
						LPSTR pTryDLLName = new char[(strlen(pDLLName)*2)+256];
						strcpy(pTryDLLName, pDLLName);

						// leeadd - 270308 - if DLL local to exe, switch to that folder for this DLL
						bool bSwitchedToLocalEXEfolder = false;
						_chdir(m_pOriginalFolderName);
						if(FileExists(pTryDLLName)==true)
							bSwitchedToLocalEXEfolder = true;
						else
							_chdir(m_pUnpackFolderName);

						// leefix - 120104 - DEBUG MODE - may use a plugins-licensed folder
						if(FileExists(pTryDLLName)==false)
						{
							strcpy(pTryDLLName, "..\\plugins-user\\");
							strcat(pTryDLLName, pDLLName);
						}
						if(FileExists(pTryDLLName)==false)
						{
							strcpy(pTryDLLName, "..\\plugins-licensed\\");
							strcat(pTryDLLName, pDLLName);
						}

						// Module is a DLL
						if(FileExists(pTryDLLName))
						{
							hDLLMod[dllindex]=LoadLibrary(pTryDLLName);
							if(hDLLMod[dllindex]==NULL)
							{
								if(*pReturnError==NULL) *pReturnError = new char[1024];
								wsprintf(*pReturnError,"Failed to load DLL (%d: %s)", dllindex, pTryDLLName);
								SAFE_DELETE(pTryDLLName);
								bResult=false;
								_chdir(m_pUnpackFolderName);//leeadd-270308-ifCWDswitched
								break;
							}
						}
						else
						{
							bool bIgnorableDLLs=false;
							if ( stricmp(pDLLName,"ConvX.dll")==0 ) bIgnorableDLLs=true;
							if ( stricmp(pDLLName,"Conv3DS.dll")==0 ) bIgnorableDLLs=true;
							if ( stricmp(pDLLName,"ConvMDL.dll")==0 ) bIgnorableDLLs=true;
							if ( stricmp(pDLLName,"ConvMD2.dll")==0 ) bIgnorableDLLs=true;
							if ( stricmp(pDLLName,"ConvMD3.dll")==0 ) bIgnorableDLLs=true;
							if ( bIgnorableDLLs )
							{
								// skips rest of nested code
								_chdir(m_pUnpackFolderName);//leeadd-270308-ifCWDswitched
								continue;
							}
							else
							{
								/* 260614 - silent fail - BulletRagdolLDemo tried to load MATRIX 20 DLL, but was never there!!
								if(*pReturnError==NULL) *pReturnError = new char[1024];
								wsprintf(*pReturnError,"Could not find a DLL (%s)", pTryDLLName);
								SAFE_DELETE(pTryDLLName);
								bResult=false;
								_chdir(m_pUnpackFolderName);//leeadd-270308-ifCWDswitched
								break;
								*/
							}
						}

						// leeadd - 270308 - if DLL local to exe, restore folder
						if ( bSwitchedToLocalEXEfolder==true ) _chdir(m_pUnpackFolderName);

						// Detect if DLL has the PassCoreData Function..
						DLL_PassCore g_DLL_PassCoreData;
						g_DLL_PassCoreData = ( DLL_PassCore ) GetProcAddress ( hDLLMod[dllindex], "?ReceiveCoreDataPtr@@YAXPAX@Z" );
						if (!g_DLL_PassCoreData)
							g_DLL_PassCoreData = ( DLL_PassCore ) GetProcAddress ( hDLLMod[dllindex], "ReceiveCoreDataPtr" );

						// Free usages
						SAFE_DELETE(pTryDLLName);
					}

					// Flag if official DLL
					bool bIsOfficialDLL=false;

					// Record Internal Functions from COREDLL
					if(stricmp(pDLLName,pCoreName)==0)
					{
						// CORE Inits
						hCoreDLL = hDLLMod[dllindex];

						// CORE Pass Ptrs To Core (Error Handling, Data Statements)
						g_CORE_PassCmdLinePtr			= ( GDI_RetVoidParamLPVOID )			GetProcAddress ( hCoreDLL, "?PassCmdLineHandlerPtr@@YAXPAX@Z" );
						g_CORE_PassErrorPtr				= ( GDI_RetVoidParamLPVOID )			GetProcAddress ( hCoreDLL, "?PassErrorHandlerPtr@@YAXPAX@Z" );
						g_CORE_PassEscapePtr			= ( GDI_RetVoidParamLPVOID )			GetProcAddress ( hCoreDLL, "?PassEscapePtr@@YAXPAX@Z" );
						g_CORE_PassBreakOutPtr			= ( GDI_RetVoidParamLPVOID )			GetProcAddress ( hCoreDLL, "?PassBreakOutPtr@@YAXPAX@Z" );
						g_CORE_PassStructurePatterns	= ( GDI_RetVoidParamLPVOIDDWORD )		GetProcAddress ( hCoreDLL, "?PassStructurePatterns@@YAXPAXK@Z" );
						g_CORE_PassDataPtrs				= ( GDI_RetVoidParamLPSTR2 )			GetProcAddress ( hCoreDLL, "?PassDataStatementPtr@@YAXPAD0@Z" );

						// CORE Display Function Calls
						g_CORE_InitDisplay		= ( GDI_RetDWORDParamDWORD4HINSTLPSTRPFN )	GetProcAddress ( hCoreDLL, "?InitDisplay@@YAKKKKKPAUHINSTANCE__@@PAD@Z" );
						g_CORE_CloseDisplay		= ( GDI_RetDWORDParamVoidPFN )			GetProcAddress ( hCoreDLL, "?CloseDisplay@@YAKXZ" );
						g_CORE_PassDLLs			= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?PassDLLs@@YAXXZ" );
						g_CORE_ConstructDLLs	= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?ConstructDLLs@@YAXXZ" );
						g_CORE_GetGlobPtr		= ( GDI_RetDWORDParamVoidPFN )			GetProcAddress ( hCoreDLL, "?GetGlobPtr@@YAKXZ" );

						// CORE Memory Management Function Calls
						g_CORE_CreateVarSpace	= ( GDI_RetDWORDParamDWORDPFN )			GetProcAddress ( hCoreDLL, "?CreateVariableSpace@@YAKK@Z" );
						g_CORE_DeleteVarSpace	= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?DeleteVariableSpace@@YAXXZ" );
						g_CORE_CreateDataSpace	= ( GDI_RetDWORDParamDWORDPFN )			GetProcAddress ( hCoreDLL, "?CreateDataSpace@@YAKK@Z" );
						g_CORE_DeleteDataSpace	= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?DeleteDataSpace@@YAXXZ" );
						g_CORE_DeleteVarItem	= ( GDI_RetVoidParamDWORDPTRPFN )		GetProcAddress ( hCoreDLL, "?DeleteSingleVariableAllocation@@YAXPAK@Z" );
						g_CORE_UnDim			= ( GDI_RetVoidParamDWORDPTRPFN )		GetProcAddress ( hCoreDLL, "?UnDimDD@@YAKK@Z" );
						g_CORE_SyncRefresh		= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?Sync@@YAXXZ" );
						
						// CORE SEcurity Functions
						g_CORE_GetSecurityCode	= ( GDI_RetIntParamVoidPFN )			GetProcAddress ( hCoreDLL, "?GetSecurityCode@@YAHXZ" );
						g_CORE_WipeSecurityCode	= ( GDI_RetVoidParamVoidPFN )			GetProcAddress ( hCoreDLL, "?WipeSecurityCode@@YAXXZ" );

						// Get GlobStruct Ptr for rest of DLL loading
						if(g_CORE_GetGlobPtr) g_pGlob = (GlobStruct*)g_CORE_GetGlobPtr();
						bIsOfficialDLL=true;
					}

					// Associated DLLs for Minimal DirectX Support
					if(stricmp(pDLLName,"DBProSetupDebug.dll")==0) { g_pGlob->g_GFX = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProBasic2DDebug.dll")==0) { g_pGlob->g_Basic2D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProTextDebug.dll")==0) { g_pGlob->g_Text = hDLLMod[dllindex]; bIsOfficialDLL=true; }

					// Transforms Support
					if(stricmp(pDLLName,"DBProTransformsDebug.dll")==0)
					{
						g_pGlob->g_Transforms = hDLLMod[dllindex];

						g_Transforms_Constructor	= ( DLL_Constructor )						GetProcAddress ( g_pGlob->g_Transforms, "Constructor" );
						g_Transforms_Destructor		= ( DLL_Destructor )						GetProcAddress ( g_pGlob->g_Transforms, "Destructor" );
						g_Transforms_Update			= ( DLL_Update )							GetProcAddress ( g_pGlob->g_Transforms, "Update" );

						bIsOfficialDLL=true;
					}

					// Sprite Support
					if(stricmp(pDLLName,"DBProSpritesDebug.dll")==0)
					{
						// SPRITES Inits
						g_pGlob->g_Sprites = hDLLMod[dllindex];

						// SPRITES Function Calls
						g_Sprites_Constructor       = ( SPRITES_RetVoidParamHINSTANCE2PFN )		GetProcAddress ( g_pGlob->g_Sprites, "?Constructor@@YAXPAUHINSTANCE__@@0@Z" );
						g_Sprites_Destructor        = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_pGlob->g_Sprites, "?Destructor@@YAXXZ" );
						g_Sprites_Update            = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_pGlob->g_Sprites, "?Update@@YAXXZ" );
						bIsOfficialDLL=true;
					}

					// Image Support
					if(stricmp(pDLLName,"DBProImageDebug.dll")==0)
					{
						// IMAGE Inits
						g_pGlob->g_Image = hDLLMod[dllindex];

						// IMAGE Function Calls
						g_Image_Constructor          = ( IMAGE_RetVoidParamVoidPFN )		GetProcAddress ( g_pGlob->g_Image, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
						g_Image_Destructor           = ( IMAGE_RetVoidParamVoidPFN )		GetProcAddress ( g_pGlob->g_Image, "?Destructor@@YAXXZ" );
						bIsOfficialDLL=true;
					}

					// Input Support
					if(stricmp(pDLLName,"DBProInputDebug.dll")==0)
					{
						g_pGlob->g_Input = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}

					// System Support
					if(stricmp(pDLLName,"DBProSystemDebug.dll")==0)
					{
						g_pGlob->g_System = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}	
					
					// Sound and Music Support
					if(stricmp(pDLLName,"DBProSoundDebug.dll")==0) { g_pGlob->g_Sound = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProMusicDebug.dll")==0) { g_pGlob->g_Music = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					
					// File Support
					if(stricmp(pDLLName,"DBProFileDebug.dll")==0)
					{
						g_pGlob->g_File = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}	
					
					// FTP Support
					if(stricmp(pDLLName,"DBProFTPDebug.dll")==0)
					{
						g_pGlob->g_FTP = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}			

					// Memblocks Support
					if(stricmp(pDLLName,"DBProMemblocksDebug.dll")==0)
					{
						g_pGlob->g_Memblocks = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}			

					// Animation Support
					if(stricmp(pDLLName,"DBProAnimationDebug.dll")==0)
					{
						g_pGlob->g_Animation = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}	
					
					// Bitmap Support
					if(stricmp(pDLLName,"DBProBitmapDebug.dll")==0)
					{
						g_pGlob->g_Bitmap = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}	

					// Multiplayer Support
					if(stricmp(pDLLName,"DBProMultiplayerDebug.dll")==0)
					{
						g_pGlob->g_Multiplayer = hDLLMod[dllindex];
						bIsOfficialDLL=true;
					}	

					// 3D System Support
					if(stricmp(pDLLName,"DBProCameraDebug.dll")==0) { g_pGlob->g_Camera3D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProLightDebug.dll")==0) { g_pGlob->g_Light3D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProMatrixDebug.dll")==0) { g_pGlob->g_Matrix3D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProBasic3DDebug.dll")==0) { g_pGlob->g_Basic3D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProWorld3DDebug.dll")==0) { g_pGlob->g_World3D = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProQ2BSPDebug.dll")==0) { g_pGlob->g_Q2BSP = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProOwnBSPDebug.dll")==0) { g_pGlob->g_OwnBSP = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProBSPCompilerDebug.dll")==0) { g_pGlob->g_BSPCompiler = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProParticlesDebug.dll")==0) { g_pGlob->g_Particles = hDLLMod[dllindex]; bIsOfficialDLL=true; }

					// Support DLLs
					if(stricmp(pDLLName,"DBProPrimObjectDebug.dll")==0) { g_pGlob->g_PrimObject = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProVectorsDebug.dll")==0) { g_pGlob->g_Vectors = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProLODTerrainDebug.dll")==0) { g_pGlob->g_LODTerrain = hDLLMod[dllindex]; bIsOfficialDLL=true; }
					if(stricmp(pDLLName,"DBProCSGDebug.dll")==0) { g_pGlob->g_CSG = hDLLMod[dllindex]; bIsOfficialDLL=true; }

					// leeadd - 1403060 - igl - DLLs supported by CORE and GLOBSTRUCT, auxiliary functions
					if(stricmp(pDLLName,"IGL.dll")==0) { g_pGlob->g_igLoader = hDLLMod[dllindex]; }

					// If none of these, must be TPC
					if ( bIsOfficialDLL==false )
						bDLLTPC[dllindex]=true;
				}
			}
		}
	}

	// [EXE] Copy vital data for all DLLs to access to Glob Structure
	if(g_pGlob)
	{
		memset ( g_pGlob->pEXEUnpackDirectory, 0, _MAX_PATH );
		strcpy(g_pGlob->pEXEUnpackDirectory, m_pUnpackFolderName);
		g_pGlob->ppEXEAbsFilename = (DWORD)m_pAbsoluteAppFile;
		g_pGlob->dwEncryptionUniqueKey = m_dwEncryptionKey;
	}

	// [EXE] Load Icons into Glob for use by Core
	if(g_pGlob)
	{
		g_pGlob->hAppIcon = LoadIcon(g_pGlob->hInstance, MAKEINTRESOURCE(IDI_ICON1));
	}

	// [EXE] Pass Pointers and Call DLLs..
	if(bResult==true)
	{
		// Always does this first, but can do later if CLI adds DX support
		bool bDoFullDisplayInitialisation=false;

		// First and subsequent passings
		if(bMiniInit==false)
		{
			// Prepare Display (GDI or EXT)
			g_CORE_PassCmdLinePtr((LPVOID)pCmdLine);
			g_CORE_PassErrorPtr((LPVOID)&m_dwRuntimeErrorDWORD);
			g_CORE_PassEscapePtr((LPVOID)&g_dwEscapeValueMem);
			g_CORE_PassBreakOutPtr((LPVOID)&g_dwBreakOutPosition);
			// U71 - also pass in structure pattern qty+ptr
			g_CORE_PassStructurePatterns((LPVOID)m_pUsertypeStringPatternArray, m_dwUsertypeStringPatternQuantity);
			g_CORE_PassDLLs();

			// 1ST : Get CORE CREATION Security Code
			int iSecurityCode = -1;
			if ( g_CORE_GetSecurityCode )
				iSecurityCode = g_CORE_GetSecurityCode();

			// Initialise each TPC DLL from the plugin-user folder 
			for(DWORD dll=0; dll<m_dwNumberOfDLLs; dll++)
			{
				int dllindex=m_pDLLIndexArray[dll];
				if(dllindex<=255 && bDLLTPC[dllindex]==true)
				{
					// Get Any DLL Function Pointers
					DLL_Constructor			g_DLL_Constructor;		// constructor
					g_DLL_Constructor		= ( DLL_Constructor )	GetProcAddress ( hDLLMod[dllindex], "?Constructor@@YAXXZ" );
					if (!g_DLL_Constructor)
						g_DLL_Constructor	= ( DLL_Constructor )	GetProcAddress ( hDLLMod[dllindex], "Constructor" );

					// Call TPC constructor functions (if any)
					if(g_DLL_Constructor)	g_DLL_Constructor();						

					// 2ND : TPC Sends special security code
					DLL_OptionalSecurityCode g_DLL_OptionalSecurityCode;
					g_DLL_OptionalSecurityCode = ( DLL_OptionalSecurityCode ) GetProcAddress ( hDLLMod[dllindex], "?OptionalSecurityCode@@YAXH@Z" );
					if (!g_DLL_OptionalSecurityCode)
						g_DLL_OptionalSecurityCode = (DLL_OptionalSecurityCode)GetProcAddress(hDLLMod[dllindex], "OptionalSecurityCode");
					if(g_DLL_OptionalSecurityCode)	g_DLL_OptionalSecurityCode ( iSecurityCode );

					// get num of additional dependencies
					int iNumDLLDependencies = 0;
					typedef int ( *RETINTNOPARAM ) ( void );
					RETINTNOPARAM GetNumDependencies = ( RETINTNOPARAM ) GetProcAddress ( hDLLMod[dllindex], "?GetNumDependencies@@YAHXZ" );
					if(!GetNumDependencies)
						GetNumDependencies = (RETINTNOPARAM)GetProcAddress(hDLLMod[dllindex], "GetNumDependencies");
					if ( GetNumDependencies ) iNumDLLDependencies=GetNumDependencies();

					// Obtain dependence and receive function pointers from DLL
					typedef void ( *RETVOIDLPSTRHINSTANCE ) ( LPSTR, HINSTANCE );
					RETVOIDLPSTRHINSTANCE ReceiveDependenceHinstance = ( RETVOIDLPSTRHINSTANCE ) GetProcAddress ( hDLLMod[dllindex], "?ReceiveDependenciesHinstance@@YAXPADPAUHINSTANCE__@@@Z" );
					if (!ReceiveDependenceHinstance)
						ReceiveDependenceHinstance = (RETVOIDLPSTRHINSTANCE)GetProcAddress(hDLLMod[dllindex], "ReceiveDependenciesHinstance");

					// If Dependency HINSTANCE passing exists, pass them now
					if ( iNumDLLDependencies > 0 )
					{
						for ( int iD=0; iD<iNumDLLDependencies; iD++ )
						{
							// get dependence name from ID
							char pDLLNameToFind [ 256 ];
							typedef const char * ( *RETLPSTRNOPARAM ) ( int n );
							RETLPSTRNOPARAM GetDependencyID = ( RETLPSTRNOPARAM ) GetProcAddress ( hDLLMod[dllindex], "?GetDependencyID@@YAPBDH@Z" );
							if (!GetDependencyID)
								GetDependencyID = (RETLPSTRNOPARAM)GetProcAddress(hDLLMod[dllindex], "GetDependencyID");
							strcpy ( pDLLNameToFind, GetDependencyID(iD) );

							// find hModuleFound of that dependence
							HINSTANCE hModuleFound = NULL;
							for ( DWORD findll=0; findll<m_dwNumberOfDLLs; findll++ )
							{
								LPSTR pFoundDLLName = (LPSTR)m_pDLLFilenameArray[findll];
								int founddllindex=m_pDLLIndexArray[findll];
								if(founddllindex<=255)
								{
									if ( stricmp( pFoundDLLName, pDLLNameToFind )==NULL )
									{
										hModuleFound = hDLLMod[founddllindex];
										break;
									}
								}
							}

							// send matching module hinstance back to DLL
							if ( hModuleFound && ReceiveDependenceHinstance )
								ReceiveDependenceHinstance ( pDLLNameToFind, hModuleFound );
						}
					}
				}
			}

			// leeadd - 280305 - copy module handles to globstruct (so setupDLL can call refreshD3Ds)
			// U58			(sizeof(HINSTANCE)*256)+(sizeof(bool)*256)
			// HINSTANCE	hDLLMod[256];
			// bool			bDLLTPC[256];
			if ( g_pGlob->pDynMemPtr==NULL )
			{
				g_pGlob->dwDynMemSize = (sizeof(HINSTANCE)*256)+(sizeof(bool)*256);
				g_pGlob->pDynMemPtr = new char [ g_pGlob->dwDynMemSize ];
				memcpy ( g_pGlob->pDynMemPtr+0, hDLLMod, (sizeof(HINSTANCE)*256) );
				memcpy ( g_pGlob->pDynMemPtr+(sizeof(HINSTANCE)*256), bDLLTPC, (sizeof(bool)*256) );
			}

			// 3RD : Wipe CORE CREATION Security Code
			if ( g_CORE_WipeSecurityCode ) g_CORE_WipeSecurityCode();

			// First time pass always init display
			bDoFullDisplayInitialisation=true;
		}
		else
		{
			// Pass DLLs that may have been loaded by miniCLI
			g_CORE_PassDLLs();

			// EXTDX Introduced, so init display now..
			if(g_hLastGFXPointer!=g_pGlob->g_GFX)
			{
				bDoFullDisplayInitialisation=true;
			}
		}

		// leeadd - 080306 - u60 - add DX version to globstruct
		g_pGlob->lpDirectXVersionString = (LPSTR)g_strDirectXVersion;

		// If full display initialisation required
		if(bDoFullDisplayInitialisation==true)
		{
			// leeadd - 070306 - u60 - istore igLoader HWND in glob struct so SETDISPLAYMODE in initdisplay can send the EMBED command
			if ( g_igLoader_HWND )
			{
				// going to embed to IGLoader
				g_pGlob->hwndIGLoader = g_igLoader_HWND;

				// superclass temp window for app-use (no second window!)
				g_pGlob->hWnd = g_hTempWindow;
			}
			else
			{
				// Remove temp window - now ready to create real window
				if(g_hTempWindow)
				{
					DestroyWindow(g_hTempWindow);
					g_hTempWindow=NULL;
				}
			}

			// Initialise Display (and DX check)
			if ( g_CORE_InitDisplay ( m_dwInitialDisplayMode, m_dwInitialDisplayWidth, m_dwInitialDisplayHeight, m_dwInitialDisplayDepth, hInstance, m_pInitialAppName)==1)
			{
				// Failed to DXSetup - Exit now
				bResult=false;
			}
		}

		// leefix - 090703 - TPC needs to call passcoredata 'after' initdisplay
		if(bResult==true)
		{
			if(bMiniInit==false)
			{
				for(DWORD dll=0; dll<m_dwNumberOfDLLs; dll++)
				{
					int dllindex=m_pDLLIndexArray[dll];
					if(dllindex<=255 && bDLLTPC[dllindex]==true)
					{
						DLL_PassCore g_DLL_PassCoreData;
						g_DLL_PassCoreData = ( DLL_PassCore ) GetProcAddress ( hDLLMod[dllindex], "?ReceiveCoreDataPtr@@YAXPAX@Z" );
						if (!g_DLL_PassCoreData)
							g_DLL_PassCoreData = ( DLL_PassCore )GetProcAddress(hDLLMod[dllindex], "ReceiveCoreDataPtr");
						if(g_DLL_PassCoreData)	g_DLL_PassCoreData( g_pGlob );
					}
				}
			}

			// Construct any new DLLs from miniCLI
			if(bMiniInit==true)
			{
				g_CORE_ConstructDLLs();
			}

			// Record DX ptr in case CLI introduces it later
			g_hLastGFXPointer=g_pGlob->g_GFX;
		}
	}

	// [EXE] Create Memory
	if(bMiniInit==false)
	{
		// [] Create Variable Space
		if(bResult==true)
		{
			if(m_dwVariableSpaceSize==0) m_dwVariableSpaceSize=1;
			m_pVariableSpace = (LPSTR)g_CORE_CreateVarSpace(m_dwVariableSpaceSize);
			ZeroMemory(m_pVariableSpace, m_dwVariableSpaceSize);
			m_dwOldVariableSpaceSize=m_dwVariableSpaceSize;
		}

		// [] Create Data Statement Space
		if(bResult==true)
		{
			if(m_dwDataSpaceSize>0)
			{
				m_pDataSpace = (LPSTR)g_CORE_CreateDataSpace(m_dwDataSpaceSize);
				memcpy(m_pDataSpace, m_pDataArray, m_dwDataSpaceSize);
				m_dwOldDataSpaceSize=m_dwDataSpaceSize;
				for(DWORD d=0; d<m_dwNumberOfDataItems; d++)
				{
					// Fill strings within data space using dynamic CExe creations
					if(m_pDataArray[(d*10)+0]==2)
					{
						char* pStr = (char*)m_pDataStringsArray[d];
						*(DWORD*)&m_pDataSpace[(d*10)+2] = (DWORD)pStr;
					}
				}
				g_CORE_PassDataPtrs(m_pDataSpace, m_pDataSpace+m_dwDataSpaceSize);
			}
		}
	}
	else
	{
		// [] Adjust Variable Space
		if(bResult==true)
		{
			if(m_dwVariableSpaceSize>0)
			{
				// Local Copy of vars
				LPSTR pTemp = new char[m_dwOldVariableSpaceSize];
				memcpy(pTemp, m_pVariableSpace, m_dwOldVariableSpaceSize);

				// Increase Size of VarSpace
				g_CORE_DeleteVarSpace();
				LPSTR pNew = (LPSTR)g_CORE_CreateVarSpace(m_dwVariableSpaceSize);
				memcpy(pNew, pTemp, m_dwOldVariableSpaceSize);
				m_dwOldVariableSpaceSize=m_dwVariableSpaceSize;
				m_pVariableSpace=pNew;

				// Free usages
				SAFE_DELETE(pTemp);
			}
		}

		// [] Create Data Statement Space
		if(bResult==true)
		{
			if(m_dwDataSpaceSize>0)
			{
				// Local Copy of data
				LPSTR pTemp = new char[m_dwOldDataSpaceSize];
				memcpy(pTemp, m_pDataSpace, m_dwOldDataSpaceSize);

				// Increase Size of DataSpace
				g_CORE_DeleteDataSpace();
				LPSTR pNew = (LPSTR)g_CORE_CreateDataSpace(m_dwDataSpaceSize);
				memcpy(pNew, pTemp, m_dwOldDataSpaceSize);
				m_dwOldDataSpaceSize=m_dwDataSpaceSize;
				m_pDataSpace=pNew;

				// Ensure Strings in DataSpace are handled (I guess...)
				for(DWORD d=0; d<m_dwNumberOfDataItems; d++)
				{
					// Fill strings within data space using dynamic CExe creations
					if(m_pDataArray[(d*10)+0]==2)
					{
						char* pStr = (char*)m_pDataStringsArray[d];
						*(DWORD*)&m_pDataSpace[(d*10)+2] = (DWORD)pStr;
					}
				}

				// Pass Updated DataSpace
				g_CORE_PassDataPtrs(m_pDataSpace, m_pDataSpace+m_dwDataSpaceSize);

				// Free usages
				SAFE_DELETE(pTemp);
			}
		}
	}

	// [EXE] Replace all XXXX Pointers with Dynamic Creations
	DWORD* pProgramRefPtr = NULL;
	if(bResult==true)
	{
		pProgramRefPtr = new DWORD[m_dwNumberOfReferences];
		for(DWORD ref=0; ref<m_dwNumberOfReferences; ref++)
		{
			int iRefType=(int)m_pRefTypeArray[ref];
			if(ref>=167)
			{
				ref=ref;
			}
			if(iRefType>0)
			{
				// lee, somehow world3d decname is being looked for in imageDLL...

				// Generate Pointers from Data
				DWORD index = m_pRefIndexArray[ref];
				if(iRefType==1)
				{
					// Command Address
					int dll=m_pCommandDLLIdArray[index];
					char* pStr = (char*)m_pCommandDLLCallArray[index];
					if(hDLLMod[dll]==0)
					{
						// Locate function ptr from EXE function ptr (passed in)
						if(stricmp(pStr, "DHookS")==NULL) *(pProgramRefPtr+ref)=(DWORD)pDHookS;
						if(stricmp(pStr, "DHookJ")==NULL) *(pProgramRefPtr+ref)=(DWORD)pDHookJ;
						if(stricmp(pStr, "DHookR")==NULL) *(pProgramRefPtr+ref)=(DWORD)pDHookR;
					}
					else
					{
						// Locate function ptr from DLL
						DWORD dwAdd = (DWORD)GetProcAddress(hDLLMod[dll], pStr);
						if(dwAdd!=NULL)
						{
							*(pProgramRefPtr+ref)=dwAdd;
						}
						else
						{
							// Exit loop
							g_bSuccessfulDLLLinks=false;
							ref=m_dwNumberOfReferences;
							if(*pReturnError==NULL) *pReturnError = new char[1024];
							int dlli = dll - 1; if(dlli<0) dlli=0;
							wsprintf(*pReturnError, "Could not find function '%s' in %d:%s", pStr, index, (LPSTR)m_pDLLFilenameArray[dlli]);
							bResult=false;
						}
					}
				}
				if(iRefType==2)
				{
					// String Address
					char* pStr = (char*)m_pStringsArray[index];
					DWORD dwAdd = (DWORD)pStr;
					if(dwAdd!=NULL)
						*(pProgramRefPtr+ref)=dwAdd;
					else
					{
						if(*pReturnError==NULL) *pReturnError = new char[1024];
						strcpy(*pReturnError,"Could not find dynamic string during referencing");
						bResult=false;
					}
				}
				if(iRefType==3)
				{
					// Second Variable (_ERR_)(bytes 4,5,6,7) is always substituted
					if(index==4)
					{
						// Pointer to Runtime Error DWORD (filled by DLLs)
						*(pProgramRefPtr+ref) = (DWORD)(&m_dwRuntimeErrorDWORD);
					}
					else
					{
						// Third Variable (_ESC_)(bytes 4,5,6,7) is always substituted
						if(index==8)
						{
							// Pointer to Runtime Escape Value DWORD (filled by DLLs)
							*(pProgramRefPtr+ref) = (DWORD)(&g_dwEscapeValueMem);
						}
						else
						{
							// Forth Variable (_REK_)(bytes 8,9,10,11) is always substituted
							if(index==12)
							{
								// Pointer to Runtime Escape Value DWORD (filled by DLLs)
								*(pProgramRefPtr+ref) = (DWORD)(&g_dwBreakOutPosition);
							}
							else
							{
								if(index==16)
								{
									// NEW Pointer to Runtime Error Line DWORD - now protection value
									*(pProgramRefPtr+ref) = (DWORD)(&m_dwRuntimeErrorLineDWORD);
								}
								else
								{
									// Variable + Plus Offset stored representing global var
									*(pProgramRefPtr+ref) = (DWORD)(m_pVariableSpace+index);
								}
							}
						}
					}
				}
				if(iRefType==4)
				{
					// Direct Immediate Value
					*(pProgramRefPtr+ref)=index;
				}
				if(iRefType==5)
				{
					// Label Jump Position is index(byte offset)
					*(pProgramRefPtr+ref)=index;
				}
				if(iRefType==6)
				{
					// Data Position is index(byte offset)
					*(pProgramRefPtr+ref)=(DWORD)(m_pDataSpace+(index*10));
				}
			}
		}

		// bSuccessfulDLLLinks ok now
		g_bSuccessfulDLLLinks=true;
	}

	// [EXE] Replace Tokens with Data in Byte Positions
	if(bResult==true)
	{
		for(DWORD ref=0; ref<m_dwNumberOfReferences; ref++)
		{
			DWORD dwDWORD=*(pProgramRefPtr+ref);
			DWORD dwBytePosition=m_pRefArray[ref];
			int iRefType=(int)m_pRefTypeArray[ref];
			if(iRefType==5)
			{
				// Adjust Label by making it a relative offset from 'here'
				DWORD dwInstructionPos=dwBytePosition+4;
				int iSigned = dwDWORD-dwInstructionPos;
				*(int*)((char*)m_pMachineCodeBlock+dwBytePosition)=iSigned;
			}
			else
			{
				*(DWORD*)((char*)m_pMachineCodeBlock+dwBytePosition)=dwDWORD;
			}
		}
	}

	// Disable EscapeKey (WHEN running debug mode)
	if(pDHookS)
	{
		if(g_pGlob)
		{
			g_pGlob->bEscapeKeyEnabled=false;
		}
	}

	// leeadd - 221008 - u71 - extra info in globstruct for external debuggers
	if ( g_pGlob ) g_pGlob->g_pMachineCodeBlock = (DWORD)m_pMachineCodeBlock;

	// Free usage
	SAFE_DELETE(pProgramRefPtr);

	// [EXE] Switch out of TEMP Folder
	_chdir(m_pOriginalFolderName);

	return bResult;
}

bool CEXEBlock::Run(bool bResult)
{
	// [EXE] Run Machine Code Program
	if (bResult != true)
		return bResult;

	// [EXE] Run Program
	if(!m_pMachineCodeBlock)
		return bResult;

	g_CORE_Program = ( GDI_RetVoidParamVoidPFN ) m_pMachineCodeBlock;
	g_CORE_Program();

	return bResult;
}

bool CEXEBlock::RunFrom(bool bResult, DWORD dwOffset)
{
	// [EXE] Run Machine Code Program
	if (bResult != true)
		return false;

	// [EXE] Run Program
	if(!m_pMachineCodeBlock)
		return bResult;

	g_CORE_Program = ( GDI_RetVoidParamVoidPFN ) ((LPSTR)m_pMachineCodeBlock + dwOffset);
	g_CORE_Program();

	return bResult;
}

void CEXEBlock::FreeUptoDisplay(void)
{
	// [EXE] Delete All Allocations Within Data Space
	if(m_pDataSpace)
	{
		// Data Item Format [Type][Reserved Byte][8byte for data]=10 bytes each
		for(DWORD dv=0; dv<m_dwDataSpaceSize; dv+=10)
		{
			if(m_pDataSpace[dv]==3)
			{
				DWORD* pMemoryAllocation = (DWORD*)((LPSTR)m_pDataSpace[dv]+2);
				g_CORE_DeleteVarItem(pMemoryAllocation);
			}
		}
	}

	// [EXE] Delete Data Space
	if(g_CORE_DeleteDataSpace) g_CORE_DeleteDataSpace();

	// [EXE] Delete All Allocations Within Var Space
	if(m_pVariableSpace)
	{
		// Scan variables for all dynamic allocations (use dynamicvaroffsetarray)
		for(DWORD dv=0; dv<m_dwDynamicVarsQuantity; dv++)
		{
			DWORD* pMemoryAllocation = (DWORD*)*((DWORD*)(m_pVariableSpace+m_pDynamicVarsArray[dv]));
			if(m_pDynamicVarsArrayType[dv]==1)
				g_CORE_UnDim(pMemoryAllocation);
			else
				g_CORE_DeleteVarItem(pMemoryAllocation);
		}
	}

	// [EXE] Delete Variable Space
	if(g_CORE_DeleteVarSpace) g_CORE_DeleteVarSpace();

	// [EXE] Close Display
	if(g_CORE_CloseDisplay) g_CORE_CloseDisplay();
}

void CEXEBlock::Free(void)
{
	// [CORE] Close any memory created for glob
	if ( g_pGlob ) if ( g_pGlob->pDynMemPtr ) SAFE_DELETE ( g_pGlob->pDynMemPtr );

	// [EXE] Free Up Data Allocations (from Load)
	Clear();

	// [EXE] Freeing Up
	if(!g_bSuccessfulDLLLinks)
		return;

	// leeadd - 010304 - predestructor for better memory management
	for(int dll=255; dll>=0; dll--)
	{
		if(bDLLTPC[dll]==true)
		{
			// Call TPC pre-destructor (if any)
			DLL_Destructor			g_DLL_Destructor;			
			g_DLL_Destructor		= ( DLL_Destructor )	GetProcAddress ( hDLLMod[dll], "?PreDestructor@@YAXXZ" );
			if (!g_DLL_Destructor)
				g_DLL_Destructor	= ( DLL_Destructor )	GetProcAddress ( hDLLMod[dll], "PreDestructor" );
			if(g_DLL_Destructor)	g_DLL_Destructor();
		}
	}
	for(dll=255; dll>=0; dll--)
	{
		if(bDLLTPC[dll]==true)
		{
			// Call TPC destructor (if any)
			DLL_Destructor			g_DLL_Destructor;
			g_DLL_Destructor		= ( DLL_Destructor )	GetProcAddress ( hDLLMod[dll], "?Destructor@@YAXXZ" );
			if (!g_DLL_Destructor)
				g_DLL_Destructor	= ( DLL_Destructor )	GetProcAddress ( hDLLMod[dll], "Destructor" );
			if(g_DLL_Destructor)	g_DLL_Destructor();
			bDLLTPC[dll]=false;
		}
		if(hDLLMod[dll])
		{
			FreeLibrary(hDLLMod[dll]);
			hDLLMod[dll]=NULL;
		}
	}
}
