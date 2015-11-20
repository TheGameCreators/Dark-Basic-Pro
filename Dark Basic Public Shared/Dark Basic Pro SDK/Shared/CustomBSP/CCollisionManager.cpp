#include "CCollisionManager.h"

const float			EPSILON	= 0.01f;
//const float			EPSILON	= 0.001f;

struct PLANE1
{
	VECTOR3 PointOnPlane;
	VECTOR3 Normal;
};


CCollisionManager::CCollisionManager ( void )
{
	m_gravity       = VECTOR3 (  0.0f, -1.00f,  0.0f );
	//m_gravity       = VECTOR3 (  0.0f, 0.0f,  0.0f );
	m_friction      = VECTOR3 (  0.0f,   0.0f,  0.0f );
	m_ellipRadius   = VECTOR3 ( 1.0f,    2.0f, 1.0f );
	m_eyePoint      = VECTOR3 (  0.0f,   1.0f,  0.0f );   
	//m_eyePoint      = VECTOR3 (  0.0f,   0.0095f,  0.0f );
	
	m_collisionType            = COLLIDE_AND_SLIDE;
	m_enableCollisionDetection = true;
}


CCollisionManager::~CCollisionManager ( void )
{

}

VECTOR3 CCollisionManager::collisionDetection ( VECTOR3& origin, VECTOR3& velocityVector, float fdeltaTime )
{
	
	m_numPolysChecked = 0;

	//if ( !m_enableCollisionDetection )
	//	return ( origin + velocityVector );

	VECTOR3 inverseRadius = 1.0 / m_ellipRadius;

	// at this point, we'll scale our inputs to the collision routine.
	VECTOR3 pos     = ( origin - m_eyePoint * m_ellipRadius ) * inverseRadius;
	VECTOR3 vel     = velocityVector * inverseRadius;
	VECTOR3 gravity = m_gravity * inverseRadius;

	pos = collideAndSlide ( pos, vel );     // apply the velocity vector
	//pos = collideAndSlide ( pos, gravity ); // apply gravity

	return ( pos * m_ellipRadius + m_eyePoint * m_ellipRadius );
}

int ClassifyPoint ( PLANE1* plane, VECTOR3* pos )
{
	VECTOR3* vec1	    = ( VECTOR3* ) &plane->PointOnPlane;
	VECTOR3  Direction  = ( *vec1 ) - ( *pos );
	float	 result	    = D3DXVec3Dot ( &Direction, &plane->Normal );

	if ( result < -EPSILON )
		return CP_FRONT;

	if ( result >  EPSILON )
		return CP_BACK;

	return CP_ONPLANE;
}

float IntersectRayPlane ( VECTOR3 &rayOrigin, VECTOR3 &rayVector, PLANE1 &plane )
{
	float d = -D3DXVec3Dot( &plane.Normal, &plane.PointOnPlane );

	float numerator   = D3DXVec3Dot ( &plane.Normal, &rayOrigin ) + d;
	float denominator = D3DXVec3Dot ( &plane.Normal, &rayVector );
	
	// check if the planes normal is orthogonal to the vector, cannot intersect
	if ( fabs ( denominator ) < EPSILON )
		return -1;

	return -( numerator / denominator );
}

bool CheckPointInPolygon ( VECTOR3 &point, POLYGON &poly )
{
	// determine if the given point is within the polygon

	for ( UINT ii = 0; ii < poly.NumberOfVertices; ++ii )
	{
		VECTOR3 v = point - *( VECTOR3* ) &poly.VertexList [ ii ];

		VECTOR3 e = *( VECTOR3* ) &poly.VertexList [ ( ii + 1 ) % poly.NumberOfVertices ] -
			        *( VECTOR3* ) &poly.VertexList [ ii ];

		VECTOR3 n;
		D3DXVec3Cross ( &n, &e, &v );

		// if the vector n is not pointing in the same direction as the polygon's normal then
		// the point is outside of the polygon
		if ( D3DXVec3Dot ( &n, &poly.Normal ) < EPSILON )
			return false;
	}

	return true;
}

VECTOR3 ClosestPointOnPolygon ( VECTOR3 &point, POLYGON &poly )
{
	// the point is not within the polygon so determine the closest point along the polygon's perimeter
	VECTOR3 closestSoFar;
	float   closestLen;
	bool    found = false;

	for ( UINT ii = 0; ii < poly.NumberOfVertices; ++ii )
	{
		VECTOR3 c = point - *( VECTOR3* ) &poly.VertexList [ ii ];
		VECTOR3 v = *( VECTOR3* ) &poly.VertexList [ ( ii + 1 ) % poly.NumberOfVertices ] - 
			        *( VECTOR3* ) &poly.VertexList [ ii ];

		float d = D3DXVec3Length ( &v );
		D3DXVec3Normalize ( &v, &v );

		float t = D3DXVec3Dot ( &c, &v );

		VECTOR3 closest;

		if ( t < 0 )
		{
			closest = *( VECTOR3* ) &poly.VertexList [ ii ];
		}
		else if ( t > d )
		{
			closest = *( VECTOR3* ) &poly.VertexList [ ( ii + 1 ) % poly.NumberOfVertices ];
		}
		else
		{
			closest = *( VECTOR3* ) &poly.VertexList [ ii ] + v * t;
		}

		VECTOR3 distVector = point - closest;

		if ( !found || D3DXVec3Length ( &distVector ) < closestLen )
		{
			closestSoFar = closest;
			closestLen   = D3DXVec3Length ( &distVector );
			found        = true;
		}
	}

	return closestSoFar;
}

float IntersectRaySphere ( VECTOR3 &rayOrigin, VECTOR3 &rayVector, VECTOR3 sphereOrigin, float sphereRadius )
{
	VECTOR3 Q = sphereOrigin - rayOrigin;

	float c = D3DXVec3Length ( &Q );
	float v = D3DXVec3Dot ( &Q, &rayVector );
	float d = sphereRadius * sphereRadius - ( c * c  - v * v );

	if ( d < 0.0 )
		return -1.0;

	return float ( v - sqrt ( d ) );
}

bool CheckPointInSphere ( VECTOR3 &point, VECTOR3 &sphereOrigin, float sphereRadius )
{
	VECTOR3 p = point - sphereOrigin;

	float d = D3DXVec3Length ( &p );

	if ( d < sphereRadius )
		return true;
	else
		return false;
}

VECTOR3 CCollisionManager::collideAndSlide ( VECTOR3 origin, VECTOR3 velocityVector )
{
	VECTOR3 inverseRadius = 1.0 / m_ellipRadius;	
	POLYGON scaledPoly;

	scaledPoly.TextureIndex  = 0;
	scaledPoly.VertexList = NULL;

	static int maxNumRecursions = 5;
	
	for ( int numRecursions = 0; numRecursions < maxNumRecursions; ++numRecursions )
	{
		// determine how far we need to go.
		double distanceToTravel = D3DXVec3Length ( &velocityVector );

		// do we need to bother.
		if ( distanceToTravel < EPSILON )
			break;
		
		// determine which polygons we might collide with
		POLYGON *polys = getAABBColliders ( origin,velocityVector );

		// determine the nearest collider from the list of potential colliders.
		bool			stuck                            = false;
		bool			collisionFound                   = false;
		float			nearestDistance                  = -1.0;
		VECTOR3			nearestIntersectionPoint;
		VECTOR3			nearestPolygonIntersectionPoint;
		int	nearestPolygonClassification     = CP_FRONT;
		VECTOR3			normVelocityVector;

		D3DXVec3Normalize ( &normVelocityVector, &velocityVector );

		for ( POLYGON *p = polys; p; p = p->Next )
		{
			// if this is a back facing plane we can ignore it since 
			// we are headed in the opposite direction
			if ( D3DXVec3Dot ( &normVelocityVector, &p->Normal ) > 0 )
				continue;

			++m_numPolysChecked;

			// get the polygons plane definition.
			copyAndScalePolygon ( p, &scaledPoly, inverseRadius );

			PLANE1 polyPlane;
			polyPlane.Normal       = scaledPoly.Normal;
			polyPlane.PointOnPlane = *( VECTOR3* ) &scaledPoly.VertexList [ 0 ];

			VECTOR3 sphereIntersection = origin - polyPlane.Normal;

			// classify the sphere intersection point to determine if the ellipsoid spans the plane
			int c = ClassifyPoint ( &polyPlane, &sphereIntersection );
			
			VECTOR3 planeIntersectionPoint;
			double distanceToPlaneIntersection;

			// find the plane intersection point
			if ( c == CP_BACK )
			{
				// the plane is embedded in the ellipsoid

				// find the plane intersection point by shooting a ray from 
				// the sphere intersection point along the planes normal
				distanceToPlaneIntersection = IntersectRayPlane ( sphereIntersection, polyPlane.Normal, polyPlane );
				planeIntersectionPoint      = sphereIntersection + ( float ) distanceToPlaneIntersection * polyPlane.Normal;
			}
			else 
			{
				// shoot a ray along the velocity vector
				distanceToPlaneIntersection = IntersectRayPlane ( sphereIntersection, normVelocityVector, polyPlane );
				planeIntersectionPoint      = sphereIntersection + ( float ) distanceToPlaneIntersection * normVelocityVector;
			}
			
			// find the polygon intersection point
			VECTOR3 polygonIntersectionPoint     = planeIntersectionPoint;
			double  distanceToSphereIntersection = distanceToPlaneIntersection;

			if ( !CheckPointInPolygon ( planeIntersectionPoint, scaledPoly ) )
			{
				polygonIntersectionPoint = ClosestPointOnPolygon ( planeIntersectionPoint, scaledPoly );
				
				VECTOR3 temp = -normVelocityVector;
				distanceToSphereIntersection = IntersectRaySphere ( polygonIntersectionPoint, temp, origin, 1.0 );

				if ( distanceToSphereIntersection > 0 )
				{
					sphereIntersection = polygonIntersectionPoint + ( float ) distanceToSphereIntersection * temp;
				}
				else 
				{
					temp = -temp;
					distanceToSphereIntersection = IntersectRaySphere ( polygonIntersectionPoint, temp, origin, 1.0 );

					if ( distanceToSphereIntersection > 0 )
					{
						sphereIntersection = polygonIntersectionPoint + ( float ) distanceToSphereIntersection * temp;
					}
					else 
					{
						// the ray does not intersect the sphere thus there is not a collision
						continue;
					}
				}
			}

			bool currPolyStuck;  // is the current polygon stuck

			// here we do the error checking to see if we got ourself stuck
			if ( CheckPointInSphere ( polygonIntersectionPoint, origin, 1.0 ) )
				currPolyStuck = true;
			else
				currPolyStuck = false;

			if ( stuck && !currPolyStuck )
				continue;
			else if ( !stuck && currPolyStuck )
			{
				// Give precedence the polygons that are stuck within our sphere
				nearestDistance                 = ( float ) distanceToSphereIntersection;
				nearestIntersectionPoint        = sphereIntersection;
				nearestPolygonIntersectionPoint = polygonIntersectionPoint;
				nearestPolygonClassification    = c;
				stuck                           = currPolyStuck;
				collisionFound                  = true;
			}
			else 
			{
				// now we might update the collision data if we hit something
				if ( distanceToSphereIntersection >= 0 && distanceToSphereIntersection <= distanceToTravel )
				{
					if ( !collisionFound || distanceToSphereIntersection < nearestDistance )
					{
						nearestDistance                 = (float)distanceToSphereIntersection;
						nearestIntersectionPoint        = sphereIntersection;
						nearestPolygonIntersectionPoint = polygonIntersectionPoint;
						nearestPolygonClassification    = c;
						stuck                           = currPolyStuck;
						collisionFound                  = true;
					}
				}
			}
		}

		if ( !collisionFound )
		{
			origin += velocityVector;
			break;
		}

		// move to the nearest collision
		VECTOR3 newSourcePoint;
		
		if ( nearestDistance >= EPSILON )
		{
			if ( nearestPolygonClassification == CP_BACK )
				newSourcePoint = origin + normVelocityVector * ( -( nearestDistance + EPSILON ) );
			else 
				newSourcePoint = origin + normVelocityVector * ( nearestDistance - EPSILON );
		}
		else 
			newSourcePoint = origin;
		
		// if this is the last time through the loop, don't waste
		// time calculating the new sliding plane
		if ( numRecursions == maxNumRecursions - 1 ) 
		{
			origin = newSourcePoint;
			break;
		}

		PLANE1 slidePlane;

		slidePlane.Normal       = newSourcePoint - nearestPolygonIntersectionPoint;
		slidePlane.PointOnPlane = nearestPolygonIntersectionPoint;
		D3DXVec3Normalize ( &slidePlane.Normal, &slidePlane.Normal );

		VECTOR3 distinationPoint = origin + velocityVector;
		float l = IntersectRayPlane ( distinationPoint, slidePlane.Normal, slidePlane );

		// we can now calculate a new destination point on the sliding plane
		VECTOR3 newDestinationPoint = distinationPoint + l * slidePlane.Normal;

		// generate the new slide vector
		velocityVector = newDestinationPoint - nearestPolygonIntersectionPoint;
		origin         = newSourcePoint;
	}

	delete [ ] scaledPoly.VertexList;
	return origin;
}

bool BoundingBoxInBoundingBox ( BOUNDINGBOX &box1, BOUNDINGBOX &box2 )
{
	if ( box2.BoxMax.x < box1.BoxMin.x || box2.BoxMin.x > box1.BoxMax.x )
	{
		return false;
	}

	if ( box2.BoxMax.y < box1.BoxMin.y || box2.BoxMin.y > box1.BoxMax.y )
	{
		return false;
	}

	if ( box2.BoxMax.z < box1.BoxMin.z || box2.BoxMin.z > box1.BoxMax.z )
	{
		return false;
	}

	return true;
}

int ClassifyPolygon ( PLANE* plane, POLYGON* poly )
{
	int Infront = 0; 
	int Behind  = 0; 
	int OnPlane = 0;

	for ( int ii = 0; ii < poly->NumberOfVertices; ++ii )
	{
		VECTOR3 Direction;
		Direction.x = plane->PointOnPlane.x - poly->VertexList [ ii ].x;
		Direction.y = plane->PointOnPlane.y - poly->VertexList [ ii ].y;
		Direction.z = plane->PointOnPlane.z - poly->VertexList [ ii ].z;
		
		float result = D3DXVec3Dot ( &Direction, &plane->Normal );

		if ( result > EPSILON )
		{
			Behind++;
		} 
		else if ( result < -EPSILON )
		{
			Infront++;
		} 
		else 
		{
			OnPlane++;
			Infront++;
			Behind++;
		} 
	} 
	
	if ( OnPlane == poly->NumberOfVertices ) return CP_ONPLANE;
	if ( Behind  == poly->NumberOfVertices ) return CP_BACK;
	if ( Infront == poly->NumberOfVertices ) return CP_FRONT;
	
	return CP_SPANNING;
}

bool PolygonInBoundingBox ( POLYGON &poly, BOUNDINGBOX box )
{
	PLANE p;

	p.Normal       = VECTOR3 ( 0, 1, 0 );
	p.PointOnPlane = VECTOR3 ( 0, box.BoxMax.y, 0 );
	
	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}
	
	p.Normal       = VECTOR3 ( 0, -1, 0 );
	p.PointOnPlane = VECTOR3 ( 0, box.BoxMin.y, 0 );
	
	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}

	p.Normal       = VECTOR3 ( 1, 0, 0 );
	p.PointOnPlane = VECTOR3 ( box.BoxMax.x, 0, 0 );
	
	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}

	p.Normal       = VECTOR3 ( -1, 0, 0 );
	p.PointOnPlane = VECTOR3 ( box.BoxMin.x, 0, 0 );
	
	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}

	p.Normal       = VECTOR3 ( 0, 0, 1 );
	p.PointOnPlane = VECTOR3 ( 0, 0, box.BoxMax.z );

	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}

	p.Normal       = VECTOR3 ( 0, 0, -1 );
	p.PointOnPlane = VECTOR3 ( 0, 0, box.BoxMin.z );
	
	if ( ClassifyPolygon ( &p, &poly ) == CP_FRONT )
	{
		return false;
	}

	return true;
}

int ClassifyPoint ( D3DXVECTOR3* pos, PLANE* Plane ) 
{
	float			result;
	D3DXVECTOR3*	vec1		= &Plane->PointOnPlane;
	D3DXVECTOR3		Direction	= ( *vec1 ) - ( *pos );

	result = D3DXVec3Dot ( &Direction, &Plane->Normal );

	if ( result < -g_EPSILON )
		return CP_FRONT;

	if ( result > g_EPSILON ) 
		return CP_BACK;

	return CP_ONPLANE;
}

POLYGON* CCollisionManager::getAABBColliders ( VECTOR3 origin, VECTOR3 velocityVector )
{
	origin         *= m_ellipRadius;
	velocityVector *= m_ellipRadius;
	
	// determine which leaf node we are in given a position
	int node = 0;
	int leaf = -1;

	bool outsideOfTree = false;

	while ( leaf == -1 && !outsideOfTree )
	{
		switch ( ClassifyPoint ( &origin, &PlaneArray [ NodeArray [ node ].Plane ] ) )
		{
			case CP_FRONT: 
			case CP_ONPLANE:
				if ( NodeArray [ node ].IsLeaf != 0 )
					leaf = NodeArray [ node ].Front;
				else
					node = NodeArray [ node ].Front;
			break;

			case CP_BACK: 
				if ( NodeArray [ node ].Back == -1 )
					outsideOfTree = true;
				else
					node = NodeArray [ node ].Back;
			break;
		} 
	} 

	// build the bounding box that defines all objects that we can possibly collide with, if
	// the polygon is not within the bounding box then we can not possibly collide with it
	BOUNDINGBOX bb;
	VECTOR3		offset;
	
	D3DXVec3Normalize ( &offset, &velocityVector );

	offset  *= VECTOR3 ( 5, 5, 5 );
	offset.x = ( float ) fabs ( offset.x );
	offset.y = ( float ) fabs ( offset.y );
	offset.z = ( float ) fabs ( offset.z );
	
	bb.BoxMin.x = origin.x - m_ellipRadius.x - offset.x;
	bb.BoxMin.y = origin.y - m_ellipRadius.y - offset.y;
	bb.BoxMin.z = origin.z - m_ellipRadius.z - offset.z;

	bb.BoxMax.x = origin.x + m_ellipRadius.x + offset.x;
	bb.BoxMax.y = origin.y + m_ellipRadius.y + offset.y;
	bb.BoxMax.z = origin.z + m_ellipRadius.z + offset.z;

	VECTOR3 endPoint = origin + velocityVector;

	if ( endPoint.x < bb.BoxMin.x )
		bb.BoxMin.x = endPoint.x - offset.x;
	else if ( endPoint.x > bb.BoxMax.x )
		bb.BoxMax.x = endPoint.x + offset.x;
	
	if ( endPoint.y < bb.BoxMin.y )
		bb.BoxMin.y = endPoint.y - offset.y;
	else if ( endPoint.y > bb.BoxMax.y )
		bb.BoxMax.y = endPoint.y + offset.y;
	
	if ( endPoint.z < bb.BoxMin.z )
		bb.BoxMin.z = endPoint.z - offset.z;
	else if ( endPoint.z > bb.BoxMax.z )
		bb.BoxMax.z = endPoint.z + offset.z;

	if ( outsideOfTree )
		leaf = 0;

	POLYGON*	poly       = NULL;
	BYTE*		PVSPointer = PVSData + LeafArray [ leaf ].PVSIndex;

	// determine which leafs are visible and should be rendered
	for ( ULONG currentLeaf = 0; currentLeaf < (ULONG)NumberOfLeafs; )
	{
		// if PVS culling is turned OFF or this is a non 0 PVS byte
		if ( outsideOfTree || *PVSPointer != 0 ) 
		{
			for ( int jj = 0; jj < 8 && currentLeaf < (ULONG)NumberOfLeafs; ++jj ) 
			{	

				// if PVS culling is turned OFF or this BIT is set.
				if ( outsideOfTree || ( *PVSPointer & ( 1 << jj ) ) ) 
				{

					// if Frustum culling is off or the leaf is within the viewing frustum.
					if ( BoundingBoxInBoundingBox ( bb, LeafArray [ currentLeaf ].BoundingBox ) )
					{
						UINT start	= LeafArray [ currentLeaf ].StartPolygon;
						UINT finish = LeafArray [ currentLeaf ].EndPolygon; 

						for ( UINT count = start; count < finish; ++count )
						{
							// make sure that the polygon is within the colliders bounding box
							if ( PolygonInBoundingBox ( PolygonArray [ count ], bb ) )
							{ 
								PolygonArray [ count ].Next = poly;
								poly = &PolygonArray [ count ];
							}
						} 
					}
				} 

				currentLeaf++;
			} 

			PVSPointer++;
		} 
		
		else	
		{
			PVSPointer++;
			currentLeaf += *PVSPointer * 8;
			PVSPointer++;
		} 
	}
	
	return poly;
}

void CCollisionManager::copyAndScalePolygon ( POLYGON* polyToCopy, POLYGON* scaledPoly, VECTOR3 &scalar )
{
	if ( polyToCopy->NumberOfVertices > scaledPoly->TextureIndex )
	{
		delete [ ] scaledPoly->VertexList;

		scaledPoly->VertexList   = new D3DVERTEX [ polyToCopy->NumberOfVertices ];
		scaledPoly->TextureIndex = polyToCopy->NumberOfVertices;
	}
	
	// scale the polygon's vertices
	for ( UINT ii = 0; ii < polyToCopy->NumberOfVertices; ++ii )
	{
		scaledPoly->VertexList [ ii ].x = polyToCopy->VertexList [ ii ].x * scalar.x;
		scaledPoly->VertexList [ ii ].y = polyToCopy->VertexList [ ii ].y * scalar.y;
		scaledPoly->VertexList [ ii ].z = polyToCopy->VertexList [ ii ].z * scalar.z;
	}
	scaledPoly->NumberOfVertices = polyToCopy->NumberOfVertices;

	// recalculate and normalize the scaled polygons normal
	VECTOR3 *vec0 = ( VECTOR3* ) &scaledPoly->VertexList [ 0 ];
	VECTOR3 *vec1 = ( VECTOR3* ) &scaledPoly->VertexList [ 1 ];
	VECTOR3 *vec2 = ( VECTOR3* ) &scaledPoly->VertexList [ scaledPoly->NumberOfVertices - 1 ];

	VECTOR3 edge1 = ( *vec1 ) - ( *vec0 );
	VECTOR3 edge2 = ( *vec2 ) - ( *vec0 );

	D3DXVec3Cross     ( &scaledPoly->Normal, &edge1, &edge2 );
	D3DXVec3Normalize ( &scaledPoly->Normal, &scaledPoly->Normal );
}
