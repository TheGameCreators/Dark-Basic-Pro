#include "stdafx.h"

DLL_MAKEEMITTER_FP Emitter::DLL_MakeEmitter=0;
DLL_DELETEEMITTER_FP Emitter::DLL_DeleteEmitter=0;
DLL_GETEMITTERDATA_FP Emitter::DLL_GetEmitterData=0;
DLL_UPDATEEMITTER_FP Emitter::DLL_UpdateEmitter=0;

const int Emitter::classID='EMIT';

Emitter::Emitter(int id, int num)
:PhysicsObject(id),
emitting(true),frozen(false),useZSort(false),rate(1.0f),
distPerFrame_Init(0.1f),mass_Init(1.0f),size_Init(1.0f),energy_Init(100),color_Init(0x0ffffffff),
distPerFrame_varAmt(0),mass_varAmt(0),size_varAmt(0),energy_varAmt(0),
explode(0.3f),numAlive(0),fracWaiting(0),damping(0),numParticles(num),depthList(0)
{
	useExternalMatrix(true);

	for(int i=0;i<num;i++)
	{
		particlesDead.Add(new Particle);
	}
}

Emitter::~Emitter()
{
	particlesDead.deleteAndClear();
	particlesAlive.deleteAndClear();	
	SAFEDELETE_ARRAY(depthList);
}


void Emitter::onUpdate()
{
	if(frozen) return;

	ParticleLink curLink;

	//Calculate number of particles to emit
	int numNewParticles=0;
	fracWaiting+=rate;	//Use this to provide smooth particle emission for a rate < 1.0
	if(fracWaiting>=1.0f)
	{
		numNewParticles=int(fracWaiting);	//Truncate float
		fracWaiting-=numNewParticles;
	}

	if(emitting)
	{
		//Check how many dead particles we're able to emit
		int numDead = particlesDead.Count();
		if(numDead<numNewParticles) numNewParticles=numDead;		

		while(numNewParticles>0)
		{
			particlesDead.gotoFirstItem();
			curLink = particlesDead.getCurLink();

			particlesDead.RemoveLink(curLink);
			particlesAlive.AddLink(curLink);
			initParticle(curLink->data);

			numNewParticles--;
		}
	}

	float timeIntSqr = PhysicsManager::iterationInterval;
	timeIntSqr*=timeIntSqr;		

	Vector3 tempPos;
	Particle * particle;

	if(particlesAlive.gotoFirstItem())
	{
		do
		{
			curLink = particlesAlive.getCurLink();
			particle=curLink->data;

			if(particle->energy==0)
			{
				//Destroy old particles
				particlesAlive.RemoveLink(curLink);
				particlesDead.AddLink(curLink);				
			}
			else
			{				
				tempPos = (2.0f-damping)*particle->pos - (1.0f-damping)*particle->lastPos + (particle->force*particle->invMass + gravity)*timeIntSqr;
				particle->lastPos=particle->pos;
				particle->pos=tempPos;
				if(particle->energy>0) particle->energy--;
				particle->frameCount++;
				particle->force.x=particle->force.y=particle->force.z=0;
				//particle->flags&=255-PARTICLEDATA_COLLIDED;	//Clear the collision flag
			}
		}while(particlesAlive.gotoNextItem());
	}
}

void Emitter::onUpdateGraphics()
{
	if(frozen) return;

	if(useZSort) 
	{
		onUpdateGraphicsZSort();
		return;
	}


	DWORD vertCount = 0;
	int * drawCount = 0;
	BYTE * _particles = 0;
	Particle * curParticle;
	DLL_GetEmitterData(objId, &_particles, &vertCount, &drawCount);
	*drawCount = particlesAlive.Count()*2;

	Vector3 norm(*(Vector3*)&PhysicsManager::DLL_GetCameraLook());
	Vector3 right(*(Vector3*)&PhysicsManager::DLL_GetCameraRight());
	Vector3 up(*(Vector3*)&PhysicsManager::DLL_GetCameraUp());
	Vector3 cpos(*(Vector3*)&PhysicsManager::DLL_GetCameraPosition());

	norm=-norm;

	PhysicsManager::DLL_LockVertexData(objId,0);

	int pIndex=0;
	int iIndex=0;

	Vector3 r,u,p0,p1,p2,p3;
	if(particlesAlive.gotoFirstItem())
	{
		do
		{
			curParticle = particlesAlive.getCurObject();
			const Vector3& center = curParticle->pos;
			r=right*curParticle->size*0.5f;
			u=up*curParticle->size*0.5f;

			p0=center-r+u;
			p1=center+r+u;
			p2=center+r-u;
			p3=center-r-u;		
			
			PhysicsManager::DLL_SetVertexPosition(pIndex, p0.x, p0.y, p0.z);
			PhysicsManager::DLL_SetVertexPosition(pIndex+1, p1.x, p1.y, p1.z);
			PhysicsManager::DLL_SetVertexPosition(pIndex+2, p2.x, p2.y, p2.z);
			PhysicsManager::DLL_SetVertexPosition(pIndex+3, p3.x, p3.y, p3.z);

			PhysicsManager::DLL_SetVertexNormal(pIndex, norm.x,norm.y,norm.z);
			PhysicsManager::DLL_SetVertexNormal(pIndex+1, norm.x,norm.y,norm.z);
			PhysicsManager::DLL_SetVertexNormal(pIndex+2, norm.x,norm.y,norm.z);
			PhysicsManager::DLL_SetVertexNormal(pIndex+3, norm.x,norm.y,norm.z);

			PhysicsManager::DLL_SetVertexDiffuse(pIndex,curParticle->color);
			PhysicsManager::DLL_SetVertexDiffuse(pIndex+1,curParticle->color);
			PhysicsManager::DLL_SetVertexDiffuse(pIndex+2,curParticle->color);
			PhysicsManager::DLL_SetVertexDiffuse(pIndex+3,curParticle->color);

			PhysicsManager::DLL_SetVertexUV(pIndex,0,0);
			PhysicsManager::DLL_SetVertexUV(pIndex+1,1,0);
			PhysicsManager::DLL_SetVertexUV(pIndex+2,1,1);
			PhysicsManager::DLL_SetVertexUV(pIndex+3,0,1);
			
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex );
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex+1 );
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex+2 );
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex+2 );
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex+3 );
			PhysicsManager::DLL_SetIndex( iIndex++, pIndex );

			pIndex+=4;
		}while(particlesAlive.gotoNextItem());
	}

	PhysicsManager::DLL_UnlockVertexData();	

	if(isFirstGFXUpdate) 
	{
		PhysicsManager::DLL_SetNewObjectFinalProperties(objId, 150);		
	}
}

int particleDepthSort(const void * a,const void * b)
{
	ParticleDepth * p = (ParticleDepth *) a;
	ParticleDepth * q = (ParticleDepth *) b;
	if(p->depth>q->depth) return -1;
	if(p->depth<q->depth) return 1;
	return 0;
}

void Emitter::onUpdateGraphicsZSort()
{
	DWORD vertCount = 0;
	int * drawCount = 0;
	BYTE * _particles = 0;
	Particle * curParticle;
	DLL_GetEmitterData(objId, &_particles, &vertCount, &drawCount);
	*drawCount = particlesAlive.Count()*2;

	Vector3 norm(*(Vector3*)&PhysicsManager::DLL_GetCameraLook());
	Vector3 right(*(Vector3*)&PhysicsManager::DLL_GetCameraRight());
	Vector3 up(*(Vector3*)&PhysicsManager::DLL_GetCameraUp());
	Vector3 cpos(*(Vector3*)&PhysicsManager::DLL_GetCameraPosition());

	int count=0;
	if(particlesAlive.gotoFirstItem())
	{
		do
		{
			depthList[count].particle=particlesAlive.getCurObject();
			Vector3 delta = particlesAlive.getCurObject()->pos - cpos;
			depthList[count].depth = delta.Dot(norm);
			count++;
		}while(particlesAlive.gotoNextItem());
	}

	qsort(depthList,count,sizeof(ParticleDepth),particleDepthSort);

	norm=-norm;

	PhysicsManager::DLL_LockVertexData(objId,0);

	int pIndex=0;
	int iIndex=0;

	Vector3 r,u,p0,p1,p2,p3;
	
	for(int i=0;i<count;i++)
	{
		curParticle = depthList[i].particle;
		const Vector3& center = curParticle->pos;
		r=right*curParticle->size*0.5f;
		u=up*curParticle->size*0.5f;

		p0=center-r+u;
		p1=center+r+u;
		p2=center+r-u;
		p3=center-r-u;		
		
		PhysicsManager::DLL_SetVertexPosition(pIndex, p0.x, p0.y, p0.z);
		PhysicsManager::DLL_SetVertexPosition(pIndex+1, p1.x, p1.y, p1.z);
		PhysicsManager::DLL_SetVertexPosition(pIndex+2, p2.x, p2.y, p2.z);
		PhysicsManager::DLL_SetVertexPosition(pIndex+3, p3.x, p3.y, p3.z);

		PhysicsManager::DLL_SetVertexNormal(pIndex, norm.x,norm.y,norm.z);
		PhysicsManager::DLL_SetVertexNormal(pIndex+1, norm.x,norm.y,norm.z);
		PhysicsManager::DLL_SetVertexNormal(pIndex+2, norm.x,norm.y,norm.z);
		PhysicsManager::DLL_SetVertexNormal(pIndex+3, norm.x,norm.y,norm.z);

		PhysicsManager::DLL_SetVertexDiffuse(pIndex,curParticle->color);
		PhysicsManager::DLL_SetVertexDiffuse(pIndex+1,curParticle->color);
		PhysicsManager::DLL_SetVertexDiffuse(pIndex+2,curParticle->color);
		PhysicsManager::DLL_SetVertexDiffuse(pIndex+3,curParticle->color);

		PhysicsManager::DLL_SetVertexUV(pIndex,0,0);
		PhysicsManager::DLL_SetVertexUV(pIndex+1,1,0);
		PhysicsManager::DLL_SetVertexUV(pIndex+2,1,1);
		PhysicsManager::DLL_SetVertexUV(pIndex+3,0,1);
		
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex );
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex+1 );
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex+2 );
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex+2 );
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex+3 );
		PhysicsManager::DLL_SetIndex( iIndex++, pIndex );

		pIndex+=4;
	}

	PhysicsManager::DLL_UnlockVertexData();	

	// leefix - 030306 - u60 - forgot this condition, kept reseting everything
	if(isFirstGFXUpdate) 
	{
		PhysicsManager::DLL_SetNewObjectFinalProperties(objId, 150);
	}
}

void Emitter::useZSorting(bool state)
{
	useZSort = state;
	SAFEDELETE_ARRAY(depthList);
	if(state) depthList = new ParticleDepth[numParticles];
}


ePhysError Emitter::setRate(float particlesPerSec)
{
	if(particlesPerSec>=0)
	{
		rate = particlesPerSec*PhysicsManager::mainTimeInterval/PhysicsManager::numIterations_Particles;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

ePhysError Emitter::setExplode(float param)
{
	if(param>=0) 
	{
		explode=param;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

ePhysError Emitter::setParticleVelocity(float velocity, float percentVar)
{
	if(percentVar>=0 && percentVar<=100)
	{
		//Manipulate the input params to give us numbers
		//for use in the form: value = (init) + (varAmt)*rand(0->1)
		//This will mimic the desired response of: value = x (+/-)y%
		distPerFrame_Init=velocity*PhysicsManager::mainTimeInterval/PhysicsManager::numIterations_Particles;
		distPerFrame_varAmt=distPerFrame_Init*percentVar*0.01f;
		distPerFrame_Init-=distPerFrame_varAmt;		
		distPerFrame_varAmt*=2;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

ePhysError Emitter::setParticleMass(float mass, float percentVar)
{	
	if(mass>=0 && percentVar>=0 && percentVar<=100)
	{		
		mass_varAmt=mass*percentVar*0.01f;
		mass_Init=mass-mass_varAmt;		
		mass_varAmt*=2;
		if(mass_Init<0.0f) return PERR_BAD_PARAMETER;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

ePhysError Emitter::setParticleSize(float size, float percentVar)
{
	if(size>=0 && percentVar>=0 && percentVar<=100)
	{
		size_varAmt=size*percentVar*0.01f;
		size_Init=size-size_varAmt;		
		size_varAmt*=2;
		if(size_Init<0.0f) return PERR_BAD_PARAMETER;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

ePhysError Emitter::setParticleLife(float secs, float percentVar)
{
	if(secs>=0 && percentVar>=0 && percentVar<=100)
	{
		energy_Init=secs/(PhysicsManager::mainTimeInterval/PhysicsManager::numIterations_Particles);
		energy_varAmt=energy_Init*percentVar*0.01f;
		energy_Init-=energy_varAmt;
		energy_varAmt*=2;
		if(energy_Init<0.0f) return PERR_BAD_PARAMETER;
		return PERR_AOK;
	}
	else if(secs<0)
	{
		//Never kill the particles
		energy_Init=-1.0f;
		energy_varAmt=0;
		return PERR_AOK;
	}
	return PERR_BAD_PARAMETER;
}

