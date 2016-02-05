
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakePrismaticJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakePrismaticJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetPrismaticJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPrismaticJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPrismaticJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPrismaticJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );

DARKSDK void dbPhyBuildPrismaticJoint				( int iJoint );

bool dbPhySetupPrismaticJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
