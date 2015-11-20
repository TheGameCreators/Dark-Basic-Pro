/*
            File : DBPro/Image.hpp
       Generated : 2011/09/02 20:14:38
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Image_hpp
#define INCLUDE_GUARD_DBPro_Image_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


#undef LoadImage
#undef LoadIcon


namespace DBPro
{
    void __cdecl CloseImageblock();
    void __cdecl DeleteIcon(DWORD IconHandle);
    void __cdecl DeleteImage(int ImageNumber);
    void __cdecl ExcludeFromImageblock(LPCSTR ExcludePath);
    void __cdecl GetImage(int ImageNumber,int Left,int Top,int Right,int Bottom);
    void __cdecl GetImage(int ImageNumber,int Left,int Top,int Right,int Bottom,int TextureFlag);
    int __cdecl ImageExist(int ImageNumber);
    int __cdecl ImageFileExist(LPCSTR Filename);
    int __cdecl ImageHeight(int ImageNumber);
    int __cdecl ImageWidth(int ImageNumber);
    DWORD __cdecl LoadIcon(LPCSTR IconFilename);
    void __cdecl LoadImage(LPCSTR Filename,int ImageNumber);
    void __cdecl LoadImage(LPCSTR Filename,int ImageNumber,int TextureFlag);
    void __cdecl LoadImage(LPCSTR Filename,int ImageNumber,int TextureFlag,int DivideTextureSize);
    void __cdecl OpenImageblock(LPCSTR Filename,int Mode);
    void __cdecl PasteImage(int ImageNumber,int X,int Y);
    void __cdecl PasteImage(int ImageNumber,int X,int Y,int Transparency);
    void __cdecl PerformChecklistForImageblockFiles();
    void __cdecl RotateImage(int ImageNumber,int Angle);
    void __cdecl SaveIconFromImage(LPCSTR IconFilename,int ImageNumber);
    void __cdecl SaveImage(LPCSTR Filename,int ImageNumber);
    void __cdecl SaveImage(LPCSTR Filename,int ImageNumber,int CompressionMode);
    void __cdecl SetImageColorkey(int RedValue,int GreenValue,int BlueValue);
    LPDIRECT3DTEXTURE9 __cdecl GetImageTexturePtr(int ImageNumber);
    LPDIRECT3DTEXTURE9 __cdecl MakeImage(int ImageNumber,int Width,int Height);
    LPDIRECT3DTEXTURE9 __cdecl MakeImage(int ImageNumber,int Width,int Height,DWORD Usage);
    LPDIRECT3DTEXTURE9 __cdecl MakeImage(int ImageNumber,int Width,int Height,enum _D3DFORMAT D3dFormat);
    LPDIRECT3DTEXTURE9 __cdecl MakeImage(int ImageNumber,int Width,int Height,enum _D3DFORMAT D3dFormat,DWORD Usage);
}

#endif
