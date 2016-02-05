
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeDistanceJoint					( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fMinDistance, float fMaxDistance );
DARKSDK void dbPhyMakeDistanceJoint					( int iJoint, int iA, int iB );

DARKSDK void dbPhySetDistanceJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetDistanceJointGlobalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetDistanceJointLocalAxis			( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetDistanceJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetDistanceJointMax				( int iJoint, float fMax );
DARKSDK void dbPhySetDistanceJointMin				( int iJoint, float fMin );
DARKSDK void dbPhySetDistanceJointSpring			( int iJoint, float fTarget, float fSpring, float fDamper );

DARKSDK void dbPhyBuildDistanceJoint				( int iJoint );

bool dbPhySetupDistanceJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
