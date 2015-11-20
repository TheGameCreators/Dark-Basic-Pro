/*
            File : DBPro/Bitmap.hpp
       Generated : 2011/09/02 20:14:29
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Bitmap_hpp
#define INCLUDE_GUARD_DBPro_Bitmap_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


#undef LoadBitmap
#undef DeleteBitmap


namespace DBPro
{
    int __cdecl BitmapDepth();
    int __cdecl BitmapDepth(int BitmapNumber);
    int __cdecl BitmapExist();
    int __cdecl BitmapExist(int BitmapNumber);
    int __cdecl BitmapFlipped();
    int __cdecl BitmapFlipped(int BitmapNumber);
    int __cdecl BitmapHeight();
    int __cdecl BitmapHeight(int BitmapNumber);
    int __cdecl BitmapMirrored();
    int __cdecl BitmapMirrored(int BitmapNumber);
    int __cdecl BitmapWidth();
    int __cdecl BitmapWidth(int BitmapNumber);
    void __cdecl BlurBitmap(int BitmapNumber,int BlurValue);
    void __cdecl CopyBitmap(int FromBitmap,int ToBitmap);
    void __cdecl CopyBitmap(int FromBitmap,int Left,int Top,int Right,int Bottom,int ToBitmap,int Left_7,int Top_8,int Right_9,int Bottom_10);
    void __cdecl CreateBitmap(int BitmapNumber,int Width,int Height);
    void __cdecl CreateBitmap(int BitmapNumber,int Width,int Height,int Systemmemorymode);
    int __cdecl CurrentBitmap();
    void __cdecl DeleteBitmap(int BitmapNumber);
    void __cdecl FadeBitmap(int BitmapNumber,int FadeValue);
    void __cdecl FlipBitmap(int BitmapNumber);
    void __cdecl LoadBitmap(LPCSTR Filename);
    void __cdecl LoadBitmap(LPCSTR Filename,int BitmapNumber);
    void __cdecl MirrorBitmap(int BitmapNumber);
    void __cdecl SetBitmapFormat(int BitmapFormatValue);
    void __cdecl SetCurrentBitmap(int BitmapNumber);
}

#endif
