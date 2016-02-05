
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COLLISION COMMANDS ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


DARKSDK int		dbPhyGetCollisionData				( void );
DARKSDK int		dbPhyGetCollisionObjectA			( void );
DARKSDK int		dbPhyGetCollisionObjectB			( void );

DARKSDK float	dbPhyGetCollisionNormalForceX		( void );
DARKSDK float	dbPhyGetCollisionNormalForceY		( void );
DARKSDK float	dbPhyGetCollisionNormalForceZ		( void );
DARKSDK float	dbPhyGetCollisionFrictionForceX		( void );
DARKSDK float	dbPhyGetCollisionFrictionForceY		( void );
DARKSDK float	dbPhyGetCollisionFrictionForceZ		( void );

/*
DARKSDK float	dbPhyGetCollisionPointX				( void );
DARKSDK float	dbPhyGetCollisionPointY				( void );
DARKSDK float	dbPhyGetCollisionPointZ				( void );
DARKSDK float	dbPhyGetCollisionNormalX			( void );
DARKSDK float	dbPhyGetCollisionNormalY			( void );
DARKSDK float	dbPhyGetCollisionNormalZ			( void );
*/

DARKSDK DWORD	dbPhyGetCollisionNormalForceXEx		( void );
DARKSDK DWORD	dbPhyGetCollisionNormalForceYEx		( void );
DARKSDK DWORD	dbPhyGetCollisionNormalForceZEx		( void );
DARKSDK DWORD	dbPhyGetCollisionFrictionForceXEx	( void );
DARKSDK DWORD	dbPhyGetCollisionFrictionForceYEx	( void );
DARKSDK DWORD	dbPhyGetCollisionFrictionForceZEx	( void );

DARKSDK int		dbPhyGetCollision					( int iObjectA, int iObjectB );
DARKSDK int		dbPhyGetCollisionCount				( int iObject );

DARKSDK void	dbPhySetCollisionState				( int iObject, int iState );

DARKSDK float	dbPhyGetCollisionFrictionForceX		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionFrictionForceY		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionFrictionForceZ		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionNormalForceX		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionNormalForceY		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionNormalForceZ		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactNormalX		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactNormalY		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactNormalZ		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactPointX		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactPointY		( int iObject, int iIndex );
DARKSDK float	dbPhyGetCollisionContactPointZ		( int iObject, int iIndex );

DARKSDK DWORD	dbPhyGetCollisionFrictionForceXEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionFrictionForceYEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionFrictionForceZEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionNormalForceXEx		( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionNormalForceYEx		( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionNormalForceZEx		( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactNormalXEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactNormalYEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactNormalZEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactPointXEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactPointYEx	( int iObject, int iIndex );
DARKSDK DWORD	dbPhyGetCollisionContactPointZEx	( int iObject, int iIndex );

DARKSDK void dbPhySetCollisionState ( int iObject, int iState );


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COLLISION COMMANDS ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

