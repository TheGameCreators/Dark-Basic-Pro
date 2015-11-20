/*
            File : DBPro/Music.hpp
       Generated : 2011/09/02 20:14:48
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Music_hpp
#define INCLUDE_GUARD_DBPro_Music_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl DeleteMusic(int MusicNumber);
    int __cdecl GetNumberOfCdTracks();
    void __cdecl LoadCdmusic(int TrackNumber,int MusicNumber);
    void __cdecl LoadMusic(LPCSTR Filename,int MusicNumber);
    void __cdecl LoopMusic(int MusicNumber);
    int __cdecl MusicExist(int MusicNumber);
    int __cdecl MusicLooping(int MusicNumber);
    int __cdecl MusicPaused(int MusicNumber);
    int __cdecl MusicPlaying(int MusicNumber);
    int __cdecl MusicSpeed(int MusicNumber);
    int __cdecl MusicVolume(int MusicNumber);
    void __cdecl PauseMusic(int MusicNumber);
    void __cdecl PlayMusic(int MusicNumber);
    void __cdecl ResumeMusic(int MusicNumber);
    void __cdecl SetMusicSpeed(int MusicNumber,int Speed);
    void __cdecl SetMusicVolume(int MusicNumber,int Volume);
    void __cdecl StopMusic(int MusicNumber);
}

#endif
