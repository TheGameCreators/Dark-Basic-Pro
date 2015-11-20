
//
// Multiplayer DLL
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
#pragma comment ( lib, "dplayx.lib" )
#endif

// Includes
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#include "network\CNetwork.h"
#include "network\NetQueue.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBitmap.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSound.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMemblocks.h"
#endif

#include "cmultiplayerc.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// leefix - 131108 - DarkGDK does not want another global instance
#ifdef DARKSDK_COMPILE
	#define DBPRO_GLOBAL static
#endif

// Global Shared Data Pointer (passed in from core)
DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;
DBPRO_GLOBAL PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;

// Global Internal Data
DBPRO_GLOBAL char					m_pWorkString[256];
DBPRO_GLOBAL CNetwork*				pCNetwork						= NULL;
DWORD					gInternalErrorCode				= 0;
DBPRO_GLOBAL bool					gbAlwaysHaveFocus				= false;

// Used to call memblockDLL for memblock return ptr function
typedef int						( *MEMBLOCKS_GetMemblockExist   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockPtr   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockSize  ) ( int );
typedef void					( *MEMBLOCKS_MemblockFromMedia ) ( int, int );
typedef void					( *MEMBLOCKS_MediaFromMemblock ) ( int, int );
typedef void					( *MEMBLOCKS_MakeMemblock  ) ( int, int );
typedef void					( *MEMBLOCKS_DeleteMemblock ) ( int );
DBPRO_GLOBAL MEMBLOCKS_GetMemblockExist		g_Memblock_GetMemblockExist;
DBPRO_GLOBAL MEMBLOCKS_GetMemblockPtr		g_Memblock_GetMemblockPtr;
DBPRO_GLOBAL MEMBLOCKS_GetMemblockSize		g_Memblock_GetMemblockSize;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromImage;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromBitmap;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromSound;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromMesh;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_ImageFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_BitmapFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_SoundFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_MeshFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MakeMemblock			g_Memblock_MakeMemblock;
DBPRO_GLOBAL MEMBLOCKS_DeleteMemblock		g_Memblock_DeleteMemblock;

// Used to call mediaDLLs
typedef int						( *MEDIA_GetExist   ) ( int );
DBPRO_GLOBAL MEDIA_GetExist					g_Image_GetExist;
DBPRO_GLOBAL MEDIA_GetExist					g_Bitmap_GetExist;
DBPRO_GLOBAL MEDIA_GetExist					g_Sound_GetExist;
DBPRO_GLOBAL MEDIA_GetExist					g_Basic3D_GetExist;

// local checklist work vars
DBPRO_GLOBAL bool							g_bCreateChecklistNow			= false;
DBPRO_GLOBAL DWORD							g_dwMaxStringSizeInEnum			= 0;


// Internal Data DBV1
DBPRO_GLOBAL bool gbNetDataExists=false;
DBPRO_GLOBAL int gdwNetDataType=0;
DBPRO_GLOBAL DWORD gdwNetDataPlayerFrom=0;
DBPRO_GLOBAL DWORD gdwNetDataPlayerTo=0;
DBPRO_GLOBAL DWORD gpNetDataDWORD=NULL;
DBPRO_GLOBAL DWORD gpNetDataDWORDSize=0;
DBPRO_GLOBAL int gPlayerIDDatabase[256];
DBPRO_GLOBAL bool gbPlayerIDDatabaseFlagged[256];

// External Data DBV1
extern CNetQueue* gpNetQueue;
extern int gGameSessionActive;
extern bool gbSystemSessionLost;
extern int gSystemPlayerCreated;
extern int gSystemPlayerDestroyed;
extern int gSystemSessionIsNowHosting;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Internal Functions
//

DARKSDK void Constructor ( void )
{
	gpNetDataDWORD = NULL;
	//pCNetwork = NULL;
}

DARKSDK void FreeNet ( void )
{
	if(gdwNetDataType>=3 && gpNetDataDWORD)
	{
		//delete (char*)gpNetDataDWORD;

		// mike - 250604
		delete [ ] ( char* ) gpNetDataDWORD;

		gpNetDataDWORD=NULL;
	}
	if(pCNetwork)
	{
		pCNetwork->CloseNetGame();
		delete pCNetwork;
		pCNetwork=NULL;
	}
}

DARKSDK void Destructor ( void )
{
	FreeNet();
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

	#ifndef DARKSDK_COMPILE
		// memblock DLL ptrs
		g_Memblock_GetMemblockExist = ( MEMBLOCKS_GetMemblockExist ) GetProcAddress ( g_pGlob->g_Memblocks, "?MemblockExist@@YAHH@Z" );
		g_Memblock_GetMemblockPtr = ( MEMBLOCKS_GetMemblockPtr ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockPtr@@YAKH@Z" );
		g_Memblock_GetMemblockSize = ( MEMBLOCKS_GetMemblockSize ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockSize@@YAHH@Z" );
		g_Memblock_MemblockFromImage = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromImage@@YAXHH@Z" );
		g_Memblock_MemblockFromBitmap = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromBitmap@@YAXHH@Z" );
		g_Memblock_MemblockFromSound = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromSound@@YAXHH@Z" );
		g_Memblock_MemblockFromMesh = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromMesh@@YAXHH@Z" );
		g_Memblock_ImageFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateImageFromMemblock@@YAXHH@Z" );
		g_Memblock_BitmapFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateBitmapFromMemblock@@YAXHH@Z" );
		g_Memblock_SoundFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateSoundFromMemblock@@YAXHH@Z" );
		g_Memblock_MeshFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMeshFromMemblock@@YAXHH@Z" );
		g_Memblock_MakeMemblock = ( MEMBLOCKS_MakeMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?MakeMemblock@@YAXHH@Z" );
		g_Memblock_DeleteMemblock = ( MEMBLOCKS_DeleteMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?DeleteMemblock@@YAXH@Z" );

		// media DLL ptrs
		g_Image_GetExist = ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Image, "?GetExistEx@@YAHH@Z" );
		g_Bitmap_GetExist = ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Bitmap, "?BitmapExist@@YAHH@Z" );
		g_Sound_GetExist = ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Sound, "?GetSoundExist@@YAHH@Z" );
		g_Basic3D_GetExist = ( MEDIA_GetExist ) GetProcAddress ( g_pGlob->g_Basic3D, "?GetMeshExist@@YAHH@Z" );
	#else
		g_Memblock_GetMemblockExist = dbMemblockExist;
		g_Memblock_GetMemblockPtr = dbGetMemblockPtr;
		g_Memblock_GetMemblockSize =  ( MEMBLOCKS_GetMemblockSize )dbGetMemblockSize;
		g_Memblock_MemblockFromImage = dbMakeMemblockFromImage;
		g_Memblock_MemblockFromBitmap = dbMakeMemblockFromBitmap;
		g_Memblock_MemblockFromSound = dbMakeMemblockFromSound;
		g_Memblock_MemblockFromMesh = dbMakeMemblockFromMesh;
		g_Memblock_ImageFromMemblock = dbMakeImageFromMemblock;
		g_Memblock_BitmapFromMemblock = dbMakeBitmapFromMemblock;
		g_Memblock_SoundFromMemblock = dbMakeSoundFromMemblock;
		g_Memblock_MeshFromMemblock = dbMakeMeshFromMemblock;
		g_Memblock_MakeMemblock = dbMakeMemblock;
		g_Memblock_DeleteMemblock = dbDeleteMemblock;

		g_Image_GetExist = dbImageExist;
		g_Bitmap_GetExist =dbBitmapExist;
		g_Sound_GetExist = dbSoundExist;
		g_Basic3D_GetExist =dbObjectExist;
	#endif
}

DBPRO_GLOBAL LPSTR GetReturnStringFromWorkString(void)
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

DARKSDK bool QuickStartInit(int* sessionnumber)
{
	bool bCreationValid=true;

	if(pCNetwork==NULL)
	{
		pCNetwork = new CNetwork;

		if(pCNetwork->SetNetConnections(-1)!=0)
		{
			if(pCNetwork->FindNetSessions("")!=0)
			{
				if(pCNetwork->SetNetSessions(1)!=0)
				{
					// QuickStart Success - reassign session number to 1
					if(sessionnumber) *sessionnumber=0;
				}
				else
				{
					RunTimeWarning(RUNTIMEERROR_MPFAILEDTOSETSESSION);
					bCreationValid=false;
				}
			}
			else
			{
				RunTimeWarning(RUNTIMEERROR_MPFAILEDTOFINDSESSION);
				bCreationValid=false;
			}
		}
		else
		{
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCONNECT);
			bCreationValid=false;
		}
	}

	return bCreationValid;
}

DARKSDK int findseqidindatabase(int dpid)
{
	int free=-1, got=-1;
	for(int s=1; s<256; s++)
	{
		if(gPlayerIDDatabase[s]==0 && free==-1) free=s;
		if(gPlayerIDDatabase[s]==dpid) { got=s; break; }
	}
	if(got==-1)
	{
		if(free!=-1)
		{
			gPlayerIDDatabase[free]=dpid;
			return free;
		}
		else
			return -1;
	}
	else
		return got;
}

DARKSDK void CoreSendNetMsg(int type, int playerid, DWORD* pMessageData, DWORD dwSize, int guarenteed)
{
	if(pCNetwork && gGameSessionActive>0)
	{
		// Get Player Id
		bool bSendValid=true;
		if(playerid==0) playerid=ALLPLAYERS;
		if(playerid!=0)
		{
			// Find player ID from database
			playerid=gPlayerIDDatabase[playerid];
			if(playerid==0)
			{
				// Player not exist - send nothing
				bSendValid=false;
			}
		}

		// If send is valid
		if(bSendValid)
		{
			// Build MSG Data
			DWORD size;
			char* pData;
			char* pStr;
			switch(type)
			{
				// Integer Message
				case 1 :
					size = 4;
					pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
					memcpy(pData, &type, sizeof(int));
					memcpy(pData+sizeof(int), &size, sizeof(DWORD));
					memcpy(pData+sizeof(int)+sizeof(DWORD), pMessageData, size);
					break;

				// Float Message
				case 2 :
					size = 4;
					pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
					memcpy(pData, &type, sizeof(int));
					memcpy(pData+sizeof(int), &size, sizeof(DWORD));
					memcpy(pData+sizeof(int)+sizeof(DWORD), pMessageData, size);
					break;

				// String Message
				case 3 :
					pStr = (char*)pMessageData;
					size = strlen(pStr) + 1;
					pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
					memcpy(pData, &type, sizeof(int));
					memcpy(pData+sizeof(int), &size, sizeof(DWORD));
					memcpy(pData+sizeof(int)+sizeof(DWORD), pStr, size);
					break;

				// Memblock Message
				case 4 :
				case 5 :
				case 6 :
				case 7 :
				case 8 :
					size = dwSize;
					pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
					memcpy(pData, &type, sizeof(int));
					memcpy(pData+sizeof(int), &size, sizeof(DWORD));
					memcpy(pData+sizeof(int)+sizeof(DWORD), pMessageData, size);
					break;
			}

			if(guarenteed==1)
			{
				if(pCNetwork->SendNetMsgGUA(playerid, (tagNetData*)pData)==0)
					RunTimeWarning(RUNTIMEERROR_MPFAILEDTOSENDMESSAGE);
			}
			else
			{
				if(pCNetwork->SendNetMsg(playerid, (tagNetData*)pData)==0)
					RunTimeWarning(RUNTIMEERROR_MPFAILEDTOSENDMESSAGE);
			}

			// Free MSG Data
			if(pData)
			{
				GlobalFree(pData);
				pData=NULL;
			}
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPPLAYERNOTEXIST);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPNOTINSESSION);
}


//
// Command Functions
//

DARKSDK void CreateNetGame(LPSTR gamename, LPSTR name, int playermax)
{
	if(gGameSessionActive==0)
	{
		// lee - 220306 - u6b4 - validation check, prevent crashing
		if ( gamename==NULL || name==NULL )
		{
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
			return;
		}

		// Easy Quick-Start if connection and session not set..
		bool bCreationValid=QuickStartInit(NULL);

		// Net Game Creation Here
		if(bCreationValid)
		{
			if(strcmp(gamename,"")!=0)
			{
				if(strcmp(name,"")!=0)
				{
					if(playermax>=2 && playermax<=255)
					{
						if(pCNetwork->CreateNetGame(gamename, name, playermax, DPSESSION_MIGRATEHOST)!=0)
						{
							// Clear PlayerID Database
							gGameSessionActive=1;
							ZeroMemory(gPlayerIDDatabase, sizeof(gPlayerIDDatabase));
						}
						else
							RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
					}
					else
						RunTimeError(RUNTIMEERROR_MPCREATEBETWEEN2AND255);
				}
				else
					RunTimeError(RUNTIMEERROR_MPMUSTGIVEPLAYERNAME);
			}
			else
				RunTimeError(RUNTIMEERROR_MPMUSTGIVEGAMENAME);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPSESSIONEXISTS);
}

DARKSDK void CreateNetGameEx(LPSTR gamename, LPSTR name, int playermax, int flagnum)
{
	if(gGameSessionActive==0)
	{
		// lee - 220306 - u6b4 - validation check, prevent crashing
		if ( gamename==NULL || name==NULL )
		{
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
			return;
		}

		// Easy Quick-Start if connection and session not set..
		bool bCreationValid=QuickStartInit(NULL);

		// Net Game Creation Here
		if(bCreationValid)
		{
			if(strcmp(gamename,"")!=0)
			{
				if(strcmp(name,"")!=0)
				{
					if(playermax>=2 && playermax<=255)
					{
						DWORD flags=0;
						if(flagnum==1) flags |= DPSESSION_MIGRATEHOST;
						if(flagnum==2) flags |= DPSESSION_CLIENTSERVER;
						if(pCNetwork->CreateNetGame(gamename, name, playermax, flags)!=0)
						{
							// Clear PlayerID Database
							gGameSessionActive=1;
							ZeroMemory(gPlayerIDDatabase, sizeof(gPlayerIDDatabase));
						}
						else
							RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
					}
					else
						RunTimeError(RUNTIMEERROR_MPCREATEBETWEEN2AND255);
				}
				else
					RunTimeError(RUNTIMEERROR_MPMUSTGIVEPLAYERNAME);
			}
			else
				RunTimeError(RUNTIMEERROR_MPMUSTGIVEGAMENAME);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCREATEGAME);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPSESSIONEXISTS);
}

DARKSDK void JoinNetGame(int sessionnum, LPSTR name)
{
	if(gGameSessionActive==0)
	{
		// lee - 220306 - u6b4 - validation check, prevent crashing
		if ( name==NULL )
		{
			RunTimeError(RUNTIMEERROR_MPMUSTGIVEPLAYERNAME);
			return;
		}

		// Easy Quick-Start if connection and session not set..
		sessionnum-=1;
		bool bCreationValid=QuickStartInit(&sessionnum);

		// Net Game Creation Here
		if(bCreationValid)
		{
			if(sessionnum>=0 && sessionnum<MAX_SESSIONS)
			{
				if(pCNetwork->SetNetSessions(sessionnum)!=0)
				{
					if(strcmp(name,"")!=0)
					{
						int iResult = pCNetwork->JoinNetGame(name);
						if(iResult==1)
						{
							// Clear PlayerID Database
							gGameSessionActive=2;
							ZeroMemory(gPlayerIDDatabase, sizeof(gPlayerIDDatabase));
						}
						else
						{
							if(iResult==2)
								RunTimeWarning(RUNTIMEERROR_MPNOTCREATEDPLAYER);
							else
							{
								if(iResult==3)
									RunTimeWarning(RUNTIMEERROR_MPTOOMANYPLAYERS);
								else
									RunTimeWarning(RUNTIMEERROR_MPFAILEDTOJOINGAME);
							}
						}
					}
					else
						RunTimeError(RUNTIMEERROR_MPMUSTGIVEPLAYERNAME);
				}
				else
					RunTimeWarning(RUNTIMEERROR_MPFAILEDTOSETSESSION);
			}
			else
				RunTimeError(RUNTIMEERROR_MPSESSIONNUMINVALID);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOJOINGAME);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPSESSIONEXISTS);
}

DARKSDK void InternalJoinNetGame(int sessionnum, LPSTR name)
{
	if(gGameSessionActive==0)
	{
		// lee - 220306 - u6b4 - validation check, prevent crashing
		if ( name==NULL ) return;

		// Easy Quick-Start if connection and session not set..
		sessionnum-=1;
		bool bCreationValid=QuickStartInit(&sessionnum);

		// Net Game Creation Here
		if(bCreationValid)
		{
			if(sessionnum>=0 && sessionnum<MAX_SESSIONS)
			{
				if(pCNetwork->SetNetSessions(sessionnum)!=0)
				{
					if(strcmp(name,"")!=0)
					{
						int iResult = pCNetwork->JoinNetGame(name);
						if(iResult==1)
						{
							// Clear PlayerID Database
							gGameSessionActive=2;
							ZeroMemory(gPlayerIDDatabase, sizeof(gPlayerIDDatabase));
						}
					}
				}
			}
		}
	}
}

DARKSDK void CloseNetGame(void)
{
	// Clear PlayerID Database
	gGameSessionActive=0;
	ZeroMemory(gPlayerIDDatabase, sizeof(gPlayerIDDatabase));

	// General network termination
	FreeNet();
}

DARKSDK void PerformChecklistForNetConnections(void)
{
	if(pCNetwork==NULL) pCNetwork = new CNetwork;

	char* data[MAX_CONNECTIONS];
	ZeroMemory(data, sizeof(data));

	// Generate Checklist
	g_pGlob->checklistqty=0;
	g_pGlob->checklistexists=true;
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	if(pCNetwork->GetNetConnections(data))
	{
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

			// Run through...
			DWORD ci=0;
			for(DWORD i=0; i<MAX_CONNECTIONS; i++)
			{
				if(g_bCreateChecklistNow==true) strcpy(g_pGlob->checklist[i].string, "");
				if(data[i])
				{
					if(g_bCreateChecklistNow==true)
					{
						if(ci<g_pGlob->dwChecklistArraySize)
						{
							strcpy(g_pGlob->checklist[i].string, data[i]);
							if(i+1>ci) ci=i+1;
						}
					}
					else
					{
						DWORD dwSize = strlen(data[i]);
						if(dwSize>g_dwMaxStringSizeInEnum) g_dwMaxStringSizeInEnum=dwSize;
						if(i+1>ci) ci=i+1;
					}
				}
			}
			g_pGlob->checklistqty=ci;
		}
	}
	else
		g_pGlob->checklistqty=0;
}

DARKSDK void SetNetConnections(int index)
{
	if(pCNetwork==NULL) pCNetwork = new CNetwork;

	index-=1;
	if(index>=0 && index<MAX_CONNECTIONS)
	{
		if(pCNetwork->SetNetConnections(index)!=0)
		{
			if(pCNetwork->FindNetSessions("")==0)
				RunTimeWarning(RUNTIMEERROR_MPFAILEDTOFINDSESSION);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCONNECT);
	}
	else
		RunTimeError(RUNTIMEERROR_MPCONNECTIONNUMINVALID);
}

DARKSDK void SetNetConnectionsEx(int index, LPSTR ipaddress)
{
	if(pCNetwork==NULL) pCNetwork = new CNetwork;

	index-=1;
	if(index>=0 && index<MAX_CONNECTIONS)
	{
		if(pCNetwork->SetNetConnections(index)!=0)
		{
			if(pCNetwork->FindNetSessions(ipaddress)==0)
				RunTimeWarning(RUNTIMEERROR_MPFAILEDTOFINDSESSION);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPFAILEDTOCONNECT);
	}
	else
		RunTimeError(RUNTIMEERROR_MPCONNECTIONNUMINVALID);
}

DARKSDK void PerformChecklistForNetSessions(void)
{
	if(pCNetwork==NULL) pCNetwork = new CNetwork;

	char* data[MAX_SESSIONS];
	ZeroMemory(data, sizeof(data));

	// Generate Checklist
	g_pGlob->checklistqty=0;
	g_pGlob->checklistexists=true;
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	if(pCNetwork->GetNetSessions(data))
	{
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

			DWORD ci=0;
			for(DWORD i=0; i<MAX_SESSIONS; i++)
			{
				if(g_bCreateChecklistNow==true)
				{
					// ensure only validf checklist items are cleared
					if( i <g_pGlob->dwChecklistArraySize)
						strcpy ( g_pGlob->checklist[i].string, "" );

					if(data[i])
					{
						if(ci<g_pGlob->dwChecklistArraySize)
						{
							strcpy(g_pGlob->checklist[i].string, data[i]);
							if(i+1>ci) ci=i+1;

							//// mike - 250604 - stores extra data
							g_pGlob->checklisthasvalues=true;
							g_pGlob->checklist [ i ].valuea = netSession[i].iPlayers;
							g_pGlob->checklist [ i ].valueb = netSession[i].iMaxPlayers;
						}
					}
				}
				else
				{
					if(data[i]) if(i+1>ci) ci=i+1;
				}
			}
			g_pGlob->checklistqty=ci;
		}
	}
	else
		g_pGlob->checklistqty=0;
}

DARKSDK void PerformChecklistForNetPlayers(void)
{
	
	DPID dpids[256];
	char* data[256];
	ZeroMemory(dpids, sizeof(dpids));
	ZeroMemory(data, sizeof(data));

		g_pGlob->checklistqty=0;
	g_pGlob->checklistexists=true;
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;
	

	if(gGameSessionActive>0)
	{
		// Update checklist with player names and ids
		DWORD playernum=pCNetwork->GetNetPlayers(data, dpids);
		
		if(playernum>255)
			playernum=255;

		if(playernum>0)
		{
			// Reset lafs for player removal
			ZeroMemory(gbPlayerIDDatabaseFlagged, sizeof(gbPlayerIDDatabaseFlagged));

			g_pGlob->checklistqty=playernum;

			for(int c=0; c<g_pGlob->checklistqty; c++)
			{
				if ( data [ c ] )
				{
					GlobExpandChecklist(c, 255);

					strcpy(g_pGlob->checklist[c].string, "");
					//strcpy(g_pGlob->checklist[c].string, "bob");

					g_pGlob->checklist[c].valuea = findseqidindatabase(dpids[c]);
					gbPlayerIDDatabaseFlagged[g_pGlob->checklist[c].valuea]=true;
					g_pGlob->checklist[c].valueb = dpids[c];
					g_pGlob->checklist[c].valuec = 0;
					g_pGlob->checklist[c].valued = 0;
					if(dpids[c]==pCNetwork->GetLocalPlayerDPID()) g_pGlob->checklist[c].valuec = 1;
					if(dpids[c]==pCNetwork->GetServerPlayerDPID()) g_pGlob->checklist[c].valued = 1;
					strcpy(g_pGlob->checklist[c].string, data[c]);
				}

			}

			

			for(int s=1; s<256; s++)
				if(gbPlayerIDDatabaseFlagged[s]==false)
					gPlayerIDDatabase[s]=0;

		}
	}
	
	/*
	DPID dpids[256];
	char* data[256];
	ZeroMemory(dpids, sizeof(dpids));
	ZeroMemory(data, sizeof(data));

	// Generate Checklist
	g_pGlob->checklistqty=0;
	g_pGlob->checklistexists=true;
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;
	
	if(gGameSessionActive>0)
	{
		// Update checklist with player names and ids
		DWORD playernum=pCNetwork->GetNetPlayers(data, dpids);
		if(playernum>255) playernum=255;
		if(playernum>0)
		{
			// Reset lafs for player removal
			ZeroMemory(gbPlayerIDDatabaseFlagged, sizeof(gbPlayerIDDatabaseFlagged));

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

				DWORD ci=0;
				for(DWORD i=0; i<playernum; i++)
				{
					if(g_bCreateChecklistNow==true)
					{
						strcpy(g_pGlob->checklist[i].string, "");
						if(data[i])
						{
							if(ci<g_pGlob->dwChecklistArraySize)
							{

								g_pGlob->checklist[i].valuea = findseqidindatabase(dpids[i]);
								gbPlayerIDDatabaseFlagged[g_pGlob->checklist[i].valuea]=true;
								g_pGlob->checklist[i].valueb = dpids[i];
								g_pGlob->checklist[i].valuec = 0;
								g_pGlob->checklist[i].valued = 0;
								if(dpids[i]==pCNetwork->GetLocalPlayerDPID()) g_pGlob->checklist[i].valuec = 1;
								if(dpids[i]==pCNetwork->GetServerPlayerDPID()) g_pGlob->checklist[i].valued = 1;
								strcpy(g_pGlob->checklist[i].string, data[i]);
								if(i+1>ci) ci=i+1;
							}
						}
					}
					else
					{
						if(data[i])
							if(i+1>ci) ci=i+1;
					}
				}
				g_pGlob->checklistqty=ci;
			}

			// Delete players from database no longer in list
			for(int s=1; s<256; s++)
				if(gbPlayerIDDatabaseFlagged[s]==false)
					gPlayerIDDatabase[s]=0;
		}
		else
			g_pGlob->checklistqty=0;
	}
	else
		g_pGlob->checklistqty=0;
	*/	
}

DARKSDK void CreatePlayer(LPSTR playername)
{
	if(pCNetwork && gGameSessionActive>0)
	{
		if(pCNetwork->CreatePlayer(playername)==0)
			RunTimeWarning(RUNTIMEERROR_MPNOTCREATEDPLAYER);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPNOTINSESSION);
}

DARKSDK int CreatePlayerEx(LPSTR playername)
{
	int playerid=0;
	if(pCNetwork && gGameSessionActive>0)
	{
		if((playerid=pCNetwork->CreatePlayer(playername))!=0)
		{
			// Return sequenced id from dpid
			playerid = findseqidindatabase(playerid);
		}
		else
			RunTimeWarning(RUNTIMEERROR_MPNOTCREATEDPLAYER);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPNOTINSESSION);

	return playerid;
}

DARKSDK void DestroyPlayer(int playerid)
{
	if(pCNetwork && gGameSessionActive>0)
	{
		if(playerid>=1 && playerid<=256)
		{
			int playerindex = gPlayerIDDatabase[playerid];
			if(playerindex!=0)
			{
				if(pCNetwork->DestroyPlayer(playerindex))
				{
					gPlayerIDDatabase[playerid]=0;
				}
				else
					RunTimeWarning(RUNTIMEERROR_MPCANNOTDELETEPLAYER);
			}
			else
				RunTimeWarning(RUNTIMEERROR_MPPLAYERNOTEXIST);
		}
		else
			RunTimeError(RUNTIMEERROR_MPPLAYERNUMINVALID);
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPNOTINSESSION);
}

DARKSDK void SendNetMsgL(int playerid, int MessageData)
{
	CoreSendNetMsg(1, playerid, (DWORD*)&MessageData, 0, 0);
}

DARKSDK void SendNetMsgF(int playerid, DWORD MessageData)
{
	CoreSendNetMsg(2, playerid, &MessageData, 0, 0);
}

DARKSDK void SendNetMsgS(int playerid, LPSTR pMessageData)
{
	CoreSendNetMsg(3, playerid, (DWORD*)pMessageData, 0, 0);
}

DARKSDK void SendNetMsgMemblock(int playerid, int mbi)
{
	if(mbi>=1 && mbi<=255)
	{
		LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
		if(pMemblockData)
		{
			DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(4, playerid, (DWORD*)pMemblockData, dwMemblockSize, 0);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void SendNetMsgMemblockEx(int playerid, int mbi, int gua)
{
	if(mbi>=1 && mbi<=255)
	{
		LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
		if(pMemblockData)
		{
			DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(4, playerid, (DWORD*)pMemblockData, dwMemblockSize, gua);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void SendNetMsgImage(int playerid, int imageindex, int gua)
{
	if(imageindex>=1 && imageindex<=MAXIMUMVALUE)
	{
		if ( g_Image_GetExist(imageindex) )
		{
			int mbi=257;
			g_Memblock_MemblockFromImage(mbi, imageindex);
			DWORD* pPtr = (DWORD*)g_Memblock_GetMemblockPtr(mbi);
			DWORD dwSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(5, playerid, pPtr, dwSize, gua);
		}
		else
			RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);	
	}
	else
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);	
}

DARKSDK void SendNetMsgBitmap(int playerid, int bitmapindex, int gua)
{
	if(bitmapindex>=0 && bitmapindex<MAXIMUMVALUE)
	{
		if(bitmapindex==0 || g_Bitmap_GetExist(bitmapindex))
		{
			int mbi=257;
			g_Memblock_MemblockFromBitmap(mbi, bitmapindex);
			DWORD* pPtr = (DWORD*)g_Memblock_GetMemblockPtr(mbi);
			DWORD dwSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(6, playerid, pPtr, dwSize, gua);
		}
		else
			RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
}

DARKSDK void SendNetMsgSound(int playerid, int soundindex, int gua)
{
	if(soundindex>=1 && soundindex<MAXIMUMVALUE)
	{
		if(g_Sound_GetExist(soundindex))
		{
			int mbi=257;
			g_Memblock_MemblockFromSound(mbi, soundindex);
			DWORD* pPtr = (DWORD*)g_Memblock_GetMemblockPtr(mbi);
			DWORD dwSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(7, playerid, pPtr, dwSize, gua);
		}
		else
			RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
}

DARKSDK void SendNetMsgMesh(int playerid, int meshindex, int gua)
{
	if(meshindex>0 && meshindex<MAXIMUMVALUE)
	{
		if(g_Basic3D_GetExist(meshindex))
		{
			int mbi=257;
			g_Memblock_MemblockFromMesh(mbi, meshindex);
			DWORD* pPtr = (DWORD*)g_Memblock_GetMemblockPtr(mbi);
			DWORD dwSize = g_Memblock_GetMemblockSize(mbi);
			CoreSendNetMsg(8, playerid, pPtr, dwSize, gua);
		}
		else
			RunTimeError(RUNTIMEERROR_B3DMESHNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_B3DMESHNUMBERILLEGAL);
}

DARKSDK void GetNetMsg(void)
{
	if(pCNetwork && gGameSessionActive>0)
	{
		QueuePacketStruct packet;
		packet.pMsg=NULL;
		if(pCNetwork->GetOneNetMsgOffQueue(&packet)!=0)
		{
			if(packet.pMsg)
			{
				// Remove old data
				if(gdwNetDataType>=3 && gpNetDataDWORD)
				{
					delete (char*)gpNetDataDWORD;
					gpNetDataDWORD=NULL;
				}

				// Prepare new data
				gbNetDataExists=true;
				gdwNetDataType=packet.pMsg->id;
				gdwNetDataPlayerFrom=packet.idFrom;
				gdwNetDataPlayerTo=packet.idTo;

				// Create Data
				switch(gdwNetDataType)
				{
					case 1 : // Integer
						memcpy((int*)&gpNetDataDWORD, &packet.pMsg->msg, 4);
						break;

					case 2 : // Float
						memcpy((float*)&gpNetDataDWORD, &packet.pMsg->msg, 4);
						break;

					case 3 : // String
						if(packet.pMsg->size>0)
						{
							gpNetDataDWORD=(DWORD)new char[packet.pMsg->size];
							memcpy((char*)gpNetDataDWORD, &packet.pMsg->msg, packet.pMsg->size);
						}
						break;

					case 4 : // Memblock
						gpNetDataDWORDSize=packet.pMsg->size;
						if(gpNetDataDWORDSize>0)
						{
							gpNetDataDWORD=(DWORD)new char[gpNetDataDWORDSize];
							memcpy((char*)gpNetDataDWORD, &packet.pMsg->msg, gpNetDataDWORDSize);
						}
						break;

					case 5 : // Image
					case 6 : // Bitmap
					case 7 : // Sound
					case 8 : // Mesh
						gpNetDataDWORDSize=packet.pMsg->size;
						if(gpNetDataDWORDSize>0)
						{
							gpNetDataDWORD=(DWORD)new char[gpNetDataDWORDSize];
							memcpy((char*)gpNetDataDWORD, &packet.pMsg->msg, gpNetDataDWORDSize);
						}
						break;
				}

				// Free MSG Data
				if(packet.pMsg)
				{
					GlobalFree(packet.pMsg);
					packet.pMsg=NULL;
				}
			}
			else
			{
				gdwNetDataType=0;
				gbNetDataExists=false;
			}
		}
		else
		{
			gdwNetDataType=0;
			gbNetDataExists=false;
		}
	}
	else
		RunTimeWarning(RUNTIMEERROR_MPNOTINSESSION);
}

DARKSDK int NetMsgExists(void)
{
	int iResult=0;
	if(pCNetwork && gGameSessionActive>0)
		iResult=(int)gbNetDataExists;

	return iResult;
}

DARKSDK int NetMsgType(void)
{
	return gdwNetDataType;
}

DARKSDK int NetMsgPlayerFrom(void)
{
	return findseqidindatabase(gdwNetDataPlayerFrom);
}

DARKSDK int NetMsgPlayerTo(void)
{
	return findseqidindatabase(gdwNetDataPlayerTo);
}

DARKSDK int NetMsgInteger(void)
{
	if(gdwNetDataType==1)
		return *(int*)&gpNetDataDWORD;

	return 0;
}

DARKSDK DWORD NetMsgFloat(void)
{
	if(gdwNetDataType==2) return gpNetDataDWORD;
	return 0;
}

DARKSDK DWORD NetMsgString(DWORD pDestStr)
{
	strcpy(m_pWorkString, "");
	if(pCNetwork && gpNetDataDWORD && gGameSessionActive>0 && gdwNetDataType==3)
		strcpy(m_pWorkString, (char*)gpNetDataDWORD);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();

	// mike - 250604 - clear
	delete (char*)gpNetDataDWORD;
	gpNetDataDWORD = NULL;

	return (DWORD)pReturnString;
}

DARKSDK void NetMsgMemblock(int mbi)
{
	if(gdwNetDataType==4)
	{
		if(mbi>=1 && mbi<=255)
		{
			if(gpNetDataDWORDSize>0)
			{
				// Free existing memblock
				if(g_Memblock_GetMemblockExist(mbi)) g_Memblock_DeleteMemblock(mbi);

				// Create memblock
				g_Memblock_MakeMemblock ( mbi, gpNetDataDWORDSize );
				if(g_Memblock_GetMemblockExist(mbi))
				{
					// Put message in memblock
					LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
					DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
					memcpy(pMemblockData, (char*)gpNetDataDWORD, gpNetDataDWORDSize);

					// mike - 250604 - clear
					delete (char*)gpNetDataDWORD;
					gpNetDataDWORD = NULL;
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
}

DARKSDK void NetMsgImage(int imageindex)
{
	if(gdwNetDataType==5)
	{
		int mbi=257;
		if(imageindex>=1 && imageindex<=MAXIMUMVALUE)
		{
			if(gpNetDataDWORDSize>0)
			{
				// Free existing memblock
				if(g_Memblock_GetMemblockExist(mbi)) g_Memblock_DeleteMemblock(mbi);

				// Create memblock
				g_Memblock_MakeMemblock ( mbi, gpNetDataDWORDSize );
				if(g_Memblock_GetMemblockExist(mbi))
				{
					// Put message in memblock
					LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
					DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
					memcpy(pMemblockData, (char*)gpNetDataDWORD, gpNetDataDWORDSize);

					// Turn memblock into media
					g_Memblock_ImageFromMemblock(imageindex,mbi);
				}
				else
					RunTimeError(RUNTIMEERROR_B3DERROR);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);	
	}
}

DARKSDK void NetMsgBitmap(int bitmapindex)
{
	if(gdwNetDataType==6)
	{
		int mbi=257;
		if(bitmapindex>=0 && bitmapindex<MAXIMUMVALUE)
		{
			if(gpNetDataDWORDSize>0)
			{
				// Free existing memblock
				if(g_Memblock_GetMemblockExist(mbi)) g_Memblock_DeleteMemblock(mbi);

				// Create memblock
				g_Memblock_MakeMemblock ( mbi, gpNetDataDWORDSize );
				if(g_Memblock_GetMemblockExist(mbi))
				{
					// Put message in memblock
					LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
					DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
					memcpy(pMemblockData, (char*)gpNetDataDWORD, gpNetDataDWORDSize);

					// Turn memblock into media
					g_Memblock_BitmapFromMemblock(bitmapindex,mbi);
				}
				else
					RunTimeError(RUNTIMEERROR_B3DERROR);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
	}
}

DARKSDK void NetMsgSound(int soundindex)
{
	if(gdwNetDataType==7)
	{
		int mbi=257;
		if(soundindex>=1 && soundindex<MAXIMUMVALUE)
		{
			if(gpNetDataDWORDSize>0)
			{
				// Free existing memblock
				if(g_Memblock_GetMemblockExist(mbi)) g_Memblock_DeleteMemblock(mbi);

				// Create memblock
				g_Memblock_MakeMemblock ( mbi, gpNetDataDWORDSize );
				if(g_Memblock_GetMemblockExist(mbi))
				{
					// Put message in memblock
					LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
					DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
					memcpy(pMemblockData, (char*)gpNetDataDWORD, gpNetDataDWORDSize);

					// Turn memblock into media
					g_Memblock_SoundFromMemblock(soundindex,mbi);
				}
				else
					RunTimeError(RUNTIMEERROR_SOUNDERROR);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
	}
}

DARKSDK void NetMsgMesh(int meshindex)
{
	if(gdwNetDataType==8)
	{
		int mbi=257;
		if(meshindex>0 && meshindex<MAXIMUMVALUE)
		{
			if(gpNetDataDWORDSize>0)
			{
				// Free existing memblock
				if(g_Memblock_GetMemblockExist(mbi)) g_Memblock_DeleteMemblock(mbi);

				// Create memblock
				g_Memblock_MakeMemblock ( mbi, gpNetDataDWORDSize );
				if(g_Memblock_GetMemblockExist(mbi))
				{
					// Put message in memblock
					LPSTR pMemblockData = (LPSTR)g_Memblock_GetMemblockPtr(mbi);
					DWORD dwMemblockSize = g_Memblock_GetMemblockSize(mbi);
					memcpy(pMemblockData, (char*)gpNetDataDWORD, gpNetDataDWORDSize);

					// Turn memblock into media
					g_Memblock_MeshFromMemblock(meshindex,mbi);
				}
				else
					RunTimeError(RUNTIMEERROR_B3DERROR);
			}
		}
		else
			RunTimeError(RUNTIMEERROR_B3DMESHNUMBERILLEGAL);
	}
}

DARKSDK int NetSessionExists(void)
{
	if(gGameSessionActive>0)
		return 1;

	return 0;
}

DARKSDK int NetSessionLost(void)
{
	int iValue=(int)gbSystemSessionLost;
	if(pCNetwork) pCNetwork->GetSysNetMsgIfAny();
	gbSystemSessionLost=false;
	return iValue;
}

DARKSDK int NetPlayerCreated(void)
{
	int iValue=0;
	if(pCNetwork) pCNetwork->GetSysNetMsgIfAny();
	if(gSystemPlayerCreated>0)
	{
		iValue=findseqidindatabase(gSystemPlayerCreated);
		gSystemPlayerCreated=0;
	}
	return iValue;
}

DARKSDK int NetPlayerDestroyed(void)
{
	int iValue=0;
	if(pCNetwork) pCNetwork->GetSysNetMsgIfAny();
	if(gSystemPlayerDestroyed>0)
	{
		iValue=findseqidindatabase(gSystemPlayerDestroyed);
		gSystemPlayerDestroyed=0;
	}
	return iValue;
}

DARKSDK int NetSessionIsNowHosting(void)
{
	int iValue;
	if(pCNetwork) pCNetwork->GetSysNetMsgIfAny();
	iValue=gSystemSessionIsNowHosting;
	gSystemSessionIsNowHosting=0;
	return iValue;
}

/*
void AlwaysActiveOn(void)
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

void AlwaysActiveOff(void)
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}
*/

//
// New Multiplayer Commands
//

DARKSDK int MagicNetGame(DWORD lpGameName, DWORD lpPlayerName, int PlayerMax, int FlagNum )
{
	// Player Num is returned
	int iPlayerNumber=0;

	// First try to join
	InternalJoinNetGame(1, (LPSTR)lpPlayerName);
	if(gGameSessionActive==0)
	{
		// Ok, create as fresh host to new game
		CreateNetGameEx( (LPSTR)lpGameName, (LPSTR)lpPlayerName, PlayerMax, FlagNum );
		iPlayerNumber=findseqidindatabase(pCNetwork->m_LocalPlayerDPID);
	}
	else
	{
		// In Magic creation, Order of players handled by ore-filling existing players
		char* data[256];
		DPID dpids[256];
		ZeroMemory(data, sizeof(data));
		ZeroMemory(dpids, sizeof(dpids));
		DWORD playernum=pCNetwork->GetNetPlayers(data, dpids);
		for(DWORD i=0; i<playernum; i++)
			if(data[i])
				if(pCNetwork->m_LocalPlayerDPID!=dpids[i])
					int ExistingPlayerNumber = findseqidindatabase(dpids[i]);

		// Get number if this player
		iPlayerNumber=findseqidindatabase(pCNetwork->m_LocalPlayerDPID);
	}
	return iPlayerNumber;
}

DARKSDK int NetBufferSize(void)
{
	if(gpNetQueue)
		return gpNetQueue->QueueSize();
	else
		return 0;
}


//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorMultiplayer ( void )
{
	Constructor ( );
}

void DestructorMultiplayer ( void )
{
	Destructor ( );
}

void SetErrorHandlerMultiplayer ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataMultiplayer ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DMultiplayer ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbCreateNetGame(LPSTR gamename, LPSTR name, int playermax)
{
	CreateNetGame( gamename,  name,  playermax);
}

void dbCreateNetGame (LPSTR gamename, LPSTR name, int playermax, int flagnum)
{
	CreateNetGameEx( gamename,  name,  playermax,  flagnum);
}

void dbJoinNetGame(int sessionnum, LPSTR name)
{
	JoinNetGame( sessionnum, name);
}

void dbFreeNetGame(void)
{
	CloseNetGame();
}

void dbPerformChecklistForNetConnections(void)
{
	PerformChecklistForNetConnections();
}

void dbSetNetConnections(int index)
{
	SetNetConnections(index);
}

void dbSetNetConnections(int index, LPSTR ipaddress)
{
	SetNetConnectionsEx( index,  ipaddress);
}

void dbPerformChecklistForNetSessions(void)
{
	PerformChecklistForNetSessions();
}

void dbPerformChecklistForNetPlayers(void)
{
	PerformChecklistForNetPlayers();
}

void dbCreateNetPlayer(LPSTR playername)
{
	CreatePlayer( playername);
}

int dbCreateNetPlayerEx(LPSTR playername)
{
	return CreatePlayerEx( playername);
}

void dbDestroyNetPlayer(int playerid)
{
	DestroyPlayer( playerid);
}

void dbSendNetMessageInteger(int playerid, int MessageData)
{
	SendNetMsgL( playerid,  MessageData);
}

void dbSendNetMessageFloat(int playerid, float MessageData)
{
	SendNetMsgF( playerid,  ( DWORD ) MessageData);
}

void dbSendNetMessageString(int playerid, LPSTR pMessageData)
{
	SendNetMsgS( playerid,  pMessageData);
}

void dbSendNetMessageMemblock(int playerid, int mbi)
{
	SendNetMsgMemblock( playerid,  mbi);
}

void dbSendNetMessageMemblock(int playerid, int mbi, int gua)
{
	SendNetMsgMemblockEx( playerid,  mbi,  gua);
}

void dbSendNetMessageImage(int playerid, int imageindex, int gua)
{
	SendNetMsgImage( playerid,  imageindex,  gua);
}

void dbSendNetMessageBitmap(int playerid, int bitmapindex, int gua)
{
	SendNetMsgBitmap( playerid,  bitmapindex,  gua);
}

void dbSendNetMessageSound(int playerid, int soundindex, int gua)
{
	SendNetMsgSound( playerid,  soundindex,  gua);
}

void dbSendNetMessageMesh(int playerid, int meshindex, int gua)
{
	SendNetMsgMesh( playerid,  meshindex,  gua);
}

void dbGetNetMessage(void)
{
	GetNetMsg ( );
}

int dbNetMessageInteger(void)
{
	return NetMsgInteger ( );
}

float dbNetMessageFloat(void)
{
	DWORD dwReturn = NetMsgFloat ( );
	
	return *( float* ) &dwReturn;
}

char* dbNetMessageString(void)
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = NetMsgString ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

void dbNetMessageMemblock(int mbi)
{
	NetMsgMemblock( mbi);
}

void dbNetMessageImage(int imageindex)
{
	NetMsgImage( imageindex);
}

void dbNetMessageBitmap(int bitmapindex)
{
	NetMsgBitmap( bitmapindex);
}

void dbNetMessageSound(int soundindex)
{
	NetMsgSound( soundindex);
}

void dbNetMessageMesh(int meshindex)
{
	NetMsgMesh( meshindex);
}

int dbNetMessageExists(void)
{
	return NetMsgExists();
}

int dbNetMessageType(void)
{
	return NetMsgType ( );
}

int dbNetMessagePlayerFrom(void)
{
	return NetMsgPlayerFrom();
}

int dbNetMessagePlayerTo(void)
{
	return NetMsgPlayerTo();
}

int dbNetSessionExists(void)
{
	return NetSessionExists();
}

int dbNetSessionLost(void)
{
	return NetSessionLost();
}

int dbNetPlayerCreated(void)
{
	return NetPlayerCreated();
}

int dbNetPlayerDestroyed(void)
{
	// lee - 300706 - fixed recursive error
	return NetPlayerDestroyed();
}

int dbNetGameNowHosting(void)
{
	return NetSessionIsNowHosting();
}

int dbDefaultNetGame(char* lpGameName, char* lpPlayerName, int PlayerMax, int FlagNum )
{
	return MagicNetGame( ( DWORD ) lpGameName,  ( DWORD ) lpPlayerName,  PlayerMax, FlagNum );
}

int dbNetBufferSize(void)
{
	return NetBufferSize ();
}

// lee - 300706 - GDK fixes
void dbFreeNetPlayer ( int playerid ) { dbDestroyNetPlayer ( playerid ); }
void dbSetNetConnection	( int index ) { SetNetConnections ( index ); }
void dbSetNetConnection ( int index, LPSTR ipaddress ) { dbSetNetConnections ( index, ipaddress ); }
int dbNetGameExists ( void ) { return dbNetSessionExists (); }
int dbNetGameLost ( void ) { return dbNetSessionLost (); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////