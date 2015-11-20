//-----------------------------------------------------------------------------
// File: NetVoice.cpp
//
// Desc: DirectPlay Voice framework class. Feel free to use 
//       this class as a starting point for adding extra functionality.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef UNDER_CE

#ifndef STRICT
#define STRICT
#endif // !STRICT
#include <windows.h>
#include <dxerr9.h>
#include <dvoice.h>
#include "NetVoice.h"
//#include "DXUtil.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


//-----------------------------------------------------------------------------
// Name: CNetVoice
// Desc: 
//-----------------------------------------------------------------------------
CNetVoice::CNetVoice( LPDVMESSAGEHANDLER pfnDirectPlayClientVoiceMessageHandler,
                      LPDVMESSAGEHANDLER pfnDirectPlayServerVoiceMessageHandler )
{
    m_bHalfDuplex  = FALSE;
    m_pVoiceClient = NULL;
    m_pVoiceServer = NULL;
    m_pfnDirectPlayClientVoiceMessageHandler = pfnDirectPlayClientVoiceMessageHandler;
    m_pfnDirectPlayServerVoiceMessageHandler = pfnDirectPlayServerVoiceMessageHandler;
}




//-----------------------------------------------------------------------------
// Name: ~CNetVoice
// Desc: 
//-----------------------------------------------------------------------------
CNetVoice::~CNetVoice()
{
    Free();
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initializes DirectPlay Voice.  
// Params:  hDlg: the HWND of the parent window for use by the voice setup wizard
//          bCreateSession:     if TRUE then it creates the DirectPlay Voice sesson.
//          bConnectToSession:  if TRUE then it connects to the DirectPlay Voice
//                              session.  
//          pDirectPlay:        inteface to the IDirectPlay8Client or 
//                              IDirectPlay8Peer interface 
//          pGuidCT:            guid of the voice compression codec
//          pdvClientConfig:    client config. Can be NULL if bConnectToSession is FALSE.
//          lpds:               pointer to an existing DirectSound object, or NULL 
//                              if none exists yet.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::Init( HWND hDlg, BOOL bCreateSession, BOOL bConnectToSession,
                         LPUNKNOWN pDirectPlay, DWORD dwSessionType, 
                         GUID* pGuidCT, DVCLIENTCONFIG* pdvClientConfig, LPDIRECTSOUND lpds )
{
    HRESULT hr;

    // Validate required parameters
    if( NULL == pDirectPlay )
        return E_INVALIDARG;

    // Typically the host player creates the voice session 
    if( bCreateSession )
    {
        if( FAILED( hr = VoiceSessionCreate( pDirectPlay, dwSessionType, pGuidCT ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionCreate"), hr );
    }

    if( bConnectToSession )
    {
        // Test the voice setup to make sure the voice setup wizard has been run
        if( FAILED( hr = VoiceSessionTestAudioSetup( hDlg ) ) )
        {
            if( hr == DVERR_USERCANCEL || hr == DVERR_ALREADYPENDING )
                return hr;
            
            return DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionTestAudioSetup"), hr );
        }

        // Typically all of the clients connect to the voice session
        if( FAILED( hr = VoiceSessionConnect( hDlg, pDirectPlay, pdvClientConfig, lpds ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionConnect"), hr );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Free()
// Desc: Frees DirectPlayVoice
//-----------------------------------------------------------------------------
HRESULT CNetVoice::Free()
{
    HRESULT hr;

    if( m_pVoiceClient )
    {
        // Have all the clients disconnect from the session
        if( FAILED( hr = VoiceSessionDisconnect() ) )
            return DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionDisconnect"), hr );
    }

    if( m_pVoiceServer )
    {
        // Have all the host player destroy the session 
        if( FAILED( hr = VoiceSessionDestroy() ) )
            return DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionDestroy"), hr );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HostMigrate()
// Desc: Starts the DirectPlayVoice session
//       The host player should call this to create the voice session.  It
//       stores the server interface, and addref's it.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::HostMigrate( LPDIRECTPLAYVOICESERVER pdvServerInterface )
{
    if( pdvServerInterface == NULL )
        return E_INVALIDARG;

    SAFE_RELEASE( m_pVoiceServer );

    m_pVoiceServer = pdvServerInterface;

    // Addref the server since we are storing the pointer.
    m_pVoiceServer->AddRef();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: VoiceSessionTestAudioSetup()
// Desc: Uses IDirectPlayVoiceSetup to test the voice setup.
//       All clients should call this once to test the voice audio setup.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::VoiceSessionTestAudioSetup( HWND hDlg )
{
    HRESULT hr;
    LPDIRECTPLAYVOICETEST pVoiceSetup = NULL;

    // Create a DirectPlayVoice setup interface.
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlayVoiceTest, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlayVoiceTest, 
                                       (LPVOID*) &pVoiceSetup) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );

    // Check to see if the audio tests have been run yet
    GUID guidPlayback = DSDEVID_DefaultVoicePlayback;
    GUID guidCapture  = DSDEVID_DefaultVoiceCapture;
    hr = pVoiceSetup->CheckAudioSetup( &guidPlayback, 
                                       &guidCapture, 
                                       hDlg, DVFLAGS_QUERYONLY );
    if( hr == DVERR_RUNSETUP )
    {
        // Perform the audio tests, since they need to be done before 
        // any of the DPVoice calls will work.
        hr = pVoiceSetup->CheckAudioSetup( &guidPlayback, &guidCapture, hDlg, DVFLAGS_ALLOWBACK );
        if( FAILED(hr) )
        {
            SAFE_RELEASE( pVoiceSetup );
            
            if( hr == DVERR_USERCANCEL || hr == DVERR_ALREADYPENDING )
                return hr;

            return DXTRACE_ERR_MSGBOX( TEXT("CheckAudioSetup"), hr );
        }
    }

    // Done with setup
    SAFE_RELEASE( pVoiceSetup );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: VoiceSessionCreate()
// Desc: Starts the DirectPlayVoice session
//       The host player should call this to create the voice session.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::VoiceSessionCreate( LPUNKNOWN pDirectPlay, DWORD dwSessionType, 
                                       GUID* pGuidCT )
{
    HRESULT hr;

    // Create a DirectPlayVoice server interface.
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlayVoiceServer, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlayVoiceServer, 
                                       (LPVOID*) &m_pVoiceServer ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );

    // Init the DirectPlayVoice server
    if( FAILED( hr = m_pVoiceServer->Initialize( pDirectPlay, m_pfnDirectPlayServerVoiceMessageHandler, 
                                                 NULL, 0, 0 ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );

    // Setup and start a DirectPlay session based on globals set by user choices 
    // in the config dlg box.
    DVSESSIONDESC dvSessionDesc;
    ZeroMemory( &dvSessionDesc, sizeof(DVSESSIONDESC) );
    dvSessionDesc.dwSize                 = sizeof( DVSESSIONDESC );
    dvSessionDesc.dwBufferAggressiveness = DVBUFFERAGGRESSIVENESS_DEFAULT;
    dvSessionDesc.dwBufferQuality        = DVBUFFERQUALITY_DEFAULT;
    dvSessionDesc.dwFlags                = 0;
    dvSessionDesc.dwSessionType          = dwSessionType;
    dvSessionDesc.guidCT                 = *pGuidCT;

    if( FAILED( hr = m_pVoiceServer->StartSession( &dvSessionDesc, 0 ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("StartSession"), hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: VoiceSessionConnect()
// Desc: Connects to the DirectPlayVoice session.  
///      All clients should call this once to join the voice session.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::VoiceSessionConnect( HWND hDlg, LPUNKNOWN pDirectPlay, 
                                        DVCLIENTCONFIG* pdvClientConfig, LPDIRECTSOUND lpds )
{
    HRESULT hr;
    DVSOUNDDEVICECONFIG  dvSoundDeviceConfig  = {0};
    DVSOUNDDEVICECONFIG* pdvSoundDeviceConfig = NULL;
    

    // Create a DirectPlayVoice client interface.
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlayVoiceClient, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlayVoiceClient, 
                                       (LPVOID*) &m_pVoiceClient ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );
        goto LCleanReturn;
    }

    // Init the DirectPlayVoice client, passing in VoiceMessageHandler() as the
    // callback handler for any messages sent to us.
    if( FAILED( hr = m_pVoiceClient->Initialize( pDirectPlay, 
                                                 m_pfnDirectPlayClientVoiceMessageHandler, 
                                                 (LPVOID*) hDlg, // context value
                                                 0, 0 ) ) )
    {
         DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );
         goto LCleanReturn;
    }

    // Setup the DirectPlayVoice sound devices.  This just uses the defaults.
    dvSoundDeviceConfig.dwSize                    = sizeof( DVSOUNDDEVICECONFIG );
    dvSoundDeviceConfig.dwFlags                   = 0;
    dvSoundDeviceConfig.guidPlaybackDevice        = DSDEVID_DefaultVoicePlayback; 
    dvSoundDeviceConfig.lpdsPlaybackDevice        = lpds;
    dvSoundDeviceConfig.guidCaptureDevice         = DSDEVID_DefaultVoiceCapture; 
    dvSoundDeviceConfig.lpdsCaptureDevice         = NULL;
    dvSoundDeviceConfig.hwndAppWindow             = hDlg;
    dvSoundDeviceConfig.lpdsMainBuffer            = NULL;
    dvSoundDeviceConfig.dwMainBufferFlags         = 0;
    dvSoundDeviceConfig.dwMainBufferPriority      = 0;

    // Connect to the voice session
    if( FAILED( hr = m_pVoiceClient->Connect( &dvSoundDeviceConfig, 
                                              pdvClientConfig, 
                                              DVFLAGS_SYNC ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Connect"), hr );
        goto LCleanReturn;
    }
        
    // Talk to everyone in the session
    DVID dvid;
    dvid = DVID_ALLPLAYERS;
    if( FAILED( hr = m_pVoiceClient->SetTransmitTargets( &dvid, 1, 0 ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("SetTransmitTargets"), hr );
        goto LCleanReturn;
    }

    // Get the sound device config and extract if its half duplex
    DWORD dwSize;
    dwSize = 0;
    hr = m_pVoiceClient->GetSoundDeviceConfig( pdvSoundDeviceConfig, &dwSize );
    if( FAILED(hr) && hr != DVERR_BUFFERTOOSMALL )
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSoundDeviceConfig"), hr );
        goto LCleanReturn;
    }

    pdvSoundDeviceConfig = (DVSOUNDDEVICECONFIG*) new BYTE[ dwSize ];
    if( NULL == pdvSoundDeviceConfig )
    {
        hr = E_OUTOFMEMORY;
        DXTRACE_ERR_MSGBOX( TEXT("VoiceSessionConnect"), hr );
        goto LCleanReturn;
    }

    ZeroMemory( pdvSoundDeviceConfig, dwSize );
    pdvSoundDeviceConfig->dwSize = sizeof(DVSOUNDDEVICECONFIG);
    hr = m_pVoiceClient->GetSoundDeviceConfig( pdvSoundDeviceConfig, &dwSize );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSoundDeviceConfig"), hr );
        goto LCleanReturn;
    }

    m_bHalfDuplex = ( (pdvSoundDeviceConfig->dwFlags & DVSOUNDCONFIG_HALFDUPLEX) != 0 );
    
    hr = S_OK;

LCleanReturn:
    SAFE_DELETE_ARRAY( pdvSoundDeviceConfig );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: ChangeVoiceClientSettings()
// Desc: Changes the client config to globals set by user choices 
//       in the config dlg box.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::ChangeVoiceClientSettings( DVCLIENTCONFIG* pdvClientConfig )
{
    HRESULT hr;

    if( m_pVoiceClient == NULL )
        return CO_E_NOTINITIALIZED;

    // Change the client config
    if( FAILED( hr = m_pVoiceClient->SetClientConfig( pdvClientConfig) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("SetClientConfig"), hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: VoiceSessionDisconnect()
// Desc: Disconnects from the DirectPlayVoice session
//       All clients should call this once to leave the voice session.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::VoiceSessionDisconnect()
{
    if( m_pVoiceClient )
    {
        m_pVoiceClient->Disconnect( DVFLAGS_SYNC );
        SAFE_RELEASE( m_pVoiceClient );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: VoiceSessionDestroy()
// Desc: Stops the DirectPlayVoice session
//       The host player should call this once to destroy the voice session.
//-----------------------------------------------------------------------------
HRESULT CNetVoice::VoiceSessionDestroy()
{
    if( m_pVoiceServer )
    {
        m_pVoiceServer->StopSession( 0 );
        SAFE_RELEASE( m_pVoiceServer );
    }

    return S_OK;
}

#endif // !UNDER_CE