#include "cFrustum.h"


//D3DXPLANE m_Planes [ 6 ];
extern LPDIRECT3DDEVICE8		m_pD3D;

/*
void UpdateCullInfo( CULLINFO* pCullInfo, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
    D3DXMATRIX mat;

    D3DXMatrixMultiply( &mat, pMatView, pMatProj );
    D3DXMatrixInverse( &mat, NULL, &mat );

    pCullInfo->vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    pCullInfo->vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    pCullInfo->vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    pCullInfo->vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    pCullInfo->vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
    pCullInfo->vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
    pCullInfo->vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
    pCullInfo->vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

    for( INT i = 0; i < 8; i++ )
        D3DXVec3TransformCoord( &pCullInfo->vecFrustum[i], &pCullInfo->vecFrustum[i], &mat );

    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[0], &pCullInfo->vecFrustum[0], 
        &pCullInfo->vecFrustum[1], &pCullInfo->vecFrustum[2] ); // Near
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[1], &pCullInfo->vecFrustum[6], 
        &pCullInfo->vecFrustum[7], &pCullInfo->vecFrustum[5] ); // Far
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[2], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[6], &pCullInfo->vecFrustum[4] ); // Left
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[3], &pCullInfo->vecFrustum[7], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[5] ); // Right
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[4], &pCullInfo->vecFrustum[2], 
        &pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[6] ); // Top
    D3DXPlaneFromPoints( &pCullInfo->planeFrustum[5], &pCullInfo->vecFrustum[1], 
        &pCullInfo->vecFrustum[0], &pCullInfo->vecFrustum[4] ); // Bottom
}
*/

//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).  
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane, 
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return 
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the 
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
/*
CULLSTATE CullObject( CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, 
                      D3DXPLANE* pPlaneBounds )
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );

    // Check boundary vertices against all 6 frustum planes, 
    // and store result (1 if outside) in a bitfield
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
			double size =	pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
							pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
							pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
							pCullInfo->planeFrustum[iPlane].d;

            if ( size < 0 )
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        // If any point is inside all 6 frustum planes, it is inside
        // the frustum, so the object must be rendered.
        if( bOutside[iPoint] == 0 )
            return CS_INSIDE;
    }

    // If all points are outside any single frustum plane, the object is
    // outside the frustum
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
        return CS_OUTSIDE;
    }

    // Now see if any of the frustum edges penetrate any of the faces of
    // the bounding box
    D3DXVECTOR3 edge[12][2] = 
    {
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[1], // front bottom
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], // front top
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], // front left
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], // front right
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], // back bottom
        pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], // back top
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], // back left
        pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], // back right
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], // left bottom
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], // left top
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[5], // right bottom
        pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], // right top
    };
    D3DXVECTOR3 face[6][4] =
    {
        pVecBounds[0], pVecBounds[2], pVecBounds[3], pVecBounds[1], // front
        pVecBounds[4], pVecBounds[5], pVecBounds[7], pVecBounds[6], // back
        pVecBounds[0], pVecBounds[4], pVecBounds[6], pVecBounds[2], // left
        pVecBounds[1], pVecBounds[3], pVecBounds[7], pVecBounds[5], // right
        pVecBounds[2], pVecBounds[6], pVecBounds[7], pVecBounds[3], // top
        pVecBounds[0], pVecBounds[4], pVecBounds[5], pVecBounds[1], // bottom
    };
    D3DXVECTOR3* pEdge;
    D3DXVECTOR3* pFace;
    pEdge = &edge[0][0];
    for( INT iEdge = 0; iEdge < 12; iEdge++ )
    {
        pFace = &face[0][0];
        for( INT iFace = 0; iFace < 6; iFace++ )
        {
            if( EdgeIntersectsFace( pEdge, pFace, &pPlaneBounds[iFace] ) )
            {
                return CS_INSIDE_SLOW;
            }
            pFace += 4;
        }
        pEdge += 2;
    }

    // Now see if any of the bounding box edges penetrate any of the faces of
    // the frustum
    D3DXVECTOR3 edge2[12][2] = 
    {
        pVecBounds[0], pVecBounds[1], // front bottom
        pVecBounds[2], pVecBounds[3], // front top
        pVecBounds[0], pVecBounds[2], // front left
        pVecBounds[1], pVecBounds[3], // front right
        pVecBounds[4], pVecBounds[5], // back bottom
        pVecBounds[6], pVecBounds[7], // back top
        pVecBounds[4], pVecBounds[6], // back left
        pVecBounds[5], pVecBounds[7], // back right
        pVecBounds[0], pVecBounds[4], // left bottom
        pVecBounds[2], pVecBounds[6], // left top
        pVecBounds[1], pVecBounds[5], // right bottom
        pVecBounds[3], pVecBounds[7], // right top
    };
    D3DXVECTOR3 face2[6][4] =
    {
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[1], // front
        pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[6], // back
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[2], // left
        pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[5], // right
        pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[3], // top
        pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[1], // bottom
    };
    pEdge = &edge2[0][0];
    for( iEdge = 0; iEdge < 12; iEdge++ )
    {
        pFace = &face2[0][0];
        for( INT iFace = 0; iFace < 6; iFace++ )
        {
            if( EdgeIntersectsFace( pEdge, pFace, &pCullInfo->planeFrustum[iFace] ) )
            {
                return CS_INSIDE_SLOW;
            }
            pFace += 4;
        }
        pEdge += 2;
    }

	// frustrum may be contained it whopping great bounc box(sky for example)
    return CS_INSIDE_SLOW;

	
	
}
*/

//-----------------------------------------------------------------------------
// Name: EdgeIntersectsFace()
// Desc: Determine if the edge bounded by the two vectors in pEdges intersects
//       the quadrilateral described by the four vectors in pFacePoints.  
//       Note: pPlane could be derived from pFacePoints using 
//       D3DXPlaneFromPoints, but it is precomputed in advance for greater
//       speed.
//-----------------------------------------------------------------------------
/*
BOOL EdgeIntersectsFace( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, 
                         D3DXPLANE* pPlane )
{
    // If both edge points are on the same side of the plane, the edge does
    // not intersect the face
    FLOAT fDist1;
    FLOAT fDist2;
    fDist1 = pPlane->a * pEdges[0].x + pPlane->b * pEdges[0].y +
             pPlane->c * pEdges[0].z + pPlane->d;
    fDist2 = pPlane->a * pEdges[1].x + pPlane->b * pEdges[1].y +
             pPlane->c * pEdges[1].z + pPlane->d;
    if( fDist1 > 0 && fDist2 > 0 ||
        fDist1 < 0 && fDist2 < 0 )
    {
        return FALSE;
    }

    // Find point of intersection between edge and face plane (if they're
    // parallel, edge does not intersect face and D3DXPlaneIntersectLine 
    // returns NULL)
    D3DXVECTOR3 ptIntersection;
    if( NULL == D3DXPlaneIntersectLine( &ptIntersection, pPlane, &pEdges[0], &pEdges[1] ) )
        return FALSE;

    // Project onto a 2D plane to make the pt-in-poly test easier
    FLOAT fAbsA = (pPlane->a > 0 ? pPlane->a : -pPlane->a);
    FLOAT fAbsB = (pPlane->b > 0 ? pPlane->b : -pPlane->b);
    FLOAT fAbsC = (pPlane->c > 0 ? pPlane->c : -pPlane->c);
    D3DXVECTOR2 facePoints[4];
    D3DXVECTOR2 point;
    if( fAbsA > fAbsB && fAbsA > fAbsC )
    {
        // Plane is mainly pointing along X axis, so use Y and Z
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].y;
            facePoints[i].y = pFacePoints[i].z;
        }
        point.x = ptIntersection.y;
        point.y = ptIntersection.z;
    }
    else if( fAbsB > fAbsA && fAbsB > fAbsC )
    {
        // Plane is mainly pointing along Y axis, so use X and Z
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].x;
            facePoints[i].y = pFacePoints[i].z;
        }
        point.x = ptIntersection.x;
        point.y = ptIntersection.z;
    }
    else
    {
        // Plane is mainly pointing along Z axis, so use X and Y
        for( INT i = 0; i < 4; i++)
        {
            facePoints[i].x = pFacePoints[i].x;
            facePoints[i].y = pFacePoints[i].y;
        }
        point.x = ptIntersection.x;
        point.y = ptIntersection.y;
    }

    // If point is on the outside of any of the face edges, it is
    // outside the face.  
    // We can do this by taking the determinant of the following matrix:
    // | x0 y0 1 |
    // | x1 y1 1 |
    // | x2 y2 1 |
    // where (x0,y0) and (x1,y1) are points on the face edge and (x2,y2) 
    // is our test point.  If this value is positive, the test point is
    // "to the left" of the line.  To determine whether a point needs to
    // be "to the right" or "to the left" of the four lines to qualify as
    // inside the face, we need to see if the faces are specified in 
    // clockwise or counter-clockwise order (it could be either, since the
    // edge could be penetrating from either side).  To determine this, we
    // do the same test to see if the third point is "to the right" or 
    // "to the left" of the line formed by the first two points.
    // See http://forum.swarthmore.edu/dr.math/problems/scott5.31.96.html
    FLOAT x0, x1, x2, y0, y1, y2;
    x0 = facePoints[0].x;
    y0 = facePoints[0].y;
    x1 = facePoints[1].x;
    y1 = facePoints[1].y;
    x2 = facePoints[2].x;
    y2 = facePoints[2].y;
    BOOL bClockwise = FALSE;
    if( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 < 0 )
        bClockwise = TRUE;
    x2 = point.x;
    y2 = point.y;
    for( INT i = 0; i < 4; i++ )
    {
        x0 = facePoints[i].x;
        y0 = facePoints[i].y;
        if( i < 3 )
        {
            x1 = facePoints[i+1].x;
            y1 = facePoints[i+1].y;
        }
        else
        {
            x1 = facePoints[0].x;
            y1 = facePoints[0].y;
        }
        if( ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 > 0 ) == bClockwise )
            return FALSE;
    }

    // If we get here, the point is inside all four face edges, 
    // so it's inside the face.
    return TRUE;
}
*/

/*
void SetupFrustum ( float fZDistance )
{
	// setup the planes for the viewing frustum

	// variable declarations
	D3DXMATRIX	Matrix,
				matView,
				matProj;

	float		ZMin,
				Q;

	// check d3d is ok
	if ( !m_pD3D )
		return;

	// get the projection matrix
	m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProj );

	if ( fZDistance != 0.0f )
	{
		// calculate new projection matrix based on distance provided
		ZMin        = -matProj._43 / matProj._33;
		Q           = fZDistance / ( fZDistance - ZMin );
		matProj._33 = Q;
		matProj._43 = -Q * ZMin;
	}

	// get the view matrix
	m_pD3D->GetTransform ( D3DTS_VIEW, &matView );

	// multiply with the projection matrix
	D3DXMatrixMultiply ( &Matrix, &matView, &matProj );

	// and now calculate the planes

	// near plane
	m_Planes [ 0 ].a = Matrix._14 + Matrix._13;
	m_Planes [ 0 ].b = Matrix._24 + Matrix._23;
	m_Planes [ 0 ].c = Matrix._34 + Matrix._33;
	m_Planes [ 0 ].d = Matrix._44 + Matrix._43;
	D3DXPlaneNormalize ( &m_Planes [ 0 ], &m_Planes [ 0 ] );

	// far plane
	m_Planes [ 1 ].a = Matrix._14 - Matrix._13;
	m_Planes [ 1 ].b = Matrix._24 - Matrix._23;
	m_Planes [ 1 ].c = Matrix._34 - Matrix._33;
	m_Planes [ 1 ].d = Matrix._44 - Matrix._43;
	D3DXPlaneNormalize ( &m_Planes [ 1 ], &m_Planes [ 1 ] );

	// left plane
	m_Planes [ 2 ].a = Matrix._14 + Matrix._11;
	m_Planes [ 2 ].b = Matrix._24 + Matrix._21;
	m_Planes [ 2 ].c = Matrix._34 + Matrix._31;
	m_Planes [ 2 ].d = Matrix._44 + Matrix._41;
	D3DXPlaneNormalize ( &m_Planes [ 2 ], &m_Planes [ 2 ] );

	// right plane
	m_Planes [ 3 ].a = Matrix._14 - Matrix._11;
	m_Planes [ 3 ].b = Matrix._24 - Matrix._21;
	m_Planes [ 3 ].c = Matrix._34 - Matrix._31;
	m_Planes [ 3 ].d = Matrix._44 - Matrix._41;
	D3DXPlaneNormalize ( &m_Planes [ 3 ], &m_Planes [ 3 ] );

	// top plane
	m_Planes [ 4 ].a = Matrix._14 - Matrix._12;
	m_Planes [ 4 ].b = Matrix._24 - Matrix._22;
	m_Planes [ 4 ].c = Matrix._34 - Matrix._32;
	m_Planes [ 4 ].d = Matrix._44 - Matrix._42;
	D3DXPlaneNormalize ( &m_Planes [ 4 ], &m_Planes [ 4 ] );

	// bottom plane
	m_Planes [ 5 ].a = Matrix._14 + Matrix._12;
	m_Planes [ 5 ].b = Matrix._24 + Matrix._22;
	m_Planes [ 5 ].c = Matrix._34 + Matrix._32;
	m_Planes [ 5 ].d = Matrix._44 + Matrix._42;
	D3DXPlaneNormalize ( &m_Planes [ 5 ], &m_Planes [ 5 ] );
}

bool CheckPoint ( float fX, float fY, float fZ )
{
	// make sure point is in frustum

	for ( int iTemp = 0; iTemp < 6; iTemp++ ) 
	{
		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX, fY, fZ ) ) < 0.0f )
			return false;
	}

	return true;
}

bool CheckCube ( float fX, float fY, float fZ, float fSize )
{
	// make sure at least one point is completely in frustum

	for ( int iTemp = 0; iTemp < 6; iTemp++ )
	{
		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fSize, fY - fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fSize, fY - fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fSize, fY + fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fSize, fY + fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fSize, fY - fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fSize, fY - fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fSize, fY + fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fSize, fY + fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		return false;
	}

  return true;
}

bool CheckRectangle ( float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize )
{
	// make sure at least one point is in frustum
	for ( int iTemp = 0; iTemp < 6; iTemp++ )
	{
		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fXSize, fY - fYSize, fZ - fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fXSize, fY - fYSize, fZ - fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fXSize, fY + fYSize, fZ - fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fXSize, fY + fYSize, fZ - fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fXSize, fY - fYSize, fZ + fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fXSize, fY - fYSize, fZ + fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX - fXSize, fY + fYSize, fZ + fZSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX + fXSize, fY + fYSize, fZ + fZSize ) ) >= 0.0f )
			continue;

		return false;
	}

	return true;
}

bool CheckSphere ( float fX, float fY, float fZ, float fRadius )
{
	// make sure radius is in frustum
	for ( int iTemp = 0; iTemp < 6; iTemp++ )
	{
		if ( D3DXPlaneDotCoord ( &m_Planes [ iTemp ], &D3DXVECTOR3 ( fX, fY, fZ ) ) < -fRadius )
			return false;
	}

	return true;
}
*/