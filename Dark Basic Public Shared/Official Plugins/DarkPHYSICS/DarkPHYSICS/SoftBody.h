
#include "globals.h"

DARKSDK void dbPhyLoadSoftBody  		( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ );
DARKSDK void dbPhyLoadSoftBody  		( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ, int iCreationMode );

DARKSDK void dbPhyAttachSoftBodyCapsule ( int iID, float fX, float fY, float fZ, float fHeight, float fRadius, float fDensity, float fAngleX, float fAngleY, float fAngleZ );
DARKSDK void dbPhyAttachSoftBodySphere	( int iID, float fX, float fY, float fZ, float fRadius, float fDensity );
DARKSDK void dbPhyAttachSoftBodyBox     ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fDensity );

DARKSDK void dbPhyDeleteSoftBody		( int iID );
DARKSDK void dbPhyDeleteSoftBody		( int iID, int iUseClone );

DARKSDK void dbPhySetSoftBodyPosition	( int iObject, float fX, float fY, float fZ, int iSmooth );
DARKSDK void dbPhySetSoftBodyRotation	( int iObject, float fX, float fY, float fZ );
DARKSDK void dbPhySetSoftBodyLinearVelocity ( int iObject, float fX, float fY, float fZ );

DARKSDK void dbPhySetSoftBodyFrozen		( int iObject, int iAxis, int iState );
