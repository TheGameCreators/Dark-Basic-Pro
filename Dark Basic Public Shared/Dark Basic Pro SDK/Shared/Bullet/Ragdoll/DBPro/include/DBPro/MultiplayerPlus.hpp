/*
            File : DBPro/MultiplayerPlus.hpp
       Generated : 2011/09/02 20:15:04
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_MultiplayerPlus_hpp
#define INCLUDE_GUARD_DBPro_MultiplayerPlus_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl MultiplayerConnectToTcpServer(LPCSTR a,LPCSTR b);
    void __cdecl MultiplayerConnectToTcpServer(LPCSTR a,LPCSTR b,int c);
    void __cdecl MultiplayerCreateTcpServer(LPCSTR a);
    void __cdecl MultiplayerCreateTcpServer(LPCSTR a,int b);
    void __cdecl MultiplayerDisconnect();
    void __cdecl MultiplayerDisplayDebug(LPCSTR a);
    int __cdecl MultiplayerGetConnection();
    int __cdecl MultiplayerGetId();
    LPSTR __cdecl MultiplayerGetIpAddress();
    float __cdecl MultiplayerGetMessageFloat();
    int __cdecl MultiplayerGetMessageInteger();
    void __cdecl MultiplayerGetMessageMemblock(int a);
    int __cdecl MultiplayerGetMessagePlayerFrom();
    LPSTR __cdecl MultiplayerGetMessageString();
    int __cdecl MultiplayerGetMessageType();
    void __cdecl MultiplayerGetMessage();
    int __cdecl MultiplayerGetPlayerCount();
    int __cdecl MultiplayerGetPlayerId(int a);
    LPSTR __cdecl MultiplayerGetPlayerName(int a);
    int __cdecl MultiplayerGetQueueSize(int a);
    int __cdecl MultiplayerMessageExists();
    void __cdecl MultiplayerSendFromPlayer(int a);
    void __cdecl MultiplayerSendMessageFloat(float a);
    void __cdecl MultiplayerSendMessageFloat(float a,int b);
    void __cdecl MultiplayerSendMessageInteger(int a);
    void __cdecl MultiplayerSendMessageInteger(int a,int b);
    void __cdecl MultiplayerSendMessageMemblock(int a);
    void __cdecl MultiplayerSendMessageMemblock(int a,int b);
    void __cdecl MultiplayerSendMessageMemblock(int a,int b,int c);
    void __cdecl MultiplayerSendMessageString(LPCSTR a);
    void __cdecl MultiplayerSendMessageString(LPCSTR a,int b);
    void __cdecl MultiplayerSendMessageTo(int a);
    void __cdecl MultiplayerSetDebugState(int a);
    void __cdecl MultiplayerStartVoiceServer();
    void __cdecl MultiplayerStart();
}

#endif
