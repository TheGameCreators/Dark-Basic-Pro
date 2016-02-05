
/////////////////////////////////////////////////////////////////////////////////////
// CLOTH PROPERTIES /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	contains all functions relating to cloth properties
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INCLUDES /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "globals.h"

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// EXPOSED CLOTH FUNCTIONS //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void  dbPhySetClothDimensions						( int iID, float fWidth, float fHeight, float fDepth );
DARKSDK void  dbPhySetClothPosition							( int iID, float fX, float fY, float fZ );
DARKSDK void  dbPhySetClothRotation							( int iID, float fX, float fY, float fZ );
DARKSDK void  dbPhySetClothStatic							( int iID, int iState );
DARKSDK void  dbPhySetClothCollision						( int iID, int iState );
DARKSDK void  dbPhySetClothGravity							( int iID, int iState );
DARKSDK void  dbPhySetClothBending							( int iID, int iState );
DARKSDK void  dbPhySetClothDamping							( int iID, int iState );
DARKSDK void  dbPhySetClothTwoWayCollision					( int iID, int iState );
DARKSDK void  dbPhySetClothTearing							( int iID, int iState );
DARKSDK void  dbPhySetClothThickness						( int iID, float fValue );
DARKSDK void  dbPhySetClothDensity							( int iID, float fValue );
DARKSDK void  dbPhySetClothBendingStiffness					( int iID, float fValue );
DARKSDK void  dbPhySetClothStretchingStiffness				( int iID, float fValue );
DARKSDK void  dbPhySetClothFriction							( int iID, float fValue );
DARKSDK void  dbPhySetClothPressure							( int iID, float fValue );
DARKSDK void  dbPhySetClothTearFactor						( int iID, float fValue );
DARKSDK void  dbPhySetClothCollisionResponseCoefficient		( int iID, float fValue );
DARKSDK void  dbPhySetClothAttachmentResponseCoefficient	( int iID, float fValue );
DARKSDK void  dbPhySetClothSolverIterations					( int iID, int iValue );
DARKSDK void  dbPhySetClothExternalAcceleration				( int iID, float fX, float fY, float fZ );
DARKSDK void  dbPhySetClothCollisionGroup					( int iID, int iGroup );

DARKSDK void  dbPhyAddForceAtClothVertex					( int iID, int iVertex, float fX, float fY, float fZ, int iMode );

DARKSDK void  dbPhyClothAttachRigidBody						( int iID, int iRigidBody, float fImpulse, float fPenetration, float fDeformation );

DARKSDK int	  dbPhyGetClothExist							( int iID );
DARKSDK int	  dbPhyGetClothBuilt							( int iID );

DARKSDK DWORD dbPhyGetClothWidthEx							( int iID );
DARKSDK DWORD dbPhyGetClothHeightEx							( int iID );
DARKSDK DWORD dbPhyGetClothDepthEx							( int iID );
DARKSDK DWORD dbPhyGetClothPositionXEx						( int iID );
DARKSDK DWORD dbPhyGetClothPositionYEx						( int iID );
DARKSDK DWORD dbPhyGetClothPositionZEx						( int iID );
DARKSDK DWORD dbPhyGetClothRotationXEx						( int iID );
DARKSDK DWORD dbPhyGetClothRotationYEx						( int iID );
DARKSDK DWORD dbPhyGetClothRotationZEx						( int iID );
DARKSDK float dbPhyGetClothWidth							( int iID );
DARKSDK float dbPhyGetClothHeight							( int iID );
DARKSDK float dbPhyGetClothDepth							( int iID );
DARKSDK float dbPhyGetClothPositionX						( int iID );
DARKSDK float dbPhyGetClothPositionY						( int iID );
DARKSDK float dbPhyGetClothPositionZ						( int iID );
DARKSDK float dbPhyGetClothRotationX						( int iID );
DARKSDK float dbPhyGetClothRotationY						( int iID );
DARKSDK float dbPhyGetClothRotationZ						( int iID );

DARKSDK int   dbPhyGetClothStatic							( int iID );
DARKSDK int   dbPhyGetClothCollision						( int iID );
DARKSDK int   dbPhyGetClothGravity							( int iID );
DARKSDK int   dbPhyGetClothBending							( int iID );
DARKSDK int   dbPhyGetClothDamping							( int iID );
DARKSDK int   dbPhyGetClothTwoWayCollision					( int iID );
DARKSDK int   dbPhyGetClothTearing							( int iID );

DARKSDK DWORD dbPhyGetClothThicknessEx						( int iID );
DARKSDK DWORD dbPhyGetClothDensityEx						( int iID );
DARKSDK DWORD dbPhyGetClothBendingStiffnessEx				( int iID );
DARKSDK DWORD dbPhyGetClothStretchingStiffnessEx			( int iID );
DARKSDK DWORD dbPhyGetClothFrictionEx						( int iID );
DARKSDK DWORD dbPhyGetClothPressureEx						( int iID );
DARKSDK DWORD dbPhyGetClothTearFactorEx						( int iID );
DARKSDK DWORD dbPhyGetClothCollisionResponseCoefficientEx	( int iID );
DARKSDK DWORD dbPhyGetClothAttachmentResponseCoefficientEx	( int iID );
DARKSDK float dbPhyGetClothThickness						( int iID );
DARKSDK float dbPhyGetClothDensity							( int iID );
DARKSDK float dbPhyGetClothBendingStiffness					( int iID );
DARKSDK float dbPhyGetClothStretchingStiffness				( int iID );
DARKSDK float dbPhyGetClothFriction							( int iID );
DARKSDK float dbPhyGetClothPressure							( int iID );
DARKSDK float dbPhyGetClothTearFactor						( int iID );
DARKSDK float dbPhyGetClothCollisionResponseCoefficient		( int iID );
DARKSDK float dbPhyGetClothAttachmentResponseCoefficient	( int iID );

DARKSDK int   dbPhyGetClothSolverIterations					( int iID );

DARKSDK DWORD dbPhyGetClothExternalAccelerationXEx			( int iID );
DARKSDK DWORD dbPhyGetClothExternalAccelerationYEx			( int iID );
DARKSDK DWORD dbPhyGetClothExternalAccelerationZEx			( int iID );
DARKSDK float dbPhyGetClothExternalAccelerationX			( int iID );
DARKSDK float dbPhyGetClothExternalAccelerationY			( int iID );
DARKSDK float dbPhyGetClothExternalAccelerationZ			( int iID );
DARKSDK int   dbPhyGetClothCollisionGroup					( int iID );

DARKSDK DWORD dbPhyGetClothMinSizeXEx						( int iID );
DARKSDK DWORD dbPhyGetClothMinSizeYEx						( int iID );
DARKSDK DWORD dbPhyGetClothMinSizeZEx						( int iID );
DARKSDK DWORD dbPhyGetClothMaxSizeXEx						( int iID );
DARKSDK DWORD dbPhyGetClothMaxSizeYEx						( int iID );
DARKSDK DWORD dbPhyGetClothMaxSizeZEx						( int iID );
DARKSDK float dbPhyGetClothMinSizeX							( int iID );
DARKSDK float dbPhyGetClothMinSizeY							( int iID );
DARKSDK float dbPhyGetClothMinSizeZ							( int iID );
DARKSDK float dbPhyGetClothMaxSizeX							( int iID );
DARKSDK float dbPhyGetClothMaxSizeY							( int iID );
DARKSDK float dbPhyGetClothMaxSizeZ							( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL CLOTH FUNCTIONS /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

float dbPhyGetClothSize								( int iID, int iIndex );
void  dbPhySetClothFlag								( int iID, NxClothFlag flag, int iState );
int   dbPhyGetClothFlag								( int iID, NxClothFlag flag );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////