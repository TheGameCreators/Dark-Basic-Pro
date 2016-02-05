
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	SPEECH COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
//#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
//#include <windows.h>
#include <sapi.h>
#include <atlbase.h>
#include <sphelper.h>
#include "core.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK int  GetVoiceAPI		( void );
DARKSDK void GetVoiceList		( void );
DARKSDK void SetVoice			( int iID );
DARKSDK void SetVoiceSpeed		( int iSpeed );
DARKSDK void SetVoiceVolume		( int iVolume );
DARKSDK void SendVoiceMessage	( DWORD dwMessage );
DARKSDK void SendVoiceMessageXML( DWORD dwMessage );
DARKSDK int  GetVoiceSpeed		( void );
DARKSDK int  GetVoiceVolume		( void );

		void VoiceSetup			( void );
		void VoiceDestroy		( void );
		void SetupVoices        ( int iMode, int iID );
		void SendVoiceMessage   ( DWORD dwMessage, int iOption );

/*
	// export names for string table -

	GET VOICE API[%L%?GetVoiceAPI@@YAHXZ%
	GET VOICE LIST%0%?GetVoiceList@@YAXXZ%
	SET VOICE%L%?SetVoice@@YAXH@Z%Voice Index
	SET VOICE SPEED%L%?SetVoiceSpeed@@YAXH@Z%Speed
	SET VOICE VOLUME%L%?SetVoiceVolume@@YAXH@Z%Volume
	SEND VOICE MESSAGE%S%?SendVoiceMessage@@YAXK@Z%Message
	SEND VOICE MESSAGE XML%S%?SendVoiceMessageXML@@YAXK@Z%Message
	GET VOICE SPEED[%L%?GetVoiceSpeed@@YAHXZ%
	GET VOICE VOLUME[%L%?GetVoiceVolume@@YAHXZ%
*/

ISpVoice* g_pVoice = NULL;

void VoiceSetup	( void )
{
	g_pVoice = NULL;

	if ( FAILED ( CoInitialize ( NULL ) ) )
		return;
	
    if ( FAILED ( CoCreateInstance ( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, ( void** ) &g_pVoice ) ) )
		return;
}

void VoiceDestroy ( void )
{
	if ( g_pVoice )
	{
		g_pVoice->Release ( );
		g_pVoice = NULL;

		CoUninitialize ( );
	}
}

int GetVoiceAPI ( void )
{
	if ( g_pVoice == NULL )
		return 0;

	return 1;
}

void SetupVoices ( int iMode, int iID )
{
	// enumerate the voices and either set the voice
	// or fill the checklist with data

	if ( !g_pVoice )
	{
		Error ( 10 );
		return;
	}

	// local variables
	IEnumSpObjectTokens*	pEnum       = NULL;
	ISpObjectToken*         pVoiceToken = NULL;
	ULONG					ulCount     = 0;
	int						iLoop       = 0;
	WCHAR*					pToken      = NULL;
	char					szBuffer [ 256 ];

	// enumerate the tokens
	if ( FAILED ( SpEnumTokens ( SPCAT_VOICES, NULL, NULL, &pEnum ) ) )
	{
		Error ( 10 );
		return;
	}

	// find out how many voices we have
	pEnum->GetCount ( &ulCount );

	if ( !g_pGlob )
	{
		Error ( 1 );
		return;
	}

	// build checklist if we're in mode 1
	if ( iMode == 1 )
	{
		g_pGlob->checklistexists     = true;
		g_pGlob->checklisthasstrings = true;
		g_pGlob->checklistqty        = ( int ) ulCount;
	}

	// loop round
	while ( iLoop++ < ( int ) ulCount )
	{
		// get the next token
		pEnum->Next ( 1, &pVoiceToken, NULL );

		// set the voice in mode 0
		if ( iMode == 0 && iLoop == iID )
			g_pVoice->SetVoice ( pVoiceToken );

		// in mode 1 get the id
		if ( iMode == 1 )
		{
			// extract id
			pVoiceToken->GetId ( &pToken );

			// convert from wide char to ansi
			WideCharToMultiByte ( CP_ACP, 0, pToken, -1, szBuffer, 256 * sizeof ( CHAR ), NULL, NULL );

			// expand the checklist
			GlobExpandChecklist ( iLoop - 1, 256 );

			// copy data into checklist
			strcpy ( g_pGlob->checklist [ iLoop - 1 ].string, szBuffer );
		}
		
		// release the voice token
		pVoiceToken->Release ( );
	}
}

void GetVoiceList ( void )
{
	SetupVoices ( 1, -1 );
}

void SetVoice ( int iID )
{
	SetupVoices ( 0, iID );
}

void SetVoiceSpeed ( int iSpeed )
{
	if ( g_pVoice )
		g_pVoice->SetRate ( iSpeed );
}

void SetVoiceVolume ( int iVolume )
{
	if ( g_pVoice )
		g_pVoice->SetVolume ( iVolume );
}

void SendVoiceMessage ( DWORD dwMessage )
{
	SendVoiceMessage ( dwMessage, 0 );
}

void SendVoiceMessageXML ( DWORD dwMessage )
{
	SendVoiceMessage ( dwMessage, 1 );
}

void SendVoiceMessage ( DWORD dwMessage, int iOption )
{
	if ( g_pVoice )
    {
		DWORD  dwLength    = strlen ( ( char* ) dwMessage ) + 1;
		WCHAR* pWideSpeech = new WCHAR [ dwLength ];

		memset ( pWideSpeech, 0, sizeof ( WCHAR ) * dwLength );

		if ( !pWideSpeech )
			return;

		MultiByteToWideChar ( CP_ACP, 0, ( char* ) dwMessage, -1, pWideSpeech, dwLength );

		if ( iOption == 0 )
			g_pVoice->Speak ( pWideSpeech, SPF_ASYNC, NULL );
		else
			g_pVoice->Speak ( pWideSpeech, SPF_ASYNC | SPF_IS_XML, NULL );
			

		delete [ ] pWideSpeech;
    }
}

int GetVoiceSpeed ( void )
{
	if ( !g_pVoice )
	{
		return -1;
	}

	LONG lRate = 0;

	g_pVoice->GetRate ( &lRate );

	return ( int ) lRate;
}

int GetVoiceVolume ( void )
{
	if ( !g_pVoice )
	{
		return -1;
	}

	USHORT usVolume = 0;

	g_pVoice->GetVolume ( &usVolume );

	return ( int ) usVolume;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
