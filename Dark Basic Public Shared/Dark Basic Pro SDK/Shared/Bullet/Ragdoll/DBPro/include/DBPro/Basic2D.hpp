/*
            File : DBPro/Basic2D.hpp
       Generated : 2011/09/02 20:14:25
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Basic2D_hpp
#define INCLUDE_GUARD_DBPro_Basic2D_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl Box(int Left,int Top,int Right,int Bottom);
    void __cdecl Box(int Left,int Top,int Right,int Bottom,DWORD Color1,DWORD Color2,DWORD Color3,DWORD Color4);
    void __cdecl Circle(int X,int Y,int Radius);
    void __cdecl Cls(int ColorValue);
    void __cdecl Dot(int X,int Y);
    void __cdecl Dot(int X,int Y,DWORD ColorValue);
    void __cdecl Ellipse(int X,int Y,int XRadius,int YRadius);
    int __cdecl GetPixelsPitch();
    DWORD __cdecl GetPixelsPointer();
    void __cdecl Ink(int ForegroundColor,int BackgroundColor);
    void __cdecl Line(int X1,int Y1,int X2,int Y2);
    void __cdecl LockPixels();
    DWORD __cdecl Point(int X,int Y);
    int __cdecl Rgbb(DWORD BlueValue);
    int __cdecl Rgbg(DWORD GreenValue);
    int __cdecl Rgbr(DWORD RedValue);
    DWORD __cdecl Rgb(int RedValue,int GreenValue,int BlueValue);
    void __cdecl UnlockPixels();
}

#endif
