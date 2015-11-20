#define _CRT_SECURE_NO_DEPRECATE
#include "cGFXc.h"	// include GFX header
#include "..\..\DarkSDK\Setup\resource.h"
#include <delayimp.h>
#include <iostream> // can use STD:: with this include
#include <vector>

// For DXDIAG version extraction
#define INITGUID
#include <dxdiag.h>

// U70 - 170608 - WOW Support (from SDK folder C:\TGC\SDK\WOWvxPlayerAPI\lib)
//#define WOWSUPPORT // comment back into code when building new WOW SETUP.DLL
#ifdef WOWSUPPORT
#include "WOWvxPlayerAPI.h"
WOWvxPlayerHandle g_WOWhandle = NULL;
DWORD m_imageWidth = 0;
DWORD m_imageHeight = 0;
DWORD m_textureWidth = 0;
DWORD m_textureHeight = 0;
#pragma comment ( lib, "WOWvxPlayerAPI.lib" )
#endif

// U74 BETA9 - new code to support VR920 (for educational FPSC and other fun things)
// U75 requires iWear VR920 SDK from VUZIX (free SDK)
// U76 Temp disable VR920 for GDK
#ifndef DARKSDK_COMPILE
//#define VR920SUPPORT // static linking and small footprint means we can support this
#endif

#ifdef VR920SUPPORT
// Includes
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
//#include "VR920\IWRsdk.h"
// Enumerations
#define						EXIT_STEREOSCOPIC_MODE	E_FAIL
enum						{ IWR_NOT_CONNECTED=-1, IWR_IS_CONNECTED };
enum						{ LEFT_EYE=0, RIGHT_EYE, MONO_EYES };
enum						{ NO_FILTERING=0, APPLICATION_METHOD, DRIVER_METHOD };
// Prototypes and externs
UINT						IWRIsVR920Connected( bool *PrimaryDevice );
HRESULT						D3DWindowedWaitForVsync( void );
extern void					IWRFilterTracking( long *yaw, long *pitch, long *roll );
// Globals
bool						g_VR920StereoMethod = false;
bool						g_VR920AdapterAvailable = false;
UINT						g_VR920Adapter	= D3DADAPTER_DEFAULT;
bool						g_StereoEnabled	= true;
HANDLE						g_StereoHandle	= INVALID_HANDLE_VALUE;
UINT						g_StereoEyeToggle = LEFT_EYE;
LPDIRECT3DQUERY9			g_pLeftEyeQuery	= NULL;
LPDIRECT3DQUERY9			g_pRightEyeQuery = NULL;
D3DCAPS9					g_d3dcaps;
#endif

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCore.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKText.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
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
#endif

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef void ( *DLL_D3DRefreshFunctionCall ) ( int );
typedef void ( *DLL_D3DDeviceLostFunctionCall ) ( void );
typedef void ( *DLL_D3DDeviceNotResetFunctionCall ) ( void );
typedef void ( *DLL_D3DDeviceResetFunctionCall ) ( void );
typedef IDirect3DTexture9* ( *DLL_D3DGetStereoFunctionCall ) ( void );
typedef void ( *DLL_Basic3DTextureScreen ) ( int, int );
typedef void ( *DLL_Basic3DSetScreenEffect ) ( int );
typedef void ( *DLL_InputSetupX ) ( DWORD );

DBPRO_GLOBAL tagInfo*						m_pInfo;			// gfx card information
DBPRO_GLOBAL int							m_iAdapterCount;	// number of graphic cars
DBPRO_GLOBAL int							m_iAdapterUsed;		// graphics card being used by number
DBPRO_GLOBAL char							m_pAdapterName[_MAX_PATH];// graphica card being used by name 
DBPRO_GLOBAL bool							m_bDraw;			// flag to start a scene
DBPRO_GLOBAL bool							m_iDisplayChange;	// set when display has been switched
DBPRO_GLOBAL HWND							m_hWnd;				// handle to main window

DBPRO_GLOBAL bool							m_bOverrideHWND;	// should we override the window
DBPRO_GLOBAL bool							m_bResizeWindow;	// should we resize the window
DBPRO_GLOBAL bool							m_bFirstTimeIGLSkip;// used for igLoader code

DBPRO_GLOBAL D3DPRESENT_PARAMETERS*			m_D3DPP				= NULL;
DBPRO_GLOBAL int							m_iWidth;			// width of display mode
DBPRO_GLOBAL int							m_iHeight;			// height of display mode
DBPRO_GLOBAL int							m_iChopWidth;		// chop width of display mode (for nonstandard setdisplaymode sizes)
DBPRO_GLOBAL int							m_iChopHeight;		// chop height of display mode
DBPRO_GLOBAL int							m_iDisplayType;		// windowed / fullscreen
DBPRO_GLOBAL int							m_iProcess;			// vertex processing mode
DBPRO_GLOBAL int							m_iDepth;			// depth of display
DBPRO_GLOBAL int							m_iBackBufferCount;	// back buffer count
DBPRO_GLOBAL bool							m_bLockable;		// lockable mode

#ifdef DARKSDK_COMPILE
	// MIKE 240303 - VSYNC ON BY DEFAULT
	DBPRO_GLOBAL bool						m_bVSync = true;
#else
	// lee - 230206 - u60 - legacy behaviour is off (SYNC RATE 0) - use SET DISPLAY MODE W,H,D,vsync=1 to enable vsync
	// lee - 270306 - u6b5 - users want legacy of vsync ON, so this is made so (=true)
	DBPRO_GLOBAL bool						m_bVSync = true;
#endif

DBPRO_GLOBAL int							m_iVSyncInterval = 1;
DBPRO_GLOBAL bool							m_bVSyncInitialDefault = false;
DBPRO_GLOBAL int							m_iMultisamplingFactor = 0;
DBPRO_GLOBAL int							m_iMultimonitorMode = 0;
DBPRO_GLOBAL int							m_iModBackbufferWidth = 0;
DBPRO_GLOBAL int							m_iModBackbufferHeight = 0;
DBPRO_GLOBAL UINT							m_uAdapterChoice = D3DADAPTER_DEFAULT;
DBPRO_GLOBAL bool							m_bNVPERFHUD = false;
DBPRO_GLOBAL int							m_iForceAdapterOrdinal = 0;

DBPRO_GLOBAL D3DFORMAT						m_Depth;			// final back bufferformat
DBPRO_GLOBAL D3DFORMAT						m_StencilDepth;		// final stencil buffer format
DBPRO_GLOBAL DWORD							m_dwFlags;			// flags
DBPRO_GLOBAL D3DDISPLAYMODE					m_WindowsD3DMODE;
DBPRO_GLOBAL D3DFORMAT						g_d3dFormat;		// dx9 adapter format of choice

DBPRO_GLOBAL bool							m_bZBuffer;			// ZBuffer present
DBPRO_GLOBAL D3DSWAPEFFECT					m_SwapMode;			// Whether in COPY or FLIP Mode

DBPRO_GLOBAL int							m_iWindowWidth;		// width of window
DBPRO_GLOBAL int							m_iWindowHeight;	// window height

DBPRO_GLOBAL LPDIRECT3DSWAPCHAIN9			m_pSwapChain [ MAX_SWAP_CHAINS ];
DBPRO_GLOBAL int							m_iSwapChainCount;

DBPRO_GLOBAL int							m_iGammaRed;
DBPRO_GLOBAL int							m_iGammaBlue;
DBPRO_GLOBAL int							m_iGammaGreen;

DBPRO_GLOBAL LPDIRECT3DSURFACE9				g_pBackBuffer = NULL;	//globals for locking backbuffer (commands)
DBPRO_GLOBAL DWORD							g_dwSurfacePtr = 0;
DBPRO_GLOBAL DWORD							g_dwSurfaceWidth = 0;
DBPRO_GLOBAL DWORD							g_dwSurfaceHeight = 0;
DBPRO_GLOBAL DWORD							g_dwSurfaceDepth = 0;
DBPRO_GLOBAL DWORD							g_dwSurfacePitch = 0;

DBPRO_GLOBAL bool							gbScreenBecomeInvalid=false;	// globals for fine windows control (commands)
DBPRO_GLOBAL bool							gbWindowMode=false;
DBPRO_GLOBAL bool							gbWindowBorderActive=true;
DBPRO_GLOBAL DWORD							gWindowSizeX=0;
DBPRO_GLOBAL DWORD							gWindowSizeY=0;
DBPRO_GLOBAL DWORD							gWindowExtraX=0;
DBPRO_GLOBAL DWORD							gWindowExtraY=0;
DBPRO_GLOBAL DWORD							gWindowExtraXForOverlap=0;
DBPRO_GLOBAL DWORD							gWindowExtraYForOverlap=0;
DBPRO_GLOBAL DWORD							gWindowVisible=SW_SHOWDEFAULT;
DBPRO_GLOBAL char							gWindowName[256];
DBPRO_GLOBAL DWORD							gWindowStyle=WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU;
DBPRO_GLOBAL HICON							gOriginalIcon=NULL;
DBPRO_GLOBAL HICON							gWindowIconHandle=NULL;
DBPRO_GLOBAL bool							gbFirstInitOfDisplayOnly=true;

//Dave - to allow true pixel representation when rendering to a child window.
DBPRO_GLOBAL DWORD							g_dwChildWindowTruePixel = 0;

//DBPRO_GLOBAL bool							g_bCreateChecklistNow=false;
//DBPRO_GLOBAL DWORD							g_dwMaxStringSizeInEnum=0;
//DBPRO_GLOBAL char							m_pWorkString[_MAX_PATH];
static bool							g_bCreateChecklistNow=false;
static DWORD							g_dwMaxStringSizeInEnum=0;
static char							m_pWorkString[_MAX_PATH];

DBPRO_GLOBAL bool							g_bValidFPS = true;
DBPRO_GLOBAL bool							g_bSceneBegun = false;

DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Text_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Basic2D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Sprites_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Image_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Input_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_System_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Memblocks_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Bitmap_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Animation_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Multiplayer_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Basic3D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Camera3D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Matrix3D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Light3D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_World3D_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Particles_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_PrimObject_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Vectors_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_XObject_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_3DSObject_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_MDLObject_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_MD2Object_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_MD3Object_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Sound_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Music_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_LODTerrain_RefreshD3D;
DBPRO_GLOBAL DLL_D3DRefreshFunctionCall		g_Q2BSP_RefreshD3D;

// U70 - 180608 - new auto stereoscopic feature
DBPRO_GLOBAL DLL_D3DGetStereoFunctionCall	g_Camera_GetStereoscopicFinalTexture;
DBPRO_GLOBAL LPDIRECT3DSURFACE9				g_pBackBufferRenderTarget = NULL; // U7.0 - filled when backbuffer created (used by WOW renderer to set target)

// U71 - 071108 - basic3D calls to assign screemn effect and texture
DBPRO_GLOBAL DLL_Basic3DTextureScreen		g_Basic3D_TextureScreen			= 0;
DBPRO_GLOBAL DLL_Basic3DSetScreenEffect		g_Basic3D_SetScreenEffect		= 0;

// 200213 - Add ability to change HWND of input targets
DBPRO_GLOBAL DLL_InputSetupX				g_Input_SetupKeyboard			= 0;
DBPRO_GLOBAL DLL_InputSetupX				g_Input_SetupMouse				= 0;

DBPRO_GLOBAL LPDIRECT3D9					m_pD3D							= NULL;		// interface to D3D
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3DDevice					= NULL;		// D3D device
DBPRO_GLOBAL LPDIRECT3DTEXTURE9				g_pDemoTexture					= NULL;

// leefix - 131108 - DarkGDK does not want another global instance of this
#ifdef DARKSDK_COMPILE
	static PTR_FuncCreateStr				g_pCreateDeleteStringFunction	= NULL;
#else
	DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;
	DBPRO_GLOBAL PTR_FuncCreateStr			g_pCreateDeleteStringFunction	= NULL;
#endif

DBPRO_GLOBAL HWND							g_OldHwnd						= NULL;
DBPRO_GLOBAL bool							g_bWindowOverride				= false;
DBPRO_GLOBAL char							g_szMainWindow  [ MAX_PATH ];
DBPRO_GLOBAL char							g_szChildWindow [ MAX_PATH ];

#if DEBUG_MODE
	DBPRO_GLOBAL FILE*	m_fp;
#endif

// mike - 070207 - need a way of overriding the present mode to draw to custom
//			     - areas, the globals are placed in a namespace as a simple way
//			     - of avoiding conflicts between the libraries
namespace DisplayLibrary
{
	bool					g_bCustomPresentMode;
	std::vector < RECT >	g_CustomPresentRectangles;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



DARKSDK GlobStruct* GetGlobalData ( void )
{
	return g_pGlob;
}

static LONG WINAPI DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pep, std::string& strError)
{
	// Structured Exception Handler for delay loaded DLLs
	// If this is a Delay-load problem, ExceptionInformation[0] points 
	// to a DelayLoadInfo structure that has detailed error info
	PDelayLoadInfo pdli = PDelayLoadInfo(pep->ExceptionRecord->ExceptionInformation[0]);
	char szBuff[512];
	switch(pep->ExceptionRecord->ExceptionCode)
	{
	case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
		_snprintf(szBuff, sizeof(szBuff), "Could not find \"%s\"", pdli->szDll);
		strError = szBuff;
		return EXCEPTION_EXECUTE_HANDLER;

	case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
		// The DLL was found but it doesn't contain the function
		if(pdli->dlp.fImportByName)
		{
			_snprintf(szBuff, sizeof(szBuff), "Could not find \"%s\" in \"%s\"",
				pdli->dlp.szProcName, pdli->szDll);
		}
		else
		{
			_snprintf(szBuff, sizeof(szBuff), "Could not find function ordinal %d in \"%s\"",
				pdli->dlp.dwOrdinal, pdli->szDll);
		}
		strError = szBuff;
		return EXCEPTION_EXECUTE_HANDLER;

	default:
		// We don't recognize this exception
  		return EXCEPTION_CONTINUE_SEARCH;
		break;
	}
}

std::string strInitD3DXError;
static bool InitD3DX ( void )
{
	bool bRet = true;
	__try
	{
		// Load D3DX DLL by calling one of its functions
		D3DXMATRIXA16 m1, m2, m3;
		memset(&m1, 0, sizeof(m1));
		memset(&m2, 0, sizeof(m2));
		D3DXMatrixMultiply(&m3, &m1, &m2);
	}
	__except (DelayLoadDllExceptionFilter(GetExceptionInformation(), strInitD3DXError))
	{
		bRet = false;
	}
	return bRet;
}

DARKSDK bool Constructor ( void )
{
	// point pointers to nothing
	m_pD3D			 = NULL;	// set d3d to null
	m_pD3DDevice	 = NULL;	// set d3d device to null

	// setup default values
	//m_iDisplayType   = DEFAULT;
	m_iDisplayType   = 1;
	m_iDisplayChange = false;
	m_bOverrideHWND  = false;
	m_bResizeWindow  = true;

	// mike - 070207 - ensure this is false by default
	DisplayLibrary::g_bCustomPresentMode = false;

	m_Depth = D3DFMT_UNKNOWN;
	m_StencilDepth = D3DFMT_UNKNOWN;

	#if DEBUG_MODE
			fwrite ( 
						"* Constructor - attempting to create Direct3D interface\n", 
						strlen ( "* Constructor - attempting to create Direct3D interface\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif
	// leeadd - 041108 - Load D3DX (eed to provoke D3DX DLL to load in, so we call a D3DX command that does not need m_pD3D)
	bool bD3DXAvailable = false;
	if( InitD3DX() ) bD3DXAvailable = true;

	//-------------------------------------------------------------------------
    // DirectX 9 Check
	//-------------------------------------------------------------------------
	//leefix-50803-no good putting check in DLL that links and needs d3dx9.dll!
	//this check is now done inside the EXE itself!!
	/* this clearly does not work!
	HMODULE hDPNHPASTDLL = LoadLibrary( "dpnhpast.dll" );
    if( hDPNHPASTDLL == NULL )
    {
		MessageBox(NULL, "This application requires DirectX 9 or above.", "DirectX Error", MB_OK | MB_ICONERROR);
		return false;
    }
    FreeLibrary( hDPNHPASTDLL );
	*/

	// setup direct3d
	if ( FAILED ( m_pD3D = Direct3DCreate9 ( D3D_SDK_VERSION ) ) || bD3DXAvailable==false )
	{
		#if DEBUG_MODE
			fwrite ( 
						"* Constructor - failed to create Direct3D interface\n", 
						strlen ( "* Constructor - failed to create Direct3D interface\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
		#endif

		Error ( "Unable to create Direct3D interface" );
	}
	#if DEBUG_MODE
		else
		{
			fwrite ( 
						"* Constructor - created Direct3D interface\n", 
						strlen ( "* Constructor - created Direct3D interface\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
		}
	#endif

	#if DEBUG_MODE
			fwrite ( 
						"* Constructor - get number of adapters\n", 
						strlen ( "* Constructor - get number of adapters\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	// obtain hardware information
	m_iAdapterCount = m_pD3D->GetAdapterCount ( );

	#if DEBUG_MODE
		if ( m_iAdapterCount == 0 )
		{
			fwrite ( 
						"* Constructor - failed to find any adapters\n", 
						strlen ( "* Constructor - failed to find any adapters\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
		}
	#endif

	#if DEBUG_MODE
			fwrite ( 
						"* Constructor - allocate adapter storage memory\n", 
						strlen ( "* Constructor - allocate adapter storage memory\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	// setup adapter storage
	m_pInfo = new tagInfo [ m_iAdapterCount ];

	if ( !m_pInfo )
	{
		#if DEBUG_MODE
			fwrite ( 
						"* Constructor - unable to allocate adapter storage memory\n", 
						strlen ( "* Constructor - unable to allocate adapter storage memory\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
		#endif

		Error ( "Failed to allocate memory for adapter information" );
	}

	memset ( m_pInfo, 0, sizeof ( m_pInfo ) * m_iAdapterCount );

	#if DEBUG_MODE
			fwrite ( 
						"* Constructor - get foreground window\n", 
						strlen ( "* Constructor - get foreground window\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	m_hWnd = GetForegroundWindow ( );

	#if DEBUG_MODE
		if ( !m_hWnd )
		{
			fwrite ( 
						"* Constructor - invalid window handle\n", 
						strlen ( "* Constructor - invalid window handle\n" ) * sizeof ( char ),
						1,
						m_fp
				   );

		}
	#endif

	m_iSwapChainCount = 0;

	m_iGammaRed   = 255;
	m_iGammaGreen = 255;
	m_iGammaBlue  = 255;

	#if DEBUG_MODE
			fwrite ( 
						"* Constructor - call FindHardwareInfo\n", 
						strlen ( "* Constructor - call FindHardwareInfo\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	FindHardwareInfo ( );

	#if DEBUG_MODE
		fwrite ( "** end Constructor\n", strlen ( "** end Constructor\n" ) * sizeof ( char ), 1, m_fp );
	#endif

	// Mike, not sure where you set the adapter so default is zero
	m_iAdapterUsed=0;		// graphics card being used by number
	strcpy(m_pAdapterName, m_pInfo [ m_iAdapterUsed ].szName);

	// Default Window Settings for Windows Control
	gbWindowMode=true;
	gbWindowBorderActive=false;
	strcpy(gWindowName,"");
	gWindowVisible=SW_SHOWDEFAULT;
	gWindowSizeX = m_iWidth;
	gWindowSizeY = m_iHeight;
	gWindowIconHandle = NULL;
	gWindowExtraX = gWindowExtraXForOverlap;
	gWindowExtraY = gWindowExtraYForOverlap;
	gWindowStyle = WS_OVERLAPPEDWINDOW;
	gbFirstInitOfDisplayOnly=true;

	// Get Window Display Mode (for later use for windows-modes)
	m_pD3D->GetAdapterDisplayMode ( 0, &m_WindowsD3DMODE );

	// mike - 010904 - 5.7 - disable screen saver at start of program
	SystemParametersInfo ( SPI_SETSCREENSAVEACTIVE, FALSE, 0, SPIF_SENDCHANGE );

	// U70 - 170608 - WOW Support
	#ifdef WOWSUPPORT
	if (FAILED(WOWvxPlayerCreate( &g_WOWhandle )))
	{
		g_WOWhandle = NULL;
	}
	#endif

	// Success
	return true;
}

#ifdef VR920SUPPORT
//-----------------------------------------------------------------------------
// Function will find the VR920 Adapter and return code if found.
//  IWR_NOT_CONNECTED:  VR920 Not found.
//  IWR_IS_CONNECTED:   VR920 is connected to an adapter in the system.
//-----------------------------------------------------------------------------
UINT IWRIsVR920Connected( bool *PrimaryDevice )
{
	DISPLAY_DEVICE	DisplayDevice, MonitorDevice;
	// Step 1: Find VR920 Device+Adapter
	*PrimaryDevice = false;
	DisplayDevice.cb = MonitorDevice.cb = sizeof( DISPLAY_DEVICE );
	for( int i=0; EnumDisplayDevices(NULL, i, &DisplayDevice, 0x0 ); i++ )
	{
		// Ignore mirrored devices and only look at desktop attachments.
		if( !(DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) &&
			(DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) )
		{
			for( int j=0; EnumDisplayDevices(DisplayDevice.DeviceName, j, &MonitorDevice, 0x0 ); j++ )
			{
				if( (MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
					(strstr( MonitorDevice.DeviceID, "IWR0002" ) ||  // VR920 id
					 strstr( MonitorDevice.DeviceID, "IWR0149" ))) { // Wrap920 id
					// Found the VR920 PnP id.
					if( DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) 
						*PrimaryDevice = true; // VR920 is the primary display device.
					return IWR_IS_CONNECTED;
				}
			}
		}
	}
	// VR920 does not appear to be on any accessible adapters.
	return IWR_NOT_CONNECTED;

	/* old one
	// Step 1: Find VR920 Device+Adapter
	*PrimaryDevice = false;
	DisplayDevice.cb = MonitorDevice.cb = sizeof( DISPLAY_DEVICE );
	for( int i=0; EnumDisplayDevices(NULL, i, &DisplayDevice, 0x0 ); i++ ) {
		// Ignore mirrored devices and only look at desktop attachments.
		if( !(DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) &&
			(DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ) {
			for( int j=0; EnumDisplayDevices(DisplayDevice.DeviceName, j, &MonitorDevice, 0x0 ); j++ ){
				if( (MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
					strstr( MonitorDevice.DeviceID, "IWR0002" ) ) {
					// Found the VR920 PnP id.
					if( DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) 
						*PrimaryDevice = true; // VR920 is the primary display device.
					return IWR_IS_CONNECTED;
					}
				}
			}
		}
	// VR920 does not appear to be on any accessible adapters.
	return IWR_NOT_CONNECTED;
	*/
}
//-----------------------------------------------------------------------------
// Provide for a method in (Windowed Mode) to poll the adapters Vertical Sync function.
//  This is a requirement for the VR920s VGA to Frame syncronization process AND
//  MUST be available for highest possible performance in windowed mode.
//-----------------------------------------------------------------------------
HRESULT	D3DWindowedWaitForVsync( void )
{
	D3DRASTER_STATUS	rStatus;
	HRESULT				hr=E_FAIL;

	if( g_d3dcaps.Caps & D3DCAPS_READ_SCANLINE )
	{
		// IF Polling VSync is available:
		hr = m_pD3DDevice->GetRasterStatus( 0, &rStatus );
		if( hr == S_OK )
		{
			// In rare case when in vblank, Wait for vblank to drop.
			while( rStatus.InVBlank )
				m_pD3DDevice->GetRasterStatus( 0,&rStatus );
			while( rStatus.ScanLine < (UINT)(m_iWindowHeight-1) )
			{
				m_pD3DDevice->GetRasterStatus( 0,&rStatus );
				// if scan ever crossed vblank again; break. could be issue with window transitioning modes.
				if( rStatus.InVBlank )
					break;
			}
		}
	}
	// IF Polling VSync is NOT available:
	if( hr != S_OK ) Sleep( 10 );
	return S_OK;
}
//-----------------------------------------------------------------------------
// Provide for a process syncronize a rendered left or right eye to the VR920s frame
// Buffers.  Common to both Windowed and FullScreen modes.
//-----------------------------------------------------------------------------
HRESULT	IWRSyncronizeEye( int Eye )
{
	HRESULT		hr=S_OK;
	// Wait for acknowledgement from previous frame, to present a new left eye.
	IWRSTEREO_WaitForAck( g_StereoHandle, Eye );
	// Windowed mode: Wait for the start of Vsync prior to presenting.
	//// if( m_iDisplayType != FULLSCREEN ) D3DWindowedWaitForVsync(); MAKES IT STUTTER!!
	// Ensure GPU is ready. 
	// If using the locking method nows the time to wait for a lock on the backbuffer.
	// Put the frame in the queue(full screen). or immediate copy(windowed mode).
	hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
	if( FAILED(hr) ) return EXIT_STEREOSCOPIC_MODE;
	// If using the Query GPU method. wait for GPU now.
	if( Eye == LEFT_EYE )
	{
		// Wait for Left eye GPU status ok.
		if ( g_pLeftEyeQuery )
			while(S_FALSE == g_pLeftEyeQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ));
	}
	else
	{
		// Wait for Right eye GPU status ok.
		if ( g_pRightEyeQuery )
			while(S_FALSE == g_pRightEyeQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ));
	}
	// Signal to the VR920 the next eyes frame is available
	// AND: Will scan out on the NEXT Vsync interval.
	if( !IWRSTEREO_SetLR( g_StereoHandle, Eye ) ) return EXIT_STEREOSCOPIC_MODE;
	return S_OK;
}
#endif

DARKSDK void LoadDemoLogo(void)
{
	#ifdef DEMO
	D3DXCreateTextureFromResourceEx 
									(
									  m_pD3DDevice,
									  g_pGlob->g_GFX,
									  MAKEINTRESOURCE ( IDB_BITMAP2 ),
									  D3DX_DEFAULT,
									  D3DX_DEFAULT,
									  1,
									  0,
									  D3DFMT_UNKNOWN,
									  D3DPOOL_DEFAULT,
									  D3DX_FILTER_NONE,
									  D3DX_DEFAULT,
									  0xFF000000,
									  NULL,
									  NULL,
									  &g_pDemoTexture
									);
	#endif
}

DARKSDK void ShowDemoLogo(void)
{
}

DARKSDK void GetBackBufferPointers(void)
{
	if(g_pGlob)
	{
		if(g_pGlob->pHoldBackBufferPtr) g_pGlob->pHoldBackBufferPtr->Release();
		if(g_pGlob->pHoldDepthBufferPtr) g_pGlob->pHoldDepthBufferPtr->Release();
		m_pD3DDevice->GetRenderTarget(0, &g_pGlob->pHoldBackBufferPtr);
		m_pD3DDevice->GetDepthStencilSurface(&g_pGlob->pHoldDepthBufferPtr);
		m_pD3DDevice->SetRenderTarget(0, g_pGlob->pHoldBackBufferPtr);
		m_pD3DDevice->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
		g_pGlob->pCurrentBitmapSurface=g_pGlob->pHoldBackBufferPtr;
		g_pGlob->iCurrentBitmapNumber=0;
	}

	// Load demo logo
	LoadDemoLogo();
}

DARKSDK void ReleaseBackBufferPointers(void)
{
	// U74 - BETA9 - 280609 - release VR920 if used
	#ifdef VR920SUPPORT
	if ( g_VR920StereoMethod==true )
	{
		// free D3D resources
		SAFE_RELEASE( g_pLeftEyeQuery );
		SAFE_RELEASE( g_pRightEyeQuery );
	}
	#endif

	// Free demo logo
	SAFE_RELEASE(g_pDemoTexture);
	g_pDemoTexture=NULL;

	// Free ref to backbuffer
	if(g_pGlob)
	{
		if(g_pGlob->pHoldBackBufferPtr)
		{
			g_pGlob->pHoldBackBufferPtr->Release();
			g_pGlob->pHoldBackBufferPtr=NULL;
		}
		if(g_pGlob->pHoldDepthBufferPtr)
		{
			g_pGlob->pHoldDepthBufferPtr->Release();
			g_pGlob->pHoldDepthBufferPtr=NULL;
		}
	}

	// Free locked surface
	if(g_dwSurfacePtr)
	{
		g_pBackBuffer->UnlockRect();
		g_dwSurfacePtr=NULL;
	}

	// Release backbuffer
	SAFE_RELEASE(g_pBackBuffer);
}

DARKSDK void Destructor ( void ) 
{
	// U74 - BETA9 - 280609 - release VR920 if used
	#ifdef VR920SUPPORT
	if ( g_VR920StereoMethod==true )
	{
		// free VR920 driver resources
		if ( g_StereoHandle )
		{
			IWRSTEREO_WaitForAck( g_StereoHandle, 0 );
			IWRSTEREO_SetStereo( g_StereoHandle, IWR_MONO_MODE );
			IWRSTEREO_Close( g_StereoHandle );
			IWRFreeDll();
			g_StereoHandle=NULL;
		}
	}
	#endif

	// U70 - 170608 - WOW Support
	#ifdef WOWSUPPORT
    if (g_WOWhandle != NULL)
    {
        // Exit the WOWvxPlayerAPI
        WOWvxPlayerExit( g_WOWhandle );
        g_WOWhandle = NULL;
    }
	#endif

	// Free ref to backbuffer
	ReleaseBackBufferPointers();

	// release all previously allocated memory
	#if DEBUG_MODE
			fwrite ( 
						"\n** begin Destructor\n", 
						strlen ( "\n** begin Destructor\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	// Delete global property-desc of device
	if ( m_D3DPP )
	{
		delete m_D3DPP;
		m_D3DPP=NULL;
	}

	// clear up D3D
	SAFE_RELEASE ( m_pD3DDevice );	// release the device
	SAFE_RELEASE ( m_pD3D       );	// release the interface

	// remove any adapter information
	for ( int iTemp = 0; iTemp < m_iAdapterCount; iTemp++ )
	{
		SAFE_DELETE_ARRAY ( m_pInfo [ iTemp ].szName     );	// clear out name
		SAFE_DELETE_ARRAY ( m_pInfo [ iTemp ].D3DDisplay );	// clear out display structure
	}

	// finally get rid of the adapter info array
	SAFE_DELETE_ARRAY ( m_pInfo );

	#if DEBUG_MODE
			fwrite ( 
						"** end Destructor\n", 
						strlen ( "** end Destructor\n" ) * sizeof ( char ),
						1,
						m_fp
				   );

		fclose ( m_fp );
	#endif

	// mike - 010904 - 5.7 - enable screen saver at end of program
	SystemParametersInfo ( SPI_SETSCREENSAVEACTIVE, TRUE, 0, SPIF_SENDCHANGE );
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr ) 
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK BOOL CALLBACK EnumChildProc ( HWND hwnd, LPARAM lParam )
{
	char szBuffer [ MAX_PATH ];
	GetWindowText ( hwnd, szBuffer, MAX_PATH );
	if ( strcmp ( szBuffer, g_szChildWindow ) == 0 )
	{
		g_pGlob->hWnd     = hwnd;
		m_hWnd            = hwnd;
		g_bWindowOverride = true;
	}
	return TRUE;
}

DARKSDK BOOL CALLBACK EnumWindowsProc ( HWND hwnd, LPARAM lParam )
{
	char szBuffer [ MAX_PATH ];
	GetWindowText ( hwnd, szBuffer, MAX_PATH );
	int iResult = strspn ( szBuffer, g_szMainWindow );
	if ( iResult )
		EnumChildWindows ( hwnd, EnumChildProc, 0 );

	return TRUE;
}

DARKSDK void AttachWindowToChildOfAnother ( LPSTR pAbsoluteAppFilename )
{
	if ( !pAbsoluteAppFilename )
		return;

	// find external settings file (230105 - added >4 detect as it corrupted stack before in release mode)
	char pAppExtFile[_MAX_PATH];
	if ( strlen ( pAbsoluteAppFilename ) > 4 )
	{
		// myprog.exe becomes myprog.ini in pAppExtFile
		strcpy ( pAppExtFile, pAbsoluteAppFilename );
		strcpy ( pAppExtFile + strlen(pAppExtFile) - 4, ".ini" );
	}
	else
		return;

	// clear strings
	strcpy ( g_szMainWindow, "" );
	strcpy ( g_szChildWindow, "" );

	// read data from settings file
	GetPrivateProfileString ( "External", "Main Window", "", g_szMainWindow, MAX_PATH, pAppExtFile );
	GetPrivateProfileString ( "External", "Child Window", "", g_szChildWindow, MAX_PATH, pAppExtFile );

	// determine if window should be attached to child of another
	if ( strlen ( g_szMainWindow ) > 1 )
		EnumWindows ( EnumWindowsProc, 0 );
}

DARKSDK void PassCoreData ( LPVOID pGlobPtr, int iStage ) 
{
	// Held in Core, used here..
	switch(iStage)
	{
		case 0 :	// Constructor Phase
					g_pGlob = (GlobStruct*)pGlobPtr;

					// U70 - 180608 - new auto stereoscopic feature
					if(g_pGlob->g_Camera3D) g_Camera_GetStereoscopicFinalTexture = ( DLL_D3DGetStereoFunctionCall ) GetProcAddress ( g_pGlob->g_Camera3D, "?GetStereoscopicFinalTexture@@YAPAUIDirect3DTexture9@@XZ" );

					// U71 - 071108 - get ptrs to Basic3D functions (for screen effect post processing)
					if(g_pGlob->g_Basic3D) g_Basic3D_TextureScreen = ( DLL_Basic3DTextureScreen ) GetProcAddress ( g_pGlob->g_Basic3D, "?TextureScreen@@YAXHH@Z" );
					if(g_pGlob->g_Basic3D) g_Basic3D_SetScreenEffect = ( DLL_Basic3DSetScreenEffect ) GetProcAddress ( g_pGlob->g_Basic3D, "?SetScreenEffect@@YAXH@Z" );

					// 200213 - Add ability to change HWND of input targets
					if(g_pGlob->g_Input) g_Input_SetupKeyboard = ( DLL_InputSetupX ) GetProcAddress ( g_pGlob->g_Input, "?SetupKeyboardEx@@YAXK@Z" );
					if(g_pGlob->g_Input) g_Input_SetupMouse = ( DLL_InputSetupX ) GetProcAddress ( g_pGlob->g_Input, "?SetupMouseEx@@YAXK@Z" );

					// Store old window handle
					g_OldHwnd = g_pGlob->hWnd;

					// Make this window child of another (if applicable)
					AttachWindowToChildOfAnother ( (LPSTR)g_pGlob->ppEXEAbsFilename );

					// done
					break;

		case 1 :	// Post-Device-Creation Phase
					g_pGlob = (GlobStruct*)pGlobPtr;
					g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;

					// Clear device now
					Clear(0,0,0);

					// Update ptr to backbuffer
					GetBackBufferPointers();

					break;
	}
}

DARKSDK void BuildFunctionsForDLLRefresh(void)
{
	// Find and assign function calls from active DLLs
	#ifndef DARKSDK_COMPILE
	if(g_pGlob)
	{
		if(g_pGlob->g_Text) g_Text_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Text, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Basic2D) g_Basic2D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Basic2D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Sprites) g_Sprites_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Sprites, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Image) g_Image_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Image, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Input) g_Input_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Input, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_System) g_System_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_System, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Memblocks) g_Memblocks_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Memblocks, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Bitmap) g_Bitmap_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Bitmap, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Animation) g_Animation_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Animation, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Multiplayer) g_Multiplayer_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Multiplayer, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Basic3D) g_Basic3D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Basic3D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Camera3D) g_Camera3D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Camera3D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Matrix3D) g_Matrix3D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Matrix3D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Light3D) g_Light3D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Light3D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_World3D) g_World3D_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_World3D, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Particles) g_Particles_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Particles, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_PrimObject) g_PrimObject_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_PrimObject, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Vectors) g_Vectors_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Vectors, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_XObject) g_XObject_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_XObject, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_3DSObject) g_3DSObject_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_3DSObject, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_MDLObject) g_MDLObject_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_MDLObject, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_MD2Object) g_MD2Object_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_MD2Object, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_MD3Object) g_MD3Object_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_MD3Object, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Sound) g_Sound_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Sound, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Music) g_Music_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Music, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_LODTerrain) g_LODTerrain_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_LODTerrain, "?RefreshD3D@@YAXH@Z" );
		if(g_pGlob->g_Q2BSP) g_Q2BSP_RefreshD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( g_pGlob->g_Q2BSP, "?RefreshD3D@@YAXH@Z" );
	}
	#else
		g_Text_RefreshD3D			= RefreshD3DText;
		g_Basic2D_RefreshD3D		= RefreshD3DBasic2D;
		g_Sprites_RefreshD3D		= RefreshD3DSprites;
		g_Image_RefreshD3D			= RefreshD3DImage;
		g_Input_RefreshD3D			= RefreshD3DInput;
		g_System_RefreshD3D			= RefreshD3DSystem;
		g_Memblocks_RefreshD3D		= RefreshD3DMemblocks;
		g_Bitmap_RefreshD3D			= RefreshD3DBitmap;
		g_Animation_RefreshD3D		= RefreshD3DAnimation;
		g_Multiplayer_RefreshD3D	= RefreshD3DMultiplayer;
		g_Basic3D_RefreshD3D		= RefreshD3DBasic3D;
		g_Camera3D_RefreshD3D		= RefreshD3DCamera;
		g_Matrix3D_RefreshD3D		= RefreshD3DMatrix;
		g_Light3D_RefreshD3D		= RefreshD3DLight;
		
		g_Particles_RefreshD3D		= RefreshD3DParticles;
		g_Vectors_RefreshD3D		= RefreshD3D3DMaths;
		g_Sound_RefreshD3D			= RefreshD3DSound;
		g_Music_RefreshD3D			= RefreshD3DMusic;

		//g_World3D_RefreshD3D = dbRefreshD3DWorld;
		//if(g_pGlob->g_XObject)		g_XObject_RefreshD3D = 
		//if(g_pGlob->g_3DSObject)	g_3DSObject_RefreshD3D =
		//if(g_pGlob->g_MDLObject)	g_MDLObject_RefreshD3D =
		//if(g_pGlob->g_MD2Object)	g_MD2Object_RefreshD3D =
		//if(g_pGlob->g_MD3Object)	g_MD3Object_RefreshD3D =
		//if(g_pGlob->g_Q2BSP)		g_Q2BSP_RefreshD3D = 
		
	#endif
}

DARKSDK void InformDLLsOfDeviceLostOrNotReset ( int iDeviceLost )
{
	// leeadd - 020308 - inform all TPC DLLs that Device Has Been Lost
	if ( g_pGlob->pDynMemPtr )
	{
		// get local copy of DLLs
		HINSTANCE	hDLLMod[256];
		bool		bDLLTPC[256];
		memcpy ( hDLLMod, g_pGlob->pDynMemPtr+0, (sizeof(HINSTANCE)*256) );
		memcpy ( bDLLTPC, g_pGlob->pDynMemPtr+(sizeof(HINSTANCE)*256), (sizeof(bool)*256) );

		// find all TPC DLLs
		for ( int iDLL=1; iDLL<256; iDLL++ )
		{
			if ( hDLLMod [ iDLL ] )
			{
				if ( bDLLTPC [ iDLL ]==true )
				{
					if ( iDeviceLost==1 )
					{
						// Device Lost
						DLL_D3DDeviceLostFunctionCall pD3DDeviceLost;
						pD3DDeviceLost = ( DLL_D3DDeviceLostFunctionCall ) GetProcAddress ( hDLLMod [ iDLL ], "?D3DDeviceLost@@YAXXZ" );
						if ( pD3DDeviceLost ) pD3DDeviceLost();
					}
					if ( iDeviceLost==2 )
					{
						// Device Not Reset
						DLL_D3DDeviceNotResetFunctionCall pD3DDeviceNotReset;
						pD3DDeviceNotReset = ( DLL_D3DDeviceNotResetFunctionCall ) GetProcAddress ( hDLLMod [ iDLL ], "?D3DDeviceNotReset@@YAXXZ" );
						if ( pD3DDeviceNotReset ) pD3DDeviceNotReset();
					}
					if ( iDeviceLost==3 )
					{
						// Device Reset
						DLL_D3DDeviceResetFunctionCall pD3DDeviceReset;
						pD3DDeviceReset = ( DLL_D3DDeviceResetFunctionCall ) GetProcAddress ( hDLLMod [ iDLL ], "?D3DDeviceReset@@YAXXZ" );
						if ( pD3DDeviceReset ) pD3DDeviceReset();
					}
				}
			}
			else
				break;
		}
	}
}

DARKSDK void InformDLLsOfD3DChange(int iMode)
{
	// Make sure altest DLLs have links to their refresh functions
	//
	// COOL TRICK : REM out refresh lines to find resource leak when SETDSPLAYMODE
	//
	BuildFunctionsForDLLRefresh();

	// D3D Change release/recreate
	if(g_pGlob)
	{
		// Release/Regrab backbuffer pointers
		if(iMode==0) ReleaseBackBufferPointers();
		if(iMode==1) GetBackBufferPointers();

		// Call Refresh Function of Any Active DLLS
		if(g_Text_RefreshD3D) g_Text_RefreshD3D(iMode);
		if(g_Basic2D_RefreshD3D) g_Basic2D_RefreshD3D(iMode);
		if(g_Sprites_RefreshD3D) g_Sprites_RefreshD3D(iMode);
		if(g_Image_RefreshD3D) g_Image_RefreshD3D(iMode);
		if(g_Input_RefreshD3D) g_Input_RefreshD3D(iMode);
		if(g_System_RefreshD3D) g_System_RefreshD3D(iMode); 
		if(g_Memblocks_RefreshD3D) g_Memblocks_RefreshD3D(iMode);
		if(g_Bitmap_RefreshD3D) g_Bitmap_RefreshD3D(iMode);
		if(g_Animation_RefreshD3D) g_Animation_RefreshD3D(iMode);
		if(g_Multiplayer_RefreshD3D) g_Multiplayer_RefreshD3D(iMode);
		if(g_Basic3D_RefreshD3D) g_Basic3D_RefreshD3D(iMode);
		if(g_Camera3D_RefreshD3D) g_Camera3D_RefreshD3D(iMode);
		if(g_Matrix3D_RefreshD3D) g_Matrix3D_RefreshD3D(iMode);
		if(g_Light3D_RefreshD3D) g_Light3D_RefreshD3D(iMode);
		#ifndef DARKSDK_COMPILE
			if(g_World3D_RefreshD3D) g_World3D_RefreshD3D(iMode);
		#endif
		if(g_Particles_RefreshD3D) g_Particles_RefreshD3D(iMode);
		if(g_PrimObject_RefreshD3D) g_PrimObject_RefreshD3D(iMode); 
		if(g_Vectors_RefreshD3D) g_Vectors_RefreshD3D(iMode);
		if(g_XObject_RefreshD3D) g_XObject_RefreshD3D(iMode);
		if(g_3DSObject_RefreshD3D) g_3DSObject_RefreshD3D(iMode);
		if(g_MDLObject_RefreshD3D) g_MDLObject_RefreshD3D(iMode);
		if(g_MD2Object_RefreshD3D) g_MD2Object_RefreshD3D(iMode);
		if(g_MD3Object_RefreshD3D) g_MD3Object_RefreshD3D(iMode);
		if(g_Sound_RefreshD3D) g_Sound_RefreshD3D(iMode);
		if(g_Music_RefreshD3D) g_Music_RefreshD3D(iMode);
		if(g_LODTerrain_RefreshD3D) g_LODTerrain_RefreshD3D(iMode);
		if(g_Q2BSP_RefreshD3D) g_Q2BSP_RefreshD3D(iMode);

		// leeadd - 280305 - must also refresh all TPC DLls (u58)
		if ( g_pGlob->pDynMemPtr )
		{
			// get local copy of DLLs
			HINSTANCE	hDLLMod[256];
			bool		bDLLTPC[256];
			memcpy ( hDLLMod, g_pGlob->pDynMemPtr+0, (sizeof(HINSTANCE)*256) );
			memcpy ( bDLLTPC, g_pGlob->pDynMemPtr+(sizeof(HINSTANCE)*256), (sizeof(bool)*256) );

			// find all TPC DLLs
			for ( int iDLL=1; iDLL<256; iDLL++ )
			{
				if ( hDLLMod [ iDLL ] )
				{
					if ( bDLLTPC [ iDLL ]==true )
					{
						DLL_D3DRefreshFunctionCall pRefreshTPCD3D;
						pRefreshTPCD3D = ( DLL_D3DRefreshFunctionCall ) GetProcAddress ( hDLLMod [ iDLL ], "?RefreshD3D@@YAXH@Z" );
						if ( pRefreshTPCD3D ) pRefreshTPCD3D ( iMode );
					}
				}
				else
					break;
			}
		}
	}
}

//DARKSDK LPSTR GetReturnStringFromWorkString(void)
static LPSTR GetReturnStringFromWorkString(void)
{
	LPSTR pReturnString=NULL;
	if(m_pWorkString)
	{
		DWORD dwSize=strlen(m_pWorkString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, m_pWorkString);
	}
	return pReturnString;
}

//DARKSDK int GetBitDepthFromFormat(D3DFORMAT Format)
static int GetBitDepthFromFormat(D3DFORMAT Format)
{
	switch(Format)
	{
		case D3DFMT_R8G8B8 :		return 24;	break;
		case D3DFMT_A8R8G8B8 :		return 32;	break;
		case D3DFMT_X8R8G8B8 :		return 32;	break;
		case D3DFMT_R5G6B5 :		return 16;	break;
		case D3DFMT_X1R5G5B5 :		return 16;	break;
		case D3DFMT_A1R5G5B5 :		return 16;	break;
		case D3DFMT_A4R4G4B4 :		return 16;	break;
		case D3DFMT_A8	:			return 8;	break;
		case D3DFMT_R3G3B2 :		return 8;	break;
		case D3DFMT_A8R3G3B2 :		return 16;	break;
		case D3DFMT_X4R4G4B4 :		return 16;	break;
		case D3DFMT_A2B10G10R10 :	return 32;	break;
		case D3DFMT_G16R16 :		return 32;	break;
		case D3DFMT_A8P8 :			return 8;	break;
		case D3DFMT_P8 :			return 8;	break;
		case D3DFMT_L8 :			return 8;	break;
		case D3DFMT_A8L8 :			return 16;	break;
		case D3DFMT_A4L4 :			return 8;	break;
	}
	return 0;
}

DARKSDK void DB_UpdateEntireWindow(bool bFullUpdate, bool bMovement)
{
	// leeadd - 130306 - igl - this prevents IGL browser window repositioning within frame
	if ( g_pGlob->hwndIGLoader==NULL )
	{
		// regular DBP window
		if(bFullUpdate==false)
		{
			ShowWindow(m_hWnd, gWindowVisible);
		}
		else
		{
			if(strlen(gWindowName)>0) SetWindowText(m_hWnd, gWindowName);
			SetWindowLong(m_hWnd, GWL_STYLE, gWindowStyle);
			DWORD dwActualWindowWidth = gWindowSizeX+gWindowExtraX;
			DWORD dwActualWindowHeight = gWindowSizeY+gWindowExtraY;
			SetWindowPos(m_hWnd, HWND_TOP, g_pGlob->dwWindowX, g_pGlob->dwWindowY, dwActualWindowWidth, dwActualWindowHeight, SWP_SHOWWINDOW);
			ShowWindow(m_hWnd, gWindowVisible);
			SetClassLong(m_hWnd, GCL_HICON, (LONG)gWindowIconHandle);
		}

		// Paint after window switch
		if(bMovement)
			InvalidateRect(NULL, NULL, FALSE);
		else
			InvalidateRect(m_hWnd, NULL, FALSE);

		UpdateWindow(m_hWnd);
	}
}

DARKSDK void DB_EnsureWindowRestored(void)
{
	if(gbWindowMode)
	{
		gbWindowMode=false;
		gbWindowBorderActive=true;
		gWindowVisible=SW_SHOWDEFAULT;
		gWindowSizeX = GetSystemMetrics(SM_CXFULLSCREEN);
		gWindowSizeY = GetSystemMetrics(SM_CYFULLSCREEN);
		gWindowStyle = WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU;
		gWindowIconHandle = gOriginalIcon;
		DB_UpdateEntireWindow(true, true);
	}
}

DARKSDK void UpdateWindowSize ( int iWidth, int iHeight )
{
	// updates the window size, remember that in windowed
	// mode the window could be resized at any stage, if this
	// happens we need to inform the setup library

	// we could always detect the size automatically but then
	// it's waste because we would be checking it every frame
	// and this would be inefficient, by doing it this way we
	// call this function when the window has been resized

	// check for valid values
	// if ( iWidth < 0 || iHeight < 0 )
	//	Error ( "Invalid window size specified for setup library" );

	// store the new window size
	m_iWindowWidth  = iWidth;
	m_iWindowHeight = iHeight;
}

DARKSDK void GetWindowSize ( int* piWidth, int* piHeight )
{
	// retrieve the size of the window, several other DLL's
	// need to be able to do this, for example when clicking
	// on objects we need to determine the size of the window

	// check for valid pointers
	if ( !piWidth || !piHeight )
		Error ( "Invalid pointers passed to GetWindowSize for setup library" );

	// assign the pointers the saved window size
	*piWidth  = m_iWindowWidth;
	*piHeight = m_iWindowHeight;
}

DARKSDK void OverrideHWND ( HWND hWnd )
{
	// use an external window instead of the default

	// check the window handle is valid
	if ( !hWnd )
		Error ( "Invalid window handle passed to OverrideHWND" );
	
	// notify that we're using an external window
	// and save the window handle
	m_bOverrideHWND = true;
	m_hWnd          = hWnd;
}

DARKSDK void DisableWindowResize ( void )
{
	// disable window resizing

	m_bResizeWindow = false;
}

DARKSDK void EnableWindowResize ( void )
{
	// enable window resizing

	m_bResizeWindow = true;
}

DARKSDK void AddSwapChain ( HWND hwnd )
{
	// add to the swap chain

	// variable declarations
	D3DPRESENT_PARAMETERS	d3dpp;	// setup structure
	D3DDISPLAYMODE			mode;	// display mode
	HRESULT					hr;		// used for error checking

	// check the window handle is valid
	if ( !hwnd )
		Error ( "Invalid window handle for AddSwapChain" );

	// clear out structures
	memset ( &d3dpp, 0, sizeof ( d3dpp ) );
	memset ( &mode,  0, sizeof ( mode  ) );

	// get the current display mode
	m_pD3D->GetAdapterDisplayMode ( m_uAdapterChoice, &mode );
	
	// setup some fields
	d3dpp.Windowed         = true;					// windowed mode
	d3dpp.SwapEffect       = D3DSWAPEFFECT_COPY;	// use copy
	d3dpp.BackBufferFormat = mode.Format;			// back buffer format is same as main mode
	d3dpp.hDeviceWindow    = hwnd;					// handle to device window

	// finally add the new swap chain
	if ( FAILED ( hr = m_pD3DDevice->CreateAdditionalSwapChain ( &d3dpp, &m_pSwapChain [ m_iSwapChainCount++ ] ) ) )
	{
		// check the type of error, while DXTrace would do a better job than this
		// we can't use it as it's only provided for debug use
		switch ( hr )
		{
			case D3DERR_INVALIDCALL:
				Error ( "Invalid parameters when creating additional swap chain" );
			break;

			case D3DERR_OUTOFVIDEOMEMORY:
				Error ( "Unable to create additional swap chain - out of video memory" );
			break;
		}
	}
}

DARKSDK void StartSwapChain ( int iID )
{
	// start rendering a swap chain

	// declare some surfaces
	LPDIRECT3DSURFACE9	pBack    = NULL;	// back buffer
	LPDIRECT3DSURFACE9	pStencil = NULL;	// stencil buffer

	// check that the device is valid
	if ( !m_pD3DDevice )
		return;

	// check the ID is valid
	if ( iID > MAX_SWAP_CHAINS )
		Error ( "Specified invalid swap chain - overrun maximum limit" );

	// check that we could begin a scene
	if ( SUCCEEDED ( m_pD3DDevice->BeginScene ( ) ) )
		m_bDraw = true;

	// get the backbuffer
	m_pSwapChain [ iID ]->GetBackBuffer ( 0, D3DBACKBUFFER_TYPE_MONO, &pBack );

	// now get the stencil buffer
	m_pD3DDevice->GetDepthStencilSurface ( &pStencil );

	// check we got the buffer
	if ( !pBack || !pStencil )
		Error ( "Unable to access back buffer / stencil buffer for swap chain" );

	// now setup the new render target
	m_pD3DDevice->SetRenderTarget ( 0, pBack );
	m_pD3DDevice->SetDepthStencilSurface ( pStencil );

	// due to the way COM works we need to release
	// these interfaces otherwise we would end up 
	// with some resource leaks
	pBack->Release    ( );
	pStencil->Release ( );
}

DARKSDK void EndSwapChain ( int iID )
{
	// finish rendering a swap chain

	// check the device is valid
	if ( !m_pD3DDevice )
		return;

	// last chance to draw demo logo
	#ifdef DEMO
	ShowDemoLogo();
	#endif

	// check we are ok to draw
	if ( m_bDraw )
	{
		m_pD3DDevice->EndScene ( );	// finish the scene drawing
		m_bDraw = false;			// report that we've finished drawing
	}
}

DARKSDK void UpdateSwapChain ( int iID )
{
	// update the swap chain and draw it's contents on screen

	// check the ID is valid
	if ( iID > MAX_SWAP_CHAINS )
		Error ( "Specified invalid swap chain - overrun maximum limit" );

	// check the swap chain is valid
	if ( !m_pSwapChain [ iID ] )
		Error ( "Swap chain pointer not setup correctly" );

	// update full screen
	m_pSwapChain [ iID ]->Present ( NULL, NULL, NULL, NULL, 0 );
}

DARKSDK void Begin ( void )
{
	// being a typical rendering session

	g_bValidFPS = true;

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	if ( g_bWindowOverride )
		ShowWindow ( g_OldHwnd, SW_HIDE );
// U55 - 080704 - This interferes with HIDDEN state of most apps
//	else
//		ShowWindow ( g_OldHwnd, SW_SHOW );
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	// check the device is valid
	if ( !m_pD3DDevice )
		return;

	// now begin scene drawing
	if ( g_bSceneBegun==false )
	{
		g_bSceneBegun = true;
		if ( SUCCEEDED ( m_pD3DDevice->BeginScene ( ) ) )
			m_bDraw = true;
	}
}

DARKSDK void End ( void )
{
	// end the rendering session

	// check the device is valid
	if ( !m_pD3DDevice )
		return;

	// last chance to draw demo logo
	#ifdef DEMO
	ShowDemoLogo();
	#endif

	// check we're ok to draw
	if ( m_bDraw && m_pD3DDevice )
	{
		if ( g_bSceneBegun==true ) m_pD3DDevice->EndScene ( );	// end the scene
		m_bDraw = false;			// notify that we've finished drawing
		g_bSceneBegun = false;
	}
}

DARKSDK HRESULT PresentRect ( RECT* pArea, RECT* pClientrc, float fX, float fY )
{
	// result
	HRESULT hRes;

	// assign src and dest rects
	RECT src = { 0, 0, g_pGlob->iScreenWidth, g_pGlob->iScreenHeight };
	for(DWORD s=0; s<4; s++)
	{
		if(s==0)
		{
			src.bottom=pArea->top;
		}
		if(s==1)
		{
			src.top=pArea->top;
			src.bottom=pArea->bottom;
			src.right=pArea->left;
		}
		if(s==2)
		{
			src.left=pArea->right;
			src.right=pClientrc->right;
		}
		if(s==3)
		{
			src.top=pArea->bottom;
			src.left=0;
			src.bottom=pClientrc->bottom;
		}
		RECT dest = { (int)(src.left*fX), (int)(src.top*fY), (int)(src.right*fX), (int)(src.bottom*fY) };
		hRes = m_pD3DDevice->Present ( &src, &dest, NULL, NULL );
	}
	return hRes;
}

DARKSDK void DivideAreaByRect ( RECT* pArea, RECT* pSafeBoxes, DWORD* pdwDrawToBoxes, RECT** ppDrawBoxes )
{
	// Determine if area is clear of safe boxes
	bool bAreaIsClearOfSafeBoxes=true;
	for ( DWORD si=0; si<g_pGlob->dwSafeRectMax; si++ )
	{
		// check whether safe box intersects area
		if ( pSafeBoxes[si].right!=0 )
		{
			if( pArea->right > pSafeBoxes[si].left
			&&	pArea->bottom > pSafeBoxes[si].top
			&&	pArea->left < pSafeBoxes[si].right
			&&	pArea->top < pSafeBoxes[si].bottom	)	
			{
				// found safe box cutting into area
				bAreaIsClearOfSafeBoxes=false;

				// Store original rect
				RECT rectOrig = pSafeBoxes[si];

				// divide area into four smaller areas (all sides of safe box)
				RECT src = *pArea;
				for(DWORD s=0; s<4; s++)
				{
					if(s==0)
					{
						src.bottom=rectOrig.top;
					}
					if(s==1)
					{
						src.top=rectOrig.top;
						src.bottom=rectOrig.bottom;
						src.right=rectOrig.left;
					}
					if(s==2)
					{
						src.left=rectOrig.right;
						src.right=pArea->right;
					}
					if(s==3)
					{
						src.top=rectOrig.bottom;
						src.left=pArea->left;
						src.bottom=pArea->bottom;
					}

					// further divide this area by remaining safe boxes
					DivideAreaByRect ( &src, pSafeBoxes, pdwDrawToBoxes, ppDrawBoxes );
				}
			}
		}
	}

	// if area clear of safe boxes, add to draw box array
	if ( bAreaIsClearOfSafeBoxes )
	{
		// check if box is a valid one
		bool bBoxIsQualified=false;
		if(	pArea->right > pArea->left && pArea->bottom > pArea->top
		&&	pArea->left >= 0 && pArea->top >= 0
		&&	pArea->right <= g_pGlob->iScreenWidth
		&&	pArea->bottom <= g_pGlob->iScreenHeight	)
			bBoxIsQualified=true;

		// only add qualified boxes
		if (bBoxIsQualified )
		{
			if ( (*ppDrawBoxes) ) (*ppDrawBoxes) [ (*pdwDrawToBoxes) ] = *pArea;
			*pdwDrawToBoxes = *pdwDrawToBoxes + 1;
		}
	}
}

DARKSDK void CreateDrawBoxes ( DWORD* pdwDrawToBoxes, RECT** ppDrawBoxes )
{
	// Go through safe boxes
	if ( g_pGlob->dwSafeRectMax > 0 )
	{
		// Wipe out any boxes entirely inside other boxes
		for ( DWORD iSR=0; iSR<g_pGlob->dwSafeRectMax; iSR++ )
		{
			for ( DWORD iSR2=0; iSR2<g_pGlob->dwSafeRectMax; iSR2++ )
			{
				if ( iSR != iSR2 )
				{
					if(	g_pGlob->pSafeRects [iSR].left >= g_pGlob->pSafeRects [iSR2].left
					&&	g_pGlob->pSafeRects [iSR].right <= g_pGlob->pSafeRects [iSR2].right
					&&	g_pGlob->pSafeRects [iSR].top >= g_pGlob->pSafeRects [iSR2].top
					&&	g_pGlob->pSafeRects [iSR].bottom <= g_pGlob->pSafeRects [iSR2].bottom )
					{
						g_pGlob->pSafeRects [iSR].right=0;
					}
				}
			}
		}

		// Create a copy of the safe boxes
		RECT* pSafeBoxesCopy = new RECT [ g_pGlob->dwSafeRectMax ];

		// Initial area is entire screen
		RECT area = { 0, 0, g_pGlob->iScreenWidth, g_pGlob->iScreenHeight };

		// Count number of draw boxes
		memcpy ( pSafeBoxesCopy, g_pGlob->pSafeRects, sizeof(RECT) * g_pGlob->dwSafeRectMax );
		DivideAreaByRect ( &area, pSafeBoxesCopy, pdwDrawToBoxes, ppDrawBoxes );

		// Create draw array
		*ppDrawBoxes = new RECT [ *pdwDrawToBoxes ];
		*pdwDrawToBoxes = 0;

		// Create draw boxes
		memcpy ( pSafeBoxesCopy, g_pGlob->pSafeRects, sizeof(RECT) * g_pGlob->dwSafeRectMax );
		DivideAreaByRect ( &area, pSafeBoxesCopy, pdwDrawToBoxes, ppDrawBoxes );

		// Free copy of safe boxes
		SAFE_DELETE(pSafeBoxesCopy);
	}
}

// mike - 070207 - switch on custom present mode
void dbSetCustomPresentMode ( bool bMode )
{
	DisplayLibrary::g_bCustomPresentMode = bMode;
}

// mike - 070207 - add a rectangle region
void dbAddCustomPresentRectangle ( RECT rect )
{
	DisplayLibrary::g_CustomPresentRectangles.push_back ( rect );
}

// mike - 070207 - clear the draw list
void dbClearCustomPresentList ( void )
{
	DisplayLibrary::g_CustomPresentRectangles.clear ( );
}

// mike - 070207 - render all regions
HRESULT dbRenderUsingCustomPresentList ( void )
{
	HRESULT hr;

	for ( int i = 0; i < (int)DisplayLibrary::g_CustomPresentRectangles.size ( ); i++ )
	{
		// dest is primary stretched maybe
		RECT clientrc = { 0,0,0,0 };
		GetClientRect(g_pGlob->hWnd, &clientrc);

		// calculate scaling between src and dest
		float fX = (float)clientrc.right / (float)g_pGlob->iScreenWidth;
		float fY = (float)clientrc.bottom / (float)g_pGlob->iScreenHeight;
		RECT src = DisplayLibrary::g_CustomPresentRectangles [ i ];
		RECT dest = { (int)(src.left*fX), (int)(src.top*fY), (int)(src.right*fX), (int)(src.bottom*fY) };
		hr = m_pD3DDevice->Present ( &src, &dest, NULL, NULL );
	}

	return hr;
}

HRESULT StandardPresent ( void )
{
	// result
	HRESULT hRes  S_OK;

	// Standard stretched present to visual surface
	#ifdef VR920SUPPORT
	if( g_StereoEnabled && g_StereoHandle )
	{
		// RENDER HANDLED BY CAMERA (to set and render from viewpoint)
		if ( g_StereoEyeToggle==LEFT_EYE )
			g_pLeftEyeQuery->Issue(D3DISSUE_END);
		else
			g_pRightEyeQuery->Issue(D3DISSUE_END);

		// 4. Syncronize the frame to the left eyes frame buffer.
		if( IWRSyncronizeEye( g_StereoEyeToggle ) == EXIT_STEREOSCOPIC_MODE )
		{
			// Lost connection possibly hot-plug VR920's USB connector.
			g_StereoHandle = INVALID_HANDLE_VALUE;
			g_StereoEnabled = false; 
		}

		// toggle stereo eyes automatically
		if ( g_StereoEyeToggle==LEFT_EYE )
		{
			g_StereoEyeToggle=RIGHT_EYE;
			g_pRightEyeQuery->Issue(D3DISSUE_BEGIN);
		}
		else
		{
			g_StereoEyeToggle=LEFT_EYE;
			g_pLeftEyeQuery->Issue(D3DISSUE_BEGIN);
		}
	}
	else
	{
		// Mono Mode rendering - standard present
		hRes = m_pD3DDevice->Present ( NULL, NULL, NULL, NULL );
	}
	#else

		//Dave - Option to not scale display when rendering to a child window
		if ( g_bWindowOverride && g_dwChildWindowTruePixel )
		{
			RECT src = { 0, 0, 0, 0 };
			GetClientRect ( m_hWnd , &src );

			hRes = m_pD3DDevice->Present ( &src , NULL, NULL, NULL );
		}
		else
			hRes = m_pD3DDevice->Present ( NULL, NULL, NULL, NULL );

	#endif

	// return if device lost during present
	return hRes;
}

DARKSDK void Render ( void )
{
	// result
	HRESULT hRes;

	// copy the buffers and show the contents
	// check the device is valid, really should use assert
	if ( !m_pD3DDevice )
		return;

	// U70 - 170608 - WOW Support
	#ifdef WOWSUPPORT
    if (g_WOWhandle != NULL)
    {
		// stopped work here
//		MessageBox ( NULL, "WOW needs to load DYNAMICALLY not as DEPENDENT DLL", "WOW", MB_OK );
//		PostQuitMessage(0);

		// Get final image (from camera to image texture created by stereoscopic command)
		IDirect3DTexture9* pFinalTexture = NULL;
		if ( g_Camera_GetStereoscopicFinalTexture ) pFinalTexture = g_Camera_GetStereoscopicFinalTexture();
		if ( pFinalTexture )
		{
			// Set render target now for WOWvxPlayerDrawScene
		    m_pD3DDevice->SetRenderTarget( 0, g_pBackBufferRenderTarget );
			m_pD3DDevice->SetDepthStencilSurface ( NULL);
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity ( &matIdentity );
			m_pD3DDevice->SetTransform ( D3DTS_VIEW, &matIdentity );
			m_pD3DDevice->SetTransform ( D3DTS_PROJECTION, &matIdentity );
			m_pD3DDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

			// Provide the texture to the API. For now, we assume that it is 2D + depth side-by-side.
			WOWvxPlayerSetSource( g_WOWhandle, WOWvxPlayerSourceMain, WOWvxPlayerTexture2D_Z, 
			pFinalTexture, m_imageWidth, m_imageHeight, m_textureWidth, m_textureHeight );

			// The API will now render the scene.
			WOWvxPlayerDrawScene( g_WOWhandle );
		}
    }
	#endif

	// dest is primary stretched maybe
	RECT clientrc = { 0,0,0,0 };
	GetClientRect(g_pGlob->hWnd, &clientrc);

	// calculate scaling between src and dest
	float fX = (float)clientrc.right / (float)g_pGlob->iScreenWidth;
	float fY = (float)clientrc.bottom / (float)g_pGlob->iScreenHeight;

	// if array of protected boxes setup (from controls requiring primary surface)
	if ( g_pGlob->dwSafeRectMax>0 )
	{
		// create draw boxes from protected boxes
		DWORD dwDrawToBoxes = 0;
		RECT* pDrawBoxes = NULL;
		CreateDrawBoxes ( &dwDrawToBoxes, &pDrawBoxes );

		// go through boxes
		if ( dwDrawToBoxes > 0 )
		{
			for ( DWORD boxindex=0; boxindex<dwDrawToBoxes; boxindex++ )
			{
				// present each one to copy to primary
				RECT src = pDrawBoxes [ boxindex ];
				RECT dest = { (int)(src.left*fX), (int)(src.top*fY), (int)(src.right*fX), (int)(src.bottom*fY) };
				hRes = m_pD3DDevice->Present ( &src, &dest, NULL, NULL );
			}
		}

		// Free usages
		SAFE_DELETE(pDrawBoxes);
	}
	else
	{
		// if area is for direct primary-surface drawing (such as a DVD video window)
		if ( g_pGlob->iNoDrawRight!=0 )
		{
			// area as src-scaled rect
			RECT area = { g_pGlob->iNoDrawLeft, g_pGlob->iNoDrawTop, g_pGlob->iNoDrawRight, g_pGlob->iNoDrawBottom };

			// Present 
			hRes = PresentRect ( &area, &clientrc, fX, fY );
		}
		else
		{
			// If chopping required (due to non standard setdisplaymode size)
			if ( m_iChopWidth!=0 || m_iChopHeight!=0)
			{
				RECT src = { 0, 0, g_pGlob->iScreenWidth, g_pGlob->iScreenHeight };
				hRes = m_pD3DDevice->Present ( &src, NULL, NULL, NULL );
			}
			else
			{
				// Standard stretched present to visual surface
				hRes = StandardPresent();
			}
		}
	}

	// Catch if present fail, device may be lost
	if(hRes==D3DERR_DEVICELOST)
	{
		// Attempt to restore device
		RestoreLostDevice();
	}
}

DARKSDK bool SetDisplayMode ( int iWidth, int iHeight )
{
	// sets the display mode to the specified
	// width and height, default values are provided
	// when this function is called the program will
	// default to setting 16 bit color depth and will
	// also default to full screen mode and attempt
	// to select hardware vertex processing
	
	if ( !SetDisplayMode ( iWidth, iHeight, 16, FULLSCREEN, HARDWARE ) )
		return false;

	return true;
}

DARKSDK bool SetDisplayMode ( int iWidth, int iHeight, int iDepth )
{
	// sets the display mode to the specified
	// width and height, default values are provided
	// when this function is called the program will
	// default to full screen mode and attempt to
	// select hardware vertex processing, lockable
	// backbuffers are switched off
	
	if(!SetDisplayMode ( iWidth, iHeight, iDepth, FULLSCREEN, HARDWARE, 0 ) )
		return false;

	return true;
}

DARKSDK bool SetDisplayMode ( int iWidth, int iHeight, int iDepth, int iMode )
{
	// sets the display mode to the specified
	// width, height, depth and mode default values
	// are provided when this function is called 
	// the program will attempt to select hardware
	// vertex processing, lockable backbuffers are
	// switched off
	if ( !SetDisplayMode ( iWidth, iHeight, iDepth, iMode, HARDWARE, 0 ) )
		return false;

	return true;
}

DARKSDK bool SetDisplayMode ( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing )
{
	// sets the display mode
	if ( !SetDisplayMode ( iWidth, iHeight, iDepth, iMode, iVertexProcessing, 0 ) )
		return false;

	return true;
}

DARKSDK bool SetDisplayMode ( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing, int iLockable )
{
	#if DEBUG_MODE
			fwrite ( 
						"\n** begin SetDisplayMode\n", 
						strlen ( "\n** begin SetDisplayMode\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	int  iTemp;
	bool bValid = false;

	#if DEBUG_MODE
			fwrite ( 
						"* SetDisplayMode - check parameters\n", 
						strlen ( "* SetDisplayMode - check parameters\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif
	
	// check the width and height
	if ( iWidth <= 0 || iHeight <= 0 )
	{
		Error ( "Invalid display resolution" );
		RunTimeError(RUNTIMEERROR_SCREENSIZEILLEGAL);
		return false;
	}

	/*
	// mike - 010904 - road signs
	if ( iDepth == 24 )
	{
		//Error ( "Large" );
		Message ( 0, "Please change your display settings to either 16 or 32 bit.", "Driving Test Success ROAD SIGNS" );
		return false;
	}
	*/

	// if depth is zero, fill with current windows depth
	if ( iDepth == 0 )
	{
		// get windows natural depth
		D3DDISPLAYMODE d3dmode;
		m_pD3D->GetAdapterDisplayMode ( 0, &d3dmode );
		iDepth = GetBitDepthFromFormat(d3dmode.Format);

		// lee - 290306 - u6rc3 - must update screendepth in case task-switch, or set window on..
		g_pGlob->iScreenDepth = iDepth;
	}

	// 24bit not supported
	if ( iDepth == 24 )
	{
		Error ( "Invalid bit depth specified" );
		RunTimeError(RUNTIMEERROR_24BITNOTSUPPORTED);
		return false;
	}

	// check the depth
	if ( iDepth != 16 && iDepth != 32 && iDepth != 24 )
	{
		Error ( "Invalid bit depth specified" );
		RunTimeError(RUNTIMEERROR_SCREENDEPTHILLEGAL);
		return false;
	}

	// mode
	if ( iMode < 0 || iMode > 1 )
	{
		Error ( "Invalid display mode specified" );
		RunTimeError(RUNTIMEERROR_SCREENMODEINVALID);
		return false;
	}

	// just check that the mode does exist
	for ( iTemp = 0; iTemp < m_pInfo [ 0 ].iDisplayCount; iTemp++ )
	{
		// find the mode which which matches
		// the selected width and height of
		// the new display settings
		if 
			(
				m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width  == (DWORD)iWidth &&
				m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height  == (DWORD)iHeight &&
				GetBitDepthFromFormat(m_pInfo [ 0 ].D3DDisplay [ iTemp ].Format) == iDepth
			)
		{
				bValid = true;
				break;
		}
	}

	// if non standard size required, submit a valid size and setup clip data
	if ( !bValid )
	{
		// Generate a standard width
		int iStWidth=iWidth, iStHeight=iHeight;
		for ( iTemp = 0; iTemp < m_pInfo [ 0 ].iDisplayCount; iTemp++ )
		{
			if (	m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width  > (DWORD)iStWidth &&
					m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height  > (DWORD)iStHeight &&
					GetBitDepthFromFormat(m_pInfo [ 0 ].D3DDisplay [ iTemp ].Format) == iDepth )
			{
				// Nearest valid display mode
				iStWidth = m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width;
				iStHeight = m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height;
				bValid = true;
				break;
			}
		}

		// Create clip data
		m_iChopWidth = iStWidth - iWidth;
		m_iChopHeight = iStHeight - iHeight;

		// Assign new width and height
		iWidth = iStWidth;
		iHeight = iStHeight;
	}
	else
	{
		// No chopping required
		m_iChopWidth = 0;
		m_iChopHeight = 0;
	}

	// ensure that the mode exists
	if ( !bValid )
	{
		Error ( "Display mode not supported" );
		if(iDepth==16) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAY16B);
		if(iDepth==24) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAY24B);
		if(iDepth==32) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAY32B);
		return false;
	}
	
	// now check that the processing mode is valid
	if ( iVertexProcessing != HARDWARE && iVertexProcessing != SOFTWARE )
	{
		if ( iVertexProcessing < 0 || iVertexProcessing > 2 )
		{
			Error ( "Unknown vertex processing mode" );
			RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYVB);
			return false;
		}
	}

	// setup the correct vertex processing mode
	if ( iVertexProcessing == 0 )
		iVertexProcessing = HARDWARE;

	if ( iVertexProcessing == 1 )
		iVertexProcessing = SOFTWARE;

	if ( iVertexProcessing == 2 )
		iVertexProcessing = SOFTWARE;

	// last thing to do is check that the lockable flag is valid
	if ( iLockable < 0 || iLockable > 1 )
	{
		Error ( "Invalid lockable flag" );
		RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYLOCK);
		return false;
	}

	// Delete global property-desc of device
	if ( m_D3DPP )
	{
		delete m_D3DPP;
		m_D3DPP=NULL;
	}

	// U69 - 180508 - multimonitor mode doubles the width
	if ( m_iMultimonitorMode==1 )
	{
		// to fill primary (first half) and second monitor (second half)
		iWidth = iWidth * 2;
	}

	// setup values
	m_D3DPP			    = NULL;					// don't use debug mode
	m_iWidth		    = iWidth;				// set width
	m_iHeight		    = iHeight;				// set height
	m_iDepth		    = iDepth;				// set default color depth
	m_iDisplayType	    = iMode;				// windowed or fullscreen
	m_iProcess		    = iVertexProcessing;	// vertex processing mode
	m_bLockable		    = true;					// default to a lockable backbuffer
	m_iBackBufferCount  = 1;					// back buffer count

	// VSYNC flag set moved to global area

	// animation directshow needs multithreading (degrades performance though)
	// 040414 - Reloaded wants performance at the most of DS and we dont need backbuffer grab functionality!
	// m_iProcess		    |= D3DCREATE_MULTITHREADED;
	//if ( iLockable )
	//	m_dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	//else
	//	m_dwFlags = 0;
	//m_dwFlags = 0;
	m_dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// now setup the device
	if ( Setup ( ) )
	{
		// Adjust window size if desktop fullscreen mode in effect
		if(g_pGlob)
		{
			// Get Default Icon
			gOriginalIcon = (HICON)GetClassLong(g_pGlob->hWnd, GCL_HICON);
			gWindowIconHandle=gOriginalIcon;

			// 0=hidden
			if(g_pGlob->dwAppDisplayModeUsing==1)
			{
				// 1=window
				RECT clientrc;
				GetClientRect(g_pGlob->hWnd, &clientrc);
				gWindowSizeX = g_pGlob->iScreenWidth;
				gWindowSizeY = g_pGlob->iScreenHeight;
				if(gWindowExtraXForOverlap==0 && gbFirstInitOfDisplayOnly==true)
				{
					// Determine Extra for Overlapped Window Border
					gWindowExtraXForOverlap = g_pGlob->iScreenWidth-clientrc.right;
					gWindowExtraYForOverlap = g_pGlob->iScreenHeight-clientrc.bottom;
				}

				// U75 - 070909 - AA must have actual correct client size matching screen size, so ensure this here
				DWORD dwWindowStyle = GetWindowLong(g_pGlob->hWnd, GWL_STYLE);
				if ( (dwWindowStyle&WS_CAPTION) && (dwWindowStyle&WS_THICKFRAME) )
				{
					gWindowExtraX = gWindowExtraXForOverlap;
					gWindowExtraY = gWindowExtraYForOverlap;
				}
				else
				{
					gWindowExtraX = 0;
					gWindowExtraY = 0;
				}

				DWORD dwActualWindowWidth = gWindowSizeX+gWindowExtraX;
				DWORD dwActualWindowHeight = gWindowSizeY+gWindowExtraY;
				if ( g_pGlob->hwndIGLoader==NULL )
					SetWindowPos ( g_pGlob->hWnd, HWND_TOP, 0, 0, dwActualWindowWidth, dwActualWindowHeight, SWP_NOMOVE | SWP_SHOWWINDOW );
			}
			if(g_pGlob->dwAppDisplayModeUsing==2)
			{
				// 2=taskbar
				RECT rc;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
				gWindowSizeX = rc.right-rc.left;
				gWindowSizeY = rc.bottom-rc.top;
				gWindowExtraXForOverlap=0;
				gWindowExtraYForOverlap=0;
				gWindowExtraX = gWindowExtraXForOverlap;
				gWindowExtraY = gWindowExtraYForOverlap;
				if ( g_pGlob->hwndIGLoader==NULL )
					SetWindowPos(g_pGlob->hWnd, HWND_TOP, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOMOVE );
			}
			if(g_pGlob->dwAppDisplayModeUsing==3)
			{
				// 3=exclusive
				gWindowSizeX = iWidth;
				gWindowSizeY = iHeight;
				gWindowExtraXForOverlap=0;
				gWindowExtraYForOverlap=0;
				gWindowExtraX = gWindowExtraXForOverlap;
				gWindowExtraY = gWindowExtraYForOverlap;
				SetWindowPos(g_pGlob->hWnd, HWND_TOP, 0, 0, iWidth, iHeight, SWP_NOMOVE );
			}
			if(g_pGlob->dwAppDisplayModeUsing==0
			|| g_pGlob->dwAppDisplayModeUsing==4)
			{
				// 0=hidden (added 101004 so hidden window can become mode4)
				// 4=notaskbar
				DWORD dwWidth=GetSystemMetrics(SM_CXSCREEN);
				DWORD dwHeight=GetSystemMetrics(SM_CYSCREEN);
				gWindowSizeX = dwWidth;
				gWindowSizeY = dwHeight;
				gWindowExtraXForOverlap=0;
				gWindowExtraYForOverlap=0;
				gWindowExtraX = gWindowExtraXForOverlap;
				gWindowExtraY = gWindowExtraYForOverlap;
				// leefix - 200906 - u63 - removed to not interfere with parallel DBP related tasks
				if ( g_pGlob->dwAppDisplayModeUsing==4 )
				{
					// non-hidden windows still must set this though
					if ( g_pGlob->hwndIGLoader==NULL )
						SetWindowPos(g_pGlob->hWnd, HWND_TOP, 0, 0, dwWidth, dwHeight, SWP_NOMOVE);
				}
			}

			// Set only when CreateWindow first creates window (used to calculate extra window borders)
			gbFirstInitOfDisplayOnly=false;
		}

		// in case drawing right away, open scene
		Begin();

		// leeadd - 080306 - u60 - igLoader requires an EMBED command after a resolution create/switch
		if ( m_bFirstTimeIGLSkip==true )
		{
			// although we will skip the first one does not need embedding as it will already be done
			if ( g_pGlob->hwndIGLoader ) SendMessage ( g_pGlob->hwndIGLoader, WM_SETTEXT, 0, (LPARAM)"EMBEDME" );
		}
		m_bFirstTimeIGLSkip = true;

		// display mode successful
		return true;
	}
	else
	{
		Error ( "Unable to setup 3D device" );
	}

	#if DEBUG_MODE
			fwrite ( 
						"** end SetDisplayMode\n", 
						strlen ( "** end SetDisplayMode\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	// Runs off the end as unknown directx, else
	// runtime picked up along the way
	return false;
}

DARKSDK void FindHardwareInfo ( void )
{
	// find out graphics card information e.g.
	// name, manufacturer etc.

	#if DEBUG_MODE
			fwrite ( 
						"\n** begin FindHardwareInfo\n", 
						strlen ( "\n** begin FindHardwareInfo\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif

	UINT iAdapter;
	int	 iTemp;

	if ( !m_pD3D )
		Error ( "Invalid D3D device for FindHardWareInfo" );

	#if DEBUG_MODE
			fwrite ( 
						"* FindHardwareInfo - check all adapters\n", 
						strlen ( "* FindHardwareInfo - check all adapters\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif
	
	for ( iAdapter = 0; iAdapter < (DWORD)m_iAdapterCount; iAdapter++ )
	{
		D3DDISPLAYMODE			d3dmode;
		D3DCAPS9				d3dcaps;
		D3DADAPTER_IDENTIFIER9	d3dAdapter;
		HRESULT					hr;

		// clear out any structures
		memset ( &d3dAdapter, 0, sizeof ( d3dAdapter ) );
		memset ( &d3dmode,    0, sizeof ( d3dmode    ) );
		memset ( &d3dcaps,    0, sizeof ( d3dcaps    ) );

		// get current display mode
		if ( FAILED ( hr = m_pD3D->GetAdapterDisplayMode ( iAdapter, &d3dmode ) ) )
			Error ( "Failed to get adapter display mode" );

		// get device caps
		if ( FAILED ( hr = m_pD3D->GetDeviceCaps ( iAdapter, D3DDEVTYPE_HAL, &d3dcaps ) ) )
			Error ( "Failed to get device caps" );

		// get adapter info
		if ( FAILED ( hr = m_pD3D->GetAdapterIdentifier ( iAdapter, 0, &d3dAdapter ) ) )
			Error ( "Failed to get adapter identifier" );

		// get the name of the device
		m_pInfo [ iAdapter ].szName = new char [ 256 ];

		if ( !m_pInfo [ iAdapter ].szName )
			Error ( "Failed to allocate adapter name storage" );

		memset ( m_pInfo [ iAdapter ].szName, 0, sizeof ( char ) * 256 );
		memcpy ( m_pInfo [ iAdapter ].szName, d3dAdapter.Description, sizeof ( char ) * 256 );

		// copy adapter info
		memset ( &m_pInfo [ iAdapter ].D3DAdapter, 0, sizeof ( m_pInfo [ iAdapter ].D3DAdapter ) );
		memcpy ( &m_pInfo [ iAdapter ].D3DAdapter, &d3dAdapter, sizeof ( d3dAdapter ) );

		// copy current display mode
		memset ( &m_pInfo [ iAdapter ].D3DMode, 0, sizeof ( m_pInfo [ iAdapter ].D3DMode ) );
		memcpy ( &m_pInfo [ iAdapter ].D3DMode, &d3dmode, sizeof ( d3dmode ) );

		// copy the device caps
		memset ( &m_pInfo [ iAdapter ].D3DCaps, 0, sizeof ( m_pInfo [ iAdapter ].D3DCaps ) );
		memcpy ( &m_pInfo [ iAdapter ].D3DCaps, &d3dcaps, sizeof ( d3dcaps ) );

		// dx8->dx9 get all available display modes
		int iModeCount=0;
		m_pInfo [ iAdapter ].iDisplayCount = 0;
		while (iModeCount>=0 && iModeCount<=5)
		{
			if ( iModeCount==0 ) g_d3dFormat = D3DFMT_X8R8G8B8;
			if ( iModeCount==1 ) g_d3dFormat = D3DFMT_A8R8G8B8;
			if ( iModeCount==2 ) g_d3dFormat = D3DFMT_X1R5G5B5;
			if ( iModeCount==3 ) g_d3dFormat = D3DFMT_A1R5G5B5;
			if ( iModeCount==4 ) g_d3dFormat = D3DFMT_R5G6B5;
			if ( iModeCount==5 ) g_d3dFormat = D3DFMT_A2R10G10B10;
			m_pInfo [ iAdapter ].iDisplayCount += m_pD3D->GetAdapterModeCount ( iAdapter, g_d3dFormat );
			iModeCount++;
		}

		// create display mode array
		m_pInfo [ iAdapter ].D3DDisplay    = new D3DDISPLAYMODE [ m_pInfo [ iAdapter ].iDisplayCount ];
		if ( !m_pInfo [ iAdapter ].D3DDisplay )
			Error ( "Failed to allocate dispay list" );

		// clear display mode array
		memset ( m_pInfo [ iAdapter ].D3DDisplay, 0, sizeof ( m_pInfo [ iAdapter ].D3DDisplay ) * m_pInfo [ iAdapter ].iDisplayCount );

		// enumerate display modes
		iModeCount=0;
		int iArrayCount=0;
		D3DDISPLAYMODE DisplayMode, LastDsiplayMode;
		while (iModeCount>=0 && iModeCount<=5)
		{
			// formats
			if ( iModeCount==0 ) g_d3dFormat = D3DFMT_X8R8G8B8;
			if ( iModeCount==1 ) g_d3dFormat = D3DFMT_A8R8G8B8;
			if ( iModeCount==2 ) g_d3dFormat = D3DFMT_X1R5G5B5;
			if ( iModeCount==3 ) g_d3dFormat = D3DFMT_A1R5G5B5;
			if ( iModeCount==4 ) g_d3dFormat = D3DFMT_R5G6B5;
			if ( iModeCount==5 ) g_d3dFormat = D3DFMT_A2R10G10B10;

			// get number of them
			int iLocalCount = m_pD3D->GetAdapterModeCount ( iAdapter, g_d3dFormat );

			// enumerate them into list
			LastDsiplayMode.Width=0;
			LastDsiplayMode.Height=0;
			for ( iTemp=0; iTemp<iLocalCount; iTemp++ )
			{
				// enum for display mode data
				if ( FAILED ( hr = m_pD3D->EnumAdapterModes ( iAdapter, g_d3dFormat, iTemp, &m_pInfo [ iAdapter ].D3DDisplay [ iArrayCount ] ) ) )
					Error ( "Failed to enumerate adapter display modes" );

				// leefix - 200603 - most modes repeated for refresh rate, ignore refresh differences
				DisplayMode = m_pInfo [ iAdapter ].D3DDisplay [ iArrayCount ];
				if( DisplayMode.Width==LastDsiplayMode.Width && DisplayMode.Height==LastDsiplayMode.Height
				&& GetBitDepthFromFormat(DisplayMode.Format)==GetBitDepthFromFormat(LastDsiplayMode.Format) )
				{
					// do not increment array, so we overwrite with next one..
				}
				else
				{
					// next in array
					iArrayCount++;
				}
				LastDsiplayMode = DisplayMode;
			}

			// next mode
			iModeCount++;
		}

		// adjust size to actual display modes to use
		m_pInfo [ iAdapter ].iDisplayCount = iArrayCount;

		#if DEBUG_MODE
			fwrite ( 
						"* FindHardwareInfo - enumerate display modes\n", 
						strlen ( "* FindHardwareInfo - enumerate display modes\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
		#endif
	}

	#if DEBUG_MODE
			fwrite ( 
						"** end FindHardwareInfo\n", 
						strlen ( "** end FindHardwareInfo\n" ) * sizeof ( char ),
						1,
						m_fp
				   );
	#endif
}

DARKSDK bool SetDisplayDebugMode ( void )
{
	Error ( "SetDisplayDebug mode disabled" );

	/*
	CHardware	cHardware;
	D3DPRESENT_PARAMETERS	d3dpp;
	cHardware.Setup ( );
	cHardware.GetSettings ( &d3dpp );

	//Create ( m_hWnd, &d3dpp );
	*/

	return true;
}

DARKSDK void RestoreDisplayMode  ( void )
{
	// Restore display mode (called after a set display mode when releasing)
	if(m_D3DPP && m_pD3DDevice)
	{
		if(m_D3DPP->Windowed==FALSE)
		{
			m_D3DPP->Windowed = TRUE;
			ReleaseBackBufferPointers();
			m_pD3DDevice->Reset ( m_D3DPP );
			GetBackBufferPointers();
		}
	}
}

BOOL Is_Win_Vista_Or_Later () 
{
   OSVERSIONINFOEX osvi;
   DWORDLONG dwlConditionMask = 0;
   int op=VER_GREATER_EQUAL;

   // Initialize the OSVERSIONINFOEX structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   osvi.dwMajorVersion = 6;
   osvi.dwMinorVersion = 0;
   osvi.wServicePackMajor = 0;
   osvi.wServicePackMinor = 0;

   // Initialize the condition mask.

   VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
   VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

   // Perform the test.

   return VerifyVersionInfo(
      &osvi, 
      VER_MAJORVERSION | VER_MINORVERSION | 
      VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
      dwlConditionMask);
}


DARKSDK bool Setup ( void )
{
	// lee - 270306 - u6b5 - Initial window VSYNC is off (legacy behaviour)
	// lee - 170508 - u69 - moved up from inside window device to prepare this value right away
	#ifndef DARKSDK_COMPILE
	if ( m_bVSyncInitialDefault==false )
	{
		m_bVSyncInitialDefault = true;
		m_bVSync = false;
	}
	#endif

	// setup the display mode using the preset values, this function is only called internally
	if ( !m_pD3D )
	{
		Error ( "Invalid D3D pointer for Setup ( )" );
		RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYNODX);
		return false;
	}

	// U69 - 170508 - check for other adapters (perhaps use later)
	UINT uNumberOfAdapters = m_pD3D->GetAdapterCount();
	for ( UINT uI=0; uI<uNumberOfAdapters; uI++ )
	{
		D3DADAPTER_IDENTIFIER9 identifier;
		HRESULT hRes = m_pD3D->GetAdapterIdentifier ( uI, 0, &identifier );
	}
	m_uAdapterChoice = D3DADAPTER_DEFAULT;

	// U69 - 170508 - Reduce overhead on system by discarding buffer after use (must refresh each SYNC)
	// but only use it with VSYNC was specified so we do not affect default behaviour
	// m_SwapMode = D3DSWAPEFFECT_COPY; // pre-U69
	if ( m_iMultisamplingFactor > 0 )
	{
		// multisampling requires a DISCARDable backbuffer
		m_SwapMode = D3DSWAPEFFECT_DISCARD;

		// Cannot lock backbuffer for multisampled devices so switch D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ''off''
		m_dwFlags = 0;
	}
	else
	{
		// default for non 3D/sprite/grabbing backbuffer
		m_SwapMode = D3DSWAPEFFECT_COPY;
	}

	// 040414 -  test for flip approach
	//potato
	if ( g_bWindowOverride )
		m_SwapMode = D3DSWAPEFFECT_COPY;
	else
		m_SwapMode = D3DSWAPEFFECT_FLIP;
	

	// this will get switched off if we can't find a stencil buffer
	m_bZBuffer = true;

	// Free before setting properties
	if ( m_D3DPP )
	{
		delete m_D3DPP;
		m_D3DPP=NULL;
	}

	if ( !m_D3DPP ) 
	{
		m_D3DPP = new D3DPRESENT_PARAMETERS;
		if ( !m_D3DPP )
		{
			Error ( "Failed to allocate display mode parameters" );
		}

		memset ( m_D3DPP, 0, sizeof ( D3DPRESENT_PARAMETERS ) );
		m_D3DPP->SwapEffect				     = m_SwapMode;
		m_D3DPP->BackBufferCount			 = m_iBackBufferCount;
		m_D3DPP->EnableAutoDepthStencil		 = m_bZBuffer;
		m_D3DPP->FullScreen_RefreshRateInHz  = D3DPRESENT_RATE_DEFAULT;
		m_D3DPP->Flags                       = m_dwFlags;

		// U74 - 120609 - can have different backbuffer size if flagged
		if ( m_iModBackbufferWidth != 0 )
		{
			// use this instead of '799 m_bUseDeskTopBB resolution trick'
			m_D3DPP->BackBufferWidth             = m_iModBackbufferWidth;
			m_D3DPP->BackBufferHeight            = m_iModBackbufferHeight;
		}
		else
		{
			// default is backbuffer matches resolution
			m_D3DPP->BackBufferWidth             = m_iWidth;
			m_D3DPP->BackBufferHeight            = m_iHeight;
		}
		
		// U69 - 170508 - if VSYNC flagged, allow antialiasing of device up-to device max.
		// m_D3DPP->MultiSampleType             = D3DMULTISAMPLE_NONE;
		if ( m_iMultisamplingFactor > 0 )
		{
			// allow anti-aliasing of device (if supported)
			DWORD dwQualityLevels = 0;
			m_D3DPP->MultiSampleType = (D3DMULTISAMPLE_TYPE)m_iMultisamplingFactor;

			// check if multisampling is available for device
			if( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(	m_uAdapterChoice, D3DDEVTYPE_HAL, m_WindowsD3DMODE.Format,
																TRUE, m_D3DPP->MultiSampleType, &dwQualityLevels ) ) )
			{
				// set quality level of multisampling we found
				m_D3DPP->MultiSampleQuality			= dwQualityLevels-1;
			}
			else
			{
				// fall back to no multisampling
				m_D3DPP->MultiSampleType			= D3DMULTISAMPLE_NONE;
				m_D3DPP->MultiSampleQuality			= 0;
			}
		}
		else
		{
			// default behaviour
			m_D3DPP->MultiSampleType			= D3DMULTISAMPLE_NONE;
			m_D3DPP->MultiSampleQuality			= 0;
		}
		
		if ( m_iDisplayType == FULLSCREEN )
		{
			GetValidBackBufferFormat ( );
			GetValidStencilBufferFormat ( );

			m_D3DPP->AutoDepthStencilFormat		= m_StencilDepth;
			m_D3DPP->Windowed					= false;
			m_D3DPP->BackBufferFormat			= m_Depth;
			m_D3DPP->FullScreen_RefreshRateInHz = 0;
			m_D3DPP->BackBufferFormat           = m_Depth;
		}
		else
		{
			m_Depth									 = m_WindowsD3DMODE.Format;
			m_D3DPP->Windowed                        = true;
			m_D3DPP->BackBufferFormat                = m_WindowsD3DMODE.Format;
			m_D3DPP->PresentationInterval			 = D3DPRESENT_INTERVAL_DEFAULT;

			GetValidStencilBufferFormat ( );
			m_D3DPP->AutoDepthStencilFormat = m_StencilDepth;
			if ( m_StencilDepth == D3DFMT_UNKNOWN )
				Error ( "Failed to find valid stencil buffer" );
		}

		// lee - 240206 - u60 - unified VSYNC handler
		if ( m_bVSync )
		{
			// CAP TO MONITOR REFRESH RATE - NO TEARING
			m_D3DPP->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			if ( m_iVSyncInterval==2 ) m_D3DPP->PresentationInterval = D3DPRESENT_INTERVAL_TWO;
			if ( m_iVSyncInterval==3 ) m_D3DPP->PresentationInterval = D3DPRESENT_INTERVAL_THREE;
		}
		else
		{
			// FAST AS YOU CAN - HAS HORIZ TEARING
			m_D3DPP->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
	}

	// MAIN CREATE FUNCTION
	if ( Create ( m_hWnd, m_D3DPP )!=1 )
	{
		// Runtime errors generated witin Create function
		return false;
	}

	// Fail if no device created
	if(m_pD3DDevice==NULL)
	{
		RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYINVALID);
		return false;
	}
	else
		return true;
}

DARKSDK D3DFORMAT GetDepthFormatFromDisplaySetting ( int Width, int Height, int Depth )
{
	D3DFORMAT m_Depth = D3DFMT_R8G8B8;
	for ( int iTemp = 0; iTemp < m_pInfo [ 0 ].iDisplayCount; iTemp++ )
	{
		if	(	m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width  == (DWORD)Width &&
				m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height == (DWORD)Height )
		{
			if ( Depth == GetBitDepthFromFormat ( m_pInfo [ 0 ].D3DDisplay [ iTemp ].Format ) )
			{
				m_Depth = m_pInfo [ 0 ].D3DDisplay [ iTemp ].Format;
				continue;
			}
		}
	}
	return m_Depth;
}

DARKSDK void GetValidBackBufferFormat ( )
{
	// find a suitable format for the backbuffer, note
	// that earlier on we stored a list of all the supported
	// display modes, now we're simply running through each
	// one to find a suitable match

	#if DEBUG_MODE
		fwrite ( 
					"\n** begin GetValidBackBufferFormat\n", 
					strlen ( "\n** begin GetValidBackBufferFormat\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif

	#if DEBUG_MODE
		fwrite ( 
					"* GetValidBackBufferFormat - finding valid format\n", 
					strlen ( "* GetValidBackBufferFormat - finding valid format\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif

	// Get available depth format from settings
	m_Depth = GetDepthFormatFromDisplaySetting ( m_iWidth, m_iHeight, m_iDepth );

	#if DEBUG_MODE
		if ( m_Depth == D3DFMT_UNKNOWN )
		{
			fwrite ( 
						"* GetValidBackBufferFormat - no format found\n", 
						strlen ( "* GetValidBackBufferFormat - no format found\n" ) * sizeof ( char ),
						1, 
						m_fp 
				   );
		}
		else
		{
			fwrite ( 
						"* GetValidBackBufferFormat - format found\n", 
						strlen ( "* GetValidBackBufferFormat - format found\n" ) * sizeof ( char ),
						1, 
						m_fp 
				   );
		}
	#endif

	#if DEBUG_MODE
		fwrite ( 
					"** end GetValidBackBufferFormat\n", 
					strlen ( "** end GetValidBackBufferFormat\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif
}

DARKSDK void GetValidStencilBufferFormat ( void )
{
	#if DEBUG_MODE
		fwrite ( 
					"\n** begin GetValidStencilBufferFormat\n", 
					strlen ( "\n** begin GetValidStencilBufferFormat\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif

	// create the list in order of precedence
	D3DFORMAT	list [ ] =
							{
								D3DFMT_D24S8, //GeForce4 top choice
								D3DFMT_R8G8B8,
								D3DFMT_A8R8G8B8,
								D3DFMT_X8R8G8B8,
								D3DFMT_R5G6B5,
								D3DFMT_X1R5G5B5,
								D3DFMT_A1R5G5B5,
								D3DFMT_A4R4G4B4,
								D3DFMT_R3G3B2,
								D3DFMT_A8,
								D3DFMT_A8R3G3B2,
								D3DFMT_X4R4G4B4,
								D3DFMT_A8P8,
								D3DFMT_P8,
								D3DFMT_L8,
								D3DFMT_A8L8,
								D3DFMT_A4L4,
								D3DFMT_V8U8,
								D3DFMT_L6V5U5,
								D3DFMT_X8L8V8U8,
								D3DFMT_Q8W8V8U8,
								D3DFMT_V16U16,
								D3DFMT_D16_LOCKABLE,
								D3DFMT_D32,
								D3DFMT_D15S1,
								D3DFMT_D16,
								D3DFMT_D24X8,
								D3DFMT_D24X4S4,
								D3DFMT_D24FS8,
								D3DFMT_D32F_LOCKABLE,
								D3DFMT_D32_LOCKABLE,
								D3DFMT_S8_LOCKABLE
							};

	#if DEBUG_MODE
		fwrite ( 
					"** GetValidStencilBufferFormat - attempting to find a valid format\n", 
					strlen ( "** GetValidStencilBufferFormat - attempting to find a valid format\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif

	for ( int iTemp = 0; iTemp < 32; iTemp++ )
	{
		// Verify that the depth format exists first
		if ( SUCCEEDED ( m_pD3D->CheckDeviceFormat( m_uAdapterChoice,
													D3DDEVTYPE_HAL,
													m_Depth,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													list [ iTemp ]						) ) )
		{
			if ( SUCCEEDED ( m_pD3D->CheckDepthStencilMatch	(	m_uAdapterChoice,
																D3DDEVTYPE_HAL,
																m_Depth,
																m_Depth,
																list [ iTemp ]				) ) )
			{
				m_StencilDepth = list [ iTemp ];
				break;
			}
		}
	}

	#if DEBUG_MODE
		if ( m_StencilDepth == D3DFMT_UNKNOWN )
		{
			fwrite ( 
						"* GetValidStencilBufferFormat - invalid format\n", 
						strlen ( "* GetValidStencilBufferFormat - invalid valid format\n" ) * sizeof ( char ),
						1, 
						m_fp 
				   );

			fwrite ( 
						"* GetValidStencilBufferFormat - turning stencil buffer off\n", 
						strlen ( "* GetValidStencilBufferFormat - turning stencil buffer off\n" ) * sizeof ( char ),
						1, 
						m_fp 
				   );

			m_bZBuffer = false;
		}
		else
		{
			fwrite ( 
						"* GetValidStencilBufferFormat - valid format found\n", 
						strlen ( "* GetValidStencilBufferFormat - valid format found\n" ) * sizeof ( char ),
						1, 
						m_fp 
				   );
		}

		fwrite ( 
					"** end GetValidStencilBufferFormat\n", 
					strlen ( "** end GetValidStencilBufferFormat\n" ) * sizeof ( char ),
					1, 
					m_fp 
			   );
	#endif
}

DARKSDK int Create ( HWND hWnd, D3DPRESENT_PARAMETERS* d3dpp )
{
	// 190315 - trace display start-up issues
	char pDisplayErrTrace[2048];
	strcpy ( pDisplayErrTrace, "" );

	HRESULT			hr;
	D3DDISPLAYMODE	d3dmode;
	memset ( &d3dmode, 0, sizeof ( d3dmode  ) );
	m_pD3D->GetAdapterDisplayMode ( 0, &d3dmode );
	wsprintf ( pDisplayErrTrace, "%sW%dH%dD%dR%d", pDisplayErrTrace, d3dmode.Width, d3dmode.Height, d3dmode.Format, d3dmode.RefreshRate );

	// End scene before any release (just in case)
	End();
	SAFE_RELEASE ( m_pD3DDevice );
	
	// Assign new window handle to D3DPP
	d3dpp->hDeviceWindow = hWnd;

	// can setup app for viewing in PerfHUD
	D3DDEVTYPE pDevType = D3DDEVTYPE_HAL;
	if ( m_bNVPERFHUD==true )
	{
		// U74BETA9 - 010709 - can switch on PERFHUD for next SET DISPLAY MODE call
		m_uAdapterChoice = m_pD3D->GetAdapterCount()-1;
		pDevType = D3DDEVTYPE_REF;
	}
	else
	{
		if ( m_iForceAdapterOrdinal>0 )
		{
			m_uAdapterChoice = m_iForceAdapterOrdinal;
		}
	}
	if ( pDevType==D3DDEVTYPE_HAL ) wsprintf ( pDisplayErrTrace, "%s HAL", pDisplayErrTrace );
	if ( pDevType==D3DDEVTYPE_REF ) wsprintf ( pDisplayErrTrace, "%s REF", pDisplayErrTrace );

	// no depth format detected, default to at D3DFMT_D24S8 (standard)
	if ( d3dpp->AutoDepthStencilFormat==0 )
	{
		d3dpp->AutoDepthStencilFormat = D3DFMT_D24X8;
		wsprintf ( pDisplayErrTrace, "%sD3DFMT_D24X8_ADDED", pDisplayErrTrace );
	}

	// create device
	wsprintf ( pDisplayErrTrace, "%s A=%d P=%d D£DPP=%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d", pDisplayErrTrace, m_uAdapterChoice, m_iProcess, d3dpp->BackBufferCount, d3dpp->BackBufferWidth, d3dpp->BackBufferHeight, d3dpp->AutoDepthStencilFormat, d3dpp->BackBufferFormat, d3dpp->EnableAutoDepthStencil, d3dpp->Flags, d3dpp->FullScreen_RefreshRateInHz, d3dpp->hDeviceWindow, d3dpp->MultiSampleQuality, d3dpp->MultiSampleType, d3dpp->PresentationInterval, d3dpp->SwapEffect, d3dpp->Windowed );
	if(g_pGlob) g_pGlob->iSoftwareVP = 0;
	if ( FAILED ( hr = m_pD3D->CreateDevice (	m_uAdapterChoice,						// use default adapter
												pDevType,								// hardware mode
												hWnd,									// handle to window
												m_iProcess,
												d3dpp,									// display info
												&m_pD3DDevice							// pointer to device
												) ) )
	{
		// try again to create device (with software processing)
		if ( FAILED ( hr = m_pD3D->CreateDevice
										(
											m_uAdapterChoice,						// use default adapter
											D3DDEVTYPE_HAL,							// hardware mode
											hWnd,									// handle to window
											//D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// software processing //040414 - oops!!
											D3DCREATE_HARDWARE_VERTEXPROCESSING,	// software processing
											d3dpp,									// display info
											&m_pD3DDevice							// pointer to device
										) ) )
		{
			if ( hr==D3DERR_INVALIDCALL ) hr=1;
			if ( hr==D3DERR_NOTAVAILABLE  ) hr=2;
			if ( hr==D3DERR_OUTOFVIDEOMEMORY  ) hr=3;
			wsprintf ( pDisplayErrTrace, "%s HR=%d", pDisplayErrTrace, hr );
			MessageBox ( NULL, pDisplayErrTrace, "Display Mode Error", MB_OK | MB_TOPMOST );

			Error ( "Unable to create device" );
			if(hr==D3DERR_INVALIDCALL) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYINVALID);
			if(hr==D3DERR_NOTAVAILABLE) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYNOTAVAIL);
			if(hr==D3DERR_OUTOFVIDEOMEMORY) RunTimeError(RUNTIMEERROR_NOTSUPPORTDISPLAYNOVID);
			return 0;
		}
		else
		{
			// Device will use software VP
			if(g_pGlob) g_pGlob->iSoftwareVP = 1;
		}
	}

	// U70 - 170608 - WOW Support
	#ifdef WOWSUPPORT
    if ( g_WOWhandle != NULL )
    {
		// Get render target of backbuffer surface for later
	    g_pBackBufferRenderTarget = NULL; 
		m_pD3DDevice->GetRenderTarget( 0, &g_pBackBufferRenderTarget );

		// Initialize the WOWvxPlayerAPI
		WOWvxPlayerSet3DDevice( g_WOWhandle, m_pD3DDevice );

		// Get backbuffer details from device
		IDirect3DSurface9* backBuffer = NULL;
		m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO,	&backBuffer );
		D3DSURFACE_DESC backBufferDesc;
		backBuffer->GetDesc( &backBufferDesc );

	    // renderTarget for the window and pass it on to the API.
		WOWvxPlayerSetDestination( g_WOWhandle, backBuffer, backBufferDesc.Width, backBufferDesc.Height, backBufferDesc.Width, backBufferDesc.Height );

		// We let the API determine the output type.
		//WOWvxPlayerSetDestinationType( g_WOWhandle, WOWvxPlayerDestinationOptimal );
		WOWvxPlayerSetDestinationType( g_WOWhandle, WOWvxPlayerDestination2D_Z );

		// Typically, we would like to use WOWvxPlayerGetDestinationInfo to calculate the 2d width and height
		// for us. However, at this point, the API does not know what kind of textures will be fed to it, so
		// it cannot calculate the 2d width and height. One way to solve this is to do the WOWvxPlayerGetDestinationInfo
		// and WOWvxPlayerSetDestinationRectangle calls between the calls to WOWvxPlayerSetSource and
		// WOWvxPlayerDrawScene in the PresentHelper function. Here we take the easy route and just assume
		// we will be in 3D mode, where the 2d width and height are half the renderTarget width and height.
		//FAIL_RET( WOWvxPlayerGetDestinationInfo( m_handle, &width, &height, NULL );
//		WOWvxPlayerSetDestinationRectangle( g_WOWhandle, WOWvxPlayerSourceMain, 0, 0, backBufferDesc.Width / 2, backBufferDesc.Height / 2 );
		WOWvxPlayerSetDestinationRectangle( g_WOWhandle, WOWvxPlayerSourceMain, 0, 0, backBufferDesc.Width / 2, backBufferDesc.Height / 2 );

		// Set the content type.
//		WOWvxPlayerSetContentType( g_WOWhandle, WOWvxPlayerContentSignage, WOW_VX_PLAYER_DEFAULT, WOW_VX_PLAYER_DEFAULT );
		WOWvxPlayerSetContentType( g_WOWhandle, WOWvxPlayerContentGame, WOW_VX_PLAYER_DEFAULT, WOW_VX_PLAYER_DEFAULT );

		// Note that most modern graphics cards support non-power2 sized textures,
		// so support for such textures is not critical any more. However, the API
		// supports it.
		DWORD dwAllocTexWidth = backBufferDesc.Width;
		DWORD dwAllocTexHeight = backBufferDesc.Height;
		D3DCAPS9 d3dcaps;
		DWORD dwWidth = 1;
		DWORD dwHeight = 1;
		m_pD3DDevice->GetDeviceCaps( &d3dcaps );
		if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
		{
			while( dwWidth < dwAllocTexWidth )
				dwWidth = dwWidth << 1;
			while( dwHeight < dwAllocTexHeight )
				dwHeight = dwHeight << 1;

			m_imageWidth = dwAllocTexWidth;
			m_imageHeight = dwAllocTexHeight;
			m_textureWidth = dwWidth;
			m_textureHeight = dwHeight;
			dwAllocTexWidth = dwWidth;
			dwAllocTexHeight = dwHeight;
		}
		else
		{
			m_imageWidth = dwAllocTexWidth;
			m_imageHeight = dwAllocTexHeight;
			m_textureWidth = dwAllocTexWidth;
			m_textureHeight = dwAllocTexHeight;
		}
    }
	#endif

	// U74 - 280609 - VR920 Support
	#ifdef VR920SUPPORT

	// Use stereo switching technique (attempt to)
	// g_VR920StereoMethod is set in SET DISPLAY MODE extended command
	g_StereoEnabled = false;
	if ( g_VR920StereoMethod==true )
	{
		// Determine if the VR920 is plugged into a Video Adapter.
		bool bPrimaryInterface = false;
		if( IWRIsVR920Connected( &bPrimaryInterface ) != IWR_NOT_CONNECTED )
		{
			// VR hardware/driver found
			g_VR920AdapterAvailable = true;
			g_StereoEnabled	= g_VR920StereoMethod;

			// Create query interfaces to sync well with GPU
			m_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pLeftEyeQuery);
			m_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pRightEyeQuery);
			memset ( &g_d3dcaps, 0, sizeof ( g_d3dcaps ) );
			m_pD3DDevice->GetDeviceCaps ( &g_d3dcaps );

			// Open the VR920's tracker driver.
			HRESULT	iwr_status = IWRLoadDll();
			if( iwr_status != IWR_OK )
				g_VR920AdapterAvailable = false;

			// Open a handle to the VR920's stereo driver.
			g_StereoHandle = IWRSTEREO_Open();
			if( g_StereoHandle == INVALID_HANDLE_VALUE )
				g_VR920AdapterAvailable = false;

			// Select MONO or STEREO
			if( g_StereoEnabled ) 
				IWRSTEREO_SetStereo( g_StereoHandle, IWR_STEREO_MODE );
			else 
				IWRSTEREO_SetStereo( g_StereoHandle, IWR_MONO_MODE );
		}
		else
		{
			MessageBox ( NULL, "VR920 does not appear to be on any accessible adapters", "VR Error", MB_OK );
			g_VR920StereoMethod=false;
			g_StereoEnabled	= g_VR920StereoMethod;
		}
	}
	#endif

	////////////////////////////////////////////////////////////////////////////////
	if ( g_bWindowOverride )
		g_pGlob->hWnd = g_OldHwnd;
	////////////////////////////////////////////////////////////////////////////////

	// setup render states for initial device
	m_pD3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	// mip mapping
	m_pD3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	// mip mapping
	m_pD3DDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping
	m_pD3DDevice->SetRenderState ( D3DRS_DITHERENABLE,   TRUE             );		// enable dither
	m_pD3DDevice->SetRenderState ( D3DRS_ZENABLE,        TRUE             );		// enable z buffer
	m_pD3DDevice->SetRenderState ( D3DRS_SHADEMODE,      D3DSHADE_GOURAUD );		// set shade mode to gourad shading
	m_pD3DDevice->SetRenderState ( D3DRS_SPECULARENABLE, FALSE            );		// turn off specular highlights
	m_pD3DDevice->SetRenderState ( D3DRS_LIGHTING,       TRUE             );		// turn lighting on
	m_pD3DDevice->SetRenderState ( D3DRS_CULLMODE,       D3DCULL_NONE     );		// set cull mode to none
	m_pD3DDevice->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB ( 255, 50, 50, 50 ) );		// set ambient light on

	// MIKE 240303 - CLEAR SCREEN STRAIGHT AWAY SO NO ARTIFACTS
	// leefix - 130306 - igl - do 'not' interfere with window refresh as much as possible
	if ( g_pGlob->hwndIGLoader==NULL ) Clear ( 0, 0, 0 );

	// success
	return 1;
}

DARKSDK void Clear ( int iR, int iG, int iB )
{
	// clear the screen to the specified colour
	if(m_bZBuffer==false)
	{
		// clear the full screen
		m_pD3DDevice->Clear ( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB ( iR, iG, iB ), 1.0f, 0 );
	}
	else
	{
		// clear the full screen and the zbuffer
		m_pD3DDevice->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB ( iR, iG, iB ), 1.0f, 0 );
	}
}

DARKSDK void GetGamma ( int* piR, int* piG, int* piB )
{
	// retrieve the current gamma settings

	// check all of the pointers are valid
	if ( !piR || !piG || !piB )
		Error ( "Invalid pointers passed to GetGamma" );

	// now assign the gamma values
	*piR = m_iGammaRed;			// copy red
	*piG = m_iGammaGreen;		// copy green
	*piB = m_iGammaBlue;		// copy blue
}

DARKSDK bool CheckDisplayMode ( int iWidth, int iHeight )
{
	int iTemp;
	
	for ( iTemp = 0; iTemp < m_pInfo [ 0 ].iDisplayCount; iTemp++ )
	{
		if ( m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width == (DWORD)iWidth && m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height == (DWORD)iHeight )
			return true;
	}

	return false;
}

DARKSDK int CheckDisplayModeANTIALIAS ( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode )
{
	// extra check if VSYNC supported, and if MULTISAMPLING supported

	// no check vsync - assume all devices support D3DPRESENT_INTERVAL_ONE and D3DPRESENT_INTERVAL_IMMEDIATE

	// no multimonitor check - assume double wide resolution (typical S3D monitor)

	// check multisampling
	if ( iMultisamplingFactor > 0 )
	{
		BOOL bWindowModeFlag = TRUE;
		D3DFORMAT d3dDeviceFormat = m_WindowsD3DMODE.Format;
		if ( m_iDisplayType == FULLSCREEN )
		{
			bWindowModeFlag = FALSE;
			d3dDeviceFormat = m_Depth;
		}
		if ( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(	m_uAdapterChoice, D3DDEVTYPE_HAL, d3dDeviceFormat,
															bWindowModeFlag, (D3DMULTISAMPLE_TYPE)iMultisamplingFactor, NULL ) ) )
			return true;
		else
			return false;
	}

	// regular check on W,H,D
	return CheckDisplayMode ( iWidth, iHeight, iDepth );
}

DARKSDK bool CheckDisplayMode ( int iWidth, int iHeight, int iDepth, int iMode )
{
	if ( CheckDisplayMode ( iWidth, iHeight, iDepth ) )
	{
		GetValidBackBufferFormat ( );

		// mike - 230604 - added in safety check
		if ( m_pD3D )
		{
			if ( SUCCEEDED ( m_pD3D->CheckDeviceType
														( 
															0, 
															D3DDEVTYPE_HAL,
															m_Depth,
															m_Depth,
															iMode 
														) ) )
				return true;
		}
	}

	return false;
}

DARKSDK bool CheckDisplayMode ( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing )
{
	if ( CheckDisplayMode ( iWidth, iHeight, iDepth, iMode ) )
	{
		D3DCAPS9	d3dCaps;

		memset ( &d3dCaps, 0, sizeof ( d3dCaps ) );

		m_pD3DDevice->GetDeviceCaps ( &d3dCaps );

		if ( iVertexProcessing == 0 && ( d3dCaps.Caps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY ) )
			return true;
		
		if ( iVertexProcessing == 1 && ( d3dCaps.Caps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY ) )
			return true;
		 
	}

	return false;
}

DARKSDK bool GetWindowedMode ( void )
{
	// returns true if the app is running in windowed mode

	// check the display mode we're in
	if ( m_iDisplayType == 1 )
		return true;
	else
		return false;
}

DARKSDK int GetNumberOfDisplayModes ( void )
{
	// return the number of display modes available
	return m_pD3D->GetAdapterModeCount ( 0, g_d3dFormat );
}

DARKSDK void GetDisplayMode ( int iID, char* szMode )
{
	// build up a string which shows the selected resolution 
	// e.g. "640 x 480 x 32"

	// variable declarations
	char	szResolution [ 256 ];	// used to store the main bulk of the string
	int		iDepth;					// stores the resolution depth
	
	// check the string passed is valid
	if ( !szMode )
		return;

	// clear out the resolution string
	memset ( szResolution, 0, sizeof ( szResolution ) );
	
	// check the format of the mode
	if ( 
			m_pInfo [ 0 ].D3DDisplay [ iID ].Format == D3DFMT_X8R8G8B8 ||
			m_pInfo [ 0 ].D3DDisplay [ iID ].Format == D3DFMT_A8R8G8B8 ||
			m_pInfo [ 0 ].D3DDisplay [ iID ].Format == D3DFMT_R8G8B8
	   )
	{
		// we're dealing with a 32 bit mode
		iDepth = 32;
	}
	else
	{
		// we're dealing with a 16 bit mode
		iDepth = 16;
	}

	// finally build up the string
	sprintf ( szResolution, ("%ld x %ld x %ld"), m_pInfo [ 0 ].D3DDisplay [ iID ].Width, m_pInfo [ 0 ].D3DDisplay [ iID ].Height, iDepth );
	
	// and now copy the resolution information across
	strcpy ( szMode, szResolution );
}

DARKSDK int GetNumberOfDisplayDevices ( void )
{
	return m_pD3D->GetAdapterCount ( );
}

DARKSDK void SetDisplayDevice ( int iID )
{

}

DARKSDK void GetDeviceName ( int iID, char* szDevice )
{
	// gets the name of the selected device

	// declare a structure which will contain device information
	D3DADAPTER_IDENTIFIER9 identifier;

	// check that D3D has been initialised and that the pointer
	// passed to the function is valid
	if ( !m_pD3D || !szDevice )
		return;

	// clear out the structure
	memset ( &identifier, 0, sizeof ( identifier ) );

	// get info about the device
	m_pD3D->GetAdapterIdentifier ( iID, 0, &identifier );

	// now check that the call succeeded, if it did
	// copy the name across to the pointer
	if ( identifier.Description )
		strcpy ( szDevice, identifier.Description );
}

DARKSDK void GetDeviceDriverName ( int iID, char* szDriver )
{
	// gets the name of the driver for the selected device

	// declare a structure which will contain device information
	D3DADAPTER_IDENTIFIER9 identifier;

	// check that D3D has been initialised and that the pointer
	// passed to the function is valid
	if ( !m_pD3D || !szDriver )
		return;

	// clear out the structure
	memset ( &identifier, 0, sizeof ( identifier ) );

	// now get info about the device
	m_pD3D->GetAdapterIdentifier ( iID, 0, &identifier );

	// now check that the call succeeded, if it did
	// copy the driver name across to the pointer
	if ( identifier.Driver )
		strcpy ( szDriver, identifier.Driver );
}

DARKSDK void SetDitherMode ( int iMode )
{
	m_pD3DDevice->SetRenderState ( D3DRS_DITHERENABLE, iMode );
}

DARKSDK void SetShadeMode ( int iMode )
{
	m_pD3DDevice->SetRenderState ( D3DRS_SHADEMODE, iMode );
}

DARKSDK void SetLightMode ( int iMode )
{
	m_pD3DDevice->SetRenderState ( D3DRS_LIGHTING, iMode );
}

DARKSDK void SetCullMode ( int iMode )
{
	m_pD3DDevice->SetRenderState ( D3DRS_CULLMODE, iMode );
}

DARKSDK void SetSpecularMode ( int iMode )
{
	m_pD3DDevice->SetRenderState ( D3DRS_SPECULARENABLE, iMode );
}

DARKSDK void SetRenderState ( int iState, int iValue )
{
	m_pD3DDevice->SetRenderState ( ( D3DRENDERSTATETYPE ) iState, iValue );
}



DARKSDK IDirect3D9* GetDirect3D ( void )
{
	return m_pD3D;
}

DARKSDK IDirect3DDevice9* GetDirect3DDevice ( void )
{
	return m_pD3DDevice;
}

DARKSDK float __stdcall Timer ( TIMER_COMMAND command )
{
	// sets up a timer, flags can be used to perform the
	// following operations -
		//          TIMER_RESET           - to reset the timer
		//          TIMER_START           - to start the timer
		//          TIMER_STOP            - to stop ( or pause ) the timer
		//          TIMER_ADVANCE         - to advance the timer by 0.1 seconds
		//          TIMER_GETABSOLUTETIME - to get the absolute system time
		//          TIMER_GETAPPTIME      - to get the current time
		//          TIMER_GETELAPSEDTIME  - to get the time that elapsed between 
		//                                  TIMER_GETELAPSEDTIME calls

    static BOOL     m_bTimerInitialized = FALSE;
    static BOOL     m_bUsingQPF         = FALSE;
    static LONGLONG m_llQPFTicksPerSec  = 0;

    // initialize the timer
    if ( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;

        // use QueryPerformanceFrequency ( ) to get frequency of timer. If QPF is
        // not supported, we will timeGetTime ( ) which returns milliseconds.
        LARGE_INTEGER qwTicksPerSec;

        m_bUsingQPF = QueryPerformanceFrequency ( &qwTicksPerSec );

        if ( m_bUsingQPF )
            m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    if ( m_bUsingQPF )
    {
        static LONGLONG m_llStopTime        = 0;
        static LONGLONG m_llLastElapsedTime = 0;
        static LONGLONG m_llBaseTime        = 0;
        double fTime;
        double fElapsedTime;
        LARGE_INTEGER qwTime;
        
        // get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if ( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME )
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter ( &qwTime );

        // return the elapsed time
        if ( command == TIMER_GETELAPSEDTIME )
        {
            fElapsedTime = ( double ) ( qwTime.QuadPart - m_llLastElapsedTime ) / ( double ) m_llQPFTicksPerSec;
            m_llLastElapsedTime = qwTime.QuadPart;
            return ( FLOAT ) fElapsedTime;
        }
    
        // return the current time
        if ( command == TIMER_GETAPPTIME )
        {
            double fAppTime = ( double ) ( qwTime.QuadPart - m_llBaseTime ) / ( double ) m_llQPFTicksPerSec;
            return ( FLOAT ) fAppTime;
        }
    
        // reset the timer
        if ( command == TIMER_RESET )
        {
            m_llBaseTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // start the timer
        if ( command == TIMER_START )
        {
            m_llBaseTime       += qwTime.QuadPart - m_llStopTime;
            m_llStopTime        = 0;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // stop the timer
        if ( command == TIMER_STOP )
        {
            m_llStopTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // advance the timer by 1/10th second
        if ( command == TIMER_ADVANCE )
        {
            m_llStopTime += m_llQPFTicksPerSec / 10;
            return 0.0f;
        }

        if ( command == TIMER_GETABSOLUTETIME )
        {
            fTime = qwTime.QuadPart / ( double ) m_llQPFTicksPerSec;
            return ( FLOAT ) fTime;
        }

		// invalid command specified
        return -1.0f;
    }
    else
    {
        // get the time using timeGetTime()
        static double m_fLastElapsedTime  = 0.0;
        static double m_fBaseTime         = 0.0;
        static double m_fStopTime         = 0.0;
        double fTime;
        double fElapsedTime;
        
        // get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if ( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME )
            fTime = m_fStopTime;
        else
            fTime = timeGetTime ( ) * 0.001;
    
        // return the elapsed time
        if ( command == TIMER_GETELAPSEDTIME )
        {   
            fElapsedTime = ( double ) ( fTime - m_fLastElapsedTime );
            m_fLastElapsedTime = fTime;
            return ( FLOAT ) fElapsedTime;
        }
    
        // return the current time
        if ( command == TIMER_GETAPPTIME )
        {
            return ( FLOAT ) ( fTime - m_fBaseTime );
        }
    
        // reset the timer
        if ( command == TIMER_RESET )
        {
            m_fBaseTime         = fTime;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        // start the timer
        if ( command == TIMER_START )
        {
            m_fBaseTime        += fTime - m_fStopTime;
            m_fStopTime         = 0.0f;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        // stop the timer
        if ( command == TIMER_STOP )
        {
            m_fStopTime = fTime;
            return 0.0f;
        }
    
        // advance the timer by 1/10th second
        if ( command == TIMER_ADVANCE )
        {
            m_fStopTime += 0.1f;
            return 0.0f;
        }

		// get absolute time
        if ( command == TIMER_GETABSOLUTETIME )
        {
            return ( FLOAT ) fTime;
        }

		// invalid command specified
        return -1.0f;
    }
}

//
// Display Command Functions
//

DARKSDK int CheckDisplayMode ( int iWidth, int iHeight, int iDepth )
{
	for ( int iTemp = 0; iTemp < m_pInfo [ 0 ].iDisplayCount; iTemp++ )
	{
		if ( m_pInfo [ 0 ].D3DDisplay [ iTemp ].Width == (DWORD)iWidth && m_pInfo [ 0 ].D3DDisplay [ iTemp ].Height == (DWORD)iHeight )
		{
			D3DFORMAT d3dformat = m_pInfo [ 0 ].D3DDisplay [ iTemp ].Format;
			if ( iDepth == 16 )
			{
				 if (	d3dformat == D3DFMT_R5G6B5   || 
						d3dformat == D3DFMT_X1R5G5B5 ||
						d3dformat == D3DFMT_A1R5G5B5  )
						return 1;
			}

			if ( iDepth == 32 )
			{
				 if (	d3dformat == D3DFMT_X8R8G8B8 || 
						d3dformat == D3DFMT_A8R8G8B8 ||
						d3dformat == D3DFMT_R8G8B8    )
						return 1;
			}
		}
	}
	return 0;
}

DARKSDK DWORD CurrentGraphicsCard ( DWORD pDestStr )
{
	// Work string
	strcpy(m_pWorkString, m_pAdapterName);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int EmulationMode ( void )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
	return 0;
}

DARKSDK void PerformChecklistForDisplayModes ( void )
{
	// mike - 230604 - changed this function so it will no longer display
	//				 - multiples e.g. 640 x 480 x 32 will not appear twice

	D3DDISPLAYMODE* pMode = new D3DDISPLAYMODE [ m_pInfo [ m_iAdapterUsed ].iDisplayCount ];

	memcpy ( pMode, m_pInfo [ m_iAdapterUsed ].D3DDisplay, sizeof ( D3DDISPLAYMODE ) * m_pInfo [ m_iAdapterUsed ].iDisplayCount );

	/*
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 0 ].Width       = 640;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 0 ].Height      = 480;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 0 ].Format      = D3DFMT_A8R8G8B8;

	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 1 ].Width       = 640;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 1 ].Height      = 480;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 1 ].Format      = D3DFMT_X8R8G8B8;

	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 2 ].Width       = 640;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 2 ].Height      = 480;
	m_pInfo [ m_iAdapterUsed ].D3DDisplay [ 2 ].Format      = D3DFMT_G16R16;
	*/

	for ( int iA = 0; iA < m_pInfo [ m_iAdapterUsed ].iDisplayCount; iA++ )
	{
		for ( int iB = 0; iB < m_pInfo [ m_iAdapterUsed ].iDisplayCount; iB++ )
		{
			if ( iA == iB )
				continue;

			int iWidthA   = pMode [ iA ].Width;
			int iHeightA  = pMode [ iA ].Height;
			int iRefreshA = pMode [ iA ].RefreshRate;

			int iWidthB   = pMode [ iB ].Width;
			int iHeightB  = pMode [ iB ].Height;
			int iRefreshB = pMode [ iB ].RefreshRate;

			if ( iWidthA == iWidthB && iHeightA == iHeightB )
			{
				DWORD BitDepthA = GetBitDepthFromFormat ( pMode [ iA ].Format );
				DWORD BitDepthB = GetBitDepthFromFormat ( pMode [ iB ].Format );

				if ( BitDepthA == BitDepthB )
				{
					pMode [ iA ].Width  = 0;
					pMode [ iA ].Height = 0;
				}
			}
		}
	}

	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		// Run through total list of enumerated display modes
		g_pGlob->checklistqty=0;
		for ( int iTemp = 0; iTemp < m_pInfo [ m_iAdapterUsed ].iDisplayCount; iTemp++ )
		{
			int BitDepth=0;
			D3DDISPLAYMODE displaymode = pMode [ iTemp ];

			// mike - 230604 - skip invalid modes
			if ( displaymode.Width == 0 && displaymode.Height == 0 )
				continue;

			// These are the only valid render depths
			BitDepth=GetBitDepthFromFormat(displaymode.Format);

			// Only list 16bit+ modes
			if(BitDepth>=16)
			{
				if(g_bCreateChecklistNow)
				{
					

					g_pGlob->checklist[g_pGlob->checklistqty].valuea = displaymode.Width;
					g_pGlob->checklist[g_pGlob->checklistqty].valueb = displaymode.Height;
					g_pGlob->checklist[g_pGlob->checklistqty].valuec = BitDepth;
					wsprintf(g_pGlob->checklist[g_pGlob->checklistqty].string, "%dx%dx%d", displaymode.Width, displaymode.Height, BitDepth);
				}
				else
				{
					DWORD dwLength=32;//00x00x00
					if(dwLength>g_dwMaxStringSizeInEnum)
						g_dwMaxStringSizeInEnum=dwLength;
				}
				g_pGlob->checklistqty++;
			}
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
	
	SAFE_DELETE_ARRAY ( pMode );

	/*
	// Generate Checklist
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		// Run through total list of enumerated display modes
		g_pGlob->checklistqty=0;
		for ( int iTemp = 0; iTemp < m_pInfo [ m_iAdapterUsed ].iDisplayCount; iTemp++ )
		{
			int BitDepth=0;
			D3DDISPLAYMODE displaymode = m_pInfo [ m_iAdapterUsed ].D3DDisplay [ iTemp ];

			// These are the only valid render depths
			BitDepth=GetBitDepthFromFormat(displaymode.Format);

			// Only list 16bit+ modes
			if(BitDepth>=16)
			{
				if(g_bCreateChecklistNow)
				{
					g_pGlob->checklist[g_pGlob->checklistqty].valuea = displaymode.Width;
					g_pGlob->checklist[g_pGlob->checklistqty].valueb = displaymode.Height;
					g_pGlob->checklist[g_pGlob->checklistqty].valuec = BitDepth;
					wsprintf(g_pGlob->checklist[g_pGlob->checklistqty].string, "%dx%dx%d", displaymode.Width, displaymode.Height, BitDepth);
				}
				else
				{
					DWORD dwLength=32;//00x00x00
					if(dwLength>g_dwMaxStringSizeInEnum)
						g_dwMaxStringSizeInEnum=dwLength;
				}
				g_pGlob->checklistqty++;
			}
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
		*/
}

DARKSDK void PerformChecklistForGraphicsCards ( void )
{
	// Generate Checklist
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		// Run through total list of enumerated adapter(card) names
		g_pGlob->checklistqty=0;
		for ( int iTemp = 0; iTemp < m_iAdapterCount; iTemp++ )
		{
			if(g_bCreateChecklistNow)
			{
				wsprintf(g_pGlob->checklist[g_pGlob->checklistqty].string, "%s", m_pInfo [ iTemp ].szName);
			}
			else
			{
				DWORD dwLength=strlen(m_pInfo [ iTemp ].szName);
				if(dwLength>g_dwMaxStringSizeInEnum)
					g_dwMaxStringSizeInEnum=dwLength;
			}
			g_pGlob->checklistqty++;
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK int GetDisplayType ( void )
{
	// find out what type of device we're dealing with
	// return true if it's a hardware device

	// variable declarations
	D3DCAPS9	d3dCaps;	// D3D capabilities structure

	// clear out the structure
	memset ( &d3dCaps, 0, sizeof ( d3dCaps ) );

	// get the device caps, first check the pointer is valid
	if ( !m_pD3DDevice )
		return 0;

	m_pD3DDevice->GetDeviceCaps ( &d3dCaps );

	// check what type of device we've got
	if ( d3dCaps.DeviceType == D3DDEVTYPE_HAL )
		return 1;
	else
		return 0;
}

DARKSDK int GetDisplayWidth ( void )
{
	// always return screen width
	return g_pGlob->iScreenWidth;

	// get the width of the display, first off check what mode
	// we're running in as we have to deal with it a bit differently

	if ( m_iDisplayType == 0 )
	{
		// when we're running in fullscreen just return
		// the width we have stored
		return m_iWidth;
	}
	else
	{
		// when we're in windowed mode return the
		// size of the window
		RECT rect;

		GetWindowRect ( m_hWnd, &rect );
		
		return rect.right - rect.left;
	}
}

DARKSDK int GetDisplayHeight ( void )
{
	// always return screen height
	return g_pGlob->iScreenHeight;

	// get the height of the display, first off check what mode
	// we're running in as we have to deal with it a bit differently

	if ( m_iDisplayType == 0 )
	{
		// when we're running in fullscreen just return
		// the width we have stored
		return m_iWidth;
	}
	else
	{
		// when we're in windowed mode return the
		// size of the window
		RECT rect;

		GetWindowRect ( m_hWnd, &rect );

		return rect.bottom - rect.top;
	}
}

DARKSDK int GetWindowWidth ( void )
{
	return g_pGlob->dwWindowWidth;
}

DARKSDK int GetWindowHeight ( void )
{
	return g_pGlob->dwWindowHeight;
}

DARKSDK int GetDesktopWidth ( void )
{
	return (int)GetSystemMetrics(SM_CXSCREEN);
}

DARKSDK int GetDesktopHeight ( void )
{
	return (int)GetSystemMetrics(SM_CYSCREEN);
}

DARKSDK int GetDisplayDepth ( void )
{
	return m_iDepth;
}

DBPRO_GLOBAL FLOAT fLastTime = 0.0f;
DBPRO_GLOBAL DWORD dwFrames  = 0L;
DBPRO_GLOBAL float m_fFPS    = 0.0f;
DBPRO_GLOBAL char FrameStats [ 256 ];
DBPRO_GLOBAL float fTime;

DARKSDK int GetDisplayFPS ( void )
{
	// get the rate at which tha app is running, must be called once
	// each frame to work properly

	// mike - 250604 - only do fps once per frame
	if ( g_bValidFPS )
	{

		fTime = Timer ( TIMER_GETABSOLUTETIME );
		++dwFrames;

		/*
		static FLOAT fLastTime = 0.0f;
		static DWORD dwFrames  = 0L;
		static float m_fFPS    = 0.0f;
		static char FrameStats [ 256 ];
		float fTime = Timer ( TIMER_GETABSOLUTETIME );
		++dwFrames;
		*/

		if ( fTime - fLastTime > 1.0f )
		{
			m_fFPS    = dwFrames / ( fTime - fLastTime );
			fLastTime = fTime;
			dwFrames  = 0L;
		}

		g_bValidFPS = false;
	}

	return ( int ) m_fFPS;
}

DARKSDK int GetDisplayInvalid ( void )
{
	// is set to 1 when the focus has been remove from the window
	int iInvalidSet=0;
	if(g_pGlob->bInvalidFlag) iInvalidSet=1;
	g_pGlob->bInvalidFlag=false;
	return iInvalidSet;
}

DARKSDK void SetGamma ( int iR, int iG, int iB )
{
	// set the gamma for the screen

	// variable declarations
	D3DGAMMARAMP	ddgr;			// gamma structure
    
	WORD			wRed   = 0;		// red value
	WORD			wGreen = 0;		// green value
	WORD			wBlue  = 0;		// blue value

	int				iColour;		// used for loops

	// clear out the gamma structu
    memset ( &ddgr, 0, sizeof ( ddgr ) );

	// store the gamma values in case we need
	// to retrieve them later on
	m_iGammaRed   = iR;		// store red
	m_iGammaGreen = iG;		// store green
	m_iGammaBlue  = iB;		// store blue
        
	// run through all of the array and setup the colours
    for ( iColour = 0; iColour < 256; iColour++ )
    {
		// setup colours
		ddgr.red   [ iColour ] = wRed;		// set red
        ddgr.green [ iColour ] = wGreen;	// set green
        ddgr.blue  [ iColour ] = wBlue;		// set blue

		// increment colours
		wRed   += ( WORD ) iR;	// add red component
		wGreen += ( WORD ) iG;	// add green component
		wBlue  += ( WORD ) iB;	// add blue component
    }

	// now that we have setup the gamma structure we can
	// pass the info across to D3D and apply the changes
	m_pD3DDevice->SetGammaRamp ( 0, D3DSGR_NO_CALIBRATION, &ddgr );        
}

DARKSDK void SetDisplayModeEx ( int iWidth, int iHeight, int iDepth )
{
	// Inform all DLLs to release resources
	InformDLLsOfD3DChange(0);

	// lee - 090312 - Detect if want to force OS to change resolution (not friendly) (an alternative to full screen mode)
	if ( iWidth<0 || iHeight<0 )
	{
		// restore resolution to positive values
		iWidth=abs(iWidth);
		iHeight=abs(iHeight);

		// now force the OS to this resolution
		DEVMODE NewDevice;
		NewDevice.dmSize = sizeof(DEVMODE);
		NewDevice.dmPelsWidth = iWidth;
		NewDevice.dmPelsHeight = iHeight;
		NewDevice.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		HRESULT hRes = ChangeDisplaySettings(&NewDevice, 0);//CDS_GLOBAL | CDS_UPDATEREGISTRY);
		if ( hRes==DISP_CHANGE_SUCCESSFUL )
		{
			// successfully changed the resolution
		}
		else
		{
			LPSTR pError = "Unknown Change Display Settings Error";
			switch ( hRes )
			{
				case DISP_CHANGE_BADDUALVIEW : pError = "The settings change was unsuccessful because the system is DualView capable."; break;
				case DISP_CHANGE_BADFLAGS : pError = "An invalid set of flags was passed in."; break;
				case DISP_CHANGE_BADMODE : pError = "The graphics mode is not supported."; break;
				case DISP_CHANGE_BADPARAM : pError = "An invalid parameter was passed in. This can include an invalid flag or combination of flags."; break;
				case DISP_CHANGE_FAILED : pError = "The display driver failed the specified graphics mode."; break;
				case DISP_CHANGE_NOTUPDATED : pError = "Unable to write settings to the registry."; break;
				case DISP_CHANGE_RESTART : pError = "The computer must be restarted for the graphics mode to work."; break;
			}
			MessageBox ( NULL, pError, "Change Display Settings Error", MB_OK );
		}
	}

	// Sets window size in any event
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	// Update global data for window size
	g_pGlob->dwWindowWidth = m_iWidth;
	g_pGlob->dwWindowHeight = m_iHeight;

	// Update global data for screen size
	g_pGlob->iScreenWidth=m_iWidth;
	g_pGlob->iScreenHeight=m_iHeight;
	g_pGlob->iScreenDepth=m_iDepth;

	// Set New Display Mode (recreates D3DDevice)
	if( !SetDisplayMode ( iWidth, iHeight, iDepth, m_iDisplayType, HARDWARE, 1 ) )
		return;

	// Inform all DLLs of new Device and recreate resources
	InformDLLsOfD3DChange(1);

	// Clear area now used as backbuffer
	// leefix - 130306 - igl - do 'not' interfere with window refresh as much as possible
	if ( g_pGlob->hwndIGLoader==NULL ) Clear ( 0, 0, 0 );
}

DARKSDK void SetDisplayModeVSYNC ( int iWidth, int iHeight, int iDepth, int iVSyncOn )
{
	// choose new global vsync state
	if ( iVSyncOn>0 )
	{
		if ( iVSyncOn==1 ) m_iVSyncInterval=1;
		if ( iVSyncOn==2 ) m_iVSyncInterval=2;
		if ( iVSyncOn==3 ) m_iVSyncInterval=3;
		m_bVSync=true;
	}
	else
	{
		m_bVSync=false;
	}

	// call regular set display mode
	SetDisplayModeEx ( iWidth, iHeight, iDepth );
}

DARKSDK void SetDisplayModeANTIALIAS ( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode )
{
	// set new multisampling factor
	m_iMultisamplingFactor = iMultisamplingFactor;

	// set new multimpnitor mode value
	m_iMultimonitorMode = iMultimonitorMode;

	// call regular set display mode
	SetDisplayModeVSYNC ( iWidth, iHeight, iDepth, iVSyncOn );
}

DARKSDK void SetDisplayModeMODBACKBUFFER ( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode, int iBackbufferWidth, int iBackbufferHeight )
{
	// adjust real backbuffer to differ form size of resolution (render 1-2-1 pixels in window of screen)
	m_iModBackbufferWidth = iBackbufferWidth;
	m_iModBackbufferHeight = iBackbufferHeight;

	// call regular set display mode
	SetDisplayModeANTIALIAS ( iWidth, iHeight, iDepth, iVSyncOn, iMultisamplingFactor, iMultimonitorMode );
}

DARKSDK void SetDisplayModeVR ( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode, int iBackbufferWidth, int iBackbufferHeight, int iActivateVRMode )
{
	// switch on special left/right present code for iActivateVRMode if flagged
	// mode 1 : VR920
	#ifdef VR920SUPPORT
	if ( iActivateVRMode==1 )
		g_VR920StereoMethod = true;
	else
		g_VR920StereoMethod = false;
	#endif

	// call regular set display mode
	SetDisplayModeMODBACKBUFFER ( iWidth, iHeight, iDepth, iVSyncOn, iMultisamplingFactor, iMultimonitorMode, iBackbufferWidth, iBackbufferHeight );
}


DARKSDK void RestoreLostDevice ( void )
{
	HRESULT hRes = m_pD3DDevice->TestCooperativeLevel();
	if(hRes!=D3D_OK)
	{
		// Fullscreen lost focus (maybe ALT+TAB)
		if(hRes==D3DERR_DEVICELOST)
		{
			// leeadd - 020308 - signal to all TPC DLLs that device has been lost
			InformDLLsOfDeviceLostOrNotReset ( 1 );
			return;
		}

		// Attempt to restore device by testing for not reset state
		if(hRes==D3DERR_DEVICENOTRESET)
		{
			// leeadd - 020308 - signal to all TPC DLLs that device has not been reset (but is no longer lost)
			InformDLLsOfDeviceLostOrNotReset ( 2 );

			// Recreates device to restore application
			SetDisplayModeEx ( g_pGlob->iScreenWidth, g_pGlob->iScreenHeight, g_pGlob->iScreenDepth );

			// leeadd - 070308 - added an extra callback to signal when successfully recreated device
			hRes = m_pD3DDevice->TestCooperativeLevel();
			if(hRes==D3D_OK)
			{
				// allows TPC DLLs to know when the device has been reset, and objects can be recreated 
				InformDLLsOfDeviceLostOrNotReset ( 3 );
			}
		}
	}
}

DARKSDK void SetEmulationOn   ( void )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK void SetEmulationOff   ( void )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK void SetGraphicsCard ( DWORD dwCardname )
{
	// lee 100206 - Not Implemented in DBPRO U6 RELEASE (was never implemented)
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}


//
// Window Command Functions
//

bool g_bDetachFromPreviousWindow = false;

DARKSDK void SetWindowModeOn(void)
{
	/*
	// 200213 - Re-attach to former child window (map-editor.exe)
	bool bUpdateDXDevice = false;
	if ( g_bDetachFromPreviousWindow==true )
	{
		// 200213 - if window has been overridden AND RESTORED, make it overridden again
		HWND hwdRestore = g_OldHwnd;
		g_OldHwnd = m_hWnd;
		g_bWindowOverride = true;
		g_pGlob->hWnd     = hwdRestore;
		m_hWnd            = hwdRestore;
		g_bDetachFromPreviousWindow = false;
		bUpdateDXDevice = true;
	}
	else
	{
		// 200213 - if window has been overridden, restore window to standalone immediately
		if ( g_bWindowOverride )
		{
			HWND hwdRestore = g_OldHwnd;
			g_OldHwnd = m_hWnd;
			g_bWindowOverride = false;
			g_pGlob->hWnd     = hwdRestore;
			m_hWnd            = hwdRestore;
			g_bDetachFromPreviousWindow = true;
			bUpdateDXDevice = true;
		}
		*/

	if ( g_bWindowOverride==false )
	{
		// Switch to window
		if ( m_iDisplayType==FULLSCREEN )
		{
			// lee - 230306 - u6b4 - only if currently fullscreen
			m_iDisplayType = WINDOWED;
			SetDisplayModeEx ( g_pGlob->iScreenWidth, g_pGlob->iScreenHeight, g_pGlob->iScreenDepth );
		}

		// lee - 290306 - u6rc3 - keep legacy behaviour by switching mode to regular window
		g_pGlob->dwAppDisplayModeUsing=1;

		gbWindowMode=true;
		gbWindowBorderActive=false;
		gWindowVisible=SW_SHOWDEFAULT;
		gWindowSizeX = g_pGlob->iScreenWidth;
		gWindowSizeY = g_pGlob->iScreenHeight;
		gWindowExtraX = gWindowExtraXForOverlap;
		gWindowExtraY = gWindowExtraYForOverlap;
		gWindowStyle = WS_OVERLAPPEDWINDOW;
		DB_UpdateEntireWindow(true,true);
	}

	/*
	// 200213 - when pull window from child window, must re-direct to new window HWND
	if ( bUpdateDXDevice )
	{
		// 200213 - Set Focus for keyboard input
		SetFocus ( m_hWnd );

		// 200213 - Leave everything in tact, just recreate D3D device with new HWND
		End();
		m_D3DPP->hDeviceWindow = m_hWnd;
		//if ( Create ( m_hWnd, m_D3DPP )!=1 )
		//{
		//	// Runtime errors generated witin Create function
		//}
		Begin();
	}
	*/

	// Focus on getting input from keyboard and mouse
	if ( g_bWindowOverride )
	{
		if ( g_bDetachFromPreviousWindow==false )
		{
			// direct input
			g_bDetachFromPreviousWindow = true;
			g_Input_SetupKeyboard(1);
			g_Input_SetupMouse(1);
		}
		else
		{
			// input via parent window
			g_bDetachFromPreviousWindow = false;
			g_Input_SetupKeyboard(0);
			g_Input_SetupMouse(0);
		}
	}
}

DARKSDK void SetWindowModeOff(void)
{
	gbWindowMode=false;
	gbWindowBorderActive=true;
	gWindowVisible=SW_SHOWDEFAULT;
	gWindowSizeX = GetSystemMetrics(SM_CXFULLSCREEN);
	gWindowSizeY = GetSystemMetrics(SM_CYFULLSCREEN);
	gWindowIconHandle = gOriginalIcon;
	gWindowExtraX = 0;
	gWindowExtraY = 0;
	gWindowStyle = WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU;
	DB_UpdateEntireWindow(true,true);

	// Switch to fullscreen
	if ( m_iDisplayType==WINDOWED )
	{
		// lee - 230306 - u6b4 - only if currently window
		m_iDisplayType = FULLSCREEN;
		g_pGlob->dwAppDisplayModeUsing=3;
		SetDisplayModeEx ( g_pGlob->iScreenWidth, g_pGlob->iScreenHeight, g_pGlob->iScreenDepth );
	}
}

DARKSDK void SetWindowSettings( int iStyle, int iCaption, int iIcon )
{
	gWindowStyle=0;
	if(iStyle==1)
	{
		gWindowExtraX=gWindowExtraXForOverlap;
		gWindowExtraY=gWindowExtraYForOverlap;
		gWindowStyle |= WS_OVERLAPPEDWINDOW;
	}
	else
	{
		gWindowExtraX=0;
		gWindowExtraY=0;
		gWindowStyle |= WS_POPUP; 
	}

	// lee - 200306 - u6b4 - added extra layout codes
	if ( iStyle==2 ) gWindowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX;
	if ( iStyle==3 ) gWindowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	if ( iStyle==4 ) gWindowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;

	if(iCaption==1)
	{
// leefi - 300305 - in XP this seemed to be the fix, though the reason??
//		gWindowExtraX=gWindowExtraXForOverlap;
//		gWindowExtraY=gWindowExtraYForOverlap;
		gWindowExtraX=gWindowExtraXForOverlap-2;
		gWindowExtraY=gWindowExtraYForOverlap-2;
		gWindowStyle |= WS_CAPTION;
	}

	if(iIcon==1)
		gWindowIconHandle=gOriginalIcon;
	else
		gWindowIconHandle=NULL;

	// update window
	DB_UpdateEntireWindow(true,true);
}

DARKSDK void SetWindowPosition( int posx, int posy )
{
	g_pGlob->dwWindowX=posx;
	g_pGlob->dwWindowY=posy;
	DB_UpdateEntireWindow(true,true);
}

DARKSDK void SetWindowSize( int sizex, int sizey )
{
	gWindowSizeX=sizex;
	gWindowSizeY=sizey;
	DB_UpdateEntireWindow(true,true);
}

DARKSDK void HideWindow(void)
{
	gWindowVisible=SW_HIDE;
	DB_UpdateEntireWindow(false,true);
}

DARKSDK void ShowWindow(void)
{
	gWindowVisible=SW_SHOW;
	DB_UpdateEntireWindow(false,true);
}

DARKSDK void MinimiseWindow(void)
{
	gWindowVisible=SW_MINIMIZE;
	DB_UpdateEntireWindow(false,true);
}

DARKSDK void MaximiseWindow(void)
{
	gWindowVisible=SW_MAXIMIZE;
	DB_UpdateEntireWindow(false,true);
}

DARKSDK void RestoreWindow(void)
{
	gWindowVisible=SW_SHOWNORMAL;
	DB_UpdateEntireWindow(false,true);
}

DARKSDK void SetWindowTitle( DWORD pTitleString )
{
	strcpy(gWindowName, (char*)pTitleString);

	// mike - 250604 - just update window text
	SetWindowText(m_hWnd, gWindowName);

	// mike - 260604 - also show cursor
	// mike - 080704 - don't show cursor anymore
	//ShowCursor(TRUE );

	//DB_UpdateEntireWindow(true,false);
}

DARKSDK int WindowExist( DWORD pTitleString )
{
	if ( FindWindow ( NULL, (LPSTR)pTitleString )!=NULL )
		return 1;
	else
		return 0;
}

DARKSDK void WindowToBack(void)
{
	SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

DARKSDK void WindowToFront(void)
{
	// U75 - 080909 - previously commented out - SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetForegroundWindow(m_hWnd);
}

DARKSDK void WindowToFront(DWORD pTitleString)
{
	LPSTR lpstrTitle = (LPSTR)pTitleString;
	if ( strnicmp ( lpstrTitle, "__topmost__", 11 )==NULL )
	{
		// U75 - 051109 - special name to send window to topmost
		// makes sense in test game for FPS Creator as mouse pointer is hijacked anyway!
		SetForegroundWindow(m_hWnd);
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		// U&5 - 070510 - Force a click on the screen to give topmost window the focus
		MOUSEINPUT Mouse;
		memset ( &Mouse, 0, sizeof(MOUSEINPUT) );
		Mouse.dx=320;
		Mouse.dy=240;
		INPUT Input;
		memset ( &Input, 0, sizeof(INPUT) );
		Input.type = INPUT_MOUSE;
		Input.mi = Mouse;
		Mouse.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput ( 1, &Input, sizeof(INPUT) );
		Mouse.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput ( 1, &Input, sizeof(INPUT) );
		Mouse.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput ( 1, &Input, sizeof(INPUT) );
		MOUSEINPUT Mouse2;
		memset ( &Mouse2, 0, sizeof(MOUSEINPUT) );
		Mouse2.dx=320;
		Mouse2.dy=240;
		INPUT Input2;
		memset ( &Input2, 0, sizeof(INPUT) );
		Input2.type = INPUT_MOUSE;
		Input2.mi = Mouse2;
		Mouse2.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput ( 1, &Input2, sizeof(INPUT) );
	}
	else
	{
		HWND hWnd = FindWindow ( NULL, lpstrTitle );
		// U75 - 080909 - previously commented out - SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetForegroundWindow(hWnd);
	}
}

DARKSDK void WindowToBack(DWORD pTitleString)
{
	HWND hWnd = FindWindow ( NULL, (LPSTR)pTitleString );
	SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

//
// Backbuffer Command Functions
//

DARKSDK void LockBackbuffer(void)
{
	if(g_pBackBuffer==NULL)
	{
		HRESULT hRes = m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer);
		if(g_pBackBuffer)
		{
			// get format of backbuffer
			D3DSURFACE_DESC ddsd;
			hRes = g_pBackBuffer->GetDesc(&ddsd);

			D3DLOCKED_RECT d3dlockedrect;
			hRes = g_pBackBuffer->LockRect(&d3dlockedrect, NULL, D3DLOCK_NOSYSLOCK);
			if(hRes==D3D_OK)
			{
				int BitDepth=GetBitDepthFromFormat(ddsd.Format);
				g_dwSurfacePtr=(DWORD)d3dlockedrect.pBits;
				g_dwSurfaceWidth=ddsd.Width;
				g_dwSurfaceHeight=ddsd.Height;
				g_dwSurfaceDepth=BitDepth;
				g_dwSurfacePitch=d3dlockedrect.Pitch;
			}
		}
	}
}

DARKSDK void UnlockBackbuffer(void)
{
	if(g_pBackBuffer)
	{
		// Free locked surface
		g_pBackBuffer->UnlockRect();
		g_dwSurfacePtr=NULL;
		g_dwSurfaceWidth=0;
		g_dwSurfaceHeight=0;
		g_dwSurfaceDepth=0;
		g_dwSurfacePitch=0;

		// Release backbuffer
		SAFE_RELEASE(g_pBackBuffer);
	}
}

DARKSDK DWORD GetBackbufferPtr()
{
	DWORD pReturnValue=-1;
	if(g_dwSurfacePtr)
		pReturnValue = g_dwSurfacePtr;

	return pReturnValue;
}

DARKSDK int GetBackbufferWidth()
{
	int pReturnValue=-1;
	if(g_dwSurfacePtr)
		pReturnValue = g_dwSurfaceWidth;

	return pReturnValue;
}

DARKSDK int GetBackbufferHeight()
{
	int pReturnValue=-1;
	if(g_dwSurfacePtr)
		pReturnValue = g_dwSurfaceHeight;

	return pReturnValue;
}

DARKSDK int GetBackbufferDepth()
{
	int pReturnValue=-1;
	if(g_dwSurfacePtr)
		pReturnValue = g_dwSurfaceDepth;

	return pReturnValue;
}

DARKSDK int GetBackbufferPitch()
{
	int pReturnValue=-1;
	if(g_dwSurfacePtr)
		pReturnValue = g_dwSurfacePitch;

	return pReturnValue;
}

// lee - 130108 - added for X10 compat.
DARKSDK void				SetNvPerfHUD						( int iUsePerfHUD )
{
	if ( iUsePerfHUD==1 )
		m_bNVPERFHUD = true;
	else
		m_bNVPERFHUD = false;
}

DARKSDK void				ForceAdapterOrdinal ( int iForceOrdinal )
{
	m_iForceAdapterOrdinal = iForceOrdinal;
}

DARKSDK void				SetCaptureName						( DWORD pFilename )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				SetCaptureCodec						( DWORD pFilename )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				SetCaptureSettings					( int iCompressed, int iFPS, int iWidth, int iHeight, int iThreaded, int iWait )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				SetCaptureMode						( int iRecordVideo )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				SaveScreenshot						( DWORD pFilename )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				StartPlayback						( DWORD pFilename, float fSpeed )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				UpdatePlayback						( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void				StopPlayback						( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK int					PlaybackPlaying						( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
	return 0;
}

DARKSDK int					GetSessionTerminate					( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
	return 0;
}

// lee - 071108 - U71 - post process commands

DARKSDK void TextureScreen ( int iStageIndex, int iImageNumber )
{
	// apply texture at stage X to screen quad object if present
	if (g_Basic3D_TextureScreen) g_Basic3D_TextureScreen ( iStageIndex, iImageNumber );
}

DARKSDK void TextureScreen ( int iImageNumber )
{
	// as above
	TextureScreen ( 0, iImageNumber );
}

DARKSDK void SetScreenEffect ( int iEffectNumber )
{
	// apply effect to screen quad object if present
	if (g_Basic3D_SetScreenEffect) g_Basic3D_SetScreenEffect ( iEffectNumber );
}

bool g_bGrabTrackingDataOncePerTrio = false;
float g_fYaw, g_fPitch, g_fRoll;

void ObtainYawPitchRoll ( int g_Filtering )
{
	#ifdef VR920SUPPORT
	// tmp vars
	long Roll=0, Yaw=0, Pitch=0;

	// Get iWear tracking yaw, pitch, roll
	HRESULT iwr_status = IWRGetTracking( &Yaw, &Pitch, &Roll );  
	if(	iwr_status != IWR_OK )
	{
		// iWear tracker could be OFFLine: just inform user or wait until plugged in...
		Yaw = Pitch = Roll = 0;
		// Attempt to re-establish communications with the VR920.
		IWROpenTracker();	
	}

	// Always provide for a means to disable filtering;
	if( g_Filtering == 1 )
	{
		// manual application filtering
		IWRFilterTracking( &Yaw, &Pitch, &Roll );
		if( IWRSetFilterState ) IWRSetFilterState(FALSE);
	}
	if( g_Filtering == 2 )
	{
		// internal driver filtering
		if( IWRSetFilterState ) IWRSetFilterState(TRUE);
	}

	// return final values
	g_fPitch =  (float)Pitch * IWR_RAWTODEG;
	g_fYaw   =  (float)Yaw * IWR_RAWTODEG;
	g_fRoll  =  (float)Roll * IWR_RAWTODEG;
	#endif
}

DARKSDK int ResetLeftEye ( void )
{
	// U75 - 150310 - extra command to detect VR920 AND reset to left eye (otherwise user cross-eyed)
	#ifdef VR920SUPPORT
		g_StereoEyeToggle=LEFT_EYE;
		if ( g_VR920AdapterAvailable==true )
			return 1;
		else
			return 0;
	#else
		return 0;
	#endif
}

DARKSDK void ResetTracking ( void )
{
	// reset to zero (forward facing set)
	#ifdef VR920SUPPORT
	IWRZeroSet();
	// U75 - 250210 - also reset left/right toggle so the eyes are not inverted
	// basically, after commands the engine presents left, then right, so reset
	g_StereoEyeToggle=LEFT_EYE;
	#endif
}

DARKSDK DWORD GetTrackingPitch ( int iFiltering )
{
	// resets grab on pitch call (X)
	g_bGrabTrackingDataOncePerTrio=false;
	if ( g_bGrabTrackingDataOncePerTrio==false )
	{
		ObtainYawPitchRoll ( iFiltering );
		g_bGrabTrackingDataOncePerTrio=true;
	}
	return *(DWORD*)&g_fPitch;
}
DARKSDK DWORD GetTrackingYaw ( int iFiltering )
{
	if ( g_bGrabTrackingDataOncePerTrio==false )
	{
		ObtainYawPitchRoll ( iFiltering );
		g_bGrabTrackingDataOncePerTrio=true;
	}
	return *(DWORD*)&g_fYaw;
}
DARKSDK DWORD GetTrackingRoll ( int iFiltering )
{
	if ( g_bGrabTrackingDataOncePerTrio==false )
	{
		ObtainYawPitchRoll ( iFiltering );
		g_bGrabTrackingDataOncePerTrio=true;
	}
	return *(DWORD*)&g_fRoll;
}

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor,
                                    DWORD* pdwDirectXVersionMinor,
									char* pcDirectXVersionLetter    )
{
    HRESULT hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingMajor = false;
    bool bSuccessGettingMinor = false;
    bool bSuccessGettingLetter = false;

    // Init COM.  COM may fail if its already been inited with a different
    // concurrency model.  And if it fails you shouldn't release it.
    hr = CoInitialize(NULL);
    bCleanupCOM = SUCCEEDED(hr);

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           (LPVOID*) &pDxDiagProvider );
    if( SUCCEEDED(hr) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks        = false;
        dxDiagInitParam.pReserved               = NULL;

        // Init the m_pDxDiagProvider
        hr = pDxDiagProvider->Initialize( &dxDiagInitParam );
        if( SUCCEEDED(hr) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED(hr) )
            {
                // Get the object called DxDiag_SystemInfo
                hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED(hr) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMajor )
                            *pdwDirectXVersionMajor = var.ulVal;
                        bSuccessGettingMajor = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMinor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMinor )
                            *pdwDirectXVersionMinor = var.ulVal;
                        bSuccessGettingMinor = true;
                    }
                    VariantClear( &var );

                    // Get the "szDirectXVersionLetter" property
                    hr = pDxDiagSystemInfo->GetProp( L"szDirectXVersionLetter", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen( var.bstrVal ) != 0 )
                    {
                        char strDestination[10];
                        WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, strDestination, 10*sizeof(CHAR), NULL, NULL );
                        if( pcDirectXVersionLetter )
                            *pcDirectXVersionLetter = strDestination[0];
                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        pDxDiagProvider->Release();
    }

    if( bCleanupCOM )
        CoUninitialize();

    if( bGotDirectXVersion )
        return S_OK;
    else
        return E_FAIL;
}

DARKSDK DWORD GetDirectXVersion ( DWORD pDestStr )
{
	// get version string
	DWORD dwDirectXVersionMajor = 0;
	DWORD dwDirectXVersionMinor = 0;
	char cDirectXVersionLetter = 0;
	HRESULT hRes = GetDirectXVersionViaDxDiag( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter );
	wsprintf ( m_pWorkString, "%d.%d%c", dwDirectXVersionMajor, dwDirectXVersionMinor, cDirectXVersionLetter );

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetDirectXRefreshRate ( DWORD pDestStr )
{
	// Create and return string
	wsprintf ( m_pWorkString, "%d", (int)m_WindowsD3DMODE.RefreshRate );
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

//Dave - new routines to work with true pixels in client windows
DARKSDK void SetChildWindowTruePixel ( int mode )
{
	g_dwChildWindowTruePixel = mode;
}

DARKSDK int GetChildWindowX()
{
	RECT src;
	GetClientRect ( m_hWnd , &src );
	return (int)src.left;
}

DARKSDK int GetChildWindowY()
{
	RECT src;
	GetClientRect ( m_hWnd , &src );
	return (int)src.top;
}

DARKSDK int GetChildWindowWidthEx(int iDPIAware)
{
	RECT src;
	GetClientRect ( m_hWnd , &src );
	int iWidth=(int)src.right;
	if ( iDPIAware==1 )
	{
		HDC hdc = GetDC(NULL);
		int LogicalScreenHeight = GetDeviceCaps(hdc, (int)VERTRES);
		int PhysicalScreenHeight = GetDeviceCaps(hdc, (int)DESKTOPVERTRES); 
		float ScreenScalingFactor = (float)PhysicalScreenHeight / (float)LogicalScreenHeight;
		iWidth = (int)g_pGlob->dwWindowWidth;
		if ( ScreenScalingFactor==1.0f || ScreenScalingFactor==1.25f )
		{
			// 100% or 125% font size
			iWidth = (int)src.right; // ALL
			//if ( g_pGlob->dwWindowWidth==1360 ) iWidth = (int)src.right; // 1360x768
			//if ( g_pGlob->dwWindowWidth==1680 ) iWidth = (int)src.right; // 1680x1050
			//if ( g_pGlob->dwWindowWidth==1920 ) iWidth = (int)src.right; // 1920x900
			//if ( g_pGlob->dwWindowWidth==2048 ) iWidth = (int)src.right; // 2048x1536
			//if ( g_pGlob->dwWindowWidth==2560 ) iWidth = (int)src.right; // 2560x1440
			//if ( g_pGlob->dwWindowWidth==3840 ) iWidth = (int)src.right; // 3840x2160
		}
		if ( ScreenScalingFactor==1.5f )
		{
			// 150% font size
			if ( g_pGlob->dwWindowWidth==1280 ) iWidth = (int)src.right; // 1920x900
			if ( g_pGlob->dwWindowWidth==2560 ) iWidth = (int)src.right; // 3840x2160
		}
	    ReleaseDC(NULL, hdc);
	}
	return iWidth;
}

DARKSDK int GetChildWindowHeightEx(int iDPIAware)
{
	RECT src;
	GetClientRect ( m_hWnd , &src );
	int iHeight=(int)src.bottom;
	if ( iDPIAware==1 )
	{
		HDC hdc = GetDC(NULL);
		int LogicalScreenHeight = GetDeviceCaps(hdc, (int)VERTRES);
		int PhysicalScreenHeight = GetDeviceCaps(hdc, (int)DESKTOPVERTRES); 
		float ScreenScalingFactor = (float)PhysicalScreenHeight / (float)LogicalScreenHeight;
		iHeight = (int)g_pGlob->dwWindowHeight;
		if ( ScreenScalingFactor==1.0f || ScreenScalingFactor==1.25f )
		{
			// 100% or 125% font size
			iHeight = (int)src.bottom; // ALL
			//if ( g_pGlob->dwWindowWidth==1360 ) iHeight = (int)src.bottom; // 1360x768
			//if ( g_pGlob->dwWindowWidth==1680 ) iHeight = (int)src.bottom; // 1680x1050
			//if ( g_pGlob->dwWindowWidth==1920 ) iHeight = (int)src.bottom; // 1920x900
			//if ( g_pGlob->dwWindowWidth==2048 ) iHeight = (int)src.bottom; // 2048x1536
			//if ( g_pGlob->dwWindowWidth==2560 ) iHeight = (int)src.bottom; // 2560x1440
			//if ( g_pGlob->dwWindowWidth==3840 ) iHeight = (int)src.bottom; // 3840x2160
		}
		if ( ScreenScalingFactor==1.5f )
		{
			// 150% font size
			if ( g_pGlob->dwWindowWidth==1280 ) iHeight = (int)src.bottom; // 1920x900
			if ( g_pGlob->dwWindowWidth==2560 ) iHeight = (int)src.bottom; // 3840x2160
		}
		ReleaseDC(NULL, hdc);
	}
	return iHeight;
}

DARKSDK int GetChildWindowWidth()
{
	return GetChildWindowWidthEx(0);
}
DARKSDK int GetChildWindowHeight()
{
	return GetChildWindowHeightEx(0);
}
//////////////////////////////////////////////////////////////////////////////////
// DARK GDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

bool ConstructorDisplay ( void )
{
	return Constructor ( );
}

void DestructorDisplay ( void )
{
	Destructor ( );
}

void SetErrorHandlerDisplay ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr);
}

void PassCoreDataDisplay ( LPVOID pGlobPtr, int iStage  )
{
	PassCoreData ( pGlobPtr, iStage  );
}

int dbCheckDisplayMode ( int iWidth, int iHeight, int iDepth )
{
	return CheckDisplayMode ( iWidth, iHeight, iDepth );
}

char* dbCurrentGraphicsCard ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = CurrentGraphicsCard ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbEmulationMode ( void )
{
	return EmulationMode ( );
}

void dbPerformChecklistForDisplayModes( void )
{
	PerformChecklistForDisplayModes ( );
}

void dbPerformChecklistForGraphicsCards	( void )
{
	PerformChecklistForGraphicsCards ( );
}

int dbScreenType ( void )
{
	return GetDisplayType ( );
}

int dbScreenWidth ( void )
{
	return GetDisplayWidth ( );
}

int dbScreenHeight ( void )
{
	return GetDisplayHeight ( );
}

int dbScreenDepth ( void )
{
	return GetDisplayDepth ( );
}

int dbScreenFPS ( void )
{
	return GetDisplayFPS ( );
}

int dbScreenInvalid ( void )
{
	return GetDisplayInvalid ( );
}

void dbSetGamma ( int iR, int iG, int iB )
{
	SetGamma ( iR, iG, iB );
}

void dbSetDisplayMode ( int iWidth, int iHeight, int iDepth )
{
	SetDisplayModeEx ( iWidth, iHeight, iDepth );
}

void dbSetEmulationOn ( void )
{
	SetEmulationOn ( );
}

void dbSetEmulationOff ( void )
{
	SetEmulationOff ( );
}

void dbSetGraphicsCard ( char* szCardname )
{
	SetGraphicsCard ( ( DWORD ) szCardname );
}

void dbSetWindowOn ( void )
{
	SetWindowModeOn ( );
}

void dbSetWindowOff ( void )
{
	SetWindowModeOff ( );
}

void dbSetWindowLayout ( int iStyle, int iCaption, int iIcon )
{
	SetWindowSettings ( iStyle, iCaption, iIcon );
}

void dbSetWindowPosition ( int posx, int posy )
{
	SetWindowPosition ( posx, posy );
}

void dbSetWindowSize ( int sizex, int sizey )
{
	SetWindowSize ( sizex, sizey );
}

void dbHideWindow ( void )
{
	HideWindow ( );
}

void dbShowWindow ( void )
{
	ShowWindow ( );
}

void dbMinimiseWindow ( void )
{
	MinimiseWindow ( );
}

void dbMaximiseWindow ( void )
{
	MaximiseWindow ( );
}

void dbRestoreWindow ( void )
{
	RestoreWindow ( );
}

void dbSetWindowTitle ( char* pTitleString )
{
	SetWindowTitle ( ( DWORD ) pTitleString );
}

int dbWindowExist ( char* pTitleString )
{
	return WindowExist ( ( DWORD ) pTitleString );
}

void dbWindowToBack ( void )
{
	WindowToBack ( );
}

void dbWindowToFront ( void )
{
	WindowToFront ( );
}

void dbWindowToFront ( char* pTitleString )
{
	WindowToFront ( ( DWORD ) pTitleString );
}

void dbWindowToBack ( char* pTitleString )
{
	WindowToBack ( ( DWORD ) pTitleString );
}

void dbLockBackbuffer ( void )
{
	LockBackbuffer ( );
}

void dbUnlockBackbuffer ( void )
{
	UnlockBackbuffer ( );
}

DWORD dbGetBackbufferPtr ( void )
{
	return GetBackbufferPtr ( );
}

int dbGetBackbufferWidth ( void )
{
	return GetBackbufferWidth ( );
}

int dbGetBackbufferHeight ( void )
{
	return GetBackbufferHeight ( );
}

int dbGetBackbufferDepth ( void )
{
	return GetBackbufferDepth ( );
}

int dbGetBackbufferPitch ( void )
{
	return GetBackbufferPitch ( );
}

IDirect3D9* dbGetDirect3D ( void )
{
	return GetDirect3D ( );
}

IDirect3DDevice9* dbGetDirect3DDevice ( void )
{
	return GetDirect3DDevice ( );
}

void dbBegin ( void )
{
	Begin ( );
}

void dbEnd ( void )
{
	End ( );
}

void dbRender ( void )
{
	Render ( );
}

void dbOverrideHWND ( HWND hWnd )
{
	OverrideHWND ( hWnd );
}

bool dbSetDisplayDebugMode ( void )
{
	return SetDisplayDebugMode ( );
}

bool dbSetDisplayModeEx ( int iWidth, int iHeight )
{
	return SetDisplayMode ( iWidth, iHeight );
}

bool dbSetDisplayModeEx ( int iWidth, int iHeight, int iDepth )
{
	return SetDisplayMode ( iWidth, iHeight, iDepth );
}

bool dbSetDisplayModeEx ( int iWidth, int iHeight, int iDepth, int iMode )
{
	return SetDisplayMode ( iWidth, iHeight, iDepth, iMode );
}

bool dbSetDisplayModeEx ( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing )
{
	return SetDisplayMode ( iWidth, iHeight, iDepth, iMode, iVertexProcessing );
}

bool dbSetDisplayModeEx ( int iWidth, int iHeight, int iDepth, int iMode, int iVertexProcessing, int iLockable )
{
	return SetDisplayMode ( iWidth, iHeight, iDepth, iMode, iVertexProcessing, iLockable );
}

bool dbSetDisplayModeVSync ( int iWidth, int iHeight, int iDepth, int iVSyncOn )
{
	SetDisplayModeVSYNC ( iWidth, iHeight, iDepth, iVSyncOn );
	return true;
}

bool dbSetDisplayModeAntialias ( int iWidth, int iHeight, int iDepth, int iVSyncOn, int iMultisamplingFactor, int iMultimonitorMode )
{
	SetDisplayModeANTIALIAS ( iWidth, iHeight, iDepth, iVSyncOn, iMultisamplingFactor, iMultimonitorMode );
	return true;
}

void dbRestoreDisplayMode ( void )
{
	RestoreDisplayMode ( );
}

void dbRestoreLostDevice ( void )
{
	RestoreLostDevice ( );
}

void dbClear ( int iR, int iG, int iB )
{
	Clear ( iR, iG, iB );
}

void dbGetGamma ( int* piR, int* piG, int* piB )
{
	GetGamma ( piR, piG, piB );
}

bool dbGetWindowedMode ( void )
{
	return GetWindowedMode ( );
}

int dbGetNumberOfDisplayModes ( void )
{
	return GetNumberOfDisplayModes ( );
}

void dbGetDisplayMode ( int iID, char* szMode )
{
	GetDisplayMode ( iID, szMode );
}

int dbGetNumberOfDisplayDevices ( void )
{
	return GetNumberOfDisplayDevices ( );
}

void dbSetDisplayDevice	( int iID )
{
	SetDisplayDevice ( iID );
}

void dbSetDitherMode ( int iMode )
{
	SetDitherMode ( iMode );
}

void dbSetShadeMode ( int iMode )
{
	SetShadeMode ( iMode );
}

void dbSetLightMode ( int iMode )
{
	SetLightMode ( iMode );
}

void dbSetCullMode ( int iMode )
{
	SetCullMode ( iMode );
}

void dbSetSpecularMode ( int iMode )
{
	SetSpecularMode ( iMode );
}

void dbSetRenderState ( int iState, int iValue )
{
	SetRenderState ( iState, iValue );
}

int dbDesktopWidth ( void )
{
	return GetDesktopWidth();
}

int dbDesktopHeight ( void )
{
	return GetDesktopHeight();
}

// lee - 300706 - GDK fixes
void dbMinimizeWindow ( void ) { return dbMinimiseWindow (); }
void dbMaximizeWindow ( void ) { return dbMaximiseWindow (); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
