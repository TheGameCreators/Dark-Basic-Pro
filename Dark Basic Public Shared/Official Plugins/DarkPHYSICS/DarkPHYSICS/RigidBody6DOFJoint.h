
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMake6DOFJoint						( int iJoint, int iA, int iB );

DARKSDK void dbPhySet6DOFJointLocalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointLocalAxis				( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointLocalNormal			( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointGlobalAnchor			( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointGlobalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointTwistMotion			( int iJoint, int iMotion );
DARKSDK void dbPhySet6DOFJointSwing1Motion			( int iJoint, int iMotion );
DARKSDK void dbPhySet6DOFJointSwing2Motion			( int iJoint, int iMotion );
DARKSDK void dbPhySet6DOFJointMotionX				( int iJoint, int iX );
DARKSDK void dbPhySet6DOFJointMotionY				( int iJoint, int iY );
DARKSDK void dbPhySet6DOFJointMotionZ				( int iJoint, int iZ );
DARKSDK void dbPhySet6DOFJointLinearLimit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
DARKSDK void dbPhySet6DOFJointSwing1Limit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
DARKSDK void dbPhySet6DOFJointSwing2Limit			( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping );
DARKSDK void dbPhySet6DOFJointTwistLimit			( int iJoint, float fDampingLow, float fRestitutionLow, float fSpringLow, float fValueLow, float fDampingHigh, float fRestitutionHigh, float fSpringHigh, float fValueHigh );
DARKSDK void dbPhySet6DOFJointDriveX				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointDriveY				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointDriveZ				( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointTwistDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointSwingDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointSlerpDrive			( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit );
DARKSDK void dbPhySet6DOFJointDrivePosition			( int iJoint, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointDriveRotation			( int iJoint, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointDriveLinearVelocity	( int iJoint, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointDriveAngularVelocity	( int iJoint, float fX, float fY, float fZ );
DARKSDK void dbPhySet6DOFJointProjection			( int iJoint, int iMode, float fDistance );
DARKSDK void dbPhySet6DOFJointSlerpDriveState		( int iJoint, int iState );
DARKSDK void dbPhySet6DOFJointGear					( int iJoint, int iState );

DARKSDK void dbPhyBuild6DOFJoint					( int iJoint );

bool dbPhySetup6DOFJoint ( int iJoint, int iA, int iB );
bool dbPhyCheck6DOFJointIndex ( int iIndex );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
