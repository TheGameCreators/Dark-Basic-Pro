
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY RAYCAST COMMANDS //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK int dbPhyRayCastAllShapes ( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ );

DARKSDK int dbPhyRayCast ( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ );
DARKSDK int dbPhyRayCast ( float fOriginX, float fOriginY, float fOriginZ, float fNewX, float fNewY, float fNewZ, int iType );

DARKSDK int   dbPhyGetRayCastHit        ( void );

DARKSDK float dbPhyGetRayCastHitPointX		( void );
DARKSDK float dbPhyGetRayCastHitPointY		( void );
DARKSDK float dbPhyGetRayCastHitPointZ		( void );
DARKSDK float dbPhyGetRayCastHitNormalX		( void );
DARKSDK float dbPhyGetRayCastHitNormalY		( void );
DARKSDK float dbPhyGetRayCastHitNormalZ		( void );
DARKSDK float dbPhyGetRayCastDistance		( void );
DARKSDK float dbPhyGetRayCastTU				( void );
DARKSDK float dbPhyGetRayCastTV				( void );

DARKSDK DWORD dbPhyGetRayCastHitPointXEx	( void );
DARKSDK DWORD dbPhyGetRayCastHitPointYEx	( void );
DARKSDK DWORD dbPhyGetRayCastHitPointZEx	( void );
DARKSDK DWORD dbPhyGetRayCastHitNormalXEx	( void );
DARKSDK DWORD dbPhyGetRayCastHitNormalYEx	( void );
DARKSDK DWORD dbPhyGetRayCastHitNormalZEx	( void );
DARKSDK DWORD dbPhyGetRayCastDistanceEx		( void );
DARKSDK DWORD dbPhyGetRayCastTUEx			( void );
DARKSDK DWORD dbPhyGetRayCastTVEx			( void );

DARKSDK int   dbPhyGetRayCastObject		    ( void );

DARKSDK int dbPhyRayCastClosestShape ( float fOriginX, float fOriginY, float fOriginZ, float fDirX, float fDirY, float fDirZ );


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY RAYCAST COMMANDS //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

