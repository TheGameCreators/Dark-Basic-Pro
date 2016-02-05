
#include "globals.h"
#include "rigidbodyproperties.h"
#include "rigidbodycreation.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY SET RIGID BODY POSITION%LFFF%?dbPhySetRigidBodyPosition@@YAXHMMM@Z%
	PHY SET RIGID BODY ROTATION%LFFF%?dbPhySetRigidBodyRotation@@YAXHMMM@Z%

	PHY SET RIGID BODY GRAVITY%LL%?dbPhySetRigidBodyGravity@@YAXHH@Z%

	PHY SET RIGID BODY GROUP%LL%?dbPhySetRigidBodyGroup@@YAXHH@Z%

	PHY SET RIGID BODY MASS%LF%?dbPhySetRigidBodyMass@@YAXHM@Z%
	PHY GET RIGID BODY MASS[%FL%?dbPhyGetRigidBodyMassEx@@YAKH@Z%
	PHY SET RIGID BODY MASS OFFSET LOCAL%LFFF%?dbPhySetRigidBodyMassOffsetLocal@@YAXHMMM@Z%
	PHY SET RIGID BODY MASS OFFSET GLOBAL%LFFF%?dbPhySetRigidBodyMassOffsetGlobal@@YAXHMMM@Z%
	PHY UPDATE RIGID BODY MASS%LFF%?dbPhyUpdateRigidBodyMass@@YAXHMM@Z%

	PHY SET RIGID BODY LINEAR DAMPING%LF%?dbPhySetRigidBodyLinearDamping@@YAXHM@Z%
	PHY SET RIGID BODY ANGULAR DAMPING%LF%?dbPhySetRigidBodyAngularDamping@@YAXHM@Z%
	PHY GET RIGID BODY LINEAR DAMPING[%FL%?dbPhyGetRigidBodyLinearDampingEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR DAMPING[%FL%?dbPhyGetRigidBodyAngularDampingEx@@YAKH@Z%

	PHY SET RIGID BODY LINEAR VELOCITY%LFFF%?dbPhySetRigidBodyLinearVelocity@@YAXHMMM@Z%
	PHY SET RIGID BODY ANGULAR VELOCITY%LFFF%?dbPhySetRigidBodyAngularVelocity@@YAXHMMM@Z%
	PHY SET RIGID BODY MAX ANGULAR VELOCITY%LF%?dbPhySetRigidBodyMaxAngularVelocity@@YAXHM@Z%
	PHY GET RIGID BODY LINEAR VELOCITY X[%FL%?dbPhyGetRigidBodyLinearVelocityXEx@@YAKH@Z%
	PHY GET RIGID BODY LINEAR VELOCITY Y[%FL%?dbPhyGetRigidBodyLinearVelocityYEx@@YAKH@Z%
	PHY GET RIGID BODY LINEAR VELOCITY Z[%FL%?dbPhyGetRigidBodyLinearVelocityZEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR VELOCITY X[%FL%?dbPhyGetRigidBodyAngularVelocityXEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR VELOCITY Y[%FL%?dbPhyGetRigidBodyAngularVelocityYEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR VELOCITY Z[%FL%?dbPhyGetRigidBodyAngularVelocityZEx@@YAKH@Z%
	PHY GET RIGID BODY MAX ANGULAR VELOCITY[%FL%?dbPhyGetRigidBodyMaxAngularVelocityEx@@YAKH@Z%
	
	PHY SET RIGID BODY CCD MOTION THRESHOLD%LF%?dbPhySetRigidBodyCCDMotionThreshold@@YAXHM@Z%
	PHY GET RIGID BODY CCD MOTION THRESHOLD[%FL%?dbPhyGetRigidBodyCCDMotionThresholdEx@@YAKH@Z%

	PHY SET RIGID BODY CCD%LL%?dbPhySetRigidBodyCCD@@YAXHH@Z%
	
	PHY SET RIGID BODY LINEAR MOMENTUM%LFFF%?dbPhySetRigidBodyLinearMomentum@@YAXHMMM@Z%
	PHY SET RIGID BODY ANGULAR MOMENTUM%LFFF%?dbPhySetRigidBodyAngularMomentum@@YAXHMMM@Z%
	PHY GET RIGID BODY LINEAR MOMENTUM X[%FL%?dbPhyGetRigidBodyLinearMomentumXEx@@YAKH@Z%
	PHY GET RIGID BODY LINEAR MOMENTUM Y[%FL%?dbPhyGetRigidBodyLinearMomentumYEx@@YAKH@Z%
	PHY GET RIGID BODY LINEAR MOMENTUM Z[%FL%?dbPhyGetRigidBodyLinearMomentumZEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR MOMENTUM X[%FL%?dbPhyGetRigidBodyAngularMomentumXEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR MOMENTUM Y[%FL%?dbPhyGetRigidBodyAngularMomentumYEx@@YAKH@Z%
	PHY GET RIGID BODY ANGULAR MOMENTUM Z[%FL%?dbPhyGetRigidBodyAngularMomentumZEx@@YAKH@Z%
	
	PHY ADD RIGID BODY FORCE%LFFFFFFL%?dbPhyAddRigidBodyForce@@YAXHMMMMMMH@Z%
	PHY ADD RIGID BODY FORCE AT LOCAL%LFFFFFFL%?dbPhyAddRigidBodyForceAtLocal@@YAXHMMMMMMH@Z%
	PHY ADD RIGID BODY LOCAL FORCE%LFFFFFFL%?dbPhyAddRigidBodyLocalForce@@YAXHMMMMMMH@Z%
	PHY ADD RIGID BODY LOCAL FORCE AT LOCAL%LFFFFFFL%?dbPhyAddRigidBodyLocalForceAtLocal@@YAXHMMMMMMH@Z%
	PHY ADD RIGID BODY FORCE%LFFFL%?dbPhyAddRigidBodyForce@@YAXHMMMH@Z%
	PHY ADD RIGID BODY LOCAL FORCE%LFFFL%?dbPhyAddRigidBodyLocalForce@@YAXHMMMH@Z%
	PHY ADD RIGID BODY TORQUE%LFFFL%?dbPhyAddRigidBodyTorque@@YAXHMMMH@Z%
	PHY ADD RIGID BODY LOCAL TORQUE%LFFFL%?dbPhyAddRigidBodyLocalTorque@@YAXHMMMH@Z%

	PHY GET RIGID BODY DYNAMIC[%LL%?dbPhyGetRigidBodyDynamic@@YAHH@Z%

	

	DARKSDK void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyForceAtLocal				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyLocalForceAtLocal		( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyTorque					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
	DARKSDK void  dbPhyAddRigidBodyLocalTorque				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );

	void dbPhyAddRigidBodyLocalForce ( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode )

	PHY GET RIGID BODY POINT VELOCITY X[%FLFFF%?dbPhyGetRigidBodyPointVelocityXEx@@YAKHMMM@Z%
	PHY GET RIGID BODY POINT VELOCITY Y[%FLFFF%?dbPhyGetRigidBodyPointVelocityYEx@@YAKHMMM@Z%
	PHY GET RIGID BODY POINT VELOCITY Z[%FLFFF%?dbPhyGetRigidBodyPointVelocityZEx@@YAKHMMM@Z%
	PHY GET RIGID BODY LOCAL POINT VELOCITY X[%FLFFF%?dbPhyGetRigidBodyLocalPointVelocityXEx@@YAKHMMM@Z%
	PHY GET RIGID BODY LOCAL POINT VELOCITY Y[%FLFFF%?dbPhyGetRigidBodyLocalPointVelocityYEx@@YAKHMMM@Z%
	PHY GET RIGID BODY LOCAL POINT VELOCITY Z[%FLFFF%?dbPhyGetRigidBodyLocalPointVelocityZEx@@YAKHMMM@Z%

	PHY SET RIGID BODY SLEEP LINEAR VELOCITY%LF%?dbPhySetRigidBodySleepLinearVelocity@@YAXHM@Z%
	PHY SET RIGID BODY SLEEP ANGULAR VELOCITY%LF%?dbPhySetRigidBodySleepAngularVelocity@@YAXHM@Z%
	PHY WAKE UP RIGID BODY%L%?dbPhyWakeUpRigidBody@@YAXHM@Z%
	PHY SLEEP RIGID BODY%L%?dbPhySleepRigidBody@@YAXH@Z%
	PHY GET RIGID BODY SLEEPING[%LL%?dbPhyGetRigidBodySleeping@@YAHH@Z%
	PHY GET RIGID BODY SLEEP LINEAR VELOCITY[%FL%?dbPhyGetRigidBodySleepLinearVelocityEx@@YAKH@Z%
	PHY GET RIGID BODY SLEEP ANGULAR VELOCITY[%FL%?dbPhyGetRigidBodySleepAngularVelocityEx@@YAKH@Z%

	PHY SET RIGID BODY KINEMATIC%LL%?dbPhySetRigidBodyKinematic@@YAXHH@Z%
	PHY SET RIGID BODY KINEMATIC POSITION%LFFF%?dbPhySetRigidBodyKinematicPosition@@YAXHMMM@Z%
	PHY SET RIGID BODY KINEMATIC ROTATION%LFFF%?dbPhySetRigidBodyKinematicRotation@@YAXHMMM@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

NxQuat AnglesToQuat(const NxVec3& angles);

void dbPhySetRigidBodyPosition ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setGlobalPosition ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRigidBodyRotation ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		NxMat33 rotX;
		NxMat33 rotY;
		NxMat33 rotZ;
		NxMat33 rotation;

		rotX.rotX ( D3DXToRadian ( pObject->pObject->position.vecRotate.x ) );
		rotY.rotY ( D3DXToRadian ( pObject->pObject->position.vecRotate.y ) );
		rotZ.rotZ ( D3DXToRadian ( pObject->pObject->position.vecRotate.z ) );
		
		rotation = rotX * rotY * rotZ;

		pObject->pActorList [ 0 ]->setGlobalOrientation ( AnglesToQuat(NxVec3( fX,fY ,fZ)) );

		pObject->pActorList [ 0 ]->setAngularVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
		pObject->pActorList [ 0 ]->setLinearVelocity  ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	}
}

void dbPhySetRigidBodyGravity ( int iObject, int iState )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iState == 0 )
			pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );
		else
			pObject->pActorList [ 0 ]->clearBodyFlag ( NX_BF_DISABLE_GRAVITY );
	}
}

void dbPhySetRigidBodyGroup ( int iObject, int iGroup )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		NxU32 nbShapes = pObject->pActorList [ 0 ]->getNbShapes();
		NxShape*const* shapes = pObject->pActorList [ 0 ]->getShapes();

		while (nbShapes--)
		{
			shapes[nbShapes]->setGroup ( iGroup );
		}

		//pObject->pActorList [ 0 ]->setGroup ( iGroup );
	}
}

void dbPhySetRigidBodyMass ( int iObject, float fMass )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setMass ( fMass );
	}
}

DWORD dbPhyGetRigidBodyMassEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyMass ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyMass ( int iObject )
{
	NxReal fMass = 0.0f;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		fMass = pObject->pActorList [ 0 ]->getMass ( );
	
	return fMass;
}

void dbPhySetRigidBodyMassOffsetLocal ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setCMassOffsetLocalPosition ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRigidBodyMassOffsetGlobal ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setCMassOffsetGlobalPosition ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhyUpdateRigidBodyMass ( int iObject, float fDensity, float fTotalMass )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->updateMassFromShapes ( fDensity, fTotalMass );
	}
}

void dbPhySetRigidBodyLinearDamping ( int iObject, float fDamping )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setLinearDamping ( fDamping );
	}
}

void dbPhySetRigidBodyAngularDamping ( int iObject, float fAngular )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setAngularDamping ( fAngular );
	}
}

DWORD dbPhyGetRigidBodyLinearDampingEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearDamping ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularDampingEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularDamping ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyLinearDamping ( int iObject )
{
	NxReal fValue = 0.0f;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		fValue = pObject->pActorList [ 0 ]->getLinearDamping ( );
	
	return fValue;
}

float dbPhyGetRigidBodyAngularDamping ( int iObject )
{
	NxReal fValue = 0.0f;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		fValue = pObject->pActorList [ 0 ]->getAngularDamping ( );
	
	return fValue;
}

void dbPhySetRigidBodyLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setLinearVelocity ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRigidBodyAngularVelocity ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setAngularVelocity ( NxVec3 ( fX, fY, fZ ) );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Rigid body does not exist in phy set rigid body angular velocity", "", 0 );
	}
}

void dbPhySetRigidBodyMaxAngularVelocity ( int iObject, float fMax )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setMaxAngularVelocity ( fMax );
	}
}

DWORD dbPhyGetRigidBodyLinearVelocityXEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearVelocityX ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLinearVelocityYEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearVelocityY ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLinearVelocityZEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearVelocityZ ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularVelocityXEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularVelocityX ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularVelocityYEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularVelocityY ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularVelocityZEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularVelocityZ ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyMaxAngularVelocityEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyMaxAngularVelocity ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyLinearVelocityX ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getLinearVelocity ( );
	
	return vec.x;
}

float dbPhyGetRigidBodyLinearVelocityY ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getLinearVelocity ( );
	
	return vec.y;
}

float dbPhyGetRigidBodyLinearVelocityZ ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getLinearVelocity ( );
	
	return vec.z;
}

float dbPhyGetRigidBodyAngularVelocityX ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getAngularVelocity ( );
	
	return vec.x;
}

float dbPhyGetRigidBodyAngularVelocityY ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getAngularVelocity ( );
	
	return vec.y;
}

float dbPhyGetRigidBodyAngularVelocityZ ( int iObject )
{
	NxVec3 vec ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		vec = pObject->pActorList [ 0 ]->getAngularVelocity ( );
	
	return vec.z;
}

float dbPhyGetRigidBodyMaxAngularVelocity ( int iObject )
{
	NxReal value = 0.0f;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getMaxAngularVelocity ( );
	
	return value;
}

void dbPhySetRigidBodyCCDMotionThreshold ( int iObject, float fThreshold )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setCCDMotionThreshold ( fThreshold );
	}
}

DWORD dbPhyGetRigidBodyCCDMotionThresholdEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyCCDMotionThreshold ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyCCDMotionThreshold ( int iObject )
{
	NxReal value = 0.0f;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getCCDMotionThreshold ( );
	
	return value;
}

bool SetupStandardVertexEx ( sMesh* pMesh, DWORD dwFVF, BYTE* pVertex, int iOffset, float x, float y, float z, float nx, float ny, float nz, DWORD dwDiffuseColour, float tu, float tv )
{
	// setup a standard lit vertex

	// check the memory pointer is valid
	SAFE_MEMORY ( pVertex );

	// get the offset map
	sOffsetMap	offsetMap;
	PhysX::GetFVFOffsetMap ( pMesh, &offsetMap );

	// we can only work with any valid formats
	if ( dwFVF & D3DFVF_XYZ )
	{
		*( ( float* ) pVertex + offsetMap.dwX       + ( offsetMap.dwSize * iOffset ) ) = x;
		*( ( float* ) pVertex + offsetMap.dwY       + ( offsetMap.dwSize * iOffset ) ) = y;
		*( ( float* ) pVertex + offsetMap.dwZ       + ( offsetMap.dwSize * iOffset ) ) = z;
	}
	if ( dwFVF & D3DFVF_NORMAL )
	{
		*( ( float* ) pVertex + offsetMap.dwNX      + ( offsetMap.dwSize * iOffset ) ) = nx;
		*( ( float* ) pVertex + offsetMap.dwNY      + ( offsetMap.dwSize * iOffset ) ) = ny;
		*( ( float* ) pVertex + offsetMap.dwNZ      + ( offsetMap.dwSize * iOffset ) ) = nz;
	}
	if ( dwFVF & D3DFVF_DIFFUSE )
	{
		*( ( DWORD* ) pVertex + offsetMap.dwDiffuse + ( offsetMap.dwSize * iOffset ) ) = dwDiffuseColour;
	}
	if ( dwFVF & D3DFVF_TEX1 )
	{
		*( ( float* ) pVertex + offsetMap.dwTU[0]      + ( offsetMap.dwSize * iOffset ) ) = tu;
		*( ( float* ) pVertex + offsetMap.dwTV[0]      + ( offsetMap.dwSize * iOffset ) ) = tv;
	}

	// mike - 160903 - point size support, set to 1.0f by default
	if ( dwFVF & D3DFVF_PSIZE )
	{
		*( ( float* ) pVertex + offsetMap.dwPointSize + ( offsetMap.dwSize * iOffset ) ) = 1.0f;
	}

	// okay
	return true;
}

float g_fSphere [ ] =
{

	0.000000,-1.000000,0.000000,
	0.000000,1.000000,0.000000,
	-0.382683,-0.923880,0.000000,
	-0.707107,-0.707107,0.000000,
	-0.923880,-0.382683,0.000000,
	-1.000000,0.000000,0.000000,
	-0.923880,0.382683,0.000000,
	-0.707107,0.707107,0.000000,
	-0.382683,0.923880,0.000000,
	-0.270598,-0.923880,0.270598,
	-0.500000,-0.707107,0.500000,
	-0.653282,-0.382683,0.653282,
	-0.707107,0.000000,0.707107,
	-0.653282,0.382683,0.653282,
	-0.500000,0.707107,0.500000,
	-0.270598,0.923880,0.270598,
	0.000000,-0.923880,0.382683,
	0.000000,-0.707107,0.707107,
	0.000000,-0.382683,0.923880,
	0.000000,0.000000,1.000000,
	0.000000,0.382683,0.923880,
	0.000000,0.707107,0.707107,
	0.000000,0.923880,0.382683,
	0.270598,-0.923880,0.270598,
	0.500000,-0.707107,0.500000,
	0.653282,-0.382683,0.653282,
	0.707107,0.000000,0.707107,
	0.653282,0.382683,0.653282,
	0.500000,0.707107,0.500000,
	0.270598,0.923880,0.270598,
	0.382683,-0.923880,0.000000,
	0.707107,-0.707107,0.000000,
	0.923880,-0.382683,0.000000,
	1.000000,0.000000,0.000000,
	0.923880,0.382683,0.000000,
	0.707107,0.707107,0.000000,
	0.382683,0.923880,0.000000,
	0.270598,-0.923880,-0.270598,
	0.500000,-0.707107,-0.500000,
	0.653282,-0.382683,-0.653282,
	0.707107,0.000000,-0.707107,
	0.653282,0.382683,-0.653282,
	0.500000,0.707107,-0.500000,
	0.270598,0.923880,-0.270598,
	0.000000,-0.923880,-0.382683,
	0.000000,-0.707107,-0.707107,
	0.000000,-0.382683,-0.923880,
	0.000000,0.000000,-1.000000,
	0.000000,0.382683,-0.923880,
	0.000000,0.707107,-0.707107,
	0.000000,0.923880,-0.382683,
	-0.270598,-0.923880,-0.270598,
	-0.500000,-0.707107,-0.500000,
	-0.653282,-0.382683,-0.653282,
	-0.707107,0.000000,-0.707107,
	-0.653282,0.382683,-0.653282,
	-0.500000,0.707107,-0.500000,
	-0.270598,0.923880,-0.270598
};

NxU32 g_iSphereTriangles [ ] =
{
   9,2,0,
   2,9,10,
   2,10,3,
   3,10,11,
   3,11,4,
   4,11,12,
   4,12,5,
   5,12,1,
   5,13,6,
   6,13,14,
   6,14,7,
   7,14,15,
   7,15,8,
   8,15,1,
   16,9,0,
   9,16,17,
   9,17,10,
   10,17,18,
   10,18,11,
   11,18,19,
   11,19,12,
   12,19,20,
   12,20,1,
   13,20,21,
   13,21,14,
   14,21,22,
   14,22,15,
   15,22,1,
   23,16,0,
   16,23,24,
   16,24,17,
   17,24,25,
   17,25,18,
   18,25,26,
   18,26,19,
   19,26,27,
   19,27,20,
   20,27,28,
   20,28,21,
   21,28,29,
   21,29,22,
   22,29,1,
   30,23,0,
   23,30,31,
   23,31,24,
   24,31,32,
   24,32,25,
   25,32,3,
   25,33,26,
   26,33,34,
   26,34,27,
   27,34,35,
   27,35,28,
   28,35,36,
   28,36,29,
   29,36,1,
   37,30,0,
   30,37,38,
   30,38,31,
   31,38,39,
   31,39,32,
   32,39,40,
   32,40,3,
   33,40,41,
   33,41,34,
   34,41,42,
   34,42,35,
   35,42,4,
   35,43,36,
   36,43,1,
   44,37,0,
   37,44,45,
   37,45,38,
   38,45,46,
   38,46,39,
   39,46,47,
   39,47,40,
   40,47,48,
   40,48,41,
   41,48,49,
   41,49,42,
   42,49,50,
   42,50,4,
   43,50,1,
   51,44,0,
   44,51,52,
   44,52,45,
   45,52,5,
   45,53,46,
   46,53,54,
   46,54,47,
   47,54,55,
   47,55,48,
   48,55,56,
   48,56,49,
   49,56,57,
   49,57,50,
   50,57,1,
   2,51,0,
   51,2,3,
   51,3,52,
   52,3,4,
   52,4,5,
   53,4,5,
   53,5,54,
   54,5,6,
   54,6,55,
   55,6,7,
   55,7,56,
   56,7,8,
   56,8,57,
   57,8,1
};

 static void CreateMeshFromShape(NxSimpleTriangleMesh &triMesh,NxShape *shape)
{
    NxBoxShape *boxShape=shape->isBox();

	NxSphereShape *sphereShape=shape->isSphere();

    if(boxShape!=NULL)
    {
        NxBox obb=NxBox(NxVec3(0.0f,0.0f,0.0f),boxShape->getDimensions(),NxMat33(NX_IDENTITY_MATRIX));
        triMesh.points=new NxVec3[8];
        triMesh.numVertices=8;
        triMesh.pointStrideBytes=sizeof(NxVec3);
        triMesh.numTriangles=2*6;
        triMesh.triangles=new NxU32[2*6*3];
        triMesh.triangleStrideBytes=sizeof(NxU32)*3;
        triMesh.flags=0;
        NxComputeBoxPoints(obb,(NxVec3 *)triMesh.points);
        memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);
    }
	else if ( sphereShape != NULL )
	{
	
		NxVec3* vec = new NxVec3 [ 58 ];
		//NxU32* ui = new NxU32 [ 112 * 3 ];

		for ( int i = 0; i < 58; i++ )
		{
			vec [ i ].x = g_fSphere [ 3 * i + 0 ];
			vec [ i ].y = g_fSphere [ 3 * i + 1 ];
			vec [ i ].z = g_fSphere [ 3 * i + 2 ];
		}

		//for ( int i = 0; i < 112 * 3; i++ )
		{
			//ui [ i ] = g_iSphereTriangles [ 3 * i + 0 ];
		}

		//triMesh.points=new NxVec3[58];
		triMesh.points=vec;
        triMesh.numVertices=58;
		triMesh.pointStrideBytes=sizeof(NxVec3);
        triMesh.numTriangles=112;
        //triMesh.triangles=new NxU32[112*3];
		triMesh.triangles=g_iSphereTriangles;
        triMesh.triangleStrideBytes=sizeof(NxU32)*3;
        triMesh.flags=0;

		
	}

#ifdef ANCIENT_CODE
    else if ( sphereShape != NULL )
	{
		//NxSphere obb=NxSphere(NxVec3(0.0f,0.0f,0.0f),sphereShape->getDimensions(),NxMat33(NX_IDENTITY_MATRIX));
		

		int		iRings			= 4;
		int		iSegments		= 4;

		DWORD	dwIndexCount	= 2 * iRings * ( iSegments + 1 );
		DWORD	dwVertexCount	= ( iRings + 1 ) * ( iSegments + 1 );

		NxU32* pIndices = new NxU32 [ dwIndexCount ];
		NxVec3* pVertices = new NxVec3 [ dwVertexCount ];
		D3DXVECTOR3 vecCentre = D3DXVECTOR3 ( 0, 0, 0 );

		// now we can fill in the vertex and index data to form the mesh
		float		fDeltaRingAngle		= ( D3DX_PI / iRings );
		float		fDeltaSegAngle		= ( 2.0f * D3DX_PI / iSegments );
		int			iVertex				= 0;
		int			iIndex				= 0;
		WORD		wVertexIndex		= 0;
		D3DXVECTOR3 vNormal;

		// generate the group of rings for the sphere
		for ( int iCurrentRing = 0; iCurrentRing < iRings + 1; iCurrentRing++ )
		{
			float r0 = sinf ( iCurrentRing * fDeltaRingAngle );
			float y0 = cosf ( iCurrentRing * fDeltaRingAngle );

			// generate the group of segments for the current ring
			for ( int iCurrentSegment = 0; iCurrentSegment < iSegments + 1; iCurrentSegment++ )
			{
				float x0 = r0 * sinf ( iCurrentSegment * fDeltaSegAngle );
				float z0 = r0 * cosf ( iCurrentSegment * fDeltaSegAngle );

				vNormal.x = x0;
				vNormal.y = y0;
				vNormal.z = z0;
		
				D3DXVec3Normalize ( &vNormal, &vNormal );

				float fRadius =sphereShape->getRadius ( );

				pVertices [ iVertex++ ] = NxVec3 ( vecCentre.x+(x0*fRadius), vecCentre.y+(y0*fRadius), vecCentre.z+(z0*fRadius) );

				// add two indices except for the last ring 
				if ( iCurrentRing != iRings )
				{
					pIndices [ iIndex ] = wVertexIndex;
					iIndex++;
					
					pIndices [ iIndex ] = wVertexIndex + ( WORD ) ( iSegments + 1 ); 
					iIndex++;
					
					wVertexIndex++; 
				}
			}
		}

		{
			WORD* pNewIndex = NULL;
			DWORD dwNewIndexCount = 0;

			// generate new mesh from indices
			dwNewIndexCount = (dwIndexCount-2) * 3;
			pNewIndex = new WORD [ dwNewIndexCount ];
			WORD wFace0 = pIndices [ 0 ];
			WORD wFace1 = 0;
			WORD wFace2 = pIndices [ 1 ];
			int iToggle = 0;
			DWORD dwIndex = 0;
			for ( DWORD i = 2; i < dwIndexCount; i++ )
			{
				// face assignments
				if ( iToggle==0 )
				{
					wFace0 = wFace0;
					wFace1 = wFace2;
					wFace2 = pIndices [ i ];
				}
				else
				{
					wFace0 = wFace2;
					wFace1 = wFace1;
					wFace2 = pIndices [ i ];
				}
				iToggle=1-iToggle;

				// get face vectors
				pNewIndex [ dwIndex++ ] = wFace0;
				pNewIndex [ dwIndex++ ] = wFace1;
				pNewIndex [ dwIndex++ ] = wFace2;

				dwIndexCount = dwNewIndexCount;
			}

			NxSphere obb = NxSphere ( NxVec3(0.0f,0.0f,0.0f), sphereShape->getRadius ( ) );
			triMesh.points=pVertices;
			triMesh.numVertices=dwVertexCount;
			triMesh.pointStrideBytes=sizeof(NxVec3);
			triMesh.numTriangles=dwIndexCount/3;
			triMesh.triangles=pIndices;
			triMesh.triangleStrideBytes=sizeof(NxU32)*3;
			triMesh.flags=0;

			/*
			NxBox obb=NxBox(NxVec3(0.0f,0.0f,0.0f),boxShape->getDimensions(),NxMat33(NX_IDENTITY_MATRIX));
			triMesh.points=new NxVec3[8];
			triMesh.numVertices=8;
			triMesh.pointStrideBytes=sizeof(NxVec3);
			triMesh.numTriangles=2*6;
			triMesh.triangles=new NxU32[2*6*3];
			triMesh.triangleStrideBytes=sizeof(NxU32)*3;
			triMesh.flags=0;
			NxComputeBoxPoints(obb,(NxVec3 *)triMesh.points);
			memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);
			*/

			//NxComputeBoxPoints(obb,(NxVec3 *)triMesh.points);

			//memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);

			//memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);
		}

		// setup mesh drawing properties
		//pMesh->iPrimitiveType   = D3DPT_TRIANGLESTRIP;
		//pMesh->iDrawVertexCount = pMesh->dwVertexCount;
		//pMesh->iDrawPrimitives  = pMesh->dwIndexCount - 2;
	}
	else
        NX_ASSERT(!"Invalid shape type");
#endif

    NX_ASSERT(triMesh.isValid());
}


void dbPhySetRigidBodyCCD ( int iObject, int iState )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		NxActor* pActor = pObject->pActorList [ 0 ];

		int				nbShapes	= pActor->getNbShapes ( );
		NxShape* const* shapeArray	= pActor->getShapes   ( ); 

		for ( int i = 0; i < nbShapes; i++ )
		{
			NxShape* shape = shapeArray [ i ];

			if ( iState == 0 )
			{
				/*
				NxCCDSkeleton* skeleton = shape->getCCDSkeleton ( );

				if ( skeleton )
					PhysX::pPhysicsSDK->releaseCCDSkeleton ( *skeleton );
				*/
				shape->setCCDSkeleton ( NULL );
				
				shape->setFlag        ( NX_SF_DYNAMIC_DYNAMIC_CCD, false );
			}

			if ( iState == 1 )
			{
				NxSimpleTriangleMesh triMesh;
				CreateMeshFromShape(triMesh,shape);
				NxCCDSkeleton *newSkeleton=PhysX::pPhysicsSDK->createCCDSkeleton(triMesh);

				//PhysX::pPhysicsSDK->releaseTriangleMesh ( &triMesh );

				shape->setCCDSkeleton(newSkeleton); 
				shape->setFlag(NX_SF_DYNAMIC_DYNAMIC_CCD, true);
			}
		}	
	}

	/*
	// mike - 070806 - commented out this line, causes problems with on screen placement of meshes
	{
		NxActor* newAct = pActor;
		int nbShapes=newAct->getNbShapes();
		NxShape *const* shapeArray=newAct->getShapes(); 
		for(int i=0;i <nbShapes;i++)
		{
			NxShape *shape=shapeArray[i];
			NxSimpleTriangleMesh triMesh;
			CreateMeshFromShape(triMesh,shape);
			NxCCDSkeleton *newSkeleton=PhysX::pPhysicsSDK->createCCDSkeleton(triMesh);
			//FreeSimpleMesh(triMesh);
			shape->setCCDSkeleton(newSkeleton); 
			shape->setFlag(NX_SF_DYNAMIC_DYNAMIC_CCD, true);
		}
	}
	*/
}

void dbPhySetRigidBodyLinearMomentum ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setLinearMomentum ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRigidBodyAngularMomentum ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setAngularMomentum ( NxVec3 ( fX, fY, fZ ) );
	}
}

DWORD dbPhyGetRigidBodyLinearMomentumXEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearMomentumX ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLinearMomentumYEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearMomentumY ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLinearMomentumZEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyLinearMomentumZ ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularMomentumXEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularMomentumX ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularMomentumYEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularMomentumY ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyAngularMomentumZEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodyAngularMomentumZ ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyLinearMomentumX ( int iObject )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLinearMomentum ( );

	return value.x;
}

float dbPhyGetRigidBodyLinearMomentumY ( int iObject )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLinearMomentum ( );
	
	return value.y;
}

float dbPhyGetRigidBodyLinearMomentumZ ( int iObject )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLinearMomentum ( );
	
	return value.z;
}

float dbPhyGetRigidBodyAngularMomentumX ( int iObject )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getAngularMomentum ( );
	
	return value.x;
}

float dbPhyGetRigidBodyAngularMomentumY ( int iObject )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getAngularMomentum ( );
	
	return value.y;
}

float dbPhyGetRigidBodyAngularMomentumZ ( int iObject )
{
		NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getAngularMomentum ( );
	
	return value.z;
}

void dbPhyAddRigidBodyForce ( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addForceAtPos ( NxVec3 ( fForceX, fForceY, fForceZ ), NxVec3 ( fX, fY, fZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyForceAtLocal ( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addForceAtLocalPos ( NxVec3 ( fForceX, fForceY, fForceZ ), NxVec3 ( fX, fY, fZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyLocalForce ( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addLocalForceAtPos ( NxVec3 ( fForceX, fForceY, fForceZ ), NxVec3 ( fX, fY, fZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyLocalForceAtLocal ( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addLocalForceAtLocalPos ( NxVec3 ( fForceX, fForceY, fForceZ ), NxVec3 ( fX, fY, fZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyForce ( int iObject, float fForceX, float fForceY, float fForceZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addForce ( NxVec3 ( fForceX, fForceY, fForceZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyLocalForce ( int iObject, float fForceX, float fForceY, float fForceZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addLocalForce ( NxVec3 ( fForceX, fForceY, fForceZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyTorque ( int iObject, float fForceX, float fForceY, float fForceZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addTorque ( NxVec3 ( fForceX, fForceY, fForceZ ), ( NxForceMode ) iMode );
	}
}

void dbPhyAddRigidBodyLocalTorque ( int iObject, float fForceX, float fForceY, float fForceZ, int iMode )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->addLocalTorque ( NxVec3 ( fForceX, fForceY, fForceZ ), ( NxForceMode ) iMode );
	}
}

DWORD dbPhyGetRigidBodyPointVelocityXEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyPointVelocityX ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyPointVelocityYEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyPointVelocityY ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyPointVelocityZEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyPointVelocityZ ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLocalPointVelocityXEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyLocalPointVelocityX ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLocalPointVelocityYEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyLocalPointVelocityY ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodyLocalPointVelocityZEx ( int iObject, float fX, float fY, float fZ )
{
	float fValue = dbPhyGetRigidBodyLocalPointVelocityZ ( iObject, fX, fY, fZ );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodyPointVelocityX ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.x;
}

float dbPhyGetRigidBodyPointVelocityY ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.y;
}

float dbPhyGetRigidBodyPointVelocityZ ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.z;
}

float dbPhyGetRigidBodyLocalPointVelocityX ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLocalPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.x;
}

float dbPhyGetRigidBodyLocalPointVelocityY ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLocalPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.y;
}

float dbPhyGetRigidBodyLocalPointVelocityZ ( int iObject, float fX, float fY, float fZ )
{
	NxVec3 value ( 0.0f, 0.0f, 0.0f );

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getLocalPointVelocity ( NxVec3 ( 0.0f, 0.0f, 0.0f ) );
	
	return value.z;
}

void dbPhySetRigidBodySleepLinearVelocity ( int iObject, float fThreshold )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setSleepLinearVelocity ( fThreshold );
	}
}

void dbPhySetRigidBodySleepAngularVelocity ( int iObject, float fThreshold )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->setSleepAngularVelocity ( fThreshold );
	}
}

void dbPhyWakeUpRigidBody ( int iObject, float fThreshold )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->wakeUp ( fThreshold );
	}
}

void dbPhySleepRigidBody ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->putToSleep ( );
	}
}

int dbPhyGetRigidBodySleeping ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( pObject->pActorList [ 0 ]->isSleeping ( ) )
			return 1;

		return 0;
	}

	return 0;
}

DWORD dbPhyGetRigidBodySleepLinearVelocityEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodySleepLinearVelocity ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetRigidBodySleepAngularVelocityEx ( int iObject )
{
	float fValue = dbPhyGetRigidBodySleepAngularVelocity ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetRigidBodySleepLinearVelocity ( int iObject )
{
	NxReal value;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getSleepLinearVelocity ( );
	
	return value;
}

float dbPhyGetRigidBodySleepAngularVelocity ( int iObject )
{
	NxReal value;

	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		value = pObject->pActorList [ 0 ]->getSleepAngularVelocity ( );
	
	return value;
}

void dbPhySetRigidBodyKinematic ( int iObject, int iState )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iState == 0 )
		{
			pObject->pActorList [ 0 ]->clearBodyFlag ( NX_BF_KINEMATIC );
			pObject->pActorList [ 0 ]->wakeUp ( );
		}

		if ( iState == 1 )
			pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_KINEMATIC );
	}
}

void dbPhySetRigidBodyKinematicPosition ( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( pObject->pActorList [ 0 ]->readBodyFlag ( NX_BF_KINEMATIC ) )
			pObject->pActorList [ 0 ]->moveGlobalPosition ( NxVec3 ( fX, fY, fZ ) );
	}
}

int dbPhyGetRigidBodyDynamic ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( pObject->state == PhysX::eDynamic )
			return 1;

		return 0;
	}

	return 0;
}



void dbPhySetRigidBodyKinematicRotation	( int iObject, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		NxMat33 rotX;
		NxMat33 rotY;
		NxMat33 rotZ;
		NxMat33 rotation;

		rotX.rotX ( D3DXToRadian ( pObject->pObject->position.vecRotate.x ) );
		rotY.rotY ( D3DXToRadian ( pObject->pObject->position.vecRotate.y ) );
		rotZ.rotZ ( D3DXToRadian ( pObject->pObject->position.vecRotate.z ) );
		
		rotation = rotX * rotY * rotZ;

		//pObject->pActorList [ 0 ]->moveGlobalOrientation ( rotation );

		if ( pObject->pActorList [ 0 ]->readBodyFlag ( NX_BF_KINEMATIC ) )
			pObject->pActorList [ 0 ]->moveGlobalOrientation ( AnglesToQuat(NxVec3( fX,fY ,fZ)) );

		//pActor->setGlobalOrientationQuat(AnglesToQuat(NxVec3(pObject->position.vecRotate.x,pObject->position.vecRotate.y,pObject->position.vecRotate.z)));
	}
}

void dbPhySetRigidBodyFrozen ( int iObject, int iAxis, int iState )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iState == 0 )
		{
			if ( iAxis == 0 ) pObject->pActorList [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_X );
			if ( iAxis == 1 ) pObject->pActorList [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_Y );
			if ( iAxis == 2 ) pObject->pActorList [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_Z );
		}

		if ( iState == 1 )
		{
			if ( iAxis == 0 ) pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_X );
			if ( iAxis == 1 ) pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_Y );
			if ( iAxis == 2 ) pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_Z );
		}
	}
}

void dbPhySetRigidBodyFrozen ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_X );
		pObject->pActorList [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_Y );
	}
}