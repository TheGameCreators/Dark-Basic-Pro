/*
            File : DBPro/Multiplayer.hpp
       Generated : 2011/09/02 20:14:47
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Multiplayer_hpp
#define INCLUDE_GUARD_DBPro_Multiplayer_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl CreateNetGame(LPCSTR Gamename,LPCSTR Playername,int NumberOfPlayers);
    void __cdecl CreateNetGame(LPCSTR Gamename,LPCSTR Playername,int NumberOfPlayers,int Flag);
    int __cdecl CreateNetPlayer(LPCSTR Playername);
    int __cdecl DefaultNetGame(LPCSTR Gamename,LPCSTR Playername,int NumberOfPlayers,int GameType);
    void __cdecl FreeNetGame();
    void __cdecl FreeNetPlayer(int PlayerNumber);
    void __cdecl GetNetMessage();
    void __cdecl JoinNetGame(int SessionNumber,LPCSTR Playername);
    int __cdecl NetBufferSize();
    int __cdecl NetGameExists();
    int __cdecl NetGameLost();
    int __cdecl NetGameNowHosting();
    void __cdecl NetMessageBitmap(int BitmapNumber);
    int __cdecl NetMessageExists();
    float __cdecl NetMessageFloat();
    void __cdecl NetMessageImage(int ImageNumber);
    int __cdecl NetMessageInteger();
    void __cdecl NetMessageMemblock(int MemblockNumber);
    void __cdecl NetMessageMesh(int MeshNumber);
    int __cdecl NetMessagePlayerFrom();
    int __cdecl NetMessagePlayerTo();
    void __cdecl NetMessageSound(int SoundNumber);
    LPSTR __cdecl NetMessageString();
    int __cdecl NetMessageType();
    int __cdecl NetPlayerCreated();
    int __cdecl NetPlayerDestroyed();
    void __cdecl PerformChecklistForNetConnections();
    void __cdecl PerformChecklistForNetPlayers();
    void __cdecl PerformChecklistForNetSessions();
    void __cdecl SendNetMessageBitmap(int PlayerNumber,int BitmapNumber,int GuarenteePacket);
    void __cdecl SendNetMessageFloat(int PlayerNumber,float FloatValue);
    void __cdecl SendNetMessageImage(int PlayerNumber,int ImageNumber,int GuarenteePacket);
    void __cdecl SendNetMessageInteger(int PlayerNumber,int IntegerValue);
    void __cdecl SendNetMessageMemblock(int PlayerNumber,int MemblockNumber);
    void __cdecl SendNetMessageMemblock(int PlayerNumber,int MemblockNumber,int GuarenteePacket);
    void __cdecl SendNetMessageMesh(int PlayerNumber,int MeshNumber,int GuarenteePacket);
    void __cdecl SendNetMessageSound(int PlayerNumber,int SoundNumber,int GuarenteePacket);
    void __cdecl SendNetMessageString(int PlayerNumber,LPCSTR String);
    void __cdecl SetNetConnection(int ConnectionNumber);
    void __cdecl SetNetConnection(int ConnectionNumber,LPCSTR AddressData);
}

#endif
