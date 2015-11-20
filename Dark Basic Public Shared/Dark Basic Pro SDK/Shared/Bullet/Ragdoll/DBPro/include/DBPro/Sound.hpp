/*
            File : DBPro/Sound.hpp
       Generated : 2011/09/02 20:14:53
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Sound_hpp
#define INCLUDE_GUARD_DBPro_Sound_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


#undef PlaySound


namespace DBPro
{
    void __cdecl CloneSound(int DestinationSound,int SourceSound);
    void __cdecl DeleteSound(int SoundNumber);
    float __cdecl ListenerAngleX();
    float __cdecl ListenerAngleY();
    float __cdecl ListenerAngleZ();
    float __cdecl ListenerPositionX();
    float __cdecl ListenerPositionY();
    float __cdecl ListenerPositionZ();
    void __cdecl Load3dsound(LPCSTR Filename,int SoundNumber);
    void __cdecl Load3dsound(LPCSTR Filename,int SoundNumber,int SilentFail);
    void __cdecl Load3dsound(LPCSTR Filename,int SoundNumber,int SilentFail,int GlobalSoundFlag);
    void __cdecl LoadSound(LPCSTR Filename,int SoundNumber);
    void __cdecl LoadSound(LPCSTR Filename,int SoundNumber,int Flag);
    void __cdecl LoadSound(LPCSTR Filename,int SoundNumber,int Flag,int SilentFail);
    void __cdecl LoadSound(LPCSTR Filename,int SoundNumber,int Flag,int SilentFail,int GlobalSoundFlag);
    void __cdecl LoopSound(int SoundNumber);
    void __cdecl LoopSound(int SoundNumber,int StartPosition);
    void __cdecl LoopSound(int SoundNumber,int StartPosition,int EndPosition);
    void __cdecl LoopSound(int SoundNumber,int StartPosition,int EndPosition,int InitialPosition);
    void __cdecl PauseSound(int SoundNumber);
    void __cdecl PlaySound(int SoundNumber);
    void __cdecl PlaySound(int SoundNumber,int StartPosition);
    void __cdecl PositionListener(float X,float Y,float Z);
    void __cdecl PositionSound(int SoundNumber,float X,float Y,float Z);
    void __cdecl RecordSound(int SoundNumber);
    void __cdecl RecordSound(int SoundNumber,int Duration);
    void __cdecl ResumeSound(int SoundNumber);
    void __cdecl RotateListener(float X,float Y,float Z);
    void __cdecl SaveSound(LPCSTR Filename,int SoundNumber);
    void __cdecl ScaleListener(float ScalingFactor);
    void __cdecl SetEax(int PresetEffectValue);
    void __cdecl SetSoundPan(int SoundNumber,int PanValue);
    void __cdecl SetSoundSpeed(int SoundNumber,int FrequencyValue);
    void __cdecl SetSoundVolume(int SoundNumber,int VolumeNumber);
    int __cdecl SoundExist(int SoundNumber);
    int __cdecl SoundLooping(int SoundNumber);
    int __cdecl SoundPan(int SoundNumber);
    int __cdecl SoundPaused(int SoundNumber);
    int __cdecl SoundPlaying(int SoundNumber);
    float __cdecl SoundPositionX(int SoundNumber);
    float __cdecl SoundPositionY(int SoundNumber);
    float __cdecl SoundPositionZ(int SoundNumber);
    int __cdecl SoundSpeed(int SoundNumber);
    int __cdecl SoundType(int SoundNumber);
    int __cdecl SoundVolume(int SoundNumber);
    void __cdecl StopRecordingSound();
    void __cdecl StopSound(int SoundNumber);
}

#endif
