#ifndef _3DMATH_H
#define _3DMATH_H

#define PI 3.1415926535897932					// This is our famous PI


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// These constants are used as return values from ClassifySphere().  Depending
// on where the sphere lies in accordance with the plane being checked, these
// will allow us to label if the sphere is in front, behind or intersecting the plane.

#define BEHIND		0	// This is returned if the sphere is completely behind the plane
#define INTERSECTS	1	// This is returned if the sphere intersects the plane
#define FRONT		2	// This is returned if the sphere is completely in front of the plane

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our 3D point and vector class
struct CVector3
{
public:
	
	// A default constructor
	CVector3() {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Here we overload the + operator so we can add vectors together 
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result.
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator*(float num)
	{
		// Return the subtracted vectors result
		return CVector3(x * num, y * num, z * num);
	}

	float x, y, z;						
};


//	This returns a perpendicular vector from 2 given vectors by taking the cross product.
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

//	This returns the magnitude of a normal (or any other vector)
float Magnitude(CVector3 vNormal);

//	This returns a normalize vector (A vector exactly of length 1)
CVector3 Normalize(CVector3 vNormal);

//	This returns the normal of a polygon (The direction the polygon is facing)
CVector3 Normal(CVector3 vPolygon[]);

// This returns the distance between 2 3D points
float Distance(CVector3 vPoint1, CVector3 vPoint2);

// This returns the point on the line segment vA_vB that is closest to point vPoint
CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint);

// This returns the distance the plane is from the origin (0, 0, 0)
// It takes the normal to the plane, along with ANY point that lies on the plane (any corner)
float PlaneDistance(CVector3 Normal, CVector3 Point);

// This takes a triangle (plane) and line and returns true if they intersected
bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance);

// This returns the dot product between 2 vectors
float Dot(CVector3 vVector1, CVector3 vVector2);

// This returns the angle between 2 vectors
double AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2);

// This returns an intersection point of a polygon and a line (assuming intersects the plane)
CVector3 IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance);

// This returns true if the intersection point is inside of the polygon
bool InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount);

// Use this function to test collision between a line and polygon
bool IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This returns the absolute value of num - a simple if/else check
float absolute(float num);

// This function classifies a sphere according to a plane.  The information returned
// tells us if the sphere is BEHIND, in FRONT, or INTERSECTS the plane.  This takes 
// the sphere's center, the plane's normal, a point on the plane, the sphere's radius
// and a referenced variable to hold the distance.  This way we can return the distance
// and the sphere's relationship to the plane.  The distance is from the plane to the center
// of the sphere.  With this information it enables us to offset the sphere if needed.
int ClassifySphere(CVector3 &vCenter, 
				   CVector3 &vNormal, CVector3 &vPoint, float radius, float &distance);

// This takes in the sphere center, radius, polygon vertices and vertex count.
// This function is only called if the intersection point failed.  The sphere
// could still possibly be intersecting the polygon, but on it's edges.
bool EdgeSphereCollision(CVector3 &vCenter, 
						 CVector3 vPolygon[], int vertexCount, float radius);

// This returns true if the sphere is intersecting with the polygon.
// The parameters are the vertices of the polygon, vertex count, along with the center 
// and radius of the sphere.
bool SpherePolygonCollision(CVector3 vPolygon[], 
							CVector3 &vCenter, int vertexCount, float radius);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added 4 new math functions and 3 new defines to our math library.  Of course,
// it is not an official library as of yet, but eventually it will be.  We still have
// many math functions to add to it before we don't need to alter it anymore.  I
// would assume though, that as you learn more about 3D graphics, this will be
// a never ending process, but for most normal games you will be making, there is
// a time when you got everything you will need.  The rest is most likely specialized.
//
// For sphere-polygon collision, there is only 3 main functions that needed to be added. 
// If you have scavenged around the internet, you will notice that this type of collision
// can be very complicated and there are quite a few ways to do it.  This is probably
// the simplest way I found, without having to add a large number of functions.  Be
// sure to check out how to find the closest point on a triangle to the sphere.  Instead
// of using edge plans, this can be a more accurate way of doing sphere-polygon collision.
//
// Check out 3DMath.cpp for the explanations of how these new functions work.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//


