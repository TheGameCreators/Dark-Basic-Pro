#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)

#pragma once

#define BUILD_PC
//#define USINGMODULE_R
//#define USINGMODULE_V

#if defined(BUILD_PC)
	#ifndef USINGMODULE_P
	#define USINGMODULE_P
	#endif
	#ifndef USINGMODULE_C
	#define USINGMODULE_C
	#endif
#endif
#if defined(BUILD_P)
	#ifndef USINGMODULE_P
	#define USINGMODULE_P
	#endif
#endif
#if defined(BUILD_C)
	#ifndef USINGMODULE_C
	#define USINGMODULE_C
	#endif
#endif

#ifndef USINGMODULE_P
class Emitter
{
	int i;
};
#endif
#ifndef USINGMODULE_C
class Cloth
{
	int i;
};
#endif
#ifndef USINGMODULE_R
class Ragdoll
{
	int i;
};
#endif
#ifndef USINGMODULE_V
class Vehicle
{
	int i;
};
#endif




#define DEBUG_TEXT_OUTPUT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <assert.h>

//Use the in built memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
#include <crtdbg.h>
#include "..\Common\MemLeakCheck.h"
#include "..\Common\QDebug.h"
#endif

//Temporary debug stuff


#include "D3D_DebugLines.h"

#include "..\Common\SmartPtr.h"
#include "..\Common\SmartLinkList.h"
#include "..\Common\TypeList.h"
#include "..\Common\LinkList.h"
#include "..\Common\LinkListID.h"
#include "..\Common\PointerList.h"
#include "LookupTables.h"
#include "..\Common\PhysStrings.h"
#include "..\Common\Envelope.h"

#include "..\Common\TimerFuncs.h"

#include "..\Common\globstruct.h"
#include "sPositionData.h"
#include "DBPro_Basic3DObj.h"

#if defined(BUILD_PC)
#include "../DBProPhysicsMaster/Resource_PC/Definitions.h"
#elif defined(BUILD_P)
#include "../DBProPhysicsMaster/Resource_P/Definitions.h"
#elif defined(BUILD_C)
#include "../DBProPhysicsMaster/Resource_C/Definitions.h"
#endif

#include "..\Common\PhysErrors.h"
#include "..\Common\Matrix.h"
#include "..\Common\Vector.h"
#include "..\Common\Plane.h"
#include "..\Common\Quaternion.h"
#include "..\Common\BoundingBox.h"
#include "Main.h"

#include "TextureAccess.h"

#include "Plugin_Manager.h"



#include "..\Common\Object_Physics.h"

#ifdef USINGMODULE_P
#include "Emitter/Object_Emitter.h"
#endif

#include "Effector/Object_Effector.h"
#include "Collider/Object_Collider.h"

#ifdef USINGMODULE_C
#include "Cloth/Object_Cloth.h"
#endif

#ifdef USINGMODULE_V
#include "Vehicle/Physics_RigidBody.h"
#include "Vehicle/Object_Vehicle.h"
#include "Vehicle/Suspension.h"
#endif

#ifdef USINGMODULE_R
#include "Ragdoll/Object_Ragdoll.h"
#endif

#include "PhysicsManager.h"

#ifdef USINGMODULE_P
#include "Emitter/Commands_Emitter.h"
#include "Emitter/Emitter_Basic.h"
#include "Emitter/Emitter_Box.h"
#include "Emitter/Emitter_Sphere.h"
#endif

#include "Effector/Commands_Effector.h"
#include "Effector/Effector_Vortex.h"
#include "Effector/Effector_Wind.h"
#include "Effector/Effector_WrapRound.h"
#include "Effector/Effector_Chaos.h"
#include "Effector/Effector_Damping.h"
#include "Effector/Effector_Drag.h"
#include "Effector/Effector_Flock.h"
#include "Effector/Effector_Force.h"
#include "Effector/Effector_Gravity.h"
#include "Effector/Effector_Point.h"
#include "Effector/Effector_Size.h"
#include "Effector/Effector_Color.h"

#include "Collider/Commands_Collider.h"
#include "Collider/Collider_Plane.h"
#include "Collider/Collider_Sphere.h"
#include "Collider/Collider_Box.h"
#include "Collider/Collider_Square.h"
#include "Collider/Collider_Disc.h"

#ifdef USINGMODULE_C
#include "Cloth/Commands_Cloth.h"
#include "Cloth/Cloth_Basic.h"
#endif

#ifdef USINGMODULE_V
#include "Vehicle/Commands_Vehicle.h"
#include "Vehicle/Vehicle_Basic.h"
#endif

#ifdef USINGMODULE_R
#include "Ragdoll/Commands_Ragdoll.h"
#include "Ragdoll/Ragdoll_Basic.h"
#endif