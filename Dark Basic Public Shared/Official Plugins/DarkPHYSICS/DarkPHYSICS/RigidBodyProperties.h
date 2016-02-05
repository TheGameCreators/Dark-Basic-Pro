
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// position
DARKSDK void  dbPhySetRigidBodyPosition					( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyRotation					( int iObject, float fX, float fY, float fZ );

// gravity
DARKSDK void  dbPhySetRigidBodyGravity					( int iObject, int iState );

// groups
DARKSDK void  dbPhySetRigidBodyGroup					( int iObject, int iGroup );

// mass
DARKSDK void  dbPhySetRigidBodyMass						( int iObject, float fMass );
DARKSDK DWORD dbPhyGetRigidBodyMassEx					( int iObject );
DARKSDK float dbPhyGetRigidBodyMass						( int iObject );
DARKSDK void  dbPhySetRigidBodyMassOffsetLocal			( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyMassOffsetGlobal			( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhyUpdateRigidBodyMass					( int iObject, float fDensity, float fTotalMass );

// damping
DARKSDK void  dbPhySetRigidBodyLinearDamping			( int iObject, float fDamping );
DARKSDK void  dbPhySetRigidBodyAngularDamping			( int iObject, float fAngular );
DARKSDK DWORD dbPhyGetRigidBodyLinearDampingEx			( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularDampingEx			( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearDamping			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularDamping			( int iObject );

// velocity
DARKSDK void  dbPhySetRigidBodyLinearVelocity			( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyAngularVelocity			( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyMaxAngularVelocity		( int iObject, float fMax );
DARKSDK DWORD dbPhyGetRigidBodyLinearVelocityXEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyLinearVelocityYEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyLinearVelocityZEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularVelocityXEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularVelocityYEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularVelocityZEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyMaxAngularVelocityEx		( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearVelocityX			( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearVelocityY			( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearVelocityZ			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularVelocityX			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularVelocityY			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularVelocityZ			( int iObject );
DARKSDK float dbPhyGetRigidBodyMaxAngularVelocity		( int iObject );

// ccd
DARKSDK void  dbPhySetRigidBodyCCDMotionThreshold		( int iObject, float fThreshold );
DARKSDK DWORD dbPhyGetRigidBodyCCDMotionThresholdEx		( int iObject );
DARKSDK float dbPhyGetRigidBodyCCDMotionThreshold		( int iObject );
DARKSDK void  dbPhySetRigidBodyCCD						( int iObject, int iState );

// momentum
DARKSDK void  dbPhySetRigidBodyLinearMomentum			( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyAngularMomentum			( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyLinearMomentumXEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyLinearMomentumYEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyLinearMomentumZEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularMomentumXEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularMomentumYEx		( int iObject );
DARKSDK DWORD dbPhyGetRigidBodyAngularMomentumZEx		( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearMomentumX			( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearMomentumY			( int iObject );
DARKSDK float dbPhyGetRigidBodyLinearMomentumZ			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularMomentumX			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularMomentumY			( int iObject );
DARKSDK float dbPhyGetRigidBodyAngularMomentumZ			( int iObject );

// forces
DARKSDK void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyForce					( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyForceAtLocal				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyLocalForce				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyLocalForceAtLocal		( int iObject, float fForceX, float fForceY, float fForceZ, float fX, float fY, float fZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyTorque					( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );
DARKSDK void  dbPhyAddRigidBodyLocalTorque				( int iObject, float fForceX, float fForceY, float fForceZ, int iMode );

// point
DARKSDK DWORD dbPhyGetRigidBodyPointVelocityXEx			( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyPointVelocityYEx			( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyPointVelocityZEx			( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyLocalPointVelocityXEx	( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyLocalPointVelocityYEx	( int iObject, float fX, float fY, float fZ );
DARKSDK DWORD dbPhyGetRigidBodyLocalPointVelocityZEx	( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyPointVelocityX			( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyPointVelocityY			( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyPointVelocityZ			( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyLocalPointVelocityX		( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyLocalPointVelocityY		( int iObject, float fX, float fY, float fZ );
DARKSDK float dbPhyGetRigidBodyLocalPointVelocityZ		( int iObject, float fX, float fY, float fZ );

// sleeping
DARKSDK void  dbPhySetRigidBodySleepLinearVelocity		( int iObject, float fThreshold );
DARKSDK void  dbPhySetRigidBodySleepAngularVelocity		( int iObject, float fThreshold );
DARKSDK void  dbPhyWakeUpRigidBody						( int iObject, float fThreshold );
DARKSDK void  dbPhySleepRigidBody						( int iObject );
DARKSDK int   dbPhyGetRigidBodySleeping					( int iObject );
DARKSDK DWORD dbPhyGetRigidBodySleepLinearVelocityEx	( int iObject );
DARKSDK DWORD dbPhyGetRigidBodySleepAngularVelocityEx	( int iObject );
DARKSDK float dbPhyGetRigidBodySleepLinearVelocity		( int iObject );
DARKSDK float dbPhyGetRigidBodySleepAngularVelocity		( int iObject );

// kinematic
DARKSDK void  dbPhySetRigidBodyKinematic				( int iObject, int iState );
DARKSDK void  dbPhySetRigidBodyKinematicPosition		( int iObject, float fX, float fY, float fZ );
DARKSDK void  dbPhySetRigidBodyKinematicRotation		( int iObject, float fX, float fY, float fZ );

// state
DARKSDK int   dbPhyGetRigidBodyDynamic					( int iObject );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

