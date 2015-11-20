
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPLAYER //////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES & DEFINES ///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <basetsd.h>
#include <dplay8.h>
#include <dpaddr.h>
#include <dxerr9.h>
#include <tchar.h>
#include <objbase.h>

#define DARKSDK	__declspec ( dllexport )
#define WIN32_LEAN_AND_MEAN

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS & STRUCTURES /////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct APP_PLAYER_INFO
{
	// information about players in the game

    LONG	lRefCount;
    DPNID	dpnidPlayer;
    char	strPlayerName [ MAX_PATH ];
};

struct sPlayer
{
	// internal player structure

    DPNID	dpnidPlayer;
    char	strPlayerName [ MAX_PATH ];
};

struct sHost
{
	//  host information

	const DPN_APPLICATION_DESC*	pHost;
	TCHAR						szName [ MAX_PATH ];
};

struct sMessage
{
	// the message data
	int		cType;						// type of data (4)
	DWORD	wSize;						// size of data (4)
	DPNID	dpnidPlayer;				// ID of player who sent the message (sizeof)
	void*	pData;						// actual data (4)

	char	szString [ MAX_PATH ];		// not used in the send process
	char*	pDataEx;
};

struct GAMEMSG_DESTROY_PLAYER
{
	// destroy player message

	DWORD dwType;
    DWORD dpnidPlayer;
};

struct sSendData
{
	// used for gathering data to send

	int		iInteger;
	DWORD	dwDword;
	float	fFloat;
	char	szString [ MAX_PATH ];
	DWORD	dwFlags;
	DPNID	dpnidPlayer;
	char*	pMemblock;
	DWORD	dwMemblockSize;
};

enum eSendType
{
	// type of data to send

	eTypeInteger,
	eTypeFloat,
	eTypeString,
	eTypeMemblock,
	eTypeDestroyPlayer,
	eTypeCreatePlayer,
};

enum eType
{
	// which mode we are in - server or client

	eServer,
	eClient
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

DARKSDK void			Constructor							        ( void );
DARKSDK void			Destructor									( void );
DARKSDK void			ReceiveCoreDataPtr							( LPVOID pCore );
DARKSDK	void			SetErrorHandler								( LPVOID pErrorHandlerPtr );

DARKSDK void			MultiplayerSetDebugState					( int iState );
DARKSDK void			MultiplayerDisplayDebug						( char* szInformation );

DARKSDK void			MultiplayerCreateTCPServer					( char* szName );
DARKSDK void			MultiplayerCreateTCPServer					( char* szName, DWORD dwPort );
DARKSDK void			MultiplayerConnectToTCPServer				( char* szName, char* szIP );
DARKSDK void			MultiplayerConnectToTCPServer				( char* szName, char* szIP, DWORD dwPort );
DARKSDK void			MultiplayerDisconnect						( void );

DARKSDK void			MultiplayerStartVoiceServer					( void );

DARKSDK void			MultiplayerSendFromPlayer					( DWORD dwID );
DARKSDK void			MultiplayerSendToPlayer						( int iSendToIndex );
DARKSDK void			MultiplayerSendMessageInteger				( int iData );
DARKSDK void			MultiplayerSendMessageFloat					( float fData );
DARKSDK void			MultiplayerSendMessageString				( char* szString );
DARKSDK void			MultiplayerSendMessageMemblock				( int iMemblock );
DARKSDK void			MultiplayerSendMessageInteger				( int iData, int iGUA );
DARKSDK void			MultiplayerSendMessageFloat					( float fData, int iGUA );
DARKSDK void			MultiplayerSendMessageString				( char* szString, int iGUA );
DARKSDK void			MultiplayerSendMessageMemblock				( int iMemblock, int iGUA );
DARKSDK void			MultiplayerSendMessageMemblock				( int iMemblock, int iGUA, int iSize );

DARKSDK int				MultiplayerMessageExists					( void );
DARKSDK void			MultiplayerGetMessage						( void );
DARKSDK int				MultiplayerGetMessageType					( void );
DARKSDK int				MultiplayerGetMessageInteger				( void );
DARKSDK float			MultiplayerGetMessageFloat					( void );
DARKSDK DWORD			MultiplayerGetMessageString					( DWORD pDestStr );
DARKSDK void			MultiplayerGetMessageMemblock				( int iMemblock );
DARKSDK DWORD			MultiplayerGetMessagePlayerFrom				( void );

DARKSDK int				MultiplayerGetPlayerCount					( void );
DARKSDK DWORD			MultiplayerGetPlayerName					( DWORD pDestStr, int iID );
DARKSDK DWORD			MultiplayerGetPlayerID						( int iID );
DARKSDK DWORD			MultiplayerGetID							( void );

DARKSDK	DWORD			MultiplayerGetIPAddress						( void );

DARKSDK	int				MultiplayerGetConnection					( void );
DARKSDK int				MultiplayerGetQueueSize						( int );

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void			CreatePlayer					( void* pBuffer );
void			DestroyPlayer					( void* pBuffer );
void			TerminateSession				( void* pBuffer );
HRESULT WINAPI	DirectPlayMessageHandler		( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
HRESULT			SendCreatePlayerMsg				( APP_PLAYER_INFO* pPlayerInfo, DPNID dpnidTarget );
HRESULT			SendWorldStateToNewPlayer		( DPNID dpnidPlayer );
HRESULT			ConvertAnsiStringToWideCch		( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );
HRESULT			ConvertWideStringToAnsiCch		( CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar );
HRESULT			CheckHostResponse				( PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg );
HRESULT			ConnectAfterEnum				( void );
void			SendMessage						( eSendType eDataType );
void			SendMessageTo					( DPNID dpnidTarget, eSendType eDataType );
HRESULT			SendDestroyPlayerMsgToAll		( APP_PLAYER_INFO* pPlayerInfo );

bool			CheckString						( char* szString );

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////