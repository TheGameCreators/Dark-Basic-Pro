
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
#define WIN32_LEAN_AND_MEAN

#include "core.h"

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"

#pragma comment ( lib, "vorbis/lib/ogg_static.lib"        )
#pragma comment ( lib, "vorbis/lib/vorbis_static.lib"     )
#pragma comment ( lib, "vorbis/lib/vorbisfile_static.lib" )
#pragma comment ( lib, "vorbis/lib/vorbisenc_static.lib"  )

#define MIN_SOUND	0
#define MAX_SOUND	65535

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
		
		//SET OGG VORBIS VOLUME%LL%?SetOggVorbisVolume@@YAXH@Z%ID, Volume
		SET OGG VORBIS VOLUME%LL%?SetOggVorbisVolume@@YAXHH@Z%ID, Volume

  

		ENCODE TO OGG VORBIS%SS%?EncodeToOggVorbis@@YAXKK@Z%Input File, Output File
		ENCODE TO OGG VORBIS%SSF%?EncodeToOggVorbis@@YAXKKM@Z%Input File, Output File, Quality

		GET OGG VORBIS EXISTS[%LL%?GetOggVorbisExists@@YAHH@Z%ID
		GET OGG VORBIS STATE[%LL%?GetOggVorbisState@@YAHH@Z%ID
		GET OGG VORBIS VOLUME[%LL%?GetOggVorbisVolume@@YAHH@Z%ID
*/

bool Check ( int iID, bool bBuffer = true )
{
	// check that the ID is valid, the buffer parameter
	// can also be used to ensure the buffer is valid

	// check the ID
	if ( iID >= 255 || iID <= 0 )
	{
		Error ( 9 );
		return false;
	}

	// check the buffer
	if ( bBuffer )
	{
		if ( !g_SoundList [ iID ].pBuffer )
		{
			// 300805
			MessageBox ( NULL, "sound object has no buffer", "error", MB_OK );

			//Error ( 10 );
			return false;
		}
	}

	return true;
}

void LoadOggVorbis ( DWORD dwFileName, int iID )
{
	// loads an ogg vorbis file into a buffer

	// check the ID only, no need to check buffer as it
	// doesn't exist yet because we have yet to create
	// the new object
	Check ( iID, false );

	// see if sound already exists in this slot
	if ( g_SoundList [ iID ].pBuffer )
	{
		Error ( 11 );
		return;
	}

	// local variables
	char*			szFilename					= NULL;		// filename
	FILE*			fp							= NULL;		// file pointer
	__int64			iSize						= 0;		// size of file
	DWORD			dwPos						= 0;		// position in ogg vorbis file
    int				iCurrentLogicalBitStream	= 0;		// bit stream indicator
    int				iBytesRead					= 1;		// number of bytes read for ogg vorbis
	char*			pSoundBuffer				= NULL;		// sound buffer data
	vorbis_info*	OggVorbisInfo				= NULL;		// ogg vorbis file information
	OggVorbis_File  OggVorbis;								// ogg vorbis file structure
	WAVEFORMATEX	wfm;									// wave format
	DSBUFFERDESC	desc;									// buffer description
	
	// get filename
	szFilename = ( char* ) dwFileName;

	// check the filename pointer is valid
	if ( !szFilename )
		Error ( 3 );

	// attempt to open the file
	fp = fopen ( szFilename, "rb" );
	
	// return if invalid file
	if ( !fp )
	{
		Error ( 4 );
		return;
	}

	// open the ogg vorbis file
	if ( ov_open ( fp, &OggVorbis, NULL, 0 ) < 0 )
	{
		Error ( 5 );
		return;
	}

	// get the size of the PCM file and multiply by 4
	iSize  = ov_pcm_total ( &OggVorbis ,-1 );
	iSize *= 4;

	// get information about the file
    OggVorbisInfo = ov_info ( &OggVorbis, -1 );

	// check returned pointer is valid
	if ( !OggVorbisInfo )
	{
		Error ( 6 );
		return;
	}

	// set the wave format
	memset ( &wfm, 0, sizeof ( wfm ) );

    wfm.cbSize          = sizeof ( wfm );							// size of the structure
    wfm.nChannels       = OggVorbisInfo->channels;					// number of channels
    wfm.wBitsPerSample  = 16;										// bits per sample
    wfm.nSamplesPerSec  = OggVorbisInfo->rate;						// rate
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;	// bytes per second
    wfm.nBlockAlign     = 2 * wfm.nChannels;						// block align
    wfm.wFormatTag      = 1;										// format tag

    // clear the information about the buffer
	memset ( &desc, 0, sizeof ( desc ) );

	// set buffer values
	desc.dwSize        = sizeof ( desc );	// size of structure
	desc.dwFlags       = DSBCAPS_CTRLVOLUME;					// specify flags
	desc.lpwfxFormat   = &wfm;				// format
	desc.dwReserved    = 0;					// ignore
	desc.dwBufferBytes = ( DWORD ) iSize;	// size of buffer

	// create the new sound buffer
	if ( FAILED ( g_pSound->CreateSoundBuffer ( &desc, &g_SoundList [ iID ].pBuffer, NULL ) ) )
	{
		Error ( 7 );
		return;
	}

	// lock the buffer
    if ( FAILED ( g_SoundList [ iID ].pBuffer->Lock ( 0, ( int ) iSize, ( LPVOID* ) &pSoundBuffer, ( ULONG* ) &iSize, NULL, NULL, DSBLOCK_ENTIREBUFFER ) ) )
	{
		Error ( 8 );
		return;
	}
    
    // now read in the bits
    while ( iBytesRead && dwPos < ( DWORD ) iSize )
    {
        iBytesRead  = ov_read ( &OggVorbis, pSoundBuffer + dwPos, ( int ) iSize - dwPos, 0, 2, 1, &iCurrentLogicalBitStream );
        dwPos      += iBytesRead;
    }

	// unlock the buffer
	g_SoundList [ iID ].pBuffer->Unlock ( pSoundBuffer, ( int ) iSize, NULL, NULL );

	// close the file
	fclose ( fp );

	// set the states
	SetStates ( iID, false, false, true, false );
}

void DeleteOggVorbis ( int iID )
{
	// delete an ogg vorbis file

	// check everything is okay
	Check ( iID );

	// safely release the buffer
	SAFE_RELEASE ( g_SoundList [ iID ].pBuffer );

	// clear the ID
	memset ( &g_SoundList [ iID ], 0, sizeof ( sSound ) );
}

void PlayOggVorbis ( int iID )
{
	// play an ogg vorbis file

	// check all is okay
	Check ( iID );

	// play the buffer
	g_SoundList [ iID ].pBuffer->Play ( 0, 0, 0 );

	// set the states
	SetStates ( iID, true, false, false, g_SoundList [ iID ].bLooping );
}

void LoopOggVorbis ( int iID )
{
	// loop an ogg vorbis file

	// check id
	Check ( iID );

	// play and loop buffer
	g_SoundList [ iID ].pBuffer->Play ( 0, 0, DSBPLAY_LOOPING );

	// set new states
	SetStates ( iID, true, false, false, true );
}

void StopOggVorbis ( int iID )
{
	// stop an ogg vorbis file

	// check the id
	Check ( iID );

	// stop the buffer from playing and reset the position
	g_SoundList [ iID ].pBuffer->Stop ( );
	g_SoundList [ iID ].pBuffer->SetCurrentPosition ( 0 );

	// set the new states
	SetStates ( iID, false, false, true, g_SoundList [ iID ].bLooping );
}

void PauseOggVorbis ( int iID )
{
	// pause an ogg vorbis file

	// check all is okay
	Check ( iID );

	// get the position
	g_SoundList [ iID ].pBuffer->GetCurrentPosition ( &g_SoundList [ iID ].dwPosition, NULL );
	g_SoundList [ iID ].pBuffer->Stop ( );

	// set the new states
	SetStates ( iID, false, true, false, g_SoundList [ iID ].bLooping );
}

void ResumeOggVorbis ( int iID )
{
	// resume an ogg vorbis file

	// we must check the ID
	Check ( iID );

	// see if it is paused
	if ( g_SoundList [ iID ].bPaused )
	{
		// set the new position
		g_SoundList [ iID ].pBuffer->SetCurrentPosition ( g_SoundList [ iID ].dwPosition );

		// see if we must resume and loop or just play normal
		if ( g_SoundList [ iID ].bLooping )
		{
			g_SoundList [ iID ].pBuffer->Play ( 0, 0, DSBPLAY_LOOPING );
		}
		else
		{
			g_SoundList [ iID ].pBuffer->Play ( 0, 0, 0 );
		}

		// set the new states
		SetStates ( iID, true, false, false, g_SoundList [ iID ].bLooping );
	}
}

void SetOggVorbisVolume ( int iID, int iVolume )
{
	// set the volume of an ogg vorbis file

	// check the ID is valid
	Check ( iID );

	// set the volume
	g_SoundList [ iID ].pBuffer->SetVolume ( ( int ) ( -9600 * ( ( 100 - iVolume ) / 100.0 ) ) );
}

void EncodeToOggVorbis ( DWORD dwFileA, DWORD dwFileB )
{
	// encode a wave file into the ogg vorbis format using default quality
	EncodeToOggVorbis ( dwFileA, dwFileB, 0.5f );
}

void EncodeToOggVorbis ( DWORD dwFileA, DWORD dwFileB, float fQuality )
{
	// encode to ogg vorbis using a set quality value

	extern int Encode ( char* szInput, char* szOutput, float fQuality );

	Encode ( ( char* ) dwFileA, ( char* ) dwFileB, fQuality );
}

int GetOggVorbisExists ( int iID )
{
	// return true if an Ogg Vorbis file exists

	// check the ID
	if ( iID >= 255 || iID <= 0 )
	{
		Error ( 9 );
		return false;
	}

	if ( !g_SoundList [ iID ].pBuffer )
	{
		return 0;
	}

	return 1;
}

int GetOggVorbisState ( int iID )
{
	// get the state of an ogg vorbis file

	// check the ID is value
	Check ( iID );

	// status variable
	DWORD dwStatus = 0;
	
	// call a get status check
	g_SoundList [ iID ].pBuffer->GetStatus ( &dwStatus );

	// if playing return 1
	if ( dwStatus & DSBSTATUS_PLAYING )
		return 1;

	if ( !( dwStatus & DSBSTATUS_PLAYING ) )
		g_SoundList [ iID ].bStopped = true;

	// if stopped return 0
	if ( g_SoundList [ iID ].bStopped )
		return 0;

	// if paused return 2
	if ( g_SoundList [ iID ].bPaused )
		return 2;

	// state unknown
	return -1;
}

int GetOggVorbisVolume ( int iID )
{
	// get the volume of the buffer

	// check the ID
	Check ( iID );

	// volume variable
	long lVolume = 0;

	// get the volume
	g_SoundList [ iID ].pBuffer->GetVolume ( &lVolume );

	// and return it
	return ( int ) lVolume;
}

void SetStates ( int iID, bool bPlaying, bool bPaused, bool bStopped, bool bLooping )
{
	// set the states of an ogg vorbis file

	g_SoundList [ iID ].bPlaying = bPlaying;		// playing
	g_SoundList [ iID ].bPaused  = bPaused;			// paused
	g_SoundList [ iID ].bStopped = bStopped;		// stopped
	g_SoundList [ iID ].bLooping = bLooping;		// looping
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
