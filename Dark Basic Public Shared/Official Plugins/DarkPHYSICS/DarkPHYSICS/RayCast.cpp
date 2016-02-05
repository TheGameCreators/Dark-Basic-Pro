
#include "DarkSDK.h"
#include "physics.h"

// global variables - used for the camera
float g_fSpeed        = 0.1f;		// speed at which camera moves
float g_fTurn         = 0.03f;		// turn speed for mouse look
float g_fOldCamAngleX = 0.0f;		// to store original x angle
float g_fOldCamAngleY = 0.0f;		// to store original y angle
float g_fCameraAngleX = 0.0f;		// to sotre new x angle
float g_fCameraAngleY = 0.0f;		// to store new y angle

// forward declaration for functions
void userInput ( void );			// handles user input
void showFPS   ( void );			// show frame rate

void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az);

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	dbPositionCamera ( 0, 20, -70 );

	
	dbMakeObjectCube ( 1, 5 );
	dbPositionObject ( 1, 0, 0, 0 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	dbMakeObjectCube ( 2, 5 );
	dbPositionObject ( 2, -15, 0, 0 );
	dbPhyMakeRigidBodyStaticBox ( 2 );

	dbMakeObjectCube ( 3, 5 );
	dbPositionObject ( 3, 15, 0, 0 );
	dbPhyMakeRigidBodyStaticBox ( 3 );

	dbMakeObjectCube ( 4, 5 );
	dbPositionObject ( 4, 0, 20, 0 );

	dbMakeObjectBox ( 5, 1, 15, 1 );
	dbPositionObject ( 5, 0, 15, 0 );

	dbRandomize ( dbTimer ( ) );

	while ( !dbEscapeKey ( ) )
	{
		float fX = dbObjectPositionX ( 4 );
		float fY = dbObjectPositionY ( 4 );
		float fZ = dbObjectPositionZ ( 4 );

		for ( int i = 1; i < 6; i++ )
			dbColorObject ( i, 0 );
		
		dbPhyRayCastAllShapes ( fX, fY, fZ, 0, -1, 0 );

		if ( dbPhyGetRayCastHit ( ) )
		{
			dbColorObject ( dbPhyGetRayCastObject ( ), dbRgb ( 0, 0, 255 ) );
		}

		char* szKey = dbInKey ( );

		if ( strcmp ( szKey, "a" ) == 0 )
		{
			dbMoveObjectLeft ( 4, 0.4f );
			dbMoveObjectLeft ( 5, 0.4f );
		}

		if ( strcmp ( szKey, "d" ) == 0 )
		{
			dbMoveObjectRight ( 4, 0.4f );
			dbMoveObjectRight ( 5, 0.4f );	
		}
		


		//showFPS   ( );
		//userInput ( );

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