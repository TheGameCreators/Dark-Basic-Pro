
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	OGG VORBIS COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
//#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include "core.h"

/*
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"

#pragma comment ( lib, "vorbis/lib/ogg_static.lib"        )
#pragma comment ( lib, "vorbis/lib/vorbis_static.lib"     )
#pragma comment ( lib, "vorbis/lib/vorbisfile_static.lib" )
#pragma comment ( lib, "vorbis/lib/vorbisenc_static.lib"  )
*/

#define MIN_SOUND	0
#define MAX_SOUND	255

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK void LoadOggVorbis      ( DWORD dwFileName, int iID );
DARKSDK void DeleteOggVorbis    ( int iID );
DARKSDK void PlayOggVorbis      ( int iID );
DARKSDK void LoopOggVorbis      ( int iID );
DARKSDK void StopOggVorbis      ( int iID );
DARKSDK void PauseOggVorbis     ( int iID );
DARKSDK void ResumeOggVorbis    ( int iID );
DARKSDK void SetOggVorbisVolume ( int iID, int iVolume );
DARKSDK void EncodeToOggVorbis  ( DWORD dwFileA, DWORD dwFileB );
DARKSDK void EncodeToOggVorbis  ( DWORD dwFileA, DWORD dwFileB, float fQuality );
DARKSDK int  GetOggVorbisExists ( int iID );
DARKSDK int  GetOggVorbisState  ( int iID );
DARKSDK int  GetOggVorbisVolume ( int iID );

		void SetStates          ( int iID, bool bPlaying, bool bPaused, bool bStopped, bool bLooping );
		void Error              ( int iID );

/*
	// export names for string table -

		LOAD OGG VORBIS%SL%?LoadOggVorbis@@YAXKH@Z%Filename, ID
		DELETE OGG VORBIS%L%?DeleteOggVorbis@@YAXH@Z%ID
		PLAY OGG VORBIS%L%?PlayOggVorbis@@YAXH@Z%ID
		LOOP OGG VORBIS%L%?LoopOggVorbis@@YAXH@Z%ID
		STOP OGG VORBIS%L%?StopOggVorbis@@YAXH@Z%ID
		PAUSE OGG VORBIS%L%?PauseOggVorbis@@YAXH@Z%ID
		RESUME OGG VORBIS%L%?ResumeOggVorbis@@YAXH@Z%ID
		SET OGG VORBIS VOLUME%LL%?SetOggVorbisVolume@@YAXH@Z%ID, Volume
		ENCODE TO OGG VORBIS%SS%?EncodeToOggVorbis@@YAXKK@Z%Input File, Output File
		ENCODE TO OGG VORBIS%SSF%?EncodeToOggVorbis@@YAXKKM@Z%Input File, Output File, Quality

		GET OGG VORBIS EXISTS[%LL%?GetOggVorbisExists@@YAHH@Z%ID
		GET OGG VORBIS STATE[%LL%?GetOggVorbisState@@YAHH@Z%ID
		GET OGG VORBIS VOLUME[%LL%?GetOggVorbisVolume@@YAHH@Z%ID
*/


bool Check ( int iID, bool bBuffer = true )
{
	if ( iID >= 255 || iID <= 0 )
		Error ( 9 );

	if ( bBuffer )
	{
		if ( !g_SoundList [ iID ].pBuffer )
			Error ( 10 );
	}

	return true;
}

void LoadOggVorbis ( DWORD dwFileName, int iID )
{
	typedef void ( *LoadOggVorbisPFN ) ( DWORD, int );
	typedef void ( *PlayOggVorbisPFN ) ( int );
	
	HINSTANCE			hOgg			= LoadLibrary ( "OggVorbis.dll" );
	LoadOggVorbisPFN	LoadOggVorbisP  = NULL;
	PlayOggVorbisPFN	PlayOggVorbisP  = NULL;

	LoadOggVorbisP = ( LoadOggVorbisPFN ) GetProcAddress ( hOgg, "?LoadOggVorbis@@YAXKH@Z" );
	PlayOggVorbisP = ( PlayOggVorbisPFN ) GetProcAddress ( hOgg, "?PlayOggVorbis@@YAXH@Z" );

	LoadOggVorbisP ( dwFileName, iID );
	PlayOggVorbisP ( iID );
}

void DeleteOggVorbis ( int iID )
{
	Check ( iID );

	SAFE_RELEASE ( g_SoundList [ iID ].pBuffer );

	memset ( &g_SoundList [ iID ], 0, sizeof ( sSound ) );
}

void PlayOggVorbis ( int iID )
{
	Check ( iID );

	// play the buffer
	g_SoundList [ iID ].pBuffer->Play ( 0, 0, 0 );

	SetStates ( iID, true, false, false, g_SoundList [ iID ].bLooping );
}

void LoopOggVorbis ( int iID )
{
	Check ( iID );

	// play and loop buffer
	
	g_SoundList [ iID ].pBuffer->Play ( 0, 0, DSBPLAY_LOOPING );

	SetStates ( iID, true, false, false, true );
}

void StopOggVorbis ( int iID )
{
	Check ( iID );

	// stop the buffer from playing
	g_SoundList [ iID ].pBuffer->Stop ( );
	g_SoundList [ iID ].pBuffer->SetCurrentPosition ( 0 );

	SetStates ( iID, false, false, true, g_SoundList [ iID ].bLooping );
}

void PauseOggVorbis ( int iID )
{
	Check ( iID );

	// get the position
	g_SoundList [ iID ].pBuffer->GetCurrentPosition ( &g_SoundList [ iID ].dwPosition, NULL );
	g_SoundList [ iID ].pBuffer->Stop ( );

	SetStates ( iID, false, true, false, g_SoundList [ iID ].bLooping );
}

void ResumeOggVorbis ( int iID )
{
	Check ( iID );

	if ( g_SoundList [ iID ].bPaused )
	{
		g_SoundList [ iID ].pBuffer->SetCurrentPosition ( g_SoundList [ iID ].dwPosition );

		if ( g_SoundList [ iID ].bLooping )
			g_SoundList [ iID ].pBuffer->Play ( 0, 0, DSBPLAY_LOOPING );
		else
			g_SoundList [ iID ].pBuffer->Play ( 0, 0, 0 );

		SetStates ( iID, true, false, false, g_SoundList [ iID ].bLooping );
	}
}

void SetOggVorbisVolume ( int iID, int iVolume )
{
	Check ( iID );

	// set the volume
	g_SoundList [ iID ].pBuffer->SetVolume ( (int)(-9600*((100-iVolume)/100.0)) );
}

void EncodeToOggVorbis ( DWORD dwFileA, DWORD dwFileB )
{
	// encode a wave file into the ogg vorbis format

	//EncodeToOggVorbis ( dwFileA, dwFileB, 0.5f );
}

void EncodeToOggVorbis ( DWORD dwFileA, DWORD dwFileB, float fQuality )
{
	//extern int Encode ( char* szInput, char* szOutput, float fQuality );

	//Encode ( ( char* ) dwFileA, ( char* ) dwFileB, fQuality );
}

int GetOggVorbisExists ( int iID )
{
	if ( !Check ( iID ) )
		return 0;

	return 1;
}

int GetOggVorbisState ( int iID )
{
	Check ( iID );

	DWORD dwStatus;
	
	g_SoundList [ iID ].pBuffer->GetStatus ( &dwStatus );

	if ( dwStatus & DSBSTATUS_PLAYING )
		return 1;

	if ( g_SoundList [ iID ].bStopped )
		return 0;

	if ( g_SoundList [ iID ].bPaused )
		return 2;

	return -1;
}

int GetOggVorbisVolume ( int iID )
{
	// get the volume of the buffer

	Check ( iID );

	long lVolume = 0;

	g_SoundList [ iID ].pBuffer->GetVolume ( &lVolume );

	return ( int ) lVolume;
}

void SetStates ( int iID, bool bPlaying, bool bPaused, bool bStopped, bool bLooping )
{
	g_SoundList [ iID ].bPlaying = bPlaying;
	g_SoundList [ iID ].bPaused  = bPaused;
	g_SoundList [ iID ].bStopped = bStopped;
	g_SoundList [ iID ].bLooping = bLooping;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
