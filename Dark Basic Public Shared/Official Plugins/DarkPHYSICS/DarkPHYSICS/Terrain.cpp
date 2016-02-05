
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
	dbSetCameraRange ( 0.5f, 30000 );	// set the camera range
	
	dbSetTextFont ( "tahoma" );
	dbSetTextSize ( 32 );

	dbSetDir ( "media" );

	// load images for the terrain
	dbLoadImage ( "texture.jpg", 1 );	// diffuse texture
	dbLoadImage ( "detail.jpg",  2 );	// detail texture
	
	// create the terrain
	dbSetupTerrain        ( );										// set up terrain library
	dbMakeObjectTerrain   ( 1 );									// make a new terrain
	dbSetTerrainHeightMap ( 1, "map.bmp" );						// set the terrain height map
	dbSetTerrainScale     ( 1, 3, 0.6f, 3 );						// set the scale
	dbSetTerrainSplit     ( 1, 16 );								// set the split value
	dbSetTerrainTiling    ( 1, 4 );									// set the detail tiling
	dbSetTerrainLight     ( 1, 1, -0.25f, 0, 1, 1, 0.78f, 0.5f );	// set the light
	dbSetTerrainTexture   ( 1, 1, 2 );								// set the textures
	dbBuildTerrain        ( 1 );									// build the terrain

	//dbSetObjectLight   ( 1, 1 );

	// create our skybox
	dbLoadObject       ( "skybox2.x", 200 );		// load the skybox model
	dbSetObjectLight   ( 200, 0 );						// turn lighting off
	dbSetObjectTexture ( 200, 3, 1 );					// set texture properties
	dbPositionObject   ( 200, 1000, 2000, 4000 );		// position the skybox
	dbScaleObject      ( 200, 30000, 30000, 30000 );	// and finally scale

	dbPhyMakeRigidBodyStaticMesh ( 1 );
	//dbPhyMakeRigidBodyStaticTerrain ( 1 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );
	
	
	//int x = 385;
	int x = 395;
	int y = 100;
	int z = 100;

	for ( int a = 300; a < 400; a++ )
	{
		dbMakeObjectSphere				( a, 5, 20, 20 );
		dbPositionObject				( a, ( float ) x, ( float ) y, ( float ) z );
		dbPhyMakeRigidBodyDynamicSphere ( a );
		dbColorObject					( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular				( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower		( a, 255 );
		dbSetObjectAmbient				( a, 0 );
		
		y = y + 20;
		//z = -10 + dbRnd ( 20 );
	}

	//dbPositionCamera ( 385, 23, 100 );

	// set starting camera position
	dbPositionCamera ( 385, 23, 100 );

	dbSetDir ( ".." );

	while ( !dbEscapeKey ( ) )
	{
		showFPS   ( );
		userInput ( );

		// update the terrain
		dbUpdateTerrain ( );

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