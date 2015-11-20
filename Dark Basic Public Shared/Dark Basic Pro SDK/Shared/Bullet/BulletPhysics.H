
////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define BT_EULER_DEFAULT_ZYX

#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

// DX9PLUS Work
#include ".\..\data\cdatac.h"
#include ".\..\DBOFormat\DBOData.h"
#include ".\..\core\globstruct.h"
#include <stdio.h>
#include <vector>
#include <Shlobj.h>

#define DARKSDK	__declspec ( dllexport )
#define WIN32_LEAN_AND_MEAN

#ifndef SAFE_DELETE
# define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
# define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
# define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#endif

// Typedefs for Basic3D functions
typedef sObject*	( *pfnGetObject				)	( int );
typedef bool		( *pfnGetFVFOffsetMap		)	( sMesh*, sOffsetMap* );
typedef void		( *pfnTransformVertices		)	( sObject*, sMesh*, D3DXMATRIX );
typedef bool		( *pfnCalculateMeshBounds	)	( sMesh* );
typedef bool		( *pfnCalcObjectWorld		)	( sObject* );
typedef bool		( *pfnMakeMeshFromOtherMesh	)	( bool, sMesh*, sMesh*, D3DXMATRIX* );
typedef void		( *pfnPositionObject		)	( int, float, float, float );
typedef void		( *pfnRotateObject			)	( int, float, float, float );
typedef void		( *pfnMakeObjectBox			)	( int, float, float, float );
typedef void		( *pfnDeleteObject			)	( int );

// Initialisation commands
DARKSDK void		ODE_Start								( void );
DARKSDK void		ODE_Update								( void );
DARKSDK void		ODE_Update								( float fManualStep );
DARKSDK void		ODE_End									( void );

// Creation commands
DARKSDK void		ODE_CreateStaticSphere					( int iObjectNumber );
DARKSDK void		ODE_CreateStaticBox						( int iObjectNumber );
DARKSDK void		ODE_CreateStaticBox						( int iObjectNumber, int iLimbNumber );
DARKSDK void		ODE_CreateStaticCylinder				( int iObjectNumber );
DARKSDK void		ODE_CreateStaticCylinder				( int iObjectNumber, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot );
DARKSDK void	    ODE_CreateTreeCylinder                  ( int iObjectNumber );
DARKSDK void		ODE_CreateStaticTerrainMesh				( int iObjectNumber );
DARKSDK void		ODE_CreateStaticTriangleMesh			( int iObjectNumber );
DARKSDK void		ODE_CreateStaticTriangleMesh			( int iObjectNumber, int iLimbNumber );
DARKSDK void		ODE_CreateStaticTriangleMesh			( int iObjectNumber, int iLimbNumber, int iCollisionScaling );
DARKSDK void        ODE_CreateStaticTriangleMesh            ( int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode );
DARKSDK void		ODE_CreateStaticTerrain					( int iObjectNumber, int iWidth, int iLength, DWORD dwMemBlockPtr );
DARKSDK void		ODE_UpdateStaticTerrain					( int iObjectNumber, int iWidth, int iLength, DWORD dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 );
DARKSDK void		ODE_CreateStaticCapsule					( int iObjectNumber );
DARKSDK void		ODE_CreateDynamicSphere					( int iObjectNumber );
DARKSDK void		ODE_CreateDynamicBox					( int iObjectNumber );
DARKSDK void		ODE_CreateDynamicBox					( int iObjectNumber, int iLimbNumber, int iTipOverMode );
DARKSDK void		ODE_CreateDynamicBox					( int iObjectNumber, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution );
DARKSDK void		ODE_CreateDynamicCylinder				( int iObjectNumber );
DARKSDK void		ODE_CreateDynamicTriangleMesh			( int iObjectNumber );
DARKSDK void		ODE_CreateDynamicCapsule				( int iObjectNumber, float fScaleModifier, float fRaised );
DARKSDK void		ODE_StartStaticObject					( int iObjectNumber );
DARKSDK void		ODE_EndStaticObject						( int iObjectNumber, int iTerrainMaterial );
DARKSDK void		ODE_AddStaticObjectBox					( int iObjectNumber, int iStaticBoxObject );
DARKSDK void        ODE_CreateDynamicBoxManual              ( int iObjectNumber, float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize, float fXAngl, float fYAng, float fZAng);
DARKSDK int         ODE_CreateSwingDoorHinge                ( int iObjectNumber , float fX, float fY, float fZ, float fAng);

// Character Controller commands
DARKSDK void		ODE_CreateDynamicCharacterController	( int iObjectNumber, float fA, float fB, float fC );
DARKSDK void		ODE_SetDynamicCharacterController		( int iObjectNumber, float fA, float fA2, float fB, float fC, float fD, float fE, float fF );
DARKSDK void		ODE_ControlDynamicCharacterController	( int iObjectNumber, float fA, float fA2, float fB, float fC, float fDucking, float fPushAngle, float fPushForce, float fThrustUpwards );
DARKSDK DWORD		ODE_GetCharacterControllerDucking		( int iObjectNumber );
DARKSDK int			ODE_GetCharacterHitFloor				( void );
DARKSDK DWORD		ODE_GetCharacterFallDistance			( void );
DARKSDK void		ODE_SetWaterLine						( float fWaterLineY );

// Ragdoll commands
DARKSDK void		BPhys_RagDollBegin(int objectID, float objTotalWeight);
DARKSDK int			BPhys_RagDollGetIDFromBoneObject(int objectID);
DARKSDK int			BPhys_RagDollAddBone(int startLimbID, int endLimbID, float diameter, int collisionGroup, int collisionMask);
DARKSDK int			BPhys_RagDollAddBone2(int startLimbID, int endLimbID, float diameter, float lengthmod, int collisionGroup, int collisionMask);
DARKSDK void		BPhys_RagDollBoneAddLimbID(int boneID, int limbID) ;
DARKSDK void		BPhys_RagDollAddHingeJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec2);
DARKSDK void		BPhys_RagDollAddTwistJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec3);
DARKSDK int			BPhys_RagdollGetBoneObjID(int ragdollID, int boneIndex);
DARKSDK void		BPhys_RagDollEnd();
DARKSDK void		BPhys_RagDollApplyForce(int ragdollID,int iLimb,float fFX,float fFY,float fFZ,float fTX,float fTY,float fTZ,float fForce);
DARKSDK int			BPhys_RagdollExist(int ragdollID);
DARKSDK int			BPhys_DeleteRagdoll(int ragdollID);
DARKSDK int			BPhys_RagdollIsStatic(int ragdollID);
DARKSDK int			BPhys_RagdollSetStatic(int ragdollID, int isStatic);
DARKSDK int			BPhys_RagDollHideBones(int ragdollID);
DARKSDK int			BPhys_RagDollShowBones(int ragdollID);
DARKSDK void		BPhys_RagDollSetDamping(float linear, float angular);
DARKSDK void		BPhys_RagDollSetSleepingThresholds(float linear, float angular);
DARKSDK void		BPhys_RagDollSetDeactivationTime(float time);

// Destruction commands
DARKSDK void		ODE_DestroyObject						( int iObjectNumber );

// Force command
DARKSDK void		ODE_SetLinearVelocity					( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODE_AddBodyForce						( int iObjectNumber, float fX, float fY, float fZ, float fPX, float fPY, float fPZ );
DARKSDK void		ODE_SetAngularVelocity					( int iObjectNumber, float fX, float fY, float fZ );

// Ray commands
DARKSDK int			ODE_RayTerrainEx						( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType );
DARKSDK int			ODE_RayTerrain							( float fX, float fY, float fZ, float fToX, float fToY, float fToZ );
DARKSDK int			ODE_RayForce							( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, float fForceValue );
DARKSDK DWORD		ODE_GetRayCollisionX					( );
DARKSDK DWORD		ODE_GetRayCollisionY					( );
DARKSDK DWORD		ODE_GetRayCollisionZ					( );
DARKSDK DWORD		ODE_GetRayNormalX						( );
DARKSDK DWORD		ODE_GetRayNormalY						( );
DARKSDK DWORD		ODE_GetRayNormalZ						( );
DARKSDK int			ODE_GetRayObjectHit						( );

// Set commands
DARKSDK void		ODE_SetActive							( int iObjectNumber, int iMode );
DARKSDK void		ODE_SetBodyPosition						( int iID, float fX, float fY, float fZ );
DARKSDK void		ODE_SetNoGravity						( int iObjectNumber, int iGravity );

// properties
DARKSDK void		ODE_SetBodyMass							( int iObject, float fMass );
DARKSDK void		ODE_SetBodyFriction						( int iObject, float fFriction );

// world functions
DARKSDK void		ODE_SetWorldGravity						( float fX, float fY, float fZ );

// Get commands
DARKSDK DWORD		ODE_GetBodyLinearVelocityX				( int iObjectNumber );
DARKSDK DWORD		ODE_GetBodyLinearVelocityY				( int iObjectNumber );
DARKSDK DWORD		ODE_GetBodyLinearVelocityZ				( int iObjectNumber );

/* OLD ODE

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

struct sODEObject
{
	bool		bEnabled;
	dBodyID		body;
	dGeomID		geom;
	dGeomID		geom2;
	int			iID;
	int			iFrame;
	dMatrix3	rotation;

	bool		bRayCast;

	float		fXSize;
	float		fYSize;
	float		fZSize;
	float		fXAngle;
	float		fYAngle;
	float		fZAngle;

	bool		bNoNeedToRecalculateShape;
	int			iResponseMode;
	int			iItemCarryGrab;
	dMatrix3	wItemCarryGrab;
	float		fActualMarginX;
	float		fActualMarginY;
	float		fActualMarginZ;
	float		fActualHeight;

	int			iSurfaceMode;
	float		fContactMU;
	float		fContactMU2;
	float		fContactBounce;
	float		fContactBounceVelocity;
	float		fContactSoftERP;
	float		fContactSoftCFM;
	float		fContactMotion1;
	float		fContactMotion2;
	float		fContactSlip1;
	float		fContactSlip2;
};

struct sODECar
{
	float		fX;
	float		fY;
	float		fZ;

	float		fMass;
	float		fWheelRadius;

	dSpaceID	space;
	dJointID	joints [ 4 ];
	dGeomID		geom   [ 5 ];
	dBodyID		bodies [ 5 ];
	int			iID    [ 5 ];
};

struct sODEBody
{
	int		iID;
	dBodyID	dBodyID;
};



struct sODECollision
{
	int iObjectA;
	int iObjectB;

	float fObjectAContact;
	float fObjectAVelocityX;
	float fObjectAVelocityY;
	float fObjectAVelocityZ;
	float fObjectAAngularVelocityX;
	float fObjectAAngularVelocityY;
	float fObjectAAngularVelocityZ;

	float fObjectBContact;
	float fObjectBVelocityX;
	float fObjectBVelocityY;
	float fObjectBVelocityZ;
	float fObjectBAngularVelocityX;
	float fObjectBAngularVelocityY;
	float fObjectBAngularVelocityZ;
};

#ifdef DARKSDK_COMPILE
	extern dWorldID						g_ODEWorld;
	extern dSpaceID						g_ODESpace;
	extern dJointGroupID				g_ODEContactGroup;
	extern std::vector < sODEObject >	g_ODEObjectList;
	extern std::vector < sODEObject >	g_ODEStaticObjectList;
	extern sODECar						g_ODECarList [ 64 ];
#endif

// internal functions
static void			ODE_Callback							( void* data, dGeomID o1, dGeomID o2 );
void				ODE_AddObject							( dBodyID body, dGeomID geom, int iID, int iFrame );
void				ODE_AddObject							( sODEObject object, int iID, int iFrame );
sODEObject*			ODE_FindID								( int iID );
sODEBody*			ODE_FindBodyID							( int iID );
int					ODE_FindBodyID							( dBodyID body );
sODEObject*			ODE_FindObjPtr							( dBodyID body );

// general
DARKSDK void		ODE_Start								( void );
DARKSDK void		ODE_Update								( void );
DARKSDK void		ODE_Update								( float fManualStep );
DARKSDK void		ODE_End									( void );


// world functions
DARKSDK void		ODE_SetWorldERP							( float fValue );
DARKSDK void		ODE_SetWorldCFM							( float fValue );
DARKSDK void		ODE_SetWorldStep						( float fStep );
DARKSDK void		ODE_SetWorldQuickStepNumIterations		( int iValue );
DARKSDK void		ODE_SetStepMode							( int iMode );
DARKSDK void		ODE_SetRagdollMode						( int iOneIsOn );

DARKSDK float		ODE_GetWorldGravityX					( void );
DARKSDK float		ODE_GetWorldGravityY					( void );
DARKSDK float		ODE_GetWorldGravityZ					( void );
DARKSDK float		ODE_GetWorldERP							( void );
DARKSDK float		ODE_GetWorldCFM							( void );
DARKSDK float		ODE_GetWorldStep						( void );
DARKSDK int			ODE_GetWorldQuickStepNumIterations		( void );

// disable functions
DARKSDK void		ODE_SetAutoDisableFlag					( int iFlag );
DARKSDK void		ODE_SetAutoDisableLinearThreshold		( float fThreshold );
DARKSDK void		ODE_SetAutoDisableAngularThreshold		( float fThreshold );
DARKSDK void		ODE_SetAutoDisableSteps					( int iSteps );
DARKSDK void		ODE_SetAutoDisableTime					( float fTime );
DARKSDK int			ODE_GetAutoDisableFlag					( void );
DARKSDK float		ODE_GetAutoDisableLinearThreshold		( void );
DARKSDK float		ODE_GetAutoDisableAngularThreshold		( void );
DARKSDK int			ODE_GetAutoDisableSteps					( void );
DARKSDK float		ODE_GetAutoDisableTime					( void );

// contact parameters
DARKSDK void		ODE_SetWorldContactSurfaceLayer			( float fDepth );
DARKSDK float		ODE_GetWorldContactSurfaceLayer			( void );

// rigid body functions
DARKSDK void		ODE_BodyCreate							( int iID );
DARKSDK void		ODE_BodyDestroy							( int iID );
DARKSDK void		ODE_SetBodyPosition						( int iID, float fX, float fY, float fZ );
DARKSDK void		ODE_SetBodyRotation						( int iID, float fX, float fY, float fZ );
DARKSDK void		ODE_SetBodyLinearVelocity				( int iID, float fX, float fY, float fZ );
DARKSDK void		ODE_SetBodyAngularVelocity				( int iID, float fX, float fY, float fZ );
DARKSDK float		ODE_GetBodyPositionX					( int iID );
DARKSDK float		ODE_GetBodyPositionY					( int iID );
DARKSDK float		ODE_GetBodyPositionZ					( int iID );
DARKSDK float		ODE_GetBodyRotationX					( int iID );
DARKSDK float		ODE_GetBodyRotationY					( int iID );
DARKSDK float		ODE_GetBodyRotationZ					( int iID );
DARKSDK DWORD		ODE_GetBodyLinearVelocityX				( int iID );
DARKSDK DWORD		ODE_GetBodyLinearVelocityY				( int iID );
DARKSDK DWORD		ODE_GetBodyLinearVelocityZ				( int iID );
DARKSDK DWORD		ODE_GetBodyAngularVelocityX				( int iID );
DARKSDK DWORD		ODE_GetBodyAngularVelocityY				( int iID );
DARKSDK DWORD		ODE_GetBodyAngularVelocityZ				( int iID );
DARKSDK DWORD		ODE_GetBodyTravelDistanceX				( int iID );
DARKSDK DWORD		ODE_GetBodyTravelDistanceY				( int iID );
DARKSDK DWORD		ODE_GetBodyTravelDistanceZ				( int iID );

DARKSDK DWORD		ODE_GetObjectAContact					( void );
DARKSDK DWORD		ODE_GetObjectBContact					( void );
DARKSDK void		ODE_SetBodyResponse						( int iID, int iResponseMode );
DARKSDK DWORD		ODE_GetBodyHeight						( int iID );
DARKSDK DWORD		ODE_GetBodyAdjustmentX					( int iID );
DARKSDK DWORD		ODE_GetBodyAdjustmentY					( int iID );
DARKSDK DWORD		ODE_GetBodyAdjustmentZ					( int iID );

// ray cast functions
DARKSDK void		ODE_SetRayCast							( int iObject, int iMode );

// car functions
DARKSDK void		ODE_CreateCar							( int iCar );
DARKSDK void		ODE_SetCarPosition						( int iCar, float fX, float fY, float fZ );
DARKSDK void		ODE_SetCarMass							( int iCar, float fMass );
DARKSDK void		ODE_SetCarMesh							( int iCar, int iObject, int iMode );
DARKSDK void		ODE_SetCarWheel							( int iCar, int iWheel, int iObject, float fRadius );
DARKSDK void		ODE_SetCarWheelMass						( int iCar, int iWheel, float fMass );
DARKSDK void		ODE_SetCarWheelRotation					( int iCar, int iWheel, float fX, float fY, float fZ );
DARKSDK void		ODE_SetCarWheelPosition					( int iCar, int iWheel, float fX, float fY, float fZ );
DARKSDK void		ODE_SetCarDefaultJoints					( int iCar );
DARKSDK void		ODE_UpdateCar							( int iCar );

DARKSDK void		ODE_CreateStaticBox						( int iObject, float fXSize, float fYSize, float fZSize );

// creation and destruction
DARKSDK void		ODE_CreateStaticUniverse				( void );
DARKSDK void		ODE_DestroyObject						( int iObject );

// body properties
DARKSDK void		ODE_SetLinearVelocity					( int iObject, float fX, float fY, float fZ );
DARKSDK void		ODE_SetAngularVelocity					( int iObject, float fX, float fY, float fZ );
DARKSDK void		ODE_SetGravity							( int iObject, int iMode );
DARKSDK void		ODE_SetActive							( int iObject, int iMode );
DARKSDK void		ODE_SetEnabled							( int iObject, int iEnabled );
DARKSDK void		ODE_SetBodyRotation						( int iObject, float fX, float fY, float fZ );

// surface properties
DARKSDK void		ODE_SetSurfaceMode						( int iID, int iMode, int iState );
DARKSDK void		ODE_SetSurfaceModeContactMu2			( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactFDir1			( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactBounce			( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactSoftERP		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactSoftCFM		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactMotion1		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactMotion2		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactSlip1			( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactSlip2			( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactApprox0		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactApprox11		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactApprox12		( int iID, int iState );
DARKSDK void		ODE_SetSurfaceModeContactApprox1		( int iID, int iState );

DARKSDK void		ODE_SetContact							( int iID, int iMode, float fValue );
DARKSDK void		ODE_SetContactMu2						( int iID, float fValue );
DARKSDK void		ODE_SetContactFDir1						( int iID, float fValue );
DARKSDK void		ODE_SetContactBounce					( int iID, float fValue );
DARKSDK void		ODE_SetContactBounceVelocity			( int iID, float fValue );
DARKSDK void		ODE_SetContactSoftERP					( int iID, float fValue );
DARKSDK void		ODE_SetContactSoftCFM					( int iID, float fValue );
DARKSDK void		ODE_SetContactMotion1					( int iID, float fValue );
DARKSDK void		ODE_SetContactMotion2					( int iID, float fValue );
DARKSDK void		ODE_SetContactSlip1						( int iID, float fValue );
DARKSDK void		ODE_SetContactSlip2						( int iID, float fValue );

DARKSDK int			ODE_CollisionMessageExists				( void );
DARKSDK void		ODE_CollisionGetMessage					( void );
DARKSDK int			ODE_GetObjectA							( void );
DARKSDK int			ODE_GetObjectB							( void );
DARKSDK DWORD		ODE_GetObjectAVelocityX					( void );
DARKSDK DWORD		ODE_GetObjectAVelocityY					( void );
DARKSDK DWORD		ODE_GetObjectAVelocityZ					( void );
DARKSDK DWORD		ODE_GetObjectAAngularVelocityX			( void );
DARKSDK DWORD		ODE_GetObjectAAngularVelocityY			( void );
DARKSDK DWORD		ODE_GetObjectAAngularVelocityZ			( void );
DARKSDK DWORD		ODE_GetObjectBVelocityX					( void );
DARKSDK DWORD		ODE_GetObjectBVelocityY					( void );
DARKSDK DWORD		ODE_GetObjectBVelocityZ					( void );
DARKSDK DWORD		ODE_GetObjectBAngularVelocityX			( void );
DARKSDK DWORD		ODE_GetObjectBAngularVelocityY			( void );
DARKSDK DWORD		ODE_GetObjectBAngularVelocityZ			( void );

DARKSDK void ODE_CreateHingeJoint ( int id, int iObj1, int iObj2, float axisX, float axisY, float axisZ, float posX, float posY, float posZ );
DARKSDK void ODE_CreateBallJoint ( int id, int iObj1, int iObj2, float posX, float posY, float posZ );
DARKSDK void ODE_SetHingeJointLimit ( int id, float low, float high );
DARKSDK void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale );
DARKSDK void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale, int iDebugObject );
DARKSDK void ODE_DestroyRagdoll( int iRagdollID );

*/
