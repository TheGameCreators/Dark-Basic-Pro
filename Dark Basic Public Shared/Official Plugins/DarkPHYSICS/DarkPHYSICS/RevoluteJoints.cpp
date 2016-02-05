
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

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	dbSetCameraRange ( 0.5f, 30000 );
	
	dbSetTextFont ( "tahoma" );
	dbSetTextSize ( 32 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	// floor
	dbMakeObjectBox             ( 1, 210, 1, 210 );
	dbPositionObject            ( 1, 0, -1, 0 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	// wall at back left
	dbMakeObjectBox             ( 2, 80, 60, 1 );
	dbPositionObject            ( 2, -60, 30, 100 );
	dbPhyMakeRigidBodyStaticBox ( 2 );
	//dbPhyMakeRigidBodyDynamicBox ( 2 );

	// wall at back right
	dbMakeObjectBox             ( 3, 80, 60, 1 );
	dbPositionObject            ( 3, 60, 30, 100 );
	dbPhyMakeRigidBodyStaticBox ( 3 );

	// wall at left
	dbMakeObjectBox             ( 4, 1, 60, 200 );
	dbPositionObject            ( 4, -100, 30, 0 );
	dbPhyMakeRigidBodyStaticBox ( 4 );

	// wall at right
	dbMakeObjectBox             ( 5, 1, 60, 200 );
	dbPositionObject            ( 5, 100, 30, 0 );
	dbPhyMakeRigidBodyStaticBox ( 5 );

	// wall at front
	dbMakeObjectBox             ( 6, 200, 60, 1 );
	dbPositionObject            ( 6, 0, 30, -100 );
	dbPhyMakeRigidBodyStaticBox ( 6 );

	// door
	dbMakeObjectBox              ( 7, 39.8, 60.0f, 1.0f );
	dbPositionObject             ( 7, 0, 30, 100 );
	dbColorObject                ( 7, dbRgb ( 255, 0, 0 ) );
	dbPhyMakeRigidBodyDynamicBox ( 7 );
	
	dbPhyMakeRevoluteJoint ( 1, 2, 7, 0, 1, 0, -20, 30, 100 );

	dbMakeObjectCube ( 11, 15 );
	dbPositionObject ( 11, 0, 15, 0 );
	dbPhyMakeRigidBodyDynamicBox ( 11, 11 );

	
	dbPositionCamera ( -75, 140, -2 );
	dbRotateCamera   (  47,  42,  0 );

	for ( int i = 1; i < 20; i++ )
	{
		if ( dbObjectExist ( i ) )
		{
			dbColorObject				( i, dbRgb ( dbRnd ( 250 ), dbRnd ( 250 ), dbRnd ( 250 ) ) );
			dbSetObjectSpecular			( i, dbRgb ( dbRnd ( 250 ), dbRnd ( 250 ), dbRnd ( 250 ) ) );
			dbSetObjectSpecularPower	( i, 255 );
			dbSetObjectAmbient			( i, 0 );
		}

	}

	dbPhySetJointBreakLimits ( 1, 0, 1 );

	while ( !dbEscapeKey ( ) )
	{
		if ( dbSpaceKey ( ) )
		{
			//dbPhySetRigidBodyLinearVelocity ( 11, 0.0, 0.0, 150.5 );
			dbPhySetRigidBodyLinearVelocity ( 11, 0.0, 0.0, 150.0 );
		}

		showFPS   ( );
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