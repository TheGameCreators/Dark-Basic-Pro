#include "stdafx.h"


DECLARE_PLUGIN(Emitter_Box);

const int Emitter_Box::classID='BBOX';

void Emitter_Box::initParticle(Particle * p)
{	
	p->lastPos.x=min.x+RAND(size.x);
	p->lastPos.y=min.y+RAND(size.y);
	p->lastPos.z=min.z+RAND(size.z);	

	m.rotateAndTransV3(&p->lastPos);

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
		p->pos=p->lastPos-m.pos();
		p->pos.Normalise(); //No way to avoid this as far as I can tell		
	}
	else
	{
		p->pos=m.unitY();
	}
	p->pos*=(1.0f-explode);

	int dir = 3*(rand()%LookupTables::numSphereCoords);
	p->pos.x+=LookupTables::sphereCoords[dir]*explode;
	p->pos.y+=LookupTables::sphereCoords[dir+1]*explode;
	p->pos.z+=LookupTables::sphereCoords[dir+2]*explode;
	p->pos*=r_distPerFrame;		
	p->pos+=p->lastPos;
}

ePhysError Emitter_Box::setBox(float sx,float sy,float sz)
{
	if(sx<=0 || sy<=0 || sz<=0) return PERR_BAD_PARAMETER;
		
	size.Set(sx,sy,sz);
	min.Set(sx*-0.5f,sy*-0.5f,sz*-0.5f);
	return PERR_AOK;
}


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeBoxEmitter(int id, int numParticles)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEmitter(id,numParticles,EMITTER_PLUGIN(Emitter_Box)),PCMD_MAKE_BOX_EMITTER);
}

DLLEXPORT void setSpawnBox(int id, float sx, float sy, float sz)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_SPAWN_BOX);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Emitter::classID,Emitter_Box::classID),PCMD_SET_SPAWN_BOX);


	Emitter_Box * box = reinterpret_cast<Emitter_Box *>(&(*tmp));
	RETURN_ON_PERR(box->setBox(sx,sy,sz),PCMD_SET_SPAWN_BOX);
}

DLLEXPORT void setRadialEmission(int id, DWORD state)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_RADIAL_EMISSION);

	// leefix - 030306 - u60 - uncommented this code, it works fine it seems
	if(tmp->checkType(Emitter::classID,Emitter_Sphere::classID)==PERR_AOK)
	{
		Emitter_Sphere * sph = reinterpret_cast<Emitter_Sphere *>(&(*tmp));
		if(state) sph->setRadialEmission(true);
		else sph->setRadialEmission(false);
	}
	else if(tmp->checkType(Emitter::classID,Emitter_Box::classID)==PERR_AOK)
	{
		Emitter_Box * box = reinterpret_cast<Emitter_Box *>(&(*tmp));
		if(state) box->setRadialEmission(true);
		else box->setRadialEmission(false);		
	}
	else
	{
		RETURN_ON_PERR(PERR_WRONG_TYPE,PCMD_SET_RADIAL_EMISSION);
	}
}
