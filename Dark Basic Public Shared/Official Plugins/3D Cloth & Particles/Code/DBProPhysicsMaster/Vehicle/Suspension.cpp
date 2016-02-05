#include "stdafx.h"

Suspension::Suspension(Vehicle * _parent,const Vector3 &_pos, const Vector3 &_up, const Vector3 &_fwd)
		:parent(_parent),wheelObjId(-1)
{	
	Vector3 fwd=_fwd;
	Vector3 up = _up;
	
	fwd.Normalise();
	up.Normalise();
	
	local.setUnitY(up);
	local.setUnitZ(fwd);
	local.setPos(_pos);

	Vector3 axle = up*fwd;
	axle.Normalise();
	local.setUnitX(axle);
	
	world=local;
	world.transformUsing(parent->body.World());

	spring=1.0f;
	damping=0.0f;
	vel=0.0f;
	minDist=0.0f;
	dist=0.4f;
	maxDist=0.4f;
	
	weightShare=0.0f;

	springForce=0.0f;

	lastDist=0.4f;
	wheelRotVel=0.0f;
	wheelRotate=0.0f;
	wheelSteer=0.0f;

	perpFriction=5.0f;
	brake=0.0f;
	accel=0.0f;

}

extern float getGroundHeight(const Vector3 &pos, Vector3 * groundNormal, float * planeD);

void Suspension::Update()
{
	world = local;	
	world.transformUsing(parent->body.World());	

	wheelBase.Set(0,-dist,0);
	world.rotateAndTransV3(&wheelBase);
	Vector3 widthAdj = world.unitX()*cosf(wheelSteer) - world.unitZ()*sinf(wheelSteer);
	if(widthAdj.y>0) widthAdj=-widthAdj;
	widthAdj*=width*0.5f;
	wheelBase+=widthAdj;

	Vector3 suspAxis = -world.unitY();
	
	float planeDist;
	groundHeight = getGroundHeight(wheelBase, &groundNorm, &planeDist);

	isOnGround=false;

	pushDist=0.0f;
	
	lastDist=dist;

	//Build in a slight tolerance
	if(groundHeight+0.01f>wheelBase.y) isOnGround=true;

	if(groundHeight>wheelBase.y)
	{		
		float hdiff=0.0f;
		if(suspAxis.y!=0.0f) hdiff=(groundHeight-wheelBase.y)/(float)fabs(suspAxis.y);

		//These two lines take into account the suspension lying down (i.e. more force is needed to push it)
		pushDist=-(hdiff+hdiff*suspAxis.y)*suspAxis.y;
		hdiff=-hdiff*suspAxis.y;

		//check suspension limit
		if(dist-hdiff<minDist)
		{
			pushDist=minDist-(dist-hdiff);
			hdiff-=pushDist;
		}	

		//Set the parameters for suspension
		dist -= hdiff;
		vel -= hdiff*PhysicsManager::iterationFps;
	}
	

	springForce = (maxDist-dist)*spring - vel*damping;

	float acc=springForce*40.0f;
	vel+=acc*PhysicsManager::iterationInterval;
	dist+=vel*PhysicsManager::iterationInterval;
	
	if(dist>maxDist) 
	{
		dist=maxDist;
	}
	if(dist<minDist) 
	{
		dist=minDist;
	}	

	vel=(dist-lastDist)*PhysicsManager::iterationFps;

	//if(myid==0) consoleDebugMsg("%.4f,",world.pos().y);
	//consoleDebugMsg("%.4f,%.4f",dist,vel);

	//if(myid==3) consoleDebugMsg("\n");
	//else  consoleDebugMsg(",");

	if(isOnGround)
	{
		Vector3 carForce = suspAxis * -springForce;
		Vector3 reaction = groundNorm*groundNorm.Dot(-carForce);
		carForce.x=carForce.z=0.0f;
		parent->body.addForce(&carForce,WORLD,&world.pos(),WORLD);
		
		//Remove component along suspension axis (we've effectively already applied this)
		reaction -= suspAxis*suspAxis.Dot(reaction);
		parent->body.addForce(&reaction,WORLD,&wheelBase,WORLD);

		//Calculate world velocity at wheelbase
		Vector3 localBase=wheelBase;
		localBase-=parent->body.World().pos();
		parent->body.invWorld().rotateV3(&localBase);
		Vector3 wheelBaseVel;
		parent->body.getPointWorldVel(localBase,&wheelBaseVel);
		
		//Remove component along suspension axis
		wheelBaseVel-=world.unitY()*world.unitY().Dot(wheelBaseVel);

		//Don't need to normalise this result
		wheelDir=world.unitX()*sinf(wheelSteer) + world.unitZ()*cosf(wheelSteer);
		velPar=wheelDir*wheelDir.Dot(wheelBaseVel);
		velPerp=wheelBaseVel-velPar;

		wheelRotVel=wheelDir.Dot(wheelBaseVel)/radius;
		
	}
	else
	{
		wheelRotVel*=0.9f;
	}

	//Construct wheel matrix
	wheelRotate+=wheelRotVel*PhysicsManager::iterationInterval;
	if(isFlipped)
	{
		wheel.make3x3RotationMatrix(-wheelRotate,wheelSteer+3.14159265f,0,ROTATE_XYZ);
	}
	else
	{
		wheel.make3x3RotationMatrix(wheelRotate,wheelSteer,0,ROTATE_XYZ);
	}
	wheel.setPos(0,radius-dist,0);
	wheel.transformUsing(world);
	if(wheelObjId!=-1)
	{
		PhysicsManager::DLL_SetWorldMatrix(wheelObjId,wheel);
	}
}

void Suspension::updateGraphics()
{
	if(isOnGround) D3D_DebugLines::getInst()->drawCross(wheelBase,0.05f,0x0ffff0000);
	else D3D_DebugLines::getInst()->drawCross(wheelBase,0.05f,0x0ff000000);
}

