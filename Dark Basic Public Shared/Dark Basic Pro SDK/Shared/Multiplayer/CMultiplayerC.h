#ifndef _CMULTIPLAYER_H_
#define _CMULTIPLAYER_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "windows.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( void );
DARKSDK void Destructor ( void );
DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData( LPVOID pGlobPtr );
DARKSDK void RefreshD3D ( int iMode );

#ifdef DARKSDK_COMPILE
		void ConstructorMultiplayer ( void );
		void DestructorMultiplayer ( void );
		void SetErrorHandlerMultiplayer ( LPVOID pErrorHandlerPtr );
		void PassCoreDataMultiplayer ( LPVOID pGlobPtr );
		void RefreshD3DMultiplayer ( int iMode );
#endif

DARKSDK void InternalJoinNetGame(int sessionnum, LPSTR name);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// COMMANDS FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// connection
DARKSDK void  CreateNetGame(LPSTR gamename, LPSTR name, int playermax);
DARKSDK void  CreateNetGameEx(LPSTR gamename, LPSTR name, int playermax, int flagnum);
DARKSDK void  JoinNetGame(int sessionnum, LPSTR name);
DARKSDK void  CloseNetGame(void);

// enumerate net data
DARKSDK void  PerformChecklistForNetConnections(void);
DARKSDK void  SetNetConnections(int index);
DARKSDK void  SetNetConnectionsEx(int index, LPSTR ipaddress);
DARKSDK void  PerformChecklistForNetSessions(void);
DARKSDK void  PerformChecklistForNetPlayers(void);

// player
DARKSDK void  CreatePlayer(LPSTR playername);
DARKSDK int   CreatePlayerEx(LPSTR playername);
DARKSDK void  DestroyPlayer(int playerid);

// send standard net messages
DARKSDK void  SendNetMsgL(int playerid, int MessageData);
DARKSDK void  SendNetMsgF(int playerid, DWORD MessageData);
DARKSDK void  SendNetMsgS(int playerid, LPSTR pMessageData);

// send memblock based messages
DARKSDK void  SendNetMsgMemblock(int playerid, int mbi);
DARKSDK void  SendNetMsgMemblockEx(int playerid, int mbi, int gua);
DARKSDK void  SendNetMsgImage(int playerid, int imageindex, int gua);
DARKSDK void  SendNetMsgBitmap(int playerid, int bitmapindex, int gua);
DARKSDK void  SendNetMsgSound(int playerid, int soundindex, int gua);
DARKSDK void  SendNetMsgMesh(int playerid, int meshindex, int gua);

// get net messages and flag states
DARKSDK void  GetNetMsg(void);
DARKSDK int   NetMsgInteger(void);
DARKSDK DWORD NetMsgFloat(void);
DARKSDK DWORD NetMsgString(DWORD pDestStr);

// get memblock based net meseages
DARKSDK void  NetMsgMemblock(int mbi);
DARKSDK void  NetMsgImage(int imageindex);
DARKSDK void  NetMsgBitmap(int bitmapindex);
DARKSDK void  NetMsgSound(int soundindex);
DARKSDK void  NetMsgMesh(int meshindex);

// get states
DARKSDK int   NetMsgExists(void);
DARKSDK int   NetMsgType(void);
DARKSDK int   NetMsgPlayerFrom(void);
DARKSDK int   NetMsgPlayerTo(void);
DARKSDK int   NetSessionExists(void);
DARKSDK int   NetSessionLost(void);
DARKSDK int   NetPlayerCreated(void);
DARKSDK int   NetPlayerDestroyed(void);
DARKSDK int   NetSessionIsNowHosting(void);

// new multiplayer commands
DARKSDK int   MagicNetGame(DWORD lpGameName, DWORD lpPlayerName, int PlayerMax, int FlagNum );
DARKSDK int   NetBufferSize(void);


#ifdef DARKSDK_COMPILE
		void  dbCreateNetGame						( LPSTR gamename, LPSTR name, int playermax );
		void  dbCreateNetGame						( LPSTR gamename, LPSTR name, int playermax, int flagnum );
		void  dbJoinNetGame							( int sessionnum, LPSTR name );
		void  dbFreeNetGame							( void );

		void  dbPerformChecklistForNetConnections	( void );
		void  dbSetNetConnections					( int index );
		void  dbSetNetConnections					( int index, LPSTR ipaddress );
		void  dbPerformChecklistForNetSessions		( void );
		void  dbPerformChecklistForNetPlayers		( void );

		void  dbCreateNetPlayer						( LPSTR playername );
		int   dbCreateNetPlayerEx					( LPSTR playername );
		void  dbDestroyNetPlayer					( int playerid );

		void  dbSendNetMessageInteger				( int playerid, int MessageData );
		void  dbSendNetMessageFloat					( int playerid, float MessageData );
		void  dbSendNetMessageString				( int playerid, LPSTR pMessageData );

		void  dbSendNetMessageMemblock				( int playerid, int mbi );
		void  dbSendNetMessageMemblock				( int playerid, int mbi, int gua );
		void  dbSendNetMessageImage					( int playerid, int imageindex, int gua );
		void  dbSendNetMessageBitmap				( int playerid, int bitmapindex, int gua );
		void  dbSendNetMessageSound					( int playerid, int soundindex, int gua );
		void  dbSendNetMessageMesh					( int playerid, int meshindex, int gua );

		void  dbGetNetMessage						( void );
		int   dbNetMessageInteger					( void );
		float dbNetMessageFloat						( void );
		char* dbNetMessageString					( void );

		void  dbNetMessageMemblock					( int mbi );
		void  dbNetMessageImage						( int imageindex );
		void  dbNetMessageBitmap					( int bitmapindex );
		void  dbNetMessageSound						( int soundindex );
		void  dbNetMessageMesh						( int meshindex );

		int   dbNetMessageExists					( void );
		int   dbNetMessageType						( void );
		int   dbNetMessagePlayerFrom				( void );
		int   dbNetMessagePlayerTo					( void );
		int   dbNetSessionExists					( void );
		int   dbNetSessionLost						( void );
		int   dbNetPlayerCreated					( void );
		int   dbNetPlayerDestroyed					( void );
		int   dbNetGameNowHosting					( void );

		int   dbDefaultNetGame						( char* lpGameName, char* lpPlayerName, int PlayerMax, int FlagNum );
		int   dbNetBufferSize						( void );

		// lee - 300706 - GDK fixes
		void  dbFreeNetPlayer						( int playerid );
		void  dbSetNetConnection					( int index );
		void  dbSetNetConnection					( int index, LPSTR ipaddress );
		int   dbNetGameExists						( void );
		int   dbNetGameLost							( void );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


// multiplayer support functions
//DARKSDK void  AlwaysActiveOn(void);
//DARKSDK void  AlwaysActiveOff(void);

#endif _CMULTIPLAYER_H_