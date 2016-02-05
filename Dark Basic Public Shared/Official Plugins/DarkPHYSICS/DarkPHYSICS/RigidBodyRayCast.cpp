
#include "globals.h"
#include "rigidbodyraycast.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY RAY CAST ALL SHAPES[%LFFFFFF%?dbPhyRayCastAllShapes@@YAHMMMMMM@Z%
	PHY RAY CAST[%LFFFFFF%?dbPhyRayCast@@YAHMMMMMM@Z%

	PHY RAY CAST[%LFFFFFFL%?dbPhyRayCast@@YAHMMMMMM@Z%

	PHY GET RAY CAST HIT[%L%?dbPhyGetRayCastHit@@YAHXZ%

	PHY GET RAY CAST HIT POINT X[%F%?dbPhyGetRayCastHitPointXEx@@YAKXZ%
	PHY GET RAY CAST HIT POINT Y[%F%?dbPhyGetRayCastHitPointYEx@@YAKXZ%
	PHY GET RAY CAST HIT POINT Z[%F%?dbPhyGetRayCastHitPointZEx@@YAKXZ%
	PHY GET RAY CAST HIT NORMAL X[%F%?dbPhyGetRayCastHitNormalXEx@@YAKXZ%
	PHY GET RAY CAST HIT NORMAL Y[%F%?dbPhyGetRayCastHitNormalYEx@@YAKXZ%
	PHY GET RAY CAST HIT NORMAL Z[%F%?dbPhyGetRayCastHitNormalZEx@@YAKXZ%
	PHY GET RAY CAST DISTANCE[%F%?dbPhyGetRayCastDistanceEx@@YAKXZ%
	PHY GET RAY CAST TU[%F%?dbPhyGetRayCastTU@@YAKXZ%
	PHY GET RAY CAST TV[%F%?dbPhyGetRayCastTV@@YAKXZ%

	PHY GET RAY CAST OBJECT[%L%?dbPhyGetRayCastObject@@YAHXZ%

	PHY RAY CAST CLOSEST SHAPE[%LFFFFFF%?dbPhyRayCastClosestShape@@YAHMMMMMM@Z%

	int dbPhyRayCastClosestShape ( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ )
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


bool PhysX::cRayCastReport::onHit ( const NxRaycastHit& hit )
{
	NxActor* pActor = &hit.shape->getActor ( );
	m_HitData = hit;

	if ( pActor )
	{
		if ( pActor->userData )
		{
			PhysX::sPhysXObject* pPhysXObject = ( PhysX::sPhysXObject* ) pActor->userData;
			m_iID = pPhysXObject->iID;
		}
	}

	return false;
}

int dbPhyRayCastAllShapes ( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ )
{
	NxRay worldRay;

	worldRay.orig = NxVec3 ( fOriginX, fOriginY, fOriginZ );
	worldRay.dir  = NxVec3 ( fDirX,    fDirY,    fDirZ );
	PhysX::RayCastReport.m_bHit        = false;
	
	if ( PhysX::pScene->raycastAllShapes ( worldRay, PhysX::RayCastReport, NX_ALL_SHAPES ) )
		PhysX::RayCastReport.m_bHit = true;

	return ( int ) PhysX::RayCastReport.m_bHit;
}

int dbPhyRayCastClosestShape ( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ )
{
	NxRay worldRay;

	worldRay.orig = NxVec3 ( fOriginX, fOriginY, fOriginZ );
	worldRay.dir  = NxVec3 ( fDirX,    fDirY,    fDirZ );
	PhysX::RayCastReport.m_bHit        = false;

	
	NxRaycastHit hit;
	NxShape* closestShape = PhysX::pScene->raycastClosestShape ( worldRay, NX_ALL_SHAPES, hit );

	if ( closestShape )
	{
		NxActor* pActor = &closestShape->getActor ( );

		if ( pActor )
		{
			if ( pActor->userData )
			{
				PhysX::sPhysXObject* pPhysXObject = ( PhysX::sPhysXObject* ) pActor->userData;

				PhysX::RayCastReport.m_HitData = hit;

				PhysX::RayCastReport.m_iID = pPhysXObject->iID;

				return 1;
			}
		}
	}

	return 0;


	//if ( PhysX::pScene->raycastClosestShape ( worldRay, NX_ALL_SHAPES, &hit, 0, 10000.0f, 0, 0, 0 ) )
	//	PhysX::RayCastReport.m_bHit = true;

	//return ( int ) PhysX::RayCastReport.m_bHit;
}

int dbPhyRayCast ( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ, int iType )
{
	// 280808 - Raycast with TYPE (0-all/1-dyn/2-static) uses DISTANCE check 
	NxRay worldRay;

	D3DXVECTOR3 StartVector = D3DXVECTOR3 ( fOriginX, fOriginY, fOriginZ ); // 280808 - fOriginX not fOriginX
	D3DXVECTOR3 EndVector   = D3DXVECTOR3 ( fNewX, fNewY, fNewZ );
	D3DXVECTOR3 DirVector   = EndVector - StartVector;
	float fLengthOfRay	    = D3DXVec3Length ( &DirVector );

	D3DXVec3Normalize( &DirVector, &DirVector );

	worldRay.orig = NxVec3 ( fOriginX, fOriginY, fOriginZ );
	worldRay.dir  = NxVec3 ( DirVector.x, DirVector.y, DirVector.z );

	PhysX::RayCastReport.m_bHit        = false;
	
	if ( iType == 0 )
	{
		if ( PhysX::pScene->raycastAllShapes ( worldRay, PhysX::RayCastReport, NX_ALL_SHAPES ) )
			if ( PhysX::RayCastReport.m_HitData.distance < fLengthOfRay )
				PhysX::RayCastReport.m_bHit = true;
	}

	if ( iType == 1 )
	{
		if ( PhysX::pScene->raycastAllShapes ( worldRay, PhysX::RayCastReport, NX_DYNAMIC_SHAPES ) )
			if ( PhysX::RayCastReport.m_HitData.distance < fLengthOfRay )
				PhysX::RayCastReport.m_bHit = true;
	}

	if ( iType == 2 )
	{
		if ( PhysX::pScene->raycastAllShapes ( worldRay, PhysX::RayCastReport, NX_STATIC_SHAPES ) )
		{
			if ( PhysX::RayCastReport.m_HitData.distance < fLengthOfRay )
				PhysX::RayCastReport.m_bHit = true;
		}
	}

	return ( int ) PhysX::RayCastReport.m_bHit;
}


int dbPhyRayCast ( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ )
{
	NxRay worldRay;

	D3DXVECTOR3 StartVector = D3DXVECTOR3 ( fOriginX, fOriginY, fOriginZ ); // 280808 - fOriginX not fOriginX
	D3DXVECTOR3 EndVector   = D3DXVECTOR3 ( fNewX, fNewY, fNewZ );
	D3DXVECTOR3 DirVector   = EndVector - StartVector;

	D3DXVec3Normalize( &DirVector, &DirVector );

	worldRay.orig = NxVec3 ( fOriginX, fOriginY, fOriginZ );
	worldRay.dir  = NxVec3 ( DirVector.x, DirVector.y, DirVector.z );

	PhysX::RayCastReport.m_bHit        = false;
	
	if ( PhysX::pScene->raycastAllShapes ( worldRay, PhysX::RayCastReport, NX_ALL_SHAPES ) )
		PhysX::RayCastReport.m_bHit = true;

	return ( int ) PhysX::RayCastReport.m_bHit;
}

int dbPhyGetRayCastHit ( void )
{
	return PhysX::RayCastReport.m_bHit;
}

float dbPhyGetRayCastHitPointX ( void )
{
	return PhysX::RayCastReport.m_HitData.worldImpact.x;
}

float dbPhyGetRayCastHitPointY ( void )
{
	return PhysX::RayCastReport.m_HitData.worldImpact.y;
}

float dbPhyGetRayCastHitPointZ ( void )
{
	return PhysX::RayCastReport.m_HitData.worldImpact.z;
}

float dbPhyGetRayCastHitNormalX ( void )
{
	return PhysX::RayCastReport.m_HitData.worldNormal.x;
}

float dbPhyGetRayCastHitNormalY ( void )
{
	return PhysX::RayCastReport.m_HitData.worldNormal.y;
}

float dbPhyGetRayCastHitNormalZ ( void )
{
	return PhysX::RayCastReport.m_HitData.worldNormal.z;
}

float dbPhyGetRayCastDistance ( void )
{
	return PhysX::RayCastReport.m_HitData.distance;
}

float dbPhyGetRayCastTU ( void )
{
	return PhysX::RayCastReport.m_HitData.u;
}

float dbPhyGetRayCastTV ( void )
{
	return PhysX::RayCastReport.m_HitData.v;
}

DARKSDK DWORD dbPhyGetRayCastHitPointXEx ( void )
{
	float fValue = dbPhyGetRayCastHitPointX ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastHitPointYEx ( void )
{
	float fValue = dbPhyGetRayCastHitPointY ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastHitPointZEx ( void )
{
	float fValue = dbPhyGetRayCastHitPointZ ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastHitNormalXEx ( void )
{
	float fValue = dbPhyGetRayCastHitNormalX ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastHitNormalYEx ( void )
{
	float fValue = dbPhyGetRayCastHitNormalY ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastHitNormalZEx ( void )
{
	float fValue = dbPhyGetRayCastHitNormalZ ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastDistanceEx ( void )
{
	float fValue = dbPhyGetRayCastDistance ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastTUEx ( void )
{
	float fValue = dbPhyGetRayCastTU ( );
	return *( DWORD* ) &fValue;
}

DARKSDK DWORD dbPhyGetRayCastTVEx ( void )
{
	float fValue = dbPhyGetRayCastTV ( );
	return *( DWORD* ) &fValue;
}

int dbPhyGetRayCastObject ( void )
{
	return PhysX::RayCastReport.m_iID;
}

/*
NxShapesType
NX_STATIC_SHAPES  Hits static shapes.  
NX_DYNAMIC_SHAPES  Hits dynamic shapes.  
NX_ALL_SHAPES  Hits both static & dynamic shapes 

virtual NxU32 NxScene::raycastAllBounds  (  const NxRay &  worldRay,  
  NxUserRaycastReport &  report,  
  NxShapesType  shapesType,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  NxU32  hintFlags = 0xffffffff,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 



virtual bool NxScene::raycastAnyBounds  (  const NxRay &  worldRay,  
  NxShapesType  shapesType,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 


 virtual NxU32 NxScene::raycastAllShapes  (  const NxRay &  worldRay,  
  NxUserRaycastReport &  report,  
  NxShapesType  shapesType,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  NxU32  hintFlags = 0xffffffff,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 


 virtual bool NxScene::raycastAnyShape  (  const NxRay &  worldRay,  
  NxShapesType  shapesType,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 
 

 virtual NxShape* NxScene::raycastClosestBounds  (  const NxRay &  worldRay,  
  NxShapesType  shapeType,  
  NxRaycastHit &  hit,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  NxU32  hintFlags = 0xffffffff,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 

 virtual NxShape* NxScene::raycastClosestShape  (  const NxRay &  worldRay,  
  NxShapesType  shapeType,  
  NxRaycastHit &  hit,  
  NxU32  groups = 0xffffffff,  
  NxReal  maxDist = NX_MAX_F32,  
  NxU32  hintFlags = 0xffffffff,  
  const NxGroupsMask *  groupsMask = NULL 
 )  const [pure virtual] 


 */