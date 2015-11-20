//-----------------------------------------------------------------------------
// File: NetVoice.h
//
// Desc: 
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef NETVOICE_H
#define NETVOICE_H


#include <windows.h>
#include <dvoice.h>


class CNetVoice
{
public:
    CNetVoice( LPDVMESSAGEHANDLER pfnDirectPlayClientVoiceMessageHandler,
               LPDVMESSAGEHANDLER pfnDirectPlayServerVoiceMessageHandler );
    virtual ~CNetVoice();

    HRESULT Init( HWND hDlg, BOOL bCreateSession, BOOL bConnectToSession, 
                  LPUNKNOWN pDirectPlay, DWORD dwSessionType, GUID* pGuidCT, 
                  DVCLIENTCONFIG* pdvClientConfig, LPDIRECTSOUND lpds = NULL );
    HRESULT Free();

    HRESULT HostMigrate( LPDIRECTPLAYVOICESERVER pdvServerInterface );
    BOOL IsHalfDuplex() { return m_bHalfDuplex; }
    HRESULT ChangeVoiceClientSettings( DVCLIENTCONFIG* pdvClientConfig );

    LPDIRECTPLAYVOICECLIENT GetVoiceClient() { return m_pVoiceClient; }
    LPDIRECTPLAYVOICESERVER GetVoiceServer() { return m_pVoiceServer; }

protected:
    LPDIRECTPLAYVOICECLIENT m_pVoiceClient;
    LPDIRECTPLAYVOICESERVER m_pVoiceServer;                                                                             
    LPDVMESSAGEHANDLER m_pfnDirectPlayClientVoiceMessageHandler;
    LPDVMESSAGEHANDLER m_pfnDirectPlayServerVoiceMessageHandler;

    HRESULT VoiceSessionCreate( LPUNKNOWN pDirectPlay, DWORD dwSessionType, GUID* pGuidCT );
    HRESULT VoiceSessionTestAudioSetup( HWND hDlg );
    HRESULT VoiceSessionConnect( HWND hDlg, LPUNKNOWN pDirectPlay, 
                                 DVCLIENTCONFIG* pdvClientConfig, LPDIRECTSOUND lpds = NULL );
    HRESULT VoiceSessionDisconnect();
    HRESULT VoiceSessionDestroy();

    BOOL m_bHalfDuplex;
};


#endif // NETVOICE_H

