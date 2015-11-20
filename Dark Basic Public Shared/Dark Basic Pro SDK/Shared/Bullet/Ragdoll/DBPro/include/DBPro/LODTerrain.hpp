/*
            File : DBPro/LODTerrain.hpp
       Generated : 2011/09/02 20:14:41
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_LODTerrain_hpp
#define INCLUDE_GUARD_DBPro_LODTerrain_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl DeleteTerrain(int TerrainNumber);
    float __cdecl GetTerrainHeight(int TerrainNumber,float X,float Z);
    float __cdecl GetTotalTerrainHeight(int TerrainNumber);
    void __cdecl MakeTerrain(int TerrainNumber,LPCSTR HeightmapFilename);
    void __cdecl PositionTerrain(int TerrainNumber,float X,float Y,float Z);
    int __cdecl TerrainExist(int TerrainNumber);
    float __cdecl TerrainPositionX(int TerrainNumber);
    float __cdecl TerrainPositionY(int TerrainNumber);
    float __cdecl TerrainPositionZ(int TerrainNumber);
    void __cdecl TextureTerrain(int TerrainNumber,int ImageNumber);
}

#endif
