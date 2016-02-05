
#ifndef _RIGIDBODYCREATION_H_
#define _RIGIDBODYCREATION_H_

/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "globals.h"

//#include "RigidBodyBox.h"
//#include "RigidBodyCapsule.h"
//#include "RigidBodyConvex.h"
//#include "RigidBodyMesh.h"
//#include "RigidBodyTerrain.h"
//#include "RigidBodySphere.h"
/*
DARKSDK void dbPhyMakeRigidBodyStaticBox		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticBoxes		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticBox		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticBoxes		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicBox		( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicBoxes		( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicBox		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicBoxes		( int iObject, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticSphere		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticSpheres	( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticSphere		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticSpheres	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicSphere	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicSpheres	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicSphere	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicSpheres	( int iObject, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticCapsule	( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticCapsules	( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticCapsule	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticCapsules	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicCapsule	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicCapsules	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicCapsule	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicCapsules	( int iObject, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticConvex		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticConvexes	( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticConvex		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticConvexes	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicConvex	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicConvexes	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicConvex	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );
DARKSDK void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );
*/

DARKSDK void dbPhyDeleteRigidBody				( int iObject );
DARKSDK int  dbPhyGetRigidBodyExist				( int iObject );
DARKSDK int  dbPhyGetRigidBodyType				( int iObject );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL RIGID BODY COMMANDS /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

float					dbPhyObjectGetSizeX					( sObject* pObject );
float					dbPhyObjectGetSizeY					( sObject* pObject );
float					dbPhyObjectGetSizeZ					( sObject* pObject );

void 					dbPhyMakeRigidBody					( int iObject, PhysX::eType eType, PhysX::eState eState, int iMaterial, int iSaveMode = 0, char* szFile = "" );
void 					dbPhyMakeRigidBodyBox				( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, bool bCCD = false );
void 					dbPhyMakeRigidBodySphere			( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial );
void 					dbPhyMakeRigidBodyCapsule			( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial );
void					dbPhyMakeRigidBodyMesh				( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile, PhysX::sPhysXObject* pPhysXObject );
void 					dbPhyMakeRigidBodyTerrain			( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile );
void 					dbPhyMakeRigidBodyConvex			( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile );
void					dbPhyMakeRigidBodyPMAP				( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile );

PhysX::sPhysXObject*	dbPhyGetObject						( int iID, bool bDisplayError = false );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#endif _RIGIDBODYCREATION_H_