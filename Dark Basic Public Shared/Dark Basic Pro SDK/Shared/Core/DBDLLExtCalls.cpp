//
// DBDLLExtCalls Function
//

// Common Includes
#include "DBDLLCore.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKText.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKInput.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKFile.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic2D.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBitmap.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSound.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMusic.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSprites.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKAnimation.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKLight.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMatrix.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKWorld.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKParticles.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKFTP.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMemblocks.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMultiplayer.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSystem.h"

	int g_iDarkGameSDKQuit = 0;
#endif

#include "RenderList.h"


// Global External Data
extern GlobStruct g_Glob;



// mike - 100405
#define DBPRO_GLOBAL 

// Global External Display Function Pointers
DBPRO_GLOBAL GFX_RetBoolParamVoidPFN				g_GFX_Constructor;					// constructor
DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_GFX_Destructor;					// destructor
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_GFX_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreData2PFN					g_GFX_PassCoreData;
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN				g_GFX_GetDirect3D;

DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_GFX_Begin;						// begin scene ( for compiler only )
DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_GFX_End;							// end scene ( for compiler only )
DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_GFX_Render;						// render finished scene ( for compiler only )
DBPRO_GLOBAL GFX_RetVoidParamHWNDPFN				g_GFX_OverrideHWND;					// Use Actual HWND

DBPRO_GLOBAL GFX_RetBoolParamVoidPFN				g_GFX_SetDisplayDebugMode;			// set debug display mode ( shows a dialog box at start of application to control settings )
DBPRO_GLOBAL GFX_RetBoolParamInt2PFN 				g_GFX_SetDisplayMode1;				// set display ( iWidth, iHeight )
DBPRO_GLOBAL GFX_RetBoolParamInt3PFN				g_GFX_SetDisplayMode2;				// set display ( iWidth, iHeight, iDepth )
DBPRO_GLOBAL GFX_RetBoolParamInt4PFN				g_GFX_SetDisplayMode3;				// set display ( iWidth, iHeight, iDepth, iMode )
DBPRO_GLOBAL GFX_RetBoolParamInt5PFN				g_GFX_SetDisplayMode4;				// set display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing )
DBPRO_GLOBAL GFX_RetBoolParamInt6PFN				g_GFX_SetDisplayMode5;				// set display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing, iLockable )
DBPRO_GLOBAL GFX_RestoreDisplayMode					g_GFX_RestoreDisplayMode;

DBPRO_GLOBAL GFX_RetVoidParamInt3PFN				g_GFX_Clear;						// clear screen to specified colour
DBPRO_GLOBAL GFX_RetVoidParamInt3PFN				g_GFX_SetGamma;						// set gamma of screen
DBPRO_GLOBAL GFX_RetVoidParamIntP3PFN				g_GFX_GetGamma;						// get gamma of screen

DBPRO_GLOBAL GFX_RetBoolParamInt2PFN				g_GFX_CheckDisplayMode1;			// check display ( iWidth, iHeight )
DBPRO_GLOBAL GFX_RetBoolParamInt3PFN				g_GFX_CheckDisplayMode2;			// check display ( iWidth, iHeight, iDepth )
DBPRO_GLOBAL GFX_RetBoolParamInt4PFN				g_GFX_CheckDisplayMode3;			// check display ( iWidth, iHeight, iDepth, iMode )
DBPRO_GLOBAL GFX_RetBoolParamInt5PFN				g_GFX_CheckDisplayMode4;			// check display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing )

DBPRO_GLOBAL GFX_RetBoolParamVoidPFN				g_GFX_GetDisplayType;				// get display type - software or hardware
DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetDisplayWidth;				// get display width
DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetDisplayHeight;				// get display height
DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetDisplayDepth;				// get display depth
DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetDisplayFPS;				// get display fps
DBPRO_GLOBAL GFX_RetBoolParamVoidPFN				g_GFX_GetWindowedMode;				// get windowed mode

DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetNumberOfDisplayModes;		// get number of display modes for adapter
DBPRO_GLOBAL GFX_RetPCharParamIntPFN				g_GFX_GetDisplayMode;				// get the specified display mode

DBPRO_GLOBAL GFX_RetIntParamVoidPFN					g_GFX_GetNumberOfDisplayDevices;	// get the number of graphics cards installed
DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetDisplayDevice;				// select which device to use
DBPRO_GLOBAL GFX_RetPCharParamIntPFN				g_GFX_GetDeviceName;				// get name of device
DBPRO_GLOBAL GFX_RetPCharParamIntPFN				g_GFX_GetDeviceDriverName;			// useful for debugging purposes

DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetDitherMode;				// set dithering on / off
DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetShadeMode;					// set shading to flat or gouraud
DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetLightMode;					// set lighting on / off, can be overrided by individual objects
DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetCullMode;					// set cull mode to none, cw, or ccw, can be overrided by individual objects
DBPRO_GLOBAL GFX_RetVoidParamIntPFN					g_GFX_SetSpecularMode;				// set specular highlighs on / off
DBPRO_GLOBAL GFX_RetVoidParamInt2PFN				g_GFX_SetRenderState;				// set the render state as in D3D, can be overrided by individual objects

// Global External Text Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Text_Constructor;
DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_Text_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Text_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Text_PassCoreData;

DBPRO_GLOBAL TEXT_VoidInt4PFN						g_Text_SetColor;
DBPRO_GLOBAL TEXT_Int2PFN							g_Text_SetCursor;
DBPRO_GLOBAL TEXT_Char1PFN							g_Text_Print;
DBPRO_GLOBAL TEXT_Int2Char1PFN						g_Text_Text;
DBPRO_GLOBAL TEXT_Int2Char1PFNEx					g_Text_TextEx;
DBPRO_GLOBAL TEXT_Int2Char1PFN						g_Text_CenterText;
DBPRO_GLOBAL TEXT_Char1PFN							g_Text_SetFont;
DBPRO_GLOBAL TEXT_Char1Int1PFN						g_Text_SetCharacterSet;
DBPRO_GLOBAL TEXT_Int1PFN							g_Text_SetSize;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToNormal;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToItalic;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToBold;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToBoldItalic;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToOpaque;
DBPRO_GLOBAL TEXT_VoidPFN							g_Text_SetToTransparent;
DBPRO_GLOBAL TEXT_retInt1Char1PFN					g_Text_asc;
DBPRO_GLOBAL TEXT_retInt1Char1PFN					g_Text_val;
DBPRO_GLOBAL TEXT_retChar1Int1PFN					g_Text_bin;
DBPRO_GLOBAL TEXT_retCharn1PFN						g_Text_chr;
DBPRO_GLOBAL TEXT_retChar1Int1PFN					g_Text_hex;
DBPRO_GLOBAL TEXT_retInt1Char1PFN					g_Text_len;
DBPRO_GLOBAL TEXT_retChar1Char1Int1PFN				g_Text_left;
DBPRO_GLOBAL TEXT_retChar1Char1Int1PFN				g_Text_mid;
DBPRO_GLOBAL TEXT_retChar1Char1Int1PFN				g_Text_right;
DBPRO_GLOBAL TEXT_retChar1Int1PFN					g_Text_str;
DBPRO_GLOBAL TEXT_retChar1Char1PFN					g_Text_lower;
DBPRO_GLOBAL TEXT_retChar1Char1PFN					g_Text_upper;
DBPRO_GLOBAL TEXT_retChar1PFN						g_Text_GetFont;
DBPRO_GLOBAL TEXT_retInta1PFN						g_Text_GetSize;
DBPRO_GLOBAL TEXT_retInt1PFN						g_Text_GetStyle;
DBPRO_GLOBAL TEXT_retInt1PFN						g_Text_GetBackgroundType;
DBPRO_GLOBAL TEXT_retInt1Char1PFN					g_Text_GetWidth;
DBPRO_GLOBAL TEXT_retInt1Char1PFN					g_Text_GetHeight;
DBPRO_GLOBAL TEXT_Char1PFN							g_Text_Text3D;

DBPRO_GLOBAL OBJECT_ScalePFN						g_Text_Scale;				// scale
DBPRO_GLOBAL OBJECT_PositionPFN						g_Text_Position;			// position
DBPRO_GLOBAL OBJECT_RotatePFN						g_Text_Rotate;				// rotation on all 3 axes
DBPRO_GLOBAL OBJECT_XRotatePFN						g_Text_XRotate;				// x rotation
DBPRO_GLOBAL OBJECT_YRotatePFN						g_Text_YRotate;				// y rotation
DBPRO_GLOBAL OBJECT_ZRotatePFN						g_Text_ZRotate;				// z rotation
DBPRO_GLOBAL OBJECT_PointPFN						g_Text_Point;				// point at a position
DBPRO_GLOBAL OBJECT_MovePFN							g_Text_Move;				// move in it's current direction
DBPRO_GLOBAL OBJECT_MoveLeftPFN						g_Text_MoveLeft;			// move left
DBPRO_GLOBAL OBJECT_MoveRightPFN					g_Text_MoveRight;			// move right
DBPRO_GLOBAL OBJECT_TurnLeftPFN						g_Text_TurnLeft;			// turn left
DBPRO_GLOBAL OBJECT_GetXPositionPFN					g_Text_GetXPosition;		// get x position
DBPRO_GLOBAL OBJECT_GetYPositionPFN					g_Text_GetYPosition;		// get y position
DBPRO_GLOBAL OBJECT_GetZPositionPFN					g_Text_GetZPosition;		// get z position
DBPRO_GLOBAL OBJECT_GetXSizePFN						g_Text_GetXSize;			// get x size
DBPRO_GLOBAL OBJECT_GetYSizePFN						g_Text_GetYSize;			// get y size
DBPRO_GLOBAL OBJECT_GetZSizePFN						g_Text_GetZSize;			// get z size
DBPRO_GLOBAL OBJECT_GetXRotationPFN					g_Text_GetXRotation;		// get x rotation
DBPRO_GLOBAL OBJECT_GetYRotationPFN					g_Text_GetYRotation;		// get y rotation
DBPRO_GLOBAL OBJECT_GetZRotationPFN					g_Text_GetZRotation;		// get z rotation

// Global External Basic2D Function Pointers


//DBPRO_GLOBAL BASIC2D_RetVoidParamVoidPFN			g_Basic2D_Constructor;
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Basic2D_Constructor;
DBPRO_GLOBAL BASIC2D_RetVoidParamVoidPFN			g_Basic2D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Basic2D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Basic2D_PassCoreData;
	
DBPRO_GLOBAL BASIC2D_RetVoidParamInt2PFN			g_Basic2D_Dot;
DBPRO_GLOBAL BASIC2D_RetVoidParamVoidPFN			g_Basic2D_Lock;
DBPRO_GLOBAL BASIC2D_RetVoidParamVoidPFN			g_Basic2D_Unlock;
DBPRO_GLOBAL BASIC2D_RetVoidParamInt4PFN			g_Basic2D_Box;
DBPRO_GLOBAL BASIC2D_RetVoidParamInt4PFN			g_Basic2D_Line;
DBPRO_GLOBAL BASIC2D_RetVoidParamInt3PFN			g_Basic2D_Circle;
DBPRO_GLOBAL BASIC2D_RetVoidParamInt6PFN			g_Basic2D_CopyArea;

// Global Exteral Sprites Function Pointers
DBPRO_GLOBAL SPRITES_RetVoidParamHINSTANCE2PFN		g_Sprites_Constructor;
DBPRO_GLOBAL SPRITES_RetVoidParamVoidPFN			g_Sprites_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Sprites_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Sprites_PassCoreData;
DBPRO_GLOBAL SPRITES_RetVoidParamVoidPFN			g_Sprites_Update;
DBPRO_GLOBAL SPRITES_RetVoidParamVoidPFN			g_Sprites_SaveBack;
DBPRO_GLOBAL SPRITES_RetVoidParamVoidPFN			g_Sprites_RestoreBack;

// Global Exteral Image Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Image_Constructor;				// constructor
DBPRO_GLOBAL IMAGE_RetVoidParamVoidPFN				g_Image_Destructor;					// destructor
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Image_SetErrorHandler;
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Image_PassSpriteInstance;		
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN				g_Image_SetMipmapNum;				// set number of mipmaps for when an image is created
DBPRO_GLOBAL IMAGE_RetVoidParamInt3PFN				g_Image_SetColorKey;				// set image color key
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Image_PassCoreData;

// Global External Input Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Input_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Input_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Input_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Input_PassCoreData;
DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_Input_ClearData;

// Global External Transforms Function Pointers
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Transforms_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Transforms_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Transforms_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Transforms_PassCoreData;

// Global External System Function Pointers
//PTR_RetVoidParamHINSTANCEPFN		g_System_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_System_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_System_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_System_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_System_PassCoreData;

// Global External Sound Function Pointers
DBPRO_GLOBAL GFX_RetVoidParamHWNDPFN				g_Sound_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Sound_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Sound_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Sound_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Sound_Update;

// Global External Music Function Pointers
DBPRO_GLOBAL GFX_RetVoidParamHWNDPFN				g_Music_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Music_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Music_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Music_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Music_Update;

// Global External File Function Pointers
#ifndef DARKSDK_COMPILE
	DBPRO_GLOBAL RetVoidFunctionPointerPFN  		g_File_Constructor;
	DBPRO_GLOBAL RetVoidFunctionPointerPFN			g_File_Destructor;
#else
	DBPRO_GLOBAL GFX_RetVoidParamVoidPFN			g_File_Constructor;
	DBPRO_GLOBAL GFX_RetVoidParamVoidPFN			g_File_Destructor;
#endif

DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_File_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_File_PassCoreData;

// Global External FTP Function Pointers
//PTR_RetVoidParamHINSTANCEPFN		g_FTP_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_FTP_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_FTP_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_FTP_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_FTP_PassCoreData;

// Global External Memblocks Function Pointers
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Memblocks_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Memblocks_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Memblocks_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Memblocks_PassCoreData;

// Global External Animation Function Pointers
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Animation_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Animation_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Animation_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Animation_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Animation_UpdateAnims;
DBPRO_GLOBAL RetVoidFunctionBoolPFN					g_Animation_PreventTextureLock;

// Global Exteral Bitmap Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Bitmap_Constructor;				// constructor
DBPRO_GLOBAL IMAGE_RetVoidParamVoidPFN				g_Bitmap_Destructor;					// destructor
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Bitmap_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Bitmap_PassCoreData;

// Global External Multiplayer Function Pointers
//PTR_RetVoidParamHINSTANCEPFN		g_Multiplayer_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Multiplayer_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Multiplayer_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Multiplayer_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Multiplayer_PassCoreData;

// Global External Camera Function Pointers
DBPRO_GLOBAL SPRITES_RetVoidParamHINSTANCE2PFN		g_Camera3D_Constructor;
//RetVoidFunctionPointerPFN			g_Camera3D_Destructor;
//DBPRO_GLOBAL GFX_RetVoidParamVoidPFN				g_Camera3D_Destructor;
RetVoidFunctionPointerPFN				g_Camera3D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Camera3D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Camera3D_PassCoreData;
//RetVoidFunctionPointerPFN			g_Camera3D_Update;
 RetVoidFunctionPointerPFN				g_Camera3D_Update;
//RetVoidFunctionPointerPFN			g_Camera3D_StartScene;
 RetVoidFunctionPointerPFN				g_Camera3D_StartScene;
 RetVoidFunctionPointerIntPFN				g_Camera3D_StartSceneInt;
//RetIntFunctionPointerPFN			g_Camera3D_FinishScene;
 RetIntFunctionPointerPFN					g_Camera3D_FinishScene;
 RetIntFunctionPointerBoolPFN					g_Camera3D_FinishSceneEx;
 RetIntFunctionPointerPFN					g_Camera3D_GetRenderCamera;
 RetVoidFunctionPointerIntPFN				g_Camera3D_RunCode;


// Global External Light Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Light3D_Constructor;
//RetVoidFunctionPointerPFN			g_Light3D_Destructor;
 RetVoidFunctionPointerPFN				g_Light3D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Light3D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Light3D_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Light3D_Update;

// Global External Matrix Function Pointers
DBPRO_GLOBAL SPRITES_RetVoidParamHINSTANCE2PFN		g_Matrix3D_Constructor;
//RetVoidFunctionPointerPFN			g_Matrix3D_Destructor;
 RetVoidFunctionPointerPFN				g_Matrix3D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Matrix3D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Matrix3D_PassCoreData;
//RetVoidFunctionPointerPFN			g_Matrix3D_Update;
 RetVoidFunctionPointerPFN				g_Matrix3D_Update;
//RetVoidFunctionPointerPFN			g_Matrix3D_LastUpdate;
 RetVoidFunctionPointerPFN				g_Matrix3D_LastUpdate;

// Global External Basic3D Function Pointers
DBPRO_GLOBAL BASIC3D_RetVoidParamHINSTANCE4PFN		g_Basic3D_Constructor;
//RetVoidFunctionPointerPFN			g_Basic3D_Destructor;
 RetVoidFunctionPointerPFN				g_Basic3D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Basic3D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Basic3D_PassCoreData;
//RetVoidFunctionPointerPFN			g_Basic3D_Update;
 RetVoidFunctionPointerPFN				g_Basic3D_UpdateViewProjForMotionBlur;
 RetVoidFunctionPointerPFN				g_Basic3D_Update;
 RetVoidFunctionPointerPFN				g_Basic3D_UpdateAnimationCycle;
 RetVoidFunctionPointerPFN				g_Basic3D_UpdateOnce;
//RetVoidFunctionPointerPFN			g_Basic3D_UpdateNoZDepth;
//RetVoidFunctionPointerPFN			g_Basic3D_StencilRenderStart;
//RetVoidFunctionPointerPFN			g_Basic3D_StencilRenderEnd;
 RetVoidFunctionPointerPFN				g_Basic3D_UpdateGhostLayer;
 RetVoidFunctionPointerPFN				g_Basic3D_UpdateNoZDepth;
 RetVoidFunctionPointerPFN				g_Basic3D_StencilRenderStart;
 RetVoidFunctionPointerPFN				g_Basic3D_StencilRenderEnd;
DBPRO_GLOBAL BASIC3D_RetVoidParamHINSTANCE6PFN		g_Basic3D_SendFormats;
//RetVoidFunctionPointerPFN			g_Basic3D_AutomaticCollisionStart;
//RetVoidFunctionPointerPFN			g_Basic3D_AutomaticCollisionEnd;
 RetVoidFunctionPointerPFN				g_Basic3D_AutomaticCollisionStart;
 RetVoidFunctionPointerPFN				g_Basic3D_AutomaticCollisionEnd;
 RetIntFunctionPointerIntPFN			g_Basic3D_RenderQuad;
 
// Global External World3D Function Pointers
DBPRO_GLOBAL WORLD3D_RetVoidParamHINSTANCE4PFN		g_World3D_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_World3D_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_World3D_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_World3D_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_World3D_Update;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_World3D_Start;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_World3D_End;

// Global External Q2BSP Function Pointers
DBPRO_GLOBAL WORLD3D_RetVoidParamHINSTANCE4PFN		g_Q2BSP_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Q2BSP_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Q2BSP_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Q2BSP_PassCoreData;

// Global External OwnBSP Function Pointers
DBPRO_GLOBAL WORLD3D_RetVoidParamHINSTANCE4PFN		g_OwnBSP_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_OwnBSP_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_OwnBSP_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_OwnBSP_PassCoreData;

// Global External BSPCompier Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_BSPCompiler_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_BSPCompiler_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_BSPCompiler_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_BSPCompiler_PassCoreData;

// Global External Vectors Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_Vectors_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Vectors_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Vectors_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Vectors_PassCoreData;

// Global External Particles Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCE2PFN			g_Particles_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Particles_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_Particles_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_Particles_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_Particles_Update;

// Global External LODTerrain Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCE3PFN			g_LODTerrain_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_LODTerrain_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_LODTerrain_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_LODTerrain_PassCoreData;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_LODTerrain_Update;

// Global External CSG Function Pointers
DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN			g_CSG_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN				g_CSG_Destructor;
DBPRO_GLOBAL PTR_RetVoidParamLPVOID					g_CSG_SetErrorHandler;
DBPRO_GLOBAL RetVoidFCoreDataPFN					g_CSG_PassCoreData;

// Setup Function
DARKSDK bool SetDBDLLExtCalls(void)
{
	#ifdef DARKSDK_COMPILE
	{
		// Global Shared Data With Other DLLs
		ZeroMemory(&g_Glob, sizeof(GlobStruct));
		
		// Fill Glob With Required Default Values
		g_Glob.bWindowsMouseVisible		= true;

		// Fill Glob With Required Default Values
		g_Glob.dwForeColor				= -1;//(white)
		g_Glob.dwBackColor				= 0;

		// Global Control Defaults
		g_Glob.bEscapeKeyEnabled		= true;

		// Must always have at least a one to create a pass
		g_Glob.dwRedrawCount=1;
		g_Glob.dwRedrawPhase=0;
	}
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX)
	{
		// Get Display Functions
		g_GFX_Constructor				= ( GFX_RetBoolParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?Constructor@@YA_NXZ" );
		g_GFX_Destructor				= ( GFX_RetVoidParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?Destructor@@YAXXZ" );
		g_GFX_SetErrorHandler			= ( PTR_RetVoidParamLPVOID )	   GetProcAddress ( g_Glob.g_GFX, "?SetErrorHandler@@YAXPAX@Z" );
		g_GFX_PassCoreData				= ( RetVoidFCoreData2PFN )		   GetProcAddress ( g_Glob.g_GFX, "?PassCoreData@@YAXPAXH@Z" );
		g_GFX_GetDirect3D				= ( GFX_GetDirect3DDevicePFN )	   GetProcAddress ( g_Glob.g_GFX, "?GetDirect3D@@YAPAUIDirect3D9@@XZ" );

		g_GFX_Begin						= ( GFX_RetVoidParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?Begin@@YAXXZ" );
		g_GFX_End						= ( GFX_RetVoidParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?End@@YAXXZ" );
		g_GFX_Render					= ( GFX_RetVoidParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?Render@@YAXXZ" );
		g_GFX_OverrideHWND				= ( GFX_RetVoidParamHWNDPFN )      GetProcAddress ( g_Glob.g_GFX, "?OverrideHWND@@YAXPAUHWND__@@@Z" );

		g_GFX_SetDisplayDebugMode		= ( GFX_RetBoolParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayDebugMode@@YA_NXZ" );
		g_GFX_SetDisplayMode1    		= ( GFX_RetBoolParamInt2PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayMode@@YA_NHH@Z" );
		g_GFX_SetDisplayMode2    		= ( GFX_RetBoolParamInt3PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayMode@@YA_NHHH@Z" );
		g_GFX_SetDisplayMode3    		= ( GFX_RetBoolParamInt4PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayMode@@YA_NHHHH@Z" );
		g_GFX_SetDisplayMode4    		= ( GFX_RetBoolParamInt5PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayMode@@YA_NHHHHH@Z" );
		g_GFX_SetDisplayMode5    		= ( GFX_RetBoolParamInt6PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDisplayMode@@YA_NHHHHHH@Z" );
		g_GFX_RestoreDisplayMode   		= ( GFX_RestoreDisplayMode  )      GetProcAddress ( g_Glob.g_GFX, "?RestoreDisplayMode@@YAXXZ" );

		g_GFX_Clear    					= ( GFX_RetVoidParamInt3PFN )      GetProcAddress ( g_Glob.g_GFX, "?Clear@@YAXHHH@Z" );
		g_GFX_SetGamma 					= ( GFX_RetVoidParamInt3PFN )      GetProcAddress ( g_Glob.g_GFX, "?SetGamma@@YAXHHH@Z" );
		g_GFX_GetGamma 					= ( GFX_RetVoidParamIntP3PFN )      GetProcAddress ( g_Glob.g_GFX, "?GetGamma@@YAXHHH@Z" );

		g_GFX_CheckDisplayMode1			= ( GFX_RetBoolParamInt2PFN )      GetProcAddress ( g_Glob.g_GFX, "?CheckDisplayMode@@YA_NHH@Z" );
		g_GFX_CheckDisplayMode2			= ( GFX_RetBoolParamInt3PFN )      GetProcAddress ( g_Glob.g_GFX, "?CheckDisplayMode@@YA_NHHH@Z" );
		g_GFX_CheckDisplayMode3			= ( GFX_RetBoolParamInt4PFN )      GetProcAddress ( g_Glob.g_GFX, "?CheckDisplayMode@@YA_NHHHH@Z" );
		g_GFX_CheckDisplayMode4			= ( GFX_RetBoolParamInt5PFN )      GetProcAddress ( g_Glob.g_GFX, "?CheckDisplayMode@@YA_NHHHHH@Z" );

		g_GFX_GetDisplayType			= ( GFX_RetBoolParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?GetDisplayType@@YA_NXZ" );
		g_GFX_GetDisplayWidth			= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetDisplayWidth@@YAHXZ" );
		g_GFX_GetDisplayHeight			= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetDisplayHeight@@YAHXZ" );
		g_GFX_GetDisplayDepth			= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetDisplayDepth@@YAHXZ" );
		g_GFX_GetDisplayFPS				= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetDisplayFPS@@YAHXZ" );
		g_GFX_GetWindowedMode			= ( GFX_RetBoolParamVoidPFN )      GetProcAddress ( g_Glob.g_GFX, "?GetWindowedMode@@YA_NXZ" );

		g_GFX_GetNumberOfDisplayModes	= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetNumberOfDisplayModes@@YAHXZ" );
		g_GFX_GetDisplayMode         	= ( GFX_RetPCharParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?GetDisplayMode@@YAPADH@Z" );

		g_GFX_GetNumberOfDisplayDevices	= ( GFX_RetIntParamVoidPFN )       GetProcAddress ( g_Glob.g_GFX, "?GetNumberOfDisplayDevices@@YAHXZ" );
		g_GFX_SetDisplayDevice	        = ( GFX_RetVoidParamIntPFN )       GetProcAddress ( g_Glob.g_GFX, "?SetDisplayDevice@@YAXH@Z" );
		g_GFX_GetDeviceName		        = ( GFX_RetPCharParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?GetDeviceName@@YAPADH@Z" );
		g_GFX_GetDeviceDriverName       = ( GFX_RetPCharParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?GetDeviceDriverName@@YAPADH@Z" );

		g_GFX_SetDitherMode		        = ( GFX_RetVoidParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetDitherMode@@YAXH@Z" );
		g_GFX_SetShadeMode		        = ( GFX_RetVoidParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetShadeMode@@YAXH@Z" );
		g_GFX_SetLightMode		        = ( GFX_RetVoidParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetLightMode@@YAXH@Z" );
		g_GFX_SetCullMode		        = ( GFX_RetVoidParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetCullMode@@YAXH@Z" );
		g_GFX_SetSpecularMode	        = ( GFX_RetVoidParamIntPFN )      GetProcAddress ( g_Glob.g_GFX, "?SetSpecularMode@@YAXH@Z" );
		g_GFX_SetRenderState	        = ( GFX_RetVoidParamInt2PFN )     GetProcAddress ( g_Glob.g_GFX, "?SetRenderState@@YAXHH@Z" );
	}
	#else
		g_GFX_Constructor				= ConstructorDisplay;
		g_GFX_Destructor				= DestructorDisplay;
		g_GFX_SetErrorHandler			= SetErrorHandlerDisplay;
		g_GFX_PassCoreData				= PassCoreDataDisplay;
		g_GFX_GetDirect3D				= dbGetDirect3DDevice;

		g_GFX_Begin						= dbBegin;
		g_GFX_End						= dbEnd;
		g_GFX_Render					= dbRender;
		g_GFX_OverrideHWND				= dbOverrideHWND;

		g_GFX_SetDisplayDebugMode		= dbSetDisplayDebugMode;
		g_GFX_SetDisplayMode1    		= dbSetDisplayModeEx;
		g_GFX_SetDisplayMode2    		= dbSetDisplayModeEx;
		g_GFX_SetDisplayMode3    		= dbSetDisplayModeEx;
		g_GFX_SetDisplayMode4    		= dbSetDisplayModeEx;
		g_GFX_SetDisplayMode5    		= dbSetDisplayModeEx;
		g_GFX_RestoreDisplayMode   		= dbRestoreDisplayMode;

		g_GFX_Clear    					= dbClear;
		g_GFX_SetGamma 					= dbSetGamma;
		g_GFX_GetGamma 					= dbGetGamma;

		g_GFX_GetDisplayWidth			= dbScreenWidth;
		g_GFX_GetDisplayHeight			= dbScreenHeight;
		g_GFX_GetDisplayDepth			= dbScreenDepth;
		//g_GFX_GetDisplayFPS				= dbGetDisplayFPS;
		g_GFX_GetWindowedMode			= dbGetWindowedMode;

		g_GFX_GetNumberOfDisplayModes	= dbGetNumberOfDisplayModes;

		g_GFX_GetNumberOfDisplayDevices	= dbGetNumberOfDisplayDevices;
		g_GFX_SetDisplayDevice	        = dbSetDisplayDevice;

		g_GFX_SetDitherMode		        = dbSetDitherMode;
		g_GFX_SetShadeMode		        = dbSetShadeMode;
		g_GFX_SetLightMode		        = dbSetLightMode;
		g_GFX_SetCullMode		        = dbSetCullMode;
		g_GFX_SetSpecularMode	        = dbSetSpecularMode;
		g_GFX_SetRenderState	        = dbSetRenderState;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Text)
	{
		// Get Text Functions
		g_Text_Constructor		    = ( PTR_RetVoidParamHINSTANCEPFN) GetProcAddress ( g_Glob.g_Text, "?TextConstructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Text_Destructor		    = ( GFX_RetVoidParamVoidPFN )	  GetProcAddress ( g_Glob.g_Text, "?TextDestructor@@YAXXZ" );
		g_Text_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )	  GetProcAddress ( g_Glob.g_Text, "?SetErrorHandler@@YAXPAX@Z" );
		g_Text_PassCoreData			= ( RetVoidFCoreDataPFN )		  GetProcAddress ( g_Glob.g_Text, "?PassCoreData@@YAXPAX@Z" );

		g_Text_SetColor		        = ( TEXT_VoidInt4PFN )			 GetProcAddress ( g_Glob.g_Text, "?SetTextColor@@YAXHHHH@Z" );
		g_Text_Text					= ( TEXT_Int2Char1PFN )			 GetProcAddress ( g_Glob.g_Text, "?BasicText@@YAXHHK@Z" );
		g_Text_SetCharacterSet		= ( TEXT_Char1Int1PFN )			 GetProcAddress ( g_Glob.g_Text, "?SetTextFont@@YAXPADH@Z" );
		g_Text_GetWidth				= ( TEXT_retInt1Char1PFN )		 GetProcAddress ( g_Glob.g_Text, "?GetTextWidth@@YAHPAD@Z" );
		g_Text_GetHeight			= ( TEXT_retInt1Char1PFN )		 GetProcAddress ( g_Glob.g_Text, "?GetTextHeight@@YAHPAD@Z" );
	}
	#else
		// mike - 020206 - addition for vs8
		//g_Text_Constructor			= ConstructorText;
		g_Text_Constructor			= ( PTR_RetVoidParamHINSTANCEPFN ) ConstructorText;
		g_Text_Destructor			= DestructorText;
		g_Text_SetErrorHandler		= SetErrorHandlerText;
		g_Text_PassCoreData			= PassCoreDataText;

		g_Text_SetColor				= dbSetTextColor;
		g_Text_Text					= dbText;
		g_Text_SetCharacterSet		= dbSetTextFont;
		g_Text_GetWidth				= dbGetTextWidth;
		g_Text_GetHeight			= dbGetTextHeight;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Basic2D)
	{
		// Get Basic2D Functions
		g_Basic2D_Constructor   = ( PTR_RetVoidParamHINSTANCEPFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Basic2D_Destructor	= ( BASIC2D_RetVoidParamVoidPFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Destructor@@YAXXZ" );
		g_Basic2D_SetErrorHandler=( PTR_RetVoidParamLPVOID )			GetProcAddress ( g_Glob.g_Basic2D, "?SetErrorHandler@@YAXPAX@Z" );
		g_Basic2D_PassCoreData	= ( RetVoidFCoreDataPFN )				GetProcAddress ( g_Glob.g_Basic2D, "?PassCoreData@@YAXPAX@Z" );

		g_Basic2D_Dot           = ( BASIC2D_RetVoidParamInt2PFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Dot@@YAXHH@Z" );
		g_Basic2D_Lock          = ( BASIC2D_RetVoidParamVoidPFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Lock@@YAXXZ" );
		g_Basic2D_Unlock        = ( BASIC2D_RetVoidParamVoidPFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Unlock@@YAXXZ" );
		g_Basic2D_Box           = ( BASIC2D_RetVoidParamInt4PFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Box@@YAXHHHH@Z" );
		g_Basic2D_Line          = ( BASIC2D_RetVoidParamInt4PFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Line@@YAXHHHH@Z" );
		g_Basic2D_Circle        = ( BASIC2D_RetVoidParamInt3PFN )		GetProcAddress ( g_Glob.g_Basic2D, "?Circle@@YAXHHH@Z" );

		g_Basic2D_CopyArea      = ( BASIC2D_RetVoidParamInt6PFN )		GetProcAddress ( g_Glob.g_Basic2D, "?CopyArea@@YAXHHHHHH@Z" );
	}
	#else
		// mike - 020206 - addition for vs8
		//g_Basic2D_Constructor		= ConstructorBasic2D;
		g_Basic2D_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN ) ConstructorBasic2D;
		g_Basic2D_Destructor		= DestructorBasic2D;
		g_Basic2D_SetErrorHandler	= SetErrorHandlerBasic2D;
		g_Basic2D_PassCoreData		= PassCoreDataBasic2D;

		g_Basic2D_Dot				= dbDot;
		g_Basic2D_Lock				= dbLockPixels;
		g_Basic2D_Unlock			= dbUnlockPixels;
		g_Basic2D_Box				= dbBox;
		g_Basic2D_Line				= dbLine;
		g_Basic2D_Circle			= dbCircle;

		g_Basic2D_CopyArea			= dbCopyArea;
	#endif
	

	// Sprite Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Sprites)
	{
		g_Sprites_Constructor       = ( SPRITES_RetVoidParamHINSTANCE2PFN )		GetProcAddress ( g_Glob.g_Sprites, "?Constructor@@YAXPAUHINSTANCE__@@0@Z" );
		g_Sprites_Destructor        = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_Glob.g_Sprites, "?Destructor@@YAXXZ" );
		g_Sprites_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Sprites, "?SetErrorHandler@@YAXPAX@Z" );
		g_Sprites_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Sprites, "?PassCoreData@@YAXPAX@Z" );
		g_Sprites_Update            = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_Glob.g_Sprites, "?Update@@YAXXZ" );
		g_Sprites_SaveBack          = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_Glob.g_Sprites, "?SaveBack@@YAXXZ" );
		g_Sprites_RestoreBack       = ( SPRITES_RetVoidParamVoidPFN )			GetProcAddress ( g_Glob.g_Sprites, "?RestoreBack@@YAXXZ" );
	}
	#else
		g_Sprites_Constructor       = ConstructorSprites;
		g_Sprites_Destructor        = DestructorSprites;
		g_Sprites_SetErrorHandler	= SetErrorHandlerSprites;
		g_Sprites_PassCoreData		= PassCoreDataSprites;
		g_Sprites_Update            = UpdateSprites;
		g_Sprites_SaveBack          = dbSaveSpriteBack;
		g_Sprites_RestoreBack       = dbRestoreSpriteBack;
	#endif
	

	// Image Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Image)
	{
		g_Image_Constructor          = ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Image, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Image_Destructor           = ( IMAGE_RetVoidParamVoidPFN )			GetProcAddress ( g_Glob.g_Image, "?Destructor@@YAXXZ" );
		g_Image_SetErrorHandler		 = ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Image, "?SetErrorHandler@@YAXPAX@Z" );
		g_Image_PassSpriteInstance	 = ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Image, "?PassSpriteInstance@@YAXPAUHINSTANCE__@@@Z" );
		g_Image_SetMipmapNum         = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( g_Glob.g_Image, "?SetMipmapNum@@YAXH@Z" );
		g_Image_SetColorKey          = ( IMAGE_RetVoidParamInt3PFN )			GetProcAddress ( g_Glob.g_Image, "?SetColorKey@@YAXHHH@Z" );
		g_Image_PassCoreData		 = ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Image, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		// mike - 020206 - addition for vs8
		//g_Image_Constructor          = ConstructorImage;
		g_Image_Constructor          = ( PTR_RetVoidParamHINSTANCEPFN ) ConstructorImage;
		g_Image_Destructor           = DestructorImage;
		g_Image_SetErrorHandler		 = SetErrorHandlerImage;
		g_Image_PassSpriteInstance	 = PassSpriteInstanceImage;
		g_Image_SetMipmapNum         = dbSetImageMipmapNum;
		g_Image_SetColorKey          = dbSetImageColorKey;
		g_Image_PassCoreData		 = PassCoreDataImage;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Input)
	{
		// Input Functions
		g_Input_Constructor			= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Input, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Input_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Input, "?Destructor@@YAXXZ" );
		g_Input_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Input, "?SetErrorHandler@@YAXPAX@Z" );
		g_Input_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Input, "?PassCoreData@@YAXPAX@Z" );
		g_Input_ClearData			= ( GFX_RetVoidParamVoidPFN )				GetProcAddress ( g_Glob.g_Input, "?ClearData@@YAXXZ" );
	}
	#else
		g_Input_Constructor			= ConstructorInput;
		g_Input_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorInput;
		g_Input_SetErrorHandler		= SetErrorHandlerInput;
		g_Input_PassCoreData		= PassCoreDataInput;
		g_Input_ClearData			= dbClearDataInput;
	#endif

	// Transforms Functions
	#ifndef DARKSDK_COMPILE
		g_Transforms_Constructor	= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Transforms, "Constructor" );
		g_Transforms_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Transforms, "Destructor" );
		g_Transforms_SetErrorHandler= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Transforms, "SetErrorHandler" );
		g_Transforms_PassCoreData	= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Transforms, "PassCoreData" );
	#else
		g_Transforms_Constructor	= ConstructorTransforms;
		g_Transforms_Destructor		= DestructorTransforms;
		g_Transforms_SetErrorHandler= SetErrorHandlerTransforms;
		g_Transforms_PassCoreData	= PassCoreDataTransforms;
	#endif

	// System Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_System)
	{
		g_System_Constructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_System, "?Constructor@@YAXXZ" );
		g_System_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_System, "?Destructor@@YAXXZ" );
		g_System_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_System, "?SetErrorHandler@@YAXPAX@Z" );
		g_System_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_System, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		g_System_Constructor		= ( RetVoidFunctionPointerPFN ) ConstructorSystem;
		g_System_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorSystem;
		g_System_SetErrorHandler	= SetErrorHandlerSystem;
		g_System_PassCoreData		= PassCoreDataSystem;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Sound)
	{
		// Sound Functions
		g_Sound_Constructor			= ( GFX_RetVoidParamHWNDPFN )				GetProcAddress ( g_Glob.g_Sound, "?Constructor@@YAXPAUHWND__@@@Z" );
		g_Sound_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Sound, "?Destructor@@YAXXZ" );
		g_Sound_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Sound, "?SetErrorHandler@@YAXPAX@Z" );
		g_Sound_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Sound, "?PassCoreData@@YAXPAX@Z" );
		g_Sound_Update				= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Sound, "?Update@@YAXXZ" );
	}
	#else
		g_Sound_Constructor			= ConstructorSound;
		g_Sound_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorSound;
		g_Sound_SetErrorHandler		= SetErrorHandlerSound;
		g_Sound_PassCoreData		= PassCoreDataSound;
		g_Sound_Update				= ( RetVoidFunctionPointerPFN ) UpdateSound;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Music)
	{
		// Music Functions
		g_Music_Constructor			= ( GFX_RetVoidParamHWNDPFN )				GetProcAddress ( g_Glob.g_Music, "?Constructor@@YAXPAUHWND__@@@Z" );
		g_Music_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Music, "?Destructor@@YAXXZ" );
		g_Music_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Music, "?SetErrorHandler@@YAXPAX@Z" );
		g_Music_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Music, "?PassCoreData@@YAXPAX@Z" );
		g_Music_Update				= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Music, "?Update@@YAXXZ" );
	}
	#else
		g_Music_Constructor			= ConstructorMusic;
		g_Music_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorMusic;
		g_Music_SetErrorHandler		= SetErrorHandlerMusic;
		g_Music_PassCoreData		= PassCoreDataMusic;
		g_Music_Update				= ( RetVoidFunctionPointerPFN ) UpdateMusic;
	#endif

	// File Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_File)
	{
		g_File_Constructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_File, "?Constructor@@YAXXZ" );
		g_File_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_File, "?Destructor@@YAXXZ" );
		g_File_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_File, "?SetErrorHandler@@YAXPAX@Z" );
		g_File_PassCoreData			= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_File, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		//g_File_Constructor			= ( RetVoidFunctionPointerPFN ) ConstructorFile;
		g_File_Constructor			=  ConstructorFile;
		g_File_Destructor			= DestructorFile;
		g_File_SetErrorHandler		= SetErrorHandlerFile;
		g_File_PassCoreData			= PassCoreDataFile;
	#endif

	// FTP Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_FTP)
	{
		g_FTP_Constructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_FTP, "?Constructor@@YAXXZ" );
		g_FTP_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_FTP, "?Destructor@@YAXXZ" );
		g_FTP_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_FTP, "?SetErrorHandler@@YAXPAX@Z" );
		g_FTP_PassCoreData			= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_FTP, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		g_FTP_Constructor			= ( RetVoidFunctionPointerPFN ) ConstructorFTP;
		g_FTP_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorFTP;
		g_FTP_SetErrorHandler		= SetErrorHandlerFTP;
		g_FTP_PassCoreData			= PassCoreDataFTP;
	#endif

	// Get Memblocks Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Memblocks)
	{
		g_Memblocks_Constructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Memblocks, "?Constructor@@YAXXZ" );
		g_Memblocks_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Memblocks, "?Destructor@@YAXXZ" );
		g_Memblocks_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Memblocks, "?SetErrorHandler@@YAXPAX@Z" );
		g_Memblocks_PassCoreData	= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Memblocks, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		g_Memblocks_Constructor		= ( RetVoidFunctionPointerPFN )	ConstructorMemblocks;
		g_Memblocks_Destructor		= ( RetVoidFunctionPointerPFN )	DestructorMemblocks;
		g_Memblocks_SetErrorHandler = SetErrorHandlerMemblocks;
		g_Memblocks_PassCoreData	= PassCoreDataMemblocks;
	#endif

	// Get Animation Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Animation)
	{
		g_Animation_Constructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Animation, "?Constructor@@YAXXZ" );
		g_Animation_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Animation, "?Destructor@@YAXXZ" );
		g_Animation_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Animation, "?SetErrorHandler@@YAXPAX@Z" );
		g_Animation_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Animation, "?PassCoreData@@YAXPAX@Z" );
		g_Animation_UpdateAnims			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Animation, "?UpdateAllAnimation@@YAXXZ" );
		g_Animation_PreventTextureLock	= ( RetVoidFunctionBoolPFN )				GetProcAddress ( g_Glob.g_Animation, "?PreventTextureLock@@YAX_N@Z" );
	}
	#else
		g_Animation_Constructor			= ( RetVoidFunctionPointerPFN ) ConstructorAnimation;
		g_Animation_Destructor			= ( RetVoidFunctionPointerPFN ) DestructorAnimation;
		g_Animation_SetErrorHandler		= SetErrorHandlerAnimation;
		g_Animation_PassCoreData		= PassCoreDataAnimation;
		g_Animation_UpdateAnims			= ( RetVoidFunctionPointerPFN ) dbUpdateAllAnimation;
		g_Animation_PreventTextureLock = dbPreventTextureLock;
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Bitmap)
	{
		// Bitmap Functions
		g_Bitmap_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Bitmap, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Bitmap_Destructor			= ( IMAGE_RetVoidParamVoidPFN )				GetProcAddress ( g_Glob.g_Bitmap, "?Destructor@@YAXXZ" );
		g_Bitmap_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Bitmap, "?SetErrorHandler@@YAXPAX@Z" );
		g_Bitmap_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Bitmap, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		// mike - 020206 - addition for vs8
		//g_Bitmap_Constructor		= ConstructorBitmap;
		g_Bitmap_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN ) ConstructorBitmap;
		g_Bitmap_Destructor			= DestructorBitmap;
		g_Bitmap_SetErrorHandler	= SetErrorHandlerBitmap;
		g_Bitmap_PassCoreData		= PassCoreDataBitmap;
	#endif

	// Multiplayer Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Multiplayer)
	{
		g_Multiplayer_Constructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Multiplayer, "?Constructor@@YAXXZ" );
		g_Multiplayer_Destructor		= ( RetVoidFunctionPointerPFN )			GetProcAddress ( g_Glob.g_Multiplayer, "?Destructor@@YAXXZ" );
		g_Multiplayer_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Multiplayer, "?SetErrorHandler@@YAXPAX@Z" );
		g_Multiplayer_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Multiplayer, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		g_Multiplayer_Constructor		= ( RetVoidFunctionPointerPFN ) ConstructorMultiplayer;
		g_Multiplayer_Destructor		= ( RetVoidFunctionPointerPFN ) DestructorMultiplayer;
		g_Multiplayer_SetErrorHandler	= SetErrorHandlerMultiplayer;
		g_Multiplayer_PassCoreData		= PassCoreDataMultiplayer;
	#endif

	// Camera Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Camera3D)
	{
		g_Camera3D_Constructor		= ( SPRITES_RetVoidParamHINSTANCE2PFN )		GetProcAddress ( g_Glob.g_Camera3D, "?Constructor@@YAXPAUHINSTANCE__@@0@Z" );
		g_Camera3D_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Camera3D, "?Destructor@@YAXXZ" );
		g_Camera3D_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Camera3D, "?SetErrorHandler@@YAXPAX@Z" );
		g_Camera3D_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Camera3D, "?PassCoreData@@YAXPAX@Z" );
		g_Camera3D_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Camera3D, "?Update@@YAXXZ" );
		g_Camera3D_StartScene		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Camera3D, "?StartScene@@YAXXZ" );
		g_Camera3D_StartSceneInt	= ( RetVoidFunctionPointerIntPFN )			GetProcAddress ( g_Glob.g_Camera3D, "?StartSceneEx@@YAXH@Z" );
		g_Camera3D_FinishScene		= ( RetIntFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Camera3D, "?FinishScene@@YAHXZ" );
		g_Camera3D_FinishSceneEx	= ( RetIntFunctionPointerBoolPFN )			GetProcAddress ( g_Glob.g_Camera3D, "?FinishSceneEx@@YAH_N@Z" );
		g_Camera3D_GetRenderCamera	= ( RetIntFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Camera3D, "?GetRenderCamera@@YAHXZ" );
		g_Camera3D_RunCode			= ( RetVoidFunctionPointerIntPFN )			GetProcAddress ( g_Glob.g_Camera3D, "?RunCode@@YAXH@Z" );
	}
	#else
		g_Camera3D_Constructor		= ConstructorCamera;
		g_Camera3D_Destructor		= ( RetVoidFunctionPointerPFN ) DestructorCamera;
		g_Camera3D_SetErrorHandler	= SetErrorHandlerCamera;
		g_Camera3D_PassCoreData		= PassCoreDataCamera;
		g_Camera3D_Update			= ( RetVoidFunctionPointerPFN ) UpdateCamera;
		g_Camera3D_StartScene		= ( RetVoidFunctionPointerPFN ) dbStartScene;
		g_Camera3D_StartSceneInt	= ( RetVoidFunctionPointerIntPFN ) dbStartSceneEx;
		g_Camera3D_FinishScene		= ( RetIntFunctionPointerPFN ) dbFinishScene;
		g_Camera3D_FinishSceneEx	= ( RetIntFunctionPointerBoolPFN )	dbFinishSceneEx;
		g_Camera3D_GetRenderCamera	= ( RetIntFunctionPointerPFN ) dbGetRenderCamera;
		g_Camera3D_RunCode			= ( RetVoidFunctionPointerIntPFN ) dbRunCode;
	#endif

	// Light Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Light3D)
	{
		g_Light3D_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Light3D, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Light3D_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Light3D, "?Destructor@@YAXXZ" );
		g_Light3D_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Light3D, "?SetErrorHandler@@YAXPAX@Z" );
		g_Light3D_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Light3D, "?PassCoreData@@YAXPAX@Z" );
		g_Light3D_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Light3D, "?Update@@YAXXZ" );
	}
	#else
		g_Light3D_Constructor		= ConstructorLight;
		g_Light3D_Destructor		= ( RetVoidFunctionPointerPFN ) DestructorLight;
		g_Light3D_SetErrorHandler	= SetErrorHandlerLight;
		g_Light3D_PassCoreData		= PassCoreDataLight;
		//g_Light3D_Update			= dbUpdateLight;
	#endif

	// Matrix Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Matrix3D)
	{
		g_Matrix3D_Constructor		= ( SPRITES_RetVoidParamHINSTANCE2PFN )		GetProcAddress ( g_Glob.g_Matrix3D, "?Constructor@@YAXPAUHINSTANCE__@@0@Z" );
		g_Matrix3D_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Matrix3D, "?Destructor@@YAXXZ" );
		g_Matrix3D_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Matrix3D, "?SetErrorHandler@@YAXPAX@Z" );
		g_Matrix3D_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Matrix3D, "?PassCoreData@@YAXPAX@Z" );
		g_Matrix3D_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Matrix3D, "?Update@@YAXXZ" );
		g_Matrix3D_LastUpdate		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Matrix3D, "?UpdateLast@@YAXXZ" );
	}
	#else
		g_Matrix3D_Constructor		= ConstructorMatrix;
		g_Matrix3D_Destructor		= ( RetVoidFunctionPointerPFN ) DestructorMatrix;
		g_Matrix3D_SetErrorHandler	= SetErrorHandlerMatrix;
		g_Matrix3D_PassCoreData		= PassCoreDataMatrix;
		g_Matrix3D_Update			= ( RetVoidFunctionPointerPFN ) UpdateMatrix;
		g_Matrix3D_LastUpdate		= ( RetVoidFunctionPointerPFN ) UpdateLastMatrix;
	#endif

	// Basic Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Basic3D)
	{
		g_Basic3D_Constructor		= ( BASIC3D_RetVoidParamHINSTANCE4PFN )		GetProcAddress ( g_Glob.g_Basic3D, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_Basic3D_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Basic3D, "?Destructor@@YAXXZ" );
		g_Basic3D_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Basic3D, "?SetErrorHandler@@YAXPAX@Z" );
		g_Basic3D_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Basic3D, "?PassCoreData@@YAXPAX@Z" );
		g_Basic3D_UpdateViewProjForMotionBlur = ( RetVoidFunctionPointerPFN )	GetProcAddress ( g_Glob.g_Basic3D, "?UpdateViewProjForMotionBlur@@YAXXZ" );
		g_Basic3D_UpdateAnimationCycle = ( RetVoidFunctionPointerPFN )		GetProcAddress ( g_Glob.g_Basic3D, "?UpdateAnimationCycle@@YAXXZ" );
		g_Basic3D_UpdateOnce		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Basic3D, "?UpdateOnce@@YAXXZ" );
		g_Basic3D_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Basic3D, "?Update@@YAXXZ" );
		g_Basic3D_UpdateGhostLayer	= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Basic3D, "?UpdateGhostLayer@@YAXXZ" );
		g_Basic3D_UpdateNoZDepth	= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Basic3D, "?UpdateNoZDepth@@YAXXZ" );
		g_Basic3D_StencilRenderStart = ( RetVoidFunctionPointerPFN )			GetProcAddress ( g_Glob.g_Basic3D, "?StencilRenderStart@@YAXXZ" );
		g_Basic3D_StencilRenderEnd = ( RetVoidFunctionPointerPFN )			GetProcAddress ( g_Glob.g_Basic3D, "?StencilRenderEnd@@YAXXZ" );

		g_Basic3D_SendFormats				= ( BASIC3D_RetVoidParamHINSTANCE6PFN )		GetProcAddress ( g_Glob.g_Basic3D, "?SendFormats@@YAXPAUHINSTANCE__@@00000@Z" );
		g_Basic3D_AutomaticCollisionStart	= ( RetVoidFunctionPointerPFN )		GetProcAddress ( g_Glob.g_Basic3D, "?AutomaticStart@@YAXXZ" );
		g_Basic3D_AutomaticCollisionEnd		= ( RetVoidFunctionPointerPFN )		GetProcAddress ( g_Glob.g_Basic3D, "?AutomaticEnd@@YAXXZ" );
		g_Basic3D_RenderQuad				= ( RetIntFunctionPointerIntPFN )	GetProcAddress ( g_Glob.g_Basic3D, "?RenderQuad@@YAHH@Z" );
	}
	#else
		g_Basic3D_Constructor				= ConstructorBasic3D;
		g_Basic3D_Destructor				= ( RetVoidFunctionPointerPFN ) DestructorBasic3D;
		g_Basic3D_SetErrorHandler			= SetErrorHandlerBasic3D;
		g_Basic3D_PassCoreData				= PassCoreDataBasic3D;
		g_Basic3D_Update					= ( RetVoidFunctionPointerPFN ) dbUpdateBasic3D;
		g_Basic3D_UpdateOnce				= ( RetVoidFunctionPointerPFN )	dbUpdateOnce;
		g_Basic3D_UpdateGhostLayer			= ( RetVoidFunctionPointerPFN )	dbUpdateGhostLayer;
		g_Basic3D_UpdateNoZDepth			= ( RetVoidFunctionPointerPFN ) dbUpdateNoZDepth;
		g_Basic3D_StencilRenderStart		= ( RetVoidFunctionPointerPFN ) dbStencilRenderStart;
		g_Basic3D_StencilRenderEnd			= ( RetVoidFunctionPointerPFN ) dbStencilRenderEnd;

		//g_Basic3D_SendFormats				= 
		g_Basic3D_AutomaticCollisionStart	= ( RetVoidFunctionPointerPFN ) dbAutomaticStart;
		g_Basic3D_AutomaticCollisionEnd		= ( RetVoidFunctionPointerPFN ) dbAutomaticEnd;
		g_Basic3D_RenderQuad				= ( RetIntFunctionPointerIntPFN )	dbRenderQuad;
	#endif

	// World Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_World3D)
	{
		g_World3D_Constructor		= ( WORLD3D_RetVoidParamHINSTANCE4PFN )		GetProcAddress ( g_Glob.g_World3D, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_World3D_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_World3D, "?Destructor@@YAXXZ" );
		g_World3D_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_World3D, "?SetErrorHandler@@YAXPAX@Z" );
		g_World3D_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_World3D, "?PassCoreData@@YAXPAX@Z" );
		g_World3D_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_World3D, "?UpdateEx@@YAXXZ" );
		g_World3D_Start				= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_World3D, "?StartEx@@YAXXZ" );
		g_World3D_End				= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_World3D, "?EndEx@@YAXXZ" );
	}
	#else
		g_World3D_Constructor		= ConstructorWorld;
		g_World3D_Destructor		= ( RetVoidFunctionPointerPFN )DestructorWorld;
		g_World3D_SetErrorHandler	= SetErrorHandlerWorld;
		g_World3D_PassCoreData		= PassCoreDataWorld;
		g_World3D_Update			= ( RetVoidFunctionPointerPFN )dbUpdateBSP;
		g_World3D_Start				= ( RetVoidFunctionPointerPFN )dbStartBSP;
		g_World3D_End				= ( RetVoidFunctionPointerPFN )dbEndBSP;
	#endif
	
	// Q2BSP Functions
	if(g_Glob.g_Q2BSP)
	{
		g_Q2BSP_Constructor			= ( WORLD3D_RetVoidParamHINSTANCE4PFN )		GetProcAddress ( g_Glob.g_Q2BSP, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_Q2BSP_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Q2BSP, "?Destructor@@YAXXZ" );
		g_Q2BSP_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Q2BSP, "?SetErrorHandler@@YAXPAX@Z" );
		g_Q2BSP_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Q2BSP, "?PassCoreData@@YAXPAX@Z" );
	}
	
	// ownBSP Functions
	if(g_Glob.g_OwnBSP)
	{
		g_OwnBSP_Constructor		= ( WORLD3D_RetVoidParamHINSTANCE4PFN )		GetProcAddress ( g_Glob.g_OwnBSP, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_OwnBSP_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_OwnBSP, "?Destructor@@YAXXZ" );
		g_OwnBSP_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_OwnBSP, "?SetErrorHandler@@YAXPAX@Z" );
		g_OwnBSP_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_OwnBSP, "?PassCoreData@@YAXPAX@Z" );
	}

	// BSPCompiler Functions
	if(g_Glob.g_BSPCompiler)
	{
		g_BSPCompiler_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_BSPCompiler, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_BSPCompiler_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_BSPCompiler, "?Destructor@@YAXXZ" );
		g_BSPCompiler_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_BSPCompiler, "?SetErrorHandler@@YAXPAX@Z" );
		g_BSPCompiler_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_BSPCompiler, "?PassCoreData@@YAXPAX@Z" );
	}

	// Vectors Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Vectors)
	{
		g_Vectors_Constructor		= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_Vectors, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Vectors_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Vectors, "?Destructor@@YAXXZ" );
		g_Vectors_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Vectors, "?SetErrorHandler@@YAXPAX@Z" );
		g_Vectors_PassCoreData		= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Vectors, "?PassCoreData@@YAXPAX@Z" );
	}
	#else
		g_Vectors_Constructor		= Constructor3DMaths;
		g_Vectors_Destructor		= ( RetVoidFunctionPointerPFN )Destructor3DMaths;
		g_Vectors_SetErrorHandler	= SetErrorHandler3DMaths;
		g_Vectors_PassCoreData		= PassCoreData3DMaths;
	#endif

	// Particles Functions
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Particles)
	{
		g_Particles_Constructor		= ( PTR_RetVoidParamHINSTANCE2PFN )			GetProcAddress ( g_Glob.g_Particles, "?Constructor@@YAXPAUHINSTANCE__@@0@Z" );
		g_Particles_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Particles, "?Destructor@@YAXXZ" );
		g_Particles_SetErrorHandler	= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_Particles, "?SetErrorHandler@@YAXPAX@Z" );
		g_Particles_PassCoreData	= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_Particles, "?PassCoreData@@YAXPAX@Z" );
		g_Particles_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_Particles, "?Update@@YAXXZ" );
	}
	#else
		g_Particles_Constructor		= ConstructorParticles;
		g_Particles_Destructor		= ( RetVoidFunctionPointerPFN ) DestructorParticles;
		g_Particles_SetErrorHandler	= SetErrorHandlerParticles;
		g_Particles_PassCoreData	= PassCoreDataParticles;
		g_Particles_Update			= ( RetVoidFunctionPointerPFN ) UpdateParticles;
	#endif

	// LODTerrain Functions
	if(g_Glob.g_LODTerrain)
	{
		g_LODTerrain_Constructor	= ( PTR_RetVoidParamHINSTANCE3PFN )			GetProcAddress ( g_Glob.g_LODTerrain, "?Constructor@@YAXPAUHINSTANCE__@@00@Z" );
		g_LODTerrain_Destructor		= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_LODTerrain, "?Destructor@@YAXXZ" );
		g_LODTerrain_SetErrorHandler= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_LODTerrain, "?SetErrorHandler@@YAXPAX@Z" );
		g_LODTerrain_PassCoreData	= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_LODTerrain, "?PassCoreData@@YAXPAX@Z" );
		g_LODTerrain_Update			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_LODTerrain, "?Update@@YAXXZ" );
	}

	// CSG Functions
	if(g_Glob.g_CSG)
	{
		g_CSG_Constructor			= ( PTR_RetVoidParamHINSTANCEPFN )			GetProcAddress ( g_Glob.g_CSG, "?ConstructorCSG@@YAXPAUHINSTANCE__@@@Z" );
		g_CSG_Destructor			= ( RetVoidFunctionPointerPFN )				GetProcAddress ( g_Glob.g_CSG, "?DestructorCSG@@YAXXZ" );
		g_CSG_SetErrorHandler		= ( PTR_RetVoidParamLPVOID )				GetProcAddress ( g_Glob.g_CSG, "?SetErrorHandler@@YAXPAX@Z" );
		g_CSG_PassCoreData			= ( RetVoidFCoreDataPFN )					GetProcAddress ( g_Glob.g_CSG, "?PassCoreData@@YAXPAX@Z" );
	}

	// U75 - 120809 - Stack up render calls in order for SETUP/DISPLAY rendering
	#ifndef DARKSDK_COMPILE
	// 20120313 IanM - Camera update not being used for anything else, so
	//                 now using to prepare the rendering surface instead of using
	//                 FinishSceneEx to do it.
    if (g_Glob.g_Camera3D)      AddToRenderList( g_Camera3D_Update,                0 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_UpdateOnce,          1000 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_StencilRenderStart,  2000 );
    if (g_Glob.g_World3D)       AddToRenderList( g_World3D_Update,              3000 );
    if (g_Glob.g_Light3D)       AddToRenderList( g_Light3D_Update,              5000 );
    if (g_Glob.g_LODTerrain)    AddToRenderList( g_LODTerrain_Update,           6000 );
    if (g_Glob.g_Matrix3D)      AddToRenderList( g_Matrix3D_Update,             7000 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_Update,              8000 );
    if (g_Glob.g_Particles)     AddToRenderList( g_Particles_Update,            9000 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_UpdateGhostLayer,    10000 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_StencilRenderEnd,    11000 );
    if (g_Glob.g_Matrix3D)      AddToRenderList( g_Matrix3D_LastUpdate,         12000 );
    if (g_Glob.g_Basic3D)       AddToRenderList( g_Basic3D_UpdateNoZDepth,      13000 );
	#else
	// 20120313 IanM - Camera update not being used for anything else, so
	//                 now using to prepare the rendering surface instead of using
	//                 FinishSceneEx to do it.
    if (g_Camera3D_Update)				AddToRenderList( g_Camera3D_Update,                0 );
    if (g_Basic3D_UpdateOnce)			AddToRenderList( g_Basic3D_UpdateOnce,          1000 );
    if (g_Basic3D_StencilRenderStart)	AddToRenderList( g_Basic3D_StencilRenderStart,  2000 );
    if (g_World3D_Update)				AddToRenderList( g_World3D_Update,              3000 );
//  if (g_Camera3D_Update)				AddToRenderList( g_Camera3D_Update,             4000 );
    if (g_Light3D_Update)				AddToRenderList( g_Light3D_Update,              5000 );
    if (g_LODTerrain_Update)			AddToRenderList( g_LODTerrain_Update,           6000 );
    if (g_Matrix3D_Update)				AddToRenderList( g_Matrix3D_Update,             7000 );
    if (g_Basic3D_Update)				AddToRenderList( g_Basic3D_Update,              8000 );
    if (g_Particles_Update)				AddToRenderList( g_Particles_Update,            9000 );
    if (g_Basic3D_UpdateGhostLayer)		AddToRenderList( g_Basic3D_UpdateGhostLayer,    10000 );
    if (g_Basic3D_StencilRenderEnd)		AddToRenderList( g_Basic3D_StencilRenderEnd,    11000 );
    if (g_Matrix3D_LastUpdate)			AddToRenderList( g_Matrix3D_LastUpdate,         12000 );
    if (g_Basic3D_UpdateNoZDepth)		AddToRenderList( g_Basic3D_UpdateNoZDepth,      13000 );
	#endif

	// Complete
	return true;
}
