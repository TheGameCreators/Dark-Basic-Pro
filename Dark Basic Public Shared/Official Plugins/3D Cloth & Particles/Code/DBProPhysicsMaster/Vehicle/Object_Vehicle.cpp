#include "stdafx.h"



float getGroundHeight(const Vector3 &pos, Vector3 * groundNormal, float * planeD)
{	
//#pragma MakeReminder("getGroundHeight - Remove key detection")

	static float ppcx=0.0f;	
	if(GetKeyState(VK_F1) & 0x8000) ppcx-=0.0001f;
	if(GetKeyState(VK_F2) & 0x8000) ppcx+=0.0001f;	

	Plane p(&Vector3(ppcx,1,0),-4,false);
	*groundNormal=p.n;
	*planeD=p.d;
	return pos.y-((p.n.Dot(pos) - p.d)/p.n.y);
}



Vehicle::Vehicle(int id):PhysicsObject(id),
frozen(false),drawDebugLines(false),damping(0),gravity(0),chassisObjId(-1)
{
	useExternalMatrix(false);
}


Vehicle::~Vehicle()
{
	particleList.deleteAndClear();
	suspensionList.deleteAndClear();
}

void Vehicle::addNewCollisionPoint(const Vector3& p)
{
	VehicleParticle * particleNew = new VehicleParticle(p);
	particleList.Push(particleNew);
}

ePhysError Vehicle::addSuspension(const Vector3 &pos, const Vector3 &up, const Vector3 &fwd, int * suspID)
{
	*suspID = suspensionList.getLowestFreeID();
	suspensionList.Add(*suspID, new Suspension(this,pos,up,fwd));

	return PERR_AOK;
}

ePhysError Vehicle::setSuspensionLimits(int id, float minDist, float maxDist)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;	
	Suspension * susp = suspensionList.Get(id);

	if(minDist>=maxDist) return PERR_BAD_PARAMETER;
	susp->minDist=minDist;
	susp->maxDist=maxDist;	
	
	return PERR_AOK;
}

ePhysError Vehicle::setSuspensionPhysics(int id, float spring, float damping)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;	
	Suspension * susp = suspensionList.Get(id);

	if(spring<0.0f || damping<0.0f) return PERR_BAD_PARAMETER;

	susp->spring=spring;
	susp->damping=damping;
	return PERR_AOK;
}

ePhysError Vehicle::setWheelSize(int id, float radius, float width)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;
	
	Suspension * susp = suspensionList.Get(id);

	if(radius<0 || width<0) return PERR_BAD_PARAMETER;
	susp->radius=radius;
	susp->width=width;
	return PERR_AOK;
}

ePhysError Vehicle::addObjectToCollisionPoints(int objectId)
{
	//Remove duplicate positioned vertices within a tolerance of 0.001f

	if(!PhysicsManager::DLL_GetExists(objectId)) return PERR_ITEM_DOESNT_EXIST;
	int numLimbs = PhysicsManager::DLL_GetLimbCount(objectId);
	if(numLimbs==0) return PERR_NO_DATA_IN_OBJECT;
	int totNumNewVerts=0;

	//Count vertices
	for(int limb=0;limb<numLimbs;limb++)
	{
		PhysicsManager::DLL_LockVertexData(objectId,limb);
		totNumNewVerts += PhysicsManager::DLL_GetVertexCount();
		PhysicsManager::DLL_UnlockVertexData();
	}
	if(totNumNewVerts==0) return PERR_NO_DATA_IN_OBJECT;

	int curNumColVerts=particleList.Count();

	Vector3 * existingPoint  = new Vector3[totNumNewVerts+curNumColVerts];

	//Add existing collision points
	for(int i=0;i<curNumColVerts;i++)
	{
		existingPoint[i] = particleList[i]->pos;
	}
	
	for(int limb=0;limb<numLimbs;limb++)
	{
		PhysicsManager::DLL_LockVertexData(objectId,limb);
		int numVerts = PhysicsManager::DLL_GetVertexCount();		
		D3DXMATRIX d3dLimbMat;		
		PhysicsManager::DLL_GetWorldMatrix(objectId,limb,&d3dLimbMat);
		Matrix limbMat;
		limbMat.convertD3DMatrix(&d3dLimbMat);

		for(int i=0;i<numVerts;i++)
		{			
			DWORD px = PhysicsManager::DLL_GetVertexPosX(i);
			DWORD py = PhysicsManager::DLL_GetVertexPosY(i);
			DWORD pz = PhysicsManager::DLL_GetVertexPosZ(i);
			Vector3 p(*((float*)(&px)),*((float*)(&py)),(*((float*)(&pz))));
			p-=limbMat.pos();
			limbMat.rotateV3(&p);

			bool disallow=false;
			for(int j=0;j<curNumColVerts;j++)
			{
				if(p.isEqual(existingPoint[j],0.001f))
				{
					disallow=true;
					break;
				}
			}
			if(!disallow)
			{				
				addNewCollisionPoint(p);
				existingPoint[curNumColVerts++]=p;
			}
		}
		PhysicsManager::DLL_UnlockVertexData();
	}
	SAFEDELETE_ARRAY(existingPoint);
	
	return PERR_AOK;
}

void Vehicle::onUpdateGraphics()
{
	if(drawDebugLines)
	{
		//Render particle positions
		int count=0;
		int n = particleList.Count();
		for(int i=0;i<n;i++)
		{
			D3D_DebugLines::getInst()->drawCross(particleList[i]->wpos,0.03f,0x0ff778899);
		}		

		if(suspensionList.gotoFirstItem())
		{
			do
			{
				suspensionList.getCurItem()->updateGraphics();
			}while(suspensionList.gotoNextItem());
		}
	}

}



void Vehicle::onUpdate()
{
	float iterTime = PhysicsManager::iterationInterval;

	float maxPushDist=-1.0f;
	//float totalSpringForce=0.0f;
	if(suspensionList.gotoFirstItem())
	{
		do
		{
			Suspension * susp = suspensionList.getCurItem();
			susp->Update();
			//if(susp->isOnGround && susp->springForce>0) totalSpringForce+=susp->springForce;
			if(susp->pushDist>maxPushDist) maxPushDist=susp->pushDist;
		}while(suspensionList.gotoNextItem());
	}

	if(maxPushDist>0)
	{
		body.Local_RW().Move(0,maxPushDist,0);
		body.vel.y+=maxPushDist*PhysicsManager::iterationFps;
	}

	float totalSpringForce=(float)fabs(gravity)*body.mass;
	
	if(suspensionList.gotoFirstItem())
	{
		do
		{
			Suspension * susp = suspensionList.getCurItem();
			if(susp->isOnGround)
			{
				//Add in ground reaction to suspension systems - slides down slopes
				if(susp->springForce>0 && totalSpringForce!=0.0f)
				{
					susp->weightShare=susp->springForce/totalSpringForce;
					Vector3 reaction(0,gravity*body.mass*susp->weightShare,0);
					reaction-=susp->groundNorm*susp->groundNorm.Dot(reaction);
					body.addForce(&reaction,WORLD,&susp->wheelBase,WORLD);
				}

				//Wheel forces

				//Sideways frictional force
				Vector3 frc = -susp->velPerp*susp->perpFriction;
				body.addForce(&frc,WORLD,&susp->wheelBase,WORLD);

				//Acceleration
				frc = susp->wheelDir*susp->accel;
				body.addForce(&frc,WORLD,&susp->wheelBase,WORLD);

				//Braking
				frc = -susp->velPar*susp->brake;
				body.addForce(&frc,WORLD,&susp->wheelBase,WORLD);
			}
		}while(suspensionList.gotoNextItem());
	}


	//Gravity
	body.addForce(&Vector3(0,gravity*body.mass,0),WORLD,0,COFG);
	
	//Damping
	body.addTorq(&(body.angVel*-damping),WORLD); 
	//body.addForce(&(body.vel*-damping),WORLD,0,COFG);

	//First calculate the inverse Inertia Tensor at time t based
	//on the Intertia Tensor at identity rotation
	Matrix matRot(body.quatLocal_RO());						//Convert quaternion to matrix
	body.invMofIT = matRot*body.invMofI*matRot.TransposeCopy();

	//First step of integration

	//Integrate to find new velocity
	body.vel = body.vel + body.totForce*body.invMass*iterTime;

	//Integrate to find new angular momentum
	body.angMom = body.angMom + body.totTorq*iterTime;

    //Add in impulses
    body.vel+=body.totImpulse*body.invMass;
    body.angMom+= body.totRotImpulse;


	body.invMofIT.rotateV3(body.angMom,&body.angVel);	//Calculate angular velocity
	body.qLocalVel = 0.5f*Quat(body.angVel)*body.quatLocal_RO();	//Calculate rate of change of rotation quaternion

	//Second step of integration

	//Integrate to find new position
	body.Local_RW().Move(body.vel*iterTime);


	//Find current position of CofG in parent space
    Vector3 oldPCofG=body.getPCofG();

	//Integrate to find new rotation
	body.quatLocal_RW()+=body.qLocalVel*iterTime;

	body.quatLocal_RW().Normalise();

	//Take old pCofG from the new position of CofG in parent space
    oldPCofG-=body.getPCofG();
    body.Local_RW().Move(oldPCofG);

    //body.quatValid_Local=false; //need this to avoid an (apparent) numerical oddity
	
	body.clearForces();
	

	//Ground collision

	int numParticles = particleList.Count();

	float planeDist;
	Vector3 n;
	Vector3 vel;
	float maxPenrt=0.0f;

	for(int i=0;i<numParticles ;i++)
	{	
		//Transform particle
		body.World().rotateAndTransV3(particleList[i]->pos,&particleList[i]->wpos);

		float grndHgt = getGroundHeight(particleList[i]->wpos, &n, &planeDist);

		float grndPenrt=particleList[i]->wpos.y-grndHgt;
		if(grndPenrt<0.0f)
		{					
			if(maxPenrt>grndPenrt) maxPenrt=grndPenrt;
			body.getPointWorldVel(particleList[i]->pos,&vel);

			if(vel.y<0.0f)
			{			
				Vector3 ra=particleList[i]->wpos - body.getPCofG();					
				//Vector3 n(0,1.0f,0);
				Vector3 cp=ra*n;
				body.getInvMofIT().rotateV3(&cp);						
				float denom = n.Dot(cp*ra) + body.getInvMass();
				
				float epsilon = -0.4f;						

				float vrel = n.Dot(vel);			

				float mag = -(1.0f+epsilon)*vrel/denom;

				body.addImpulseAndUpdate(&(n*mag),WORLD,&particleList[i]->wpos,WORLD);						

				//Basic friction
				Vector3 vSurf=-10.0f*vel;
				vSurf.y=0;
				body.addForce(&vSurf,WORLD,&particleList[i]->wpos,WORLD);					
			}
		}										
	}

	if(maxPenrt<0)
	{
		body.Local_RW().Move(0,-maxPenrt,0);	
		body.vel.y+=-maxPenrt*PhysicsManager::iterationFps;
		for(int i=0;i<numParticles ;i++)
		{
			particleList[i]->wpos.y-=maxPenrt;
		}
	}

	//Copy rigid body matrix to vehicle matrix
	m=body.World();

	//Set matrix of chassis object if it exists
	if(chassisObjId!=-1)
	{
		PhysicsManager::DLL_SetWorldMatrix(chassisObjId,m);
	}
}

//Delete object callback
void onVehicleObjectDeletedCallback(int id, int userData)
{
	VehiclePtr tmp;
	if(physics->getVehicle(id,tmp)==PERR_ITEM_DOESNT_EXIST) return;
	if(!tmp->isValid()) return;
	if(tmp->removeBodyObject(userData)!=PERR_AOK)
	{
		tmp->removeWheelObject(userData);
	}
}

ePhysError Vehicle::removeBodyObject(int id)
{
	if(chassisObjId==id) 
	{
		chassisObjId=-1;
		return PERR_AOK;
	}
	return PERR_ITEM_DOESNT_EXIST;
}

ePhysError Vehicle::removeWheelObject(int id)
{
	if(suspensionList.gotoFirstItem())
	{
		do
		{
			if(suspensionList.getCurItem()->wheelObjId==id)
			{
				suspensionList.getCurItem()->wheelObjId=-1;
				return PERR_AOK;
			}			
		}while(suspensionList.gotoNextItem());
	}
	return PERR_ITEM_DOESNT_EXIST;
}

ePhysError Vehicle::setBodyObject(int id)
{
	if(!PhysicsManager::DLL_GetExists(id)) return PERR_ITEM_DOESNT_EXIST;
	chassisObjId=id;
	PhysicsManager::DLL_SetDeleteCallback(id,onVehicleObjectDeletedCallback,objId);
	return PERR_AOK;
}

ePhysError Vehicle::setWheelObject(int suspId, int wheelObjectId, bool flip)
{
	if(!suspensionList.Exists(suspId)) return PERR_SUSPENSION_DOESNT_EXIST;	
	if(!PhysicsManager::DLL_GetExists(wheelObjectId)) return PERR_ITEM_DOESNT_EXIST;
	Suspension * susp = suspensionList.Get(suspId);
	susp->wheelObjId=wheelObjectId;
	susp->isFlipped=flip;
	PhysicsManager::DLL_SetDeleteCallback(wheelObjectId,onVehicleObjectDeletedCallback,objId);
	return PERR_AOK;
}


ePhysError Vehicle::addWheelImpulse(int id, const Vector3& impulse)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;
	Suspension * susp = suspensionList.Get(id);
	//Split force into two components
	Vector3 impulsePar = susp->world.unitY()*susp->world.unitY().Dot(impulse);
	Vector3 impulsePerp = impulse-impulsePar; 
	body.addImpulse(&impulsePar,WORLD,&susp->world.pos(),WORLD); //Non rigid, so apply at suspension fix point
	body.addImpulse(&impulsePerp,WORLD,&susp->wheelBase,WORLD); //Rigid in the XZ plane
	return PERR_AOK;
}

ePhysError Vehicle::addWheelForce(int id, const Vector3& force)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;
	Suspension * susp = suspensionList.Get(id);
	//Split force into two components
	Vector3 forcePar = susp->world.unitY()*susp->world.unitY().Dot(force);
	Vector3 forcePerp = force-forcePar; 
	body.addForce(&forcePar,WORLD,&susp->world.pos(),WORLD); //Non rigid, so apply at suspension fix point
	body.addForce(&forcePerp,WORLD,&susp->wheelBase,WORLD); //Rigid in the XZ plane
	return PERR_AOK;
}


ePhysError Vehicle::setWheelSteer(int id, float angle)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;
	
	Suspension * susp = suspensionList.Get(id);
	
	susp->wheelSteer=TORADIANS(angle);
	return PERR_AOK;
}

ePhysError Vehicle::setWheelControls(int id, float accel, float brake)
{
	if(!suspensionList.Exists(id)) return PERR_SUSPENSION_DOESNT_EXIST;
	
	Suspension * susp = suspensionList.Get(id);
	
	if(brake<0) brake=0;

	susp->accel=accel;
	susp->brake=brake;
	return PERR_AOK;
}

void Vehicle::setVehicleCenterOfGravity(float cx, float cy, float cz)
{
	body.setCofG(&Vector3(cx,cy,cz));
}

ePhysError Vehicle::setVehicleMass(float mass)
{
	if(mass<=0.001f) return PERR_BAD_PARAMETER;
	body.setMass(mass);
	return PERR_AOK;
}

ePhysError Vehicle::setVehicleMomentOfIntertia(float mx, float my, float mz)
{
	if(mx<=0.001f || my<=0.001f || mz<=0.001f) return PERR_BAD_PARAMETER;
	Matrix mofi;
	mofi.e00=mx;
	mofi.e11=my;
	mofi.e22=mz;
	body.setMofI(&mofi);
	return PERR_AOK;
}
