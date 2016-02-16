//*****************************************************************************
//*****************************************************************************
//*** WARNING!! This is *auto-generated source code! DO NOT HAND EDIT!
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma warning(disable:4800)  // Disabling stupid bool conversion warning
#pragma warning(disable:4996)  // Disabling stupid .NET deprecated warnings.
#endif

#include "NXU_schema.h"
#include "NXU_SchemaStream.h"
#include "NXU_customcopy.h"

namespace NXU
{

extern bool gSaveDefaults;
extern bool gSaveCooked;
static bool isSame(const NxMat34 &a,const NxMat34 &b)
{
	bool ret = false;

	float m1[16];
	float m2[16];
	a.getColumnMajor44(m1);
	b.getColumnMajor44(m2);
	if ( memcmp(m1,m2,sizeof(float)*16) == 0 )
		ret = true;

  return ret;
}

static bool isSame(const NxMat33 &a,const NxMat33 &b)
{
	bool ret = false;

	float m1[9];
	float m2[9];
	a.getColumnMajor(m1);
	b.getColumnMajor(m2);
	if ( memcmp(m1,m2,sizeof(float)*9) == 0 )
		ret = true;

  return ret;
}

static bool isSame(const NxQuat &a,const NxQuat &b)
{
	bool ret = false;

	float q1[4];
	float q2[4];

  a.getXYZW(q1);
  b.getXYZW(q2);

  if ( q1[0] == q2[0] &&
  	   q1[1] == q2[1] &&
  	   q1[2] == q2[2] &&
  	   q1[3] == q2[3] )
  {
   	ret = true;
  }
	return ret;
}

static bool isSame(const NxBounds3 &a,const NxBounds3 &b)
{
	bool ret = false;

  if ( a.min == b.min && a.max == b.max )
  	ret = true;

  return ret;
}

#if NX_USE_FLUID_API
static bool isSame(const NxPlane &a,const NxPlane &b)
{
	bool ret = false;

  if ( a.normal == b.normal && a.d == b.d )
  	ret = true;

  return ret;
}
#endif 

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER >=270
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >=280
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER == 250 || NX_SDK_VERSION_NUMBER == 251
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
const char * EnumToString(NxParameter v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_PENALTY_FORCE: ret = "NX_PENALTY_FORCE"; break;
    case NX_SKIN_WIDTH: ret = "NX_SKIN_WIDTH"; break;
    case NX_DEFAULT_SLEEP_LIN_VEL_SQUARED: ret = "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED"; break;
    case NX_DEFAULT_SLEEP_ANG_VEL_SQUARED: ret = "NX_DEFAULT_SLEEP_ANG_VEL_SQUARED"; break;
    case NX_BOUNCE_THRESHOLD: ret = "NX_BOUNCE_THRESHOLD"; break;
    case NX_DYN_FRICT_SCALING: ret = "NX_DYN_FRICT_SCALING"; break;
    case NX_STA_FRICT_SCALING: ret = "NX_STA_FRICT_SCALING"; break;
    case NX_MAX_ANGULAR_VELOCITY: ret = "NX_MAX_ANGULAR_VELOCITY"; break;
    case NX_CONTINUOUS_CD: ret = "NX_CONTINUOUS_CD"; break;
    case NX_VISUALIZATION_SCALE: ret = "NX_VISUALIZATION_SCALE"; break;
    case NX_VISUALIZE_WORLD_AXES: ret = "NX_VISUALIZE_WORLD_AXES"; break;
    case NX_VISUALIZE_BODY_AXES: ret = "NX_VISUALIZE_BODY_AXES"; break;
    case NX_VISUALIZE_BODY_MASS_AXES: ret = "NX_VISUALIZE_BODY_MASS_AXES"; break;
    case NX_VISUALIZE_BODY_LIN_VELOCITY: ret = "NX_VISUALIZE_BODY_LIN_VELOCITY"; break;
    case NX_VISUALIZE_BODY_ANG_VELOCITY: ret = "NX_VISUALIZE_BODY_ANG_VELOCITY"; break;
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_LIN_MOMENTUM: ret = "NX_VISUALIZE_BODY_LIN_MOMENTUM"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_ANG_MOMENTUM: ret = "NX_VISUALIZE_BODY_ANG_MOMENTUM"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_LIN_ACCEL: ret = "NX_VISUALIZE_BODY_LIN_ACCEL"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_ANG_ACCEL: ret = "NX_VISUALIZE_BODY_ANG_ACCEL"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_LIN_FORCE: ret = "NX_VISUALIZE_BODY_LIN_FORCE"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_ANG_FORCE: ret = "NX_VISUALIZE_BODY_ANG_FORCE"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_REDUCED: ret = "NX_VISUALIZE_BODY_REDUCED"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY15: ret = "NX_DUMMY15"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY16: ret = "NX_DUMMY16"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY17: ret = "NX_DUMMY17"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY18: ret = "NX_DUMMY18"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY19: ret = "NX_DUMMY19"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY20: ret = "NX_DUMMY20"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY21: ret = "NX_DUMMY21"; break;
#endif
    case NX_VISUALIZE_BODY_JOINT_GROUPS: ret = "NX_VISUALIZE_BODY_JOINT_GROUPS"; break;
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_CONTACT_LIST: ret = "NX_VISUALIZE_BODY_CONTACT_LIST"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_JOINT_LIST: ret = "NX_VISUALIZE_BODY_JOINT_LIST"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_DAMPING: ret = "NX_VISUALIZE_BODY_DAMPING"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_BODY_SLEEP: ret = "NX_VISUALIZE_BODY_SLEEP"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY23: ret = "NX_DUMMY23"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY24: ret = "NX_DUMMY24"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY25: ret = "NX_DUMMY25"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY26: ret = "NX_DUMMY26"; break;
#endif
    case NX_VISUALIZE_JOINT_LOCAL_AXES: ret = "NX_VISUALIZE_JOINT_LOCAL_AXES"; break;
    case NX_VISUALIZE_JOINT_WORLD_AXES: ret = "NX_VISUALIZE_JOINT_WORLD_AXES"; break;
    case NX_VISUALIZE_JOINT_LIMITS: ret = "NX_VISUALIZE_JOINT_LIMITS"; break;
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_JOINT_ERROR: ret = "NX_VISUALIZE_JOINT_ERROR"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_JOINT_FORCE: ret = "NX_VISUALIZE_JOINT_FORCE"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
    case NX_VISUALIZE_JOINT_REDUCED: ret = "NX_VISUALIZE_JOINT_REDUCED"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY30: ret = "NX_DUMMY30"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY31: ret = "NX_DUMMY31"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=270
    case NX_DUMMY32: ret = "NX_DUMMY32"; break;
#endif
    case NX_VISUALIZE_CONTACT_POINT: ret = "NX_VISUALIZE_CONTACT_POINT"; break;
    case NX_VISUALIZE_CONTACT_NORMAL: ret = "NX_VISUALIZE_CONTACT_NORMAL"; break;
    case NX_VISUALIZE_CONTACT_ERROR: ret = "NX_VISUALIZE_CONTACT_ERROR"; break;
    case NX_VISUALIZE_CONTACT_FORCE: ret = "NX_VISUALIZE_CONTACT_FORCE"; break;
    case NX_VISUALIZE_ACTOR_AXES: ret = "NX_VISUALIZE_ACTOR_AXES"; break;
    case NX_VISUALIZE_COLLISION_AABBS: ret = "NX_VISUALIZE_COLLISION_AABBS"; break;
    case NX_VISUALIZE_COLLISION_SHAPES: ret = "NX_VISUALIZE_COLLISION_SHAPES"; break;
    case NX_VISUALIZE_COLLISION_AXES: ret = "NX_VISUALIZE_COLLISION_AXES"; break;
    case NX_VISUALIZE_COLLISION_COMPOUNDS: ret = "NX_VISUALIZE_COLLISION_COMPOUNDS"; break;
    case NX_VISUALIZE_COLLISION_VNORMALS: ret = "NX_VISUALIZE_COLLISION_VNORMALS"; break;
    case NX_VISUALIZE_COLLISION_FNORMALS: ret = "NX_VISUALIZE_COLLISION_FNORMALS"; break;
    case NX_VISUALIZE_COLLISION_EDGES: ret = "NX_VISUALIZE_COLLISION_EDGES"; break;
    case NX_VISUALIZE_COLLISION_SPHERES: ret = "NX_VISUALIZE_COLLISION_SPHERES"; break;
#if NX_SDK_VERSION_NUMBER < 280
    case NX_VISUALIZE_COLLISION_SAP: ret = "NX_VISUALIZE_COLLISION_SAP"; break;
#endif
#if NX_SDK_VERSION_NUMBER >=280
    case NX_DUMMY46: ret = "NX_DUMMY46"; break;
#endif
    case NX_VISUALIZE_COLLISION_STATIC: ret = "NX_VISUALIZE_COLLISION_STATIC"; break;
    case NX_VISUALIZE_COLLISION_DYNAMIC: ret = "NX_VISUALIZE_COLLISION_DYNAMIC"; break;
    case NX_VISUALIZE_COLLISION_FREE: ret = "NX_VISUALIZE_COLLISION_FREE"; break;
    case NX_VISUALIZE_COLLISION_CCD: ret = "NX_VISUALIZE_COLLISION_CCD"; break;
    case NX_VISUALIZE_COLLISION_SKELETONS: ret = "NX_VISUALIZE_COLLISION_SKELETONS"; break;
    case NX_VISUALIZE_FLUID_EMITTERS: ret = "NX_VISUALIZE_FLUID_EMITTERS"; break;
    case NX_VISUALIZE_FLUID_POSITION: ret = "NX_VISUALIZE_FLUID_POSITION"; break;
    case NX_VISUALIZE_FLUID_VELOCITY: ret = "NX_VISUALIZE_FLUID_VELOCITY"; break;
    case NX_VISUALIZE_FLUID_KERNEL_RADIUS: ret = "NX_VISUALIZE_FLUID_KERNEL_RADIUS"; break;
    case NX_VISUALIZE_FLUID_BOUNDS: ret = "NX_VISUALIZE_FLUID_BOUNDS"; break;
    case NX_VISUALIZE_FLUID_PACKETS: ret = "NX_VISUALIZE_FLUID_PACKETS"; break;
    case NX_VISUALIZE_FLUID_MOTION_LIMIT: ret = "NX_VISUALIZE_FLUID_MOTION_LIMIT"; break;
    case NX_VISUALIZE_FLUID_DYN_COLLISION: ret = "NX_VISUALIZE_FLUID_DYN_COLLISION"; break;
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_VISUALIZE_FLUID_STC_COLLISION: ret = "NX_VISUALIZE_FLUID_STC_COLLISION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_VISUALIZE_FLUID_MESH_PACKETS: ret = "NX_VISUALIZE_FLUID_MESH_PACKETS"; break;
#endif
    case NX_VISUALIZE_FLUID_DRAINS: ret = "NX_VISUALIZE_FLUID_DRAINS"; break;
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_VISUALIZE_CLOTH_MESH: ret = "NX_VISUALIZE_CLOTH_MESH"; break;
#endif
    case NX_VISUALIZE_CLOTH_COLLISIONS: ret = "NX_VISUALIZE_CLOTH_COLLISIONS"; break;
    case NX_VISUALIZE_CLOTH_SELFCOLLISIONS: ret = "NX_VISUALIZE_CLOTH_SELFCOLLISIONS"; break;
    case NX_VISUALIZE_CLOTH_WORKPACKETS: ret = "NX_VISUALIZE_CLOTH_WORKPACKETS"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_VISUALIZE_CLOTH_SLEEP: ret = "NX_VISUALIZE_CLOTH_SLEEP"; break;
#endif
    case NX_ADAPTIVE_FORCE: ret = "NX_ADAPTIVE_FORCE"; break;
    case NX_COLL_VETO_JOINTED: ret = "NX_COLL_VETO_JOINTED"; break;
    case NX_TRIGGER_TRIGGER_CALLBACK: ret = "NX_TRIGGER_TRIGGER_CALLBACK"; break;
    case NX_SELECT_HW_ALGO: ret = "NX_SELECT_HW_ALGO"; break;
    case NX_VISUALIZE_ACTIVE_VERTICES: ret = "NX_VISUALIZE_ACTIVE_VERTICES"; break;
    case NX_CCD_EPSILON: ret = "NX_CCD_EPSILON"; break;
    case NX_SOLVER_CONVERGENCE_THRESHOLD: ret = "NX_SOLVER_CONVERGENCE_THRESHOLD"; break;
    case NX_BBOX_NOISE_LEVEL: ret = "NX_BBOX_NOISE_LEVEL"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_IMPLICIT_SWEEP_CACHE_SIZE: ret = "NX_IMPLICIT_SWEEP_CACHE_SIZE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_DEFAULT_SLEEP_ENERGY: ret = "NX_DEFAULT_SLEEP_ENERGY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_CONSTANT_FLUID_MAX_PACKETS: ret = "NX_CONSTANT_FLUID_MAX_PACKETS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP: ret = "NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP"; break;
#endif
#if NX_SDK_VERSION_NUMBER == 250 || NX_SDK_VERSION_NUMBER == 251
    case NX_GRID_HASH_CELL_SIZE: ret = "NX_GRID_HASH_CELL_SIZE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_CLOTH_TEARABLE_VERTICES: ret = "NX_VISUALIZE_CLOTH_TEARABLE_VERTICES"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_CLOTH_TEARING: ret = "NX_VISUALIZE_CLOTH_TEARING"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_CLOTH_ATTACHMENT: ret = "NX_VISUALIZE_CLOTH_ATTACHMENT"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_MESH: ret = "NX_VISUALIZE_SOFTBODY_MESH"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_COLLISIONS: ret = "NX_VISUALIZE_SOFTBODY_COLLISIONS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_WORKPACKETS: ret = "NX_VISUALIZE_SOFTBODY_WORKPACKETS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_SLEEP: ret = "NX_VISUALIZE_SOFTBODY_SLEEP"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES: ret = "NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_TEARING: ret = "NX_VISUALIZE_SOFTBODY_TEARING"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_SOFTBODY_ATTACHMENT: ret = "NX_VISUALIZE_SOFTBODY_ATTACHMENT"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_VISUALIZE_FLUID_PACKET_DATA: ret = "NX_VISUALIZE_FLUID_PACKET_DATA"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_VISUALIZE_FORCE_FIELDS: ret = "NX_VISUALIZE_FORCE_FIELDS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_VISUALIZE_CLOTH_VALIDBOUNDS: ret = "NX_VISUALIZE_CLOTH_VALIDBOUNDS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_VISUALIZE_SOFTBODY_VALIDBOUNDS: ret = "NX_VISUALIZE_SOFTBODY_VALIDBOUNDS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_VISUALIZE_CLOTH_SLEEP_VERTEX: ret = "NX_VISUALIZE_CLOTH_SLEEP_VERTEX"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX: ret = "NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_ASYNCHRONOUS_MESH_CREATION: ret = "NX_ASYNCHRONOUS_MESH_CREATION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON: ret = "NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
    case NX_IMPROVED_SPRING_SOLVER: ret = "NX_IMPROVED_SPRING_SOLVER"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
    case NX_FAST_MASSIVE_BP_VOLUME_DELETION: ret = "NX_FAST_MASSIVE_BP_VOLUME_DELETION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
    case NX_LEGACY_JOINT_DRIVE: ret = "NX_LEGACY_JOINT_DRIVE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_VISUALIZE_CLOTH_HIERARCHY: ret = "NX_VISUALIZE_CLOTH_HIERARCHY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_VISUALIZE_CLOTH_HARD_STRETCHING_LIMITATION: ret = "NX_VISUALIZE_CLOTH_HARD_STRETCHING_LIMITATION"; break;
#endif
    case NX_PARAMS_NUM_VALUES: ret = "NX_PARAMS_NUM_VALUES"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxParameter &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_PENALTY_FORCE") == 0 ) { v = NX_PENALTY_FORCE; ret = true; }
  else if ( strcasecmp(str,"NX_SKIN_WIDTH") == 0 ) { v = NX_SKIN_WIDTH; ret = true; }
  else if ( strcasecmp(str,"NX_DEFAULT_SLEEP_LIN_VEL_SQUARED") == 0 ) { v = NX_DEFAULT_SLEEP_LIN_VEL_SQUARED; ret = true; }
  else if ( strcasecmp(str,"NX_DEFAULT_SLEEP_ANG_VEL_SQUARED") == 0 ) { v = NX_DEFAULT_SLEEP_ANG_VEL_SQUARED; ret = true; }
  else if ( strcasecmp(str,"NX_BOUNCE_THRESHOLD") == 0 ) { v = NX_BOUNCE_THRESHOLD; ret = true; }
  else if ( strcasecmp(str,"NX_DYN_FRICT_SCALING") == 0 ) { v = NX_DYN_FRICT_SCALING; ret = true; }
  else if ( strcasecmp(str,"NX_STA_FRICT_SCALING") == 0 ) { v = NX_STA_FRICT_SCALING; ret = true; }
  else if ( strcasecmp(str,"NX_MAX_ANGULAR_VELOCITY") == 0 ) { v = NX_MAX_ANGULAR_VELOCITY; ret = true; }
  else if ( strcasecmp(str,"NX_CONTINUOUS_CD") == 0 ) { v = NX_CONTINUOUS_CD; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZATION_SCALE") == 0 ) { v = NX_VISUALIZATION_SCALE; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_WORLD_AXES") == 0 ) { v = NX_VISUALIZE_WORLD_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_AXES") == 0 ) { v = NX_VISUALIZE_BODY_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_MASS_AXES") == 0 ) { v = NX_VISUALIZE_BODY_MASS_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_LIN_VELOCITY") == 0 ) { v = NX_VISUALIZE_BODY_LIN_VELOCITY; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_ANG_VELOCITY") == 0 ) { v = NX_VISUALIZE_BODY_ANG_VELOCITY; ret = true; }
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_LIN_MOMENTUM") == 0 ) { v = NX_VISUALIZE_BODY_LIN_MOMENTUM; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_ANG_MOMENTUM") == 0 ) { v = NX_VISUALIZE_BODY_ANG_MOMENTUM; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_LIN_ACCEL") == 0 ) { v = NX_VISUALIZE_BODY_LIN_ACCEL; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_ANG_ACCEL") == 0 ) { v = NX_VISUALIZE_BODY_ANG_ACCEL; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_LIN_FORCE") == 0 ) { v = NX_VISUALIZE_BODY_LIN_FORCE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_ANG_FORCE") == 0 ) { v = NX_VISUALIZE_BODY_ANG_FORCE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_REDUCED") == 0 ) { v = NX_VISUALIZE_BODY_REDUCED; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY15") == 0 ) { v = NX_DUMMY15; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY16") == 0 ) { v = NX_DUMMY16; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY17") == 0 ) { v = NX_DUMMY17; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY18") == 0 ) { v = NX_DUMMY18; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY19") == 0 ) { v = NX_DUMMY19; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY20") == 0 ) { v = NX_DUMMY20; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY21") == 0 ) { v = NX_DUMMY21; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_JOINT_GROUPS") == 0 ) { v = NX_VISUALIZE_BODY_JOINT_GROUPS; ret = true; }
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_CONTACT_LIST") == 0 ) { v = NX_VISUALIZE_BODY_CONTACT_LIST; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_JOINT_LIST") == 0 ) { v = NX_VISUALIZE_BODY_JOINT_LIST; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_DAMPING") == 0 ) { v = NX_VISUALIZE_BODY_DAMPING; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_BODY_SLEEP") == 0 ) { v = NX_VISUALIZE_BODY_SLEEP; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY23") == 0 ) { v = NX_DUMMY23; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY24") == 0 ) { v = NX_DUMMY24; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY25") == 0 ) { v = NX_DUMMY25; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY26") == 0 ) { v = NX_DUMMY26; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_LOCAL_AXES") == 0 ) { v = NX_VISUALIZE_JOINT_LOCAL_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_WORLD_AXES") == 0 ) { v = NX_VISUALIZE_JOINT_WORLD_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_LIMITS") == 0 ) { v = NX_VISUALIZE_JOINT_LIMITS; ret = true; }
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_ERROR") == 0 ) { v = NX_VISUALIZE_JOINT_ERROR; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_FORCE") == 0 ) { v = NX_VISUALIZE_JOINT_FORCE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  else if ( strcasecmp(str,"NX_VISUALIZE_JOINT_REDUCED") == 0 ) { v = NX_VISUALIZE_JOINT_REDUCED; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY30") == 0 ) { v = NX_DUMMY30; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY31") == 0 ) { v = NX_DUMMY31; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=270
  else if ( strcasecmp(str,"NX_DUMMY32") == 0 ) { v = NX_DUMMY32; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_CONTACT_POINT") == 0 ) { v = NX_VISUALIZE_CONTACT_POINT; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_CONTACT_NORMAL") == 0 ) { v = NX_VISUALIZE_CONTACT_NORMAL; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_CONTACT_ERROR") == 0 ) { v = NX_VISUALIZE_CONTACT_ERROR; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_CONTACT_FORCE") == 0 ) { v = NX_VISUALIZE_CONTACT_FORCE; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_ACTOR_AXES") == 0 ) { v = NX_VISUALIZE_ACTOR_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_AABBS") == 0 ) { v = NX_VISUALIZE_COLLISION_AABBS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_SHAPES") == 0 ) { v = NX_VISUALIZE_COLLISION_SHAPES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_AXES") == 0 ) { v = NX_VISUALIZE_COLLISION_AXES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_COMPOUNDS") == 0 ) { v = NX_VISUALIZE_COLLISION_COMPOUNDS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_VNORMALS") == 0 ) { v = NX_VISUALIZE_COLLISION_VNORMALS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_FNORMALS") == 0 ) { v = NX_VISUALIZE_COLLISION_FNORMALS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_EDGES") == 0 ) { v = NX_VISUALIZE_COLLISION_EDGES; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_SPHERES") == 0 ) { v = NX_VISUALIZE_COLLISION_SPHERES; ret = true; }
#if NX_SDK_VERSION_NUMBER < 280
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_SAP") == 0 ) { v = NX_VISUALIZE_COLLISION_SAP; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >=280
  else if ( strcasecmp(str,"NX_DUMMY46") == 0 ) { v = NX_DUMMY46; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_STATIC") == 0 ) { v = NX_VISUALIZE_COLLISION_STATIC; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_DYNAMIC") == 0 ) { v = NX_VISUALIZE_COLLISION_DYNAMIC; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_FREE") == 0 ) { v = NX_VISUALIZE_COLLISION_FREE; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_CCD") == 0 ) { v = NX_VISUALIZE_COLLISION_CCD; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_COLLISION_SKELETONS") == 0 ) { v = NX_VISUALIZE_COLLISION_SKELETONS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_EMITTERS") == 0 ) { v = NX_VISUALIZE_FLUID_EMITTERS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_POSITION") == 0 ) { v = NX_VISUALIZE_FLUID_POSITION; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_VELOCITY") == 0 ) { v = NX_VISUALIZE_FLUID_VELOCITY; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_KERNEL_RADIUS") == 0 ) { v = NX_VISUALIZE_FLUID_KERNEL_RADIUS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_BOUNDS") == 0 ) { v = NX_VISUALIZE_FLUID_BOUNDS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_PACKETS") == 0 ) { v = NX_VISUALIZE_FLUID_PACKETS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_MOTION_LIMIT") == 0 ) { v = NX_VISUALIZE_FLUID_MOTION_LIMIT; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_DYN_COLLISION") == 0 ) { v = NX_VISUALIZE_FLUID_DYN_COLLISION; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_STC_COLLISION") == 0 ) { v = NX_VISUALIZE_FLUID_STC_COLLISION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_MESH_PACKETS") == 0 ) { v = NX_VISUALIZE_FLUID_MESH_PACKETS; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_DRAINS") == 0 ) { v = NX_VISUALIZE_FLUID_DRAINS; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_MESH") == 0 ) { v = NX_VISUALIZE_CLOTH_MESH; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_COLLISIONS") == 0 ) { v = NX_VISUALIZE_CLOTH_COLLISIONS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_SELFCOLLISIONS") == 0 ) { v = NX_VISUALIZE_CLOTH_SELFCOLLISIONS; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_WORKPACKETS") == 0 ) { v = NX_VISUALIZE_CLOTH_WORKPACKETS; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_SLEEP") == 0 ) { v = NX_VISUALIZE_CLOTH_SLEEP; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_ADAPTIVE_FORCE") == 0 ) { v = NX_ADAPTIVE_FORCE; ret = true; }
  else if ( strcasecmp(str,"NX_COLL_VETO_JOINTED") == 0 ) { v = NX_COLL_VETO_JOINTED; ret = true; }
  else if ( strcasecmp(str,"NX_TRIGGER_TRIGGER_CALLBACK") == 0 ) { v = NX_TRIGGER_TRIGGER_CALLBACK; ret = true; }
  else if ( strcasecmp(str,"NX_SELECT_HW_ALGO") == 0 ) { v = NX_SELECT_HW_ALGO; ret = true; }
  else if ( strcasecmp(str,"NX_VISUALIZE_ACTIVE_VERTICES") == 0 ) { v = NX_VISUALIZE_ACTIVE_VERTICES; ret = true; }
  else if ( strcasecmp(str,"NX_CCD_EPSILON") == 0 ) { v = NX_CCD_EPSILON; ret = true; }
  else if ( strcasecmp(str,"NX_SOLVER_CONVERGENCE_THRESHOLD") == 0 ) { v = NX_SOLVER_CONVERGENCE_THRESHOLD; ret = true; }
  else if ( strcasecmp(str,"NX_BBOX_NOISE_LEVEL") == 0 ) { v = NX_BBOX_NOISE_LEVEL; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_IMPLICIT_SWEEP_CACHE_SIZE") == 0 ) { v = NX_IMPLICIT_SWEEP_CACHE_SIZE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_DEFAULT_SLEEP_ENERGY") == 0 ) { v = NX_DEFAULT_SLEEP_ENERGY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_CONSTANT_FLUID_MAX_PACKETS") == 0 ) { v = NX_CONSTANT_FLUID_MAX_PACKETS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP") == 0 ) { v = NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER == 250 || NX_SDK_VERSION_NUMBER == 251
  else if ( strcasecmp(str,"NX_GRID_HASH_CELL_SIZE") == 0 ) { v = NX_GRID_HASH_CELL_SIZE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_TEARABLE_VERTICES") == 0 ) { v = NX_VISUALIZE_CLOTH_TEARABLE_VERTICES; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_TEARING") == 0 ) { v = NX_VISUALIZE_CLOTH_TEARING; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_ATTACHMENT") == 0 ) { v = NX_VISUALIZE_CLOTH_ATTACHMENT; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_MESH") == 0 ) { v = NX_VISUALIZE_SOFTBODY_MESH; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_COLLISIONS") == 0 ) { v = NX_VISUALIZE_SOFTBODY_COLLISIONS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_WORKPACKETS") == 0 ) { v = NX_VISUALIZE_SOFTBODY_WORKPACKETS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_SLEEP") == 0 ) { v = NX_VISUALIZE_SOFTBODY_SLEEP; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES") == 0 ) { v = NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_TEARING") == 0 ) { v = NX_VISUALIZE_SOFTBODY_TEARING; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_ATTACHMENT") == 0 ) { v = NX_VISUALIZE_SOFTBODY_ATTACHMENT; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_VISUALIZE_FLUID_PACKET_DATA") == 0 ) { v = NX_VISUALIZE_FLUID_PACKET_DATA; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_VISUALIZE_FORCE_FIELDS") == 0 ) { v = NX_VISUALIZE_FORCE_FIELDS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_VALIDBOUNDS") == 0 ) { v = NX_VISUALIZE_CLOTH_VALIDBOUNDS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_VALIDBOUNDS") == 0 ) { v = NX_VISUALIZE_SOFTBODY_VALIDBOUNDS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_SLEEP_VERTEX") == 0 ) { v = NX_VISUALIZE_CLOTH_SLEEP_VERTEX; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX") == 0 ) { v = NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_ASYNCHRONOUS_MESH_CREATION") == 0 ) { v = NX_ASYNCHRONOUS_MESH_CREATION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON") == 0 ) { v = NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  else if ( strcasecmp(str,"NX_IMPROVED_SPRING_SOLVER") == 0 ) { v = NX_IMPROVED_SPRING_SOLVER; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  else if ( strcasecmp(str,"NX_FAST_MASSIVE_BP_VOLUME_DELETION") == 0 ) { v = NX_FAST_MASSIVE_BP_VOLUME_DELETION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  else if ( strcasecmp(str,"NX_LEGACY_JOINT_DRIVE") == 0 ) { v = NX_LEGACY_JOINT_DRIVE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_HIERARCHY") == 0 ) { v = NX_VISUALIZE_CLOTH_HIERARCHY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_VISUALIZE_CLOTH_HARD_STRETCHING_LIMITATION") == 0 ) { v = NX_VISUALIZE_CLOTH_HARD_STRETCHING_LIMITATION; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_PARAMS_NUM_VALUES") == 0 ) { v = NX_PARAMS_NUM_VALUES; ret = true; }
  }
  return ret;
}
#if NX_SDK_VERSION_NUMBER >= 260

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxCompartmentType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SCT_RIGIDBODY: ret = "NX_SCT_RIGIDBODY"; break;
    case NX_SCT_FLUID: ret = "NX_SCT_FLUID"; break;
    case NX_SCT_CLOTH: ret = "NX_SCT_CLOTH"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxCompartmentType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SCT_RIGIDBODY") == 0 ) { v = NX_SCT_RIGIDBODY; ret = true; }
  else if ( strcasecmp(str,"NX_SCT_FLUID") == 0 ) { v = NX_SCT_FLUID; ret = true; }
  else if ( strcasecmp(str,"NX_SCT_CLOTH") == 0 ) { v = NX_SCT_CLOTH; ret = true; }
  }
  return ret;
}
#endif
#if NX_SDK_VERSION_NUMBER >= 262

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxPruningStructure v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_PRUNING_NONE: ret = "NX_PRUNING_NONE"; break;
    case NX_PRUNING_OCTREE: ret = "NX_PRUNING_OCTREE"; break;
    case NX_PRUNING_QUADTREE: ret = "NX_PRUNING_QUADTREE"; break;
    case NX_PRUNING_DYNAMIC_AABB_TREE: ret = "NX_PRUNING_DYNAMIC_AABB_TREE"; break;
    case NX_PRUNING_STATIC_AABB_TREE: ret = "NX_PRUNING_STATIC_AABB_TREE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxPruningStructure &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_PRUNING_NONE") == 0 ) { v = NX_PRUNING_NONE; ret = true; }
  else if ( strcasecmp(str,"NX_PRUNING_OCTREE") == 0 ) { v = NX_PRUNING_OCTREE; ret = true; }
  else if ( strcasecmp(str,"NX_PRUNING_QUADTREE") == 0 ) { v = NX_PRUNING_QUADTREE; ret = true; }
  else if ( strcasecmp(str,"NX_PRUNING_DYNAMIC_AABB_TREE") == 0 ) { v = NX_PRUNING_DYNAMIC_AABB_TREE; ret = true; }
  else if ( strcasecmp(str,"NX_PRUNING_STATIC_AABB_TREE") == 0 ) { v = NX_PRUNING_STATIC_AABB_TREE; ret = true; }
  }
  return ret;
}
#endif
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxBroadPhaseType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_BP_TYPE_SAP_SINGLE: ret = "NX_BP_TYPE_SAP_SINGLE"; break;
    case NX_BP_TYPE_SAP_MULTI: ret = "NX_BP_TYPE_SAP_MULTI"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxBroadPhaseType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_BP_TYPE_SAP_SINGLE") == 0 ) { v = NX_BP_TYPE_SAP_SINGLE; ret = true; }
  else if ( strcasecmp(str,"NX_BP_TYPE_SAP_MULTI") == 0 ) { v = NX_BP_TYPE_SAP_MULTI; ret = true; }
  }
  return ret;
}
#endif
#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 271
#endif
#if NX_SDK_VERSION_NUMBER >= 271
#endif
#if NX_SDK_VERSION_NUMBER >= 271
#endif
const char * EnumToString(NxCompartmentFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CF_SLEEP_NOTIFICATION: ret = "NX_CF_SLEEP_NOTIFICATION"; break;
#if NX_SDK_VERSION_NUMBER >= 271
    case NX_CF_CONTINUOUS_CD: ret = "NX_CF_CONTINUOUS_CD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
    case NX_CF_RESTRICTED_SCENE: ret = "NX_CF_RESTRICTED_SCENE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
    case NX_CF_INHERIT_SETTINGS: ret = "NX_CF_INHERIT_SETTINGS"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxCompartmentFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CF_SLEEP_NOTIFICATION") == 0 ) { v = NX_CF_SLEEP_NOTIFICATION; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 271
  else if ( strcasecmp(str,"NX_CF_CONTINUOUS_CD") == 0 ) { v = NX_CF_CONTINUOUS_CD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  else if ( strcasecmp(str,"NX_CF_RESTRICTED_SCENE") == 0 ) { v = NX_CF_RESTRICTED_SCENE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  else if ( strcasecmp(str,"NX_CF_INHERIT_SETTINGS") == 0 ) { v = NX_CF_INHERIT_SETTINGS; ret = true; }
#endif
  return ret;
}
#endif
#if NX_SDK_VERSION_NUMBER >= 260

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
const char * EnumToString(NxConvexFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CF_FLIPNORMALS: ret = "NX_CF_FLIPNORMALS"; break;
    case NX_CF_16_BIT_INDICES: ret = "NX_CF_16_BIT_INDICES"; break;
    case NX_CF_COMPUTE_CONVEX: ret = "NX_CF_COMPUTE_CONVEX"; break;
    case NX_CF_INFLATE_CONVEX: ret = "NX_CF_INFLATE_CONVEX"; break;
#if NX_SDK_VERSION_NUMBER < 280
    case NX_CF_USE_LEGACY_COOKER: ret = "NX_CF_USE_LEGACY_COOKER"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_CF_DUMMY4: ret = "NX_CF_DUMMY4"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_CF_USE_UNCOMPRESSED_NORMALS: ret = "NX_CF_USE_UNCOMPRESSED_NORMALS"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxConvexFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CF_FLIPNORMALS") == 0 ) { v = NX_CF_FLIPNORMALS; ret = true; }
  else if ( strcasecmp(str,"NX_CF_16_BIT_INDICES") == 0 ) { v = NX_CF_16_BIT_INDICES; ret = true; }
  else if ( strcasecmp(str,"NX_CF_COMPUTE_CONVEX") == 0 ) { v = NX_CF_COMPUTE_CONVEX; ret = true; }
  else if ( strcasecmp(str,"NX_CF_INFLATE_CONVEX") == 0 ) { v = NX_CF_INFLATE_CONVEX; ret = true; }
#if NX_SDK_VERSION_NUMBER < 280
  else if ( strcasecmp(str,"NX_CF_USE_LEGACY_COOKER") == 0 ) { v = NX_CF_USE_LEGACY_COOKER; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_CF_DUMMY4") == 0 ) { v = NX_CF_DUMMY4; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_CF_USE_UNCOMPRESSED_NORMALS") == 0 ) { v = NX_CF_USE_UNCOMPRESSED_NORMALS; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxHeightFieldAxis v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_X: ret = "NX_X"; break;
    case NX_Y: ret = "NX_Y"; break;
    case NX_Z: ret = "NX_Z"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxHeightFieldAxis &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_X") == 0 ) { v = NX_X; ret = true; }
  else if ( strcasecmp(str,"NX_Y") == 0 ) { v = NX_Y; ret = true; }
  else if ( strcasecmp(str,"NX_Z") == 0 ) { v = NX_Z; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxMeshFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_MF_FLIPNORMALS: ret = "NX_MF_FLIPNORMALS"; break;
    case NX_MF_16_BIT_INDICES: ret = "NX_MF_16_BIT_INDICES"; break;
    case NX_MF_HARDWARE_MESH: ret = "NX_MF_HARDWARE_MESH"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxMeshFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_MF_FLIPNORMALS") == 0 ) { v = NX_MF_FLIPNORMALS; ret = true; }
  else if ( strcasecmp(str,"NX_MF_16_BIT_INDICES") == 0 ) { v = NX_MF_16_BIT_INDICES; ret = true; }
  else if ( strcasecmp(str,"NX_MF_HARDWARE_MESH") == 0 ) { v = NX_MF_HARDWARE_MESH; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxHeightFieldFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_HF_NO_BOUNDARY_EDGES: ret = "NX_HF_NO_BOUNDARY_EDGES"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxHeightFieldFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_HF_NO_BOUNDARY_EDGES") == 0 ) { v = NX_HF_NO_BOUNDARY_EDGES; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 280
#endif
const char * EnumToString(NxClothMeshFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CLOTH_MESH_DUMMY0: ret = "NX_CLOTH_MESH_DUMMY0"; break;
    case NX_CLOTH_MESH_DUMMY1: ret = "NX_CLOTH_MESH_DUMMY1"; break;
    case NX_CLOTH_MESH_DUMMY2: ret = "NX_CLOTH_MESH_DUMMY2"; break;
    case NX_CLOTH_MESH_DUMMY3: ret = "NX_CLOTH_MESH_DUMMY3"; break;
    case NX_CLOTH_MESH_DUMMY4: ret = "NX_CLOTH_MESH_DUMMY4"; break;
    case NX_CLOTH_MESH_DUMMY5: ret = "NX_CLOTH_MESH_DUMMY5"; break;
    case NX_CLOTH_MESH_DUMMY6: ret = "NX_CLOTH_MESH_DUMMY6"; break;
    case NX_CLOTH_MESH_DUMMY7: ret = "NX_CLOTH_MESH_DUMMY7"; break;
    case NX_CLOTH_MESH_TEARABLE: ret = "NX_CLOTH_MESH_TEARABLE"; break;
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_CLOTH_MESH_WELD_VERTICES: ret = "NX_CLOTH_MESH_WELD_VERTICES"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxClothMeshFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY0") == 0 ) { v = NX_CLOTH_MESH_DUMMY0; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY1") == 0 ) { v = NX_CLOTH_MESH_DUMMY1; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY2") == 0 ) { v = NX_CLOTH_MESH_DUMMY2; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY3") == 0 ) { v = NX_CLOTH_MESH_DUMMY3; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY4") == 0 ) { v = NX_CLOTH_MESH_DUMMY4; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY5") == 0 ) { v = NX_CLOTH_MESH_DUMMY5; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY6") == 0 ) { v = NX_CLOTH_MESH_DUMMY6; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_DUMMY7") == 0 ) { v = NX_CLOTH_MESH_DUMMY7; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_MESH_TEARABLE") == 0 ) { v = NX_CLOTH_MESH_TEARABLE; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_CLOTH_MESH_WELD_VERTICES") == 0 ) { v = NX_CLOTH_MESH_WELD_VERTICES; ret = true; }
#endif
  return ret;
}
#endif
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxClothVertexFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CLOTH_VERTEX_DUMMY0: ret = "NX_CLOTH_VERTEX_DUMMY0"; break;
    case NX_CLOTH_VERTEX_DUMMY1: ret = "NX_CLOTH_VERTEX_DUMMY1"; break;
    case NX_CLOTH_VERTEX_DUMMY2: ret = "NX_CLOTH_VERTEX_DUMMY2"; break;
    case NX_CLOTH_VERTEX_DUMMY3: ret = "NX_CLOTH_VERTEX_DUMMY3"; break;
    case NX_CLOTH_VERTEX_DUMMY4: ret = "NX_CLOTH_VERTEX_DUMMY4"; break;
    case NX_CLOTH_VERTEX_DUMMY5: ret = "NX_CLOTH_VERTEX_DUMMY5"; break;
    case NX_CLOTH_VERTEX_DUMMY6: ret = "NX_CLOTH_VERTEX_DUMMY6"; break;
    case NX_CLOTH_VERTEX_TEARABLE: ret = "NX_CLOTH_VERTEX_TEARABLE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxClothVertexFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY0") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY0; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY1") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY1; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY2") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY2; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY3") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY3; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY4") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY4; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY5") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY5; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_DUMMY6") == 0 ) { v = NX_CLOTH_VERTEX_DUMMY6; ret = true; }
  else if ( strcasecmp(str,"NX_CLOTH_VERTEX_TEARABLE") == 0 ) { v = NX_CLOTH_VERTEX_TEARABLE; ret = true; }
  return ret;
}
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxFilterOp v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FILTEROP_AND: ret = "NX_FILTEROP_AND"; break;
    case NX_FILTEROP_OR: ret = "NX_FILTEROP_OR"; break;
    case NX_FILTEROP_XOR: ret = "NX_FILTEROP_XOR"; break;
    case NX_FILTEROP_NAND: ret = "NX_FILTEROP_NAND"; break;
    case NX_FILTEROP_NOR: ret = "NX_FILTEROP_NOR"; break;
    case NX_FILTEROP_NXOR: ret = "NX_FILTEROP_NXOR"; break;
    case NX_FILTEROP_SWAP_AND: ret = "NX_FILTEROP_SWAP_AND"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxFilterOp &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FILTEROP_AND") == 0 ) { v = NX_FILTEROP_AND; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_OR") == 0 ) { v = NX_FILTEROP_OR; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_XOR") == 0 ) { v = NX_FILTEROP_XOR; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_NAND") == 0 ) { v = NX_FILTEROP_NAND; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_NOR") == 0 ) { v = NX_FILTEROP_NOR; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_NXOR") == 0 ) { v = NX_FILTEROP_NXOR; ret = true; }
  else if ( strcasecmp(str,"NX_FILTEROP_SWAP_AND") == 0 ) { v = NX_FILTEROP_SWAP_AND; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxTimeStepMethod v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_TIMESTEP_FIXED: ret = "NX_TIMESTEP_FIXED"; break;
    case NX_TIMESTEP_VARIABLE: ret = "NX_TIMESTEP_VARIABLE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxTimeStepMethod &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_TIMESTEP_FIXED") == 0 ) { v = NX_TIMESTEP_FIXED; ret = true; }
  else if ( strcasecmp(str,"NX_TIMESTEP_VARIABLE") == 0 ) { v = NX_TIMESTEP_VARIABLE; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxSimulationType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SIMULATION_SW: ret = "NX_SIMULATION_SW"; break;
    case NX_SIMULATION_HW: ret = "NX_SIMULATION_HW"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSimulationType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SIMULATION_SW") == 0 ) { v = NX_SIMULATION_SW; ret = true; }
  else if ( strcasecmp(str,"NX_SIMULATION_HW") == 0 ) { v = NX_SIMULATION_HW; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 271
#endif
#if NX_SDK_VERSION_NUMBER >= 271
#endif
const char * EnumToString(NxThreadPriority v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_TP_HIGH: ret = "NX_TP_HIGH"; break;
#if NX_SDK_VERSION_NUMBER >= 271
    case NX_TP_ABOVE_NORMAL: ret = "NX_TP_ABOVE_NORMAL"; break;
#endif
    case NX_TP_NORMAL: ret = "NX_TP_NORMAL"; break;
#if NX_SDK_VERSION_NUMBER >= 271
    case NX_TP_BELOW_NORMAL: ret = "NX_TP_BELOW_NORMAL"; break;
#endif
    case NX_TP_LOW: ret = "NX_TP_LOW"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxThreadPriority &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_TP_HIGH") == 0 ) { v = NX_TP_HIGH; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 271
  else if ( strcasecmp(str,"NX_TP_ABOVE_NORMAL") == 0 ) { v = NX_TP_ABOVE_NORMAL; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_TP_NORMAL") == 0 ) { v = NX_TP_NORMAL; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 271
  else if ( strcasecmp(str,"NX_TP_BELOW_NORMAL") == 0 ) { v = NX_TP_BELOW_NORMAL; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_TP_LOW") == 0 ) { v = NX_TP_LOW; ret = true; }
  }
  return ret;
}
#endif

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 261
#endif
#if NX_SDK_VERSION_NUMBER >= 263
#endif
#if NX_SDK_VERSION_NUMBER >= 263
#endif
const char * EnumToString(NxSceneFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SF_DISABLE_SSE: ret = "NX_SF_DISABLE_SSE"; break;
    case NX_SF_DISABLE_COLLISIONS: ret = "NX_SF_DISABLE_COLLISIONS"; break;
    case NX_SF_SIMULATE_SEPARATE_THREAD: ret = "NX_SF_SIMULATE_SEPARATE_THREAD"; break;
    case NX_SF_ENABLE_MULTITHREAD: ret = "NX_SF_ENABLE_MULTITHREAD"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_SF_ENABLE_ACTIVETRANSFORMS: ret = "NX_SF_ENABLE_ACTIVETRANSFORMS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_SF_RESTRICTED_SCENE: ret = "NX_SF_RESTRICTED_SCENE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_SF_DISABLE_SCENE_MUTEX: ret = "NX_SF_DISABLE_SCENE_MUTEX"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 261
    case NX_SF_FORCE_CONE_FRICTION: ret = "NX_SF_FORCE_CONE_FRICTION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 263
    case NX_SF_SEQUENTIAL_PRIMARY: ret = "NX_SF_SEQUENTIAL_PRIMARY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 263
    case NX_SF_FLUID_PERFORMANCE_HINT: ret = "NX_SF_FLUID_PERFORMANCE_HINT"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSceneFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SF_DISABLE_SSE") == 0 ) { v = NX_SF_DISABLE_SSE; ret = true; }
  else if ( strcasecmp(str,"NX_SF_DISABLE_COLLISIONS") == 0 ) { v = NX_SF_DISABLE_COLLISIONS; ret = true; }
  else if ( strcasecmp(str,"NX_SF_SIMULATE_SEPARATE_THREAD") == 0 ) { v = NX_SF_SIMULATE_SEPARATE_THREAD; ret = true; }
  else if ( strcasecmp(str,"NX_SF_ENABLE_MULTITHREAD") == 0 ) { v = NX_SF_ENABLE_MULTITHREAD; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_SF_ENABLE_ACTIVETRANSFORMS") == 0 ) { v = NX_SF_ENABLE_ACTIVETRANSFORMS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_SF_RESTRICTED_SCENE") == 0 ) { v = NX_SF_RESTRICTED_SCENE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_SF_DISABLE_SCENE_MUTEX") == 0 ) { v = NX_SF_DISABLE_SCENE_MUTEX; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 261
  else if ( strcasecmp(str,"NX_SF_FORCE_CONE_FRICTION") == 0 ) { v = NX_SF_FORCE_CONE_FRICTION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 263
  else if ( strcasecmp(str,"NX_SF_SEQUENTIAL_PRIMARY") == 0 ) { v = NX_SF_SEQUENTIAL_PRIMARY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 263
  else if ( strcasecmp(str,"NX_SF_FLUID_PERFORMANCE_HINT") == 0 ) { v = NX_SF_FLUID_PERFORMANCE_HINT; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxCombineMode v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CM_AVERAGE: ret = "NX_CM_AVERAGE"; break;
    case NX_CM_MIN: ret = "NX_CM_MIN"; break;
    case NX_CM_MULTIPLY: ret = "NX_CM_MULTIPLY"; break;
    case NX_CM_MAX: ret = "NX_CM_MAX"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxCombineMode &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CM_AVERAGE") == 0 ) { v = NX_CM_AVERAGE; ret = true; }
  else if ( strcasecmp(str,"NX_CM_MIN") == 0 ) { v = NX_CM_MIN; ret = true; }
  else if ( strcasecmp(str,"NX_CM_MULTIPLY") == 0 ) { v = NX_CM_MULTIPLY; ret = true; }
  else if ( strcasecmp(str,"NX_CM_MAX") == 0 ) { v = NX_CM_MAX; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxMaterialFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_MF_ANISOTROPIC: ret = "NX_MF_ANISOTROPIC"; break;
    case NX_MF_DUMMY1: ret = "NX_MF_DUMMY1"; break;
    case NX_MF_DUMMY2: ret = "NX_MF_DUMMY2"; break;
    case NX_MF_DUMMY3: ret = "NX_MF_DUMMY3"; break;
    case NX_MF_DISABLE_FRICTION: ret = "NX_MF_DISABLE_FRICTION"; break;
    case NX_MF_DISABLE_STRONG_FRICTION: ret = "NX_MF_DISABLE_STRONG_FRICTION"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxMaterialFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_MF_ANISOTROPIC") == 0 ) { v = NX_MF_ANISOTROPIC; ret = true; }
  else if ( strcasecmp(str,"NX_MF_DUMMY1") == 0 ) { v = NX_MF_DUMMY1; ret = true; }
  else if ( strcasecmp(str,"NX_MF_DUMMY2") == 0 ) { v = NX_MF_DUMMY2; ret = true; }
  else if ( strcasecmp(str,"NX_MF_DUMMY3") == 0 ) { v = NX_MF_DUMMY3; ret = true; }
  else if ( strcasecmp(str,"NX_MF_DISABLE_FRICTION") == 0 ) { v = NX_MF_DISABLE_FRICTION; ret = true; }
  else if ( strcasecmp(str,"NX_MF_DISABLE_STRONG_FRICTION") == 0 ) { v = NX_MF_DISABLE_STRONG_FRICTION; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
const char * EnumToString(NxBodyFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_BF_DISABLE_GRAVITY: ret = "NX_BF_DISABLE_GRAVITY"; break;
    case NX_BF_FROZEN_POS_X: ret = "NX_BF_FROZEN_POS_X"; break;
    case NX_BF_FROZEN_POS_Y: ret = "NX_BF_FROZEN_POS_Y"; break;
    case NX_BF_FROZEN_POS_Z: ret = "NX_BF_FROZEN_POS_Z"; break;
    case NX_BF_FROZEN_ROT_X: ret = "NX_BF_FROZEN_ROT_X"; break;
    case NX_BF_FROZEN_ROT_Y: ret = "NX_BF_FROZEN_ROT_Y"; break;
    case NX_BF_FROZEN_ROT_Z: ret = "NX_BF_FROZEN_ROT_Z"; break;
    case NX_BF_KINEMATIC: ret = "NX_BF_KINEMATIC"; break;
    case NX_BF_VISUALIZATION: ret = "NX_BF_VISUALIZATION"; break;
    case NX_BF_DUMMY_0: ret = "NX_BF_DUMMY_0"; break;
    case NX_BF_FILTER_SLEEP_VEL: ret = "NX_BF_FILTER_SLEEP_VEL"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_BF_ENERGY_SLEEP_TEST: ret = "NX_BF_ENERGY_SLEEP_TEST"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxBodyFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_BF_DISABLE_GRAVITY") == 0 ) { v = NX_BF_DISABLE_GRAVITY; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_POS_X") == 0 ) { v = NX_BF_FROZEN_POS_X; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_POS_Y") == 0 ) { v = NX_BF_FROZEN_POS_Y; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_POS_Z") == 0 ) { v = NX_BF_FROZEN_POS_Z; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_ROT_X") == 0 ) { v = NX_BF_FROZEN_ROT_X; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_ROT_Y") == 0 ) { v = NX_BF_FROZEN_ROT_Y; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FROZEN_ROT_Z") == 0 ) { v = NX_BF_FROZEN_ROT_Z; ret = true; }
  else if ( strcasecmp(str,"NX_BF_KINEMATIC") == 0 ) { v = NX_BF_KINEMATIC; ret = true; }
  else if ( strcasecmp(str,"NX_BF_VISUALIZATION") == 0 ) { v = NX_BF_VISUALIZATION; ret = true; }
  else if ( strcasecmp(str,"NX_BF_DUMMY_0") == 0 ) { v = NX_BF_DUMMY_0; ret = true; }
  else if ( strcasecmp(str,"NX_BF_FILTER_SLEEP_VEL") == 0 ) { v = NX_BF_FILTER_SLEEP_VEL; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_BF_ENERGY_SLEEP_TEST") == 0 ) { v = NX_BF_ENERGY_SLEEP_TEST; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER < 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 261
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
const char * EnumToString(NxActorFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_AF_DISABLE_COLLISION: ret = "NX_AF_DISABLE_COLLISION"; break;
    case NX_AF_DISABLE_RESPONSE: ret = "NX_AF_DISABLE_RESPONSE"; break;
    case NX_AF_LOCK_COM: ret = "NX_AF_LOCK_COM"; break;
    case NX_AF_FLUID_DISABLE_COLLISION: ret = "NX_AF_FLUID_DISABLE_COLLISION"; break;
#if NX_SDK_VERSION_NUMBER < 250
    case NX_AF_FLUID_ACTOR_REACTION: ret = "NX_AF_FLUID_ACTOR_REACTION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_AF_CONTACT_MODIFICATION: ret = "NX_AF_CONTACT_MODIFICATION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 261
    case NX_AF_FORCE_CONE_FRICTION: ret = "NX_AF_FORCE_CONE_FRICTION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_AF_USER_ACTOR_PAIR_FILTERING: ret = "NX_AF_USER_ACTOR_PAIR_FILTERING"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxActorFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_AF_DISABLE_COLLISION") == 0 ) { v = NX_AF_DISABLE_COLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_AF_DISABLE_RESPONSE") == 0 ) { v = NX_AF_DISABLE_RESPONSE; ret = true; }
  else if ( strcasecmp(str,"NX_AF_LOCK_COM") == 0 ) { v = NX_AF_LOCK_COM; ret = true; }
  else if ( strcasecmp(str,"NX_AF_FLUID_DISABLE_COLLISION") == 0 ) { v = NX_AF_FLUID_DISABLE_COLLISION; ret = true; }
#if NX_SDK_VERSION_NUMBER < 250
  else if ( strcasecmp(str,"NX_AF_FLUID_ACTOR_REACTION") == 0 ) { v = NX_AF_FLUID_ACTOR_REACTION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_AF_CONTACT_MODIFICATION") == 0 ) { v = NX_AF_CONTACT_MODIFICATION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 261
  else if ( strcasecmp(str,"NX_AF_FORCE_CONE_FRICTION") == 0 ) { v = NX_AF_FORCE_CONE_FRICTION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_AF_USER_ACTOR_PAIR_FILTERING") == 0 ) { v = NX_AF_USER_ACTOR_PAIR_FILTERING; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxShapeType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SHAPE_PLANE: ret = "NX_SHAPE_PLANE"; break;
    case NX_SHAPE_SPHERE: ret = "NX_SHAPE_SPHERE"; break;
    case NX_SHAPE_BOX: ret = "NX_SHAPE_BOX"; break;
    case NX_SHAPE_CAPSULE: ret = "NX_SHAPE_CAPSULE"; break;
    case NX_SHAPE_WHEEL: ret = "NX_SHAPE_WHEEL"; break;
    case NX_SHAPE_CONVEX: ret = "NX_SHAPE_CONVEX"; break;
    case NX_SHAPE_MESH: ret = "NX_SHAPE_MESH"; break;
    case NX_SHAPE_HEIGHTFIELD: ret = "NX_SHAPE_HEIGHTFIELD"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxShapeType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SHAPE_PLANE") == 0 ) { v = NX_SHAPE_PLANE; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_SPHERE") == 0 ) { v = NX_SHAPE_SPHERE; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_BOX") == 0 ) { v = NX_SHAPE_BOX; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_CAPSULE") == 0 ) { v = NX_SHAPE_CAPSULE; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_WHEEL") == 0 ) { v = NX_SHAPE_WHEEL; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_CONVEX") == 0 ) { v = NX_SHAPE_CONVEX; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_MESH") == 0 ) { v = NX_SHAPE_MESH; ret = true; }
  else if ( strcasecmp(str,"NX_SHAPE_HEIGHTFIELD") == 0 ) { v = NX_SHAPE_HEIGHTFIELD; ret = true; }
  }
  return ret;
}
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxShapeCompartmentType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_COMPARTMENT_SW_RIGIDBODY: ret = "NX_COMPARTMENT_SW_RIGIDBODY"; break;
    case NX_COMPARTMENT_HW_RIGIDBODY: ret = "NX_COMPARTMENT_HW_RIGIDBODY"; break;
    case NX_COMPARTMENT_SW_FLUID: ret = "NX_COMPARTMENT_SW_FLUID"; break;
    case NX_COMPARTMENT_HW_FLUID: ret = "NX_COMPARTMENT_HW_FLUID"; break;
    case NX_COMPARTMENT_SW_CLOTH: ret = "NX_COMPARTMENT_SW_CLOTH"; break;
    case NX_COMPARTMENT_HW_CLOTH: ret = "NX_COMPARTMENT_HW_CLOTH"; break;
    case NX_COMPARTMENT_SW_SOFTBODY: ret = "NX_COMPARTMENT_SW_SOFTBODY"; break;
    case NX_COMPARTMENT_HW_SOFTBODY: ret = "NX_COMPARTMENT_HW_SOFTBODY"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxShapeCompartmentType &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_COMPARTMENT_SW_RIGIDBODY") == 0 ) { v = NX_COMPARTMENT_SW_RIGIDBODY; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_HW_RIGIDBODY") == 0 ) { v = NX_COMPARTMENT_HW_RIGIDBODY; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_SW_FLUID") == 0 ) { v = NX_COMPARTMENT_SW_FLUID; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_HW_FLUID") == 0 ) { v = NX_COMPARTMENT_HW_FLUID; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_SW_CLOTH") == 0 ) { v = NX_COMPARTMENT_SW_CLOTH; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_HW_CLOTH") == 0 ) { v = NX_COMPARTMENT_HW_CLOTH; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_SW_SOFTBODY") == 0 ) { v = NX_COMPARTMENT_SW_SOFTBODY; ret = true; }
  else if ( strcasecmp(str,"NX_COMPARTMENT_HW_SOFTBODY") == 0 ) { v = NX_COMPARTMENT_HW_SOFTBODY; ret = true; }
  return ret;
}
#endif

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
#endif
const char * EnumToString(NxShapeFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_TRIGGER_ON_ENTER: ret = "NX_TRIGGER_ON_ENTER"; break;
    case NX_TRIGGER_ON_LEAVE: ret = "NX_TRIGGER_ON_LEAVE"; break;
    case NX_TRIGGER_ON_STAY: ret = "NX_TRIGGER_ON_STAY"; break;
    case NX_SF_VISUALIZATION: ret = "NX_SF_VISUALIZATION"; break;
    case NX_SF_DISABLE_COLLISION: ret = "NX_SF_DISABLE_COLLISION"; break;
    case NX_SF_FEATURE_INDICES: ret = "NX_SF_FEATURE_INDICES"; break;
    case NX_SF_DISABLE_RAYCASTING: ret = "NX_SF_DISABLE_RAYCASTING"; break;
    case NX_SF_POINT_CONTACT_FORCE: ret = "NX_SF_POINT_CONTACT_FORCE"; break;
    case NX_SF_FLUID_DRAIN: ret = "NX_SF_FLUID_DRAIN"; break;
    case NX_SF_FLUID_DUMMY9: ret = "NX_SF_FLUID_DUMMY9"; break;
    case NX_SF_FLUID_DISABLE_COLLISION: ret = "NX_SF_FLUID_DISABLE_COLLISION"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_SF_FLUID_TWOWAY: ret = "NX_SF_FLUID_TWOWAY"; break;
#endif
    case NX_SF_DISABLE_RESPONSE: ret = "NX_SF_DISABLE_RESPONSE"; break;
    case NX_SF_DYNAMIC_DYNAMIC_CCD: ret = "NX_SF_DYNAMIC_DYNAMIC_CCD"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_SF_DISABLE_SCENE_QUERIES: ret = "NX_SF_DISABLE_SCENE_QUERIES"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_SF_CLOTH_DRAIN: ret = "NX_SF_CLOTH_DRAIN"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_SF_CLOTH_DISABLE_COLLISION: ret = "NX_SF_CLOTH_DISABLE_COLLISION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_SF_CLOTH_TWOWAY: ret = "NX_SF_CLOTH_TWOWAY"; break;
#endif
#if NX_USE_SOFTBODY_API
    case NX_SF_SOFTBODY_DRAIN: ret = "NX_SF_SOFTBODY_DRAIN"; break;
#endif
#if NX_USE_SOFTBODY_API
    case NX_SF_SOFTBODY_DISABLE_COLLISION: ret = "NX_SF_SOFTBODY_DISABLE_COLLISION"; break;
#endif
#if NX_USE_SOFTBODY_API
    case NX_SF_SOFTBODY_TWOWAY: ret = "NX_SF_SOFTBODY_TWOWAY"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxShapeFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_TRIGGER_ON_ENTER") == 0 ) { v = NX_TRIGGER_ON_ENTER; ret = true; }
  else if ( strcasecmp(str,"NX_TRIGGER_ON_LEAVE") == 0 ) { v = NX_TRIGGER_ON_LEAVE; ret = true; }
  else if ( strcasecmp(str,"NX_TRIGGER_ON_STAY") == 0 ) { v = NX_TRIGGER_ON_STAY; ret = true; }
  else if ( strcasecmp(str,"NX_SF_VISUALIZATION") == 0 ) { v = NX_SF_VISUALIZATION; ret = true; }
  else if ( strcasecmp(str,"NX_SF_DISABLE_COLLISION") == 0 ) { v = NX_SF_DISABLE_COLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_SF_FEATURE_INDICES") == 0 ) { v = NX_SF_FEATURE_INDICES; ret = true; }
  else if ( strcasecmp(str,"NX_SF_DISABLE_RAYCASTING") == 0 ) { v = NX_SF_DISABLE_RAYCASTING; ret = true; }
  else if ( strcasecmp(str,"NX_SF_POINT_CONTACT_FORCE") == 0 ) { v = NX_SF_POINT_CONTACT_FORCE; ret = true; }
  else if ( strcasecmp(str,"NX_SF_FLUID_DRAIN") == 0 ) { v = NX_SF_FLUID_DRAIN; ret = true; }
  else if ( strcasecmp(str,"NX_SF_FLUID_DUMMY9") == 0 ) { v = NX_SF_FLUID_DUMMY9; ret = true; }
  else if ( strcasecmp(str,"NX_SF_FLUID_DISABLE_COLLISION") == 0 ) { v = NX_SF_FLUID_DISABLE_COLLISION; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_SF_FLUID_TWOWAY") == 0 ) { v = NX_SF_FLUID_TWOWAY; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_SF_DISABLE_RESPONSE") == 0 ) { v = NX_SF_DISABLE_RESPONSE; ret = true; }
  else if ( strcasecmp(str,"NX_SF_DYNAMIC_DYNAMIC_CCD") == 0 ) { v = NX_SF_DYNAMIC_DYNAMIC_CCD; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_SF_DISABLE_SCENE_QUERIES") == 0 ) { v = NX_SF_DISABLE_SCENE_QUERIES; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_SF_CLOTH_DRAIN") == 0 ) { v = NX_SF_CLOTH_DRAIN; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_SF_CLOTH_DISABLE_COLLISION") == 0 ) { v = NX_SF_CLOTH_DISABLE_COLLISION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_SF_CLOTH_TWOWAY") == 0 ) { v = NX_SF_CLOTH_TWOWAY; ret = true; }
#endif
#if NX_USE_SOFTBODY_API
  else if ( strcasecmp(str,"NX_SF_SOFTBODY_DRAIN") == 0 ) { v = NX_SF_SOFTBODY_DRAIN; ret = true; }
#endif
#if NX_USE_SOFTBODY_API
  else if ( strcasecmp(str,"NX_SF_SOFTBODY_DISABLE_COLLISION") == 0 ) { v = NX_SF_SOFTBODY_DISABLE_COLLISION; ret = true; }
#endif
#if NX_USE_SOFTBODY_API
  else if ( strcasecmp(str,"NX_SF_SOFTBODY_TWOWAY") == 0 ) { v = NX_SF_SOFTBODY_TWOWAY; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxCapsuleShapeFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SWEPT_SHAPE: ret = "NX_SWEPT_SHAPE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxCapsuleShapeFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SWEPT_SHAPE") == 0 ) { v = NX_SWEPT_SHAPE; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 264
#endif
#if NX_SDK_VERSION_NUMBER >= 264
#endif
const char * EnumToString(NxWheelShapeFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_WF_WHEEL_AXIS_CONTACT_NORMAL: ret = "NX_WF_WHEEL_AXIS_CONTACT_NORMAL"; break;
    case NX_WF_INPUT_LAT_SLIPVELOCITY: ret = "NX_WF_INPUT_LAT_SLIPVELOCITY"; break;
    case NX_WF_INPUT_LNG_SLIPVELOCITY: ret = "NX_WF_INPUT_LNG_SLIPVELOCITY"; break;
    case NX_WF_UNSCALED_SPRING_BEHAVIOR: ret = "NX_WF_UNSCALED_SPRING_BEHAVIOR"; break;
    case NX_WF_AXLE_SPEED_OVERRIDE: ret = "NX_WF_AXLE_SPEED_OVERRIDE"; break;
#if NX_SDK_VERSION_NUMBER >= 264
    case NX_WF_EMULATE_LEGACY_WHEEL: ret = "NX_WF_EMULATE_LEGACY_WHEEL"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 264
    case NX_WF_CLAMPED_FRICTION: ret = "NX_WF_CLAMPED_FRICTION"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxWheelShapeFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_WF_WHEEL_AXIS_CONTACT_NORMAL") == 0 ) { v = NX_WF_WHEEL_AXIS_CONTACT_NORMAL; ret = true; }
  else if ( strcasecmp(str,"NX_WF_INPUT_LAT_SLIPVELOCITY") == 0 ) { v = NX_WF_INPUT_LAT_SLIPVELOCITY; ret = true; }
  else if ( strcasecmp(str,"NX_WF_INPUT_LNG_SLIPVELOCITY") == 0 ) { v = NX_WF_INPUT_LNG_SLIPVELOCITY; ret = true; }
  else if ( strcasecmp(str,"NX_WF_UNSCALED_SPRING_BEHAVIOR") == 0 ) { v = NX_WF_UNSCALED_SPRING_BEHAVIOR; ret = true; }
  else if ( strcasecmp(str,"NX_WF_AXLE_SPEED_OVERRIDE") == 0 ) { v = NX_WF_AXLE_SPEED_OVERRIDE; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 264
  else if ( strcasecmp(str,"NX_WF_EMULATE_LEGACY_WHEEL") == 0 ) { v = NX_WF_EMULATE_LEGACY_WHEEL; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 264
  else if ( strcasecmp(str,"NX_WF_CLAMPED_FRICTION") == 0 ) { v = NX_WF_CLAMPED_FRICTION; ret = true; }
#endif
  return ret;
}
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxMeshPagingMode v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_MESH_PAGING_MANUAL: ret = "NX_MESH_PAGING_MANUAL"; break;
    case NX_MESH_PAGING_FALLBACK: ret = "NX_MESH_PAGING_FALLBACK"; break;
    case NX_MESH_PAGING_AUTO: ret = "NX_MESH_PAGING_AUTO"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxMeshPagingMode &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_MESH_PAGING_MANUAL") == 0 ) { v = NX_MESH_PAGING_MANUAL; ret = true; }
  else if ( strcasecmp(str,"NX_MESH_PAGING_FALLBACK") == 0 ) { v = NX_MESH_PAGING_FALLBACK; ret = true; }
  else if ( strcasecmp(str,"NX_MESH_PAGING_AUTO") == 0 ) { v = NX_MESH_PAGING_AUTO; ret = true; }
  }
  return ret;
}
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxMeshShapeFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_MESH_SMOOTH_SPHERE_COLLISIONS: ret = "NX_MESH_SMOOTH_SPHERE_COLLISIONS"; break;
    case NX_MESH_DOUBLE_SIDED: ret = "NX_MESH_DOUBLE_SIDED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxMeshShapeFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_MESH_SMOOTH_SPHERE_COLLISIONS") == 0 ) { v = NX_MESH_SMOOTH_SPHERE_COLLISIONS; ret = true; }
  else if ( strcasecmp(str,"NX_MESH_DOUBLE_SIDED") == 0 ) { v = NX_MESH_DOUBLE_SIDED; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxJointType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_JOINT_PRISMATIC: ret = "NX_JOINT_PRISMATIC"; break;
    case NX_JOINT_REVOLUTE: ret = "NX_JOINT_REVOLUTE"; break;
    case NX_JOINT_CYLINDRICAL: ret = "NX_JOINT_CYLINDRICAL"; break;
    case NX_JOINT_SPHERICAL: ret = "NX_JOINT_SPHERICAL"; break;
    case NX_JOINT_POINT_ON_LINE: ret = "NX_JOINT_POINT_ON_LINE"; break;
    case NX_JOINT_POINT_IN_PLANE: ret = "NX_JOINT_POINT_IN_PLANE"; break;
    case NX_JOINT_DISTANCE: ret = "NX_JOINT_DISTANCE"; break;
    case NX_JOINT_PULLEY: ret = "NX_JOINT_PULLEY"; break;
    case NX_JOINT_FIXED: ret = "NX_JOINT_FIXED"; break;
    case NX_JOINT_D6: ret = "NX_JOINT_D6"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxJointType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_JOINT_PRISMATIC") == 0 ) { v = NX_JOINT_PRISMATIC; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_REVOLUTE") == 0 ) { v = NX_JOINT_REVOLUTE; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_CYLINDRICAL") == 0 ) { v = NX_JOINT_CYLINDRICAL; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_SPHERICAL") == 0 ) { v = NX_JOINT_SPHERICAL; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_POINT_ON_LINE") == 0 ) { v = NX_JOINT_POINT_ON_LINE; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_POINT_IN_PLANE") == 0 ) { v = NX_JOINT_POINT_IN_PLANE; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_DISTANCE") == 0 ) { v = NX_JOINT_DISTANCE; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_PULLEY") == 0 ) { v = NX_JOINT_PULLEY; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_FIXED") == 0 ) { v = NX_JOINT_FIXED; ret = true; }
  else if ( strcasecmp(str,"NX_JOINT_D6") == 0 ) { v = NX_JOINT_D6; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxJointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_JF_COLLISION_ENABLED: ret = "NX_JF_COLLISION_ENABLED"; break;
    case NX_JF_VISUALIZATION: ret = "NX_JF_VISUALIZATION"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxJointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_JF_COLLISION_ENABLED") == 0 ) { v = NX_JF_COLLISION_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_JF_VISUALIZATION") == 0 ) { v = NX_JF_VISUALIZATION; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxD6JointMotion v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_D6JOINT_MOTION_LOCKED: ret = "NX_D6JOINT_MOTION_LOCKED"; break;
    case NX_D6JOINT_MOTION_LIMITED: ret = "NX_D6JOINT_MOTION_LIMITED"; break;
    case NX_D6JOINT_MOTION_FREE: ret = "NX_D6JOINT_MOTION_FREE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxD6JointMotion &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_D6JOINT_MOTION_LOCKED") == 0 ) { v = NX_D6JOINT_MOTION_LOCKED; ret = true; }
  else if ( strcasecmp(str,"NX_D6JOINT_MOTION_LIMITED") == 0 ) { v = NX_D6JOINT_MOTION_LIMITED; ret = true; }
  else if ( strcasecmp(str,"NX_D6JOINT_MOTION_FREE") == 0 ) { v = NX_D6JOINT_MOTION_FREE; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxD6JointDriveType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_D6JOINT_DRIVE_POSITION: ret = "NX_D6JOINT_DRIVE_POSITION"; break;
    case NX_D6JOINT_DRIVE_VELOCITY: ret = "NX_D6JOINT_DRIVE_VELOCITY"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxD6JointDriveType &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_D6JOINT_DRIVE_POSITION") == 0 ) { v = NX_D6JOINT_DRIVE_POSITION; ret = true; }
  else if ( strcasecmp(str,"NX_D6JOINT_DRIVE_VELOCITY") == 0 ) { v = NX_D6JOINT_DRIVE_VELOCITY; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 260
#endif
const char * EnumToString(NxJointProjectionMode v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_JPM_NONE: ret = "NX_JPM_NONE"; break;
    case NX_JPM_POINT_MINDIST: ret = "NX_JPM_POINT_MINDIST"; break;
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_JPM_LINEAR_MINDIST: ret = "NX_JPM_LINEAR_MINDIST"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxJointProjectionMode &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_JPM_NONE") == 0 ) { v = NX_JPM_NONE; ret = true; }
  else if ( strcasecmp(str,"NX_JPM_POINT_MINDIST") == 0 ) { v = NX_JPM_POINT_MINDIST; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_JPM_LINEAR_MINDIST") == 0 ) { v = NX_JPM_LINEAR_MINDIST; ret = true; }
#endif
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxD6JointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_D6JOINT_SLERP_DRIVE: ret = "NX_D6JOINT_SLERP_DRIVE"; break;
    case NX_D6JOINT_GEAR_ENABLED: ret = "NX_D6JOINT_GEAR_ENABLED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxD6JointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_D6JOINT_SLERP_DRIVE") == 0 ) { v = NX_D6JOINT_SLERP_DRIVE; ret = true; }
  else if ( strcasecmp(str,"NX_D6JOINT_GEAR_ENABLED") == 0 ) { v = NX_D6JOINT_GEAR_ENABLED; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxDistanceJointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_DJF_MAX_DISTANCE_ENABLED: ret = "NX_DJF_MAX_DISTANCE_ENABLED"; break;
    case NX_DJF_MIN_DISTANCE_ENABLED: ret = "NX_DJF_MIN_DISTANCE_ENABLED"; break;
    case NX_DJF_SPRING_ENABLED: ret = "NX_DJF_SPRING_ENABLED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxDistanceJointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_DJF_MAX_DISTANCE_ENABLED") == 0 ) { v = NX_DJF_MAX_DISTANCE_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_DJF_MIN_DISTANCE_ENABLED") == 0 ) { v = NX_DJF_MIN_DISTANCE_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_DJF_SPRING_ENABLED") == 0 ) { v = NX_DJF_SPRING_ENABLED; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxRevoluteJointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_RJF_LIMIT_ENABLED: ret = "NX_RJF_LIMIT_ENABLED"; break;
    case NX_RJF_MOTOR_ENABLED: ret = "NX_RJF_MOTOR_ENABLED"; break;
    case NX_RJF_SPRING_ENABLED: ret = "NX_RJF_SPRING_ENABLED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxRevoluteJointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_RJF_LIMIT_ENABLED") == 0 ) { v = NX_RJF_LIMIT_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_RJF_MOTOR_ENABLED") == 0 ) { v = NX_RJF_MOTOR_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_RJF_SPRING_ENABLED") == 0 ) { v = NX_RJF_SPRING_ENABLED; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 281
#endif
const char * EnumToString(NxSphericalJointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SJF_TWIST_LIMIT_ENABLED: ret = "NX_SJF_TWIST_LIMIT_ENABLED"; break;
    case NX_SJF_SWING_LIMIT_ENABLED: ret = "NX_SJF_SWING_LIMIT_ENABLED"; break;
    case NX_SJF_TWIST_SPRING_ENABLED: ret = "NX_SJF_TWIST_SPRING_ENABLED"; break;
    case NX_SJF_SWING_SPRING_ENABLED: ret = "NX_SJF_SWING_SPRING_ENABLED"; break;
    case NX_SJF_JOINT_SPRING_ENABLED: ret = "NX_SJF_JOINT_SPRING_ENABLED"; break;
#if NX_SDK_VERSION_NUMBER >= 281
    case NX_SJF_PERPENDICULAR_DIR_CONSTRAINTS: ret = "NX_SJF_PERPENDICULAR_DIR_CONSTRAINTS"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSphericalJointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SJF_TWIST_LIMIT_ENABLED") == 0 ) { v = NX_SJF_TWIST_LIMIT_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_SJF_SWING_LIMIT_ENABLED") == 0 ) { v = NX_SJF_SWING_LIMIT_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_SJF_TWIST_SPRING_ENABLED") == 0 ) { v = NX_SJF_TWIST_SPRING_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_SJF_SWING_SPRING_ENABLED") == 0 ) { v = NX_SJF_SWING_SPRING_ENABLED; ret = true; }
  else if ( strcasecmp(str,"NX_SJF_JOINT_SPRING_ENABLED") == 0 ) { v = NX_SJF_JOINT_SPRING_ENABLED; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 281
  else if ( strcasecmp(str,"NX_SJF_PERPENDICULAR_DIR_CONSTRAINTS") == 0 ) { v = NX_SJF_PERPENDICULAR_DIR_CONSTRAINTS; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxPulleyJointFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_PJF_IS_RIGID: ret = "NX_PJF_IS_RIGID"; break;
    case NX_PJF_MOTOR_ENABLED: ret = "NX_PJF_MOTOR_ENABLED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxPulleyJointFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_PJF_IS_RIGID") == 0 ) { v = NX_PJF_IS_RIGID; ret = true; }
  else if ( strcasecmp(str,"NX_PJF_MOTOR_ENABLED") == 0 ) { v = NX_PJF_MOTOR_ENABLED; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
const char * EnumToString(NxContactPairFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_IGNORE_PAIR: ret = "NX_IGNORE_PAIR"; break;
    case NX_NOTIFY_ON_START_TOUCH: ret = "NX_NOTIFY_ON_START_TOUCH"; break;
    case NX_NOTIFY_ON_END_TOUCH: ret = "NX_NOTIFY_ON_END_TOUCH"; break;
    case NX_NOTIFY_ON_TOUCH: ret = "NX_NOTIFY_ON_TOUCH"; break;
    case NX_NOTIFY_ON_IMPACT: ret = "NX_NOTIFY_ON_IMPACT"; break;
    case NX_NOTIFY_ON_ROLL: ret = "NX_NOTIFY_ON_ROLL"; break;
    case NX_NOTIFY_ON_SLIDE: ret = "NX_NOTIFY_ON_SLIDE"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_NOTIFY_FORCES: ret = "NX_NOTIFY_FORCES"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD: ret = "NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD: ret = "NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD: ret = "NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_NOTIFY_CONTACT_MODIFICATION: ret = "NX_NOTIFY_CONTACT_MODIFICATION"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxContactPairFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_IGNORE_PAIR") == 0 ) { v = NX_IGNORE_PAIR; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_START_TOUCH") == 0 ) { v = NX_NOTIFY_ON_START_TOUCH; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_END_TOUCH") == 0 ) { v = NX_NOTIFY_ON_END_TOUCH; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_TOUCH") == 0 ) { v = NX_NOTIFY_ON_TOUCH; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_IMPACT") == 0 ) { v = NX_NOTIFY_ON_IMPACT; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_ROLL") == 0 ) { v = NX_NOTIFY_ON_ROLL; ret = true; }
  else if ( strcasecmp(str,"NX_NOTIFY_ON_SLIDE") == 0 ) { v = NX_NOTIFY_ON_SLIDE; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_NOTIFY_FORCES") == 0 ) { v = NX_NOTIFY_FORCES; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD") == 0 ) { v = NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD") == 0 ) { v = NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD") == 0 ) { v = NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_NOTIFY_CONTACT_MODIFICATION") == 0 ) { v = NX_NOTIFY_CONTACT_MODIFICATION; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxFluidSimulationMethod v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_F_SPH: ret = "NX_F_SPH"; break;
    case NX_F_NO_PARTICLE_INTERACTION: ret = "NX_F_NO_PARTICLE_INTERACTION"; break;
    case NX_F_MIXED_MODE: ret = "NX_F_MIXED_MODE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxFluidSimulationMethod &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_F_SPH") == 0 ) { v = NX_F_SPH; ret = true; }
  else if ( strcasecmp(str,"NX_F_NO_PARTICLE_INTERACTION") == 0 ) { v = NX_F_NO_PARTICLE_INTERACTION; ret = true; }
  else if ( strcasecmp(str,"NX_F_MIXED_MODE") == 0 ) { v = NX_F_MIXED_MODE; ret = true; }
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxFluidCollisionMethod v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_F_STATIC: ret = "NX_F_STATIC"; break;
    case NX_F_DYNAMIC: ret = "NX_F_DYNAMIC"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxFluidCollisionMethod &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_F_STATIC") == 0 ) { v = NX_F_STATIC; ret = true; }
  else if ( strcasecmp(str,"NX_F_DYNAMIC") == 0 ) { v = NX_F_DYNAMIC; ret = true; }
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
const char * EnumToString(NxFluidFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FF_VISUALIZATION: ret = "NX_FF_VISUALIZATION"; break;
    case NX_FF_DISABLE_GRAVITY: ret = "NX_FF_DISABLE_GRAVITY"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_FF_COLLISION_TWOWAY: ret = "NX_FF_COLLISION_TWOWAY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_FF_ENABLED: ret = "NX_FF_ENABLED"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_FF_HARDWARE: ret = "NX_FF_HARDWARE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_FF_PRIORITY_MODE: ret = "NX_FF_PRIORITY_MODE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FF_PROJECT_TO_PLANE: ret = "NX_FF_PROJECT_TO_PLANE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FF_FORCE_STRICT_COOKING_FORMAT: ret = "NX_FF_FORCE_STRICT_COOKING_FORMAT"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxFluidFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FF_VISUALIZATION") == 0 ) { v = NX_FF_VISUALIZATION; ret = true; }
  else if ( strcasecmp(str,"NX_FF_DISABLE_GRAVITY") == 0 ) { v = NX_FF_DISABLE_GRAVITY; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_FF_COLLISION_TWOWAY") == 0 ) { v = NX_FF_COLLISION_TWOWAY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_FF_ENABLED") == 0 ) { v = NX_FF_ENABLED; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_FF_HARDWARE") == 0 ) { v = NX_FF_HARDWARE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_FF_PRIORITY_MODE") == 0 ) { v = NX_FF_PRIORITY_MODE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FF_PROJECT_TO_PLANE") == 0 ) { v = NX_FF_PROJECT_TO_PLANE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FF_FORCE_STRICT_COOKING_FORMAT") == 0 ) { v = NX_FF_FORCE_STRICT_COOKING_FORMAT; ret = true; }
#endif
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxEmitterType v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FE_CONSTANT_PRESSURE: ret = "NX_FE_CONSTANT_PRESSURE"; break;
    case NX_FE_CONSTANT_FLOW_RATE: ret = "NX_FE_CONSTANT_FLOW_RATE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxEmitterType &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FE_CONSTANT_PRESSURE") == 0 ) { v = NX_FE_CONSTANT_PRESSURE; ret = true; }
  else if ( strcasecmp(str,"NX_FE_CONSTANT_FLOW_RATE") == 0 ) { v = NX_FE_CONSTANT_FLOW_RATE; ret = true; }
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxEmitterShape v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FE_RECTANGULAR: ret = "NX_FE_RECTANGULAR"; break;
    case NX_FE_ELLIPSE: ret = "NX_FE_ELLIPSE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxEmitterShape &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FE_RECTANGULAR") == 0 ) { v = NX_FE_RECTANGULAR; ret = true; }
  else if ( strcasecmp(str,"NX_FE_ELLIPSE") == 0 ) { v = NX_FE_ELLIPSE; ret = true; }
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER < 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER < 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER < 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
const char * EnumToString(NxFluidEmitterFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FEF_VISUALIZATION: ret = "NX_FEF_VISUALIZATION"; break;
#if NX_SDK_VERSION_NUMBER < 250
    case NX_FEF_BROKEN_ACTOR_REF: ret = "NX_FEF_BROKEN_ACTOR_REF"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_FEF_DUMMY1: ret = "NX_FEF_DUMMY1"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 260
    case NX_FEF_FORCE_ON_ACTOR: ret = "NX_FEF_FORCE_ON_ACTOR"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_FEF_FORCE_ON_BODY: ret = "NX_FEF_FORCE_ON_BODY"; break;
#endif
#if NX_SDK_VERSION_NUMBER < 260
    case NX_FEF_ADD_ACTOR_VELOCITY: ret = "NX_FEF_ADD_ACTOR_VELOCITY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
    case NX_FEF_ADD_BODY_VELOCITY: ret = "NX_FEF_ADD_BODY_VELOCITY"; break;
#endif
    case NX_FEF_ENABLED: ret = "NX_FEF_ENABLED"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxFluidEmitterFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FEF_VISUALIZATION") == 0 ) { v = NX_FEF_VISUALIZATION; ret = true; }
#if NX_SDK_VERSION_NUMBER < 250
  else if ( strcasecmp(str,"NX_FEF_BROKEN_ACTOR_REF") == 0 ) { v = NX_FEF_BROKEN_ACTOR_REF; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_FEF_DUMMY1") == 0 ) { v = NX_FEF_DUMMY1; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 260
  else if ( strcasecmp(str,"NX_FEF_FORCE_ON_ACTOR") == 0 ) { v = NX_FEF_FORCE_ON_ACTOR; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_FEF_FORCE_ON_BODY") == 0 ) { v = NX_FEF_FORCE_ON_BODY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER < 260
  else if ( strcasecmp(str,"NX_FEF_ADD_ACTOR_VELOCITY") == 0 ) { v = NX_FEF_ADD_ACTOR_VELOCITY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  else if ( strcasecmp(str,"NX_FEF_ADD_BODY_VELOCITY") == 0 ) { v = NX_FEF_ADD_BODY_VELOCITY; ret = true; }
#endif
  else if ( strcasecmp(str,"NX_FEF_ENABLED") == 0 ) { v = NX_FEF_ENABLED; ret = true; }
  return ret;
}
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
const char * EnumToString(NxClothFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_CLF_PRESSURE: ret = "NX_CLF_PRESSURE"; break;
    case NX_CLF_STATIC: ret = "NX_CLF_STATIC"; break;
    case NX_CLF_DISABLE_COLLISION: ret = "NX_CLF_DISABLE_COLLISION"; break;
    case NX_CLF_SELFCOLLISION: ret = "NX_CLF_SELFCOLLISION"; break;
    case NX_CLF_VISUALIZATION: ret = "NX_CLF_VISUALIZATION"; break;
    case NX_CLF_GRAVITY: ret = "NX_CLF_GRAVITY"; break;
    case NX_CLF_BENDING: ret = "NX_CLF_BENDING"; break;
    case NX_CLF_BENDING_ORTHO: ret = "NX_CLF_BENDING_ORTHO"; break;
    case NX_CLF_DAMPING: ret = "NX_CLF_DAMPING"; break;
    case NX_CLF_COLLISION_TWOWAY: ret = "NX_CLF_COLLISION_TWOWAY"; break;
    case NX_CLF_DUMMY1: ret = "NX_CLF_DUMMY1"; break;
    case NX_CLF_TRIANGLE_COLLISION: ret = "NX_CLF_TRIANGLE_COLLISION"; break;
    case NX_CLF_TEARABLE: ret = "NX_CLF_TEARABLE"; break;
    case NX_CLF_HARDWARE: ret = "NX_CLF_HARDWARE"; break;
#if NX_SDK_VERSION_NUMBER >= 250
    case NX_CLF_COMDAMPING: ret = "NX_CLF_COMDAMPING"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_CLF_VALIDBOUNDS: ret = "NX_CLF_VALIDBOUNDS"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    case NX_CLF_FLUID_COLLISION: ret = "NX_CLF_FLUID_COLLISION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_CLF_DISABLE_DYNAMIC_CCD: ret = "NX_CLF_DISABLE_DYNAMIC_CCD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_CLF_ADHERE: ret = "NX_CLF_ADHERE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_CLF_DUMMY19: ret = "NX_CLF_DUMMY19"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_CLF_HARD_STRETCH_LIMITATION: ret = "NX_CLF_HARD_STRETCH_LIMITATION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_CLF_UNTANGLING: ret = "NX_CLF_UNTANGLING"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_CLF_INTER_COLLISION: ret = "NX_CLF_INTER_COLLISION"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxClothFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_CLF_PRESSURE") == 0 ) { v = NX_CLF_PRESSURE; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_STATIC") == 0 ) { v = NX_CLF_STATIC; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_DISABLE_COLLISION") == 0 ) { v = NX_CLF_DISABLE_COLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_SELFCOLLISION") == 0 ) { v = NX_CLF_SELFCOLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_VISUALIZATION") == 0 ) { v = NX_CLF_VISUALIZATION; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_GRAVITY") == 0 ) { v = NX_CLF_GRAVITY; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_BENDING") == 0 ) { v = NX_CLF_BENDING; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_BENDING_ORTHO") == 0 ) { v = NX_CLF_BENDING_ORTHO; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_DAMPING") == 0 ) { v = NX_CLF_DAMPING; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_COLLISION_TWOWAY") == 0 ) { v = NX_CLF_COLLISION_TWOWAY; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_DUMMY1") == 0 ) { v = NX_CLF_DUMMY1; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_TRIANGLE_COLLISION") == 0 ) { v = NX_CLF_TRIANGLE_COLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_TEARABLE") == 0 ) { v = NX_CLF_TEARABLE; ret = true; }
  else if ( strcasecmp(str,"NX_CLF_HARDWARE") == 0 ) { v = NX_CLF_HARDWARE; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 250
  else if ( strcasecmp(str,"NX_CLF_COMDAMPING") == 0 ) { v = NX_CLF_COMDAMPING; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_CLF_VALIDBOUNDS") == 0 ) { v = NX_CLF_VALIDBOUNDS; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  else if ( strcasecmp(str,"NX_CLF_FLUID_COLLISION") == 0 ) { v = NX_CLF_FLUID_COLLISION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_CLF_DISABLE_DYNAMIC_CCD") == 0 ) { v = NX_CLF_DISABLE_DYNAMIC_CCD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_CLF_ADHERE") == 0 ) { v = NX_CLF_ADHERE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_CLF_DUMMY19") == 0 ) { v = NX_CLF_DUMMY19; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_CLF_HARD_STRETCH_LIMITATION") == 0 ) { v = NX_CLF_HARD_STRETCH_LIMITATION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_CLF_UNTANGLING") == 0 ) { v = NX_CLF_UNTANGLING; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_CLF_INTER_COLLISION") == 0 ) { v = NX_CLF_INTER_COLLISION; ret = true; }
#endif
  return ret;
}

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxForceFieldCoordinates v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FFC_CARTESIAN: ret = "NX_FFC_CARTESIAN"; break;
    case NX_FFC_SPHERICAL: ret = "NX_FFC_SPHERICAL"; break;
    case NX_FFC_CYLINDRICAL: ret = "NX_FFC_CYLINDRICAL"; break;
    case NX_FFC_TOROIDAL: ret = "NX_FFC_TOROIDAL"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxForceFieldCoordinates &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FFC_CARTESIAN") == 0 ) { v = NX_FFC_CARTESIAN; ret = true; }
  else if ( strcasecmp(str,"NX_FFC_SPHERICAL") == 0 ) { v = NX_FFC_SPHERICAL; ret = true; }
  else if ( strcasecmp(str,"NX_FFC_CYLINDRICAL") == 0 ) { v = NX_FFC_CYLINDRICAL; ret = true; }
  else if ( strcasecmp(str,"NX_FFC_TOROIDAL") == 0 ) { v = NX_FFC_TOROIDAL; ret = true; }
  }
  return ret;
}
#endif

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
const char * EnumToString(NxForceFieldType v)
{
  const char *ret = 0;
  switch ( v )
  {
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
    case NX_FF_TYPE_DUMMY_0: ret = "NX_FF_TYPE_DUMMY_0"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
    case NX_FF_TYPE_DUMMY_1: ret = "NX_FF_TYPE_DUMMY_1"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FF_TYPE_GRAVITATIONAL: ret = "NX_FF_TYPE_GRAVITATIONAL"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FF_TYPE_OTHER: ret = "NX_FF_TYPE_OTHER"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FF_TYPE_NO_INTERACTION: ret = "NX_FF_TYPE_NO_INTERACTION"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxForceFieldType &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  else if ( strcasecmp(str,"NX_FF_TYPE_DUMMY_0") == 0 ) { v = NX_FF_TYPE_DUMMY_0; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  else if ( strcasecmp(str,"NX_FF_TYPE_DUMMY_1") == 0 ) { v = NX_FF_TYPE_DUMMY_1; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FF_TYPE_GRAVITATIONAL") == 0 ) { v = NX_FF_TYPE_GRAVITATIONAL; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FF_TYPE_OTHER") == 0 ) { v = NX_FF_TYPE_OTHER; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FF_TYPE_NO_INTERACTION") == 0 ) { v = NX_FF_TYPE_NO_INTERACTION; ret = true; }
#endif
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
const char * EnumToString(NxForceFieldFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_FFF_DUMMY_0: ret = "NX_FFF_DUMMY_0"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_FFF_DUMMY_1: ret = "NX_FFF_DUMMY_1"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_FFF_DUMMY_2: ret = "NX_FFF_DUMMY_2"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_FFF_DUMMY_3: ret = "NX_FFF_DUMMY_3"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    case NX_FFF_DUMMY_4: ret = "NX_FFF_DUMMY_4"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FFF_VOLUMETRIC_SCALING_FLUID: ret = "NX_FFF_VOLUMETRIC_SCALING_FLUID"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FFF_VOLUMETRIC_SCALING_CLOTH: ret = "NX_FFF_VOLUMETRIC_SCALING_CLOTH"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FFF_VOLUMETRIC_SCALING_SOFTBODY: ret = "NX_FFF_VOLUMETRIC_SCALING_SOFTBODY"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY: ret = "NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxForceFieldFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_FFF_DUMMY_0") == 0 ) { v = NX_FFF_DUMMY_0; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_FFF_DUMMY_1") == 0 ) { v = NX_FFF_DUMMY_1; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_FFF_DUMMY_2") == 0 ) { v = NX_FFF_DUMMY_2; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_FFF_DUMMY_3") == 0 ) { v = NX_FFF_DUMMY_3; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  else if ( strcasecmp(str,"NX_FFF_DUMMY_4") == 0 ) { v = NX_FFF_DUMMY_4; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FFF_VOLUMETRIC_SCALING_FLUID") == 0 ) { v = NX_FFF_VOLUMETRIC_SCALING_FLUID; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FFF_VOLUMETRIC_SCALING_CLOTH") == 0 ) { v = NX_FFF_VOLUMETRIC_SCALING_CLOTH; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FFF_VOLUMETRIC_SCALING_SOFTBODY") == 0 ) { v = NX_FFF_VOLUMETRIC_SCALING_SOFTBODY; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY") == 0 ) { v = NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY; ret = true; }
#endif
  return ret;
}
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxForceFieldShapeGroupFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_FFSG_EXCLUDE_GROUP: ret = "NX_FFSG_EXCLUDE_GROUP"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxForceFieldShapeGroupFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_FFSG_EXCLUDE_GROUP") == 0 ) { v = NX_FFSG_EXCLUDE_GROUP; ret = true; }
  return ret;
}
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxSoftBodyMeshFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SOFTBODY_MESH_DUMMY: ret = "NX_SOFTBODY_MESH_DUMMY"; break;
    case NX_SOFTBODY_MESH_16_BIT_INDICES: ret = "NX_SOFTBODY_MESH_16_BIT_INDICES"; break;
    case NX_SOFTBODY_MESH_TEARABLE: ret = "NX_SOFTBODY_MESH_TEARABLE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSoftBodyMeshFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SOFTBODY_MESH_DUMMY") == 0 ) { v = NX_SOFTBODY_MESH_DUMMY; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_MESH_16_BIT_INDICES") == 0 ) { v = NX_SOFTBODY_MESH_16_BIT_INDICES; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_MESH_TEARABLE") == 0 ) { v = NX_SOFTBODY_MESH_TEARABLE; ret = true; }
  return ret;
}
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 283
#endif
const char * EnumToString(NxSoftBodyVertexFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SOFTBODY_VERTEX_DUMMY0: ret = "NX_SOFTBODY_VERTEX_DUMMY0"; break;
    case NX_SOFTBODY_VERTEX_DUMMY1: ret = "NX_SOFTBODY_VERTEX_DUMMY1"; break;
    case NX_SOFTBODY_VERTEX_DUMMY2: ret = "NX_SOFTBODY_VERTEX_DUMMY2"; break;
    case NX_SOFTBODY_VERTEX_DUMMY3: ret = "NX_SOFTBODY_VERTEX_DUMMY3"; break;
    case NX_SOFTBODY_VERTEX_DUMMY4: ret = "NX_SOFTBODY_VERTEX_DUMMY4"; break;
    case NX_SOFTBODY_VERTEX_DUMMY5: ret = "NX_SOFTBODY_VERTEX_DUMMY5"; break;
    case NX_SOFTBODY_VERTEX_DUMMY6: ret = "NX_SOFTBODY_VERTEX_DUMMY6"; break;
    case NX_SOFTBODY_VERTEX_TEARABLE: ret = "NX_SOFTBODY_VERTEX_TEARABLE"; break;
    case NX_SOFTBODY_VERTEX_DUMMY8: ret = "NX_SOFTBODY_VERTEX_DUMMY8"; break;
    case NX_SOFTBODY_VERTEX_DUMMY9: ret = "NX_SOFTBODY_VERTEX_DUMMY9"; break;
    case NX_SOFTBODY_VERTEX_DUMMY10: ret = "NX_SOFTBODY_VERTEX_DUMMY10"; break;
    case NX_SOFTBODY_VERTEX_DUMMY11: ret = "NX_SOFTBODY_VERTEX_DUMMY11"; break;
    case NX_SOFTBODY_VERTEX_DUMMY12: ret = "NX_SOFTBODY_VERTEX_DUMMY12"; break;
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SOFTBODY_VERTEX_SECONDARY: ret = "NX_SOFTBODY_VERTEX_SECONDARY"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSoftBodyVertexFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY0") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY0; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY1") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY1; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY2") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY2; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY3") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY3; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY4") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY4; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY5") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY5; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY6") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY6; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_TEARABLE") == 0 ) { v = NX_SOFTBODY_VERTEX_TEARABLE; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY8") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY8; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY9") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY9; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY10") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY10; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY11") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY11; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_DUMMY12") == 0 ) { v = NX_SOFTBODY_VERTEX_DUMMY12; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SOFTBODY_VERTEX_SECONDARY") == 0 ) { v = NX_SOFTBODY_VERTEX_SECONDARY; ret = true; }
#endif
  return ret;
}
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
const char * EnumToString(NxSoftBodyFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SBF_DUMMY0: ret = "NX_SBF_DUMMY0"; break;
    case NX_SBF_STATIC: ret = "NX_SBF_STATIC"; break;
    case NX_SBF_DISABLE_COLLISION: ret = "NX_SBF_DISABLE_COLLISION"; break;
    case NX_SBF_SELFCOLLISION: ret = "NX_SBF_SELFCOLLISION"; break;
    case NX_SBF_VISUALIZATION: ret = "NX_SBF_VISUALIZATION"; break;
    case NX_SBF_GRAVITY: ret = "NX_SBF_GRAVITY"; break;
    case NX_SBF_VOLUME_CONSERVATION: ret = "NX_SBF_VOLUME_CONSERVATION"; break;
    case NX_SBF_DAMPING: ret = "NX_SBF_DAMPING"; break;
    case NX_SBF_COLLISION_TWOWAY: ret = "NX_SBF_COLLISION_TWOWAY"; break;
    case NX_SBF_TEARABLE: ret = "NX_SBF_TEARABLE"; break;
    case NX_SBF_HARDWARE: ret = "NX_SBF_HARDWARE"; break;
    case NX_SBF_COMDAMPING: ret = "NX_SBF_COMDAMPING"; break;
    case NX_SBF_VALIDBOUNDS: ret = "NX_SBF_VALIDBOUNDS"; break;
    case NX_SBF_FLUID_COLLISION: ret = "NX_SBF_FLUID_COLLISION"; break;
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_SBF_DISABLE_DYNAMIC_CCD: ret = "NX_SBF_DISABLE_DYNAMIC_CCD"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    case NX_SBF_ADHERE: ret = "NX_SBF_ADHERE"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_DUMMY16: ret = "NX_SBF_DUMMY16"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_DUMMY17: ret = "NX_SBF_DUMMY17"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_DUMMY18: ret = "NX_SBF_DUMMY18"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_DUMMY19: ret = "NX_SBF_DUMMY19"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_HARD_STRETCH_LIMITATION: ret = "NX_SBF_HARD_STRETCH_LIMITATION"; break;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    case NX_SBF_INTER_COLLISION: ret = "NX_SBF_INTER_COLLISION"; break;
#endif
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSoftBodyFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SBF_DUMMY0") == 0 ) { v = NX_SBF_DUMMY0; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_STATIC") == 0 ) { v = NX_SBF_STATIC; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_DISABLE_COLLISION") == 0 ) { v = NX_SBF_DISABLE_COLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_SELFCOLLISION") == 0 ) { v = NX_SBF_SELFCOLLISION; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_VISUALIZATION") == 0 ) { v = NX_SBF_VISUALIZATION; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_GRAVITY") == 0 ) { v = NX_SBF_GRAVITY; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_VOLUME_CONSERVATION") == 0 ) { v = NX_SBF_VOLUME_CONSERVATION; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_DAMPING") == 0 ) { v = NX_SBF_DAMPING; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_COLLISION_TWOWAY") == 0 ) { v = NX_SBF_COLLISION_TWOWAY; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_TEARABLE") == 0 ) { v = NX_SBF_TEARABLE; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_HARDWARE") == 0 ) { v = NX_SBF_HARDWARE; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_COMDAMPING") == 0 ) { v = NX_SBF_COMDAMPING; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_VALIDBOUNDS") == 0 ) { v = NX_SBF_VALIDBOUNDS; ret = true; }
  else if ( strcasecmp(str,"NX_SBF_FLUID_COLLISION") == 0 ) { v = NX_SBF_FLUID_COLLISION; ret = true; }
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_SBF_DISABLE_DYNAMIC_CCD") == 0 ) { v = NX_SBF_DISABLE_DYNAMIC_CCD; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  else if ( strcasecmp(str,"NX_SBF_ADHERE") == 0 ) { v = NX_SBF_ADHERE; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_DUMMY16") == 0 ) { v = NX_SBF_DUMMY16; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_DUMMY17") == 0 ) { v = NX_SBF_DUMMY17; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_DUMMY18") == 0 ) { v = NX_SBF_DUMMY18; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_DUMMY19") == 0 ) { v = NX_SBF_DUMMY19; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_HARD_STRETCH_LIMITATION") == 0 ) { v = NX_SBF_HARD_STRETCH_LIMITATION; ret = true; }
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  else if ( strcasecmp(str,"NX_SBF_INTER_COLLISION") == 0 ) { v = NX_SBF_INTER_COLLISION; ret = true; }
#endif
  return ret;
}
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxSoftBodyAttachmentFlag v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_SOFTBODY_ATTACHMENT_TWOWAY: ret = "NX_SOFTBODY_ATTACHMENT_TWOWAY"; break;
    case NX_SOFTBODY_ATTACHMENT_TEARABLE: ret = "NX_SOFTBODY_ATTACHMENT_TEARABLE"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxSoftBodyAttachmentFlag &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_SOFTBODY_ATTACHMENT_TWOWAY") == 0 ) { v = NX_SOFTBODY_ATTACHMENT_TWOWAY; ret = true; }
  else if ( strcasecmp(str,"NX_SOFTBODY_ATTACHMENT_TEARABLE") == 0 ) { v = NX_SOFTBODY_ATTACHMENT_TEARABLE; ret = true; }
  return ret;
}
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(NxMeshDataFlags v)
{
  const char *ret = 0;
  switch ( v )
  {
    case NX_MDF_16_BIT_INDICES: ret = "NX_MDF_16_BIT_INDICES"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,NxMeshDataFlags &v)
{
  bool ret = false;
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"NX_MDF_16_BIT_INDICES") == 0 ) { v = NX_MDF_16_BIT_INDICES; ret = true; }
  return ret;
}

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
const char * EnumToString(SCHEMA_CLASS v)
{
  const char *ret = 0;
  switch ( v )
  {
    case SC_NXUSTREAM2: ret = "SC_NXUSTREAM2"; break;
    case SC_NxCompartmentFlag: ret = "SC_NxCompartmentFlag"; break;
    case SC_NxCompartmentDesc: ret = "SC_NxCompartmentDesc"; break;
    case SC_NxParameterDesc: ret = "SC_NxParameterDesc"; break;
    case SC_NxConvexFlags: ret = "SC_NxConvexFlags"; break;
    case SC_NxConvexMeshDesc: ret = "SC_NxConvexMeshDesc"; break;
    case SC_NxMeshFlags: ret = "SC_NxMeshFlags"; break;
    case SC_NxSimpleTriangleMesh: ret = "SC_NxSimpleTriangleMesh"; break;
    case SC_NxCCDSkeletonDesc: ret = "SC_NxCCDSkeletonDesc"; break;
    case SC_NxTriangleMeshDesc: ret = "SC_NxTriangleMeshDesc"; break;
    case SC_NxHeightFieldFlags: ret = "SC_NxHeightFieldFlags"; break;
    case SC_NxHeightFieldDesc: ret = "SC_NxHeightFieldDesc"; break;
    case SC_NxClothAttachDesc: ret = "SC_NxClothAttachDesc"; break;
    case SC_NxConstraintDominanceDesc: ret = "SC_NxConstraintDominanceDesc"; break;
    case SC_NxClothMeshFlags: ret = "SC_NxClothMeshFlags"; break;
    case SC_NxClothVertexFlags: ret = "SC_NxClothVertexFlags"; break;
    case SC_NxClothMeshDesc: ret = "SC_NxClothMeshDesc"; break;
    case SC_NxGroupsMask: ret = "SC_NxGroupsMask"; break;
    case SC_NxActorGroupPair: ret = "SC_NxActorGroupPair"; break;
    case SC_NxSceneLimits: ret = "SC_NxSceneLimits"; break;
    case SC_NxSceneFlags: ret = "SC_NxSceneFlags"; break;
    case SC_NxMaterialFlag: ret = "SC_NxMaterialFlag"; break;
    case SC_NxSpringDesc: ret = "SC_NxSpringDesc"; break;
    case SC_NxMaterialDesc: ret = "SC_NxMaterialDesc"; break;
    case SC_NxBodyFlag: ret = "SC_NxBodyFlag"; break;
    case SC_NxBodyDesc: ret = "SC_NxBodyDesc"; break;
    case SC_NxActorFlag: ret = "SC_NxActorFlag"; break;
    case SC_NxShapeCompartmentType: ret = "SC_NxShapeCompartmentType"; break;
    case SC_NxShapeFlag: ret = "SC_NxShapeFlag"; break;
    case SC_NxCapsuleShapeFlag: ret = "SC_NxCapsuleShapeFlag"; break;
    case SC_NxTireFunctionDesc: ret = "SC_NxTireFunctionDesc"; break;
    case SC_NxWheelShapeFlags: ret = "SC_NxWheelShapeFlags"; break;
    case SC_NxShapeDesc: ret = "SC_NxShapeDesc"; break;
    case SC_NxBoxShapeDesc: ret = "SC_NxBoxShapeDesc"; break;
    case SC_NxPlaneShapeDesc: ret = "SC_NxPlaneShapeDesc"; break;
    case SC_NxSphereShapeDesc: ret = "SC_NxSphereShapeDesc"; break;
    case SC_NxCapsuleShapeDesc: ret = "SC_NxCapsuleShapeDesc"; break;
    case SC_NxMeshShapeFlag: ret = "SC_NxMeshShapeFlag"; break;
    case SC_NxConvexShapeDesc: ret = "SC_NxConvexShapeDesc"; break;
    case SC_NxTriangleMeshShapeDesc: ret = "SC_NxTriangleMeshShapeDesc"; break;
    case SC_NxWheelShapeDesc: ret = "SC_NxWheelShapeDesc"; break;
    case SC_NxHeightFieldShapeDesc: ret = "SC_NxHeightFieldShapeDesc"; break;
    case SC_NxActorDesc: ret = "SC_NxActorDesc"; break;
    case SC_NxJointFlag: ret = "SC_NxJointFlag"; break;
    case SC_NxPlaneInfoDesc: ret = "SC_NxPlaneInfoDesc"; break;
    case SC_NxJointDesc: ret = "SC_NxJointDesc"; break;
    case SC_NxCylindricalJointDesc: ret = "SC_NxCylindricalJointDesc"; break;
    case SC_NxJointLimitSoftDesc: ret = "SC_NxJointLimitSoftDesc"; break;
    case SC_NxJointLimitSoftPairDesc: ret = "SC_NxJointLimitSoftPairDesc"; break;
    case SC_NxD6JointDriveType: ret = "SC_NxD6JointDriveType"; break;
    case SC_NxJointDriveDesc: ret = "SC_NxJointDriveDesc"; break;
    case SC_NxD6JointFlag: ret = "SC_NxD6JointFlag"; break;
    case SC_NxD6JointDesc: ret = "SC_NxD6JointDesc"; break;
    case SC_NxDistanceJointFlag: ret = "SC_NxDistanceJointFlag"; break;
    case SC_NxDistanceJointDesc: ret = "SC_NxDistanceJointDesc"; break;
    case SC_NxFixedJointDesc: ret = "SC_NxFixedJointDesc"; break;
    case SC_NxPointInPlaneJointDesc: ret = "SC_NxPointInPlaneJointDesc"; break;
    case SC_NxPointOnLineJointDesc: ret = "SC_NxPointOnLineJointDesc"; break;
    case SC_NxPrismaticJointDesc: ret = "SC_NxPrismaticJointDesc"; break;
    case SC_NxJointLimitDesc: ret = "SC_NxJointLimitDesc"; break;
    case SC_NxJointLimitPairDesc: ret = "SC_NxJointLimitPairDesc"; break;
    case SC_NxMotorDesc: ret = "SC_NxMotorDesc"; break;
    case SC_NxRevoluteJointFlag: ret = "SC_NxRevoluteJointFlag"; break;
    case SC_NxRevoluteJointDesc: ret = "SC_NxRevoluteJointDesc"; break;
    case SC_NxSphericalJointFlag: ret = "SC_NxSphericalJointFlag"; break;
    case SC_NxSphericalJointDesc: ret = "SC_NxSphericalJointDesc"; break;
    case SC_NxPulleyJointFlag: ret = "SC_NxPulleyJointFlag"; break;
    case SC_NxPulleyJointDesc: ret = "SC_NxPulleyJointDesc"; break;
    case SC_NxContactPairFlag: ret = "SC_NxContactPairFlag"; break;
    case SC_NxPairFlagDesc: ret = "SC_NxPairFlagDesc"; break;
    case SC_NxCollisionGroupDesc: ret = "SC_NxCollisionGroupDesc"; break;
    case SC_NxParticleData: ret = "SC_NxParticleData"; break;
    case SC_NxFluidSimulationMethod: ret = "SC_NxFluidSimulationMethod"; break;
    case SC_NxFluidCollisionMethod: ret = "SC_NxFluidCollisionMethod"; break;
    case SC_NxFluidFlag: ret = "SC_NxFluidFlag"; break;
    case SC_NxEmitterType: ret = "SC_NxEmitterType"; break;
    case SC_NxEmitterShape: ret = "SC_NxEmitterShape"; break;
    case SC_NxFluidEmitterFlag: ret = "SC_NxFluidEmitterFlag"; break;
    case SC_NxFluidEmitterDesc: ret = "SC_NxFluidEmitterDesc"; break;
    case SC_NxFluidDesc: ret = "SC_NxFluidDesc"; break;
    case SC_NxClothFlag: ret = "SC_NxClothFlag"; break;
    case SC_NxClothDesc: ret = "SC_NxClothDesc"; break;
    case SC_NxForceFieldFlags: ret = "SC_NxForceFieldFlags"; break;
    case SC_NxForceFieldShapeDesc: ret = "SC_NxForceFieldShapeDesc"; break;
    case SC_NxBoxForceFieldShapeDesc: ret = "SC_NxBoxForceFieldShapeDesc"; break;
    case SC_NxSphereForceFieldShapeDesc: ret = "SC_NxSphereForceFieldShapeDesc"; break;
    case SC_NxCapsuleForceFieldShapeDesc: ret = "SC_NxCapsuleForceFieldShapeDesc"; break;
    case SC_NxConvexForceFieldShapeDesc: ret = "SC_NxConvexForceFieldShapeDesc"; break;
    case SC_NxForceFieldShapeGroupFlags: ret = "SC_NxForceFieldShapeGroupFlags"; break;
    case SC_NxForceFieldShapeGroupDesc: ret = "SC_NxForceFieldShapeGroupDesc"; break;
    case SC_NxForceFieldLinearKernelDesc: ret = "SC_NxForceFieldLinearKernelDesc"; break;
    case SC_NxForceFieldScaleTableEntry: ret = "SC_NxForceFieldScaleTableEntry"; break;
    case SC_NxForceFieldGroupReference: ret = "SC_NxForceFieldGroupReference"; break;
    case SC_NxForceFieldDesc: ret = "SC_NxForceFieldDesc"; break;
    case SC_NxSpringAndDamperEffectorDesc: ret = "SC_NxSpringAndDamperEffectorDesc"; break;
    case SC_NxSoftBodyMeshFlags: ret = "SC_NxSoftBodyMeshFlags"; break;
    case SC_NxSoftBodyVertexFlags: ret = "SC_NxSoftBodyVertexFlags"; break;
    case SC_NxSoftBodyMeshDesc: ret = "SC_NxSoftBodyMeshDesc"; break;
    case SC_NxSoftBodyFlag: ret = "SC_NxSoftBodyFlag"; break;
    case SC_NxSoftBodyAttachmentFlag: ret = "SC_NxSoftBodyAttachmentFlag"; break;
    case SC_NxSoftBodyAttachDesc: ret = "SC_NxSoftBodyAttachDesc"; break;
    case SC_NxSoftBodyDesc: ret = "SC_NxSoftBodyDesc"; break;
    case SC_NxSceneDesc: ret = "SC_NxSceneDesc"; break;
    case SC_NxSceneInstanceDesc: ret = "SC_NxSceneInstanceDesc"; break;
    case SC_NxMeshDataFlags: ret = "SC_NxMeshDataFlags"; break;
    case SC_NxPhysicsSDKDesc: ret = "SC_NxPhysicsSDKDesc"; break;
    case SC_NxuPhysicsCollection: ret = "SC_NxuPhysicsCollection"; break;
    case SC_NxScene: ret = "SC_NxScene"; break;
    case SC_LAST: ret = "SC_LAST"; break;
    default: break;
  }
  return ret;
}
bool         StringToEnum(const char *str,SCHEMA_CLASS &v)
{
  bool ret = false;
  if ( str )
  {
	if (0) { assert (0); } 
  else if ( strcasecmp(str,"SC_NXUSTREAM2") == 0 ) { v = SC_NXUSTREAM2; ret = true; }
  else if ( strcasecmp(str,"SC_NxCompartmentFlag") == 0 ) { v = SC_NxCompartmentFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxCompartmentDesc") == 0 ) { v = SC_NxCompartmentDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxParameterDesc") == 0 ) { v = SC_NxParameterDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxConvexFlags") == 0 ) { v = SC_NxConvexFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxConvexMeshDesc") == 0 ) { v = SC_NxConvexMeshDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxMeshFlags") == 0 ) { v = SC_NxMeshFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxSimpleTriangleMesh") == 0 ) { v = SC_NxSimpleTriangleMesh; ret = true; }
  else if ( strcasecmp(str,"SC_NxCCDSkeletonDesc") == 0 ) { v = SC_NxCCDSkeletonDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxTriangleMeshDesc") == 0 ) { v = SC_NxTriangleMeshDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxHeightFieldFlags") == 0 ) { v = SC_NxHeightFieldFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxHeightFieldDesc") == 0 ) { v = SC_NxHeightFieldDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothAttachDesc") == 0 ) { v = SC_NxClothAttachDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxConstraintDominanceDesc") == 0 ) { v = SC_NxConstraintDominanceDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothMeshFlags") == 0 ) { v = SC_NxClothMeshFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothVertexFlags") == 0 ) { v = SC_NxClothVertexFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothMeshDesc") == 0 ) { v = SC_NxClothMeshDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxGroupsMask") == 0 ) { v = SC_NxGroupsMask; ret = true; }
  else if ( strcasecmp(str,"SC_NxActorGroupPair") == 0 ) { v = SC_NxActorGroupPair; ret = true; }
  else if ( strcasecmp(str,"SC_NxSceneLimits") == 0 ) { v = SC_NxSceneLimits; ret = true; }
  else if ( strcasecmp(str,"SC_NxSceneFlags") == 0 ) { v = SC_NxSceneFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxMaterialFlag") == 0 ) { v = SC_NxMaterialFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxSpringDesc") == 0 ) { v = SC_NxSpringDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxMaterialDesc") == 0 ) { v = SC_NxMaterialDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxBodyFlag") == 0 ) { v = SC_NxBodyFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxBodyDesc") == 0 ) { v = SC_NxBodyDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxActorFlag") == 0 ) { v = SC_NxActorFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxShapeCompartmentType") == 0 ) { v = SC_NxShapeCompartmentType; ret = true; }
  else if ( strcasecmp(str,"SC_NxShapeFlag") == 0 ) { v = SC_NxShapeFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxCapsuleShapeFlag") == 0 ) { v = SC_NxCapsuleShapeFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxTireFunctionDesc") == 0 ) { v = SC_NxTireFunctionDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxWheelShapeFlags") == 0 ) { v = SC_NxWheelShapeFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxShapeDesc") == 0 ) { v = SC_NxShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxBoxShapeDesc") == 0 ) { v = SC_NxBoxShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxPlaneShapeDesc") == 0 ) { v = SC_NxPlaneShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSphereShapeDesc") == 0 ) { v = SC_NxSphereShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxCapsuleShapeDesc") == 0 ) { v = SC_NxCapsuleShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxMeshShapeFlag") == 0 ) { v = SC_NxMeshShapeFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxConvexShapeDesc") == 0 ) { v = SC_NxConvexShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxTriangleMeshShapeDesc") == 0 ) { v = SC_NxTriangleMeshShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxWheelShapeDesc") == 0 ) { v = SC_NxWheelShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxHeightFieldShapeDesc") == 0 ) { v = SC_NxHeightFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxActorDesc") == 0 ) { v = SC_NxActorDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointFlag") == 0 ) { v = SC_NxJointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxPlaneInfoDesc") == 0 ) { v = SC_NxPlaneInfoDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointDesc") == 0 ) { v = SC_NxJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxCylindricalJointDesc") == 0 ) { v = SC_NxCylindricalJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointLimitSoftDesc") == 0 ) { v = SC_NxJointLimitSoftDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointLimitSoftPairDesc") == 0 ) { v = SC_NxJointLimitSoftPairDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxD6JointDriveType") == 0 ) { v = SC_NxD6JointDriveType; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointDriveDesc") == 0 ) { v = SC_NxJointDriveDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxD6JointFlag") == 0 ) { v = SC_NxD6JointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxD6JointDesc") == 0 ) { v = SC_NxD6JointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxDistanceJointFlag") == 0 ) { v = SC_NxDistanceJointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxDistanceJointDesc") == 0 ) { v = SC_NxDistanceJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxFixedJointDesc") == 0 ) { v = SC_NxFixedJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxPointInPlaneJointDesc") == 0 ) { v = SC_NxPointInPlaneJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxPointOnLineJointDesc") == 0 ) { v = SC_NxPointOnLineJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxPrismaticJointDesc") == 0 ) { v = SC_NxPrismaticJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointLimitDesc") == 0 ) { v = SC_NxJointLimitDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxJointLimitPairDesc") == 0 ) { v = SC_NxJointLimitPairDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxMotorDesc") == 0 ) { v = SC_NxMotorDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxRevoluteJointFlag") == 0 ) { v = SC_NxRevoluteJointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxRevoluteJointDesc") == 0 ) { v = SC_NxRevoluteJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSphericalJointFlag") == 0 ) { v = SC_NxSphericalJointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxSphericalJointDesc") == 0 ) { v = SC_NxSphericalJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxPulleyJointFlag") == 0 ) { v = SC_NxPulleyJointFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxPulleyJointDesc") == 0 ) { v = SC_NxPulleyJointDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxContactPairFlag") == 0 ) { v = SC_NxContactPairFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxPairFlagDesc") == 0 ) { v = SC_NxPairFlagDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxCollisionGroupDesc") == 0 ) { v = SC_NxCollisionGroupDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxParticleData") == 0 ) { v = SC_NxParticleData; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidSimulationMethod") == 0 ) { v = SC_NxFluidSimulationMethod; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidCollisionMethod") == 0 ) { v = SC_NxFluidCollisionMethod; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidFlag") == 0 ) { v = SC_NxFluidFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxEmitterType") == 0 ) { v = SC_NxEmitterType; ret = true; }
  else if ( strcasecmp(str,"SC_NxEmitterShape") == 0 ) { v = SC_NxEmitterShape; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidEmitterFlag") == 0 ) { v = SC_NxFluidEmitterFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidEmitterDesc") == 0 ) { v = SC_NxFluidEmitterDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxFluidDesc") == 0 ) { v = SC_NxFluidDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothFlag") == 0 ) { v = SC_NxClothFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxClothDesc") == 0 ) { v = SC_NxClothDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldFlags") == 0 ) { v = SC_NxForceFieldFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldShapeDesc") == 0 ) { v = SC_NxForceFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxBoxForceFieldShapeDesc") == 0 ) { v = SC_NxBoxForceFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSphereForceFieldShapeDesc") == 0 ) { v = SC_NxSphereForceFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxCapsuleForceFieldShapeDesc") == 0 ) { v = SC_NxCapsuleForceFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxConvexForceFieldShapeDesc") == 0 ) { v = SC_NxConvexForceFieldShapeDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldShapeGroupFlags") == 0 ) { v = SC_NxForceFieldShapeGroupFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldShapeGroupDesc") == 0 ) { v = SC_NxForceFieldShapeGroupDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldLinearKernelDesc") == 0 ) { v = SC_NxForceFieldLinearKernelDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldScaleTableEntry") == 0 ) { v = SC_NxForceFieldScaleTableEntry; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldGroupReference") == 0 ) { v = SC_NxForceFieldGroupReference; ret = true; }
  else if ( strcasecmp(str,"SC_NxForceFieldDesc") == 0 ) { v = SC_NxForceFieldDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSpringAndDamperEffectorDesc") == 0 ) { v = SC_NxSpringAndDamperEffectorDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyMeshFlags") == 0 ) { v = SC_NxSoftBodyMeshFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyVertexFlags") == 0 ) { v = SC_NxSoftBodyVertexFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyMeshDesc") == 0 ) { v = SC_NxSoftBodyMeshDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyFlag") == 0 ) { v = SC_NxSoftBodyFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyAttachmentFlag") == 0 ) { v = SC_NxSoftBodyAttachmentFlag; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyAttachDesc") == 0 ) { v = SC_NxSoftBodyAttachDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSoftBodyDesc") == 0 ) { v = SC_NxSoftBodyDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSceneDesc") == 0 ) { v = SC_NxSceneDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxSceneInstanceDesc") == 0 ) { v = SC_NxSceneInstanceDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxMeshDataFlags") == 0 ) { v = SC_NxMeshDataFlags; ret = true; }
  else if ( strcasecmp(str,"SC_NxPhysicsSDKDesc") == 0 ) { v = SC_NxPhysicsSDKDesc; ret = true; }
  else if ( strcasecmp(str,"SC_NxuPhysicsCollection") == 0 ) { v = SC_NxuPhysicsCollection; ret = true; }
  else if ( strcasecmp(str,"SC_NxScene") == 0 ) { v = SC_NxScene; ret = true; }
  else if ( strcasecmp(str,"SC_LAST") == 0 ) { v = SC_LAST; ret = true; }
  }
  return ret;
}

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 260

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 262

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 260

//***********************************************************************************
// Constructor for 'NxCompartmentDesc'
//***********************************************************************************
NxCompartmentDesc::NxCompartmentDesc(void)
{
  ::NxCompartmentDesc def;
  mId                                           = 0;
  type                                          = NX_SCT_RIGIDBODY;
  deviceCode                                    = def.deviceCode;
  gridHashCellSize                              = def.gridHashCellSize;
  gridHashTablePower                            = def.gridHashTablePower;
#if NX_SDK_VERSION_NUMBER >= 270
  flags                                         = (NxCompartmentFlag) def.flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  timeScale                                     = def.timeScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  threadMask                                    = def.threadMask;
#endif
  mInstance = 0;
}

NxCompartmentDesc::~NxCompartmentDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 271
#endif
}

void NxCompartmentDesc::store(SchemaStream &stream,const char *parent)
{
  NxCompartmentDesc def;
  stream.beginHeader(SC_NxCompartmentDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( stream.isBinary() )
    stream.store((NxU32)type,"type",true);
   else
   {
     if ( gSaveDefaults || def.type != type )
      stream.store(EnumToString(type),"type",true);
   }
  if ( gSaveDefaults || def.deviceCode != deviceCode )
    stream.store(deviceCode,"deviceCode",true);
  if ( gSaveDefaults || def.gridHashCellSize != gridHashCellSize )
    stream.store(gridHashCellSize,"gridHashCellSize",true);
  if ( gSaveDefaults || def.gridHashTablePower != gridHashTablePower )
    stream.store(gridHashTablePower,"gridHashTablePower",true);
#if NX_SDK_VERSION_NUMBER >= 270
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxCompartmentFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxCompartmentFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.timeScale != timeScale )
    stream.store(timeScale,"timeScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  if ( gSaveDefaults || def.threadMask != threadMask )
    stream.store(threadMask,"threadMask",false);
#endif
  stream.endHeader();
}

void NxCompartmentDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxCompartmentDesc,parent) )
  {
    stream.load(mId,"mId",true);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"type",true);
    type = (NxCompartmentType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"type",true);
     StringToEnum(enumName,type);
  }
    stream.load(deviceCode,"deviceCode",true);
    stream.load(gridHashCellSize,"gridHashCellSize",true);
    stream.load(gridHashTablePower,"gridHashTablePower",true);
#if NX_SDK_VERSION_NUMBER >= 270
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxCompartmentFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxCompartmentFlag,"flags") )
    {
      flags = (NxCompartmentFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxCompartmentFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxCompartmentFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(timeScale,"timeScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 271
    stream.load(threadMask,"threadMask",false);
#endif
    stream.endHeader();
  }
}

void NxCompartmentDesc::copyFrom(const ::NxCompartmentDesc &desc,CustomCopy &cc)
{
  type = (NxCompartmentType) desc.type;
  deviceCode = desc.deviceCode;
  gridHashCellSize = desc.gridHashCellSize;
  gridHashTablePower = desc.gridHashTablePower;
#if NX_SDK_VERSION_NUMBER >= 270
  flags = (NxCompartmentFlag) desc.flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  timeScale = desc.timeScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  threadMask = desc.threadMask;
#endif
}

void NxCompartmentDesc::copyTo(::NxCompartmentDesc &desc,CustomCopy &cc)
{
  desc.type = (::NxCompartmentType) type;
  desc.deviceCode = deviceCode;
  desc.gridHashCellSize = gridHashCellSize;
  desc.gridHashTablePower = gridHashTablePower;
#if NX_SDK_VERSION_NUMBER >= 270
  desc.flags = (::NxCompartmentFlag) flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  desc.timeScale = timeScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  desc.threadMask = threadMask;
#endif
}

#endif

//***********************************************************************************
// Constructor for 'NxParameterDesc'
//***********************************************************************************
NxParameterDesc::NxParameterDesc(void)
{
  param                                         = NX_PENALTY_FORCE;
  value                                         = 0;
  mInstance = 0;
}

NxParameterDesc::~NxParameterDesc(void)
{
}

void NxParameterDesc::store(SchemaStream &stream,const char *parent)
{
  NxParameterDesc def;
  stream.beginHeader(SC_NxParameterDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( stream.isBinary() )
    stream.store((NxU32)param,"param",true);
   else
   {
     if ( gSaveDefaults || def.param != param )
      stream.store(EnumToString(param),"param",true);
   }
  if ( gSaveDefaults || def.value != value )
    stream.store(value,"value",true);
  stream.endHeader();
}

void NxParameterDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxParameterDesc,parent) )
  {
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"param",true);
    param = (NxParameter) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"param",true);
     StringToEnum(enumName,param);
  }
    stream.load(value,"value",true);
    stream.endHeader();
  }
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxConvexMeshDesc'
//***********************************************************************************
NxConvexMeshDesc::NxConvexMeshDesc(void)
{
  ::NxConvexMeshDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  // NxArray< NxVec3 >: mPoints
  // NxArray< NxTri >: mTriangles
  flags                                         = (NxConvexFlags) def.flags;
  mCookedDataSize                               = 0;
  // NxArray< NxU8 >: mCookedData
  mInstance = 0;
// Member variables that are used internally to support auto-generated CCD skeletons.
  mCCDSkeleton = 0;
}

NxConvexMeshDesc::~NxConvexMeshDesc(void)
{
}

void NxConvexMeshDesc::store(SchemaStream &stream,const char *parent)
{
  NxConvexMeshDesc def;
  stream.beginHeader(SC_NxConvexMeshDesc);
  if ( gSaveCooked )
  {
    createCookedData(*this);
  }
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveCooked )
  {
  NxArray< NxVec3 > empty;
  stream.store(empty,"mPoints",false);
  }
  else
  {
  stream.store(mPoints,"mPoints",false);
  }
  if ( gSaveCooked )
  {
  NxArray< NxTri > empty;
  stream.store(empty,"mTriangles",false);
  }
  else
  {
  stream.store(mTriangles,"mTriangles",false);
  }
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxConvexFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxConvexFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( !gSaveCooked )
  {
  NxU32 empty = 0;
    stream.store(empty,"mCookedDataSize",false);
  }
  else
  {
  if ( gSaveDefaults || def.mCookedDataSize != mCookedDataSize )
    stream.store(mCookedDataSize,"mCookedDataSize",false);
  }
  if ( !gSaveCooked )
  {
  NxArray< NxU8 > empty;
  stream.store(empty,"mCookedData",false);
  }
  else
  {
  stream.store(mCookedData,"mCookedData",false);
  }
  stream.endHeader();
}

void NxConvexMeshDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxConvexMeshDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
  stream.load(mPoints,"mPoints",false);
  stream.load(mTriangles,"mTriangles",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxConvexFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxConvexFlags,"flags") )
    {
      flags = (NxConvexFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxConvexFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxConvexFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(mCookedDataSize,"mCookedDataSize",false);
  stream.load(mCookedData,"mCookedData",false);
    stream.endHeader();
  }
}

void NxConvexMeshDesc::copyFrom(const ::NxConvexMeshDesc &desc,CustomCopy &cc)
{
  flags = (NxConvexFlags) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxConvexMeshDesc::copyTo(::NxConvexMeshDesc &desc,CustomCopy &cc)
{
  desc.flags = (::NxConvexFlags) flags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxSimpleTriangleMesh'
//***********************************************************************************
NxSimpleTriangleMesh::NxSimpleTriangleMesh(void)
{
  ::NxSimpleTriangleMesh def;
  // NxArray< NxVec3 >: mPoints
  // NxArray< NxTri >: mTriangles
  flags                                         = (NxMeshFlags) def.flags;
  mInstance = 0;
}

NxSimpleTriangleMesh::~NxSimpleTriangleMesh(void)
{
}

void NxSimpleTriangleMesh::store(SchemaStream &stream,const char *parent)
{
  NxSimpleTriangleMesh def;
  stream.beginHeader(SC_NxSimpleTriangleMesh);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveCooked )
  {
  NxArray< NxVec3 > empty;
  stream.store(empty,"mPoints",false);
  }
  else
  {
  stream.store(mPoints,"mPoints",false);
  }
  if ( gSaveCooked )
  {
  NxArray< NxTri > empty;
  stream.store(empty,"mTriangles",false);
  }
  else
  {
  stream.store(mTriangles,"mTriangles",false);
  }
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxMeshFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxMeshFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  stream.endHeader();
}

void NxSimpleTriangleMesh::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSimpleTriangleMesh,parent) )
  {
  stream.load(mPoints,"mPoints",false);
  stream.load(mTriangles,"mTriangles",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxMeshFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxMeshFlags,"flags") )
    {
      flags = (NxMeshFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxMeshFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxMeshFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.endHeader();
  }
}

void NxSimpleTriangleMesh::copyFrom(const ::NxSimpleTriangleMesh &desc,CustomCopy &cc)
{
  flags = (NxMeshFlags) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxSimpleTriangleMesh::copyTo(::NxSimpleTriangleMesh &desc,CustomCopy &cc)
{
  desc.flags = (::NxMeshFlags) flags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxCCDSkeletonDesc'
//***********************************************************************************
NxCCDSkeletonDesc::NxCCDSkeletonDesc(void)
{
  mId                                           = 0;
  mUserProperties                               = 0;
  mInstance = 0;
// Member variables that are used internally to support auto-generated CCD skeletons.
  mPrimitive = SC_LAST;
  mDimensions.set(0,0,0);
  mRadius = 0;
  mHeight = 0;
  mConvexInstance = 0;
}

NxCCDSkeletonDesc::~NxCCDSkeletonDesc(void)
{
}

void NxCCDSkeletonDesc::store(SchemaStream &stream,const char *parent)
{
  NxCCDSkeletonDesc def;
  stream.beginHeader(SC_NxCCDSkeletonDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  bool cooked = gSaveCooked;  // preserve the cooked state since CCD Skeletons have no cooked representation.
  gSaveCooked = false;
  NxSimpleTriangleMesh::store(stream);
  gSaveCooked = cooked; // restore save cooking state.
  stream.endHeader();
}

void NxCCDSkeletonDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxCCDSkeletonDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
  NxSimpleTriangleMesh::load(stream);
    stream.endHeader();
  }
}


//***********************************************************************************
// Constructor for 'NxTriangleMeshDesc'
//***********************************************************************************
NxTriangleMeshDesc::NxTriangleMeshDesc(void)
{
  ::NxTriangleMeshDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  // NxArray< NxU32 >: mMaterialIndices
  heightFieldVerticalAxis                       = (NxHeightFieldAxis) def.heightFieldVerticalAxis;
  heightFieldVerticalExtent                     = def.heightFieldVerticalExtent;
  mPmapSize                                     = 0;
  mPmapDensity                                  = 0;
  // NxArray< NxU8 >: mPmapData
  convexEdgeThreshold                           = def.convexEdgeThreshold;
  mCookedDataSize                               = 0;
  // NxArray< NxU8 >: mCookedData
  mInstance = 0;
}

NxTriangleMeshDesc::~NxTriangleMeshDesc(void)
{
}

void NxTriangleMeshDesc::store(SchemaStream &stream,const char *parent)
{
  NxTriangleMeshDesc def;
  stream.beginHeader(SC_NxTriangleMeshDesc);
  if ( gSaveCooked )
  {
    createCookedData(*this);
  }
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveCooked )
  {
  NxArray< NxU32 > empty;
  stream.store(empty,"mMaterialIndices",false);
  }
  else
  {
  stream.store(mMaterialIndices,"mMaterialIndices",false);
  }
  if ( stream.isBinary() )
    stream.store((NxU32)heightFieldVerticalAxis,"heightFieldVerticalAxis",false);
   else
   {
     if ( gSaveDefaults || def.heightFieldVerticalAxis != heightFieldVerticalAxis )
      stream.store(EnumToString(heightFieldVerticalAxis),"heightFieldVerticalAxis",false);
   }
  if ( gSaveDefaults || def.heightFieldVerticalExtent != heightFieldVerticalExtent )
    stream.store(heightFieldVerticalExtent,"heightFieldVerticalExtent",false);
  if ( gSaveDefaults || def.mPmapSize != mPmapSize )
    stream.store(mPmapSize,"mPmapSize",false);
  if ( gSaveDefaults || def.mPmapDensity != mPmapDensity )
    stream.store(mPmapDensity,"mPmapDensity",false);
  stream.store(mPmapData,"mPmapData",false);
  if ( gSaveDefaults || def.convexEdgeThreshold != convexEdgeThreshold )
    stream.store(convexEdgeThreshold,"convexEdgeThreshold",false);
  if ( !gSaveCooked )
  {
  NxU32 empty = 0;
    stream.store(empty,"mCookedDataSize",false);
  }
  else
  {
  if ( gSaveDefaults || def.mCookedDataSize != mCookedDataSize )
    stream.store(mCookedDataSize,"mCookedDataSize",false);
  }
  if ( !gSaveCooked )
  {
  NxArray< NxU8 > empty;
  stream.store(empty,"mCookedData",false);
  }
  else
  {
  stream.store(mCookedData,"mCookedData",false);
  }
  NxSimpleTriangleMesh::store(stream);
  stream.endHeader();
}

void NxTriangleMeshDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxTriangleMeshDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
  stream.load(mMaterialIndices,"mMaterialIndices",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"heightFieldVerticalAxis",false);
    heightFieldVerticalAxis = (NxHeightFieldAxis) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"heightFieldVerticalAxis",false);
     StringToEnum(enumName,heightFieldVerticalAxis);
  }
    stream.load(heightFieldVerticalExtent,"heightFieldVerticalExtent",false);
    stream.load(mPmapSize,"mPmapSize",false);
    stream.load(mPmapDensity,"mPmapDensity",false);
  stream.load(mPmapData,"mPmapData",false);
    stream.load(convexEdgeThreshold,"convexEdgeThreshold",false);
    stream.load(mCookedDataSize,"mCookedDataSize",false);
  stream.load(mCookedData,"mCookedData",false);
  NxSimpleTriangleMesh::load(stream);
    stream.endHeader();
  }
}

void NxTriangleMeshDesc::copyFrom(const ::NxTriangleMeshDesc &desc,CustomCopy &cc)
{
  heightFieldVerticalAxis = (NxHeightFieldAxis) desc.heightFieldVerticalAxis;
  heightFieldVerticalExtent = desc.heightFieldVerticalExtent;
  convexEdgeThreshold = desc.convexEdgeThreshold;
  cc.customCopyFrom(*this,desc);
  NxSimpleTriangleMesh::copyFrom(desc,cc);
}

void NxTriangleMeshDesc::copyTo(::NxTriangleMeshDesc &desc,CustomCopy &cc)
{
  NxSimpleTriangleMesh::copyTo(desc,cc);
  desc.heightFieldVerticalAxis = (::NxHeightFieldAxis) heightFieldVerticalAxis;
  desc.heightFieldVerticalExtent = heightFieldVerticalExtent;
  desc.convexEdgeThreshold = convexEdgeThreshold;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxHeightFieldDesc'
//***********************************************************************************
NxHeightFieldDesc::NxHeightFieldDesc(void)
{
  ::NxHeightFieldDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  nbRows                                        = def.nbRows;
  nbColumns                                     = def.nbColumns;
  // NxArray< NxU32 >: mSamples
  verticalExtent                                = def.verticalExtent;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  thickness                                     = def.thickness;
#endif
  convexEdgeThreshold                           = def.convexEdgeThreshold;
  flags                                         = (NxHeightFieldFlags) def.flags;
  mInstance = 0;
}

NxHeightFieldDesc::~NxHeightFieldDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
}

void NxHeightFieldDesc::store(SchemaStream &stream,const char *parent)
{
  NxHeightFieldDesc def;
  stream.beginHeader(SC_NxHeightFieldDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.nbRows != nbRows )
    stream.store(nbRows,"nbRows",false);
  if ( gSaveDefaults || def.nbColumns != nbColumns )
    stream.store(nbColumns,"nbColumns",false);
  stream.store(mSamples,"mSamples",false);
  if ( gSaveDefaults || def.verticalExtent != verticalExtent )
    stream.store(verticalExtent,"verticalExtent",false);
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( gSaveDefaults || def.thickness != thickness )
    stream.store(thickness,"thickness",false);
#endif
  if ( gSaveDefaults || def.convexEdgeThreshold != convexEdgeThreshold )
    stream.store(convexEdgeThreshold,"convexEdgeThreshold",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxHeightFieldFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxHeightFieldFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  stream.endHeader();
}

void NxHeightFieldDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxHeightFieldDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(nbRows,"nbRows",false);
    stream.load(nbColumns,"nbColumns",false);
  stream.load(mSamples,"mSamples",false);
    stream.load(verticalExtent,"verticalExtent",false);
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
    stream.load(thickness,"thickness",false);
#endif
    stream.load(convexEdgeThreshold,"convexEdgeThreshold",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxHeightFieldFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxHeightFieldFlags,"flags") )
    {
      flags = (NxHeightFieldFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxHeightFieldFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxHeightFieldFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.endHeader();
  }
}

void NxHeightFieldDesc::copyFrom(const ::NxHeightFieldDesc &desc,CustomCopy &cc)
{
  nbRows = desc.nbRows;
  nbColumns = desc.nbColumns;
  verticalExtent = desc.verticalExtent;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  thickness = desc.thickness;
#endif
  convexEdgeThreshold = desc.convexEdgeThreshold;
  flags = (NxHeightFieldFlags) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxHeightFieldDesc::copyTo(::NxHeightFieldDesc &desc,CustomCopy &cc)
{
  desc.nbRows = nbRows;
  desc.nbColumns = nbColumns;
  desc.verticalExtent = verticalExtent;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.thickness = thickness;
#endif
  desc.convexEdgeThreshold = convexEdgeThreshold;
  desc.flags = (::NxHeightFieldFlags) flags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxClothAttachDesc'
//***********************************************************************************
NxClothAttachDesc::NxClothAttachDesc(void)
{
  mAttachActor                                  = 0;
  mAttachShapeIndex                             = 0;
  mTwoWay                                       = false;
  mTearable                                     = false;
  mCore                                         = false;
  mInstance = 0;
}

NxClothAttachDesc::~NxClothAttachDesc(void)
{
}

void NxClothAttachDesc::store(SchemaStream &stream,const char *parent)
{
  NxClothAttachDesc def;
  stream.beginHeader(SC_NxClothAttachDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mAttachActor != mAttachActor )
    stream.store(mAttachActor,"mAttachActor",true);
  if ( gSaveDefaults || def.mAttachShapeIndex != mAttachShapeIndex )
    stream.store(mAttachShapeIndex,"mAttachShapeIndex",true);
  if ( gSaveDefaults || def.mTwoWay != mTwoWay )
    stream.store(mTwoWay,"mTwoWay",true);
  if ( gSaveDefaults || def.mTearable != mTearable )
    stream.store(mTearable,"mTearable",true);
  if ( gSaveDefaults || def.mCore != mCore )
    stream.store(mCore,"mCore",false);
  stream.endHeader();
}

void NxClothAttachDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxClothAttachDesc,parent) )
  {
    stream.load(mAttachActor,"mAttachActor",true);
    stream.load(mAttachShapeIndex,"mAttachShapeIndex",true);
    stream.load(mTwoWay,"mTwoWay",true);
    stream.load(mTearable,"mTearable",true);
    stream.load(mCore,"mCore",false);
    stream.endHeader();
  }
}


//***********************************************************************************
// Constructor for 'NxConstraintDominanceDesc'
//***********************************************************************************
NxConstraintDominanceDesc::NxConstraintDominanceDesc(void)
{
  mGroup0                                       = 0;
  mGroup1                                       = 0;
  mDominance0                                   = 0;
  mDominance1                                   = 0;
  mInstance = 0;
}

NxConstraintDominanceDesc::~NxConstraintDominanceDesc(void)
{
}

void NxConstraintDominanceDesc::store(SchemaStream &stream,const char *parent)
{
  NxConstraintDominanceDesc def;
  stream.beginHeader(SC_NxConstraintDominanceDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mGroup0 != mGroup0 )
    stream.store(mGroup0,"mGroup0",true);
  if ( gSaveDefaults || def.mGroup1 != mGroup1 )
    stream.store(mGroup1,"mGroup1",true);
  if ( gSaveDefaults || def.mDominance0 != mDominance0 )
    stream.store(mDominance0,"mDominance0",false);
  if ( gSaveDefaults || def.mDominance1 != mDominance1 )
    stream.store(mDominance1,"mDominance1",false);
  stream.endHeader();
}

void NxConstraintDominanceDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxConstraintDominanceDesc,parent) )
  {
    stream.load(mGroup0,"mGroup0",true);
    stream.load(mGroup1,"mGroup1",true);
    stream.load(mDominance0,"mDominance0",false);
    stream.load(mDominance1,"mDominance1",false);
    stream.endHeader();
  }
}

#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
// Constructor for 'NxClothMeshDesc'
//***********************************************************************************
NxClothMeshDesc::NxClothMeshDesc(void)
{
  ::NxClothMeshDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  // NxArray< NxU32 >: mVertexMasses
  // NxArray< NxU32 >: mVertexFlags
#if NX_SDK_VERSION_NUMBER >= 250
  // NxClothMeshFlags: mMeshFlags
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  weldingDistance                               = def.weldingDistance;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  numHierarchyLevels                            = def.numHierarchyLevels;
#endif
  mInstance = 0;
}

NxClothMeshDesc::~NxClothMeshDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
}

void NxClothMeshDesc::store(SchemaStream &stream,const char *parent)
{
  NxClothMeshDesc def;
  stream.beginHeader(SC_NxClothMeshDesc);
  bool saveCooked = gSaveCooked;
	 gSaveCooked = false;
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  stream.store(mVertexMasses,"mVertexMasses",false);
  stream.store(mVertexFlags,"mVertexFlags",false);
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
  {
    stream.store((NxU32)mMeshFlags,"mMeshFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.mMeshFlags != mMeshFlags )
    {
      stream.beginHeader(SC_NxClothMeshFlags);
      stream.store("mMeshFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxClothMeshFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( mMeshFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.weldingDistance != weldingDistance )
    stream.store(weldingDistance,"weldingDistance",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.numHierarchyLevels != numHierarchyLevels )
    stream.store(numHierarchyLevels,"numHierarchyLevels",false);
#endif
  NxSimpleTriangleMesh::store(stream);
	 gSaveCooked = saveCooked;
  stream.endHeader();
}

void NxClothMeshDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxClothMeshDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
  stream.load(mVertexMasses,"mVertexMasses",false);
  stream.load(mVertexFlags,"mVertexFlags",false);
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"mMeshFlags",false);
    mMeshFlags = (NxClothMeshFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxClothMeshFlags,"mMeshFlags") )
    {
      mMeshFlags = (NxClothMeshFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxClothMeshFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	mMeshFlags = (NxClothMeshFlags) (mMeshFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(weldingDistance,"weldingDistance",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(numHierarchyLevels,"numHierarchyLevels",false);
#endif
  NxSimpleTriangleMesh::load(stream);
    stream.endHeader();
  }
}

void NxClothMeshDesc::copyFrom(const ::NxClothMeshDesc &desc,CustomCopy &cc)
{
#if NX_SDK_VERSION_NUMBER >= 280
  weldingDistance = desc.weldingDistance;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  numHierarchyLevels = desc.numHierarchyLevels;
#endif
  cc.customCopyFrom(*this,desc);
  NxSimpleTriangleMesh::copyFrom(desc,cc);
}

void NxClothMeshDesc::copyTo(::NxClothMeshDesc &desc,CustomCopy &cc)
{
  NxSimpleTriangleMesh::copyTo(desc,cc);
#if NX_SDK_VERSION_NUMBER >= 280
  desc.weldingDistance = weldingDistance;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.numHierarchyLevels = numHierarchyLevels;
#endif
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxGroupsMask'
//***********************************************************************************
NxGroupsMask::NxGroupsMask(void)
{
  ::NxGroupsMask def;
  bits0                                         = 0;
  bits1                                         = 0;
  bits2                                         = 0;
  bits3                                         = 0;
  mInstance = 0;
}

NxGroupsMask::~NxGroupsMask(void)
{
}

void NxGroupsMask::store(SchemaStream &stream,const char *parent)
{
  NxGroupsMask def;
  stream.beginHeader(SC_NxGroupsMask);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.bits0 != bits0 )
    stream.store(bits0,"bits0",true);
  if ( gSaveDefaults || def.bits1 != bits1 )
    stream.store(bits1,"bits1",true);
  if ( gSaveDefaults || def.bits2 != bits2 )
    stream.store(bits2,"bits2",true);
  if ( gSaveDefaults || def.bits3 != bits3 )
    stream.store(bits3,"bits3",true);
  stream.endHeader();
}

void NxGroupsMask::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxGroupsMask,parent) )
  {
    stream.load(bits0,"bits0",true);
    stream.load(bits1,"bits1",true);
    stream.load(bits2,"bits2",true);
    stream.load(bits3,"bits3",true);
    stream.endHeader();
  }
}

void NxGroupsMask::copyFrom(const ::NxGroupsMask &desc,CustomCopy &cc)
{
  bits0 = desc.bits0;
  bits1 = desc.bits1;
  bits2 = desc.bits2;
  bits3 = desc.bits3;
}

void NxGroupsMask::copyTo(::NxGroupsMask &desc,CustomCopy &cc)
{
  desc.bits0 = bits0;
  desc.bits1 = bits1;
  desc.bits2 = bits2;
  desc.bits3 = bits3;
}


//***********************************************************************************
// Constructor for 'NxActorGroupPair'
//***********************************************************************************
NxActorGroupPair::NxActorGroupPair(void)
{
  group0                                        = 0;
  group1                                        = 0;
  // NxContactPairFlag: flags
  mInstance = 0;
}

NxActorGroupPair::~NxActorGroupPair(void)
{
}

void NxActorGroupPair::store(SchemaStream &stream,const char *parent)
{
  NxActorGroupPair def;
  stream.beginHeader(SC_NxActorGroupPair);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.group0 != group0 )
    stream.store(group0,"group0",true);
  if ( gSaveDefaults || def.group1 != group1 )
    stream.store(group1,"group1",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxContactPairFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxContactPairFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  stream.endHeader();
}

void NxActorGroupPair::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxActorGroupPair,parent) )
  {
    stream.load(group0,"group0",true);
    stream.load(group1,"group1",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxContactPairFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxContactPairFlag,"flags") )
    {
      flags = (NxContactPairFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxContactPairFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxContactPairFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.endHeader();
  }
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxSceneLimits'
//***********************************************************************************
NxSceneLimits::NxSceneLimits(void)
{
  ::NxSceneLimits def;
  maxNbActors                                   = def.maxNbActors;
  maxNbBodies                                   = def.maxNbBodies;
  maxNbStaticShapes                             = def.maxNbStaticShapes;
  maxNbDynamicShapes                            = def.maxNbDynamicShapes;
  maxNbJoints                                   = def.maxNbJoints;
  mInstance = 0;
}

NxSceneLimits::~NxSceneLimits(void)
{
}

void NxSceneLimits::store(SchemaStream &stream,const char *parent)
{
  NxSceneLimits def;
  stream.beginHeader(SC_NxSceneLimits);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.maxNbActors != maxNbActors )
    stream.store(maxNbActors,"maxNbActors",false);
  if ( gSaveDefaults || def.maxNbBodies != maxNbBodies )
    stream.store(maxNbBodies,"maxNbBodies",false);
  if ( gSaveDefaults || def.maxNbStaticShapes != maxNbStaticShapes )
    stream.store(maxNbStaticShapes,"maxNbStaticShapes",false);
  if ( gSaveDefaults || def.maxNbDynamicShapes != maxNbDynamicShapes )
    stream.store(maxNbDynamicShapes,"maxNbDynamicShapes",false);
  if ( gSaveDefaults || def.maxNbJoints != maxNbJoints )
    stream.store(maxNbJoints,"maxNbJoints",false);
  stream.endHeader();
}

void NxSceneLimits::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSceneLimits,parent) )
  {
    stream.load(maxNbActors,"maxNbActors",false);
    stream.load(maxNbBodies,"maxNbBodies",false);
    stream.load(maxNbStaticShapes,"maxNbStaticShapes",false);
    stream.load(maxNbDynamicShapes,"maxNbDynamicShapes",false);
    stream.load(maxNbJoints,"maxNbJoints",false);
    stream.endHeader();
  }
}

void NxSceneLimits::copyFrom(const ::NxSceneLimits &desc,CustomCopy &cc)
{
  maxNbActors = desc.maxNbActors;
  maxNbBodies = desc.maxNbBodies;
  maxNbStaticShapes = desc.maxNbStaticShapes;
  maxNbDynamicShapes = desc.maxNbDynamicShapes;
  maxNbJoints = desc.maxNbJoints;
}

void NxSceneLimits::copyTo(::NxSceneLimits &desc,CustomCopy &cc)
{
  desc.maxNbActors = maxNbActors;
  desc.maxNbBodies = maxNbBodies;
  desc.maxNbStaticShapes = maxNbStaticShapes;
  desc.maxNbDynamicShapes = maxNbDynamicShapes;
  desc.maxNbJoints = maxNbJoints;
}

#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxSpringDesc'
//***********************************************************************************
NxSpringDesc::NxSpringDesc(void)
{
  ::NxSpringDesc def;
  spring                                        = def.spring;
  damper                                        = def.damper;
  targetValue                                   = def.targetValue;
  mInstance = 0;
}

NxSpringDesc::~NxSpringDesc(void)
{
}

void NxSpringDesc::store(SchemaStream &stream,const char *parent)
{
  NxSpringDesc def;
  stream.beginHeader(SC_NxSpringDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.spring != spring )
    stream.store(spring,"spring",false);
  if ( gSaveDefaults || def.damper != damper )
    stream.store(damper,"damper",false);
  if ( gSaveDefaults || def.targetValue != targetValue )
    stream.store(targetValue,"targetValue",false);
  stream.endHeader();
}

void NxSpringDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSpringDesc,parent) )
  {
    stream.load(spring,"spring",false);
    stream.load(damper,"damper",false);
    stream.load(targetValue,"targetValue",false);
    stream.endHeader();
  }
}

void NxSpringDesc::copyFrom(const ::NxSpringDesc &desc,CustomCopy &cc)
{
  spring = desc.spring;
  damper = desc.damper;
  targetValue = desc.targetValue;
}

void NxSpringDesc::copyTo(::NxSpringDesc &desc,CustomCopy &cc)
{
  desc.spring = spring;
  desc.damper = damper;
  desc.targetValue = targetValue;
}


//***********************************************************************************
// Constructor for 'NxMaterialDesc'
//***********************************************************************************
NxMaterialDesc::NxMaterialDesc(void)
{
  ::NxMaterialDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  mHasSpring                                    = false;
  mMaterialIndex                                = 0;
  dynamicFriction                               = def.dynamicFriction;
  staticFriction                                = def.staticFriction;
  restitution                                   = def.restitution;
  dynamicFrictionV                              = def.dynamicFrictionV;
  staticFrictionV                               = def.staticFrictionV;
  frictionCombineMode                           = NX_CM_AVERAGE;
  restitutionCombineMode                        = NX_CM_AVERAGE;
  dirOfAnisotropy                               = def.dirOfAnisotropy;
  flags                                         = (NxMaterialFlag) def.flags;
  // NxSpringDesc: mSpring
  mInstanceIndex = 0;
}

NxMaterialDesc::~NxMaterialDesc(void)
{
}

void NxMaterialDesc::store(SchemaStream &stream,const char *parent)
{
  NxMaterialDesc def;
  stream.beginHeader(SC_NxMaterialDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mHasSpring != mHasSpring )
    stream.store(mHasSpring,"mHasSpring",true);
  if ( gSaveDefaults || def.mMaterialIndex != mMaterialIndex )
    stream.store(mMaterialIndex,"mMaterialIndex",true);
  if ( gSaveDefaults || def.dynamicFriction != dynamicFriction )
    stream.store(dynamicFriction,"dynamicFriction",false);
  if ( gSaveDefaults || def.staticFriction != staticFriction )
    stream.store(staticFriction,"staticFriction",false);
  if ( gSaveDefaults || def.restitution != restitution )
    stream.store(restitution,"restitution",false);
  if ( gSaveDefaults || def.dynamicFrictionV != dynamicFrictionV )
    stream.store(dynamicFrictionV,"dynamicFrictionV",false);
  if ( gSaveDefaults || def.staticFrictionV != staticFrictionV )
    stream.store(staticFrictionV,"staticFrictionV",false);
  if ( stream.isBinary() )
    stream.store((NxU32)frictionCombineMode,"frictionCombineMode",false);
   else
   {
     if ( gSaveDefaults || def.frictionCombineMode != frictionCombineMode )
      stream.store(EnumToString(frictionCombineMode),"frictionCombineMode",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)restitutionCombineMode,"restitutionCombineMode",false);
   else
   {
     if ( gSaveDefaults || def.restitutionCombineMode != restitutionCombineMode )
      stream.store(EnumToString(restitutionCombineMode),"restitutionCombineMode",false);
   }
  if ( gSaveDefaults || def.dirOfAnisotropy != dirOfAnisotropy )
    stream.store(dirOfAnisotropy,"dirOfAnisotropy",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxMaterialFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxMaterialFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( mHasSpring ) mSpring.store(stream,"mSpring");
  stream.endHeader();
}

void NxMaterialDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxMaterialDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mHasSpring,"mHasSpring",true);
    stream.load(mMaterialIndex,"mMaterialIndex",true);
    stream.load(dynamicFriction,"dynamicFriction",false);
    stream.load(staticFriction,"staticFriction",false);
    stream.load(restitution,"restitution",false);
    stream.load(dynamicFrictionV,"dynamicFrictionV",false);
    stream.load(staticFrictionV,"staticFrictionV",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"frictionCombineMode",false);
    frictionCombineMode = (NxCombineMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"frictionCombineMode",false);
     StringToEnum(enumName,frictionCombineMode);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"restitutionCombineMode",false);
    restitutionCombineMode = (NxCombineMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"restitutionCombineMode",false);
     StringToEnum(enumName,restitutionCombineMode);
  }
    stream.load(dirOfAnisotropy,"dirOfAnisotropy",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxMaterialFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxMaterialFlag,"flags") )
    {
      flags = (NxMaterialFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxMaterialFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxMaterialFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  if ( mHasSpring ) mSpring.load(stream,"mSpring");
    stream.endHeader();
  }
}

void NxMaterialDesc::copyFrom(const ::NxMaterialDesc &desc,CustomCopy &cc)
{
  dynamicFriction = desc.dynamicFriction;
  staticFriction = desc.staticFriction;
  restitution = desc.restitution;
  dynamicFrictionV = desc.dynamicFrictionV;
  staticFrictionV = desc.staticFrictionV;
  frictionCombineMode = (NxCombineMode) desc.frictionCombineMode;
  restitutionCombineMode = (NxCombineMode) desc.restitutionCombineMode;
  dirOfAnisotropy = desc.dirOfAnisotropy;
  flags = (NxMaterialFlag) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxMaterialDesc::copyTo(::NxMaterialDesc &desc,CustomCopy &cc)
{
  desc.dynamicFriction = dynamicFriction;
  desc.staticFriction = staticFriction;
  desc.restitution = restitution;
  desc.dynamicFrictionV = dynamicFrictionV;
  desc.staticFrictionV = staticFrictionV;
  desc.frictionCombineMode = (::NxCombineMode) frictionCombineMode;
  desc.restitutionCombineMode = (::NxCombineMode) restitutionCombineMode;
  desc.dirOfAnisotropy = dirOfAnisotropy;
  desc.flags = (::NxMaterialFlag) flags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxBodyDesc'
//***********************************************************************************
NxBodyDesc::NxBodyDesc(void)
{
  ::NxBodyDesc def;
  massLocalPose                                 = def.massLocalPose;
  massSpaceInertia                              = def.massSpaceInertia;
  mass                                          = def.mass;
  linearVelocity                                = def.linearVelocity;
  angularVelocity                               = def.angularVelocity;
  wakeUpCounter                                 = def.wakeUpCounter;
  linearDamping                                 = def.linearDamping;
  angularDamping                                = def.angularDamping;
  maxAngularVelocity                            = def.maxAngularVelocity;
  CCDMotionThreshold                            = def.CCDMotionThreshold;
  flags                                         = (NxBodyFlag) def.flags;
  sleepLinearVelocity                           = def.sleepLinearVelocity;
  sleepAngularVelocity                          = def.sleepAngularVelocity;
  solverIterationCount                          = def.solverIterationCount;
#if NX_SDK_VERSION_NUMBER >= 280
  contactReportThreshold                        = def.contactReportThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepEnergyThreshold                          = def.sleepEnergyThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepDamping                                  = def.sleepDamping;
#endif
  mInstance = 0;
}

NxBodyDesc::~NxBodyDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
}

void NxBodyDesc::store(SchemaStream &stream,const char *parent)
{
  NxBodyDesc def;
  stream.beginHeader(SC_NxBodyDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || !isSame(def.massLocalPose,massLocalPose) )
    stream.store(massLocalPose,"massLocalPose",false);
  if ( gSaveDefaults || def.massSpaceInertia != massSpaceInertia )
    stream.store(massSpaceInertia,"massSpaceInertia",false);
  if ( gSaveDefaults || def.mass != mass )
    stream.store(mass,"mass",false);
  if ( gSaveDefaults || def.linearVelocity != linearVelocity )
    stream.store(linearVelocity,"linearVelocity",false);
  if ( gSaveDefaults || def.angularVelocity != angularVelocity )
    stream.store(angularVelocity,"angularVelocity",false);
  if ( gSaveDefaults || def.wakeUpCounter != wakeUpCounter )
    stream.store(wakeUpCounter,"wakeUpCounter",false);
  if ( gSaveDefaults || def.linearDamping != linearDamping )
    stream.store(linearDamping,"linearDamping",false);
  if ( gSaveDefaults || def.angularDamping != angularDamping )
    stream.store(angularDamping,"angularDamping",false);
  if ( gSaveDefaults || def.maxAngularVelocity != maxAngularVelocity )
    stream.store(maxAngularVelocity,"maxAngularVelocity",false);
  if ( gSaveDefaults || def.CCDMotionThreshold != CCDMotionThreshold )
    stream.store(CCDMotionThreshold,"CCDMotionThreshold",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxBodyFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxBodyFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.sleepLinearVelocity != sleepLinearVelocity )
    stream.store(sleepLinearVelocity,"sleepLinearVelocity",false);
  if ( gSaveDefaults || def.sleepAngularVelocity != sleepAngularVelocity )
    stream.store(sleepAngularVelocity,"sleepAngularVelocity",false);
  if ( gSaveDefaults || def.solverIterationCount != solverIterationCount )
    stream.store(solverIterationCount,"solverIterationCount",false);
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.contactReportThreshold != contactReportThreshold )
    stream.store(contactReportThreshold,"contactReportThreshold",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.sleepEnergyThreshold != sleepEnergyThreshold )
    stream.store(sleepEnergyThreshold,"sleepEnergyThreshold",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.sleepDamping != sleepDamping )
    stream.store(sleepDamping,"sleepDamping",false);
#endif
  stream.endHeader();
}

void NxBodyDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxBodyDesc,parent) )
  {
    stream.load(massLocalPose,"massLocalPose",false);
    stream.load(massSpaceInertia,"massSpaceInertia",false);
    stream.load(mass,"mass",false);
    stream.load(linearVelocity,"linearVelocity",false);
    stream.load(angularVelocity,"angularVelocity",false);
    stream.load(wakeUpCounter,"wakeUpCounter",false);
    stream.load(linearDamping,"linearDamping",false);
    stream.load(angularDamping,"angularDamping",false);
    stream.load(maxAngularVelocity,"maxAngularVelocity",false);
    stream.load(CCDMotionThreshold,"CCDMotionThreshold",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxBodyFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxBodyFlag,"flags") )
    {
      flags = (NxBodyFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxBodyFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxBodyFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(sleepLinearVelocity,"sleepLinearVelocity",false);
    stream.load(sleepAngularVelocity,"sleepAngularVelocity",false);
    stream.load(solverIterationCount,"solverIterationCount",false);
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(contactReportThreshold,"contactReportThreshold",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(sleepEnergyThreshold,"sleepEnergyThreshold",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(sleepDamping,"sleepDamping",false);
#endif
    stream.endHeader();
  }
}

void NxBodyDesc::copyFrom(const ::NxBodyDesc &desc,CustomCopy &cc)
{
  massLocalPose = desc.massLocalPose;
  massSpaceInertia = desc.massSpaceInertia;
  mass = desc.mass;
  linearVelocity = desc.linearVelocity;
  angularVelocity = desc.angularVelocity;
  wakeUpCounter = desc.wakeUpCounter;
  linearDamping = desc.linearDamping;
  angularDamping = desc.angularDamping;
  maxAngularVelocity = desc.maxAngularVelocity;
  CCDMotionThreshold = desc.CCDMotionThreshold;
  flags = (NxBodyFlag) desc.flags;
  sleepLinearVelocity = desc.sleepLinearVelocity;
  sleepAngularVelocity = desc.sleepAngularVelocity;
  solverIterationCount = desc.solverIterationCount;
#if NX_SDK_VERSION_NUMBER >= 280
  contactReportThreshold = desc.contactReportThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepEnergyThreshold = desc.sleepEnergyThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepDamping = desc.sleepDamping;
#endif
}

void NxBodyDesc::copyTo(::NxBodyDesc &desc,CustomCopy &cc)
{
  desc.massLocalPose = massLocalPose;
  desc.massSpaceInertia = massSpaceInertia;
  desc.mass = mass;
  desc.linearVelocity = linearVelocity;
  desc.angularVelocity = angularVelocity;
  desc.wakeUpCounter = wakeUpCounter;
  desc.linearDamping = linearDamping;
  desc.angularDamping = angularDamping;
  desc.maxAngularVelocity = maxAngularVelocity;
  desc.CCDMotionThreshold = CCDMotionThreshold;
  desc.flags = (::NxBodyFlag) flags;
  desc.sleepLinearVelocity = sleepLinearVelocity;
  desc.sleepAngularVelocity = sleepAngularVelocity;
  desc.solverIterationCount = solverIterationCount;
#if NX_SDK_VERSION_NUMBER >= 280
  desc.contactReportThreshold = contactReportThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.sleepEnergyThreshold = sleepEnergyThreshold;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.sleepDamping = sleepDamping;
#endif
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxTireFunctionDesc'
//***********************************************************************************
NxTireFunctionDesc::NxTireFunctionDesc(void)
{
  ::NxTireFunctionDesc def;
  extremumSlip                                  = def.extremumSlip;
  extremumValue                                 = def.extremumValue;
  asymptoteSlip                                 = def.asymptoteSlip;
  asymptoteValue                                = def.asymptoteValue;
  stiffnessFactor                               = def.stiffnessFactor;
  mInstance = 0;
}

NxTireFunctionDesc::~NxTireFunctionDesc(void)
{
}

void NxTireFunctionDesc::store(SchemaStream &stream,const char *parent)
{
  NxTireFunctionDesc def;
  stream.beginHeader(SC_NxTireFunctionDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.extremumSlip != extremumSlip )
    stream.store(extremumSlip,"extremumSlip",false);
  if ( gSaveDefaults || def.extremumValue != extremumValue )
    stream.store(extremumValue,"extremumValue",false);
  if ( gSaveDefaults || def.asymptoteSlip != asymptoteSlip )
    stream.store(asymptoteSlip,"asymptoteSlip",false);
  if ( gSaveDefaults || def.asymptoteValue != asymptoteValue )
    stream.store(asymptoteValue,"asymptoteValue",false);
  if ( gSaveDefaults || def.stiffnessFactor != stiffnessFactor )
    stream.store(stiffnessFactor,"stiffnessFactor",false);
  stream.endHeader();
}

void NxTireFunctionDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxTireFunctionDesc,parent) )
  {
    stream.load(extremumSlip,"extremumSlip",false);
    stream.load(extremumValue,"extremumValue",false);
    stream.load(asymptoteSlip,"asymptoteSlip",false);
    stream.load(asymptoteValue,"asymptoteValue",false);
    stream.load(stiffnessFactor,"stiffnessFactor",false);
    stream.endHeader();
  }
}

void NxTireFunctionDesc::copyFrom(const ::NxTireFunctionDesc &desc,CustomCopy &cc)
{
  extremumSlip = desc.extremumSlip;
  extremumValue = desc.extremumValue;
  asymptoteSlip = desc.asymptoteSlip;
  asymptoteValue = desc.asymptoteValue;
  stiffnessFactor = desc.stiffnessFactor;
}

void NxTireFunctionDesc::copyTo(::NxTireFunctionDesc &desc,CustomCopy &cc)
{
  desc.extremumSlip = extremumSlip;
  desc.extremumValue = extremumValue;
  desc.asymptoteSlip = asymptoteSlip;
  desc.asymptoteValue = asymptoteValue;
  desc.stiffnessFactor = stiffnessFactor;
}


//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 250

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
// Constructor for 'NxShapeDesc'
//***********************************************************************************
NxShapeDesc::NxShapeDesc(void)
{
  ::NxBoxShapeDesc def;
  mUserProperties                               = 0;
  name                                          = def.name;
  localPose                                     = def.localPose;
  shapeFlags                                    = (NxShapeFlag) def.shapeFlags;
  group                                         = def.group;
  materialIndex                                 = def.materialIndex;
  mCCDSkeleton                                  = 0;
  mShapeDensity                                 = 1;
  mShapeMass                                    = -1;
  skinWidth                                     = def.skinWidth;
  // NxGroupsMask: groupsMask
#if NX_SDK_VERSION_NUMBER >= 280
  nonInteractingCompartmentTypes                = (NxShapeCompartmentType) def.nonInteractingCompartmentTypes;
#endif
  mInstance = 0;
  mType = SC_LAST;
}

NxShapeDesc::~NxShapeDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 280
#endif
}

void NxShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxShapeDesc def;
  stream.beginHeader(SC_NxShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || !isSame(def.localPose,localPose) )
    stream.store(localPose,"localPose",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)shapeFlags,"shapeFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.shapeFlags != shapeFlags )
    {
      stream.beginHeader(SC_NxShapeFlag);
      stream.store("shapeFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( shapeFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.group != group )
    stream.store(group,"group",false);
  if ( gSaveDefaults || def.materialIndex != materialIndex )
    stream.store(materialIndex,"materialIndex",false);
  if ( gSaveDefaults || def.mCCDSkeleton != mCCDSkeleton )
    stream.store(mCCDSkeleton,"mCCDSkeleton",false);
  if ( gSaveDefaults || def.mShapeDensity != mShapeDensity )
    stream.store(mShapeDensity,"mShapeDensity",false);
  if ( gSaveDefaults || def.mShapeMass != mShapeMass )
    stream.store(mShapeMass,"mShapeMass",false);
  if ( gSaveDefaults || def.skinWidth != skinWidth )
    stream.store(skinWidth,"skinWidth",false);
  groupsMask.store(stream,"groupsMask");
#if NX_SDK_VERSION_NUMBER >= 280
  if ( stream.isBinary() )
  {
    stream.store((NxU32)nonInteractingCompartmentTypes,"nonInteractingCompartmentTypes",false);
  }
  else
  {
    if ( gSaveDefaults || def.nonInteractingCompartmentTypes != nonInteractingCompartmentTypes )
    {
      stream.beginHeader(SC_NxShapeCompartmentType);
      stream.store("nonInteractingCompartmentTypes","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxShapeCompartmentType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( nonInteractingCompartmentTypes & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#endif
  stream.endHeader();
}

void NxShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxShapeDesc,parent) )
  {
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(localPose,"localPose",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"shapeFlags",false);
    shapeFlags = (NxShapeFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxShapeFlag,"shapeFlags") )
    {
      shapeFlags = (NxShapeFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	shapeFlags = (NxShapeFlag) (shapeFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(group,"group",false);
    stream.load(materialIndex,"materialIndex",false);
    stream.load(mCCDSkeleton,"mCCDSkeleton",false);
    stream.load(mShapeDensity,"mShapeDensity",false);
    stream.load(mShapeMass,"mShapeMass",false);
    stream.load(skinWidth,"skinWidth",false);
  groupsMask.load(stream,"groupsMask");
#if NX_SDK_VERSION_NUMBER >= 280
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"nonInteractingCompartmentTypes",false);
    nonInteractingCompartmentTypes = (NxShapeCompartmentType) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxShapeCompartmentType,"nonInteractingCompartmentTypes") )
    {
      nonInteractingCompartmentTypes = (NxShapeCompartmentType) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxShapeCompartmentType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	nonInteractingCompartmentTypes = (NxShapeCompartmentType) (nonInteractingCompartmentTypes|shift);
        }
      }
      stream.endHeader();
    }
  }
#endif
    stream.endHeader();
  }
}

void NxShapeDesc::copyFrom(const ::NxShapeDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  localPose = desc.localPose;
  shapeFlags = (NxShapeFlag) desc.shapeFlags;
  group = desc.group;
  materialIndex = desc.materialIndex;
  skinWidth = desc.skinWidth;
  groupsMask.copyFrom(desc.groupsMask,cc);
#if NX_SDK_VERSION_NUMBER >= 280
  nonInteractingCompartmentTypes = (NxShapeCompartmentType) desc.nonInteractingCompartmentTypes;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxShapeDesc::copyTo(::NxShapeDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.localPose = localPose;
  desc.shapeFlags = (::NxShapeFlag) shapeFlags;
  desc.group = group;
  desc.materialIndex = materialIndex;
  desc.skinWidth = skinWidth;
  groupsMask.copyTo(desc.groupsMask,cc);
#if NX_SDK_VERSION_NUMBER >= 280
  desc.nonInteractingCompartmentTypes = (::NxShapeCompartmentType) nonInteractingCompartmentTypes;
#endif
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxBoxShapeDesc'
//***********************************************************************************
NxBoxShapeDesc::NxBoxShapeDesc(void)
{
  ::NxBoxShapeDesc def;
  dimensions                                    = def.dimensions;
  mType = SC_NxBoxShapeDesc;
  mInstance = 0;
}

NxBoxShapeDesc::~NxBoxShapeDesc(void)
{
}

void NxBoxShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxBoxShapeDesc def;
  stream.beginHeader(SC_NxBoxShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.dimensions != dimensions )
    stream.store(dimensions,"dimensions",true);
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxBoxShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxBoxShapeDesc,parent) )
  {
    stream.load(dimensions,"dimensions",true);
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxBoxShapeDesc::copyFrom(const ::NxBoxShapeDesc &desc,CustomCopy &cc)
{
  dimensions = desc.dimensions;
  NxShapeDesc::copyFrom(desc,cc);
}

void NxBoxShapeDesc::copyTo(::NxBoxShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.dimensions = dimensions;
}


//***********************************************************************************
// Constructor for 'NxPlaneShapeDesc'
//***********************************************************************************
NxPlaneShapeDesc::NxPlaneShapeDesc(void)
{
  ::NxPlaneShapeDesc def;
  normal                                        = def.normal;
  d                                             = 0;
  mType = SC_NxPlaneShapeDesc;
  mInstance = 0;
}

NxPlaneShapeDesc::~NxPlaneShapeDesc(void)
{
}

void NxPlaneShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxPlaneShapeDesc def;
  stream.beginHeader(SC_NxPlaneShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.normal != normal )
    stream.store(normal,"normal",true);
  if ( gSaveDefaults || def.d != d )
    stream.store(d,"d",true);
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxPlaneShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxPlaneShapeDesc,parent) )
  {
    stream.load(normal,"normal",true);
    stream.load(d,"d",true);
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxPlaneShapeDesc::copyFrom(const ::NxPlaneShapeDesc &desc,CustomCopy &cc)
{
  normal = desc.normal;
  d = desc.d;
  NxShapeDesc::copyFrom(desc,cc);
}

void NxPlaneShapeDesc::copyTo(::NxPlaneShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.normal = normal;
  desc.d = d;
}


//***********************************************************************************
// Constructor for 'NxSphereShapeDesc'
//***********************************************************************************
NxSphereShapeDesc::NxSphereShapeDesc(void)
{
  ::NxSphereShapeDesc def;
  radius                                        = def.radius;
  mType = SC_NxSphereShapeDesc;
  mInstance = 0;
}

NxSphereShapeDesc::~NxSphereShapeDesc(void)
{
}

void NxSphereShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxSphereShapeDesc def;
  stream.beginHeader(SC_NxSphereShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.radius != radius )
    stream.store(radius,"radius",true);
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxSphereShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxSphereShapeDesc,parent) )
  {
    stream.load(radius,"radius",true);
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxSphereShapeDesc::copyFrom(const ::NxSphereShapeDesc &desc,CustomCopy &cc)
{
  radius = desc.radius;
  NxShapeDesc::copyFrom(desc,cc);
}

void NxSphereShapeDesc::copyTo(::NxSphereShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.radius = radius;
}


//***********************************************************************************
// Constructor for 'NxCapsuleShapeDesc'
//***********************************************************************************
NxCapsuleShapeDesc::NxCapsuleShapeDesc(void)
{
  ::NxCapsuleShapeDesc def;
  radius                                        = def.radius;
  height                                        = def.height;
  flags                                         = (NxCapsuleShapeFlag) def.flags;
  mType = SC_NxCapsuleShapeDesc;
  mInstance = 0;
}

NxCapsuleShapeDesc::~NxCapsuleShapeDesc(void)
{
}

void NxCapsuleShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxCapsuleShapeDesc def;
  stream.beginHeader(SC_NxCapsuleShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.radius != radius )
    stream.store(radius,"radius",true);
  if ( gSaveDefaults || def.height != height )
    stream.store(height,"height",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxCapsuleShapeFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxCapsuleShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxCapsuleShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxCapsuleShapeDesc,parent) )
  {
    stream.load(radius,"radius",true);
    stream.load(height,"height",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxCapsuleShapeFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxCapsuleShapeFlag,"flags") )
    {
      flags = (NxCapsuleShapeFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxCapsuleShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxCapsuleShapeFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxCapsuleShapeDesc::copyFrom(const ::NxCapsuleShapeDesc &desc,CustomCopy &cc)
{
  radius = desc.radius;
  height = desc.height;
  flags = (NxCapsuleShapeFlag) desc.flags;
  NxShapeDesc::copyFrom(desc,cc);
}

void NxCapsuleShapeDesc::copyTo(::NxCapsuleShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.radius = radius;
  desc.height = height;
  desc.flags = (::NxCapsuleShapeFlag) flags;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxConvexShapeDesc'
//***********************************************************************************
NxConvexShapeDesc::NxConvexShapeDesc(void)
{
  ::NxConvexShapeDesc def;
  mMeshData                                     = 0;
  meshFlags                                     = NX_MESH_SMOOTH_SPHERE_COLLISIONS;
  mType = SC_NxConvexShapeDesc;
  mInstance = 0;
}

NxConvexShapeDesc::~NxConvexShapeDesc(void)
{
}

void NxConvexShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxConvexShapeDesc def;
  stream.beginHeader(SC_NxConvexShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mMeshData != mMeshData )
    stream.store(mMeshData,"mMeshData",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)meshFlags,"meshFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.meshFlags != meshFlags )
    {
      stream.beginHeader(SC_NxMeshShapeFlag);
      stream.store("meshFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( meshFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxConvexShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxConvexShapeDesc,parent) )
  {
    stream.load(mMeshData,"mMeshData",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"meshFlags",false);
    meshFlags = (NxMeshShapeFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxMeshShapeFlag,"meshFlags") )
    {
      meshFlags = (NxMeshShapeFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	meshFlags = (NxMeshShapeFlag) (meshFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxConvexShapeDesc::copyFrom(const ::NxConvexShapeDesc &desc,CustomCopy &cc)
{
  meshFlags = (NxMeshShapeFlag) desc.meshFlags;
  cc.customCopyFrom(*this,desc);
  NxShapeDesc::copyFrom(desc,cc);
}

void NxConvexShapeDesc::copyTo(::NxConvexShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.meshFlags = (::NxMeshShapeFlag) meshFlags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxTriangleMeshShapeDesc'
//***********************************************************************************
NxTriangleMeshShapeDesc::NxTriangleMeshShapeDesc(void)
{
  ::NxTriangleMeshShapeDesc def;
  mMeshData                                     = 0;
  meshFlags                                     = (NxMeshShapeFlag) def.meshFlags;
#if NX_SDK_VERSION_NUMBER >= 250
  meshPagingMode                                = (NxMeshPagingMode) def.meshPagingMode;
#endif
  mType = SC_NxTriangleMeshShapeDesc;
  mInstance = 0;
}

NxTriangleMeshShapeDesc::~NxTriangleMeshShapeDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 250
#endif
}

void NxTriangleMeshShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxTriangleMeshShapeDesc def;
  stream.beginHeader(SC_NxTriangleMeshShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mMeshData != mMeshData )
    stream.store(mMeshData,"mMeshData",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)meshFlags,"meshFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.meshFlags != meshFlags )
    {
      stream.beginHeader(SC_NxMeshShapeFlag);
      stream.store("meshFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( meshFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
    stream.store((NxU32)meshPagingMode,"meshPagingMode",false);
   else
   {
     if ( gSaveDefaults || def.meshPagingMode != meshPagingMode )
      stream.store(EnumToString(meshPagingMode),"meshPagingMode",false);
   }
#endif
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxTriangleMeshShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxTriangleMeshShapeDesc,parent) )
  {
    stream.load(mMeshData,"mMeshData",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"meshFlags",false);
    meshFlags = (NxMeshShapeFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxMeshShapeFlag,"meshFlags") )
    {
      meshFlags = (NxMeshShapeFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	meshFlags = (NxMeshShapeFlag) (meshFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"meshPagingMode",false);
    meshPagingMode = (NxMeshPagingMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"meshPagingMode",false);
     StringToEnum(enumName,meshPagingMode);
  }
#endif
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxTriangleMeshShapeDesc::copyFrom(const ::NxTriangleMeshShapeDesc &desc,CustomCopy &cc)
{
  meshFlags = (NxMeshShapeFlag) desc.meshFlags;
#if NX_SDK_VERSION_NUMBER >= 250
  meshPagingMode = (NxMeshPagingMode) desc.meshPagingMode;
#endif
  cc.customCopyFrom(*this,desc);
  NxShapeDesc::copyFrom(desc,cc);
}

void NxTriangleMeshShapeDesc::copyTo(::NxTriangleMeshShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.meshFlags = (::NxMeshShapeFlag) meshFlags;
#if NX_SDK_VERSION_NUMBER >= 250
  desc.meshPagingMode = (::NxMeshPagingMode) meshPagingMode;
#endif
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxWheelShapeDesc'
//***********************************************************************************
NxWheelShapeDesc::NxWheelShapeDesc(void)
{
  ::NxWheelShapeDesc def;
  radius                                        = def.radius;
  suspensionTravel                              = def.suspensionTravel;
  // NxSpringDesc: suspension
  // NxTireFunctionDesc: longitudalTireForceFunction
  // NxTireFunctionDesc: lateralTireForceFunction
  inverseWheelMass                              = def.inverseWheelMass;
  wheelFlags                                    = (NxWheelShapeFlags) def.wheelFlags;
  motorTorque                                   = def.motorTorque;
  brakeTorque                                   = def.brakeTorque;
  steerAngle                                    = def.steerAngle;
  mType = SC_NxWheelShapeDesc;
  mInstance = 0;
}

NxWheelShapeDesc::~NxWheelShapeDesc(void)
{
}

void NxWheelShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxWheelShapeDesc def;
  stream.beginHeader(SC_NxWheelShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.radius != radius )
    stream.store(radius,"radius",false);
  if ( gSaveDefaults || def.suspensionTravel != suspensionTravel )
    stream.store(suspensionTravel,"suspensionTravel",false);
  suspension.store(stream,"suspension");
  longitudalTireForceFunction.store(stream,"longitudalTireForceFunction");
  lateralTireForceFunction.store(stream,"lateralTireForceFunction");
  if ( gSaveDefaults || def.inverseWheelMass != inverseWheelMass )
    stream.store(inverseWheelMass,"inverseWheelMass",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)wheelFlags,"wheelFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.wheelFlags != wheelFlags )
    {
      stream.beginHeader(SC_NxWheelShapeFlags);
      stream.store("wheelFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxWheelShapeFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( wheelFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.motorTorque != motorTorque )
    stream.store(motorTorque,"motorTorque",false);
  if ( gSaveDefaults || def.brakeTorque != brakeTorque )
    stream.store(brakeTorque,"brakeTorque",false);
  if ( gSaveDefaults || def.steerAngle != steerAngle )
    stream.store(steerAngle,"steerAngle",false);
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxWheelShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxWheelShapeDesc,parent) )
  {
    stream.load(radius,"radius",false);
    stream.load(suspensionTravel,"suspensionTravel",false);
  suspension.load(stream,"suspension");
  longitudalTireForceFunction.load(stream,"longitudalTireForceFunction");
  lateralTireForceFunction.load(stream,"lateralTireForceFunction");
    stream.load(inverseWheelMass,"inverseWheelMass",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"wheelFlags",false);
    wheelFlags = (NxWheelShapeFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxWheelShapeFlags,"wheelFlags") )
    {
      wheelFlags = (NxWheelShapeFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxWheelShapeFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	wheelFlags = (NxWheelShapeFlags) (wheelFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(motorTorque,"motorTorque",false);
    stream.load(brakeTorque,"brakeTorque",false);
    stream.load(steerAngle,"steerAngle",false);
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxWheelShapeDesc::copyFrom(const ::NxWheelShapeDesc &desc,CustomCopy &cc)
{
  radius = desc.radius;
  suspensionTravel = desc.suspensionTravel;
  suspension.copyFrom(desc.suspension,cc);
  longitudalTireForceFunction.copyFrom(desc.longitudalTireForceFunction,cc);
  lateralTireForceFunction.copyFrom(desc.lateralTireForceFunction,cc);
  inverseWheelMass = desc.inverseWheelMass;
  wheelFlags = (NxWheelShapeFlags) desc.wheelFlags;
  motorTorque = desc.motorTorque;
  brakeTorque = desc.brakeTorque;
  steerAngle = desc.steerAngle;
  NxShapeDesc::copyFrom(desc,cc);
}

void NxWheelShapeDesc::copyTo(::NxWheelShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.radius = radius;
  desc.suspensionTravel = suspensionTravel;
  suspension.copyTo(desc.suspension,cc);
  longitudalTireForceFunction.copyTo(desc.longitudalTireForceFunction,cc);
  lateralTireForceFunction.copyTo(desc.lateralTireForceFunction,cc);
  desc.inverseWheelMass = inverseWheelMass;
  desc.wheelFlags = (::NxWheelShapeFlags) wheelFlags;
  desc.motorTorque = motorTorque;
  desc.brakeTorque = brakeTorque;
  desc.steerAngle = steerAngle;
}


//***********************************************************************************
// Constructor for 'NxHeightFieldShapeDesc'
//***********************************************************************************
NxHeightFieldShapeDesc::NxHeightFieldShapeDesc(void)
{
  ::NxHeightFieldShapeDesc def;
  mHeightField                                  = 0;
  heightScale                                   = def.heightScale;
  rowScale                                      = def.rowScale;
  columnScale                                   = def.columnScale;
  materialIndexHighBits                         = def.materialIndexHighBits;
  holeMaterial                                  = def.holeMaterial;
  meshFlags                                     = (NxMeshShapeFlag) def.meshFlags;
  mType = SC_NxHeightFieldShapeDesc;
  mInstance = 0;
}

NxHeightFieldShapeDesc::~NxHeightFieldShapeDesc(void)
{
}

void NxHeightFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxHeightFieldShapeDesc def;
  stream.beginHeader(SC_NxHeightFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mHeightField != mHeightField )
    stream.store(mHeightField,"mHeightField",true);
  if ( gSaveDefaults || def.heightScale != heightScale )
    stream.store(heightScale,"heightScale",false);
  if ( gSaveDefaults || def.rowScale != rowScale )
    stream.store(rowScale,"rowScale",false);
  if ( gSaveDefaults || def.columnScale != columnScale )
    stream.store(columnScale,"columnScale",false);
  if ( gSaveDefaults || def.materialIndexHighBits != materialIndexHighBits )
    stream.store(materialIndexHighBits,"materialIndexHighBits",false);
  if ( gSaveDefaults || def.holeMaterial != holeMaterial )
    stream.store(holeMaterial,"holeMaterial",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)meshFlags,"meshFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.meshFlags != meshFlags )
    {
      stream.beginHeader(SC_NxMeshShapeFlag);
      stream.store("meshFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( meshFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::store(stream);
  stream.endHeader();
}

void NxHeightFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentShape(this);
  if ( stream.beginHeader(SC_NxHeightFieldShapeDesc,parent) )
  {
    stream.load(mHeightField,"mHeightField",true);
    stream.load(heightScale,"heightScale",false);
    stream.load(rowScale,"rowScale",false);
    stream.load(columnScale,"columnScale",false);
    stream.load(materialIndexHighBits,"materialIndexHighBits",false);
    stream.load(holeMaterial,"holeMaterial",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"meshFlags",false);
    meshFlags = (NxMeshShapeFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxMeshShapeFlag,"meshFlags") )
    {
      meshFlags = (NxMeshShapeFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxMeshShapeFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	meshFlags = (NxMeshShapeFlag) (meshFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
  NxShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxHeightFieldShapeDesc::copyFrom(const ::NxHeightFieldShapeDesc &desc,CustomCopy &cc)
{
  heightScale = desc.heightScale;
  rowScale = desc.rowScale;
  columnScale = desc.columnScale;
  materialIndexHighBits = desc.materialIndexHighBits;
  holeMaterial = desc.holeMaterial;
  meshFlags = (NxMeshShapeFlag) desc.meshFlags;
  cc.customCopyFrom(*this,desc);
  NxShapeDesc::copyFrom(desc,cc);
}

void NxHeightFieldShapeDesc::copyTo(::NxHeightFieldShapeDesc &desc,CustomCopy &cc)
{
  NxShapeDesc::copyTo(desc,cc);
  desc.heightScale = heightScale;
  desc.rowScale = rowScale;
  desc.columnScale = columnScale;
  desc.materialIndexHighBits = materialIndexHighBits;
  desc.holeMaterial = holeMaterial;
  desc.meshFlags = (::NxMeshShapeFlag) meshFlags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxActorDesc'
//***********************************************************************************
NxActorDesc::NxActorDesc(void)
{
  ::NxActorDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  mHasBody                                      = false;
  name                                          = def.name;
  globalPose                                    = def.globalPose;
  // NxBodyDesc: mBody
  density                                       = def.density;
  flags                                         = (NxActorFlag) def.flags;
  group                                         = def.group;
#if NX_SDK_VERSION_NUMBER >= 260
  mCompartment                                  = 0;
#endif
  // NxArray< NxShapeDesc *>: mShapes
#if NX_SDK_VERSION_NUMBER >= 272
  dominanceGroup                                = def.dominanceGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  contactReportFlags                            = def.contactReportFlags;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial                            = def.forceFieldMaterial;
#endif
  mInstance = 0;
  mColladaName = 0;
}

NxActorDesc::~NxActorDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 260
#endif
  for (NxU32 i=0; i<mShapes.size(); i++)
  {
     NxShapeDesc  *v = mShapes[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
}

void NxActorDesc::store(SchemaStream &stream,const char *parent)
{
  NxActorDesc def;
  stream.beginHeader(SC_NxActorDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mHasBody != mHasBody )
    stream.store(mHasBody,"mHasBody",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || !isSame(def.globalPose,globalPose) )
    stream.store(globalPose,"globalPose",false);
  if ( mHasBody ) mBody.store(stream,"mBody");
  if ( gSaveDefaults || def.density != density )
    stream.store(density,"density",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxActorFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxActorFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.group != group )
    stream.store(group,"group",false);
#if NX_SDK_VERSION_NUMBER >= 260
  if ( gSaveDefaults || def.mCompartment != mCompartment )
    stream.store(mCompartment,"mCompartment",false);
#endif
  for (NxU32 i=0; i<mShapes.size(); i++)
  {
    NxShapeDesc *v = mShapes[i];
    switch ( v->mType )
    {
      case SC_NxPlaneShapeDesc:
        if ( 1 )
        {
          NxPlaneShapeDesc *p = static_cast<NxPlaneShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxSphereShapeDesc:
        if ( 1 )
        {
          NxSphereShapeDesc *p = static_cast<NxSphereShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxBoxShapeDesc:
        if ( 1 )
        {
          NxBoxShapeDesc *p = static_cast<NxBoxShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxCapsuleShapeDesc:
        if ( 1 )
        {
          NxCapsuleShapeDesc *p = static_cast<NxCapsuleShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxWheelShapeDesc:
        if ( 1 )
        {
          NxWheelShapeDesc *p = static_cast<NxWheelShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxConvexShapeDesc:
        if ( 1 )
        {
          NxConvexShapeDesc *p = static_cast<NxConvexShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxTriangleMeshShapeDesc:
        if ( 1 )
        {
          NxTriangleMeshShapeDesc *p = static_cast<NxTriangleMeshShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxHeightFieldShapeDesc:
        if ( 1 )
        {
          NxHeightFieldShapeDesc *p = static_cast<NxHeightFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      default:
        break;
    }
  }

#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.dominanceGroup != dominanceGroup )
    stream.store(dominanceGroup,"dominanceGroup",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.contactReportFlags != contactReportFlags )
    stream.store(contactReportFlags,"contactReportFlags",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.forceFieldMaterial != forceFieldMaterial )
    stream.store(forceFieldMaterial,"forceFieldMaterial",false);
#endif
  stream.endHeader();
}

void NxActorDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentActor(this);
  if ( stream.beginHeader(SC_NxActorDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mHasBody,"mHasBody",true);
    stream.load(name,"name",true);
    stream.load(globalPose,"globalPose",false);
  if ( mHasBody ) mBody.load(stream,"mBody");
    stream.load(density,"density",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxActorFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxActorFlag,"flags") )
    {
      flags = (NxActorFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxActorFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxActorFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(group,"group",false);
#if NX_SDK_VERSION_NUMBER >= 260
    stream.load(mCompartment,"mCompartment",false);
#endif
    NxShapeDesc *shape = 0;
    do
    {
      shape = 0;
			NxI32 v=-1;

      stream.peekHeader(SC_NxPlaneShapeDesc,&v,
											  SC_NxSphereShapeDesc,
											  SC_NxBoxShapeDesc,
												SC_NxCapsuleShapeDesc,
												SC_NxWheelShapeDesc,
												SC_NxConvexShapeDesc,
												SC_NxTriangleMeshShapeDesc,
												SC_NxHeightFieldShapeDesc);
      switch ( v )
      {
        case SC_NxPlaneShapeDesc:
          if ( 1 )
          {
            NxPlaneShapeDesc *p = new NxPlaneShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxSphereShapeDesc:
          if ( 1 )
          {
            NxSphereShapeDesc *p = new NxSphereShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxBoxShapeDesc:
          if ( 1 )
          {
            NxBoxShapeDesc *p = new NxBoxShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxCapsuleShapeDesc:
          if ( 1 )
          {
            NxCapsuleShapeDesc *p = new NxCapsuleShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxWheelShapeDesc:
          if ( 1 )
          {
            NxWheelShapeDesc *p = new NxWheelShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxConvexShapeDesc:
          if ( 1 )
          {
            NxConvexShapeDesc *p = new NxConvexShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxTriangleMeshShapeDesc:
          if ( 1 )
          {
            NxTriangleMeshShapeDesc *p = new NxTriangleMeshShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        case SC_NxHeightFieldShapeDesc:
          if ( 1 )
          {
            NxHeightFieldShapeDesc *p = new NxHeightFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxShapeDesc *>(p);
          }
          break;
        default:
          break;
     }
     if ( shape )
     {
       mShapes.push_back(shape);
     }
   } while ( shape );
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(dominanceGroup,"dominanceGroup",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(contactReportFlags,"contactReportFlags",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(forceFieldMaterial,"forceFieldMaterial",false);
#endif
    stream.endHeader();
  }
}

void NxActorDesc::copyFrom(const ::NxActorDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  globalPose = desc.globalPose;
  density = desc.density;
  flags = (NxActorFlag) desc.flags;
  group = desc.group;
#if NX_SDK_VERSION_NUMBER >= 272
  dominanceGroup = desc.dominanceGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  contactReportFlags = desc.contactReportFlags;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial = desc.forceFieldMaterial;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxActorDesc::copyTo(::NxActorDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.globalPose = globalPose;
  desc.density = density;
  desc.flags = (::NxActorFlag) flags;
  desc.group = group;
#if NX_SDK_VERSION_NUMBER >= 272
  desc.dominanceGroup = dominanceGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.contactReportFlags = contactReportFlags;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.forceFieldMaterial = forceFieldMaterial;
#endif
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxPlaneInfoDesc'
//***********************************************************************************
NxPlaneInfoDesc::NxPlaneInfoDesc(void)
{
  mPlaneNormal.set(0,0,0);
  mPlaneD                                       = 0;
#if NX_SDK_VERSION_NUMBER >= 272
  restitution                                   = 0;
#endif
  mInstance = 0;
}

NxPlaneInfoDesc::~NxPlaneInfoDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 272
#endif
}

void NxPlaneInfoDesc::store(SchemaStream &stream,const char *parent)
{
  NxPlaneInfoDesc def;
  stream.beginHeader(SC_NxPlaneInfoDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mPlaneNormal != mPlaneNormal )
    stream.store(mPlaneNormal,"mPlaneNormal",false);
  if ( gSaveDefaults || def.mPlaneD != mPlaneD )
    stream.store(mPlaneD,"mPlaneD",false);
#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.restitution != restitution )
    stream.store(restitution,"restitution",false);
#endif
  stream.endHeader();
}

void NxPlaneInfoDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPlaneInfoDesc,parent) )
  {
    stream.load(mPlaneNormal,"mPlaneNormal",false);
    stream.load(mPlaneD,"mPlaneD",false);
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(restitution,"restitution",false);
#endif
    stream.endHeader();
  }
}


//***********************************************************************************
// Constructor for 'NxJointDesc'
//***********************************************************************************
NxJointDesc::NxJointDesc(void)
{
  ::NxFixedJointDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  mActor0                                       = 0;
  mActor1                                       = 0;
  localNormal0                                  = def.localNormal[0];
  localNormal1                                  = def.localNormal[1];
  localAxis0                                    = def.localAxis[0];
  localAxis1                                    = def.localAxis[1];
  localAnchor0                                  = def.localAnchor[0];
  localAnchor1                                  = def.localAnchor[1];
  maxForce                                      = def.maxForce;
  maxTorque                                     = def.maxTorque;
#if NX_SDK_VERSION_NUMBER >= 281
  solverExtrapolationFactor                     = def.solverExtrapolationFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  useAccelerationSpring                         = def.useAccelerationSpring;
#endif
  jointFlags                                    = NX_JF_VISUALIZATION;
  mPlaneLimitPoint.set(0,0,0);
  mOnActor2                                     = false;
  // NxArray< NxPlaneInfoDesc *>: mPlaneInfo
  mInstance = 0;
  mType = SC_LAST;
  mColladaName = 0;
}

NxJointDesc::~NxJointDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
  for (NxU32 i=0; i<mPlaneInfo.size(); i++)
  {
     NxPlaneInfoDesc  *v = mPlaneInfo[i];
    delete v;
  }

}

void NxJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointDesc def;
  stream.beginHeader(SC_NxJointDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || def.mActor0 != mActor0 )
    stream.store(mActor0,"mActor0",false);
  if ( gSaveDefaults || def.mActor1 != mActor1 )
    stream.store(mActor1,"mActor1",false);
  if ( gSaveDefaults || def.localNormal0 != localNormal0 )
    stream.store(localNormal0,"localNormal0",false);
  if ( gSaveDefaults || def.localNormal1 != localNormal1 )
    stream.store(localNormal1,"localNormal1",false);
  if ( gSaveDefaults || def.localAxis0 != localAxis0 )
    stream.store(localAxis0,"localAxis0",false);
  if ( gSaveDefaults || def.localAxis1 != localAxis1 )
    stream.store(localAxis1,"localAxis1",false);
  if ( gSaveDefaults || def.localAnchor0 != localAnchor0 )
    stream.store(localAnchor0,"localAnchor0",false);
  if ( gSaveDefaults || def.localAnchor1 != localAnchor1 )
    stream.store(localAnchor1,"localAnchor1",false);
  if ( gSaveDefaults || def.maxForce != maxForce )
    stream.store(maxForce,"maxForce",false);
  if ( gSaveDefaults || def.maxTorque != maxTorque )
    stream.store(maxTorque,"maxTorque",false);
#if NX_SDK_VERSION_NUMBER >= 281
  if ( gSaveDefaults || def.solverExtrapolationFactor != solverExtrapolationFactor )
    stream.store(solverExtrapolationFactor,"solverExtrapolationFactor",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  if ( gSaveDefaults || def.useAccelerationSpring != useAccelerationSpring )
    stream.store(useAccelerationSpring,"useAccelerationSpring",false);
#endif
  if ( stream.isBinary() )
  {
    stream.store((NxU32)jointFlags,"jointFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.jointFlags != jointFlags )
    {
      stream.beginHeader(SC_NxJointFlag);
      stream.store("jointFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( jointFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.mPlaneLimitPoint != mPlaneLimitPoint )
    stream.store(mPlaneLimitPoint,"mPlaneLimitPoint",false);
  if ( gSaveDefaults || def.mOnActor2 != mOnActor2 )
    stream.store(mOnActor2,"mOnActor2",false);
  for (NxU32 i=0; i<mPlaneInfo.size(); i++)
  {
    NxPlaneInfoDesc *v = mPlaneInfo[i];
    v->store(stream);
  }

  stream.endHeader();
}

void NxJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(mActor0,"mActor0",false);
    stream.load(mActor1,"mActor1",false);
    stream.load(localNormal0,"localNormal0",false);
    stream.load(localNormal1,"localNormal1",false);
    stream.load(localAxis0,"localAxis0",false);
    stream.load(localAxis1,"localAxis1",false);
    stream.load(localAnchor0,"localAnchor0",false);
    stream.load(localAnchor1,"localAnchor1",false);
    stream.load(maxForce,"maxForce",false);
    stream.load(maxTorque,"maxTorque",false);
#if NX_SDK_VERSION_NUMBER >= 281
    stream.load(solverExtrapolationFactor,"solverExtrapolationFactor",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 281
    stream.load(useAccelerationSpring,"useAccelerationSpring",false);
#endif
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"jointFlags",false);
    jointFlags = (NxJointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxJointFlag,"jointFlags") )
    {
      jointFlags = (NxJointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	jointFlags = (NxJointFlag) (jointFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(mPlaneLimitPoint,"mPlaneLimitPoint",false);
    stream.load(mOnActor2,"mOnActor2",false);
  while ( stream.peekHeader(SC_NxPlaneInfoDesc) )
  {
     NxPlaneInfoDesc *temp = new NxPlaneInfoDesc;
     temp->load(stream);
     mPlaneInfo.push_back(temp);
   }
    stream.endHeader();
  }
}

void NxJointDesc::copyFrom(const ::NxJointDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  localNormal0 = desc.localNormal[0];
  localNormal1 = desc.localNormal[1];
  localAxis0 = desc.localAxis[0];
  localAxis1 = desc.localAxis[1];
  localAnchor0 = desc.localAnchor[0];
  localAnchor1 = desc.localAnchor[1];
  maxForce = desc.maxForce;
  maxTorque = desc.maxTorque;
#if NX_SDK_VERSION_NUMBER >= 281
  solverExtrapolationFactor = desc.solverExtrapolationFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  useAccelerationSpring = desc.useAccelerationSpring;
#endif
  jointFlags = (NxJointFlag) desc.jointFlags;
  cc.customCopyFrom(*this,desc);
}

void NxJointDesc::copyTo(::NxJointDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.localNormal[0] = localNormal0;
  desc.localNormal[1] = localNormal1;
  desc.localAxis[0] = localAxis0;
  desc.localAxis[1] = localAxis1;
  desc.localAnchor[0] = localAnchor0;
  desc.localAnchor[1] = localAnchor1;
  desc.maxForce = maxForce;
  desc.maxTorque = maxTorque;
#if NX_SDK_VERSION_NUMBER >= 281
  desc.solverExtrapolationFactor = solverExtrapolationFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  desc.useAccelerationSpring = useAccelerationSpring;
#endif
  desc.jointFlags = (::NxJointFlag) jointFlags;
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxCylindricalJointDesc'
//***********************************************************************************
NxCylindricalJointDesc::NxCylindricalJointDesc(void)
{
  ::NxCylindricalJointDesc def;
  mType = SC_NxCylindricalJointDesc;
  mInstance = 0;
}

NxCylindricalJointDesc::~NxCylindricalJointDesc(void)
{
}

void NxCylindricalJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxCylindricalJointDesc def;
  stream.beginHeader(SC_NxCylindricalJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxCylindricalJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxCylindricalJointDesc,parent) )
  {
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxCylindricalJointDesc::copyFrom(const ::NxCylindricalJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyFrom(desc,cc);
}

void NxCylindricalJointDesc::copyTo(::NxCylindricalJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxJointLimitSoftDesc'
//***********************************************************************************
NxJointLimitSoftDesc::NxJointLimitSoftDesc(void)
{
  ::NxJointLimitSoftDesc def;
  value                                         = def.value;
  restitution                                   = def.restitution;
  spring                                        = def.spring;
  damping                                       = def.damping;
  mInstance = 0;
}

NxJointLimitSoftDesc::~NxJointLimitSoftDesc(void)
{
}

void NxJointLimitSoftDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointLimitSoftDesc def;
  stream.beginHeader(SC_NxJointLimitSoftDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.value != value )
    stream.store(value,"value",false);
  if ( gSaveDefaults || def.restitution != restitution )
    stream.store(restitution,"restitution",false);
  if ( gSaveDefaults || def.spring != spring )
    stream.store(spring,"spring",false);
  if ( gSaveDefaults || def.damping != damping )
    stream.store(damping,"damping",false);
  stream.endHeader();
}

void NxJointLimitSoftDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointLimitSoftDesc,parent) )
  {
    stream.load(value,"value",false);
    stream.load(restitution,"restitution",false);
    stream.load(spring,"spring",false);
    stream.load(damping,"damping",false);
    stream.endHeader();
  }
}

void NxJointLimitSoftDesc::copyFrom(const ::NxJointLimitSoftDesc &desc,CustomCopy &cc)
{
  value = desc.value;
  restitution = desc.restitution;
  spring = desc.spring;
  damping = desc.damping;
}

void NxJointLimitSoftDesc::copyTo(::NxJointLimitSoftDesc &desc,CustomCopy &cc)
{
  desc.value = value;
  desc.restitution = restitution;
  desc.spring = spring;
  desc.damping = damping;
}


//***********************************************************************************
// Constructor for 'NxJointLimitSoftPairDesc'
//***********************************************************************************
NxJointLimitSoftPairDesc::NxJointLimitSoftPairDesc(void)
{
  ::NxJointLimitSoftPairDesc def;
  // NxJointLimitSoftDesc: low
  // NxJointLimitSoftDesc: high
  mInstance = 0;
}

NxJointLimitSoftPairDesc::~NxJointLimitSoftPairDesc(void)
{
}

void NxJointLimitSoftPairDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointLimitSoftPairDesc def;
  stream.beginHeader(SC_NxJointLimitSoftPairDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  low.store(stream,"low");
  high.store(stream,"high");
  stream.endHeader();
}

void NxJointLimitSoftPairDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointLimitSoftPairDesc,parent) )
  {
  low.load(stream,"low");
  high.load(stream,"high");
    stream.endHeader();
  }
}

void NxJointLimitSoftPairDesc::copyFrom(const ::NxJointLimitSoftPairDesc &desc,CustomCopy &cc)
{
  low.copyFrom(desc.low,cc);
  high.copyFrom(desc.high,cc);
}

void NxJointLimitSoftPairDesc::copyTo(::NxJointLimitSoftPairDesc &desc,CustomCopy &cc)
{
  low.copyTo(desc.low,cc);
  high.copyTo(desc.high,cc);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxJointDriveDesc'
//***********************************************************************************
NxJointDriveDesc::NxJointDriveDesc(void)
{
  ::NxJointDriveDesc def;
  driveType                                     = (NxD6JointDriveType) def.driveType.bitField;
  spring                                        = def.spring;
  damping                                       = def.damping;
  forceLimit                                    = def.forceLimit;
  mInstance = 0;
}

NxJointDriveDesc::~NxJointDriveDesc(void)
{
}

void NxJointDriveDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointDriveDesc def;
  stream.beginHeader(SC_NxJointDriveDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)driveType,"driveType",false);
  }
  else
  {
    if ( gSaveDefaults || def.driveType != driveType )
    {
      stream.beginHeader(SC_NxD6JointDriveType);
      stream.store("driveType","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxD6JointDriveType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( driveType & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.spring != spring )
    stream.store(spring,"spring",false);
  if ( gSaveDefaults || def.damping != damping )
    stream.store(damping,"damping",false);
  if ( gSaveDefaults || def.forceLimit != forceLimit )
    stream.store(forceLimit,"forceLimit",false);
  stream.endHeader();
}

void NxJointDriveDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointDriveDesc,parent) )
  {
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"driveType",false);
    driveType = (NxD6JointDriveType) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxD6JointDriveType,"driveType") )
    {
      driveType = (NxD6JointDriveType) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxD6JointDriveType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	driveType = (NxD6JointDriveType) (driveType|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(spring,"spring",false);
    stream.load(damping,"damping",false);
    stream.load(forceLimit,"forceLimit",false);
    stream.endHeader();
  }
}

void NxJointDriveDesc::copyFrom(const ::NxJointDriveDesc &desc,CustomCopy &cc)
{
  driveType = (NxD6JointDriveType) desc.driveType.bitField;
  spring = desc.spring;
  damping = desc.damping;
  forceLimit = desc.forceLimit;
}

void NxJointDriveDesc::copyTo(::NxJointDriveDesc &desc,CustomCopy &cc)
{
  desc.driveType.bitField = driveType;
  desc.spring = spring;
  desc.damping = damping;
  desc.forceLimit = forceLimit;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxD6JointDesc'
//***********************************************************************************
NxD6JointDesc::NxD6JointDesc(void)
{
  ::NxD6JointDesc def;
  xMotion                                       = (NxD6JointMotion) def.xMotion;
  yMotion                                       = (NxD6JointMotion) def.yMotion;
  zMotion                                       = (NxD6JointMotion) def.zMotion;
  swing1Motion                                  = (NxD6JointMotion) def.swing1Motion;
  swing2Motion                                  = (NxD6JointMotion) def.swing2Motion;
  twistMotion                                   = (NxD6JointMotion) def.twistMotion;
  // NxJointLimitSoftDesc: linearLimit
  // NxJointLimitSoftDesc: swing1Limit
  // NxJointLimitSoftDesc: swing2Limit
  // NxJointLimitSoftPairDesc: twistLimit
  // NxJointDriveDesc: xDrive
  // NxJointDriveDesc: yDrive
  // NxJointDriveDesc: zDrive
  // NxJointDriveDesc: swingDrive
  // NxJointDriveDesc: twistDrive
  // NxJointDriveDesc: slerpDrive
  drivePosition                                 = def.drivePosition;
  driveOrientation                              = def.driveOrientation;
  driveLinearVelocity                           = def.driveLinearVelocity;
  driveAngularVelocity                          = def.driveAngularVelocity;
  projectionMode                                = (NxJointProjectionMode) def.projectionMode;
  projectionDistance                            = def.projectionDistance;
  projectionAngle                               = def.projectionAngle;
  gearRatio                                     = def.gearRatio;
  flags                                         = (NxD6JointFlag) def.flags;
  mType = SC_NxD6JointDesc;
  mInstance = 0;
}

NxD6JointDesc::~NxD6JointDesc(void)
{
}

void NxD6JointDesc::store(SchemaStream &stream,const char *parent)
{
  NxD6JointDesc def;
  stream.beginHeader(SC_NxD6JointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( stream.isBinary() )
    stream.store((NxU32)xMotion,"xMotion",false);
   else
   {
     if ( gSaveDefaults || def.xMotion != xMotion )
      stream.store(EnumToString(xMotion),"xMotion",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)yMotion,"yMotion",false);
   else
   {
     if ( gSaveDefaults || def.yMotion != yMotion )
      stream.store(EnumToString(yMotion),"yMotion",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)zMotion,"zMotion",false);
   else
   {
     if ( gSaveDefaults || def.zMotion != zMotion )
      stream.store(EnumToString(zMotion),"zMotion",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)swing1Motion,"swing1Motion",false);
   else
   {
     if ( gSaveDefaults || def.swing1Motion != swing1Motion )
      stream.store(EnumToString(swing1Motion),"swing1Motion",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)swing2Motion,"swing2Motion",false);
   else
   {
     if ( gSaveDefaults || def.swing2Motion != swing2Motion )
      stream.store(EnumToString(swing2Motion),"swing2Motion",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)twistMotion,"twistMotion",false);
   else
   {
     if ( gSaveDefaults || def.twistMotion != twistMotion )
      stream.store(EnumToString(twistMotion),"twistMotion",false);
   }
  linearLimit.store(stream,"linearLimit");
  swing1Limit.store(stream,"swing1Limit");
  swing2Limit.store(stream,"swing2Limit");
  twistLimit.store(stream,"twistLimit");
  xDrive.store(stream,"xDrive");
  yDrive.store(stream,"yDrive");
  zDrive.store(stream,"zDrive");
  swingDrive.store(stream,"swingDrive");
  twistDrive.store(stream,"twistDrive");
  slerpDrive.store(stream,"slerpDrive");
  if ( gSaveDefaults || def.drivePosition != drivePosition )
    stream.store(drivePosition,"drivePosition",false);
  if ( gSaveDefaults || !isSame(def.driveOrientation,driveOrientation) )
    stream.store(driveOrientation,"driveOrientation",false);
  if ( gSaveDefaults || def.driveLinearVelocity != driveLinearVelocity )
    stream.store(driveLinearVelocity,"driveLinearVelocity",false);
  if ( gSaveDefaults || def.driveAngularVelocity != driveAngularVelocity )
    stream.store(driveAngularVelocity,"driveAngularVelocity",false);
  if ( stream.isBinary() )
    stream.store((NxU32)projectionMode,"projectionMode",false);
   else
   {
     if ( gSaveDefaults || def.projectionMode != projectionMode )
      stream.store(EnumToString(projectionMode),"projectionMode",false);
   }
  if ( gSaveDefaults || def.projectionDistance != projectionDistance )
    stream.store(projectionDistance,"projectionDistance",false);
  if ( gSaveDefaults || def.projectionAngle != projectionAngle )
    stream.store(projectionAngle,"projectionAngle",false);
  if ( gSaveDefaults || def.gearRatio != gearRatio )
    stream.store(gearRatio,"gearRatio",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxD6JointFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxD6JointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxD6JointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxD6JointDesc,parent) )
  {
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"xMotion",false);
    xMotion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"xMotion",false);
     StringToEnum(enumName,xMotion);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"yMotion",false);
    yMotion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"yMotion",false);
     StringToEnum(enumName,yMotion);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"zMotion",false);
    zMotion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"zMotion",false);
     StringToEnum(enumName,zMotion);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"swing1Motion",false);
    swing1Motion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"swing1Motion",false);
     StringToEnum(enumName,swing1Motion);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"swing2Motion",false);
    swing2Motion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"swing2Motion",false);
     StringToEnum(enumName,swing2Motion);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"twistMotion",false);
    twistMotion = (NxD6JointMotion) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"twistMotion",false);
     StringToEnum(enumName,twistMotion);
  }
  linearLimit.load(stream,"linearLimit");
  swing1Limit.load(stream,"swing1Limit");
  swing2Limit.load(stream,"swing2Limit");
  twistLimit.load(stream,"twistLimit");
  xDrive.load(stream,"xDrive");
  yDrive.load(stream,"yDrive");
  zDrive.load(stream,"zDrive");
  swingDrive.load(stream,"swingDrive");
  twistDrive.load(stream,"twistDrive");
  slerpDrive.load(stream,"slerpDrive");
    stream.load(drivePosition,"drivePosition",false);
    stream.load(driveOrientation,"driveOrientation",false);
    stream.load(driveLinearVelocity,"driveLinearVelocity",false);
    stream.load(driveAngularVelocity,"driveAngularVelocity",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"projectionMode",false);
    projectionMode = (NxJointProjectionMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"projectionMode",false);
     StringToEnum(enumName,projectionMode);
  }
    stream.load(projectionDistance,"projectionDistance",false);
    stream.load(projectionAngle,"projectionAngle",false);
    stream.load(gearRatio,"gearRatio",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxD6JointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxD6JointFlag,"flags") )
    {
      flags = (NxD6JointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxD6JointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxD6JointFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxD6JointDesc::copyFrom(const ::NxD6JointDesc &desc,CustomCopy &cc)
{
  xMotion = (NxD6JointMotion) desc.xMotion;
  yMotion = (NxD6JointMotion) desc.yMotion;
  zMotion = (NxD6JointMotion) desc.zMotion;
  swing1Motion = (NxD6JointMotion) desc.swing1Motion;
  swing2Motion = (NxD6JointMotion) desc.swing2Motion;
  twistMotion = (NxD6JointMotion) desc.twistMotion;
  linearLimit.copyFrom(desc.linearLimit,cc);
  swing1Limit.copyFrom(desc.swing1Limit,cc);
  swing2Limit.copyFrom(desc.swing2Limit,cc);
  twistLimit.copyFrom(desc.twistLimit,cc);
  xDrive.copyFrom(desc.xDrive,cc);
  yDrive.copyFrom(desc.yDrive,cc);
  zDrive.copyFrom(desc.zDrive,cc);
  swingDrive.copyFrom(desc.swingDrive,cc);
  twistDrive.copyFrom(desc.twistDrive,cc);
  slerpDrive.copyFrom(desc.slerpDrive,cc);
  drivePosition = desc.drivePosition;
  driveOrientation = desc.driveOrientation;
  driveLinearVelocity = desc.driveLinearVelocity;
  driveAngularVelocity = desc.driveAngularVelocity;
  projectionMode = (NxJointProjectionMode) desc.projectionMode;
  projectionDistance = desc.projectionDistance;
  projectionAngle = desc.projectionAngle;
  gearRatio = desc.gearRatio;
  flags = (NxD6JointFlag) desc.flags;
  NxJointDesc::copyFrom(desc,cc);
}

void NxD6JointDesc::copyTo(::NxD6JointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
  desc.xMotion = (::NxD6JointMotion) xMotion;
  desc.yMotion = (::NxD6JointMotion) yMotion;
  desc.zMotion = (::NxD6JointMotion) zMotion;
  desc.swing1Motion = (::NxD6JointMotion) swing1Motion;
  desc.swing2Motion = (::NxD6JointMotion) swing2Motion;
  desc.twistMotion = (::NxD6JointMotion) twistMotion;
  linearLimit.copyTo(desc.linearLimit,cc);
  swing1Limit.copyTo(desc.swing1Limit,cc);
  swing2Limit.copyTo(desc.swing2Limit,cc);
  twistLimit.copyTo(desc.twistLimit,cc);
  xDrive.copyTo(desc.xDrive,cc);
  yDrive.copyTo(desc.yDrive,cc);
  zDrive.copyTo(desc.zDrive,cc);
  swingDrive.copyTo(desc.swingDrive,cc);
  twistDrive.copyTo(desc.twistDrive,cc);
  slerpDrive.copyTo(desc.slerpDrive,cc);
  desc.drivePosition = drivePosition;
  desc.driveOrientation = driveOrientation;
  desc.driveLinearVelocity = driveLinearVelocity;
  desc.driveAngularVelocity = driveAngularVelocity;
  desc.projectionMode = (::NxJointProjectionMode) projectionMode;
  desc.projectionDistance = projectionDistance;
  desc.projectionAngle = projectionAngle;
  desc.gearRatio = gearRatio;
  desc.flags = (::NxD6JointFlag) flags;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxDistanceJointDesc'
//***********************************************************************************
NxDistanceJointDesc::NxDistanceJointDesc(void)
{
  ::NxDistanceJointDesc def;
  maxDistance                                   = def.maxDistance;
  minDistance                                   = def.minDistance;
  // NxSpringDesc: spring
  flags                                         = (NxDistanceJointFlag) def.flags;
  mType = SC_NxDistanceJointDesc;
  mInstance = 0;
}

NxDistanceJointDesc::~NxDistanceJointDesc(void)
{
}

void NxDistanceJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxDistanceJointDesc def;
  stream.beginHeader(SC_NxDistanceJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.maxDistance != maxDistance )
    stream.store(maxDistance,"maxDistance",false);
  if ( gSaveDefaults || def.minDistance != minDistance )
    stream.store(minDistance,"minDistance",false);
  spring.store(stream,"spring");
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxDistanceJointFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxDistanceJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxDistanceJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxDistanceJointDesc,parent) )
  {
    stream.load(maxDistance,"maxDistance",false);
    stream.load(minDistance,"minDistance",false);
  spring.load(stream,"spring");
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxDistanceJointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxDistanceJointFlag,"flags") )
    {
      flags = (NxDistanceJointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxDistanceJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxDistanceJointFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxDistanceJointDesc::copyFrom(const ::NxDistanceJointDesc &desc,CustomCopy &cc)
{
  maxDistance = desc.maxDistance;
  minDistance = desc.minDistance;
  spring.copyFrom(desc.spring,cc);
  flags = (NxDistanceJointFlag) desc.flags;
  NxJointDesc::copyFrom(desc,cc);
}

void NxDistanceJointDesc::copyTo(::NxDistanceJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
  desc.maxDistance = maxDistance;
  desc.minDistance = minDistance;
  spring.copyTo(desc.spring,cc);
  desc.flags = (::NxDistanceJointFlag) flags;
}


//***********************************************************************************
// Constructor for 'NxFixedJointDesc'
//***********************************************************************************
NxFixedJointDesc::NxFixedJointDesc(void)
{
  ::NxFixedJointDesc def;
  mType = SC_NxFixedJointDesc;
  mInstance = 0;
}

NxFixedJointDesc::~NxFixedJointDesc(void)
{
}

void NxFixedJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxFixedJointDesc def;
  stream.beginHeader(SC_NxFixedJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxFixedJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxFixedJointDesc,parent) )
  {
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxFixedJointDesc::copyFrom(const ::NxFixedJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyFrom(desc,cc);
}

void NxFixedJointDesc::copyTo(::NxFixedJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
}


//***********************************************************************************
// Constructor for 'NxPointInPlaneJointDesc'
//***********************************************************************************
NxPointInPlaneJointDesc::NxPointInPlaneJointDesc(void)
{
  ::NxPointInPlaneJointDesc def;
  mType = SC_NxPointInPlaneJointDesc;
  mInstance = 0;
}

NxPointInPlaneJointDesc::~NxPointInPlaneJointDesc(void)
{
}

void NxPointInPlaneJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxPointInPlaneJointDesc def;
  stream.beginHeader(SC_NxPointInPlaneJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxPointInPlaneJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPointInPlaneJointDesc,parent) )
  {
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxPointInPlaneJointDesc::copyFrom(const ::NxPointInPlaneJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyFrom(desc,cc);
}

void NxPointInPlaneJointDesc::copyTo(::NxPointInPlaneJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
}


//***********************************************************************************
// Constructor for 'NxPointOnLineJointDesc'
//***********************************************************************************
NxPointOnLineJointDesc::NxPointOnLineJointDesc(void)
{
  ::NxPointOnLineJointDesc def;
  mType = SC_NxPointOnLineJointDesc;
  mInstance = 0;
}

NxPointOnLineJointDesc::~NxPointOnLineJointDesc(void)
{
}

void NxPointOnLineJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxPointOnLineJointDesc def;
  stream.beginHeader(SC_NxPointOnLineJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxPointOnLineJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPointOnLineJointDesc,parent) )
  {
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxPointOnLineJointDesc::copyFrom(const ::NxPointOnLineJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyFrom(desc,cc);
}

void NxPointOnLineJointDesc::copyTo(::NxPointOnLineJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
}


//***********************************************************************************
// Constructor for 'NxPrismaticJointDesc'
//***********************************************************************************
NxPrismaticJointDesc::NxPrismaticJointDesc(void)
{
  ::NxPrismaticJointDesc def;
  mType = SC_NxPrismaticJointDesc;
  mInstance = 0;
}

NxPrismaticJointDesc::~NxPrismaticJointDesc(void)
{
}

void NxPrismaticJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxPrismaticJointDesc def;
  stream.beginHeader(SC_NxPrismaticJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxPrismaticJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPrismaticJointDesc,parent) )
  {
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxPrismaticJointDesc::copyFrom(const ::NxPrismaticJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyFrom(desc,cc);
}

void NxPrismaticJointDesc::copyTo(::NxPrismaticJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
}


//***********************************************************************************
// Constructor for 'NxJointLimitDesc'
//***********************************************************************************
NxJointLimitDesc::NxJointLimitDesc(void)
{
  ::NxJointLimitDesc def;
  value                                         = def.value;
  restitution                                   = def.restitution;
  hardness                                      = def.hardness;
  mInstance = 0;
}

NxJointLimitDesc::~NxJointLimitDesc(void)
{
}

void NxJointLimitDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointLimitDesc def;
  stream.beginHeader(SC_NxJointLimitDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.value != value )
    stream.store(value,"value",false);
  if ( gSaveDefaults || def.restitution != restitution )
    stream.store(restitution,"restitution",false);
  if ( gSaveDefaults || def.hardness != hardness )
    stream.store(hardness,"hardness",false);
  stream.endHeader();
}

void NxJointLimitDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointLimitDesc,parent) )
  {
    stream.load(value,"value",false);
    stream.load(restitution,"restitution",false);
    stream.load(hardness,"hardness",false);
    stream.endHeader();
  }
}

void NxJointLimitDesc::copyFrom(const ::NxJointLimitDesc &desc,CustomCopy &cc)
{
  value = desc.value;
  restitution = desc.restitution;
  hardness = desc.hardness;
}

void NxJointLimitDesc::copyTo(::NxJointLimitDesc &desc,CustomCopy &cc)
{
  desc.value = value;
  desc.restitution = restitution;
  desc.hardness = hardness;
}


//***********************************************************************************
// Constructor for 'NxJointLimitPairDesc'
//***********************************************************************************
NxJointLimitPairDesc::NxJointLimitPairDesc(void)
{
  ::NxJointLimitPairDesc def;
  // NxJointLimitDesc: low
  // NxJointLimitDesc: high
  mInstance = 0;
}

NxJointLimitPairDesc::~NxJointLimitPairDesc(void)
{
}

void NxJointLimitPairDesc::store(SchemaStream &stream,const char *parent)
{
  NxJointLimitPairDesc def;
  stream.beginHeader(SC_NxJointLimitPairDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  low.store(stream,"low");
  high.store(stream,"high");
  stream.endHeader();
}

void NxJointLimitPairDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxJointLimitPairDesc,parent) )
  {
  low.load(stream,"low");
  high.load(stream,"high");
    stream.endHeader();
  }
}

void NxJointLimitPairDesc::copyFrom(const ::NxJointLimitPairDesc &desc,CustomCopy &cc)
{
  low.copyFrom(desc.low,cc);
  high.copyFrom(desc.high,cc);
}

void NxJointLimitPairDesc::copyTo(::NxJointLimitPairDesc &desc,CustomCopy &cc)
{
  low.copyTo(desc.low,cc);
  high.copyTo(desc.high,cc);
}


//***********************************************************************************
// Constructor for 'NxMotorDesc'
//***********************************************************************************
NxMotorDesc::NxMotorDesc(void)
{
  ::NxMotorDesc def;
  velTarget                                     = def.velTarget;
  maxForce                                      = def.maxForce;
  freeSpin                                      = def.freeSpin;
  mInstance = 0;
}

NxMotorDesc::~NxMotorDesc(void)
{
}

void NxMotorDesc::store(SchemaStream &stream,const char *parent)
{
  NxMotorDesc def;
  stream.beginHeader(SC_NxMotorDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.velTarget != velTarget )
    stream.store(velTarget,"velTarget",false);
  if ( gSaveDefaults || def.maxForce != maxForce )
    stream.store(maxForce,"maxForce",false);
  if ( gSaveDefaults || def.freeSpin != freeSpin )
    stream.store(freeSpin,"freeSpin",false);
  stream.endHeader();
}

void NxMotorDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxMotorDesc,parent) )
  {
    stream.load(velTarget,"velTarget",false);
    stream.load(maxForce,"maxForce",false);
    stream.load(freeSpin,"freeSpin",false);
    stream.endHeader();
  }
}

void NxMotorDesc::copyFrom(const ::NxMotorDesc &desc,CustomCopy &cc)
{
  velTarget = desc.velTarget;
  maxForce = desc.maxForce;
  freeSpin = desc.freeSpin;
}

void NxMotorDesc::copyTo(::NxMotorDesc &desc,CustomCopy &cc)
{
  desc.velTarget = velTarget;
  desc.maxForce = maxForce;
  desc.freeSpin = freeSpin;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxRevoluteJointDesc'
//***********************************************************************************
NxRevoluteJointDesc::NxRevoluteJointDesc(void)
{
  ::NxRevoluteJointDesc def;
  // NxJointLimitPairDesc: limit
  // NxMotorDesc: motor
  // NxSpringDesc: spring
  projectionDistance                            = def.projectionDistance;
  projectionAngle                               = def.projectionAngle;
  flags                                         = (NxRevoluteJointFlag) def.flags;
  projectionMode                                = (NxJointProjectionMode) def.projectionMode;
  mType = SC_NxRevoluteJointDesc;
  mInstance = 0;
}

NxRevoluteJointDesc::~NxRevoluteJointDesc(void)
{
}

void NxRevoluteJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxRevoluteJointDesc def;
  stream.beginHeader(SC_NxRevoluteJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  limit.store(stream,"limit");
  motor.store(stream,"motor");
  spring.store(stream,"spring");
  if ( gSaveDefaults || def.projectionDistance != projectionDistance )
    stream.store(projectionDistance,"projectionDistance",false);
  if ( gSaveDefaults || def.projectionAngle != projectionAngle )
    stream.store(projectionAngle,"projectionAngle",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxRevoluteJointFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxRevoluteJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
    stream.store((NxU32)projectionMode,"projectionMode",false);
   else
   {
     if ( gSaveDefaults || def.projectionMode != projectionMode )
      stream.store(EnumToString(projectionMode),"projectionMode",false);
   }
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxRevoluteJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxRevoluteJointDesc,parent) )
  {
  limit.load(stream,"limit");
  motor.load(stream,"motor");
  spring.load(stream,"spring");
    stream.load(projectionDistance,"projectionDistance",false);
    stream.load(projectionAngle,"projectionAngle",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxRevoluteJointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxRevoluteJointFlag,"flags") )
    {
      flags = (NxRevoluteJointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxRevoluteJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxRevoluteJointFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"projectionMode",false);
    projectionMode = (NxJointProjectionMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"projectionMode",false);
     StringToEnum(enumName,projectionMode);
  }
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxRevoluteJointDesc::copyFrom(const ::NxRevoluteJointDesc &desc,CustomCopy &cc)
{
  limit.copyFrom(desc.limit,cc);
  motor.copyFrom(desc.motor,cc);
  spring.copyFrom(desc.spring,cc);
  projectionDistance = desc.projectionDistance;
  projectionAngle = desc.projectionAngle;
  flags = (NxRevoluteJointFlag) desc.flags;
  projectionMode = (NxJointProjectionMode) desc.projectionMode;
  NxJointDesc::copyFrom(desc,cc);
}

void NxRevoluteJointDesc::copyTo(::NxRevoluteJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
  limit.copyTo(desc.limit,cc);
  motor.copyTo(desc.motor,cc);
  spring.copyTo(desc.spring,cc);
  desc.projectionDistance = projectionDistance;
  desc.projectionAngle = projectionAngle;
  desc.flags = (::NxRevoluteJointFlag) flags;
  desc.projectionMode = (::NxJointProjectionMode) projectionMode;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxSphericalJointDesc'
//***********************************************************************************
NxSphericalJointDesc::NxSphericalJointDesc(void)
{
  ::NxSphericalJointDesc def;
  swingAxis                                     = def.swingAxis;
  projectionDistance                            = def.projectionDistance;
  // NxJointLimitPairDesc: twistLimit
  // NxJointLimitDesc: swingLimit
  // NxSpringDesc: twistSpring
  // NxSpringDesc: swingSpring
  // NxSpringDesc: jointSpring
  flags                                         = (NxSphericalJointFlag) def.flags;
  projectionMode                                = (NxJointProjectionMode) def.projectionMode;
  mType = SC_NxSphericalJointDesc;
  mInstance = 0;
}

NxSphericalJointDesc::~NxSphericalJointDesc(void)
{
}

void NxSphericalJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxSphericalJointDesc def;
  stream.beginHeader(SC_NxSphericalJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.swingAxis != swingAxis )
    stream.store(swingAxis,"swingAxis",false);
  if ( gSaveDefaults || def.projectionDistance != projectionDistance )
    stream.store(projectionDistance,"projectionDistance",false);
  twistLimit.store(stream,"twistLimit");
  swingLimit.store(stream,"swingLimit");
  twistSpring.store(stream,"twistSpring");
  swingSpring.store(stream,"swingSpring");
  jointSpring.store(stream,"jointSpring");
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxSphericalJointFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxSphericalJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
    stream.store((NxU32)projectionMode,"projectionMode",false);
   else
   {
     if ( gSaveDefaults || def.projectionMode != projectionMode )
      stream.store(EnumToString(projectionMode),"projectionMode",false);
   }
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxSphericalJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSphericalJointDesc,parent) )
  {
    stream.load(swingAxis,"swingAxis",false);
    stream.load(projectionDistance,"projectionDistance",false);
  twistLimit.load(stream,"twistLimit");
  swingLimit.load(stream,"swingLimit");
  twistSpring.load(stream,"twistSpring");
  swingSpring.load(stream,"swingSpring");
  jointSpring.load(stream,"jointSpring");
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxSphericalJointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxSphericalJointFlag,"flags") )
    {
      flags = (NxSphericalJointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxSphericalJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxSphericalJointFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"projectionMode",false);
    projectionMode = (NxJointProjectionMode) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"projectionMode",false);
     StringToEnum(enumName,projectionMode);
  }
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxSphericalJointDesc::copyFrom(const ::NxSphericalJointDesc &desc,CustomCopy &cc)
{
  swingAxis = desc.swingAxis;
  projectionDistance = desc.projectionDistance;
  twistLimit.copyFrom(desc.twistLimit,cc);
  swingLimit.copyFrom(desc.swingLimit,cc);
  twistSpring.copyFrom(desc.twistSpring,cc);
  swingSpring.copyFrom(desc.swingSpring,cc);
  jointSpring.copyFrom(desc.jointSpring,cc);
  flags = (NxSphericalJointFlag) desc.flags;
  projectionMode = (NxJointProjectionMode) desc.projectionMode;
  NxJointDesc::copyFrom(desc,cc);
}

void NxSphericalJointDesc::copyTo(::NxSphericalJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
  desc.swingAxis = swingAxis;
  desc.projectionDistance = projectionDistance;
  twistLimit.copyTo(desc.twistLimit,cc);
  swingLimit.copyTo(desc.swingLimit,cc);
  twistSpring.copyTo(desc.twistSpring,cc);
  swingSpring.copyTo(desc.swingSpring,cc);
  jointSpring.copyTo(desc.jointSpring,cc);
  desc.flags = (::NxSphericalJointFlag) flags;
  desc.projectionMode = (::NxJointProjectionMode) projectionMode;
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxPulleyJointDesc'
//***********************************************************************************
NxPulleyJointDesc::NxPulleyJointDesc(void)
{
  ::NxPulleyJointDesc def;
  pulley0                                       = def.pulley[0];
  pulley1                                       = def.pulley[1];
  distance                                      = def.distance;
  stiffness                                     = def.stiffness;
  ratio                                         = false;
  flags                                         = (NxPulleyJointFlag) def.flags;
  // NxMotorDesc: motor
  mType = SC_NxPulleyJointDesc;
  mInstance = 0;
}

NxPulleyJointDesc::~NxPulleyJointDesc(void)
{
}

void NxPulleyJointDesc::store(SchemaStream &stream,const char *parent)
{
  NxPulleyJointDesc def;
  stream.beginHeader(SC_NxPulleyJointDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.pulley0 != pulley0 )
    stream.store(pulley0,"pulley0",false);
  if ( gSaveDefaults || def.pulley1 != pulley1 )
    stream.store(pulley1,"pulley1",false);
  if ( gSaveDefaults || def.distance != distance )
    stream.store(distance,"distance",false);
  if ( gSaveDefaults || def.stiffness != stiffness )
    stream.store(stiffness,"stiffness",false);
  if ( gSaveDefaults || def.ratio != ratio )
    stream.store(ratio,"ratio",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxPulleyJointFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxPulleyJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  motor.store(stream,"motor");
  NxJointDesc::store(stream);
  stream.endHeader();
}

void NxPulleyJointDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPulleyJointDesc,parent) )
  {
    stream.load(pulley0,"pulley0",false);
    stream.load(pulley1,"pulley1",false);
    stream.load(distance,"distance",false);
    stream.load(stiffness,"stiffness",false);
    stream.load(ratio,"ratio",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxPulleyJointFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxPulleyJointFlag,"flags") )
    {
      flags = (NxPulleyJointFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxPulleyJointFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxPulleyJointFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
  motor.load(stream,"motor");
  NxJointDesc::load(stream);
    stream.endHeader();
  }
}

void NxPulleyJointDesc::copyFrom(const ::NxPulleyJointDesc &desc,CustomCopy &cc)
{
  pulley0 = desc.pulley[0];
  pulley1 = desc.pulley[1];
  distance = desc.distance;
  stiffness = desc.stiffness;
  ratio = desc.ratio;
  flags = (NxPulleyJointFlag) desc.flags;
  motor.copyFrom(desc.motor,cc);
  NxJointDesc::copyFrom(desc,cc);
}

void NxPulleyJointDesc::copyTo(::NxPulleyJointDesc &desc,CustomCopy &cc)
{
  NxJointDesc::copyTo(desc,cc);
  desc.pulley[0] = pulley0;
  desc.pulley[1] = pulley1;
  desc.distance = distance;
  desc.stiffness = stiffness;
  desc.ratio = ratio;
  desc.flags = (::NxPulleyJointFlag) flags;
  motor.copyTo(desc.motor,cc);
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxPairFlagDesc'
//***********************************************************************************
NxPairFlagDesc::NxPairFlagDesc(void)
{
  mId                                           = 0;
  mUserProperties                               = 0;
  mIsActorPair                                  = false;
  mFlags                                        = NX_IGNORE_PAIR;
  mActor0                                       = 0;
  mShapeIndex0                                  = 0;
  mActor1                                       = 0;
  mShapeIndex1                                  = 0;
  mInstance = 0;
}

NxPairFlagDesc::~NxPairFlagDesc(void)
{
}

void NxPairFlagDesc::store(SchemaStream &stream,const char *parent)
{
  NxPairFlagDesc def;
  stream.beginHeader(SC_NxPairFlagDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mIsActorPair != mIsActorPair )
    stream.store(mIsActorPair,"mIsActorPair",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)mFlags,"mFlags",false);
  }
  else
  {
    if ( gSaveDefaults || def.mFlags != mFlags )
    {
      stream.beginHeader(SC_NxContactPairFlag);
      stream.store("mFlags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxContactPairFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( mFlags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.mActor0 != mActor0 )
    stream.store(mActor0,"mActor0",false);
  if ( gSaveDefaults || def.mShapeIndex0 != mShapeIndex0 )
    stream.store(mShapeIndex0,"mShapeIndex0",false);
  if ( gSaveDefaults || def.mActor1 != mActor1 )
    stream.store(mActor1,"mActor1",false);
  if ( gSaveDefaults || def.mShapeIndex1 != mShapeIndex1 )
    stream.store(mShapeIndex1,"mShapeIndex1",false);
  stream.endHeader();
}

void NxPairFlagDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPairFlagDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mIsActorPair,"mIsActorPair",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"mFlags",false);
    mFlags = (NxContactPairFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxContactPairFlag,"mFlags") )
    {
      mFlags = (NxContactPairFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxContactPairFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	mFlags = (NxContactPairFlag) (mFlags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(mActor0,"mActor0",false);
    stream.load(mShapeIndex0,"mShapeIndex0",false);
    stream.load(mActor1,"mActor1",false);
    stream.load(mShapeIndex1,"mShapeIndex1",false);
    stream.endHeader();
  }
}


//***********************************************************************************
// Constructor for 'NxCollisionGroupDesc'
//***********************************************************************************
NxCollisionGroupDesc::NxCollisionGroupDesc(void)
{
  mCollisionGroupA                              = 0;
  mCollisionGroupB                              = 0;
  mEnable                                       = false;
  mInstance = 0;
}

NxCollisionGroupDesc::~NxCollisionGroupDesc(void)
{
}

void NxCollisionGroupDesc::store(SchemaStream &stream,const char *parent)
{
  NxCollisionGroupDesc def;
  stream.beginHeader(SC_NxCollisionGroupDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mCollisionGroupA != mCollisionGroupA )
    stream.store(mCollisionGroupA,"mCollisionGroupA",true);
  if ( gSaveDefaults || def.mCollisionGroupB != mCollisionGroupB )
    stream.store(mCollisionGroupB,"mCollisionGroupB",true);
  if ( gSaveDefaults || def.mEnable != mEnable )
    stream.store(mEnable,"mEnable",true);
  stream.endHeader();
}

void NxCollisionGroupDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxCollisionGroupDesc,parent) )
  {
    stream.load(mCollisionGroupA,"mCollisionGroupA",true);
    stream.load(mCollisionGroupB,"mCollisionGroupB",true);
    stream.load(mEnable,"mEnable",true);
    stream.endHeader();
  }
}

#if NX_USE_FLUID_API

//***********************************************************************************
// Constructor for 'NxParticleData'
//***********************************************************************************
NxParticleData::NxParticleData(void)
{
  ::NxParticleData def;
  name                                          = def.name;
#if NX_SDK_VERSION_NUMBER < 270
  maxParticles                                  = def.maxParticles;
#endif
  // NxArray< NxVec3 >: mBufferPos
  // NxArray< NxVec3 >: mBufferVel
  // NxArray< NxF32 >: mBufferLife
  // NxArray< NxF32 >: mBufferDensity
#if NX_SDK_VERSION_NUMBER >= 260
  // NxArray< NxU32 >: mBufferId
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  // NxArray< NxU32 >: mBufferFlag
#endif
  mParticleCount                                = 0;
  mInstance = 0;
}

NxParticleData::~NxParticleData(void)
{
#if NX_SDK_VERSION_NUMBER < 270
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
}

void NxParticleData::store(SchemaStream &stream,const char *parent)
{
  NxParticleData def;
  stream.beginHeader(SC_NxParticleData);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
#if NX_SDK_VERSION_NUMBER < 270
  if ( gSaveDefaults || def.maxParticles != maxParticles )
    stream.store(maxParticles,"maxParticles",false);
#endif
  stream.store(mBufferPos,"mBufferPos",false);
  stream.store(mBufferVel,"mBufferVel",false);
  stream.store(mBufferLife,"mBufferLife",false);
  stream.store(mBufferDensity,"mBufferDensity",false);
#if NX_SDK_VERSION_NUMBER >= 260
  stream.store(mBufferId,"mBufferId",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  stream.store(mBufferFlag,"mBufferFlag",false);
#endif
  if ( gSaveDefaults || def.mParticleCount != mParticleCount )
    stream.store(mParticleCount,"mParticleCount",false);
  stream.endHeader();
}

void NxParticleData::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxParticleData,parent) )
  {
    stream.load(name,"name",true);
#if NX_SDK_VERSION_NUMBER < 270
    stream.load(maxParticles,"maxParticles",false);
#endif
  stream.load(mBufferPos,"mBufferPos",false);
  stream.load(mBufferVel,"mBufferVel",false);
  stream.load(mBufferLife,"mBufferLife",false);
  stream.load(mBufferDensity,"mBufferDensity",false);
#if NX_SDK_VERSION_NUMBER >= 260
  stream.load(mBufferId,"mBufferId",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  stream.load(mBufferFlag,"mBufferFlag",false);
#endif
    stream.load(mParticleCount,"mParticleCount",false);
    stream.endHeader();
  }
}

void NxParticleData::copyFrom(const ::NxParticleData &desc,CustomCopy &cc)
{
  name = desc.name;
#if NX_SDK_VERSION_NUMBER < 270
  maxParticles = desc.maxParticles;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxParticleData::copyTo(::NxParticleData &desc,CustomCopy &cc)
{
  desc.name = name;
#if NX_SDK_VERSION_NUMBER < 270
  desc.maxParticles = maxParticles;
#endif
  cc.customCopyTo(desc,*this);
}

#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_FLUID_API

//***********************************************************************************
// Constructor for 'NxFluidEmitterDesc'
//***********************************************************************************
NxFluidEmitterDesc::NxFluidEmitterDesc(void)
{
  ::NxFluidEmitterDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  relPose                                       = def.relPose;
  mFrameActor                                   = 0;
  mFrameShape                                   = 0;
  type                                          = NX_FE_CONSTANT_PRESSURE;
  maxParticles                                  = def.maxParticles;
  shape                                         = (NxEmitterShape) def.shape;
  dimensionX                                    = def.dimensionX;
  dimensionY                                    = def.dimensionY;
  randomPos                                     = def.randomPos;
  randomAngle                                   = def.randomAngle;
  fluidVelocityMagnitude                        = def.fluidVelocityMagnitude;
  rate                                          = def.rate;
  particleLifetime                              = def.particleLifetime;
#if NX_SDK_VERSION_NUMBER >= 270
  repulsionCoefficient                          = def.repulsionCoefficient;
#endif
  flags                                         = (NxFluidEmitterFlag) def.flags;
  mInstance = 0;
}

NxFluidEmitterDesc::~NxFluidEmitterDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 270
#endif
}

void NxFluidEmitterDesc::store(SchemaStream &stream,const char *parent)
{
  NxFluidEmitterDesc def;
  stream.beginHeader(SC_NxFluidEmitterDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || !isSame(def.relPose,relPose) )
    stream.store(relPose,"relPose",false);
  if ( gSaveDefaults || def.mFrameActor != mFrameActor )
    stream.store(mFrameActor,"mFrameActor",false);
  if ( gSaveDefaults || def.mFrameShape != mFrameShape )
    stream.store(mFrameShape,"mFrameShape",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)type,"type",false);
  }
  else
  {
    if ( gSaveDefaults || def.type != type )
    {
      stream.beginHeader(SC_NxEmitterType);
      stream.store("type","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxEmitterType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( type & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.maxParticles != maxParticles )
    stream.store(maxParticles,"maxParticles",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)shape,"shape",false);
  }
  else
  {
    if ( gSaveDefaults || def.shape != shape )
    {
      stream.beginHeader(SC_NxEmitterShape);
      stream.store("shape","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxEmitterShape) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( shape & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.dimensionX != dimensionX )
    stream.store(dimensionX,"dimensionX",false);
  if ( gSaveDefaults || def.dimensionY != dimensionY )
    stream.store(dimensionY,"dimensionY",false);
  if ( gSaveDefaults || def.randomPos != randomPos )
    stream.store(randomPos,"randomPos",false);
  if ( gSaveDefaults || def.randomAngle != randomAngle )
    stream.store(randomAngle,"randomAngle",false);
  if ( gSaveDefaults || def.fluidVelocityMagnitude != fluidVelocityMagnitude )
    stream.store(fluidVelocityMagnitude,"fluidVelocityMagnitude",false);
  if ( gSaveDefaults || def.rate != rate )
    stream.store(rate,"rate",false);
  if ( gSaveDefaults || def.particleLifetime != particleLifetime )
    stream.store(particleLifetime,"particleLifetime",false);
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.repulsionCoefficient != repulsionCoefficient )
    stream.store(repulsionCoefficient,"repulsionCoefficient",false);
#endif
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxFluidEmitterFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxFluidEmitterFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  stream.endHeader();
}

void NxFluidEmitterDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxFluidEmitterDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(relPose,"relPose",false);
    stream.load(mFrameActor,"mFrameActor",false);
    stream.load(mFrameShape,"mFrameShape",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"type",false);
    type = (NxEmitterType) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxEmitterType,"type") )
    {
      type = (NxEmitterType) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxEmitterType) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	type = (NxEmitterType) (type|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(maxParticles,"maxParticles",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"shape",false);
    shape = (NxEmitterShape) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxEmitterShape,"shape") )
    {
      shape = (NxEmitterShape) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxEmitterShape) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	shape = (NxEmitterShape) (shape|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(dimensionX,"dimensionX",false);
    stream.load(dimensionY,"dimensionY",false);
    stream.load(randomPos,"randomPos",false);
    stream.load(randomAngle,"randomAngle",false);
    stream.load(fluidVelocityMagnitude,"fluidVelocityMagnitude",false);
    stream.load(rate,"rate",false);
    stream.load(particleLifetime,"particleLifetime",false);
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(repulsionCoefficient,"repulsionCoefficient",false);
#endif
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxFluidEmitterFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxFluidEmitterFlag,"flags") )
    {
      flags = (NxFluidEmitterFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxFluidEmitterFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxFluidEmitterFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.endHeader();
  }
}

void NxFluidEmitterDesc::copyFrom(const ::NxFluidEmitterDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  relPose = desc.relPose;
  type = (NxEmitterType) desc.type;
  maxParticles = desc.maxParticles;
  shape = (NxEmitterShape) desc.shape;
  dimensionX = desc.dimensionX;
  dimensionY = desc.dimensionY;
  randomPos = desc.randomPos;
  randomAngle = desc.randomAngle;
  fluidVelocityMagnitude = desc.fluidVelocityMagnitude;
  rate = desc.rate;
  particleLifetime = desc.particleLifetime;
#if NX_SDK_VERSION_NUMBER >= 270
  repulsionCoefficient = desc.repulsionCoefficient;
#endif
  flags = (NxFluidEmitterFlag) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxFluidEmitterDesc::copyTo(::NxFluidEmitterDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.relPose = relPose;
  desc.type = (::NxEmitterType) type;
  desc.maxParticles = maxParticles;
  desc.shape = (::NxEmitterShape) shape;
  desc.dimensionX = dimensionX;
  desc.dimensionY = dimensionY;
  desc.randomPos = randomPos;
  desc.randomAngle = randomAngle;
  desc.fluidVelocityMagnitude = fluidVelocityMagnitude;
  desc.rate = rate;
  desc.particleLifetime = particleLifetime;
#if NX_SDK_VERSION_NUMBER >= 270
  desc.repulsionCoefficient = repulsionCoefficient;
#endif
  desc.flags = (::NxFluidEmitterFlag) flags;
  cc.customCopyTo(desc,*this);
}

#endif
#if NX_USE_FLUID_API

//***********************************************************************************
// Constructor for 'NxFluidDesc'
//***********************************************************************************
NxFluidDesc::NxFluidDesc(void)
{
  ::NxFluidDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  // NxParticleData: mInitialParticleData
  mMaxFluidParticles                            = 0;
#if NX_SDK_VERSION_NUMBER >= 280
  mCurrentParticleLimit                         = 0xffffffff;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  numReserveParticles                           = def.numReserveParticles;
#endif
  restDensity                                   = def.restDensity;
  restParticlesPerMeter                         = def.restParticlesPerMeter;
  kernelRadiusMultiplier                        = def.kernelRadiusMultiplier;
  motionLimitMultiplier                         = def.motionLimitMultiplier;
#if NX_SDK_VERSION_NUMBER >= 260
  collisionDistanceMultiplier                   = def.collisionDistanceMultiplier;
#endif
  packetSizeMultiplier                          = def.packetSizeMultiplier;
  stiffness                                     = def.stiffness;
  viscosity                                     = def.viscosity;
  damping                                       = def.damping;
#if NX_SDK_VERSION_NUMBER >= 270
  fadeInTime                                    = def.fadeInTime;
#endif
  externalAcceleration                          = def.externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 280
  projectionPlane                               = def.projectionPlane;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  staticCollisionRestitution                    = def.staticCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  restitutionForStaticShapes                    = def.restitutionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  staticCollisionAdhesion                       = def.staticCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dynamicFrictionForStaticShapes                = def.dynamicFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  staticFrictionForStaticShapes                 = def.staticFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  attractionForStaticShapes                     = def.attractionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  dynamicCollisionRestitution                   = def.dynamicCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  restitutionForDynamicShapes                   = def.restitutionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  dynamicCollisionAdhesion                      = def.dynamicCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dynamicFrictionForDynamicShapes               = def.dynamicFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  staticFrictionForDynamicShapes                = def.staticFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  attractionForDynamicShapes                    = def.attractionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  collisionResponseCoefficient                  = def.collisionResponseCoefficient;
#endif
  simulationMethod                              = (NxFluidSimulationMethod) def.simulationMethod;
  collisionMethod                               = (NxFluidCollisionMethod) def.collisionMethod;
#if NX_SDK_VERSION_NUMBER >= 250
  collisionGroup                                = def.collisionGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  // NxGroupsMask: groupsMask
#endif
  flags                                         = NX_FF_VISUALIZATION;
#if NX_SDK_VERSION_NUMBER >= 260
  mCompartment                                  = 0;
#endif
  // NxArray< NxFluidEmitterDesc *>: mEmitters
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial                            = def.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  surfaceTension                                = def.surfaceTension;
#endif
  mInstance = 0;
}

NxFluidDesc::~NxFluidDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
  for (NxU32 i=0; i<mEmitters.size(); i++)
  {
     NxFluidEmitterDesc  *v = mEmitters[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
}

void NxFluidDesc::store(SchemaStream &stream,const char *parent)
{
  NxFluidDesc def;
  stream.beginHeader(SC_NxFluidDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  mInitialParticleData.store(stream,"mInitialParticleData");
  if ( gSaveDefaults || def.mMaxFluidParticles != mMaxFluidParticles )
    stream.store(mMaxFluidParticles,"mMaxFluidParticles",false);
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.mCurrentParticleLimit != mCurrentParticleLimit )
    stream.store(mCurrentParticleLimit,"mCurrentParticleLimit",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.numReserveParticles != numReserveParticles )
    stream.store(numReserveParticles,"numReserveParticles",false);
#endif
  if ( gSaveDefaults || def.restDensity != restDensity )
    stream.store(restDensity,"restDensity",false);
  if ( gSaveDefaults || def.restParticlesPerMeter != restParticlesPerMeter )
    stream.store(restParticlesPerMeter,"restParticlesPerMeter",false);
  if ( gSaveDefaults || def.kernelRadiusMultiplier != kernelRadiusMultiplier )
    stream.store(kernelRadiusMultiplier,"kernelRadiusMultiplier",false);
  if ( gSaveDefaults || def.motionLimitMultiplier != motionLimitMultiplier )
    stream.store(motionLimitMultiplier,"motionLimitMultiplier",false);
#if NX_SDK_VERSION_NUMBER >= 260
  if ( gSaveDefaults || def.collisionDistanceMultiplier != collisionDistanceMultiplier )
    stream.store(collisionDistanceMultiplier,"collisionDistanceMultiplier",false);
#endif
  if ( gSaveDefaults || def.packetSizeMultiplier != packetSizeMultiplier )
    stream.store(packetSizeMultiplier,"packetSizeMultiplier",false);
  if ( gSaveDefaults || def.stiffness != stiffness )
    stream.store(stiffness,"stiffness",false);
  if ( gSaveDefaults || def.viscosity != viscosity )
    stream.store(viscosity,"viscosity",false);
  if ( gSaveDefaults || def.damping != damping )
    stream.store(damping,"damping",false);
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.fadeInTime != fadeInTime )
    stream.store(fadeInTime,"fadeInTime",false);
#endif
  if ( gSaveDefaults || def.externalAcceleration != externalAcceleration )
    stream.store(externalAcceleration,"externalAcceleration",false);
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || !isSame(def.projectionPlane,projectionPlane) )
    stream.store(projectionPlane,"projectionPlane",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.staticCollisionRestitution != staticCollisionRestitution )
    stream.store(staticCollisionRestitution,"staticCollisionRestitution",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.restitutionForStaticShapes != restitutionForStaticShapes )
    stream.store(restitutionForStaticShapes,"restitutionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.staticCollisionAdhesion != staticCollisionAdhesion )
    stream.store(staticCollisionAdhesion,"staticCollisionAdhesion",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.dynamicFrictionForStaticShapes != dynamicFrictionForStaticShapes )
    stream.store(dynamicFrictionForStaticShapes,"dynamicFrictionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.staticFrictionForStaticShapes != staticFrictionForStaticShapes )
    stream.store(staticFrictionForStaticShapes,"staticFrictionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.attractionForStaticShapes != attractionForStaticShapes )
    stream.store(attractionForStaticShapes,"attractionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.dynamicCollisionRestitution != dynamicCollisionRestitution )
    stream.store(dynamicCollisionRestitution,"dynamicCollisionRestitution",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.restitutionForDynamicShapes != restitutionForDynamicShapes )
    stream.store(restitutionForDynamicShapes,"restitutionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.dynamicCollisionAdhesion != dynamicCollisionAdhesion )
    stream.store(dynamicCollisionAdhesion,"dynamicCollisionAdhesion",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.dynamicFrictionForDynamicShapes != dynamicFrictionForDynamicShapes )
    stream.store(dynamicFrictionForDynamicShapes,"dynamicFrictionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.staticFrictionForDynamicShapes != staticFrictionForDynamicShapes )
    stream.store(staticFrictionForDynamicShapes,"staticFrictionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.attractionForDynamicShapes != attractionForDynamicShapes )
    stream.store(attractionForDynamicShapes,"attractionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.collisionResponseCoefficient != collisionResponseCoefficient )
    stream.store(collisionResponseCoefficient,"collisionResponseCoefficient",false);
#endif
  if ( stream.isBinary() )
  {
    stream.store((NxU32)simulationMethod,"simulationMethod",false);
  }
  else
  {
    if ( gSaveDefaults || def.simulationMethod != simulationMethod )
    {
      stream.beginHeader(SC_NxFluidSimulationMethod);
      stream.store("simulationMethod","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxFluidSimulationMethod) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( simulationMethod & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
  {
    stream.store((NxU32)collisionMethod,"collisionMethod",false);
  }
  else
  {
    if ( gSaveDefaults || def.collisionMethod != collisionMethod )
    {
      stream.beginHeader(SC_NxFluidCollisionMethod);
      stream.store("collisionMethod","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxFluidCollisionMethod) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( collisionMethod & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.collisionGroup != collisionGroup )
    stream.store(collisionGroup,"collisionGroup",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  groupsMask.store(stream,"groupsMask");
#endif
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxFluidFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxFluidFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 260
  if ( gSaveDefaults || def.mCompartment != mCompartment )
    stream.store(mCompartment,"mCompartment",false);
#endif
  for (NxU32 i=0; i<mEmitters.size(); i++)
  {
    NxFluidEmitterDesc *v = mEmitters[i];
    v->store(stream);
  }

#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.forceFieldMaterial != forceFieldMaterial )
    stream.store(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.surfaceTension != surfaceTension )
    stream.store(surfaceTension,"surfaceTension",false);
#endif
  stream.endHeader();
}

void NxFluidDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentFluid(this);
  if ( stream.beginHeader(SC_NxFluidDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
  mInitialParticleData.load(stream,"mInitialParticleData");
    stream.load(mMaxFluidParticles,"mMaxFluidParticles",false);
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(mCurrentParticleLimit,"mCurrentParticleLimit",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(numReserveParticles,"numReserveParticles",false);
#endif
    stream.load(restDensity,"restDensity",false);
    stream.load(restParticlesPerMeter,"restParticlesPerMeter",false);
    stream.load(kernelRadiusMultiplier,"kernelRadiusMultiplier",false);
    stream.load(motionLimitMultiplier,"motionLimitMultiplier",false);
#if NX_SDK_VERSION_NUMBER >= 260
    stream.load(collisionDistanceMultiplier,"collisionDistanceMultiplier",false);
#endif
    stream.load(packetSizeMultiplier,"packetSizeMultiplier",false);
    stream.load(stiffness,"stiffness",false);
    stream.load(viscosity,"viscosity",false);
    stream.load(damping,"damping",false);
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(fadeInTime,"fadeInTime",false);
#endif
    stream.load(externalAcceleration,"externalAcceleration",false);
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(projectionPlane,"projectionPlane",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(staticCollisionRestitution,"staticCollisionRestitution",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(restitutionForStaticShapes,"restitutionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(staticCollisionAdhesion,"staticCollisionAdhesion",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(dynamicFrictionForStaticShapes,"dynamicFrictionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(staticFrictionForStaticShapes,"staticFrictionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(attractionForStaticShapes,"attractionForStaticShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(dynamicCollisionRestitution,"dynamicCollisionRestitution",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(restitutionForDynamicShapes,"restitutionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(dynamicCollisionAdhesion,"dynamicCollisionAdhesion",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(dynamicFrictionForDynamicShapes,"dynamicFrictionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(staticFrictionForDynamicShapes,"staticFrictionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(attractionForDynamicShapes,"attractionForDynamicShapes",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(collisionResponseCoefficient,"collisionResponseCoefficient",false);
#endif
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"simulationMethod",false);
    simulationMethod = (NxFluidSimulationMethod) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxFluidSimulationMethod,"simulationMethod") )
    {
      simulationMethod = (NxFluidSimulationMethod) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxFluidSimulationMethod) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	simulationMethod = (NxFluidSimulationMethod) (simulationMethod|shift);
        }
      }
      stream.endHeader();
    }
  }
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"collisionMethod",false);
    collisionMethod = (NxFluidCollisionMethod) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxFluidCollisionMethod,"collisionMethod") )
    {
      collisionMethod = (NxFluidCollisionMethod) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxFluidCollisionMethod) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	collisionMethod = (NxFluidCollisionMethod) (collisionMethod|shift);
        }
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(collisionGroup,"collisionGroup",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  groupsMask.load(stream,"groupsMask");
#endif
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxFluidFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxFluidFlag,"flags") )
    {
      flags = (NxFluidFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxFluidFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxFluidFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 260
    stream.load(mCompartment,"mCompartment",false);
#endif
  while ( stream.peekHeader(SC_NxFluidEmitterDesc) )
  {
     NxFluidEmitterDesc *temp = new NxFluidEmitterDesc;
     temp->load(stream);
     mEmitters.push_back(temp);
   }
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(surfaceTension,"surfaceTension",false);
#endif
    stream.endHeader();
  }
}

void NxFluidDesc::copyFrom(const ::NxFluidDesc &desc,CustomCopy &cc)
{
  name = desc.name;
#if NX_SDK_VERSION_NUMBER >= 270
  numReserveParticles = desc.numReserveParticles;
#endif
  restDensity = desc.restDensity;
  restParticlesPerMeter = desc.restParticlesPerMeter;
  kernelRadiusMultiplier = desc.kernelRadiusMultiplier;
  motionLimitMultiplier = desc.motionLimitMultiplier;
#if NX_SDK_VERSION_NUMBER >= 260
  collisionDistanceMultiplier = desc.collisionDistanceMultiplier;
#endif
  packetSizeMultiplier = desc.packetSizeMultiplier;
  stiffness = desc.stiffness;
  viscosity = desc.viscosity;
  damping = desc.damping;
#if NX_SDK_VERSION_NUMBER >= 270
  fadeInTime = desc.fadeInTime;
#endif
  externalAcceleration = desc.externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 280
  projectionPlane = desc.projectionPlane;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  staticCollisionRestitution = desc.staticCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  restitutionForStaticShapes = desc.restitutionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  staticCollisionAdhesion = desc.staticCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dynamicFrictionForStaticShapes = desc.dynamicFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  staticFrictionForStaticShapes = desc.staticFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  attractionForStaticShapes = desc.attractionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  dynamicCollisionRestitution = desc.dynamicCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  restitutionForDynamicShapes = desc.restitutionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  dynamicCollisionAdhesion = desc.dynamicCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dynamicFrictionForDynamicShapes = desc.dynamicFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  staticFrictionForDynamicShapes = desc.staticFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  attractionForDynamicShapes = desc.attractionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  collisionResponseCoefficient = desc.collisionResponseCoefficient;
#endif
  simulationMethod = (NxFluidSimulationMethod) desc.simulationMethod;
  collisionMethod = (NxFluidCollisionMethod) desc.collisionMethod;
#if NX_SDK_VERSION_NUMBER >= 250
  collisionGroup = desc.collisionGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  groupsMask.copyFrom(desc.groupsMask,cc);
#endif
  flags = (NxFluidFlag) desc.flags;
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial = desc.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  surfaceTension = desc.surfaceTension;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxFluidDesc::copyTo(::NxFluidDesc &desc,CustomCopy &cc)
{
  desc.name = name;
#if NX_SDK_VERSION_NUMBER >= 270
  desc.numReserveParticles = numReserveParticles;
#endif
  desc.restDensity = restDensity;
  desc.restParticlesPerMeter = restParticlesPerMeter;
  desc.kernelRadiusMultiplier = kernelRadiusMultiplier;
  desc.motionLimitMultiplier = motionLimitMultiplier;
#if NX_SDK_VERSION_NUMBER >= 260
  desc.collisionDistanceMultiplier = collisionDistanceMultiplier;
#endif
  desc.packetSizeMultiplier = packetSizeMultiplier;
  desc.stiffness = stiffness;
  desc.viscosity = viscosity;
  desc.damping = damping;
#if NX_SDK_VERSION_NUMBER >= 270
  desc.fadeInTime = fadeInTime;
#endif
  desc.externalAcceleration = externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 280
  desc.projectionPlane = projectionPlane;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.staticCollisionRestitution = staticCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.restitutionForStaticShapes = restitutionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.staticCollisionAdhesion = staticCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.dynamicFrictionForStaticShapes = dynamicFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.staticFrictionForStaticShapes = staticFrictionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.attractionForStaticShapes = attractionForStaticShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.dynamicCollisionRestitution = dynamicCollisionRestitution;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.restitutionForDynamicShapes = restitutionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.dynamicCollisionAdhesion = dynamicCollisionAdhesion;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.dynamicFrictionForDynamicShapes = dynamicFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.staticFrictionForDynamicShapes = staticFrictionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.attractionForDynamicShapes = attractionForDynamicShapes;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.collisionResponseCoefficient = collisionResponseCoefficient;
#endif
  desc.simulationMethod = (::NxFluidSimulationMethod) simulationMethod;
  desc.collisionMethod = (::NxFluidCollisionMethod) collisionMethod;
#if NX_SDK_VERSION_NUMBER >= 250
  desc.collisionGroup = collisionGroup;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  groupsMask.copyTo(desc.groupsMask,cc);
#endif
  desc.flags = (::NxFluidFlag) flags;
#if NX_SDK_VERSION_NUMBER >= 280
  desc.forceFieldMaterial = forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.surfaceTension = surfaceTension;
#endif
  cc.customCopyTo(desc,*this);
}

#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxClothDesc'
//***********************************************************************************
NxClothDesc::NxClothDesc(void)
{
  ::NxClothDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  mClothMesh                                    = 0;
  globalPose                                    = def.globalPose;
  thickness                                     = 0.01f;
  density                                       = 1;
  bendingStiffness                              = def.bendingStiffness;
  stretchingStiffness                           = def.stretchingStiffness;
  dampingCoefficient                            = def.dampingCoefficient;
  friction                                      = def.friction;
  pressure                                      = def.pressure;
  tearFactor                                    = def.tearFactor;
  collisionResponseCoefficient                  = def.collisionResponseCoefficient;
  attachmentResponseCoefficient                 = def.attachmentResponseCoefficient;
#if NX_SDK_VERSION_NUMBER >= 250
  attachmentTearFactor                          = def.attachmentTearFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  toFluidResponseCoefficient                    = def.toFluidResponseCoefficient;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  fromFluidResponseCoefficient                  = def.fromFluidResponseCoefficient;
#endif
  solverIterations                              = def.solverIterations;
  externalAcceleration                          = def.externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  windAcceleration                              = def.windAcceleration;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  wakeUpCounter                                 = def.wakeUpCounter;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepLinearVelocity                           = def.sleepLinearVelocity;
#endif
  collisionGroup                                = def.collisionGroup;
  // NxGroupsMask: groupsMask
  flags                                         = (NxClothFlag) def.flags;
#if NX_SDK_VERSION_NUMBER >= 260
  mCompartment                                  = 0;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  // NxArray< NxU8 >: mActiveState
#endif
  // NxArray< NxClothAttachDesc *>: mAttachments
#if NX_SDK_VERSION_NUMBER >= 272
  // NxBounds3: validBounds
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  relativeGridSpacing                           = def.relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  minAdhereVelocity                             = def.minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial                            = def.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hierarchicalSolverIterations                  = def.hierarchicalSolverIterations;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  selfCollisionThickness                        = def.selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hardStretchLimitationFactor                   = def.hardStretchLimitationFactor;
#endif
  mInstance = 0;
}

NxClothDesc::~NxClothDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
  for (NxU32 i=0; i<mAttachments.size(); i++)
  {
     NxClothAttachDesc  *v = mAttachments[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
}

void NxClothDesc::store(SchemaStream &stream,const char *parent)
{
  NxClothDesc def;
  stream.beginHeader(SC_NxClothDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || def.mClothMesh != mClothMesh )
    stream.store(mClothMesh,"mClothMesh",false);
  if ( gSaveDefaults || !isSame(def.globalPose,globalPose) )
    stream.store(globalPose,"globalPose",false);
  if ( gSaveDefaults || def.thickness != thickness )
    stream.store(thickness,"thickness",false);
  if ( gSaveDefaults || def.density != density )
    stream.store(density,"density",false);
  if ( gSaveDefaults || def.bendingStiffness != bendingStiffness )
    stream.store(bendingStiffness,"bendingStiffness",false);
  if ( gSaveDefaults || def.stretchingStiffness != stretchingStiffness )
    stream.store(stretchingStiffness,"stretchingStiffness",false);
  if ( gSaveDefaults || def.dampingCoefficient != dampingCoefficient )
    stream.store(dampingCoefficient,"dampingCoefficient",false);
  if ( gSaveDefaults || def.friction != friction )
    stream.store(friction,"friction",false);
  if ( gSaveDefaults || def.pressure != pressure )
    stream.store(pressure,"pressure",false);
  if ( gSaveDefaults || def.tearFactor != tearFactor )
    stream.store(tearFactor,"tearFactor",false);
  if ( gSaveDefaults || def.collisionResponseCoefficient != collisionResponseCoefficient )
    stream.store(collisionResponseCoefficient,"collisionResponseCoefficient",false);
  if ( gSaveDefaults || def.attachmentResponseCoefficient != attachmentResponseCoefficient )
    stream.store(attachmentResponseCoefficient,"attachmentResponseCoefficient",false);
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.attachmentTearFactor != attachmentTearFactor )
    stream.store(attachmentTearFactor,"attachmentTearFactor",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.toFluidResponseCoefficient != toFluidResponseCoefficient )
    stream.store(toFluidResponseCoefficient,"toFluidResponseCoefficient",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.fromFluidResponseCoefficient != fromFluidResponseCoefficient )
    stream.store(fromFluidResponseCoefficient,"fromFluidResponseCoefficient",false);
#endif
  if ( gSaveDefaults || def.solverIterations != solverIterations )
    stream.store(solverIterations,"solverIterations",false);
  if ( gSaveDefaults || def.externalAcceleration != externalAcceleration )
    stream.store(externalAcceleration,"externalAcceleration",false);
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( gSaveDefaults || def.windAcceleration != windAcceleration )
    stream.store(windAcceleration,"windAcceleration",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.wakeUpCounter != wakeUpCounter )
    stream.store(wakeUpCounter,"wakeUpCounter",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.sleepLinearVelocity != sleepLinearVelocity )
    stream.store(sleepLinearVelocity,"sleepLinearVelocity",false);
#endif
  if ( gSaveDefaults || def.collisionGroup != collisionGroup )
    stream.store(collisionGroup,"collisionGroup",false);
  groupsMask.store(stream,"groupsMask");
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxClothFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxClothFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 260
  if ( gSaveDefaults || def.mCompartment != mCompartment )
    stream.store(mCompartment,"mCompartment",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  stream.store(mActiveState,"mActiveState",false);
#endif
  for (NxU32 i=0; i<mAttachments.size(); i++)
  {
    NxClothAttachDesc *v = mAttachments[i];
    v->store(stream);
  }

#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || !isSame(def.validBounds,validBounds) )
    stream.store(validBounds,"validBounds",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.relativeGridSpacing != relativeGridSpacing )
    stream.store(relativeGridSpacing,"relativeGridSpacing",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.minAdhereVelocity != minAdhereVelocity )
    stream.store(minAdhereVelocity,"minAdhereVelocity",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.forceFieldMaterial != forceFieldMaterial )
    stream.store(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.hierarchicalSolverIterations != hierarchicalSolverIterations )
    stream.store(hierarchicalSolverIterations,"hierarchicalSolverIterations",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.selfCollisionThickness != selfCollisionThickness )
    stream.store(selfCollisionThickness,"selfCollisionThickness",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.hardStretchLimitationFactor != hardStretchLimitationFactor )
    stream.store(hardStretchLimitationFactor,"hardStretchLimitationFactor",false);
#endif
  stream.endHeader();
}

void NxClothDesc::load(SchemaStream &stream,const char *parent)
{
 stream.setCurrentCloth(this);
  if ( stream.beginHeader(SC_NxClothDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(mClothMesh,"mClothMesh",false);
    stream.load(globalPose,"globalPose",false);
    stream.load(thickness,"thickness",false);
    stream.load(density,"density",false);
    stream.load(bendingStiffness,"bendingStiffness",false);
    stream.load(stretchingStiffness,"stretchingStiffness",false);
    stream.load(dampingCoefficient,"dampingCoefficient",false);
    stream.load(friction,"friction",false);
    stream.load(pressure,"pressure",false);
    stream.load(tearFactor,"tearFactor",false);
    stream.load(collisionResponseCoefficient,"collisionResponseCoefficient",false);
    stream.load(attachmentResponseCoefficient,"attachmentResponseCoefficient",false);
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(attachmentTearFactor,"attachmentTearFactor",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(toFluidResponseCoefficient,"toFluidResponseCoefficient",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(fromFluidResponseCoefficient,"fromFluidResponseCoefficient",false);
#endif
    stream.load(solverIterations,"solverIterations",false);
    stream.load(externalAcceleration,"externalAcceleration",false);
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
    stream.load(windAcceleration,"windAcceleration",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(wakeUpCounter,"wakeUpCounter",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(sleepLinearVelocity,"sleepLinearVelocity",false);
#endif
    stream.load(collisionGroup,"collisionGroup",false);
  groupsMask.load(stream,"groupsMask");
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxClothFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxClothFlag,"flags") )
    {
      flags = (NxClothFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxClothFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxClothFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
#if NX_SDK_VERSION_NUMBER >= 260
    stream.load(mCompartment,"mCompartment",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  stream.load(mActiveState,"mActiveState",false);
#endif
  while ( stream.peekHeader(SC_NxClothAttachDesc) )
  {
     NxClothAttachDesc *temp = new NxClothAttachDesc;
     temp->load(stream);
     mAttachments.push_back(temp);
   }
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(validBounds,"validBounds",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(relativeGridSpacing,"relativeGridSpacing",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(minAdhereVelocity,"minAdhereVelocity",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(hierarchicalSolverIterations,"hierarchicalSolverIterations",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(selfCollisionThickness,"selfCollisionThickness",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(hardStretchLimitationFactor,"hardStretchLimitationFactor",false);
#endif
    stream.endHeader();
  }
}

void NxClothDesc::copyFrom(const ::NxClothDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  globalPose = desc.globalPose;
  thickness = desc.thickness;
  density = desc.density;
  bendingStiffness = desc.bendingStiffness;
  stretchingStiffness = desc.stretchingStiffness;
  dampingCoefficient = desc.dampingCoefficient;
  friction = desc.friction;
  pressure = desc.pressure;
  tearFactor = desc.tearFactor;
  collisionResponseCoefficient = desc.collisionResponseCoefficient;
  attachmentResponseCoefficient = desc.attachmentResponseCoefficient;
#if NX_SDK_VERSION_NUMBER >= 250
  attachmentTearFactor = desc.attachmentTearFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  toFluidResponseCoefficient = desc.toFluidResponseCoefficient;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  fromFluidResponseCoefficient = desc.fromFluidResponseCoefficient;
#endif
  solverIterations = desc.solverIterations;
  externalAcceleration = desc.externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  windAcceleration = desc.windAcceleration;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  wakeUpCounter = desc.wakeUpCounter;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  sleepLinearVelocity = desc.sleepLinearVelocity;
#endif
  collisionGroup = desc.collisionGroup;
  groupsMask.copyFrom(desc.groupsMask,cc);
  flags = (NxClothFlag) desc.flags;
#if NX_SDK_VERSION_NUMBER >= 272
  validBounds = desc.validBounds;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  relativeGridSpacing = desc.relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  minAdhereVelocity = desc.minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial = desc.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hierarchicalSolverIterations = desc.hierarchicalSolverIterations;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  selfCollisionThickness = desc.selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hardStretchLimitationFactor = desc.hardStretchLimitationFactor;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxClothDesc::copyTo(::NxClothDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.globalPose = globalPose;
  desc.thickness = thickness;
  desc.density = density;
  desc.bendingStiffness = bendingStiffness;
  desc.stretchingStiffness = stretchingStiffness;
  desc.dampingCoefficient = dampingCoefficient;
  desc.friction = friction;
  desc.pressure = pressure;
  desc.tearFactor = tearFactor;
  desc.collisionResponseCoefficient = collisionResponseCoefficient;
  desc.attachmentResponseCoefficient = attachmentResponseCoefficient;
#if NX_SDK_VERSION_NUMBER >= 250
  desc.attachmentTearFactor = attachmentTearFactor;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  desc.toFluidResponseCoefficient = toFluidResponseCoefficient;
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  desc.fromFluidResponseCoefficient = fromFluidResponseCoefficient;
#endif
  desc.solverIterations = solverIterations;
  desc.externalAcceleration = externalAcceleration;
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.windAcceleration = windAcceleration;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.wakeUpCounter = wakeUpCounter;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.sleepLinearVelocity = sleepLinearVelocity;
#endif
  desc.collisionGroup = collisionGroup;
  groupsMask.copyTo(desc.groupsMask,cc);
  desc.flags = (::NxClothFlag) flags;
#if NX_SDK_VERSION_NUMBER >= 272
  desc.validBounds = validBounds;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  desc.relativeGridSpacing = relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.minAdhereVelocity = minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.forceFieldMaterial = forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.hierarchicalSolverIterations = hierarchicalSolverIterations;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.selfCollisionThickness = selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.hardStretchLimitationFactor = hardStretchLimitationFactor;
#endif
  cc.customCopyTo(desc,*this);
}

#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
//***********************************************************************************
#endif

//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
// Constructor for 'NxForceFieldShapeDesc'
//***********************************************************************************
NxForceFieldShapeDesc::NxForceFieldShapeDesc(void)
{
  ::NxBoxForceFieldShapeDesc def;
  mUserProperties                               = 0;
  name                                          = def.name;
  pose                                          = def.pose;
#if NX_SDK_VERSION_NUMBER < 280
  flags                                         = def.flags;
#endif
  mInstance = 0;
  mType = SC_LAST;
}

NxForceFieldShapeDesc::~NxForceFieldShapeDesc(void)
{
#if NX_SDK_VERSION_NUMBER < 280
#endif
}

void NxForceFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldShapeDesc def;
  stream.beginHeader(SC_NxForceFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || !isSame(def.pose,pose) )
    stream.store(pose,"pose",false);
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.flags != flags )
    stream.store(flags,"flags",false);
#endif
  stream.endHeader();
}

void NxForceFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldShapeDesc,parent) )
  {
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(pose,"pose",false);
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(flags,"flags",false);
#endif
    stream.endHeader();
  }
}

void NxForceFieldShapeDesc::copyFrom(const ::NxForceFieldShapeDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  pose = desc.pose;
#if NX_SDK_VERSION_NUMBER < 280
  flags = desc.flags;
#endif
}

void NxForceFieldShapeDesc::copyTo(::NxForceFieldShapeDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.pose = pose;
#if NX_SDK_VERSION_NUMBER < 280
  desc.flags = flags;
#endif
}

#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
// Constructor for 'NxBoxForceFieldShapeDesc'
//***********************************************************************************
NxBoxForceFieldShapeDesc::NxBoxForceFieldShapeDesc(void)
{
  ::NxBoxForceFieldShapeDesc def;
  dimensions                                    = def.dimensions;
  mType = SC_NxBoxForceFieldShapeDesc;
  mInstance = 0;
}

NxBoxForceFieldShapeDesc::~NxBoxForceFieldShapeDesc(void)
{
}

void NxBoxForceFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxBoxForceFieldShapeDesc def;
  stream.beginHeader(SC_NxBoxForceFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.dimensions != dimensions )
    stream.store(dimensions,"dimensions",false);
  NxForceFieldShapeDesc::store(stream);
  stream.endHeader();
}

void NxBoxForceFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxBoxForceFieldShapeDesc,parent) )
  {
    stream.load(dimensions,"dimensions",false);
  NxForceFieldShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxBoxForceFieldShapeDesc::copyFrom(const ::NxBoxForceFieldShapeDesc &desc,CustomCopy &cc)
{
  dimensions = desc.dimensions;
  NxForceFieldShapeDesc::copyFrom(desc,cc);
}

void NxBoxForceFieldShapeDesc::copyTo(::NxBoxForceFieldShapeDesc &desc,CustomCopy &cc)
{
  NxForceFieldShapeDesc::copyTo(desc,cc);
  desc.dimensions = dimensions;
}

#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
// Constructor for 'NxSphereForceFieldShapeDesc'
//***********************************************************************************
NxSphereForceFieldShapeDesc::NxSphereForceFieldShapeDesc(void)
{
  ::NxSphereForceFieldShapeDesc def;
  radius                                        = def.radius;
  mType = SC_NxSphereForceFieldShapeDesc;
  mInstance = 0;
}

NxSphereForceFieldShapeDesc::~NxSphereForceFieldShapeDesc(void)
{
}

void NxSphereForceFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxSphereForceFieldShapeDesc def;
  stream.beginHeader(SC_NxSphereForceFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.radius != radius )
    stream.store(radius,"radius",false);
  NxForceFieldShapeDesc::store(stream);
  stream.endHeader();
}

void NxSphereForceFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSphereForceFieldShapeDesc,parent) )
  {
    stream.load(radius,"radius",false);
  NxForceFieldShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxSphereForceFieldShapeDesc::copyFrom(const ::NxSphereForceFieldShapeDesc &desc,CustomCopy &cc)
{
  radius = desc.radius;
  NxForceFieldShapeDesc::copyFrom(desc,cc);
}

void NxSphereForceFieldShapeDesc::copyTo(::NxSphereForceFieldShapeDesc &desc,CustomCopy &cc)
{
  NxForceFieldShapeDesc::copyTo(desc,cc);
  desc.radius = radius;
}

#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
// Constructor for 'NxCapsuleForceFieldShapeDesc'
//***********************************************************************************
NxCapsuleForceFieldShapeDesc::NxCapsuleForceFieldShapeDesc(void)
{
  ::NxCapsuleForceFieldShapeDesc def;
  radius                                        = def.radius;
  height                                        = def.height;
  mType = SC_NxCapsuleForceFieldShapeDesc;
  mInstance = 0;
}

NxCapsuleForceFieldShapeDesc::~NxCapsuleForceFieldShapeDesc(void)
{
}

void NxCapsuleForceFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxCapsuleForceFieldShapeDesc def;
  stream.beginHeader(SC_NxCapsuleForceFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.radius != radius )
    stream.store(radius,"radius",false);
  if ( gSaveDefaults || def.height != height )
    stream.store(height,"height",false);
  NxForceFieldShapeDesc::store(stream);
  stream.endHeader();
}

void NxCapsuleForceFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxCapsuleForceFieldShapeDesc,parent) )
  {
    stream.load(radius,"radius",false);
    stream.load(height,"height",false);
  NxForceFieldShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxCapsuleForceFieldShapeDesc::copyFrom(const ::NxCapsuleForceFieldShapeDesc &desc,CustomCopy &cc)
{
  radius = desc.radius;
  height = desc.height;
  NxForceFieldShapeDesc::copyFrom(desc,cc);
}

void NxCapsuleForceFieldShapeDesc::copyTo(::NxCapsuleForceFieldShapeDesc &desc,CustomCopy &cc)
{
  NxForceFieldShapeDesc::copyTo(desc,cc);
  desc.radius = radius;
  desc.height = height;
}

#endif
#if NX_SDK_VERSION_NUMBER >= 272

//***********************************************************************************
// Constructor for 'NxConvexForceFieldShapeDesc'
//***********************************************************************************
NxConvexForceFieldShapeDesc::NxConvexForceFieldShapeDesc(void)
{
  ::NxConvexForceFieldShapeDesc def;
  mMeshData                                     = 0;
  mType = SC_NxConvexForceFieldShapeDesc;
  mInstance = 0;
}

NxConvexForceFieldShapeDesc::~NxConvexForceFieldShapeDesc(void)
{
}

void NxConvexForceFieldShapeDesc::store(SchemaStream &stream,const char *parent)
{
  NxConvexForceFieldShapeDesc def;
  stream.beginHeader(SC_NxConvexForceFieldShapeDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mMeshData != mMeshData )
    stream.store(mMeshData,"mMeshData",true);
  NxForceFieldShapeDesc::store(stream);
  stream.endHeader();
}

void NxConvexForceFieldShapeDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxConvexForceFieldShapeDesc,parent) )
  {
    stream.load(mMeshData,"mMeshData",true);
  NxForceFieldShapeDesc::load(stream);
    stream.endHeader();
  }
}

void NxConvexForceFieldShapeDesc::copyFrom(const ::NxConvexForceFieldShapeDesc &desc,CustomCopy &cc)
{
  cc.customCopyFrom(*this,desc);
  NxForceFieldShapeDesc::copyFrom(desc,cc);
}

void NxConvexForceFieldShapeDesc::copyTo(::NxConvexForceFieldShapeDesc &desc,CustomCopy &cc)
{
  NxForceFieldShapeDesc::copyTo(desc,cc);
  cc.customCopyTo(desc,*this);
}

#endif

//***********************************************************************************
//***********************************************************************************
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
// Constructor for 'NxForceFieldShapeGroupDesc'
//***********************************************************************************
NxForceFieldShapeGroupDesc::NxForceFieldShapeGroupDesc(void)
{
  ::NxForceFieldShapeGroupDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  flags                                         = (NxForceFieldShapeGroupFlags) def.flags;
  // NxArray<NxForceFieldShapeDesc*>: mShapes
  mInstance = 0;
}

NxForceFieldShapeGroupDesc::~NxForceFieldShapeGroupDesc(void)
{
  for (NxU32 i=0; i<mShapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mShapes[i];
    delete v;
  }

}

void NxForceFieldShapeGroupDesc::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldShapeGroupDesc def;
  stream.beginHeader(SC_NxForceFieldShapeGroupDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxForceFieldShapeGroupFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxForceFieldShapeGroupFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  for (NxU32 i=0; i<mShapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mShapes[i];
    switch ( v->mType )
    {
      case SC_NxSphereForceFieldShapeDesc:
        if ( 1 )
        {
          NxSphereForceFieldShapeDesc *p = static_cast<NxSphereForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxBoxForceFieldShapeDesc:
        if ( 1 )
        {
          NxBoxForceFieldShapeDesc *p = static_cast<NxBoxForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxCapsuleForceFieldShapeDesc:
        if ( 1 )
        {
          NxCapsuleForceFieldShapeDesc *p = static_cast<NxCapsuleForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxConvexForceFieldShapeDesc:
        if ( 1 )
        {
          NxConvexForceFieldShapeDesc *p = static_cast<NxConvexForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      default:
        break;
    }
  }
  stream.endHeader();
}

void NxForceFieldShapeGroupDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldShapeGroupDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxForceFieldShapeGroupFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxForceFieldShapeGroupFlags,"flags") )
    {
      flags = (NxForceFieldShapeGroupFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxForceFieldShapeGroupFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxForceFieldShapeGroupFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    NxForceFieldShapeDesc *shape = 0;
    do
    {
      shape = 0;
			NxI32 v=-1;

      stream.peekHeader(SC_NxSphereForceFieldShapeDesc,&v,
											  SC_NxBoxForceFieldShapeDesc,
												SC_NxCapsuleForceFieldShapeDesc,
												SC_NxConvexForceFieldShapeDesc);
      switch ( v )
      {
        case SC_NxSphereForceFieldShapeDesc:
          if ( 1 )
          {
            NxSphereForceFieldShapeDesc *p = new NxSphereForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxBoxForceFieldShapeDesc:
          if ( 1 )
          {
            NxBoxForceFieldShapeDesc *p = new NxBoxForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxCapsuleForceFieldShapeDesc:
          if ( 1 )
          {
            NxCapsuleForceFieldShapeDesc *p = new NxCapsuleForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxConvexForceFieldShapeDesc:
          if ( 1 )
          {
            NxConvexForceFieldShapeDesc *p = new NxConvexForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        default:
          break;
     }
     if ( shape )
     {
       mShapes.push_back(shape);
     }
   } while ( shape );
    stream.endHeader();
  }
}

void NxForceFieldShapeGroupDesc::copyFrom(const ::NxForceFieldShapeGroupDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  flags = (NxForceFieldShapeGroupFlags) desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxForceFieldShapeGroupDesc::copyTo(::NxForceFieldShapeGroupDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.flags = (::NxForceFieldShapeGroupFlags) flags;
  cc.customCopyTo(desc,*this);
}

#endif
#if NX_SDK_VERSION_NUMBER >= 280

//***********************************************************************************
// Constructor for 'NxForceFieldLinearKernelDesc'
//***********************************************************************************
NxForceFieldLinearKernelDesc::NxForceFieldLinearKernelDesc(void)
{
  ::NxForceFieldLinearKernelDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  constant                                      = def.constant;
  positionMultiplier                            = def.positionMultiplier;
  positionTarget                                = def.positionTarget;
  velocityMultiplier                            = def.velocityMultiplier;
  velocityTarget                                = def.velocityTarget;
  torusRadius                                   = def.torusRadius;
  falloffLinear                                 = def.falloffLinear;
  falloffQuadratic                              = def.falloffQuadratic;
  noise                                         = def.noise;
  mInstance = 0;
}

NxForceFieldLinearKernelDesc::~NxForceFieldLinearKernelDesc(void)
{
}

void NxForceFieldLinearKernelDesc::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldLinearKernelDesc def;
  stream.beginHeader(SC_NxForceFieldLinearKernelDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || def.constant != constant )
    stream.store(constant,"constant",false);
  if ( gSaveDefaults || !isSame(def.positionMultiplier,positionMultiplier) )
    stream.store(positionMultiplier,"positionMultiplier",false);
  if ( gSaveDefaults || def.positionTarget != positionTarget )
    stream.store(positionTarget,"positionTarget",false);
  if ( gSaveDefaults || !isSame(def.velocityMultiplier,velocityMultiplier) )
    stream.store(velocityMultiplier,"velocityMultiplier",false);
  if ( gSaveDefaults || def.velocityTarget != velocityTarget )
    stream.store(velocityTarget,"velocityTarget",false);
  if ( gSaveDefaults || def.torusRadius != torusRadius )
    stream.store(torusRadius,"torusRadius",false);
  if ( gSaveDefaults || def.falloffLinear != falloffLinear )
    stream.store(falloffLinear,"falloffLinear",false);
  if ( gSaveDefaults || def.falloffQuadratic != falloffQuadratic )
    stream.store(falloffQuadratic,"falloffQuadratic",false);
  if ( gSaveDefaults || def.noise != noise )
    stream.store(noise,"noise",false);
  stream.endHeader();
}

void NxForceFieldLinearKernelDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldLinearKernelDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(constant,"constant",false);
    stream.load(positionMultiplier,"positionMultiplier",false);
    stream.load(positionTarget,"positionTarget",false);
    stream.load(velocityMultiplier,"velocityMultiplier",false);
    stream.load(velocityTarget,"velocityTarget",false);
    stream.load(torusRadius,"torusRadius",false);
    stream.load(falloffLinear,"falloffLinear",false);
    stream.load(falloffQuadratic,"falloffQuadratic",false);
    stream.load(noise,"noise",false);
    stream.endHeader();
  }
}

void NxForceFieldLinearKernelDesc::copyFrom(const ::NxForceFieldLinearKernelDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  constant = desc.constant;
  positionMultiplier = desc.positionMultiplier;
  positionTarget = desc.positionTarget;
  velocityMultiplier = desc.velocityMultiplier;
  velocityTarget = desc.velocityTarget;
  torusRadius = desc.torusRadius;
  falloffLinear = desc.falloffLinear;
  falloffQuadratic = desc.falloffQuadratic;
  noise = desc.noise;
}

void NxForceFieldLinearKernelDesc::copyTo(::NxForceFieldLinearKernelDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.constant = constant;
  desc.positionMultiplier = positionMultiplier;
  desc.positionTarget = positionTarget;
  desc.velocityMultiplier = velocityMultiplier;
  desc.velocityTarget = velocityTarget;
  desc.torusRadius = torusRadius;
  desc.falloffLinear = falloffLinear;
  desc.falloffQuadratic = falloffQuadratic;
  desc.noise = noise;
}

#endif

//***********************************************************************************
// Constructor for 'NxForceFieldScaleTableEntry'
//***********************************************************************************
NxForceFieldScaleTableEntry::NxForceFieldScaleTableEntry(void)
{
  var                                           = 0;
  mat                                           = 0;
  scale                                         = 0;
  mInstance = 0;
}

NxForceFieldScaleTableEntry::~NxForceFieldScaleTableEntry(void)
{
}

void NxForceFieldScaleTableEntry::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldScaleTableEntry def;
  stream.beginHeader(SC_NxForceFieldScaleTableEntry);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.var != var )
    stream.store(var,"var",false);
  if ( gSaveDefaults || def.mat != mat )
    stream.store(mat,"mat",false);
  if ( gSaveDefaults || def.scale != scale )
    stream.store(scale,"scale",false);
  stream.endHeader();
}

void NxForceFieldScaleTableEntry::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldScaleTableEntry,parent) )
  {
    stream.load(var,"var",false);
    stream.load(mat,"mat",false);
    stream.load(scale,"scale",false);
    stream.endHeader();
  }
}


//***********************************************************************************
// Constructor for 'NxForceFieldGroupReference'
//***********************************************************************************
NxForceFieldGroupReference::NxForceFieldGroupReference(void)
{
  // const char*: mRef
  mInstance = 0;
}

NxForceFieldGroupReference::~NxForceFieldGroupReference(void)
{
}

void NxForceFieldGroupReference::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldGroupReference def;
  stream.beginHeader(SC_NxForceFieldGroupReference);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mRef != mRef )
    stream.store(mRef,"mRef",true);
  stream.endHeader();
}

void NxForceFieldGroupReference::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldGroupReference,parent) )
  {
    stream.load(mRef,"mRef",true);
    stream.endHeader();
  }
}

#if NX_SDK_VERSION_NUMBER >= 270

//***********************************************************************************
// Constructor for 'NxForceFieldDesc'
//***********************************************************************************
NxForceFieldDesc::NxForceFieldDesc(void)
{
  ::NxForceFieldDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
#if NX_SDK_VERSION_NUMBER >= 272
  name                                          = def.name;
#endif
  mActor                                        = 0;
  pose                                          = def.pose;
  coordinates                                   = (NxForceFieldCoordinates) def.coordinates;
#if NX_SDK_VERSION_NUMBER < 280
  constant                                      = def.constant;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  noisiness                                     = def.noisiness;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  linearFalloff                                 = def.linearFalloff;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  offset                                        = def.offset;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  magnitude                                     = def.magnitude;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  positionMultiplier                            = def.positionMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  velocityMultiplier                            = def.velocityMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  torusRadius                                   = def.torusRadius;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  positionTarget                                = def.positionTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  velocityTarget                                = def.velocityTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  noise                                         = def.noise;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  // NxArray<NxForceFieldShapeDesc*>: mFFshapes
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  group                                         = def.group;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  // NxGroupsMask: groupsMask
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  falloffLinear                                 = def.falloffLinear;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  falloffQuadratic                              = def.falloffQuadratic;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  fluidScale                                    = def.fluidScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  clothScale                                    = def.clothScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  softBodyScale                                 = def.softBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  rigidBodyScale                                = def.rigidBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  flags                                         = (NxForceFieldFlags) def.flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  fluidType                                     = (NxForceFieldType) def.fluidType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  clothType                                     = (NxForceFieldType) def.clothType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  softBodyType                                  = (NxForceFieldType) def.softBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  rigidBodyType                                 = (NxForceFieldType) def.rigidBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldVariety                             = def.forceFieldVariety;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  mKernel                                       = 0;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  // NxArray<NxForceFieldShapeDesc*>: mIncludeShapes
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  // NxArray<NxForceFieldGroupReference*>: mShapeGroups
#endif
  mInstance = 0;
}

NxForceFieldDesc::~NxForceFieldDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  for (NxU32 i=0; i<mFFshapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mFFshapes[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mIncludeShapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mIncludeShapes[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mShapeGroups.size(); i++)
  {
    NxForceFieldGroupReference *v = mShapeGroups[i];
    delete v;
  }

#endif
}

void NxForceFieldDesc::store(SchemaStream &stream,const char *parent)
{
  NxForceFieldDesc def;
  stream.beginHeader(SC_NxForceFieldDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
#endif
  if ( gSaveDefaults || def.mActor != mActor )
    stream.store(mActor,"mActor",true);
  if ( gSaveDefaults || !isSame(def.pose,pose) )
    stream.store(pose,"pose",false);
  if ( stream.isBinary() )
    stream.store((NxU32)coordinates,"coordinates",false);
   else
   {
     if ( gSaveDefaults || def.coordinates != coordinates )
      stream.store(EnumToString(coordinates),"coordinates",false);
   }
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.constant != constant )
    stream.store(constant,"constant",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.noisiness != noisiness )
    stream.store(noisiness,"noisiness",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.linearFalloff != linearFalloff )
    stream.store(linearFalloff,"linearFalloff",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.offset != offset )
    stream.store(offset,"offset",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.magnitude != magnitude )
    stream.store(magnitude,"magnitude",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || !isSame(def.positionMultiplier,positionMultiplier) )
    stream.store(positionMultiplier,"positionMultiplier",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || !isSame(def.velocityMultiplier,velocityMultiplier) )
    stream.store(velocityMultiplier,"velocityMultiplier",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.torusRadius != torusRadius )
    stream.store(torusRadius,"torusRadius",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.positionTarget != positionTarget )
    stream.store(positionTarget,"positionTarget",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.velocityTarget != velocityTarget )
    stream.store(velocityTarget,"velocityTarget",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.noise != noise )
    stream.store(noise,"noise",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  for (NxU32 i=0; i<mFFshapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mFFshapes[i];
    switch ( v->mType )
    {
      case SC_NxSphereForceFieldShapeDesc:
        if ( 1 )
        {
          NxSphereForceFieldShapeDesc *p = static_cast<NxSphereForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxBoxForceFieldShapeDesc:
        if ( 1 )
        {
          NxBoxForceFieldShapeDesc *p = static_cast<NxBoxForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxCapsuleForceFieldShapeDesc:
        if ( 1 )
        {
          NxCapsuleForceFieldShapeDesc *p = static_cast<NxCapsuleForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxConvexForceFieldShapeDesc:
        if ( 1 )
        {
          NxConvexForceFieldShapeDesc *p = static_cast<NxConvexForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      default:
        break;
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.group != group )
    stream.store(group,"group",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  groupsMask.store(stream,"groupsMask");
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.falloffLinear != falloffLinear )
    stream.store(falloffLinear,"falloffLinear",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.falloffQuadratic != falloffQuadratic )
    stream.store(falloffQuadratic,"falloffQuadratic",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.fluidScale != fluidScale )
    stream.store(fluidScale,"fluidScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.clothScale != clothScale )
    stream.store(clothScale,"clothScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.softBodyScale != softBodyScale )
    stream.store(softBodyScale,"softBodyScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  if ( gSaveDefaults || def.rigidBodyScale != rigidBodyScale )
    stream.store(rigidBodyScale,"rigidBodyScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxForceFieldFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxForceFieldFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
    stream.store((NxU32)fluidType,"fluidType",false);
   else
   {
     if ( gSaveDefaults || def.fluidType != fluidType )
      stream.store(EnumToString(fluidType),"fluidType",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
    stream.store((NxU32)clothType,"clothType",false);
   else
   {
     if ( gSaveDefaults || def.clothType != clothType )
      stream.store(EnumToString(clothType),"clothType",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
    stream.store((NxU32)softBodyType,"softBodyType",false);
   else
   {
     if ( gSaveDefaults || def.softBodyType != softBodyType )
      stream.store(EnumToString(softBodyType),"softBodyType",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
    stream.store((NxU32)rigidBodyType,"rigidBodyType",false);
   else
   {
     if ( gSaveDefaults || def.rigidBodyType != rigidBodyType )
      stream.store(EnumToString(rigidBodyType),"rigidBodyType",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.forceFieldVariety != forceFieldVariety )
    stream.store(forceFieldVariety,"forceFieldVariety",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.mKernel != mKernel )
    stream.store(mKernel,"mKernel",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mIncludeShapes.size(); i++)
  {
    NxForceFieldShapeDesc *v = mIncludeShapes[i];
    switch ( v->mType )
    {
      case SC_NxSphereForceFieldShapeDesc:
        if ( 1 )
        {
          NxSphereForceFieldShapeDesc *p = static_cast<NxSphereForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxBoxForceFieldShapeDesc:
        if ( 1 )
        {
          NxBoxForceFieldShapeDesc *p = static_cast<NxBoxForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxCapsuleForceFieldShapeDesc:
        if ( 1 )
        {
          NxCapsuleForceFieldShapeDesc *p = static_cast<NxCapsuleForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxConvexForceFieldShapeDesc:
        if ( 1 )
        {
          NxConvexForceFieldShapeDesc *p = static_cast<NxConvexForceFieldShapeDesc *>(v);
          p->store(stream);
        }
        break;
      default:
        break;
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mShapeGroups.size(); i++)
  {
    NxForceFieldGroupReference *v = mShapeGroups[i];
    v->store(stream);
  }

#endif
  stream.endHeader();
}

void NxForceFieldDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxForceFieldDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(name,"name",true);
#endif
    stream.load(mActor,"mActor",true);
    stream.load(pose,"pose",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"coordinates",false);
    coordinates = (NxForceFieldCoordinates) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"coordinates",false);
     StringToEnum(enumName,coordinates);
  }
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(constant,"constant",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
    stream.load(noisiness,"noisiness",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
    stream.load(linearFalloff,"linearFalloff",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
    stream.load(offset,"offset",false);
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
    stream.load(magnitude,"magnitude",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(positionMultiplier,"positionMultiplier",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(velocityMultiplier,"velocityMultiplier",false);
#endif
#if NX_SDK_VERSION_NUMBER < 280
    stream.load(torusRadius,"torusRadius",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(positionTarget,"positionTarget",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(velocityTarget,"velocityTarget",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(noise,"noise",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    NxForceFieldShapeDesc *shape = 0;
    do
    {
      shape = 0;
			NxI32 v=-1;

      stream.peekHeader(SC_NxSphereForceFieldShapeDesc,&v,
											  SC_NxBoxForceFieldShapeDesc,
												SC_NxCapsuleForceFieldShapeDesc,
												SC_NxConvexForceFieldShapeDesc);
      switch ( v )
      {
        case SC_NxSphereForceFieldShapeDesc:
          if ( 1 )
          {
            NxSphereForceFieldShapeDesc *p = new NxSphereForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxBoxForceFieldShapeDesc:
          if ( 1 )
          {
            NxBoxForceFieldShapeDesc *p = new NxBoxForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxCapsuleForceFieldShapeDesc:
          if ( 1 )
          {
            NxCapsuleForceFieldShapeDesc *p = new NxCapsuleForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxConvexForceFieldShapeDesc:
          if ( 1 )
          {
            NxConvexForceFieldShapeDesc *p = new NxConvexForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        default:
          break;
     }
     if ( shape )
     {
       mFFshapes.push_back(shape);
     }
   } while ( shape );
#endif
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(group,"group",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  groupsMask.load(stream,"groupsMask");
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(falloffLinear,"falloffLinear",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(falloffQuadratic,"falloffQuadratic",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(fluidScale,"fluidScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(clothScale,"clothScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(softBodyScale,"softBodyScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
    stream.load(rigidBodyScale,"rigidBodyScale",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxForceFieldFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxForceFieldFlags,"flags") )
    {
      flags = (NxForceFieldFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxForceFieldFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxForceFieldFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"fluidType",false);
    fluidType = (NxForceFieldType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"fluidType",false);
     StringToEnum(enumName,fluidType);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"clothType",false);
    clothType = (NxForceFieldType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"clothType",false);
     StringToEnum(enumName,clothType);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"softBodyType",false);
    softBodyType = (NxForceFieldType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"softBodyType",false);
     StringToEnum(enumName,softBodyType);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"rigidBodyType",false);
    rigidBodyType = (NxForceFieldType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"rigidBodyType",false);
     StringToEnum(enumName,rigidBodyType);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(forceFieldVariety,"forceFieldVariety",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(mKernel,"mKernel",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    NxForceFieldShapeDesc *shape = 0;
    do
    {
      shape = 0;
			NxI32 v=-1;

      stream.peekHeader(SC_NxSphereForceFieldShapeDesc,&v,
											  SC_NxBoxForceFieldShapeDesc,
												SC_NxCapsuleForceFieldShapeDesc,
												SC_NxConvexForceFieldShapeDesc);
      switch ( v )
      {
        case SC_NxSphereForceFieldShapeDesc:
          if ( 1 )
          {
            NxSphereForceFieldShapeDesc *p = new NxSphereForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxBoxForceFieldShapeDesc:
          if ( 1 )
          {
            NxBoxForceFieldShapeDesc *p = new NxBoxForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxCapsuleForceFieldShapeDesc:
          if ( 1 )
          {
            NxCapsuleForceFieldShapeDesc *p = new NxCapsuleForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        case SC_NxConvexForceFieldShapeDesc:
          if ( 1 )
          {
            NxConvexForceFieldShapeDesc *p = new NxConvexForceFieldShapeDesc;
            p->load(stream);
            shape = static_cast<NxForceFieldShapeDesc *>(p);
          }
          break;
        default:
          break;
     }
     if ( shape )
     {
       mIncludeShapes.push_back(shape);
     }
   } while ( shape );
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  while ( stream.peekHeader(SC_NxForceFieldGroupReference) )
  {
     NxForceFieldGroupReference *temp = new NxForceFieldGroupReference;
     temp->load(stream);
     mShapeGroups.push_back(temp);
   }
#endif
    stream.endHeader();
  }
}

void NxForceFieldDesc::copyFrom(const ::NxForceFieldDesc &desc,CustomCopy &cc)
{
#if NX_SDK_VERSION_NUMBER >= 272
  name = desc.name;
#endif
  pose = desc.pose;
  coordinates = (NxForceFieldCoordinates) desc.coordinates;
#if NX_SDK_VERSION_NUMBER < 280
  constant = desc.constant;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  noisiness = desc.noisiness;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  linearFalloff = desc.linearFalloff;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  offset = desc.offset;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  magnitude = desc.magnitude;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  positionMultiplier = desc.positionMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  velocityMultiplier = desc.velocityMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  torusRadius = desc.torusRadius;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  positionTarget = desc.positionTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  velocityTarget = desc.velocityTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  noise = desc.noise;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  group = desc.group;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  groupsMask.copyFrom(desc.groupsMask,cc);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  falloffLinear = desc.falloffLinear;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  falloffQuadratic = desc.falloffQuadratic;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  fluidScale = desc.fluidScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  clothScale = desc.clothScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  softBodyScale = desc.softBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  rigidBodyScale = desc.rigidBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  flags = (NxForceFieldFlags) desc.flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  fluidType = (NxForceFieldType) desc.fluidType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  clothType = (NxForceFieldType) desc.clothType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  softBodyType = (NxForceFieldType) desc.softBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  rigidBodyType = (NxForceFieldType) desc.rigidBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldVariety = desc.forceFieldVariety;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxForceFieldDesc::copyTo(::NxForceFieldDesc &desc,CustomCopy &cc)
{
#if NX_SDK_VERSION_NUMBER >= 272
  desc.name = name;
#endif
  desc.pose = pose;
  desc.coordinates = (::NxForceFieldCoordinates) coordinates;
#if NX_SDK_VERSION_NUMBER < 280
  desc.constant = constant;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  desc.noisiness = noisiness;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  desc.linearFalloff = linearFalloff;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  desc.offset = offset;
#endif
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  desc.magnitude = magnitude;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.positionMultiplier = positionMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.velocityMultiplier = velocityMultiplier;
#endif
#if NX_SDK_VERSION_NUMBER < 280
  desc.torusRadius = torusRadius;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.positionTarget = positionTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.velocityTarget = velocityTarget;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.noise = noise;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  desc.group = group;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  groupsMask.copyTo(desc.groupsMask,cc);
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.falloffLinear = falloffLinear;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.falloffQuadratic = falloffQuadratic;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.fluidScale = fluidScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.clothScale = clothScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.softBodyScale = softBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  desc.rigidBodyScale = rigidBodyScale;
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  desc.flags = (::NxForceFieldFlags) flags;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.fluidType = (::NxForceFieldType) fluidType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.clothType = (::NxForceFieldType) clothType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.softBodyType = (::NxForceFieldType) softBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  desc.rigidBodyType = (::NxForceFieldType) rigidBodyType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.forceFieldVariety = forceFieldVariety;
#endif
  cc.customCopyTo(desc,*this);
}

#endif

//***********************************************************************************
// Constructor for 'NxSpringAndDamperEffectorDesc'
//***********************************************************************************
NxSpringAndDamperEffectorDesc::NxSpringAndDamperEffectorDesc(void)
{
  ::NxSpringAndDamperEffectorDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
#if NX_SDK_VERSION_NUMBER >= 270
  name                                          = def.name;
#endif
  mBody1                                        = 0;
  mBody2                                        = 0;
  pos1                                          = def.pos1;
  pos2                                          = def.pos2;
  springDistCompressSaturate                    = def.springDistCompressSaturate;
  springDistRelaxed                             = def.springDistRelaxed;
  springDistStretchSaturate                     = def.springDistStretchSaturate;
  springMaxCompressForce                        = def.springMaxCompressForce;
  springMaxStretchForce                         = def.springMaxStretchForce;
  damperVelCompressSaturate                     = def.damperVelCompressSaturate;
  damperVelStretchSaturate                      = def.damperVelStretchSaturate;
  damperMaxCompressForce                        = def.damperMaxCompressForce;
  damperMaxStretchForce                         = def.damperMaxStretchForce;
  mInstance = 0;
}

NxSpringAndDamperEffectorDesc::~NxSpringAndDamperEffectorDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 270
#endif
}

void NxSpringAndDamperEffectorDesc::store(SchemaStream &stream,const char *parent)
{
  NxSpringAndDamperEffectorDesc def;
  stream.beginHeader(SC_NxSpringAndDamperEffectorDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
#if NX_SDK_VERSION_NUMBER >= 270
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
#endif
  if ( gSaveDefaults || def.mBody1 != mBody1 )
    stream.store(mBody1,"mBody1",true);
  if ( gSaveDefaults || def.mBody2 != mBody2 )
    stream.store(mBody2,"mBody2",true);
  if ( gSaveDefaults || def.pos1 != pos1 )
    stream.store(pos1,"pos1",false);
  if ( gSaveDefaults || def.pos2 != pos2 )
    stream.store(pos2,"pos2",false);
  if ( gSaveDefaults || def.springDistCompressSaturate != springDistCompressSaturate )
    stream.store(springDistCompressSaturate,"springDistCompressSaturate",false);
  if ( gSaveDefaults || def.springDistRelaxed != springDistRelaxed )
    stream.store(springDistRelaxed,"springDistRelaxed",false);
  if ( gSaveDefaults || def.springDistStretchSaturate != springDistStretchSaturate )
    stream.store(springDistStretchSaturate,"springDistStretchSaturate",false);
  if ( gSaveDefaults || def.springMaxCompressForce != springMaxCompressForce )
    stream.store(springMaxCompressForce,"springMaxCompressForce",false);
  if ( gSaveDefaults || def.springMaxStretchForce != springMaxStretchForce )
    stream.store(springMaxStretchForce,"springMaxStretchForce",false);
  if ( gSaveDefaults || def.damperVelCompressSaturate != damperVelCompressSaturate )
    stream.store(damperVelCompressSaturate,"damperVelCompressSaturate",false);
  if ( gSaveDefaults || def.damperVelStretchSaturate != damperVelStretchSaturate )
    stream.store(damperVelStretchSaturate,"damperVelStretchSaturate",false);
  if ( gSaveDefaults || def.damperMaxCompressForce != damperMaxCompressForce )
    stream.store(damperMaxCompressForce,"damperMaxCompressForce",false);
  if ( gSaveDefaults || def.damperMaxStretchForce != damperMaxStretchForce )
    stream.store(damperMaxStretchForce,"damperMaxStretchForce",false);
  stream.endHeader();
}

void NxSpringAndDamperEffectorDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSpringAndDamperEffectorDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
#if NX_SDK_VERSION_NUMBER >= 270
    stream.load(name,"name",true);
#endif
    stream.load(mBody1,"mBody1",true);
    stream.load(mBody2,"mBody2",true);
    stream.load(pos1,"pos1",false);
    stream.load(pos2,"pos2",false);
    stream.load(springDistCompressSaturate,"springDistCompressSaturate",false);
    stream.load(springDistRelaxed,"springDistRelaxed",false);
    stream.load(springDistStretchSaturate,"springDistStretchSaturate",false);
    stream.load(springMaxCompressForce,"springMaxCompressForce",false);
    stream.load(springMaxStretchForce,"springMaxStretchForce",false);
    stream.load(damperVelCompressSaturate,"damperVelCompressSaturate",false);
    stream.load(damperVelStretchSaturate,"damperVelStretchSaturate",false);
    stream.load(damperMaxCompressForce,"damperMaxCompressForce",false);
    stream.load(damperMaxStretchForce,"damperMaxStretchForce",false);
    stream.endHeader();
  }
}

void NxSpringAndDamperEffectorDesc::copyFrom(const ::NxSpringAndDamperEffectorDesc &desc,CustomCopy &cc)
{
#if NX_SDK_VERSION_NUMBER >= 270
  name = desc.name;
#endif
  pos1 = desc.pos1;
  pos2 = desc.pos2;
  springDistCompressSaturate = desc.springDistCompressSaturate;
  springDistRelaxed = desc.springDistRelaxed;
  springDistStretchSaturate = desc.springDistStretchSaturate;
  springMaxCompressForce = desc.springMaxCompressForce;
  springMaxStretchForce = desc.springMaxStretchForce;
  damperVelCompressSaturate = desc.damperVelCompressSaturate;
  damperVelStretchSaturate = desc.damperVelStretchSaturate;
  damperMaxCompressForce = desc.damperMaxCompressForce;
  damperMaxStretchForce = desc.damperMaxStretchForce;
  cc.customCopyFrom(*this,desc);
}

void NxSpringAndDamperEffectorDesc::copyTo(::NxSpringAndDamperEffectorDesc &desc,CustomCopy &cc)
{
#if NX_SDK_VERSION_NUMBER >= 270
  desc.name = name;
#endif
  desc.pos1 = pos1;
  desc.pos2 = pos2;
  desc.springDistCompressSaturate = springDistCompressSaturate;
  desc.springDistRelaxed = springDistRelaxed;
  desc.springDistStretchSaturate = springDistStretchSaturate;
  desc.springMaxCompressForce = springMaxCompressForce;
  desc.springMaxStretchForce = springMaxStretchForce;
  desc.damperVelCompressSaturate = damperVelCompressSaturate;
  desc.damperVelStretchSaturate = damperVelStretchSaturate;
  desc.damperMaxCompressForce = damperMaxCompressForce;
  desc.damperMaxStretchForce = damperMaxStretchForce;
  cc.customCopyTo(desc,*this);
}

#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
// Constructor for 'NxSoftBodyMeshDesc'
//***********************************************************************************
NxSoftBodyMeshDesc::NxSoftBodyMeshDesc(void)
{
  ::NxSoftBodyMeshDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  // NxArray< NxVec3 >: mVertices
  // NxArray< NxTetra >: mTetrahedra
  flags                                         = def.flags;
  // NxArray< NxF32 >: mVertexMasses
  // NxArray< NxU32 >: mVertexFlags
  mInstance = 0;
}

NxSoftBodyMeshDesc::~NxSoftBodyMeshDesc(void)
{
}

void NxSoftBodyMeshDesc::store(SchemaStream &stream,const char *parent)
{
  NxSoftBodyMeshDesc def;
  stream.beginHeader(SC_NxSoftBodyMeshDesc);
  bool saveCooked = gSaveCooked;
	 gSaveCooked = false;
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  stream.store(mVertices,"mVertices",false);
  stream.store(mTetrahedra,"mTetrahedra",false);
  if ( gSaveDefaults || def.flags != flags )
    stream.store(flags,"flags",false);
  stream.store(mVertexMasses,"mVertexMasses",false);
  stream.store(mVertexFlags,"mVertexFlags",false);
	 gSaveCooked = saveCooked;
  stream.endHeader();
}

void NxSoftBodyMeshDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSoftBodyMeshDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
  stream.load(mVertices,"mVertices",false);
  stream.load(mTetrahedra,"mTetrahedra",false);
    stream.load(flags,"flags",false);
  stream.load(mVertexMasses,"mVertexMasses",false);
  stream.load(mVertexFlags,"mVertexFlags",false);
    stream.endHeader();
  }
}

void NxSoftBodyMeshDesc::copyFrom(const ::NxSoftBodyMeshDesc &desc,CustomCopy &cc)
{
  flags = desc.flags;
  cc.customCopyFrom(*this,desc);
}

void NxSoftBodyMeshDesc::copyTo(::NxSoftBodyMeshDesc &desc,CustomCopy &cc)
{
  desc.flags = flags;
  cc.customCopyTo(desc,*this);
}

#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
//***********************************************************************************
#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
// Constructor for 'NxSoftBodyAttachDesc'
//***********************************************************************************
NxSoftBodyAttachDesc::NxSoftBodyAttachDesc(void)
{
  mAttachActor                                  = 0;
  mAttachShapeIndex                             = 0;
  // NxSoftBodyAttachmentFlag: flags
  mInstance = 0;
}

NxSoftBodyAttachDesc::~NxSoftBodyAttachDesc(void)
{
}

void NxSoftBodyAttachDesc::store(SchemaStream &stream,const char *parent)
{
  NxSoftBodyAttachDesc def;
  stream.beginHeader(SC_NxSoftBodyAttachDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.mAttachActor != mAttachActor )
    stream.store(mAttachActor,"mAttachActor",true);
  if ( gSaveDefaults || def.mAttachShapeIndex != mAttachShapeIndex )
    stream.store(mAttachShapeIndex,"mAttachShapeIndex",true);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",true);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxSoftBodyAttachmentFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxSoftBodyAttachmentFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,true);
      }
      stream.endHeader();
    }
  }
  stream.endHeader();
}

void NxSoftBodyAttachDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSoftBodyAttachDesc,parent) )
  {
    stream.load(mAttachActor,"mAttachActor",true);
    stream.load(mAttachShapeIndex,"mAttachShapeIndex",true);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",true);
    flags = (NxSoftBodyAttachmentFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxSoftBodyAttachmentFlag,"flags") )
    {
      flags = (NxSoftBodyAttachmentFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxSoftBodyAttachmentFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxSoftBodyAttachmentFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.endHeader();
  }
}

#endif
#if NX_USE_SOFTBODY_API

//***********************************************************************************
// Constructor for 'NxSoftBodyDesc'
//***********************************************************************************
NxSoftBodyDesc::NxSoftBodyDesc(void)
{
  ::NxSoftBodyDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  name                                          = def.name;
  mSoftBodyMesh                                 = 0;
  globalPose                                    = def.globalPose;
  particleRadius                                = def.particleRadius;
  density                                       = def.density;
  volumeStiffness                               = def.volumeStiffness;
  stretchingStiffness                           = def.stretchingStiffness;
  dampingCoefficient                            = def.dampingCoefficient;
  friction                                      = def.friction;
  tearFactor                                    = def.tearFactor;
  collisionResponseCoefficient                  = def.collisionResponseCoefficient;
  attachmentResponseCoefficient                 = def.attachmentResponseCoefficient;
  attachmentTearFactor                          = def.attachmentTearFactor;
  toFluidResponseCoefficient                    = def.toFluidResponseCoefficient;
  fromFluidResponseCoefficient                  = def.fromFluidResponseCoefficient;
  solverIterations                              = def.solverIterations;
  externalAcceleration                          = def.externalAcceleration;
  wakeUpCounter                                 = def.wakeUpCounter;
  sleepLinearVelocity                           = def.sleepLinearVelocity;
  collisionGroup                                = def.collisionGroup;
  // NxGroupsMask: groupsMask
  // NxBounds3: validBounds
  flags                                         = (NxSoftBodyFlag) def.flags;
  mCompartment                                  = 0;
  // NxArray< NxU8 >: mActiveState
  // NxArray< NxSoftBodyAttachDesc *>: mAttachments
#if NX_SDK_VERSION_NUMBER >= 272
  relativeGridSpacing                           = def.relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  minAdhereVelocity                             = def.minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial                            = def.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  selfCollisionThickness                        = def.selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hardStretchLimitationFactor                   = def.hardStretchLimitationFactor;
#endif
  mInstance = 0;
}

NxSoftBodyDesc::~NxSoftBodyDesc(void)
{
  for (NxU32 i=0; i<mAttachments.size(); i++)
  {
     NxSoftBodyAttachDesc  *v = mAttachments[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
#if NX_SDK_VERSION_NUMBER >= 283
#endif
}

void NxSoftBodyDesc::store(SchemaStream &stream,const char *parent)
{
  NxSoftBodyDesc def;
  stream.beginHeader(SC_NxSoftBodyDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.name != name )
    stream.store(name,"name",true);
  if ( gSaveDefaults || def.mSoftBodyMesh != mSoftBodyMesh )
    stream.store(mSoftBodyMesh,"mSoftBodyMesh",true);
  if ( gSaveDefaults || !isSame(def.globalPose,globalPose) )
    stream.store(globalPose,"globalPose",false);
  if ( gSaveDefaults || def.particleRadius != particleRadius )
    stream.store(particleRadius,"particleRadius",false);
  if ( gSaveDefaults || def.density != density )
    stream.store(density,"density",false);
  if ( gSaveDefaults || def.volumeStiffness != volumeStiffness )
    stream.store(volumeStiffness,"volumeStiffness",false);
  if ( gSaveDefaults || def.stretchingStiffness != stretchingStiffness )
    stream.store(stretchingStiffness,"stretchingStiffness",false);
  if ( gSaveDefaults || def.dampingCoefficient != dampingCoefficient )
    stream.store(dampingCoefficient,"dampingCoefficient",false);
  if ( gSaveDefaults || def.friction != friction )
    stream.store(friction,"friction",false);
  if ( gSaveDefaults || def.tearFactor != tearFactor )
    stream.store(tearFactor,"tearFactor",false);
  if ( gSaveDefaults || def.collisionResponseCoefficient != collisionResponseCoefficient )
    stream.store(collisionResponseCoefficient,"collisionResponseCoefficient",false);
  if ( gSaveDefaults || def.attachmentResponseCoefficient != attachmentResponseCoefficient )
    stream.store(attachmentResponseCoefficient,"attachmentResponseCoefficient",false);
  if ( gSaveDefaults || def.attachmentTearFactor != attachmentTearFactor )
    stream.store(attachmentTearFactor,"attachmentTearFactor",false);
  if ( gSaveDefaults || def.toFluidResponseCoefficient != toFluidResponseCoefficient )
    stream.store(toFluidResponseCoefficient,"toFluidResponseCoefficient",false);
  if ( gSaveDefaults || def.fromFluidResponseCoefficient != fromFluidResponseCoefficient )
    stream.store(fromFluidResponseCoefficient,"fromFluidResponseCoefficient",false);
  if ( gSaveDefaults || def.solverIterations != solverIterations )
    stream.store(solverIterations,"solverIterations",false);
  if ( gSaveDefaults || def.externalAcceleration != externalAcceleration )
    stream.store(externalAcceleration,"externalAcceleration",false);
  if ( gSaveDefaults || def.wakeUpCounter != wakeUpCounter )
    stream.store(wakeUpCounter,"wakeUpCounter",false);
  if ( gSaveDefaults || def.sleepLinearVelocity != sleepLinearVelocity )
    stream.store(sleepLinearVelocity,"sleepLinearVelocity",false);
  if ( gSaveDefaults || def.collisionGroup != collisionGroup )
    stream.store(collisionGroup,"collisionGroup",false);
  groupsMask.store(stream,"groupsMask");
  if ( gSaveDefaults || !isSame(def.validBounds,validBounds) )
    stream.store(validBounds,"validBounds",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxSoftBodyFlag);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxSoftBodyFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.mCompartment != mCompartment )
    stream.store(mCompartment,"mCompartment",false);
  stream.store(mActiveState,"mActiveState",false);
  for (NxU32 i=0; i<mAttachments.size(); i++)
  {
    NxSoftBodyAttachDesc *v = mAttachments[i];
    v->store(stream);
  }

#if NX_SDK_VERSION_NUMBER >= 272
  if ( gSaveDefaults || def.relativeGridSpacing != relativeGridSpacing )
    stream.store(relativeGridSpacing,"relativeGridSpacing",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.minAdhereVelocity != minAdhereVelocity )
    stream.store(minAdhereVelocity,"minAdhereVelocity",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.forceFieldMaterial != forceFieldMaterial )
    stream.store(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.selfCollisionThickness != selfCollisionThickness )
    stream.store(selfCollisionThickness,"selfCollisionThickness",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  if ( gSaveDefaults || def.hardStretchLimitationFactor != hardStretchLimitationFactor )
    stream.store(hardStretchLimitationFactor,"hardStretchLimitationFactor",false);
#endif
  stream.endHeader();
}

void NxSoftBodyDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSoftBodyDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(name,"name",true);
    stream.load(mSoftBodyMesh,"mSoftBodyMesh",true);
    stream.load(globalPose,"globalPose",false);
    stream.load(particleRadius,"particleRadius",false);
    stream.load(density,"density",false);
    stream.load(volumeStiffness,"volumeStiffness",false);
    stream.load(stretchingStiffness,"stretchingStiffness",false);
    stream.load(dampingCoefficient,"dampingCoefficient",false);
    stream.load(friction,"friction",false);
    stream.load(tearFactor,"tearFactor",false);
    stream.load(collisionResponseCoefficient,"collisionResponseCoefficient",false);
    stream.load(attachmentResponseCoefficient,"attachmentResponseCoefficient",false);
    stream.load(attachmentTearFactor,"attachmentTearFactor",false);
    stream.load(toFluidResponseCoefficient,"toFluidResponseCoefficient",false);
    stream.load(fromFluidResponseCoefficient,"fromFluidResponseCoefficient",false);
    stream.load(solverIterations,"solverIterations",false);
    stream.load(externalAcceleration,"externalAcceleration",false);
    stream.load(wakeUpCounter,"wakeUpCounter",false);
    stream.load(sleepLinearVelocity,"sleepLinearVelocity",false);
    stream.load(collisionGroup,"collisionGroup",false);
  groupsMask.load(stream,"groupsMask");
    stream.load(validBounds,"validBounds",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxSoftBodyFlag) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxSoftBodyFlag,"flags") )
    {
      flags = (NxSoftBodyFlag) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxSoftBodyFlag) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxSoftBodyFlag) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(mCompartment,"mCompartment",false);
  stream.load(mActiveState,"mActiveState",false);
  while ( stream.peekHeader(SC_NxSoftBodyAttachDesc) )
  {
     NxSoftBodyAttachDesc *temp = new NxSoftBodyAttachDesc;
     temp->load(stream);
     mAttachments.push_back(temp);
   }
#if NX_SDK_VERSION_NUMBER >= 272
    stream.load(relativeGridSpacing,"relativeGridSpacing",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(minAdhereVelocity,"minAdhereVelocity",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(forceFieldMaterial,"forceFieldMaterial",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(selfCollisionThickness,"selfCollisionThickness",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 283
    stream.load(hardStretchLimitationFactor,"hardStretchLimitationFactor",false);
#endif
    stream.endHeader();
  }
}

void NxSoftBodyDesc::copyFrom(const ::NxSoftBodyDesc &desc,CustomCopy &cc)
{
  name = desc.name;
  globalPose = desc.globalPose;
  particleRadius = desc.particleRadius;
  density = desc.density;
  volumeStiffness = desc.volumeStiffness;
  stretchingStiffness = desc.stretchingStiffness;
  dampingCoefficient = desc.dampingCoefficient;
  friction = desc.friction;
  tearFactor = desc.tearFactor;
  collisionResponseCoefficient = desc.collisionResponseCoefficient;
  attachmentResponseCoefficient = desc.attachmentResponseCoefficient;
  attachmentTearFactor = desc.attachmentTearFactor;
  toFluidResponseCoefficient = desc.toFluidResponseCoefficient;
  fromFluidResponseCoefficient = desc.fromFluidResponseCoefficient;
  solverIterations = desc.solverIterations;
  externalAcceleration = desc.externalAcceleration;
  wakeUpCounter = desc.wakeUpCounter;
  sleepLinearVelocity = desc.sleepLinearVelocity;
  collisionGroup = desc.collisionGroup;
  groupsMask.copyFrom(desc.groupsMask,cc);
  validBounds = desc.validBounds;
  flags = (NxSoftBodyFlag) desc.flags;
#if NX_SDK_VERSION_NUMBER >= 272
  relativeGridSpacing = desc.relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  minAdhereVelocity = desc.minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  forceFieldMaterial = desc.forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  selfCollisionThickness = desc.selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  hardStretchLimitationFactor = desc.hardStretchLimitationFactor;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxSoftBodyDesc::copyTo(::NxSoftBodyDesc &desc,CustomCopy &cc)
{
  desc.name = name;
  desc.globalPose = globalPose;
  desc.particleRadius = particleRadius;
  desc.density = density;
  desc.volumeStiffness = volumeStiffness;
  desc.stretchingStiffness = stretchingStiffness;
  desc.dampingCoefficient = dampingCoefficient;
  desc.friction = friction;
  desc.tearFactor = tearFactor;
  desc.collisionResponseCoefficient = collisionResponseCoefficient;
  desc.attachmentResponseCoefficient = attachmentResponseCoefficient;
  desc.attachmentTearFactor = attachmentTearFactor;
  desc.toFluidResponseCoefficient = toFluidResponseCoefficient;
  desc.fromFluidResponseCoefficient = fromFluidResponseCoefficient;
  desc.solverIterations = solverIterations;
  desc.externalAcceleration = externalAcceleration;
  desc.wakeUpCounter = wakeUpCounter;
  desc.sleepLinearVelocity = sleepLinearVelocity;
  desc.collisionGroup = collisionGroup;
  groupsMask.copyTo(desc.groupsMask,cc);
  desc.validBounds = validBounds;
  desc.flags = (::NxSoftBodyFlag) flags;
#if NX_SDK_VERSION_NUMBER >= 272
  desc.relativeGridSpacing = relativeGridSpacing;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.minAdhereVelocity = minAdhereVelocity;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.forceFieldMaterial = forceFieldMaterial;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.selfCollisionThickness = selfCollisionThickness;
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  desc.hardStretchLimitationFactor = hardStretchLimitationFactor;
#endif
  cc.customCopyTo(desc,*this);
}

#endif

//***********************************************************************************
// Constructor for 'NxSceneDesc'
//***********************************************************************************
NxSceneDesc::NxSceneDesc(void)
{
  ::NxSceneDesc def;
  mId                                           = 0;
  mUserProperties                               = 0;
  mHasMaxBounds                                 = false;
  mHasLimits                                    = false;
  mHasFilter                                    = false;
  mFilterBool                                   = false;
  mFilterOp0                                    = NX_FILTEROP_AND;
  mFilterOp1                                    = NX_FILTEROP_AND;
  mFilterOp2                                    = NX_FILTEROP_AND;
  // NxGroupsMask: mGroupMask0
  // NxGroupsMask: mGroupMask1
  gravity                                       = def.gravity;
  maxTimestep                                   = 0.01666666f;
  maxIter                                       = 8;
  timeStepMethod                                = NX_TIMESTEP_FIXED;
  // NxBounds3: mMaxBounds
  // NxSceneLimits: mLimits
  simType                                       = NX_SIMULATION_SW;
  groundPlane                                   = def.groundPlane;
  boundsPlanes                                  = def.boundsPlanes;
  flags                                         = (NxSceneFlags) def.flags;
  internalThreadCount                           = def.internalThreadCount;
  backgroundThreadCount                         = def.backgroundThreadCount;
  threadMask                                    = 1431655764;
  backgroundThreadMask                          = 1431655764;
#if NX_SDK_VERSION_NUMBER >= 250
  simThreadStackSize                            = def.simThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  simThreadPriority                             = NX_TP_NORMAL;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  simThreadMask                                 = def.simThreadMask;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  workerThreadStackSize                         = def.workerThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  workerThreadPriority                          = NX_TP_HIGH;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  backgroundThreadPriority                      = NX_TP_NORMAL;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  upAxis                                        = def.upAxis;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  subdivisionLevel                              = def.subdivisionLevel;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  staticStructure                               = (NxPruningStructure) def.staticStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  dynamicStructure                              = (NxPruningStructure) def.dynamicStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  dynamicTreeRebuildRateHint                    = def.dynamicTreeRebuildRateHint;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  bpType                                        = (NxBroadPhaseType) def.bpType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  nbGridCellsX                                  = def.nbGridCellsX;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  nbGridCellsY                                  = def.nbGridCellsY;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  solverBatchSize                               = def.solverBatchSize;
#endif
  // NxArray< NxMaterialDesc * >: mMaterials
  // NxArray< NxActorDesc *>: mActors
  // NxArray< NxJointDesc *>: mJoints
  // NxArray< NxPairFlagDesc *>: mPairFlags
  // NxArray< NxSpringAndDamperEffectorDesc *>: mEffectors
#if NX_SDK_VERSION_NUMBER >= 270
  // NxArray< NxForceFieldDesc *>: mForceFields
#endif
  // NxArray< NxCollisionGroupDesc *>: mCollisionGroups
  // NxArray< NxActorGroupPair *>: mActorGroups
#if NX_SDK_VERSION_NUMBER >= 260
  // NxArray< NxCompartmentDesc *>: mCompartments
#endif
#if NX_USE_FLUID_API
  // NxArray< NxFluidDesc *>: mFluids
#endif
  // NxArray< NxClothDesc *>: mCloths
#if NX_USE_SOFTBODY_API
  // NxArray< NxSoftBodyDesc *>: mSoftBodies
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  // NxArray< NxConstraintDominanceDesc * >: mDominanceGroupPairs
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  // NxArray< NxForceFieldScaleTableEntry * >: mForceFieldScaleTable
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  // NxArray< NxForceFieldShapeGroupDesc * >: mForceFieldShapeGroups
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  // NxArray< NxForceFieldLinearKernelDesc * >: mForceFieldLinearKernels
#endif
  mMaxBounds.min.set(FLT_MIN,FLT_MIN,FLT_MIN);
  mMaxBounds.max.set(FLT_MAX,FLT_MAX,FLT_MAX);
  mInstance = 0;
}

NxSceneDesc::~NxSceneDesc(void)
{
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 271
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 262
#endif
#if NX_SDK_VERSION_NUMBER >= 262
#endif
#if NX_SDK_VERSION_NUMBER >= 262
#endif
#if NX_SDK_VERSION_NUMBER >= 262
#endif
#if NX_SDK_VERSION_NUMBER >= 281
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
  for (NxU32 i=0; i<mMaterials.size(); i++)
  {
     NxMaterialDesc  *v = mMaterials[i];
    delete v;
  }

  for (NxU32 i=0; i<mActors.size(); i++)
  {
     NxActorDesc  *v = mActors[i];
    delete v;
  }

  for (NxU32 i=0; i<mJoints.size(); i++)
  {
     NxJointDesc  *v = mJoints[i];
    delete v;
  }

  for (NxU32 i=0; i<mPairFlags.size(); i++)
  {
     NxPairFlagDesc  *v = mPairFlags[i];
    delete v;
  }

  for (NxU32 i=0; i<mEffectors.size(); i++)
  {
     NxSpringAndDamperEffectorDesc  *v = mEffectors[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 270
  for (NxU32 i=0; i<mForceFields.size(); i++)
  {
     NxForceFieldDesc  *v = mForceFields[i];
    delete v;
  }

#endif
  for (NxU32 i=0; i<mCollisionGroups.size(); i++)
  {
     NxCollisionGroupDesc  *v = mCollisionGroups[i];
    delete v;
  }

  for (NxU32 i=0; i<mActorGroups.size(); i++)
  {
     NxActorGroupPair  *v = mActorGroups[i];
    delete v;
  }

#if NX_SDK_VERSION_NUMBER >= 260
  for (NxU32 i=0; i<mCompartments.size(); i++)
  {
     NxCompartmentDesc  *v = mCompartments[i];
    delete v;
  }

#endif
#if NX_USE_FLUID_API
  for (NxU32 i=0; i<mFluids.size(); i++)
  {
     NxFluidDesc  *v = mFluids[i];
    delete v;
  }

#endif
  for (NxU32 i=0; i<mCloths.size(); i++)
  {
     NxClothDesc  *v = mCloths[i];
    delete v;
  }

#if NX_USE_SOFTBODY_API
  for (NxU32 i=0; i<mSoftBodies.size(); i++)
  {
     NxSoftBodyDesc  *v = mSoftBodies[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 272
  for (NxU32 i=0; i<mDominanceGroupPairs.size(); i++)
  {
     NxConstraintDominanceDesc  *v = mDominanceGroupPairs[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldScaleTable.size(); i++)
  {
     NxForceFieldScaleTableEntry  *v = mForceFieldScaleTable[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldShapeGroups.size(); i++)
  {
     NxForceFieldShapeGroupDesc  *v = mForceFieldShapeGroups[i];
    delete v;
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldLinearKernels.size(); i++)
  {
     NxForceFieldLinearKernelDesc  *v = mForceFieldLinearKernels[i];
    delete v;
  }

#endif
}

void NxSceneDesc::store(SchemaStream &stream,const char *parent)
{
  NxSceneDesc def;
  stream.beginHeader(SC_NxSceneDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mHasMaxBounds != mHasMaxBounds )
    stream.store(mHasMaxBounds,"mHasMaxBounds",true);
  if ( gSaveDefaults || def.mHasLimits != mHasLimits )
    stream.store(mHasLimits,"mHasLimits",true);
  if ( gSaveDefaults || def.mHasFilter != mHasFilter )
    stream.store(mHasFilter,"mHasFilter",true);
  if ( gSaveDefaults || def.mFilterBool != mFilterBool )
    stream.store(mFilterBool,"mFilterBool",false);
  if ( stream.isBinary() )
    stream.store((NxU32)mFilterOp0,"mFilterOp0",false);
   else
   {
     if ( gSaveDefaults || def.mFilterOp0 != mFilterOp0 )
      stream.store(EnumToString(mFilterOp0),"mFilterOp0",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)mFilterOp1,"mFilterOp1",false);
   else
   {
     if ( gSaveDefaults || def.mFilterOp1 != mFilterOp1 )
      stream.store(EnumToString(mFilterOp1),"mFilterOp1",false);
   }
  if ( stream.isBinary() )
    stream.store((NxU32)mFilterOp2,"mFilterOp2",false);
   else
   {
     if ( gSaveDefaults || def.mFilterOp2 != mFilterOp2 )
      stream.store(EnumToString(mFilterOp2),"mFilterOp2",false);
   }
  mGroupMask0.store(stream,"mGroupMask0");
  mGroupMask1.store(stream,"mGroupMask1");
  if ( gSaveDefaults || def.gravity != gravity )
    stream.store(gravity,"gravity",false);
  if ( gSaveDefaults || def.maxTimestep != maxTimestep )
    stream.store(maxTimestep,"maxTimestep",false);
  if ( gSaveDefaults || def.maxIter != maxIter )
    stream.store(maxIter,"maxIter",false);
  if ( stream.isBinary() )
    stream.store((NxU32)timeStepMethod,"timeStepMethod",false);
   else
   {
     if ( gSaveDefaults || def.timeStepMethod != timeStepMethod )
      stream.store(EnumToString(timeStepMethod),"timeStepMethod",false);
   }
  if ( gSaveDefaults || !isSame(def.mMaxBounds,mMaxBounds) )
    stream.store(mMaxBounds,"mMaxBounds",false);
  mLimits.store(stream,"mLimits");
  if ( stream.isBinary() )
    stream.store((NxU32)simType,"simType",false);
   else
   {
     if ( gSaveDefaults || def.simType != simType )
      stream.store(EnumToString(simType),"simType",false);
   }
  if ( gSaveDefaults || def.groundPlane != groundPlane )
    stream.store(groundPlane,"groundPlane",false);
  if ( gSaveDefaults || def.boundsPlanes != boundsPlanes )
    stream.store(boundsPlanes,"boundsPlanes",false);
  if ( stream.isBinary() )
  {
    stream.store((NxU32)flags,"flags",false);
  }
  else
  {
    if ( gSaveDefaults || def.flags != flags )
    {
      stream.beginHeader(SC_NxSceneFlags);
      stream.store("flags","mId",true);
      for (int i=0; i<32; i++)
      {
	       int shift = (1<<i);
        const char *str = EnumToString( (NxSceneFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
	       if ( flags & shift )
          b = true;
        stream.store(b,str,false);
      }
      stream.endHeader();
    }
  }
  if ( gSaveDefaults || def.internalThreadCount != internalThreadCount )
    stream.store(internalThreadCount,"internalThreadCount",false);
  if ( gSaveDefaults || def.backgroundThreadCount != backgroundThreadCount )
    stream.store(backgroundThreadCount,"backgroundThreadCount",false);
  if ( gSaveDefaults || def.threadMask != threadMask )
    stream.store(threadMask,"threadMask",false);
  if ( gSaveDefaults || def.backgroundThreadMask != backgroundThreadMask )
    stream.store(backgroundThreadMask,"backgroundThreadMask",false);
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.simThreadStackSize != simThreadStackSize )
    stream.store(simThreadStackSize,"simThreadStackSize",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  if ( stream.isBinary() )
    stream.store((NxU32)simThreadPriority,"simThreadPriority",false);
   else
   {
     if ( gSaveDefaults || def.simThreadPriority != simThreadPriority )
      stream.store(EnumToString(simThreadPriority),"simThreadPriority",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  if ( gSaveDefaults || def.simThreadMask != simThreadMask )
    stream.store(simThreadMask,"simThreadMask",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( gSaveDefaults || def.workerThreadStackSize != workerThreadStackSize )
    stream.store(workerThreadStackSize,"workerThreadStackSize",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
    stream.store((NxU32)workerThreadPriority,"workerThreadPriority",false);
   else
   {
     if ( gSaveDefaults || def.workerThreadPriority != workerThreadPriority )
      stream.store(EnumToString(workerThreadPriority),"workerThreadPriority",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  if ( stream.isBinary() )
    stream.store((NxU32)backgroundThreadPriority,"backgroundThreadPriority",false);
   else
   {
     if ( gSaveDefaults || def.backgroundThreadPriority != backgroundThreadPriority )
      stream.store(EnumToString(backgroundThreadPriority),"backgroundThreadPriority",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( gSaveDefaults || def.upAxis != upAxis )
    stream.store(upAxis,"upAxis",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( gSaveDefaults || def.subdivisionLevel != subdivisionLevel )
    stream.store(subdivisionLevel,"subdivisionLevel",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( stream.isBinary() )
    stream.store((NxU32)staticStructure,"staticStructure",false);
   else
   {
     if ( gSaveDefaults || def.staticStructure != staticStructure )
      stream.store(EnumToString(staticStructure),"staticStructure",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( stream.isBinary() )
    stream.store((NxU32)dynamicStructure,"dynamicStructure",false);
   else
   {
     if ( gSaveDefaults || def.dynamicStructure != dynamicStructure )
      stream.store(EnumToString(dynamicStructure),"dynamicStructure",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  if ( gSaveDefaults || def.dynamicTreeRebuildRateHint != dynamicTreeRebuildRateHint )
    stream.store(dynamicTreeRebuildRateHint,"dynamicTreeRebuildRateHint",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( stream.isBinary() )
    stream.store((NxU32)bpType,"bpType",false);
   else
   {
     if ( gSaveDefaults || def.bpType != bpType )
      stream.store(EnumToString(bpType),"bpType",false);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.nbGridCellsX != nbGridCellsX )
    stream.store(nbGridCellsX,"nbGridCellsX",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.nbGridCellsY != nbGridCellsY )
    stream.store(nbGridCellsY,"nbGridCellsY",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( gSaveDefaults || def.solverBatchSize != solverBatchSize )
    stream.store(solverBatchSize,"solverBatchSize",false);
#endif
  for (NxU32 i=0; i<mMaterials.size(); i++)
  {
    NxMaterialDesc *v = mMaterials[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mActors.size(); i++)
  {
    NxActorDesc *v = mActors[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mJoints.size(); i++)
  {
    NxJointDesc *v = mJoints[i];
    switch ( v->mType )
    {
      case SC_NxD6JointDesc:
        if ( 1 )
        {
          NxD6JointDesc *p = static_cast<NxD6JointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxCylindricalJointDesc:
        if ( 1 )
        {
          NxCylindricalJointDesc *p = static_cast<NxCylindricalJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxDistanceJointDesc:
        if ( 1 )
        {
          NxDistanceJointDesc *p = static_cast<NxDistanceJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxFixedJointDesc:
        if ( 1 )
        {
          NxFixedJointDesc *p = static_cast<NxFixedJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxPointInPlaneJointDesc:
        if ( 1 )
        {
          NxPointInPlaneJointDesc *p = static_cast<NxPointInPlaneJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxPointOnLineJointDesc:
        if ( 1 )
        {
          NxPointOnLineJointDesc *p = static_cast<NxPointOnLineJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxPrismaticJointDesc:
        if ( 1 )
        {
          NxPrismaticJointDesc *p = static_cast<NxPrismaticJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxRevoluteJointDesc:
        if ( 1 )
        {
          NxRevoluteJointDesc *p = static_cast<NxRevoluteJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxSphericalJointDesc:
        if ( 1 )
        {
          NxSphericalJointDesc *p = static_cast<NxSphericalJointDesc *>(v);
          p->store(stream);
        }
        break;
      case SC_NxPulleyJointDesc:
        if ( 1 )
        {
          NxPulleyJointDesc *p = static_cast<NxPulleyJointDesc *>(v);
          p->store(stream);
        }
        break;
      default:
        break;
    }
  }

  for (NxU32 i=0; i<mPairFlags.size(); i++)
  {
    NxPairFlagDesc *v = mPairFlags[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mEffectors.size(); i++)
  {
    NxSpringAndDamperEffectorDesc *v = mEffectors[i];
    v->store(stream);
  }

#if NX_SDK_VERSION_NUMBER >= 270
  for (NxU32 i=0; i<mForceFields.size(); i++)
  {
    NxForceFieldDesc *v = mForceFields[i];
    v->store(stream);
  }

#endif
  for (NxU32 i=0; i<mCollisionGroups.size(); i++)
  {
    NxCollisionGroupDesc *v = mCollisionGroups[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mActorGroups.size(); i++)
  {
    NxActorGroupPair *v = mActorGroups[i];
    v->store(stream);
  }

#if NX_SDK_VERSION_NUMBER >= 260
  for (NxU32 i=0; i<mCompartments.size(); i++)
  {
    NxCompartmentDesc *v = mCompartments[i];
    v->store(stream);
  }

#endif
#if NX_USE_FLUID_API
  for (NxU32 i=0; i<mFluids.size(); i++)
  {
    NxFluidDesc *v = mFluids[i];
    v->store(stream);
  }

#endif
  for (NxU32 i=0; i<mCloths.size(); i++)
  {
    NxClothDesc *v = mCloths[i];
    v->store(stream);
  }

#if NX_USE_SOFTBODY_API
  for (NxU32 i=0; i<mSoftBodies.size(); i++)
  {
    NxSoftBodyDesc *v = mSoftBodies[i];
    v->store(stream);
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 272
  for (NxU32 i=0; i<mDominanceGroupPairs.size(); i++)
  {
    NxConstraintDominanceDesc *v = mDominanceGroupPairs[i];
    v->store(stream);
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldScaleTable.size(); i++)
  {
    NxForceFieldScaleTableEntry *v = mForceFieldScaleTable[i];
    v->store(stream);
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldShapeGroups.size(); i++)
  {
    NxForceFieldShapeGroupDesc *v = mForceFieldShapeGroups[i];
    v->store(stream);
  }

#endif
#if NX_SDK_VERSION_NUMBER >= 280
  for (NxU32 i=0; i<mForceFieldLinearKernels.size(); i++)
  {
    NxForceFieldLinearKernelDesc *v = mForceFieldLinearKernels[i];
    v->store(stream);
  }

#endif
  stream.endHeader();
}

void NxSceneDesc::load(SchemaStream &stream,const char *parent)
{
  stream.setCurrentScene(this);
  if ( stream.beginHeader(SC_NxSceneDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mHasMaxBounds,"mHasMaxBounds",true);
    stream.load(mHasLimits,"mHasLimits",true);
    stream.load(mHasFilter,"mHasFilter",true);
    stream.load(mFilterBool,"mFilterBool",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"mFilterOp0",false);
    mFilterOp0 = (NxFilterOp) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"mFilterOp0",false);
     StringToEnum(enumName,mFilterOp0);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"mFilterOp1",false);
    mFilterOp1 = (NxFilterOp) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"mFilterOp1",false);
     StringToEnum(enumName,mFilterOp1);
  }
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"mFilterOp2",false);
    mFilterOp2 = (NxFilterOp) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"mFilterOp2",false);
     StringToEnum(enumName,mFilterOp2);
  }
  mGroupMask0.load(stream,"mGroupMask0");
  mGroupMask1.load(stream,"mGroupMask1");
    stream.load(gravity,"gravity",false);
    stream.load(maxTimestep,"maxTimestep",false);
    stream.load(maxIter,"maxIter",false);
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"timeStepMethod",false);
    timeStepMethod = (NxTimeStepMethod) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"timeStepMethod",false);
     StringToEnum(enumName,timeStepMethod);
  }
    stream.load(mMaxBounds,"mMaxBounds",false);
  mLimits.load(stream,"mLimits");
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"simType",false);
    simType = (NxSimulationType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"simType",false);
     StringToEnum(enumName,simType);
  }
    stream.load(groundPlane,"groundPlane",false);
    stream.load(boundsPlanes,"boundsPlanes",false);
  if ( stream.isBinary() )
  {
    NxU32 v;
    stream.load(v,"flags",false);
    flags = (NxSceneFlags) v;
  }
  else
  {
    if ( stream.beginHeader(SC_NxSceneFlags,"flags") )
    {
      flags = (NxSceneFlags) 0;
      for (int i=0; i<32; i++)
      {
        int shift = (1<<i);
        const char *str = EnumToString( (NxSceneFlags) shift);
        if ( !str) break;
        const char *dummy = strstr(str,"DUMMY");
        if ( dummy ) continue;
        NX_BOOL b = false;
        stream.load(b,str,false);
        if ( b )
        {
        	flags = (NxSceneFlags) (flags|shift);
        }
      }
      stream.endHeader();
    }
  }
    stream.load(internalThreadCount,"internalThreadCount",false);
    stream.load(backgroundThreadCount,"backgroundThreadCount",false);
    stream.load(threadMask,"threadMask",false);
    stream.load(backgroundThreadMask,"backgroundThreadMask",false);
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(simThreadStackSize,"simThreadStackSize",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"simThreadPriority",false);
    simThreadPriority = (NxThreadPriority) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"simThreadPriority",false);
     StringToEnum(enumName,simThreadPriority);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 271
    stream.load(simThreadMask,"simThreadMask",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
    stream.load(workerThreadStackSize,"workerThreadStackSize",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"workerThreadPriority",false);
    workerThreadPriority = (NxThreadPriority) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"workerThreadPriority",false);
     StringToEnum(enumName,workerThreadPriority);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"backgroundThreadPriority",false);
    backgroundThreadPriority = (NxThreadPriority) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"backgroundThreadPriority",false);
     StringToEnum(enumName,backgroundThreadPriority);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 262
    stream.load(upAxis,"upAxis",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 262
    stream.load(subdivisionLevel,"subdivisionLevel",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"staticStructure",false);
    staticStructure = (NxPruningStructure) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"staticStructure",false);
     StringToEnum(enumName,staticStructure);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"dynamicStructure",false);
    dynamicStructure = (NxPruningStructure) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"dynamicStructure",false);
     StringToEnum(enumName,dynamicStructure);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 281
    stream.load(dynamicTreeRebuildRateHint,"dynamicTreeRebuildRateHint",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  if ( stream.isBinary() )
  {
    NxU32 temp;
    stream.load(temp,"bpType",false);
    bpType = (NxBroadPhaseType) temp;
  }
  else
  {
     const char *enumName = 0;
     stream.load(enumName,"bpType",false);
     StringToEnum(enumName,bpType);
  }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(nbGridCellsX,"nbGridCellsX",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(nbGridCellsY,"nbGridCellsY",false);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
    stream.load(solverBatchSize,"solverBatchSize",false);
#endif
  bool isOld = stream.peekHeader(SC_NxScene);
  if ( !isOld || stream.beginHeader(SC_NxScene) )
  {
  while ( stream.peekHeader(SC_NxMaterialDesc) )
  {
     NxMaterialDesc *temp = new NxMaterialDesc;
     temp->load(stream);
     mMaterials.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxActorDesc) )
  {
     NxActorDesc *temp = new NxActorDesc;
     temp->load(stream);
     mActors.push_back(temp);
   }
    NxJointDesc *joint = 0;
    do
    {
      joint = 0;
			NxI32 v = -1;

      stream.peekHeader(SC_NxD6JointDesc,&v,
											  SC_NxCylindricalJointDesc,
												SC_NxDistanceJointDesc,
												SC_NxFixedJointDesc,
												SC_NxPointInPlaneJointDesc,
												SC_NxPointOnLineJointDesc,
                        SC_NxPrismaticJointDesc,
                        SC_NxRevoluteJointDesc,
                        SC_NxSphericalJointDesc,
                        SC_NxPulleyJointDesc);
      switch ( v )
      {
        case SC_NxD6JointDesc:
          if ( 1 )
          {
            NxD6JointDesc *p = new NxD6JointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxCylindricalJointDesc:
          if ( 1 )
          {
            NxCylindricalJointDesc *p = new NxCylindricalJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxDistanceJointDesc:
          if ( 1 )
          {
            NxDistanceJointDesc *p = new NxDistanceJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxFixedJointDesc:
          if ( 1 )
          {
            NxFixedJointDesc *p = new NxFixedJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxPointInPlaneJointDesc:
          if ( 1 )
          {
            NxPointInPlaneJointDesc *p = new NxPointInPlaneJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxPointOnLineJointDesc:
          if ( 1 )
          {
            NxPointOnLineJointDesc *p = new NxPointOnLineJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxPrismaticJointDesc:
          if ( 1 )
          {
            NxPrismaticJointDesc *p = new NxPrismaticJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxRevoluteJointDesc:
          if ( 1 )
          {
            NxRevoluteJointDesc *p = new NxRevoluteJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxSphericalJointDesc:
          if ( 1 )
          {
            NxSphericalJointDesc *p = new NxSphericalJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        case SC_NxPulleyJointDesc:
          if ( 1 )
          {
            NxPulleyJointDesc *p = new NxPulleyJointDesc;
            p->load(stream);
            joint = static_cast<NxJointDesc *>(p);
          }
          break;
        default:
          break;
     }
     if ( joint )
     {
       mJoints.push_back(joint);
     }
   } while ( joint );
  while ( stream.peekHeader(SC_NxPairFlagDesc) )
  {
     NxPairFlagDesc *temp = new NxPairFlagDesc;
     temp->load(stream);
     mPairFlags.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxSpringAndDamperEffectorDesc) )
  {
     NxSpringAndDamperEffectorDesc *temp = new NxSpringAndDamperEffectorDesc;
     temp->load(stream);
     mEffectors.push_back(temp);
   }
#if NX_SDK_VERSION_NUMBER >= 270
  while ( stream.peekHeader(SC_NxForceFieldDesc) )
  {
     NxForceFieldDesc *temp = new NxForceFieldDesc;
     temp->load(stream);
     mForceFields.push_back(temp);
   }
#endif
  while ( stream.peekHeader(SC_NxCollisionGroupDesc) )
  {
     NxCollisionGroupDesc *temp = new NxCollisionGroupDesc;
     temp->load(stream);
     mCollisionGroups.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxActorGroupPair) )
  {
     NxActorGroupPair *temp = new NxActorGroupPair;
     temp->load(stream);
     mActorGroups.push_back(temp);
   }
#if NX_SDK_VERSION_NUMBER >= 260
  while ( stream.peekHeader(SC_NxCompartmentDesc) )
  {
     NxCompartmentDesc *temp = new NxCompartmentDesc;
     temp->load(stream);
     mCompartments.push_back(temp);
   }
#endif
#if NX_USE_FLUID_API
  while ( stream.peekHeader(SC_NxFluidDesc) )
  {
     NxFluidDesc *temp = new NxFluidDesc;
     temp->load(stream);
     mFluids.push_back(temp);
   }
#endif
  while ( stream.peekHeader(SC_NxClothDesc) )
  {
     NxClothDesc *temp = new NxClothDesc;
     temp->load(stream);
     mCloths.push_back(temp);
   }
#if NX_USE_SOFTBODY_API
  while ( stream.peekHeader(SC_NxSoftBodyDesc) )
  {
     NxSoftBodyDesc *temp = new NxSoftBodyDesc;
     temp->load(stream);
     mSoftBodies.push_back(temp);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  while ( stream.peekHeader(SC_NxConstraintDominanceDesc) )
  {
     NxConstraintDominanceDesc *temp = new NxConstraintDominanceDesc;
     temp->load(stream);
     mDominanceGroupPairs.push_back(temp);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  while ( stream.peekHeader(SC_NxForceFieldScaleTableEntry) )
  {
     NxForceFieldScaleTableEntry *temp = new NxForceFieldScaleTableEntry;
     temp->load(stream);
     mForceFieldScaleTable.push_back(temp);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  while ( stream.peekHeader(SC_NxForceFieldShapeGroupDesc) )
  {
     NxForceFieldShapeGroupDesc *temp = new NxForceFieldShapeGroupDesc;
     temp->load(stream);
     mForceFieldShapeGroups.push_back(temp);
   }
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  while ( stream.peekHeader(SC_NxForceFieldLinearKernelDesc) )
  {
     NxForceFieldLinearKernelDesc *temp = new NxForceFieldLinearKernelDesc;
     temp->load(stream);
     mForceFieldLinearKernels.push_back(temp);
   }
#endif
  if ( isOld ) stream.endHeader();
  }
    stream.endHeader();
  }
}

void NxSceneDesc::copyFrom(const ::NxSceneDesc &desc,CustomCopy &cc)
{
  gravity = desc.gravity;
  maxTimestep = desc.maxTimestep;
  maxIter = desc.maxIter;
  timeStepMethod = (NxTimeStepMethod) desc.timeStepMethod;
  simType = (NxSimulationType) desc.simType;
  groundPlane = desc.groundPlane;
  boundsPlanes = desc.boundsPlanes;
  flags = (NxSceneFlags) desc.flags;
  internalThreadCount = desc.internalThreadCount;
  backgroundThreadCount = desc.backgroundThreadCount;
  threadMask = desc.threadMask;
  backgroundThreadMask = desc.backgroundThreadMask;
#if NX_SDK_VERSION_NUMBER >= 250
  simThreadStackSize = desc.simThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  simThreadPriority = (NxThreadPriority) desc.simThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  simThreadMask = desc.simThreadMask;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  workerThreadStackSize = desc.workerThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  workerThreadPriority = (NxThreadPriority) desc.workerThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  backgroundThreadPriority = (NxThreadPriority) desc.backgroundThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  upAxis = desc.upAxis;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  subdivisionLevel = desc.subdivisionLevel;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  staticStructure = (NxPruningStructure) desc.staticStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  dynamicStructure = (NxPruningStructure) desc.dynamicStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  dynamicTreeRebuildRateHint = desc.dynamicTreeRebuildRateHint;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  bpType = (NxBroadPhaseType) desc.bpType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  nbGridCellsX = desc.nbGridCellsX;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  nbGridCellsY = desc.nbGridCellsY;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  solverBatchSize = desc.solverBatchSize;
#endif
  cc.customCopyFrom(*this,desc);
}

void NxSceneDesc::copyTo(::NxSceneDesc &desc,CustomCopy &cc)
{
  desc.gravity = gravity;
  desc.maxTimestep = maxTimestep;
  desc.maxIter = maxIter;
  desc.timeStepMethod = (::NxTimeStepMethod) timeStepMethod;
  desc.simType = (::NxSimulationType) simType;
  desc.groundPlane = groundPlane;
  desc.boundsPlanes = boundsPlanes;
  desc.flags = (::NxSceneFlags) flags;
  desc.internalThreadCount = internalThreadCount;
  desc.backgroundThreadCount = backgroundThreadCount;
  desc.threadMask = threadMask;
  desc.backgroundThreadMask = backgroundThreadMask;
#if NX_SDK_VERSION_NUMBER >= 250
  desc.simThreadStackSize = simThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  desc.simThreadPriority = (::NxThreadPriority) simThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  desc.simThreadMask = simThreadMask;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.workerThreadStackSize = workerThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  desc.workerThreadPriority = (::NxThreadPriority) workerThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  desc.backgroundThreadPriority = (::NxThreadPriority) backgroundThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  desc.upAxis = upAxis;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  desc.subdivisionLevel = subdivisionLevel;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  desc.staticStructure = (::NxPruningStructure) staticStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  desc.dynamicStructure = (::NxPruningStructure) dynamicStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  desc.dynamicTreeRebuildRateHint = dynamicTreeRebuildRateHint;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.bpType = (::NxBroadPhaseType) bpType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.nbGridCellsX = nbGridCellsX;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.nbGridCellsY = nbGridCellsY;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  desc.solverBatchSize = solverBatchSize;
#endif
  cc.customCopyTo(desc,*this);
}


//***********************************************************************************
// Constructor for 'NxSceneInstanceDesc'
//***********************************************************************************
NxSceneInstanceDesc::NxSceneInstanceDesc(void)
{
  mId                                           = 0;
  mUserProperties                               = 0;
  mSceneName                                    = 0;
  mRootNode.id();
  mIgnorePlane                                  = false;
  mNumSceneInstances                            = 0;
  // NxArray< NxSceneInstanceDesc *>: mSceneInstances
  mInstance = 0;
 mParent = 0;
}

NxSceneInstanceDesc::~NxSceneInstanceDesc(void)
{
  for (NxU32 i=0; i<mSceneInstances.size(); i++)
  {
     NxSceneInstanceDesc  *v = mSceneInstances[i];
    delete v;
  }

}

void NxSceneInstanceDesc::store(SchemaStream &stream,const char *parent)
{
  NxSceneInstanceDesc def;
  stream.beginHeader(SC_NxSceneInstanceDesc);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mSceneName != mSceneName )
    stream.store(mSceneName,"mSceneName",true);
  if ( gSaveDefaults || !isSame(def.mRootNode,mRootNode) )
    stream.store(mRootNode,"mRootNode",false);
  if ( gSaveDefaults || def.mIgnorePlane != mIgnorePlane )
    stream.store(mIgnorePlane,"mIgnorePlane",true);
    mNumSceneInstances = mSceneInstances.size();
  if ( gSaveDefaults || def.mNumSceneInstances != mNumSceneInstances )
    stream.store(mNumSceneInstances,"mNumSceneInstances",true);
  for (NxU32 i=0; i<mSceneInstances.size(); i++)
  {
    NxSceneInstanceDesc *v = mSceneInstances[i];
    v->store(stream);
  }

  stream.endHeader();
}

void NxSceneInstanceDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxSceneInstanceDesc,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mSceneName,"mSceneName",true);
    stream.load(mRootNode,"mRootNode",false);
    stream.load(mIgnorePlane,"mIgnorePlane",true);
    stream.load(mNumSceneInstances,"mNumSceneInstances",true);
  for (NxU32 i=0; i<mNumSceneInstances; i++)
  {
     NxSceneInstanceDesc *temp = new NxSceneInstanceDesc;
     temp->load(stream);
     mSceneInstances.push_back(temp);
   }
    stream.endHeader();
  }
}


//***********************************************************************************
//***********************************************************************************

//***********************************************************************************
// Constructor for 'NxPhysicsSDKDesc'
//***********************************************************************************
NxPhysicsSDKDesc::NxPhysicsSDKDesc(void)
{
  ::NxPhysicsSDKDesc def;
  hwPageSize                                    = def.hwPageSize;
  hwPageMax                                     = def.hwPageMax;
  hwConvexMax                                   = def.hwConvexMax;
  mInstance = 0;
}

NxPhysicsSDKDesc::~NxPhysicsSDKDesc(void)
{
}

void NxPhysicsSDKDesc::store(SchemaStream &stream,const char *parent)
{
  NxPhysicsSDKDesc def;
  stream.beginHeader(SC_NxPhysicsSDKDesc);
      if ( parent && !stream.isBinary() )
        stream.store(getElement(parent),"mId",true);
  if ( gSaveDefaults || def.hwPageSize != hwPageSize )
    stream.store(hwPageSize,"hwPageSize",false);
  if ( gSaveDefaults || def.hwPageMax != hwPageMax )
    stream.store(hwPageMax,"hwPageMax",false);
  if ( gSaveDefaults || def.hwConvexMax != hwConvexMax )
    stream.store(hwConvexMax,"hwConvexMax",false);
  stream.endHeader();
}

void NxPhysicsSDKDesc::load(SchemaStream &stream,const char *parent)
{
  if ( stream.beginHeader(SC_NxPhysicsSDKDesc,parent) )
  {
    stream.load(hwPageSize,"hwPageSize",false);
    stream.load(hwPageMax,"hwPageMax",false);
    stream.load(hwConvexMax,"hwConvexMax",false);
    stream.endHeader();
  }
}

void NxPhysicsSDKDesc::copyFrom(const ::NxPhysicsSDKDesc &desc,CustomCopy &cc)
{
  hwPageSize = desc.hwPageSize;
  hwPageMax = desc.hwPageMax;
  hwConvexMax = desc.hwConvexMax;
}

void NxPhysicsSDKDesc::copyTo(::NxPhysicsSDKDesc &desc,CustomCopy &cc)
{
  desc.hwPageSize = hwPageSize;
  desc.hwPageMax = hwPageMax;
  desc.hwConvexMax = hwConvexMax;
}


//***********************************************************************************
// Constructor for 'NxuPhysicsCollection'
//***********************************************************************************
NxuPhysicsCollection::NxuPhysicsCollection(void)
{
  mId                                           = 0;
  mUserProperties                               = 0;
  mSdkVersion                                   = 0;
  mNxuVersion                                   = 0;
  // NxPhysicsSDKDesc: mSDK
  // NxArray< NxParameterDesc *>: mParameters
  // NxArray< NxConvexMeshDesc *>: mConvexMeshes
  // NxArray< NxTriangleMeshDesc *>: mTriangleMeshes
  // NxArray< NxHeightFieldDesc *>: mHeightFields
  // NxArray< NxCCDSkeletonDesc *>: mSkeletons
  // NxArray< NxClothMeshDesc *>: mClothMeshes
#if NX_USE_SOFTBODY_API
  // NxArray< NxSoftBodyMeshDesc *>: mSoftBodyMeshes
#endif
  // NxArray< NxSceneDesc *>: mScenes
  // NxArray< NxSceneInstanceDesc *>: mSceneInstances
  mInstance = 0;
  mCurrentScene = 0;
  mCurrentSceneInstance = 0;
  mReferenceCount = 0;
}

NxuPhysicsCollection::~NxuPhysicsCollection(void)
{
  for (NxU32 i=0; i<mParameters.size(); i++)
  {
     NxParameterDesc  *v = mParameters[i];
    delete v;
  }

  for (NxU32 i=0; i<mConvexMeshes.size(); i++)
  {
     NxConvexMeshDesc  *v = mConvexMeshes[i];
    delete v;
  }

  for (NxU32 i=0; i<mTriangleMeshes.size(); i++)
  {
     NxTriangleMeshDesc  *v = mTriangleMeshes[i];
    delete v;
  }

  for (NxU32 i=0; i<mHeightFields.size(); i++)
  {
     NxHeightFieldDesc  *v = mHeightFields[i];
    delete v;
  }

  for (NxU32 i=0; i<mSkeletons.size(); i++)
  {
     NxCCDSkeletonDesc  *v = mSkeletons[i];
    delete v;
  }

  for (NxU32 i=0; i<mClothMeshes.size(); i++)
  {
     NxClothMeshDesc  *v = mClothMeshes[i];
    delete v;
  }

#if NX_USE_SOFTBODY_API
  for (NxU32 i=0; i<mSoftBodyMeshes.size(); i++)
  {
     NxSoftBodyMeshDesc  *v = mSoftBodyMeshes[i];
    delete v;
  }

#endif
  for (NxU32 i=0; i<mScenes.size(); i++)
  {
     NxSceneDesc  *v = mScenes[i];
    delete v;
  }

  for (NxU32 i=0; i<mSceneInstances.size(); i++)
  {
     NxSceneInstanceDesc  *v = mSceneInstances[i];
    delete v;
  }

}

void NxuPhysicsCollection::store(SchemaStream &stream,const char *parent)
{
  NxuPhysicsCollection def;
  stream.beginHeader(SC_NxuPhysicsCollection);
  if ( gSaveDefaults || def.mId != mId )
    stream.store(mId,"mId",true);
  if ( gSaveDefaults || def.mUserProperties != mUserProperties )
    stream.store(mUserProperties,"mUserProperties",true);
  if ( gSaveDefaults || def.mSdkVersion != mSdkVersion )
    stream.store(mSdkVersion,"mSdkVersion",true);
  if ( gSaveDefaults || def.mNxuVersion != mNxuVersion )
    stream.store(mNxuVersion,"mNxuVersion",true);
  mSDK.store(stream,"mSDK");
  for (NxU32 i=0; i<mParameters.size(); i++)
  {
    NxParameterDesc *v = mParameters[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mConvexMeshes.size(); i++)
  {
    NxConvexMeshDesc *v = mConvexMeshes[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mTriangleMeshes.size(); i++)
  {
    NxTriangleMeshDesc *v = mTriangleMeshes[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mHeightFields.size(); i++)
  {
    NxHeightFieldDesc *v = mHeightFields[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mSkeletons.size(); i++)
  {
    NxCCDSkeletonDesc *v = mSkeletons[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mClothMeshes.size(); i++)
  {
    NxClothMeshDesc *v = mClothMeshes[i];
    v->store(stream);
  }

#if NX_USE_SOFTBODY_API
  for (NxU32 i=0; i<mSoftBodyMeshes.size(); i++)
  {
    NxSoftBodyMeshDesc *v = mSoftBodyMeshes[i];
    v->store(stream);
  }

#endif
  for (NxU32 i=0; i<mScenes.size(); i++)
  {
    NxSceneDesc *v = mScenes[i];
    v->store(stream);
  }

  for (NxU32 i=0; i<mSceneInstances.size(); i++)
  {
    NxSceneInstanceDesc *v = mSceneInstances[i];
    v->store(stream);
  }

  stream.endHeader();
}

void NxuPhysicsCollection::load(SchemaStream &stream,const char *parent)
{
  stream.setCurrentCollection(this);
  if ( stream.beginHeader(SC_NxuPhysicsCollection,parent) )
  {
    stream.load(mId,"mId",true);
    stream.load(mUserProperties,"mUserProperties",true);
    stream.load(mSdkVersion,"mSdkVersion",true);
    stream.load(mNxuVersion,"mNxuVersion",true);
  mSDK.load(stream,"mSDK");
  while ( stream.peekHeader(SC_NxParameterDesc) )
  {
     NxParameterDesc *temp = new NxParameterDesc;
     temp->load(stream);
     mParameters.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxConvexMeshDesc) )
  {
     NxConvexMeshDesc *temp = new NxConvexMeshDesc;
     temp->load(stream);
     mConvexMeshes.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxTriangleMeshDesc) )
  {
     NxTriangleMeshDesc *temp = new NxTriangleMeshDesc;
     temp->load(stream);
     mTriangleMeshes.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxHeightFieldDesc) )
  {
     NxHeightFieldDesc *temp = new NxHeightFieldDesc;
     temp->load(stream);
     mHeightFields.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxCCDSkeletonDesc) )
  {
     NxCCDSkeletonDesc *temp = new NxCCDSkeletonDesc;
     temp->load(stream);
     mSkeletons.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxClothMeshDesc) )
  {
     NxClothMeshDesc *temp = new NxClothMeshDesc;
     temp->load(stream);
     mClothMeshes.push_back(temp);
   }
#if NX_USE_SOFTBODY_API
  while ( stream.peekHeader(SC_NxSoftBodyMeshDesc) )
  {
     NxSoftBodyMeshDesc *temp = new NxSoftBodyMeshDesc;
     temp->load(stream);
     mSoftBodyMeshes.push_back(temp);
   }
#endif
  while ( stream.peekHeader(SC_NxSceneDesc) )
  {
     NxSceneDesc *temp = new NxSceneDesc;
     temp->load(stream);
     mScenes.push_back(temp);
   }
  while ( stream.peekHeader(SC_NxSceneInstanceDesc) )
  {
     NxSceneInstanceDesc *temp = new NxSceneInstanceDesc;
     temp->load(stream);
     mSceneInstances.push_back(temp);
   }
    stream.endHeader();
  }
}


//***********************************************************************************
//***********************************************************************************
}; // End namespace NXU
