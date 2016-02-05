#include "stdafx.h"


DECLARE_PLUGIN(Emitter_Sphere);

const int Emitter_Sphere::classID='SPHR';

void Emitter_Sphere::initParticle(Particle * p)
{	
	int posDir = 3*(rand()%LookupTables::numSphereCoords);
	float rad = spawnRadius + RAND(spawnRadiusVar);
	p->lastPos.x=rad*LookupTables::sphereCoords[posDir];
	p->lastPos.y=rad*LookupTables::sphereCoords[posDir+1];
	p->lastPos.z=rad*LookupTables::sphereCoords[posDir+2];
	if(useHemisph)
	{
		if(p->lastPos.y<0) p->lastPos.y=-p->lastPos.y;
		m.rotateAndTransV3(&p->lastPos);
	}
	else
	{
		p->lastPos+=m.pos();
	}

	p->force=gravity;

	float r_distPerFrame = distPerFrame_Init + RAND(distPerFrame_varAmt);
	float r_mass = mass_Init + RAND(mass_varAmt);
	float r_size = size_Init + RAND(size_varAmt);
	float r_energy = energy_Init + RAND(energy_varAmt);	

	if(r_mass>0) p->invMass=1.0f/r_mass;
	else p->invMass=0;
	p->size=r_size;
	p->energy=int(r_energy);
	p->frameCount=0;
	p->color=color_Init;
	p->flags=0;
		
	//Linear combination of out vector and a random unit vector based on "explode"	
	if(radialEmit)
	{
		p->pos.x=LookupTables::sphereCoords[posDir]*(1.0f-explode);
		p->pos.y=LookupTables::sphereCoords[posDir+1]*(1.0f-explode);
		p->pos.z=LookupTables::sphereCoords[posDir+2]*(1.0f-explode);
		if(useHemisph && p->pos.y<0) p->pos.y=-p->pos.y;
	}
	else
	{
		p->pos=m.unitY();
		p->pos*=(1.0f-explode);
	}
	
	int dir = 3*(rand()%LookupTables::numSphereCoords);
	p->pos.x+=LookupTables::sphereCoords[dir]*explode;
	p->pos.y+=LookupTables::sphereCoords[dir+1]*explode;
	p->pos.z+=LookupTables::sphereCoords[dir+2]*explode;
	p->pos*=r_distPerFrame;		
	p->pos+=p->lastPos;
}

ePhysError Emitter_Sphere::setRadius(float minRad, float maxRad)
{
	if(minRad<0 || maxRad<0 || minRad>=maxRad) return PERR_BAD_PARAMETER;
	spawnRadius=minRad;
	spawnRadiusVar=maxRad-minRad;
	return PERR_AOK;
}

//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeSphericalEmitter(int id, int numParticles)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEmitter(id,numParticles,EMITTER_PLUGIN(Emitter_Sphere)),PCMD_MAKE_SPHERICAL_EMITTER);
}

DLLEXPORT void setSpawnRadius(int id, float minRadius, float maxRadius)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_SPAWN_RADIUS);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Emitter::classID,Emitter_Sphere::classID),PCMD_SET_SPAWN_RADIUS);

	Emitter_Sphere * sph = reinterpret_cast<Emitter_Sphere *>(&(*tmp));
	RETURN_ON_PERR(sph->setRadius(minRadius,maxRadius),PCMD_SET_SPAWN_RADIUS);
}

DLLEXPORT void setSpawnHemisphere(int id, DWORD state)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_SPAWN_HEMISPHERE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Emitter::classID,Emitter_Sphere::classID),PCMD_SET_SPAWN_HEMISPHERE);

	Emitter_Sphere * sph = reinterpret_cast<Emitter_Sphere *>(&(*tmp));
	if(state) sph->useHemisphere(true);
	else sph->useHemisphere(false);
}