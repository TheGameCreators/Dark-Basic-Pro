
/////////////////////////////////////////////////////////////////////////////////////
// CLOTH PROPERTIES /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	contains all functions relating to cloth properties
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INCLUDES /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "ClothProperties.h"
#include "globals.h"
#include "clothobject.h"
#include "clothstructure.h"
#include "rigidbodycreation.h"
#include "error.h"


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE DATA ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY SET CLOTH DIMENSIONS%LFFF%?dbPhySetClothDimensions@@YAXHMMM@Z%
	PHY SET CLOTH POSITION%LFFF%?dbPhySetClothPosition@@YAXHMMM@Z%
	PHY SET CLOTH ROTATION%LFFF%?dbPhySetClothRotation@@YAXHMMM@Z%
	PHY SET CLOTH STATIC%LL%?dbPhySetClothStatic@@YAXHH@Z%
	PHY SET CLOTH COLLISION%LL%?dbPhySetClothCollision@@YAXHH@Z%
	PHY SET CLOTH GRAVITY%LL%?dbPhySetClothGravity@@YAXHH@Z%
	PHY SET CLOTH BENDING%LL%?dbPhySetClothBending@@YAXHH@Z%
	PHY SET CLOTH DAMPING%LL%?dbPhySetClothDamping@@YAXHH@Z%
	PHY SET CLOTH TWO WAY COLLISION%LL%?dbPhySetClothTwoWayCollision@@YAXHH@Z%
	PHY SET CLOTH TEARING%LL%?dbPhySetClothTearing@@YAXHH@Z%
	PHY SET CLOTH THICKNESS%LF%?dbPhySetClothThickness@@YAXHM@Z%
	PHY SET CLOTH DENSITY%LF%?dbPhySetClothDensity@@YAXHM@Z%
	PHY SET CLOTH BENDING STIFFNESS%LF%?dbPhySetClothBendingStiffness@@YAXHM@Z%
	PHY SET CLOTH STRETCHING STIFFNESS%LF%?dbPhySetClothStretchingStiffness@@YAXHM@Z%
	PHY SET CLOTH FRICTION%LF%?dbPhySetClothFriction@@YAXHM@Z%
	PHY SET CLOTH PRESSURE%LF%?dbPhySetClothPressure@@YAXHM@Z%
	PHY SET CLOTH TEAR FACTOR%LF%?dbPhySetClothTearFactor@@YAXHM@Z%
	PHY SET CLOTH COLLISION RESPONSE COEFFICIENT%LF%?dbPhySetClothCollisionResponseCoefficient@@YAXHM@Z%
	PHY SET CLOTH ATTACHMENT RESPONSE COEFFICIENT%LF%?dbPhySetClothAttachmentResponseCoefficient@@YAXHM@Z%
	PHY SET CLOTH SOLVER ITERATIONS%LF%?dbPhySetClothSolverIterations@@YAXHH@Z%
	PHY SET CLOTH EXTERNAL ACCELERATION%LFFF%?dbPhySetClothExternalAcceleration@@YAXHMMM@Z%
	PHY SET CLOTH COLLISION GROUP%LL%?dbPhySetClothCollisionGroup@@YAXHH@Z%

	PHY ADD FORCE AT CLOTH VERTEX%LLFFFL%?dbPhyAddForceAtClothVertex@@YAXHHMMMH@Z%

	PHY GET CLOTH EXIST[%LL%?dbPhyGetClothExist@@YAHH@Z%
	PHY GET CLOTH BUILT[%LL%?dbPhyGetClothBuilt@@YAHH@Z%

	PHY CLOTH ATTACH RIGID BODY%LLFFF%?dbPhyClothAttachRigidBody@@YAXHHMMM@Z%

	DARKSDK void  dbPhyClothAttachRigidBody						( int iID, int iRigidBody, float fImpulse, float fPenetration, float fDeformation );

	PHY GET CLOTH WIDTH[%FL%?dbPhyGetClothWidthEx@@YAKH@Z%
	PHY GET CLOTH HEIGHT[%FL%?dbPhyGetClothHeightEx@@YAKH@Z%
	PHY GET CLOTH DEPTH[%FL%?dbPhyGetClothDepthEx@@YAKH@Z%
	PHY GET CLOTH POSITION X[%FL%?dbPhyGetClothPositionXEx@@YAKH@Z%
	PHY GET CLOTH POSITION Y[%FL%?dbPhyGetClothPositionYEx@@YAKH@Z%
	PHY GET CLOTH POSITION Z[%FL%?dbPhyGetClothPositionZEx@@YAKH@Z%
	PHY GET CLOTH ROTATION X[%FL%?dbPhyGetClothRotationXEx@@YAKH@Z%
	PHY GET CLOTH ROTATION Y[%FL%?dbPhyGetClothRotationYEx@@YAKH@Z%
	PHY GET CLOTH ROTATION Z[%FL%?dbPhyGetClothRotationZEx@@YAKH@Z%
	PHY GET CLOTH STATIC[%LL%?dbPhyGetClothStatic@@YAHH@Z%
	PHY GET CLOTH COLLISION[%LL%?dbPhyGetClothCollision@@YAHH@Z%
	PHY GET CLOTH GRAVITY[%LL%?dbPhyGetClothGravity@@YAHH@Z%
	PHY GET CLOTH BENDING[%LL%?dbPhyGetClothBending@@YAHH@Z%
	PHY GET CLOTH DAMPING[%LL%?dbPhyGetClothDamping@@YAHH@Z%
	PHY GET CLOTH TWO WAY COLLISION[%LL%?dbPhyGetClothTwoWayCollision@@YAHH@Z%
	PHY GET CLOTH TEARING[%LL%?dbPhyGetClothTearing@@YAHH@Z%
	PHY GET CLOTH THICKNESS[%FL%?dbPhyGetClothThicknessEx@@YAKH@Z%
	PHY GET CLOTH DENSITY[%FL%?dbPhyGetClothDensityEx@@YAKH@Z%
	PHY GET CLOTH BENDING STIFFNESS[%FL%?dbPhyGetClothBendingStiffnessEx@@YAKH@Z%
	PHY GET CLOTH STRETCHING STIFFNESS[%FL%?dbPhyGetClothStretchingStiffnessEx@@YAKH@Z%
	PHY GET CLOTH FRICTION[%FL%?dbPhyGetClothFrictionEx@@YAKH@Z%
	PHY GET CLOTH PRESSURE[%FL%?dbPhyGetClothPressureEx@@YAKH@Z%
	PHY GET CLOTH TEAR FACTOR[%FL%?dbPhyGetClothTearFactorEx@@YAKH@Z%
	PHY GET CLOTH COLLISION RESPONSE COEFFICIENT[%FL%?dbPhyGetClothCollisionResponseCoefficientEx@@YAKH@Z%
	PHY GET CLOTH ATTACHMENT RESPONSE COEFFICIENT[%FL%?dbPhyGetClothAttachmentResponseCoefficientEx@@YAKH@Z%
	PHY GET CLOTH SOLVER ITERATIONS[%LL%?dbPhyGetClothSolverIterations@@YAHH@Z%
	PHY GET CLOTH EXTERNAL ACCLERATION X[%FL%?dbPhyGetClothExternalAccelerationXEx@@YAKH@Z%
	PHY GET CLOTH EXTERNAL ACCLERATION Y[%FL%?dbPhyGetClothExternalAccelerationYEx@@YAKH@Z%
	PHY GET CLOTH EXTERNAL ACCLERATION Z[%FL%?dbPhyGetClothExternalAccelerationZEx@@YAKH@Z%
	PHY GET CLOTH COLLISION GROUP[%LL%?dbPhyGetClothCollisionGroup@@YAHH@Z%
	PHY GET CLOTH MIN SIZE X[%FL%?dbPhyGetClothMinSizeXEx@@YAKH@Z%
	PHY GET CLOTH MIN SIZE Y[%FL%?dbPhyGetClothMinSizeYEx@@YAKH@Z%
	PHY GET CLOTH MIN SIZE Z[%FL%?dbPhyGetClothMinSizeZEx@@YAKH@Z%
	PHY GET CLOTH MAX SIZE X[%FL%?dbPhyGetClothMaxSizeXEx@@YAKH@Z%
	PHY GET CLOTH MAX SIZE Y[%FL%?dbPhyGetClothMaxSizeYEx@@YAKH@Z%
	PHY GET CLOTH MAX SIZE Z[%FL%?dbPhyGetClothMaxSizeZEx@@YAKH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// FUNCTION LISTINGS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhySetClothDimensions ( int iID, float fWidth, float fHeight, float fDepth )
{
	// set dimensions of the cloth object

	// get the cloth
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		// store width, height and depth
		pCloth->fWidth  = fWidth;
		pCloth->fHeight = fHeight;
		pCloth->fDepth  = fDepth;

		// got to have this check along with the call to isValid later as
		// without it the cooking function can crash
		if ( fDepth <= 0.0f )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Depth value cannot be less than 0.0 for a cloth object", "", 0 );
			return;
		}
	}
}

void dbPhySetClothPosition ( int iID, float fX, float fY, float fZ )
{
	// set position of the cloth object

	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		pCloth->desc.globalPose.t = NxVec3 ( fX, fY, fZ );	
}

void dbPhySetClothRotation ( int iID, float fX, float fY, float fZ )
{
	// set rotation of the cloth object

	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		NxMat33 rotX;
		NxMat33 rotY;
		NxMat33 rotZ;
		NxMat33 rotation;
		rotX.rotX ( D3DXToRadian ( fX ) );
		rotY.rotY ( D3DXToRadian ( fY ) );
		rotZ.rotZ ( D3DXToRadian ( fZ ) );
		rotation = rotX * rotY * rotZ;

		pCloth->desc.globalPose.M = rotation;
	}
}

void dbPhySetClothFlag ( int iID, NxClothFlag flag, int iState )
{
	// set or remove a flag on a cloth object

	// get the cloth
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		// switch off
		if ( iState == 0 )
		{
			if ( pCloth->desc.flags & flag )
			{
				pCloth->desc.flags ^= flag;
			}
		}

		// switch on
		if ( iState == 1 )
		{
			pCloth->desc.flags |= flag;
		}

		// update the object if it has been built
		if ( NxCloth* pClothObject = dbPhyGetClothObject ( iID, false ) )
		{
			pClothObject->setFlags ( pCloth->desc.flags );
		}
	}
}

int dbPhyGetClothFlag ( int iID, NxClothFlag flag )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		if ( pCloth->desc.flags & flag )
		{
			return 1;
		}

		return 0;
	}

	return 0;
}

void dbPhySetClothStatic ( int iID, int iState )
{
	// static property
	dbPhySetClothFlag ( iID, NX_CLF_STATIC, iState );
}

void dbPhySetClothCollision	( int iID, int iState )
{
	// disable collision
	dbPhySetClothFlag ( iID, NX_CLF_DISABLE_COLLISION, iState );
}

void dbPhySetClothGravity ( int iID, int iState )
{
	// gravity
	dbPhySetClothFlag ( iID, NX_CLF_GRAVITY, iState );
}

void dbPhySetClothBending ( int iID, int iState )
{
	// bending state
	dbPhySetClothFlag ( iID, NX_CLF_BENDING, iState );
}

void dbPhySetClothDamping ( int iID, int iState )
{
	// damping on the cloth
	dbPhySetClothFlag ( iID, NX_CLF_DAMPING, iState );
}

void dbPhySetClothTwoWayCollision ( int iID, int iState )
{
	// two way collision
	dbPhySetClothFlag ( iID, NX_CLF_COLLISION_TWOWAY, iState );
}

void dbPhySetClothTearing ( int iID, int iState )
{
	// tearing
	dbPhySetClothFlag ( iID, NX_CLF_TEARABLE, iState );
}

enum eClothProperty
{
	eClothThickness,
	eClothDensity,
	eClothBendingStiffness,
	eClothStretchingStiffness,
	eClothFriction,
	eClothPressure,
	eClothTearFactor,
	eClothCollisionResponse,
	eClothAttachmentResponse,
	eClothSolverIterations,
	eClothAcceleration,
	eClothGroup
};

void dbPhySetClothProperty ( int iID, eClothProperty prop, float fValue = 0.0f, int iValue = 0, NxVec3 vec = NxVec3 ( ) )
{
	int iUpdate = 0;

	PhysX::sCloth*	pCloth		 = dbPhyGetCloth       ( iID, true  );
	NxCloth*		pClothObject = dbPhyGetClothObject ( iID, false );

	if ( pClothObject )
		iUpdate = 1;
		
	switch ( prop )
	{
		case eClothThickness:			if ( iUpdate == 0 ) pCloth->desc.thickness					   = fValue; else pCloth->pCloth->getNxCloth ( )->setThickness					   ( fValue );	break;
		case eClothDensity:				if ( iUpdate == 0 ) pCloth->desc.density					   = fValue;																					break;
		case eClothBendingStiffness:	if ( iUpdate == 0 ) pCloth->desc.bendingStiffness			   = fValue; else pCloth->pCloth->getNxCloth ( )->setBendingStiffness			   ( fValue );	break;
		case eClothStretchingStiffness:	if ( iUpdate == 0 ) pCloth->desc.stretchingStiffness		   = fValue; else pCloth->pCloth->getNxCloth ( )->setStretchingStiffness		   ( fValue );	break;
		case eClothFriction:			if ( iUpdate == 0 ) pCloth->desc.friction					   = fValue; else pCloth->pCloth->getNxCloth ( )->setFriction					   ( fValue );	break;
		case eClothPressure:			if ( iUpdate == 0 ) pCloth->desc.pressure					   = fValue; else pCloth->pCloth->getNxCloth ( )->setPressure					   ( fValue );	break;
		case eClothTearFactor:			if ( iUpdate == 0 ) pCloth->desc.tearFactor					   = fValue;																					break;
		case eClothCollisionResponse:	if ( iUpdate == 0 ) pCloth->desc.collisionResponseCoefficient  = fValue; else pCloth->pCloth->getNxCloth ( )->setCollisionResponseCoefficient  ( fValue );	break;
		case eClothAttachmentResponse:	if ( iUpdate == 0 ) pCloth->desc.attachmentResponseCoefficient = fValue; else pCloth->pCloth->getNxCloth ( )->setAttachmentResponseCoefficient ( fValue );	break;
		case eClothSolverIterations:	if ( iUpdate == 0 ) pCloth->desc.solverIterations              = iValue; else pCloth->pCloth->getNxCloth ( )->setSolverIterations			   ( iValue );	break;
		case eClothAcceleration:		if ( iUpdate == 0 ) pCloth->desc.externalAcceleration          = vec;    else pCloth->pCloth->getNxCloth ( )->setExternalAcceleration		   ( vec );		break;

		default:
			break;
	}
}

void dbPhySetClothThickness	( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothThickness, fValue );
}

void dbPhySetClothDensity ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothDensity, fValue );
}

void dbPhySetClothBendingStiffness ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothBendingStiffness, fValue );
}

void dbPhySetClothStretchingStiffness ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothStretchingStiffness, fValue );
}

void dbPhySetClothFriction ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothFriction, fValue );
}

void dbPhySetClothPressure ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothPressure, fValue );
}

void dbPhySetClothTearFactor ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothTearFactor, fValue );
}

void dbPhySetClothCollisionResponseCoefficient ( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothCollisionResponse, fValue );
}

void dbPhySetClothAttachmentResponseCoefficient	( int iID, float fValue )
{
	dbPhySetClothProperty ( iID, eClothAttachmentResponse, fValue );
}

void dbPhySetClothSolverIterations ( int iID, int iValue )
{
	dbPhySetClothProperty ( iID, eClothSolverIterations, 0.0f, iValue );
}

void dbPhySetClothExternalAcceleration ( int iID, float fX, float fY, float fZ )
{
	// set external acceleration on the cloth, this can be applied to
	// the descriptor or the cloth when it has been built

	dbPhySetClothProperty ( iID, eClothAcceleration, 0.0f, 0, NxVec3 ( fX, fY, fZ ) );
}

void dbPhySetClothCollisionGroup ( int iID, int iGroup )
{
	// group
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		pCloth->iGroup = iGroup;
}

void dbPhyAddForceAtClothVertex ( int iID, int iVertex, float fX, float fY, float fZ, int iMode )
{
	/* add force at the specified vertex on the cloth, the force modes are -
	
		0, force, has unit of mass * distance / time ^ 2, i.e. a force
		1, impulse, has unit of mass * distance / time
		2, velocity change, has unit of distance / time, i.e. the effect is mass independent: a velocity change
		3, smooth impulse, same as NX_IMPULSE but the effect is applied over all substeps, use this for motion controllers that repeatedly apply an impulse
		4, smooth velocity change, same as NX_VELOCITY_CHANGE but the effect is applied over all substeps, use this for motion controllers that repeatedly apply an impulse
		5, acceleration, has unit of distance / time ^ 2, i.e. an acceleration, it gets treated just like a force except the mass is not divided out before integration
	*/

	if ( NxCloth* pCloth = dbPhyGetClothObject ( iID, true ) )
		pCloth->addForceAtVertex ( NxVec3 ( fX, fY, fZ ), ( NxU32 ) iVertex, ( NxForceMode ) iMode );
}

int dbPhyGetClothExist ( int iID )
{
	// this function checks if the specified cloth object exists

	// return true when the cloth exists
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
		return 1;

	// and return false when it does not
	return 0;
}

int dbPhyGetClothBuilt ( int iID )
{
	// determine if a cloth object has been built

	// get the cloth
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		// if the data exists we're ok
		if ( pCloth->pCloth )
			return 1;
	}

	// if not then return false
	return 0;
}

void dbPhyClothAttachRigidBody ( int iID, int iRigidBody, float fImpulse, float fPenetration, float fDeformation )
{
	PhysX::sCloth*       pCloth  = dbPhyGetCloth  ( iID,        true );
	PhysX::sPhysXObject* pObject = dbPhyGetObject ( iRigidBody, true );
	NxActor* pActor = pObject->pActorList [ 0 ];
	pCloth->pCloth->getNxCloth ( )->attachToCore ( pActor, fImpulse, fPenetration, fDeformation ); 
}

//void NxCloth::attachToCore (NxActor *actor, NxReal impulseThreshold, NxReal penetrationDepth=0.0f, NxReal maxDeformationDistance = 0.0f);

DWORD dbPhyGetClothWidthEx ( int iID )
{
	float fValue = dbPhyGetClothWidth ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothHeightEx ( int iID )
{
	float fValue = dbPhyGetClothHeight ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothDepthEx ( int iID )
{
	float fValue = dbPhyGetClothDepth ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothPositionXEx ( int iID )
{
	float fValue = dbPhyGetClothPositionX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothPositionYEx ( int iID )
{
	float fValue = dbPhyGetClothPositionY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothPositionZEx ( int iID )
{
	float fValue = dbPhyGetClothPositionZ ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothRotationXEx ( int iID )
{
	float fValue = dbPhyGetClothRotationX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothRotationYEx ( int iID )
{
	float fValue = dbPhyGetClothRotationY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothRotationZEx ( int iID )
{
	float fValue = dbPhyGetClothRotationZ ( iID );
	return *( DWORD* ) &fValue;
}

float dbPhyGetClothWidth ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->fWidth;

	return 0.0f;
}

float dbPhyGetClothHeight ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->fHeight;
	
	return 0.0f;
}

float dbPhyGetClothDepth ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->fDepth;
	
	return 0.0f;
}

float dbPhyGetClothPositionX ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

float dbPhyGetClothPositionY ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

float dbPhyGetClothPositionZ ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

float dbPhyGetClothRotationX ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

float dbPhyGetClothRotationY ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

float dbPhyGetClothRotationZ ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return 0.0f;
	
	return 0.0f;
}

int dbPhyGetClothStatic	( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_STATIC );
}

int dbPhyGetClothCollision ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_DISABLE_COLLISION );
}

int dbPhyGetClothGravity ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_GRAVITY );
}

int dbPhyGetClothBending ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_BENDING );
}

int dbPhyGetClothDamping ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_DAMPING );
}

int dbPhyGetClothTwoWayCollision ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_COLLISION_TWOWAY );
}

int dbPhyGetClothTearing ( int iID )
{
	return dbPhyGetClothFlag ( iID, NX_CLF_TEARABLE );
}

DWORD dbPhyGetClothThicknessEx ( int iID )
{
	float fValue = dbPhyGetClothThickness ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothDensityEx ( int iID )
{
	float fValue = dbPhyGetClothDensity ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothBendingStiffnessEx ( int iID )
{
	float fValue = dbPhyGetClothBendingStiffness ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothStretchingStiffnessEx ( int iID )
{
	float fValue = dbPhyGetClothStretchingStiffness ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothFrictionEx ( int iID )
{
	float fValue = dbPhyGetClothFriction ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothPressureEx ( int iID )
{
	float fValue = dbPhyGetClothPressure ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothTearFactorEx ( int iID )
{
	float fValue = dbPhyGetClothTearFactor ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothCollisionResponseCoefficientEx ( int iID )
{
	float fValue = dbPhyGetClothCollisionResponseCoefficient ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothAttachmentResponseCoefficientEx ( int iID )
{
	float fValue = dbPhyGetClothAttachmentResponseCoefficient ( iID );
	return *( DWORD* ) &fValue;
}

float dbPhyGetClothThickness ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.thickness;
	
	return 0.0f;
}

float dbPhyGetClothDensity ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.density;
	
	return 0.0f;
}

float dbPhyGetClothBendingStiffness ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.bendingStiffness;

	return 0.0f;
}

float dbPhyGetClothStretchingStiffness ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.stretchingStiffness;
	
	return 0.0f;
}

float dbPhyGetClothFriction ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.friction;
	
	return 0.0f;
}

float dbPhyGetClothPressure	( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.pressure;
	
	return 0.0f;
}

float dbPhyGetClothTearFactor ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.tearFactor;
	
	return 0.0f;
}

float dbPhyGetClothCollisionResponseCoefficient	( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.collisionResponseCoefficient;
	
	return 0.0f;
}

float dbPhyGetClothAttachmentResponseCoefficient ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.attachmentResponseCoefficient;
	
	return 0.0f;
}

int dbPhyGetClothSolverIterations ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.solverIterations;
	
	return 0;
}

DWORD dbPhyGetClothExternalAccelerationXEx ( int iID )
{
	float fValue = dbPhyGetClothExternalAccelerationX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothExternalAccelerationYEx ( int iID )
{
	float fValue = dbPhyGetClothExternalAccelerationY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothExternalAccelerationZEx ( int iID )
{
	float fValue = dbPhyGetClothExternalAccelerationZ ( iID );
	return *( DWORD* ) &fValue;
}

float dbPhyGetClothExternalAccelerationX ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.externalAcceleration.x;
	
	return 0.0f;
}

float dbPhyGetClothExternalAccelerationY ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.externalAcceleration.y;
	
	return 0.0f;
}

float dbPhyGetClothExternalAccelerationZ ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->desc.externalAcceleration.z;
	
	return 0.0f;
}

int dbPhyGetClothCollisionGroup	( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
		return pCloth->iGroup;
	
	return 0;
}

DWORD dbPhyGetClothMinSizeXEx ( int iID )
{
	float fValue = dbPhyGetClothMinSizeX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothMinSizeYEx ( int iID )
{
	float fValue = dbPhyGetClothMinSizeY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothMinSizeZEx ( int iID )
{
	float fValue = dbPhyGetClothMinSizeZ ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothMaxSizeXEx ( int iID )
{
	float fValue = dbPhyGetClothMaxSizeX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothMaxSizeYEx ( int iID )
{
	float fValue = dbPhyGetClothMaxSizeY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetClothMaxSizeZEx ( int iID )
{
	float fValue = dbPhyGetClothMaxSizeZ ( iID );
	return *( DWORD* ) &fValue;
}

float dbPhyGetClothMinSizeX ( int iID )
{
	return dbPhyGetClothSize ( iID, 0 );
}

float dbPhyGetClothMinSizeY	( int iID )
{
	return dbPhyGetClothSize ( iID, 1 );
}

float dbPhyGetClothMinSizeZ	( int iID )
{
	return dbPhyGetClothSize ( iID, 2 );
}

float dbPhyGetClothMaxSizeX	( int iID )
{
	return dbPhyGetClothSize ( iID, 3 );
}

float dbPhyGetClothMaxSizeY	( int iID )
{
	return dbPhyGetClothSize ( iID, 4 );
}

float dbPhyGetClothMaxSizeZ	( int iID )
{
	return dbPhyGetClothSize ( iID, 5 );
}

float dbPhyGetClothSize ( int iID, int iIndex )
{
	if ( PhysX::sCloth*	pCloth = dbPhyGetCloth ( iID, true ) )
	{
		NxBounds3 bounds;
		
		pCloth->pCloth->getNxCloth ( )->getWorldBounds ( bounds );

		switch ( iIndex )
		{
			case 0:	return bounds.min.x;
			case 1:	return bounds.min.y;
			case 2:	return bounds.min.z;
			case 3:	return bounds.max.x;
			case 4:	return bounds.max.y;
			case 5:	return bounds.max.z;
		}
	}

	return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////