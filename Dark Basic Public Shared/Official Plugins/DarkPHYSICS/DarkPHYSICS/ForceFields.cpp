


#include "globals.h"
#include "forcefields.h"
#include "engineproperties.h"
#include "error.h"
#include "cRuntimeErrors.h"
#include "rigidbodycreation.h"
#include <windows.h>

#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY CREATE FORCE FIELD%L%?dbPhyCreateForceField@@YAXH@Z%
	PHY DELETE FORCE FIELD%L%?dbPhyDeleteForceField@@YAXH@Z%
	
	PHY SET FORCE FIELD RIGID BODY%LL%?dbPhySetForceFieldRigidBody@@YAXHH@Z%
	PHY SET FORCE FIELD CLOTH SCALE%LF%?dbPhySetForceFieldClothScale@@YAXHM@Z%
	PHY SET FORCE FIELD CONSTANT%LFFF%?dbPhySetForceFieldConstant@@YAXHMMM@Z%
	PHY SET FORCE FIELD VELOCITY TARGET%LFFF%?dbPhySetForceFieldVelocityTarget@@YAXHMMM@Z%
	PHY SET FORCE FIELD COORDINATES%LL%?dbPhySetForceFieldCoordinates@@YAXHH@Z%
	PHY SET FORCE FIELD LINEAR FALLOFF%LFFF%?dbPhySetForceFieldLinearFalloff@@YAXHMMM@Z%
	PHY SET FORCE FIELD QUADRATIC FALLOFF%LFFF%?dbPhySetForceFieldQuadraticFalloff@@YAXHMMM@Z%
	PHY SET FORCE FIELD NOISE%LFFF%?dbPhySetForceFieldNoise@@YAXHMMM@Z%
	PHY SET FORCE FIELD POSITION%LFFF%?dbPhySetForceFieldPosition@@YAXHMMM@Z%
	PHY SET FORCE FIELD RIGID BODY SCALE%LF%?dbPhySetForceFieldRigidBodyScale@@YAXHM@Z%
	PHY SET FORCE FIELD SOFT BODY SCALE%LF%?dbPhySetForceFieldSoftBodyScale@@YAXHM@Z%
	PHY SET FORCE FIELD TORUS RADIUS%LF%?dbPhySetForceFieldTorusRadius@@YAXHM@Z%
	
	PHY BUILD FORCE FIELD%L%?dbPhyBuildForceField@@YAXH@Z%

	PHY ADD FORCE FIELD BOX%LLFFFFFF%?dbPhyAddForceFieldBox@@YAXHHMMMMMM@Z%
	PHY ADD FORCE FIELD SPHERE%LLFFFF%?dbPhyAddForceFieldSphere@@YAXHHMMMM@Z%
	PHY ADD FORCE FIELD CAPSULE%LLFFFFF%?dbPhyAddForceFieldCapsule@@YAXHHMMMMM@Z%
	
	PHY GET FORCE FIELD EXIST[%LL%?dbPhyGetForceFieldExist@@YAHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


NxForceField* gForceField;
NxActor* gForceFieldActor;

namespace PhysX
{
	struct sForceField
	{
		int					iID;
		
		NxForceFieldDesc	description;
		NxForceField*		pData;

		NxForceFieldShape*	pShape;
		
	};

	std::vector < sForceField* > pForceFieldList;
};

PhysX::sForceField* dbPhyGetForceField ( int iID, bool bDisplayError = false )
{
	for ( DWORD dwForceField = 0; dwForceField < PhysX::pForceFieldList.size ( ); dwForceField++ )
	{
		if ( PhysX::pForceFieldList [ dwForceField ]->iID == iID )
		{
			return PhysX::pForceFieldList [ dwForceField ];
		}
	}

	return NULL;
}

void dbPhyCreateForceField ( int iID )
{
	PhysX::sForceField* pForceField = dbPhyGetForceField ( iID );

	if ( pForceField )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a force field with an ID that is already in use.", "", 0 );

	pForceField = new PhysX::sForceField;

	pForceField->iID = iID;

	PhysX::pForceFieldList.push_back ( pForceField );
}

void dbPhyDeleteForceField ( int iID )
{
	for ( DWORD dwForceField = 0; dwForceField < PhysX::pForceFieldList.size ( ); dwForceField++ )
	{
		if ( PhysX::pForceFieldList [ dwForceField ]->iID == iID )
		{
			PhysX::sForceField* pForceField = PhysX::pForceFieldList [ dwForceField ];

			if ( PhysX::pScene && pForceField->pData )
				PhysX::pScene->releaseForceField ( *pForceField->pData );

			PhysX::pForceFieldList.erase ( PhysX::pForceFieldList.begin ( ) + dwForceField );

			delete pForceField;
			pForceField = NULL;

			break;
		}
	}
}

void dbPhySetForceFieldRigidBody ( int iID, int iObject )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
		{
			pForceField->description.actor = pObject->pActorList [ 0 ];
		}
	}
}

void dbPhySetForceFieldClothScale ( int iID, float fScale )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.clothScale = fScale;
	}
}

void dbPhySetForceFieldConstant ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.constant = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldVelocityTarget ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.velocityTarget = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldCoordinates ( int iID, int iType )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		NxForceFieldCoordinates coordinates = NX_FFC_CARTESIAN;

		if ( iType == 0 ) coordinates = NX_FFC_CARTESIAN;
		if ( iType == 1 ) coordinates = NX_FFC_SPHERICAL;
		if ( iType == 2 ) coordinates = NX_FFC_CYLINDRICAL;
		if ( iType == 3 ) coordinates = NX_FFC_TOROIDAL;

		pForceField->description.coordinates = coordinates;
	}
}

void dbPhySetForceFieldLinearFalloff ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.falloffLinear = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldQuadraticFalloff ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.falloffQuadratic = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldNoise ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.noise = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldPosition ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.pose.t = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetForceFieldRigidBodyScale ( int iID, float fScale )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.rigidBodyScale = fScale;
	}
}

void dbPhySetForceFieldSoftBodyScale ( int iID, float fScale )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.softBodyScale = fScale;
	}
}

void dbPhySetForceFieldTorusRadius ( int iID, float fRadius )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->description.torusRadius = fRadius;
	}
}

void dbPhyBuildForceField ( int iID )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		pForceField->pData = PhysX::pScene->createForceField ( pForceField->description );
	}
}

void dbPhyAddForceFieldBox ( int iID, int iMode, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		if ( pForceField->pData )
		{
			NxBoxForceFieldShapeDesc box;

			box.pose.t     = NxVec3 ( fX, fY, fZ );
			box.dimensions = NxVec3 ( fSizeX, fSizeY, fSizeZ );
			
			if ( iMode == 1 )
				box.flags |= NX_FFS_EXCLUDE;

			pForceField->pShape = pForceField->pData->createShape ( box );
		}
	}
}

void dbPhyAddForceFieldSphere ( int iID, int iMode, float fX, float fY, float fZ, float fRadius )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		if ( pForceField->pData )
		{
			NxSphereForceFieldShapeDesc sphere;

			sphere.pose.t = NxVec3 ( fX, fY, fZ );
			sphere.radius = fRadius;
			
			if ( iMode == 1 )
				sphere.flags |= NX_FFS_EXCLUDE;

			pForceField->pShape = pForceField->pData->createShape ( sphere );
		}
	}
}

void dbPhyAddForceFieldCapsule ( int iID, int iMode, float fX, float fY, float fZ, float fHeight, float fRadius )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
	{
		if ( pForceField->pData )
		{
			NxCapsuleForceFieldShapeDesc capsule;

			capsule.pose.t = NxVec3 ( fX, fY, fZ );
			capsule.radius = fRadius;
			capsule.height = fHeight;
			
			if ( iMode == 1 )
				capsule.flags |= NX_FFS_EXCLUDE;

			pForceField->pShape = pForceField->pData->createShape ( capsule );
		}
	}
}

int dbPhyGetForceFieldExist ( int iID )
{
	if ( PhysX::sForceField* pForceField = dbPhyGetForceField ( iID ) )
		return 1;

	return 0;
}

/*
void dbPhyCreateForceFieldWind ( int iID, float fDirectionX, float fDirectionY, float fDirectionZ, float fSizeX, float fSizeY, float fSizeZ )
{
	// create a simple wind effect

	// declare our force field
	PhysX::sForceField* pForceField = NULL;

	// check force field doesn't already exist
	if ( pForceField = dbPhyGetForceField ( iID ) )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a force field with an ID that is already in use.", "", 0 );
	
	// allocate our new force field
	pForceField = new PhysX::sForceField;

	// check allocation was ok
	if ( !pForceField )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for force field.", "", 0 );

	// set up a matrix
	NxMat33 matrix;
	matrix.zero ( );

	// define properties for the wind
	pForceField->iID                            = iID;
	//pForceField->description.coordinates        = NX_FFC_CARTESIAN;
	pForceField->description.coordinates        = NX_FFC_SPHERICAL;
	pForceField->description.constant           = NxVec3 ( fDirectionX, fDirectionY, fDirectionZ );
	//pForceField->description.positionMultiplier = matrix;
	//pForceField->description.velocityTarget     = NxVec3 ( fDirectionX, fDirectionY, fDirectionZ );

	//matrix.diagonal ( NxVec3 ( 0.01f, 0.01f, 0.01f ) );
	//pForceField->description.velocityMultiplier = matrix;

	// we don't need an actor attached to this field
	pForceField->description.actor = NULL;

	// reset the post
	pForceField->description.pose.id ( );

	// check the scene is valid
	if ( !PhysX::pScene )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene. Cannot create force field.", "", 0 );

	// attempt to create the force field
	pForceField->pData = PhysX::pScene->createForceField ( pForceField->description );

	// check the force field was created
	if ( !pForceField->pData )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Force field creation failed.", "", 0 );

	// finally create a shape to define the wind effect
	NxBoxForceFieldShapeDesc box;

	box.dimensions = NxVec3 ( fSizeX, fSizeY, fSizeZ );
	box.pose.t     = NxVec3 ( 0.0f, 0.0f, 0.0f );

	pForceField->pShape = pForceField->pData->createShape ( box );

	// check shape was allocated
	if ( !pForceField->pShape )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to create shape for force field.", "", 0 );

	// add this force field into the list
	PhysX::pForceFieldList.push_back ( pForceField );
}


DARKSDK void dbPhyCreateForceField ( void )
{
	NxForceFieldDesc desc;
	desc.coordinates = NX_FFC_CARTESIAN;

	desc.constant = NxVec3(0.5, 0, 0);

	//The forces do not depend on where the objects are positioned
	NxMat33 m;
	m.zero();
	desc.positionMultiplier = m;
	desc.noise = NxVec3(5,5,5); //adds a random noise on the forces to make the objects a little more chaotic

	//Set target velocity along the main axis to 20
	desc.velocityTarget = NxVec3(0.01,0,0);
	m.diagonal(NxVec3(0.01,0,0)); //Acts with a force relative to the current velocity to reach the
							   //target velocities. 0 means that those components won't be affected
	desc.velocityMultiplier = m;

	//We don't attach this force field to an actor
	desc.actor = NULL;

	//Create the force field around origo
	desc.pose.id();

	gForceField = PhysX::pScene->createForceField(desc);

	//Attach an include shape, we position this so that it covers the whole scene
	NxForceFieldShape* s = NULL;
	NxBoxForceFieldShapeDesc b;
	b.dimensions = NxVec3(150, 20, 150);
	b.pose.t = NxVec3(0, 3.5f, 0);
	s = gForceField->createShape(b);
}
*/