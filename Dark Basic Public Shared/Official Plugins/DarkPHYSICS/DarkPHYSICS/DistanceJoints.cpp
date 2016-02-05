
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
	//dbSetCameraRange ( 0.5f, 30000 );
	
	//dbSetTextFont ( "tahoma" );
	//dbSetTextSize ( 32 );

	dbPositionCamera ( 0, 20, -70 );

	//dbPositionCamera ( -20, 20, -25 );
	//dbPointCamera ( 0, 10, 0 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbMakeObjectBox ( 1, 50, 1, 50 );
	dbPositionObject ( 1, 0, 0, 0 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	dbMakeObjectCube ( 4, 5 );
	dbPositionObject ( 4, -12, 5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 4 );

	dbMakeObjectCube			 ( 2, 5 );
	//dbPositionObject			 ( 2, 0, 50, 0 );
	dbPositionObject			 ( 2, -10, 50, 0 );
	//dbPhyMakeRigidBodyStaticBox ( 2 );
	dbPhyMakeRigidBodyDynamicBox ( 2 );

	dbMakeObjectCube			 ( 3, 5 );
	//dbPositionObject			 ( 3, 0, 10, 0 );
	dbPositionObject			 ( 3, 10, 50, 0 );
	dbPhyMakeRigidBodyDynamicBox ( 3 );

	//dbPhyMakeDistanceJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fMinDistance, float fMaxDistance )
	dbPhyMakeDistanceJoint ( 1, 2, 3, 0, 50, 0, 10, 40 );

	dbRandomize ( dbTimer ( ) );

	for ( int i = 1; i < 7; i++ )
	{
		if ( dbObjectExist ( i ) )
		{
			dbColorObject			 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecular		 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecularPower ( i, 255 );
			dbSetObjectAmbient		 ( i, 0 );
		}
	}

	/*
	dbMakeObjectBox ( 1, 50, 1, 50 );
	dbColorObject ( 1, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	float fX = -35.0f;
	float fY = 5.0f;

	for ( int i = 2; i < 4; i++ )
	{
		dbMakeObjectCube			( i, 10.0f );
		dbPositionObject			( i, 0.0f, fY, 0.0f );
		dbColorObject				( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular			( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower	( i, 255 );
		dbSetObjectAmbient			( i, 0 );

		dbPhyMakeRigidBodyDynamicBox ( i );

		fY = fY + 24.0f;
	}

	int iJointID = 1;

	//for ( int i = 2; i < 3; i++ )
	{
		//dbPhyMakePrismaticJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ )

		//dbPhyMakePrismaticJoint ( 1, 2, 3, dbObjectPositionX ( 2 ), dbObjectPositionY ( 2 ), dbObjectPositionZ ( 2 ), 0, 5, 0 );

		dbPhyMakeCylindricalJoint ( 1, 2, 3, dbObjectPositionX ( 2 ), dbObjectPositionY ( 2 ), dbObjectPositionZ ( 2 ), 0, 5, 0 );

		//dbPhyMakePrismaticJoint ( iJointID++, i, i + 1, dbObjectPositionX ( i ), 15, dbObjectPositionZ ( i ), 0, 1, 0 );
	}
	
	DWORD dwFrame = 0;
	*/

	while ( !dbEscapeKey ( ) )
	{
		//dbPhySetRigidBodyLinearVelocity ( 2, 0, 10, 0 );

		if ( dbSpaceKey ( ) )
		{
			dbPhySetRigidBodyLinearVelocity ( 2, -10, 0, 0 );
		}

		showFPS   ( );
		userInput ( );

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