#ifndef _CINPUT_H_
#define _CINPUT_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#define DIRECTINPUT_VERSION 0x0800
#pragma comment ( lib, "dinput8.lib" )
#include <dinput.h>

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
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define KEYDOWN(name, key) (name[key] & 0x80) 

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void			SetupKeyboardEx				( DWORD dwForeOrBackGround );
DARKSDK void 			SetupKeyboard      			( void );
DARKSDK void			SetupMouseEx				( DWORD dwForeOrBackGround );
DARKSDK void 			SetupMouse         			( void );
DARKSDK void 			SetupJoystick      			( void );
DARKSDK void 			SetupForceFeedback 			( void );

DARKSDK void 			UpdateKeyboard     			( void );
DARKSDK void 			UpdateMouse					( void );

DARKSDK BOOL CALLBACK	EnumerateJoysticksCallback	( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
DARKSDK BOOL CALLBACK	EnumAxesCallback			( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

DARKSDK char			InKey      					( void );
DARKSDK void			ClearData 					( void );

#ifdef DARKSDK_COMPILE
		void			ConstructorInput 			( HINSTANCE );
		void 			DestructorInput  			( void );
		void 			SetErrorHandlerInput		( LPVOID pErrorHandlerPtr );
		void 			PassCoreDataInput			( LPVOID pGlobPtr );
		void 			RefreshD3DInput 			( int iMode );
#endif

DARKSDK void 			Constructor 				( HINSTANCE );
DARKSDK void 			Destructor  				( void );
DARKSDK void 			SetErrorHandler 			( LPVOID pErrorHandlerPtr );
DARKSDK void 			PassCoreData				( LPVOID pGlobPtr );
DARKSDK void 			RefreshD3D 					( int iMode );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// entry commands
DARKSDK void 			ClearEntryBuffer 					( void );					// clear holding buffer
DARKSDK DWORD			GetEntryEx							( DWORD pDestStr, int iAutoBackSpaceMode );
DARKSDK DWORD 			GetEntry 							( DWORD pDestStr );			// get from windows entry buffer

// keyboard commands
DARKSDK int 			UpKey      							( void );					// is the up key being pressed
DARKSDK int 			DownKey    							( void );					// is the down key being pressed
DARKSDK int 			LeftKey   							( void );					// is the left key being pressed
DARKSDK int 			RightKey   							( void );					// is the right key being pressed
DARKSDK int 			ControlKey 							( void );					// is the control key being pressed
DARKSDK int 			ShiftKey   							( void );					// is the shift key being pressed
DARKSDK int 			SpaceKey   							( void );					// is the space key being pressed
DARKSDK int 			EscapeKey  							( void );					// is the escape key being pressed
DARKSDK int 			ReturnKey  							( void );					// is the return key being pressed
DARKSDK int 			KeyState   							( int iKey );				// return true if the key is being pressed

// mike - 230107 - new function to get state, useful for caps lock etc
DARKSDK int				GetKeyStateEx						( int iKey );

DARKSDK int 			ScanCode   							( void );					// return the scan code of the current key press

// misc commands
DARKSDK void 			WriteToClipboard					( LPSTR );				// write to clkipboard
DARKSDK void 			SetRegistryHKEY						( int );
DARKSDK void 			WriteToRegistry						( LPSTR, LPSTR, int );		// write to registry
DARKSDK DWORD			GetClipboard						( DWORD );					// get from clipboard
DARKSDK int 			GetRegistry							( LPSTR, LPSTR );				// get from registry
DARKSDK void 			WriteToRegistryS					( LPSTR pfolder, LPSTR valuekey, DWORD pString );
DARKSDK void 			WriteToRegistrySL					( LPSTR pfolder, LPSTR valuekey, DWORD pString, int iCurrentUserMode );
DARKSDK DWORD 			GetRegistryS						( DWORD pDestStr, LPSTR pfolder, LPSTR valuekey );
DARKSDK DWORD 			GetRegistrySL						( DWORD pDestStr, LPSTR pfolder, LPSTR valuekey, int iCurrentUserMode );

// mouse commands
DARKSDK void 			HideMouse     						( void );	// hide
DARKSDK void 			ShowMouse     						( void );	// show
DARKSDK void 			PositionMouse 						( int, int );// position
DARKSDK void 			ChangeMouse   						( int );		// change
DARKSDK void			ChangeMouse							( int, int );	// 13/01/11 - touch friendly flag
DARKSDK int  			GetMouseX     						( void );	// get x position
DARKSDK int  			GetMouseY     						( void );	// get y position
DARKSDK int  			GetMouseZ     						( void );	// get z position ( mouse wheel )
DARKSDK int  			GetMouseClick 						( void );	// get which mouse button has been clicked
DARKSDK int  			GetMouseMoveX 						( void );	// get the relative x movement
DARKSDK int  			GetMouseMoveY 						( void );	// get the relative y movement
DARKSDK int  			GetMouseMoveZ 						( void );	// get the relative z movement

// joystick commands
DARKSDK int 			JoystickUp 							( void );
DARKSDK int 			JoystickDown 						( void );
DARKSDK int 			JoystickLeft 						( void );
DARKSDK int 			JoystickRight 						( void );
DARKSDK int 			JoystickX 							( void );
DARKSDK int 			JoystickY 							( void );
DARKSDK int 			JoystickZ 							( void );
DARKSDK int 			JoystickFireA 						( void );
DARKSDK int 			JoystickFireB 						( void );
DARKSDK int 			JoystickFireC 						( void );
DARKSDK int 			JoystickFireD 						( void );
DARKSDK int 			JoystickFireXL 						( int iButton );
DARKSDK int 			JoystickSliderA 					( void );
DARKSDK int 			JoystickSliderB 					( void );
DARKSDK int 			JoystickSliderC 					( void );
DARKSDK int 			JoystickSliderD 					( void );
DARKSDK int 			JoystickTwistX 						( void );
DARKSDK int 			JoystickTwistY 						( void );
DARKSDK int 			JoystickTwistZ 						( void );
DARKSDK int 			JoystickHatAngle 					( int iHatID );

// force commands
DARKSDK void 			ForceUp 							( int iMagnitude );
DARKSDK void 			ForceDown							( int iMagnitude );
DARKSDK void 			ForceLeft 							( int iMagnitude );
DARKSDK void 			ForceRight 							( int iMagnitude );
DARKSDK void 			ForceAngle							( int iMagnitude, int iAngle, int iDuration );
DARKSDK void 			ForceChainsaw						( int iMagnitude, int iDuration );
DARKSDK void 			ForceShoot							( int iMagnitude, int iDuration );
DARKSDK void 			ForceImpact							( int iMagnitude, int iDuration );
DARKSDK void 			ForceNoEffect						( void );
DARKSDK void 			ForceWaterEffect					( int iMagnitude, int iDuration );
DARKSDK void 			ForceAutoCenterOn					( void );
DARKSDK void 			ForceAutoCenterOff					( void );

// control device commands
DARKSDK void 			PerformChecklistControlDevices		( void );
DARKSDK void			SetControlDeviceEx					( DWORD pName, int iSubIndex );
DARKSDK void 			SetControlDevice					( DWORD pName );
DARKSDK void			SetControlDeviceIndex				( int iIndex );
DARKSDK DWORD 			GetControlDevice 					( DWORD pDestStr );
DARKSDK int 			ControlDeviceX						( void );
DARKSDK int 			ControlDeviceY						( void );
DARKSDK int 			ControlDeviceZ						( void );

#ifdef DARKSDK_COMPILE
		void 			dbClearEntryBuffer 					( void );
		char* 			dbGetEntry 							( void );

		int 			dbUpKey      						( void );
		int 			dbDownKey    						( void );
		int 			dbLeftKey   						( void );
		int 			dbRightKey   						( void );
		int 			dbControlKey 						( void );
		int 			dbShiftKey   						( void );
		int 			dbSpaceKey   						( void );
		int 			dbEscapeKey  						( void );
		int 			dbReturnKey  						( void );
		int 			dbKeyState   						( int iKey );
		int 			dbScanCode   						( void );

		void 			dbWriteToClipboard					( char* pString );
		void 			dbWriteToRegistry					( char* a, char* b, int c );
		char*			dbGetClipboard						( void );
		int 			dbGetRegistry						( LPSTR, LPSTR );
		// mike - 220107 - update last param of function
		void 			dbWriteToRegistryS					( LPSTR pfolder, LPSTR valuekey, char* pString );
		char* 			dbGetRegistryS						( LPSTR pfolder, LPSTR valuekey );

		void 			dbHideMouse     					( void );
		void 			dbShowMouse     					( void );
		void 			dbPositionMouse 					( int, int );
		void 			dbChangeMouse   					( int );
		int  			dbMouseX     						( void );
		int  			dbMouseY     						( void );
		int  			dbMouseZ     						( void );
		int  			dbMouseClick 						( void );
		int  			dbMouseMoveX 						( void );
		int  			dbMouseMoveY 						( void );
		int  			dbMouseMoveZ 						( void );

		int 			dbJoystickUp 						( void );
		int 			dbJoystickDown 						( void );
		int 			dbJoystickLeft 						( void );
		int 			dbJoystickRight 					( void );
		int 			dbJoystickX 						( void );
		int 			dbJoystickY 						( void );
		int 			dbJoystickZ 						( void );
		int 			dbJoystickFireA 					( void );
		int 			dbJoystickFireB 					( void );
		int 			dbJoystickFireC 					( void );
		int 			dbJoystickFireD 					( void );
		int 			dbJoystickFireXL 					( int iButton );
		int 			dbJoystickSliderA 					( void );
		int 			dbJoystickSliderB 					( void );
		int 			dbJoystickSliderC 					( void );
		int 			dbJoystickSliderD 					( void );
		int 			dbJoystickTwistX 					( void );
		int 			dbJoystickTwistY 					( void );
		int 			dbJoystickTwistZ 					( void );
		int 			dbJoystickHatAngle 					( int iHatID );

		void 			dbForceUp 							( int iMagnitude );
		void 			dbForceDown							( int iMagnitude );
		void 			dbForceLeft 						( int iMagnitude );
		void 			dbForceRight 						( int iMagnitude );
		void 			dbForceAngle						( int iMagnitude, int iAngle, int iDuration );
		void 			dbForceChainsaw						( int iMagnitude, int iDuration );
		void 			dbForceShoot						( int iMagnitude, int iDuration );
		void 			dbForceImpact						( int iMagnitude, int iDuration );
		void 			dbForceNoEffect						( void );
		void 			dbForceWaterEffect					( int iMagnitude, int iDuration );
		void 			dbForceAutoCenterOn					( void );
		void 			dbForceAutoCenterOff				( void );

		void 			dbPerformChecklistControlDevices	( void );
		void 			dbSetControlDevice					( char* pName );
		char* 			dbGetControlDevice 					( void );
		int 			dbControlDeviceX					( void );
		int 			dbControlDeviceY					( void );
		int 			dbControlDeviceZ					( void );
		void			dbClearDataInput 					( void );

		// lee - 300706 - GDK fixes
		char* 			dbEntry 							( void );
		void 			dbWriteStringToRegistry				( LPSTR pfolder, LPSTR valuekey, DWORD pString );
		char* 			dbGetRegistryString					( LPSTR pfolder, LPSTR valuekey );
		int 			dbJoystickFireX 					( int iButton );

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif _CINPUT_H_