//
// DarkDLLCore
//

// Standard Includes
#define _CRT_SECURE_NO_DEPRECATE
#define WINVER 0x0601
#include "windows.h"
#include "math.h"
#include "time.h"

// External Includes
#include "..\error\cerror.h"
#include "..\..\DarkSDK\Core\resource.h"
#include "..\..\..\DBPCompiler\Encryptor.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

// Include Memory Manager & Globals
#include ".\..\MemoryManager\DarkMemoryManager.h"
char g_MM_DLLName [ 256 ] = { "Core" };
char g_MM_FunctionName [ 256 ]= { "<none>" };

// Internal Includes
#include "DBDLLCore.h"
#include "DBDLLDisplay.h"
#include "DBDLLCoreInternal.h"
#include "DBDLLArray.h"
#include "RenderList.h"

// Vectors and stack
#include <vector>
#include <stack>

DB_ENTER_NS()
	
//f64 CMathTable::g_sin[RESOLUTION];

DB_LEAVE_NS()

// External Pointers (for cores own error handling)
extern CRuntimeErrorHandler* g_pErrorHandler;

// Prototypes
LPSTR GetTypePatternCore ( LPSTR dwTypeName, DWORD dwTypeIndex );
DWORD GetNextSyncDelay();

// Touch System works under XP and Win7 now
bool bDetectAndActivateWindows7TouchSystem = false;

// Global Core Vars
DBPRO_GLOBAL LPSTR			g_pVarSpace					= NULL;
DBPRO_GLOBAL LPSTR			g_pDataSpace				= NULL;

// Global Stack Store Vars
DBPRO_GLOBAL DWORD			g_dwStackStoreSize			= 0;
DBPRO_GLOBAL DWORD*			g_pStackStore				= NULL;

// Global Performance Switches
DBPRO_GLOBAL bool			g_bAlwaysActiveOff			= false;
DBPRO_GLOBAL bool			g_bProcessorFriendly		= false; // leefix - 070403 - patch 4 slowdown bug
DBPRO_GLOBAL bool			g_bAlwaysActiveOneOff		= false; // leeadd - 201204 - flag to draw just once (typically when PAINT refreshes)
DBPRO_GLOBAL bool			g_bSyncOff					= true;
DBPRO_GLOBAL bool			g_bSceneStarted				= false;
DBPRO_GLOBAL bool			g_bCanRenderNow				= true;
DBPRO_GLOBAL DWORD			g_dwSyncMask				= 0xFFFFFFFF;

// Global Sync Settings
DBPRO_GLOBAL DWORD			g_dwManualSuperStepSetting	= 0;
DBPRO_GLOBAL DWORD*         g_pdwSyncRateSetting        = NULL;
DBPRO_GLOBAL DWORD          g_dwSyncRateSettingSize     = 0;
DBPRO_GLOBAL DWORD          g_dwSyncRateCurrent         = 0;

// Global Performance Flags used Internally
DBPRO_GLOBAL bool			g_bCascadeQuitFlag			= false;
DBPRO_GLOBAL bool			g_bUseExternalDisplayLayer	= false;
DBPRO_GLOBAL bool			g_bExternalDisplayActive	= false;
DBPRO_GLOBAL DWORD			g_dwRecordedTimer			= 0;

// Global Error Handling and Pointers
DBPRO_GLOBAL LPSTR			g_pCommandLineString		= NULL;
DBPRO_GLOBAL LPVOID			g_ErrorHandler				= NULL;
DBPRO_GLOBAL LPVOID			g_EscapeValue				= NULL;
DBPRO_GLOBAL LPVOID			g_BreakOutPosition			= NULL;

// U71 - added to store structure patterns in core (passed in from EXEBlock)
DBPRO_GLOBAL DWORD			g_dwStructPatternQty		= 0;
DBPRO_GLOBAL LPSTR			g_pStructPatternsPtr		= NULL;

// Global Display Vars
DBPRO_GLOBAL HBITMAP		g_hDisplayBitmap			= NULL;
DBPRO_GLOBAL HDC			g_hdcDisplay				= NULL;
DBPRO_GLOBAL COLORREF		g_colFore					= RGB(255,255,255);
DBPRO_GLOBAL COLORREF		g_colBack					= RGB(0,0,0);
DBPRO_GLOBAL HBRUSH			g_hBrush					= NULL;
DBPRO_GLOBAL DWORD			g_dwScreenWidth				= 0;
DBPRO_GLOBAL DWORD			g_dwScreenHeight			= 0;

DBPRO_GLOBAL HICON			g_hUseIcon					= NULL;
DBPRO_GLOBAL HCURSOR		g_hUseArrow 				= NULL;
DBPRO_GLOBAL HCURSOR		g_hUseHourglass 			= NULL;
DBPRO_GLOBAL HCURSOR		g_hCustomCursors[30];
DBPRO_GLOBAL HCURSOR		g_ActiveCursor 				= NULL;
DBPRO_GLOBAL HCURSOR		g_OldCursor 				= NULL;

// Global Draw Order Flags
DBPRO_GLOBAL bool			g_bDrawAutoStuffFirst		= true;
DBPRO_GLOBAL bool			g_bDrawSpritesFirst			= false;
DBPRO_GLOBAL bool			g_bDrawEntirelyToCamera		= false;
DBPRO_GLOBAL bool			g_bDrawQuadInSync			= true; // but quad draw skipped if RenderQuad(0) returns zero

// Global Input Vars
DBPRO_GLOBAL DWORD			g_dwWindowsTextEntrySize	= 0;
DBPRO_GLOBAL DWORD			g_dwWindowsTextEntryPos		= 0;
DBPRO_GLOBAL unsigned char	g_cKeyPressed				= 0;
DBPRO_GLOBAL unsigned char	g_cInkeyCodeKey				= 0;
DBPRO_GLOBAL int			g_iEntryCursorState			= 0;
DBPRO_GLOBAL WORD			g_wWinKey					= 0;

// Global Data Vars
DBPRO_GLOBAL LPSTR			g_pDataLabelStart			= NULL;
DBPRO_GLOBAL LPSTR			g_pDataLabelPtr				= NULL;
DBPRO_GLOBAL LPSTR			g_pDataLabelEnd				= NULL;

// Global Security Data
DBPRO_GLOBAL int			g_iSecurityCode				= 0;

// Global Data Shared By DLLs
DBPRO_GLOBAL GlobStruct		g_Glob;
DBPRO_GLOBAL GlobStruct*	g_pGlob = &g_Glob;

// Small helper function to get more accurate timings
class AccurateTimer
{
private:
	UINT  Period;
	DWORD LastTime;

	// Disable copying
	AccurateTimer(const AccurateTimer&);
	AccurateTimer& operator=(const AccurateTimer&);
public:
	AccurateTimer() 
	{
		TIMECAPS caps;
		timeGetDevCaps(&caps, sizeof(caps));
		Period = caps.wPeriodMin;
		timeBeginPeriod(Period);
	}
	~AccurateTimer()
	{
		timeEndPeriod(Period);
	}
	DWORD Get()
	{
		LastTime = timeGetTime();
		return LastTime;
	}
	DWORD Last() const
	{
		return LastTime;
	}
};




//
// DLL Entry and Exit Function
//
BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// Memory Manager Identity
	strcpy ( g_MM_FunctionName, "DllMain" );
	#ifdef  __USE_MEMORY_MANAGER__
	mm_SnapShot();
	#endif

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:

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

			// leeprepare - 130104 - U6 GLOBSTRUCT DYNAMIC ARRAY
			//g_Glob.dwDynMemSize = (DWORD)(1*sizeof(DWORD));
			//g_Glob.pDynMemPtr = new char [ g_Glob.dwDynMemSize ];

			// aaron - 20120813 - sine table generation
			{
				db3::CMathTable genSinTable;
			}

			break;

		case DLL_PROCESS_DETACH:
		{
			// free dynamic array within globstruct
			//SAFE_DELETE ( g_Glob.pDynMemPtr );

			// done
			break;
		}
	}
	return TRUE;
}

// WINDOWS FRIENDLY FUNCTIONS

bool IsArraySingleDim ( DWORD dwArrayPtr )
{
	// Detect if array has single dimension only, false if multi or irregular array
	DWORD* pOldHeader = (DWORD*)(((LPSTR)dwArrayPtr)-HEADERSIZEINBYTES);
	DWORD dwSizeOfOneDataItem = pOldHeader[11];
	if ( dwSizeOfOneDataItem > 1024000 ) return false;
	if ( pOldHeader [ 1 ] > 0 ) return false;
	return true;
}

DARKSDK DWORD ProcessMessagesOnly(void)
{
	// U76 - Windows 7 touch has no 'touch-release' via WM_MOUSE commands
	// so create an artificial persistence so MOUSECLICK(DX) can detect it
	if ( g_Glob.dwWindowsMouseLeftTouchPersist > 0 )
		if ( timeGetTime() > g_Glob.dwWindowsMouseLeftTouchPersist )
			g_Glob.dwWindowsMouseLeftTouchPersist=0;

	// Vars
	MSG msg;

	// Cascade means it will continue to quit (for rapid exit)
	if(g_bCascadeQuitFlag==true)
		return 1;

	// Message Pump
	while(TRUE)
	{
		// Standard Windows Processing
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			if(msg.message!=WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				g_bCascadeQuitFlag=true;
				return 1;
			}
		}
		else
		{
			// Processor Friendly
			if(g_bProcessorFriendly) Sleep(1);
			break;
		}
	}

	// Complete
	return 0;
}

DARKSDK void ConstantNonDisplayUpdate(void)
{
	// Update All NonVisuals (this gets called about six times because of processmessage calls..)
	if(g_Music_Update) g_Music_Update();
	if(g_Sound_Update) g_Sound_Update();
	if(g_Animation_UpdateAnims) g_Animation_UpdateAnims();
}

DARKSDK void ExternalDisplaySync ( int iSkipSyncRateCodeAkaFastSync )
{
	// Skip this phase if app has been shut down (always active off)
	if ( g_bAlwaysActiveOneOff ) 
		return;

	// V111 - 110608 - FASTSYNC should not use sync delay!
	if ( iSkipSyncRateCodeAkaFastSync==0 )
	{
		AccurateTimer Timer;

		// Skip refreshes causing even faster FPS rates!
		if(g_dwManualSuperStepSetting>0)
		{
			if(Timer.Get()-g_dwRecordedTimer<g_dwManualSuperStepSetting)
				return;
		}

		// Force FPS
		if(g_dwSyncRateSettingSize>0)
		{
			DWORD dwDifference = GetNextSyncDelay();
			while(Timer.Get()-g_dwRecordedTimer < dwDifference)
				if(ProcessMessagesOnly()==1) return;
		}
		else
		{
			// Need to ad least process these monitors
			ConstantNonDisplayUpdate();
		}

		// u74b7 - Only update the sync timer if not using fastsync
		// Record time of update
		g_dwRecordedTimer = Timer.Last();
	}

	// leefix - 260604 - u54 - in case input wants single data-grab functionality
	if(g_Input_ClearData) g_Input_ClearData();

	// 270515 - calls this to grab latest viewproj and record previous viewproj
	if ( iSkipSyncRateCodeAkaFastSync==0 )
		if ( g_Basic3D_UpdateViewProjForMotionBlur ) 
			g_Basic3D_UpdateViewProjForMotionBlur();

	// If using External Graphics API
	if(g_bExternalDisplayActive)
	{
		// leeadd - 160306 - u60 - camera zero off suspends normal operations
		bool bSuspendScreenOperations = false;
		if ( !(g_dwSyncMask & 1) )
		{
			// flag the suspension of regular screen zero activity
			bSuspendScreenOperations = true;
		}
		if ( bSuspendScreenOperations==false )
		{
			// If BSP used, compute responses
			if(g_World3D_End) g_World3D_End();
			if(g_Basic3D_AutomaticCollisionEnd) g_Basic3D_AutomaticCollisionEnd();

			// Draw Phase : Store backbuffer before any 3D is drawn..
			if(g_Sprites_SaveBack) g_Sprites_SaveBack();

			// Draw Phase : Draw Sprites Last
			if(g_bDrawSpritesFirst==false)
				if(g_Sprites_Update)
					g_Sprites_Update();
			
			// Ensures AutoStuff is first to be rendered
			if(g_bDrawAutoStuffFirst==true)
			{
				if(g_bSceneStarted)
				{
					// leeadd - 071108 - U71 - render quad if flagged
					if ( g_bDrawQuadInSync )
					{
						if(g_Basic3D_RenderQuad)
						{
							if(g_Basic3D_RenderQuad(0)==1)
							{
								g_Camera3D_RunCode ( 0 );
								g_Basic3D_RenderQuad(1);
							}
						}
					}
					g_GFX_End();
					if ( g_bCanRenderNow )
						g_GFX_Render();
				}
				g_bSceneStarted=true;
				g_GFX_Begin();

				// restore before-Sprites-drawn on new screen render
				if(g_Sprites_RestoreBack) g_Sprites_RestoreBack();
			}

			// Draw Phase : Draw Sprites First
			if(g_bDrawSpritesFirst==true)
				if(g_Sprites_Update)
					g_Sprites_Update();
		}

		// Draw Phase : Draw 3D Gemoetry
		if( g_Camera3D_StartSceneInt )
		{
			// Reset polycount and drawprim count
			if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn=0;
			if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls=0;

			// Disable backdrop if camera zero disabled
			int iMode = 0; if ( bSuspendScreenOperations ) iMode = 1;

			// U75 - 080410 - ensure animation in scene only calculated once (on SYNC)
			if ( iSkipSyncRateCodeAkaFastSync==0 )
				if(g_Basic3D_UpdateAnimationCycle)
					g_Basic3D_UpdateAnimationCycle();

			// Draw all 3D - all cameras loop
			g_Camera3D_StartSceneInt ( iMode );
			do 
			{
				int iThisCamera = 1 + g_Camera3D_GetRenderCamera();
				if ( iThisCamera <= 32 )
				{
					// camera 0 - 31 can be masked
					DWORD dwCamBit = 1;
					if ( iThisCamera > 1 ) dwCamBit = dwCamBit << (DWORD)(iThisCamera-1);
					dwCamBit = dwCamBit & g_dwSyncMask;
					if ( dwCamBit==0 ) iThisCamera = 0;
					// 20120313 IanM - Removed incorrect 'prediction' code for next camera rendering.
				}
				if ( iThisCamera > 0 )
				{
					// Push all polygons for 3D components
					for(g_pGlob->dwRedrawPhase=0; g_pGlob->dwRedrawPhase<g_pGlob->dwRedrawCount; g_pGlob->dwRedrawPhase++)
					{
						// u74b8 - replace hard-coded calls with a dynamic list of function pointers
						ExecuteRenderList();
					}
				}
				// Next camera or finish..
			} while (g_Camera3D_FinishSceneEx(false)==0);

			// leeadd - 071107 - U71 - after 3D operations, direct whether SPRITES/2D/IMAGE
			// drawing is to take place by default (bitmap or camera zero)
			if ( g_bDrawEntirelyToCamera==true ) g_Camera3D_RunCode ( 1 );

		}
		if ( bSuspendScreenOperations==false )
		{
			// Ensures AutoStuff is last to be rendered
			if(g_bDrawAutoStuffFirst==false)
			{
				if(g_bSceneStarted)
				{
					// leeadd - 071108 - U71 - render quad if flagged
					if ( g_bDrawQuadInSync )
					{
						if(g_Basic3D_RenderQuad)
						{
							if(g_Basic3D_RenderQuad(0)==1)
							{
								g_Camera3D_RunCode ( 0 );
								g_Basic3D_RenderQuad(1);
							}
						}
					}
					g_GFX_End();
					if ( g_bCanRenderNow )
						g_GFX_Render();
				}
				g_bSceneStarted=true;
				g_GFX_Begin();

				// restore before-Sprites-drawn on new screen render
				if(g_Sprites_RestoreBack) g_Sprites_RestoreBack();
			}

			// If BSP used, set response check
			if(g_World3D_Start) g_World3D_Start();
			if(g_Basic3D_AutomaticCollisionStart) g_Basic3D_AutomaticCollisionStart();
		}
	}
}

DARKSDK void ExternalDisplayUpdate(void)
{
	// Call external sync if automatic
	if(g_bSyncOff) ExternalDisplaySync(0);
}

#ifdef DARKSDK_COMPILE
extern int g_iDarkGameSDKQuit;
#endif

/* Proved incompatible with Windows XP (user.dll)
LRESULT DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// U76 - added but not used (yet)
	GESTUREINFO gi;  
	ZeroMemory(&gi, sizeof(GESTUREINFO));
	gi.cbSize = sizeof(GESTUREINFO);
	BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);
	BOOL bHandled = FALSE;
	if ( bResult )
	{
		// now interpret the gesture
		switch (gi.dwID)
		{
		   case GID_ZOOM:
			   // Code for zooming goes here     
			   break;
		   case GID_PAN:
			   // Code for panning goes here
			   break;
		   case GID_ROTATE:
			   // Code for rotation goes here
			   break;
		   case GID_TWOFINGERTAP:
			   // Code for two-finger tap goes here
			   break;
		   case GID_PRESSANDTAP:
			   // Code for tap goes here
			   break;
		   default:
			   // A gesture was not recognized
			   break;
		}
	}
	else
	{
		DWORD dwErr = GetLastError();
		if (dwErr > 0)
		{
			//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
		}
	}
	if ( bHandled )
	{
		return 0;
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
*/

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_SETTEXT:
		{
			// leeadd - 130306 - igl - handle HWND message
			LPSTR pIncomingStr = (LPSTR)lParam;
			if ( g_Glob.hwndIGLoader==NULL )
			{
				if ( pIncomingStr )
				{
					if ( strlen ( pIncomingStr ) > 7 )
					{
						// only if IGL text message
						if ( strnicmp ( pIncomingStr, "PARENT|", 7  )==NULL )
						{
							// extract HWND from lParam
							char str [ 256 ];
							strcpy ( str, pIncomingStr + 7 );
							int num = atoi ( str );
							g_Glob.hwndIGLoader = (HWND)num;

							// not a title text change
							return 0;
						}
					}
				}
			}
			else
			{
				// messages can be sent from igloader, relay them to the igLoader DLL if present
				if ( hWnd==g_Glob.hWnd )
				{
					// from igLoader, if IGL DLL present, send this text to it
					if ( g_Glob.g_igLoader )
					{
						// send raw string to igloader dll for adding to callback list
						typedef void ( *IGL_AddStringToCallbackList ) ( LPSTR );
						IGL_AddStringToCallbackList AddToCBList;
						AddToCBList = ( IGL_AddStringToCallbackList ) GetProcAddress ( g_Glob.g_igLoader, "?IglAddSetTextString@@YAXPAD@Z" );
						if ( AddToCBList ) AddToCBList ( pIncomingStr );

						// no window text change
						return 0;
					}
				}
			}
		}

		case WM_ACTIVATE:
		{
			HWND hwndPrevious = (HWND) lParam;       // window handle
			if ( hwndPrevious==NULL || hwndPrevious==hWnd )
			{
				if(LOWORD(wParam)==WA_ACTIVE
				|| LOWORD(wParam)==WA_CLICKACTIVE)
				{
					// Used to signal a refresh
					///g_Glob.bInvalidFlag=true; 060215 - interferes with foreground enforcement (SteamUI)!

//					// leeremove - 180705 - caused huge delays if active messages got confused
//					// leeadd - 201204 - if active, enable sync
//					if ( g_bAlwaysActiveOff )
//					{
//						g_bAlwaysActiveOneOff = false;
//						g_bProcessorFriendly = false;
//					}
				}
				else
				{
//					// leeremove - 180705 - caused huge delays if active messages got confused
//					// leeadd - 201204 - if inactive, disable sync
//					if ( g_bAlwaysActiveOff )
//					{
//						g_bAlwaysActiveOneOff = true;
//						g_bProcessorFriendly = true;
//					}
				}
			}

			// 20/7/11 - Win7 - ensure we register for TOUCH over GESTURE (also allows LBUTTONDOWN to happen instantly!)
			if ( bDetectAndActivateWindows7TouchSystem==false )
			{
				bDetectAndActivateWindows7TouchSystem = true;
				OSVERSIONINFO osvi;
				BOOL bIsWindows7orLater;
				ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				GetVersionEx(&osvi);
				bIsWindows7orLater = ( (osvi.dwMajorVersion > 6) || ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1) ));
				if ( bIsWindows7orLater==TRUE )
				{
					// must dynamically find the user32.dll function and call it IF Windows 7 (allows Windows XP to run)
					// RegisterTouchWindow(g_Glob.hWnd, 0);
					typedef UINT (CALLBACK* sRegisterTouchWindowFnc)(HWND,ULONG);
					HMODULE hWinUserDLL = LoadLibrary ( "user32.dll" );
					if ( hWinUserDLL )
					{
						sRegisterTouchWindowFnc pRegTouchWin = (sRegisterTouchWindowFnc) GetProcAddress ( hWinUserDLL, "RegisterTouchWindow" );
						if ( pRegTouchWin ) BOOL bRes = pRegTouchWin ( g_Glob.hWnd, 0 );
						FreeLibrary ( hWinUserDLL );
					}
				}
			}

			break;
		}

		case WM_CLOSE:
		{
			#ifdef DARKSDK_COMPILE
			g_iDarkGameSDKQuit = 1;
			#endif
			PostQuitMessage(0);
			return TRUE;
		}

		case WM_DESTROY:
		case WM_NCDESTROY:
		{
			#ifdef DARKSDK_COMPILE
			g_iDarkGameSDKQuit = 1;
			#endif
			PostQuitMessage(0);
			break;
		}

		case WM_ERASEBKGND:
			return TRUE;

		case WM_SIZE:
		case WM_SIZING:
		case WM_MOVE:
		case WM_MOVING:
		case WM_PAINT:
			{
				// 180214 - record new size in glob struct
				RECT rc;
				//if ( message==WM_SIZE )
				//{
				//	GetClientRect(hWnd, &rc);
				//	g_pGlob->dwWindowWidth = rc.right - rc.left;
				//	g_pGlob->dwWindowHeight = rc.bottom - rc.top;
				//}

				// GDI Paint
				PAINTSTRUCT ps;
				HDC hdcClient = BeginPaint(hWnd, &ps);
				if(hdcClient)
				{
					if(g_hdcDisplay)
					{
						GetClientRect(hWnd, &rc);
						HGDIOBJ hdcOld = SelectObject(g_hdcDisplay, g_hDisplayBitmap);
						BitBlt(hdcClient, rc.left, rc.top, rc.right, rc.bottom, g_hdcDisplay, 0, 0, SRCCOPY);
						SelectObject(g_hdcDisplay, hdcOld);
					}
					else
					{
						// 210203 - if array of protected boxes setup (from controls requiring primary surface)
						if ( g_pGlob->dwSafeRectMax>0 )
						{
							// Clear Device
							GetClientRect(hWnd, &rc);
							HBRUSH bGrey = GetSysColorBrush ( COLOR_3DFACE );
							HBRUSH bOld = (HBRUSH)SelectObject(hdcClient, bGrey ); 
							Rectangle(hdcClient, -5, -5, rc.right+5, rc.bottom+5);
							SelectObject(hdcClient, bOld ); 
						}
					}
					EndPaint(hWnd, &ps);
				}

				// Ensures rendered areas are retained (when moving window or menu refreshing)
				if ( g_Glob.dwAppDisplayModeUsing==1 )
				{
					// only dwDisplayMode=1 (window) should do this (otherwise render several times!!)
					if (g_GFX_End && g_GFX_Render && g_GFX_Begin)
					{
						// ensure refresh is not done in middle of draw-phase
						g_GFX_End(); g_GFX_Render(); g_GFX_Begin();
					}
				}
			}
			return TRUE;

		case WM_MOUSEMOVE:
			{
				// Get Client Raw Mouse Position
				g_Glob.iWindowsMouseX = LOWORD(lParam);  // horizontal position of cursor 
				g_Glob.iWindowsMouseY = HIWORD(lParam);  // vertical position of cursor 
				
				// Special Scale for When Windows Stretch Beyond Physical Size of Backbuffer
				RECT rc;
				GetClientRect(hWnd, &rc);
				float xRatio = (float)g_Glob.dwWindowWidth/(float)rc.right;
				float yRatio = (float)g_Glob.dwWindowHeight/(float)rc.bottom;
				g_Glob.iWindowsMouseX = (int)((float)g_Glob.iWindowsMouseX * xRatio);
				g_Glob.iWindowsMouseY = (int)((float)g_Glob.iWindowsMouseY * yRatio);

				// Restore cursor when move mouse
				if ( g_ActiveCursor != NULL ) SetCursor ( g_ActiveCursor );

			}
			break;

		case WM_LBUTTONDOWN:
			g_Glob.iWindowsMouseClick|=1;
			g_Glob.dwWindowsMouseLeftTouchPersist=timeGetTime()+250; // U76 - many cycles
			if ( GetFocus()!=hWnd ) 
			{
				SetFocus ( hWnd );
				// 060215 - puts this back in foreground
				//SetForegroundWindow ( hWnd );
			}
			break;

		case WM_RBUTTONDOWN:
			g_Glob.iWindowsMouseClick|=2;
			if ( GetFocus()!=hWnd ) SetFocus ( hWnd );
			break;

		// aaron - 20120811 - Potential issues when using xor depending on obscure and rare window interaction
		case WM_LBUTTONUP:
			g_Glob.iWindowsMouseClick &= ~1UL;
			//g_Glob.iWindowsMouseClick^=1;
			break;

		case WM_RBUTTONUP:
			g_Glob.iWindowsMouseClick &= ~2UL;
			//g_Glob.iWindowsMouseClick^=2;
			break;

//		case WM_GESTURE:
//			// U77 - Touch for Windows 7
//			// ensure tap = click (not mouse position)
//			return DecodeGesture(hWnd, message, wParam, lParam);

		case WM_SYSKEYDOWN:
			g_wWinKey = wParam; // leefix - 240604 - u54 - for WAIT KEY bug F10
			break;

		case WM_KEYDOWN:
			g_wWinKey = wParam;
			if((int)wParam==VK_ESCAPE)
			{
				if(g_EscapeValue) *(DWORD*)g_EscapeValue=1;
				if(g_Glob.bEscapeKeyEnabled)
				{
					#ifdef DARKSDK_COMPILE
					g_iDarkGameSDKQuit = 1;
					#endif
					PostQuitMessage(0);
				}
			}
			return TRUE;

		case WM_SYSKEYUP:
			g_wWinKey=0; // leefix - 240604 - u54 - for WAIT KEY bug F10
			return TRUE;

		case WM_KEYUP:
			g_cInkeyCodeKey=0;
			g_wWinKey=0;
			return TRUE;

		case WM_CHAR:

			// If win string cleared externally (InputDLL)
			if(g_Glob.pWindowsTextEntry)
				if(g_Glob.pWindowsTextEntry[0]==0)
					g_dwWindowsTextEntryPos=0;

			// Key that was pressed
			g_cKeyPressed = (unsigned char)wParam;
			g_cInkeyCodeKey = g_cKeyPressed;

			// Ensure string is always big enough
			if(g_Glob.pWindowsTextEntry==NULL)
			{
				g_dwWindowsTextEntrySize = 32;
				g_Glob.pWindowsTextEntry = new char[g_dwWindowsTextEntrySize];
				g_dwWindowsTextEntryPos = 0;
			}
			if(g_dwWindowsTextEntryPos>g_dwWindowsTextEntrySize-4)
			{
				g_dwWindowsTextEntrySize = g_dwWindowsTextEntrySize * 2;
				LPSTR pNewString = new char[g_dwWindowsTextEntrySize];
				strcpy(pNewString, g_Glob.pWindowsTextEntry);
				delete[] g_Glob.pWindowsTextEntry;
				g_Glob.pWindowsTextEntry=pNewString;
			}

			// leeadd - 020605 - Add/Remove from entry$() string
			// leeafix - 110206 - created behaviour disaster in U59 - place this behaviour in ENTRY$(1) parameter of CINPUT.CPP
//			if ( g_cKeyPressed==8 )
//			{
//				// Remove character from entry
//				if ( g_dwWindowsTextEntryPos>0 )
//				{
//					g_dwWindowsTextEntryPos--;
//					g_Glob.pWindowsTextEntry[g_dwWindowsTextEntryPos]=0;
//				}
//			}
			// Add character to entry string
			g_Glob.pWindowsTextEntry[g_dwWindowsTextEntryPos]=g_cKeyPressed;
			g_dwWindowsTextEntryPos++;
			g_Glob.pWindowsTextEntry[g_dwWindowsTextEntryPos]=0;

			return TRUE;

		case WM_USER+1: // Show/Hide Cursor
			if(wParam==0) ShowCursor(FALSE);
			if(wParam==1) ShowCursor(TRUE);
			return TRUE;
	}
	
	// Default Action
	return DefWindowProc(hWnd, message, wParam, lParam);
}

DARKSDK void InternalClearWindowsEntry(void)
{
	if(g_Glob.pWindowsTextEntry)
	{
		strcpy(g_Glob.pWindowsTextEntry,"");
		g_dwWindowsTextEntryPos		= 0;
		g_cKeyPressed				= 0;
	}
}

DARKSDK DWORD InternalProcessMessages(void)
{
	DWORD dwResult = ProcessMessagesOnly();
	//if(g_bSyncOff) ConstantNonDisplayUpdate(); // leefix - 100605 - added 'g_bSyncOff' so ONLY SYNC does this when SYNC ON

	// mike - 101005 - not calling this and using fastsync results in problems for 3D sound
	// lee - 100208 - placing this here slows down tight loops and large programs by several MS (moved to FASTSYNC)
	// ConstantNonDisplayUpdate();


	if(g_bUseExternalDisplayLayer) ExternalDisplayUpdate();
	return dwResult;
}

DARKSDK DWORD ProcessMessages(DWORD dwPositionInMachineCode)
{
	// Process Messages from a program in debug mode
	DWORD dwReturnValue=0;

	// When breakout position filled, leave immediately
	if(g_BreakOutPosition)
	{
		// If Exit requested, store position before leave
		if(*(DWORD*)g_BreakOutPosition==1)
		{
			*(DWORD*)g_BreakOutPosition=dwPositionInMachineCode;
			return 1;
		}
	}

	// Process Internal Message Loop
	dwReturnValue = InternalProcessMessages();

	// Return Value
	return dwReturnValue;
}

DARKSDK DWORD ProcessMessages(void)
{
	// Process Messages from a program in fullspeed mode
	return InternalProcessMessages();
}

DARKSDK DWORD Quit(void)
{
	// Initate Cascade Quit
	g_bCascadeQuitFlag=true;
	if(g_EscapeValue) *(DWORD*)g_EscapeValue=2;

	// Process any other tasks during Final QUIT
	if(g_Glob.pExitPromptString)
	{
		// Produce an Exit Window with Strings
		MessageBox(NULL, g_Glob.pExitPromptString, g_Glob.pExitPromptString2, MB_OK);

		// Free Strings
		SAFE_DELETE(g_Glob.pExitPromptString);
		SAFE_DELETE(g_Glob.pExitPromptString2);
	}

	// Complete
	return 0;
}

DARKSDK void StackSnapshotStore(DWORD dwStackPositionNow)
{
	// No Stack save - data would be useless when new m/c executed
}

DARKSDK DWORD StackSnapshotRestore(void)
{
	// No Stack save - data would be useless when new m/c executed
	return 0;
}

// aaron - 20120811 - more flexible memory management routine
// p: in pointer (nullptr: alloc; else: realloc)
// n: size to have (0: free)
DB_EXPORT void *ManageMemory(void *p, size_t n) {
	void *q;

	if (n) {
		if (p)
			q = realloc(p, n);
		else
			q = malloc(n);
	} else {
		if (p)
			free(p);

		return nullptr;
	}

	return q;
}

DARKSDK int TestMemory ( int iSizeInBytes )
{
	try
	{
		void* pMem = new char[iSizeInBytes];
		if ( pMem )
		{
			// can still reserve memory chunk
			delete pMem;
			return 1;
		}
		else
			return 0;
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}

DARKSDK void CreateSingleString(DWORD* dwVariableSpaceAddress, DWORD dwSize)
{
	// aaron - 20120811 - more efficient implementation (see above)
#if 0
	*dwVariableSpaceAddress = (DWORD)ManageMemory(*(void **)dwVariableSpaceAddress, dwSize);
#else
	if(dwSize>0)
	{
		// Create a core string
		*dwVariableSpaceAddress = (DWORD)new char[dwSize];
	}
	else
	{
		// Delete a core string
		delete[] (LPSTR)*dwVariableSpaceAddress;
	}
#endif
}

DARKSDK void Break(void)
{
	// Set Escape Value to Break Into Debugger
	if(g_EscapeValue) *(DWORD*)g_EscapeValue=1;
}

DARKSDK LRESULT SendDataToDebugger(int iType, LPSTR pData, DWORD dwDataSize)
{
	LRESULT lResult=0;

	// Create Virtual File for Transfer
	HANDLE hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,dwDataSize,"DBPROEDITORMESSAGE");
	if(hFileMap)
	{
		LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,dwDataSize+4);
		if(lpVoid)
		{
			// Copy to Virtual File
			*(DWORD*)lpVoid = dwDataSize;
			memcpy((LPSTR)lpVoid+4, pData, dwDataSize);

			// Find Debugger to send to
			HWND hWnd = FindWindow(NULL,"DBProDebugger");
			if(hWnd)
			{
				// Found - transmit
				lResult = SendMessage(hWnd, WM_USER+10, iType, 0);
			}

			// Release virtual file
			UnmapViewOfFile(lpVoid);
		}
		CloseHandle(hFileMap);
	}

	// May have result
	return lResult;
}

DARKSDK void BreakS(DWORD pString)
{
	// Send String to CLI Debug Console
	LPSTR lpReturnError = new char[1024];
	wsprintf(lpReturnError, "%s", pString);
	SendDataToDebugger(31, lpReturnError, strlen(lpReturnError));
	delete[] lpReturnError;
	lpReturnError=NULL;

	// Set Escape Value to Break Into Debugger
	if(g_EscapeValue) *(DWORD*)g_EscapeValue=1;
}

DARKSDK bool DoesFileExist(LPSTR Filename)
{
	// success or failure
	bool bSuccess = true;

	// open File To See If Exist
	HANDLE hfile = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		bSuccess=false;
	else
		CloseHandle(hfile);

	// return result
	return bSuccess;
}

DARKSDK void UpdateFilenameFromVirtualTable( DWORD dwStringAddress )
{
	// String is input with external filename
	if(dwStringAddress==0)
		return;
	
	// If Virtual Table area available
	if(g_Glob.pEXEUnpackDirectory==NULL)
		return;

	// Construct path to virtual file (if it is there or not) leefix - 200704 - can get very big!
	LPSTR pFilename = new char[_MAX_PATH*3];
	strcpy(pFilename, g_Glob.pEXEUnpackDirectory);
	strcat(pFilename, "\\media\\");
	strcat(pFilename, (LPSTR)dwStringAddress);

	// If File exists, use that instead of external file
	if(DoesFileExist(pFilename)==true)
	{
		// Virtual Table File better than local external file
		strcpy((LPSTR)dwStringAddress, pFilename);
	}

	// Free usages
	delete[] pFilename;
}

DARKSDK LPSTR ReadFileData(LPSTR FilenameString, DWORD* dwDataSize)
{
	// Read File Data
	HANDLE hreadfile = CreateFile(FilenameString, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile==INVALID_HANDLE_VALUE)
		return NULL;

	// Read readout file into memory
	DWORD bytesread=0;
	DWORD filebuffersize = GetFileSize(hreadfile, NULL);	
	LPSTR filebuffer = (char*)GlobalAlloc(GMEM_FIXED, filebuffersize);
	ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
	CloseHandle(hreadfile);

	*dwDataSize = filebuffersize;
	return filebuffer;
}

DARKSDK void WriteFileData(LPSTR pFilename, LPSTR pData, DWORD dwDataSize)
{
	// Delete existing file
	DeleteFile(pFilename);

	// Write New File with new data
	DWORD byteswritten=0;
	HANDLE hfile = CreateFile(pFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile!=INVALID_HANDLE_VALUE)
	{
		WriteFile(hfile, pData, dwDataSize, &byteswritten, NULL); 
		CloseHandle(hfile);
	}
}

DARKSDK void EncryptDecrypt( DWORD dwStringAddress, bool bEncryptIfTrue, bool bDoNotUseTempFolder )
{
	// Dave - for safety
	g_Glob.dwEncryptionUniqueKey = 0;

	// String is input with external filename
	if(dwStringAddress==0)
		return;

	// If re-encrypting, and filename is in temp, just delete it and finish
	LPSTR pLocalTRemp = g_Glob.pEXEUnpackDirectory;
	if ( bDoNotUseTempFolder==false && bEncryptIfTrue==true )
	{
		if ( strnicmp ( (LPSTR)dwStringAddress, pLocalTRemp, strlen(pLocalTRemp) )==NULL )
		{
			// Delete temp decrypted file
			if(DoesFileExist((LPSTR)dwStringAddress)) DeleteFile((LPSTR)dwStringAddress);

			// Finish early
			return;
		}
	}

	// length of six... e.g. "_e_Z.x" is the min size file (1 letter name)
	if ( strlen((LPSTR)dwStringAddress) < 6 ) return;

	char checkForEncryptName[_MAX_PATH];
	char keyName[10] = "7yFkC3Oa";
		
	strcpy( checkForEncryptName , (LPSTR)dwStringAddress );

	// a file that needs decrypting will start with "_e_"
	bool okayToProceed = false;
	if ( ( checkForEncryptName[0] == '_' && checkForEncryptName[1] == 'e' && checkForEncryptName[2] == '_' ) || strstr ( checkForEncryptName , "\\_e_" )  )
		okayToProceed = true;
	else
	{
		if ( Steam_CanIUse_W_() || bEncryptIfTrue )
		{
			if ( ( checkForEncryptName[0] == '_' && checkForEncryptName[1] == 'w' && checkForEncryptName[2] == '_' ) || strstr ( checkForEncryptName , "\\_w_" )  )
			{
				okayToProceed = true;
				strcpy ( keyName , "iG72VL8q" );
			}
		}
	}

	if ( okayToProceed )
	{
		// Dave - set key to 1 to show this file is encrypted
		g_Glob.dwEncryptionUniqueKey = 1;

		char stringToMakeKey[_MAX_PATH];
		strcpy ( stringToMakeKey , "" );
		int tLength = 0;
		for ( DWORD c = 0 ; c < strlen(checkForEncryptName) ; c++ )
		{
			if ( checkForEncryptName[c] != '.' )
				stringToMakeKey[tLength++] = checkForEncryptName[c];
				stringToMakeKey[tLength] = 0;
		}

		int len = strlen(stringToMakeKey)-1;
		if ( len <= 0 ) return;

		for ( int c = len; c >= 0 && c > len-9 ; c-- )
		{
			keyName[len - c] = stringToMakeKey[c];
			if ( keyName[len - c] >= 65 && keyName[len - c] < 90 ) keyName[len - c]++;
			else if ( keyName[len - c] >= 97 && keyName[len - c] < 122 ) keyName[len - c]++;
			else if ( keyName[len - c] == 90 ) keyName[len - c] = 65;
			else if ( keyName[len - c] == 122 ) keyName[len - c] = 97;
		}
	}
	else
		return;

	// If exe not setup for encryption, dont use it
	if(g_Glob.dwEncryptionUniqueKey==0)
		return;

	// Check if file exists
	LPSTR pFilename = new char[_MAX_PATH];
	strcpy(pFilename, (LPSTR)dwStringAddress);
	if(DoesFileExist(pFilename))
	{
		// Open File Data
		DWORD dwDataSize = 0;
		LPSTR pData = ReadFileData(pFilename, &dwDataSize);

		// Decrypt File Data
		CEncryptor Encryptor(0);
		Encryptor.SetUniqueKey(1);
		Encryptor.SetUniqueKeyName(keyName);
		Encryptor.EncryptFileData(pData, dwDataSize, bEncryptIfTrue);

		// temp or real
		if ( bDoNotUseTempFolder==false )
		{
			// get file ext
			char pExt[32];
			strcpy ( pExt, "" );
			for ( int n=strlen(pFilename)-1; n>0; n-- )
			{
				if ( pFilename[n]=='.' )
				{
					strcpy ( pExt, pFilename+n );
					break;
				}
			}

			// work out write file to local write-safe area
			strcpy ( pFilename, pLocalTRemp );
			strcat ( pFilename, "\\decrypted" );
			strcat ( pFilename, pExt );

			// if decrypting file, write decrypted file to safe-write area
			// if encrypting, simply delete the temp writted file in safe-write area
			if ( bEncryptIfTrue==false )
			{
				// Write New File
				WriteFileData(pFilename, pData, dwDataSize);

				// And point module using this function to the new filename
				strcpy ( (LPSTR)dwStringAddress, pFilename );
			}
		}
		else
		{
			// Write New File and overrite what is there
			WriteFileData(pFilename, pData, dwDataSize);
		}

		// Free FileData
		if(pData)
		{
			GlobalFree(pData);
			pData=NULL;
		}
	}

	// Free usages
	delete[] pFilename;

	//Dave - set key back to 0 after
	g_Glob.dwEncryptionUniqueKey = 0;
}

DARKSDK void Decrypt( DWORD dwStringAddress )
{
	// lee - 230306 - u6b4 - only encrypt/decrypt if from DBPDATA temp folder (should not touch local files!)
	// Dave - 28/03/2014 - commented this out because we will check if a file needs decrypting from now on
	//if ( strnicmp ( (LPSTR)dwStringAddress, g_Glob.pEXEUnpackDirectory, strlen(g_Glob.pEXEUnpackDirectory) )==NULL )
		EncryptDecrypt ( dwStringAddress, false, false );
}

DARKSDK void Encrypt( DWORD dwStringAddress )
{
	// lee - 230306 - u6b4 - only encrypt/decrypt if from DBPDATA temp folder (should not touch local files!)
	// Dave - 28/03/2014 - commented this out because we will check if a file needs decrypting from now on
	//if ( strnicmp ( (LPSTR)dwStringAddress, g_Glob.pEXEUnpackDirectory, strlen(g_Glob.pEXEUnpackDirectory) )==NULL )
		EncryptDecrypt ( dwStringAddress, true, false );
}

//Dave added 28/03/2014 so we can encrypt from dbpro
DARKSDK void EncryptDBPro ( DWORD dwStringAddress )
{

	LPSTR pFilename = new char[_MAX_PATH];
	strcpy(pFilename, (LPSTR)dwStringAddress);
	if(!DoesFileExist(pFilename))
		return;

	char newFileName[_MAX_PATH];
	sprintf ( newFileName , "_e_%s" , (LPSTR)dwStringAddress );

	char buf[BUFSIZ];
	size_t size;

	FILE* source = fopen( (LPSTR)dwStringAddress , "rb");
	FILE* dest = fopen(newFileName, "wb");

	// clean and more secure
	// feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	fclose(source);
	fclose(dest);

	EncryptDecrypt ( (DWORD)newFileName, true, true );
}

//Dave used for workshop encryption
DARKSDK void EncryptWorkshopDBPro ( DWORD dwStringAddress )
{

	LPSTR pFilename = new char[_MAX_PATH];
	strcpy(pFilename, (LPSTR)dwStringAddress);
	if(!DoesFileExist(pFilename))
		return;

	char originalPath[MAX_PATH];
	GetCurrentDirectory ( MAX_PATH, originalPath );

	char* pLocalFile = NULL;
	char filePath[MAX_PATH];
	strcpy( filePath, pFilename );
	pLocalFile = strrchr ( pFilename , '\\' );
	if ( pLocalFile )
	{	
		strcpy ( pFilename, pLocalFile+1 );
		pLocalFile = strrchr ( filePath , '\\' );
		pLocalFile[0] = '\0';
		SetCurrentDirectory ( filePath );
	}

	char newFileName[_MAX_PATH];
	sprintf ( newFileName , "_w_%s" , pFilename );

	char buf[BUFSIZ];
	size_t size;

	FILE* source = fopen( pFilename , "rb");
	FILE* dest = fopen(newFileName, "wb");

	// clean and more secure
	// feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	fclose(source);
	fclose(dest);

	EncryptDecrypt ( (DWORD)newFileName, true, true );

	SetCurrentDirectory(originalPath);
}

DARKSDK bool EncryptNewFile ( DWORD dwStringAddress )
{
	// do not encrypt any sky models (as they use internal image loads which are also encrypted 
	// and I cannot load encryped files from within the temp folder where the decrypted.x is)
	LPSTR pScanFilename = (LPSTR)dwStringAddress;
	char pThisDirAndFile[MAX_PATH];
	GetCurrentDirectory ( MAX_PATH, pThisDirAndFile );
	strcat ( pThisDirAndFile, "\\" );
	strcat ( pThisDirAndFile, pScanFilename );
	int iScanMax = strlen(pThisDirAndFile)-8;
	if ( iScanMax < 0 ) iScanMax = 0;
	if ( strlen ( pThisDirAndFile ) > 8 )
	{
		for ( int n=0; n<iScanMax; n++ )
		{
			if ( strnicmp ( pThisDirAndFile + n, "skybank\\", 8 )==NULL || strnicmp ( pThisDirAndFile + n, "skybank/", 8 )==NULL )
			{
				if ( strnicmp ( pThisDirAndFile + n, "skybank\\", 8 )==NULL || strnicmp ( pThisDirAndFile + n, "skybank/", 8 )==NULL )
				{
					if ( strnicmp ( pThisDirAndFile + strlen(pThisDirAndFile) - 2, ".x", 2 )==NULL )
					{
						return false;
					}
				}
			}
		}
	}

	char newFileName[_MAX_PATH];
	sprintf ( newFileName , "_e_%s" , (LPSTR)dwStringAddress );

	char buf[BUFSIZ];
	size_t size;

	FILE* source = fopen( (LPSTR)dwStringAddress , "rb");
	FILE* dest = fopen(newFileName, "wb");

	// clean and more secure
	// feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	fclose(source);
	fclose(dest);

	EncryptDecrypt ( (DWORD)newFileName, true, true );

	return true;
}

// Delete any empty folders
DARKSDK void EncryptAllFiles(DWORD dwStringAddress)
{

	LPSTR pFilename = new char[_MAX_PATH];
	strcpy(pFilename, (LPSTR)dwStringAddress);
	//if(!DoesFileExist(pFilename))
		//return;

	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA data  = { 0 };
    
	std::stack  < char* > directoryListStack;

	char folderToCheck[MAX_PATH];
	sprintf ( folderToCheck , pFilename );

	// add first directory into the listing
	directoryListStack.push ( folderToCheck );

	// keep going until we have emptied the directory stack
	while ( !directoryListStack.empty ( ) )
	{
		// get the first directory
		char  szLocation [ 256 ] = "";
		char* szCurrentDirectory = directoryListStack.top ( );
		
		// now add this to the location to check plus no mask so we search for everything
		sprintf ( szLocation, "%s\\*.*", szCurrentDirectory );

		// pop this directory off the stack
		directoryListStack.pop ( );

		// find the first file in the location
		hFind = FindFirstFile ( szLocation, &data );

		// break if nothing is there
		if ( hFind == INVALID_HANDLE_VALUE )
			break;

		// cycle through all files
		do 
		{
			// only proceed if it's not . or ..
			if ( strcmp ( data.cFileName, "." ) != 0 && strcmp ( data.cFileName, ".." ) != 0 )
			{
				// deal with a directory
				if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					// add this directory onto the stack
					char* p = new char [ 256 ];
					
					sprintf ( p, "%s\\%s", szCurrentDirectory, data.cFileName );
					
					directoryListStack.push ( p );
				}
				else
				{
					if ( strstr(data.cFileName, ".dds") != NULL ||  strstr(data.cFileName, ".png") != NULL ||  strstr(data.cFileName, ".jpg") != NULL || strstr(data.cFileName, ".x") != NULL || strstr(data.cFileName, ".dbo") != NULL ||  strstr(data.cFileName, ".wav") != NULL ||  strstr(data.cFileName, ".mp3") != NULL )
					{
						// dont encrypt a file if it already is
						if ( strstr ( data.cFileName, "_e_" )  !=  data.cFileName )
						{
							// encrypt the file
							char p[ MAX_PATH ];
							char f[ MAX_PATH ];
					
							sprintf ( p, "%s\\", szCurrentDirectory );
							strcpy ( f , data.cFileName );

							char originalFolder[MAX_PATH];
							GetCurrentDirectory ( MAX_PATH, originalFolder );						
							SetCurrentDirectory ( szCurrentDirectory );
							bool bEncryptedOkay = EncryptNewFile( (DWORD)f );
							SetCurrentDirectory ( originalFolder );
							UpdateWindow ( NULL );
							sprintf ( p, "%s\\%s", szCurrentDirectory , f );
							if ( bEncryptedOkay==true ) DeleteFile ( p );
						}
					}
				}
			}
		}
		while ( FindNextFile ( hFind, &data ) != 0 );

		// now break out if needed
		if ( GetLastError ( ) != ERROR_NO_MORE_FILES )
		{
			FindClose ( hFind );
			break;
		}

		FindClose ( hFind );
		hFind = INVALID_HANDLE_VALUE;
	}


}

// DISPLAY FUNCTIONS
DARKSDK void ConstructPostDisplayItems(HINSTANCE hInstance)
{
	// Construct internals of SupportDLLs (After Display Created)
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX && g_Glob.g_GFXmade==false)
	#endif
	{
		g_Glob.g_GFXmade=true;
		if(g_GFX_PassCoreData) g_GFX_PassCoreData( (LPVOID)&g_Glob, 1 );
	}
	
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Basic2D && g_Glob.g_Basic2Dmade==false)
	#endif
	{
		g_Glob.g_Basic2Dmade=true;
		g_Basic2D_Constructor( g_Glob.g_GFX );
		if(g_Basic2D_SetErrorHandler) g_Basic2D_SetErrorHandler(g_ErrorHandler);
		if(g_Basic2D_PassCoreData) g_Basic2D_PassCoreData( (LPVOID)&g_Glob );
	}
	
	/*
	if(g_Glob.g_Text && g_Glob.g_Textmade==false)
	{
		g_Glob.g_Textmade=true;
		g_Text_Constructor( g_Glob.g_GFX );
		if(g_Text_SetErrorHandler) g_Text_SetErrorHandler(g_ErrorHandler);
		if(g_Text_PassCoreData) g_Text_PassCoreData( (LPVOID)&g_Glob );
	}
	*/

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Text)
	#endif
	{
		if(g_Glob.g_Textmade==false)
		{
			g_Glob.g_Textmade=true;
			g_Text_Constructor( g_Glob.g_GFX );
			if(g_Text_SetErrorHandler) g_Text_SetErrorHandler(g_ErrorHandler);
			if(g_Text_PassCoreData) g_Text_PassCoreData( (LPVOID)&g_Glob );
		}
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Image && g_Glob.g_Imagemade==false)
	#endif
	{
		g_Glob.g_Imagemade=true;
		g_Image_Constructor ( g_Glob.g_GFX );
		if(g_Image_PassSpriteInstance) g_Image_PassSpriteInstance ( g_Glob.g_Sprites );
		if(g_Image_SetErrorHandler) g_Image_SetErrorHandler( g_ErrorHandler );
		if(g_Image_PassCoreData) g_Image_PassCoreData( (LPVOID)&g_Glob );
		g_Image_SetColorKey  ( 0, 0, 0 );
//		g_Image_SetMipmapNum ( 1 ); // leefix - 200303 - allow mipmaps
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Transforms && g_Glob.g_Transformsmade==false)
	#endif
	{
		g_Glob.g_Transformsmade=true;
		g_Transforms_Constructor();
		if(g_Transforms_SetErrorHandler) g_Transforms_SetErrorHandler( g_ErrorHandler );
		if(g_Transforms_PassCoreData) g_Transforms_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Sprites && g_Glob.g_Spritesmade==false)
	#endif
	{
		
		g_Glob.g_Spritesmade=true;
		g_Sprites_Constructor ( g_Glob.g_GFX, g_Glob.g_Image );
		if(g_Sprites_PassCoreData) g_Sprites_PassCoreData( (LPVOID)&g_Glob );
		if(g_Sprites_SetErrorHandler) g_Sprites_SetErrorHandler( g_ErrorHandler );
		
	}
}

DARKSDK void ConstructPostDLLItems(HINSTANCE hInstance)
{
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Input && g_Glob.g_Inputmade==false)
	#endif
	{
		g_Glob.g_Inputmade=true;
		g_Input_Constructor ( hInstance );
		if(g_Input_SetErrorHandler) g_Input_SetErrorHandler( g_ErrorHandler );
		if(g_Input_PassCoreData) g_Input_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_System && g_Glob.g_Systemmade==false)
	#endif
	{
		g_Glob.g_Systemmade=true;
		g_System_Constructor();
		if(g_System_SetErrorHandler) g_System_SetErrorHandler( g_ErrorHandler );
		if(g_System_PassCoreData) g_System_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Sound && g_Glob.g_Soundmade==false)
	#endif
	{
		g_Glob.g_Soundmade=true;
		g_Sound_Constructor ( g_Glob.hWnd );
		if(g_Sound_SetErrorHandler) g_Sound_SetErrorHandler( g_ErrorHandler );
		if(g_Sound_PassCoreData) g_Sound_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Music && g_Glob.g_Musicmade==false)
	#endif
	{
		g_Glob.g_Musicmade=true;
		g_Music_Constructor ( g_Glob.hWnd );
		if(g_Music_SetErrorHandler) g_Music_SetErrorHandler( g_ErrorHandler );
		if(g_Music_PassCoreData) g_Music_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_File && g_Glob.g_Filemade==false)
	{
		g_Glob.g_Filemade=true;
		g_File_Constructor ( hInstance );
		if(g_File_SetErrorHandler) g_File_SetErrorHandler( g_ErrorHandler );
		if(g_File_PassCoreData) g_File_PassCoreData( (LPVOID)&g_Glob );
	}
	#else
		g_Glob.g_Filemade=true;
		g_File_Constructor ( );
		if(g_File_SetErrorHandler) g_File_SetErrorHandler( g_ErrorHandler );
		if(g_File_PassCoreData) g_File_PassCoreData( (LPVOID)&g_Glob );
	#endif

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_FTP && g_Glob.g_FTPmade==false)
	#endif
	{
		g_Glob.g_FTPmade=true;
		g_FTP_Constructor ( hInstance );
		if(g_FTP_SetErrorHandler) g_FTP_SetErrorHandler( g_ErrorHandler );
		if(g_FTP_PassCoreData) g_FTP_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Memblocks && g_Glob.g_Memblocksmade==false)
	#endif
	{
		g_Glob.g_Memblocksmade=true;
		g_Memblocks_Constructor ( hInstance );
		if(g_Memblocks_SetErrorHandler) g_Memblocks_SetErrorHandler( g_ErrorHandler );
		if(g_Memblocks_PassCoreData) g_Memblocks_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Animation && g_Glob.g_Animationmade==false)
	#endif
	{
		g_Glob.g_Animationmade=true;
		g_Animation_Constructor ( hInstance );
		if(g_Animation_SetErrorHandler) g_Animation_SetErrorHandler( g_ErrorHandler );
		if(g_Animation_PassCoreData) g_Animation_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Bitmap && g_Glob.g_Bitmapmade==false)
	#endif
	{
		g_Glob.g_Bitmapmade=true;
		g_Bitmap_Constructor ( hInstance );
		if(g_Bitmap_SetErrorHandler) g_Bitmap_SetErrorHandler( g_ErrorHandler );
		if(g_Bitmap_PassCoreData) g_Bitmap_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Multiplayer && g_Glob.g_Multiplayermade==false)
	#endif
	{
		g_Glob.g_Multiplayermade=true;
		g_Multiplayer_Constructor();
		if(g_Multiplayer_SetErrorHandler) g_Multiplayer_SetErrorHandler( g_ErrorHandler );
		if(g_Multiplayer_PassCoreData) g_Multiplayer_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Camera3D && g_Glob.g_Camera3Dmade==false)
	#endif
	{
		g_Glob.g_Camera3Dmade=true;
		g_Camera3D_Constructor(g_Glob.g_GFX, g_Glob.g_Image);
		if(g_Camera3D_SetErrorHandler) g_Camera3D_SetErrorHandler( g_ErrorHandler );
		if(g_Camera3D_PassCoreData) g_Camera3D_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Light3D && g_Glob.g_Light3Dmade==false)
	#endif
	{
		g_Glob.g_Light3Dmade=true;
		g_Light3D_Constructor(g_Glob.g_GFX);
		if(g_Light3D_SetErrorHandler) g_Light3D_SetErrorHandler( g_ErrorHandler );
		if(g_Light3D_PassCoreData) g_Light3D_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Matrix3D && g_Glob.g_Matrix3Dmade==false)
	#endif
	{
		g_Glob.g_Matrix3Dmade=true;
		g_Matrix3D_Constructor(g_Glob.g_GFX, g_Glob.g_Image);
		if(g_Matrix3D_SetErrorHandler) g_Matrix3D_SetErrorHandler( g_ErrorHandler );
		if(g_Matrix3D_PassCoreData) g_Matrix3D_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Basic3D && g_Glob.g_Basic3Dmade==false)
#endif
	{
		g_Glob.g_Basic3Dmade=true;
		g_Basic3D_Constructor(g_Glob.g_GFX, g_Glob.g_Image, g_Glob.g_Vectors, g_Glob.g_Basic3D );
		if(g_Basic3D_SetErrorHandler) g_Basic3D_SetErrorHandler( g_ErrorHandler );
		if(g_Basic3D_PassCoreData) g_Basic3D_PassCoreData( (LPVOID)&g_Glob );
//		if(g_Basic3D_SendFormats) g_Basic3D_SendFormats ( g_Glob.g_XObject, g_Glob.g_3DSObject, g_Glob.g_MDLObject, g_Glob.g_MD2Object, g_Glob.g_MD3Object, g_Glob.g_PrimObject );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_World3D && g_Glob.g_World3Dmade==false)
	#endif
	{
		g_Glob.g_World3Dmade=true;
		g_World3D_Constructor(g_Glob.g_GFX, g_Glob.g_Image, g_Glob.g_Camera3D, g_Glob.g_Basic3D );
		if(g_World3D_SetErrorHandler) g_World3D_SetErrorHandler( g_ErrorHandler );
		if(g_World3D_PassCoreData) g_World3D_PassCoreData( (LPVOID)&g_Glob );
	}

	if(g_Glob.g_Q2BSP && g_Glob.g_Q2BSPmade==false)
	{
		g_Glob.g_Q2BSPmade=true;
// Handled by Q3BSPInternals
//		g_Q2BSP_Constructor(g_Glob.g_GFX, g_Glob.g_Image, g_Glob.g_Camera3D, g_Glob.g_Basic3D );
//		if(g_Q2BSP_SetErrorHandler) g_Q2BSP_SetErrorHandler( g_ErrorHandler );
//		if(g_Q2BSP_PassCoreData) g_Q2BSP_PassCoreData( (LPVOID)&g_Glob );
	}
	
	if(g_Glob.g_OwnBSP && g_Glob.g_OwnBSPmade==false)
	{
		g_Glob.g_OwnBSPmade=true;
// Handled by Q3BSPInternals
//		g_OwnBSP_Constructor(g_Glob.g_GFX, g_Glob.g_Image, g_Glob.g_Camera3D, g_Glob.g_Basic3D );
//		if(g_OwnBSP_SetErrorHandler) g_OwnBSP_SetErrorHandler( g_ErrorHandler );
//		if(g_OwnBSP_PassCoreData) g_OwnBSP_PassCoreData( (LPVOID)&g_Glob );
	}
	
	if(g_Glob.g_BSPCompiler && g_Glob.g_BSPCompilermade==false)
	{
		g_Glob.g_BSPCompilermade=true;
// Handled by Q3BSPInternals
//		g_BSPCompiler_Constructor(g_Glob.g_GFX );
//		if(g_BSPCompiler_SetErrorHandler) g_BSPCompiler_SetErrorHandler( g_ErrorHandler );
//		if(g_BSPCompiler_PassCoreData) g_BSPCompiler_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Vectors && g_Glob.g_Vectorsmade==false)
	#endif
	{
		g_Glob.g_Vectorsmade=true;
		if(g_Vectors_Constructor) g_Vectors_Constructor(g_Glob.g_GFX);
		if(g_Vectors_SetErrorHandler) g_Vectors_SetErrorHandler( g_ErrorHandler );
		if(g_Vectors_PassCoreData) g_Vectors_PassCoreData( (LPVOID)&g_Glob );
	}

	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Particles && g_Glob.g_Particlesmade==false)
	#endif
	{
		g_Glob.g_Particlesmade=true;
		g_Particles_Constructor(g_Glob.g_GFX, g_Glob.g_Image);
		if(g_Particles_SetErrorHandler) g_Particles_SetErrorHandler( g_ErrorHandler );
		if(g_Particles_PassCoreData) g_Particles_PassCoreData( (LPVOID)&g_Glob );
	}
	
	if(g_Glob.g_LODTerrain && g_Glob.g_LODTerrainmade==false)
	{
		g_Glob.g_LODTerrainmade=true;
		g_LODTerrain_Constructor(g_Glob.g_GFX, g_Glob.g_Image, g_Glob.g_Camera3D);
		if(g_LODTerrain_SetErrorHandler) g_LODTerrain_SetErrorHandler( g_ErrorHandler );
		if(g_LODTerrain_PassCoreData) g_LODTerrain_PassCoreData( (LPVOID)&g_Glob );
	}

	if(g_Glob.g_CSG && g_Glob.g_CSGmade==false)
	{
		g_Glob.g_CSGmade=true;
		g_CSG_Constructor(g_Glob.g_GFX);
		if(g_CSG_SetErrorHandler) g_CSG_SetErrorHandler( g_ErrorHandler );
		if(g_CSG_PassCoreData) g_CSG_PassCoreData( (LPVOID)&g_Glob );
	}
}

DARKSDK void FreeExternalDLLItems(void)
{
	#ifndef DARKSDK_COMPILE
		// Apply Destructors before we leave (to support DLLs)
		if(g_Glob.g_Vectors) g_Vectors_Destructor();
		if(g_Glob.g_LODTerrain) g_LODTerrain_Destructor();
		if(g_Glob.g_CSG) g_CSG_Destructor();

		// Apply Destructors before we leave (DLLs are unloaded later in EXE)
		if(g_Glob.g_Camera3D) g_Camera3D_Destructor();
		if(g_Glob.g_Light3D) g_Light3D_Destructor();
		if(g_Glob.g_Matrix3D) g_Matrix3D_Destructor();
		if(g_Glob.g_Basic3D) g_Basic3D_Destructor();
		if(g_Glob.g_World3D) g_World3D_Destructor();
	// Handled by Q3BSPInternals
	//	if(g_Glob.g_Q2BSP) g_Q2BSP_Destructor(); -Q3BSP internals
	//	if(g_Glob.g_OwnBSP) g_OwnBSP_Destructor(); -Q3BSP internals
	//	if(g_Glob.g_BSPCompiler) g_BSPCompiler_Destructor();
		if(g_Glob.g_Particles) g_Particles_Destructor();
		if(g_Glob.g_Multiplayer) g_Multiplayer_Destructor();
		if(g_Glob.g_Transforms) g_Transforms_Destructor();
		if(g_Glob.g_Bitmap) g_Bitmap_Destructor();
		if(g_Glob.g_Animation) g_Animation_Destructor();
		if(g_Glob.g_Memblocks) g_Memblocks_Destructor();
		if(g_Glob.g_FTP) g_FTP_Destructor();
		if(g_Glob.g_File) g_File_Destructor();
		if(g_Glob.g_System) g_System_Destructor();
		if(g_Glob.g_Input) g_Input_Destructor();
		if(g_Glob.g_Sound) g_Sound_Destructor();
		if(g_Glob.g_Music) g_Music_Destructor();
	#else
		g_Vectors_Destructor();
		g_Camera3D_Destructor();
		g_Light3D_Destructor();
		g_Matrix3D_Destructor();
		g_Basic3D_Destructor();
		g_World3D_Destructor();
		g_Particles_Destructor();
		g_Multiplayer_Destructor();
		g_Transforms_Destructor();
		g_Bitmap_Destructor();
		g_Animation_Destructor();
		g_Memblocks_Destructor();
		g_FTP_Destructor();
		g_File_Destructor();
		g_System_Destructor();
		g_Input_Destructor();
		g_Sound_Destructor();
		g_Music_Destructor();

	#endif
}

DARKSDK void FreeExternalDisplayDLLFriends(void)
{
	// Apply Destructors before we leave (DLLs are unloaded later in EXE)
	if(g_Glob.g_Sprites) g_Sprites_Destructor();
	if(g_Glob.g_Image) g_Image_Destructor();
	
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_Text)		
	#endif
		g_Text_Destructor();
	

	if(g_Glob.g_Basic2D) g_Basic2D_Destructor();
}

DARKSDK void FreeExternalDisplayDLL(void)
{
	// Free main DirectX DLL
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX)
	#endif
		g_GFX_Destructor();

	// Reset Flags
	g_bExternalDisplayActive=false;
	g_bUseExternalDisplayLayer=false;
}

DARKSDK void PassCmdLineHandlerPtr(LPVOID pCmdLinePtr)
{
	// Store pointer to command line string passed into EXE
	g_pCommandLineString = (LPSTR)pCmdLinePtr;
}

DARKSDK void PassErrorHandlerPtr(LPVOID pErrorPtr)
{
	// Store position of runtime error DWORD (held in executable dataspace)
	g_ErrorHandler = pErrorPtr;
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorPtr;

	// LEEMOD - 150803 - Also store reference in GLOBSTRUCT for ThirdPartyDLLs
	g_Glob.g_pErrorHandlerRef = pErrorPtr;
}

DARKSDK void PassEscapePtr(LPVOID pEscapePtr)
{
	// Store position ofescape value DWORD (held in executable dataspace)
	g_EscapeValue = pEscapePtr;
}

DARKSDK void PassBreakOutPtr(LPVOID pBreakOutPtr)
{
	// Store position of breakout position value DWORD (held in executable dataspace)
	g_BreakOutPosition = pBreakOutPtr;
}

DARKSDK void PassDataStatementPtr(LPSTR pDataStatements, LPSTR pDataStatementsEnd)
{
	g_pDataLabelStart = pDataStatements;
	g_pDataLabelEnd = pDataStatementsEnd;
	g_pDataLabelPtr = pDataStatements;
}

DARKSDK void PassStructurePatterns(LPVOID pPtrToPatternStrings, DWORD dwQty)
{
	// stores structure patterns
	g_dwStructPatternQty		= dwQty;
	g_pStructPatternsPtr		= (LPSTR)pPtrToPatternStrings;
}

DARKSDK void ChangeMouse( DWORD dwCursorID )
{
	// Set Cursor Shape (0-31)
	if ( dwCursorID==0 ) g_ActiveCursor=g_hUseArrow;
	if ( dwCursorID==1 ) g_ActiveCursor=g_hUseHourglass;
	if ( dwCursorID>=2 && dwCursorID<=31 ) g_ActiveCursor=g_hCustomCursors[dwCursorID-2];
	if ( dwCursorID==32 ) g_ActiveCursor=NULL;
	if ( dwCursorID<=31 )
	{
		// change cursor
		SetCursor ( g_ActiveCursor );
	}

}

DARKSDK DWORD InitDisplayEx(DWORD dwDisplayType, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, HINSTANCE hInstance, LPSTR pApplicationName, HWND pParentHWND, DWORD dwInExStyle, DWORD dwInStyle)
{
	// dwDisplayType
	// =============
	// 0=Hidden Mode
	// 1=Window Mode
	// 2=Desktop Fullscreen Mode
	// 3=Exclusive Fullscreen Mode
	// 4=Desktop Fullscreen Mode (No Taskbar)
	// 5=Use EX and STYLE from values passed in

	// System Settings
	g_dwScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	g_dwScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Window Default Settings
	bool bWindowIsDisplayable=true;
	g_Glob.dwWindowX = 0;
	g_Glob.dwWindowY = 0;

	// U75 - 260210 - to support Web Game Builder style ActiveX framing
	// if pass in window mode width and height of 1,2, place window offscreen
	bool bOverrideWindowCenteringToSupportActiveXFraming = false;
	if ( dwWidth==1 && dwHeight==2 )
	{
		bOverrideWindowCenteringToSupportActiveXFraming = true;
		g_Glob.dwWindowX = 5000;
		g_Glob.dwWindowY = 5000;
		dwWidth = 640;
		dwHeight = 480;
	}
	
	// Apply size of screen to global data
	g_Glob.dwWindowWidth = dwWidth;
	g_Glob.dwWindowHeight = dwHeight;
	g_Glob.iScreenWidth = dwWidth;
	g_Glob.iScreenHeight = dwHeight;
	g_Glob.iScreenDepth = dwDepth;

	if(g_Glob.g_GFX==NULL)
	{
		// Using GDI for Display
		if(dwDisplayType==2)
		{
			// Fullscreen Mode - With Taskbar
			RECT rc;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
			g_Glob.dwWindowWidth = rc.right-rc.left;
			g_Glob.dwWindowHeight = rc.bottom-rc.top;
		}
		if(dwDisplayType>=3)
		{
			// Fullscreen Mode - Simply Resize Window
			g_Glob.dwWindowWidth = g_dwScreenWidth;
			g_Glob.dwWindowHeight = g_dwScreenHeight;
		}
	}
	
	// Window Settings
	DWORD dwWindowStyle=0;
	DWORD dwWindowExStyle=0;
	switch(dwDisplayType)
	{
		// leechange - 101004 - should be FULL DESKTOP FULLSCREEN if made visible
		case 0 :	dwWindowStyle = WS_POPUP; // HIDDEN APP   // was WS_MINIMIZE;
					bWindowIsDisplayable=false;
					break;

		case 1 :	dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU; // WINDOW APP
					break;

		case 2 :	dwWindowStyle = WS_POPUP; // DESKTOP FULLSCREEN (see taskbar)
					break;

		case 3 :	dwWindowStyle = WS_POPUP; // EXCLUSIVE FULLSCREEN
					break;

		case 4 :	dwWindowStyle = WS_POPUP; // FULL DESKTOP FULLSCREEN (no taskbar)
					break;

		case 5 :	dwWindowStyle = dwInStyle;	// PASSED IN USING DARKGDKINIT
					dwWindowExStyle = dwInExStyle;
					break;

		case 6 :	dwWindowStyle = dwInStyle;	// PASSED IN USING DARKGDKINIT (HIDDEN)
					dwWindowExStyle = dwInExStyle;
					bWindowIsDisplayable=false;
					break;
	}

	// Icons and Cursors
	#ifndef DARKSDK_COMPILE
		g_hUseIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	#else
		g_hUseIcon = LoadIcon(hInstance,  MAKEINTRESOURCE(101));

		DWORD dwErr = GetLastError ( );
	#endif
	
	// Load Custom Cursors (first slot is ARROW, second is WAIT and third onwards is own)
	HCURSOR hCursor = NULL;
	hCursor = (HCURSOR)LoadImage(hInstance, "arrow.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	if (hCursor) g_hUseArrow=hCursor;
	hCursor = (HCURSOR)LoadImage(hInstance, "hourglass.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	if (hCursor) g_hUseHourglass=hCursor;
	for ( DWORD c=2; c<32; c++)
	{
		char str[_MAX_PATH];
		wsprintf(str, "pointer%d.cur", c);
		hCursor = (HCURSOR)LoadImage(hInstance, str, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
		g_hCustomCursors[c-2]=hCursor;
	}

	// Use Default Cursor otherwise
	if(g_hUseArrow==NULL) g_hUseArrow = LoadCursor(NULL, IDC_ARROW);
	if(g_hUseHourglass==NULL) g_hUseHourglass = LoadCursor(NULL, IDC_WAIT);

	// Vars
	WNDCLASS wc;

	// Appname
	char pAppName[256];
	char pAppNameUnique[256];
	if ( pApplicationName )
		strcpy( pAppName, pApplicationName );
	else
		strcpy( pAppName, "DB3 Application" );

	// leeadd - 280305 - this ensures no conflict between window class name and application class name
	strcpy ( pAppNameUnique, pAppName );
	strcat ( pAppNameUnique, "12345" );

	// Register window
	if ( g_Glob.hWnd==NULL )
	{
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = g_hUseIcon;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = pAppNameUnique;
		RegisterClass( &wc );
	}

	// Icon Set Manually (also in winproc too - for cursor restore control)
	g_ActiveCursor = g_hUseArrow;
	g_OldCursor = SetCursor ( g_ActiveCursor );

	// If running in window mode, start in center of screen
	if ( dwDisplayType==1 && bOverrideWindowCenteringToSupportActiveXFraming==false )
	{
		g_Glob.dwWindowX=(GetSystemMetrics(SM_CXSCREEN)-g_Glob.dwWindowWidth)/2;
		g_Glob.dwWindowY=(GetSystemMetrics(SM_CYSCREEN)-g_Glob.dwWindowHeight)/2;
	}

	// Create Window (if one not already created)
	g_Glob.hInstance = hInstance;
	if ( g_Glob.hWnd )
	{
		// override window handle with new winproc
		SetWindowLong ( g_Glob.hWnd, GWL_WNDPROC, (LONG)WindowProc );
	}
	else
	{
		g_Glob.hWnd = CreateWindow(	pAppNameUnique,
									pAppName,
									dwWindowStyle,
									g_Glob.dwWindowX,
									g_Glob.dwWindowY,
									g_Glob.dwWindowWidth,
									g_Glob.dwWindowHeight,
									NULL,
									NULL,
									hInstance,
									NULL);
	}

	//Fire up steam and call steamInit()

	// Steam Multiplayer DLL functions needed
	HMODULE SteamMultiplayerModule = NULL;

	typedef int		(*t_SteamInit)();

	t_SteamInit Steam_SteamInit = NULL;

	// Setup pointers to Steam functions
	SteamMultiplayerModule = LoadLibrary ( "SteamMultiplayer.dll" );

	if ( SteamMultiplayerModule )
	{
		Steam_SteamInit=(t_SteamInit)GetProcAddress( SteamMultiplayerModule , "?SteamInit@@YAHXZ" );

		if ( Steam_SteamInit )
			Steam_SteamInit();
	}

	// Load External DLL Displayer (DirectX/OpenGL/SmegSoft)
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX)
	#endif
	{
		// Construct internals of DisplayDLL First
		bool bDXFailed=false;
		if ( g_GFX_Constructor()==true )
		{
			if(g_GFX_PassCoreData) g_GFX_PassCoreData( (LPVOID)&g_Glob, 0 );
			if(g_GFX_SetErrorHandler) g_GFX_SetErrorHandler(g_ErrorHandler);

			#ifndef DARKSDK_COMPILE
			// If DXcheck failed, exit now
			if(g_GFX_GetDirect3D()==NULL) bDXFailed=true;
			#endif
		}
		else
			bDXFailed=true;

		// Release all if failed
		if ( bDXFailed==true )
		{
			// Failed DX9 check
			return 1;
		}

		// Initialise DisplayDLL
		g_bUseExternalDisplayLayer=true;
		g_GFX_OverrideHWND(g_Glob.hWnd);
	}
	#ifndef DARKSDK_COMPILE
	else
	{
		// not using external display (DX), must be GDI lightwight
		// leeadd - 080306 - u60 - still need to EMBED igLoader if there
		if ( g_pGlob->hwndIGLoader ) SendMessage ( g_pGlob->hwndIGLoader, WM_SETTEXT, 0, (LPARAM)"EMBEDME" );
	}
	#endif

	// Activate COM
	CoInitialize(NULL);

	// Create Display (dwAppDisplayModeUsing controls window handler)
	g_Glob.dwAppDisplayModeUsing=dwDisplayType;
	CreateDisplay(dwDisplayType);

	// Can fail to create starter resolution
	if(*(DWORD*)g_ErrorHandler>0) return 1;

	// Assign Function Ptrs to Glob (for other DLLs to use)
	g_Glob.CreateDeleteString = CreateSingleString;
	g_Glob.ProcessMessageFunction = ProcessMessagesOnly;
	g_Glob.PrintStringFunction = PrintString;
	g_Glob.UpdateFilenameFromVirtualTable = UpdateFilenameFromVirtualTable;
	g_Glob.Decrypt = Decrypt;
	g_Glob.Encrypt = Encrypt;
	g_Glob.ChangeMouseFunction = ChangeMouse;

	// Load External DLL Displayer (DirectX/OpenGL/SmegSoft)
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX)
	#endif
		ConstructPostDisplayItems(hInstance);

	// Prepare Other DLLs
	ConstructPostDLLItems(hInstance);

	// Visible Window
	if(bWindowIsDisplayable)
	{
		// igLoader window requires re-positioning after subclassing WinProc
		if ( g_pGlob->hwndIGLoader )
		{
			// Prepare window within browser, zero update until first SYNC command
			g_GFX_Clear( 255, 255, 255 ); // white initial screen (until figure out better way)
			SetWindowPos ( g_Glob.hWnd, HWND_TOP, 0, 0, 640, 480, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW );
		}
		else
		{
			// Clear Display Area
			ClearPrintArea();

			// Clear Screen
			InvalidateRect(g_Glob.hWnd, NULL, TRUE);
			UpdateWindow(g_Glob.hWnd);

			// Reveal Window
			ShowWindow(g_Glob.hWnd, SW_SHOW);
		}
	}

	// External DisplayDLL Fully Active
	#ifndef DARKSDK_COMPILE
	if(g_Glob.g_GFX)
	#endif
		g_bExternalDisplayActive=true;

	// Set Any After Display Properties (ink, font, etc)
	SetDefaultDisplayProperties();

	// Process any messages prior to program start (also for begin scene call)
	DWORD dwTimer=timeGetTime();
	while(timeGetTime()<dwTimer+100)
	{
		if(g_bUseExternalDisplayLayer) ExternalDisplaySync(0);

		InternalProcessMessages();
	}

	return 0;
}

DARKSDK DWORD InitDisplay(DWORD dwDisplayType, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, HINSTANCE hInstance, LPSTR pApplicationName)
{
	return InitDisplayEx(dwDisplayType, dwWidth, dwHeight, dwDepth, hInstance, pApplicationName, NULL, 0, 0);
}

DARKSDK void PassDLLs(void)
{
	// Make All External DLL Functions
	SetDBDLLExtCalls();
}

DARKSDK void ConstructDLLs(void)
{
	// Prepare Other DLLs
	ConstructPostDisplayItems(g_Glob.hInstance);
	ConstructPostDLLItems(g_Glob.hInstance);
}

DARKSDK int GetSecurityCode(void)
{	
	// gewnerate once
	srand((int)timeGetTime());
	if ( g_iSecurityCode!=-1 )
	{
		int iSecurityCode = rand()%1000000;
		if ( g_iSecurityCode==0 ) g_iSecurityCode = iSecurityCode;
	}
	return g_iSecurityCode;
}

DARKSDK void WipeSecurityCode(void)
{
	// clear forever
	g_iSecurityCode=-1;
}

DARKSDK DWORD GetGlobPtr(void)
{
	return (DWORD)&g_Glob;
}

DARKSDK void FreeChecklistStrings(void)
{
	// Free checklist strings
	for(DWORD c=0; c<g_Glob.dwChecklistArraySize; c++)
		if(g_Glob.checklist[c].string)
			SAFE_DELETE(g_Glob.checklist[c].string);

	// Free main block
	if(g_pGlob->checklist)
	{
		g_pGlob->CreateDeleteString((DWORD*)&g_pGlob->checklist, 0);
		g_pGlob->checklist=NULL;
	}
}

DARKSDK DWORD CloseDisplay(void)
{
	// Free checklist strings
	FreeChecklistStrings();

	// Close down MiscDLLs
	FreeExternalDLLItems();

	// Close down DisplayDLL Sublinks
	if(g_bUseExternalDisplayLayer) FreeExternalDisplayDLLFriends();

	// Restore Display to Windowed Mode
	DeleteDisplay();

	// Close down DisplayDLL Main
	if(g_bUseExternalDisplayLayer) FreeExternalDisplayDLL();

	// Free default input resources
	if(g_Glob.pWindowsTextEntry)
	{
		delete[] g_Glob.pWindowsTextEntry;
		g_Glob.pWindowsTextEntry=NULL;
	}

	// Free safe rects arrays
	SAFE_DELETE ( g_Glob.pSafeRects );

	// Close Window
	if(g_Glob.hWnd)
	{
		ShowWindow ( g_Glob.hWnd, SW_HIDE );
		CloseWindow(g_Glob.hWnd);
		g_Glob.hWnd=NULL;
	}

	// Free Cursors and Icons
	if(g_hUseIcon) DestroyIcon(g_hUseIcon);
	if(g_hUseArrow) DestroyCursor(g_hUseArrow);
	if(g_hUseHourglass) DestroyCursor(g_hUseHourglass);

	// Free COM
	CoUninitialize();

	// Complete
	return 0;
}

// ABSOLUTE BASIC COMMANDS (PRINT and INPUT)

DARKSDK void Cls(void)
{
	ClearPrintArea();
	SetPrintCursor(0,0);
}
DARKSDK void SetCursor(int iX, int iY)
{
	SetPrintCursor(iX,iY);
}
DARKSDK void PrintR(LONGLONG lValue)
{
	PrintInteger(lValue, true);
}
DARKSDK void PrintO(double dValue)
{
	PrintFloat(dValue, true);
}
DARKSDK void PrintS(LPSTR pString)
{
	PrintString(pString, true);
}
DARKSDK void Print0(void)
{
	PrintNothing();
}
DARKSDK void PrintCR(LONGLONG lValue)
{
	PrintInteger(lValue, false);
}
DARKSDK void PrintCO(double dValue)
{
	PrintFloat(dValue, false);
}
DARKSDK void PrintCS(LPSTR pString)
{
	PrintString(pString, false);
}

DARKSDK LONGLONG PerformanceTimer ( void )
{
	LARGE_INTEGER large;
	if (!QueryPerformanceCounter ( &large ))
	{
		large.QuadPart = 0;
	};
	return large.QuadPart;
}

DARKSDK LONGLONG PerformanceFrequency ( void )
{
	LARGE_INTEGER large;
	if (! QueryPerformanceFrequency( &large ))
	{
		large.QuadPart = 0;
	}
	return large.QuadPart;
}

DARKSDK LONGLONG InputR(void)
{
	LONGLONG lValue;
	InputInteger(&lValue);
	return lValue;
}
DARKSDK double InputO(void)
{
	double dValue;
	InputFloat(&dValue);
	return dValue;
}
DARKSDK DWORD InputS(DWORD pDestStr)
{
	if(pDestStr) delete (LPSTR)pDestStr;

	LPSTR pString=NULL;
	InputString(&pString);
	return (DWORD)pString;
}

// MEMORY MANAGEMENT FUNCTIONS

DARKSDK DWORD CreateVariableSpace(DWORD VariableSpaceSize)
{
	// Create Variable Space
	g_pVarSpace = (LPSTR)GlobalAlloc(GMEM_FIXED, VariableSpaceSize);
	g_Glob.g_pVariableSpace = (LPVOID)g_pVarSpace;
	return (DWORD)g_pVarSpace;
}

DARKSDK DWORD CreateDataSpace(DWORD DataSpaceSize)
{
	// Create Data Space
	g_pDataSpace = (LPSTR)GlobalAlloc(GMEM_FIXED, DataSpaceSize);
	return (DWORD)g_pDataSpace;
}

DARKSDK void DeleteVariableSpace(void)
{
	// Delete Variable Space Itself
	SAFE_FREE(g_pVarSpace);
}

DARKSDK void DeleteDataSpace(void)
{
	// Delete Data Space Itself
	SAFE_FREE(g_pDataSpace);
}

DARKSDK void DeleteSingleVariableAllocation(DWORD* dwVariableSpaceAddress)
{
	// Delete Actual Allocation within Variable Space (no need to clear)
	if(dwVariableSpaceAddress)
	{
		delete dwVariableSpaceAddress;
	}
}

DARKSDK DWORD CreateArray(DWORD dwSizeOfArray, DWORD dwSizeOfOneDataItem, DWORD dwTypeValueOfOneDataItem)
{
	// Calculate Total Size of Array
	DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
	DWORD dwDimSizeBytes = 40;
	DWORD dwRefSizeInBytes = dwSizeOfArray * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfArray * 1;
	DWORD dwDataSizeInBytes = dwSizeOfArray * dwSizeOfOneDataItem;

	// Total Size
	DWORD dwTotalSize = dwHeaderSizeInBytes + dwRefSizeInBytes + dwFlagSizeInBytes + dwDataSizeInBytes;

	// Error Trap for debug to discover larger chunk allocations mid-app activity (fragmentation danger)
	if ( dwTotalSize > 1024*1000*4 )
	{
		// can put breakpoint here when checking for large allocations mid-flow
		int iMB = dwTotalSize / 1024 / 1000;
		int stopwhen4megallocated = 42;
	}

	// Create Array Memory
	LPSTR pArrayPtr = new char[dwTotalSize];
	memset(pArrayPtr, 0, sizeof(pArrayPtr));

	// Derive Pointers into Array
	DWORD* pHeader	= (DWORD*)(pArrayPtr);
	DWORD* pRef		= (DWORD*)(pArrayPtr+dwHeaderSizeInBytes);
	LPSTR  pFlag	= (LPSTR )(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes);
	LPSTR  pData	= (LPSTR )(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes+dwFlagSizeInBytes);

	// Create Header
	for(DWORD d=0; d<=9; d++) pHeader[0]=0;
	pHeader[10]=dwSizeOfArray;
	pHeader[11]=dwSizeOfOneDataItem;
	pHeader[12]=dwTypeValueOfOneDataItem;
	pHeader[13]=0;

	// Create Ref Table
	LPSTR pDataPointer = pData;
	for(DWORD r=0; r<dwSizeOfArray; r++)
	{
		pRef[r] = (DWORD)pDataPointer;
		pDataPointer+=dwSizeOfOneDataItem;
	}

	// Create DataBlockFlag Table (all flags to 1)
	memset(pFlag, 1, dwSizeOfArray);

	// Clear DataBlock Memory
	DWORD dwTotalDataSize = dwSizeOfArray * dwSizeOfOneDataItem;
	memset(pData, 0, dwTotalDataSize);

	// Advance ArrayPtr to First Byte in RefTable
	pArrayPtr+=dwHeaderSizeInBytes;

	// Return ArrayPtr
	return (DWORD)pArrayPtr;
}

DARKSDK void FreeStringsFromArray(DWORD dwArrayPtr)
{
	// Get Array Information
	if ( dwArrayPtr )
	{
		DWORD dwTypeValueOfOneDataItem = *((DWORD*)dwArrayPtr-2);
		if ( dwTypeValueOfOneDataItem == 2 ) 
		{
			// only free strings if array holds string items
			DWORD dwSizeOfTable = *((DWORD*)dwArrayPtr-4);
			DWORD dwDataItemSize = *((DWORD*)dwArrayPtr-3);
			DWORD dwRefSizeInBytes = dwSizeOfTable * 4;
			DWORD dwFlagSizeInBytes = dwSizeOfTable * 1;
			LPSTR* pData = (LPSTR*)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes+dwFlagSizeInBytes);
			for ( DWORD dwDataOffset=0; dwDataOffset<dwSizeOfTable; dwDataOffset++)
			{
				if ( pData [ dwDataOffset ] )
				{
					delete[] pData [ dwDataOffset ];
// Unnecessary clearance - this function only called from Undim.
//					pData [ dwDataOffset ] = NULL;
				}
			}
		}
		// Clear strings from UDT's
		else if (dwTypeValueOfOneDataItem >= 9)
		{
			// Grab a copy of the arrays format string
			LPSTR UdtFormat = GetTypePatternCore( NULL, dwTypeValueOfOneDataItem );

			// Search the format string to see if the UDT contains any strings
			bool ContainsString = false;
			for ( LPSTR CurrentItem = UdtFormat; *CurrentItem; ++CurrentItem )
			{
				if (*CurrentItem == 'S')
				{
					ContainsString = true;
					break;
				}
			}

			// If it does, loop through every UDT and release those strings
			if (ContainsString)
			{
				DWORD* ArrayPtr = (DWORD*)dwArrayPtr;
				DWORD  ArraySize = ArrayPtr[-4];

				for ( DWORD Position = 0; Position < ArraySize; ++Position )
				{
					DWORD ItemOffset = 0;
					for ( LPSTR CurrentItem = UdtFormat; *CurrentItem; ++CurrentItem )
					{
						if (*CurrentItem == 'S')
						{
							DWORD P = ArrayPtr[ Position ] + ItemOffset;
							delete[] *(LPSTR*)P;
							ItemOffset += 4;            // Strings are 4 bytes
						}
						else if (*CurrentItem == 'O' || *CurrentItem == 'R')
						{
							ItemOffset += 8;            // Double float/integer are 8 bytes
						}
						else
						{
							ItemOffset += 4;            // Everything else is 4 bytes
						}
					}
				}
			}

			// Release the copy of the arrays format string
			delete[] UdtFormat;
		}
	}
}

DARKSDK void DeleteArray(DWORD dwArrayPtr)
{
	// If Array exists
	if(dwArrayPtr)
	{
		// Array Ptr Skips Header
		dwArrayPtr-=HEADERSIZEINBYTES;

		// Delete Array Memory
		delete[] (DWORD*)dwArrayPtr;
	}
}

DARKSDK DWORD ExpandArray(DWORD dwOldArrayPtr, DWORD dwAddElements)
{
	// Get Old ArrayPtr
	LPSTR pOldArrayPtr = ((LPSTR)dwOldArrayPtr)-HEADERSIZEINBYTES;

	// Old Array Pointers and Data
	DWORD* pHeader	= (DWORD*)(pOldArrayPtr);
	DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;

	// Extract header info
	DWORD dwOldSizeOfArray = pHeader[10];
	DWORD dwOldSizeOfOneDataItem = pHeader[11];
	DWORD dwOldTypeValueOfOneDataItem = pHeader[12];
	DWORD dwOldInternalIndex = pHeader[13];

	DWORD dwOldRefSizeInBytes = dwOldSizeOfArray * 4;
	DWORD dwOldFlagSizeInBytes = dwOldSizeOfArray * 1;
	DWORD dwOldDataSizeInBytes = dwOldSizeOfArray * dwOldSizeOfOneDataItem;
	DWORD* pOldRef = (DWORD*)(pOldArrayPtr+dwHeaderSizeInBytes);
	LPSTR pOldFlag = (LPSTR)(pOldArrayPtr+dwHeaderSizeInBytes+dwOldRefSizeInBytes);
	LPSTR pOldData = (LPSTR)(pOldArrayPtr+dwHeaderSizeInBytes+dwOldRefSizeInBytes+dwOldFlagSizeInBytes);

	// Create New Size of Array
	DWORD dwSizeOfArray = dwOldSizeOfArray + dwAddElements;
	LPSTR pArrayPtr = (LPSTR)CreateArray(dwSizeOfArray, dwOldSizeOfOneDataItem, dwOldTypeValueOfOneDataItem);

	// Return ptr to beginning of memory
	pArrayPtr = pArrayPtr - HEADERSIZEINBYTES;

	// Copy dimension-size block over (10xDWORD values)
	memcpy(pArrayPtr, pOldArrayPtr, 40);

	// Calculate Sizes of New Array
	DWORD dwRefSizeInBytes = dwSizeOfArray * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfArray * 1;
	DWORD dwDataSizeInBytes = dwSizeOfArray * dwOldSizeOfOneDataItem;

	// Derive Pointers into New Array
	DWORD* pNewRef		= (DWORD*)(pArrayPtr+dwHeaderSizeInBytes);
	LPSTR  pNewFlag		= (LPSTR )(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes);
	LPSTR  pNewData		= (LPSTR )(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes+dwFlagSizeInBytes);

	// Clear new data and copy old data to it
	memset(pNewData, 0, dwDataSizeInBytes);
	memcpy(pNewData, pOldData, dwOldDataSizeInBytes);

	// Update New Array Refs from Old Array Refs
	for(DWORD i=0; i<dwOldSizeOfArray; i++)
	{
		DWORD dwOffset = (DWORD)(pOldRef[i]) - (DWORD)pOldData;
		pNewRef[i] = (DWORD)(pNewData + dwOffset);
	}

	// Copy flag states from old to new
	memcpy(pNewFlag, pOldFlag, dwOldFlagSizeInBytes);

	// Create flags for new part of array
	memset(pNewFlag+dwOldFlagSizeInBytes, 1, dwFlagSizeInBytes-dwOldFlagSizeInBytes);

	// Destroy old array
	DeleteArray(dwOldArrayPtr);

	// Advance ArrayPtr to First Byte in RefTable
	pArrayPtr+=dwHeaderSizeInBytes;

	// Return ArrayPtr
	return (DWORD)pArrayPtr;
}

DARKSDK void ClearDataBlock(DWORD dwArrayPtr, DWORD dwIndex, DWORD dwQuantity)
{
	DWORD dwSizeOfTable = *((DWORD*)dwArrayPtr-4);
	DWORD dwDataItemSize = *((DWORD*)dwArrayPtr-3);
	DWORD dwRefSizeInBytes = dwSizeOfTable * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfTable * 1;
	LPSTR pData = (LPSTR)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes+dwFlagSizeInBytes);
	DWORD dwDataOffset = dwIndex * dwDataItemSize;
	memset(pData+dwDataOffset, 0, dwQuantity * dwDataItemSize);
}

// ARRAY COMMANDS

DARKSDK DWORD DimCore(DWORD dwOldArrayPtr, DWORD dwTypeAndSizeOfElement, DWORD dwD1, DWORD dwD2, DWORD dwD3, DWORD dwD4, DWORD dwD5, DWORD dwD6, DWORD dwD7, DWORD dwD8, DWORD dwD9)
{
	if ( dwD1 > 1024 )
	{
		int leehmm=42;
	}

	// Increment all DBPro dimensions (+1 based)
	dwD1+=1;
	if(dwD2>0) dwD2+=1;
	if(dwD3>0) dwD3+=1;
	if(dwD4>0) dwD4+=1;
	if(dwD5>0) dwD5+=1;
	if(dwD6>0) dwD6+=1;
	if(dwD7>0) dwD7+=1;
	if(dwD8>0) dwD8+=1;
	if(dwD9>0) dwD9+=1;

	// Work out array size (can be no bigger than DWORD)
	__int64 iiSize = dwD1;
	if(dwD2>0) iiSize *= dwD2;
	if(dwD3>0) iiSize *= dwD3;
	if(dwD4>0) iiSize *= dwD4;
	if(dwD5>0) iiSize *= dwD5;
	if(dwD6>0) iiSize *= dwD6;
	if(dwD7>0) iiSize *= dwD7;
	if(dwD8>0) iiSize *= dwD8;
	if(dwD9>0) iiSize *= dwD9;
	DWORD dwSizeOfArray = (DWORD)iiSize;
	if(dwSizeOfArray!=iiSize)
		return NULL;

	// Leeadd - 211008 - u71 - new idea for dwTypeAndSizeOfElement
	// where the first 0-4095 specify a type index (>9 = user types)
	// and then a multiple of 4096 controls the size of the datatype
	// Type Value and Size as one DWORD value
	//DWORD dwSizeOfOneDataItem = dwTypeAndSizeOfElement;
	//dwSizeOfOneDataItem = dwSizeOfOneDataItem/10;
	//DWORD dwTypeValueOfOneDataItem = dwTypeAndSizeOfElement-(dwSizeOfOneDataItem*10);
	DWORD dwSizeOfOneDataItem = dwTypeAndSizeOfElement;
	dwSizeOfOneDataItem = dwSizeOfOneDataItem/4096;
	DWORD dwTypeValueOfOneDataItem = dwTypeAndSizeOfElement-(dwSizeOfOneDataItem*4096);

	// Create New Array
	DWORD dwArrayPtr =  CreateArray(dwSizeOfArray, dwSizeOfOneDataItem, dwTypeValueOfOneDataItem);

	// Fill array with dimension size data (D1-D9)
	DWORD* pHeader = (DWORD*)(((LPSTR)dwArrayPtr)-HEADERSIZEINBYTES);
	DWORD dwDimOverallSize=dwD1;
	for(DWORD h=0; h<=8; h++)
	{
		pHeader[h]=dwDimOverallSize;
		if(h==0) dwDimOverallSize=dwDimOverallSize*dwD2;
		if(h==1) dwDimOverallSize=dwDimOverallSize*dwD3;
		if(h==2) dwDimOverallSize=dwDimOverallSize*dwD4;
		if(h==3) dwDimOverallSize=dwDimOverallSize*dwD5;
		if(h==4) dwDimOverallSize=dwDimOverallSize*dwD6;
		if(h==5) dwDimOverallSize=dwDimOverallSize*dwD7;
		if(h==6) dwDimOverallSize=dwDimOverallSize*dwD8;
		if(h==7) dwDimOverallSize=dwDimOverallSize*dwD9;
	}

	// Return ArrayPtr
	return dwArrayPtr;
}

// ADDED DUE TO POPULAR DEMAND ON 040304

DARKSDK DWORD ReDimCore(DWORD dwOldArrayPtr, DWORD dwNewTypeAndSizeOfElement, DWORD dwOD1, DWORD dwOD2, DWORD dwOD3, DWORD dwOD4, DWORD dwOD5, DWORD dwOD6, DWORD dwOD7, DWORD dwOD8, DWORD dwOD9)
{
	// Increment all DBPro dimensions (+1 based) (as done is DimCore)
	DWORD dwD1=dwOD1, dwD2=dwOD2, dwD3=dwOD3, dwD4=dwOD4, dwD5=dwOD5, dwD6=dwOD6, dwD7=dwOD7, dwD8=dwOD8, dwD9=dwOD9;
	dwD1+=1;
	if(dwD2>0) dwD2+=1;
	if(dwD3>0) dwD3+=1;
	if(dwD4>0) dwD4+=1;
	if(dwD5>0) dwD5+=1;
	if(dwD6>0) dwD6+=1;
	if(dwD7>0) dwD7+=1;
	if(dwD8>0) dwD8+=1;
	if(dwD9>0) dwD9+=1;

	// Old Header Info
	DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
	DWORD* pOldHeader = (DWORD*)(((LPSTR)dwOldArrayPtr)-HEADERSIZEINBYTES);
	DWORD dwSizeOfOneDataItem = pOldHeader[11];

	// lee - 130206 - can detect if LOCAL DIM ARRAY attempte a REDIM with corrupt data
	// prevent bug by ignoring a REDIM and starting with a brand new Array
	if ( dwSizeOfOneDataItem > 1024000 ) // a data item over 1MB is a little extreme
		return NULL;

	// continue as the REDIM appears to be valid..
	DWORD dwTypeValueOfOneDataItem = pOldHeader[12];

	// Work out size and type of new array early
	// Leeadd - 211008 - u71 - new idea for dwTypeAndSizeOfElement
	//DWORD dwNewSizeOfOneDataItem = dwNewTypeAndSizeOfElement;
	//dwNewSizeOfOneDataItem = dwNewSizeOfOneDataItem/10;
	//DWORD dwNewTypeValueOfOneDataItem = dwNewTypeAndSizeOfElement-(dwNewSizeOfOneDataItem*10);
	DWORD dwNewSizeOfOneDataItem = dwNewTypeAndSizeOfElement;
	dwNewSizeOfOneDataItem = dwNewSizeOfOneDataItem/4096;
	DWORD dwNewTypeValueOfOneDataItem = dwNewTypeAndSizeOfElement-(dwNewSizeOfOneDataItem*4096);

	// Leave if core datachunk size (type) different
	if ( dwSizeOfOneDataItem!=dwNewSizeOfOneDataItem
	||   dwTypeValueOfOneDataItem!=dwNewTypeValueOfOneDataItem )
		return dwOldArrayPtr;

	// Create a New Array of new size
	DWORD dwNewArrayPtr = DimCore ( dwOldArrayPtr, dwNewTypeAndSizeOfElement, dwOD1, dwOD2, dwOD3, dwOD4, dwOD5, dwOD6, dwOD7, dwOD8, dwOD9 );
	DWORD* pNewHeader = (DWORD*)(((LPSTR)dwNewArrayPtr)-HEADERSIZEINBYTES);

	// Old Array Offsets
	DWORD dwOld[9];	for ( int i=0; i<9; i++ ) dwOld[i] = pOldHeader[i];

	// New Array Offsets
	// mike - 020206 - addition for vs8
	//DWORD dwNew[9];	for ( i=0; i<9; i++ ) dwNew[i] = pNewHeader[i];
	DWORD dwNew[9];	for ( int i=0; i<9; i++ ) dwNew[i] = pNewHeader[i];

	// Find old and new ptrs to reference tables of the arrays
	DWORD* pOldRef = (DWORD*)dwOldArrayPtr;
	DWORD* pNewRef = (DWORD*)dwNewArrayPtr;

	// Work out old dim values from data chunk sizes
	DWORD dwOldDims [ 9 ];
	for(DWORD h=0; h<=8; h++)
	{
		DWORD dwDataChunkSize;
		if(h==0) dwDataChunkSize=1;
		if(h==1) dwDataChunkSize=dwOld[0];
		if(h==2) dwDataChunkSize=dwOld[0]*dwOld[1];
		if(h==3) dwDataChunkSize=dwOld[0]*dwOld[1];
		if(h==4) dwDataChunkSize=dwOld[0]*dwOld[1]*dwOld[2];
		if(h==5) dwDataChunkSize=dwOld[0]*dwOld[1]*dwOld[2]*dwOld[3];
		if(h==6) dwDataChunkSize=dwOld[0]*dwOld[1]*dwOld[2]*dwOld[3]*dwOld[4];
		if(h==7) dwDataChunkSize=dwOld[0]*dwOld[1]*dwOld[2]*dwOld[3]*dwOld[4]*dwOld[5];
		if(h==8) dwDataChunkSize=dwOld[0]*dwOld[1]*dwOld[2]*dwOld[3]*dwOld[4]*dwOld[5]*dwOld[6];
		DWORD dwActualDimValue=0;
		if ( dwDataChunkSize>0 ) dwActualDimValue=dwOld[h]/dwDataChunkSize;
		dwOldDims[h]=dwActualDimValue;
	}

	// Trim if new array is smaller than old array (odd redim but possible)
	if ( dwOldDims[0] > dwD1 ) dwOldDims[0]=dwD1;
	if ( dwOldDims[1] > dwD2 ) dwOldDims[1]=dwD2;
	if ( dwOldDims[2] > dwD3 ) dwOldDims[2]=dwD3;
	if ( dwOldDims[3] > dwD4 ) dwOldDims[3]=dwD4;
	if ( dwOldDims[4] > dwD5 ) dwOldDims[4]=dwD5;
	if ( dwOldDims[5] > dwD6 ) dwOldDims[5]=dwD6;
	if ( dwOldDims[6] > dwD7 ) dwOldDims[6]=dwD7;
	if ( dwOldDims[7] > dwD8 ) dwOldDims[7]=dwD8;
	if ( dwOldDims[8] > dwD9 ) dwOldDims[8]=dwD9;

	// make sure can get through all fornext conditions at least once (to get to code)
	for(int h=0; h<=8; h++)
	{
		DWORD dwActualDimValue=dwOldDims[h];
		if ( dwActualDimValue==0 ) dwActualDimValue=1;
		dwOldDims[h]=dwActualDimValue;
	}

	// u55 - 080704 - do not copy if old or new array is EMPTY
	if ( pOldHeader[10]>0 && pNewHeader[10]>0 )
	{
		// Copy Old array data to new array
		for ( DWORD dwI1=0; dwI1<dwOldDims[0]; dwI1++ )
		for ( DWORD dwI2=0; dwI2<dwOldDims[1]; dwI2++ )
		for ( DWORD dwI3=0; dwI3<dwOldDims[2]; dwI3++ )
		for ( DWORD dwI4=0; dwI4<dwOldDims[3]; dwI4++ )
		for ( DWORD dwI5=0; dwI5<dwOldDims[4]; dwI5++ )
		for ( DWORD dwI6=0; dwI6<dwOldDims[5]; dwI6++ )
		for ( DWORD dwI7=0; dwI7<dwOldDims[6]; dwI7++ )
		for ( DWORD dwI8=0; dwI8<dwOldDims[7]; dwI8++ )
		for ( DWORD dwI9=0; dwI9<dwOldDims[8]; dwI9++ )
		{
			// copy old block of data to new array 
			DWORD dwOldIndex = (dwI1)+(dwI2*dwOld[0])+(dwI3*dwOld[1])+(dwI4*dwOld[2])+(dwI5*dwOld[3])+(dwI6*dwOld[4])+(dwI7*dwOld[5])+(dwI8*dwOld[6])+(dwI9*dwOld[7]);
			DWORD dwNewIndex = (dwI1)+(dwI2*dwNew[0])+(dwI3*dwNew[1])+(dwI4*dwNew[2])+(dwI5*dwNew[3])+(dwI6*dwNew[4])+(dwI7*dwNew[5])+(dwI8*dwNew[6])+(dwI9*dwNew[7]);
			LPSTR pOldPtr = (LPSTR)pOldRef[dwOldIndex];
			LPSTR pNewPtr = (LPSTR)pNewRef[dwNewIndex];
			memcpy ( pNewPtr, pOldPtr, dwSizeOfOneDataItem );
		}
	}

	// Free Old Array (if any)
	DeleteArray ( dwOldArrayPtr );

	// return new sized arrau
	return dwNewArrayPtr;
}

DARKSDK DWORD DimDDD(DWORD dwOldArrayPtr, DWORD dwTypeAndSizeOfElement, DWORD dwD1, DWORD dwD2, DWORD dwD3, DWORD dwD4, DWORD dwD5, DWORD dwD6, DWORD dwD7, DWORD dwD8, DWORD dwD9)
{
	try
	{
		// leechange - 050304 - now REDIMs if array already exists
		if ( dwOldArrayPtr )
		{
			// Change Size Of Array (and retain contents)
			DWORD dwNewArrPtr = ReDimCore ( dwOldArrayPtr, dwTypeAndSizeOfElement, dwD1, dwD2, dwD3, dwD4, dwD5, dwD6, dwD7, dwD8, dwD9 );

			// If corruption detected, can resort to a new array as follows..
			if ( dwNewArrPtr!=NULL ) return dwNewArrPtr;
		}

		// Create a New Array
		return DimCore ( dwOldArrayPtr, dwTypeAndSizeOfElement, dwD1, dwD2, dwD3, dwD4, dwD5, dwD6, dwD7, dwD8, dwD9 );
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwOldArrayPtr;
	}
}

DARKSDK DWORD UnDimDD(DWORD dwAllocation)
{
	// leefix - 070308 - U6.7 - will free strings if string array (fixes string leak)
	FreeStringsFromArray(dwAllocation);

	DeleteArray(dwAllocation);
	return NULL;
}

// ADVANCED UNIFIED ARRAY HANLDING

DARKSDK void ArrayIndexToBottom(DWORD dwArrayPtr)
{
	// set index to last item in array
	if(dwArrayPtr) *((DWORD*)dwArrayPtr-1) = *((DWORD*)dwArrayPtr-4)-1;
}
DARKSDK void ArrayIndexToTop(DWORD dwArrayPtr)
{
	// set index to first item in array
	if(dwArrayPtr) *((DWORD*)dwArrayPtr-1) = 0;
}
DARKSDK void NextArrayIndex(DWORD dwArrayPtr)
{
	// inc array index
	if(dwArrayPtr)
	{
		*((DWORD*)dwArrayPtr-1) = *((DWORD*)dwArrayPtr-1) + 1;
		if(*((DWORD*)dwArrayPtr-1) > *((DWORD*)dwArrayPtr-4))
		{
			// Last index reachable just just outside range >N
			*((DWORD*)dwArrayPtr-1) = *((DWORD*)dwArrayPtr-4);
		}
	}
}
DARKSDK void PreviousArrayIndex(DWORD dwArrayPtr)
{
	// dec array index
	if(dwArrayPtr)
	{
		if(*((DWORD*)dwArrayPtr-1)>0)
		{
			*((DWORD*)dwArrayPtr-1)=(*((DWORD*)dwArrayPtr-1))-1;
		}
		else
		{
			// First index reachable just just outside range <0
			*((DWORD*)dwArrayPtr-1)=(DWORD)-1;
		}
	}
}
DARKSDK DWORD ArrayIndexValid(DWORD dwArrayPtr)
{
	// check if index is valid (pointing to valid item)
	if(dwArrayPtr)
	{
		if(*((DWORD*)dwArrayPtr-1)<*((DWORD*)dwArrayPtr-4))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
DARKSDK DWORD ArrayCount(DWORD dwArrayPtr)
{
	// return array size
	if(dwArrayPtr) 
		return (*((DWORD*)dwArrayPtr-4))-1;
	else
		return -1;
}
DARKSDK DWORD ArrayInsertAtBottom(DWORD dwArrayPtr)
{
	try
	{
		// If no array, leave now
		if(dwArrayPtr==NULL) return dwArrayPtr;

		// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
		if ( IsArraySingleDim ( dwArrayPtr )==false )
		{
			RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM);
			return dwArrayPtr;
		}

		// Adjust Size Of Entire Array
		DWORD dwAllocation = ExpandArray(dwArrayPtr, 1);

		// Determine index
		int iIndex = (*((DWORD*)dwAllocation-4)) - 1;
		if(iIndex<0) iIndex=0;

		// Update array index to last in list
		*((DWORD*)dwAllocation-1) = iIndex;

		// Overwrites current array ptr
		return dwAllocation;
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwArrayPtr;
	}
}
DARKSDK DWORD ArrayInsertAtBottom(DWORD dwArrayPtr, int iQuantity)
{
	try
	{
		// If no array, leave now
		if(dwArrayPtr==NULL) return dwArrayPtr;

		// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
		if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

		// Autohandler
		if(iQuantity<1) iQuantity=1;

		// Adjust Size Of Entire Array
		DWORD dwAllocation = ExpandArray(dwArrayPtr, iQuantity);

		// Determine index
		int iIndex = (*((DWORD*)dwAllocation-4)) - iQuantity;
		if(iIndex<0) iIndex=0;

		// Update array index to fisrt new item at end of list
		*((DWORD*)dwAllocation-1) = iIndex;

		// Overwrites current array ptr
		return dwAllocation;
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwArrayPtr;
	}
}

DARKSDK DWORD ArrayInsertAtTop(DWORD dwArrayPtr)
{
	try
	{
		// If no array, leave now
		if(dwArrayPtr==NULL) return dwArrayPtr;

		// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
		if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

		// Adjust Size Of Entire Array
		DWORD dwAllocation = ExpandArray(dwArrayPtr, 1);

		// Store Ref located at end of list
		DWORD dwSizeOfTable = *((DWORD*)dwAllocation-4);
		DWORD dwIndex = dwSizeOfTable - 1;
		DWORD* pRef = (DWORD*)dwAllocation;
		DWORD dwRefItem = pRef[dwIndex];

		// Shuffle ref table to make space at top
		if(dwSizeOfTable>0) memcpy(pRef+1, pRef, (dwSizeOfTable-1)*4);

		// Copy refitem to top position
		pRef[0] = dwRefItem;

		// Update array index to new item
		*((DWORD*)dwAllocation-1) = 0;

		// Overwrites current array ptr
		return dwAllocation;
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwArrayPtr;
	}
}

DARKSDK DWORD ArrayInsertAtTop(DWORD dwArrayPtr, int iQuantity)
{
	try
	{
		// If no array, leave now
		if(dwArrayPtr==NULL) return dwArrayPtr;

		// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
		if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

		// Autohandler
		if(iQuantity<1) iQuantity=1;

		// Adjust Size Of Entire Array
		DWORD dwAllocation = ExpandArray(dwArrayPtr, iQuantity);

		// Store RefItems(iQuantity) located at end of list
		DWORD* pStoreRefs = (DWORD*)new DWORD[iQuantity];
		DWORD dwSizeOfTable = *((DWORD*)dwAllocation-4);
		DWORD dwIndexOfFirstRef = dwSizeOfTable-iQuantity;
		DWORD* pRef = (DWORD*)dwAllocation;
		memcpy(pStoreRefs, (DWORD*)pRef + dwIndexOfFirstRef, iQuantity*4);

		// Shuffle ref table to make space at top
		DWORD dwAmountToShuffle = 0;
		if(dwSizeOfTable>(DWORD)iQuantity) dwAmountToShuffle=(dwSizeOfTable-iQuantity)*4;
		if(dwAmountToShuffle>0) memcpy(pRef+iQuantity, pRef, dwAmountToShuffle);

		// Copy refitem to top position
		memcpy(pRef, pStoreRefs, iQuantity*4);

		// Update array index to new item
		*((DWORD*)dwAllocation-1) = 0;

		// Overwrites current array ptr
		return dwAllocation;
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwArrayPtr;
	}
}
DARKSDK DWORD ArrayInsertAtElement(DWORD dwArrayPtr, int iIndex)
{
	try
	{
		// If no array, leave now
		if(dwArrayPtr==NULL) return dwArrayPtr;

		// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
		if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

		DWORD dwSizeOfTable = *((DWORD*)dwArrayPtr-4);
		if(iIndex<0 || iIndex>=(int)dwSizeOfTable)
		{
			RunTimeError(RUNTIMEERROR_ARRAYINDEXINVALID);
			return dwArrayPtr;
		}
		
		// Size of insert
		int iQuantity=1;

		// Adjust Size Of Entire Array
		DWORD dwAllocation = ExpandArray(dwArrayPtr, iQuantity);

		// Store RefItems(iQuantity) located at end of list
		DWORD* pStoreRefs = (DWORD*)new DWORD[iQuantity];
		DWORD dwIndexOfFirstRef = dwSizeOfTable-(iQuantity-1);  //leefix-230603-corrected ptr to new item-ref in expanded array
		DWORD* pRef = (DWORD*)dwAllocation;
		memcpy(pStoreRefs, (DWORD*)pRef + dwIndexOfFirstRef, iQuantity*4);

		// Shuffle iIndex to End onwards
		DWORD dwSizeOfLaterChunk = 0;
		if(dwSizeOfTable>(DWORD)iIndex) dwSizeOfLaterChunk = dwSizeOfTable-iIndex;
		if(dwSizeOfLaterChunk>0) memcpy(pRef+iIndex+iQuantity, pRef+iIndex, dwSizeOfLaterChunk*4);

		// Copy RefItems into space created inside table
		memcpy(pRef+iIndex, pStoreRefs, iQuantity*4);
		delete[] pStoreRefs;    // Remove memory leak

		// Update array index to new item
		*((DWORD*)dwAllocation-1) = iIndex;

		// Overwrites current array ptr
		return dwAllocation;
	}
	catch (...)
	{
		RunTimeError(RUNTIMEERROR_NOTENOUGHMEMORY);
		return dwArrayPtr;
	}
}
DARKSDK void ArrayDeleteElement(DWORD dwArrayPtr, int iIndex)
{
	// If no array, leave now
	if(dwArrayPtr==NULL) return;

	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return; }

	DWORD dwSizeOfTable = *((DWORD*)dwArrayPtr-4);
	if(dwSizeOfTable==0)
	{
		// already empty - silent failure
		return;
	}
	if(iIndex<0 || iIndex>=(int)dwSizeOfTable)
	{
		RunTimeError(RUNTIMEERROR_ARRAYINDEXINVALID);
		return;
	}

	// Prepare pointers
	DWORD dwDataItemSize = *((DWORD*)dwArrayPtr-3);
	DWORD dwRefSizeInBytes = dwSizeOfTable * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfTable * 1;
	DWORD* pRef = (DWORD*)dwArrayPtr;
	LPSTR pFlag = (LPSTR)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes);
	LPSTR pData = (LPSTR)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes+dwFlagSizeInBytes);
	DWORD dwOffset = ((DWORD)pRef[iIndex] - (DWORD)pData);// / dwDataItemSize;

	// leeadd - 211008 - u71 - check for strings before remove this element
	DWORD dwInternalTypeIndex = *((DWORD*)dwArrayPtr-2);
	LPSTR pPattern = GetTypePatternCore ( NULL, dwInternalTypeIndex );
	if ( pPattern )
	{
		// go through pattern which matches basic types
		DWORD dwTypeInternalOffset = 0;
		for ( DWORD n=0; n<strlen(pPattern); n++ )
		{
			// delete any strings in the user type
			if ( pPattern[n]=='S' )
			{
				// U74 - 050509 - delete CORRECT part of block!
				//LPSTR* pStringData = (LPSTR*)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes+dwFlagSizeInBytes+(dwLocationWithinBlock*dwDataItemSize)+dwTypeInternalOffset);
				LPSTR* pStringData = (LPSTR*)(pData+dwOffset+dwTypeInternalOffset);
				if ( *pStringData )
				{
					delete[] *pStringData;
					*pStringData=NULL;
				}
			}

			// next one..
			switch ( pPattern[n] )
			{
				case 'B'	: dwTypeInternalOffset+=4; break;//1
				case 'Y'	: dwTypeInternalOffset+=4; break;//1
				case 'W'	: dwTypeInternalOffset+=4; break;//2
				case 'O'	: dwTypeInternalOffset+=8; break;
				case 'R'	: dwTypeInternalOffset+=8; break;
				default		: dwTypeInternalOffset+=4; break;
			}
		}
		delete[] pPattern;
	}

	// Shuffle to remove item from ref table
	DWORD dwAmountToShuffle = 0;
	if((dwSizeOfTable-iIndex-1)>0) dwAmountToShuffle = (dwSizeOfTable-iIndex-1)*4;
	if(dwAmountToShuffle>0) memcpy(pRef+iIndex, pRef+iIndex+1, dwAmountToShuffle);

	// Store ref data
	DWORD** pStoreRef = new DWORD* [ dwSizeOfTable ];
	memcpy ( pStoreRef, pRef, dwSizeOfTable * sizeof(DWORD*) );

	// First shuffle out deleted data
	dwAmountToShuffle = 0;
	DWORD dwTotalSizeOfData = dwSizeOfTable * dwDataItemSize;
	if((dwTotalSizeOfData-dwOffset-dwDataItemSize)>0) dwAmountToShuffle = (dwTotalSizeOfData-dwOffset-dwDataItemSize);
	if(dwAmountToShuffle>0) memcpy(pData+dwOffset, pData+dwOffset+dwDataItemSize, dwAmountToShuffle);

	// Reduce size of array
	dwSizeOfTable = dwSizeOfTable - 1;
	*((DWORD*)dwArrayPtr-4) = dwSizeOfTable;

	// Get new sizes and pointers
	DWORD dwNewRefSizeInBytes = dwSizeOfTable * 4;
	DWORD dwNewFlagSizeInBytes = dwSizeOfTable * 1;
	LPSTR pNewFlag = (LPSTR)(((LPSTR)dwArrayPtr)+dwNewRefSizeInBytes);
	LPSTR pNewData = (LPSTR)(((LPSTR)dwArrayPtr)+dwNewRefSizeInBytes+dwNewFlagSizeInBytes);

	// Generate new ref data
	for(DWORD i=0; i<dwSizeOfTable; i++)
	{
		// leefix - 210604 - retain pattern of ref data / 260604-u54-dwDataItemSize not 1
		DWORD dwRedirectOffset = (DWORD)pStoreRef[i] - (DWORD)pData;
		if ( dwRedirectOffset >= dwOffset ) dwRedirectOffset-=dwDataItemSize;
		pRef[i] = (DWORD)(pNewData + dwRedirectOffset);
	}

	// free stored ref data
	delete[] pStoreRef;

	// Set Flag Data with ones
	memset(pNewFlag, 1, dwNewFlagSizeInBytes);

	// Then shuffle all data to new position
	DWORD dwNewTotalSizeOfData = dwSizeOfTable * dwDataItemSize;
	memcpy(pNewData, pData, dwNewTotalSizeOfData);

	// LEEFIX - 021202 - The delete shuffled the ref, blanked a flag and left the
	// data alone. When another insert occured, the size of the array made the old
	// data reader read the wrong flag and data blocks as the ref was 4 bytes smaller
	// solution was to write it so that the data is shuffled and ref/flag recalculated
	// LEEADD - 210604 - IN ADDITION, simply shuffling the data out is not an option
	// because all the insert functions ADD to the size of the array meaning correct
	// reconstruction is the only option here..

	/*
	// Get Ref Table Info
	DWORD* pRef = (DWORD*)dwArrayPtr;

	// Clear flag
	DWORD dwDataItemSize = *((DWORD*)dwArrayPtr-3);
	DWORD dwRefSizeInBytes = dwSizeOfTable * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfTable * 1;
	LPSTR pFlag = (LPSTR)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes);
	LPSTR pData = (LPSTR)(((LPSTR)dwArrayPtr)+dwRefSizeInBytes+dwFlagSizeInBytes);
	DWORD dwOffset = ((DWORD)pRef[iIndex] - (DWORD)pData) / dwDataItemSize;

	// Blank flag in list (redundant but shows what I'm doing)
	pFlag[dwOffset]=0;

	// Shuffle to remove item from ref table
	DWORD dwAmountToShuffle = 0;
	if((dwSizeOfTable-dwOffset-1)>0) dwAmountToShuffle = (dwSizeOfTable-dwOffset-1);
	if(dwAmountToShuffle>0) memcpy(pFlag+dwOffset, pFlag+dwOffset+1, dwAmountToShuffle);

	// Blank ref in list (redundant but shows what I'm doing)
	memset(pRef+iIndex, 0, 4);

	// Shuffle to remove item from ref table
	dwAmountToShuffle = 0;
	if((dwSizeOfTable-iIndex-1)>0) dwAmountToShuffle = (dwSizeOfTable-iIndex-1)*4;
	if(dwAmountToShuffle>0) memcpy(pRef+iIndex, pRef+iIndex+1, dwAmountToShuffle);

	// Clear end of list
	memset(pRef+(dwSizeOfTable-1), 0, 4);

	// Reduce size of array
	dwSizeOfTable = dwSizeOfTable - 1;
	*((DWORD*)dwArrayPtr-4) = dwSizeOfTable;
	*/

	// Ensure internal index is still valid
	if( *((DWORD*)dwArrayPtr-1) >= dwSizeOfTable )
		*((DWORD*)dwArrayPtr-1) = dwSizeOfTable - 1;
}
DARKSDK void ArrayDeleteElement(DWORD dwArrayPtr)
{
	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return; }

	DWORD dwSizeOfTable = *((DWORD*)dwArrayPtr-4);
	if(dwSizeOfTable==0)
	{
		// already empty - silent failure
		return;
	}

	int iCurrentIndex = *((DWORD*)dwArrayPtr-1);
	ArrayDeleteElement ( dwArrayPtr, iCurrentIndex );
}
DARKSDK void EmptyArray(DWORD dwAllocation)
{
	// If no array, leave now
	if(dwAllocation==NULL) return;

	DWORD dwSizeOfTable = *((DWORD*)dwAllocation-4);
	if(dwSizeOfTable==0)
	{
		// already empty - silent failure
		return;
	}

	// Get Array Information
	LPSTR pArrayPtr = ((LPSTR)dwAllocation)-HEADERSIZEINBYTES;
	DWORD* pHeader	= (DWORD*)(pArrayPtr);
	DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
	// should this not be 10,11,12,13??  - 140104
	DWORD dwSizeOfArray = pHeader[0];
	DWORD dwSizeOfOneDataItem = pHeader[1];
	DWORD dwTypeValueOfOneDataItem = pHeader[2];
	DWORD dwInternalIndex = pHeader[3];
	DWORD dwRefSizeInBytes = dwSizeOfArray * 4;
	DWORD dwFlagSizeInBytes = dwSizeOfArray * 1;
	DWORD dwDataSizeInBytes = dwSizeOfArray * dwSizeOfOneDataItem;
	DWORD* pRef = (DWORD*)(pArrayPtr+dwHeaderSizeInBytes);
	LPSTR pFlag = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes);
	LPSTR pData = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes+dwFlagSizeInBytes);

	// Clear all data from array
	FreeStringsFromArray( dwAllocation );
	memset(pRef, 0, dwRefSizeInBytes);
	memset(pFlag, 0, dwFlagSizeInBytes);
	memset(pData, 0, dwDataSizeInBytes);

	// Reset size of array to empty
	*((DWORD*)dwAllocation-4) = 0;
	*((DWORD*)dwAllocation-1) = 0;
}
DARKSDK DWORD PushToStack(DWORD dwArrayPtr)
{
	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

	// add item to bottom of list
	dwArrayPtr = ArrayInsertAtBottom(dwArrayPtr);

	// place index to end
	int iIndexAtEnd = *((DWORD*)dwArrayPtr-4) - 1;
	*((DWORD*)dwArrayPtr-1) = iIndexAtEnd;

	// return array ptr
	return dwArrayPtr;
}
DARKSDK void PopFromStack(DWORD dwArrayPtr)
{
	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return; }

	// remove from bottom if list
	int iIndexAtEnd = *((DWORD*)dwArrayPtr-4) - 1;
	ArrayDeleteElement(dwArrayPtr, iIndexAtEnd);

	// place index to end
	iIndexAtEnd = *((DWORD*)dwArrayPtr-4) - 1;
	*((DWORD*)dwArrayPtr-1) = iIndexAtEnd;
}
DARKSDK DWORD AddToQueue(DWORD dwArrayPtr)
{
	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return dwArrayPtr; }

	// add to top of list
	dwArrayPtr = ArrayInsertAtBottom(dwArrayPtr);

	// place index to end
	int iIndexAtEnd = *((DWORD*)dwArrayPtr-4) - 1;
	*((DWORD*)dwArrayPtr-1) = iIndexAtEnd;

	// return array ptr
	return dwArrayPtr;
}
DARKSDK void RemoveFromQueue(DWORD dwArrayPtr)
{
	// lee - 140306 - u60b3 - Do not allow multi-dimensional arrays
	if ( IsArraySingleDim ( dwArrayPtr )==false ) { RunTimeError(RUNTIMEERROR_ARRAYMUSTBESINGLEDIM); return; }

	// remove from top of list
	ArrayDeleteElement(dwArrayPtr, 0);

	// place index to zero
	*((DWORD*)dwArrayPtr-1) = 0;
}
DARKSDK void ArrayIndexToStack(DWORD dwArrayPtr)
{
	// set index to last item in array
	if(dwArrayPtr) *((DWORD*)dwArrayPtr-1) = *((DWORD*)dwArrayPtr-4)-1;
}
DARKSDK void ArrayIndexToQueue(DWORD dwArrayPtr)
{
	// set index to first item in array
	if(dwArrayPtr) *((DWORD*)dwArrayPtr-1) = 0;
}

// HARDCODE COMMANDS

DARKSDK DWORD MakeByteMemory(int iSize)
{
	LPSTR pMem = new char[iSize];
	return (DWORD)pMem;
}

DARKSDK void DeleteByteMemory(DWORD dwMem)
{
	if(dwMem) delete[] (LPSTR)dwMem;
}

DARKSDK void FillByteMemory(DWORD dwDest, int iValue, int iSize)
{
	memset((LPSTR)dwDest, iValue, iSize);
}

DARKSDK void CopyByteMemory(DWORD dwDest, DWORD dwSrc, int iSize)
{
	memcpy((LPSTR)dwDest, (LPSTR)dwSrc, iSize);
}

// DATA STATEMENT COMMAND FUNCTIONS
DARKSDK void Restore(void)
{
	g_pDataLabelPtr = g_pDataLabelStart;		
}
DARKSDK void RestoreD(DWORD dwDataLabel)
{
	if ( dwDataLabel==0 )
		g_pDataLabelPtr = g_pDataLabelStart;
	else
		g_pDataLabelPtr = (LPSTR)dwDataLabel;
		
}
DARKSDK DWORD ReadL(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	int iValue = (int)dData;

	return *(DWORD*)&iValue;
}
DARKSDK DWORD ReadF(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	float fValue = (float)dData;

	return *(DWORD*)&fValue;
}
DARKSDK DWORD ReadS(DWORD pDestStr)
{
	if(pDestStr) delete[] (LPSTR)pDestStr;

	LPSTR pDatStr = NULL;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==2)
			pDatStr = (LPSTR)*(DWORD*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	LPSTR pString;
	if (pDatStr)
	{
		DWORD dwLength = strlen(pDatStr);
		pString=new char[dwLength+1];
		strcpy(pString, pDatStr);
	}
	else
	{
		pString = new char[1];
		*pString = 0;
	}

	return (DWORD)pString;
}
DARKSDK BYTE ReadB(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	BYTE dwValue = (BYTE)dData;

	return dwValue;
}
DARKSDK WORD ReadW(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	WORD dwValue = (WORD)dData;

	return dwValue;
}
DARKSDK DWORD ReadD(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	DWORD dwValue = (DWORD)dData;

	return dwValue;
}
DARKSDK LONGLONG ReadR(void)
{
	double dData=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dData = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	LONGLONG lValue = (LONGLONG)dData;

	return lValue;
}
DARKSDK double ReadO(void)
{
	double dValue=0;
	if(g_pDataLabelPtr && g_pDataLabelPtr<g_pDataLabelEnd)
	{
		if(*(g_pDataLabelPtr+0)==1)
			dValue = *(double*)(g_pDataLabelPtr+2);

		// Advance After Read, but only to end of data
		g_pDataLabelPtr+=10;
	}

	return dValue;
}

// DWORD POINTER MATHS

/*
DARKDLL DWORD MulDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA*dwValueB;
	return result;
}
DARKDLL DWORD DivDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA/dwValueB;
	return result;
}
DARKDLL DWORD AddDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA+dwValueB;
	return result;
}
DARKDLL DWORD SubDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA-dwValueB;
	return result;
}
*/

// lee - 240306 - u6b4 - re-introduced so that 0xFF < 0x00 can be false

DARKSDK DWORD EqualDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA==dwValueB;
	return result;
}
DARKSDK DWORD GreaterDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA>dwValueB;
	return result;
}
DARKSDK DWORD LessDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA<dwValueB;
	return result;
}
DARKSDK DWORD NotEqualDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA!=dwValueB;
	return result;
}
DARKSDK DWORD GreaterEqualDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA>=dwValueB;
	return result;
}
DARKSDK DWORD LessEqualDDD(DWORD dwValueA, DWORD dwValueB)
{
	int result = dwValueA<=dwValueB;
	return result;
}

// EXTERNAL SUPPORT MATHS

DARKSDK DWORD PowerLLL(int iValueA, int iValueB)
{
	// do not know the ASM version of this
	// mike - 020206 - addition for vs8
	//int result = (int)pow(iValueA,iValueB);
	int result = (int)pow((long double)iValueA,(long double)iValueB);
	return *((DWORD*)&result);
}
DARKSDK DWORD PowerBBB(DWORD dwValueA, DWORD dwValueB)
{
	// mike - 020206 - addition for vs8
	//DWORD result = (unsigned char)pow((unsigned char)dwValueA,(unsigned char)dwValueB);
	DWORD result = (unsigned char)pow((long double)dwValueA,(long double)dwValueB);
	return result;
}
DARKSDK DWORD PowerWWW(DWORD dwValueA, DWORD dwValueB)
{
	// mike - 020206 - addition for vs8
	//DWORD result = (WORD)pow((WORD)dwValueA,(WORD)dwValueB);
	DWORD result = (WORD)pow((long double)dwValueA,(long double)dwValueB);
	return result;
}
DARKSDK DWORD PowerDDD(DWORD dwValueA, DWORD dwValueB)
{
	DWORD result = (DWORD)pow(( double ) dwValueA,( double ) dwValueB);
	return result;
}

/*
DARKDLL DWORD MulLLL(int iValueA, int iValueB)
{
	int result = iValueA*iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD DivLLL(int iValueA, int iValueB)
{
	if(iValueB==0) return 0;
	int result = iValueA/iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD AddLLL(int iValueA, int iValueB)
{
	int result = iValueA+iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD SubLLL(int iValueA, int iValueB)
{
	int result = iValueA-iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD ModLLL(int iValueA, int iValueB)
{
	if(iValueB==0) return 0;
	int result = iValueA % iValueB;
	return *((DWORD*)&result);
}
*/

// BITWISE COMMANDS
/*
DARKDLL DWORD ShiftLeftLLL(int iValueA, int iValueB)
{
	int result = iValueA<<iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD ShiftRightLLL(int iValueA, int iValueB)
{
	int result = iValueA>>iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD BitANDLLL(int iValueA, int iValueB)
{
	int result = iValueA & iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD BitORLLL(int iValueA, int iValueB)
{
	int result = iValueA | iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD BitXORLLL(int iValueA, int iValueB)
{
	int result = iValueA ^ iValueB;
	return *((DWORD*)&result);
}
*/

// LOGIC OPERATION MATHS

/*
DARKDLL DWORD OrLLL(int iValueA, int iValueB)
{
	int result = iValueA || iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD AndLLL(int iValueA, int iValueB)
{
	int result = iValueA && iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD NotLLL(int iValueA, int iValueB)
{
	// iValueA ignored
	int result = !iValueB;
	return *((DWORD*)&result);
}
*/

// COMPARISON MATH

/*
DARKDLL DWORD EqualLLL(int iValueA, int iValueB)
{
	int result = iValueA==iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD GreaterLLL(int iValueA, int iValueB)
{
	int result = iValueA>iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD LessLLL(int iValueA, int iValueB)
{
	int result = iValueA<iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD NotEqualLLL(int iValueA, int iValueB)
{
	int result = iValueA!=iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD GreaterEqualLLL(int iValueA, int iValueB)
{
	int result = iValueA>=iValueB;
	return *((DWORD*)&result);
}
DARKDLL DWORD LessEqualLLL(int iValueA, int iValueB)
{
	int result = iValueA<=iValueB;
	return *((DWORD*)&result);
}
*/

// FLOAT MATHS

DARKSDK DWORD PowerFFF(float fValueA, float fValueB)
{
	float result = (float)pow(fValueA,fValueB);
	return *((DWORD*)&result);
}
DARKSDK DWORD MulFFF(float fValueA, float fValueB)
{
	float result = fValueA*fValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD DivFFF(float fValueA, float fValueB)
{
	if(fValueB==0) return 0;
	float result = fValueA/fValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD AddFFF(float fValueA, float fValueB)
{
	float result = fValueA+fValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD SubFFF(float fValueA, float fValueB)
{
	float result = fValueA-fValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD ModFFF(float fValueA, float fValueB)
{
	// lee - 150206 - u60 -floating point MOD added
	if(fValueB==0) return 0;
	double w = (double)fValueA;
	double x = (double)fValueB;
	double z = fmod( w, x );
	float result = (float)z;
	return *((DWORD*)&result);
}

// FLOAT COMPARE MATHS

DARKSDK DWORD EqualLFF(float fValueA, float fValueB)
{
	return fValueA==fValueB;
}
DARKSDK DWORD GreaterLFF(float fValueA, float fValueB)
{
	return fValueA>fValueB;
}
DARKSDK DWORD LessLFF(float fValueA, float fValueB)
{
	return fValueA<fValueB;
}
DARKSDK DWORD NotEqualLFF(float fValueA, float fValueB)
{
	return fValueA!=fValueB;
}
DARKSDK DWORD GreaterEqualLFF(float fValueA, float fValueB)
{
	return fValueA>=fValueB;
}
DARKSDK DWORD LessEqualLFF(float fValueA, float fValueB)
{
	return fValueA<=fValueB;
}

// STRING COMPARE MATHS

DARKSDK DWORD EqualLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)==NULL)
			return 1;
		else
			return 0;
	}
	else
	{
		if(dwSrcStr==dwDestStr)
		{
			return 1;
		}
		else
		{
			if(dwSrcStr && dwDestStr==NULL)
			{
				if(strcmp((LPSTR)dwSrcStr, "")==NULL)
					return 1;
				else
					return 0;
			}
			if(dwDestStr && dwSrcStr==NULL)
			{
				if(strcmp((LPSTR)dwDestStr, "")==NULL)
					return 1;
				else
					return 0;
			}
			return 0;
		}
	}
}
DARKSDK DWORD GreaterLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)>0)
			return 1;
		else
			return 0;
	}
	else
	{
		if(dwSrcStr==dwDestStr)
			return 1;
		else
			return 0;
	}
}
DARKSDK DWORD LessLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)<0)
			return 1;
		else
			return 0;
	}
	else
	{
		if(dwSrcStr==dwDestStr)
			return 1;
		else
			return 0;
	}
}
DARKSDK DWORD NotEqualLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)!=0)
			return 1;
		else
			return 0;
	}
	else
	{
		// leefix - 060405 - inverse of equal
		if(dwSrcStr==dwDestStr)
		{
			return 0;
		}
		else
		{
			if(dwSrcStr && dwDestStr==NULL)
			{
				if(strcmp((LPSTR)dwSrcStr, "")==NULL)
					return 0;
				else
					return 1;
			}
			if(dwDestStr && dwSrcStr==NULL)
			{
				if(strcmp((LPSTR)dwDestStr, "")==NULL)
					return 0;
				else
					return 1;
			}
			return 1;
		}
		// leefix - 060405 - old way and very wrong
		// if(dwSrcStr!=dwDestStr)
		//	return 1;
		// else
		//	return 0;
	}
}
DARKSDK DWORD GreaterEqualLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)>=0)
			return 1;
		else
			return 0;
	}
	else
	{
		if(dwSrcStr==dwDestStr)
			return 1;
		else
			return 0;
	}
}
DARKSDK DWORD LessEqualLSS(DWORD dwSrcStr,DWORD dwDestStr)
{
	if(dwSrcStr && dwDestStr)
	{
		if(strcmp((LPSTR)dwSrcStr, (LPSTR)dwDestStr)<=0)
			return 1;
		else
			return 0;
	}
	else
	{
		if(dwSrcStr==dwDestStr)
			return 1;
		else
			return 0;
	}
}


// STRING MATHS

DARKSDK DWORD AddSSS(DWORD dwRetStr, DWORD dwSrcStrA, DWORD dwSrcStrB)
{
	DWORD length=1;
	if((DWORD*)dwSrcStrA) length=strlen((LPSTR)(DWORD*)dwSrcStrA);
	if((DWORD*)dwSrcStrB) length+=strlen((LPSTR)(DWORD*)dwSrcStrB);
	LPSTR lpNewStr = new char[length+1];
	strcpy(lpNewStr,"");
	if((DWORD*)dwSrcStrA) strcat(lpNewStr, (LPSTR)dwSrcStrA);
	if((DWORD*)dwSrcStrB) strcat(lpNewStr, (LPSTR)dwSrcStrB);
	if((DWORD*)dwRetStr) delete[] (DWORD*)dwRetStr;
	return (DWORD)lpNewStr;
}
DARKSDK DWORD EquateSS(DWORD dwDestStr,DWORD dwSrcStr)
{
	DWORD length=1;
	if((DWORD*)dwSrcStr) length=strlen((LPSTR)(DWORD*)dwSrcStr);
	LPSTR lpNewStr = NULL;
	if ( dwSrcStr!=0 )
	{
		lpNewStr = new char[length+1];
		if((DWORD*)dwSrcStr) strcpy(lpNewStr, (LPSTR)dwSrcStr); else strcpy(lpNewStr,"");
	}
	if((DWORD*)dwDestStr) delete[] (DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
}
DARKSDK DWORD FreeSS(DWORD dwDestStr)
{
	if(dwDestStr) delete[] (DWORD*)dwDestStr;
	return 0;
}
DARKSDK DWORD FreeStringSS(DWORD dwDestStr)
{
	if(dwDestStr) delete[] (DWORD*)dwDestStr;
	return 0;
}

// DOUBLE FLOAT MATHS

DARKSDK double PowerOOO(double dValueA, double dValueB)
{
	double result = (float)pow(dValueA,dValueB);
	return result;
}
DARKSDK double MulOOO(double dValueA, double dValueB)
{
	double result = dValueA*dValueB;
	return result;
}
DARKSDK double DivOOO(double dValueA, double dValueB)
{
	if(dValueB==0) return 0;
	double result = dValueA/dValueB;
	return result;
}
DARKSDK double AddOOO(double dValueA, double dValueB)
{
	double result = dValueA+dValueB;
	return result;
}
DARKSDK double SubOOO(double dValueA, double dValueB)
{
	double result = dValueA-dValueB;
	return result;
}

// DOUBLE FLOAT COMPARISONS

DARKSDK DWORD EqualLOO(double dValueA, double dValueB)
{
	int result = dValueA==dValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD GreaterLOO(double dValueA, double dValueB)
{
	int result = dValueA>dValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD LessLOO(double dValueA, double dValueB)
{
	int result = dValueA<dValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD NotEqualLOO(double dValueA, double dValueB)
{
	int result = dValueA!=dValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD GreaterEqualLOO(double dValueA, double dValueB)
{
	int result = dValueA>=dValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD LessEqualLOO(double dValueA, double dValueB)
{
	int result = dValueA<=dValueB;
	return *((DWORD*)&result);
}

// DOUBLE INTEGER MATHS

DARKSDK LONGLONG PowerRRR(LONGLONG dValueA, LONGLONG dValueB)
{
	LONGLONG result = (LONGLONG)pow((double)dValueA,(double)dValueB);
	return result;
}
DARKSDK LONGLONG MulRRR(LONGLONG dValueA, LONGLONG dValueB)
{
	LONGLONG result = dValueA*dValueB;
	return result;
}
DARKSDK LONGLONG DivRRR(LONGLONG dValueA, LONGLONG dValueB)
{
	if(dValueB==0) return 0;
	LONGLONG result = dValueA/dValueB;
	return result;
}
DARKSDK LONGLONG AddRRR(LONGLONG dValueA, LONGLONG dValueB)
{
	LONGLONG result = dValueA+dValueB;
	return result;
}
DARKSDK LONGLONG SubRRR(LONGLONG dValueA, LONGLONG dValueB)
{
	LONGLONG result = dValueA-dValueB;
	return result;
}

// DOUBLE INTEGER COMPARISONS

DARKSDK DWORD EqualLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA==lValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD GreaterLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA>lValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD LessLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA<lValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD NotEqualLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA!=lValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD GreaterEqualLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA>=lValueB;
	return *((DWORD*)&result);
}
DARKSDK DWORD LessEqualLRR(LONGLONG lValueA, LONGLONG lValueB)
{
	int result = lValueA<=lValueB;
	return *((DWORD*)&result);
}

// CASTING MATHS

DARKSDK DWORD CastLtoF(int iValue)
{
	float result = (float)iValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastLtoB(int iValue)
{
	unsigned char result = (unsigned char)iValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastLtoY(int iValue)
{
	unsigned char result = (unsigned char)iValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastLtoW(int iValue)
{
	WORD result = (WORD)iValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastLtoD(int iValue)
{
	DWORD result = (DWORD)iValue;
	return *((DWORD*)&result);
}
DARKSDK double CastLtoO(int iValue)
{
	return (double)iValue;
}
DARKSDK LONGLONG CastLtoR(int iValue)
{
	return (LONGLONG)iValue;
}
DARKSDK DWORD CastFtoL(float fValue)
{
	int result = (int)fValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastFtoB(float fValue)
{
	unsigned char result = (unsigned char)fValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastFtoW(float fValue)
{
	WORD result = (WORD)fValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastFtoD(float fValue)
{
	// a maxed out DWORD produces wrong float, so keep it within 4bytes 
	LONGLONG Long = (LONGLONG)fValue;
	if(Long>4294967295) Long=4294967295;

	DWORD result = (DWORD)Long;
	return *((DWORD*)&result);
}
DARKSDK double CastFtoO(float fValue)
{
	// LEEFIX - 141102 - FLD is different on AMD processprs, so truncate..
	return (double)fValue;
	/*
	LONGLONG iBig = (LONGLONG)f;
	double a=(double)iBig/100000.0;
	return a;
	*/
}
DARKSDK LONGLONG CastFtoR(float fValue)
{
	return (LONGLONG)fValue;
}
DARKSDK DWORD CastBtoL(unsigned char cValue)
{
	int result = (int)cValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastBtoF(unsigned char cValue)
{
	float result = (float)cValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastBtoW(unsigned char cValue)
{
	WORD result = (WORD)cValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastBtoD(unsigned char cValue)
{
	DWORD result = (DWORD)cValue;
	return *((DWORD*)&result);
}
DARKSDK double CastBtoO(unsigned char cValue)
{
	return (double)cValue;
}
DARKSDK LONGLONG CastBtoR(unsigned char cValue)
{
	return (LONGLONG)cValue;
}
DARKSDK DWORD CastWtoL(WORD wValue)
{
	int result = (int)wValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastWtoF(WORD wValue)
{
	float result = (float)wValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastWtoB(WORD wValue)
{
	unsigned char result = (unsigned char)wValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastWtoD(WORD wValue)
{
	DWORD result = (DWORD)wValue;
	return *((DWORD*)&result);
}
DARKSDK double CastWtoO(WORD wValue)
{
	return (double)wValue;
}
DARKSDK LONGLONG CastWtoR(WORD wValue)
{
	return (LONGLONG)wValue;
}
DARKSDK DWORD CastDtoL(DWORD dwValue)
{
	int result = (int)dwValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastDtoF(DWORD dwValue)
{
	float result = (float)dwValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastDtoB(DWORD dwValue)
{
	unsigned char result = (unsigned char)dwValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastDtoW(DWORD dwValue)
{
	WORD result = (WORD)dwValue;
	return *((DWORD*)&result);
}
DARKSDK double CastDtoO(DWORD dwValue)
{
	return (double)dwValue;
}
DARKSDK LONGLONG CastDtoR(DWORD dwValue)
{
	return (LONGLONG)dwValue;
}
DARKSDK DWORD CastOtoL(double dValue)
{
	int result = (int)dValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastOtoF(double dValue)
{
	float result = (float)dValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastOtoB(double dValue)
{
	unsigned char result = (unsigned char)dValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastOtoW(double dValue)
{
	WORD result = (WORD)dValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastOtoD(double dValue)
{
	DWORD result = (DWORD)dValue;
	return *((DWORD*)&result);
}
DARKSDK LONGLONG CastOtoR(double dValue)
{
	LONGLONG result = (LONGLONG)dValue;
	return result;
}
DARKSDK DWORD CastRtoL(LONGLONG lValue)
{
	int result = (int)lValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastRtoF(LONGLONG lValue)
{
	float result = (float)lValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastRtoB(LONGLONG lValue)
{
	unsigned char result = (unsigned char)lValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastRtoW(LONGLONG lValue)
{
	WORD result = (WORD)lValue;
	return *((DWORD*)&result);
}
DARKSDK DWORD CastRtoD(LONGLONG lValue)
{
	DWORD result = (DWORD)lValue;
	return *((DWORD*)&result);
}
DARKSDK double CastRtoO(LONGLONG lValue)
{
	return (double)lValue;
}

// MATHEMATICAL COMMANDS
DBPRO_GLOBAL double gDegToRad = 3.141592654f/180.0f;
DBPRO_GLOBAL double gRadToDeg = 180.0f/3.141592654f;

DB_EXPORT dbReturnFloat_t AbsFF(float fValue)
{
	/*
	float result = (float)fabs(fValue);
	return *((DWORD*)&result);
	*/
	return dbReturnFloat( db3::Abs( fValue ) );
}

DARKSDK DWORD IntLF(float fValue)
{
	int result = (int)fValue;
	return *((DWORD*)&result);
}

DARKSDK DWORD AcosFF(float fValue)
{
	float result = (float)(acos(fValue)*gRadToDeg);
	return *((DWORD*)&result);
}

DARKSDK DWORD AsinFF(float fValue)
{
	float result = (float)(asin(fValue)*gRadToDeg);
	return *((DWORD*)&result);
}

DARKSDK DWORD AtanFF(float fValue)
{
	float result = (float)(atan(fValue)*gRadToDeg);
	return *((DWORD*)&result);
}

DARKSDK DWORD Atan2FFF(float fA, float fB)
{
	float result = (float)(atan2(fA, fB)*gRadToDeg);
	return *((DWORD*)&result);
}

DARKSDK dbReturnFloat_t CosFF(float fAngle)
{
	return dbReturnFloat( db3::Cos(fAngle) );
	/*
	float result = (float)cos(fAngle*gDegToRad);
	return *((DWORD*)&result);
	//*/
}

DARKSDK dbReturnFloat_t SinFF(float fAngle)
{
	return dbReturnFloat( db3::Sin(fAngle) );
	/*
	float result = (float)sin(fAngle*gDegToRad);
	return *((DWORD*)&result);
	//*/
}

DARKSDK dbReturnFloat_t TanFF(float fAngle)
{
	return dbReturnFloat( db3::Tan(fAngle) );
	/*
	float result = (float)tan(fAngle*gDegToRad);
	return *((DWORD*)&result);
	//*/
}

DARKSDK DWORD HcosFF(float fAngle)
{
	float result = (float)cosh(fAngle*gDegToRad);
	return *((DWORD*)&result);
}

DARKSDK DWORD HsinFF(float fAngle)
{
	float result = (float)sinh(fAngle*gDegToRad);
	return *((DWORD*)&result);
}

DARKSDK DWORD HtanFF(float fAngle)
{
	float result = (float)tanh(fAngle*gDegToRad);
	return *((DWORD*)&result);
}

DARKSDK DWORD SqrtFF(float fValue)
{
	float result = (float)sqrt(fValue);
	return *((DWORD*)&result);
}

DARKSDK DWORD ExpFF(float fExp)
{
	float result = (float)exp(fExp);
	return *((DWORD*)&result);
}

DB_EXPORT dbReturnFloat_t SignF(float a) {
	return dbReturnFloat( db3::Sign( a ) );
}
DB_EXPORT dbReturnFloat_t CopySign(float a, float b) {
	return dbReturnFloat( db3::CopySign( a, b ) );
}

DB_EXPORT int FloatToIntFast(float x) {
	return db3::FloatToIntFast( x );
}
DB_EXPORT DWORD FloatToDwordFast(float x) {
	return db3::FloatToUIntFast( x );
}

DB_EXPORT dbReturnFloat_t SqrtFast(float x) {
	return dbReturnFloat( db3::SqrtFast( x ) );
}
DB_EXPORT dbReturnFloat_t InvSqrtFast(float x) {
	return dbReturnFloat( db3::InvSqrtFast( x ) );
}

DB_EXPORT dbReturnFloat_t Lerp(float x, float y, float t) {
	return dbReturnFloat( db3::Lerp(x, y, t) );
}
	
DARKSDK void Randomize(int iSeed)
{
	srand(iSeed);
}

/* LEEFIX = 121102 - Should return a float A=RND(20)+B should make INT result
DARKDLL DWORD RndFL(int r)
{
	float result=0.0f;
	if(r>0) result = (float)(rand()%(r+1));
	return *((DWORD*)&result);
}
*/
DARKSDK int RndLL(int r)
{
	int result=0;
	if(r>0)
	{
		// leefix - 250604 - u54 - 0 to 22 million now
		if ( r>1000 )  result += (rand()*1000);
		if ( r>100 ) result += (rand()*100);
		if ( r>10 ) result += (rand()*10);
		result += rand();
		result %= r+1;
	}
	return result;
}

// New MATH FUNCTIONS

DARKSDK DWORD CeilFF(float x)
{
	float value = ceil ( x );
	return *((DWORD*)&value);
}

DARKSDK DWORD FloorFF(float x)
{
	float value = floor ( x );
	return *((DWORD*)&value);
}

// 3D MATH EXPRESSIONS

DARKSDK float wrapangleoffset(float da)
{
	// leefix - 250604 - u54 - resolve LARGE wrap input values
	// leefix - 090704 - u55 - and minus wrap fix too
	/*
	int iChunkOut = (int)da;
	iChunkOut = iChunkOut - (iChunkOut % 360);
	da = da - iChunkOut;
	int breakout=10000;
	while(da<0.0f || da>=360.0f)
	{
		if(da<0.0f) da=da+360.0f;
		if(da>=360.0f) da=da-360.0f;
		breakout--;
		if(breakout==0) break;
	}
	if(breakout==0) da=0.0f;
	return da;
	*/
	// aaron - 20120811 - Faster version from NormalizeAngle360
	return db3::NormalizeAngle360(da);
}

DARKSDK DWORD CurveValueFFFF(float a, float da, float sp)
{
	if(sp<1.0f) sp=1.0f;
	float diff = a-da;
	da=da+(diff/sp);
	return *((DWORD*)&da);
}

DARKSDK DWORD WrapValueFF(float da)
{
	da = wrapangleoffset(da);
	return *((DWORD*)&da);
}

DARKSDK DWORD NewXValueFFFF(float x, float a, float b)
{
	float da = x + ((float)sin(D3DXToRadian(a))*b);
	return *((DWORD*)&da);
}

DARKSDK DWORD NewZValueFFFF(float z, float a, float b)
{
	float da = z + ((float)cos(D3DXToRadian(a))*b);
	return *((DWORD*)&da);
}

DARKSDK DWORD NewYValueFFFF(float y, float a, float b)
{
	float da = y - ((float)sin(D3DXToRadian(a))*b);
	return *((DWORD*)&da);
}

DARKSDK DWORD CurveAngleFFFF(float a, float da, float sp)
{
	if(sp<1.0f) sp=1.0f;
	a = wrapangleoffset(a);
	da = wrapangleoffset(da);
	float diff = a-da;
	if(diff<-180.0f) diff=(a+360.0f)-da;
	if(diff>180.0f) diff=a-(da+360.0f);
	da=da+(diff/sp);
	da = wrapangleoffset(da);
	return *((DWORD*)&da);
}

DB_EXPORT int NextPowerOfTwo1(int x) {
	return db3::NextPowerOfTwo(x);
}
DB_EXPORT int NextPowerOfTwo2(int x, int y) {
	return db3::NextSquarePowerOfTwo(x, y);
}

DB_EXPORT dbReturnFloat_t Clamp(float x, float l, float h) {
	return dbReturnFloat( db3::Clamp(x, l,h) );
}
DB_EXPORT dbReturnFloat_t ClampSNorm(float x) {
	return dbReturnFloat( db3::ClampSNorm(x) );
}
DB_EXPORT dbReturnFloat_t ClampUNorm(float x) {
	return dbReturnFloat( db3::ClampUNorm(x) );
}

DB_EXPORT dbReturnFloat_t Min(float x, float y) {
	return dbReturnFloat( db3::Min(x, y) );
}
DB_EXPORT dbReturnFloat_t Max(float x, float y) {
	return dbReturnFloat( db3::Max(x, y) );
}


// MISCLANIOUS CORE COMMANDS

DARKSDK int TimerL(void)
{
	// leefix - 230606 - u62 - timeBeginPeriod/timeEndPeriod added
	timeBeginPeriod(1);
	DWORD dwTimer = timeGetTime();
	timeEndPeriod(1);
	// davefix - 240615 - casting from dword to int can cause the number to be negative
	int iTimer = (int)dwTimer;
	// if negative, turn that frown upside down
	if ( iTimer < 0 ) iTimer = INT_MAX-iTimer;
	return iTimer;
}

DARKSDK void SleepL(int iDelay)
{
	DWORD dwTimeNow=timeGetTime();
	while(timeGetTime()<=dwTimeNow+iDelay)
	{
		if(InternalProcessMessages()==1) break;
	}
}

DARKSDK void WaitL(int iDelay)
{
	DWORD dwTimeNow=timeGetTime();
	while(timeGetTime()<=dwTimeNow+iDelay)
	{
		if(InternalProcessMessages()==1) break;
	}
}

DARKSDK void MemorySnapshot(int iMode)
{
	// Ask MM system to make snapshot of core
	#ifdef  __USE_MEMORY_MANAGER__
	mm_SnapShot();
	#endif

	// also go through all DLLs in DBP and ask them to make a snapshot prior to this report
	for ( int iDLL=0; iDLL<1; iDLL++ )
	{
		HINSTANCE hThis = NULL;
		if ( iDLL==0 ) hThis = g_Glob.g_Basic3D;
		if ( hThis )
		{
			typedef void ( *MM_SNAPSHOT ) ( void );
			MM_SNAPSHOT pSnapShotFunc;
			pSnapShotFunc = ( MM_SNAPSHOT ) GetProcAddress ( hThis, "?mm_SnapShot@@YAXXZ" );
			if ( pSnapShotFunc ) pSnapShotFunc();
		}
	}
	//g_Glob.g_Text;
	//g_Glob.g_Basic2D;
	//g_Glob.g_Sprites;
	//g_Glob.g_Image;
	//g_Glob.g_Input;
	//g_Glob.g_System;
	//g_Glob.g_File;
	//g_Glob.g_FTP;
	//g_Glob.g_Memblocks;
	//g_Glob.g_Bitmap;
	//g_Glob.g_Animation;
	//g_Glob.g_Multiplayer;
	//g_Glob.g_Camera3D;
	//g_Glob.g_Matrix3D;
	//g_Glob.g_Light3D;
	//g_Glob.g_World3D;
	//g_Glob.g_Particles;
	//g_Glob.g_PrimObject;
	//g_Glob.g_Vectors;
	//g_Glob.g_XObject;
	//g_Glob.g_3DSObject;
	//g_Glob.g_MDLObject;
	//g_Glob.g_MD2Object;
	//g_Glob.g_MD3Object;
	//g_Glob.g_Sound;
	//g_Glob.g_Music;
	//g_Glob.g_LODTerrain;
	//g_Glob.g_Q2BSP;
	//g_Glob.g_OwnBSP;
	//g_Glob.g_BSPCompiler;
	//g_Glob.g_CSG;
	//g_Glob.g_igLoader;
	//g_Glob.g_GameFX;
	//g_Glob.g_Transforms;

	// then produce a unified report
	#ifdef  __USE_MEMORY_MANAGER__
	mm_Report();
	#endif
}

DARKSDK void WaitForKey(void)
{
	while(g_wWinKey!=0)
	{
		if(InternalProcessMessages()==1) break;
	}
	while(g_wWinKey==0)
	{
		if(InternalProcessMessages()==1) break;
	}
}

DARKSDK void WaitForMouse(void)
{
	while(g_Glob.iWindowsMouseClick!=0)
	{
		if(InternalProcessMessages()==1) break;
	}
	while(g_Glob.iWindowsMouseClick==0)
	{
		if(InternalProcessMessages()==1) break;
	}
}

DARKSDK DWORD Cl$(DWORD dwDestStr)
{
	// get command line from main program...
	LPSTR lpNewStr = NULL;
	if(g_pCommandLineString)
	{
		lpNewStr = new char[strlen(g_pCommandLineString)+1];
		strcpy(lpNewStr, g_pCommandLineString);
	}
	else
	{
		lpNewStr = new char[2];
		strcpy(lpNewStr, "");
	}
	if((DWORD*)dwDestStr) delete[] (LPSTR)(DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
}

DARKSDK DWORD GetDate$(DWORD dwDestStr)
{
	/*
	LPSTR lpNewStr = new char[255];
	_strdate(lpNewStr);
	if((DWORD*)dwDestStr) delete[] (LPSTR)(DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
	*/
	char buf[256];
	_strdate(buf);
	return reinterpret_cast<DWORD>(dbReturnString(reinterpret_cast<char *>(dwDestStr), buf));
}

DARKSDK DWORD GetTime$(DWORD dwDestStr)
{
	/*
	LPSTR lpNewStr = new char[255];
	_strtime(lpNewStr);
	if((DWORD*)dwDestStr) delete[] (LPSTR)(DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
	*/
	char buf[256];
	_strtime(buf);
	return reinterpret_cast<DWORD>(dbReturnString(reinterpret_cast<char *>(dwDestStr), buf));
}

DARKSDK DWORD InkeyS(DWORD dwDestStr)
{
	/*
	LPSTR lpNewStr = new char[2];
	lpNewStr[0]=g_cInkeyCodeKey;
	lpNewStr[1]=0;
	if((DWORD*)dwDestStr) delete[] (LPSTR)(DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
	*/
	char buf[2];

	buf[0] = g_cInkeyCodeKey;
	buf[1] = '\0';

	return reinterpret_cast<DWORD>(dbReturnString(reinterpret_cast<char *>(dwDestStr), buf));
}

DARKSDK void SyncOn(void)
{
	g_bSyncOff = false;
	g_bProcessorFriendly = false;
	g_bCanRenderNow = false;
}

DARKSDK void SyncOff(void)
{
	g_bSyncOff = true;
	g_bProcessorFriendly = true;
	g_bCanRenderNow = true;
}

DARKSDK void Sync(void)
{
	ExternalDisplaySync(0);
	ProcessMessagesOnly();
	ConstantNonDisplayUpdate();
	g_bCanRenderNow = true;
}

DARKSDK void Sync(int iProcessMessages)
{
	ExternalDisplaySync(0);
	if ( iProcessMessages==1 ) ProcessMessagesOnly();
	ConstantNonDisplayUpdate();
	g_bCanRenderNow = true;
}

DARKSDK void FastSync(void)
{
	ExternalDisplaySync(1);
	//V111 - 110608 - no need for this as FASTSYNC just used to render cameras (main SYNC handles proper update per cycle)
	//ConstantNonDisplayUpdate(); // lee - 100208 - moved from ConstantNonDisplayUpdate(), added by Mike in 2005
	g_bCanRenderNow = true;
}

DARKSDK void FastSync ( int iNonDisplayUpdates )
{
	ExternalDisplaySync(1);
	if ( iNonDisplayUpdates==1 )
	{
		// leeadd - 061108 - reinstated for U71 by request under parameter
		ConstantNonDisplayUpdate();
	}
	g_bCanRenderNow = true;
}

DARKSDK void SyncRate(int iRate)
{
	// Reset everything to run full speed
	SAFE_DELETE_ARRAY( g_pdwSyncRateSetting);
	g_dwSyncRateSettingSize = 0;
	g_dwManualSuperStepSetting = 0;

	// Zero is full speed
	// Anything over 1000 can't be measured, so treat that as full speed too
	if (iRate == 0 || iRate > 1000)
		return;

	// Negative is super stepping mode
	if (iRate < 0)
	{
		g_dwManualSuperStepSetting = abs(iRate);
		return;
	}

	// What's left can be dealt with.
	// Generate a table that covers 1 second of frames and fill it out with
	// the basic MS-per-frame value. Any milliseconds dropped using the calculation
	// will be evenly distributed within the table.
	g_dwSyncRateSettingSize = iRate;
	g_pdwSyncRateSetting = new DWORD[ iRate ];

	DWORD RoundedMS                 =   1000 / iRate;
	DWORD DroppedTotalMS            =   1000 - (RoundedMS * iRate);
	float DroppedPerFrameMS         =   (float)(DroppedTotalMS) / iRate;
	float AccumulatedDroppedMS      =   0.0;

	for (int i = 0; i < iRate; ++i)
	{
		if (AccumulatedDroppedMS >= 1.0)
		{
			g_pdwSyncRateSetting[i] = RoundedMS + 1;
			AccumulatedDroppedMS -= 1.0;
			--DroppedTotalMS;
		}
		else
		{
			g_pdwSyncRateSetting[i] = RoundedMS;
		}
		AccumulatedDroppedMS += DroppedPerFrameMS;
	}

	// Any further dropped milliseconds, just use them against entries that haven't
	// already had them added previously until they are all used up.
	// This needed because of float (in)accuracy.
	for (int i = 0; i < iRate && DroppedTotalMS > 0; ++i)
	{
		if (g_pdwSyncRateSetting[i] == RoundedMS)
		{
			++g_pdwSyncRateSetting[i];
			--DroppedTotalMS;
		}
	}
}

DWORD GetNextSyncDelay()
{
	// If there is no table, then run return a 'no delay'
	if (g_dwSyncRateSettingSize == 0)
		return 0;

	// Advance the index, reset to start if gone beyond the end of the table
	++g_dwSyncRateCurrent;
	if (g_dwSyncRateCurrent >= g_dwSyncRateSettingSize)
		g_dwSyncRateCurrent = 0;

	// Return the current delay
	return g_pdwSyncRateSetting[ g_dwSyncRateCurrent ];
}

DARKSDK void SyncDisableQuad(void)
{
	g_bDrawQuadInSync = false;
}

DARKSDK void SyncEnableQuad(void)
{
	g_bDrawQuadInSync = true;
}

DARKSDK void SyncRenderQuad(void)
{
	if(g_Basic3D_RenderQuad)
	{
		if(g_Basic3D_RenderQuad(0)==1)
		{
			g_Camera3D_RunCode ( 0 );
			g_Basic3D_RenderQuad(1);
		}
	}
}

DARKSDK void DrawToBack(void)
{
	g_bDrawAutoStuffFirst = false;
}

DARKSDK void DrawToFront(void)
{
	g_bDrawAutoStuffFirst = true;
}

DARKSDK void DrawToCamera(void)
{
	g_bDrawEntirelyToCamera = true;
}

DARKSDK void DrawToScreen(void)
{
	g_bDrawEntirelyToCamera = false;
}

DARKSDK void DrawSpritesFirst(void)
{
	g_bDrawSpritesFirst=true;
}

DARKSDK void DrawSpritesLast(void)
{
	g_bDrawSpritesFirst=false;
}

DARKSDK void SaveArray(LPSTR pFilename, DWORD dwAllocation)
{
	// Temp vars
	DWORD written;

	// If Array Exists
	if(dwAllocation)
	{
		// Header Info
		DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
		DWORD dwSizeOfArray = *((DWORD*)dwAllocation-4);
		DWORD dwElementSize = *((DWORD*)dwAllocation-3);
		DWORD dwExistingElementType = *((DWORD*)dwAllocation-2);
		DWORD dwTableSizeInBytes = dwSizeOfArray * 4;

		// Can only save pure types
		if(dwExistingElementType<9)
		{
			// Create File for array
			HANDLE hFile = CreateFile(pFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile!=INVALID_HANDLE_VALUE)
			{
				// String arrays can be text file dumps
				if(dwExistingElementType==2)
				{
					// Save Out Array (of x size)
					DWORD dwDataPointer=dwAllocation+dwTableSizeInBytes;
					for(DWORD n=0; n<dwSizeOfArray; n++)
					{
						DWORD* pEntry = *((DWORD**)dwAllocation+n);
						if(pEntry)
						{
							// String data
							DWORD dwStringSize=0;
							LPSTR pStr = (LPSTR)*pEntry;
							if(*pEntry) dwStringSize = strlen(pStr);
							if(dwStringSize>0) WriteFile(hFile, pStr, dwStringSize, &written, FALSE);

							// carriage return
							char CR[2]; CR[0]=13; CR[1]=10;
							WriteFile(hFile, &CR, 2, &written, FALSE);
						}
					}
				}
				else
				{
					// Write Type of Array (element type 2=string)
					WriteFile(hFile, &dwExistingElementType, 4, &written, FALSE);

					// Write Size of Array (elements)
					WriteFile(hFile, &dwSizeOfArray, 4, &written, FALSE);

					// Save Out Array (of x size)
					DWORD dwDataPointer=dwAllocation+dwTableSizeInBytes;
					for(DWORD n=0; n<dwSizeOfArray; n++)
					{
						DWORD* pEntry = *((DWORD**)dwAllocation+n);
						if(pEntry)
						{
							// Write Index + Datablock
							int indexn=(int)n;
							WriteFile(hFile, &indexn, 4, &written, FALSE);

							// Value
							WriteFile(hFile, pEntry, dwElementSize, &written, FALSE);
						}
					}

					// Write Index of -1 to end
					int endn=-1;
					WriteFile(hFile, &endn, 4, &written, FALSE);
				}

				// Close file
				CloseHandle(hFile);
			}
			else
			{
				// runtime - could not create array file
				char pErrStr[1024];
				sprintf ( pErrStr, "Failed to CreateFile with: %s", pFilename );
				Message ( 0, pErrStr, "" );
				RunTimeError(RUNTIMEERROR_INVALIDFILE);
			}
		}
		else
		{
			//runtime not right type
			RunTimeError(RUNTIMEERROR_ARRAYTYPEINVALID);
		}
	}
}

DARKSDK void LoadArrayCore(LPSTR pFilename, DWORD dwAllocation)
{
	// Temp vars
	DWORD readen;

	// If Array Exists
	if(dwAllocation)
	{
		// Header Info
		DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
		DWORD dwExistingSizeOfArray = *((DWORD*)dwAllocation-4);
		DWORD dwElementSize = *((DWORD*)dwAllocation-3);
		DWORD dwExistingElementType = *((DWORD*)dwAllocation-2);
		DWORD dwTableSizeInBytes = dwExistingSizeOfArray * 4;

		// Can only save pure types
		if(dwExistingElementType<9)
		{
			// Load File for array
			HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile!=INVALID_HANDLE_VALUE)
			{
				// If array string, load in as string table
				if(dwExistingElementType==2)
				{
					// Read Type of Array (element type 2=string)
					DWORD dwDataSize=GetFileSize(hFile, 0);
					LPSTR pData=new char[dwDataSize+2];
					ReadFile(hFile, pData, dwDataSize, &readen, FALSE);
					pData[dwDataSize]=0;
					pData[dwDataSize+1]=0;

					// Scan all lines into array
					int arrindex = 0;
					LPSTR pPtr = pData;
					LPSTR pStart = pPtr;
					LPSTR pPtrEnd = pData + dwDataSize;
					while(pPtr<=pPtrEnd && arrindex<(int)dwExistingSizeOfArray)
					{
						if( (*(pPtr+0)==13 && *(pPtr+1)==10) || *(pPtr+0)==0 )
						{
							DWORD* pEntry = *((DWORD**)dwAllocation+arrindex);
							if(pEntry)
							{
								// Free any existing string
								if(*pEntry) delete[] (LPSTR)(*pEntry);

								// Make string
								LPSTR pNewStr = NULL;
								DWORD dwStringSize=pPtr-pStart;
								pNewStr = new char[dwStringSize+1];
								memcpy(pNewStr, pStart, dwStringSize);
								pNewStr[dwStringSize]=0;

								// New string
								*pEntry = (DWORD)pNewStr;
								arrindex++;
							}

							// Next line
							pStart = pPtr+2;
							pPtr++;
						}
						pPtr++;
					}

					// Free data
					delete[] pData;
				}
				else
				{
					// Read Type of Array (element type 2=string)
					DWORD dwElementType=0;
					ReadFile(hFile, &dwElementType, 4, &readen, FALSE);

					// Read Size of Array (elements)
					DWORD dwSizeOfArray=0;
					ReadFile(hFile, &dwSizeOfArray, 4, &readen, FALSE);

					// Verify corect array loaded into
					if(dwElementType==dwExistingElementType && dwSizeOfArray==dwExistingSizeOfArray)
					{
						// Clear Array of old data
						DWORD dwDataPointer=dwAllocation+dwTableSizeInBytes;
						DWORD dwDataBlockSizeInBytes = dwSizeOfArray * dwElementSize;
						ZeroMemory((LPSTR)dwDataPointer, dwDataBlockSizeInBytes);

						// Load In Array (of x size)
						int arrindex = 0;
						ReadFile(hFile, &arrindex, 4, &readen, FALSE);
						while(arrindex!=-1)
						{
							DWORD* pEntry = *((DWORD**)dwAllocation+arrindex);
							if(pEntry)
							{
								// Value
								ReadFile(hFile, pEntry, dwElementSize, &readen, FALSE);
							}

							// Read ext index
							ReadFile(hFile, &arrindex, 4, &readen, FALSE);
						}
					}
					else
					{
						// runtime not same aray
					}
				}
			
				// Close file
				CloseHandle(hFile);
			}
			else
			{
				// runtime - could not read array file
				RunTimeError(RUNTIMEERROR_FILENOTEXIST);
			}
		}
		else
		{
			//runtime not right type
			RunTimeError(RUNTIMEERROR_ARRAYTYPEINVALID);
		}
	}
}

DARKSDK void LoadArray( LPSTR szFilename, DWORD dwAllocation )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadArrayCore ( VirtualFilename, dwAllocation );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

//
// DX Detect Check (from globstruct filled in DarkEXE)
//

DARKSDK DWORD GetDXVer$(DWORD dwDestStr)
{
	char buf[256];

	buf[0] = '\0';

	if (g_pGlob && g_pGlob->lpDirectXVersionString)
	{
		if (g_pGlob->lpDirectXVersionString[ 0 ])
		{
#if __STDC_WANT_SECURE_LIB__
			strcpy_s(buf, sizeof(buf), g_pGlob->lpDirectXVersionString);
#else
			strncpy(buf, sizeof(buf) - 1, g_pGlob->lpDirectXVersionString);
			buf[sizeof(buf) - 1] = '\0';
#endif
		}
	}

	if (!buf[0])
	{
		// Could get the DX version the 'proper' way using the DXDiag library,
		// except that it always fetches the latest version of DX on the system,
		// ie 10 on Vista, 11 on Windows 7.
		// Luckily those don't overwrite the DX version held in the registry so
		// we can collect the value from there instead.
		// If the exe is running in compatibility mode for XP or prior, then this
		// will never be executed anyway.
		// (TBH, I don't really understand why we even bother as if the required
		//  version is not installed, we won't be able to run the exe anyway...)

		HKEY Key;
		LONG Status;

		Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\DirectX", NULL, KEY_READ, &Key);
		if (Status == ERROR_SUCCESS)
		{
			DWORD DataType = REG_SZ, DataSize = sizeof(buf);

			Status = RegQueryValueEx( Key, "Version", NULL, &DataType, (LPBYTE)buf, &DataSize );
			if (Status != ERROR_SUCCESS)
				buf[0] = 0;
			else
			{
				// Please excuse this rather nasty bodge...
				if (strncmp(buf, "4.09.00.0904", 12) == 0)
					strcpy(buf, "9.0c");
				else if (strncmp(buf, "4.09.00.0903", 11) == 0)
				{
					char Letter = buf[11] - '1' + 'a';
					strcpy(buf, "9.0?");
					buf[3] = Letter;
				}
				else
					buf[0] = 0;
			}

			RegCloseKey( Key );
		}
	}

	if (!buf[0])
		strcpy( buf, "DirectX 9.0 not installed" );

	return reinterpret_cast<DWORD>(dbReturnString(reinterpret_cast<char *>(dwDestStr), buf));
}

//
// Suspend App - used when multiple apps running, want to shut one down
//

DARKSDK void AlwaysActiveOff ( void )
{
	// Will shut down all 3D, sound and music processing (plus any secondary monitoring)
	// Will keep input and general program execution naturally
	g_bAlwaysActiveOff = true;
	g_bAlwaysActiveOneOff = false;
}

DARKSDK void AlwaysActiveOn ( void )
{
	// Restores systems previously shutdown with AlwaysActiveOff
	g_bAlwaysActiveOff = false;
}

DARKSDK void EarlyEnd ( void )
{
	// Report an error
	MessageBox ( NULL, "You have hit a FUNCTION declaration mid-program!", "Early Exit Error", MB_OK );
}

DARKSDK void SyncSleep ( int iFlag )
{
	// controls process friendly flag
	if ( iFlag==1 ) 
		g_bProcessorFriendly = true;
	else
		g_bProcessorFriendly = false;
}

DARKSDK void SyncMask ( DWORD dwMask )
{
	// copy to master sync mask
	g_dwSyncMask = dwMask;
}

DARKSDK DWORD GetArrayType(DWORD dwArrayPtr)
{
	// return array size
	if(dwArrayPtr) 
	{
		DWORD dwTypeIndex = (*((DWORD*)dwArrayPtr-2));
		return dwTypeIndex;
	}
	else
		return 0;
}

LPSTR GetTypePatternCore ( LPSTR dwTypeName, DWORD dwTypeIndex )
{
	// U73 - 210309 - if basic string, return simple STRING pattern
	if ( dwTypeIndex==2 )
	{
		LPSTR pSimplePattern = new char[2];
		strcpy ( pSimplePattern, "S" );
		return pSimplePattern;
	}

	// U73 - 210309 - if no structures, exit now as rest is structure type stuff only
	if ( g_dwStructPatternQty==0 )
		return NULL;

	// look for type that matches name
	DWORD dwPatternDataBeginsAt = 0;
	if ( g_pStructPatternsPtr )
	{
		if ( dwTypeName )
		{
			LPSTR pFindName = new char[strlen((LPSTR)dwTypeName)+2];
			strcpy ( pFindName, (LPSTR)dwTypeName );
			strcat ( pFindName, ":" );
			DWORD dwFindLength = strlen ( pFindName );
			for ( DWORD dwI=0; dwI<g_dwStructPatternQty-dwFindLength; dwI++ )
			{
				if ( strnicmp ( g_pStructPatternsPtr+dwI, pFindName, dwFindLength )==NULL )
				{
					dwPatternDataBeginsAt = dwI+dwFindLength;
					break;
				}
			}
			delete[] pFindName;
		}
		if ( dwTypeIndex>0 )
		{
			LPSTR pFindName = new char[g_dwStructPatternQty+1];
			wsprintf ( pFindName, ":%d:", dwTypeIndex );
			DWORD dwFindLength = strlen ( pFindName );
			for ( DWORD dwI=0; dwI<g_dwStructPatternQty-dwFindLength; dwI++ )
			{
				if ( strnicmp ( g_pStructPatternsPtr+dwI, pFindName, dwFindLength )==NULL )
				{
					dwPatternDataBeginsAt = dwI+dwFindLength;
					break;
				}
			}
			delete[] pFindName;
		}
	}

	// copy pattern to return string, or null
	LPSTR lpNewStr = new char[(strlen(g_pStructPatternsPtr)-dwPatternDataBeginsAt)+1];
	if ( dwPatternDataBeginsAt > 0 )
	{
		// get type index, then go to get pattern
		if ( dwTypeName )
		{
			LPSTR lpNum = new char[(strlen(g_pStructPatternsPtr)-dwPatternDataBeginsAt)+1];
			LPSTR pSourceStr = g_pStructPatternsPtr + dwPatternDataBeginsAt;
			strcpy ( lpNum, pSourceStr );
			DWORD dwI = 0;
			for (; dwI<strlen(pSourceStr); dwI++ )
			{
				if ( lpNum[dwI]==':' )
				{
					lpNum[dwI]=0;
					break;
				}
			}
			delete[] lpNum;
			dwPatternDataBeginsAt += dwI + 1;
		}

		// get pattern, then cut off at : colon
		LPSTR pSourceStr = g_pStructPatternsPtr + dwPatternDataBeginsAt;
		strcpy ( lpNewStr, pSourceStr );
		for ( DWORD dwI=0; dwI<strlen(pSourceStr); dwI++ )
		{
			if ( lpNewStr[dwI]==':' )
			{
				lpNewStr[dwI]=0;
				break;
			}
		}
	}
	else
		strcpy ( lpNewStr, "" );

	// return pattern from type found
	return lpNewStr;
}

DARKSDK DWORD GetTypePattern$(DWORD dwDestStr,DWORD dwTypeName,DWORD dwTypeIndex)
{
	DWORD r;

	// determine if type name string passed in has contents
	LPSTR pTypeName = NULL;
	if ( dwTypeName )
		if ( strlen ( (LPSTR)dwTypeName ) > 0 )
			pTypeName = (LPSTR)dwTypeName;

	// get pattern from type name
	LPSTR lpNewStr = GetTypePatternCore( pTypeName, dwTypeIndex );

	r = reinterpret_cast<DWORD>(dbReturnString(reinterpret_cast<char *>(dwDestStr), lpNewStr));
	delete [] lpNewStr;

	return r;

	/*
	// return pattern from type found
	if((DWORD*)dwDestStr) delete[] (LPSTR)(DWORD*)dwDestStr;
	return (DWORD)lpNewStr;
	*/
}




/*
 *
 * Additions for plug-ins only
 *
 */

// Get/Set data pointers
DARKSDK void GetDataPointers(LPSTR* Start, LPSTR* End, LPSTR* Current)
{
	if (Start)      *Start      = g_pDataLabelStart;
	if (End)        *End        = g_pDataLabelEnd;
	if (Current)    *Current    = g_pDataLabelPtr;
}

DARKSDK void SetDataPointer(LPSTR Current)
{
	if (Current < g_pDataLabelStart)
		Current = g_pDataLabelStart;
	if (Current > g_pDataLabelEnd)
		Current = g_pDataLabelEnd;
	g_pDataLabelPtr = Current;
}

