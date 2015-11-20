/*
            File : DBPro/Memblock.hpp
       Generated : 2011/09/02 20:14:46
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Memblock_hpp
#define INCLUDE_GUARD_DBPro_Memblock_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl ChangeMeshFromMemblock(int MeshNumber,int MemblockNumber);
    void __cdecl CopyMemblock(int From,int To,int Posfrom,int Posto,int Bytes);
    void __cdecl DeleteMemblock(int MemblockNumber);
    LPVOID __cdecl GetMemblockPtr(int MemblockNumber);
    int __cdecl GetMemblockSize(int MemblockNumber);
    void __cdecl MakeBitmapFromMemblock(int BitmapNumber,int MemblockNumber);
    void __cdecl MakeImageFromMemblock(int ImageNumber,int MemblockNumber);
    void __cdecl MakeMemblockFromBitmap(int MemblockNumber,int BitmapNumber);
    void __cdecl MakeMemblockFromImage(int MemblockNumber,int ImageNumber);
    void __cdecl MakeMemblockFromMesh(int MemblockNumber,int MeshNumber);
    void __cdecl MakeMemblockFromSound(int MemblockNumber,int SoundNumber);
    void __cdecl MakeMemblock(int MemblockNumber,int SizeInBytes);
    void __cdecl MakeMeshFromMemblock(int MeshNumber,int MemblockNumber);
    void __cdecl MakeSoundFromMemblock(int SoundNumber,int MemblockNumber);
    int __cdecl MemblockByte(int MemblockNumber,int Position);
    DWORD __cdecl MemblockDword(int MemblockNumber,int Position);
    int __cdecl MemblockExist(int MemblockNumber);
    float __cdecl MemblockFloat(int MemblockNumber,int Position);
    int __cdecl MemblockWord(int MemblockNumber,int Position);
    void __cdecl WriteMemblockByte(int MemblockNumber,int Position,int Byte);
    void __cdecl WriteMemblockDword(int MemblockNumber,int Position,DWORD Dword);
    void __cdecl WriteMemblockFloat(int MemblockNumber,int Position,float Float);
    void __cdecl WriteMemblockWord(int MemblockNumber,int Position,int Word);
    void __cdecl ReturnMemblockPtrs(DWORD* SizeArray,LPSTR* DataArray);
}

#endif
