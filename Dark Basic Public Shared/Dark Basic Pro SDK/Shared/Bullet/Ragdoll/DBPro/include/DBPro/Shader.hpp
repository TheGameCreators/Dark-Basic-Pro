/*
            File : DBPro/Shader.hpp
       Generated : 2011/09/02 20:15:09
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Shader_hpp
#define INCLUDE_GUARD_DBPro_Shader_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl ApplyCubeMapToObject(int a,int b,int c);
    void __cdecl ApplyCubeMapToObject(int a,int b,int c,int d);
    int __cdecl CameraEffectExist(int a);
    void __cdecl CheckData();
    int __cdecl CountExternalParameters();
    int __cdecl DebugCompileEffect(LPCSTR a);
    void __cdecl DeleteCameraEffect(int a);
    void __cdecl DeleteDynamicCubeMap(int a);
    int __cdecl DynamicCubeMapExist(int a);
    LPSTR __cdecl FindDefaultTechnique();
    LPSTR __cdecl FindLightTechnique(int a);
    int __cdecl GetCameraEffect(int a);
    LPSTR __cdecl GetDynamicTextureName(int a);
    LPSTR __cdecl GetEffectDescription();
    LPSTR __cdecl GetEffectErrorString(int a);
    LPSTR __cdecl GetEffectImage();
    int __cdecl GetEffectPsVersion();
    int __cdecl GetEffectParameterCount();
    float __cdecl GetEffectParameterFloatValue(LPCSTR a);
    void __cdecl GetEffectParameterVectorValue(LPCSTR a,int b);
    int __cdecl GetEffectTextureCount();
    LPSTR __cdecl GetEffectTextureName(int a);
    int __cdecl GetEffectVsVersion();
    int __cdecl GetExternalParameterAnnotationCount(int a);
    int __cdecl GetExternalParameterColumns(int a);
    LPSTR __cdecl GetExternalParameterName(int a);
    int __cdecl GetExternalParameterRows(int a);
    LPSTR __cdecl GetExternalParameterSemantic(int a);
    int __cdecl GetExternalParameterType(int a);
    float __cdecl GetMatrix4(int a,int b,int c);
    LPSTR __cdecl GetModelTextureName(int a,int b,int c);
    int __cdecl GetObjectEffect(int a,int b);
    int __cdecl GetParameterAnnotationExist(LPCSTR a,LPCSTR b);
    float __cdecl GetParameterAnnotationFloat(LPCSTR a,LPCSTR b);
    LPSTR __cdecl GetParameterAnnotationString(LPCSTR a,LPCSTR b);
    int __cdecl GetTextureStageInUse(int a,int b,int c);
    int __cdecl GetTextureType(LPCSTR a);
    int __cdecl IsAlphaMappingShader();
    int __cdecl IsDynamicTexture(int a);
    int __cdecl IsLightingShader();
    void __cdecl LoadCameraEffect(LPCSTR a,int b,int c);
    void __cdecl MakeDynamicCubeMap(int a,int b);
    void __cdecl MakeDynamicCubeMap(int a,int b,int c);
    void __cdecl OnResetDevice();
    void __cdecl ReleaseEffectPointer();
    void __cdecl ReleaseErrorPointer();
    void __cdecl ReloadDisplayPointer();
    void __cdecl RemoveCameraEffect(int a);
    void __cdecl RemoveCameraFromImage(int a);
    void __cdecl RenderDynamicCubeMap(int a,int b,float c,float d,float e);
    void __cdecl Render();
    void __cdecl SaveEffect(LPCSTR a);
    void __cdecl SetCameraEffectConstantFloat(int a,LPCSTR b,float c);
    void __cdecl SetCameraEffectConstantInteger(int a,LPCSTR b,int c);
    void __cdecl SetCameraEffectConstantVector(int a,LPCSTR b,int c);
    void __cdecl SetCameraEffect(int a,int b,int c);
    void __cdecl SetCameraToSurface(int a);
    void __cdecl SetEffectConstantVectorElement(LPCSTR a,int b,int c);
    void __cdecl SetMatrix4(int a,int b,int c,float d);
    void __cdecl SyncCamera(int a);
}

#endif
