/*
            File : DBPro/Matrix.hpp
       Generated : 2011/09/02 20:14:44
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Matrix_hpp
#define INCLUDE_GUARD_DBPro_Matrix_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl DeleteMatrix(int MatrixNumber);
    void __cdecl FillMatrix(int MatrixNumber,float Height,int TileNumber);
    float __cdecl GetGroundHeight(int MatrixNumber,float X,float Z);
    float __cdecl GetMatrixHeight(int MatrixNumber,int Tilex,int Tilez);
    void __cdecl GhostMatrixOff(int MatrixNumber);
    void __cdecl GhostMatrixOn(int MatrixNumber);
    void __cdecl GhostMatrixOn(int MatrixNumber,int Mode);
    void __cdecl MakeMatrix(int MatrixNumber,float Width,float Height,int Xsegments,int Zsegments);
    int __cdecl MatrixExist(int MatrixNumber);
    float __cdecl MatrixPositionX(int MatrixNumber);
    float __cdecl MatrixPositionY(int MatrixNumber);
    float __cdecl MatrixPositionZ(int MatrixNumber);
    int __cdecl MatrixTileCount(int MatrixNumber);
    int __cdecl MatrixTilesExist(int MatrixNumber);
    int __cdecl MatrixWireframeState(int MatrixNumber);
    void __cdecl PositionMatrix(int MatrixNumber,float X,float Y,float Z);
    void __cdecl PositionMatrix(int MatrixNumber,int Vector);
    void __cdecl PrepareMatrixTexture(int MatrixNumber,int ImageNumber,int Across,int Down);
    void __cdecl RandomizeMatrix(int MatrixNumber,int MaximumHeight);
    void __cdecl SetMatrixHeight(int MatrixNumber,int Tilex,int Tilez,float Height);
    void __cdecl SetMatrixNormal(int MatrixNumber,int Tilex,int Tilez,float Nx,float Ny,float Nz);
    void __cdecl SetMatrixPriority(int MatrixNumber,int PriorityFlag);
    void __cdecl SetMatrixTexture(int MatrixNumber,int TextureMode,int MipMode);
    void __cdecl SetMatrixTile(int MatrixNumber,int Tilex,int Tilez,int TileNumber);
    void __cdecl SetMatrixTrim(int MatrixNumber,float Trimx,float Trimy);
    void __cdecl SetMatrixWireframeOff(int MatrixNumber);
    void __cdecl SetMatrixWireframeOn(int MatrixNumber);
    void __cdecl SetMatrix(int MatrixNumber,int Wire,int Transparency,int Cull,int Filter,int Light,int Fog,int Ambient);
    void __cdecl SetVector3ToMatrixPosition(int Vector,int MatrixNumber);
    void __cdecl ShiftMatrixDown(int MatrixNumber);
    void __cdecl ShiftMatrixLeft(int MatrixNumber);
    void __cdecl ShiftMatrixRight(int MatrixNumber);
    void __cdecl ShiftMatrixUp(int MatrixNumber);
    void __cdecl UpdateMatrix(int MatrixNumber);
    int __cdecl GetMatrixXSegments(int Matrix);
    int __cdecl GetMatrixZSegments(int Matrix);
    int __cdecl GetMatrixWidth(int Matrix);
    int __cdecl GetMatrixDepth(int Matrix);
}

#endif
