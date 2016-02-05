
#include "DarkSDK.h"
//#include "physics.h"

// physxloader.lib nxcooking.lib nxcharacter.lib

// global variables - used for the camera
float g_fSpeed        = 5.0f;		// speed at which camera moves
float g_fTurn         = 0.03f;		// turn speed for mouse look
float g_fOldCamAngleX = 0.0f;		// to store original x angle
float g_fOldCamAngleY = 0.0f;		// to store original y angle
float g_fCameraAngleX = 0.0f;		// to sotre new x angle
float g_fCameraAngleY = 0.0f;		// to store new y angle

// forward declaration for functions
void userInput ( void );			// handles user input
void showFPS   ( void );			// show frame rate

//void dbPhyCharacterTest ( void );
//void dbPhyCharacterTest ( float fX, float fY, float fZ );
//void dbPhyBent ( float x, float y, float z );

void DarkSDK ( void )
{
	//dbSyncOn   ( );
	//dbSyncRate ( 0 );
	dbSetDisplayMode(800,600,32);
	//dbSetWindowOff();
	dbDisableSystemKeys (  );
	dbHideMouse();
	dbBackdropOn();
	dbColorBackdrop(dbRgb(0,0,0));
	dbDisableEscapeKey (  );

	/*
	//Pre-Loading animations

	dbLoadAnimation("anim1.mpg",1);	
	dbLoadAnimation("anim2.mpg",2);	
	dbLoadAnimation("anim3.mpg",3);	
	dbLoadAnimation("anim4.mpg",4);	

	//Playing animations first time (That works)

	for(int i=1;i<5;i++)
	{
		dbLoopAnimation(i);
		dbWaitKey();
		dbStopAnimation(i);
	}

	//Playing them a second time... 

	for(int i=1;i<5;i++)
	{
		dbLoopAnimation(i);
		dbWaitKey();
		dbStopAnimation(i);
	}
	*/

	/*
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	dbPositionCamera ( 0, 200, -1500 );

	dbLoadObject ( "happyland.x", 1 );
	dbPhyMakeRigidBodyStaticMesh ( 1 );

	dbLoadObject ( "happyland_lmap.x", 2 );
	dbGhostObjectOn ( 2 );

	for ( int i = 1; i < 50; i++ )
	{
		dbMakeObjectSphere ( 100 + i, 50 );
		dbColorObject ( 100 + i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbPositionObject ( 100 + i, -300 + dbRnd ( 600 ), 500 + dbRnd ( 300 ), 100 - dbRnd ( 1000 ) );
		dbPhyMakeRigidBodyDynamicSphere ( 100 + i );
	}
	
	while ( !dbEscapeKey ( ) )
	{
		showFPS   ( );
		userInput ( );

		dbPhyUpdate ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
	*/
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