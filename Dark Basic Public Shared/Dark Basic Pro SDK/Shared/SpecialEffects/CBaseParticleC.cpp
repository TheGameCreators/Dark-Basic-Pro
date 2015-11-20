
#include "CBaseParticleC.h"

CParticleSystem::CParticleSystem ( int maxParticles, D3DXVECTOR3 origin )
{
	// store initialization values and set defaults
	m_maxParticles = maxParticles;
	m_origin       = origin;
	m_particleList = NULL;
}

int CParticleSystem::Emit ( int numParticles )
{
	// creates the number of new particles specified by the parameter, using
	// the general particle system values with some random element. Note that
	// only initial values will be randomized

	// create numParticles new particles ( if there's room )
	while ( numParticles && ( m_numParticles < m_maxParticles ) )
	{
		// initialize the current particle and increase the count
		InitializeParticle ( m_numParticles++ );
		--numParticles;
	}

	return numParticles;
}

void CParticleSystem::InitializeSystem ( )
{
	// Allocate memory for the maximum number of particles in the system

	// if this is just a reset, free the memory
	if ( m_particleList )
	{
		delete [ ] m_particleList;
		m_particleList = NULL;
	}

	// allocate the maximum number of particles
	m_particleList = new particle_t [ m_maxParticles ];

	// reset the number of particles and accumulated time
	m_numParticles    = 0;
	m_accumulatedTime = 0.0f;
}

void CParticleSystem::KillSystem ( )
{
	// Tells the emitter to stop emitting. If the parameter is true, all live
	// particles are killed as well.  Otherwise, they are allowed to die off on
	// their own.

	if ( m_particleList )
	{
		delete [ ] m_particleList;
		m_particleList = NULL;
	}

	m_numParticles = 0;
}