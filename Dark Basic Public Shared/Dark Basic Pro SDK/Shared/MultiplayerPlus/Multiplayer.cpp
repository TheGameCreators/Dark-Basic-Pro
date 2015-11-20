
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPLAYER //////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
		MULTIPLAYER SET DEBUG STATE%L%?MultiplayerSetDebugState@@YAXH@Z%
		MULTIPLAYER DISPLAY DEBUG%S%?MultiplayerDisplayDebug@@YAXPAD@Z%
		
		MULTIPLAYER CREATE TCP SERVER%S%?MultiplayerCreateTCPServer@@YAXPAD@Z%
		MULTIPLAYER CREATE TCP SERVER%SL%?MultiplayerCreateTCPServer@@YAXPADK@Z%
		MULTIPLAYER CONNECT TO TCP SERVER%SS%?MultiplayerConnectToTCPServer@@YAXPAD0@Z%
		MULTIPLAYER CONNECT TO TCP SERVER%SSL%?MultiplayerConnectToTCPServer@@YAXPAD0K@Z%
		MULTIPLAYER DISCONNECT%0%?MultiplayerDisconnect@@YAXXZ%%

		MULTIPLAYER SEND FROM PLAYER%L%?MultiplayerSendFromPlayer@@YAXK@Z%
		MULTIPLAYER SEND MESSAGE INTEGER%L%?MultiplayerSendMessageInteger@@YAXH@Z%
		MULTIPLAYER SEND MESSAGE FLOAT%F%?MultiplayerSendMessageFloat@@YAXM@Z%
		MULTIPLAYER SEND MESSAGE STRING%S%?MultiplayerSendMessageString@@YAXPAD@Z%
		MULTIPLAYER SEND MESSAGE MEMBLOCK%L%?MultiplayerSendMessageMemblock@@YAXH@Z%

		MULTIPLAYER SEND MESSAGE INTEGER%LL%?MultiplayerSendMessageInteger@@YAXHH@Z%
		MULTIPLAYER SEND MESSAGE FLOAT%FL%?MultiplayerSendMessageFloat@@YAXMH@Z%
		MULTIPLAYER SEND MESSAGE STRING%SL%?MultiplayerSendMessageString@@YAXPADH@Z%
		MULTIPLAYER SEND MESSAGE MEMBLOCK%LL%?MultiplayerSendMessageMemblock@@YAXHH@Z%

		MULTIPLAYER MESSAGE EXISTS[%L%?MultiplayerMessageExists@@YAHXZ%
		MULTIPLAYER GET MESSAGE%0%?MultiplayerGetMessage@@YAXXZ%
		MULTIPLAYER GET MESSAGE TYPE[%L%?MultiplayerGetMessageType@@YAHXZ%
		MULTIPLAYER GET MESSAGE INTEGER[%L%?MultiplayerGetMessageInteger@@YAHXZ%
		MULTIPLAYER GET MESSAGE FLOAT[%F%?MultiplayerGetMessageFloat@@YAMXZ%
		MULTIPLAYER GET MESSAGE STRING[%S%?MultiplayerGetMessageString@@YAKK@Z%
		MULTIPLAYER GET MESSAGE MEMBLOCK%L%?MultiplayerGetMessageMemblock@@YAXH@Z%
		MULTIPLAYER GET MESSAGE PLAYER FROM[%L%?MultiplayerGetMessagePlayerFrom@@YAKXZ%
		
		MULTIPLAYER GET PLAYER COUNT[%L%?MultiplayerGetPlayerCount@@YAHXZ%
		MULTIPLAYER GET PLAYER NAME[%SL%?MultiplayerGetPlayerName@@YAKKH@Z%
		MULTIPLAYER GET PLAYER ID[%LL%?MultiplayerGetPlayerID@@YAKH@Z%
		MULTIPLAYER GET ID[%L%?MultiplayerGetID@@YAKXZ%

		MULTIPLAYER GET IP ADDRESS$[%S%?MultiplayerGetIPAddress@@YAKXZ%

		MULTIPLAYER GET CONNECTION[%L%?MultiplayerGetConnection@@YAHXZ%

		MULTIPLAYER START VOICE SERVER%0%?MultiplayerStartVoiceServer@@YAXXZ%

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES & DEFINES ///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _WIN32_DCOM

#include "multiplayer.h"
//#include "darksdk.h"
#include <stdio.h>
#include <vector>
#include <stack>

//#include "netvoice.h"

#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#include "DarkSDKMemblocks.h"
	#include "DarkSDKBitmap.h"
	#include "DarkSDKImage.h"
	#include "DarkSDKSound.h"
	#include "DarkSDKBasic3D.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION POINTERS ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int						( *MEMBLOCKS_GetMemblockExist   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockPtr		) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockSize	) ( int );
typedef void					( *MEMBLOCKS_MemblockFromMedia	) ( int, int );
typedef void					( *MEMBLOCKS_MediaFromMemblock	) ( int, int );
typedef void					( *MEMBLOCKS_MakeMemblock		) ( int, int );
typedef void					( *MEMBLOCKS_DeleteMemblock		) ( int );
typedef int						( *MEDIA_GetExist				) ( int );

MEMBLOCKS_GetMemblockExist		g_Memblock_GetMemblockExist		= NULL;
MEMBLOCKS_GetMemblockPtr		g_Memblock_GetMemblockPtr		= NULL;
MEMBLOCKS_GetMemblockSize		g_Memblock_GetMemblockSize		= NULL;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromImage	= NULL;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromBitmap	= NULL;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromSound	= NULL;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromMesh		= NULL;
MEMBLOCKS_MediaFromMemblock		g_Memblock_ImageFromMemblock	= NULL;
MEMBLOCKS_MediaFromMemblock		g_Memblock_BitmapFromMemblock	= NULL;
MEMBLOCKS_MediaFromMemblock		g_Memblock_SoundFromMemblock	= NULL;
MEMBLOCKS_MediaFromMemblock		g_Memblock_MeshFromMemblock		= NULL;
MEMBLOCKS_MakeMemblock			g_Memblock_MakeMemblock			= NULL;
MEMBLOCKS_DeleteMemblock		g_Memblock_DeleteMemblock		= NULL;
MEDIA_GetExist					g_Image_GetExist				= NULL;
MEDIA_GetExist					g_Bitmap_GetExist				= NULL;
MEDIA_GetExist					g_Sound_GetExist				= NULL;
MEDIA_GetExist					g_Basic3D_GetExist				= NULL;



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL DATA //////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static const GUID					g_guidApp					= { 0xe683971c, 0xf36c, 0x4759, { 0xa3, 0x92, 0x60, 0x32, 0x8c, 0x99, 0xd1, 0x3 } };
IDirectPlay8Server*					g_pDPServer					= NULL;
int									g_iDPServerSendTo			= 0;
IDirectPlay8Client*					g_pDPClient					= NULL;
LONG								g_lNumberOfActivePlayers    = 0;
CRITICAL_SECTION					m_csHostEnum				= { 0 };
IDirectPlay8Address*				g_pHostAddr					= NULL;
IDirectPlay8Address*				g_pDeviceAddr				= NULL;

DPNHANDLE							m_hConnectAsyncOp			= 0;
CRITICAL_SECTION					m_csLock;
bool								g_bMultiplayerDebug			= false;

//std::vector < sHost >				g_HostList;
DPN_APPLICATION_DESC				g_ResponseMsgAppDesc;

std::stack < sMessage >				g_MessageList;
std::vector < APP_PLAYER_INFO >		g_PlayerList;
char								g_szName [ 256 ]			= "";
sMessage							g_CurrentMessage;
sSendData							g_SendData;
DWORD								g_PlayerID					= 0;
CRITICAL_SECTION					g_csPlayerContext;
//CNetVoice*							g_pNetVoice                 = NULL;
int									g_iConnection				= 0;
GUID								g_guidDVSessionCT;
//DVCLIENTCONFIG						g_dvClientConfig;
BOOL								g_bMixingSessionType        = FALSE;   // TRUE if the server is mixing, otherwise its forwarding.

#ifndef DARKSDK_COMPILE
	GlobStruct*						g_pGlob							= NULL;

	PTR_FuncCreateStr				g_pCreateDeleteStringFunction	= NULL;
#endif

HRESULT CALLBACK DirectPlayVoiceServerMessageHandler ( LPVOID lpvUserContext, DWORD dwMessageType, LPVOID lpMessage );
HRESULT CALLBACK DirectPlayVoiceClientMessageHandler ( LPVOID lpvUserContext, DWORD dwMessageType, LPVOID lpMessage );

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Constructor ( void )
{
	// initialize the critical section
	InitializeCriticalSection ( &g_csPlayerContext );
	memset ( &g_CurrentMessage, 0, sizeof ( g_CurrentMessage ) );

	#ifdef DARKSDK_COMPILE
		ReceiveCoreDataPtr ( g_pGlob );
	#endif
}

void Destructor	( void )
{
	PLAYER_LOCK();
//	SAFE_DELETE( g_pNetVoice );
	MultiplayerDisconnect ( );
	PLAYER_UNLOCK();
	DeleteCriticalSection ( &g_csPlayerContext );
}

void ReceiveCoreDataPtr	( LPVOID pCore )
{
	g_pGlob = ( GlobStruct* ) pCore;

	if ( !g_pGlob )
		return;

	#ifndef DARKSDK_COMPILE
		// memblock DLL ptrs
		g_Memblock_GetMemblockExist		= ( MEMBLOCKS_GetMemblockExist	) GetProcAddress ( g_pGlob->g_Memblocks, "?MemblockExist@@YAHH@Z" );
		g_Memblock_GetMemblockPtr		= ( MEMBLOCKS_GetMemblockPtr	) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockPtr@@YAKH@Z" );
		g_Memblock_GetMemblockSize		= ( MEMBLOCKS_GetMemblockSize	) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockSize@@YAHH@Z" );
		g_Memblock_MemblockFromImage	= ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromImage@@YAXHH@Z" );
		g_Memblock_MemblockFromBitmap	= ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromBitmap@@YAXHH@Z" );
		g_Memblock_MemblockFromSound	= ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromSound@@YAXHH@Z" );
		g_Memblock_MemblockFromMesh		= ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromMesh@@YAXHH@Z" );
		g_Memblock_ImageFromMemblock	= ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateImageFromMemblock@@YAXHH@Z" );
		g_Memblock_BitmapFromMemblock	= ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateBitmapFromMemblock@@YAXHH@Z" );
		g_Memblock_SoundFromMemblock	= ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateSoundFromMemblock@@YAXHH@Z" );
		g_Memblock_MeshFromMemblock		= ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMeshFromMemblock@@YAXHH@Z" );
		g_Memblock_MakeMemblock			= ( MEMBLOCKS_MakeMemblock		) GetProcAddress ( g_pGlob->g_Memblocks, "?MakeMemblock@@YAXHH@Z" );
		g_Memblock_DeleteMemblock		= ( MEMBLOCKS_DeleteMemblock	) GetProcAddress ( g_pGlob->g_Memblocks, "?DeleteMemblock@@YAXH@Z" );

		// media DLL ptrs
		g_Image_GetExist	= ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Image,		"?GetExistEx@@YAHH@Z" );
		g_Bitmap_GetExist	= ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Bitmap,	"?BitmapExist@@YAHH@Z" );
		g_Sound_GetExist	= ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Sound,		"?GetSoundExist@@YAHH@Z" );
		g_Basic3D_GetExist	= ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Basic3D,	"?GetMeshExist@@YAHH@Z" );
	#else
		g_Memblock_GetMemblockExist		= dbMemblockExist;
		g_Memblock_GetMemblockPtr		= dbGetMemblockPtr;
		g_Memblock_GetMemblockSize		= ( MEMBLOCKS_GetMemblockSize ) dbGetMemblockSize;
		g_Memblock_MemblockFromImage	= dbMakeMemblockFromImage;
		g_Memblock_MemblockFromBitmap	= dbMakeMemblockFromBitmap;
		g_Memblock_MemblockFromSound	= dbMakeMemblockFromSound;
		g_Memblock_MemblockFromMesh		= dbMakeMemblockFromMesh;
		g_Memblock_ImageFromMemblock	= dbMakeImageFromMemblock;
		g_Memblock_BitmapFromMemblock	= dbMakeBitmapFromMemblock;
		g_Memblock_SoundFromMemblock	= dbMakeSoundFromMemblock;
		g_Memblock_MeshFromMemblock		= dbMakeMeshFromMemblock;
		g_Memblock_MakeMemblock			= dbMakeMemblock;
		g_Memblock_DeleteMemblock		= dbDeleteMemblock;

		g_Image_GetExist				= dbImageExist;
		g_Bitmap_GetExist				= dbBitmapExist;
		g_Sound_GetExist				= dbSoundExist;
		g_Basic3D_GetExist				= dbObjectExist;
	#endif
}

void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	g_pErrorHandler = ( CRuntimeErrorHandler* ) pErrorHandlerPtr;
}

DWORD MultiplayerGetID ( void )
{
	// get the ID of a player

	return g_PlayerID;
}

int	MultiplayerGetPlayerCount ( void )
{
	// get the number of players in a game

	return g_PlayerList.size ( );
}

static char* SetupString ( char* szInput )
{
	char* pReturn = NULL;
	DWORD dwSize  = strlen ( szInput );

	g_pGlob->CreateDeleteString ( ( DWORD* ) &pReturn, dwSize + 1 );

	if ( !pReturn )
		return NULL;

	memcpy ( pReturn, szInput, dwSize );

	pReturn [ dwSize ] = 0;

	return pReturn;
}

DWORD MultiplayerGetPlayerName ( DWORD pDestStr, int iID )
{
	if ( iID >= (int) g_PlayerList.size ( ) )
		return ( DWORD ) SetupString ( "" );

	return ( DWORD ) SetupString ( g_PlayerList [ iID ].strPlayerName );
}

DWORD MultiplayerGetPlayerID ( int iID )
{
	// get the ID of a given player

	if ( iID >= (int) g_PlayerList.size ( ) )
		return 0;

	return g_PlayerList [ iID ].dpnidPlayer;
}

void MultiplayerDisconnect ( void )
{
	// disconnect from a game
	if ( g_pDPClient )
	{
  		g_pDPClient->CancelAsyncOperation ( 0, DPNCANCEL_SEND );
		g_pDPClient->Close ( DPNCLOSE_IMMEDIATE );
		g_pDPClient->Release ( );
		g_pDPClient = NULL;
	}
	if ( g_pDPServer )
	{
		g_pDPServer->CancelAsyncOperation ( 0, DPNCANCEL_SEND );
		g_pDPServer->Close ( DPNCLOSE_IMMEDIATE );
		g_pDPServer->Release ( );
		g_pDPServer = NULL;
	}

	// free connection
	g_PlayerID = 0;
	g_iConnection = 0;
	CoUninitialize();
}

void MultiplayerCreateTCPServer ( char* szName )
{
	// create a tcp server on the default port
//	MultiplayerCreateTCPServer ( szName, 2507 );
	MultiplayerCreateTCPServer ( szName, 2303 ); //UDP in DP
}

void MultiplayerCreateTCPServer ( char* szName, DWORD dwPort )
{
	// create a tcp server on the given port

	// local variables
	PDIRECTPLAY8ADDRESS		pDP8AddrLocal			= NULL;		// local address
	HRESULT					hr						= NULL;		// return data for errors
	WCHAR					wcName [ MAX_PATH ];				// name of server
	DPN_APPLICATION_DESC	dpnAppDesc;							// application description

	MultiplayerDisplayDebug ( "Starting TCP Server..." );

	// clear player list right away
//	g_HostList.clear();
	g_PlayerList.clear();

	// start com
	CoInitializeEx ( NULL, COINIT_MULTITHREADED );

	MultiplayerDisplayDebug ( "Starting DirectPlay..." );

	// start directplay
	if ( FAILED ( hr = CoCreateInstance ( CLSID_DirectPlay8Server, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server, ( LPVOID* ) &g_pDPServer ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Deploying Message Handler..." );

	// set up the message handler
	if ( FAILED ( hr = g_pDPServer->Initialize ( NULL, DirectPlayMessageHandler, 0 ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Setting Address..." );

	// set up the local address
	if ( FAILED ( hr = CoCreateInstance ( CLSID_DirectPlay8Address, NULL, CLSCTX_ALL, IID_IDirectPlay8Address, ( LPVOID* ) &pDP8AddrLocal ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Connecting to TCP/IP..." );

	// we want to use tcp / ip
	if ( FAILED ( hr = pDP8AddrLocal->SetSP ( &CLSID_DP8SP_TCPIP ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Adding Port..." );

	// set the port
	if ( FAILED ( hr = pDP8AddrLocal->AddComponent ( DPNA_KEY_PORT, &dwPort, sizeof ( dwPort ), DPNA_DATATYPE_DWORD ) ) )
    {
		return;
    }
    
	// clear the application description structure
	ZeroMemory ( &dpnAppDesc, sizeof ( DPN_APPLICATION_DESC ) );

	// convert the name to wide char
	ConvertAnsiStringToWideCch ( wcName, szName, MAX_PATH );
	
	// set the properties
    dpnAppDesc.dwSize           = sizeof ( DPN_APPLICATION_DESC );					// size of the structure
    dpnAppDesc.dwFlags          = DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;	// we want to use client / server
    dpnAppDesc.guidApplication  = g_guidApp;										// the guid of the server
    dpnAppDesc.pwszSessionName  = wcName;											// the name of the server

	MultiplayerDisplayDebug ( "Hosting Server..." );

	// now start hosting the server
	if ( FAILED ( hr = g_pDPServer->Host ( &dpnAppDesc, &pDP8AddrLocal, 1, NULL, NULL, NULL, 0 ) ) )
	{
		return;
	}

	// indicate connection made
	g_iConnection = 1;
}

void MultiplayerStartVoiceServer ( void )
{
	/*
	if ( g_pDPServer )
		g_pNetVoice = new CNetVoice ( NULL, DirectPlayVoiceServerMessageHandler );
	else
		g_pNetVoice = new CNetVoice ( DirectPlayVoiceClientMessageHandler, NULL );

	if ( !g_pNetVoice )
	{
		return;
	}

	if ( g_pDPServer )
	{
		g_guidDVSessionCT = DPVCTGUID_DEFAULT;

		if ( FAILED ( g_pNetVoice->Init ( g_pGlob->hWnd, TRUE, FALSE, g_pDPServer, DVSESSIONTYPE_MIXING, &g_guidDVSessionCT, NULL ) ) )
		{
			return;
		}

		return;
	}

	if ( g_pDPClient )
	{
		ZeroMemory ( &g_dvClientConfig, sizeof ( g_dvClientConfig ) );

		g_dvClientConfig.dwSize                 = sizeof ( g_dvClientConfig );
		g_dvClientConfig.dwFlags                = DVCLIENTCONFIG_AUTOVOICEACTIVATED | DVCLIENTCONFIG_AUTORECORDVOLUME;
		g_dvClientConfig.lPlaybackVolume        = DVPLAYBACKVOLUME_DEFAULT;
		g_dvClientConfig.dwBufferQuality        = DVBUFFERQUALITY_DEFAULT;
		g_dvClientConfig.dwBufferAggressiveness = DVBUFFERAGGRESSIVENESS_DEFAULT;
		g_dvClientConfig.dwThreshold            = DVTHRESHOLD_UNUSED;
		g_dvClientConfig.lRecordVolume          = DVRECORDVOLUME_LAST;
		g_dvClientConfig.dwNotifyPeriod         = 0;

		if ( FAILED ( g_pNetVoice->Init ( g_pGlob->hWnd, FALSE, TRUE, g_pDPClient, DVSESSIONTYPE_MIXING, NULL, &g_dvClientConfig ) ) )
		{
			return;
		}

		IDirectPlayVoiceClient* pVoiceClient = g_pNetVoice->GetVoiceClient ( );
		DVSESSIONDESC dvsd;
		ZeroMemory ( &dvsd, sizeof ( DVSESSIONDESC ) );
		dvsd.dwSize = sizeof ( DVSESSIONDESC );
		pVoiceClient->GetSessionDesc ( &dvsd );
		g_bMixingSessionType = ( dvsd.dwSessionType == DVSESSIONTYPE_MIXING );
	}
	*/
}

HRESULT CALLBACK DirectPlayVoiceServerMessageHandler ( LPVOID lpvUserContext, DWORD dwMessageType, LPVOID lpMessage )
{
    return S_OK;
}

HRESULT CALLBACK DirectPlayVoiceClientMessageHandler ( LPVOID lpvUserContext, DWORD dwMessageType, LPVOID lpMessage )
{
	return S_OK;
}

bool CheckString ( char* szString )
{
	// check a string passed in is valid

	// see if it's null
	if ( !szString )
		return false;

	// check the length
	if ( strlen ( szString ) < 1 )
		return false;

	// valid string so return ok
	return true;
}

void MultiplayerConnectToTCPServer ( char* szName, char* szIP )
{
	// connect to a tcp server with the default port

	// check that the string parameters are valid
	if ( !CheckString ( szName ) || !CheckString ( szIP ) )
		return;

	// finally connect to the server (and port)
//	MultiplayerConnectToTCPServer ( szName, szIP, 2507 );
	MultiplayerConnectToTCPServer ( szName, szIP, 2303 ); //UDP in DP
}

void MultiplayerConnectToTCPServer ( char* szName, char* szIP, DWORD dwPort )
{
	// connect to a tcp server on the given port

	// check that the string parameters are valid
	if ( !CheckString ( szName ) || !CheckString ( szIP ) )
		return;

	// local variable declarations
	HRESULT					hr					= NULL;		// return value for function calls
	DPNHANDLE				m_hEnumAsyncOp		= 0;		// directplay handle
	DPN_APPLICATION_DESC	dpnAppDesc			= { 0 };	// application description
	IDirectPlay8Address*	pDP8AddressHost		= NULL;		// host address
	IDirectPlay8Address*	pDP8AddressLocal	= NULL;		// local address
	WCHAR*					wszHostName			= NULL;		// host name

	// copy the name of the server into the global string array
	strcpy ( g_szName, szName );

	// clear player list right away
//	g_HostList.clear();
	g_PlayerList.clear();

	// start com
	CoInitializeEx ( NULL, COINIT_MULTITHREADED );
	
	// start directplay client interface
	SAFE_RELEASE( g_pDPClient );

	MultiplayerDisplayDebug ( "Starting Client..." );
	if ( FAILED ( hr = CoCreateInstance ( CLSID_DirectPlay8Client, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Client, ( LPVOID* ) &g_pDPClient ) ) )
	{
		return;
	}

	// set up the message handler
	MultiplayerDisplayDebug ( "Deploying Message Handler..." );
	if ( FAILED( hr = g_pDPClient->Initialize ( NULL, DirectPlayMessageHandler, 0 ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Set address..." );

	// set the local address interface
	if ( FAILED ( hr = CoCreateInstance ( CLSID_DirectPlay8Address, NULL, CLSCTX_ALL, IID_IDirectPlay8Address, ( LPVOID* ) &pDP8AddressLocal ) ) )
    {
		return;
    }

	MultiplayerDisplayDebug ( "Set TCP/IP..." );

	// set the service provider for the local address - tcp / ip
    if ( FAILED ( hr = pDP8AddressLocal->SetSP ( &CLSID_DP8SP_TCPIP ) ) )
    {
		return;
    }

	MultiplayerDisplayDebug ( "Set Host.." );

	// set the host address
	if ( FAILED ( hr = CoCreateInstance ( CLSID_DirectPlay8Address, NULL, CLSCTX_ALL, IID_IDirectPlay8Address, ( LPVOID* ) &pDP8AddressHost ) ) )
    {
		return;
    }

	MultiplayerDisplayDebug ( "Set SP..." );

	// set the service provider for the host address - tcp / ip
	if ( FAILED ( hr = pDP8AddressHost->SetSP ( &CLSID_DP8SP_TCPIP ) ) )
	{
		return;
	}

	MultiplayerDisplayDebug ( "Add IP Address..." );

	// set the name of the host
	wszHostName = new WCHAR [ _tcslen ( szIP ) + 1 ];

	// check array is valid
	if ( !wszHostName )
		return;
	
	// convert from ansi to wide char
	ConvertAnsiStringToWideCch ( wszHostName, szIP, ( int ) _tcslen ( szIP ) + 1 );

	// add the host component
	if ( FAILED ( hr = pDP8AddressHost->AddComponent ( 
														DPNA_KEY_HOSTNAME,
														wszHostName,
														( DWORD ) ( wcslen ( wszHostName ) + 1 ) * sizeof ( WCHAR ),
														DPNA_DATATYPE_STRING
													 ) ) )
	{
		return;
	}

	// free host name
	SAFE_DELETE ( wszHostName);
	
	MultiplayerDisplayDebug ( "Add Port..." );

	// add the port
	if ( FAILED ( hr = pDP8AddressHost->AddComponent ( DPNA_KEY_PORT, &dwPort, sizeof ( dwPort ), DPNA_DATATYPE_DWORD ) ) )
	{
		return;
	}

	// clear the application description structure
	ZeroMemory ( &dpnAppDesc, sizeof ( DPN_APPLICATION_DESC ) );

	// set up the application description
    dpnAppDesc.dwSize          = sizeof ( DPN_APPLICATION_DESC );	// size of the structure
    dpnAppDesc.guidApplication = g_guidApp;							// the guid

	MultiplayerDisplayDebug ( "Enumerate Hosts..." );

	// now find the hosts
//	if ( FAILED ( hr = g_pDPClient->EnumHosts ( &dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, NULL,  0, INFINITE, 0, INFINITE, NULL, &m_hEnumAsyncOp, 0 ) ) )
	if ( FAILED ( hr = g_pDPClient->EnumHosts ( &dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, NULL,  0, 0, 0, 0, NULL, NULL, DPNENUMHOSTS_SYNC ) ) )
	{
		return;
	}

	// The above ENUMHOSTS will not be called any more, so can act on the HOST I have found
	// that is, to connect (which should not have been called inside ENUMHOSTS Mike!)
	if ( g_pHostAddr )
	{
		// indicate connection made - see enumhost
		ConnectAfterEnum ( );
		g_iConnection = 1;
	}
	else
	{
		// no host found
		g_iConnection = 0;
	}

	// free usages
	SAFE_RELEASE(pDP8AddressHost);
	SAFE_RELEASE(pDP8AddressLocal);
}

void MultiplayerSendMessageInteger ( int iData, int iGUA )
{
	// iGUA = DPNSEND_PRIORITY_HIGH = 0x0080
	// set the send data
	g_SendData.iInteger = iData;			// the data
	g_SendData.dwFlags  = ( DWORD ) iGUA;	// default flags

	// now send the message
	SendMessage ( eTypeInteger );
}

void MultiplayerSendMessageFloat ( float fData, int iGUA )
{
	// iGUA = DPNSEND_PRIORITY_HIGH = 0x0080
	// set up the send data
	g_SendData.fFloat  = fData;
	g_SendData.dwFlags = ( DWORD ) iGUA;

	// now send the data
	SendMessage ( eTypeFloat );
}

void MultiplayerSendMessageString ( char* szString, int iGUA )
{
	// iGUA = DPNSEND_PRIORITY_HIGH = 0x0080
	// first check the string
	if ( !CheckString ( szString ) )
		return;

	// copy the string into the send data
	strcpy ( g_SendData.szString, szString );
	g_SendData.dwFlags = ( DWORD ) iGUA;

	// now send the message
	SendMessage ( eTypeString );
}

void MultiplayerSendMessageMemblock ( int iMemblock, int iGUA, int iSize )
{
	// iGUA = DPNSEND_PRIORITY_HIGH = 0x0080
	char* pMemblockData = ( char* ) g_Memblock_GetMemblockPtr ( iMemblock );
	if ( !pMemblockData )
		return;

	g_SendData.iInteger  = iMemblock;
	g_SendData.pMemblock = pMemblockData;
	g_SendData.dwMemblockSize = g_Memblock_GetMemblockSize ( iMemblock );
	if ( iSize!=-1 ) g_SendData.dwMemblockSize = iSize;
	g_SendData.dwFlags   = ( DWORD ) iGUA;

	SendMessage ( eTypeMemblock );
}

void MultiplayerSendMessageMemblock ( int iMemblock, int iGUA )
{
	// send memblock of the entire size of memblock
	MultiplayerSendMessageMemblock ( iMemblock, iGUA, -1 );
}

void MultiplayerSendMessageFloat ( float fData )
{
	MultiplayerSendMessageFloat ( fData, 0 );
}

void MultiplayerSendMessageString ( char* szString )
{
	MultiplayerSendMessageString ( szString, 0 );
}

void MultiplayerSendMessageMemblock	( int iMemblock )
{
	MultiplayerSendMessageMemblock ( iMemblock, 0 );
}

void MultiplayerSendMessageInteger ( int iData )
{
	MultiplayerSendMessageInteger ( iData, 0 );
}

void MultiplayerSendFromPlayer ( DWORD dwID )
{
	// set player ID for who is sending the message
	g_SendData.dpnidPlayer = dwID;
}

void MultiplayerSendToPlayer ( int iSendToIndex )
{
	// comes in as the index to the playerlist, but as +1 to leave zero to equal ALL
	g_iDPServerSendTo = iSendToIndex;
}

void SendMessage ( eSendType eDataType )
{
	// default send message function - goes to all players in the group
	SendMessageTo ( DPNID_ALL_PLAYERS_GROUP, eDataType );
}

void SendMessageTo ( DPNID dpnidTarget, eSendType eDataType )
{
	// send a message to the given target, this function creates
	// a byte array, copies the data in in the format of the
	// sMessage structure and then sends it

	// we start with the type of the data, then the size, then
	// the id of the player who has sent it and finally the
	// data itself

	// set the size of the message to 0
	DWORD dwSize = 0;

	// check the data type we are sending and work out it's size
	switch ( eDataType )
	{
		case eTypeInteger:			dwSize = sizeof ( int   );															break;
		case eTypeFloat:			dwSize = sizeof ( float );															break;
		case eTypeString:			dwSize = sizeof ( char  ) * strlen ( g_SendData.szString );							break;
		case eTypeDestroyPlayer:	dwSize = sizeof ( DWORD );															break;
		case eTypeCreatePlayer:		dwSize = sizeof ( DWORD ) + ( sizeof ( char ) * strlen ( g_SendData.szString ) );	break;
		case eTypeMemblock:			dwSize = g_SendData.dwMemblockSize;													break;
	}

	// set up some local variables including the total size of the
	// data to send and then create an array so we can copy the data
	DPNHANDLE		hAsync;
	DPN_BUFFER_DESC	send;
	DWORD			dwTotalSize = sizeof ( int ) + sizeof ( DWORD ) + sizeof ( DPNID ) + dwSize;
	BYTE*			pData		= new BYTE [ dwTotalSize ];

	// check the allocated array is valid
	if ( !pData )
		return;

	// first clear it out
	memset ( pData, 0, dwTotalSize );

	// copy the data into it
	memcpy ( pData,										&eDataType,				 sizeof ( int   ) );
	memcpy ( pData + sizeof ( int ),					&dwSize,				 sizeof ( DWORD ) );
	memcpy ( pData + sizeof ( int ) + sizeof ( DWORD ), &g_SendData.dpnidPlayer, sizeof ( DPNID ) );

	// copy the actual data into the array
	switch ( eDataType )
	{
		case eTypeInteger:			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ), &g_SendData.iInteger, dwSize );	break;
		case eTypeFloat:			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ), &g_SendData.fFloat,   dwSize );	break;
		case eTypeString:			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ), &g_SendData.szString, dwSize );	break;
		case eTypeDestroyPlayer:	memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ), &g_SendData.dwDword,  dwSize );	break;
		case eTypeMemblock:			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ), g_SendData.pMemblock, dwSize );	break;
		
		case eTypeCreatePlayer:
		{
			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ),				      &g_SendData.dwDword,  sizeof ( DWORD ) );
			memcpy ( pData + sizeof ( int ) + sizeof ( DPNID ) + sizeof ( DWORD ) + sizeof ( DWORD ), &g_SendData.szString, sizeof ( char ) * strlen ( g_SendData.szString ) );
		}
		break;
	}

	// now set up the send message for directplay
	send.dwBufferSize = dwTotalSize;				// total size of all data
    send.pBufferData  = ( BYTE* ) pData;			// the actual data itself

	// skip if not high priority and queue exceeding treshold
    //DPN_CONNECTION_INFO dpnConInfo = {0}; - useful net monitoring function
    //dpnConInfo.dwSize = sizeof(DPN_CONNECTION_INFO);
	//g_pDPClient->GetConnectionInfo( &dpnConInfo, 0 );
	DWORD dwMsgWaiting, dwQueueSize;
	dwMsgWaiting=0;
	dwQueueSize=0;

	// use client
	if ( g_pDPClient )
	{
		// throttle skip and timeout
		DWORD dwTimeOut = 500;
		if ( g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH ) dwTimeOut=10000;
		g_pDPClient->GetSendQueueInfo( &dwMsgWaiting, &dwQueueSize, DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW );
		if ( dwMsgWaiting < 5 || g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH )
		{
			// only have five non-high items at a time
			DWORD dwFlag = g_SendData.dwFlags;
			if ( g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH )
			{
				dwFlag = dwFlag | DPNSEND_GUARANTEED;
			}
			g_pDPClient->Send ( &send, 1, dwTimeOut, NULL, &hAsync, dwFlag );
		}
	}

	// use server - use no loopback as the server does not want this message coming back
	if ( g_pDPServer )
	{
		// throttle skip and timeout
		DWORD dwTimeOut = 500;
		if ( g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH ) dwTimeOut=10000;
		g_pDPServer->GetSendQueueInfo( dpnidTarget, &dwMsgWaiting, &dwQueueSize, DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW );
		if ( dwMsgWaiting < 5 || g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH )
		{
			// only have five non-high items at a time
			DWORD dwFlag = g_SendData.dwFlags | DPNSEND_NOLOOPBACK;
			if ( g_SendData.dwFlags & DPNSEND_PRIORITY_HIGH )
			{
				dwFlag = dwFlag | DPNSEND_GUARANTEED;
			}

			// who to send to..
			if ( g_iDPServerSendTo>=0 )
			{
				// ALL or JUST ONE
				if ( g_iDPServerSendTo==0 )
					g_pDPServer->SendTo ( DPNID_ALL_PLAYERS_GROUP, &send, 1, dwTimeOut, NULL, &hAsync, dwFlag );
				else
					g_pDPServer->SendTo ( g_PlayerList [ g_iDPServerSendTo-2 ].dpnidPlayer, &send, 1, dwTimeOut, NULL, &hAsync, dwFlag );
			}
			else
			{
				// ALL EXCEPT stated (for sending info to all clients except the originator)
				int iTotal = g_PlayerList.size ( );
				int iRealIndex = g_iDPServerSendTo;
				iRealIndex = iRealIndex * -1;
				iRealIndex = iRealIndex - 2;
				for ( int iID=0; iID<iTotal; iID++ )
					if ( iRealIndex != iID )
						g_pDPServer->SendTo ( g_PlayerList [ iID ].dpnidPlayer, &send, 1, dwTimeOut, NULL, &hAsync, dwFlag );
			}
		}
	}

	// delete the previously allocated memory
	delete [ ] pData;
}

void MultiplayerSetDebugState ( int iState )
{
	// set the debug state in case we want to print messages
	g_bMultiplayerDebug = true;
}

void MultiplayerDisplayDebug ( char* szInformation )
{
	// can cause a crash due to threading - leave this for now

	/*
	if ( g_bMultiplayerDebug )
	{
		dbPrint ( szInformation );
		dbSync  ( );
	}
	*/
}

void CreatePlayer ( void* pBuffer )
{
	// called by the message handler when a new player is created

	MultiplayerDisplayDebug ( "CreatePlayer" );

	// local variable declaration
	HRESULT					hr					= 0;
    PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg	= ( PDPNMSG_CREATE_PLAYER ) pBuffer;
	DWORD					dwSize				= 0;
    DPN_PLAYER_INFO*		pdpPlayerInfo		= NULL;

    hr = g_pDPServer->GetClientInfo ( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
    
	if ( FAILED ( hr ) && hr != DPNERR_BUFFERTOOSMALL )
    {
        // ignore this message if this is for the host
        //if ( hr != DPNERR_INVALIDPLAYER )
            //DXTRACE_ERR_MSGBOX ( TEXT ( "GetClientInfo" ), hr );

        return;
    }

    // allocate space for client info
    pdpPlayerInfo = ( DPN_PLAYER_INFO* ) new BYTE [ dwSize ];

    if ( NULL == pdpPlayerInfo )
    {
        //DXTRACE_ERR_MSGBOX ( TEXT ( "DPN_MSGID_CREATE_PLAYER" ), E_OUTOFMEMORY );
        return;
    }

    ZeroMemory ( pdpPlayerInfo, dwSize );
    pdpPlayerInfo->dwSize = sizeof ( DPN_PLAYER_INFO );

    // get the peer info and extract its name  
    hr = g_pDPServer->GetClientInfo ( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );

    if ( FAILED ( hr ) )
    {
        //DXTRACE_ERR_MSGBOX ( TEXT ( "GetClientInfo" ), hr );
        SAFE_DELETE_ARRAY ( pdpPlayerInfo );
        return;
    }

    // create a new and fill in a APP_PLAYER_INFO
    APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
    if ( NULL == pPlayerInfo )
    {
        SAFE_DELETE_ARRAY ( pdpPlayerInfo );
        return;
    }

    ZeroMemory ( pPlayerInfo, sizeof ( APP_PLAYER_INFO ) );
    pPlayerInfo->lRefCount   = 1;
    pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

    // this stores an extra copy of the player name for 
    // easier access.  This will be a redundant copy since DPlay 
    // also keeps a copy of the player name in GetClientInfo()
    ConvertWideStringToAnsiCch ( pPlayerInfo->strPlayerName, pdpPlayerInfo->pwszName, MAX_PATH );

	char szInfo [ MAX_PATH ] = "";
	sprintf ( szInfo, "User Login: %s", pPlayerInfo->strPlayerName );
	MultiplayerDisplayDebug ( szInfo );

    SAFE_DELETE_ARRAY ( pdpPlayerInfo );

    // tell DirectPlay to store this pPlayerInfo 
    // pointer in the pvPlayerContext.
    pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

    // send all connected players a message telling about this new player
    SendCreatePlayerMsg( pPlayerInfo, DPNID_ALL_PLAYERS_GROUP );

    // tell this new player about the world state
    SendWorldStateToNewPlayer ( pCreatePlayerMsg->dpnidPlayer );

	// not only copied by data here, but memory created here freed in destroyplayer
	g_PlayerList.push_back ( *pPlayerInfo );

    // Update the number of active players, and 
    // post a message to the dialog thread to update the 
    // UI.  This keeps the DirectPlay message handler 
    // from blocking
    InterlockedIncrement ( &g_lNumberOfActivePlayers );
}

void DestroyPlayer ( void* pBuffer )
{
	MultiplayerDisplayDebug ( "DestroyPlayer" );

	PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
    pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pBuffer;
    APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;

    // Ignore this message if this is the host player
    if( pPlayerInfo == NULL )
        return;

	char szInfo [ MAX_PATH ] = "";

	//sprintf ( szInfo, "User Logout: %s", pPlayerInfo->strPlayerName );
	//MultiplayerDisplayDebug ( szInfo );
	//MessageBox ( NULL, szInfo, "info", MB_OK );

	//dbPrint ( szInfo );

    SendDestroyPlayerMsgToAll ( pPlayerInfo );

	// free surely
	SAFE_DELETE_ARRAY ( pPlayerInfo );

    // Update the number of active players, and 
    // post a message to the dialog thread to update the 
    // UI.  This keeps the DirectPlay message handler 
    // from blocking
    InterlockedDecrement ( &g_lNumberOfActivePlayers );
}

HRESULT SendDestroyPlayerMsgToAll ( APP_PLAYER_INFO* pPlayerInfo )
{
	g_SendData.dwDword = pPlayerInfo->dpnidPlayer;
	g_SendData.dwFlags  = DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED | DPNSEND_PRIORITY_HIGH;
	SendMessage ( eTypeDestroyPlayer );
	for ( DWORD i = 0; i < g_PlayerList.size ( ); i++ )
	{
		if ( g_PlayerList [ i ].dpnidPlayer == pPlayerInfo->dpnidPlayer )
		{
			//g_PlayerList.erase ( &g_PlayerList [ i ] );
			g_PlayerList.erase ( g_PlayerList.begin ( ) + i );
			break;
		}
	}
    return S_OK;
}

void TerminateSession ( void* pBuffer )
{
	PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
	pTerminateSessionMsg = ( PDPNMSG_TERMINATE_SESSION ) pBuffer;
//	MultiplayerDisconnect(); - inside a callback i think crashes it
	g_iConnection = 0;
}

int MultiplayerGetConnection ( void )
{
	return g_iConnection;
}


HRESULT WINAPI DirectPlayMessageHandler ( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
	switch ( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
			PLAYER_LOCK();
			CreatePlayer ( pMsgBuffer );
			PLAYER_UNLOCK();
        }
		break;

		case DPN_MSGID_DESTROY_PLAYER:
        {
			PLAYER_LOCK();
            DestroyPlayer ( pMsgBuffer );
			PLAYER_UNLOCK();
		}
		break;

		case DPN_MSGID_TERMINATE_SESSION:
        {
			PLAYER_LOCK();
			TerminateSession ( pMsgBuffer );
			PLAYER_UNLOCK();
			return S_OK;
        }
		break;

        case DPN_MSGID_RECEIVE:
        {
			PLAYER_LOCK();

			/*
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;
			// Validate incoming data: A malicious user could modify or create an application
            // to send bogus information; to help guard against logical errors and denial 
            // of service attacks, the size of incoming data should be checked against what
            // is expected.
            if( pReceiveMsg->dwReceiveDataSize < sizeof(GAMEMSG_GENERIC) )
                break;
            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            if( pMsg->dwType == GAME_MSGID_WAVE )
                SendWaveMessageToAll( pPlayerInfo->dpnidPlayer );
            break;
			*/

			// get plr id and message data ptr
			PDPNMSG_RECEIVE		pReceiveMsg = ( PDPNMSG_RECEIVE ) pMsgBuffer;
            APP_PLAYER_INFO*	pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;
			sMessage*			pMessage	= ( sMessage* ) pReceiveMsg->pReceiveData;

			// final message prepared for list
			sMessage FinalMessage;
			memset ( &FinalMessage, 0, sizeof ( FinalMessage ) );
			FinalMessage.cType=pMessage->cType;
			FinalMessage.wSize=pMessage->wSize;
			FinalMessage.dpnidPlayer=pMessage->dpnidPlayer;

			/*
			g_SendData.iInteger = pPlayerAbout->dpnidPlayer;
			g_SendData.dwFlags  = DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED;
			strcpy ( g_SendData.szString, pPlayerAbout->strPlayerName );
			SendMessage ( eTypeCreatePlayer );
			*/

			bool bAddMessage = true;

			switch ( pMessage->cType )
			{
				case eTypeDestroyPlayer:
				{
					DWORD dwID = 0;
					memcpy ( &dwID, &pMessage->pData, sizeof ( DWORD ) );
					for ( DWORD i = 0; i < g_PlayerList.size ( ); i++ )
					{
						if ( g_PlayerList [ i ].dpnidPlayer == dwID )
						{
							//g_PlayerList.erase ( &g_PlayerList [ i ] );
							g_PlayerList.erase ( g_PlayerList.begin ( ) + i );
							break;
						}
					}
					bAddMessage = false;
				}
				break;

				case eTypeCreatePlayer:
				{
					bool				bAdd   = true;
					sPlayer*			pPlayer = ( sPlayer* ) &pMessage->pData;
					APP_PLAYER_INFO		player   = { 0 };
					player.lRefCount   = 0;
					player.dpnidPlayer = pPlayer->dpnidPlayer;
					memcpy ( player.strPlayerName, pPlayer->strPlayerName, pMessage->wSize - sizeof ( DWORD ) );
					if ( g_PlayerID == 0 )
					{
						g_PlayerID = pPlayer->dpnidPlayer;
						g_SendData.dpnidPlayer = g_PlayerID;
					}
					g_PlayerList.push_back ( player );
					bAddMessage = false;
				}
				break;
			}
			
			if ( pMessage->cType == eTypeString )
			{
				char	szString1 [ MAX_PATH ]	= "";
				memcpy ( szString1, ( char* ) &pMessage->pData, pMessage->wSize );
				strcpy ( FinalMessage.szString, szString1 );
			}

			if ( pMessage->cType == eTypeMemblock )
			{
				FinalMessage.pDataEx = new char [ pMessage->wSize ];
				memcpy ( FinalMessage.pDataEx, ( char* ) &pMessage->pData, pMessage->wSize );
			}
			
			// add FinalMessage to queue
			if ( bAddMessage )
				g_MessageList.push ( FinalMessage );
			
			PLAYER_UNLOCK();
		}
		break;

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			HRESULT hr;
			PLAYER_LOCK();
			PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg;
			pEnumHostsResponseMsg = ( PDPNMSG_ENUM_HOSTS_RESPONSE ) pMsgBuffer;
			hr = CheckHostResponse ( pEnumHostsResponseMsg );
			PLAYER_UNLOCK();
			return hr;
		}
	}
	
    return S_OK;
}


int	MultiplayerMessageExists ( void )
{
	if ( g_MessageList.size ( ) )
		return 1;

	return 0;
}

void MultiplayerGetMessage ( void )
{
	// ensure current message has no residual data ptrs
	SAFE_DELETE_ARRAY ( g_CurrentMessage.pDataEx );

	// get top message
	PLAYER_LOCK();
	if ( g_MessageList.size ( ) )
	{
		// get next message
		g_CurrentMessage = g_MessageList.top ( );
		g_MessageList.pop ( );
	}
	else
	{
		// no message
		memset ( &g_CurrentMessage, 0, sizeof ( g_CurrentMessage ) );
		g_CurrentMessage.cType = -1;
	}
	PLAYER_UNLOCK();
}

DWORD MultiplayerGetMessagePlayerFrom ( void )
{
	return g_CurrentMessage.dpnidPlayer;
}

int	MultiplayerGetMessageType ( void )
{
	return g_CurrentMessage.cType;
}

int	MultiplayerGetMessageInteger ( void )
{
	int i = -1;
	memcpy( ( int*) &i, &g_CurrentMessage.pData, 4);

	return i;
}

float MultiplayerGetMessageFloat ( void )
{
	float i = -1;
	memcpy( ( float*) &i, &g_CurrentMessage.pData, 4);

	return i;
}

void MultiplayerGetMessageMemblock ( int iMemblock )
{
	// create memblock if does not exist
	if ( !g_Memblock_GetMemblockExist ( iMemblock ) )
		g_Memblock_MakeMemblock ( iMemblock, g_CurrentMessage.wSize );

	// use memblock that exists
	if ( g_Memblock_GetMemblockExist ( iMemblock ) )
	{
		char* pMemblockData = ( char* ) g_Memblock_GetMemblockPtr ( iMemblock );
		if ( pMemblockData )
		{
			if ( g_CurrentMessage.pDataEx )
			{
				memcpy ( pMemblockData, ( char* ) g_CurrentMessage.pDataEx, g_CurrentMessage.wSize );
				delete [ ] g_CurrentMessage.pDataEx;
				g_CurrentMessage.pDataEx = NULL;
			}
		}
	}
}

DWORD MultiplayerGetMessageString ( DWORD pDestStr )
{
	//strcpy ( szString, g_CurrentMessage.szString );

	//delete [ ] g_CurrentMessage.pDataEx;

	//memcpy ( szString, ( char* ) &g_CurrentMessage.pData, g_CurrentMessage.dwSize );

	return ( DWORD ) SetupString ( g_CurrentMessage.szString );
}

int MultiplayerGetQueueSize ( int iType )
{
	DWORD dwQueueSize, dwMsgWaiting;
	dwQueueSize=0;
	dwMsgWaiting=0;
	DWORD dwValue=0;
	if ( g_pDPClient )
	{
		g_pDPClient->GetSendQueueInfo( &dwMsgWaiting, &dwQueueSize, DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW );
	}
	if ( g_pDPServer )
	{
		int iTotal = g_PlayerList.size ( );
		for ( int iID=0; iID<iTotal; iID++ )
		{
			DWORD dwLocalMsg=0;
			DWORD dwLocalSize=0;
			g_pDPServer->GetSendQueueInfo( g_PlayerList [ iID ].dpnidPlayer, &dwLocalMsg, &dwLocalSize, DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW );
			dwMsgWaiting += dwLocalMsg;
			dwQueueSize += dwLocalSize;
		}
	}
	if ( iType==0 ) dwValue = dwMsgWaiting;
	if ( iType==1 ) dwValue = dwQueueSize;
	return (int)dwValue;
}

//-----------------------------------------------------------------------------
// Name: SendCreatePlayerMsg
// Desc: Send the target player a creation message about the player identified
//       in the APP_PLAYER_INFO struct.
//-----------------------------------------------------------------------------
HRESULT SendCreatePlayerMsg( APP_PLAYER_INFO* pPlayerAbout, DPNID dpnidTarget )
{
	g_SendData.dwDword = pPlayerAbout->dpnidPlayer;
	g_SendData.dwFlags = DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED | DPNSEND_PRIORITY_HIGH;
	strcpy ( g_SendData.szString, pPlayerAbout->strPlayerName );
	SendMessageTo ( dpnidTarget, eTypeCreatePlayer );
	return S_OK;
}



    
//-----------------------------------------------------------------------------
// Name: SendWorldStateToNewPlayer
// Desc: Send the world state to the new player.  For this sample, it is just
//       "create player" message for every connected player
//-----------------------------------------------------------------------------
HRESULT SendWorldStateToNewPlayer( DPNID dpnidNewPlayer )
{
	HRESULT hr;
	DWORD dwNumPlayers = 0;
	DPNID* aPlayers = NULL;

	{
		//g_SendData.dwDword = dpnidNewPlayer;
		//g_SendData.dwFlags = DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED;
		//strcpy ( g_SendData.szString, g_PlayerList [ i ].strPlayerName );
		//SendMessage ( eTypeCreatePlayer );
		
		for( ; ; )
		{
			hr = g_pDPServer->EnumPlayersAndGroups( aPlayers, &dwNumPlayers, DPNENUM_PLAYERS );
			
			if( SUCCEEDED(hr) )
				break;

			SAFE_DELETE_ARRAY( aPlayers );

			//if( FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL )
			//	return DXTRACE_ERR_MSGBOX( TEXT("EnumPlayersAndGroups"), hr );

			aPlayers = new DPNID[ dwNumPlayers ];

			//if( NULL == aPlayers )
			//	return DXTRACE_ERR_MSGBOX( TEXT("SendWorldStateToNewPlayer"), E_OUTOFMEMORY );
		}

		// For each player, send a "create player" message to the new player
		for( DWORD i = 0; i<dwNumPlayers; i++ )
		{
			APP_PLAYER_INFO* pPlayerInfo = NULL;

			// Don't send a create msg to the new player about itself.  This will 
			// be already done when we sent one to DPNID_ALL_PLAYERS_GROUP
			if( aPlayers[i] == dpnidNewPlayer )
			{
				

				continue;  
			}

			// Get the player context accosicated with this DPNID
			hr = g_pDPServer->GetPlayerContext( aPlayers[i], (LPVOID*) &pPlayerInfo, 0 );

			// Ignore this player if we can't get the context
			if( pPlayerInfo == NULL || FAILED(hr) )
				continue; 

			SendCreatePlayerMsg( pPlayerInfo, dpnidNewPlayer );
		}

		SAFE_DELETE_ARRAY( aPlayers );
	}

    return S_OK;
}


HRESULT ConvertAnsiStringToWideCch ( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                                       wstrDestination, cchDestChar );
    wstrDestination[cchDestChar-1] = 0;
    
    if( nResult == 0 )
        return E_FAIL;

    return S_OK;
}

HRESULT ConvertWideStringToAnsiCch ( CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar )
{
    if( strDestination==NULL || wstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = WideCharToMultiByte( CP_ACP, 0, wstrSource, -1, strDestination, 
                                       cchDestChar*sizeof(CHAR), NULL, NULL );
    strDestination[cchDestChar-1] = 0;
    
    if( nResult == 0 )
        return E_FAIL;
    return S_OK;
}

HRESULT CheckHostResponse ( PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg )
{
	HRESULT hr		= S_OK;
    BOOL    bFound	= FALSE;
	TCHAR	strName [ MAX_PATH ];

	const DPN_APPLICATION_DESC* pResponseMsgAppDesc = pEnumHostsResponseMsg->pApplicationDescription;
	ConvertWideStringToAnsiCch ( strName, pResponseMsgAppDesc->pwszSessionName, MAX_PATH );
	if ( pResponseMsgAppDesc->guidApplication == g_guidApp )
	{
		/* leechange - 230605 - mike this seems round the houses, why not just collect the host that fits
		// and connect to it after the ENUMHOSTS is complete?!
		sHost host    = { 0 };
		bool  bExists = false;
		host.pHost = pResponseMsgAppDesc;
		strcpy ( host.szName, strName );
		for ( int i = 0; i < g_HostList.size ( ); i++ )
		{
			if ( host.pHost->guidApplication == g_HostList [ i ].pHost->guidApplication )
			{
				if ( FAILED ( hr = pEnumHostsResponseMsg->pAddressSender->Duplicate ( &g_pHostAddr ) ) )
				{
				}
				if ( FAILED ( hr = pEnumHostsResponseMsg->pAddressDevice->Duplicate ( &g_pDeviceAddr ) ) )
				{
				}

				MultiplayerDisplayDebug ( "Found Server..." );

				{
					WCHAR wszPeerName [ MAX_PATH ];
					ConvertAnsiStringToWideCch ( wszPeerName, g_szName, MAX_PATH );

					DPN_PLAYER_INFO dpPlayerInfo;
					ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
					dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
					dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
					dpPlayerInfo.pwszName = wszPeerName;

					if( FAILED( hr = g_pDPClient->SetClientInfo( &dpPlayerInfo, NULL, NULL, DPNOP_SYNC ) ) )
					{

					}

					hr = g_pDPClient->Connect( pResponseMsgAppDesc,       // the application desc
                               g_pHostAddr,      // address of the host of the session
                               g_pDeviceAddr,    // address of the local device the enum responses were received on
                               NULL, NULL,                          // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
                               NULL, 0,                             // user data, user data size
                               NULL, &m_hConnectAsyncOp,            // async context, async handle,
							   0 ); // flags

					// free when connected
					SAFE_RELEASE(g_pHostAddr);
					SAFE_RELEASE(g_pDeviceAddr);

					MultiplayerDisplayDebug ( "Connected..." );

					// indicate connection made
					g_iConnection = 2;
				}

				bExists = true;
				break;
			}
		}

		if ( !bExists )
		{
			MultiplayerDisplayDebug ( "" );
			MultiplayerDisplayDebug ( strName );
			g_HostList.push_back ( host );
		}
		*/

		// Duplicate host and device addresses (ony need first host we find)
		if ( g_pHostAddr==NULL )
		{
			// get first host that suits
			if ( FAILED ( hr = pEnumHostsResponseMsg->pAddressSender->Duplicate ( &g_pHostAddr ) ) )
			{
			}
			if ( FAILED ( hr = pEnumHostsResponseMsg->pAddressDevice->Duplicate ( &g_pDeviceAddr ) ) )
			{
			}

			// copy application data for later connection
			memcpy ( &g_ResponseMsgAppDesc, pResponseMsgAppDesc, sizeof ( g_ResponseMsgAppDesc ) );

			// return from the message callback
			return DPN_OK;
		}
	}
	return S_OK;
}

HRESULT ConnectAfterEnum ( void )
{
	// work vars
	HRESULT hr = S_OK;

	// send client info to...
	WCHAR wszPeerName [ MAX_PATH ];
	ConvertAnsiStringToWideCch ( wszPeerName, g_szName, MAX_PATH );
	DPN_PLAYER_INFO dpPlayerInfo;
	ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wszPeerName;
	if( FAILED( hr = g_pDPClient->SetClientInfo( &dpPlayerInfo, NULL, NULL, DPNOP_SYNC ) ) )
	{
		// if fails, so what, of dear
	}

	// connect to server
	hr = g_pDPClient->Connect( &g_ResponseMsgAppDesc,       // the application desc
							   g_pHostAddr,      // address of the host of the session
							   g_pDeviceAddr,    // address of the local device the enum responses were received on
							   NULL, NULL,                          // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
							   NULL, 0,                             // user data, user data size
							   NULL, &m_hConnectAsyncOp,            // async context, async handle,
							   0 ); // flags

	// free when connected
	SAFE_RELEASE(g_pHostAddr);
	SAFE_RELEASE(g_pDeviceAddr);

	// indicate connection made
	g_iConnection = 2;

	return S_OK;
}

/*
DWORD MultiplayerGetIPAddress ( void )
{
	WORD		wVersionRequested;
	PHOSTENT	hostinfo;
    WSADATA		wsaData;
    char		szName [ 255 ] = "";
	char*		IP             = NULL;
    
	wVersionRequested = MAKEWORD ( 2, 0 );
	
	if ( WSAStartup ( wVersionRequested, &wsaData ) == 0 )
	{
		if ( gethostname ( szName, sizeof ( szName ) ) == 0 )
		{
			if ( ( hostinfo = gethostbyname ( szName ) ) != NULL )
			{
				IP = inet_ntoa ( *( struct in_addr* ) *hostinfo->h_addr_list );
				
				strcpy ( szName, IP );
			}
		}
		
		WSACleanup ( );
	}
	else
	{
		return 0;
	}
	
	return ( DWORD ) SetupString ( szName );
}
*/

//-----------------------------------------------------------------------------
// Name: GetHostnamePortString
// Desc: Stores the hostname and port number of the given DirectPlay 
//       address in the provided string.
//-----------------------------------------------------------------------------
HRESULT GetHostnamePortString( TCHAR* str, LPDIRECTPLAY8ADDRESS pAddress, DWORD dwBufferLen )
{
    HRESULT hr = S_OK;

    // Sanity check
    if( NULL == str || NULL == pAddress )
        return E_FAIL;

    // Get the hostname string from the DirectPlay address
    WCHAR wstrHostname[ 256 ] = {0};
    DWORD dwSize = sizeof(wstrHostname);
    DWORD dwDataType = 0;
    hr = pAddress->GetComponentByName( L"hostname", wstrHostname, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Convert from wide character to generic
    TCHAR strHostname[ 256 ] = {0};
    ConvertWideStringToAnsiCch( strHostname, wstrHostname, 256 );

    // Get the port value from the DirectPlay address
    DWORD dwPort = 0;
    dwSize = sizeof(DWORD);
    hr = pAddress->GetComponentByName( L"port", &dwPort, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Copy the address string and null terminate the result
    _sntprintf( str, dwBufferLen, TEXT("%s:%d"), strHostname, dwPort );
    str[ dwBufferLen-1 ] = TEXT('\0');

    return S_OK;
}


DWORD MultiplayerGetIPAddress ( void )
{
	// work vars
	HRESULT hr = S_OK; 
    TCHAR strStatus[ 1024 ] = {0};
    TCHAR strAddress[ 256 ] = {0};
    DWORD dwNumAddresses = 0;
    LPDIRECTPLAY8ADDRESS *rpAddresses = NULL;
    DWORD i = 0;

	// Clear IP return string
    _tcsncpy( strStatus, TEXT(""), 1023 );
//    _tcsncpy( strStatus, TEXT("   "), 1023 );
//   strStatus[ 1023 ] = 0;

	// If Server created
    if( g_pDPServer )
    {
        // Session has started
        dwNumAddresses = 0;

        // Determine the buffer size needed to hold the addresses
        hr = g_pDPServer->GetLocalHostAddresses( rpAddresses, &dwNumAddresses, 0 );
        if( DPNERR_BUFFERTOOSMALL != hr )
            goto LCleanReturn;  

        // Allocate the array
        rpAddresses = new LPDIRECTPLAY8ADDRESS[ dwNumAddresses ];
        if( NULL == rpAddresses )
        {
            hr = E_OUTOFMEMORY;
            goto LCleanReturn;
        }

		// Clear addresses buffer
        ZeroMemory( rpAddresses, dwNumAddresses * sizeof(LPDIRECTPLAY8ADDRESS) );

        // Retrieve the addresses
        hr = g_pDPServer->GetLocalHostAddresses( rpAddresses, &dwNumAddresses, 0 );
        if( FAILED(hr) )
            goto LCleanReturn;  
    }
    else
    {
		// Use WinSock to get IP (not used in FPSC-MP-system, ie 'host' gets IP after server created)
		WORD		wVersionRequested;
		PHOSTENT	hostinfo;
		WSADATA		wsaData;
		char		szName [ 256 ] = "";
		char*		IP             = NULL;
		wVersionRequested = MAKEWORD ( 2, 0 );
		if ( WSAStartup ( wVersionRequested, &wsaData ) == 0 )
		{
			if ( gethostname ( szName, sizeof ( szName ) ) == 0 )
			{
				if ( ( hostinfo = gethostbyname ( szName ) ) != NULL )
				{
					IP = inet_ntoa ( *( struct in_addr* ) *hostinfo->h_addr_list );
					
					strcpy ( szName, IP );
				}
			}
			WSACleanup ( );
		}
		else
		{
			return 0;
		}
		return ( DWORD ) SetupString ( szName );
    }
    

    // If we have addresses to report, determine a reasonable way to output them
    if( dwNumAddresses > 0 )
    {
        // Get the provider from the DirectPlay address
        GUID guidSP;
        hr = rpAddresses[0]->GetSP( &guidSP );
        if( FAILED(hr) ) goto LCleanReturn;

		// Construct description string
        TCHAR strProvider[ 256 ] = {0};
        if( CLSID_DP8SP_IPX == guidSP )
            _tcsncpy( strProvider, TEXT(" via IPX"), 256 );
        else if( CLSID_DP8SP_TCPIP == guidSP )
            _tcsncpy( strProvider, TEXT(" via TCP/IP"), 256 );
        else if( CLSID_DP8SP_MODEM == guidSP )
            _tcsncpy( strProvider, TEXT(" via Modem"), 256 );
        else if( CLSID_DP8SP_SERIAL == guidSP )
            _tcsncpy( strProvider, TEXT(" via Serial"), 256 );
        else if( CLSID_DP8SP_BLUETOOTH == guidSP )
            _tcsncpy( strProvider, TEXT(" via Bluetooth"), 256 );
        
        // If we are using TCP/IP or IPX, display the address list
        if( CLSID_DP8SP_IPX == guidSP || CLSID_DP8SP_TCPIP == guidSP )
        {
			// find an IP that is not local, else use local
            for( i=0; i < dwNumAddresses; i++ )
            {
                // Get the IP address
                if( FAILED( GetHostnamePortString( strAddress, rpAddresses[ i ], 256 ) ) )
                    break;

				// copy IP-address to string
			    _tcsncpy( strStatus, strAddress, 1023 );
                strStatus[ 1023 ] = 0;

				// if IP address not the local one, leave with it as likely main IP address for machine
				if ( _tcsncmp ( strStatus, _TEXT("192.168."), 8 )!=NULL )
					break;
            }

			/* old way to collect entire list
            // Append the addresses to the status text
            for( i=0; i < dwNumAddresses; i++ )
            {
                // Get the IP address
                if( FAILED( GetHostnamePortString( strAddress, rpAddresses[ i ], 256 ) ) )
                    break;

				TCHAR* strSpacing = ( i != 0 ) ? TEXT(" ") : TEXT("");
                _tcsncat( strStatus, strSpacing, 1023 - lstrlen( strStatus ) );
                strStatus[ 1023 ] = 0;

                // Add the IP address string
                _tcsncat( strStatus, strAddress, 1023 - lstrlen( strStatus ) );
                strStatus[ 1023 ] = 0;
            }
			*/
        }
    }

// remove when find out why status text needs margin.
//    // Margin the status text
//    _tcsncat( strStatus, TEXT("   "), 1023 - lstrlen( strStatus ) );
//    strStatus[ 1023 ] = 0;
	
LCleanReturn:
    for( i=0; i < dwNumAddresses; i++ )
        SAFE_RELEASE( rpAddresses[i] );

    SAFE_DELETE_ARRAY( rpAddresses );

	// return IP address in string
	return ( DWORD ) SetupString ( strStatus );
}