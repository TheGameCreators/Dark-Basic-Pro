//Collision step
#include "col_local.h"

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
	if ( (int)( ((( in(z)& ~(in(x)|in(y)) ) & 0x80000000)) )==0 )
		return false;
	else
		return true;
}

// Assumes: p1,p2 and p3 are given in ellisoid space:
void checkTriangle(CollisionPacket* colPackage,VECTOR p1,VECTOR p2,VECTOR p3)
{
    #ifndef __GNUC__
	// Make the plane containing this triangle.
	PLANE trianglePlane(p1,p2,p3);

	// Is triangle front-facing to the velocity vector?
	// We only check front-facing triangles
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

				// trigger collection of triangle index when leave
				colPackage->triangleindex = -2;
			}
		}
	} // if not backface
	#endif
}
