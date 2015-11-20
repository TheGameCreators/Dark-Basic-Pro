/*
            File : DBPro/Sprite.hpp
       Generated : 2011/09/02 20:14:55
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Sprite_hpp
#define INCLUDE_GUARD_DBPro_Sprite_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl CloneSprite(int SpriteNumber,int DestinationSpriteNumber);
    void __cdecl CreateAnimatedSprite(int SpriteNumber,LPCSTR Filename,int Across,int Down,int ImageNumber);
    void __cdecl DeleteSprite(int SpriteNumber);
    void __cdecl FlipSprite(int SpriteNumber);
    void __cdecl HideAllSprites();
    void __cdecl HideSprite(int SpriteNumber);
    void __cdecl MirrorSprite(int SpriteNumber);
    void __cdecl MoveSprite(int SpriteNumber,float Velocity);
    void __cdecl OffsetSprite(int SpriteNumber,int Xoffset,int Yoffset);
    void __cdecl PasteSprite(int SpriteNumber,int Xpos,int Ypos);
    void __cdecl PlaySprite(int SpriteNumber,int StartFrame,int EndFrame,int DelayValue);
    void __cdecl RotateSprite(int SpriteNumber,float Angle);
    void __cdecl ScaleSprite(int SpriteNumber,float Scale);
    void __cdecl SetSpriteAlpha(int SpriteNumber,int AlphaValue);
    void __cdecl SetSpriteDiffuse(int SpriteNumber,int RedValue,int GreenValue,int BlueValue);
    void __cdecl SetSpriteFilterMode(int ClampMode);
    void __cdecl SetSpriteFrame(int SpriteNumber,int FrameValue);
    void __cdecl SetSpriteImage(int SpriteNumber,int ImageNumber);
    void __cdecl SetSpritePriority(int SpriteNumber,int Priority);
    void __cdecl SetSpriteResize(int Mode);
    void __cdecl SetSpriteResize(int Mode,float GlobalXOffset,float GlobalXScale);
    void __cdecl SetSpriteTextureCoord(int SpriteNumber,int VertexIndex,float Uvalue,float Vvalue);
    void __cdecl SetSprite(int SpriteNumber,int Backsave,int Transparency);
    void __cdecl ShowAllSprites();
    void __cdecl ShowSprite(int SpriteNumber);
    void __cdecl SizeSprite(int SpriteNumber,int Xsize,int Ysize);
    int __cdecl SpriteAlpha(int SpriteNumber);
    float __cdecl SpriteAngle(int SpriteNumber);
    int __cdecl SpriteBlue(int SpriteNumber);
    int __cdecl SpriteCollision(int SpriteNumber,int TargetSpriteNumber);
    int __cdecl SpriteExist(int SpriteNumber);
    int __cdecl SpriteFlipped(int SpriteNumber);
    int __cdecl SpriteFrame(int SpriteNumber);
    int __cdecl SpriteGreen(int SpriteNumber);
    int __cdecl SpriteHeight(int SpriteNumber);
    int __cdecl SpriteHit(int SpriteNumber,int TargetSpriteNumber);
    int __cdecl SpriteImage(int SpriteNumber);
    int __cdecl SpriteMirrored(int SpriteNumber);
    int __cdecl SpriteOffsetX(int SpriteNumber);
    int __cdecl SpriteOffsetY(int SpriteNumber);
    int __cdecl SpritePriority(int SpriteNumber);
    int __cdecl SpriteRed(int SpriteNumber);
    int __cdecl SpriteScaleX(int SpriteNumber);
    int __cdecl SpriteScaleY(int SpriteNumber);
    int __cdecl SpriteVisible(int SpriteNumber);
    int __cdecl SpriteWidth(int SpriteNumber);
    int __cdecl SpriteX(int SpriteNumber);
    int __cdecl SpriteY(int SpriteNumber);
    void __cdecl Sprite(int SpriteNumber,int Xpos,int Ypos,int ImageNumber);
    void __cdecl StretchSprite(int SpriteNumber,int Xscale,int Yscale);
}

#endif
