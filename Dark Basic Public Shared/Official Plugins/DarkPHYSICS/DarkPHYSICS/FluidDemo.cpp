
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

//void dbPhyMakeParticleEmitter ( int iID, float fX, float fY, float fZ, float fSpeed, float fXDir, float fYDir, float fZDir );
//void dbUpdateParticles ( void );

void dbPhyMakeFluidEmitterTest ( int iID );
void dbPhyUpdateFluidTest ( int iID );

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

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbPositionCamera ( -20, 20, -25 );
	dbPointCamera ( 0, 10, 0 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	
	
	dbMakeObjectBox ( 1, 40, 1, 40 );
	dbPositionObject ( 1, 0, -5, 0 );
	//dbRotateObject ( 1, 0, 0, 20 );
	dbPhyMakeRigidBodyStaticBox ( 1 );
	dbColorObject ( 1, dbRgb ( 10, 10, 10 ) );
	

	
	dbMakeObjectBox ( 2, 20, 1, 10 );
	dbPositionObject ( 2, 10, 5, 0 );
	dbRotateObject ( 2, 0, 0, 20 );
	dbPhyMakeRigidBodyStaticBox ( 2 );
	dbColorObject ( 2, dbRgb ( 10, 10, 10 ) );
	
	
	
	//dbPhyTestFluid ( );

	//dbPhyMakeFluid	( 10 );
	
	//dbPhySetFluidPosition ( 10, 12, 16, 1 );
	//dbPhySetFluidPosition ( 10, 12, 16, -3 );
	//dbPhySetFluidPosition ( 10, 0, 4, 0 );
	//dbPhyBuildFluid	( 10 );

	//dbPhyMakeFluidEmitter ( 10 );

	//dbPhySetFluidAcceleration ( 10, -10, -2, 0 );
	

	//dbPhyMakeFluid	( 10 );
	//dbPhyMakeFluidEmitter ( 10 );
	//dbPhySetFluidPosition ( 10, 4, 8, 0 );
	//dbPhyBuildFluid	( 10 );

	dbPhyMakeFluidEmitter ( 10 );
	//dbPhyMakeFluid ( 10 );
	dbPhySetFluidPosition ( 10, 4, 8, 0 );
	dbPhySetFluidRotation ( 10, -90, 0, 0 );
	dbPhyBuildFluid ( 10 );

	//dbPhyMakeFluidEmitter ( 11 );
	dbPhyMakeFluid ( 11 );
	dbPhySetFluidPosition ( 11, 8, 16, 0 );
	dbPhyBuildFluid ( 11 );
	
	
	//dbPhyMakeFluidEmitterTest ( 10 );
	dbLoadImage ( "fire2.png", 2 );
	dbTextureObject ( 10, 2 );
	dbGhostObjectOn ( 10 );

	dbTextureObject ( 11, 2 );
	dbGhostObjectOn ( 11 );
	
	/////////////

	//dbLoadObject ( "pipe.x", 3 );
	//dbRotateObject ( 3, 0, 0, 20 );
	//dbPhyMakeRigidBodyStaticMesh ( 3 );
	

	
	/*
	dbLoadObject ( "bowl.x", 4 );
	dbPositionObject ( 4, 0, 5, 2 );
	dbPhyMakeRigidBodyStaticMesh ( 4 );
	//dbPhyMakeRigidBodyStaticBox ( 4 );
	dbSetObjectSpecular ( 4, 0 );
	//dbGhostObjectOn ( 4 );
	*/

	while ( !dbEscapeKey ( ) )
	{
		showFPS   ( );
		userInput ( );

		dbPhyUpdate ( );
		dbSync      ( );
		//dbPhyUpdateFluidTest ( 10 );
//		dbPhyUpdateFluid ( );
		//dbPhyUpdateFluid ( );
		//dbUpdateParticles ( );
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

	dbSetCursor ( 0, 0 );
	strcpy ( szFPS, dbStr ( dbStatistic ( 1 ) ) );
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