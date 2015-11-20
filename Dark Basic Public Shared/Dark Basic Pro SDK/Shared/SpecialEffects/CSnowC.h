
#ifndef __SNOWSTORM_H_INCLUDED__
#define __SNOWSTORM_H_INCLUDED__


#include "CBaseParticleC.h"


const D3DXVECTOR3 SNOWFLAKE_VELOCITY  ( 0.0f, -3.0f, 0.0f );
const D3DXVECTOR3 VELOCITY_VARIATION  ( 1.2f,  1.5f, 1.2f );
const float       SNOWFLAKE_SIZE      = 0.08f;
const float       SNOWFLAKES_PER_SEC  = 4000;

#define FRAND   (((float)rand()-(float)rand())/RAND_MAX)

class CSnowstorm : public CParticleSystem
{
	public:
		CSnowstorm ( int maxParticles, D3DXVECTOR3 origin, float height, float width, float depth, bool rev );

		void Update ( float elapsedTime, int iExternalNumberOfEmissions );
		void Render ( );

		void InitializeSystem ( int ImageIndex );
		void KillSystem ( );

	public:
		void    InitializeParticle ( int index );
		float   m_height;
		float   m_width;
		float   m_depth;
		bool	m_bRev;

		LPDIRECT3DTEXTURE9		m_lpTexture;
};

#endif