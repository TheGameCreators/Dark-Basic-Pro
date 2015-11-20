#include <d3d9.h>
#include <D3DX9.h>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h>
#include <iostream>

struct particle_t
{
	D3DXVECTOR3	m_pos;				// current position of the particle
	D3DXVECTOR3	m_prevPos;			// last position of the particle
	D3DXVECTOR3	m_velocity;			// direction and speed
	D3DXVECTOR3	m_acceleration;		// acceleration

	float		m_energy;           // determines how long the particle is alive

	float		m_size;             // size of particle
	float		m_sizeDelta;        // amount to change the size over time

	float		m_weight;           // determines how gravity affects the particle
	float		m_weightDelta;      // change over time

	float		m_color      [ 4 ]; // current color of the particle
	float		m_colorDelta [ 4 ];	// how the color changes with time
};

class CParticleSystem
{
	public:
		CParticleSystem ( int maxParticles, D3DXVECTOR3 origin );

		// abstract functions
		virtual void  Update ( float elapsedTime, int iParticlesToEmitMax )     = 0;
		virtual void  Render ( )                       = 0;

		virtual int   Emit ( int numParticles );

		virtual void  InitializeSystem ( );
		virtual void  KillSystem ( );

	protected:
		virtual void  InitializeParticle ( int index ) = 0;
		
		particle_t*		m_particleList;							// particles for this emitter
		int				m_maxParticles;							// maximum number of particles in total
		int				m_numParticles;							// indicies of all free particles
		D3DXVECTOR3		m_origin;								// center of the particle system

		float			m_accumulatedTime;						// used to track how long since the last particle was emitted

		D3DXVECTOR3		m_force;								// force ( gravity, wind, etc.) acting on the particle system
};
