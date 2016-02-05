
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeRevoluteJoint				( int iJoint, int iA, int iB, float fAxisX, float fAxisY, float fAxisZ, float fX, float fY, float fZ );
DARKSDK void dbPhyMakeRevoluteJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetRevoluteJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetRevoluteJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetRevoluteJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetRevoluteJointGlobalAxis	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetRevoluteJointSpring		( int iJoint, float fTarget, float fSpring );
DARKSDK void dbPhySetRevoluteJointLimit			( int iJoint, float fLow, float fHigh );
DARKSDK void dbPhySetRevoluteJointMotor			( int iJoint, float fVelocityTarget, float fMaxForce, int iFreeSpin );
DARKSDK void dbPhySetRevoluteJointProjection	( int iJoint, int iMode, float fDistance, float fAngle );

DARKSDK void dbPhyBuildRevoluteJoint			( int iJoint );

bool dbPhySetupRevoluteJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
