
#ifndef _PARTICLE_EMITTER_H
#define _PARTICLE_EMITTER_H

// 2 methods - original particles 'make em all now, move em later'
//				 - new particles make em when we need em'
#define ORIGINAL_PARTICLES

#ifndef ORIGINAL_PARTICLES

namespace PhysX
{
	struct sParticleVertex
	{
		float x, y, z;
		DWORD dwDiffuse;
		float tu, tv;
	};

	//const NxCollisionGroup cParticleCollisionGroup = 1;
	const NxCollisionGroup cParticleCollisionGroup = 1;
	//const NxCollisionGroup cParticleCollisionGroup = 0;

	class Particle 
	{
		public:
			Particle ( );
			Particle ( NxActor* pToSet, NxVec3& vThermoDynamicAcceleration );

			void		update				( NxReal fDeltaTime );
			void		setActor			( NxActor* pToSet );
			NxActor&	getActorReference	( );

		public:
			float		m_fMaxRadius;
			NxActor*	_pActor;
			NxVec3		_vThermoDynamicForce;
			float		m_fLife;
			float		m_fLifeSpan;
			float		m_fCurrentRadius;
			float		m_fCollisionScale;

			DWORD		m_dwDiffuse;
			float		m_fGrowthRate;

			float		m_fDisplayRadius;
	};

	class ParticleEmitter 
	{
		public:
			 ParticleEmitter ( int iMaxParticles, float fEmitRate, NxScene* pSceneToSet, NxVec3& vStartingPos, NxReal fStartingVelScale, NxVec3& vThermoDynamicAcceleration );
			~ParticleEmitter ( );

			void setLinearVelocity	( float fX, float fY, float fZ );
			void setGravity			( int iState );
			void setLifeSpan		( float fLifeSpan );
			void setSize			( float fSize );
			void setMaxSize			( float fSize );
			void setGrowthRate		( float fRate );
			void setPosition		( float fX, float fY, float fZ );

			void setCollisionScale	( float fScale );
			void setCollision		( int iMode );

			void setEmitRate		( float fRate );

			void removeAllParticles	( );
			void update				( NxReal fDeltaTime );
			void addParticle		( );
			void removeParticle		( );

		public:
			NxScene*	_pScene;

			float		m_fInitialSize;
			float		m_fMaxSize;
			int			m_fEmitRate;
			int			m_iGravity;
			float		m_fLifeSpan;
			float		m_fGrowthRate;

			float		m_fCollisionScale;
			
			bool		m_bOverrideLinearVelocity;
			NxVec3		m_vecLinearVelocity;

			int			m_iMaxParticles;
			Particle* _aParticleArray [ 2000 ];
			int			_iHeadIndex;
			int			_iTailIndex;
			int			_iParticleCount;
			NxVec3		_vStartingPos;
			NxReal		_fStartingVelScale;
			NxReal		_fNewParticleTimer;
			NxVec3		_vThermoDynamicAcceleration;

			bool		m_bRange;
			float		m_fRangeX1;
			float		m_fRangeY1;
			float		m_fRangeZ1;
			float		m_fRangeX2;
			float		m_fRangeY2;
			float		m_fRangeZ2;
	};
}

#else


namespace PhysX
{
	struct sParticleVertex
	{
		float x, y, z;
		DWORD dwDiffuse;
		float tu, tv;
	};

	//const NxCollisionGroup cParticleCollisionGroup = 1;
	const NxCollisionGroup cParticleCollisionGroup = 1;
	//const NxCollisionGroup cParticleCollisionGroup = 0;

	class Particle 
	{
		public:
			Particle ( );
			Particle ( NxActor* pToSet, NxVec3& vThermoDynamicAcceleration );

			void		update				( NxReal fDeltaTime );
			void		setActor			( NxActor* pToSet );
			NxActor&	getActorReference	( );

		public:
			float		m_fMaxRadius;
			NxActor*	_pActor;
			NxVec3		_vThermoDynamicForce;
			float		m_fLife;
			float		m_fLifeSpan;
			float		m_fCurrentRadius;
			float		m_fCollisionScale;

			DWORD		m_dwDiffuse;
			float		m_fGrowthRate;

			float		m_fDisplayRadius;
	};

	class ParticleEmitter 
	{
		public:
			 ParticleEmitter ( int iMaxParticles, float fEmitRate, NxScene* pSceneToSet, NxVec3& vStartingPos, NxReal fStartingVelScale, NxVec3& vThermoDynamicAcceleration );
			~ParticleEmitter ( );

			void setLinearVelocity	( float fX, float fY, float fZ );
			void setGravity			( int iState );
			void setLifeSpan		( float fLifeSpan );
			void setSize			( float fSize );
			void setMaxSize			( float fSize );
			void setGrowthRate		( float fRate );
			void setPosition		( float fX, float fY, float fZ );

			void setCollisionScale	( float fScale );
			void setCollision		( int iMode );

			void setEmitRate		( float fRate );

			void removeAllParticles	( );
			void update				( NxReal fDeltaTime );
			void addParticle		( );
			void removeParticle		( );

		public:
			NxScene*	_pScene;

			NxActor**	m_ppActorList;

			float		m_fInitialSize;
			float		m_fMaxSize;
			int			m_fEmitRate;
			int			m_iGravity;
			float		m_fLifeSpan;
			float		m_fGrowthRate;

			float		m_fCollisionScale;
			
			bool		m_bOverrideLinearVelocity;
			NxVec3		m_vecLinearVelocity;

			int			m_iMaxParticles;
			Particle**	_aParticleArray;
			int			_iHeadIndex;
			int			_iTailIndex;
			int			_iParticleCount;
			NxVec3		_vStartingPos;
			NxReal		_fStartingVelScale;
			NxReal		_fNewParticleTimer;
			NxVec3		_vThermoDynamicAcceleration;

			bool		m_bRange;
			float		m_fRangeX1;
			float		m_fRangeY1;
			float		m_fRangeZ1;
			float		m_fRangeX2;
			float		m_fRangeY2;
			float		m_fRangeZ2;
	};
}

#endif

#endif