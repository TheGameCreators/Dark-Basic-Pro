/*
            File : DBPro/Setup.hpp
       Generated : 2011/09/02 20:14:51
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Setup_hpp
#define INCLUDE_GUARD_DBPro_Setup_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    int __cdecl CheckDisplayMode(int Width,int Height,int Depth);
    int __cdecl CheckDisplayMode(int Width,int Height,int Depth,int Vsyncon,int MultisamplingFactor,int MultimonitorMode);
    LPSTR __cdecl CurrentGraphicsCard();
    int __cdecl DesktopHeight();
    int __cdecl DesktopWidth();
    int __cdecl EmulationMode();
    int __cdecl GetBackbufferDepth();
    int __cdecl GetBackbufferHeight();
    int __cdecl GetBackbufferPitch();
    LPVOID __cdecl GetBackbufferPtr();
    int __cdecl GetBackbufferWidth();
    float __cdecl GetTrackingPitch(int FilteringMode);
    float __cdecl GetTrackingRoll(int FilteringMode);
    float __cdecl GetTrackingYaw(int FilteringMode);
    void __cdecl HideWindow();
    void __cdecl LockBackbuffer();
    void __cdecl MaximizeWindow();
    void __cdecl MinimizeWindow();
    void __cdecl PerformChecklistForDisplayModes();
    void __cdecl PerformChecklistForGraphicsCards();
    int __cdecl ResetLeftEye();
    void __cdecl ResetTracking();
    void __cdecl RestoreWindow();
    int __cdecl ScreenDepth();
    int __cdecl ScreenFps();
    int __cdecl ScreenHeight();
    int __cdecl ScreenInvalid();
    int __cdecl ScreenType();
    int __cdecl ScreenWidth();
    void __cdecl SetDisplayMode(int Width,int Height,int Depth);
    void __cdecl SetDisplayMode(int Width,int Height,int Depth,int Vsyncon);
    void __cdecl SetDisplayMode(int Width,int Height,int Depth,int Vsyncon,int MultisamplingFactor,int MultimonitorMode);
    void __cdecl SetDisplayMode(int Width,int Height,int Depth,int Vsyncon,int MultisamplingFactor,int MultimonitorMode,int BackbufferWidth,int BackbufferHeight);
    void __cdecl SetDisplayMode(int Width,int Height,int Depth,int Vsyncon,int MultisamplingFactor,int MultimonitorMode,int BackbufferWidth,int BackbufferHeight,int VirtualRealityMode);
    void __cdecl SetEmulationOff();
    void __cdecl SetEmulationOn();
    void __cdecl SetGamma(int Red,int Green,int Blue);
    void __cdecl SetGraphicsCard(LPCSTR Cardname);
    void __cdecl SetNvperfhud(int UseNvperfhudFlag);
    void __cdecl SetScreenEffect(int EffectNumber);
    void __cdecl SetWindowLayout(int Style,int Caption,int IconNumber);
    void __cdecl SetWindowOff();
    void __cdecl SetWindowOn();
    void __cdecl SetWindowPosition(int X,int Y);
    void __cdecl SetWindowSize(int Width,int Height);
    void __cdecl SetWindowTitle(LPCSTR CaptionString);
    void __cdecl ShowWindow();
    void __cdecl TextureScreen(int ImageNumber);
    void __cdecl TextureScreen(int StageIndex,int ImageNumber);
    void __cdecl UnlockBackbuffer();
    int __cdecl WindowExist(LPCSTR WindowName);
    void __cdecl WindowToBack();
    void __cdecl WindowToBack(LPCSTR WindowName);
    void __cdecl WindowToFront();
    void __cdecl WindowToFront(LPCSTR WindowName);
    IDirect3D9* __cdecl GetDirect3D();
    IDirect3DDevice9* __cdecl GetDirect3DDevice();
}

#endif
