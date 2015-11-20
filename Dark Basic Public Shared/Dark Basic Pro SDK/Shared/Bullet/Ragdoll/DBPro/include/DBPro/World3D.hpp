/*
            File : DBPro/World3D.hpp
       Generated : 2011/09/02 20:15:03
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_World3D_hpp
#define INCLUDE_GUARD_DBPro_World3D_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    int __cdecl BspCollisionHit(int CollisionIndex);
    float __cdecl BspCollisionX(int CollisionIndex);
    float __cdecl BspCollisionY(int CollisionIndex);
    float __cdecl BspCollisionZ(int CollisionIndex);
    void __cdecl DeleteBsp();
    void __cdecl LoadBsp(LPCSTR Pk3Filename,LPCSTR BspFilename);
    void __cdecl ProcessBspCollision(int CollisionIndex);
    void __cdecl SetBspCameraCollisionRadius(int CollisionIndex,int CameraNumber,float X,float Y,float Z);
    void __cdecl SetBspCameraCollision(int CollisionIndex,int CameraNumber,float Radius,int Response);
    void __cdecl SetBspCamera(int CameraNumber);
    void __cdecl SetBspCollisionHeightAdjustment(int CollisionIndex,float Height);
    void __cdecl SetBspCollisionOff(int CollisionIndex);
    void __cdecl SetBspCollisionThreshhold(int CollisionIndex,float Sensitivity);
    void __cdecl SetBspMultitexturingOff();
    void __cdecl SetBspMultitexturingOn();
    void __cdecl SetBspObjectCollisionRadius(int CollisionIndex,int ObjectNumber,float X,float Y,float Z);
    void __cdecl SetBspObjectCollision(int CollisionIndex,int ObjectNumber,float Radius,int Response);
}

#endif
