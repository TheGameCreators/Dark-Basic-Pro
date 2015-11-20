//Collision step
#include "col_local.h"
#include < D3DX9.h >

bool getLowestRoot(float a, float b, float c, float maxR,float* root) 
{
	// Check if a solution exists
	float determinant = b*b - 4.0f*a*c;
	
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	
	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but let's disregard that slight optimization)
	float sqrtD = (float)sqrt(determinant);
	float r1 = (-b - sqrtD) / (2*a);
	float r2 = (-b + sqrtD) / (2*a);
	
	// Sort so x1 <= x2
	if (r1 > r2) 
	{
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) 
	{
		*root = r1;
		return true;
	}
	
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) 
	{
		*root = r2;
		return true;
	}
	
	// No (valid) solutions
	return false;
}

typedef unsigned int uint32;
#define in(a) ((uint32&) a)

bool checkPointInTriangle(VECTOR point,VECTOR pa,VECTOR pb,VECTOR pc)
{
	VECTOR e10=pb-pa;
	VECTOR e20=pc-pa;
	
	float a = e10.dot(e10);
	float b = e10.dot(e20);
	float c = e20.dot(e20);
	float ac_bb=(a*c)-(b*b);
	
	VECTOR vp(point.x-pa.x, point.y-pa.y, point.z-pa.z);
	
	float d = vp.dot(e10);
	float e = vp.dot(e20);
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;
	return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000) != 0;
}

/*
#define EPSILON 0.001f

D3DXVECTOR3 g_vecNormal;

D3DXVECTOR3 g_vecCache [ 3 ];


bool XXIntersectPlane ( const D3DXVECTOR3& start, const D3DXVECTOR3& dir,  const D3DXVECTOR3& pt, D3DXVECTOR3& ret )
{
	const D3DXVECTOR3& n = g_vecNormal;
	float denom = D3DXVec3Dot ( &n,&dir );

	if ( denom <= -EPSILON || denom >= EPSILON )
	{
		float numer = D3DXVec3Dot ( &n,&( pt - start ) );
		float u = numer / denom;

		if ( u > 0 && u <= 1.0f )
		{
			ret = start + dir * u;
			return true;
		}
	}
	return false;
}

bool XXPointInPoly ( const D3DXVECTOR3& p )
{
    D3DXVECTOR3 v [ 32 ];

    for ( int i = 0; i < 3; i++ )
	{
        v[i] = p - g_vecCache[i];
        D3DXVec3Normalize(&v[i],&v[i]);
    }

    float total = 0.0f;

    for ( i = 0; i < 3 - 1; i++ )
	{
		total += ( float ) acos ( D3DXVec3Dot ( &v [ i ], &v [ i + 1 ] ) );
	}

    total += ( float ) acos ( D3DXVec3Dot ( &v [ 3 - 1 ], &v [ 0 ] ) );

    if ( fabsf ( total - 6.28f ) < EPSILON )
	{
        return true;
    }

    return false;
}

D3DXVECTOR3 XXclosest_on_line ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& p )
{
    D3DXVECTOR3 c = p - a;
    D3DXVECTOR3 v = b - a;

    float d = D3DXVec3Length ( &v );

    if ( d )
		v /= d;

    float t = D3DXVec3Dot ( &v, &c );
    
	if ( t < 0.0f )
		return a;

    if ( t > d )
		return b;

    return a + v * t;
}



D3DXVECTOR3 XXclosest_on_poly ( const D3DXVECTOR3& p, D3DXVECTOR3* line_dir )
{
    D3DXVECTOR3 v [ 32 ];
    float d [ 32 ];

    for ( int i = 0; i < 3 - 1; i++ )
	{
        v [ i ] = XXclosest_on_line ( g_vecCache [ i ], g_vecCache [ i + 1 ], p );

        D3DXVECTOR3 t = p - v [ i ];
        
		d [ i ] = D3DXVec3Length ( &t );
    }

    i = 3 - 1;

    v [ i ] = XXclosest_on_line ( g_vecCache [ i ],g_vecCache[ 0 ], p );
    
	D3DXVECTOR3 t = p - v [ i ];

    d [ i ] = D3DXVec3Length ( &t );

    float min = d [ 0 ];
    D3DXVECTOR3 r = v [ 0 ];
	int cl_line = 0;

    for ( i = 1; i < 3; i++ ) 
	{
        if ( d [ i ] < min )
		{
            min     = d [ i ];
            r       = v [ i ];
			cl_line = i;
        }
    }

	// calculate direction vector of closest line
	if ( cl_line < 3 - 1 )  
		*line_dir = g_vecCache [ cl_line + 1 ] - g_vecCache [ cl_line ];
	else
		*line_dir =g_vecCache [ 0 ] - g_vecCache [ cl_line ];


    return r;
}


float XXintersect_sphere ( const D3DXVECTOR3& r, const D3DXVECTOR3& rv, const D3DXVECTOR3& s, float sr )
{
    D3DXVECTOR3 q = s - r;
    float c = D3DXVec3Length ( &q );
    float v = D3DXVec3Dot ( &q, &rv );
    float d = sr * sr - ( c * c - v * v );

    if ( d < 0.0f )
		return -1.0f;

    return ( float ) ( v - sqrt ( d ) );
}
void checkTriangle(CollisionPacket* colPackage,VECTOR p1,VECTOR p2,VECTOR p3)
{
	D3DXVECTOR3 r;
	D3DXVECTOR3 pt;
	D3DXVECTOR3 n;
    D3DXVECTOR3 s;

	pt.x = p1.x;
	pt.y = p1.y;
	pt.z = p1.z;

	{
		// calculate the polygon normal
		D3DXVECTOR3 vec0 = D3DXVECTOR3 ( p1.x, p1.y, p1.z );
		D3DXVECTOR3 vec1 = D3DXVECTOR3 ( p2.x, p2.y, p2.z );
		D3DXVECTOR3 vec2 = D3DXVECTOR3 ( p3.x, p3.y, p3.z );

		// get the edges
		D3DXVECTOR3 edge1 = ( vec1 ) - ( vec0 );
		D3DXVECTOR3 edge2 = ( vec2 ) - ( vec0 );
		D3DXVECTOR3 vecNormal;

		// get the cross product and normalize it
		D3DXVec3Cross     ( &vecNormal, &edge1, &edge2 );
		D3DXVec3Normalize ( &vecNormal, &vecNormal );

		n = vecNormal;

		g_vecNormal = n;

		g_vecCache [ 0 ] = vec0;
		g_vecCache [ 1 ] = vec1;
		g_vecCache [ 2 ] = vec2;
	}

	D3DXVECTOR3 vecSrc = D3DXVECTOR3 ( colPackage->basePoint.x, colPackage->basePoint.y, colPackage->basePoint.z );

	s = vecSrc - n * 1.0f;

	float t = D3DXVec3Dot ( &( s - pt ), &n );

	if ( t > colPackage->velocityLength )
	{
        return;
    }

    if ( t < -2 * 1.0f )
	{
        return;
    }

    if ( t < 0.0f ) 
	{
        if ( !XXIntersectPlane ( s, n * 1.0f, pt, r ) )
			return;
    }
    else 
	{
		D3DXVECTOR3 dir = D3DXVECTOR3 ( colPackage->velocity.x, colPackage->velocity.y, colPackage->velocity.z );

     	if ( !XXIntersectPlane ( s, dir, pt, r ) )
			return;
    }

	
	if ( !XXPointInPoly ( r ) )
	{
		D3DXVECTOR3 line_dir;
        r = XXclosest_on_poly ( r, &line_dir );
		
		D3DXVECTOR3 ndir;
		D3DXVECTOR3 cndir = D3DXVECTOR3 ( colPackage->normalizedVelocity.x, colPackage->normalizedVelocity.y, colPackage->normalizedVelocity.z );
		
		D3DXVec3Cross ( &ndir, &cndir, &line_dir );
		D3DXVec3Cross ( &ndir, &line_dir, &ndir );
		D3DXVec3Normalize ( &ndir, &ndir );

		t = XXintersect_sphere ( r, -ndir, vecSrc, 1.0f );
	}
	else
	{
		D3DXVECTOR3 cndir = D3DXVECTOR3 ( colPackage->normalizedVelocity.x, colPackage->normalizedVelocity.y, colPackage->normalizedVelocity.z );

		t = XXintersect_sphere ( r, -cndir, vecSrc, 1.0f );
	}

	 //if ( t >= 0.0f && t <= coldat.dir_len )
	if ( t >= 0.0f && t <= colPackage->velocityLength )
		{
			//if ( !coldat.found || t < coldat.dist )
			{
				//coldat.found        = true;
				//coldat.dist         = t;
				//coldat.nearest_poly = r;

				//float distToCollision = t*colPackage->velocity.length();

				colPackage->nearestDistance = t;
				colPackage->intersectionPoint.x=r.x;
				colPackage->intersectionPoint.y=r.y;
				colPackage->intersectionPoint.z=r.z;
				colPackage->foundCollision = true;
        }
	}
}
*/

#include "3dmath.h"

extern float g_fDistance;
extern CVector3 g_vecPoint;


void checkTriangle(CollisionPacket* colPackage,VECTOR p1,VECTOR p2,VECTOR p3)
{
	CVector3 g_vTriangle[3];
	CVector3 g_vPosition;

	g_fDistance = 0.0f;
	g_vecPoint = CVector3 ( 0.0f, 0.0f, 0.0f );

	g_vTriangle [ 0 ].x = p1.x;
	g_vTriangle [ 0 ].y = p1.y;
	g_vTriangle [ 0 ].z = p1.z;

	g_vTriangle [ 1 ].x = p2.x;
	g_vTriangle [ 1 ].y = p2.y;
	g_vTriangle [ 1 ].z = p2.z;

	g_vTriangle [ 2 ].x = p3.x;
	g_vTriangle [ 2 ].y = p3.y;
	g_vTriangle [ 2 ].z = p3.z;

	g_vPosition.x = colPackage->basePoint.x;
	g_vPosition.y = colPackage->basePoint.y;
	g_vPosition.z = colPackage->basePoint.z;

	bool bCollided = SpherePolygonCollision(g_vTriangle, g_vPosition, 3, 1.0f); 

	if ( bCollided )
	{
		int c = 0;

		colPackage->nearestDistance     = g_fDistance;
		colPackage->intersectionPoint.x = g_vecPoint.x;
		colPackage->intersectionPoint.y = g_vecPoint.y;
		colPackage->intersectionPoint.z = g_vecPoint.z;
		colPackage->foundCollision      = true;
	}

	/*
	// Set result:
	if (foundCollison == true) 
	{
		// distance to collision: 't' is time of collision
		float distToCollision = t*colPackage->velocity.length();

		// Does this triangle qualify for the closest hit?
		if (distToCollision < colPackage->nearestDistance) 
		{
			// Collision information nessesary for sliding
			colPackage->nearestDistance = distToCollision;
			colPackage->intersectionPoint=collisionPoint;
			colPackage->foundCollision = true;
		}
	}
	*/
}

/*
//Sphere->triangle collision
// Assumes: p1,p2 and p3 are given in ellipsoid space:
void checkTriangle(CollisionPacket* colPackage,VECTOR p1,VECTOR p2,VECTOR p3)
{
	// Make the plane containing this triangle.
	PLANE trianglePlane(p1,p2,p3);

	// Is triangle front-facing to the velocity vector?
	// We only check front-facing triangles
	// (your choice of course)
	if( !trianglePlane.isFrontFacingTo(colPackage->normalizedVelocity) && (!isBackCulling))
	{
		trianglePlane.equation[0] *=-1;
		trianglePlane.equation[1] *=-1;
		trianglePlane.equation[2] *=-1;
		trianglePlane.equation[3] *=-1;

		trianglePlane.normal.x*=-1;
		trianglePlane.normal.y*=-1;
		trianglePlane.normal.z*=-1;
	}

	if (trianglePlane.isFrontFacingTo(colPackage->normalizedVelocity) || (! isBackCulling)) 
	{
		// Get interval of plane intersection:
		double t0, t1;
		bool embeddedInPlane = false;
	
		// Calculate the signed distance from sphere
		// position to triangle plane
		double signedDistToTrianglePlane =	trianglePlane.signedDistanceTo(colPackage->basePoint);
		
		// cache this as we're going to use it a few times below:
		float normalDotVelocity =	trianglePlane.normal.dot(colPackage->velocity);
		
		// if sphere is travelling parrallel to the plane:
		if (normalDotVelocity == 0.0f) 
		{
			if (fabs(signedDistToTrianglePlane) >= 1.0f) 
			{
				// Sphere is not embedded in plane.
				// No collision possible:
				return;
			}
			else 
			{
				// sphere is embedded in plane.
				// It intersects in the whole range [0..1]
				embeddedInPlane = true;
				t0 = 0.0;
				t1 = 1.0;
			}
		}
		else
		{
			// N dot D is not 0. Calculate intersection interval:
			t0=(-1.0-signedDistToTrianglePlane)/normalDotVelocity;
			t1=( 1.0-signedDistToTrianglePlane)/normalDotVelocity;
		
			// Swap so t0 < t1
			if (t0 > t1) {
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}
			
			// Check that at least one result is within range:
			if (t0 > 1.0f || t1 < 0.0f) 
			{
				// Both t values are outside values [0,1]
				// No collision possible:
				return;
			}
			
			// Clamp to [0,1]
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		// OK, at this point we have two time values t0 and t1
		// between which the swept sphere intersects with the
		// triangle plane. If any collision is to occur it must
		// happen within this interval.
		VECTOR collisionPoint;
		bool foundCollison = false;
		float t = 1.0;

		// First we check for the easy case - collision inside
		// the triangle. If this happens it must be at time t0
		// as this is when the sphere rests on the front side
		// of the triangle plane. Note, this can only happen if
		// the sphere is not embedded in the triangle plane.		
		if (!embeddedInPlane) 
		{
			VECTOR planeIntersectionPoint =	(colPackage->basePoint-trianglePlane.normal) + colPackage->velocity*t0;
			if (checkPointInTriangle(planeIntersectionPoint,p1,p2,p3))
			{
				foundCollison = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		// if we haven't found a collision already we'll have to
		// sweep sphere against points and edges of the triangle.
		// Note: A collision inside the triangle (the check above)
		// will always happen before a vertex or edge collision!
		// This is why we can skip the swept test if the above
		// gives a collision!
		if (foundCollison == false) 
		{
			// some commonly used terms:
			VECTOR velocity = colPackage->velocity;
			VECTOR base = colPackage->basePoint;
			float velocitySquaredLength = velocity.squaredLength();
			float a,b,c; // Params for equation
			float newT;
		
			// For each vertex or edge a quadratic equation have to
			// be solved. We parameterize this equation as
			// a*t^2 + b*t + c = 0 and below we calculate the
			// parameters a,b and c for each test.
			// Check against points:
			a = velocitySquaredLength;
			
			// P1
			b = 2.0*(velocity.dot(base-p1));
			c = (p1-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p1;
			}
			
			// P2
			b = 2.0*(velocity.dot(base-p2));
			c = (p2-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p2;
			}
			
			// P3
			b = 2.0*(velocity.dot(base-p3));
			c = (p3-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p3;
			}
			
			// Check agains edges:
			// p1 -> p2:
			VECTOR edge = p2-p1;
			VECTOR baseToVertex = p1 - base;
			float edgeSquaredLength = edge.squaredLength();
			float edgeDotVelocity = edge.dot(velocity);
			float edgeDotBaseToVertex = edge.dot(baseToVertex);
			
			// Calculate parameters for equation
			a = edgeSquaredLength*-velocitySquaredLength +	edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex)) - 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength()) + edgeDotBaseToVertex*edgeDotBaseToVertex;

			// Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				// Check if intersection is within line segment:
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex) / edgeSquaredLength;

				if (f >= 0.0 && f <= 1.0) 
				{
					// intersection took place within segment.
					t = newT;
					foundCollison = true;
					collisionPoint = p1 + edge*f;
				}
			}

			// p2 -> p3:
			edge = p3-p2;
			baseToVertex = p2 - base;
			edgeSquaredLength = edge.squaredLength();
			edgeDotVelocity = edge.dot(velocity);
			edgeDotBaseToVertex = edge.dot(baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex)) - 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength()) + edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex) / edgeSquaredLength;
				
				if (f >= 0.0 && f <= 1.0) 
				{
					t = newT;
					foundCollison = true;
					collisionPoint = p2 + edge*f;
				}
			}

			// p3 -> p1:
			edge = p1-p3;
			baseToVertex = p3 - base;
			edgeSquaredLength = edge.squaredLength();
			edgeDotVelocity = edge.dot(velocity);
			edgeDotBaseToVertex = edge.dot(baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex)) - 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength()) + edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex) / edgeSquaredLength;
				
				if (f >= 0.0 && f <= 1.0) 
				{
					t = newT;
					foundCollison = true;
					collisionPoint = p3 + edge*f;
				}
			}
		}

		// Set result:
		if (foundCollison == true) 
		{
			// distance to collision: 't' is time of collision
			float distToCollision = t*colPackage->velocity.length();

			// Does this triangle qualify for the closest hit?
			if (distToCollision < colPackage->nearestDistance) 
			{
				// Collision information nessesary for sliding
				colPackage->nearestDistance = distToCollision;
				colPackage->intersectionPoint=collisionPoint;
				colPackage->foundCollision = true;
			}
		}
	} // if not backface
}
*/

//Cilinder-cilinder collision
// Assumes: pos and radius are given in normal space:
//Whole routine is in normal space, so we have to convert
void checkCilinder(CollisionPacket* colPackage, VECTOR entpos, VECTOR entradius)
{
	//Convert
	VECTOR pos, vel;

	pos=colPackage->basePoint * colPackage->eRadius;
	vel=colPackage->velocity * colPackage->eRadius;

	float a,b,c,d;
	float t1,t2,ttemp,D;
	float r;

	//------------------------First calculate circle intersection-------------------------------

	//at^2+bt+c=r^2
	a=vel.x*vel.x + vel.z*vel.z;
	b=-2*vel.x*pos.x+2*vel.x*entpos.x  + -2*vel.z*pos.z+2*vel.z*entpos.z;
	c=pos.x*pos.x+entpos.x*entpos.x-2*entpos.x*pos.x + pos.z*pos.z+entpos.z*entpos.z-2*entpos.z*pos.z;
	r=colPackage->eRadius.x + entradius.x; //Improve
	d=r*r;

	//Discriminant
	D=b*b-4*a*(c-d);
	if(D<0)return;	

	if(a!=0.0)t1= -(-b+sqrt(D))/(2*a); //HACK
	else t1=-1000; //Eternity
	if(a!=0.0)t2= -(-b-sqrt(D))/(2*a); //HACK
	else t2=1000; //Eternity

	if(t2<t1)
	{
		ttemp=t1;
		t1=t2;
		t2=ttemp;
	}

	//------------------------Then calculate height intersection-------------------------------
	float t3,t4;

	//at^2+bt+c=r^2
	a=vel.y*vel.y;
	b=-2*vel.y*pos.y+2*vel.y*entpos.y;
	c=pos.y*pos.y+entpos.y*entpos.y-2*entpos.y*pos.y;
	r=colPackage->eRadius.y + entradius.y;
	d=r*r;

	//Discriminant
	D=b*b-4*a*(c-d);
	if(D<0)return;

	if(a!=0.0)t3= -(-b+sqrt(D))/(2*a); //HACK
	else t3=-1000; //Eternity
	if(a!=0.0)t4= -(-b-sqrt(D))/(2*a); //HACK
	else t4=1000; //Eternity

	if(t4<t3)
	{
		ttemp=t3;
		t3=t4;
		t4=ttemp;
	}

	//HACK
	if(t1==-1000)return;
	if(t3==-1000 && (fabs(pos.y-entpos.y)>r)) return;

	//------------------------Determine the AND of the two timeframes-------------------------------
	bool intersect=false;
	float begin=1000, end=-1000;

	if( (t1<=t3 && t2>=t3) ){intersect=true;begin=t3;}
	if( (t1<=t4 && t2>=t4) ){intersect=true;end=t4;}
	if( (t3<=t1 && t4>=t1) ){intersect=true;begin=t1;}
	if( (t3<=t2 && t4>=t2) ){intersect=true;end=t2;}

	if(!intersect)return;
	//if( (begin<0.0 || begin>1.0) && (end<0.0 || end>1.0))return;
	if( (begin<0.0 && end<0.0) || (begin>1.0 && end>1.0) )return;

	//We have a collision!

	VECTOR collisionPoint=(pos - entpos);
	collisionPoint.y=0;
	collisionPoint.SetLength(entradius.x); //HACK
	collisionPoint = collisionPoint + entpos;
	collisionPoint.y = pos.y + vel.y * begin;
	collisionPoint = collisionPoint / colPackage->eRadius;

	// distance to collision: 'begin' is time of collision
	float distToCollision = begin * colPackage->velocity.length();

	// Does this triangle qualify for the closest hit?
	if (distToCollision < colPackage->nearestDistance) 
	{
		// Collision information nessesary for sliding
		colPackage->nearestDistance = distToCollision;
		colPackage->intersectionPoint = collisionPoint;	
		colPackage->foundCollision = true;
	}
}