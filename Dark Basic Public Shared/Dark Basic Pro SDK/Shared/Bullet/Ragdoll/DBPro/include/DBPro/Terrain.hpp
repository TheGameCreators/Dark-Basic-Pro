/*
            File : DBPro/Terrain.hpp
       Generated : 2011/09/02 20:14:58
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Terrain_hpp
#define INCLUDE_GUARD_DBPro_Terrain_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl BuildTerrain(int a);
    void __cdecl BuildTerrain(int a,int b);
    void __cdecl DestroyTerrain(int a);
    float __cdecl GetTerrainGroundHeight(int a,float b,float c);
    float __cdecl GetTerrainXSize(int a);
    float __cdecl GetTerrainZSize(int a);
    void __cdecl LoadTerrain(LPCSTR a,int b);
    void __cdecl MakeObjectTerrain(int a);
    void __cdecl SaveTerrain(LPCSTR a,int b);
    void __cdecl SetTerrainHeightmap(int a,LPCSTR b);
    void __cdecl SetTerrainLight(int a,float b,float c,float d,float e,float f,float g,float h);
    void __cdecl SetTerrainScale(int a,float b,float c,float d);
    void __cdecl SetTerrainSplit(int a,int b);
    void __cdecl SetTerrainTexture(int a,int b,int c);
    void __cdecl SetTerrainTiling(int a,int b);
    void __cdecl UpdateTerrain();
    void __cdecl UpdateTerrain(int NoCullingMode);
}

#endif
