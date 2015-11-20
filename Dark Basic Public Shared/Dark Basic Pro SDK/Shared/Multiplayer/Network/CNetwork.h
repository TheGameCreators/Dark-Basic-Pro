///////////////////////////////////
// CNETWORK CLASS /////////////////
///////////////////////////////////

///////////////////////////////////
// INCLUDES ///////////////////////
#include <objbase.h>
#include <wtypes.h>
#include <cguid.h>
#include "dplay.h"
#include "dplobby.h"

#define WIN32_LEAN_AND_MEAN  
#define INITGUID

#include <windows.h>   
#include <windowsx.h> 
#include <mmsystem.h>
//#include <iostream.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#include "netqueue.h"

#ifndef _CNETWORK_H_
#define _CNETWORK_H_

// session info
struct DPSessionInfo
{
	GUID  guidSession;		// session GUID
	TCHAR szSession[256];	// session name

	// mike - 250604 - addition for storing player count
	int	 iPlayers;
	int	 iMaxPlayers;
};

// Defines
#define		MAX_CONNECTIONS		64
#define		MAX_SESSIONS		256
#define		MAX_PLAYERS			256

// main guid
extern GUID DPCREATE_GUID;

// Externally Prototyped internal Data
extern int				netTypeNum;
extern char				*netType[MAX_CONNECTIONS];
extern VOID				*gConnection[MAX_CONNECTIONS];

extern int				netSessionPos;
extern DPSessionInfo	netSession[MAX_SESSIONS];
extern char				*netSessionName[MAX_SESSIONS];

extern int				netPlayersPos;
extern DPID				netPlayersID[MAX_PLAYERS];
extern char				*netPlayersName[MAX_PLAYERS];

//extern int				gNetQueueCount;
//extern CNetQueue*		gpNetQueue;

//extern std::stack < QueuePacketStruct > g_NewNetQueue;
extern std::vector < QueuePacketStruct > g_NewNetQueue;

#define ALLPLAYERS DPID_ALLPLAYERS	

///////////////////////////////////
// DEFINITION /////////////////////
///////////////////////////////////
class CNetwork
{
	public:
		CNetwork();		// constructor
		~CNetwork();	// destructor	

		// GAME FUNCTIONS ///////////////////
		int CreateNetGame(char *gamename, char *name, int players, DWORD flags);		// create a game
		int JoinNetGame(char *name);					// join a game
		void CloseNetGame(void);

		DPID CreatePlayer(char* name);
		bool DestroyPlayer(DPID dpid);

		int GetNetConnections(char *data[MAX_CONNECTIONS]);	// get a list of connections
		int SetNetConnections(int index);		// set the current connection

		int FindNetSessions(char *ipaddress);
		int GetNetSessions(char *data[MAX_SESSIONS]);		// get a list of sessions
		int SetNetSessions(int num);

		int GetNetPlayers(char *data[], DPID dpids[]);		// get a list of players
		/////////////////////////////////////
		DPID GetLocalPlayerDPID(void) { return m_LocalPlayerDPID; }
		DPID GetServerPlayerDPID(void) { return DPID_SERVERPLAYER; }
 
		// MESSAGE FUNCTIONS ////////////////	
		int SendNetMsg(int player, tagNetData *msg);
		int SendNetMsgGUA(int player, tagNetData *msg);
		int AddAnyNewNetMsgToQueue(void);
		int GetOneNetMsgOffQueue(QueuePacketStruct* pQPacket);
		int GetSysNetMsgIfAny(void);

		/////////////////////////////////////

		int CNetwork::DebugMSG(int value, char *msg);

//		tagNetData data;
		
	public:
		DPID                 m_LocalPlayerDPID;		// player id

	private:
		LPDIRECTPLAY4A       m_pDP;					// main DirectPlay object
		HANDLE               m_hDPMessageEvent;		// player event info
		VOID*				 m_pvDPMsgBuffer;		// message buffer
		DWORD				 m_dwDPMsgBufferSize;	// message buffer size

		int					 selectedNetSession;
		LPVOID				 Connection;

		// process system messages
		HRESULT ProcessSystemMsg(DPMSG_GENERIC* pMsg, DWORD dwMsgSize, DPID idFrom, DPID idTo);
		HRESULT ProcessAppMsg(DPMSG_GENERIC* pMsg, DWORD dwMsgSize, DPID idFrom, DPID idTo);

		// get connections / get sessions
		static INT_PTR CALLBACK StaticGetConnection(LPCGUID pguidSP, VOID* pConnection, DWORD dwConnectionSize, LPCDPNAME pName, DWORD dwFlags, VOID* pvContext);
		static INT_PTR CALLBACK StaticGetSessions(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext);
		static INT_PTR CALLBACK StaticGetPlayers(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext);
};
///////////////////////////////////

// error codes used
enum CNetwork_Return
{
	MNA_CNETWORK_OK,
	MNA_CNETWORK_CANNOTCREATE_PEER,
	MNA_CNETWORK_CANNOTCREATE_LOBBY,
};

#endif _CNETWORK_H_