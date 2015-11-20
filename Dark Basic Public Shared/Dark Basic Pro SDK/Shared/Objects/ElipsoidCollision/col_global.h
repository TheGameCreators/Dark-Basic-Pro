//Collision global header
#pragma warning(disable:4244)
#include <math.h>

//Collision global header, include this in your application

//Output structure, contains all the information of the collision
struct CollisionTrace
{
	bool foundCollision; //True if a collision was found
	float nearestDistance; //Distance to collision point (first collision)
	float intersectionPoint[3]; //Point where ellipsoid collided with a triangle (first collision)
	float finalPosition[3]; //Final position of ellipsoid
	int triangleindex; //The triangle I hit
	bool collidedagainstfloor; //whether hit a floor to defeat external gravity
};

extern CollisionTrace* collideAndSlide(float* position,float* vel); // Collide with triangle pool
extern void colEnableGravity(bool grav); // Enable/Disable gravity
extern void colEnableBackCulling(bool backcul); // Enable/Disable backface cullling
extern void colEnableResponse(bool resp);// Enable/Disable response (sliding)
extern void colSetUnitsPerMeter(float UPM); // Sets the units per meter
extern void colSetGravity(float* grav ); // Sets the directional gravity
extern void colSetRadius(float* radius); // Set the ellipsoid radius
extern void colSetTrianglePool(int numtriangle, float* pool, int stride); // Set the triangle pool

//Exported functions not needed as src included inside
//extern "C" __declspec(dllexport) CollisionTrace* collideAndSlide(float* position,float* vel); // Collide with triangle pool
//extern "C" __declspec(dllexport) void colEnableGravity(bool grav); // Enable/Disable gravity
//extern "C" __declspec(dllexport) void colEnableBackCulling(bool backcul); // Enable/Disable backface cullling
//extern "C" __declspec(dllexport) void colEnableResponse(bool resp);// Enable/Disable response (sliding)
//extern "C" __declspec(dllexport) void colSetUnitsPerMeter(float UPM); // Sets the units per meter
//extern "C" __declspec(dllexport) void colSetGravity(float* grav ); // Sets the directional gravity
//extern "C" __declspec(dllexport) void colSetRadius(float* radius); // Set the ellipsoid radius
//extern "C" __declspec(dllexport) void colSetTrianglePool(int numtriangle, float* pool, int stride); // Set the triangle pool