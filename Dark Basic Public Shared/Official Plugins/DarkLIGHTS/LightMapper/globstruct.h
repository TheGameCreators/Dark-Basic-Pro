//
// Global Structure (data shared by DLLs)
//

// Requirement of VS2005
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)

// Glob structure define
#ifndef _GLOBSTRUCT_H_
#define _GLOBSTRUCT_H_

// Header to support texture ptrs
#include "dxdiag.h"
#include "D3dx9tex.h"

// Define compile mode
#define DBPRO_COMPILE 1

// Typdefs shared by all DLLs
typedef void ( *PTR_FuncCreateStr ) ( DWORD*, DWORD );
typedef DWORD ( *PTR_ProcessMessages ) ( void );
typedef void ( *PTR_PrintString ) ( LPSTR, bool );
typedef void ( *PTR_VOID ) ( void );
typedef void ( *PTR_DWORD ) ( DWORD );

// Data Structures
struct GlobChecklistStruct
{
	DWORD						dwStringSize;
	LPSTR						string;
	int							valuea;
	int							valueb;
	int							valuec;
	int							valued;
	float						fvaluea;
	float						fvalueb;
	float						fvaluec;
	float						fvalued;
};

struct GlobStruct
{
	// Function Ptrs (for remote DLLs)
	PTR_FuncCreateStr			CreateDeleteString;
	PTR_ProcessMessages			ProcessMessageFunction;
	PTR_PrintString				PrintStringFunction;
	PTR_DWORD					UpdateFilenameFromVirtualTable;
	PTR_DWORD					Decrypt;
	PTR_DWORD					Encrypt;
	PTR_DWORD					ChangeMouseFunction;
	PTR_DWORD					SpareFunction1;
	PTR_VOID					SpareFunction2;
	PTR_VOID					SpareFunction3;

	// Variable Memory Ptr
	LPVOID						g_pVariableSpace;

	// Error Handler Ptr
	LPVOID						g_pErrorHandlerRef;

	// DLL Handles and Active Flags
	HINSTANCE					g_GFX				;
	HINSTANCE					g_Text				;
	HINSTANCE					g_Basic2D			;
	HINSTANCE					g_Sprites			;
	HINSTANCE					g_Image				;
	HINSTANCE					g_Input				;
	HINSTANCE					g_System			;
	HINSTANCE					g_File				;
	HINSTANCE					g_FTP				;
	HINSTANCE					g_Memblocks			;
	HINSTANCE					g_Bitmap			;
	HINSTANCE					g_Animation			;
	HINSTANCE					g_Multiplayer		;
	HINSTANCE					g_Basic3D			;
	HINSTANCE					g_Camera3D			;
	HINSTANCE					g_Matrix3D			;
	HINSTANCE					g_Light3D			;
	HINSTANCE					g_World3D			;
	HINSTANCE					g_Particles			;
	HINSTANCE					g_PrimObject		;
	HINSTANCE					g_Vectors			;
	HINSTANCE					g_XObject			;
	HINSTANCE					g_3DSObject			;
	HINSTANCE					g_MDLObject			;
	HINSTANCE					g_MD2Object			;
	HINSTANCE					g_MD3Object			;
	HINSTANCE					g_Sound				;
	HINSTANCE					g_Music				;
	HINSTANCE					g_LODTerrain		;
	HINSTANCE					g_Q2BSP				;
	HINSTANCE					g_OwnBSP			;
	HINSTANCE					g_BSPCompiler		;
	HINSTANCE					g_CSG				;
	HINSTANCE					g_igLoader			;
	HINSTANCE					g_GameFX			;
	HINSTANCE					g_Spare03			;
	HINSTANCE					g_Spare04			;
	HINSTANCE					g_Spare05			;
	HINSTANCE					g_Spare06			;
	HINSTANCE					g_Spare07			;
	HINSTANCE					g_Spare08			;
	HINSTANCE					g_Spare09			;
	HINSTANCE					g_Spare10			;
	HINSTANCE					g_Spare11			;
	HINSTANCE					g_Spare12			;
	HINSTANCE					g_Spare13			;
	HINSTANCE					g_Spare14			;
	HINSTANCE					g_Spare15			;
	HINSTANCE					g_Spare16			;
	HINSTANCE					g_Spare17			;
	HINSTANCE					g_Spare18			;
	HINSTANCE					g_Spare19			;
	HINSTANCE					g_Spare20			;
	bool						g_GFXmade			;
	bool						g_Textmade			;
	bool						g_Basic2Dmade		;
	bool						g_Spritesmade		;
	bool						g_Imagemade			;
	bool						g_Inputmade			;
	bool						g_Systemmade		;
	bool						g_Filemade			;
	bool						g_FTPmade			;
	bool						g_Memblocksmade		;
	bool						g_Bitmapmade		;
	bool						g_Animationmade		;
	bool						g_Multiplayermade	;
	bool						g_Basic3Dmade		;
	bool						g_Camera3Dmade		;
	bool						g_Matrix3Dmade		;
	bool						g_Light3Dmade		;
	bool						g_World3Dmade		;
	bool						g_Particlesmade		;
	bool						g_PrimObjectmade	;
	bool						g_Vectorsmade		;
	bool						g_XObjectmade		;
	bool						g_3DSObjectmade		;
	bool						g_MDLObjectmade		;
	bool						g_MD2Objectmade		;
	bool						g_MD3Objectmade		;
	bool						g_Soundmade			;
	bool						g_Musicmade			;
	bool						g_LODTerrainmade	;
	bool						g_Q2BSPmade			;
	bool						g_OwnBSPmade		;
	bool						g_BSPCompilermade	;
	bool						g_CSGmade			;
	bool						g_igLoadermade		;
	bool						g_GameFXmade		;
	bool						g_Spare03made		;
	bool						g_Spare04made		;
	bool						g_Spare05made		;
	bool						g_Spare06made		;
	bool						g_Spare07made		;
	bool						g_Spare08made		;
	bool						g_Spare09made		;
	bool						g_Spare10made		;
	bool						g_Spare11made		;
	bool						g_Spare12made		;
	bool						g_Spare13made		;
	bool						g_Spare14made		;
	bool						g_Spare15made		;
	bool						g_Spare16made		;
	bool						g_Spare17made		;
	bool						g_Spare18made		;
	bool						g_Spare19made		;
	bool						g_Spare20made		;

	// Executable Media Handling Data
	char						pEXEUnpackDirectory[_MAX_PATH];
	DWORD						dwEncryptionUniqueKey;
	DWORD						ppEXEAbsFilename;
	DWORD						dwInternalFunctionCode;
	DWORD						g_pMachineCodeBlock;
	DWORD						dwEMHDSpare4;
	DWORD						dwEMHDSpare5;

	// Windows General Data
	HWND						hWnd;
	HINSTANCE					hInstance;
	LPSTR						pWindowsTextEntry;
	bool						bInvalidFlag;
	int							dwWindowWidth;
	int							dwWindowHeight;
	HICON						hAppIcon;
	DWORD						dwAppDisplayModeUsing;
	DWORD						dwWindowX;
	DWORD						dwWindowY;
	HWND						hwndIGLoader;
	DWORD						dwWGDSpare2;
	DWORD						dwWGDSpare3;
	DWORD						dwWGDSpare4;
	DWORD						dwWGDSpare5;

	// Windows Mouse Data
	bool						bWindowsMouseVisible;
	int							iWindowsMouseX;
	int							iWindowsMouseY;
	int							iWindowsMouseClick;
	DWORD						dwWMDSpare2;
	DWORD						dwWMDSpare3;
	DWORD						dwWMDSpare4;
	DWORD						dwWMDSpare5;

	// Main Screen Data (backbuffer)
	int							iScreenWidth;
	int							iScreenHeight;
	int							iScreenDepth;
	DWORD						iNoDrawLeft;
	DWORD						iNoDrawTop;
	DWORD						iNoDrawRight;
	DWORD						iNoDrawBottom;
	DWORD						dwSafeRectMax;
	RECT*						pSafeRects;
	DWORD						dwMSDSpare3;
	DWORD						dwMSDSpare4;
	DWORD						dwMSDSpare5;

	// Bitmap and Surface Data (for drawing offscreen)
	int							iCurrentBitmapNumber;
	LPDIRECT3DTEXTURE9			pCurrentBitmapTexture;
	LPDIRECT3DSURFACE9			pCurrentBitmapSurface;
	LPDIRECT3DSURFACE9			pHoldBackBufferPtr;
	LPDIRECT3DSURFACE9			pHoldDepthBufferPtr;
	DWORD						dwBSDSpare1;
	DWORD						dwBSDSpare2;
	DWORD						dwBSDSpare3;
	DWORD						dwBSDSpare4;
	DWORD						dwBSDSpare5;

	// Drawing Data
	int							iCursorX;
	int							iCursorY;
	DWORD						dwForeColor;
	DWORD						dwBackColor;
	DWORD						dwRenderCameraID;
	float						fReflectionPlaneX;
	float						fReflectionPlaneY;
	float						fReflectionPlaneZ;
	DWORD						dwCurrentSetCameraID;
	LPSTR						lpDirectXVersionString;
	DWORD						dw3DBackColor;
	DWORD						dwDDSpare4;
	DWORD						dwDDSpare5;

	// Checklist Data
	bool						checklistexists;
	bool						checklisthasvalues;
	bool						checklisthasstrings;
	int							checklistqty;
	DWORD						dwChecklistArraySize;
	GlobChecklistStruct*		checklist;

	// Dependent 3D Data Exchange
	int							iFogState;
	DWORD						dwRedrawPhase;
	DWORD						dwRedrawCount;
	DWORD						dwStencilMode;
	DWORD						dwStencilShadowCount;
	DWORD						dwStencilReflectionCount;
	DWORD						dwNumberOfPolygonsDrawn;
	DWORD						dwNumberOfPrimCalls;
	DWORD						dwStencilSpare3;
	DWORD						dwStencilSpare4;
	DWORD						dwStencilSpare5;

	// System States and Global Controls
	bool						bEscapeKeyEnabled;
	bool						bSystemKeyEnabled;
	bool						bSpareBool1;
	bool						bSpareBool2;
	bool						bSpareBool3;
	bool						bSpareBool4;
	bool						bSpareBool5;
	bool						bSpareBool6;
	bool						bSpareBool7;
	bool						bSpareBool8;
	bool						bSpareBool9;
	LPSTR						pExitPromptString;
	LPSTR						pExitPromptString2;
	int							iSoftwareVP;

	// Dynamic Memory Area for future expansion
	DWORD						dwDynMemSize;
	LPSTR						pDynMemPtr;
};

// External Reference to Glob Pointer
extern GlobStruct*				g_pGlob;

// Functions shared by all DLLs to act on Shared Core Data
extern void GlobExpandChecklist( DWORD iIndex, DWORD dwImminentStringSize );

// Glob structure define ends
#endif _GLOBSTRUCT_H_
