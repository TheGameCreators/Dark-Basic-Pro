/*
            File : DBPro/Particle.hpp
       Generated : 2011/09/02 20:14:50
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Particle_hpp
#define INCLUDE_GUARD_DBPro_Particle_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl ColorParticles(int ParticleNumber,int Red,int Green,int Blue);
    void __cdecl DeleteParticles(int ParticleNumber);
    void __cdecl GhostParticlesOff(int ParticleNumber);
    void __cdecl GhostParticlesOn(int ParticleNumber,int GhostMode);
    void __cdecl GhostParticlesOn(int ParticleNumber,int GhostMode,float Percentage);
    void __cdecl HideParticles(int ParticleNumber);
    void __cdecl MakeFireParticles(int ParticleNumber,int ImageNumber,int Frequency,float X,float Y,float Z,float Width,float Height,float Depth);
    void __cdecl MakeParticles(int ParticleNumber,int ImageNumber,int Frequency,float Radius);
    void __cdecl MakeSnowParticles(int ParticleNumber,int ImageNumber,int Frequency,float X,float Y,float Z,float Width,float Height,float Depth);
    int __cdecl ParticlesExist(int ParticleNumber);
    float __cdecl ParticlesPositionX(int ParticleNumber);
    float __cdecl ParticlesPositionY(int ParticleNumber);
    float __cdecl ParticlesPositionZ(int ParticleNumber);
    void __cdecl PositionParticleEmissions(int ParticleNumber,float X,float Y,float Z);
    void __cdecl PositionParticles(int ParticleNumber,float X,float Y,float Z);
    void __cdecl PositionParticles(int ParticleNumber,int Vector);
    void __cdecl RotateParticles(int ParticleNumber,float X,float Y,float Z);
    void __cdecl RotateParticles(int ParticleNumber,int Vector);
    void __cdecl SetParticleChaos(int ParticleNumber,float ChaosValue);
    void __cdecl SetParticleEmissions(int ParticleNumber,int Frequency);
    void __cdecl SetParticleFloor(int ParticleNumber,int FloorFlag);
    void __cdecl SetParticleGravity(int ParticleNumber,float GravityValue);
    void __cdecl SetParticleLife(int ParticleNumber,int LifePercentage);
    void __cdecl SetParticleSpeed(int ParticleNumber,float SecondsPerFrame);
    void __cdecl SetParticleVelocity(int ParticleNumber,float VelocityValue);
    void __cdecl SetVector3ToParticlesPosition(int Vector,int ParticleNumber);
    void __cdecl SetVector3ToParticlesRotation(int Vector,int ParticleNumber);
    void __cdecl ShowParticles(int ParticleNumber);
}

#endif
