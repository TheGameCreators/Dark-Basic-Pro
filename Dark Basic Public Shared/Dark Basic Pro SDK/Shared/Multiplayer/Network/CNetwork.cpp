// include definition
#define _CRT_SECURE_NO_DEPRECATE
#include "cnetwork.h"

// Defines for this code
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

// GUID Value
GUID DPCREATE_GUID = { 0x7d2f0893, 0xb73, 0x48ff, { 0xae, 0x7f, 0x66, 0x83, 0xec, 0x72, 0x6b, 0x21 } };

// External Data
extern DWORD	gInternalErrorCode;

// Internal Data
int				netTypeNum;
char			*netType[MAX_CONNECTIONS];
GUID			netTypeGUID[MAX_CONNECTIONS];
VOID			*gConnection[MAX_CONNECTIONS];
int				netConnectionTypeSelected;

int				netSessionPos;
DPSessionInfo	netSession[MAX_SESSIONS];
char			*netSessionName[MAX_SESSIONS];

int				netPlayersPos;
DPID			netPlayersID[MAX_PLAYERS];
char			*netPlayersName[MAX_PLAYERS];

int				gNetQueueCount=0;
CNetQueue*		gpNetQueue=NULL;

int				gGameSessionActive=0;
bool			gbSystemSessionLost=false;
int				gSystemPlayerCreated=0;
int				gSystemPlayerDestroyed=0;
int				gSystemSessionIsNowHosting=0;


std::vector < QueuePacketStruct > g_NewNetQueue;

////////////////////////////////////////////////////////////////////////////////
// Function Name: CNetwork
// Description:	  Default constructor for class, is called upon creation of
//                instance
// Parameters:    none
// Example:		  none
////////////////////////////////////////////////////////////////////////////////
CNetwork::CNetwork()
{
	HRESULT hr;
	int temp = 0;

	m_pDP=NULL;					// main DirectPlay object
	m_LocalPlayerDPID=0;		// player id
	m_hDPMessageEvent=NULL;		// player event info
	m_pvDPMsgBuffer=NULL;		// message buffer
	m_dwDPMsgBufferSize=0;	// message buffer size
	selectedNetSession=0;

	memset(netType, NULL, sizeof(netType));
	memset(netTypeGUID, NULL, sizeof(netTypeGUID));
	netTypeNum = 0;

	memset(gConnection, NULL, sizeof(gConnection));
	netConnectionTypeSelected=0;

	netPlayersPos = 0;
	
	// create the main DirectPlay interface
	if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL, IID_IDirectPlay4A, (VOID**)&m_pDP)))
		gInternalErrorCode=18501;

	// find out all of the available network connections
	// by setting up this callback function, remember that
	// because it is a callback function that it cannot
	// be included in the class, it has to be global
	if ( m_pDP )
	{
		if (FAILED(hr = m_pDP->EnumConnections(NULL, (LPDPENUMCONNECTIONSCALLBACK)StaticGetConnection, NULL, DPCONNECTION_DIRECTPLAY)))
			gInternalErrorCode=18502;
	}

	netSessionPos = 0;

	/*
	// mike - 300305
	// Global Creation of New Queue
	if(gpNetQueue)
	{
		gpNetQueue->Clear();
		gNetQueueCount=0;
	}
	else
	{
		gpNetQueue = new CNetQueue;
		gNetQueueCount=0;
	}
	*/

	// clear connection ptr
	Connection=NULL;

	// Clear flags
	gGameSessionActive=0;
	gbSystemSessionLost=false;
	gSystemPlayerCreated=0;
	gSystemPlayerDestroyed=0;
	gSystemSessionIsNowHosting=0;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Name: ~CNetwork
// Description:	  Default destructor for class, is called upon deletion of
//                instance
// Parameters:    none
// Example:		  none
////////////////////////////////////////////////////////////////////////////////
CNetwork::~CNetwork()
{
	int temp;

	// delete all of the arrays
	for (temp = 0; temp < MAX_PLAYERS; temp++)
		SAFE_DELETE_ARRAY(netPlayersName[temp]);

	for (temp = 0; temp < MAX_CONNECTIONS; temp++)
		SAFE_DELETE_ARRAY(netType[temp]);

	for (temp = 0; temp < MAX_SESSIONS; temp++)
		SAFE_DELETE_ARRAY(netSessionName[temp]);

	for (temp = 0; temp < MAX_CONNECTIONS; temp++)
		SAFE_DELETE_ARRAY(gConnection[temp]);

	// set them all to null
	memset(netPlayersName, NULL, sizeof(netPlayersName));
	memset(netType,        NULL, sizeof(netType));
	memset(netTypeGUID,    NULL, sizeof(netTypeGUID));
	memset(netSessionName, NULL, sizeof(netSessionName));
	memset(gConnection,    NULL, sizeof(gConnection));

	if(m_pDP)
	{
		if(m_LocalPlayerDPID!=0)
		{
			m_pDP->DestroyPlayer(m_LocalPlayerDPID);
			m_LocalPlayerDPID=0;
		}
		m_pDP->Close();

		
	}

	SAFE_RELEASE(m_pDP);

	/*
	// mike - 300305
	if(gpNetQueue)
	{
		delete gpNetQueue;
		gpNetQueue=NULL;
	}
	*/

	// free allocated connection memory
	if(Connection)
	{
		GlobalFreePtr(Connection);
		Connection=NULL;
	}

	// Clear flags
	gGameSessionActive=0;
	gbSystemSessionLost=false;
	gSystemPlayerCreated=0;
	gSystemPlayerDestroyed=0;
	gSystemSessionIsNowHosting=0;
}
////////////////////////////////////////////////////////////////////////////////

int CNetwork::SetNetConnections(int index)
{
	if ( !m_pDP )
		return 0;

	// Find an IPX, or TCP/IP if -1 (better for auto-connection than serial or modem)
	if(index==-1)
	{
		index=0;
		int iBetterIndex=-1;
		for(int t=0; t<netTypeNum; t++)
		{
			if(netTypeGUID[t]==DPSPGUID_IPX) iBetterIndex=t;
			if(netTypeGUID[t]==DPSPGUID_TCPIP && iBetterIndex==-1) iBetterIndex=t;
		}
		if(iBetterIndex!=-1) index=iBetterIndex;
	}

	HRESULT hr;
	if (FAILED (hr = m_pDP->InitializeConnection(gConnection[index], 0)))
		return 0;
	
	// Later used in find session...
	netConnectionTypeSelected=index;

	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// gamename = session name
// name = player name
// players = max. players
// flags = session protocols 
////////////////////////////////////////////////////////////////////////////////
int CNetwork::CreateNetGame(char *gamename, char *name, int players, DWORD flags)
{
	HRESULT hr;	
	DPSESSIONDESC2 dpsd;
	DPNAME         dpname;

	ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize           = sizeof(dpsd);
    dpsd.guidApplication  = DPCREATE_GUID;
	dpsd.lpszSessionNameA = gamename;
    dpsd.dwMaxPlayers     = players;
    dpsd.dwFlags          = flags | DPSESSION_DIRECTPLAYPROTOCOL;
	if (FAILED(hr = m_pDP->Open(&dpsd, DPOPEN_CREATE)))
	{
		if(hr==DPERR_ALREADYINITIALIZED) gInternalErrorCode=18531;
		if(hr==DPERR_CANTCREATEPLAYER) gInternalErrorCode=18532;
		if(hr==DPERR_TIMEOUT) gInternalErrorCode=18533;
		if(hr==DPERR_USERCANCEL) gInternalErrorCode=18534;
		if(hr==DPERR_NOSESSIONS) gInternalErrorCode=18535;
		return 0;
	}

	ZeroMemory(&dpname, sizeof(DPNAME));
    dpname.dwSize         = sizeof(DPNAME);
    dpname.lpszShortNameA = name;
	
	if ( !m_pDP )
	{
		gInternalErrorCode=18504;
		return 0;
	}

    if (FAILED(hr = m_pDP->CreatePlayer(&m_LocalPlayerDPID, &dpname, m_hDPMessageEvent, NULL, 0, DPPLAYER_SERVERPLAYER)))
	{
		gInternalErrorCode=18504;
		return 0;
	}

	return 1;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ipaddress = ip address of session
// name = player name
////////////////////////////////////////////////////////////////////////////////
int CNetwork::JoinNetGame(char *name)
{
	DPSESSIONDESC2 dpsd;
	DPNAME         dpname;

	ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize           = sizeof(dpsd);
    dpsd.guidApplication  = DPCREATE_GUID;
	dpsd.dwFlags          = 0;

	// get all available sessions
	HRESULT hr;

	if ( !m_pDP )
	{
		gInternalErrorCode=18512;
		return 0;
	}

	if (FAILED(hr = m_pDP->EnumSessions(&dpsd, 0, (LPDPENUMSESSIONSCALLBACK2)StaticGetSessions, NULL, 0)))
	{
		gInternalErrorCode=18512;
		return 0;
	}

	// enum sessions changes the dpsd struct, for the demo
	// just select the first session that was found
	ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize           = sizeof(dpsd);
    dpsd.guidApplication  = DPCREATE_GUID;
	dpsd.guidInstance     = netSession[selectedNetSession].guidSession;
	dpsd.dwFlags          = 0;

	// attempt to join the session
	while(1)
	{
		hr = m_pDP->Open(&dpsd, DPOPEN_JOIN);
		if(hr==DP_OK)
			break;

		if(hr!=DPERR_CONNECTING)
		{
			gInternalErrorCode=18549;
			if(hr==DPERR_ACCESSDENIED) gInternalErrorCode=18525;
			if(hr==DPERR_ALREADYINITIALIZED  ) gInternalErrorCode=18526;
			if(hr==DPERR_AUTHENTICATIONFAILED  ) gInternalErrorCode=18527;
			if(hr==DPERR_CANNOTCREATESERVER  ) gInternalErrorCode=18528;
			if(hr==DPERR_CANTCREATEPLAYER ) gInternalErrorCode=18529;
			if(hr==DPERR_CANTLOADCAPI  ) gInternalErrorCode=18530;
			if(hr==DPERR_CANTLOADSECURITYPACKAGE  ) gInternalErrorCode=18531;
			if(hr==DPERR_CANTLOADSSPI  ) gInternalErrorCode=18532;
			if(hr==DPERR_CONNECTING  ) gInternalErrorCode=18533;
			if(hr==DPERR_CONNECTIONLOST  ) gInternalErrorCode=18534;
			if(hr==DPERR_ENCRYPTIONFAILED  ) gInternalErrorCode=18535;
			if(hr==DPERR_ENCRYPTIONNOTSUPPORTED  ) gInternalErrorCode=18536;
			if(hr==DPERR_INVALIDFLAGS  ) gInternalErrorCode=18537;
			if(hr==DPERR_INVALIDPARAMS  ) gInternalErrorCode=18538;
			if(hr==DPERR_INVALIDPASSWORD  ) gInternalErrorCode=18539;
			if(hr==DPERR_LOGONDENIED  ) gInternalErrorCode=18540;
			if(hr==DPERR_NOCONNECTION  ) gInternalErrorCode=18541;
			if(hr==DPERR_NONEWPLAYERS  ) gInternalErrorCode=18542;
			if(hr==DPERR_NOSESSIONS  ) gInternalErrorCode=18543;
			if(hr==DPERR_SIGNFAILED  ) gInternalErrorCode=18544;
			if(hr==DPERR_TIMEOUT  ) gInternalErrorCode=18545;
			if(hr==DPERR_UNINITIALIZED  ) gInternalErrorCode=18546;
			if(hr==DPERR_USERCANCEL  ) gInternalErrorCode=18547;
			if(hr==DPERR_CANTCREATEPLAYER) { gInternalErrorCode=18548; return 3; }
			return 0;
		}
	}
	
	// create the player
	ZeroMemory(&dpname, sizeof(DPNAME));
    dpname.dwSize         = sizeof(DPNAME);
    dpname.lpszShortNameA = name;
	if (FAILED(hr = m_pDP->CreatePlayer(&m_LocalPlayerDPID, &dpname, m_hDPMessageEvent, NULL, 0, 0)))
	{
		gInternalErrorCode=18514;
		return 2;
	}

	return 1;
}
////////////////////////////////////////////////////////////////////////////////

void CNetwork::CloseNetGame(void)
{
	int temp;

	// delete all of the arrays
	for (temp = 0; temp < MAX_PLAYERS; temp++)
		SAFE_DELETE_ARRAY(netPlayersName[temp]);

	for (temp = 0; temp < MAX_SESSIONS; temp++)
		SAFE_DELETE_ARRAY(netSessionName[temp]);

	// set them all to null
	memset(netPlayersName, NULL, sizeof(netPlayersName));
	memset(netSessionName, NULL, sizeof(netSessionName));

	if(m_pDP)
	{
		if(m_LocalPlayerDPID!=0)
		{
			m_pDP->DestroyPlayer(m_LocalPlayerDPID);
			m_LocalPlayerDPID=0;
		}
		m_pDP->Close();
	}
}

////////////////////////////////////////////////////////////////////////////////

DPID CNetwork::CreatePlayer(char* name)
{
	// create the player
	DPNAME			dpname;
	DPID			dpid; 

	ZeroMemory(&dpname, sizeof(DPNAME));
    dpname.dwSize         = sizeof(DPNAME);
    dpname.lpszShortNameA = name;

	if ( !m_pDP )
	{
		gInternalErrorCode=18523;
		return 0;
	}

	if(FAILED(m_pDP->CreatePlayer(&dpid, &dpname, m_hDPMessageEvent, NULL, 0, 0)))
	{
		gInternalErrorCode=18523;
		return 0;
	}

	return dpid;
}

bool CNetwork::DestroyPlayer(DPID dpid)
{
	if ( !m_pDP )
	{
		gInternalErrorCode=18524;
		return false;
	}

	if(dpid!=DPID_SERVERPLAYER)
	{
		if(FAILED(m_pDP->DestroyPlayer(dpid)))
		{
			gInternalErrorCode=18524;
			return false;
		}
		return true;
	}
	else
		return false;
}

int CNetwork::SetNetSessions(int num)
{
	selectedNetSession = num;
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Function Name: INT_PTR CALLBACK CNetwork::GetConnection(LPCGUID pguidSP, VOID* pConnection, DWORD dwConnectionSize, LPCDPNAME pName, DWORD dwFlags, VOID* pvContext)
// Description:	  Finds all available network connections
// Parameters:    
// Example:		  none as this is only called internally
////////////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK CNetwork::StaticGetConnection(LPCGUID pguidSP, VOID* pConnection, DWORD dwConnectionSize, LPCDPNAME pName, DWORD dwFlags, VOID* pvContext)
{
	HRESULT			hr;
	LPDIRECTPLAY4A  pDP;
	VOID*			pConnectionBuffer = NULL;

	if (FAILED (hr = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL, IID_IDirectPlay4A, (VOID**)&pDP)))
        return FALSE;

	// attempt to init the connection
	if (FAILED (hr = pDP->InitializeConnection(pConnection, 0)))
	{
		SAFE_RELEASE(pDP);
		return 1;
	}

	pConnectionBuffer = new BYTE [dwConnectionSize];
    if (pConnectionBuffer == NULL)
        return FALSE;

    memcpy (pConnectionBuffer, pConnection, dwConnectionSize);
	gConnection[netTypeNum] = pConnectionBuffer;

	netType[netTypeNum] = new char[256];
	strcpy(netType[netTypeNum], (CHAR *)pName->lpszShortNameA);
	netTypeGUID[netTypeNum] = *pguidSP;
	netTypeNum++;
	
	SAFE_RELEASE(pDP);

	return 1;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Name: INT_PTR CALLBACK CNetwork::GetSessions(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext)
// Description:	  finds all available sessions
// Parameters:    none
// Example:		  none as this is only called internally
////////////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK CNetwork::StaticGetSessions(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext)
{
	if (dwFlags & DPESC_TIMEDOUT)
	  return FALSE;
	
	netSession[netSessionPos].guidSession = pdpsd->guidInstance;

	sprintf(netSession[netSessionPos].szSession, "%s (%d/%d)", pdpsd->lpszSessionNameA, pdpsd->dwCurrentPlayers, pdpsd->dwMaxPlayers);
	
	netSessionName[netSessionPos] = new char[256];
	strcpy(netSessionName[netSessionPos], netSession[netSessionPos].szSession);

	// mike - 250604 - stores extra data
	netSession[netSessionPos].iPlayers = pdpsd->dwCurrentPlayers;
	netSession[netSessionPos].iMaxPlayers = pdpsd->dwMaxPlayers;

	netSessionPos++;
	
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Name: EXPORT int CNetwork::SendMsg(char *msg)
// Description:	  sends a text msg to all players
// Parameters:    char *msg, text to send
// Example:		  SendMsg("hello");
////////////////////////////////////////////////////////////////////////////////
int CNetwork::SendNetMsg(int player, tagNetData* pMsg)
{
	if ( !m_pDP )
		return 1;

	DWORD size = sizeof(int) + sizeof(DWORD) + pMsg->size;
	m_pDP->Send(m_LocalPlayerDPID, player, 0, pMsg, size);
	return 1;
}
int CNetwork::SendNetMsgGUA(int player, tagNetData* pMsg)
{
	if ( !m_pDP )
		return 1;

	DWORD size = sizeof(int) + sizeof(DWORD) + pMsg->size;
	m_pDP->Send(m_LocalPlayerDPID, player, DPSEND_GUARANTEED, pMsg, size);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Name: EXPORT int CNetwork::GetMsg(void)
// Description:	  Searches for any msg that have been sent
// Parameters:    none
// Example:		  GetMsg()
////////////////////////////////////////////////////////////////////////////////
bool gbProcessMessageElsewhere=false;
int CNetwork::AddAnyNewNetMsgToQueue(void)
{
	if ( !m_pDP )
		return S_FALSE;

	DPID    idFrom;
    DPID    idTo;
    void	*pvMsgBuffer;
    DWORD   dwMsgBufferSize;
    HRESULT hr;

    // Read all messages in queue
    dwMsgBufferSize = m_dwDPMsgBufferSize;
    pvMsgBuffer     = m_pvDPMsgBuffer;

	gbProcessMessageElsewhere=false;
	while(TRUE)
    {
        // See what's out there
        idFrom = 0;
        idTo   = 0;

		// get msg
        hr = m_pDP->Receive(&idFrom, &idTo, DPRECEIVE_ALL, pvMsgBuffer, &dwMsgBufferSize);

		// resize buffer if it's too small
        if (hr == DPERR_BUFFERTOOSMALL)
        {
            SAFE_DELETE_ARRAY(pvMsgBuffer);

            pvMsgBuffer = new BYTE[dwMsgBufferSize];
            
			if (pvMsgBuffer == NULL)
				return E_OUTOFMEMORY;

            m_pvDPMsgBuffer     = pvMsgBuffer;
            m_dwDPMsgBufferSize = dwMsgBufferSize;

            continue;
        }

        if (DPERR_NOMESSAGES == hr)
			return S_OK;

        if (FAILED(hr))
            return hr;

		// check if system or app msg
        if( idFrom == DPID_SYSMSG )
        {
			hr = ProcessSystemMsg((DPMSG_GENERIC*)pvMsgBuffer, dwMsgBufferSize, idFrom, idTo);
            
			if (FAILED(hr))
				return hr;
		}
        else
        {
			hr = ProcessAppMsg((DPMSG_GENERIC*)pvMsgBuffer, dwMsgBufferSize, idFrom, idTo );
			
			if (FAILED(hr))
				return hr;
		}

		
		// mike - 250604 - remove memory
		delete [ ] pvMsgBuffer;
		pvMsgBuffer = NULL;
		m_pvDPMsgBuffer = NULL;
		m_dwDPMsgBufferSize = 0;
		

		// Quick Exit Where System Flag Was Set  - To Process Elsewhere
		if(gbProcessMessageElsewhere==true)
			return 1;
    }

	return 1;
}
////////////////////////////////////////////////////////////////////////////////

int CNetwork::GetOneNetMsgOffQueue(QueuePacketStruct* pQPacket)
{
	/*
	// mike - 300305
	// Scan for new messages when queue empty
	if(gpNetQueue)
	{
		AddAnyNewNetMsgToQueue();
	}

	// Obtain oldest message and give to MSG
	if(gpNetQueue)
	{
		if(gNetQueueCount>0)
		{
			gpNetQueue->TakeNextFromQueue(pQPacket);

			gNetQueueCount--;
			return 1;
		}
	}
	*/

	// mike - 300305
	AddAnyNewNetMsgToQueue();

	if(g_NewNetQueue.size ( ) > 0 )
	{
		QueuePacketStruct msg = g_NewNetQueue [ 0 ];

		int id=msg.pMsg->id;

		DWORD size = msg.pMsg->size;
		char* pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
		memcpy(pData, &id, sizeof(int));
		memcpy(pData+sizeof(int), &size, sizeof(DWORD));
		memcpy(pData+sizeof(int)+sizeof(DWORD), &msg.pMsg->msg, size);

		pQPacket->pMsg = (tagNetData*)pData;
		pQPacket->idFrom = msg.idFrom;
		pQPacket->idTo = msg.idTo;

		// mike - 020206 - addition for vs8, is this right?
		//g_NewNetQueue.erase ( &g_NewNetQueue [ 0 ] );
		g_NewNetQueue.erase ( g_NewNetQueue.begin ( ) + 0 );

		return 1;
	}

	return 0;
}

int CNetwork::GetSysNetMsgIfAny(void)
{
	// Scan for new messages when queue empty

	// mike - 300305
	//if(gpNetQueue)
		AddAnyNewNetMsgToQueue();
	
	return 1;
}

HRESULT CNetwork::ProcessSystemMsg(DPMSG_GENERIC* pMsg, DWORD dwMsgSize, DPID idFrom, DPID idTo )
{  
	// check msg type
	switch (pMsg->dwType)
    {
		// chat msg
        case DPSYS_CHAT:
		{
        	DPMSG_CHAT* pChatMsg = (DPMSG_CHAT*) pMsg;
			DPCHAT* pChatStruct = pChatMsg->lpChat;
			break;
		}

		// session lost
        case DPSYS_SESSIONLOST:
			gbProcessMessageElsewhere=true;
			gbSystemSessionLost=true;
			gGameSessionActive=0;
			break;

		// session being hosted by new player
        case DPSYS_HOST:
			gbProcessMessageElsewhere=true;
			gSystemSessionIsNowHosting=1;
			break;

		// player or group created
        case DPSYS_CREATEPLAYERORGROUP:
            DPMSG_CREATEPLAYERORGROUP* pCreateMsg;
            pCreateMsg = (DPMSG_CREATEPLAYERORGROUP*) pMsg;
			gSystemPlayerCreated=pCreateMsg->dpId;
			gbProcessMessageElsewhere=true;
			break;

		// player or group destroyed
        case DPSYS_DESTROYPLAYERORGROUP:
            DPMSG_DESTROYPLAYERORGROUP* pDeleteMsg;
            pDeleteMsg = (DPMSG_DESTROYPLAYERORGROUP*) pMsg;
			gSystemPlayerDestroyed=pDeleteMsg->dpId;
			gbProcessMessageElsewhere=true;
			break;

		default:
			break;
	}
	return 1;
}

HRESULT CNetwork::ProcessAppMsg(DPMSG_GENERIC* pMsg, DWORD dwMsgSize, DPID idFrom, DPID idTo )
{
	tagNetData* msg = (tagNetData*)pMsg;

	/*
	// mike - 300305
	if(gpNetQueue)
	{
		QueuePacketStruct packet;
		packet.pMsg=msg;
		packet.idFrom=idFrom;
		packet.idTo=idTo;		
		gpNetQueue->AddToQueue(&packet);
		gNetQueueCount++;
	}
	*/

	QueuePacketStruct packet;
	QueuePacketStruct packetNew;
	
	packet.pMsg=msg;
	packet.idFrom=idFrom;
	packet.idTo=idTo;
	

	int id=packet.pMsg->id;
	DWORD size = packet.pMsg->size;
	char* pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
	memcpy(pData, &id, sizeof(int));
	memcpy(pData+sizeof(int), &size, sizeof(DWORD));
	memcpy(pData+sizeof(int)+sizeof(DWORD), &packet.pMsg->msg, size);

	// Fill item with MSG data
	packetNew.pMsg = (tagNetData*)pData;
	packetNew.idFrom = packet.idFrom;
	packetNew.idTo = packet.idTo;

	g_NewNetQueue.push_back ( packetNew );

	return 1;
}

////////////////////////////////////////////////////////////////////////////////

int CNetwork::GetNetConnections(char *data[MAX_CONNECTIONS])
{
	for (int temp = 0; temp < netTypeNum; temp++)
		data[temp] = netType[temp];
	
	return 1;
}

int CNetwork::FindNetSessions(char *ExtraData)
{
	LPDIRECTPLAYLOBBY2A m_DPLobby;
	LPDIRECTPLAYLOBBYA DPLobbyA = NULL;
	HRESULT	hr;

	// create instance
	if(m_pDP) SAFE_RELEASE(m_pDP);
//	if(m_pDP==NULL)
//	{
		if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay4A, (LPVOID*)&m_pDP)))
		{
			gInternalErrorCode=18515;
			return 0;
		}
//	}
	
	// create lobby
	if (FAILED(hr = DirectPlayLobbyCreate(NULL, &DPLobbyA, NULL, NULL, 0)))
	{
		gInternalErrorCode=18516;
		return 0;
	}
	
	// query interface
	if (FAILED(hr = DPLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *)&m_DPLobby)))
	{
		gInternalErrorCode=18517;
		return 0;
	}

	// Release unused lobby
	SAFE_RELEASE(DPLobbyA);

	// fill in information for create compound address
	// this way we can get around the windows dialog boxes
	DPCOMPOUNDADDRESSELEMENT Address[2];
	DWORD AddressSize = 0;
	BOOL SkipEnum=FALSE;

	// IPX
	int AddressMax=1;
	if(netTypeGUID[netConnectionTypeSelected]==DPSPGUID_IPX)
	{
		Address[0].guidDataType = DPAID_ServiceProvider;
		Address[0].dwDataSize   = sizeof(GUID);
		Address[0].lpData       = (LPVOID)&DPSPGUID_IPX;
		AddressMax=1;
	}
	// TCP IP
	if(netTypeGUID[netConnectionTypeSelected]==DPSPGUID_TCPIP)
	{
		Address[0].guidDataType = DPAID_ServiceProvider;
		Address[0].dwDataSize   = sizeof(GUID);
		Address[0].lpData       = (LPVOID)&DPSPGUID_TCPIP;
		if(strcmp(ExtraData,"")!=0)
		{
			Address[1].guidDataType = DPAID_INet;
			Address[1].dwDataSize   = lstrlen(ExtraData) + 1;
			Address[1].lpData       = ExtraData;
			AddressMax=2;
		}
	}
	// MODEM
	if(netTypeGUID[netConnectionTypeSelected]==DPSPGUID_MODEM)
	{
		Address[0].guidDataType = DPAID_ServiceProvider;
		Address[0].dwDataSize   = sizeof(GUID);
		Address[0].lpData       = (LPVOID)&DPSPGUID_MODEM;
		if(strcmp(ExtraData,"")!=0)
		{
			Address[1].guidDataType = DPAID_Phone;
			Address[1].dwDataSize   = lstrlen(ExtraData) + 1;
			Address[1].lpData       = ExtraData;
			AddressMax=2;
		}
		else
		{
			// No Enumeration if Modem Answering..
			SkipEnum=TRUE;
		}
	}
	// SERIAL
	if(netTypeGUID[netConnectionTypeSelected]==DPSPGUID_SERIAL)
	{
		Address[0].guidDataType = DPAID_ServiceProvider;
		Address[0].dwDataSize   = sizeof(GUID);
		Address[0].lpData       = (LPVOID)&DPSPGUID_SERIAL;
		if(strcmp(ExtraData,"")!=0)
		{
			Address[1].guidDataType = DPAID_ComPort;
			Address[1].dwDataSize   = lstrlen(ExtraData) + 1;
			Address[1].lpData       = ExtraData;
			AddressMax=2;
		}
	}

	// create compound address
	hr = m_DPLobby->CreateCompoundAddress(Address, AddressMax, NULL, &AddressSize);
	if (hr != DPERR_BUFFERTOOSMALL)
	{
		gInternalErrorCode=18518;
	}

	// allocate memory
	Connection = GlobalAllocPtr(GHND, AddressSize);
	if(Connection == NULL)
	{
		gInternalErrorCode=18519;
		return 0;
	}

	// create new compound address
	if (FAILED(hr = m_DPLobby->CreateCompoundAddress(Address, AddressMax, Connection, &AddressSize)))
	{
		gInternalErrorCode=18520;
		return 0;
	}

	// now init connection
	if (FAILED(hr = m_pDP->InitializeConnection(Connection, 0)))
	{
		gInternalErrorCode=18521;
		return 0;
	}

	// Free Connection and clear
	GlobalFreePtr(Connection);
	Connection=NULL;//fixed 4/1/3

	// Delete connection allocation
	SAFE_RELEASE(m_DPLobby);

	// Can be skipped
	if(SkipEnum==FALSE)
	{
		// get all available sessions
		netSessionPos = 0;
		DPSESSIONDESC2 dpsd;
		ZeroMemory(&dpsd, sizeof(dpsd));
		dpsd.dwSize           = sizeof(dpsd);
		dpsd.guidApplication  = DPCREATE_GUID;
		dpsd.dwFlags          = 0;
		if (FAILED(hr = m_pDP->EnumSessions(&dpsd, 0, (LPDPENUMSESSIONSCALLBACK2)StaticGetSessions, NULL, 0)))
		{
			gInternalErrorCode=18522;
			return 0;
		}
	}

	return 1;
}

int CNetwork::GetNetSessions(char *data[MAX_SESSIONS])
{
	if ( !m_pDP )
	{
		gInternalErrorCode=18522;
		return 0;
	}

	// Update Session Liost (as it is dynamic)
	HRESULT	hr;
	netSessionPos = 0;
	DPSESSIONDESC2 dpsd;
	ZeroMemory(&dpsd, sizeof(dpsd));
	dpsd.dwSize           = sizeof(dpsd);
	dpsd.guidApplication  = DPCREATE_GUID;
	dpsd.dwFlags          = 0;
	if (FAILED(hr = m_pDP->EnumSessions(&dpsd, 0, (LPDPENUMSESSIONSCALLBACK2)StaticGetSessions, NULL, 0)))
	{
		gInternalErrorCode=18522;
		return 0;
	}

	for (int temp = 0; temp < netSessionPos; temp++)
		data[temp] = netSessionName[temp];
	
	return 1;
}
////////////////////////////////////////////////////////////////////////////////

int CNetwork::GetNetPlayers(char *data[], DPID dpids[])
{
	if ( !m_pDP )
	{
		gInternalErrorCode=18523;
		return 0;
	}

	HRESULT hr;		// for error codes
	int		temp;	// used for loops etc.

	netPlayersPos = 0;	// reset to 0

	// find all of the players in a session
	if (FAILED(hr = m_pDP->EnumPlayers(NULL, (LPDPENUMPLAYERSCALLBACK2)StaticGetPlayers, NULL, DPENUMPLAYERS_ALL)))
	{
		gInternalErrorCode=18523;
		return 0;
	}

	// store the players names
	for (temp = 0; temp < netPlayersPos; temp++)
	{
		dpids[temp] = netPlayersID[temp];
		data[temp] = netPlayersName[temp];
	}
	
	return netPlayersPos;
}

INT_PTR CALLBACK CNetwork::StaticGetPlayers(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	netPlayersID[netPlayersPos] = dpId;
	netPlayersName[netPlayersPos] = new char[256];
	strcpy(netPlayersName[netPlayersPos], (CHAR *)lpName->lpszShortName);
	netPlayersPos++;
	return 1;
}
