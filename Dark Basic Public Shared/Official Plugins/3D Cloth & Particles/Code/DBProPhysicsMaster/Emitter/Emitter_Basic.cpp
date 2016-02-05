#include "stdafx.h"


DECLARE_PLUGIN(Emitter_Basic);

const int Emitter_Basic::classID='BEMT';

// mike
bool bTrash = false;
float fX = -5.0f;
float fY =  7.0f;
float fZ =  0.0f;
/////////

void Emitter_Basic::initParticle(Particle * p)
{
	// mike
	if ( bTrash )
	{
	//	m.setPos ( fX, fY, fZ );
	}
	/////////

	p->lastPos=m.pos();

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

	int dir = 3*(rand()%LookupTables::numSphereCoords);

	//Linear combination of up vector and a random unit vector based on "explode"
	//leenote -030306 - u60 - m.unitY() controls the up vector (which is based on the world matrix rotation)
	p->pos=m.unitY()*(1.0f-explode);
	p->pos.x+=LookupTables::sphereCoords[dir]*explode;
	p->pos.y+=LookupTables::sphereCoords[dir+1]*explode;
	p->pos.z+=LookupTables::sphereCoords[dir+2]*explode;

	p->pos*=r_distPerFrame;		
	p->pos+=p->lastPos;
}



//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeEmitter(int id, int numParticles)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	// mike
	bTrash = false;

	/////////
	if ( id == 1 )
		bTrash = true;

	ALERT_ON_PERR(physics->addEmitter(id,numParticles,EMITTER_PLUGIN(Emitter_Basic)),PCMD_MAKE_EMITTER);
}
