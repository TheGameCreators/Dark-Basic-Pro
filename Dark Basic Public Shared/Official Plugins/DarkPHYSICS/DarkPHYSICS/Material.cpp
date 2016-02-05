
#include "globals.h"
#include "material.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE MATERIAL%LS%?dbPhyMakeMaterial@@YAXHPAD@Z%
	PHY DELETE MATERIAL%L%?dbPhyDeleteMaterial@@YAXH@Z%
	PHY SET MATERIAL DYNAMIC FRICTION%LF%?dbPhySetMaterialDynamicFriction@@YAXHM@Z%
	PHY SET MATERIAL STATIC FRICTION%LF%?dbPhySetMaterialStaticFriction@@YAXHM@Z%
	PHY SET MATERIAL RESTITUTION%LF%?dbPhySetMaterialRestitution@@YAXHM@Z%
	PHY SET MATERIAL DYNAMIC FRICTION V%LF%?dbPhySetMaterialDynamicFrictionV@@YAXHM@Z%
	PHY SET MATERIAL STATIC FRICTION V%LF%?dbPhySetMaterialStaticFrictionV@@YAXHM@Z%
	PHY SET MATERIAL DIR OF ANISOTROPY%LFFF%?dbPhySetMaterialDirOfAnisotropy@@YAXHMMM@Z%
	PHY SET MATERIAL FRICTION COMBINE%LL%?dbPhySetMaterialFrictionCombine@@YAXHH@Z%
	PHY SET MATERIAL RESTITUTION COMBINE%LL%?dbPhySetMaterialRestitutionCombine@@YAXHH@Z%
	PHY BUILD MATERIAL%L%?dbPhyBuildMaterial@@YAXH@Z%
	PHY GET MATERIAL EXIST[%LL%?dbPhyGetMaterialExist@@YAHH@Z%

	PHY GET MATERIAL DYNAMIC FRICTION[%FL%?dbPhyGetMaterialDynamicFrictionEx@@YAKH@Z%
	PHY GET MATERIAL STATIC FRICTION[%FL%?dbPhyGetMaterialStaticFrictionEx@@YAKH@Z%
	PHY GET MATERIAL RESTITUTION[%FL%?dbPhyGetMaterialRestitutionEx@@YAKH@Z%
	PHY GET MATERIAL DYNAMIC FRICTION V[%FL%?dbPhyGetMaterialDynamicFrictionVEx@@YAKH@Z%
	PHY GET MATERIAL STATIC FRICTION V[%FL%?dbPhyGetMaterialStaticFrictionVEx@@YAKH@Z%
	PHY GET MATERIAL DIR OF ANISOTROPY X[%FL%?dbPhyGetMaterialDirOfAnisotropyXEx@@YAKH@Z%
	PHY GET MATERIAL DIR OF ANISOTROPY Y[%FL%?dbPhyGetMaterialDirOfAnisotropyYEx@@YAKH@Z%
	PHY GET MATERIAL DIR OF ANISOTROPY Z[%FL%?dbPhyGetMaterialDirOfAnisotropyZEx@@YAKH@Z%
	PHY GET MATERIAL FRICTION COMBINE[%LL%?dbPhyGetMaterialFrictionCombine@@YAHH@Z%
	PHY GET MATERIAL RESTITUTION COMBINE[%LL%?dbPhyGetMaterialRestitutionCombine@@YAHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeMaterial ( int iID, char* szName )
{
	// make a new material

	// see if the material already exists
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID ) )
	{
		// display an error
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a material with an ID number that is already in use", "", 0 );
		return;
	}

	// set up memory for the material
	PhysX::sPhysXMaterial* pMaterial = new PhysX::sPhysXMaterial;

	// check it was allocated
	if ( !pMaterial )
	{
		// display an error
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create material", "", 0 );
		return;
	}

	// set up initial data for the material
	pMaterial->iID       = iID;
	pMaterial->matIndex  = -1;
	pMaterial->pMaterial = NULL;

	// send this material to the back of the list
	PhysX::pMaterialList.push_back ( pMaterial );
}

void dbPhyDeleteMaterial ( int iID )
{
	// delete the specified material

	// get material data and index
	PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial      ( iID );
	int                    iIndex    = dbPhyGetMaterialIndex ( iID );

	// if the material does not exist then display an error message
	if ( !pMaterial )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to delete material that does not exist", "", 0 );
		return;
	}

	// ensure we have a valid material before attempting to delete
	if ( !pMaterial->pMaterial )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot delete material", "", 0 );
		return;
	}

	// check the scene
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene, cannot delete material", "", 0 );
		return;
	}

	// release the material
	PhysX::pScene->releaseMaterial ( *pMaterial->pMaterial );

	// erase it from the list
	PhysX::pMaterialList.erase ( PhysX::pMaterialList.begin ( ) + iIndex );

	// free up previously allocate memory
	delete pMaterial;
	pMaterial = NULL;
}

void dbPhySetMaterialDynamicFriction ( int iID, float fValue )
{
	// set the dynamic friction property of a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.dynamicFriction = fValue;
	}
}

void dbPhySetMaterialStaticFriction ( int iID, float fValue )
{
	// set the static friction property of a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.staticFriction = fValue;
	}
}

void dbPhySetMaterialRestitution ( int iID, float fValue )
{
	// set the restitution property of a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.restitution = fValue;
	}
}

void dbPhySetMaterialDynamicFrictionV ( int iID, float fValue )
{
	// set the dynamic friction v property of a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.dynamicFrictionV = fValue;
		pMaterial->matMaterial.flags |= ( NX_MF_ANISOTROPIC );
	}
}

void dbPhySetMaterialStaticFrictionV ( int iID, float fValue )
{
	// set the static friction v property of a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.staticFrictionV = fValue;
		pMaterial->matMaterial.flags |= ( NX_MF_ANISOTROPIC );
	}
}

void dbPhySetMaterialDirOfAnisotropy ( int iID, float fX, float fY, float fZ )
{
	// set the direction of anisotropy for a material
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		pMaterial->matMaterial.dirOfAnisotropy = NxVec3 ( fX, fY, fZ );
		pMaterial->matMaterial.flags |= ( NX_MF_ANISOTROPIC );
	}
}

void dbPhySetMaterialFrictionCombine ( int iID, int iMode )
{
	// set the friction combine mode
	dbPhySetMaterialCombine ( iID, 0, iMode );
}

void dbPhySetMaterialRestitutionCombine	( int iID, int iMode )
{
	// set the restitution combine mode
	dbPhySetMaterialCombine ( iID, 1, iMode );
}

void dbPhyBuildMaterial ( int iID )
{
	// build the specified material

	// get the material first
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		// check the material configuration is valid
		if ( pMaterial->matMaterial.isValid ( ) == false )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to build material as data is invalid. Check ranges of properties", "", 0 );
			return;
		}

		// create the material
		pMaterial->pMaterial = PhysX::pScene->createMaterial ( pMaterial->matMaterial );

		// check material is ok
		if ( !pMaterial->pMaterial )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to build material", "", 0 );
			return;
		}

		// store the index
		pMaterial->matIndex = pMaterial->pMaterial->getMaterialIndex ( );
	}
}

int dbPhyGetMaterialExist ( int iID )
{
	// check if a material exists

	// return true if it exists
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID ) )
		return 1;

	// and false if not
	return 0;
}

float dbPhyGetMaterialDynamicFriction ( int iID )
{
	// return dynamic friction
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.dynamicFriction;

	return 0.0f;
}

float dbPhyGetMaterialStaticFriction ( int iID )
{
	// return static friction
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.staticFriction;

	return 0.0f;
}

float dbPhyGetMaterialRestitution ( int iID )
{
	// return restitution
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.restitution;

	return 0.0f;
}

float dbPhyGetMaterialDynamicFrictionV ( int iID )
{
	// return dynamic friction v
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.dynamicFrictionV;

	return 0.0f;
}

float dbPhyGetMaterialStaticFrictionV ( int iID )
{
	// return static friction v
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.staticFrictionV;

	return 0.0f;
}

float dbPhyGetMaterialDirOfAnisotropyX ( int iID )
{
	// return direction of anisotropy x
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.dirOfAnisotropy.x;
	
	return 0.0f;
}

float dbPhyGetMaterialDirOfAnisotropyY ( int iID )
{
	// return direction of anisotropy y
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.dirOfAnisotropy.y;
	
	return 0.0f;
}

float dbPhyGetMaterialDirOfAnisotropyZ ( int iID )
{
	// return direction of anisotropy z
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.dirOfAnisotropy.z;
	
	return 0.0f;
}

DWORD dbPhyGetMaterialDynamicFrictionEx ( int iID )
{
	float fValue = dbPhyGetMaterialDynamicFriction ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialStaticFrictionEx ( int iID )
{
	float fValue = dbPhyGetMaterialStaticFriction ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialRestitutionEx ( int iID )
{
	float fValue = dbPhyGetMaterialRestitution ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialDynamicFrictionVEx ( int iID )
{
	float fValue = dbPhyGetMaterialDynamicFrictionV ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialStaticFrictionVEx	( int iID )
{
	float fValue = dbPhyGetMaterialStaticFrictionV ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialDirOfAnisotropyXEx ( int iID )
{
	float fValue = dbPhyGetMaterialDirOfAnisotropyX ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialDirOfAnisotropyYEx ( int iID )
{
	float fValue = dbPhyGetMaterialDirOfAnisotropyY ( iID );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetMaterialDirOfAnisotropyZEx ( int iID )
{
	float fValue = dbPhyGetMaterialDirOfAnisotropyZ ( iID );
	return *( DWORD* ) &fValue;
}

int dbPhyGetMaterialFrictionCombine ( int iID )
{
	// get the friction combine mode
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.frictionCombineMode;

	return 0;
}

int dbPhyGetMaterialRestitutionCombine ( int iID )
{
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
		return pMaterial->matMaterial.restitutionCombineMode;

	return 0;
}

void dbPhySetMaterialCombine ( int iID, int iType, int iMode )
{
	if ( PhysX::sPhysXMaterial* pMaterial = dbPhyGetMaterial ( iID, true ) )
	{
		NxCombineMode mode;

		switch ( iMode )
		{
			case 0: mode = NX_CM_AVERAGE;	break;
			case 1: mode = NX_CM_MIN;		break;
			case 2: mode = NX_CM_MULTIPLY;	break;
			case 3: mode = NX_CM_MAX;		break;
		};

		if ( iType == 0 )
			pMaterial->matMaterial.frictionCombineMode = mode;

		if ( iType == 1 )
			pMaterial->matMaterial.restitutionCombineMode = mode;
	}
}

PhysX::sPhysXMaterial* dbPhyGetMaterial ( int iID )
{
	for ( DWORD dwMaterial = 0; dwMaterial < PhysX::pMaterialList.size ( ); dwMaterial++ )
	{
		if ( PhysX::pMaterialList [ dwMaterial ]->iID == iID )
			return PhysX::pMaterialList [ dwMaterial ];
	}

	return NULL;
}

int dbPhyGetMaterialIndex ( int iID )
{
	for ( DWORD dwMaterial = 0; dwMaterial < PhysX::pMaterialList.size ( ); dwMaterial++ )
	{
		if ( PhysX::pMaterialList [ dwMaterial ]->iID == iID )
			return dwMaterial;
	}

	return 0;
}

void dbPhyClearMaterials ( void )
{
	if ( PhysX::pScene == NULL )
		return;

	for ( DWORD dwMaterial = 0; dwMaterial < PhysX::pMaterialList.size ( ); dwMaterial++ )
	{
		PhysX::sPhysXMaterial* pMaterial = PhysX::pMaterialList [ dwMaterial ];

		PhysX::pScene->releaseMaterial ( *pMaterial->pMaterial );

		delete pMaterial;
		pMaterial = NULL;
	}

	PhysX::pMaterialList.clear ( );
}

PhysX::sPhysXMaterial* dbPhyGetMaterial ( int iID, bool bDisplayError )
{
	for ( DWORD dwMaterial = 0; dwMaterial < PhysX::pMaterialList.size ( ); dwMaterial++ )
	{
		if ( PhysX::pMaterialList [ dwMaterial ]->iID == iID )
			return PhysX::pMaterialList [ dwMaterial ];
	}

	if ( bDisplayError && iID != -1 )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Material with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}