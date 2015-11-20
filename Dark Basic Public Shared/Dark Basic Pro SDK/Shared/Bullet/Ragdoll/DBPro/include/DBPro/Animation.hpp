/*
            File : DBPro/Animation.hpp
       Generated : 2011/09/02 20:14:23
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Animation_hpp
#define INCLUDE_GUARD_DBPro_Animation_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    int __cdecl AnimationExist(int AnimationNumber);
    int __cdecl AnimationHeight(int AnimationNumber);
    int __cdecl AnimationLooping(int AnimationNumber);
    int __cdecl AnimationPaused(int AnimationNumber);
    int __cdecl AnimationPlaying(int AnimationNumber);
    int __cdecl AnimationPositionX(int AnimationNumber);
    int __cdecl AnimationPositionY(int AnimationNumber);
    int __cdecl AnimationSpeed(int AnimationNumber);
    int __cdecl AnimationVolume(int AnimationNumber);
    int __cdecl AnimationWidth(int AnimationNumber);
    void __cdecl DeleteAnimation(int AnimationNumber);
    void __cdecl LoadAnimation(LPCSTR Filename,int AnimationNumber);
    void __cdecl LoadDvdAnimation(int AnimationNumber);
    void __cdecl LoopAnimation(int AnimationNumber);
    void __cdecl LoopAnimation(int AnimationNumber,int BitmapNumber,int X,int Y,int X_5,int Y_6);
    void __cdecl PauseAnimation(int AnimationNumber);
    void __cdecl PlaceAnimation(int AnimationNumber,int X,int Y,int X_4,int Y_5);
    void __cdecl PlayAnimationToImage(int AnimationNumber,int Image,int Left,int Top,int Right,int Bottom);
    void __cdecl PlayAnimation(int AnimationNumber);
    void __cdecl PlayAnimation(int AnimationNumber,int X,int Y);
    void __cdecl PlayAnimation(int AnimationNumber,int X,int Y,int X_4,int Y_5);
    void __cdecl PlayAnimation(int AnimationNumber,int BitmapNumber,int X,int Y,int X_5,int Y_6);
    void __cdecl ResumeAnimation(int AnimationNumber);
    void __cdecl SetAnimationSpeed(int AnimationNumber,int Speed);
    void __cdecl SetAnimationVolume(int AnimationNumber,int Volume);
    void __cdecl SetDvdChapter(int AnimationNumber,int TitleNumber,int ChapterNumber);
    void __cdecl StopAnimation(int AnimationNumber);
    int __cdecl TotalDvdChapters(int AnimationNumber,int TitleNumber);
}

#endif
