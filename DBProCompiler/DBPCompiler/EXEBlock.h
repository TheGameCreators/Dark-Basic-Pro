// EXEBlock.h: interface for the CEXEBlock class.

// Includes used for DX check
#include <windows.h>
#include <stdio.h>

// Safe Macros
#include "macros.h"

// GDI CoreDLL Typedefs
typedef void				( *GDI_RetVoidParamLPVOID )				( LPVOID );
typedef void				( *GDI_RetVoidParamLPVOIDDWORD )		( LPVOID, DWORD );
typedef void				( *GDI_RetVoidParamLPVOID2 )			( LPVOID, LPVOID );
typedef void				( *GDI_RetVoidParamVoidPFN )			( void );
typedef void				( *GDI_RetVoidParamLPSTR2 )				( LPSTR, LPSTR );
typedef DWORD				( *GDI_RetDWORDParamDWORDHINSTLPPFN )	( DWORD, HINSTANCE );
typedef DWORD				( *GDI_RetDWORDParamDWORD3HINSTLPPFN )	( DWORD, DWORD, DWORD, HINSTANCE );
typedef DWORD				( *GDI_RetDWORDParamDWORD4HINSTLPPFN )	( DWORD, DWORD, DWORD, DWORD, HINSTANCE );
typedef DWORD				( *GDI_RetDWORDParamDWORD4HINSTLPSTRPFN )	( DWORD, DWORD, DWORD, DWORD, HINSTANCE, LPSTR );
typedef DWORD				( *GDI_RetDWORDParamPASSDLLS )			( HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE );
typedef DWORD				( *GDI_RetDWORDParamVoidPFN )			( void );
typedef DWORD				( *GDI_RetDWORDParamDWORDPFN )			( DWORD );
typedef void				( *GDI_RetVoidParamDWORDPTRPFN )		( DWORD* );
typedef int					( *GDI_RetIntParamVoidPFN )				( void );

// DLL Function Typdefs
typedef void				( *DLL_Constructor )					( void );
typedef void				( *DLL_Destructor )						( void );
typedef void 				( *DLL_PassCore )						( LPVOID );
typedef void				( *DLL_SetErrorHandle )					( LPVOID );
typedef void				( *DLL_OptionalSecurityCode )			( int );
typedef void				( *DLL_Update )							( void );

// Image Function Typdefs
typedef void				( *IMAGE_RetVoidParamVoidPFN )      ( void );
typedef void				( *IMAGE_RetVoidParamHINSTANCEPFN ) ( HINSTANCE );

// Image Function Pointers
extern IMAGE_RetVoidParamVoidPFN			g_Image_Constructor;				// constructor
extern IMAGE_RetVoidParamVoidPFN			g_Image_Destructor;					// destructor

// Transforms Function Pointers
extern DLL_Constructor						g_Transforms_Constructor;
extern DLL_Destructor						g_Transforms_Destructor;
extern DLL_Update							g_Transforms_Update;

// Sprite Function Typdefs
typedef void				( *SPRITES_RetVoidParamVoidPFN )      ( void );
typedef void                ( *SPRITES_RetVoidParamHINSTANCE2PFN )  ( HINSTANCE, HINSTANCE );

// Sprite Function Pointers
extern SPRITES_RetVoidParamHINSTANCE2PFN	g_Sprites_Constructor;
extern SPRITES_RetVoidParamVoidPFN			g_Sprites_Destructor;
extern SPRITES_RetVoidParamVoidPFN			g_Sprites_Update;

// EXEBlock Class Defenition
class CEXEBlock  
{
	public:
		CEXEBlock();
		virtual ~CEXEBlock();

		void			Clear(void);
		bool			StartInfo(LPSTR pUnpackFolderName, DWORD dwEncryptionKey);
		bool			Load(char* filename);
		bool			LoadValue(HANDLE hFile, DWORD* Value);
		bool			LoadBlock(HANDLE hFile, LPSTR* pMem, DWORD dwSize);
		bool			LoadValueArray(HANDLE hFile, DWORD** pArray, DWORD* Count);
		bool			LoadValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count, DWORD dwType);
		bool			LoadValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count);
		bool			LoadStringArray(HANDLE hFile, DWORD** pArray, DWORD* Count);
		bool			Save(char* filename);
		bool			SaveValue(HANDLE hFile, DWORD* Value);
		bool			SaveBlock(HANDLE hFile, LPSTR* pMem, DWORD dwSize);
		bool			SaveValueArray(HANDLE hFile, DWORD** pArray, DWORD* Count);
		bool			SaveValueArrayBytes(HANDLE hFile, DWORD** pArray, DWORD* Count);
		bool			SaveStringArray(HANDLE hFile, DWORD** pArray, DWORD* Count);
		DWORD*			CreateArray(DWORD dwCount,DWORD dwType);
		DWORD*			CreateArray(DWORD dwCount);
		bool			RecreateArray(DWORD** pArray, DWORD dwCount, DWORD NewCount);
		void			DeleteArrayContents(DWORD* pArray, DWORD dwCount);
		bool			FileExists(LPSTR pFilename);

		bool			CheckIfGotLatestDirectX ( bool bSilent );
		bool			Init(HINSTANCE hInstance, bool bResult, LPSTR* pReturnError, LPSTR pCmdLine);
		bool			InitDebug(HINSTANCE hInstance, LPVOID pDHookA, LPVOID pDHookB, LPVOID pDHookC, bool bResult, LPSTR* pReturnError, LPSTR pCmdLine, bool bMiniInit);
		bool			InitMini(LPVOID pDHookS, LPVOID pDHookJ, LPVOID pDHookR, bool bResult, LPSTR* pReturnError);
		bool			Run(bool bResult);
		bool			RunFrom(bool bResult, DWORD dwOffset);
		void			Free(void);
		void			FreeUptoDisplay(void);

	public:

		// General Flags
		bool			m_bEXEBlockPresent;

		// Executable Settings
		DWORD			m_dwInitialDisplayMode;
		DWORD			m_dwInitialDisplayWidth;
		DWORD			m_dwInitialDisplayHeight;
		DWORD			m_dwInitialDisplayDepth;
		LPSTR			m_pInitialAppName;
		LPSTR			m_pOriginalFolderName;
		LPSTR			m_pUnpackFolderName;
		LPSTR			m_pAbsoluteAppFile;
		DWORD			m_dwEncryptionKey;

		// DLL Data
		DWORD			m_dwNumberOfDLLs;
		DWORD*			m_pDLLIndexArray;
		DWORD*			m_pDLLFilenameArray;
		DWORD*			m_pDLLLoadedAlreadyArray;

		// MCB Reference Data
		DWORD			m_dwNumberOfReferences;
		DWORD*			m_pRefArray;
		DWORD*			m_pRefTypeArray;
		DWORD*			m_pRefIndexArray;

		// Runtime Error and Escape Value DWORDs
		DWORD			m_dwRuntimeErrorDWORD;
		DWORD			m_dwRuntimeErrorLineDWORD;
		DWORD			m_dwEscapeValueDWORD;

		// Runtime Error String Database
		DWORD			m_dwNumberOfRuntimeErrorStrings;
		DWORD*			m_pRuntimeErrorStringsArray;

		// Machine Code Block (MCB)
		DWORD			m_dwSizeOfMCB;
		DWORD*			m_pMachineCodeBlock;
		DWORD			m_dwStartOfMiniMC;

		// Commands Data
		DWORD			m_dwNumberOfCommands;
		DWORD*			m_pCommandDLLIdArray;
		DWORD*			m_pCommandDLLCallArray;

		// Strings Data
		DWORD			m_dwNumberOfStrings;
		DWORD*			m_pStringsArray;

		// Data Statements Data
		DWORD			m_dwNumberOfDataItems;
		LPSTR			m_pDataArray;
		DWORD*			m_pDataStringsArray;

		// Variable Space Data
		DWORD			m_dwOldVariableSpaceSize;
		DWORD			m_dwVariableSpaceSize;
		LPSTR			m_pVariableSpace;

		// Datat Statement Space Data
		DWORD			m_dwOldDataSpaceSize;
		DWORD			m_dwDataSpaceSize;
		LPSTR			m_pDataSpace;

		// Dynamic Variable Offset Array (points into VarSpace)
		DWORD			m_dwDynamicVarsQuantity;
		DWORD*			m_pDynamicVarsArray;
		DWORD*			m_pDynamicVarsArrayType;

		// New U71 - stores structure patterns
		DWORD			m_dwUsertypeStringPatternQuantity;
		LPSTR			m_pUsertypeStringPatternArray;
};
