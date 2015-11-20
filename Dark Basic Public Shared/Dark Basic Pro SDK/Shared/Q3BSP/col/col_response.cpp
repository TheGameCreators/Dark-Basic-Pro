//Collision response
#include "col_local.h"

CollisionPacket collisionPackage; //Stores all the parameters and returnvalues
int collisionRecursionDepth; //Internal variable tracking the recursion depth

bool isGravity=true; //Do we need to apply gravity?
bool isBackCulling=false; //Do we need to apply backface culling?
bool isResponse=true; //Do we need to apply response + recursion?

float * triangle_pool;  //Stores the pointers to the traingles used for collision detection
int numtriangle; //Number of traingles in pool
int offset; //The number of floats between individual vertices in the triangle pool, so texture coordinates can be skipped

int numcilinder; //Number of cilinders
VECTOR cilinder_pos[64]; //Position of the cilinders
VECTOR cilinder_radius[64]; //Radius of the cilinders

float unitsPerMeter; // Set this to match application scale..
VECTOR gravity; //Gravity

CollisionTrace trace; //Output structure telling the application everything about the collision

void checkCollision()
{
	VECTOR P1,P2,P3; //Temporary variables holding the triangle in R3
	VECTOR eP1, eP2, eP3; //Temporary variables holding the triangle in eSpace

	for(int i=0; i < numtriangle ; i++) //Iterate trough the entire triangle pool
	{
		
		P1.set((triangle_pool[i*9]), (triangle_pool[i*9+1]), (triangle_pool[i*9+2]) );//First vertex
		P2.set((triangle_pool[i*9+3+offset]), (triangle_pool[i*9+4+offset]), (triangle_pool[i*9+5+offset]) );//Second vertex
		P3.set((triangle_pool[i*9+6+2*offset]), (triangle_pool[i*9+7+2*offset]), (triangle_pool[i*9+8+2*offset]) );//Third vertex

		//Transform to eSpace
		eP1 = P1 / collisionPackage.eRadius;
		eP2 = P2 / collisionPackage.eRadius;
		eP3 = P3 / collisionPackage.eRadius;

		checkTriangle(&collisionPackage, eP1, eP2, eP3);
	}

	for(i=0; i < numcilinder ; i++) //Iterate trough the entire cilinder pool
	{
		checkCilinder(&collisionPackage, cilinder_pos[i], cilinder_radius[i]);
	}
}

VECTOR collideWithWorld(VECTOR pos,VECTOR vel)
{
	// All hard-coded distances in this function is
	// scaled to fit the setting above..
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.001f * unitScale; //0.005f * unitScale;

	//float unitScale = 1.0f;
	//float veryCloseDistance = 0.001f;

	// do we need to worry?
	if (collisionRecursionDepth>5 || vel.length() < veryCloseDistance*2)
		return pos;

	///////////
	//if (collisionRecursionDepth>5)
	//	return pos;
	///////////

	// Ok, we need to worry:
	collisionPackage.velocity = vel;
	collisionPackage.normalizedVelocity = vel;
	collisionPackage.normalizedVelocity.normalize();
	collisionPackage.velocityLength = vel.length();

	///////////
	/*
	if ( vel.x == 0.0f && vel.y == 0.0f && vel.z == 0.0f)
	{
		collisionPackage.normalizedVelocity.x = 0.0f;
		collisionPackage.normalizedVelocity.y = 0.0f;
		collisionPackage.normalizedVelocity.z = 0.0f;
	}
	*/
	///////////

	collisionPackage.basePoint = pos;
	collisionPackage.foundCollision = false;
	collisionPackage.nearestDistance = 100000.0f;
	

	// Check for collision (calls the collision routines)
	// Application specific!!
	checkCollision();

	// If no collision we just move along the velocity
	if (collisionPackage.foundCollision == false)
	{
		return pos + vel;
	}

	// *** Collision occured ***
	// The original destination point
	VECTOR destinationPoint = pos + vel;
	VECTOR newSourcePoint = pos;
	
	// only update if we are not already very close
	// and if so we only move very close to intersection..not
	// to the exact spot.
	VECTOR V = vel;
	
	//If we are at the ideal point, just back off
	if(collisionPackage.nearestDistance==0.0f)
		return pos + vel;

	V.SetLength( collisionPackage.nearestDistance);
	newSourcePoint = collisionPackage.basePoint + V;

	// Determine the sliding plane
	VECTOR slidePlaneNormal = newSourcePoint - collisionPackage.intersectionPoint;
	slidePlaneNormal.normalize();

	//If this was the initial collision, ouput in CollisionTrace
	if ( collisionRecursionDepth==0)
	{
		trace.foundCollision = true;
		trace.nearestDistance = (newSourcePoint-pos).length();
		trace.intersectionPoint[0]=newSourcePoint.x;
		trace.intersectionPoint[1]=newSourcePoint.y;
		trace.intersectionPoint[2]=newSourcePoint.z;
		trace.slidePlaneNormal[0]=slidePlaneNormal.x;
		trace.slidePlaneNormal[1]=slidePlaneNormal.y;
		trace.slidePlaneNormal[2]=slidePlaneNormal.z;
	}

	// If no response needed, return the collision point
	if ( !isResponse ) 
		return newSourcePoint;	

	// Determine the displacement vector
	
	float factor;

	
	V.SetLength(veryCloseDistance);
	if( V.dot(slidePlaneNormal) == 0.0f)
		factor = 0.0f;
	else 
		factor = veryCloseDistance / ( V.dot(slidePlaneNormal) );

  VECTOR displacementVector = V * factor;
	
	
	//Move away
	newSourcePoint = newSourcePoint + displacementVector;
	collisionPackage.intersectionPoint = collisionPackage.intersectionPoint + displacementVector;

	VECTOR slidePlaneOrigin = collisionPackage.intersectionPoint;	
	PLANE slidingPlane(slidePlaneOrigin,slidePlaneNormal);
	
	// Again, sorry about formatting.. but look carefully ;)
	VECTOR newDestinationPoint = destinationPoint -	slidePlaneNormal * slidingPlane.signedDistanceTo(destinationPoint);

	// Generate the slide vector, which will become our new
	// velocity vector for the next iteration
	VECTOR newVelocityVector = newDestinationPoint - collisionPackage.intersectionPoint;

	// Recurse:
	// dont recurse if the new velocity is very small	
	if (newVelocityVector.length() <= veryCloseDistance) 
	{
		return newSourcePoint;
	}

	collisionRecursionDepth++;
	return collideWithWorld(newSourcePoint,newVelocityVector);
}

CollisionTrace* collideAndSlide(float* position,float* vel)
{
	//Init CollisionTrace structure
	trace.foundCollision=false;

	// Do collision detection:
	collisionPackage.R3Position.set(position[0], position[1], position[2]);
	collisionPackage.R3Velocity.set(vel[0], vel[1], vel[2]);

	// calculate position and velocity in eSpace
	VECTOR eSpacePosition = collisionPackage.R3Position / collisionPackage.eRadius;
	VECTOR eSpaceVelocity = collisionPackage.R3Velocity / collisionPackage.eRadius;

	// Iterate until we have our final position.
	collisionRecursionDepth = 0;
	VECTOR finalPosition = collideWithWorld(eSpacePosition,eSpaceVelocity);

	// Add gravity pull if wanted
	if(isGravity)
	{
		// Set the new R3 position (convert back from eSpace to R3
		collisionPackage.R3Position =	finalPosition*collisionPackage.eRadius;
		collisionPackage.R3Velocity = gravity;
		eSpaceVelocity = gravity / collisionPackage.eRadius;

		collisionRecursionDepth = 0;
		finalPosition = collideWithWorld(finalPosition,eSpaceVelocity);
	}

	// Convert final result back to R3:
	finalPosition = finalPosition*collisionPackage.eRadius;	
	collisionPackage.intersectionPoint = collisionPackage.intersectionPoint*collisionPackage.eRadius;

	//Output the final position in the CollisionTrace structure
	trace.finalPosition[0] = finalPosition.x;
	trace.finalPosition[1] = finalPosition.y;
	trace.finalPosition[2] = finalPosition.z;

	//Transform the rest of trace back to the normal domain
	VECTOR iPoint(trace.intersectionPoint[0], trace.intersectionPoint[1], trace.intersectionPoint[2]);
	
	//Output intersection point and nearestdistance
	trace.intersectionPoint[0] *= collisionPackage.eRadius.x;
	trace.intersectionPoint[1] *= collisionPackage.eRadius.y;
	trace.intersectionPoint[2] *= collisionPackage.eRadius.z;
	trace.nearestDistance = ( iPoint*collisionPackage.eRadius - finalPosition ).length();
	

	//Ouput normal
	VECTOR normal(trace.slidePlaneNormal[0], trace.slidePlaneNormal[1], trace.slidePlaneNormal[2]);
	normal.x *= collisionPackage.eRadius.y * collisionPackage.eRadius.z;
	normal.y *= collisionPackage.eRadius.x * collisionPackage.eRadius.z;
	normal.z *= collisionPackage.eRadius.x * collisionPackage.eRadius.y;
	normal.normalize();
	trace.slidePlaneNormal[0]=normal.x;
	trace.slidePlaneNormal[1]=normal.y;
	trace.slidePlaneNormal[2]=normal.z;

	return &trace; //Return the adress of the Trace structure to the application
}

void colEnableGravity(bool grav)
{
	isGravity=grav;
}

void colEnableBackCulling(bool backcul)
{
	isBackCulling=backcul;
}

void colEnableResponse(bool resp)
{
	isResponse=resp;
}

void colSetUnitsPerMeter(float UPM)
{
	unitsPerMeter=UPM;
}

void colSetGravity(float* grav )
{
	gravity.set(grav[0], grav[1], grav[2]);
}

void colSetRadius(float* radius)
{
	collisionPackage.eRadius.set(radius[0], radius[1], radius[2]);
}

void colSetTrianglePool(int numtri, float* pool, int stride)
{
	numtriangle=numtri;
	triangle_pool=pool;
	offset=stride;	
}

void colInitCilinderPool()
{
	numcilinder=0;
}

void colAddCilinder(float* pos, float* radius)
{
	cilinder_pos[numcilinder].set(pos[0], pos[1], pos[2]);
	cilinder_radius[numcilinder].set(radius[0], radius[1], radius[2]);
	numcilinder++;
}

