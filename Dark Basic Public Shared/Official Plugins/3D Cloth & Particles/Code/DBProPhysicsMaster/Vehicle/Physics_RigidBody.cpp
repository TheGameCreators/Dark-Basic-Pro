#include "stdafx.h"


RigidBody::RigidBody()
{
	mass=1.0f;
	invMass=1.0f;

	CofG.Init();

	MofI.Init();
	invMofI.Init();

	Reset(true);
}

RigidBody::~RigidBody()
{

}

void RigidBody::Reset(bool resetTransforms)
{
	//Resets the dynamics system (and optionally the position and rotation)

	if(resetTransforms)
	{
		world.Init();
		invworld.Init();
		local.Init();
		invlocal.Init();

		invMofIT=invMofI;
	}

	vel.Init();

	qLocal.Init();
	qLocalVel.Init();
	
	angVel.Init();
	angMom.Init();


	totForce.Init();
	totTorq.Init();

    totImpulse.Init();
    totRotImpulse.Init();

	angThresh.x=0.0f;
	angThresh.y=0.0f;
	angThresh.z=0.0f;


    pCofGValid=false;

	updatedParentMatrix=true;
	matValid_World=false;
	matValid_invWorld=false;
	matValid_invLocal=false;
	quatValid_Local=false;
    matValid_Local=true;

}

//A Note On The Methods Used
//~~~~~~~~~~~~~~~~~~~~~~~~~~
//Forces and points of application must always be converted to parent space.
//(Parent space will be identical to world space if the object has no parent.)
//This is because all the dynamics properties (momentum, angular momentum, etc.) are
//defined in parent space. Parenting an object to another like this isn't physically
//correct, but it is extremely convenient to do this.

//For example: This method allows a space ship to accelerate without its crew
//feeling the the effects of the acceleration. It effectively isolates its child
//objects from it's motion.

//If a child object is required to respond to the parent's motion, then the appropriate
//forces can easily be calculated for the correct response. This can make for extremely
//effective constraint systems.


void RigidBody::addForce(const Vector3 * force, eRefFrame fref, const Vector3 * point,eRefFrame pref)
{
	static Vector3 pnt,frc;

    //Allow NULL to be passed if they wish when using pref==COFG
	if(point) pnt = *point;

	frc = *force;

    if(fref==LOCAL)
    {
    	Local_RO().rotateV3(&frc); //Transform local force direction into parent space
    }
    else if(fref==COFG)
    {
    	assert(false); //This should never be used for forces, as it makes no physical sense
    }


    if(pref==LOCAL)
    {
    	Local_RO().rotateAndTransV3(&pnt); //Transform local point into parent space
    }   

	if(pref!=COFG)
    {
    	pnt-=getPCofG();
		totTorq+=pnt*frc;
    }

	totForce+=frc;
}

void RigidBody::addTorq(const Vector3 * torq, eRefFrame tref)
{
	static Vector3 trq;
    trq = *torq;

    if(tref==LOCAL)
    {
    	Local_RO().rotateV3(&trq); //Transform local torq direction into parent space
    }
    else if(tref==COFG)
    {
    	assert(false); //This should never be used for this, as it makes no physical sense
    }

	totTorq+=trq;
}


void RigidBody::addImpulse(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref)
{
	static Vector3 pnt,imp;

    //Allow NULL to be passed if they wish when using pref==COFG
	if(point) pnt = *point;
    
	imp = *impulse;

    if(iref==LOCAL)
    {
    	Local_RO().rotateV3(&imp); //Transform local force direction into parent space
    }
    else if(iref==COFG)
    {
    	assert(false); //This should never be used for forces, as it makes no physical sense
    }


    if(iref==LOCAL)
    {
    	Local_RO().rotateAndTransV3(&pnt); //Transform local point into parent space
    }

	if(pref!=COFG)
    {
    	pnt-=getPCofG();
		totRotImpulse+=pnt*imp;
    }

	totImpulse+=imp;
}



void RigidBody::addRotImpulse(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref)
{
	static Vector3 pnt,imp;

	pnt = *point; 
	imp = *impulse;

    if(iref==LOCAL)
    {
    	Local_RO().rotateV3(&imp); //Transform local force direction into parent space
    }
    else if(iref==COFG)
    {
    	assert(false); //This should never be used for forces, as it makes no physical sense
    }


    if(iref==LOCAL)
    {
    	Local_RO().rotateAndTransV3(&pnt); //Transform local point into parent space
    }

	if(pref!=COFG)
    {
    	pnt-=getPCofG();
		totRotImpulse+=pnt*imp;
    }

	//totImpulse+=imp;
}


void RigidBody::addRotImpulse(const Vector3 * rotImp, eRefFrame iref)
{
	static Vector3 trq;
    trq = *rotImp;

    if(iref==LOCAL)
    {
    	Local_RO().rotateV3(&trq); //Transform local rotImpulse direction into parent space
    }
    else if(iref==COFG)
    {
    	assert(false); //This should never be used for this, as it makes no physical sense
    }

	totRotImpulse+=trq;
}

void RigidBody::addImpulseAndUpdate(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref)
{
	Vector3 imp(totImpulse);
	Vector3 rotimp(totRotImpulse);

	addImpulse(impulse,iref,point,pref);

	imp = totImpulse-imp;	
	rotimp = totRotImpulse-rotimp;
	
	totImpulse-=imp;
	totRotImpulse-=rotimp;

    //Add in impulses
    vel+=imp*invMass;
    angMom+= rotimp;
		
	if(angMom.x<angThresh.x && angMom.x>-angThresh.x) angMom.x=0.0f;
	if(angMom.y<angThresh.y && angMom.y>-angThresh.y) angMom.y=0.0f;
	if(angMom.z<angThresh.z && angMom.z>-angThresh.z) angMom.z=0.0f;

	//Calculate angular velocity
	invMofIT.rotateV3(angMom,&angVel);
	//Calculate rate of change of rotation quaternion
	qLocalVel = 0.5f*Quat(angVel)*quatLocal_RO();
}

const Matrix& RigidBody::World()
{
	if(!matValid_World)
	{
		//No parent exists, so world==local
		world=Local_RO();
	}
	
	matValid_World=true;
	return world;
}



const Matrix& RigidBody::invWorld()
{

	if(!matValid_invWorld)
	{
		//Assumes orthogonal matrices
		invworld=World();
		invworld.TransposeThis();
		matValid_invWorld=true;
	}

	return invworld;
}



const Matrix& RigidBody::Local_RO()
{
	//Use for read only access
    if(!matValid_Local)
    {
		assert(quatValid_Local); //local quaternion must be valid for this to happen
		local.set3x3ToQuat(qLocal);

        matValid_Local=true;
    }

	return local;
}



Matrix& RigidBody::Local_RW()
{
	//This will be more expensive as it marks the other matrices to be recalculated

    if(!matValid_Local)
    {
		assert(quatValid_Local); //local quaternion must be valid for this to happen
		local.set3x3ToQuat(qLocal);

        matValid_Local=true;
    }

	matValid_World=false;
	matValid_invWorld=false;
	matValid_invLocal=false;
    quatValid_Local=false;

	return local;
}

const Quat& RigidBody::quatLocal_RO()
{
	//Use for read only access
    if(!quatValid_Local)
    {
		assert(matValid_Local);//local matrix must be valid for this to happen
		qLocal.Set(local);

        quatValid_Local=true;
    }

	return qLocal;
}


Quat& RigidBody::quatLocal_RW()
{
	//This will be more expensive as it marks the other matrices to be recalculated

    if(!quatValid_Local)
    {
		assert(matValid_Local); //local matrix must be valid for this to happen
		qLocal.Set(local);

        quatValid_Local=true;
    }

	matValid_World=false;
	matValid_invWorld=false;
	matValid_invLocal=false;
    matValid_Local=false;

	return qLocal;
}

const Matrix& RigidBody::invLocal()
{
	if(!matValid_invLocal)
	{
		//Assumes orthogonal matrices
		invlocal=Local_RO();
		invlocal.TransposeThis();
		matValid_invLocal=true;
	}

	return invlocal;
}

void RigidBody::getPointWorldVel(const Vector3& localP, Vector3 * velOut)
{
	//p is in local space for the rigid body

	static Vector3 pnt,tmpVel;

	pnt=localP;
	pnt-=CofG;
	Local_RO().rotateV3(&pnt);
	*velOut=vel+angVel*pnt;

}


const Vector3& RigidBody::getPCofG()
{
	//Transfer local space centre of gravity into parent space
	if(!pCofGValid)
    {
		Local_RO().rotateAndTransV3(CofG,&pCofG);
    }
    return pCofG;
}


