
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeSphereJoint				( int iJoint, int iA, int iB, float fX, float fY, float fZ );
DARKSDK void dbPhyMakeSphereJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetSphereJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetSphereJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetSphereJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetSphereJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetSphereJointSwingAxis		( int iJoint, float fX, float fY, float fZ );
DARKSDK void dbPhySetSphereJointSwingLimit		( int iJoint, float fValue, float fRestitution, float fHardness );
DARKSDK void dbPhySetSphereJointTwistLimit		( int iJoint, float fLow, float fHigh );
DARKSDK void dbPhySetSphereJointTwistSpring		( int iJoint, float fTarget, float fSpring );
DARKSDK void dbPhySetSphereJointSwingSpring		( int iJoint, float fTarget, float fSpring );
DARKSDK void dbPhySetSphereJointSpring			( int iJoint, float fTarget, float fSpring );
DARKSDK void dbPhySetSphereJointProjection		( int iJoint, int iMode, float fDistance );

DARKSDK void dbPhyBuildSphereJoint				( int iJoint );

bool dbPhySetupSphereJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
