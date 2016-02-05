
#include "DarkSDK.h"
#include "physics.h"

float g_fSpeed        = 0.1f;
float g_fTurn         = 0.03f;
float g_fOldCamAngleX = 0.0f;
float g_fOldCamAngleY = 0.0f;
float g_fCameraAngleX = 0.0f;
float g_fCameraAngleY = 0.0f;

void userInput ( void );
void showFPS   ( void );

/*
// RESITUTION
void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );

	dbMakeObjectBox ( 1, 40, 1, 40 );
	dbPhyMakeRigidBodyStaticBox ( 1 );
	dbColorObject ( 1, dbRgb ( 10, 10, 10 ) );

	dbPhyMakeMaterial ( 1, "bouncy" );
	dbPhySetMaterialRestitution	( 1, 0.0f );
	dbPhyBuildMaterial ( 1 );

	dbMakeObjectCube ( 2, 2.5 );
	dbPositionObject ( 2, 0, 10, 0 );
	dbPhyMakeRigidBodyDynamicBox ( 2, 1 );
	dbColorObject ( 2, dbRgb ( 0, 0, 255 ) );

	dbPositionCamera ( 0, 5, -20 );
	
	while ( !dbEscapeKey ( ) )
	{
		dbPhyUpdate ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
}
*/

// DYNAMIC FRICTION
void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );

	dbMakeObjectBox ( 1, 40, 1, 40 );
	dbPhyMakeRigidBodyStaticBox ( 1 );
	dbColorObject ( 1, dbRgb ( 10, 10, 10 ) );

	dbPhyMakeMaterial ( 1, "bouncy" );
	//dbPhySetMaterialRestitution	( 1, 0.0f );
	dbPhySetMaterialDynamicFriction ( 1, 1.0f );
	dbPhyBuildMaterial ( 1 );

	dbMakeObjectCube ( 2, 2.5 );
	dbPositionObject ( 2, -10, 1.5, 0 );
	dbPhyMakeRigidBodyDynamicBox ( 2, 1 );
	dbColorObject ( 2, dbRgb ( 0, 0, 255 ) );

	dbPositionCamera ( 0, 5, -20 );
	
	while ( !dbEscapeKey ( ) )
	{
		if ( dbSpaceKey ( ) )
			dbPhySetRigidBodyLinearVelocity ( 2, 10, 0, 0 );

		dbPhyUpdate ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
}


void showFPS ( void )
{
	char szFPS [ 256 ] = "";
	strcpy ( szFPS, "fps = " );
	strcat ( szFPS, dbStr ( dbScreenFPS ( ) ) );
	dbText ( dbScreenWidth ( ) - 20 - dbTextWidth ( szFPS ), dbScreenHeight ( ) - 40, szFPS );

	dbSetCursor ( 0, 0 );
	strcpy ( szFPS, dbStr ( dbCameraPositionX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionZ ( ) ) );
	dbPrint ( szFPS );

	strcpy ( szFPS, dbStr ( dbCameraAngleX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleZ ( ) ) );
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
}