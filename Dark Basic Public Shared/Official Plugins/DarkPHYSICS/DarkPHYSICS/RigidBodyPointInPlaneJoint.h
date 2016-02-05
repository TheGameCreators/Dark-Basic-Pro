
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakePointInPlaneJoint				( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakePointInPlaneJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetPointInPlaneJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointInPlaneJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointInPlaneJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPointInPlaneJointGlobalAxis	( int iJoint, int iIndex, float fX, float fY, float fZ );

DARKSDK void dbPhyBuildPointInPlaneJoint			( int iJoint );

bool dbPhySetupPointInPlaneJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
