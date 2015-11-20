/*
            File : DBPro/Vector.hpp
       Generated : 2011/09/02 20:15:01
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Vector_hpp
#define INCLUDE_GUARD_DBPro_Vector_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl AddMatrix4(int Matrix4result,int Matrix4a,int Matrix4b);
    void __cdecl AddVector2(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl AddVector3(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl AddVector4(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl BccVector2(int Vectorresult,int Vectora,int Vectorb,int Vectorc,float Fvalue,float Gbvalue);
    void __cdecl BccVector3(int Vectorresult,int Vectora,int Vectorb,int Vectorc,float Fvalue,float Gbvalue);
    void __cdecl BccVector4(int Vectorresult,int Vectora,int Vectorb,int Vectorc,float Fvalue,float Gbvalue);
    void __cdecl BuildFovLhmatrix4(int Matrix4result,float Fov,float Aspect,float Near,float Far);
    void __cdecl BuildFovRhmatrix4(int Matrix4result,float Fov,float Aspect,float Near,float Far);
    void __cdecl BuildLookatLhmatrix4(int Matrix4result,int Vector3eye,int Vector3at,int Vector3up);
    void __cdecl BuildLookatRhmatrix4(int Matrix4result,int Vector3eye,int Vector3at,int Vector3up);
    void __cdecl BuildOrthoLhmatrix4(int Matrix4result,float Width,float Height,float Near,float Far);
    void __cdecl BuildOrthoRhmatrix4(int Matrix4result,float Width,float Height,float Near,float Far);
    void __cdecl BuildPerspectiveLhmatrix4(int Matrix4result,float Width,float Height,float Near,float Far);
    void __cdecl BuildPerspectiveRhmatrix4(int Matrix4result,float Width,float Height,float Near,float Far);
    void __cdecl BuildReflectionMatrix4(int Matrix4result,float Planea,float Planeb,float Planec,float Planed);
    void __cdecl BuildRotationAxisMatrix4(int Matrix4result,int Vector3axis,float Angle);
    void __cdecl CatmullromVector2(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Value);
    void __cdecl CatmullromVector3(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Value);
    void __cdecl CatmullromVector4(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Value);
    float __cdecl CcwVector2(int Vectora,int Vectorb);
    void __cdecl CopyMatrix4(int Matrix4result,int Matrix4source);
    void __cdecl CopyVector2(int Vectorresult,int Vectorsource);
    void __cdecl CopyVector3(int Vectorresult,int Vectorsource);
    void __cdecl CopyVector4(int Vectorresult,int Vectorsource);
    void __cdecl CrossProductVector3(int Vectorresult,int Vectora,int Vectorb);
    int __cdecl DeleteMatrix4(int Matrix4result);
    int __cdecl DeleteVector2(int Vector);
    int __cdecl DeleteVector3(int Vector);
    int __cdecl DeleteVector4(int Vector);
    void __cdecl DivideMatrix4(int Matrix4result,float Value);
    void __cdecl DivideVector2(int Vectorresult,float Value);
    void __cdecl DivideVector3(int Vectorresult,float Value);
    void __cdecl DivideVector4(int Vectorresult,float Value);
    float __cdecl DotProductVector2(int Vectora,int Vectorb);
    float __cdecl DotProductVector3(int Vectora,int Vectorb);
    float __cdecl GetMatrix4Element(int Matrix4result,int ElementIndex);
    void __cdecl HermiteVector2(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Svalue);
    void __cdecl HermiteVector3(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Svalue);
    void __cdecl HermiteVector4(int Vectorresult,int Vectora,int Vectorb,int Vectorc,int Vectord,float Svalue);
    float __cdecl InverseMatrix4(int Matrix4result,int Matrix4source);
    int __cdecl IsEqualMatrix4(int Matrix4a,int Matrix4b);
    int __cdecl IsEqualVector2(int Vectora,int Vectorb);
    int __cdecl IsEqualVector3(int Vectora,int Vectorb);
    int __cdecl IsEqualVector4(int Vectora,int Vectorb);
    int __cdecl IsIdentityMatrix4(int Matrix4result);
    float __cdecl LengthVector2(int Vector);
    float __cdecl LengthVector3(int Vector);
    float __cdecl LengthVector4(int Vector);
    void __cdecl LinearInterpolateVector2(int Vectorresult,int Vectora,int Vectorb,float Svalue);
    void __cdecl LinearInterpolateVector3(int Vectorresult,int Vectora,int Vectorb,float Svalue);
    void __cdecl LinearInterpolateVector4(int Vectorresult,int Vectora,int Vectorb,float Svalue);
    int __cdecl MakeMatrix4(int Matrix4);
    int __cdecl MakeVector2(int Vector);
    int __cdecl MakeVector3(int Vector);
    int __cdecl MakeVector4(int Vector);
    void __cdecl MaximizeVector2(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MaximizeVector3(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MaximizeVector4(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MinimizeVector2(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MinimizeVector3(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MinimizeVector4(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl MultiplyMatrix4(int Matrix4result,float Value);
    void __cdecl MultiplyMatrix4(int Matrix4result,int Matrix4a,int Matrix4b);
    void __cdecl MultiplyVector2(int Vectorresult,float Value);
    void __cdecl MultiplyVector3(int Vectorresult,float Value);
    void __cdecl MultiplyVector4(int Vectorresult,float Value);
    void __cdecl NormalizeVector2(int Vectorresult,int Vectorsource);
    void __cdecl NormalizeVector3(int Vectorresult,int Vectorsource);
    void __cdecl NormalizeVector4(int Vectorresult,int Vectorsource);
    void __cdecl ProjectVector3(int Vectorresult,int Vectorsource,int Matrix4projection,int Matrix4view,int Matrix4world);
    void __cdecl ProjectionMatrix4(int Matrix4result);
    void __cdecl RotateXMatrix4(int Matrix4result,float Angle);
    void __cdecl RotateYMatrix4(int Matrix4result,float Angle);
    void __cdecl RotateYprMatrix4(int Matrix4result,float Yaw,float Pitch,float Roll);
    void __cdecl RotateZMatrix4(int Matrix4result,float Angle);
    void __cdecl ScaleMatrix4(int Matrix4result,float X,float Y,float Z);
    void __cdecl ScaleVector2(int Vectorresult,int Vectorsource,float Value);
    void __cdecl ScaleVector3(int Vectorresult,int Vectorsource,float Value);
    void __cdecl ScaleVector4(int Vectorresult,int Vectorsource,float Value);
    void __cdecl SetIdentityMatrix4(int Matrix4result);
    void __cdecl SetVector2(int Vectorresult,float X,float Y);
    void __cdecl SetVector3(int Vectorresult,float X,float Y,float Z);
    void __cdecl SetVector4(int Vectorresult,float X,float Y,float Z,float W);
    float __cdecl SquaredLengthVector2(int Vector);
    float __cdecl SquaredLengthVector3(int Vector);
    float __cdecl SquaredLengthVector4(int Vector);
    void __cdecl SubtractMatrix4(int Matrix4result,int Matrix4a,int Matrix4b);
    void __cdecl SubtractVector2(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl SubtractVector3(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl SubtractVector4(int Vectorresult,int Vectora,int Vectorb);
    void __cdecl TransformCoordsVector2(int Vectorresult,int Vectorsource,int Matrix4source);
    void __cdecl TransformCoordsVector3(int Vectorresult,int Vectorsource,int Matrix4source);
    void __cdecl TransformNormalsVector3(int Vectorresult,int Vectorsource,int Matrix4source);
    void __cdecl TransformVector4(int Vectorresult,int Vectorsource,int Matrix4source);
    void __cdecl TranslateMatrix4(int Matrix4result,float X,float Y,float Z);
    void __cdecl TransposeMatrix4(int Matrix4result,int Matrix4source);
    void __cdecl ViewMatrix4(int Matrix4result);
    float __cdecl WVector4(int Vector);
    void __cdecl WorldMatrix4(int Matrix4result);
    float __cdecl XVector2(int Vector);
    float __cdecl XVector3(int Vector);
    float __cdecl XVector4(int Vector);
    float __cdecl YVector2(int Vector);
    float __cdecl YVector3(int Vector);
    float __cdecl YVector4(int Vector);
    float __cdecl ZVector3(int Vector);
    float __cdecl ZVector4(int Vector);
    D3DXMATRIX __cdecl GetMatrix4(int Matrix4);
    D3DXVECTOR2 __cdecl GetVector2(int Vector2);
    D3DXVECTOR3 __cdecl GetVector3(int Vector3);
    D3DXVECTOR4 __cdecl GetVector4(int Vector4);
    int __cdecl VectorExist(int VectorID);
    bool __cdecl CheckVectorType(int a,int b);
}

#endif
