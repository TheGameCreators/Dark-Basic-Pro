/*
            File : DBPro/ODE.hpp
       Generated : 2011/09/02 20:15:05
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_ODE_hpp
#define INCLUDE_GUARD_DBPro_ODE_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl OdeAddForce(int a,float b,float c,float d,float e,float f,float g);
    void __cdecl OdeCollisionGetMessage();
    int __cdecl OdeCollisionMessageExists();
    void __cdecl OdeCreateDynamicBox(int a);
    void __cdecl OdeCreateDynamicBox(int a,float b,float c,float d);
    void __cdecl OdeCreateDynamicCylinder(int a);
    void __cdecl OdeCreateDynamicSphere(int a);
    void __cdecl OdeCreateDynamicTriangleMesh(int a);
    void __cdecl OdeCreateStaticBox(int a);
    void __cdecl OdeCreateStaticSphere(int a);
    void __cdecl OdeCreateStaticTriangleMesh(int a);
    void __cdecl OdeCreateStaticUniverse();
    void __cdecl OdeCreateBallJoint(int a,int b,int c,float d,float e,float f);
    void __cdecl OdeCreateHingeJoint(int a,int b,float c,float d,float e,float f,float g,float h);
    void __cdecl OdeCreateHingeJoint(int a,int b,int c,float d,float e,float f,float g,float h,float i);
    void __cdecl OdeCreateRagdollFromBoneModel(int a,float b);
    void __cdecl OdeCreateRagdollFromBoneModel(int a,float b,int c);
    void __cdecl OdeDestroyObject(int a);
    void __cdecl OdeDestroyRagdoll(int a);
    void __cdecl OdeEnd();
    float __cdecl OdeGetBodyAdjustmentX(int a);
    float __cdecl OdeGetBodyAdjustmentY(int a);
    float __cdecl OdeGetBodyAdjustmentZ(int a);
    float __cdecl OdeGetBodyHeight(int a);
    float __cdecl OdeGetBodyLinearVelocityX(int a);
    float __cdecl OdeGetBodyLinearVelocityY(int a);
    float __cdecl OdeGetBodyLinearVelocityZ(int a);
    float __cdecl OdeGetBodyTravelDistanceX(int a);
    float __cdecl OdeGetBodyTravelDistanceY(int a);
    float __cdecl OdeGetBodyTravelDistanceZ(int a);
    float __cdecl OdeGetObjectAAngularVelocityX();
    float __cdecl OdeGetObjectAAngularVelocityY();
    float __cdecl OdeGetObjectAAngularVelocityZ();
    float __cdecl OdeGetObjectAContact();
    float __cdecl OdeGetObjectAVelocityX();
    float __cdecl OdeGetObjectAVelocityY();
    float __cdecl OdeGetObjectAVelocityZ();
    int __cdecl OdeGetObjectA();
    float __cdecl OdeGetObjectBAngularVelocityX();
    float __cdecl OdeGetObjectBAngularVelocityY();
    float __cdecl OdeGetObjectBAngularVelocityZ();
    float __cdecl OdeGetObjectBContact();
    float __cdecl OdeGetObjectBVelocityX();
    float __cdecl OdeGetObjectBVelocityY();
    float __cdecl OdeGetObjectBVelocityZ();
    int __cdecl OdeGetObjectB();
    float __cdecl OdeGetRayCollisionX();
    float __cdecl OdeGetRayCollisionY();
    float __cdecl OdeGetRayCollisionZ();
    float __cdecl OdeGetRayNormalX();
    float __cdecl OdeGetRayNormalY();
    float __cdecl OdeGetRayNormalZ();
    int __cdecl OdeRayCast(int a,float b,float c,float d,float e,float f,float g);
    void __cdecl OdeSetActive(int a,int b);
    void __cdecl OdeSetAngularVelocity(int a,float b,float c,float d);
    void __cdecl OdeSetBodyMass(int a,float b);
    void __cdecl OdeSetBodyRotation(int a,float b,float c,float d);
    void __cdecl OdeSetContactBounceVelocity(int a,float b);
    void __cdecl OdeSetContactBounce(int a,float b);
    void __cdecl OdeSetContactFdir1(int a,float b);
    void __cdecl OdeSetContactMotion1(int a,float b);
    void __cdecl OdeSetContactMotion2(int a,float b);
    void __cdecl OdeSetContactMu2(int a,float b);
    void __cdecl OdeSetContactSlip1(int a,float b);
    void __cdecl OdeSetContactSlip2(int a,float b);
    void __cdecl OdeSetContactSoftCfm(int a,float b);
    void __cdecl OdeSetContactSoftErp(int a,float b);
    void __cdecl OdeSetContactVelocity(int a,float b);
    void __cdecl OdeSetContact(int a,int b,int c);
    void __cdecl OdeSetEnabled(int a,int b);
    void __cdecl OdeSetGravity(int a,int b);
    void __cdecl OdeSetLinearVelocity(int a,float b,float c,float d);
    void __cdecl OdeSetRagdollMode(int a);
    void __cdecl OdeSetRayCast(int a,int b);
    void __cdecl OdeSetResponse(int a,int b);
    void __cdecl OdeSetStepMode(int a);
    void __cdecl OdeSetWorldCfm(float a);
    void __cdecl OdeSetWorldErp(float a);
    void __cdecl OdeSetWorldGravity(float a,float b,float c);
    void __cdecl OdeSetWorldQuickStepNumIterations(int a);
    void __cdecl OdeSetWorldStep(float a);
    void __cdecl OdeSetsurfaceModeContactApprox0(int a,int b);
    void __cdecl OdeSetsurfaceModeContactApprox1(int a,int b);
    void __cdecl OdeSetsurfaceModeContactApprox11(int a,int b);
    void __cdecl OdeSetsurfaceModeContactApprox12(int a,int b);
    void __cdecl OdeSetsurfaceModeContactBounce(int a,int b);
    void __cdecl OdeSetsurfaceModeContactMotion1(int a,int b);
    void __cdecl OdeSetsurfaceModeContactMotion2(int a,int b);
    void __cdecl OdeSetsurfaceModeContactMu2(int a,int b);
    void __cdecl OdeSetsurfaceModeContactSlip1(int a,int b);
    void __cdecl OdeSetsurfaceModeContactSlip2(int a,int b);
    void __cdecl OdeSetsurfaceModeContactSoftCfm(int a,int b);
    void __cdecl OdeSetsurfaceModeContactSoftErp(int a,int b);
    void __cdecl OdeSetsurfaceMode(int a,int b,int c);
    void __cdecl OdeStart();
    void __cdecl OdeSetHingeJointLimit(int a,float b,float c);
    void __cdecl OdeUpdate();
    void __cdecl OdeUpdate(float a);
}

#endif
