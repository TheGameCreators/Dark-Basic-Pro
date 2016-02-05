
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeCylindricalJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakeCylindricalJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetCylindricalJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetCylindricalJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetCylindricalJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetCylindricalJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );

DARKSDK void dbPhyBuildCylindricalJoint				( int iJoint );

bool dbPhySetupCylindricalJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
