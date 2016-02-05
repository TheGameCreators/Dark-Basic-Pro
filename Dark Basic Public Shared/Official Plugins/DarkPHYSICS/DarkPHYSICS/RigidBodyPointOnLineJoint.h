
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakePointOnLineJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakePointOnLineJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetPointOnLineJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointOnLineJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointOnLineJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointOnLineJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );

DARKSDK void dbPhyBuildPointOnLineJoint				( int iJoint );

bool dbPhySetupPointOnLineJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
