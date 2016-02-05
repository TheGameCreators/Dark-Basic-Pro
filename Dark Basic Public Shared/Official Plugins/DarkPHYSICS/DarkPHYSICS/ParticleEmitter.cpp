
#include "globals.h"
#include "particleemitter.h"

using namespace PhysX;

#ifndef ORIGINAL_PARTICLES

Particle::Particle ( )
{
	setActor ( NULL );
	_vThermoDynamicForce.zero ( );
	m_fLife = 1.0f;
	m_fLifeSpan = 0.01f;
	m_fMaxRadius = 1.0f;
	m_dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	m_fGrowthRate = 0.1f;
	m_fCollisionScale = 1.0f;
	m_fDisplayRadius = 1.0f;
}

Particle::Particle ( NxActor* pToSet, NxVec3& vThermoDynamicAcceleration )
{
	setActor ( pToSet );
	_vThermoDynamicForce = pToSet->getMass ( ) * vThermoDynamicAcceleration;
	m_fLife = 1.0f;
	m_fLifeSpan = 0.01f;
	m_fMaxRadius = 1.0f;
	m_dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	m_fGrowthRate = 0.1f;
	m_fCollisionScale = 1.0f;
	m_fDisplayRadius = 1.0f;
}

void Particle::setActor ( NxActor* pToSet )
{
	_pActor = pToSet;
}

void Particle::update ( NxReal fDeltaTime )
{
	_pActor->addForce ( _vThermoDynamicForce );

	NxSphereShape* pSphere = _pActor->getShapes ( ) [ 0 ]->isSphere ( );

	if ( m_fLife > 0.0f )
	{
		m_fLife -= m_fLifeSpan;
	}

	if ( pSphere )
	{
		m_fCurrentRadius = m_fDisplayRadius;

		if ( m_fCurrentRadius < m_fMaxRadius )
		{
			m_fDisplayRadius = ( m_fDisplayRadius + m_fGrowthRate * fDeltaTime );

			pSphere->setRadius ( m_fCurrentRadius + m_fGrowthRate * fDeltaTime );

			float fCurrentSize = pSphere->getRadius ( );
			float fNewSize = pSphere->getRadius ( ) * m_fCollisionScale;

			pSphere->setRadius ( ( pSphere->getRadius ( ) * m_fCollisionScale ) );
			
			if ( pSphere->getRadius ( ) > m_fMaxRadius )
				pSphere->setRadius ( m_fMaxRadius );
		}

		if ( pSphere->getRadius ( ) > m_fMaxRadius )
			pSphere->setRadius ( m_fMaxRadius );
	}
}

NxActor& Particle::getActorReference ( )
{
	return *_pActor;
}

ParticleEmitter::ParticleEmitter ( int iMaxParticles, float fEmitRate, NxScene* pSceneToSet, NxVec3& vStartingPos, NxReal fStartingVelScale, NxVec3& vThermoDynamicAcceleration )
{
	//_aParticleArray					= new Particle* [ iMaxParticles ];
	m_iMaxParticles					= iMaxParticles;
	m_fEmitRate						= ( int ) fEmitRate;
	_iHeadIndex						= 0;
	_iTailIndex						= 0;
	_iParticleCount					= 0;
	_pScene							= pSceneToSet;
	_vStartingPos					= vStartingPos;
	_fStartingVelScale				= ( 1.0f / NxReal ( RAND_MAX ) ) * fStartingVelScale;
	_fNewParticleTimer				= 0.0f;
	_vThermoDynamicAcceleration		= vThermoDynamicAcceleration;
	m_iGravity						= 0;
	m_bOverrideLinearVelocity		= false;
	m_fLifeSpan						= 0.01f;
	m_fGrowthRate					= 0.1f;
	m_bRange						= false;
	m_fRangeX1						= 0.0f;
	m_fRangeY1						= 0.0f;
	m_fRangeZ1						= 0.0f;
	m_fRangeX2						= 0.0f;
	m_fRangeY1						= 0.0f;
	m_fRangeZ1						= 0.0f;
	m_fMaxSize						= 1.0f;
	m_fCollisionScale				= 1.0f;
}

void ParticleEmitter::setCollision ( int iMode )
{
	// mode = 0, collide with each other
	// mode = 1, collide with scenery
	// mode = 2, no collision

	/*
	if ( iMode == 0 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 3 );

			NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 3 );
			}
		}

		PhysX::pScene->setGroupCollisionFlag ( 3, 3, true );
		PhysX::pScene->setGroupCollisionFlag ( 3, 0, true );
		PhysX::pScene->setGroupCollisionFlag ( 3, 2, false );
	}

	if ( iMode == 1 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 1 );

		    NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 1 );
			}

		}
	}

	if ( iMode == 2 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 2 );

			NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 2 );
			}
		}

		PhysX::pScene->setGroupCollisionFlag ( 2, 2, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 0, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 3, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 1, false );
	}
	*/
}

ParticleEmitter::~ParticleEmitter ( )
{
	/*
	for ( int i = 0; i < m_iMaxParticles; i++ )
	{
		m_ppActorList [ i ]->setGlobalPosition ( NxVec3 ( 0, 0, 0 ) );

		_pScene->releaseActor ( *m_ppActorList [ i ] );
		delete _aParticleArray [ i ];
	}

	if ( m_ppActorList )
		delete [ ] m_ppActorList;
	*/
}

void ParticleEmitter::setLifeSpan ( float fLifeSpan )
{
	m_fLifeSpan = fLifeSpan;
}

void ParticleEmitter::setSize ( float fSize )
{
	m_fInitialSize = fSize;	
}

void ParticleEmitter::setMaxSize ( float fSize )
{
	m_fMaxSize = fSize;	
}

void ParticleEmitter::setGrowthRate ( float fRate )
{
	m_fGrowthRate = fRate;
}

void ParticleEmitter::removeAllParticles ( )
{
	while ( _iParticleCount > 0 )
		removeParticle ( );
}

void ParticleEmitter::update ( NxReal fDeltaTime )
{
	_fNewParticleTimer -= fDeltaTime;

	if ( _fNewParticleTimer < 0.0f )
	{
		addParticle ( );
		_fNewParticleTimer += m_fEmitRate;
	}

	int iParticlesUpdated = 0;

	for ( int iParticleIndex = _iTailIndex; iParticlesUpdated < _iParticleCount; iParticleIndex = ( ( iParticleIndex + 1 ) % m_iMaxParticles ) )
	{
		_aParticleArray [ iParticleIndex ]->m_fMaxRadius  = m_fMaxSize;
		_aParticleArray [ iParticleIndex ]->m_fLifeSpan   = m_fLifeSpan;
		_aParticleArray [ iParticleIndex ]->m_fGrowthRate = m_fGrowthRate;

		_aParticleArray [ iParticleIndex ]->m_fCollisionScale  = m_fCollisionScale;

		_aParticleArray [ iParticleIndex ]->update ( fDeltaTime );
		++iParticlesUpdated;
	}
}

void ParticleEmitter::setCollisionScale	( float fScale )
{
	m_fCollisionScale = fScale;
}

void ParticleEmitter::setEmitRate ( float fRate )
{
	m_fEmitRate = fRate;
}

void ParticleEmitter::setPosition ( float fX, float fY, float fZ )
{
	_vStartingPos = NxVec3 ( fX, fY, fZ );
}

DARKSDK int RndLLEx(int r)
{
	int result=0;
	if(r>0)
	{
		// leefix - 250604 - u54 - 0 to 22 million now
		if ( r>1000 )  result += (rand()*1000);
		if ( r>100 ) result += (rand()*100);
		if ( r>10 ) result += (rand()*10);
		result += rand();
		result %= r+1;
	}
	return result;
}

void ParticleEmitter::addParticle ( )
{
	/*
	if ( ( _iTailIndex == _iHeadIndex ) && ( _iParticleCount != 0 ) )
	{
		removeParticle ( );
	}

	NxActor* pActor = m_ppActorList [ _iHeadIndex ];

	pActor->wakeUp ( );
	pActor->clearBodyFlag ( NX_BF_FROZEN_POS );
	pActor->setGlobalPosition ( _vStartingPos );

	if ( m_bRange )
		pActor->setGlobalPosition ( NxVec3 ( m_fRangeX1 + RndLLEx ( ( int ) m_fRangeX2 ), m_fRangeY1 + RndLLEx ( ( int ) m_fRangeY2 ), m_fRangeZ1 + RndLLEx ( ( int ) m_fRangeZ2 ) ) );

	// give it an initial linear velocity, scaled by _fStartingVelScale
	NxVec3 vRandVec ( NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ) );

	if ( m_bOverrideLinearVelocity )
		pActor->setLinearVelocity ( m_vecLinearVelocity );
	else
		pActor->setLinearVelocity ( vRandVec );

	// turn off gravity if required
	if ( m_iGravity == 0 )
		pActor->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );

	_aParticleArray [ _iHeadIndex ]->setActor ( pActor );

	NxSphereShape* pSphere = pActor->getShapes ( ) [ 0 ]->isSphere ( );
	pSphere->setRadius ( m_fInitialSize * m_fCollisionScale );

	_aParticleArray [ _iHeadIndex ]->m_fDisplayRadius = m_fInitialSize;

	_aParticleArray [ _iHeadIndex ]->_vThermoDynamicForce = pActor->getMass ( ) * _vThermoDynamicAcceleration;

	_aParticleArray [ _iHeadIndex ]->m_fLife = 1.0f;
	_iHeadIndex = ( _iHeadIndex + 1 ) % m_iMaxParticles;
	++_iParticleCount;
	*/

	if ( ( _iTailIndex == _iHeadIndex ) && ( _iParticleCount != 0 ) )
	{
		removeParticle ( );
	}

	NxActorDesc			actorDesc;
	NxBodyDesc			bodyDesc;
	NxSphereShapeDesc	sphereDesc;

	sphereDesc.radius	= m_fInitialSize;
	sphereDesc.group	= cParticleCollisionGroup;

	NxMaterialDesc material;
	NxMaterialIndex materialIndex;
	material.restitution     = 0.0f;
	material.staticFriction  = 1.0f;
	material.dynamicFriction = 1.0f;

	// don't create a material
	// materialIndex = PhysX::pScene->createMaterial ( material )->getMaterialIndex ( );
	// sphereDesc.materialIndex = materialIndex;

	// use standard scene material for now
	sphereDesc.materialIndex = PhysX::SceneMaterialIndex;

	actorDesc.shapes.pushBack ( &sphereDesc );

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 0.01f;
	actorDesc.globalPose.t	= _vStartingPos;

	//if ( m_bRange )
	//	actorDesc.globalPose.t = NxVec3 ( m_fRangeX1 + dbRnd ( m_fRangeX2 ), m_fRangeY1 + dbRnd ( m_fRangeY2 ), m_fRangeZ1 + dbRnd ( m_fRangeZ2 ) );

	NxActor* pNewActor = _pScene->createActor ( actorDesc );

	/*
	if ( !pNewActor )
		return;

	// give it an initial linear velocity, scaled by _fStartingVelScale
	NxVec3 vRandVec ( NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ) );

	if ( m_bOverrideLinearVelocity )
		pNewActor->setLinearVelocity ( m_vecLinearVelocity );
	else
		pNewActor->setLinearVelocity ( vRandVec );

	// turn off gravity if required
	if ( m_iGravity == 0 )
		pNewActor->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );
	*/

	if ( m_bRange )
		pNewActor->setGlobalPosition ( NxVec3 ( m_fRangeX1 + RndLLEx ( ( int ) m_fRangeX2 ), m_fRangeY1 + RndLLEx ( ( int ) m_fRangeY2 ), m_fRangeZ1 + RndLLEx ( ( int ) m_fRangeZ2 ) ) );

	// give it an initial linear velocity, scaled by _fStartingVelScale
	NxVec3 vRandVec ( NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ) );

	if ( m_bOverrideLinearVelocity )
		pNewActor->setLinearVelocity ( m_vecLinearVelocity );
	else
		pNewActor->setLinearVelocity ( vRandVec );

	// turn off gravity if required
	if ( m_iGravity == 0 )
		pNewActor->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );


	_aParticleArray [ _iHeadIndex ] = new Particle ( pNewActor, _vThermoDynamicAcceleration );

	_aParticleArray [ _iHeadIndex ]->setActor ( pNewActor );
	NxSphereShape* pSphere = pNewActor->getShapes ( ) [ 0 ]->isSphere ( );
	pSphere->setRadius ( m_fInitialSize * m_fCollisionScale );
	_aParticleArray [ _iHeadIndex ]->m_fDisplayRadius = m_fInitialSize;
	_aParticleArray [ _iHeadIndex ]->_vThermoDynamicForce = pNewActor->getMass ( ) * _vThermoDynamicAcceleration;

	_aParticleArray [ _iHeadIndex ]->m_fLife = 1.0f;
	_iHeadIndex = ( _iHeadIndex + 1 ) % m_iMaxParticles;
	++_iParticleCount;
}

void ParticleEmitter::setLinearVelocity ( float fX, float fY, float fZ )
{
	m_vecLinearVelocity = NxVec3 ( fX, fY, fZ );
	m_bOverrideLinearVelocity = true;
}

void ParticleEmitter::setGravity ( int iState )
{
	m_iGravity = iState;
}

void ParticleEmitter::removeParticle ( )
{
	_pScene->releaseActor ( _aParticleArray [ _iTailIndex ]->getActorReference ( ) );

	delete _aParticleArray [ _iTailIndex ];

	_iTailIndex = ( _iTailIndex + 1 ) % m_iMaxParticles;

	--_iParticleCount;

}




#else

Particle::Particle ( )
{
	setActor ( NULL );
	_vThermoDynamicForce.zero ( );
	m_fLife = 1.0f;
	m_fLifeSpan = 0.01f;
	m_fMaxRadius = 1.0f;
	m_dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	m_fGrowthRate = 0.1f;
	m_fCollisionScale = 1.0f;
	m_fDisplayRadius = 1.0f;
}

Particle::Particle ( NxActor* pToSet, NxVec3& vThermoDynamicAcceleration )
{
	setActor ( pToSet );
	_vThermoDynamicForce = pToSet->getMass ( ) * vThermoDynamicAcceleration;
	m_fLife = 1.0f;
	m_fLifeSpan = 0.01f;
	m_fMaxRadius = 1.0f;
	m_dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	m_fGrowthRate = 0.1f;
	m_fCollisionScale = 1.0f;
	m_fDisplayRadius = 1.0f;
}

void Particle::setActor ( NxActor* pToSet )
{
	_pActor = pToSet;
}

void Particle::update ( NxReal fDeltaTime )
{
	_pActor->addForce ( _vThermoDynamicForce );

	NxSphereShape* pSphere = _pActor->getShapes ( ) [ 0 ]->isSphere ( );

	if ( m_fLife > 0.0f )
	{
		m_fLife -= m_fLifeSpan;
	}

	if ( pSphere )
	{
		m_fCurrentRadius = m_fDisplayRadius;

		if ( m_fCurrentRadius < m_fMaxRadius )
		{
			m_fDisplayRadius = ( m_fDisplayRadius + m_fGrowthRate * fDeltaTime );

			pSphere->setRadius ( m_fCurrentRadius + m_fGrowthRate * fDeltaTime );

			float fCurrentSize = pSphere->getRadius ( );
			float fNewSize = pSphere->getRadius ( ) * m_fCollisionScale;

			pSphere->setRadius ( ( pSphere->getRadius ( ) * m_fCollisionScale ) );
			
			if ( pSphere->getRadius ( ) > m_fMaxRadius )
				pSphere->setRadius ( m_fMaxRadius );
		}

		if ( pSphere->getRadius ( ) > m_fMaxRadius )
			pSphere->setRadius ( m_fMaxRadius );
	}
}

NxActor& Particle::getActorReference ( )
{
	return *_pActor;
}

ParticleEmitter::ParticleEmitter ( int iMaxParticles, float fEmitRate, NxScene* pSceneToSet, NxVec3& vStartingPos, NxReal fStartingVelScale, NxVec3& vThermoDynamicAcceleration )
{
	_aParticleArray					= new Particle* [ iMaxParticles ];
	m_iMaxParticles					= iMaxParticles;
	m_fEmitRate						= ( int ) fEmitRate;
	_iHeadIndex						= 0;
	_iTailIndex						= 0;
	_iParticleCount					= 0;
	_pScene							= pSceneToSet;
	_vStartingPos					= vStartingPos;
	_fStartingVelScale				= ( 1.0f / NxReal ( RAND_MAX ) ) * fStartingVelScale;
	_fNewParticleTimer				= 0.0f;
	_vThermoDynamicAcceleration		= vThermoDynamicAcceleration;
	m_iGravity						= 0;
	m_bOverrideLinearVelocity		= false;
	m_fLifeSpan						= 0.01f;
	m_fGrowthRate					= 0.1f;
	m_bRange						= false;
	m_fRangeX1						= 0.0f;
	m_fRangeY1						= 0.0f;
	m_fRangeZ1						= 0.0f;
	m_fRangeX2						= 0.0f;
	m_fRangeY1						= 0.0f;
	m_fRangeZ1						= 0.0f;
	m_fMaxSize						= 1.0f;
	m_fCollisionScale				= 1.0f;
	m_fInitialSize					= 1.0f;

	//
	
	{
		NxActorDesc			actorDesc;
		NxBodyDesc			bodyDesc;
		NxSphereShapeDesc	sphereDesc;
		NxMaterialDesc		material;

		sphereDesc.radius		 = 1.0f;
		sphereDesc.group		 = cParticleCollisionGroup;
		
		sphereDesc.materialIndex = PhysX::SceneMaterialIndex;


			// group = 0, collide with each other
		// group = 1, collide with scenery
		// group = 2,

		actorDesc.shapes.pushBack ( &sphereDesc );

		actorDesc.body			= &bodyDesc;
		actorDesc.density		= 0.01f;
		//actorDesc.globalPose.t	= NxVec3 ( 0.0f, 10000.0f, 0.0f );

		m_ppActorList = new NxActor* [ iMaxParticles ];

		float fY = 100.0f;

		for ( int i = 0; i < iMaxParticles; i++ )
		{
			actorDesc.globalPose.t	= NxVec3 ( 0.0f, fY, 0.0f );
			fY += 5.0f;

			m_ppActorList [ i ] = _pScene->createActor ( actorDesc );

			m_ppActorList [ i ]->putToSleep    ( );
			m_ppActorList [ i ]->raiseBodyFlag ( NX_BF_FROZEN_POS );

			_aParticleArray [ i ] = new Particle;
		}
	}
	//
}

void ParticleEmitter::setCollision ( int iMode )
{
	// mode = 0, collide with each other
	// mode = 1, collide with scenery
	// mode = 2, no collision

	if ( iMode == 0 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 3 );

			NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 3 );
			}
		}

		PhysX::pScene->setGroupCollisionFlag ( 3, 3, true );
		PhysX::pScene->setGroupCollisionFlag ( 3, 0, true );
		PhysX::pScene->setGroupCollisionFlag ( 3, 2, false );
	}

	if ( iMode == 1 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 1 );

		    NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 1 );
			}
		}
	}

	if ( iMode == 2 )
	{
		for ( int i = 0; i < m_iMaxParticles; i++ )
		{
			m_ppActorList [ i ]->setGroup ( 2 );

			NxU32 nbShapes = m_ppActorList [ i ]->getNbShapes();
			NxShape*const* shapes = m_ppActorList [ i ]->getShapes();

			while (nbShapes--)
			{
				shapes[nbShapes]->setGroup ( 2 );
			}
		}

		PhysX::pScene->setGroupCollisionFlag ( 2, 2, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 0, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 3, false );
		PhysX::pScene->setGroupCollisionFlag ( 2, 1, false );
	}
}

ParticleEmitter::~ParticleEmitter ( )
{
	for ( int i = 0; i < m_iMaxParticles; i++ )
	{
		m_ppActorList [ i ]->setGlobalPosition ( NxVec3 ( 0, 0, 0 ) );

		_pScene->releaseActor ( *m_ppActorList [ i ] );

		delete _aParticleArray [ i ];
	}

	if ( m_ppActorList )
		delete [ ] m_ppActorList;
}

void ParticleEmitter::setLifeSpan ( float fLifeSpan )
{
	m_fLifeSpan = fLifeSpan;
}

void ParticleEmitter::setSize ( float fSize )
{
	m_fInitialSize = fSize;	
}

void ParticleEmitter::setMaxSize ( float fSize )
{
	m_fMaxSize = fSize;	
}

void ParticleEmitter::setGrowthRate ( float fRate )
{
	m_fGrowthRate = fRate;
}

void ParticleEmitter::removeAllParticles ( )
{
	while ( _iParticleCount > 0 )
		removeParticle ( );
}

void ParticleEmitter::update ( NxReal fDeltaTime )
{
	_fNewParticleTimer -= fDeltaTime;

	if ( _fNewParticleTimer < 0.0f )
	{
		addParticle ( );
		_fNewParticleTimer += m_fEmitRate;
	}

	int iParticlesUpdated = 0;

	for ( int iParticleIndex = _iTailIndex; iParticlesUpdated < _iParticleCount; iParticleIndex = ( ( iParticleIndex + 1 ) % m_iMaxParticles ) )
	{
		_aParticleArray [ iParticleIndex ]->m_fMaxRadius  = m_fMaxSize;
		_aParticleArray [ iParticleIndex ]->m_fLifeSpan   = m_fLifeSpan;
		_aParticleArray [ iParticleIndex ]->m_fGrowthRate = m_fGrowthRate;

		_aParticleArray [ iParticleIndex ]->m_fCollisionScale  = m_fCollisionScale;

		_aParticleArray [ iParticleIndex ]->update ( fDeltaTime );
		++iParticlesUpdated;
	}
}

void ParticleEmitter::setCollisionScale	( float fScale )
{
	m_fCollisionScale = fScale;
}

void ParticleEmitter::setEmitRate ( float fRate )
{
	m_fEmitRate = (int)fRate;
}

void ParticleEmitter::setPosition ( float fX, float fY, float fZ )
{
	_vStartingPos = NxVec3 ( fX, fY, fZ );
}

DARKSDK int RndLLEx(int r)
{
	int result=0;
	if(r>0)
	{
		// leefix - 250604 - u54 - 0 to 22 million now
		if ( r>1000 )  result += (rand()*1000);
		if ( r>100 ) result += (rand()*100);
		if ( r>10 ) result += (rand()*10);
		result += rand();
		result %= r+1;
	}
	return result;
}

void ParticleEmitter::addParticle ( )
{
	if ( ( _iTailIndex == _iHeadIndex ) && ( _iParticleCount != 0 ) )
	{
		removeParticle ( );
	}

	NxActor* pActor = m_ppActorList [ _iHeadIndex ];

	pActor->wakeUp ( );
	pActor->clearBodyFlag ( NX_BF_FROZEN_POS );
	pActor->setGlobalPosition ( _vStartingPos );

	if ( m_bRange )
		pActor->setGlobalPosition ( NxVec3 ( m_fRangeX1 + RndLLEx ( ( int ) m_fRangeX2 ), m_fRangeY1 + RndLLEx ( ( int ) m_fRangeY2 ), m_fRangeZ1 + RndLLEx ( ( int ) m_fRangeZ2 ) ) );

	// give it an initial linear velocity, scaled by _fStartingVelScale
	NxVec3 vRandVec ( NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ) );

	if ( m_bOverrideLinearVelocity )
		pActor->setLinearVelocity ( m_vecLinearVelocity );
	else
		pActor->setLinearVelocity ( vRandVec );

	// turn off gravity if required
	if ( m_iGravity == 0 )
		pActor->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );

	_aParticleArray [ _iHeadIndex ]->setActor ( pActor );

	NxSphereShape* pSphere = pActor->getShapes ( ) [ 0 ]->isSphere ( );
	pSphere->setRadius ( m_fInitialSize * m_fCollisionScale );

	_aParticleArray [ _iHeadIndex ]->m_fDisplayRadius = m_fInitialSize;

	_aParticleArray [ _iHeadIndex ]->_vThermoDynamicForce = pActor->getMass ( ) * _vThermoDynamicAcceleration;

	_aParticleArray [ _iHeadIndex ]->m_fLife = 1.0f;
	_iHeadIndex = ( _iHeadIndex + 1 ) % m_iMaxParticles;
	++_iParticleCount;
	

	/*
	if ( ( _iTailIndex == _iHeadIndex ) && ( _iParticleCount != 0 ) )
	{
		removeParticle ( );
	}

	NxActorDesc			actorDesc;
	NxBodyDesc			bodyDesc;
	NxSphereShapeDesc	sphereDesc;

	sphereDesc.radius	= m_fInitialSize;
	sphereDesc.group	= cParticleCollisionGroup;

	NxMaterialDesc material;
	NxMaterialIndex materialIndex;
	material.restitution     = 0.0f;
	material.staticFriction  = 1.0f;
	material.dynamicFriction = 1.0f;

	// don't create a material
	// materialIndex = PhysX::pScene->createMaterial ( material )->getMaterialIndex ( );
	// sphereDesc.materialIndex = materialIndex;

	// use standard scene material for now
	sphereDesc.materialIndex = PhysX::SceneMaterialIndex;

	actorDesc.shapes.pushBack ( &sphereDesc );

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 0.01f;
	actorDesc.globalPose.t	= _vStartingPos;

	if ( m_bRange )
		actorDesc.globalPose.t = NxVec3 ( m_fRangeX1 + dbRnd ( m_fRangeX2 ), m_fRangeY1 + dbRnd ( m_fRangeY2 ), m_fRangeZ1 + dbRnd ( m_fRangeZ2 ) );

	NxActor* pNewActor = _pScene->createActor ( actorDesc );

	if ( !pNewActor )
		return;

	// give it an initial linear velocity, scaled by _fStartingVelScale
	NxVec3 vRandVec ( NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ), NxReal ( rand ( ) * _fStartingVelScale ) );

	if ( m_bOverrideLinearVelocity )
		pNewActor->setLinearVelocity ( m_vecLinearVelocity );
	else
		pNewActor->setLinearVelocity ( vRandVec );

	// turn off gravity if required
	if ( m_iGravity == 0 )
		pNewActor->raiseBodyFlag ( NX_BF_DISABLE_GRAVITY );

	_aParticleArray [ _iHeadIndex ] = new Particle ( pNewActor, _vThermoDynamicAcceleration );

	_aParticleArray [ _iHeadIndex ]->m_fLife = 1.0f;
	_iHeadIndex = ( _iHeadIndex + 1 ) % m_iMaxParticles;
	++_iParticleCount;
	*/
	
}

void ParticleEmitter::setLinearVelocity ( float fX, float fY, float fZ )
{
	m_vecLinearVelocity = NxVec3 ( fX, fY, fZ );
	m_bOverrideLinearVelocity = true;
}

void ParticleEmitter::setGravity ( int iState )
{
	m_iGravity = iState;
}

void ParticleEmitter::removeParticle ( )
{
	_aParticleArray [ _iTailIndex ]->getActorReference ( ).putToSleep    ( );
	_aParticleArray [ _iTailIndex ]->getActorReference ( ).raiseBodyFlag ( NX_BF_FROZEN_POS );

	_aParticleArray [ _iTailIndex ]->getActorReference ( ).setGlobalPosition ( NxVec3 ( -1000000000, -100000000, -100000000 ) );

	//delete _aParticleArray [ _iTailIndex ];

	_iTailIndex = ( _iTailIndex + 1 ) % m_iMaxParticles;

	--_iParticleCount;

	
	/*	
	_pScene->releaseActor ( _aParticleArray [ _iTailIndex ]->getActorReference ( ) );

	delete _aParticleArray [ _iTailIndex ];

	_iTailIndex = ( _iTailIndex + 1 ) % m_iMaxParticles;

	--_iParticleCount;
*/	
}

#endif