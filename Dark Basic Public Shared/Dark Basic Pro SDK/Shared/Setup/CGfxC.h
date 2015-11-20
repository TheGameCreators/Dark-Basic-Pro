#ifndef _CGFX_H_
#define _CGFX_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3d9.lib" )		// include the direct3d library
#pragma comment ( lib, "winmm.lib" )	// include windows multimedia extensions ( timers etc )

#include < d3d9.h >						// include direct3d header

#include <stdio.h>
#include ".\..\error\cerror.h"			// contains error functions
#include ".\..\core\globstruct.h"		// contains glub struct data

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
//	#define DBPRO_GLOBAL static
#endif

#define DEBUG_MODE 0
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }	// safely delete an object
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }	// safely release a COM interface
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }	// safely delete an array
#define MAX_SWAP_CHAINS	4															// maximum number of swap chains
#define D3DFVF_VERTEX2D ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// ENUMERATED VALUES /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

enum TIMER_COMMAND	{
						// timer values
						TIMER_RESET,				// reset the timer
						TIMER_START,				// start the timer
						TIMER_STOP,					// stop the timer
						TIMER_ADVANCE,				// advance the timer
						TIMER_GETABSOLUTETIME,		// absolute time of the timer
						TIMER_GETAPPTIME,			// app time
						TIMER_GETELAPSEDTIME		// elapsed time
					};

enum CGfx_Return
					{
						// return values only useful for DarkSDK
						OK,							// everything went ok
						CANNOTCREATEINTERFACE,		// failed to setup interface
						CANNOTGETDISPLAYINFO,		// failed to get display information
						CANNOTCREATEDEVICE,			// failed to create device
						INVALIDMODE,				// invalid more specified
					};

enum CGfx_Flags
					{
						// flags only useful for DarkSDK
						FULLSCREEN,										// fullscreen mode
						WINDOWED,										// windowed mode
						DEFAULT,										// default goes to windowed mode
						HARDWARE = D3DCREATE_HARDWARE_VERTEXPROCESSING,	// hardware vertex processing
						SOFTWARE = D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// software vertex processing
						MIXED    = D3DCREATE_MIXED_VERTEXPROCESSING,	// mixed vertex processing
					};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct tagInfo
{
	// graphics card information structure
	char*					szName;				// name of graphics card
	D3DCAPS9				D3DCaps;			// capabilities
	D3DADAPTER_IDENTIFIER9	D3DAdapter;			// adapter identifier
	D3DDISPLAYMODE			D3DMode;			// current display mode
	D3DDISPLAYMODE*			D3DDisplay;			// display mode info
	int						iDisplayCount;		// num of available display modes
};

struct VERTEX2D
{
	float	x;
	float	y;
	float	z;

	float	rhw;

	DWORD	color;

	float	tu;
	float	tv;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool				Setup                       		( void );
DARKSDK void				FindHardwareInfo            		( void );
DARKSDK void				GetValidBackBufferFormat    		( void );
DARKSDK void				GetValidStencilBufferFormat 		( void );
DARKSDK int					Create                      		( HWND hWnd, D3DPRESENT_PARAMETERS* d3dpp );
DARKSDK float __stdcall		Timer								( TIMER_COMMAND command );
DARKSDK void				Begin								( void );
DARKSDK void				End									( void );
DARKSDK void				Render								( void );
DARKSDK IDirect3D9*			GetDirect3D							( void );
DARKSDK IDirect3DDevice9*	GetDirect3DDevice					( void );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		bool 				ConstructorDisplay					( void );
		void 				DestructorDisplay					( void );
		void 				SetErrorHandlerDisplay				( LPVOID pErrorHandlerPtr );
		void 				PassCoreDataDisplay					( LPVOID pGlobPtr, int iStage );
#endif

DARKSDK bool				Constructor 						( void );
DARKSDK void				Destructor  						( void );
DARKSDK void 				SetErrorHandler 					( LPVOID pErrorHandlerPtr );
DARKSDK void 				PassCoreData						( LPVOID pGlobPtr, int iStage );

DARKSDK bool 				SetDisplayDebugMode					( void );
DARKSDK bool 				SetDisplayMode						( int iWidth, int iHeight );
DARKSDK bool 				SetDisplayMode						( int iWidth, int iHeight, int iDepth );
DARKSDK void 				SetDisplayModeVSYNC					( int iWidth, int iHeight, int iDepth, int iVSyncOn );
DARKSDK void 				SetDisplayModeANTIALIAS				( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode );
DARKSDK void 				SetDisplayModeMODBACKBUFFER			( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode, int iBackbufferWidth, int iBackbufferHeight );
DARKSDK void				SetDisplayModeVR					( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode, int iBackbufferWidth, int iBackbufferHeight, int iActivateVRMode );
DARKSDK bool 				SetDisplayMode						( int iWidth, int iHeight, int iDepth, int iMode );
DARKSDK bool 				SetDisplayMode						( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing );
DARKSDK bool 				SetDisplayMode						( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing, int iLockable );
DARKSDK void 				RestoreDisplayMode  				( void );
DARKSDK void 				RestoreLostDevice	 				( void );

DARKSDK void 				AddSwapChain    					( HWND hwnd );
DARKSDK void 				StartSwapChain	 					( int iID );
DARKSDK void 				EndSwapChain    					( int iID );
DARKSDK void 				UpdateSwapChain 					( int iID );

DARKSDK void 				Clear    							( int   iR, int   iG, int   iB );
DARKSDK void 				GetGamma 							( int* piR, int* piG, int* piB );

DARKSDK bool 				CheckDisplayMode 					( int iWidth, int iHeight );
DARKSDK bool 				CheckDisplayMode 					( int iWidth, int iHeight, int iDepth, int iMode );
DARKSDK bool 				CheckDisplayMode 					( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing );

DARKSDK bool 				GetWindowedMode						( void );
DARKSDK int  				GetNumberOfDisplayModes				( void );
DARKSDK void 				GetDisplayMode						( int iID, char* szMode );
DARKSDK int  				GetNumberOfDisplayDevices			( void );
DARKSDK void 				SetDisplayDevice					( int iID );
DARKSDK void 				GetDeviceName						( int iID, char* szDevice );
DARKSDK void 				GetDeviceDriverName					( int iID, char* szDriver );

DARKSDK void 				SetDitherMode   					( int iMode );
DARKSDK void 				SetShadeMode    					( int iMode );
DARKSDK void 				SetLightMode    					( int iMode );
DARKSDK void 				SetCullMode     					( int iMode );
DARKSDK void 				SetSpecularMode 					( int iMode );
DARKSDK void 				SetRenderState  					( int iState, int iValue );

DARKSDK void 				OverrideHWND						( HWND hWnd );
DARKSDK void 				DisableWindowResize					( void );
DARKSDK void 				EnableWindowResize  				( void );
DARKSDK void 				UpdateWindowSize 					( int  iWidth,  int  iHeight  );
DARKSDK void 				GetWindowSize    					( int* piWidth, int* piHeight );

DARKSDK int   				CheckDisplayMode					( int iWidth, int iHeight, int iDepth );
DARKSDK int 				CheckDisplayModeANTIALIAS			( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode );

DARKSDK DWORD 				CurrentGraphicsCard					( DWORD pDestStr );
DARKSDK int   				EmulationMode 						( void );
DARKSDK void  				PerformChecklistForDisplayModes		( void );
DARKSDK void  				PerformChecklistForGraphicsCards	( void );
DARKSDK int   				GetDisplayType   					( void );
DARKSDK int   				GetDisplayWidth  					( void );
DARKSDK int   				GetDisplayHeight 					( void );
DARKSDK int   				GetWindowWidth  					( void );
DARKSDK int   				GetWindowHeight 					( void );
DARKSDK int   				GetDesktopWidth  					( void );
DARKSDK int   				GetDesktopHeight 					( void );
DARKSDK int   				GetDisplayDepth  					( void );
DARKSDK int   				GetDisplayFPS    					( void );
DARKSDK int   				GetDisplayInvalid 					( void );
DARKSDK void  				SetGamma		  					( int   iR, int   iG, int   iB );
DARKSDK void  				SetDisplayModeEx 					( int iWidth, int iHeight, int iDepth );
DARKSDK void  				SetEmulationOn   					( void );
DARKSDK void  				SetEmulationOff   					( void );
DARKSDK void  				SetGraphicsCard 					( DWORD dwCardname );

DARKSDK void  				SetWindowModeOn						( void );
DARKSDK void  				SetWindowModeOff					( void );
DARKSDK void  				SetWindowSettings					( int iStyle, int iCaption, int iIcon );
DARKSDK void 				SetWindowPosition					( int posx, int posy );
DARKSDK void  				SetWindowSize						( int sizex, int sizey );
DARKSDK void  				HideWindow							( void );
DARKSDK void				ShowWindow							( void );
DARKSDK void  				MinimiseWindow						( void );
DARKSDK void  				MaximiseWindow						( void );
DARKSDK void  				RestoreWindow						( void );
DARKSDK void  				SetWindowTitle						( DWORD pTitleString );
DARKSDK int   				WindowExist							( DWORD pTitleString );
DARKSDK void  				WindowToBack						( void );
DARKSDK void  				WindowToFront						( void );
DARKSDK void  				WindowToFront						( DWORD pTitleString );
DARKSDK void  				WindowToBack						( DWORD pTitleString );

DARKSDK void  				LockBackbuffer						( void );
DARKSDK void  				UnlockBackbuffer					( void );
DARKSDK DWORD 				GetBackbufferPtr					( void );
DARKSDK int   				GetBackbufferWidth					( void );
DARKSDK int   				GetBackbufferHeight					( void );
DARKSDK int   				GetBackbufferDepth					( void );
DARKSDK int   				GetBackbufferPitch					( void );

// lee - 130108 - added for X10 compat.
DARKSDK void				SetNvPerfHUD						( int iUsePerfHUD );
DARKSDK void				ForceAdapterOrdinal					( int iForceOrdinal );
DARKSDK void				SetCaptureName						( DWORD pFilename );
DARKSDK void				SetCaptureCodec						( DWORD pFilename );
DARKSDK void				SetCaptureSettings					( int iCompressed, int iFPS, int iWidth, int iHeight, int iThreaded, int iWait );
DARKSDK void				SetCaptureMode						( int iRecordVideo );
DARKSDK void				SaveScreenshot						( DWORD pFilename );
DARKSDK void				StartPlayback						( DWORD pFilename, float fSpeed );
DARKSDK void				UpdatePlayback						( void );
DARKSDK void				StopPlayback						( void );
DARKSDK int					PlaybackPlaying						( void );
DARKSDK int					GetSessionTerminate					( void );

// lee - 071108 - U71
DARKSDK void				TextureScreen						( int iImageNumber );
DARKSDK void				TextureScreen						( int iStageIndex, int iImageNumber );
DARKSDK void				SetScreenEffect						( int iEffectNumber );

// lee - 030414 - Reloaded
DARKSDK DWORD				GetDirectXVersion					( DWORD pDestStr );
DARKSDK int					GetDirectXRefreshRate					( void );

#ifdef DARKSDK_COMPILE
		int   				dbCheckDisplayMode					( int iWidth, int iHeight, int iDepth );
		char* 				dbCurrentGraphicsCard				( void );
		int   				dbEmulationMode 					( void );
		void  				dbPerformChecklistForDisplayModes	( void );
		void  				dbPerformChecklistForGraphicsCards	( void );
		int   				dbScreenType	   					( void );
		int   				dbScreenWidth  						( void );
		int   				dbScreenHeight 						( void );
		int   				dbScreenDepth  						( void );
		int   				dbScreenFPS    						( void );
		int   				dbScreenInvalid						( void );
		void  				dbSetGamma		  					( int   iR, int   iG, int   iB );
		void  				dbSetDisplayMode 					( int iWidth, int iHeight, int iDepth );
		void  				dbSetEmulationOn   					( void );
		void  				dbSetEmulationOff   				( void );
		void  				dbSetGraphicsCard 					( char* szCardname );

		void  				dbSetWindowOn						( void );
		void  				dbSetWindowOff						( void );
		void  				dbSetWindowLayout					( int iStyle, int iCaption, int iIcon );
		void 				dbSetWindowPosition					( int posx, int posy );
		void  				dbSetWindowSize						( int sizex, int sizey );
		void  				dbHideWindow						( void );
		void				dbShowWindow						( void );
		void  				dbMinimiseWindow					( void );
		void  				dbMaximiseWindow					( void );
		void  				dbRestoreWindow						( void );
		void  				dbSetWindowTitle					( char* pTitleString );
		int   				dbWindowExist						( char* pTitleString );
		void  				dbWindowToBack						( void );
		void  				dbWindowToFront						( void );
		void  				dbWindowToFront						( char* pTitleString );
		void  				dbWindowToBack						( char* pTitleString );

		void  				dbLockBackbuffer					( void );
		void  				dbUnlockBackbuffer					( void );
		DWORD 				dbGetBackbufferPtr					( void );
		int   				dbGetBackbufferWidth				( void );
		int   				dbGetBackbufferHeight				( void );
		int   				dbGetBackbufferDepth				( void );
		int   				dbGetBackbufferPitch				( void );

		IDirect3D9*			dbGetDirect3D						( void );
		IDirect3DDevice9*	dbGetDirect3DDevice					( void );

		void				dbBegin								( void );
		void				dbEnd								( void );
		void				dbRender							( void );
		void 				dbOverrideHWND						( HWND hWnd );

		bool 				dbSetDisplayDebugMode				( void );
		bool 				dbSetDisplayModeEx					( int iWidth, int iHeight );
		bool 				dbSetDisplayModeEx					( int iWidth, int iHeight, int iDepth );
		bool 				dbSetDisplayModeEx					( int iWidth, int iHeight, int iDepth, int iMode );
		bool 				dbSetDisplayModeEx					( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing );
		bool 				dbSetDisplayModeEx					( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing, int iLockable );

		// leeadd - 111108 - u71
		bool				dbSetDisplayModeVSync				( int iWidth, int iHeight, int iDepth, int iVSyncOn );
		bool				dbSetDisplayModeAntialias			( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode );

		void 				dbRestoreDisplayMode  				( void );
		void 				dbRestoreLostDevice	 				( void );

		void 				dbClear    							( int   iR, int   iG, int   iB );
		void 				dbGetGamma 							( int* piR, int* piG, int* piB );

		bool 				dbGetWindowedMode					( void );
		int  				dbGetNumberOfDisplayModes			( void );
		void 				dbGetDisplayMode					( int iID, char* szMode );
		int  				dbGetNumberOfDisplayDevices			( void );
		void 				dbSetDisplayDevice					( int iID );

		void 				dbSetDitherMode   					( int iMode );
		void 				dbSetShadeMode    					( int iMode );
		void 				dbSetLightMode    					( int iMode );
		void 				dbSetCullMode     					( int iMode );
		void 				dbSetSpecularMode 					( int iMode );
		void 				dbSetRenderState  					( int iState, int iValue );

		// lee - 300706 - GDK fixes
		void  				dbMinimizeWindow					( void );
		void  				dbMaximizeWindow					( void );

		// lee - 081108 - fix
		int					dbDesktopWidth						( void );
		int					dbDesktopHeight						( void );

		// lee - 071108 - U71
		void				dbTextureScreen						( int iImageNumber );
		void				dbTextureScreen						( int iStageIndex, int iImageNumber );
		void				dbSetScreenEffect					( int iEffectNumber );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CGFX_H_
