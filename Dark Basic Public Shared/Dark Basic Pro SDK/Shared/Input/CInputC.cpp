//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE
#include "cinputc.h"
#include "controller.h"
#include ".\..\..\Shared\Error\cerror.h"
#include ".\..\..\Shared\Core\globstruct.h"
#include "time.h"

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// shared data segment
DBPRO_GLOBAL LPDIRECTINPUT8			m_lpDI				= NULL;			// direct input interface
DBPRO_GLOBAL LPDIRECTINPUTDEVICE8	m_lpDIKeyboard      = NULL;			// keyboard interface
DBPRO_GLOBAL LPDIRECTINPUTDEVICE8	m_lpDIMouse         = NULL;			// mouse interface

// U73 - 210309 - support for multiple controllers (upto 8)
#define								CONTROLDEVICEMAX					8
DBPRO_GLOBAL int					m_iCDI								= 0;	// control device selector (zero based index) 
DBPRO_GLOBAL CController	        m_lpDIControlDevice [ CONTROLDEVICEMAX ];	// control device

// global variables
DBPRO_GLOBAL unsigned char			m_KeyBuffer [ 256 ];				// keyboard input buffer
DBPRO_GLOBAL DIMOUSESTATE			m_MouseBuffer;						// mouse data
DBPRO_GLOBAL DIJOYSTATE2 			m_JoyStickBuffer;					// joystick data
DBPRO_GLOBAL DIJOYSTATE2 			m_ControlDeviceBuffer;				// control device data
DBPRO_GLOBAL DIDEVCAPS				m_JoyStickCaps;
DBPRO_GLOBAL unsigned char			m_KeyScanCodes [ ] =				// key scan code array
						{
							// numbers on main keyboard
							DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9,
							
							// main alphabet
							DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J,
							DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T,
							DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z,

							// symbols
							DIK_ADD,      DIK_APOSTROPHE, DIK_BACKSLASH, DIK_COMMA,     DIK_DECIMAL,
							DIK_DIVIDE,   DIK_EQUALS,     DIK_GRAVE,     DIK_LBRACKET,  DIK_MINUS,
							DIK_MULTIPLY, DIK_PERIOD,     DIK_RBRACKET,  DIK_SEMICOLON, DIK_SLASH,
							DIK_SPACE,    DIK_SUBTRACT,

							// numpad
							DIK_NUMPAD0, DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4,
							DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9,
						};

DBPRO_GLOBAL char					m_KeyCharacters [ ] =			// key characters
						{
							// numbers on main keyboard
							'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

							// main alphabet
							'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
							'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
							'u', 'v', 'w', 'x', 'y', 'z',

							// symbols
							'+',  '\'', '\\', ',', '.',
							'\\', '=',  '\'', '[', '-',
							'*',  '.',  ']',  ',', '/',
							' ',  '-',

							// numpad
							'0', '1', '2', '3', '4', 
							'5', '6', '7', '8', '9',
						};
DBPRO_GLOBAL bool m_GetKeyStateToggle[ 256 ] = { false };

// Globals for Joystick/Controllers
//char		gJoystickNameUsed[256];
DBPRO_GLOBAL char		gFindController[256];							// store controller name
DBPRO_GLOBAL int		giChoseSameControllerByIndex;					// used to skip same-name controllers
DBPRO_GLOBAL HKEY		g_hkeyRegistry						= HKEY_LOCAL_MACHINE;

// Global settings
DBPRO_GLOBAL HWND		g_phWnd								= NULL;		// handle to window
DBPRO_GLOBAL DWORD*		g_pWindowsEntryString				= NULL;		// passed in from window core
DBPRO_GLOBAL int			g_iMouseLocalZ						= 0;
DBPRO_GLOBAL int			g_iMouseDeltaX						= 0;
DBPRO_GLOBAL int			g_iMouseDeltaY						= 0;
DBPRO_GLOBAL int			g_iMouseDeltaZ						= 0;

DBPRO_GLOBAL GlobStruct*					g_pGlob								= NULL;					// glob struct

DBPRO_GLOBAL PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;

DBPRO_GLOBAL int			g_iTouchFriendly					= 0;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( HINSTANCE hInstance )
{
	// setup the input library

	// variable declarations
	HRESULT		hr;		// used to check error codes
	
	// clear out the keyboard buffer
	memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );

	// setup the direct input interface
	if ( FAILED ( hr = DirectInput8Create ( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, ( void** ) &m_lpDI, NULL ) ) )
	{
		MessageBox ( NULL, "Unable to create DirectInput interface", "Unable to create DirectInput interface", MB_OK );
		Error ( "Unable to create DirectInput interface" );
		return;
	}

	// No Controller as default
	m_iCDI = 0; // U73 - 210309 - support for multiple controllers

	strcpy(gFindController, "");
	giChoseSameControllerByIndex = 0;

    // Init now done using PassCoreData(...)
}

DARKSDK void ClearData ( void )
{
	/*
	g_iMouseLocalZ	= 0;
	g_iMouseDeltaX	= 0;
	g_iMouseDeltaY	= 0;
	g_iMouseDeltaZ	= 0;
	UpdateMouse ( );
	*/
}

DARKSDK void FreeControlDevice ( void )
{
	// Release any Controller Device
	m_iCDI = 0; // U73 - 210309 - support for multiple controllers
	for ( int n=0; n<CONTROLDEVICEMAX; n++ )
	{
		m_lpDIControlDevice[n].Release();
	}
}

DARKSDK void FreeDevices( void )
{
	// control device (force feedback, etc)
	FreeControlDevice();

	// keyboard
    if ( m_lpDIKeyboard )
    {
        m_lpDIKeyboard->Unacquire ( );
        m_lpDIKeyboard->Release   ( );
		m_lpDIKeyboard = NULL;
    }

	// mouse
	if ( m_lpDIMouse )
    {
        m_lpDIMouse->Unacquire ( );
        m_lpDIMouse->Release   ( );
		m_lpDIMouse = NULL;
    }
}

DARKSDK void Destructor ( void )
{
	// release all created devices
	if ( m_lpDI )
    {
		// Free devices
		FreeDevices();

		// now release interface
        m_lpDI->Release ( );
		m_lpDI = NULL; 
    }
}

DARKSDK void RefreshD3D ( int iMode )
{
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
	g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;
	g_pWindowsEntryString = (DWORD*)&g_pGlob->pWindowsTextEntry;
	g_phWnd = g_pGlob->hWnd;
	g_iMouseLocalZ = 0;

	// new HWND, so new setups
	FreeDevices();
	SetupKeyboard      ( );		// keyboard
	SetupMouse         ( );		// mouse
	SetupForceFeedback ( );		// force feedback
}

//
// Setups
//

DARKSDK void SetupKeyboardEx ( DWORD dwForeOrBackGround )
{
	// setup the keyboard
	if ( dwForeOrBackGround==1 ) dwForeOrBackGround=DISCL_BACKGROUND; else dwForeOrBackGround=DISCL_FOREGROUND;

	// variable declarations
	HRESULT hr;		// used to check return codes

	// create the device
	if ( FAILED ( hr = m_lpDI->CreateDevice ( GUID_SysKeyboard, &m_lpDIKeyboard, NULL ) ) )
	{
		// if it fails call the destructor and show an error message
		Destructor ( );
		Error ( "Unable to access keyboard for input library" );
	}
	
	// set the data format
	if ( FAILED ( hr = m_lpDIKeyboard->SetDataFormat ( &c_dfDIKeyboard ) ) )
	{
		// if it fails call the destructor and show an error message
		Destructor ( );
		Error ( "Failed to set data format for keyboard in input library" );
	}
	
	// request foregound, non exclusive and disable windows key DISCL_BACKGROUND
//	if ( FAILED ( hr = m_lpDIKeyboard->SetCooperativeLevel ( g_phWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_NOWINKEY ) ) )
	if ( FAILED ( hr = m_lpDIKeyboard->SetCooperativeLevel ( g_phWnd, dwForeOrBackGround | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_NOWINKEY ) ) )
//	if ( FAILED ( hr = m_lpDIKeyboard->SetCooperativeLevel ( g_phWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) //| DISCL_NOWINKEY ) ) )
	{
		// if it fails call the destructor and show an error message
		Destructor ( );
		Error ( "Failed to set cooperative level for keyboard in input library" );
	}

	// now acquire the device, we may not always be able to acquire
	// so we don't need to check for an error here
	if ( m_lpDIKeyboard )
		m_lpDIKeyboard->Acquire ( );
}

DARKSDK void SetupKeyboard ( void )
{
	SetupKeyboardEx ( DISCL_FOREGROUND );
}

DARKSDK void SetupMouseEx ( DWORD dwForeOrBackGround )
{
	// setup the mouse, this is an essential
	if ( dwForeOrBackGround==1 ) dwForeOrBackGround=DISCL_BACKGROUND; else dwForeOrBackGround=DISCL_FOREGROUND;

	// device and must be present
	HRESULT hr = NULL;

	// create the device
	if ( FAILED ( hr = m_lpDI->CreateDevice ( GUID_SysMouse, &m_lpDIMouse, NULL ) ) )
	{
		Destructor ( );
		Error ( "Unable to access mouse for input library" );
	}

	// request background and non exclusive mode
	//if ( FAILED ( hr = m_lpDIMouse->SetCooperativeLevel ( g_phWnd, DISCL_NONEXCLUSIVE | dwForeOrBackGround ) ) )
	if ( FAILED ( hr = m_lpDIMouse->SetCooperativeLevel ( g_phWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND ) ) )
	{
		Destructor ( );
		Error ( "Failed to set cooperative level for mouse in input library" );
	}

	// set the data format
	if ( FAILED ( hr = m_lpDIMouse->SetDataFormat ( &c_dfDIMouse ) ) )
	{
		if ( FAILED ( hr = m_lpDIMouse->SetDataFormat ( &c_dfDIMouse2 ) ) )
		{
			// 091013 - do not kill input device - just silently ignore this attempt Destructor ( );
			Error ( "Failed to set data format for mouse in input library" );
		}
	}

	// now acquire the device
	if ( m_lpDIMouse )
		m_lpDIMouse->Acquire ( );
}

DARKSDK void SetupMouse ( void )
{
	SetupMouseEx ( DISCL_FOREGROUND );
}

DARKSDK HRESULT SetDIDwordProperty(LPDIRECTINPUTDEVICE8 pDev, REFGUID guidProperty, DWORD dwObject, DWORD dwHow, DWORD dwValue)
{
   DIPROPDWORD dipdw;
   dipdw.diph.dwSize       = sizeof(dipdw);
   dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
   dipdw.diph.dwObj        = dwObject;
   dipdw.diph.dwHow        = dwHow;
   dipdw.dwData            = dwValue;
   return pDev->SetProperty(guidProperty, &dipdw.diph);
}

DARKSDK BOOL CALLBACK EnumAxesCallback ( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	return DIENUM_STOP;
}

DARKSDK BOOL CALLBACK EnumerateControllerCallback(LPDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
	HRESULT					hRes;
	LPDIRECTINPUTDEVICE8	lpdid8Game;  

	// lee - 200206 - u60 - search index used to determine controller choice of same-name
	int iSearchIndex = 0;
	if ( pvRef )
	{
		iSearchIndex = *(int*)pvRef;
		*(int*)pvRef = (*(int*)pvRef) + 1;
	}

	// must match required name
	if(strcmp(gFindController, "")!=NULL)
		if(strcmp(pdinst->tszProductName, gFindController)!=0)
			return DIENUM_CONTINUE;

	// matches name, but ensure search index is correct
	// U73 - 210309 - added !=0 so giChoseSameControllerByIndex of zero ignores index selection altogether!
	if ( giChoseSameControllerByIndex!=0 )
		if ( iSearchIndex != giChoseSameControllerByIndex )
			return DIENUM_CONTINUE;

	// create the DirectInput joystick device 
	hRes = m_lpDI->CreateDevice(pdinst->guidInstance, &lpdid8Game, NULL);
	if(FAILED(hRes))
		return DIENUM_CONTINUE;  

	// set joystick behaviour
	//hRes = lpdid8Game->SetCooperativeLevel(g_phWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
	hRes = lpdid8Game->SetCooperativeLevel(g_phWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if(FAILED(hRes))
	{ 
		lpdid8Game->Release();      
		return DIENUM_CONTINUE;   
	} 

	// set data format
	hRes = lpdid8Game->SetDataFormat(&c_dfDIJoystick2);
	if(FAILED(hRes))
	{ 
		lpdid8Game->Release();      
		return DIENUM_CONTINUE;    
	}  

	// set property for axis range
	DIPROPRANGE diprg;   
	diprg.diph.dwSize       = sizeof(diprg); 
	diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
	diprg.diph.dwObj        = DIJOFS_X; 
	diprg.diph.dwHow        = DIPH_BYOFFSET;  
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000;  
	hRes = lpdid8Game->SetProperty(DIPROP_RANGE, &diprg.diph);  
	diprg.diph.dwObj        = DIJOFS_Y; 
	hRes = lpdid8Game->SetProperty(DIPROP_RANGE, &diprg.diph);  
	diprg.diph.dwObj        = DIJOFS_Z; 
	hRes = lpdid8Game->SetProperty(DIPROP_RANGE, &diprg.diph);  

	// Switch off auto-center - FF actions cancel it later on :(*
	DIPROPDWORD DIPropAutoCenter; 
	DIPropAutoCenter.diph.dwSize = sizeof(DIPropAutoCenter);
	DIPropAutoCenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DIPropAutoCenter.diph.dwObj = 0;
	DIPropAutoCenter.diph.dwHow = DIPH_DEVICE;
	DIPropAutoCenter.dwData = FALSE; 
	hRes = lpdid8Game->SetProperty(DIPROP_AUTOCENTER, &DIPropAutoCenter.diph);

	// Release any previous controllers
	// Aquire successful device
	m_lpDIControlDevice[m_iCDI].Acquire( lpdid8Game ); 

	// copy found device to found-name
	strcpy(gFindController, pdinst->tszProductName);
	//giChoseSameControllerByIndex = iSearchIndex; //210309 - do not interfere with selection by NAME if giChoseSameControllerByIndex is zero!
	if ( giChoseSameControllerByIndex!=0 ) giChoseSameControllerByIndex = iSearchIndex;

    // Stop after the first successful device has been found and created
    return DIENUM_STOP;
}

DARKSDK BOOL CALLBACK DIEnumEffectsProc(LPCDIEFFECTINFO pei, LPVOID pv)
{
	*((GUID *)pv) = pei->guid;
	return DIENUM_STOP;
}

DARKSDK bool DB_CreateFFDirectionEffect(short id, int magnitude, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].DirectionEffect(id, magnitude, bStart);
    return false;
}




DARKSDK bool DB_CreateFFAngleEffect(int magnitude, int angle, int delay, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].AngleEffect(magnitude, angle, delay, bStart);
    return false;
}

DARKSDK bool DB_CreateFFChainsawEffect(int magnitude, int delay, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].ChainsawEffect(magnitude, delay, bStart);
    return false;
}

DARKSDK bool DB_CreateFFShootEffect(int magnitude, int delay, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].ShootEffect(magnitude, delay, bStart);
    return false;
}

DARKSDK bool DB_CreateFFImpactEffect(int magnitude, int delay, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].ImpactEffect(magnitude, delay, bStart);
    return false;
}

DARKSDK bool DB_CreateFFWaterEffect(int magnitude, int delay, bool bStart)
{
    if (m_lpDIControlDevice[m_iCDI])
        return m_lpDIControlDevice[m_iCDI].WaterEffect(magnitude, delay, bStart);
    return false;
}

DARKSDK bool DB_SelectNewControlDevice(void)
{
	// 091013 - previous mouse init call can DESTROY input ptr!!
	if ( m_lpDI )
	{
		// Release any previous controllers
		m_lpDIControlDevice[m_iCDI].Release();

		// Emumerate for the name (with forcefeedback)
		int iSearchIndex = 0;
		m_lpDI->EnumDevices ( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumerateControllerCallback, (LPVOID)&iSearchIndex, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
		if(m_lpDIControlDevice[m_iCDI])
		{
			// New Controller Device
			m_lpDIControlDevice[m_iCDI].SetName(gFindController);

			// Got ForceFeedback
			m_lpDIControlDevice[m_iCDI].SetForceFeedback();
			return true;
		}
		else
		{
			// Emumerate for the name : 'gFindController' (without FF)
			int iSearchIndex = 0;
			m_lpDI->EnumDevices ( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumerateControllerCallback, (LPVOID)&iSearchIndex, DIEDFL_ATTACHEDONLY);
			if(m_lpDIControlDevice[m_iCDI])
			{
				// New Controller Device
				m_lpDIControlDevice[m_iCDI].SetName(gFindController);

				return true;
			}
		}
	}
	return false;
}

DARKSDK void DB_StopFFEffect()
{
    if (m_lpDIControlDevice[m_iCDI])
        m_lpDIControlDevice[m_iCDI].StopEffect();
}

DARKSDK void DB_AutoCenter(bool AutoCenterOn )
{
    if (m_lpDIControlDevice[m_iCDI])
        m_lpDIControlDevice[m_iCDI].AutoCenter( AutoCenterOn );
}

DARKSDK void SetupForceFeedback ( void )
{
	// Sets up default control device for any force feedback or normal joystick
	strcpy(gFindController, "");
	giChoseSameControllerByIndex = 0;
	DB_SelectNewControlDevice();
}

DBPRO_GLOBAL bool g_bCreateChecklistNow=false;
DBPRO_GLOBAL DWORD g_dwMaxStringSizeInEnum=0;//must be done this way as cannot create dynamic mem inside a DX enum

DARKSDK BOOL CALLBACK ChecklistInputControllers(LPDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
	LPDIRECTINPUT			pdi = (LPDIRECTINPUT)pvRef;   
	char					productname[256];

	// Convert wide name to char name
	strcpy(productname, pdinst->tszProductName);

	// Ensure checklist item prepared
	DWORD dwLengthOfName=strlen(productname);
	if(g_dwMaxStringSizeInEnum<dwLengthOfName) g_dwMaxStringSizeInEnum=dwLengthOfName;

	// Reset Capability Flags
	if(g_bCreateChecklistNow)
	{
		g_pGlob->checklist[g_pGlob->checklistqty].valuea=0;
		g_pGlob->checklist[g_pGlob->checklistqty].valueb=0;
		g_pGlob->checklist[g_pGlob->checklistqty].valuec=0;
		g_pGlob->checklist[g_pGlob->checklistqty].valued=0;

		// Record found controller and move on
		g_pGlob->checklist[g_pGlob->checklistqty].dwStringSize = strlen(productname);
		strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, productname);

        // Look for matching names.
        // Count them to provide a device index
        int Count = 0;
        for (int i = 0; i < g_pGlob->checklistqty; ++i)
        {
            if (strcmp(productname, g_pGlob->checklist[i].string) == 0)
            {
                ++Count;
            }
        }
        g_pGlob->checklist[g_pGlob->checklistqty].valueb = Count;
	}
	g_pGlob->checklistqty++;

	// Until no more..
    return DIENUM_CONTINUE;
}

DARKSDK BOOL CALLBACK ChecklistAddFFValueFlag(LPDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
	LPDIRECTINPUT			pdi = (LPDIRECTINPUT)pvRef;   
	char					productname[256];

	// Convert wide name to char name
	strcpy(productname, pdinst->tszProductName);

	for(int ffi=0; ffi<g_pGlob->checklistqty; ffi++)
	{
		if(strcmp(g_pGlob->checklist[ffi].string, productname)==0)
		{
			// This device is Forcefeedback also!
			g_pGlob->checklist[ffi].valuea=1;
		}
	}

	// Until no more..
    return DIENUM_CONTINUE;
}

//
// Update Functions
//

DARKSDK void UpdateKeyboard ( void )
{
	bool bInvalid=true;

	HRESULT  hr;
	if ( FAILED ( hr = m_lpDIKeyboard->GetDeviceState ( sizeof ( m_KeyBuffer ), ( LPVOID ) &m_KeyBuffer ) ) )
	{
		// the device has probably been lost if the
		// get device state has failed, attempt to
		// reacquire it
		if ( hr == DIERR_INPUTLOST || hr != S_OK )
		{
			if ( m_lpDIKeyboard )
			{
				hr = m_lpDIKeyboard->Acquire ( );
			}
		}
	}
	else
		bInvalid=false;

	if(bInvalid)
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
	}
}

DARKSDK void UpdateMouse ( void )
{
	bool bInvalid=true;

	HRESULT  hr;
    if ( FAILED ( hr = m_lpDIMouse->GetDeviceState ( sizeof ( m_MouseBuffer ), ( LPVOID ) &m_MouseBuffer ) ) )
	{
		// the device has probably been lost if the
		// get device state has failed, attempt to
		// reacquire it
		if ( hr == DIERR_INPUTLOST || hr != S_OK )
		{
			if ( m_lpDIMouse )
				hr = m_lpDIMouse->Acquire ( );

			// Get capture data
			if ( SUCCEEDED ( hr = m_lpDIMouse->GetDeviceState ( sizeof ( m_MouseBuffer ), ( LPVOID ) &m_MouseBuffer ) ) )
				bInvalid=false;
		}
	}
	else
		bInvalid=false;

	if(bInvalid)
	{
		memset ( &m_MouseBuffer, 0, sizeof ( m_MouseBuffer ) );
		return;
	}

	// Accumilate Deltas
	g_iMouseDeltaX+=m_MouseBuffer.lX;
	g_iMouseDeltaY+=m_MouseBuffer.lY;
	g_iMouseDeltaZ+=m_MouseBuffer.lZ;
	g_iMouseLocalZ = g_iMouseLocalZ + m_MouseBuffer.lZ;
}

DARKSDK void UpdateControlDevice ( void )
{
	if(m_lpDIControlDevice[m_iCDI])
	{
        m_lpDIControlDevice[m_iCDI].Poll( m_ControlDeviceBuffer );
        return;
	}

	// Clear control device flags if not aquired
	ZeroMemory(&m_ControlDeviceBuffer, sizeof(DIJOYSTATE2));
}

DARKSDK void UpdateJoystick ( void )
{
	UpdateControlDevice();
	memcpy(&m_JoyStickBuffer, &m_ControlDeviceBuffer, sizeof(DIJOYSTATE2));
	return;
}

//
// NonDBPro Commands
//

DARKSDK char InKey ( void )
{
	// return the character of the current key
	int iTemp = 0;

	UpdateKeyboard ( );

	for ( iTemp = 0; iTemp < sizeof ( m_KeyScanCodes ) / sizeof ( char ); iTemp++ )
	{
		if ( KEYDOWN ( m_KeyBuffer, m_KeyScanCodes [ iTemp ] ) )
		{
			memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
			return m_KeyCharacters [ iTemp ];
		}
	}

	return -1;
}


//
// Command Functions
//

DARKSDK void ClearEntryBuffer ( void )
{
	// Clear Windows String
	if((LPSTR)*g_pWindowsEntryString) strcpy((LPSTR)*g_pWindowsEntryString,"");
}

DARKSDK DWORD GetEntryEx ( DWORD pDestStr, int iAutoBackSpaceMode )
{
	// Free old string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

	// lee - 110206 - prepare alternate string if auto-backspace-mode
	LPSTR pRefStr = (LPSTR)*g_pWindowsEntryString;
	LPSTR pTempStr = NULL;
	if ( iAutoBackSpaceMode==1 && pRefStr )
	{
		// go through insert/backspace history of entry$() string to make final string result
		DWORD dwSize = strlen(pRefStr);
		pTempStr = new char [ dwSize+1 ];
		DWORD i = 0;
		for ( DWORD c=0; c<dwSize; c++ )
		{
			if ( pRefStr [ c ]!=8 )
				pTempStr [ i++ ] = pRefStr [ c ];
			else
				if ( i>0 )
					i--;
		}
		pTempStr [ i ] = 0;

		// place this as new use reference
		pRefStr = pTempStr;
	}

	// create/copy string for use
	LPSTR pReturnString=NULL;
	if(pRefStr)
	{
		DWORD dwSize=strlen(pRefStr);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, pRefStr);
	}

	// free usages
	delete[] pTempStr;

	// return DB string
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetEntry ( DWORD pDestStr )
{
	return GetEntryEx ( pDestStr, 0 );
}

DARKSDK int UpKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_UP ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int DownKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_DOWN ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int LeftKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_LEFT ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int RightKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_RIGHT ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int ControlKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_LCONTROL ) || KEYDOWN ( m_KeyBuffer, DIK_RCONTROL ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int ShiftKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_LSHIFT ) || KEYDOWN ( m_KeyBuffer, DIK_RSHIFT ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int SpaceKey ( void )
{
	UpdateKeyboard ( );

	if ( m_KeyBuffer [ DIK_SPACE ] )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}
	
	return 0;
}

DARKSDK int EscapeKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_ESCAPE ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int ReturnKey ( void )
{
	UpdateKeyboard ( );

	if ( KEYDOWN ( m_KeyBuffer, DIK_RETURN ) )
	{
		memset ( &m_KeyBuffer, 0, sizeof ( m_KeyBuffer ) );
		return 1;
	}

	return 0;
}

DARKSDK int KeyState ( int iKey )
{
	// DBKeystate: return true if the key is being pressed
	UpdateKeyboard();
	if(KEYDOWN(m_KeyBuffer, iKey))
		return 1;
	else
		return 0;
	/*
	// translate the scan code into a key code and check its
	// current state
	if ( GetAsyncKeyState ( MapVirtualKey ( iKey, 3 ) ) & 1 )
		return 1;
	
	// return false if nothing is being pressed
	return 0;
	*/
}

// mike - 230107 - new function to get state, useful for caps lock etc
DARKSDK int	GetKeyStateEx ( int iKey )
{
    SHORT KeyState = GetKeyState( iKey );

    if ( (iKey > 0) && (iKey < sizeof(m_GetKeyStateToggle)) && (m_GetKeyStateToggle[iKey]) )
        KeyState ^= 1;
    
    return (int) KeyState;
}

DARKSDK void SetKeyStateEx( int iKey, int iFlag )
{
    if ( (iKey < 0) && (iKey >= sizeof(m_GetKeyStateToggle)) )
        return;

    if (iFlag != 0)
        iFlag = 1;

    // Get the current state of the toggle flag for the key
    int iState = GetKeyStateEx( iKey ) & 1;

    // If the state isn't the same as the required value, flip the flipping bit
    if (iState != iFlag)
        m_GetKeyStateToggle[iKey] = ! m_GetKeyStateToggle[iKey];
}

DARKSDK int ScanCode ( void )
{
	// DBScancode: get the scan code of the current key being pressed
	int iScanCode=0;
	UpdateKeyboard();
	for ( int iTemp = 0; iTemp < 256; iTemp++ )
	{
		if(m_KeyBuffer[iTemp]>0)
		{
			iScanCode=iTemp;
			break;
		}
	}
	return iScanCode;
	/*
	// check all virtual key codes and see if they are
	// being pressed, if so map them to the scan code
	for ( int iTemp = 0; iTemp < 256; iTemp++ )
	{
		// is the key being pressed
		if ( GetAsyncKeyState ( iTemp ) & 1 )
		{
			// return scan code
			return MapVirtualKey ( iTemp, 0 );
		}
	}
	
	// return -1 if nothing is being pressed
	return -1;
	*/
}

DARKSDK void HideMouse( void )
{
	if(g_pGlob)
	{
		if(g_pGlob->bWindowsMouseVisible==true)
		{
			g_pGlob->bWindowsMouseVisible=false;
			ShowCursor(FALSE);
		}
	}
}

DARKSDK void ShowMouse( void )
{
	if(g_pGlob)
	{
		if(g_pGlob->bWindowsMouseVisible==false)
		{
			g_pGlob->bWindowsMouseVisible=true;
			ShowCursor(TRUE);
		}
	}
}

DARKSDK void PositionMouse( int iX, int iY )
{
	// Standard or Direct Mode
	if ( iX>=0 )
	{
		// Special Scale for When Windows Stretch Beyond Physical Size of Backbuffer
		RECT rc;
		GetClientRect(g_pGlob->hWnd, &rc);
		float xRatio = (float)g_pGlob->dwWindowWidth/(float)rc.right;
		float yRatio = (float)g_pGlob->dwWindowHeight/(float)rc.bottom;
		iX = (int)(iX / xRatio);
		iY = (int)(iY / yRatio);

		// Add client start position to coords as window may be offset to screen
		RECT windowrc;
		GetWindowRect(g_pGlob->hWnd, &windowrc);
		int iXMargin = (int)((windowrc.right-windowrc.left)-rc.right)/2;
		int iYMargin = (int)((windowrc.bottom-windowrc.top)-rc.bottom);
		if(iYMargin!=0)
		{
			iX += windowrc.left + iXMargin;
			iY += windowrc.top - iXMargin + iYMargin;
		}
	}
	else
	{
		// passed in as negatives to allow direct mouse ptr control without scaling
		iX = iX * -1;
		iY = iY * -1;
	}

	// Reposition mouse
	if(g_pGlob)
	{
		g_pGlob->iWindowsMouseX = iX;
		g_pGlob->iWindowsMouseY = iY;
	}
	SetCursorPos(iX, iY);

	// 280305 - no longer rest z value when positioning the mouse
	//g_iMouseLocalZ = 0;
}

DARKSDK void ChangeMouse( int iCursorID )
{
	if(g_pGlob) g_pGlob->ChangeMouseFunction( (DWORD)iCursorID );
}

DARKSDK void ChangeMouse( int iCursorID, int iTouchFriendly )
{
	if(g_pGlob) g_pGlob->ChangeMouseFunction( (DWORD)iCursorID );
	g_iTouchFriendly = iTouchFriendly;
}

DARKSDK int GetMouseX ( void )
{
	// return the mouse x position
	UpdateMouse ( );
	if(g_pGlob)
		return g_pGlob->iWindowsMouseX;
	else
		return 0;
}

DARKSDK int GetMouseY ( void )
{
	// return the mouse y position
	UpdateMouse ( );
	if(g_pGlob)
		return g_pGlob->iWindowsMouseY;
	else
		return 0;
}

DARKSDK int GetMouseZ ( void )
{
	// return the mouse z position ( scroll wheel )
	UpdateMouse ( );
	return g_iMouseLocalZ;
}

DARKSDK int GetMouseClick ( void )
{
	// get which mouse button has been clicked
	// button 0 = 1
	// button 1 = 2
	// button 2 = 4
	// button 3 = 8
	// so if button 1 and 2 were clicked you
	// would get a return value of 3

	int iTemp  = 0;
	int iCount = 0;

	// update the mouse data
	UpdateMouse ( );

    for (int i = 0; i < sizeof(m_MouseBuffer.rgbButtons); ++i)
    {
        if (m_MouseBuffer.rgbButtons[i] & 0x80)
        {
            iCount |= (1 << i);
        }
    }

    memset ( &m_MouseBuffer, 0, sizeof ( m_MouseBuffer ) );

	// U76 - Windows 7 can produce mouse event!
	if ( g_pGlob )
	{
		if ( g_pGlob->dwWindowsMouseLeftTouchPersist != 0 ) 
		{
			// U76 - Windows 7 touch has no 'touch-release' via WM_MOUSE commands
			// so we wait 250ms and then switch it off anyway
			if ( timeGetTime() > g_pGlob->dwWindowsMouseLeftTouchPersist )
				g_pGlob->dwWindowsMouseLeftTouchPersist=0;
			else
				iCount |= 1;

			if ( g_iTouchFriendly==0 )
			{
				// for U77 - old style, none persistent
				g_pGlob->dwWindowsMouseLeftTouchPersist=0;
				iCount |= 1;
			}
			else
			{
				// persistent is touch friendly
			}
		}
	}

	return iCount;
}

DARKSDK int GetMouseMoveX ( void )
{
	// get the relative x movement
	UpdateMouse ( );
	int iLatest=g_iMouseDeltaX;
	// mike - 250604 - don't clear
	g_iMouseDeltaX=0;
	return iLatest;
}

DARKSDK int GetMouseMoveY ( void )
{
	// get the relative y movement
	UpdateMouse ( );
	int iLatest=g_iMouseDeltaY;
	// mike - 250604 - don't clear
	g_iMouseDeltaY=0;
	return iLatest;
}

DARKSDK int GetMouseMoveZ ( void )
{
	// get the relative z movement
	UpdateMouse ( );
	int iLatest=g_iMouseDeltaZ;
	// mike - 250604 - don't clear
	g_iMouseDeltaZ=0;
	return iLatest;
}

DARKSDK void WriteToClipboard ( LPSTR pString )
{
	char* lpCopy;
	HGLOBAL hglbCopy;

	if(!OpenClipboard(g_phWnd)) return;
	EmptyClipboard();

	// Allocate a global memory object for the text.  
	hglbCopy = GlobalAlloc(GMEM_DDESHARE, (strlen(pString)+1)*sizeof(char));
	if(hglbCopy==NULL)
	{ 
		CloseClipboard();
		return;
	} 

	// Lock the handle and copy the text to the buffer.  
	lpCopy = (char*)GlobalLock(hglbCopy); 
	memcpy(lpCopy, pString, (strlen(pString)+1)*sizeof(char)); 
	lpCopy[strlen(pString)] = (char)0;

	// Unlock handle
	GlobalUnlock(hglbCopy);          
	
	// Place the handle on the clipboard.  
	SetClipboardData(CF_TEXT, hglbCopy); 

	// Close it up
	CloseClipboard();
}

DARKSDK DWORD GetClipboard ( DWORD pDestStr )
{
	// Free old string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

	// Clipboard Vars
	HGLOBAL hglb = NULL;
	LPSTR pString = NULL;
	LPSTR pClipboardData;
	int ClipboardDataSize;

	// Get Data from clipboard
	if(!IsClipboardFormatAvailable(CF_TEXT))
		return NULL;

	if(!OpenClipboard(g_phWnd))
		return NULL;

	// Extract string from buffer
	hglb = GetClipboardData(CF_TEXT);   
	if(hglb!=NULL)    
	{ 
		pClipboardData = (LPSTR)GlobalLock(hglb);       
		if(pClipboardData!=NULL) 
		{
			// Find size of clipboard data
			ClipboardDataSize=0;
			while(pClipboardData[ClipboardDataSize]!=0) ClipboardDataSize++;
			ClipboardDataSize++;

			// Allocate new size
			g_pCreateDeleteStringFunction((DWORD*)&pString, ClipboardDataSize+1);
			ZeroMemory(pString, ClipboardDataSize+1);
			memcpy(pString, pClipboardData, ClipboardDataSize);
			GlobalUnlock(hglb);        
		}    
	} 
	CloseClipboard(); 

	// Return String
	return (DWORD)pString;
}

DARKSDK void SetRegistryHKEY ( int iMode )
{
	switch ( iMode )
	{
		case 0 : g_hkeyRegistry = HKEY_LOCAL_MACHINE; break;
		case 1 : g_hkeyRegistry = HKEY_CURRENT_USER; break;
	}
}

DARKSDK void WriteToRegistry ( LPSTR pfolder, LPSTR valuekey, int iValue )
{
	if(strlen(pfolder)<255 && strlen(valuekey)<255)
	{
		HKEY hKeyNames = 0;
		DWORD Status;
		DWORD dwDisposition;
		char ObjectType[256];
		strcpy(ObjectType,"Num");
		Status = RegCreateKeyEx(g_hkeyRegistry, pfolder, 0L, ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKeyNames, &dwDisposition);
		if(dwDisposition==REG_OPENED_EXISTING_KEY)
		{
			RegCloseKey(hKeyNames);
			Status = RegOpenKeyEx(g_hkeyRegistry, pfolder, 0L, KEY_WRITE, &hKeyNames);
		}
		if(Status==ERROR_SUCCESS)
		{
			DWORD Datavalue=0;
			memcpy(&Datavalue,&iValue,4);
			Status = RegSetValueEx(hKeyNames, valuekey, 0, REG_DWORD, (LPBYTE)&Datavalue, sizeof(DWORD));
		}
		RegCloseKey(hKeyNames);
		hKeyNames=0;
	}
	else
	{
		// runtime fields tool large
	}
}

DARKSDK bool CoreWriteStringToRegistry(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	DWORD dwDisposition;
	char ObjectType[256];
	strcpy(ObjectType,"Num");
	Status = RegCreateKeyEx(g_hkeyRegistry, PerfmonNamesKey, 0L, ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKeyNames, &dwDisposition);
	if(dwDisposition==REG_OPENED_EXISTING_KEY)
	{
		RegCloseKey(hKeyNames);
		Status = RegOpenKeyEx(g_hkeyRegistry, PerfmonNamesKey, 0L, KEY_WRITE, &hKeyNames);
	}
    if(Status==ERROR_SUCCESS)
	{
        Status = RegSetValueEx(hKeyNames, valuekey, 0, REG_SZ, (LPBYTE)string, (strlen(string)+1)*sizeof(char));
	}
	RegCloseKey(hKeyNames);
	hKeyNames=0;
	return true;
}

DARKSDK void CoreReadStringFromRegistry(char* PerfmonNamesKey, char* valuekey, char* string)
{
	HKEY hKeyNames = 0;
	DWORD Status;
	char ObjectType[256];
	DWORD Datavalue = 0;

	strcpy(string,"");
	strcpy(ObjectType,"Num");
	Status = RegOpenKeyEx(g_hkeyRegistry, PerfmonNamesKey, 0L, KEY_READ, &hKeyNames);
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

DARKSDK int GetRegistry ( LPSTR pfolder, LPSTR valuekey )
{
	int iValue=0;
	if(strlen(pfolder)<255 && strlen(valuekey)<255)
	{
		HKEY hKeyNames = 0;
		DWORD Status;
		char ObjectType[256];
		DWORD Datavalue = 0;
		strcpy(ObjectType,"Num");
		Status = RegOpenKeyEx(g_hkeyRegistry, pfolder, 0L, KEY_READ, &hKeyNames);
		if(Status==ERROR_SUCCESS)
		{
			DWORD Type=REG_DWORD;
			DWORD Size=sizeof(DWORD);
			Status = RegQueryValueEx(hKeyNames, valuekey, NULL, &Type, (LPBYTE)&Datavalue, &Size);
			RegCloseKey(hKeyNames);
		}
		memcpy(&iValue,&Datavalue,4);
	}
	else
	{
		// runtime fields tool large
	}
	return iValue;
}

DARKSDK void WriteToRegistryS ( LPSTR pfolder, LPSTR valuekey, DWORD pString )
{
	if(strlen(pfolder)<255 && strlen(valuekey)<255)
	{
		char pStr[256];
		if(pString) strcpy(pStr, (LPSTR)pString); else strcpy(pStr, "");
		CoreWriteStringToRegistry ( pfolder, valuekey, pStr );
	}
	else
	{
		// runtime fields too large
	}
}

DARKSDK void WriteToRegistrySL ( LPSTR pfolder, LPSTR valuekey, DWORD pString, int iCurrentUserMode )
{
	SetRegistryHKEY ( iCurrentUserMode );
	return WriteToRegistryS ( pfolder, valuekey, pString );
}

DARKSDK DWORD GetRegistryS ( DWORD pDestStr, LPSTR pfolder, LPSTR valuekey )
{
	LPSTR pString=NULL;

	int iValue=0;
	if(strlen(pfolder)<255 && strlen(valuekey)<255)
	{
		char pStr[256];
		CoreReadStringFromRegistry ( pfolder, valuekey, pStr );

		// Free old string
		if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

		// Allocate new size
		g_pCreateDeleteStringFunction((DWORD*)&pString, strlen(pStr)+1);
		ZeroMemory(pString, strlen(pStr)+1);
		memcpy(pString, pStr, strlen(pStr));
	}
	else
	{
		// runtime fields tool large
	}

	// Return String
	return (DWORD)pString;
}

DARKSDK DWORD GetRegistrySL ( DWORD pDestStr, LPSTR pfolder, LPSTR valuekey, int iCurrentUserMode )
{
	SetRegistryHKEY ( iCurrentUserMode );
	return GetRegistryS ( pDestStr, pfolder, valuekey );
}

//
// Josytick commands
//
DARKSDK int JoystickIndex ( void )
{
    return m_iCDI;
}

DARKSDK int JoystickInformation( int iInfo )
{
    switch (iInfo)
    {
    case 0:
        return m_lpDIControlDevice[m_iCDI].ButtonCount;
    case 1:
        return m_lpDIControlDevice[m_iCDI].HasForceFeedback();
    case 2:
        return m_lpDIControlDevice[m_iCDI].HasZ ? 1 : 0;
    case 3:
        return m_lpDIControlDevice[m_iCDI].HasRx ? 1 : 0;
    case 4:
        return m_lpDIControlDevice[m_iCDI].HasRy ? 1 : 0;
    case 5:
        return m_lpDIControlDevice[m_iCDI].HasRz ? 1 : 0;
    case 6:
        return m_lpDIControlDevice[m_iCDI].SliderCount;
    case 7:
        return m_lpDIControlDevice[m_iCDI].PovCount;
    case 8:
        return m_lpDIControlDevice[m_iCDI].Deadzone / 10;
    default:
        return 0;
    }
}

DARKSDK void SetJoystickDeadzone(int Percentage)
{
    if (Percentage < 0)
        Percentage = 0;
    if (Percentage > 99)
        Percentage = 99;
    m_lpDIControlDevice[m_iCDI].Deadzone = Percentage * 10;
}

DARKSDK int JoystickUp ( void )
{
	UpdateJoystick();

	if(m_JoyStickBuffer.lY < -m_lpDIControlDevice[m_iCDI].Deadzone) return 1; else return 0;
}

DARKSDK int JoystickDown ( void )
{
	UpdateJoystick();
	if(m_JoyStickBuffer.lY > m_lpDIControlDevice[m_iCDI].Deadzone) return 1; else return 0;
}

DARKSDK int JoystickLeft ( void )
{
	UpdateJoystick();
	if(m_JoyStickBuffer.lX < -m_lpDIControlDevice[m_iCDI].Deadzone) return 1; else return 0;
}

DARKSDK int JoystickRight ( void )
{
	UpdateJoystick();
	if(m_JoyStickBuffer.lX > m_lpDIControlDevice[m_iCDI].Deadzone) return 1; else return 0;
}

DARKSDK int JoystickX ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lX;
}

DARKSDK int JoystickY ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lY;
}

DARKSDK int JoystickZ ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lZ;
}

DARKSDK int JoystickFireA ( void )
{
	UpdateJoystick();
	if(KEYDOWN(m_JoyStickBuffer.rgbButtons, 0)) return 1; else return 0;
}

DARKSDK int JoystickFireB ( void )
{
	UpdateJoystick();
	if(KEYDOWN(m_JoyStickBuffer.rgbButtons, 1)) return 1; else return 0;
}

DARKSDK int JoystickFireC ( void )
{
	UpdateJoystick();
	if(KEYDOWN(m_JoyStickBuffer.rgbButtons, 2)) return 1; else return 0;
}

DARKSDK int JoystickFireD ( void )
{
	UpdateJoystick();
	if(KEYDOWN(m_JoyStickBuffer.rgbButtons, 3)) return 1; else return 0;
}

DARKSDK int JoystickFireXL ( int iButton )
{
	UpdateJoystick();
	if(iButton>=0 && iButton<=31)
	{
		if(KEYDOWN(m_JoyStickBuffer.rgbButtons, iButton))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

DARKSDK int JoystickSliderA ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.rglSlider[0];
}

DARKSDK int JoystickSliderB ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.rglSlider[1];
}

DARKSDK int JoystickSliderC ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.rglVSlider[0];
}

DARKSDK int JoystickSliderD ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.rglVSlider[1];
}

DARKSDK int JoystickTwistX ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lRx;
}

DARKSDK int JoystickTwistY ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lRy;
}

DARKSDK int JoystickTwistZ ( void )
{
	UpdateJoystick();
	return m_JoyStickBuffer.lRz;
}

DARKSDK int JoystickHatAngle ( int iHatID )
{
	UpdateJoystick();
	if(iHatID>=0 && iHatID<=3)
	{
		DWORD dwPOV = m_JoyStickBuffer.rgdwPOV[iHatID];
// Based on a bug report of twitching going on
//		BOOL POVCentered = (LOWORD(dwPOV) == 0xFFFF);
//		if(!POVCentered)
		return (int)dwPOV;
//		else
//			return -1;
	}
	else
		return -1;
}

//
// Joystick FORCE Commands
//
DARKSDK void ForceUp ( int iMagnitude )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFDirectionEffect(1, iMagnitude, true);
}

DARKSDK void ForceDown ( int iMagnitude )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFDirectionEffect(2, iMagnitude, true);
}

DARKSDK void ForceLeft ( int iMagnitude )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFDirectionEffect(3, iMagnitude, true);
}

DARKSDK void ForceRight ( int iMagnitude )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFDirectionEffect(4, iMagnitude, true);
}

DARKSDK void ForceAngle( int iMagnitude, int iAngle, int iDuration )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	if(iAngle<0 || iAngle>360)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFANGLEERROR);
		return;
	}

	if(iDuration<0)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFDURATIONERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFAngleEffect(iMagnitude, iAngle, iDuration, true);
}

DARKSDK void ForceChainsaw( int iMagnitude, int iDuration )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	if(iDuration<0)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFDURATIONERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFChainsawEffect(iMagnitude, iDuration, true);
}

DARKSDK void ForceShoot( int iMagnitude, int iDuration )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	if(iDuration<0)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFDURATIONERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFShootEffect(iMagnitude, iDuration, true);
}

DARKSDK void ForceImpact( int iMagnitude, int iDuration )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	if(iDuration<0)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFDURATIONERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFImpactEffect(iMagnitude, iDuration, true);
}

DARKSDK void ForceNoEffect( void )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

    DB_StopFFEffect();
}

DARKSDK void ForceWaterEffect( int iMagnitude, int iDuration )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

	if(iMagnitude<0 || iMagnitude>100)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFMAGNITUDEERROR);
		return;
	}

	if(iDuration<0)
	{
		RunTimeError(RUNTIMEERROR_INPUTFFDURATIONERROR);
		return;
	}

	// Perform Effect
	DB_CreateFFWaterEffect(iMagnitude, iDuration, true);
}

DARKSDK void ForceAutoCenterOn( void )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

    DB_AutoCenter(true);
}

DARKSDK void ForceAutoCenterOff( void )
{
	if(m_lpDIControlDevice[m_iCDI].HasForceFeedback()==false)
	{
		RunTimeError(RUNTIMEERROR_INPUTFORCEFEEDBACKNOTAVAIL);
		return;
	}

    DB_AutoCenter(false);
}


//
// Control Device Commands
//

DARKSDK void PerformChecklistControlDevices( void )
{
	HRESULT hRes;

	// Generate Checklist
	if(g_pGlob)
	{
		g_pGlob->checklistqty=0;
		g_pGlob->checklisthasvalues=true;
		g_pGlob->checklisthasstrings=true;
		g_bCreateChecklistNow=false;
		hRes = m_lpDI->EnumDevices ( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)ChecklistInputControllers, NULL, DIEDFL_ATTACHEDONLY);

		// Ensure checklist is large enough
		for(int c=0; c<g_pGlob->checklistqty; c++)
			GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);

		g_pGlob->checklistqty=0;
		g_bCreateChecklistNow=true;
		hRes = m_lpDI->EnumDevices ( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)ChecklistInputControllers, NULL, DIEDFL_ATTACHEDONLY);
		if(hRes==DI_OK)
		{
			// Additionally overwrite capability flags if any devices are forcefeedback..
			m_lpDI->EnumDevices ( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)ChecklistAddFFValueFlag, NULL, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);

			// Checklist is valid
			g_pGlob->checklistexists=true;
		}
		else
			g_pGlob->checklistexists=false;
	}
}

DARKSDK void SetControlDeviceEx( DWORD pName, int iSubIndex )
{
	// find this controller
	strcpy(gFindController, (LPSTR)pName);

	// use subindex (0-default, 1-skips first occurance of name)
	giChoseSameControllerByIndex = iSubIndex;

	// call find routine
	if(!DB_SelectNewControlDevice())
	{
		RunTimeError(RUNTIMEERROR_INPUTCONTROLLERNOTAVAIL);
	}
}

DARKSDK void SetControlDevice( DWORD pName )
{
	SetControlDeviceEx ( pName, 0 );
}

DARKSDK void SetControlDeviceIndex ( int iIndex )
{
	// U73 - 210309 - new command to set which control device we are manipulating
	if ( iIndex>=0 && iIndex<CONTROLDEVICEMAX )
		m_iCDI = iIndex;
}


DARKSDK DWORD GetControlDevice ( DWORD pDestStr )
{
	// Free old string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

	LPSTR pReturnString=NULL;
	if(m_lpDIControlDevice[m_iCDI].Name())
	{
		DWORD dwSize=strlen(m_lpDIControlDevice[m_iCDI].Name());
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, m_lpDIControlDevice[m_iCDI].Name());
	}
	return (DWORD)pReturnString;
}

DARKSDK int ControlDeviceX( void )
{
	UpdateControlDevice();
	return m_ControlDeviceBuffer.lX;
}

DARKSDK int ControlDeviceY( void )
{
	UpdateControlDevice();
	return m_ControlDeviceBuffer.lY;
}

DARKSDK int ControlDeviceZ( void )
{
	UpdateControlDevice();
	return m_ControlDeviceBuffer.lZ;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#ifdef DARKSDK_COMPILE

void ConstructorInput ( HINSTANCE hInstance )
{
	Constructor ( hInstance );
}

void DestructorInput ( void )
{
	Destructor ( );
}

void SetErrorHandlerInput ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataInput ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DInput ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbClearEntryBuffer ( void )
{
	ClearEntryBuffer ( );
}

char* dbGetEntry ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetEntry ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbUpKey ( void )
{
	return UpKey ( );
}

int dbDownKey ( void )
{
	return DownKey ( );
}

int dbLeftKey ( void )
{
	return LeftKey ( );
}

int dbRightKey ( void )
{
	return RightKey ( );
}

int dbControlKey ( void )
{
	return ControlKey ( );
}

int dbShiftKey ( void )
{
	return ShiftKey ( );
}

int dbSpaceKey ( void )
{
	return SpaceKey ( );
}

int dbEscapeKey ( void )
{
	return EscapeKey ( );
}

int dbReturnKey ( void )
{
	return ReturnKey ( );
}

int dbKeyState ( int iKey )
{
	return KeyState ( iKey );
}

int dbScanCode ( void )
{
	return ScanCode ( );
}

void dbWriteToClipboard ( char* pString )
{
	WriteToClipboard ( pString );
}

void dbWriteToRegistry ( char* a, char* b, int c )
{
	WriteToRegistry ( a, b, c );
}

char* dbGetClipboard ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetClipboard ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbGetRegistry ( LPSTR a, LPSTR b )
{
	return GetRegistry ( a, b );
}

void dbWriteToRegistryS ( LPSTR pfolder, LPSTR valuekey, char* pString )
{
	WriteToRegistryS ( pfolder, valuekey, ( DWORD ) pString );
}

char* dbGetRegistryS ( char* pfolder, char* valuekey )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetRegistryS ( NULL, pfolder, valuekey );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

void dbHideMouse ( void )
{
	HideMouse ( );
}

void dbShowMouse ( void )
{
	ShowMouse ( );
}

void dbPositionMouse ( int x, int y )
{
	PositionMouse ( x, y );
}

void dbChangeMouse ( int a )
{
	ChangeMouse ( a );
}

int dbMouseX ( void )
{
	return GetMouseX ( );
}

int dbMouseY ( void )
{
	return GetMouseY ( );
}

int dbMouseZ ( void )
{
	return GetMouseZ ( );
}

int dbMouseClick ( void )
{
	return GetMouseClick ( );
}

int dbMouseMoveX ( void )
{
	return GetMouseMoveX ( );
}

int dbMouseMoveY ( void )
{
	return GetMouseMoveY ( );
}

int dbMouseMoveZ ( void )
{
	return GetMouseMoveZ ( );
}

int dbJoystickUp ( void )
{
	return JoystickUp ( );
}

int dbJoystickDown ( void )
{
	return JoystickDown ( );
}

int dbJoystickLeft ( void )
{
	return JoystickLeft ( );
}

int dbJoystickRight ( void )
{
	return JoystickRight ( );
}

int dbJoystickX ( void )
{
	return JoystickX ( );
}

int dbJoystickY ( void )
{
	return JoystickY ( );
}

int dbJoystickZ ( void )
{
	return JoystickZ ( );
}

int dbJoystickFireA ( void )
{
	return JoystickFireA ( );
}

int dbJoystickFireB ( void )
{
	return JoystickFireB ( );
}

int dbJoystickFireC ( void )
{
	return JoystickFireC ( );
}

int dbJoystickFireD ( void )
{
	return JoystickFireD ( );
}

int dbJoystickFireXL ( int iButton )
{
	return JoystickFireXL ( iButton );
}

int dbJoystickSliderA ( void )
{
	return JoystickSliderA ( );
}

int dbJoystickSliderB ( void )
{
	return JoystickSliderB ( );
}

int dbJoystickSliderC ( void )
{
	return JoystickSliderC ( );
}

int dbJoystickSliderD ( void )
{
	return JoystickSliderD ( );
}

int dbJoystickTwistX ( void )
{
	return JoystickTwistX ( );
}

int dbJoystickTwistY ( void )
{
	return JoystickTwistY ( );
}

int dbJoystickTwistZ ( void )
{
	return JoystickTwistZ ( );
}

int dbJoystickHatAngle ( int iHatID )
{
	return JoystickHatAngle ( iHatID );
}

void dbForceUp ( int iMagnitude )
{
	ForceUp ( iMagnitude );
}

void dbForceDown ( int iMagnitude )
{
	ForceDown ( iMagnitude );
}

void dbForceLeft ( int iMagnitude )
{
	ForceLeft ( iMagnitude );
}

void dbForceRight ( int iMagnitude )
{
	ForceRight ( iMagnitude );
}

void dbForceAngle ( int iMagnitude, int iAngle, int iDuration )
{
	ForceAngle ( iMagnitude, iAngle, iDuration );
}

void dbForceChainsaw ( int iMagnitude, int iDuration )
{
	ForceChainsaw ( iMagnitude, iDuration );
}

void dbForceShoot ( int iMagnitude, int iDuration )
{
	ForceShoot ( iMagnitude, iDuration );
}

void dbForceImpact ( int iMagnitude, int iDuration )
{
	ForceImpact ( iMagnitude, iDuration );
}

void dbForceNoEffect ( void )
{
	ForceNoEffect ( );
}

void dbForceWaterEffect ( int iMagnitude, int iDuration )
{
	ForceWaterEffect ( iMagnitude, iDuration );
}

void dbForceAutoCenterOn ( void )
{
	ForceAutoCenterOn ( );
}

void dbForceAutoCenterOff ( void )
{
	ForceAutoCenterOff ( );
}

void dbPerformChecklistControlDevices ( void )
{
	PerformChecklistControlDevices ( );
}

void dbSetControlDevice ( char* pName )
{
	SetControlDevice ( ( DWORD ) pName );
}

char* dbGetControlDevice ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetControlDevice ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbControlDeviceX ( void )
{
	return ControlDeviceX ( );
}

int dbControlDeviceY ( void )
{
	return ControlDeviceY ( );
}

int dbControlDeviceZ ( void )
{
	return ControlDeviceZ ( );
}

void dbClearDataInput ( void )
{
	ClearData ( );
}

// lee - 300706 - GDK fixes
char* dbEntry ( void ) { return dbGetEntry(); }
void dbWriteStringToRegistry ( LPSTR pfolder, LPSTR valuekey, DWORD pString ) { WriteToRegistryS ( pfolder, valuekey, pString ); }
char* dbGetRegistryString ( LPSTR pfolder, LPSTR valuekey ) { return dbGetRegistryS ( pfolder, valuekey ); }
int dbJoystickFireX ( int iButton ) { return dbJoystickFireXL ( iButton ); }

// Missing
char* dbGetEntry(int iAutoBackSpaceMode)        { return (char*)GetEntryEx(NULL, iAutoBackSpaceMode); }
char* dbEntry(int iAutoBackSpaceMode)           { return (char*)GetEntryEx(NULL, iAutoBackSpaceMode); }
void  dbSetControlDeviceIndex(int Index)        { SetControlDeviceIndex(Index); }

// New additions
int   dbControlDeviceIndex(void)                 { return JoystickIndex(); }
int   dbGetKeyState(int VirtKey)                 { return GetKeyStateEx(VirtKey); }
int   dbJoystickInformation(int Info)            { return JoystickInformation(Info); }

void  dbSetKeyStateToggle(int VirtKey, int Flag) { SetKeyStateEx(VirtKey, Flag); }
void  dbSetJoystickDeadzone(int Percentage)      { SetJoystickDeadzone(Percentage); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////