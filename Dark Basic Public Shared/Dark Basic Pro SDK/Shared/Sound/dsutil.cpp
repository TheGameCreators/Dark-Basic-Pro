//-----------------------------------------------------------------------------
// File: DSUtil.cpp
//
// Desc: DirectSound framework classes for reading and writing wav files and
//       playing them in DirectSound buffers. Feel free to use this class 
//       as a starting point for adding extra functionality.
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)

// Includes
#include <windows.h>
#include <mmsystem.h>
//#include <dxerr8.h>
#include <dsound.h>
#include "DSUtil.h"
#include "DXUtil.h"
#include "ADPCMtoWAV.h" // contains WaveDecompress

/////////////////////////////////////////////////////////////////////////////////////
// mike 260705 ///////////////////////////////////////////////////////////////////////
#ifndef DARKSDK_COMPILE

#include ".\..\core\globstruct.h"
extern GlobStruct* g_pGlob;


#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"

#pragma comment ( lib, "vorbis/lib/ogg_static.lib"        )
#pragma comment ( lib, "vorbis/lib/vorbis_static.lib"     )
#pragma comment ( lib, "vorbis/lib/vorbisfile_static.lib" )
#pragma comment ( lib, "vorbis/lib/vorbisenc_static.lib"  )

LPDIRECTSOUNDBUFFER* CSoundManager::LoadOggVorbis ( char* dwFileName, DWORD* pdwSizeOfSound )
{
	LPDIRECTSOUNDBUFFER* pDSBuffer = NULL;

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
	
	pDSBuffer = new LPDIRECTSOUNDBUFFER[1];

	// get filename
	szFilename = ( char* ) dwFileName;

	// attempt to open the file
	fp = fopen ( szFilename, "rb" );
	
	// return if invalid file
	if ( !fp )
	{
		return NULL;
	}

	// open the ogg vorbis file
	if ( ov_open ( fp, &OggVorbis, NULL, 0 ) < 0 )
	{
		return NULL;
	}

	// get the size of the PCM file and multiply by 4
	int iPCMTotalSize = static_cast<int>(ov_pcm_total ( &OggVorbis ,-1 ));
	iSize = iPCMTotalSize * 4;

	// get information about the file
    OggVorbisInfo = ov_info ( &OggVorbis, -1 );

	// check returned pointer is valid
	if ( !OggVorbisInfo )
	{
		return NULL;
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
	if ( FAILED ( m_pDS->CreateSoundBuffer ( &desc, &pDSBuffer [ 0 ], NULL ) ) )
	{
		return NULL;
	}

	// lock the buffer
    if ( FAILED ( pDSBuffer [ 0 ]->Lock ( 0, ( int ) iSize, ( LPVOID* ) &pSoundBuffer, ( ULONG* ) &iSize, NULL, NULL, DSBLOCK_ENTIREBUFFER ) ) )
	{
		return NULL;
	}
    
    // now read in the bits
    while ( iBytesRead && dwPos < ( DWORD ) iSize )
    {
        iBytesRead  = ov_read ( &OggVorbis, pSoundBuffer + dwPos, ( int ) iSize - dwPos, 0, 2, 1, &iCurrentLogicalBitStream );
        dwPos      += iBytesRead;
    }

	// unlock the buffer
	pDSBuffer [ 0 ]->Unlock ( pSoundBuffer, ( int ) iSize, NULL, NULL );

	// record the size of the buffer (OGG cannot detect END of sound = no loop no playing()=0!)
	// 110211 - correct calculation of size for end detection is PCM * BA
	*pdwSizeOfSound = (DWORD)iPCMTotalSize * wfm.nBlockAlign;

	// close the file
	fclose ( fp );

	return pDSBuffer;
}
#else
LPDIRECTSOUNDBUFFER* CSoundManager::LoadOggVorbis ( char* dwFileName, DWORD* pdwSizeOfSound )
{
	return NULL;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
    m_pDS = NULL;

	// mike - 010904 - 5.7 - variable to store existance of sound card
	m_bSoundCardExists = false;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
    SAFE_RELEASE( m_pDS ); 
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize( HWND  hWnd, 
                                   DWORD dwCoopLevel, 
                                   DWORD dwPrimaryChannels, 
                                   DWORD dwPrimaryFreq, 
                                   DWORD dwPrimaryBitRate )
{
    HRESULT             hr;

    SAFE_RELEASE( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
        return hr;//DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return hr;//DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );
    
    // Set primary buffer format
    if( FAILED( hr = SetPrimaryBufferFormat( dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) ) )
        return hr;//DXTRACE_ERR( TEXT("SetPrimaryBufferFormat"), hr );

	// mike - 010904 - 5.7 - if all is okay then sound card is fine
	m_bSoundCardExists = true;

    return S_OK;
}

// mike - 010904 - 5.7 - function to check sound card
bool CSoundManager::GetExists ( void )
{
	return m_bSoundCardExists;
}


//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return hr;//DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

	// set window of sound buffer to FPSC Editor
	//HWND hwndEditorWindow = FindWindow (NULL, "FPS Creator - [Editor]");
	//if ( hwndEditorWindow )
	//	if ( FAILED( hr = m_pDS->SetCooperativeLevel (hwndEditorWindow, DSSCL_PRIORITY)) )
	//	    return hr;//DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );

	// set window of sound buffer to GameGuru Editor
	//HWND hwndEditorWindow = FindWindow ("Game Guru12345",NULL);
	HWND hwndEditorWindow = FindWindow (NULL, "Game Guru - [Editor]");
	if ( hwndEditorWindow )
		if ( FAILED( hr = m_pDS->SetCooperativeLevel (hwndEditorWindow, DSSCL_PRIORITY)) )
		    return hr;
	
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return hr;//DXTRACE_ERR( TEXT("SetFormat"), hr );

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( ppDSListener == NULL )
        return E_INVALIDARG;
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return hr;//DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)ppDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return hr;//DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Create()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Create( CSound** ppSound, 
                               LPTSTR strWaveFileName, 
                               DWORD dwCreationFlags, 
                               GUID guid3DAlgorithm,
                               DWORD dwNumBuffers )
{
    HRESULT hr;
    HRESULT hrRet = S_OK;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

	// checks
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15011; 
	#endif
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

	#ifndef DARKSDK_COMPILE
	g_pGlob->dwInternalFunctionCode=15012;
	#endif
    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15013;
	#endif
    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15014; 
	#endif
    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );
    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        hr = E_FAIL;
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

	// leeadd - 270205 - u60 - convert non-PCM WAV to PCM WAV (using acm)
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15015; 
	#endif
	if ( pWaveFile->m_pwfx->wFormatTag != 0x0001 )
	{
		DWORD dwWavDataRead = 0;
		#ifndef DARKSDK_COMPILE 
		g_pGlob->dwInternalFunctionCode=15016; 
		#endif
		LPBYTE lpCopyOfBuffer = (LPBYTE)new char[dwDSBufferSize]; // compressed WAV data
		if( FAILED( hr = pWaveFile->Read( (BYTE*)lpCopyOfBuffer, dwDSBufferSize, &dwWavDataRead ) ) )           
		{
			// failed to read in WAV data now, for decompression
			SAFE_DELETE ( lpCopyOfBuffer );
			hr = E_FAIL;
			goto LFail;
		}
		LPWAVEFORMATEX lpwIN = pWaveFile->m_pwfx; // compressed format
		DWORD dwDataSize = dwDSBufferSize;
		LPBYTE lpSnd = NULL; // uncompressed WAV data
		WAVEFORMATEX wOUT;
		memcpy ( &wOUT, pWaveFile->m_pwfx, sizeof(WAVEFORMATEX) );
		DWORD dwOutSize = 0;
		lpSnd = WaveDecompress ( lpCopyOfBuffer, lpwIN, dwDataSize, &wOUT, &dwOutSize );
		memcpy ( pWaveFile->m_pwfx, &wOUT, sizeof(WAVEFORMATEX) );
		pWaveFile->m_pbData = lpSnd;
		SAFE_DELETE ( lpCopyOfBuffer );
	    dwDSBufferSize = dwOutSize;
	}

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15017; 
	#endif
    hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    if( hr == DS_NO_VIRTUALIZATION )
        hrRet = DS_NO_VIRTUALIZATION;
            
    if( FAILED(hr) )
    {
        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN (100ms) and the buffer is created
        // with DSBCAPS_CTRLFX.
        //if( hr != DSERR_BUFFERTOOSMALL )
          //  ;//DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
            
        goto LFail;
    }

	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15018; 
	#endif
    for( i=1; i<dwNumBuffers; i++ )
    {
        if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
        {
            //DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
            goto LFail;
        }
    }

    // Create the sound
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15019; 
	#endif
    *ppSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );

	// leeadd - 270206 - u60 - Free internal wav data if any
	SAFE_DELETE ( pWaveFile->m_pbData );
    SAFE_DELETE( apDSBuffer );

	// leefix - 210306- u6b4 - absolutely must close WAV file, cannot delete otherwise
	// the new CSound keeps the object alive in its own members, but file must be closed
	pWaveFile->Close();

	// success
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15020; 
	#endif

	// return success
    return hrRet;

LFail:
    // Cleanup
    SAFE_DELETE( pWaveFile );
    SAFE_DELETE( apDSBuffer );
	#ifndef DARKSDK_COMPILE 
	g_pGlob->dwInternalFunctionCode=15021; 
	#endif
    return hr;
}









//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateFromMemory()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateFromMemory( CSound** ppSound, 
                                        BYTE* pbData,
                                        ULONG  ulDataSize,
                                        LPWAVEFORMATEX pwfx,
                                        DWORD dwCreationFlags, 
                                        GUID guid3DAlgorithm,
                                        DWORD dwNumBuffers )
{
    HRESULT hr;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbData == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->OpenFromMemory( pbData,ulDataSize, pwfx, WAVEFILE_READ );


    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = ulDataSize;

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL ) ) )
    {
        //DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
        goto LFail;
    }

    for( i=1; i<dwNumBuffers; i++ )
    {
        if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
        {
           // DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
            goto LFail;
        }
    }

    // Create the sound
    *ppSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );

    SAFE_DELETE( apDSBuffer );
    return S_OK;

LFail:
    // Cleanup
   
    SAFE_DELETE( apDSBuffer );
    return hr;
}





//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateStreaming()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateStreaming( CStreamingSound** ppStreamingSound, 
                                        LPTSTR strWaveFileName, 
                                        DWORD dwCreationFlags, 
                                        GUID guid3DAlgorithm,
                                        DWORD dwNotifyCount, 
                                        DWORD dwNotifySize, 
                                        HANDLE hNotifyEvent )
{
    HRESULT hr;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( strWaveFileName == NULL || ppStreamingSound == NULL || hNotifyEvent == NULL )
        return E_INVALIDARG;

    LPDIRECTSOUNDBUFFER pDSBuffer      = NULL;
    DWORD               dwDSBufferSize = NULL;
    CWaveFile*          pWaveFile      = NULL;
    DSBPOSITIONNOTIFY*  aPosNotify     = NULL; 
    LPDIRECTSOUNDNOTIFY pDSNotify      = NULL;

    pWaveFile = new CWaveFile();
    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    // Figure out how big the DSound buffer should be 
    dwDSBufferSize = dwNotifySize * dwNotifyCount;

    // Set up the direct sound buffer.  Request the NOTIFY flag, so
    // that we are notified as the sound buffer plays.  Note, that using this flag
    // may limit the amount of hardware acceleration that can occur. 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags | 
                           DSBCAPS_CTRLPOSITIONNOTIFY | 
                           DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) )
    {
        // If wave format isn't then it will return 
        // either DSERR_BADFORMAT or E_INVALIDARG
        if( hr == DSERR_BADFORMAT || hr == E_INVALIDARG )
            return hr;//return DXTRACE_ERR_NOMSGBOX( TEXT("CreateSoundBuffer"), hr );

        return hr;//DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
    }

    // Create the notification events, so that we know when to fill
    // the buffer as the sound plays. 
    if( FAILED( hr = pDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
                                                (VOID**)&pDSNotify ) ) )
    {
        SAFE_DELETE( aPosNotify );
        return hr;//DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    aPosNotify = new DSBPOSITIONNOTIFY[ dwNotifyCount ];
    if( aPosNotify == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < dwNotifyCount; i++ )
    {
        aPosNotify[i].dwOffset     = (dwNotifySize * i) + dwNotifySize - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;             
    }
    
    // Tell DirectSound when to notify us. The notification will come in the from 
    // of signaled events that are handled in WinMain()
    if( FAILED( hr = pDSNotify->SetNotificationPositions( dwNotifyCount, 
                                                          aPosNotify ) ) )
    {
        SAFE_RELEASE( pDSNotify );
        SAFE_DELETE( aPosNotify );
        return hr;//DXTRACE_ERR( TEXT("SetNotificationPositions"), hr );
    }

    SAFE_RELEASE( pDSNotify );
    SAFE_DELETE( aPosNotify );

    // Create the sound
    *ppStreamingSound = new CStreamingSound( pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize );

    return S_OK;
}


// Added by Lee
CSound* CSoundManager::CloneSound ( CSound* pSound )
{
    HRESULT hr;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;
	DWORD				dwNumBuffers	= pSound->m_dwNumBuffers;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
		return NULL;
    }

    pWaveFile = pSound->m_pWaveFile;

	// no wave file - copy from source

	// Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pSound->m_dwDSBufferSize;

	// Clone existing sound buffer
    if( FAILED( hr = m_pDS->DuplicateSoundBuffer( pSound->GetBuffer(0), &apDSBuffer[0] ) ) )
    {
		return NULL;
    }
    for( i=1; i<dwNumBuffers; i++ )
    {
        if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
        {
			return NULL;
        }
    }

    // Create the sound
    pSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );
	pSound->m_bClone=true;

    SAFE_DELETE( apDSBuffer );
    return pSound;
}


// mike 260705
CSound::CSound( )
{
	m_dwDSBufferSize = 0;
	m_pCopyOfBuffer  = NULL;
    m_dwNumBuffers   = 1;
    m_pWaveFile      = NULL;
	m_bClone		 = false;

}

//-----------------------------------------------------------------------------
// Name: CSound::CSound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSound::CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, 
                DWORD dwNumBuffers, CWaveFile* pWaveFile )
{
    DWORD i;

    m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    for( i=0; i<dwNumBuffers; i++ )
        m_apDSBuffer[i] = apDSBuffer[i];

    m_dwDSBufferSize = dwDSBufferSize;
	m_pCopyOfBuffer  = NULL;
    m_dwNumBuffers   = dwNumBuffers;
    m_pWaveFile      = pWaveFile;
	m_bClone		 = false;

    FillBufferWithSound( m_apDSBuffer[0], FALSE );

    // Make DirectSound do pre-processing on sound effects
    for( i=0; i<dwNumBuffers; i++ )
        m_apDSBuffer[i]->SetCurrentPosition(0);
}




//-----------------------------------------------------------------------------
// Name: CSound::~CSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSound::~CSound()
{
	// leefix - 111108 - u71 - so cannot crash if delete unloaded OGG sound
	if ( m_apDSBuffer )
	{
		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			SAFE_RELEASE( m_apDSBuffer[i] ); 
		}
	}
    SAFE_DELETE_ARRAY( m_apDSBuffer ); 
	SAFE_DELETE(m_pCopyOfBuffer);

	if(m_bClone==false)
	{
	    SAFE_DELETE( m_pWaveFile );
	}
	else
	{
		m_pWaveFile=NULL;
	}
}




//-----------------------------------------------------------------------------
// Name: CSound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file 
//-----------------------------------------------------------------------------
HRESULT CSound::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) ) 
        return hr;//DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    // Lock the buffer down
    if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, 
                                 &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, 0L ) ) )
        return hr;//DXTRACE_ERR( TEXT("Lock"), hr );

	// leeadd - 270206 - u60 - may already have loaded/decompressed the wav data, so check
	if ( m_pWaveFile )
	{
		if ( m_pWaveFile->m_pbData )
		{
			// decompressed data already available, no need to read from wave file object
			SAFE_DELETE(m_pCopyOfBuffer);
			m_pCopyOfBuffer = new char[dwDSLockedBufferSize];
			memcpy ( m_pCopyOfBuffer, m_pWaveFile->m_pbData, dwDSLockedBufferSize );
			dwWavDataRead = dwDSLockedBufferSize;
		}
		else
		{
			// Reset the wave file to the beginning 
			m_pWaveFile->ResetFile();

			// Read First into buffer
			SAFE_DELETE(m_pCopyOfBuffer);
			m_pCopyOfBuffer = new char[dwDSLockedBufferSize];
			if( FAILED( hr = m_pWaveFile->Read( (BYTE*)m_pCopyOfBuffer,
												dwDSLockedBufferSize, 
												&dwWavDataRead ) ) )           
			{
				return hr;//DXTRACE_ERR( TEXT("Read"), hr );
			}
		}

		// Read buffer into directsoundbuffer
		memcpy((BYTE*)pDSLockedBuffer, m_pCopyOfBuffer, dwDSLockedBufferSize);

		if( dwWavDataRead == 0 )
		{
			// Wav is blank, so just fill with silence
			FillMemory( (BYTE*) pDSLockedBuffer, 
						dwDSLockedBufferSize, 
						(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
		}
		else if( dwWavDataRead < dwDSLockedBufferSize )
		{
			// If the wav file was smaller than the DirectSound buffer, 
			// we need to fill the remainder of the buffer with data 
			if( bRepeatWavIfBufferLarger )
			{       
				// Reset the file and fill the buffer with wav data
				DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
				while( dwReadSoFar < dwDSLockedBufferSize )
				{  
					// This will keep reading in until the buffer is full 
					// for very short files
					if( FAILED( hr = m_pWaveFile->ResetFile() ) )
						return hr;//DXTRACE_ERR( TEXT("ResetFile"), hr );

					hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
											dwDSLockedBufferSize - dwReadSoFar,
											&dwWavDataRead );
					if( FAILED(hr) )
						return hr;//DXTRACE_ERR( TEXT("Read"), hr );

					dwReadSoFar += dwWavDataRead;
				} 
			}
			else
			{
				// Don't repeat the wav file, just fill in silence 
				FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
							dwDSLockedBufferSize - dwWavDataRead, 
							(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
			}
		}
	}
	else
	{
		// no WAV data?!
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    0);
	}

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also be NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
{
    HRESULT hr;

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return hr;//DXTRACE_ERR( TEXT("GetStatus"), hr );

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then DirectSound 
        // may not be giving us control yet, so restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( hr != DS_OK );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: CSound::GetFreeBuffer()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
    if( m_apDSBuffer == NULL )
        return FALSE; 

	// mike - 020206 - addition for vs8
	DWORD i = 0;

    //for( DWORD i=0; i<m_dwNumBuffers; i++ )
	for( i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
        return m_apDSBuffer[ i ];
    else
        return m_apDSBuffer[ rand() % m_dwNumBuffers ];
}




//-----------------------------------------------------------------------------
// Name: CSound::GetBuffer()
// Desc: 
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetBuffer( DWORD dwIndex )
{
    if( m_apDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_apDSBuffer[dwIndex];
};




//-----------------------------------------------------------------------------
// Name: CSound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSound::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;
    if( dwIndex >= m_dwNumBuffers )
        return E_INVALIDARG;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}


//-----------------------------------------------------------------------------
// Name: CSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play( DWORD dwPriority, DWORD dwFlags )
{
    HRESULT hr = S_OK;
    BOOL    bRestored;

    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

    if( pDSB == NULL )
        return hr;//DXTRACE_ERR( TEXT("GetFreeBuffer"), E_FAIL );

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return hr;//DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return hr;//DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );

        // Make DirectSound do pre-processing on sound effects
        Reset();
    }

    return pDSB->Play( 0, dwPriority, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: CSound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->Stop();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
HRESULT CSound::Reset()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->SetCurrentPosition( 0 );

    return hr;
}


//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::CStreamingSound()
// Desc: Setups up a buffer so data can be streamed from the wave file into 
//       buffer.  This is very useful for large wav files that would take a 
//       while to load.  The buffer is initially filled with data, then 
//       as sound is played the notification events are signaled and more data
//       is written into the buffer by calling HandleWaveStreamNotification()
//-----------------------------------------------------------------------------
CStreamingSound::CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, 
                                  CWaveFile* pWaveFile, DWORD dwNotifySize ) 
                : CSound( &pDSBuffer, dwDSBufferSize, 1, pWaveFile )           
{
    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = dwNotifySize;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::~CStreamingSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CStreamingSound::~CStreamingSound()
{
}



//-----------------------------------------------------------------------------
// Name: CStreamingSound::HandleWaveStreamNotification()
// Desc: Handle the notification that tell us to put more wav data in the 
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::HandleWaveStreamNotification( BOOL bLoopedPlay )
{
    HRESULT hr;
    DWORD   dwCurrentPlayPos;
    DWORD   dwPlayDelta;
    DWORD   dwBytesWrittenToBuffer;
    VOID*   pDSLockedBuffer = NULL;
    VOID*   pDSLockedBuffer2 = NULL;
    DWORD   dwDSLockedBufferSize;
    DWORD   dwDSLockedBufferSize2;

    if( m_apDSBuffer == NULL || m_pWaveFile == NULL )
        return CO_E_NOTINITIALIZED;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
        return hr;//DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
            return hr;//DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );
        return S_OK;
    }

    // Lock the DirectSound buffer
    if( FAILED( hr = m_apDSBuffer[0]->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
                                            &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                            &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
        return hr;//DXTRACE_ERR( TEXT("Lock"), hr );

    // m_dwDSBufferSize and m_dwNextWriteOffset are both multiples of m_dwNotifySize, 
    // it should the second buffer should never be valid
    if( pDSLockedBuffer2 != NULL )
        return E_UNEXPECTED; 

    if( !m_bFillNextNotificationWithSilence )
    {
        // Fill the DirectSound buffer with wav data
        if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer, 
                                                  dwDSLockedBufferSize, 
                                                  &dwBytesWrittenToBuffer ) ) )           
            return hr;//DXTRACE_ERR( TEXT("Read"), hr );
    }
    else
    {
        // Fill the DirectSound buffer with silence
        FillMemory( pDSLockedBuffer, dwDSLockedBufferSize, 
                    (BYTE)( m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        dwBytesWrittenToBuffer = dwDSLockedBufferSize;
    }

    // If the number of bytes written is less than the 
    // amount we requested, we have a short file.
    if( dwBytesWrittenToBuffer < dwDSLockedBufferSize )
    {
        if( !bLoopedPlay ) 
        {
            // Fill in silence for the rest of the buffer.
            FillMemory( (BYTE*) pDSLockedBuffer + dwBytesWrittenToBuffer, 
                        dwDSLockedBufferSize - dwBytesWrittenToBuffer, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );

            // Any future notifications should just fill the buffer with silence
            m_bFillNextNotificationWithSilence = TRUE;
        }
        else
        {
            // We are looping, so reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwBytesWrittenToBuffer;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full (for very short files).
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )
                    return hr;//DXTRACE_ERR( TEXT("ResetFile"), hr );

                if( FAILED( hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                                          dwDSLockedBufferSize - dwReadSoFar,
                                                          &dwBytesWrittenToBuffer ) ) )
                    return hr;//DXTRACE_ERR( TEXT("Read"), hr );

                dwReadSoFar += dwBytesWrittenToBuffer;
            } 
        } 
    }

    // Unlock the DirectSound buffer
    m_apDSBuffer[0]->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    // Figure out how much data has been played so far.  When we have played
    // passed the end of the file, we will either need to start filling the
    // buffer with silence or starting reading from the beginning of the file, 
    // depending if the user wants to loop the sound
    if( FAILED( hr = m_apDSBuffer[0]->GetCurrentPosition( &dwCurrentPlayPos, NULL ) ) )
        return hr;//DXTRACE_ERR( TEXT("GetCurrentPosition"), hr );

    // Check to see if the position counter looped
    if( dwCurrentPlayPos < m_dwLastPlayPos )
        dwPlayDelta = ( m_dwDSBufferSize - m_dwLastPlayPos ) + dwCurrentPlayPos;
    else
        dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

    m_dwPlayProgress += dwPlayDelta;
    m_dwLastPlayPos = dwCurrentPlayPos;

    // If we are now filling the buffer with silence, then we have found the end so 
    // check to see if the entire sound has played, if it has then stop the buffer.
    if( m_bFillNextNotificationWithSilence )
    {
        // We don't want to cut off the sound before it's done playing.
        if( m_dwPlayProgress >= m_pWaveFile->GetSize() )
        {
            m_apDSBuffer[0]->Stop();
        }
    }

    // Update where the buffer will lock (for next time)
    m_dwNextWriteOffset += dwDSLockedBufferSize; 
    m_dwNextWriteOffset %= m_dwDSBufferSize; // Circular buffer

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::Reset()
// Desc: Resets the sound so it will begin playing at the beginning
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Reset()
{
    HRESULT hr;

    if( m_apDSBuffer[0] == NULL || m_pWaveFile == NULL )
        return CO_E_NOTINITIALIZED;

    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
        return hr;//DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
            return hr;//DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );
    }

    m_pWaveFile->ResetFile();

    return m_apDSBuffer[0]->SetCurrentPosition( 0L );  
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::CWaveFile()
// Desc: Constructs the class.  Call Open() to open a wave file for reading.  
//       Then call Read() as needed.  Calling the destructor or Close() 
//       will close the file.  
//-----------------------------------------------------------------------------
CWaveFile::CWaveFile()
{
    m_pwfx    = NULL;
    m_hmmio   = NULL;
    m_pResourceBuffer = NULL;
    m_dwSize  = 0;
    m_pbData  = 0;
    m_bIsReadingFromMemory = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::~CWaveFile()
// Desc: Destructs the class
//-----------------------------------------------------------------------------
CWaveFile::~CWaveFile()
{
    Close();

    if( !m_bIsReadingFromMemory )
        SAFE_DELETE_ARRAY( m_pwfx );
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Open()
// Desc: Opens a wave file for reading
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    HRESULT hr = S_OK;

    m_dwFlags = dwFlags;
    m_bIsReadingFromMemory = FALSE;

    if( m_dwFlags == WAVEFILE_READ )
    {
        if( strFileName == NULL )
            return E_INVALIDARG;
        SAFE_DELETE_ARRAY( m_pwfx );

        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

        if( NULL == m_hmmio )
        {
            HRSRC   hResInfo;
            HGLOBAL hResData;
            DWORD   dwSize;
            VOID*   pvRes;

            // Loading it as a file failed, so try it as a resource
            if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAVE") ) ) )
            {
                if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAV") ) ) )
                    return hr;//DXTRACE_ERR_NOMSGBOX( TEXT("FindResource"), E_FAIL );
            }

            if( NULL == ( hResData = LoadResource( NULL, hResInfo ) ) )
                return hr;//DXTRACE_ERR( TEXT("LoadResource"), E_FAIL );

            if( 0 == ( dwSize = SizeofResource( NULL, hResInfo ) ) ) 
                return hr;//DXTRACE_ERR( TEXT("SizeofResource"), E_FAIL );

            if( NULL == ( pvRes = LockResource( hResData ) ) )
                return hr;//DXTRACE_ERR( TEXT("LockResource"), E_FAIL );

            m_pResourceBuffer = new CHAR[ dwSize ];
            memcpy( m_pResourceBuffer, pvRes, dwSize );

            MMIOINFO mmioInfo;
            ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
            mmioInfo.fccIOProc = FOURCC_MEM;
            mmioInfo.cchBuffer = dwSize;
            mmioInfo.pchBuffer = (CHAR*) m_pResourceBuffer;

            m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
        }

        if( FAILED( hr = ReadMMIO() ) )
        {
            // ReadMMIO will fail if its an not a wave file
            mmioClose( m_hmmio, 0 );
            return hr;//DXTRACE_ERR_NOMSGBOX( TEXT("ReadMMIO"), hr );
        }

        if( FAILED( hr = ResetFile() ) )
            return hr;//DXTRACE_ERR( TEXT("ResetFile"), hr );

        // After the reset, the size of the wav file is m_ck.cksize so store it now
        m_dwSize = m_ck.cksize;
    }
    else
    {
        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF  | 
                                                  MMIO_READWRITE | 
                                                  MMIO_CREATE );
        if( NULL == m_hmmio )
            return hr;//DXTRACE_ERR( TEXT("mmioOpen"), E_FAIL );

        if( FAILED( hr = WriteMMIO( pwfx ) ) )
        {
            mmioClose( m_hmmio, 0 );
            return hr;//DXTRACE_ERR( TEXT("WriteMMIO"), hr );
        }
                        
        if( FAILED( hr = ResetFile() ) )
            return hr;//DXTRACE_ERR( TEXT("ResetFile"), hr );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::OpenFromMemory()
// Desc: copy data to CWaveFile member variable from memory
//-----------------------------------------------------------------------------
HRESULT CWaveFile::OpenFromMemory( BYTE* pbData, ULONG ulDataSize, 
                                   WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    m_pwfx       = pwfx;
    m_ulDataSize = ulDataSize;
    m_pbData     = pbData;
    m_pbDataCur  = m_pbData;
    m_bIsReadingFromMemory = TRUE;
    
    if( dwFlags != WAVEFILE_READ )
        return E_NOTIMPL;       
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx. 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return E_FAIL;//DXTRACE_ERR_NOMSGBOX( TEXT("mmioFOURCC"), E_FAIL ); 

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
           return E_FAIL;//DXTRACE_ERR( TEXT("sizeof(PCMWAVEFORMAT)"), E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
            return E_FAIL;//DXTRACE_ERR( TEXT("m_pwfx"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return E_FAIL;//DXTRACE_ERR( TEXT("new"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            SAFE_DELETE( m_pwfx );
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SAFE_DELETE( m_pwfx );
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::GetSize()
// Desc: Retuns the size of the read access wave file 
//-----------------------------------------------------------------------------
DWORD CWaveFile::GetSize()
{
    return m_dwSize;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the 
//       beginning of the file again 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ResetFile()
{
    if( m_bIsReadingFromMemory )
    {
        m_pbDataCur = m_pbData;
    }
    else 
    {
        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( m_dwFlags == WAVEFILE_READ )
        {
            // Seek to the data
            if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                            SEEK_SET ) )
                return E_FAIL;//DXTRACE_ERR( TEXT("mmioSeek"), E_FAIL );

            // Search the input file for the 'data' chunk.
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
              return E_FAIL;//DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
        }
        else
        {
            // Create the 'data' chunk that holds the waveform samples.  
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            m_ck.cksize = 0;

            if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) ) 
                return E_FAIL;//DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );

            if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
                return E_FAIL;//DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
        }
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns 
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So 
//       subsequent calls will be continue where the last left off unless 
//       Reset() is called.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    if( m_bIsReadingFromMemory )
    {
        if( m_pbDataCur == NULL )
            return CO_E_NOTINITIALIZED;
        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( (BYTE*)(m_pbDataCur + dwSizeToRead) > 
            (BYTE*)(m_pbData + m_ulDataSize) )
        {
            dwSizeToRead = m_ulDataSize - (DWORD)(m_pbDataCur - m_pbData);
        }
        
        CopyMemory( pBuffer, m_pbDataCur, dwSizeToRead );
        
        if( pdwSizeRead != NULL )
            *pdwSizeRead = dwSizeToRead;

        return S_OK;
    }
    else 
    {
        MMIOINFO mmioinfoIn; // current status of m_hmmio

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;
        if( pBuffer == NULL || pdwSizeRead == NULL )
            return E_INVALIDARG;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
                
        UINT cbDataIn = dwSizeToRead;
        if( cbDataIn > m_ck.cksize ) 
            cbDataIn = m_ck.cksize;       

        m_ck.cksize -= cbDataIn;
    
        for( DWORD cT = 0; cT < cbDataIn; cT++ )
        {
            // Copy the bytes from the io to the buffer.
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
                if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                    return E_FAIL;//DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );

                if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                    return E_FAIL;//DXTRACE_ERR( TEXT("mmioinfoIn.pchNext"), E_FAIL );
            }

            // Actual copy.
            *((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
        }

        if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );

        if( pdwSizeRead != NULL )
            *pdwSizeRead = cbDataIn;

        return S_OK;
    }
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Close()
// Desc: Closes the wave file 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Close()
{
    if( m_dwFlags == WAVEFILE_READ )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
        SAFE_DELETE_ARRAY( m_pResourceBuffer );
    }
    else
    {
        m_mmioinfoOut.dwFlags |= MMIO_DIRTY;

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( 0 != mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );
    
        // Ascend the output file out of the 'data' chunk -- this will cause
        // the chunk size of the 'data' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
        // Do this here instead...
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
        
        mmioSeek( m_hmmio, 0, SEEK_SET );

        if( 0 != (INT)mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
    
        m_ck.ckid = mmioFOURCC('f', 'a', 'c', 't');

        if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) ) 
        {
            DWORD dwSamples = 0;
            mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(DWORD) );
            mmioAscend( m_hmmio, &m_ck, 0 ); 
        }
    
        // Ascend the output file out of the 'RIFF' chunk -- this will cause
        // the chunk size of the 'RIFF' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::WriteMMIO()
// Desc: Support function for reading from a multimedia I/O stream
//       pwfxDest is the WAVEFORMATEX for this new wave file.  
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_ck.  
//-----------------------------------------------------------------------------
HRESULT CWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
{
    DWORD    dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
    MMCKINFO ckOut1;
    
    dwFactChunk = (DWORD)-1;

    // Create the output file RIFF chunk of form type 'WAVE'.
    m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');       
    m_ckRiff.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    // We are now descended into the 'RIFF' chunk we just created.
    // Now create the 'fmt ' chunk. Since we know the size of this chunk,
    // specify it in the MMCKINFO structure so MMIO doesn't have to seek
    // back and set the chunk size after ascending from the chunk.
    m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
    m_ck.cksize = sizeof(PCMWAVEFORMAT);   

    if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    // Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type. 
    if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
    {
        if( mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                       sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    }   
    else 
    {
        // Write the variable length size.
        if( (UINT)mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                             sizeof(*pwfxDest) + pwfxDest->cbSize ) != 
                             ( sizeof(*pwfxDest) + pwfxDest->cbSize ) )
            return E_FAIL;//DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    }  
    
    // Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
    if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
    // Now create the fact chunk, not required for PCM but nice to have.  This is filled
    // in when the close routine is called.
    ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
    ckOut1.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &ckOut1, 0 ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
    if( mmioWrite( m_hmmio, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != 
                    sizeof(dwFactChunk) )
         return E_FAIL;//DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    
    // Now ascend out of the fact chunk...
    if( 0 != mmioAscend( m_hmmio, &ckOut1, 0 ) )
        return E_FAIL;//DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
       
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Write()
// Desc: Writes data to the open wave file
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Write( UINT nSizeToWrite, BYTE* pbSrcData, UINT* pnSizeWrote )
{
    UINT cT;

    if( m_bIsReadingFromMemory )
        return E_NOTIMPL;
    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pnSizeWrote == NULL || pbSrcData == NULL )
        return E_INVALIDARG;

    *pnSizeWrote = 0;
    
    for( cT = 0; cT < nSizeToWrite; cT++ )
    {       
        if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
        {
            m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
            if( 0 != mmioAdvance( m_hmmio, &m_mmioinfoOut, MMIO_WRITE ) )
                return E_FAIL;//DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );
        }

        *((BYTE*)m_mmioinfoOut.pchNext) = *((BYTE*)pbSrcData+cT);
        (BYTE*)m_mmioinfoOut.pchNext++;

        (*pnSizeWrote)++;
    }

    return S_OK;
}




