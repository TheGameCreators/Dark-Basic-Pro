
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY CHARACTER CONTROLLER COMMANDS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeBoxCharacterController			( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, int iUp, float fStep, float fSlopeLimit );
DARKSDK void dbPhyMakeCapsuleCharacterController		( int iID, float fX, float fY, float fZ, float fRadius, float fHeight, int iUp, float fStep, float fSlopeLimit );
DARKSDK void dbPhyMoveCharacterController				( int iID, float fSpeed );

DARKSDK void dbPhySetCharacterControllerDisplacement	( int iID, float fForward, float fBackward );
DARKSDK void dbPhySetCharacterControllerDisplacement	( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetCharacterControllerDisplacement	( int iID, int iState );
DARKSDK void dbPhySetCharacterControllerSharpness		( int iID, float fValue );
DARKSDK void dbPhySetCharacterControllerExtents			( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetCharacterControllerExtents			( int iID, float fRadius, float fHeight );
DARKSDK void dbPhySetCharacterControllerMinDistance     ( int iID, float fValue );

DARKSDK int  dbPhyGetCharacterControllerExist			( int iID );
DARKSDK void dbPhyDeleteCharacterController				( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY CHARACTER CONTROLLER COMMANDS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

