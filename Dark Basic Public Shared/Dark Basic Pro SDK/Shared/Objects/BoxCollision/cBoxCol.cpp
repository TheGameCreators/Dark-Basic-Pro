//
//
//

#include "cBoxCol.h"
#include "PolyPool.h"
#include "3DCol.h"

// Global Collision Data
D3DXVECTOR3 gvLatestStaticCollisionResult;
D3DXVECTOR3 gvLatestObjectCollisionResult;
D3DXVECTOR3 gvLightOfSight;

// Poly Pool Object
CPolyPool gPolyPool;
C3DCollision NewCol;

//
// Internal Functions
//

namespace
{
	// 20120404 IanM - Common method of calculating the object rotation matrix
	D3DXMATRIX CalcObjectRotationMatrix(sObject* Obj)
	{
		D3DXMATRIX matRotation;
		if ( Obj->position.bFreeFlightRotation )
			matRotation = Obj->position.matFreeFlightRotate;
		else
		{
			D3DXMATRIX matRotateX, matRotateY, matRotateZ;
			D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( Obj->position.vecRotate.x ) );	// x rotation
			D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( Obj->position.vecRotate.y ) );	// y rotation
			D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( Obj->position.vecRotate.z ) );	// z rotation

			matRotation = matRotateX * matRotateY * matRotateZ;
		}
		// 20120404 IanM - Apply any pivot to the rotation matrix
		if (Obj->position.bApplyPivot)
		{
			matRotation *= Obj->position.matPivot;
		}
		return matRotation;
	}
}


int CheckWorldBoxCollision ( D3DXVECTOR3 box1p, D3DXMATRIX matARotation, sCollisionData* pCollisionA, D3DXVECTOR3 box2p, D3DXMATRIX matBRotation, sCollisionData* pCollisionB )
{
	// Box 1
	D3DXVECTOR3 box1s;
	box1s.x=(pCollisionA->vecMax.x-pCollisionA->vecMin.x)*0.5f;
	box1s.y=(pCollisionA->vecMax.y-pCollisionA->vecMin.y)*0.5f;
	box1s.z=(pCollisionA->vecMax.z-pCollisionA->vecMin.z)*0.5f;
	D3DXVECTOR3 box1offset;
	box1offset.x=pCollisionA->vecMin.x+box1s.x;
	box1offset.y=pCollisionA->vecMin.y+box1s.y;
	box1offset.z=pCollisionA->vecMin.z+box1s.z;
	D3DXVec3TransformCoord ( &box1offset, &box1offset,  &matARotation );
	box1p+=box1offset;

	// Box 2
	D3DXVECTOR3 box2s;
	box2s.x=(pCollisionB->vecMax.x-pCollisionB->vecMin.x)*0.5f;
	box2s.y=(pCollisionB->vecMax.y-pCollisionB->vecMin.y)*0.5f;
	box2s.z=(pCollisionB->vecMax.z-pCollisionB->vecMin.z)*0.5f;
	D3DXVECTOR3 box2offset;
	box2offset.x=pCollisionB->vecMin.x+box2s.x;
	box2offset.y=pCollisionB->vecMin.y+box2s.y;
	box2offset.z=pCollisionB->vecMin.z+box2s.z;
	D3DXVec3TransformCoord ( &box2offset, &box2offset,  &matBRotation );
	box2p+=box2offset;

	// Perform Test
	return NewCol.TestBoxInBox(box1p, box1s, matARotation, box2p, box2s, matBRotation);
}

int CheckCollisionPool(float ax1, float ay1, float az1, float ax2, float ay2, float az2, float x1, float y1, float z1, float x2, float y2, float z2, D3DXVECTOR3* vResult)
{
	int iResult=0;
	iResult = gPolyPool.CheckColBox(ax1,ay1,az1,ax2,ay2,az2,x1,y1,z1,x2,y2,z2,vResult);
	return iResult;
}

int CheckCollisionPoolWithLine(float sx, float sy, float sz, float dx, float dy, float dz, float width, float acc, D3DXVECTOR3* vResult)
{
	int iResult=0;
	iResult = gPolyPool.CheckColLine(sx,sy,sz,dx,dy,dz,width,acc,vResult);
	return iResult;
}

int CheckUnRotatedBoxes( sObject* pA, sObject* pB )
{
	/* lee - 310306 - u6rc4 - unrotatedboxes do not account for scale, and should
	float oAx1 = pA->position.vecLastPosition.x + pA->collision.vecMin.x;
	float oAx2 = pA->position.vecLastPosition.x + pA->collision.vecMax.x;
	float oAy1 = pA->position.vecLastPosition.y + pA->collision.vecMin.y;
	float oAy2 = pA->position.vecLastPosition.y + pA->collision.vecMax.y;
	float oAz1 = pA->position.vecLastPosition.z + pA->collision.vecMin.z;
	float oAz2 = pA->position.vecLastPosition.z + pA->collision.vecMax.z;
	float oBx1 = pB->position.vecLastPosition.x + pB->collision.vecMin.x;
	float oBx2 = pB->position.vecLastPosition.x + pB->collision.vecMax.x;
	float oBy1 = pB->position.vecLastPosition.y + pB->collision.vecMin.y;
	float oBy2 = pB->position.vecLastPosition.y + pB->collision.vecMax.y;
	float oBz1 = pB->position.vecLastPosition.z + pB->collision.vecMin.z;
	float oBz2 = pB->position.vecLastPosition.z + pB->collision.vecMax.z;
	float Ax1 = pA->position.vecPosition.x + pA->collision.vecMin.x;
	float Ax2 = pA->position.vecPosition.x + pA->collision.vecMax.x;
	float Ay1 = pA->position.vecPosition.y + pA->collision.vecMin.y;
	float Ay2 = pA->position.vecPosition.y + pA->collision.vecMax.y;
	float Az1 = pA->position.vecPosition.z + pA->collision.vecMin.z;
	float Az2 = pA->position.vecPosition.z + pA->collision.vecMax.z;
	float Bx1 = pB->position.vecPosition.x + pB->collision.vecMin.x;
	float Bx2 = pB->position.vecPosition.x + pB->collision.vecMax.x;
	float By1 = pB->position.vecPosition.y + pB->collision.vecMin.y;
	float By2 = pB->position.vecPosition.y + pB->collision.vecMax.y;
	float Bz1 = pB->position.vecPosition.z + pB->collision.vecMin.z;
	float Bz2 = pB->position.vecPosition.z + pB->collision.vecMax.z;
	*/
	float oAx1 = pA->position.vecLastPosition.x + pA->collision.vecMin.x * pA->position.vecScale.x;
	float oAx2 = pA->position.vecLastPosition.x + pA->collision.vecMax.x * pA->position.vecScale.x;
	float oAy1 = pA->position.vecLastPosition.y + pA->collision.vecMin.y * pA->position.vecScale.y;
	float oAy2 = pA->position.vecLastPosition.y + pA->collision.vecMax.y * pA->position.vecScale.y;
	float oAz1 = pA->position.vecLastPosition.z + pA->collision.vecMin.z * pA->position.vecScale.z;
	float oAz2 = pA->position.vecLastPosition.z + pA->collision.vecMax.z * pA->position.vecScale.z;
	float oBx1 = pB->position.vecLastPosition.x + pB->collision.vecMin.x * pB->position.vecScale.x;
	float oBx2 = pB->position.vecLastPosition.x + pB->collision.vecMax.x * pB->position.vecScale.x;
	float oBy1 = pB->position.vecLastPosition.y + pB->collision.vecMin.y * pB->position.vecScale.y;
	float oBy2 = pB->position.vecLastPosition.y + pB->collision.vecMax.y * pB->position.vecScale.y;
	float oBz1 = pB->position.vecLastPosition.z + pB->collision.vecMin.z * pB->position.vecScale.z;
	float oBz2 = pB->position.vecLastPosition.z + pB->collision.vecMax.z * pB->position.vecScale.z;
	float Ax1 = pA->position.vecPosition.x + pA->collision.vecMin.x * pA->position.vecScale.x;
	float Ax2 = pA->position.vecPosition.x + pA->collision.vecMax.x * pA->position.vecScale.x;
	float Ay1 = pA->position.vecPosition.y + pA->collision.vecMin.y * pA->position.vecScale.y;
	float Ay2 = pA->position.vecPosition.y + pA->collision.vecMax.y * pA->position.vecScale.y;
	float Az1 = pA->position.vecPosition.z + pA->collision.vecMin.z * pA->position.vecScale.z;
	float Az2 = pA->position.vecPosition.z + pA->collision.vecMax.z * pA->position.vecScale.z;
	float Bx1 = pB->position.vecPosition.x + pB->collision.vecMin.x * pB->position.vecScale.x;
	float Bx2 = pB->position.vecPosition.x + pB->collision.vecMax.x * pB->position.vecScale.x;
	float By1 = pB->position.vecPosition.y + pB->collision.vecMin.y * pB->position.vecScale.y;
	float By2 = pB->position.vecPosition.y + pB->collision.vecMax.y * pB->position.vecScale.y;
	float Bz1 = pB->position.vecPosition.z + pB->collision.vecMin.z * pB->position.vecScale.z;
	float Bz2 = pB->position.vecPosition.z + pB->collision.vecMax.z * pB->position.vecScale.z;
	int collision=0;
	if(Ax1<Bx2)
	{
		if(Az1<Bz2)
		{
			if(Ax2>Bx1)
			{
				if(Az2>Bz1)
				{
					if(Ay1<By2)
					{
						if(Ay2>By1)
						{
							if(pB->collision.bBoundProduceResult==1)
							{
								short side=0;
								float distx1 = (float)fabs(oAx2-oBx1);
								float distx2 = (float)fabs(oBx2-oAx1);
								float disty1 = (float)fabs(oAy2-oBy1);
								float disty2 = (float)fabs(oBy2-oAy1);
								float distz1 = (float)fabs(oAz2-oBz1);
								float distz2 = (float)fabs(oBz2-oAz1);
								float dist = distx1;
								if(distx2<dist) { side=1; dist=distx2; }
								if(disty1<dist) { side=2; dist=disty1; }
								if(disty2<dist) { side=3; dist=disty2; }
								if(distz1<dist) { side=4; dist=distz1; }
								if(distz2<dist) { side=5; dist=distz2; }
								switch(side)
								{
									case 0 : gvLatestObjectCollisionResult.x = Ax2-Bx1; break;
									case 1 : gvLatestObjectCollisionResult.x = Ax1-Bx2; break;
									case 2 : gvLatestObjectCollisionResult.y = Ay2-By1; break;
									case 3 : gvLatestObjectCollisionResult.y = Ay1-By2; break;
									case 4 : gvLatestObjectCollisionResult.z = Az2-Bz1; break;
									case 5 : gvLatestObjectCollisionResult.z = Az1-Bz2; break;
								}
							}
							collision=1;
						}
					}
				}
			}
		}
	}
	return collision;
}

int CheckAdjustedAagainstB ( D3DXVECTOR3 box1p, sObject* pA, sObject* pB )
{
	// Work vars
	D3DXMATRIX matRotateX, matRotateY, matRotateZ;

	// Box 1
	D3DXVECTOR3 box1s;
	box1s.x=(pA->collision.vecMax.x-pA->collision.vecMin.x)*0.5f;
	box1s.y=(pA->collision.vecMax.y-pA->collision.vecMin.y)*0.5f;
	box1s.z=(pA->collision.vecMax.z-pA->collision.vecMin.z)*0.5f;
	D3DXVECTOR3 box1offset;
	// lee - 140206 - u60 - you have to scale offset as well - otherwise the center of the box is wrong
//	box1offset.x=pA->collision.vecMin.x+box1s.x;
//	box1offset.y=pA->collision.vecMin.y+box1s.y;
//	box1offset.z=pA->collision.vecMin.z+box1s.z;
	box1offset.x=(pA->collision.vecMin.x+box1s.x)*pA->position.vecScale.x;
	box1offset.y=(pA->collision.vecMin.y+box1s.y)*pA->position.vecScale.y;
	box1offset.z=(pA->collision.vecMin.z+box1s.z)*pA->position.vecScale.z;
	box1s.x *= pA->position.vecScale.x;
	box1s.y *= pA->position.vecScale.y;
	box1s.z *= pA->position.vecScale.z;

	// 20120404 IanM - Use common method of calculating the object rotation matrix
	D3DXMATRIX matARotation = CalcObjectRotationMatrix( pA );
/*
	D3DXMATRIX matARotation;
	if ( pA->position.bFreeFlightRotation )
	{
		matARotation = pA->position.matFreeFlightRotate;
	}
	else
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( pA->position.vecRotate.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( pA->position.vecRotate.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( pA->position.vecRotate.z ) );	// z rotation

		// build final rotation matrix
		matARotation = matRotateX * matRotateY * matRotateZ;
	}
*/
	D3DXVec3TransformCoord ( &box1offset, &box1offset,  &matARotation );
	box1p+=box1offset;

	// Box 2
// leefix - 250604 - u54- this and similar changes to allow gled objects to register collision
//	D3DXVECTOR3 box2p = pB->position.vecPosition;
	D3DXVECTOR3 box2p = D3DXVECTOR3(pB->position.matWorld._41,pB->position.matWorld._42,pB->position.matWorld._43);
	D3DXVECTOR3 box2s;
	box2s.x=(pB->collision.vecMax.x-pB->collision.vecMin.x)*0.5f;
	box2s.y=(pB->collision.vecMax.y-pB->collision.vecMin.y)*0.5f;
	box2s.z=(pB->collision.vecMax.z-pB->collision.vecMin.z)*0.5f;
	D3DXVECTOR3 box2offset;
	// lee - 140206 - u60 - you have to scale offset as well - otherwise the center of the box is wrong
//	box2offset.x=pB->collision.vecMin.x+box2s.x;
//	box2offset.y=pB->collision.vecMin.y+box2s.y;
//	box2offset.z=pB->collision.vecMin.z+box2s.z;
	box2offset.x=(pB->collision.vecMin.x+box2s.x)*pB->position.vecScale.x;
	box2offset.y=(pB->collision.vecMin.y+box2s.y)*pB->position.vecScale.y;
	box2offset.z=(pB->collision.vecMin.z+box2s.z)*pB->position.vecScale.z;
	box2s.x *= pB->position.vecScale.x;
	box2s.y *= pB->position.vecScale.y;
	box2s.z *= pB->position.vecScale.z;

	// 20120404 IanM - Use common method of calculating the object rotation matrix
	D3DXMATRIX matBRotation = CalcObjectRotationMatrix( pB );
/*
	D3DXMATRIX matBRotation;
	if ( pB->position.bFreeFlightRotation )
	{
		matBRotation = pB->position.matFreeFlightRotate;
	}
	else
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( pB->position.vecRotate.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( pB->position.vecRotate.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( pB->position.vecRotate.z ) );	// z rotation

		// build final rotation matrix
		matBRotation = matRotateX * matRotateY * matRotateZ;
	}
*/
	D3DXVec3TransformCoord ( &box2offset, &box2offset,  &matBRotation );
	box2p+=box2offset;

	// Perform Test
	return NewCol.TestBoxInBox(box1p, box1s, matARotation, box2p, box2s, matBRotation);
}

int CheckBoxPairSystemCollision( sObject* pA, sObject* pB )
{
	// Check if either box is rotated
	bool bYesRotated=true;

	// 20120404 IanM - Need to use rotated collision check if a pivot has been applied
	if(pA->position.bApplyPivot == false && pB->position.bApplyPivot == false)
		if(pA->collision.bFixedBoxCheck==true && pB->collision.bFixedBoxCheck==true)
			bYesRotated=false;

	// lee - 240306 - u6b4 - any sphere types use fixed box for non-rotating sliding
	if(pA->collision.eCollisionType==COLLISION_SPHERE || pA->collision.eCollisionType==COLLISION_SPHERE )
	{
		// leefix - 020308 - does not account for rotated elongated boxes (collision error with 90 degree rotated box(10,10,30))
		// bYesRotated=false;
		// so instead, use rotated algorythm but fix sphere to a non-rotated box so rotating does not get it stuck
		// inside collider (new sphere collision type detection inside code below)
	}

	// Use rotated-box check if rotated..
	if(bYesRotated==true)
	{
		// Multiple checks yield best sliding result (if any)
		D3DXVECTOR3 box1p = D3DXVECTOR3(pA->position.matWorld._41,pA->position.matWorld._42,pA->position.matWorld._43);
		int iInitialCollisionResult = CheckAdjustedAagainstB ( box1p, pA, pB );
		if(iInitialCollisionResult>0)
		{
			// COPIED FROM ADJUSTMENTAGAINSTB FUNCTION (should combine the work)
			D3DXMATRIX matRotateX, matRotateY, matRotateZ;
			// Box 1
			D3DXVECTOR3 box1s;
			box1s.x=(pA->collision.vecMax.x-pA->collision.vecMin.x)*0.5f;
			box1s.y=(pA->collision.vecMax.y-pA->collision.vecMin.y)*0.5f;
			box1s.z=(pA->collision.vecMax.z-pA->collision.vecMin.z)*0.5f;
			D3DXVECTOR3 box1offset;
			box1offset.x=pA->collision.vecMin.x+box1s.x;
			box1offset.y=pA->collision.vecMin.y+box1s.y;
			box1offset.z=pA->collision.vecMin.z+box1s.z;
			box1s.x *= pA->position.vecScale.x;
			box1s.y *= pA->position.vecScale.y;
			box1s.z *= pA->position.vecScale.z;
			D3DXMATRIX matARotation;
			if ( pA->collision.eCollisionType==COLLISION_SPHERE )
				D3DXMatrixIdentity ( &matARotation );
			else
			{
				// 20120404 IanM - Use common method of calculating the object rotation matrix
				matARotation = CalcObjectRotationMatrix( pA );
/*
				if ( pA->position.bFreeFlightRotation )
					matARotation = pA->position.matFreeFlightRotate;
				else
				{
					D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( pA->position.vecRotate.x ) );	// x rotation
					D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( pA->position.vecRotate.y ) );	// y rotation
					D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( pA->position.vecRotate.z ) );	// z rotation
					matARotation = matRotateX * matRotateY * matRotateZ;
				}
*/
			}
			D3DXVec3TransformCoord ( &box1offset, &box1offset,  &matARotation );
			box1p = pA->position.vecLastPosition;//forslidingcalc, need old A pos
			box1p+=box1offset;
			// Box 2
			D3DXVECTOR3 box2p = D3DXVECTOR3(pB->position.matWorld._41,pB->position.matWorld._42,pB->position.matWorld._43);
			D3DXVECTOR3 box2s;
			box2s.x=(pB->collision.vecMax.x-pB->collision.vecMin.x)*0.5f;
			box2s.y=(pB->collision.vecMax.y-pB->collision.vecMin.y)*0.5f;
			box2s.z=(pB->collision.vecMax.z-pB->collision.vecMin.z)*0.5f;
			D3DXVECTOR3 box2offset;
			box2offset.x=pB->collision.vecMin.x+box2s.x;
			box2offset.y=pB->collision.vecMin.y+box2s.y;
			box2offset.z=pB->collision.vecMin.z+box2s.z;
			box2s.x *= pB->position.vecScale.x;
			box2s.y *= pB->position.vecScale.y;
			box2s.z *= pB->position.vecScale.z;
			D3DXMATRIX matBRotation;
			if ( pB->collision.eCollisionType==COLLISION_SPHERE )
				D3DXMatrixIdentity ( &matBRotation );
			else
			{
				// 20120404 IanM - Use common method of calculating the object rotation matrix
				matBRotation = CalcObjectRotationMatrix( pB );
/*
				if ( pB->position.bFreeFlightRotation )
					matBRotation = pB->position.matFreeFlightRotate;
				else
				{
					D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( pB->position.vecRotate.x ) );	// x rotation
					D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( pB->position.vecRotate.y ) );	// y rotation
					D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( pB->position.vecRotate.z ) );	// z rotation
					matBRotation = matRotateX * matRotateY * matRotateZ;
				}
*/
			}
			D3DXVec3TransformCoord ( &box2offset, &box2offset,  &matBRotation );
			box2p+=box2offset;

			// inverse of box Bs rotation
			float fDeterminant;
			D3DXMATRIX matInvBoxBRot;
			D3DXMatrixInverse ( &matInvBoxBRot, &fDeterminant, &matBRotation );

			// rotate Start and End vectors of A using B
			D3DXVECTOR3 vecAS = pA->position.vecLastPosition;
			D3DXVECTOR3 vecAE = pA->position.vecPosition;
			D3DXVECTOR3 vecOldDirection = vecAE - vecAS;
			D3DXVec3TransformCoord ( &vecAS, &vecAS, &matInvBoxBRot );
			D3DXVec3TransformCoord ( &vecAE, &vecAE, &matInvBoxBRot );

			// work out closest point in A
			D3DXVECTOR3 vecCLOSEST;
			float fDistClosest=9999.0f;
			D3DXVECTOR3 vecB = pB->position.vecPosition;
			for ( int iPoint=0; iPoint<4; iPoint++ )
			{
				// center of each side
				D3DXVECTOR3 vP = D3DXVECTOR3(0,0,0);
				if ( iPoint==0 ) { vP.x-=box1s.x; vP.z-=box1s.z; }
				if ( iPoint==1 ) { vP.x-=box1s.x; vP.z+=box1s.z; }
				if ( iPoint==2 ) { vP.x+=box1s.x; vP.z-=box1s.z; }
				if ( iPoint==3 ) { vP.x+=box1s.x; vP.z+=box1s.z; }
				D3DXVec3TransformCoord ( &vP, &vP, &matARotation );
				vP+=box1p;

				// work out distance from start vector
				float fDx = (float)fabs ( vP.x - vecB.x );
				float fDz = (float)fabs ( vP.z - vecB.z );
				float fDist = (float)sqrt((fDx*fDx)+(fDz*fDz));
				if ( fDist < fDistClosest )
				{
					// record best corner
					fDistClosest = fDist;
					vecCLOSEST = vP;
				}
			}
			vecCLOSEST -= box2p;
			D3DXVec3TransformCoord ( &vecCLOSEST, &vecCLOSEST, &matInvBoxBRot );

			// sliding collision for X and Z sides
			int iSideBest = 0;
			float fSidePos, fDist;
			float fDistBest = 99999.0f;
			fSidePos = 0-box2s.x;
			fDist = fSidePos - vecCLOSEST.x;
			if ( fDist>=0.0f )
			{
				fDist = (float)fabs(fDist);
				if ( fDist<fDistBest ) { iSideBest=0; fDistBest=fDist; }
			}
			fSidePos = box2s.x;
			fDist = vecCLOSEST.x - fSidePos;
			if ( fDist>=0.0f )
			{
				fDist = (float)fabs(fDist);
				if ( fDist<fDistBest ) { iSideBest=1; fDistBest=fDist; }
			}
			fSidePos = 0-box2s.z;
			fDist = fSidePos - vecCLOSEST.z;
			if ( fDist>=0.0f )
			{
				fDist = (float)fabs(fDist);
				if ( fDist<fDistBest ) { iSideBest=4; fDistBest=fDist; }
			}
			fSidePos = box2s.z;
			fDist = vecCLOSEST.z - fSidePos;
			if ( fDist>=0.0f )
			{
				fDist = (float)fabs(fDist);
				if ( fDist<fDistBest ) { iSideBest=5; fDistBest=fDist; }
			}

			// bring Start and End vectors onto plane of the side
			if ( iSideBest==0 ) { vecAS.x = box2p.x-box2s.x; vecAE.x = box2p.x-box2s.x; }
			if ( iSideBest==1 ) { vecAS.x = box2p.x+box2s.x; vecAE.x = box2p.x+box2s.x; }
			if ( iSideBest==2 ) { vecAS.y = box2p.y-box2s.y; vecAE.y = box2p.y-box2s.y; }
			if ( iSideBest==3 ) { vecAS.y = box2p.y+box2s.y; vecAE.y = box2p.y+box2s.y; }
			if ( iSideBest==4 ) { vecAS.z = box2p.z-box2s.z; vecAE.z = box2p.z-box2s.z; }
			if ( iSideBest==5 ) { vecAS.z = box2p.z+box2s.z; vecAE.z = box2p.z+box2s.z; }

			// work out direction based on new start and end
			D3DXVECTOR3 vecAdjustedDirection = vecAE - vecAS;

			// orient direction to box Bs rotation for new sliding data
			D3DXVec3TransformCoord ( &vecAdjustedDirection, &vecAdjustedDirection, &matBRotation );

			// if a good collision free slide, use it, else restore to old position
			box1p = pA->position.vecPosition - (vecOldDirection - vecAdjustedDirection);
			if ( CheckAdjustedAagainstB ( box1p, pA, pB )==0 )
			{
				// sliding adjustment worked fine
				gvLatestObjectCollisionResult = vecOldDirection - vecAdjustedDirection;
			}
			else
			{
				// movement cannot proceed, must restore
				gvLatestObjectCollisionResult = vecOldDirection;
			}
		}
		return iInitialCollisionResult;
	}
	else
		return CheckUnRotatedBoxes( pA, pB );
}

int DBV1_ReturnModelBoxSystemOverlapResult( sObject* pA, sObject* pB )
{
	// Clear slide data
	gvLatestObjectCollisionResult.x=0;
	gvLatestObjectCollisionResult.y=0;
	gvLatestObjectCollisionResult.z=0;

	// perform A V B check
	return CheckBoxPairSystemCollision(pA, pB);
}

//
// DBV1 Legacy Commands
//

void DBV1_MakeObjectCollisionBox( sObject* pObj, float x1, float y1, float z1, float x2, float y2, float z2, int iRotationCollisionMode )
{
	// Make collision box for object
	pObj->collision.vecMin.x = x1;
	pObj->collision.vecMin.y = y1;
	pObj->collision.vecMin.z = z1;
	pObj->collision.vecMax.x = x2;
	pObj->collision.vecMax.y = y2;
	pObj->collision.vecMax.z = z2;
	if ( iRotationCollisionMode==1 )
	{
		// Use rotated box collision code, produces no extra data
		pObj->collision.bBoundProduceResult = false;
		pObj->collision.bFixedBoxCheck = false;
	}
	else
	{
		// Use standard fixed box check, produces return data
		pObj->collision.bBoundProduceResult = true;
		pObj->collision.bFixedBoxCheck = true;
	}
}

void DBV1_DeleteObjectCollisionBox( sObject* pObj )
{
	// Disable collision box for object
	pObj->collision.bUseBoxCollision = false;
}

float DBV1_ObjectCollisionRadius( sObject* pObj )
{
	return pObj->collision.fRadius;
}

float DBV1_ObjectCollisionCenterX( sObject* pObj )
{
	return pObj->collision.vecCentre.x;
}

float DBV1_ObjectCollisionCenterY( sObject* pObj )
{
	return pObj->collision.vecCentre.y;
}

float DBV1_ObjectCollisionCenterZ( sObject* pObj )
{
	return pObj->collision.vecCentre.z;
}

float DBV1_ObjectCollisionXF(void)
{
	return gvLatestObjectCollisionResult.x;
}

float DBV1_ObjectCollisionYF(void)
{
	return gvLatestObjectCollisionResult.y;
}

float DBV1_ObjectCollisionZF(void)
{
	return gvLatestObjectCollisionResult.z;
}

void DBV1_MakeStaticCollisionBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
	gPolyPool.AddColBox(x1,y1,z1,x2,y2,z2);
}

void DBV1_DeleteAllStaticCollisionBoxes(void)
{
	gPolyPool.ReleasePool();
}

int DBV1_StaticCollisionBoxHit(	float x1, float y1, float z1, float x2, float y2, float z2,
								float Bx1, float By1, float Bz1, float Bx2, float By2, float Bz2 )
{
	return CheckCollisionPool(x1, y1, z1, x2, y2, z2, Bx1, By1, Bz1, Bx2, By2, Bz2, &gvLatestStaticCollisionResult);
}

float DBV1_StaticCollisionXF(void)
{
	return gvLatestStaticCollisionResult.x;
}

float DBV1_StaticCollisionYF(void)
{
	return gvLatestStaticCollisionResult.y;
}

float DBV1_StaticCollisionZF(void)
{
	return gvLatestStaticCollisionResult.z;
}

int DBV1_StaticLineOfSightHit(	float sx, float sy, float sz, float dx, float dy, float dz, float width, float acc )
{
	int hitflag=0;
	if(CheckCollisionPoolWithLine(	sx, sy, sz, dx, dy, dz, width, acc, &gvLightOfSight )>0)
	{
		hitflag=1;
	}
	return hitflag;
}

float DBV1_LineOfSightXF(void)
{
	return gvLightOfSight.x;
}

float DBV1_LineOfSightYF(void)
{
	return gvLightOfSight.y;
}

float DBV1_LineOfSightZF(void)
{
	return gvLightOfSight.z;
}
