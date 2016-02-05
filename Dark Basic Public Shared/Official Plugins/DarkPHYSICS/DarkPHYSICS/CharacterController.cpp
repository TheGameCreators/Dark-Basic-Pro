
#include "DarkSDK.h"
#include "physics.h"

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
	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );
	
	//dbSetTextFont ( "tahoma" );
	//dbSetTextSize ( 32 );

	dbMakeObjectBox ( 1, 50, 1, 50 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	

	float fX = 10.0f;
	float fY = 1.0f;

	for ( int i = 10; i < 20; i++ )
	{
		dbMakeObjectBox ( i, 3, 2, 10 );
		dbPositionObject ( i, fX, fY, 0 );
	
		fX += 3.0f;
		fY += 1.0f;

		dbPhyMakeRigidBodyStaticBox ( i );
	}

	fY = 1.0f;

	for ( int i = 20; i < 30; i++ )
	{
		dbMakeObjectBox ( i, 2, 2, 2 );
		//dbPositionObject ( i, -20, fY, 0 );
		dbPositionObject ( i, -10, fY, 0 );
	
		fY += 2.0f;

		dbPhyMakeRigidBodyDynamicBox ( i );
	}

	dbMakeObjectCube ( 2, 5 );
	//dbPositionObject ( 2, 0, 10, 0 );
	
	// dbPhyMakeBoxCharacterController ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, int iUp, float fSlopeLimit )
	//dbPhyMakeBoxCharacterController ( 2, 0, 10, 0, 2.5f, 2.5f, 2.5f, 0, 45.0f );

	dbPhyMakeBoxCharacterController ( 2, 0, 10, 0, 2.5f, 2.5f, 2.5f, 1, 1.5f, 45.0f );
	//dbPhyMakeCapsuleCharacterController ( 2, 0, 10, 0, 2.5, 2.5, 1, 45.0f );

	//dbMakeObjectBox ( 3, 2, 2, 5 );
	//dbPositionObject ( 3, 10, 2, 0 );
	//dbPhyMakeRigidBodyStaticBox ( 3 );

	for ( int i = 1; i < 30; i++ )
	{
		if ( dbObjectExist ( i ) )
		{
			dbColorObject			 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecular		 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecularPower ( i, 255 );
			dbSetObjectAmbient		 ( i, 0 );
		}
	}
	
	
	dbPositionCamera ( 0, 20, -70 );

	dbRotateObject ( 2, 0, 90, 0 );

	while ( !dbEscapeKey ( ) )
	{
		if ( dbLeftKey ( ) )
			dbTurnObjectLeft ( 2, 1.0f );

		if ( dbRightKey ( ) )
			dbTurnObjectRight ( 2, 1.0f );

		if ( dbUpKey ( ) )
			dbPhyMoveCharacterController ( 2, 10.0f );
		else
			dbPhyMoveCharacterController ( 2, 0.0f );

		
		//	showFPS   ( );
		//userInput ( );

		//dbPhyUpdateStep1 ( );

		dbPhyUpdate ( );
		dbSync      ( );

		//dbPhyUpdateStep2 ( );
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