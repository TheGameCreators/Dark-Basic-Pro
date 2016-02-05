
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakePulleyJoint				( int iJoint, int iA, int iB, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float fX3, float fY3, float fZ3, float fX4, float fY4, float fZ4, float fDistance, float fStiffness, float fRatio );
DARKSDK void dbPhyMakePulleyJoint				( int iJoint, int iA, int iB );

DARKSDK void dbPhySetPulleyJointLocalAnchor		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPulleyJointGlobalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPulleyJointLocalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPulleyJointGlobalAxis		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPulleyJointSuspension		( int iJoint, int iIndex, float fX, float fY, float fZ );
DARKSDK void dbPhySetPulleyJointDistance		( int iJoint, float fDistance );
DARKSDK void dbPhySetPulleyJointStiffness		( int iJoint, float fStiffness );
DARKSDK void dbPhySetPulleyJointRatio			( int iJoint, float fRatio );
DARKSDK void dbPhySetPulleyJointRigid			( int iJoint, int iState );
DARKSDK void dbPhySetPulleyJointMotor			( int iJoint, int iState );

DARKSDK void dbPhyBuildPulleyJoint				( int iJoint );

bool dbPhySetupPulleyJoint ( int iJoint, int iA, int iB );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
