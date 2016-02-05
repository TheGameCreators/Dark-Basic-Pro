
/////////////////////////////////////////////////////////////////////////////////////
// EXPOSED PARTICLE FUNCTIONS ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeParticleEmitter			( int iID, int iMaximum, float fEmitRate, float fX, float fY, float fZ, float fSpeed, float fXDir, float fYDir, float fZDir );
DARKSDK void dbPhyMakeFireEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
DARKSDK void dbPhyMakeSmokeEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
DARKSDK void dbPhyMakeSnowEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
DARKSDK void dbPhyDeleteEmitter					( int iID );

DARKSDK void dbPhySetEmitterLinearVelocity		( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetEmitterLinearVelocityRange ( int iID, float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ );
DARKSDK void dbPhySetEmitterGravity				( int iID, int iState );
DARKSDK void dbPhySetEmitterLifeSpan			( int iID, float fLife );
DARKSDK void dbPhySetEmitterFade				( int iID, int iState );
DARKSDK void dbPhySetEmitterSize				( int iID, float fSize );
DARKSDK void dbPhySetEmitterMaxSize				( int iID, float fSize );
DARKSDK void dbPhySetEmitterGrowthRate			( int iID, float fRate );
DARKSDK void dbPhySetEmitterRange				( int iID, float fRangeX1, float fRangeY1, float fRangeZ1, float fRangeX2, float fRangeY2, float fRangeZ2 );
DARKSDK void dbPhySetEmitterStartColor			( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue );
DARKSDK void dbPhySetEmitterEndColor			( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue );
DARKSDK void dbPhySetEmitterPosition			( int iID, float fX, float fY, float fZ );


DARKSDK void dbPhySetEmitterCollisionScale		( int iID, float fScale );
DARKSDK void dbPhySetEmitterCollision			( int iID, int iMode );
DARKSDK void dbPhySetEmitterRate				( int iID, float fRate );
DARKSDK void dbPhySetEmitterUpdate				( int iID, float fUpdate );

DARKSDK int  dbPhyGetEmitterExist				( int iID );



void dbPhyUpdateParticles				( void );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
