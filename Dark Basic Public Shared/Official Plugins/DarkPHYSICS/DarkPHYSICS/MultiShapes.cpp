
#include "DarkSDK.h"
#include "physics.h"
#include "clothcommands.h"

float g_fSpeed        = 0.1f;
float g_fTurn         = 0.03f;
float g_fOldCamAngleX = 0.0f;
float g_fOldCamAngleY = 0.0f;
float g_fCameraAngleX = 0.0f;
float g_fCameraAngleY = 0.0f;

void userInput ( void );
void showFPS   ( void );

void dbPhyMakeCloth ( int iID );
void dbPhyUpdateCloth ( void );

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );

	dbRandomize ( dbTimer ( ) );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbMakeObjectBox ( 1, 40, 1, 40 );
	dbPositionObject ( 1, 0, -10, 0 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	float fHeight = 15.0f;

	/*
	//dbLoadObject ( "boxes.x", 2 );
	//dbLoadObject ( "boxes1.x", 2 );
	dbLoadObject ( "table.x", 2 );
	//dbPositionObject ( 2, 10, 0, 0 );
	dbPositionObject ( 2, 0, 0, 0 );
	//dbPositionObject ( 2, 0, 0.0f, 0 );
	dbShowObjectBounds ( 2, 1 );



	dbOffsetLimb ( 2, 0, dbObjectCollisionCenterX ( 2 ) * -1, dbObjectCollisionCenterY ( 2 ) * -1, dbObjectCollisionCenterZ ( 2 ) * -1 );

	dbPhyMakeRigidBodyDynamicBoxes ( 2 );
	//dbPositionObject ( 2, 0, 0.0f, 0 );

	//dbLoadObject ( "boxes1.x", 202 );
	//dbPositionObject ( 202, 0, 15, 0 );
	//dbShowObjectBounds ( 202, 1 );
	
	//dbPhyMakeRigidBodyDynamicBox ( 2 );

	//dbMakeObjectCube ( 201, 5 );
	//dbPositionObject ( 201, 0, fHeight, 0 );
	*/

	dbMakeObjectCube ( 2, 5 );
	dbPositionObject ( 2, 0, 2.5, 0 );
	dbPhyMakeRigidBodyDynamicBox ( 2 );

	float fX = -10;

	for ( int i = 3; i < 10; i++ )
	{
		dbMakeObjectSphere ( i, 2.0f );
		dbPositionObject ( i, fX, -9, -10.0f );
		//dbPositionObject ( i, -10 + dbRnd ( 20 ),fY, -10.0f + dbRnd ( 20 ) );
		dbPhyMakeRigidBodyDynamicSphere ( i );

		fX += 3.0f;
		//fY += 5.0f;
	}
	

	for ( int i = 1; i < 200; i++ )
	{
		if ( dbObjectExist ( i ) )
		{
			dbColorObject			 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecular		 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecularPower ( i, 255 );
			dbSetObjectAmbient		 ( i, 0 );
		}
	}

	//dbPositionCamera ( 0, 10, -90 );
	dbPositionCamera ( 0, 10, -50 );

	while ( !dbEscapeKey ( ) )
	{
		if ( dbSpaceKey ( ) )
		{
			for ( int i = 3; i < 10; i++ )
				dbPhySetRigidBodyLinearVelocity ( i, 0, 0, 4 );
		}

		if ( dbReturnKey ( ) )
		{
			dbPhyDeleteRigidBody ( 2 );
			dbPhyMakeRigidBodyStaticBox ( 2 );
		}
			//dbPhySetRigidBodyPosition ( 2, 10.0f, 2.5f, 0.0f );

		showFPS ( );
		userInput ( );
	
		dbPhyUpdate ( );
		//dbPhyUpdateCloth ( );
		dbSync      ( );

		//dbPositionObject ( 2, dbObjectPositionX ( 2 ), dbObjectPositionY ( 2 ) - dbObjectCollisionCenterY ( 2 ), dbObjectPositionZ ( 2 ) );

		//dbSleep ( 1000 );
	}

	dbPhyEnd ( );
}


void showFPS ( void )
{
	dbSetCursor ( 0, 0 );

	char szFPS [ 256 ] = "";
	strcpy ( szFPS, "fps = " );
	strcat ( szFPS, dbStr ( dbScreenFPS ( ) ) );
	dbText ( dbScreenWidth ( ) - 20 - dbTextWidth ( szFPS ), dbScreenHeight ( ) - 40, szFPS );

	strcpy ( szFPS, dbStr ( dbStatistic ( 1 ) ) );
	dbPrint ( szFPS );

	/*
	strcpy ( szFPS, dbStr ( dbCameraAngleX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleZ ( ) ) );
	dbPrint ( szFPS );
	*/

	strcpy ( szFPS, dbStr ( dbCameraPositionX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionZ ( ) ) );
	dbPrint ( szFPS );

}

void userInput ( void )
{
	dbControlCameraUsingArrowKeys ( 0, g_fSpeed, g_fTurn );

	g_fOldCamAngleY = g_fCameraAngleY;
	g_fOldCamAngleX = g_fCameraAngleX;

	g_fCameraAngleY = dbWrapValue ( g_fCameraAngleY + dbMouseMoveX ( ) * 0.4f );
	g_fCameraAngleX = dbWrapValue ( g_fCameraAngleX + dbMouseMoveY ( ) * 0.4f );

	dbYRotateCamera ( dbCurveAngle ( g_fCameraAngleY, g_fOldCamAngleY, 24 ) );
	dbXRotateCamera ( dbCurveAngle ( g_fCameraAngleX, g_fOldCamAngleX, 24 ) );

	char* szKey = dbInKey ( );

	if ( strcmp ( szKey, "+" ) == 0 )
	{
		if ( g_fSpeed < 1000 )
			g_fSpeed = g_fSpeed + 0.01f;
	}

	if ( strcmp ( szKey, "-" ) == 0 )
	{
		if ( g_fSpeed > 0.02f )
			g_fSpeed = g_fSpeed - 0.01f;
	}

	//float fHeight = dbGetTerrainGroundHeight ( 1, dbCameraPositionX ( ), dbCameraPositionZ ( ) );
	//dbPositionCamera ( dbCameraPositionX ( ), fHeight + 3.0f, dbCameraPositionZ ( ) );
}