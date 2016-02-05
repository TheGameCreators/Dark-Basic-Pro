
/////////////////////////////////////////////////////////////////////////////////////
// ENGINE COMMANDS //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyCreateForceField				( int iID );
DARKSDK void dbPhyDeleteForceField				( int iID );

DARKSDK void dbPhySetForceFieldRigidBody		( int iID, int iObject );
DARKSDK void dbPhySetForceFieldClothScale		( int iID, float fScale );
DARKSDK void dbPhySetForceFieldConstant			( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldVelocityTarget	( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldCoordinates		( int iID, int iType );
DARKSDK void dbPhySetForceFieldLinearFalloff	( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldQuadraticFalloff ( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldNoise			( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldPosition			( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetForceFieldRigidBodyScale	( int iID, float fScale );
DARKSDK void dbPhySetForceFieldSoftBodyScale	( int iID, float fScale );
DARKSDK void dbPhySetForceFieldTorusRadius		( int iID, float fRadius );

DARKSDK void dbPhyBuildForceField				( int iID );

DARKSDK void dbPhyAddForceFieldBox				( int iID, int iMode, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ );
DARKSDK void dbPhyAddForceFieldSphere			( int iID, int iMode, float fX, float fY, float fZ, float fRadius );
DARKSDK void dbPhyAddForceFieldCapsule			( int iID, int iMode, float fX, float fY, float fZ, float fHeight, float fRadius );

DARKSDK int  dbPhyGetForceFieldExist			( int iID );

//DARKSDK void dbPhyCreateForceFieldWind			( int iID, float fDirectionX, float fDirectionY, float fDirectionZ, float fSizeX, float fSizeY, float fSizeZ );



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
