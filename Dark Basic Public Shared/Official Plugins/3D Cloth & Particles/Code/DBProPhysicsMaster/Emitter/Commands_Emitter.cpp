#include "stdafx.h"


DLLEXPORT void deleteEmitter(int id)
{
	ALERT_ON_PERR(physics->deleteEmitter(id),PCMD_DELETE_EMITTER);
}

DLLEXPORT void freezeEmitter(int id, DWORD state)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_FREEZE_EMITTER);
	if(state) tmp->Freeze();
	else tmp->unFreeze();
}

DLLEXPORT void setParticleZSorting(int id, DWORD state)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_PARTICLE_ZSORTING);
	tmp->useZSorting(state?true:false);
}

DLLEXPORT void setRate(int id, float particlesPerSec)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_RATE);
	RETURN_ON_PERR(tmp->setRate(particlesPerSec),PCMD_SET_EMITTER_RATE);
}

DLLEXPORT void setExplode(int id, float param)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_EXPLODE);
	RETURN_ON_PERR(tmp->setExplode(param),PCMD_SET_EMITTER_EXPLODE);
}

DLLEXPORT void setParticleVelocity(int id, float velocity, float percentVar)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_PARTICLE_VELOCITY);
	RETURN_ON_PERR(tmp->setParticleVelocity(velocity,percentVar),PCMD_SET_EMITTER_PARTICLE_VELOCITY);
}

DLLEXPORT void setParticleMass(int id, float mass, float percentVar)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_PARTICLE_MASS);
	RETURN_ON_PERR(tmp->setParticleMass(mass,percentVar),PCMD_SET_EMITTER_PARTICLE_MASS);
}

DLLEXPORT void setParticleSize(int id, float size, float percentVar)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_PARTICLE_SIZE);
	RETURN_ON_PERR(tmp->setParticleSize(size,percentVar),PCMD_SET_EMITTER_PARTICLE_SIZE);
}

DLLEXPORT void setParticleLife(int id, float secs, float percentVar)
{
	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_PARTICLE_LIFE);
	RETURN_ON_PERR(tmp->setParticleLife(secs,percentVar),PCMD_SET_EMITTER_PARTICLE_LIFE);
}

DLLEXPORT void setParticleColor(int id, int R, int G, int B, int A)
{

	EmitterPtr tmp;
	RETURN_ON_PERR(physics->getEmitter(id,tmp),PCMD_SET_EMITTER_PARTICLE_COLOR);

	bool result;
	result |= R<=255 && R>=0;
	result |= G<=255 && G>=0;
	result |= B<=255 && B>=0;
	result |= A<=255 && A>=0;
	if(!result)	RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_SET_EMITTER_PARTICLE_COLOR);	

	unsigned int color = B + (G<<8) + (R<<16) + (A<<24);
	tmp->setColor(color);
}

