//
// Prototype DBDLLExtCalls Setup Function
//

#ifndef DBDLLEXTCALLS_H
#define DBDLLEXTCALLS_H

//#define DARKSDK __declspec ( dllexport )

#include "..\global.h"

DARKSDK bool SetDBDLLExtCalls(void);

//
// General DLL Typedefs
//
typedef void				( *PTR_RetVoidParamLPVOID )  ( LPVOID );
typedef void				( *PTR_RetVoidParamHINSTANCEPFN ) ( HINSTANCE );
typedef void				( *PTR_RetVoidParamHINSTANCE2PFN ) ( HINSTANCE, HINSTANCE );
typedef void				( *PTR_RetVoidParamHINSTANCE3PFN ) ( HINSTANCE, HINSTANCE, HINSTANCE );
typedef void		        ( *RetVoidFunctionPointerPFN ) ( ... );
typedef void		        ( *RetVoidFunctionPointerIntPFN ) ( int );
typedef int			        ( *RetIntFunctionPointerPFN ) ( ... );
typedef int			        ( *RetIntFunctionPointerBoolPFN ) ( bool );
typedef void			    ( *RetVoidFCoreDataPFN ) ( LPVOID );
typedef void				( *RetVoidFCoreData2PFN ) ( LPVOID, int );
typedef int			        ( *RetIntFunctionPointerIntPFN ) ( int );

//
// External Display DLL Typedefs
//
typedef void				( *GFX_RetVoidParamVoidPFN )  ( void );
typedef bool				( *GFX_RetBoolParamVoidPFN )  ( void );
typedef bool				( *GFX_RetBoolParamInt2PFN )  ( int, int );
typedef bool				( *GFX_RetBoolParamInt3PFN )  ( int, int, int );
typedef bool				( *GFX_RetBoolParamInt4PFN )  ( int, int, int, int );
typedef bool				( *GFX_RetBoolParamInt5PFN )  ( int, int, int, int, int );
typedef bool				( *GFX_RetBoolParamInt6PFN )  ( int, int, int, int, int, int );
typedef void				( *GFX_RestoreDisplayMode  )  ( void );
typedef void				( *GFX_RetVoidParamInt3PFN )  ( int, int, int );
typedef void				( *GFX_RetVoidParamIntP3PFN )  ( int*, int*, int* );
typedef int 				( *GFX_RetIntParamVoidPFN  )  ( void );
typedef char*				( *GFX_RetPCharParamIntPFN )  ( int );
typedef void 				( *GFX_RetVoidParamIntPFN  )  ( int );
typedef void 				( *GFX_RetVoidParamInt2PFN )  ( int, int );
typedef void 				( *GFX_RetVoidParamHWNDPFN )  ( HWND );
typedef void 				( *RetVoidFunctionBoolPFN )   ( bool );
typedef IDirect3DDevice9*   ( *GFX_GetDirect3DDevicePFN ) ( void );

//
// External Display Function Pointers
//
extern GFX_RetBoolParamVoidPFN		g_GFX_Constructor;					// constructor
extern GFX_RetVoidParamVoidPFN		g_GFX_Destructor;					// destructor
extern PTR_RetVoidParamLPVOID		g_GFX_SetErrorHandler;
extern RetVoidFCoreData2PFN			g_GFX_PassCoreData;
extern GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3D;

extern GFX_RetVoidParamVoidPFN		g_GFX_Begin;						// begin scene ( for compiler only )
extern GFX_RetVoidParamVoidPFN		g_GFX_End;							// end scene ( for compiler only )
extern GFX_RetVoidParamVoidPFN		g_GFX_Render;						// render finished scene ( for compiler only )
extern GFX_RetVoidParamHWNDPFN		g_GFX_OverrideHWND;					// Use Actual HWND

extern GFX_RetBoolParamVoidPFN		g_GFX_SetDisplayDebugMode;			// set debug display mode ( shows a dialog box at start of application to control settings )
extern GFX_RetBoolParamInt2PFN 		g_GFX_SetDisplayMode1;				// set display ( iWidth, iHeight )
extern GFX_RetBoolParamInt3PFN		g_GFX_SetDisplayMode2;				// set display ( iWidth, iHeight, iDepth )
extern GFX_RetBoolParamInt4PFN		g_GFX_SetDisplayMode3;				// set display ( iWidth, iHeight, iDepth, iMode )
extern GFX_RetBoolParamInt5PFN		g_GFX_SetDisplayMode4;				// set display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing )
extern GFX_RetBoolParamInt6PFN		g_GFX_SetDisplayMode5;				// set display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing, iLockable )
extern GFX_RestoreDisplayMode		g_GFX_RestoreDisplayMode;
extern GFX_RetVoidParamInt3PFN		g_GFX_Clear;						// clear screen to specified colour
extern GFX_RetVoidParamInt3PFN		g_GFX_SetGamma;						// set gamma of screen
extern GFX_RetVoidParamIntP3PFN		g_GFX_GetGamma;						// get gamma of screen

extern GFX_RetBoolParamInt2PFN		g_GFX_CheckDisplayMode1;			// check display ( iWidth, iHeight )
extern GFX_RetBoolParamInt3PFN		g_GFX_CheckDisplayMode2;			// check display ( iWidth, iHeight, iDepth )
extern GFX_RetBoolParamInt4PFN		g_GFX_CheckDisplayMode3;			// check display ( iWidth, iHeight, iDepth, iMode )
extern GFX_RetBoolParamInt5PFN		g_GFX_CheckDisplayMode4;			// check display ( iWidth, iHeight, iDepth, iMode, iVertexProcessing )

extern GFX_RetBoolParamVoidPFN		g_GFX_GetDisplayType;				// get display type - software or hardware
extern GFX_RetIntParamVoidPFN		g_GFX_GetDisplayWidth;				// get display width
extern GFX_RetIntParamVoidPFN		g_GFX_GetDisplayHeight;				// get display height
extern GFX_RetIntParamVoidPFN		g_GFX_GetDisplayDepth;				// get display depth
extern GFX_RetIntParamVoidPFN		g_GFX_GetDisplayFPS;				// get display fps
extern GFX_RetBoolParamVoidPFN		g_GFX_GetWindowedMode;				// get windowed mode

extern GFX_RetIntParamVoidPFN		g_GFX_GetNumberOfDisplayModes;		// get number of display modes for adapter
extern GFX_RetPCharParamIntPFN		g_GFX_GetDisplayMode;				// get the specified display mode

extern GFX_RetIntParamVoidPFN		g_GFX_GetNumberOfDisplayDevices;	// get the number of graphics cards installed
extern GFX_RetVoidParamIntPFN		g_GFX_SetDisplayDevice;				// select which device to use
extern GFX_RetPCharParamIntPFN		g_GFX_GetDeviceName;				// get name of device
extern GFX_RetPCharParamIntPFN		g_GFX_GetDeviceDriverName;			// useful for debugging purposes

extern GFX_RetVoidParamIntPFN		g_GFX_SetDitherMode;				// set dithering on / off
extern GFX_RetVoidParamIntPFN		g_GFX_SetShadeMode;					// set shading to flat or gouraud
extern GFX_RetVoidParamIntPFN		g_GFX_SetLightMode;					// set lighting on / off, can be overrided by individual objects
extern GFX_RetVoidParamIntPFN		g_GFX_SetCullMode;					// set cull mode to none, cw, or ccw, can be overrided by individual objects
extern GFX_RetVoidParamIntPFN		g_GFX_SetSpecularMode;				// set specular highlighs on / off
extern GFX_RetVoidParamInt2PFN		g_GFX_SetRenderState;				// set the render state as in D3D, can be overrided by individual objects


//
// External Text DLL Typedefs
//
typedef void			    ( *TEXT_VoidPFN )				( void );
typedef void				( *TEXT_Int2PFN )				( int, int );
typedef void				( *TEXT_Char1PFN )				( char* );
typedef void				( *TEXT_Int2Char1PFN )			( int, int, char* );
typedef void				( *TEXT_Int2Char1PFNEx )		( int, int, DWORD );
typedef void				( *TEXT_Char1Int1PFN )			( char*, int );
typedef void				( *TEXT_Int1PFN )				( int );
typedef int  				( *TEXT_retInt1Char1PFN )		( char* );
typedef char*  				( *TEXT_retChar1Int1PFN )		( int );
typedef char*  				( *TEXT_retChar1Char1Int1PFN )	( char*, int );
typedef char*  				( *TEXT_retChar1Char1PFN )		( char* );
typedef char*  				( *TEXT_retChar1PFN )			( void );
typedef int*  				( *TEXT_retInt1PFN )			( void );
typedef char  				( *TEXT_retCharn1PFN )			( int );
typedef int  				( *TEXT_retInta1PFN )			( void );
typedef void				( *TEXT_VoidInt4PFN )			( int, int, int, int );

typedef void               ( *OBJECT_ScalePFN          ) ( int, float, float, float );
typedef void               ( *OBJECT_PositionPFN       ) ( int, float, float, float );
typedef void               ( *OBJECT_RotatePFN         ) ( int, float, float, float );
typedef void			   ( *OBJECT_XRotatePFN        ) ( int, float );
typedef void			   ( *OBJECT_YRotatePFN        ) ( int, float );
typedef void			   ( *OBJECT_ZRotatePFN		   ) ( int, float );
typedef void			   ( *OBJECT_PointPFN		   ) ( int, float, float, float );
typedef void			   ( *OBJECT_MovePFN		   ) ( int, float );
typedef void			   ( *OBJECT_MoveLeftPFN	   ) ( int, float );
typedef void			   ( *OBJECT_MoveRightPFN	   ) ( int, float );
typedef void			   ( *OBJECT_TurnLeftPFN	   ) ( int, float );
typedef float			   ( *OBJECT_GetXPositionPFN   ) ( int );
typedef float			   ( *OBJECT_GetYPositionPFN   ) ( int );
typedef float			   ( *OBJECT_GetZPositionPFN   ) ( int );
typedef float			   ( *OBJECT_GetXSizePFN       ) ( int );
typedef float			   ( *OBJECT_GetYSizePFN       ) ( int );
typedef float			   ( *OBJECT_GetZSizePFN       ) ( int );
typedef float			   ( *OBJECT_GetXRotationPFN   ) ( int );
typedef float			   ( *OBJECT_GetYRotationPFN   ) ( int );
typedef float			   ( *OBJECT_GetZRotationPFN   ) ( int );

//
// External Text Function Pointers
//
extern PTR_RetVoidParamHINSTANCEPFN	g_Text_Constructor;
extern GFX_RetVoidParamVoidPFN		g_Text_Destructor;
extern PTR_RetVoidParamLPVOID		g_Text_SetErrorHandler;
extern RetVoidFCoreDataPFN			g_Text_PassCoreData;

extern TEXT_VoidInt4PFN				g_Text_SetColor;
extern TEXT_Int2PFN					g_Text_SetCursor;
extern TEXT_Char1PFN				g_Text_Print;
extern TEXT_Int2Char1PFN			g_Text_Text;
extern TEXT_Int2Char1PFNEx			g_Text_TextEx;
extern TEXT_Int2Char1PFN			g_Text_CenterText;

extern TEXT_Char1PFN				g_Text_SetFont;
extern TEXT_Char1Int1PFN			g_Text_SetCharacterSet;
extern TEXT_Int1PFN					g_Text_SetSize;
extern TEXT_VoidPFN					g_Text_SetToNormal;
extern TEXT_VoidPFN					g_Text_SetToItalic;
extern TEXT_VoidPFN					g_Text_SetToBold;
extern TEXT_VoidPFN					g_Text_SetToBoldItalic;
extern TEXT_VoidPFN					g_Text_SetToOpaque;
extern TEXT_VoidPFN					g_Text_SetToTransparent;

extern TEXT_retInt1Char1PFN			g_Text_asc;
extern TEXT_retInt1Char1PFN			g_Text_val;
extern TEXT_retChar1Int1PFN			g_Text_bin;
extern TEXT_retCharn1PFN			g_Text_chr;
extern TEXT_retChar1Int1PFN			g_Text_hex;
extern TEXT_retInt1Char1PFN			g_Text_len;
extern TEXT_retChar1Char1Int1PFN	g_Text_left;
extern TEXT_retChar1Char1Int1PFN	g_Text_mid;
extern TEXT_retChar1Char1Int1PFN	g_Text_right;
extern TEXT_retChar1Int1PFN			g_Text_str;
extern TEXT_retChar1Char1PFN		g_Text_lower;
extern TEXT_retChar1Char1PFN		g_Text_upper;

extern TEXT_retChar1PFN				g_Text_GetFont;
extern TEXT_retInta1PFN				g_Text_GetSize;
extern TEXT_retInt1PFN				g_Text_GetStyle;
extern TEXT_retInt1PFN				g_Text_GetBackgroundType;

extern TEXT_retInt1Char1PFN			g_Text_GetWidth;
extern TEXT_retInt1Char1PFN			g_Text_GetHeight;

extern TEXT_Char1PFN				g_Text_Text3D;
extern OBJECT_ScalePFN				g_Text_Scale;				// scale
extern OBJECT_PositionPFN			g_Text_Position;			// position
extern OBJECT_RotatePFN				g_Text_Rotate;				// rotation on all 3 axes
extern OBJECT_XRotatePFN			g_Text_XRotate;				// x rotation
extern OBJECT_YRotatePFN			g_Text_YRotate;				// y rotation
extern OBJECT_ZRotatePFN			g_Text_ZRotate;				// z rotation
extern OBJECT_PointPFN				g_Text_Point;				// point at a position
extern OBJECT_MovePFN				g_Text_Move;				// move in it's current direction
extern OBJECT_MoveLeftPFN			g_Text_MoveLeft;			// move left
extern OBJECT_MoveRightPFN			g_Text_MoveRight;			// move right
extern OBJECT_TurnLeftPFN			g_Text_TurnLeft;			// turn left
extern OBJECT_GetXPositionPFN		g_Text_GetXPosition;		// get x position
extern OBJECT_GetYPositionPFN		g_Text_GetYPosition;		// get y position
extern OBJECT_GetZPositionPFN		g_Text_GetZPosition;		// get z position
extern OBJECT_GetXSizePFN			g_Text_GetXSize;			// get x size
extern OBJECT_GetYSizePFN			g_Text_GetYSize;			// get y size
extern OBJECT_GetZSizePFN			g_Text_GetZSize;			// get z size
extern OBJECT_GetXRotationPFN		g_Text_GetXRotation;		// get x rotation
extern OBJECT_GetYRotationPFN		g_Text_GetYRotation;		// get y rotation
extern OBJECT_GetZRotationPFN		g_Text_GetZRotation;		// get z rotation

//
// External Basic2D DLL Typedefs
//
typedef void				( *BASIC2D_RetVoidParamVoidPFN )		( void );
typedef void				( *BASIC2D_RetVoidParamInt2PFN )		( int, int );
typedef void				( *BASIC2D_RetVoidParamInt3PFN )		( int, int, int );
typedef void				( *BASIC2D_RetVoidParamInt4PFN )		( int, int, int, int );
typedef void				( *BASIC2D_RetVoidParamInt6PFN )		( int, int, int, int, int, int );

//
// External Basic2D DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN		g_Basic2D_Constructor;
extern BASIC2D_RetVoidParamVoidPFN		g_Basic2D_Destructor;
extern PTR_RetVoidParamLPVOID			g_Basic2D_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Basic2D_PassCoreData;

extern BASIC2D_RetVoidParamInt2PFN		g_Basic2D_Dot;
extern BASIC2D_RetVoidParamVoidPFN		g_Basic2D_Lock;
extern BASIC2D_RetVoidParamVoidPFN		g_Basic2D_Unlock;
extern BASIC2D_RetVoidParamInt4PFN		g_Basic2D_Box;
extern BASIC2D_RetVoidParamInt4PFN		g_Basic2D_Line;
extern BASIC2D_RetVoidParamInt3PFN		g_Basic2D_Circle;

extern BASIC2D_RetVoidParamInt6PFN		g_Basic2D_CopyArea;

//
// External Sprites DLL Typedefs
//
typedef void				( *SPRITES_RetVoidParamVoidPFN )      ( void );
typedef void                ( *SPRITES_RetVoidParamHINSTANCE2PFN )  ( HINSTANCE, HINSTANCE );

//
// External Sprites DLL Functions
//
extern SPRITES_RetVoidParamHINSTANCE2PFN	g_Sprites_Constructor;
extern SPRITES_RetVoidParamVoidPFN			g_Sprites_Destructor;
extern RetVoidFCoreDataPFN					g_Sprites_PassCoreData;
extern PTR_RetVoidParamLPVOID				g_Sprites_SetErrorHandler;

extern SPRITES_RetVoidParamVoidPFN			g_Sprites_Update;
extern SPRITES_RetVoidParamVoidPFN			g_Sprites_SaveBack;
extern SPRITES_RetVoidParamVoidPFN			g_Sprites_RestoreBack;

//
// External Image DLL Typedefs
//
typedef void				( *IMAGE_RetVoidParamVoidPFN )      ( void );
typedef bool				( *IMAGE_RetBoolParamIntPCharPFN )  ( int, char* );
typedef void				( *IMAGE_RetVoidParamIntPFN )       ( int );
typedef int  				( *IMAGE_RetIntParamIntPFN )        ( int );
typedef bool  				( *IMAGE_RetBoolParamIntPFN )       ( int );
typedef void  				( *IMAGE_RetVoidParamBoolPFN )      ( bool );
typedef void  				( *IMAGE_RetVoidParamInt7PFN )      ( int, int, int, int, int, int, int );
typedef void  				( *IMAGE_RetVoidParamFloatPFN )     ( float );
typedef void  				( *IMAGE_RetVoidParamInt3PFN )      ( int, int, int );
typedef void  				( *IMAGE_RetVoidParamInt2PFN )      ( int, int );

//
// External Image DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN		g_Image_Constructor;				// constructor
extern IMAGE_RetVoidParamVoidPFN		g_Image_Destructor;					// destructor
extern PTR_RetVoidParamLPVOID			g_Image_SetErrorHandler;
extern PTR_RetVoidParamHINSTANCEPFN		g_Image_PassSpriteInstance;			// pass sprite
extern RetVoidFCoreDataPFN				g_Image_PassCoreData;

extern IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapNum;				// set number of mipmaps for when an image is created
extern IMAGE_RetVoidParamInt3PFN		g_Image_SetColorKey;				// set image color key

//
// External Input DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN		g_Input_Constructor;
extern RetVoidFunctionPointerPFN		g_Input_Destructor;
extern PTR_RetVoidParamLPVOID			g_Input_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Input_PassCoreData;
extern GFX_RetVoidParamVoidPFN			g_Input_ClearData;

//
// External Transforms DLL Functions
//
extern RetVoidFunctionPointerPFN		g_Transforms_Constructor;
extern RetVoidFunctionPointerPFN		g_Transforms_Destructor;
extern PTR_RetVoidParamLPVOID			g_Transforms_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Transforms_PassCoreData;

//
// External System DLL Functions
//
extern RetVoidFunctionPointerPFN		g_System_Constructor;
extern RetVoidFunctionPointerPFN		g_System_Destructor;
extern PTR_RetVoidParamLPVOID			g_System_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_System_PassCoreData;

//
// External Sound DLL Functions
//
extern GFX_RetVoidParamHWNDPFN			g_Sound_Constructor;
extern RetVoidFunctionPointerPFN		g_Sound_Destructor;
extern PTR_RetVoidParamLPVOID			g_Sound_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Sound_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Sound_Update;

//
// External Music DLL Functions
//
extern GFX_RetVoidParamHWNDPFN			g_Music_Constructor;
extern RetVoidFunctionPointerPFN		g_Music_Destructor;
extern PTR_RetVoidParamLPVOID			g_Music_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Music_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Music_Update;

//
// External File DLL Functions
//
//extern RetVoidFunctionPointerPFN		g_File_Constructor;

// Global External File Function Pointers
#ifndef DARKSDK_COMPILE
//	extern PTR_RetVoidParamHINSTANCEPFN		g_File_Constructor;
	extern RetVoidFunctionPointerPFN		g_File_Constructor; //leefix - 020605 - coreDLL stopped compiling
	extern RetVoidFunctionPointerPFN		g_File_Destructor;
#else
	extern GFX_RetVoidParamVoidPFN			g_File_Constructor;
	extern GFX_RetVoidParamVoidPFN		g_File_Destructor;
#endif


extern PTR_RetVoidParamLPVOID			g_File_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_File_PassCoreData;

//
// External FTP DLL Functions
//
extern RetVoidFunctionPointerPFN		g_FTP_Constructor;
extern RetVoidFunctionPointerPFN		g_FTP_Destructor;
extern PTR_RetVoidParamLPVOID			g_FTP_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_FTP_PassCoreData;

//
// External Memblocks DLL Functions
//
extern RetVoidFunctionPointerPFN		g_Memblocks_Constructor;
extern RetVoidFunctionPointerPFN		g_Memblocks_Destructor;
extern PTR_RetVoidParamLPVOID			g_Memblocks_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Memblocks_PassCoreData;

//
// External Animation DLL Functions
//
extern RetVoidFunctionPointerPFN		g_Animation_Constructor;
extern RetVoidFunctionPointerPFN		g_Animation_Destructor;
extern PTR_RetVoidParamLPVOID			g_Animation_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Animation_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Animation_UpdateAnims;
extern RetVoidFunctionBoolPFN			g_Animation_PreventTextureLock;

//
// External Bitmap DLL Functions
//
// mike - 020206 - addition for vs8
//extern PTR_RetVoidParamHINSTANCEPFN		g_Bitmap_Constructor;				// constructor
//extern DBPRO_GLOBAL PTR_RetVoidParamHINSTANCEPFN				g_Bitmap_Constructor;				// constructor
extern PTR_RetVoidParamHINSTANCEPFN		g_Bitmap_Constructor;
extern IMAGE_RetVoidParamVoidPFN		g_Bitmap_Destructor;
extern PTR_RetVoidParamLPVOID			g_Bitmap_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Bitmap_PassCoreData;

//
// External Multiplayer DLL Functions
//
extern RetVoidFunctionPointerPFN		g_Multiplayer_Constructor;
extern RetVoidFunctionPointerPFN		g_Multiplayer_Destructor;
extern PTR_RetVoidParamLPVOID			g_Multiplayer_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Multiplayer_PassCoreData;

//
// External Camera DLL Functions
//
extern SPRITES_RetVoidParamHINSTANCE2PFN g_Camera3D_Constructor;
extern RetVoidFunctionPointerPFN		g_Camera3D_Destructor;
//extern GFX_RetVoidParamVoidPFN			g_Camera3D_Destructor;
extern PTR_RetVoidParamLPVOID			g_Camera3D_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Camera3D_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Camera3D_Update;
//extern GFX_RetVoidParamVoidPFN		g_Camera3D_Update;
extern RetVoidFunctionPointerPFN		g_Camera3D_StartScene;
extern RetVoidFunctionPointerIntPFN		g_Camera3D_StartSceneInt;
//extern GFX_RetVoidParamVoidPFN		g_Camera3D_StartScene;
extern RetIntFunctionPointerPFN			g_Camera3D_FinishScene;
extern RetIntFunctionPointerBoolPFN		g_Camera3D_FinishSceneEx;
//extern GFX_RetIntParamVoidPFN			g_Camera3D_FinishScene;
extern RetVoidFunctionPointerIntPFN		g_Camera3D_RunCode;
extern RetIntFunctionPointerPFN g_Camera3D_GetRenderCamera;


//
// External Light DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN		g_Light3D_Constructor;
extern RetVoidFunctionPointerPFN		g_Light3D_Destructor;
//extern GFX_RetVoidParamVoidPFN			g_Light3D_Destructor;
extern PTR_RetVoidParamLPVOID			g_Light3D_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Light3D_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Light3D_Update;

//
// External Matrix DLL Functions
//
extern SPRITES_RetVoidParamHINSTANCE2PFN g_Matrix3D_Constructor;
extern RetVoidFunctionPointerPFN		g_Matrix3D_Destructor;
//extern GFX_RetVoidParamVoidPFN			g_Matrix3D_Destructor;
extern PTR_RetVoidParamLPVOID			g_Matrix3D_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Matrix3D_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Matrix3D_Update;
//extern GFX_RetVoidParamVoidPFN		g_Matrix3D_Update;
extern RetVoidFunctionPointerPFN		g_Matrix3D_LastUpdate;
//extern GFX_RetVoidParamVoidPFN		g_Matrix3D_LastUpdate;

//
// External Basic3D DLL Functions
//
typedef void                ( *BASIC3D_RetVoidParamHINSTANCE4PFN )  ( HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE );
typedef void                ( *BASIC3D_RetVoidParamHINSTANCE6PFN )  ( HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE );

extern BASIC3D_RetVoidParamHINSTANCE4PFN g_Basic3D_Constructor;
extern RetVoidFunctionPointerPFN		g_Basic3D_Destructor;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_Destructor;
extern PTR_RetVoidParamLPVOID			g_Basic3D_SetErrorHandler;
extern RetVoidFCoreDataPFN				g_Basic3D_PassCoreData;
extern RetVoidFunctionPointerPFN		g_Basic3D_UpdateViewProjForMotionBlur;
extern RetVoidFunctionPointerPFN		g_Basic3D_Update;
extern RetVoidFunctionPointerPFN		g_Basic3D_UpdateAnimationCycle;
extern RetVoidFunctionPointerPFN		g_Basic3D_UpdateOnce;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_Update;
extern RetVoidFunctionPointerPFN		g_Basic3D_UpdateGhostLayer;
extern RetVoidFunctionPointerPFN		g_Basic3D_UpdateNoZDepth;
extern RetVoidFunctionPointerPFN		g_Basic3D_StencilRenderStart;
extern RetVoidFunctionPointerPFN		g_Basic3D_StencilRenderEnd;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_UpdateNoZDepth;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_StencilRenderStart;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_StencilRenderEnd;
extern BASIC3D_RetVoidParamHINSTANCE6PFN g_Basic3D_SendFormats;
extern RetVoidFunctionPointerPFN		g_Basic3D_AutomaticCollisionStart;
extern RetVoidFunctionPointerPFN		g_Basic3D_AutomaticCollisionEnd;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_AutomaticCollisionStart;
//extern GFX_RetVoidParamVoidPFN		g_Basic3D_AutomaticCollisionEnd;
extern RetIntFunctionPointerIntPFN		g_Basic3D_RenderQuad;					

//
// External World3D DLL Functions
//
typedef void                ( *WORLD3D_RetVoidParamHINSTANCE4PFN )  ( HINSTANCE, HINSTANCE, HINSTANCE, HINSTANCE );

extern WORLD3D_RetVoidParamHINSTANCE4PFN	g_World3D_Constructor;
extern RetVoidFunctionPointerPFN			g_World3D_Destructor;
extern PTR_RetVoidParamLPVOID				g_World3D_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_World3D_PassCoreData;
extern RetVoidFunctionPointerPFN			g_World3D_Update;
extern RetVoidFunctionPointerPFN			g_World3D_Start;
extern RetVoidFunctionPointerPFN			g_World3D_End;

//
// External Q2BSP DLL Functions
//
extern WORLD3D_RetVoidParamHINSTANCE4PFN	g_Q2BSP_Constructor;
extern RetVoidFunctionPointerPFN			g_Q2BSP_Destructor;
extern PTR_RetVoidParamLPVOID				g_Q2BSP_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_Q2BSP_PassCoreData;

//
// External OwnBSP DLL Functions
//
extern WORLD3D_RetVoidParamHINSTANCE4PFN	g_OwnBSP_Constructor;
extern RetVoidFunctionPointerPFN			g_OwnBSP_Destructor;
extern PTR_RetVoidParamLPVOID				g_OwnBSP_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_OwnBSP_PassCoreData;

//
// External BSPCompiler DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN			g_BSPCompiler_Constructor;
extern RetVoidFunctionPointerPFN			g_BSPCompiler_Destructor;
extern PTR_RetVoidParamLPVOID				g_BSPCompiler_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_BSPCompiler_PassCoreData;

//
// External Vectors DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN			g_Vectors_Constructor;
extern RetVoidFunctionPointerPFN			g_Vectors_Destructor;
extern PTR_RetVoidParamLPVOID				g_Vectors_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_Vectors_PassCoreData;

//
// External Particles DLL Functions
//
extern PTR_RetVoidParamHINSTANCE2PFN		g_Particles_Constructor;
extern RetVoidFunctionPointerPFN			g_Particles_Destructor;
extern PTR_RetVoidParamLPVOID				g_Particles_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_Particles_PassCoreData;
extern RetVoidFunctionPointerPFN			g_Particles_Update;

//
// External LODTerrain DLL Functions
//
extern PTR_RetVoidParamHINSTANCE3PFN		g_LODTerrain_Constructor;
extern RetVoidFunctionPointerPFN			g_LODTerrain_Destructor;
extern PTR_RetVoidParamLPVOID				g_LODTerrain_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_LODTerrain_PassCoreData;
extern RetVoidFunctionPointerPFN			g_LODTerrain_Update;

//
// External CSG DLL Functions
//
extern PTR_RetVoidParamHINSTANCEPFN			g_CSG_Constructor;
extern RetVoidFunctionPointerPFN			g_CSG_Destructor;
extern PTR_RetVoidParamLPVOID				g_CSG_SetErrorHandler;
extern RetVoidFCoreDataPFN					g_CSG_PassCoreData;

#endif
