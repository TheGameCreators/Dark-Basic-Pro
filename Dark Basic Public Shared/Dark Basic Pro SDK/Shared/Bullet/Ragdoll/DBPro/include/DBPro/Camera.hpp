/*
            File : DBPro/Camera.hpp
       Generated : 2011/09/02 20:14:31
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Camera_hpp
#define INCLUDE_GUARD_DBPro_Camera_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


// Include DBPro specific types, if not already included
#ifndef _CCAMERADATA_H_
#include <Official/ccameradatac.h>
#endif
#ifndef INCLUDE_GUARD_Support_DBProTypes_hpp
#include <Support/DBProTypes.hpp>
#endif



namespace DBPro
{
    void __cdecl AutocamOff();
    void __cdecl AutocamOn();
    void __cdecl BackdropOff();
    void __cdecl BackdropOff(int CameraNumber);
    void __cdecl BackdropOn();
    void __cdecl BackdropOn(int CameraNumber);
    float __cdecl CameraAngleX();
    float __cdecl CameraAngleX(int CameraNumber);
    float __cdecl CameraAngleY();
    float __cdecl CameraAngleY(int CameraNumber);
    float __cdecl CameraAngleZ();
    float __cdecl CameraAngleZ(int CameraNumber);
    int __cdecl CameraExist(int CameraNumber);
    float __cdecl CameraPositionX();
    float __cdecl CameraPositionX(int CameraNumber);
    float __cdecl CameraPositionY();
    float __cdecl CameraPositionY(int CameraNumber);
    float __cdecl CameraPositionZ();
    float __cdecl CameraPositionZ(int CameraNumber);
    void __cdecl ClearCameraView(int ColorValue);
    void __cdecl ClearCameraView(int CameraNumber,DWORD ColorValue);
    void __cdecl ColorBackdrop(DWORD ColorValue);
    void __cdecl ColorBackdrop(int CameraNumber,DWORD ColorValue);
    void __cdecl ColorBackdrop(int CameraNumber,DWORD ColorValue,DWORD ForegroundWipeColor);
    void __cdecl ControlCameraUsingArrowkeys(int CameraNumber,float Movespeed,float Turnspeed);
    void __cdecl DeleteCamera(int CameraNumber);
    void __cdecl MakeCamera(int CameraNumber);
    void __cdecl MoveCamera(float DistanceValue);
    void __cdecl MoveCamera(int CameraNumber,float DistanceValue);
    void __cdecl PitchCameraDown(float AngleValue);
    void __cdecl PitchCameraDown(int CameraNumber,float AngleValue);
    void __cdecl PitchCameraUp(float AngleValue);
    void __cdecl PitchCameraUp(int CameraNumber,float AngleValue);
    void __cdecl PointCamera(float X,float Y,float Z);
    void __cdecl PointCamera(int CameraNumber,float X,float Y,float Z);
    void __cdecl PositionCamera(float X,float Y,float Z);
    void __cdecl PositionCamera(int CameraNumber,float X,float Y,float Z);
    void __cdecl PositionCamera(int CameraNumber,int Vector);
    void __cdecl RollCameraLeft(float AngleValue);
    void __cdecl RollCameraLeft(int CameraNumber,float AngleValue);
    void __cdecl RollCameraRight(float AngleValue);
    void __cdecl RollCameraRight(int CameraNumber,float AngleValue);
    void __cdecl RotateCamera(float Xangle,float Yangle,float Zangle);
    void __cdecl RotateCamera(int CameraNumber,float Xangle,float Yangle,float Zangle);
    void __cdecl RotateCamera(int CameraNumber,int Vector);
    void __cdecl ScrollBackdrop(int U,int V);
    void __cdecl ScrollBackdrop(int CameraNumber,int U,int V);
    void __cdecl SetCameraAspect(float  aspectRatio);
    void __cdecl SetCameraAspect(int CameraNumber,float AspectRatio);
    void __cdecl SetCameraClip(int CameraNumber,int Cliponoff,float X,float Y,float Z,float Nx,float Ny,float Nz);
    void __cdecl SetCameraFov(float FieldofviewAngle);
    void __cdecl SetCameraFov(int CameraNumber,float FieldofviewAngle);
    void __cdecl SetCameraRange(float NearValue,float FarValue);
    void __cdecl SetCameraRange(int CameraNumber,float NearValue,float FarValue);
    void __cdecl SetCameraRotationXyz();
    void __cdecl SetCameraRotationXyz(int CameraNumber);
    void __cdecl SetCameraRotationZyx();
    void __cdecl SetCameraRotationZyx(int CameraNumber);
    void __cdecl SetCameraToFollow(float X,float Y,float Z,float Angle,float Distance,float Height,float Smooth,int Collision);
    void __cdecl SetCameraToFollow(int CameraNumber,float X,float Y,float Z,float Angle,float Distance,float Height,float Smooth,int Collision);
    void __cdecl SetCameraToImage(int CameraNumber,int ImageNumber,int Width,int Height);
    void __cdecl SetCameraToImage(int CameraNumber,int ImageNumber,int Width,int Height,int GenerateAlphaMode);
    void __cdecl SetCameraToImage(int CameraNumber,int ImageNumber,int Width,int Height,int GenerateAlphaMode,DWORD D3dFormat);
    void __cdecl SetCameraToObjectOrientation(int ObjectNumber);
    void __cdecl SetCameraToObjectOrientation(int CameraNumber,int ObjectNumber);
    void __cdecl SetCameraView(int Left,int Top,int Right,int Bottom);
    void __cdecl SetCameraView(int CameraNumber,int Left,int Top,int Right,int Bottom);
    void __cdecl SetCamerasToStereoscopic(int StereoscopicMode,int CameraLeft,int CameraRight,int ImageBack,int ImageFront);
    void __cdecl SetCurrentCamera(int CameraNumber);
    void __cdecl SetVector3ToCameraPosition(int Vector,int CameraNumber);
    void __cdecl SetVector3ToCameraRotation(int Vector,int CameraNumber);
    void __cdecl TextureBackdrop(int ImageNumber);
    void __cdecl TextureBackdrop(int CameraNumber,int ImageNumber);
    void __cdecl TurnCameraLeft(float AngleValue);
    void __cdecl TurnCameraLeft(int CameraNumber,float AngleValue);
    void __cdecl TurnCameraRight(float AngleValue);
    void __cdecl TurnCameraRight(int CameraNumber,float AngleValue);
    void __cdecl XrotateCamera(float Xangle);
    void __cdecl XrotateCamera(int CameraNumber,float Xangle);
    void __cdecl YrotateCamera(float Yangle);
    void __cdecl YrotateCamera(int CameraNumber,float Yangle);
    void __cdecl ZrotateCamera(float Zangle);
    void __cdecl ZrotateCamera(int CameraNumber,float Zangle);
    void __cdecl MoveCameraLeft(int Camera,float Distance);
    void __cdecl MoveCameraRight(int Camera,float Distance);
    void __cdecl MoveCameraUp(int Camera,float Distance);
    void __cdecl MoveCameraDown(int Camera,float Distance);
    D3DXMATRIX __cdecl GetCameraMatrix(int Camera);
    int __cdecl GetRenderCamera();
    sCamera* __cdecl GetCameraData(int Camera);
    void __cdecl CameraInternalUpdate(int Camera);
}

#endif
