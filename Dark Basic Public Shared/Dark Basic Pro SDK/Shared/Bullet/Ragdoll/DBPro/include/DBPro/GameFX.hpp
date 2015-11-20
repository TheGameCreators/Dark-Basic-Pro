/*
            File : DBPro/GameFX.hpp
       Generated : 2011/09/02 20:15:07
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_GameFX_hpp
#define INCLUDE_GUARD_DBPro_GameFX_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl AddLightMapLight(float X,float Y,float Z,float Radius,float Red,float Green,float Blue,float Brightness,int Shadow);
    void __cdecl AddLimbToLightMapPool(int a,int b);
    void __cdecl AddLodToObject(int ObjectNumber,int LodObject,int LodLevel,float LodDistance);
    void __cdecl AddObjectToLightMapPool(int ObjectNumber);
    void __cdecl AddStaticObjectsToLightMapPool();
    void __cdecl AddStaticScorch(float ScorchSize,int ScorchType);
    void __cdecl AttachObjectToStatic(int ObjectNumber);
    void __cdecl BuildStaticPortals();
    void __cdecl CreateLightMaps(int TextureSize,int TextureQuality,LPCSTR Lightmapsavepath);
    void __cdecl DeleteLightMapLights();
    void __cdecl DeleteStaticObject(int ObjectNumber);
    void __cdecl DeleteStaticObjects();
    void __cdecl DetachObjectFromStatic(int ObjectNumber);
    int __cdecl GetStaticCollisionCount();
    int __cdecl GetStaticCollisionFloor();
    int __cdecl GetStaticCollisionHit(float Ox1,float Oy1,float Oz1,float Ox2,float Oy2,float Oz2,float Nx1,float Ny1,float Nz1,float Nx2,float Ny2,float Nz2);
    int __cdecl GetStaticCollisionValue();
    float __cdecl GetStaticCollisionX();
    float __cdecl GetStaticCollisionY();
    float __cdecl GetStaticCollisionZ();
    void __cdecl LoadStaticObjects(LPCSTR Filename,int DivideTextureSize);
    void __cdecl MakeStaticCollisionBox(float X1,float Y1,float Z1,float X2,float Y2,float Z2);
    void __cdecl MakeStaticLimb(int ObjectNumber,int LimbNumber,int Collision,int ArbitaryValue,int CastShadow,int PortalBlocker);
    void __cdecl MakeStaticObject(int ObjectNumber,int Collision,int ArbitaryValue,int CastShadow,int PortalBlocker);
    void __cdecl PerformCsgClip(int TargetObject,int BrushObject);
    void __cdecl PerformCsgDifferenceOnVertexdata(int BrushMesh);
    void __cdecl PerformCsgDifference(int TargetObject,int BrushObject);
    void __cdecl PerformCsgIntersectionOnVertexdata(int BrushMesh);
    void __cdecl PerformCsgIntersection(int TargetObject,int BrushObject);
    void __cdecl PerformCsgUnionOnVertexdata(int BrushMesh);
    void __cdecl PerformCsgUnion(int TargetObject,int BrushObject);
    void __cdecl ReduceMesh(int MeshNumber,int BlockMode,int NearMode,int Gx,int Gy,int Gz);
    void __cdecl SaveStaticObjects(LPCSTR Filename);
    void __cdecl SetGlobalShadowColor(int Red,int Green,int Blue,int Alpha);
    void __cdecl SetGlobalShadowShades(int NumberOfShades);
    void __cdecl SetGlobalShadowsOff();
    void __cdecl SetGlobalShadowsOn();
    void __cdecl SetShadowLight(int LightNumber,float X,float Y,float Z,float Range);
    void __cdecl SetShadowPosition(int Mode,float X,float Y,float Z);
    void __cdecl SetStaticCulling(int FullPortalTraverseFlag);
    void __cdecl SetStaticEffectTechnique(LPCSTR TechniqueName);
    void __cdecl SetStaticObjectsWireframeOff();
    void __cdecl SetStaticObjectsWireframeOn();
    void __cdecl SetStaticPortalsOff();
    void __cdecl SetStaticPortalsOn();
    void __cdecl SetStaticScorch(int ImageNumber,int Width,int Height);
    void __cdecl SetStaticUniverse(float X,float Y,float Z);
    int __cdecl StaticLineOfSight(float X1,float Y1,float Z1,float X2,float Y2,float Z2,float Width,float Accuracy);
    int __cdecl StaticRaycast(float X1,float Y1,float Z1,float X2,float Y2,float Z2);
    int __cdecl StaticVolume(float X1,float Y1,float Z1,float X2,float Y2,float Z2,float Size);
}

#endif
