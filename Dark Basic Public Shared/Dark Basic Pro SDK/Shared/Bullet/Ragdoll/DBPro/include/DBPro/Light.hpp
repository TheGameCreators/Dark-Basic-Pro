/*
            File : DBPro/Light.hpp
       Generated : 2011/09/02 20:14:42
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Light_hpp
#define INCLUDE_GUARD_DBPro_Light_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    int __cdecl AmbientLight();
    void __cdecl ColorAmbientLight(DWORD ColorValue);
    void __cdecl ColorLight(int LightNumber,DWORD ColorValue);
    void __cdecl ColorLight(int LightNumber,int Red,int Green,int Blue);
    void __cdecl DeleteLight(int LightNumber);
    void __cdecl FogColor(DWORD ColorValue);
    void __cdecl FogColor(int Red,int Green,int Blue);
    void __cdecl FogColor(int Red,int Green,int Blue,int Alpha);
    void __cdecl FogDistance(int Distance);
    void __cdecl FogDistance(int StartDistance,int EndDistance);
    void __cdecl FogOff();
    void __cdecl FogOn();
    void __cdecl HideLight(int LightNumber);
    float __cdecl LightDirectionX(int LightNumber);
    float __cdecl LightDirectionY(int LightNumber);
    float __cdecl LightDirectionZ(int LightNumber);
    int __cdecl LightExist(int LightNumber);
    float __cdecl LightPositionX(int LightNumber);
    float __cdecl LightPositionY(int LightNumber);
    float __cdecl LightPositionZ(int LightNumber);
    float __cdecl LightRange(int LightNumber);
    int __cdecl LightType(int LightNumber);
    int __cdecl LightVisible(int LightNumber);
    void __cdecl MakeLight(int LightNumber);
    void __cdecl PointLight(int LightNumber,float X,float Y,float Z);
    void __cdecl PositionLight(int LightNumber,float X,float Y,float Z);
    void __cdecl PositionLight(int LightNumber,int Vector);
    void __cdecl RotateLight(int LightNumber,float Xangle,float Yangle,float Zangle);
    void __cdecl RotateLight(int LightNumber,int Vector);
    void __cdecl SetAmbientLight(int Percentage);
    void __cdecl SetDirectionalLight(int LightNumber,float Nx,float Ny,float Nz);
    void __cdecl SetLightRange(int LightNumber,float Distance);
    void __cdecl SetLightToObjectOrientation(int LightNumber,int ObjectNumber);
    void __cdecl SetLightToObjectPosition(int LightNumber,int ObjectNumber);
    void __cdecl SetNormalizationOff();
    void __cdecl SetNormalizationOn();
    void __cdecl SetPointLight(int LightNumber,float X,float Y,float Z);
    void __cdecl SetSpotLight(int LightNumber,float InnerAngle,float OuterAngle);
    void __cdecl SetVector3ToLightPosition(int Vector,int LightNumber);
    void __cdecl SetVector3ToLightRotation(int Vector,int LightNumber);
    void __cdecl ShowLight(int LightNumber);
}

#endif
