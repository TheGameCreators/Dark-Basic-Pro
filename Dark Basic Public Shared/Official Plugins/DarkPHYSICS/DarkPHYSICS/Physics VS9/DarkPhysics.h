
#ifndef _DARK_PHYSICS_H_
#define _DARK_PHYSICS_H_

/////////////////////////////////////////////////////////////////////////////////////
// CLOTH FUNCTIONS //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void  dbPhyAttachClothToShape						( int iID, int iObject, int iMode );
void  dbPhyAttachClothToAllShapes					( int iID, int iMode );
void  dbPhyDetachClothFromShape						( int iID, int iShape );
void  dbPhyAttachClothVertex						( int iID, int iVertex, float fX, float fY, float fZ );
void  dbPhyAttachClothVertexToShape					( int iID, int iVertex, int iObject, float fX, float fY, float fZ, int iMode );
void  dbPhyFreeClothVertex							( int iID, int iVertex );

void  dbPhyClothAttachRigidBody						( int iID, int iRigidBody, float fImpulse, float fPenetration, float fDeformation );

void  dbPhyMakeCloth								( int iID );
void  dbPhyMakeClothFromObject						( int iID, int iObject );
void  dbPhyMakeClothFromLimb						( int iID, int iObject, int iLimb );
void  dbPhyDeleteCloth								( int iID );

void  dbPhyBuildCloth								( int iID );

void  dbPhySetClothDimensions						( int iID, float fWidth, float fHeight, float fDepth );
void  dbPhySetClothPosition							( int iID, float fX, float fY, float fZ );
void  dbPhySetClothRotation							( int iID, float fX, float fY, float fZ );
void  dbPhySetClothStatic							( int iID, int iState );
void  dbPhySetClothCollision						( int iID, int iState );
void  dbPhySetClothGravity							( int iID, int iState );
void  dbPhySetClothBending							( int iID, int iState );
void  dbPhySetClothDamping							( int iID, int iState );
void  dbPhySetClothTwoWayCollision					( int iID, int iState );
void  dbPhySetClothTearing							( int iID, int iState );
void  dbPhySetClothThickness						( int iID, float fValue );
void  dbPhySetClothDensity							( int iID, float fValue );
void  dbPhySetClothBendingStiffness					( int iID, float fValue );
void  dbPhySetClothStretchingStiffness				( int iID, float fValue );
void  dbPhySetClothFriction							( int iID, float fValue );
void  dbPhySetClothPressure							( int iID, float fValue );
void  dbPhySetClothTearFactor						( int iID, float fValue );
void  dbPhySetClothCollisionResponseCoefficient		( int iID, float fValue );
void  dbPhySetClothAttachmentResponseCoefficient	( int iID, float fValue );
void  dbPhySetClothSolverIterations					( int iID, int iValue );
void  dbPhySetClothExternalAcceleration				( int iID, float fX, float fY, float fZ );
void  dbPhySetClothCollisionGroup					( int iID, int iGroup );

void  dbPhyAddForceAtClothVertex					( int iID, int iVertex, float fX, float fY, float fZ, int iMode );

int	  dbPhyGetClothExist							( int iID );
int	  dbPhyGetClothBuilt							( int iID );

float dbPhyGetClothWidth							( int iID );
float dbPhyGetClothHeight							( int iID );
float dbPhyGetClothDepth							( int iID );
float dbPhyGetClothPositionX						( int iID );
float dbPhyGetClothPositionY						( int iID );
float dbPhyGetClothPositionZ						( int iID );
float dbPhyGetClothRotationX						( int iID );
float dbPhyGetClothRotationY						( int iID );
float dbPhyGetClothRotationZ						( int iID );

int   dbPhyGetClothStatic							( int iID );
int   dbPhyGetClothCollision						( int iID );
int   dbPhyGetClothGravity							( int iID );
int   dbPhyGetClothBending							( int iID );
int   dbPhyGetClothDamping							( int iID );
int   dbPhyGetClothTwoWayCollision					( int iID );
int   dbPhyGetClothTearing							( int iID );

float dbPhyGetClothThickness						( int iID );
float dbPhyGetClothDensity							( int iID );
float dbPhyGetClothBendingStiffness					( int iID );
float dbPhyGetClothStretchingStiffness				( int iID );
float dbPhyGetClothFriction							( int iID );
float dbPhyGetClothPressure							( int iID );
float dbPhyGetClothTearFactor						( int iID );
float dbPhyGetClothCollisionResponseCoefficient		( int iID );
float dbPhyGetClothAttachmentResponseCoefficient	( int iID );

int   dbPhyGetClothSolverIterations					( int iID );

float dbPhyGetClothExternalAccelerationX			( int iID );
float dbPhyGetClothExternalAccelerationY			( int iID );
float dbPhyGetClothExternalAccelerationZ			( int iID );
int   dbPhyGetClothCollisionGroup					( int iID );

float dbPhyGetClothMinSizeX							( int iID );
float dbPhyGetClothMinSizeY							( int iID );
float dbPhyGetClothMinSizeZ							( int iID );
float dbPhyGetClothMaxSizeX							( int iID );
float dbPhyGetClothMaxSizeY							( int iID );
float dbPhyGetClothMaxSizeZ							( int iID );

int   dbPhyClothRayCast								( int iID, float fX, float fY, float fZ, float fDirX, float fDirY, float fDirZ );
float dbPhyClothRayCastHitX							( int iID );
float dbPhyClothRayCastHitY							( int iID );
float dbPhyClothRayCastHitZ							( int iID );
int   dbPhyClothRayCastHitVertex					( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// ENGINE FUNCTIONS /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void  dbPhySetGravity								( float fX, float fY, float fZ );
void  dbPhySetTiming								( float fMaxTimeStep, int iMaxIterations, int iMethod );
void  dbPhySetFixedTiming							( float fElapsedTime );
void  dbPhySetAutoFixedTiming						( void );

void  dbPhySetSkinWidth								( float fValue );
void  dbPhySetDefaultSleepLinearVelocitySquared		( float fValue );
void  dbPhySetDefaultSleepAngularVelocitySquared	( float fValue );
void  dbPhySetBounceThreshold						( float fValue );
void  dbPhySetDynamicFriction						( float fValue );
void  dbPhySetStaticFriction						( float fValue );
void  dbPhySetMaxAngularVelocity					( float fValue );
void  dbPhySetContinuousCD							( float fValue );
void  dbPhySetAdaptiveForce							( float fValue );
void  dbPhySetControlledFiltering					( float fValue );
void  dbPhySetTriggerCallback						( float fValue );

void  dbPhySetGroupCollision						( int iA, int iB, int iState );

void  dbPhyEnableDebug								( void );
void  dbPhyStart									( void );
void  dbPhyStart									( int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
void  dbPhyEnd										( void );
void  dbPhyClear									( void );
int	  dbPhysX										( void );

void  dbPhyMakeScene								( int iIndex, int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
void  dbPhySetCurrentScene							( int iIndex );

void  dbPhySetGroundPlane							( int iState );

void  dbPhySetFluidRendererParticleRadius			( float v );
void  dbPhySetFluidRendererParticleSurfaceScale		( float v );
void  dbPhySetFluidRendererDensityThreshold			( float v );
void  dbPhySetFluidRendererFluidColor				( float r, float g, float b, float a );
void  dbPhySetFluidRendererFluidColorFalloff		( float r, float g, float b, float a );
void  dbPhySetFluidRendererColorFalloffScale		( float v );
void  dbPhySetFluidRendererSpecularColor			( float r, float g, float b, float a );
void  dbPhySetFluidRendererShininess				( float v );
void  dbPhySetFluidRendererFresnelColor				( float r, float g, float b, float a );

void  dbPhyUpdate									( void );
void  dbPhyUpdate									( int iUserControlledUpdate );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// FLUID FUNCTIONS //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeFluid						( int iID );
void dbPhyMakeFluidEmitter  			( int iID );
void dbPhyBuildFluid					( int iID );
void dbPhyDeleteFluid					( int iID );
int  dbPhyGetFluidExist					( int iID );

void dbPhyMakeBoxDrain					( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ );
void dbPhyMakeSphereDrain				( int iID, float fX, float fY, float fZ, float fRadius );
void dbPhyDeleteDrain					( int iID );
int  dbPhyGetDrainExist					( int iID );

void dbPhyClearFluids					( void );

void dbPhySetFluidParticleBufferCap		( int iID, int iCap );
void dbPhySetFluidRestPerMetre			( int iID, float fRest );
void dbPhySetFluidRadius				( int iID, float fRadius );
void dbPhySetFluidMotionLimit			( int iID, float fMotion );
void dbPhySetFluidPacketSize			( int iID, int iSize );
void dbPhySetFluidPosition				( int iID, float fX, float fY, float fZ );
void dbPhySetFluidRotation				( int iID, float fX, float fY, float fZ );
void dbPhySetFluidStiffness				( int iID, float fStiffness );
void dbPhySetFluidViscosity				( int iID, float fViscosity );
void dbPhySetFluidRestDensity			( int iID, float fRestDensity );
void dbPhySetFluidDamping				( int iID, float fDamping );
void dbPhySetFluidRestitution			( int iID, float fRestitution );
void dbPhySetFluidAdhesion				( int iID, float fAdhesion );
void dbPhySetFluidMethod				( int iID, int iMethod );
void dbPhySetFluidAcceleration			( int iID, float fX, float fY, float fZ );

void dbPhySetFluidScreenSpacing			( int iID, float fSpacing );
void dbPhySetFluidTriangleWinding		( int iID, int iMode );
void dbPhySetFluidThreshold				( int iID, float fThreshold );
void dbPhySetFluidParticleRadius		( int iID, float fRadius );
void dbPhySetFluidDepthSmoothing		( int iID, float fSmoothing );
void dbPhySetFluidSilhouetteSmoothing	( int iID, float fSmoothing );

void dbPhySetFluidEmitterType			( int iID, int iType );
void dbPhySetFluidEmitterMaxParticles	( int iID, int iMax );
void dbPhySetFluidEmitterShape			( int iID, int iShape );
void dbPhySetFluidEmitterDimensions		( int iID, float fX, float fY );
void dbPhySetFluidEmitterRandomPosition ( int iID, float fX, float fY, float fZ );
void dbPhySetFluidEmitterRandomAngle	( int iID, float fAngle );
void dbPhySetFluidEmitterVelocity		( int iID, float fVelocity );
void dbPhySetFluidEmitterRate			( int iID, float fRate );
void dbPhySetFluidEmitterLifetime		( int iID, float fLife );
void dbPhySetFluidEmitterFlags			( int iID, int iFlags );

void dbPhySetFluidEmitterRotation		( int iID, float fX, float fY, float fZ );
void dbPhySetFluidEmitterPosition		( int iID, float fX, float fY, float fZ );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// PARTICLE FUNCTIONS ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeParticleEmitter			( int iID, int iMaximum, float fEmitRate, float fX, float fY, float fZ, float fSpeed, float fXDir, float fYDir, float fZDir );
void dbPhyMakeFireEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
void dbPhyMakeSmokeEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
void dbPhyMakeSnowEmitter				( int iID, int iMaximum, float fX, float fY, float fZ );
void dbPhyDeleteEmitter					( int iID );

void dbPhySetEmitterLinearVelocity		( int iID, float fX, float fY, float fZ );
void dbPhySetEmitterLinearVelocityRange ( int iID, float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ );
void dbPhySetEmitterGravity				( int iID, int iState );
void dbPhySetEmitterLifeSpan			( int iID, float fLife );
void dbPhySetEmitterFade				( int iID, int iState );
void dbPhySetEmitterSize				( int iID, float fSize );
void dbPhySetEmitterMaxSize				( int iID, float fSize );
void dbPhySetEmitterGrowthRate			( int iID, float fRate );
void dbPhySetEmitterRange				( int iID, float fRangeX1, float fRangeY1, float fRangeZ1, float fRangeX2, float fRangeY2, float fRangeZ2 );
void dbPhySetEmitterStartColor			( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue );
void dbPhySetEmitterEndColor			( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue );
void dbPhySetEmitterPosition			( int iID, float fX, float fY, float fZ );


void dbPhySetEmitterCollisionScale		( int iID, float fScale );
void dbPhySetEmitterCollision			( int iID, int iMode );
void dbPhySetEmitterRate				( int iID, float fRate );
void dbPhySetEmitterUpdate				( int iID, float fUpdate );

int  dbPhyGetEmitterExist				( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// CHARACTER CONTROLLER FUNCTIONS ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeBoxCharacterController			( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, int iUp, float fStep, float fSlopeLimit );
void dbPhyMakeCapsuleCharacterController		( int iID, float fX, float fY, float fZ, float fRadius, float fHeight, int iUp, float fStep, float fSlopeLimit );
void dbPhyMoveCharacterController				( int iID, float fSpeed );

void dbPhySetCharacterControllerDisplacement	( int iID, float fForward, float fBackward );
void dbPhySetCharacterControllerDisplacement	( int iID, float fX, float fY, float fZ );
void dbPhySetCharacterControllerDisplacement	( int iID, int iState );
void dbPhySetCharacterControllerSharpness		( int iID, float fValue );
void dbPhySetCharacterControllerExtents			( int iID, float fX, float fY, float fZ );
void dbPhySetCharacterControllerExtents			( int iID, float fRadius, float fHeight );
void dbPhySetCharacterControllerMinDistance     ( int iID, float fValue );

int  dbPhyGetCharacterControllerExist			( int iID );
void dbPhyDeleteCharacterController				( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// COLLISION DATA FUNCTIONS /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int		dbPhyGetCollisionData				( void );
int		dbPhyGetCollisionObjectA			( void );
int		dbPhyGetCollisionObjectB			( void );

float	dbPhyGetCollisionNormalForceX		( void );
float	dbPhyGetCollisionNormalForceY		( void );
float	dbPhyGetCollisionNormalForceZ		( void );
float	dbPhyGetCollisionFrictionForceX		( void );
float	dbPhyGetCollisionFrictionForceY		( void );
float	dbPhyGetCollisionFrictionForceZ		( void );

int		dbPhyGetCollision					( int iObjectA, int iObjectB );
int		dbPhyGetCollisionCount				( int iObject );

void	dbPhySetCollisionState				( int iObject, int iState );

float	dbPhyGetCollisionFrictionForceX		( int iObject, int iIndex );
float	dbPhyGetCollisionFrictionForceY		( int iObject, int iIndex );
float	dbPhyGetCollisionFrictionForceZ		( int iObject, int iIndex );
float	dbPhyGetCollisionNormalForceX		( int iObject, int iIndex );
float	dbPhyGetCollisionNormalForceY		( int iObject, int iIndex );
float	dbPhyGetCollisionNormalForceZ		( int iObject, int iIndex );
float	dbPhyGetCollisionContactNormalX		( int iObject, int iIndex );
float	dbPhyGetCollisionContactNormalY		( int iObject, int iIndex );
float	dbPhyGetCollisionContactNormalZ		( int iObject, int iIndex );
float	dbPhyGetCollisionContactPointX		( int iObject, int iIndex );
float	dbPhyGetCollisionContactPointY		( int iObject, int iIndex );
float	dbPhyGetCollisionContactPointZ		( int iObject, int iIndex );

void	dbPhySetCollisionState				( int iObject, int iState );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY CREATION FUNCTIONS ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticBox		( int iObject );
void dbPhyMakeRigidBodyStaticBox		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticBoxes		( int iObject );
void dbPhyMakeRigidBodyStaticBoxes		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicBox		( int iObject );
void dbPhyMakeRigidBodyDynamicBox		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicCCDBox	( int iObject );
void dbPhyMakeRigidBodyDynamicCCDBox	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicBoxes		( int iObject );
void dbPhyMakeRigidBodyDynamicBoxes		( int iObject, int iMaterial );

void dbPhyMakeRigidBodyStaticCapsule	( int iObject );
void dbPhyMakeRigidBodyStaticCapsule	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticCapsules	( int iObject );
void dbPhyMakeRigidBodyStaticCapsules	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicCapsule	( int iObject );
void dbPhyMakeRigidBodyDynamicCapsule	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicCapsules	( int iObject );
void dbPhyMakeRigidBodyDynamicCapsules	( int iObject, int iMaterial );

void dbPhyMakeRigidBodyStaticConvex		( int iObject );
void dbPhyMakeRigidBodyStaticConvex		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticConvex		( int iObject, char* szFile );
void dbPhyMakeRigidBodyStaticConvex		( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyStaticConvexes	( int iObject );
void dbPhyMakeRigidBodyStaticConvexes	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticConvexes	( int iObject, char* szFile );
void dbPhyMakeRigidBodyStaticConvexes	( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyDynamicConvex	( int iObject );
void dbPhyMakeRigidBodyDynamicConvex	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicConvex	( int iObject, char* szFile );
void dbPhyMakeRigidBodyDynamicConvex	( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyDynamicConvexes	( int iObject );
void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, char* szFile );
void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyStaticConvex		( int iObject, char* szFile );
void dbPhyLoadRigidBodyStaticConvex		( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyStaticConvexes	( int iObject, char* szFile );
void dbPhyLoadRigidBodyStaticConvexes	( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyDynamicConvex	( int iObject, char* szFile );
void dbPhyLoadRigidBodyDynamicConvex	( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyDynamicConvexes	( int iObject, char* szFile );
void dbPhyLoadRigidBodyDynamicConvexes	( int iObject, char* szFile, int iMaterial );

void dbPhyMakeRigidBodyStaticMesh		( int iObject );
void dbPhyMakeRigidBodyStaticMesh		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile );
void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyStaticMeshes		( int iObject );
void dbPhyMakeRigidBodyStaticMeshes		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile );
void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyDynamicMesh		( int iObject );
void dbPhyMakeRigidBodyDynamicMesh		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile );
void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );
void dbPhyMakeRigidBodyDynamicMeshes	( int iObject );
void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile );
void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile );
void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile );
void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile );
void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile );
void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );

void dbPhyMakeRagdoll					( int iRagdollID, int iObject, int iPhysicsObject );
void dbPhyAssignRagdollPart				( int iRagdollID, int iMesh, int iBone, int iObject );
void dbPhyDeleteRagdoll					( int iRagdollID );
int  dbPhyGetRagdollExist				( int iRagdollID );

void dbPhyMakeRigidBodyStaticSphere		( int iObject );
void dbPhyMakeRigidBodyStaticSphere		( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticSpheres	( int iObject );
void dbPhyMakeRigidBodyStaticSpheres	( int iObject, int iMaterial );

void dbPhyMakeRigidBodyDynamicSphere	( int iObject );
void dbPhyMakeRigidBodyDynamicSphere	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyDynamicSpheres	( int iObject );
void dbPhyMakeRigidBodyDynamicSpheres	( int iObject, int iMaterial );

void dbPhyMakeRigidBodyStaticTerrain	( int iObject );
void dbPhyMakeRigidBodyStaticTerrain	( int iObject, int iMaterial );
void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile );
void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );
void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile );
void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );
void dbPhySetRigidBodyTerrainExtent		( float fExtent );

void dbPhyDeleteRigidBody				( int iObject );
int  dbPhyGetRigidBodyExist				( int iObject );
int  dbPhyGetRigidBodyType				( int iObject );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY JOINT FUNCTIONS ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMake6DOFJoint						( int iJoint, int iA, int iB );

void dbPhySet6DOFJointLocalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySet6DOFJointLocalAxis				( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySet6DOFJointLocalNormal			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySet6DOFJointGlobalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySet6DOFJointGlobalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySet6DOFJointTwistMotion			( int iJoint, int iMotion );
void dbPhySet6DOFJointSwing1Motion			( int iJoint, int iMotion );
void dbPhySet6DOFJointSwing2Motion			( int iJoint, int iMotion );
void dbPhySet6DOFJointMotionX				( int iJoint, int iX );
void dbPhySet6DOFJointMotionY				( int iJoint, int iY );
void dbPhySet6DOFJointMotionZ				( int iJoint, int iZ );
void dbPhySet6DOFJointLinearLimit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
void dbPhySet6DOFJointSwing1Limit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
void dbPhySet6DOFJointSwing2Limit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
void dbPhySet6DOFJointTwistLimit			( int iJoint, float fDampingLow, float fRestitutionLow, float fSpringLow, float fValueLow, float fDampingHigh, float fRestitutionHigh, float fSpringHigh, float fValueHigh );
void dbPhySet6DOFJointDriveX				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointDriveY				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointDriveZ				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointTwistDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointSwingDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointSlerpDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
void dbPhySet6DOFJointDrivePosition			( int iJoint, float fX, float fY, float fZ );
void dbPhySet6DOFJointDriveRotation			( int iJoint, float fX, float fY, float fZ );
void dbPhySet6DOFJointDriveLinearVelocity	( int iJoint, float fX, float fY, float fZ );
void dbPhySet6DOFJointDriveAngularVelocity	( int iJoint, float fX, float fY, float fZ );
void dbPhySet6DOFJointProjection			( int iJoint, int iMode, float fDistance );
void dbPhySet6DOFJointSlerpDriveState		( int iJoint, int iState );
void dbPhySet6DOFJointGear					( int iJoint, int iState );
void dbPhyBuild6DOFJoint					( int iJoint );

void dbPhyMakeCylindricalJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
void dbPhyMakeCylindricalJoint				( int iJoint, int iA, int iB );
void dbPhySetCylindricalJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetCylindricalJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetCylindricalJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetCylindricalJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhyBuildCylindricalJoint				( int iJoint );

void dbPhyMakeDistanceJoint					( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fMinDistance, float fMaxDistance );
void dbPhyMakeDistanceJoint					( int iJoint, int iA, int iB );
void dbPhySetDistanceJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetDistanceJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetDistanceJointLocalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetDistanceJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetDistanceJointMax				( int iJoint, float fMax );
void dbPhySetDistanceJointMin				( int iJoint, float fMin );
void dbPhySetDistanceJointSpring			( int iJoint, float fTarget, float fSpring, float fDamper );
void dbPhyBuildDistanceJoint				( int iJoint );

void dbPhyMakeFixedJoint					( int iJoint, int iA, int iB );
void dbPhyMakeFixedJoint					( int iJoint, int iA, int iB, int iExtras );
void dbPhyBuildFixedJoint					( int iJoint );

void dbPhyMakePointInPlaneJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
void dbPhyMakePointInPlaneJoint				( int iJoint, int iA, int iB );
void dbPhySetPointInPlaneJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointInPlaneJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointInPlaneJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointInPlaneJointGlobalAxis	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhyBuildPointInPlaneJoint			( int iJoint );

void dbPhyMakePointOnLineJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
void dbPhyMakePointOnLineJoint				( int iJoint, int iA, int iB );
void dbPhySetPointOnLineJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointOnLineJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointOnLineJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPointOnLineJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhyBuildPointOnLineJoint				( int iJoint );

void dbPhyMakePrismaticJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
void dbPhyMakePrismaticJoint				( int iJoint, int iA, int iB );
void dbPhySetPrismaticJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPrismaticJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPrismaticJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPrismaticJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhyBuildPrismaticJoint				( int iJoint );

void dbPhyMakePulleyJoint					( int iJoint, int iA, int iB, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float fX3, float fY3, float fZ3, float fX4, float fY4, float fZ4, float fDistance, float fStiffness, float fRatio );
void dbPhyMakePulleyJoint					( int iJoint, int iA, int iB );
void dbPhySetPulleyJointLocalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPulleyJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPulleyJointLocalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPulleyJointGlobalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPulleyJointSuspension			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetPulleyJointDistance			( int iJoint, float fDistance );
void dbPhySetPulleyJointStiffness			( int iJoint, float fStiffness );
void dbPhySetPulleyJointRatio				( int iJoint, float fRatio );
void dbPhySetPulleyJointRigid				( int iJoint, int iState );
void dbPhySetPulleyJointMotor				( int iJoint, int iState );
void dbPhyBuildPulleyJoint					( int iJoint );

void dbPhyMakeRevoluteJoint					( int iJoint, int iA, int iB, float fAxisX, float fAxisY, float fAxisZ, float fX, float fY, float fZ );
void dbPhyMakeRevoluteJoint					( int iJoint, int iA, int iB );
void dbPhySetRevoluteJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetRevoluteJointLocalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetRevoluteJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetRevoluteJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetRevoluteJointSpring			( int iJoint, float fTarget, float fSpring );
void dbPhySetRevoluteJointLimit				( int iJoint, float fLow, float fHigh );
void dbPhySetRevoluteJointMotor				( int iJoint, float fVelocityTarget, float fMaxForce, int iFreeSpin );
void dbPhySetRevoluteJointProjection		( int iJoint, int iMode, float fDistance, float fAngle );
void dbPhyBuildRevoluteJoint				( int iJoint );

void dbPhyMakeSphereJoint					( int iJoint, int iA, int iB, float fX, float fY, float fZ );
void dbPhyMakeSphereJoint					( int iJoint, int iA, int iB );
void dbPhySetSphereJointLocalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetSphereJointLocalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetSphereJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetSphereJointGlobalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
void dbPhySetSphereJointSwingAxis			( int iJoint, float fX, float fY, float fZ );
void dbPhySetSphereJointSwingLimit			( int iJoint, float fValue, float fRestitution, float fHardness );
void dbPhySetSphereJointTwistLimit			( int iJoint, float fLow, float fHigh );
void dbPhySetSphereJointTwistSpring			( int iJoint, float fTarget, float fSpring );
void dbPhySetSphereJointSwingSpring			( int iJoint, float fTarget, float fSpring );
void dbPhySetSphereJointSpring				( int iJoint, float fTarget, float fSpring );
void dbPhySetSphereJointProjection			( int iJoint, int iMode, float fDistance );
void dbPhyBuildSphereJoint					( int iJoint );

void dbPhyDeleteJoint						( int iJoint );
int  dbPhyGetJointExist						( int iJoint );
int  dbPhyGetJointType						( int iJoint );
void dbPhySetJointBreakLimits				( int iJoint, float fMaxForce, float fMaxTorque );
int  dbPhyGetJointState						( int iJoint );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// MATERIAL FUNCTIONS ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void  dbPhyMakeMaterial						( int iID, char* szName );
void  dbPhyDeleteMaterial					( int iID );
void  dbPhySetMaterialDynamicFriction		( int iID, float fValue );
void  dbPhySetMaterialStaticFriction		( int iID, float fValue );
void  dbPhySetMaterialRestitution			( int iID, float fValue );
void  dbPhySetMaterialDynamicFrictionV		( int iID, float fValue );
void  dbPhySetMaterialStaticFrictionV		( int iID, float fValue );
void  dbPhySetMaterialDirOfAnisotropy		( int iID, float fX, float fY, float fZ );
void  dbPhySetMaterialFrictionCombine		( int iID, int iMode );
void  dbPhySetMaterialRestitutionCombine	( int iID, int iMode );
void  dbPhyBuildMaterial					( int iID );

int   dbPhyGetMaterialExist					( int iID );

float dbPhyGetMaterialDynamicFriction		( int iID );
float dbPhyGetMaterialStaticFriction		( int iID );
float dbPhyGetMaterialRestitution			( int iID );
float dbPhyGetMaterialDynamicFrictionV		( int iID );
float dbPhyGetMaterialStaticFrictionV		( int iID );
float dbPhyGetMaterialDirOfAnisotropyX		( int iID );
float dbPhyGetMaterialDirOfAnisotropyY		( int iID );
float dbPhyGetMaterialDirOfAnisotropyZ		( int iID );

int   dbPhyGetMaterialFrictionCombine		( int iID );
int   dbPhyGetMaterialRestitutionCombine	( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY PROPERTY FUNCTIONS ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void  dbPhySetRigidBodyPosition					( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyRotation					( int iObject, float fX, float fY, float fZ );

void  dbPhySetRigidBodyGravity					( int iObject, int iState );

void  dbPhySetRigidBodyGroup					( int iObject, int iGroup );

void  dbPhySetRigidBodyMass						( int iObject, float fMass );
float dbPhyGetRigidBodyMass						( int iObject );
void  dbPhySetRigidBodyMassOffsetLocal			( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyMassOffsetGlobal			( int iObject, float fX, float fY, float fZ );
void  dbPhyUpdateRigidBodyMass					( int iObject, float fDensity, float fTotalMass );

void  dbPhySetRigidBodyLinearDamping			( int iObject, float fDamping );
void  dbPhySetRigidBodyAngularDamping			( int iObject, float fAngular );
float dbPhyGetRigidBodyLinearDamping			( int iObject );
float dbPhyGetRigidBodyAngularDamping			( int iObject );

void  dbPhySetRigidBodyLinearVelocity			( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyAngularVelocity			( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyMaxAngularVelocity		( int iObject, float fMax );
float dbPhyGetRigidBodyLinearVelocityX			( int iObject );
float dbPhyGetRigidBodyLinearVelocityY			( int iObject );
float dbPhyGetRigidBodyLinearVelocityZ			( int iObject );
float dbPhyGetRigidBodyAngularVelocityX			( int iObject );
float dbPhyGetRigidBodyAngularVelocityY			( int iObject );
float dbPhyGetRigidBodyAngularVelocityZ			( int iObject );
float dbPhyGetRigidBodyMaxAngularVelocity		( int iObject );

void  dbPhySetRigidBodyCCDMotionThreshold		( int iObject, float fThreshold );
float dbPhyGetRigidBodyCCDMotionThreshold		( int iObject );
void  dbPhySetRigidBodyCCD						( int iObject, int iState );

void  dbPhySetRigidBodyLinearMomentum			( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyAngularMomentum			( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyLinearMomentumX			( int iObject );
float dbPhyGetRigidBodyLinearMomentumY			( int iObject );
float dbPhyGetRigidBodyLinearMomentumZ			( int iObject );
float dbPhyGetRigidBodyAngularMomentumX			( int iObject );
float dbPhyGetRigidBodyAngularMomentumY			( int iObject );
float dbPhyGetRigidBodyAngularMomentumZ			( int iObject );

void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
void  dbPhyAddRigidBodyForceAtLocal				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
void  dbPhyAddRigidBodyLocalForceAtLocal		( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
void  dbPhyAddRigidBodyTorque					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
void  dbPhyAddRigidBodyLocalTorque				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );

float dbPhyGetRigidBodyPointVelocityX			( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyPointVelocityY			( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyPointVelocityZ			( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyLocalPointVelocityX		( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyLocalPointVelocityY		( int iObject, float fX, float fY, float fZ );
float dbPhyGetRigidBodyLocalPointVelocityZ		( int iObject, float fX, float fY, float fZ );

void  dbPhySetRigidBodySleepLinearVelocity		( int iObject, float fThreshold );
void  dbPhySetRigidBodySleepAngularVelocity		( int iObject, float fThreshold );
void  dbPhyWakeUpRigidBody						( int iObject, float fThreshold );
void  dbPhySleepRigidBody						( int iObject );
int   dbPhyGetRigidBodySleeping					( int iObject );
float dbPhyGetRigidBodySleepLinearVelocity		( int iObject );
float dbPhyGetRigidBodySleepAngularVelocity		( int iObject );

void  dbPhySetRigidBodyKinematic				( int iObject, int iState );
void  dbPhySetRigidBodyKinematicPosition		( int iObject, float fX, float fY, float fZ );
void  dbPhySetRigidBodyKinematicRotation		( int iObject, float fX, float fY, float fZ );

int   dbPhyGetRigidBodyDynamic					( int iObject );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RAY CAST FUNCTIONS ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int   dbPhyRayCastAllShapes			( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ );
int   dbPhyRayCast					( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ );
int   dbPhyRayCast					( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ, int iType );
int   dbPhyGetRayCastHit			( void );

float dbPhyGetRayCastHitPointX		( void );
float dbPhyGetRayCastHitPointY		( void );
float dbPhyGetRayCastHitPointZ		( void );
float dbPhyGetRayCastHitNormalX		( void );
float dbPhyGetRayCastHitNormalY		( void );
float dbPhyGetRayCastHitNormalZ		( void );
float dbPhyGetRayCastDistance		( void );
float dbPhyGetRayCastTU				( void );
float dbPhyGetRayCastTV				( void );

int   dbPhyGetRayCastObject		    ( void );

int	  dbPhyRayCastClosestShape		( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RAY CAST FUNCTIONS ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeTriggerBox			( int iTrigger, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ );
void dbPhyMakeTriggerSphere			( int iTrigger, float fX, float fY, float fZ, float fRadius );
void dbPhyDeleteTrigger				( int iTrigger );

int  dbPhyTriggerExist				( int iTrigger );
int  dbPhyTriggerType				( int iTrigger );
int	 dbPhyGetTriggerData         	( void );
int	 dbPhyGetTriggerObjectA			( void );
int	 dbPhyGetTriggerObjectB			( void );
int  dbPhyGetTriggerAction			( void );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// VEHICLE FUNCTIONS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyCreateVehicle								( int iObject );
void dbPhyAddVehicleBody							( int iObject, float fSizeX, float fSizeY, float fSizeZ, float fOriginX, float fOriginY, float fOriginZ );
void dbPhyAddVehicleWheel							( int iObject, int iLimb, float fX, float fY, float fZ, float fRadius, float fHeight, int iLocation, int iDirection );
void dbPhyBuildVehicle								( int iObject );

void dbPhySetVehicleMass							( int iObject, float fMass );
void dbPhySetVehicleAuto							( int iObject, int iState );
void dbPhySetVehicleMaxMotor						( int iObject, float fMax );
void dbPhySetVehicleMotorForce						( int iObject, float fForce );
void dbPhySetVehicleWheelPower						( int iObject, int iFront, int iRear );
void dbPhySetVehicleSteeringDelta					( int iObject, float fDelta );
void dbPhySetVehicleMaxSteeringAngle				( int iObject, float fAngle );
void dbPhySetVehicleWheelRotation					( int iObject, int iWheel, int iDirection );
void dbPhySetVehicleSteeringAngle					( int iObject, float fAngle );
void dbPhySetVehicleSteeringValue					( int iObject, float fValue );

void dbPhySetVehicleSuspensionSpring				( int iObject, float fSpring );
void dbPhySetVehicleSuspensionDamping				( int iObject, float fDamping );
void dbPhySetVehicleSuspensionBias					( int iObject, float fBias );

void dbPhySetVehicleRestitution						( int iObject, float fValue );
void dbPhySetVehicleStaticFriction					( int iObject, float fValue );
void dbPhySetVehicleStaticFrictionV					( int iObject, float fValue );
void dbPhySetVehicleDynamicFriction					( int iObject, float fValue );
void dbPhySetVehicleDynamicFrictionV				( int iObject, float fValue );

void dbPhySetVehicleWheelMultiplier					( int iObject, float fValue );

float dbPhyGetVehicleSteeringAngle					( int iObject );
float dbPhyGetVehicleSteeringDelta					( int iObject );
float dbPhyGetVehicleMotorForce						( int iObject );

void  dbPhySetVehicleBrakeTorque					( int iObject, float fValue );

void  dbPhySetVehicleBrakeTorque					( int iObject, int iWheel, float fValue );
void  dbPhySetVehicleMotorTorque					( int iObject, int iWheel, float fValue );
void  dbPhySetVehicleSteeringAngle					( int iObject, int iWheel, float fValue );
void  dbPhySetVehicleWheelRadius					( int iObject, int iWheel, float fValue );
void  dbPhySetVehicleSuspensionTravel				( int iObject, int iWheel, float fValue );
void  dbPhySetVehicleLongitudalTireForceFunction 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
void  dbPhySetVehicleLateralTireForceFunction	 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
void  dbPhySetVehicleWheelFlags						( int iObject, int iWheel, int iFlag );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// FORCE FIELD FUNCTIONS ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyCreateForceField				( int iID );
void dbPhyDeleteForceField				( int iID );

void dbPhySetForceFieldRigidBody		( int iID, int iObject );
void dbPhySetForceFieldClothScale		( int iID, float fScale );
void dbPhySetForceFieldConstant			( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldVelocityTarget	( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldCoordinates		( int iID, int iType );
void dbPhySetForceFieldLinearFalloff	( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldQuadraticFalloff ( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldNoise			( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldPosition			( int iID, float fX, float fY, float fZ );
void dbPhySetForceFieldRigidBodyScale	( int iID, float fScale );
void dbPhySetForceFieldSoftBodyScale	( int iID, float fScale );
void dbPhySetForceFieldTorusRadius		( int iID, float fRadius );

void dbPhyBuildForceField				( int iID );

void dbPhyAddForceFieldBox				( int iID, int iMode, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ );
void dbPhyAddForceFieldSphere			( int iID, int iMode, float fX, float fY, float fZ, float fRadius );
void dbPhyAddForceFieldCapsule			( int iID, int iMode, float fX, float fY, float fZ, float fHeight, float fRadius );

int  dbPhyGetForceFieldExist			( int iID );

// soft body commands

void dbPhyLoadSoftBody  				( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ );
void dbPhyLoadSoftBody  				( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ, int iUseClone );
void dbPhyAttachSoftBodyCapsule			( int iID, float fX, float fY, float fZ, float fHeight, float fRadius, float fDensity, float fAngleX, float fAngleY, float fAngleZ );
void dbPhyAttachSoftBodySphere			( int iID, float fX, float fY, float fZ, float fRadius, float fDensity );
void dbPhyAttachSoftBodyBox				( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fDensity );
void dbPhyDeleteSoftBody				( int iID );
void dbPhyDeleteSoftBody				( int iID, int iUseClone );
void dbPhySetSoftBodyPosition			( int iObject, float fX, float fY, float fZ, int iSmooth );
void dbPhySetSoftBodyRotation			( int iObject, float fX, float fY, float fZ );
void dbPhySetSoftBodyLinearVelocity		( int iID, float fX, float fY, float fZ );
void dbPhySetSoftBodyFrozen				( int iObject, int iAxis, int iState );


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


#endif _DARK_PHYSICS_H_


