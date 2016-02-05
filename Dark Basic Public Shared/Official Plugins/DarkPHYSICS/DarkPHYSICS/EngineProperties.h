
/////////////////////////////////////////////////////////////////////////////////////
// ENGINE COMMANDS //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void  dbPhySetGravity								( float fX, float fY, float fZ );
DARKSDK void  dbPhySetTiming								( float fMaxTimeStep, int iMaxIterations, int iMethod );
DARKSDK void  dbPhySetFixedTiming							( float fElapsedTime );
DARKSDK void  dbPhySetAutoFixedTiming						( void );

DARKSDK void  dbPhySetSkinWidth								( float fValue );
DARKSDK void  dbPhySetDefaultSleepLinearVelocitySquared		( float fValue );
DARKSDK void  dbPhySetDefaultSleepAngularVelocitySquared	( float fValue );
DARKSDK void  dbPhySetBounceThreshold						( float fValue );
DARKSDK void  dbPhySetDynamicFriction						( float fValue );
DARKSDK void  dbPhySetStaticFriction						( float fValue );
DARKSDK void  dbPhySetMaxAngularVelocity					( float fValue );
DARKSDK void  dbPhySetContinuousCD							( float fValue );
DARKSDK void  dbPhySetAdaptiveForce							( float fValue );
DARKSDK void  dbPhySetControlledFiltering					( float fValue );
DARKSDK void  dbPhySetTriggerCallback						( float fValue );

DARKSDK void  dbPhySetGroupCollision						( int iA, int iB, int iState );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
