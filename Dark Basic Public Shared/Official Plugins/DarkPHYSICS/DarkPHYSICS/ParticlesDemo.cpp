
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
	dbPhyMakeRigidBodyStaticBox ( 1 );
	dbColorObject					( 1, dbRgb ( 10, 10, 10 ) );
	
	//dbLoadImage ( "media\\stripe5.png", 1 );
	//dbMakeObjectSphere ( 2, 400, 48, 48 );

	//dbTextureObject			( 2, 1 );
	//dbScaleObjectTexture	( 2, 6, 6 );
	//dbSetObjectCull			( 2, 0 );
	//dbSetAmbientLight		( 80 );
	//dbGhostObjectOn			( 2, 0 );
	//dbRotateObject			( 2, 0, 0, 90 );


	//dbMakeObjectBox ( 2, 40, 1, 40 );
	//dbMakeObjectSphere ( 2, 20 );
	//dbSetObjectLight ( 2, 0 );
	//dbPositionObject ( 2, 0, 40, 0 );
	//dbPhyMakeRigidBodyStaticBox ( 2 );
	//dbHideObject ( 2 );

	//dbMakeObjectCube ( 2, 10 );
	//dbPositionObject ( 2, 0, 5, 0 );
	//dbPhyMakeRigidBodyStaticBox ( 2 );
	
	//dbMakeObjectBox ( 2, 40, 1, 40 );
	//dbMakeObjectSphere ( 2, 40 );
	//dbPositionObject ( 2, 0, 40, 0 );
	//dbPhyMakeRigidBodyStaticBox ( 2 );
	//dbPhyMakeRigidBodyDynamicBox ( 2 );

	//dbMakeObjectSphere ( 1, 40 );
	//dbSetObjectWireframe ( 1, 1 );
	//dbSetObjectCull ( 1, 0 );
	//dbPhyMakeRigidBodyStaticSphere ( 1 );


	//dbPhySetGravity ( 0, -1, 0 );

	//dbPhyMakeParticleEmitter ( 4, 20, 5, 0, 5, 0, 0, 1 );
	//dbTextureObject ( 4, 1 );
	
	/*
	for ( int i = 256; i < 512; i++ )
	{
		dbMakeObjectSphere ( i, 2.0f );
	}
	*/

	
	int y = 40;
	int x = 0;
	int z = 0;

	//for ( int a = 100; a < 500; a++ )
	for ( int a = 1; a < 3; a++ )
	{
		//dbMakeObjectSphere				( a, 20, 20, 20 );
		//dbPositionObject				( a, x, y, z );
		//dbPhyMakeRigidBodyDynamicSphere ( a );
		//dbColorObject					( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		//dbSetObjectSpecular				( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		//dbSetObjectSpecularPower		( a, 255 );
		//dbSetObjectAmbient				( a, 0 );
		
		//x = -25 + dbRnd ( 50 );
		//y = y + 15;
		//z = -25 + dbRnd ( 50 );
	}
	
	
	//dbLoadImage ( "ball.bmp", 2 );
	//dbLoadImage ( "fire.png", 2 );
	//dbLoadImage ( "ptcl_64_blue.bmp", 2 );
	//dbLoadImage ( "smoke2.bmp", 2 );

	
	/*
	//dbMakeParticles ( 3 );
	//dbPhyMakeParticleEmitter ( 3, 0, 5, 0, 5, 0, 1, 0 );
	//dbPhyMakeParticleEmitter ( 3, 512, 0.01f, 0, 10, 0, 1, 0, 1, 0 );
	dbPhyMakeParticleEmitter ( 3, 512, 0.01f, 0, 10, 0, 0.1, 0, 10, 0 );
	dbPhySetEmitterLifeSpan ( 3, 0.005f );
	//dbPhySetEmitterFade ( 3, 0 );
	//dbPhySetEmitterGravity ( 3, 1 );

	dbPhySetEmitterSize ( 3, 5.0f );
	dbPhySetEmitterMaxSize ( 3, 20.0f );

	dbTextureObject ( 3, 2 );

	//dbGhostObjectOn ( 3 );
	//dbSetObjectLight ( 3, 0 );
	//dbSetAlphaMappingOn ( 3, 10 );

	//dbPhyAddEmitterColorKey	( 3, 0.0, 255, 0, 0 );
	//dbPhyAddEmitterColorKey	( 3, 0.4,   0, 255,   0 );
	//dbPhyAddEmitterColorKey	( 3, 0.8,   0,   0, 255 );

	dbPhySetEmitterStartColor ( 3, 255, 255, 255 );
	dbPhySetEmitterEndColor ( 3, 0, 0, 0 );
	//dbPhySetEmitterEndColor ( 3, 50, 0, 0);



	sObject* pObject = dbGetObject ( 3 );

	dbGhostObjectOn ( 3, 0 );
	//dbGhostObjectOn ( 3 );
	*/

dbLoadImage ( "fire.png", 1 );
dbPhyMakeFireEmitter ( 1, 512, 0.0f, 5.0f, 0.0f );
dbTextureObject ( 1, 2 );

/*
	//dbLoadImage ( "fire.png", 2 );
	//dbLoadImage ( "snow.bmp", 2 );
	//dbLoadImage ( "mike_smoke.tga", 2 );

	//dbPhyMakeFireEmitter ( 3, 512, 0.0f, 5.0f, 0.0f );
	//dbPhyMakeSmokeEmitter ( 3, 512, 0.0f, 5.0f, 0.0f );
	//dbPhyMakeSnowEmitter ( 3, 512, 0.0f, 30.0f, 0.0f );

	int iID = 3;

	//dbPhyMakeParticleEmitter ( int iID, int iMaximum, float fEmitRate, float fX, float fY, float fZ, float fSpeed, float fXDir, float fYDir, float fZDir )

	dbPhyMakeParticleEmitter			( iID, 512, 0.001f, 0.0, 5.0, 5.0, 0.01, 0, 1, 0 );
	dbPhySetEmitterLifeSpan				( iID, 0.005f );
	dbPhySetEmitterSize					( iID, 5.0f );
	dbPhySetEmitterMaxSize				( iID, 10.0f );
	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	//dbGhostObjectOn						( iID, 0 );
	//dbPhySetEmitterLinearVelocityRange	( iID, -8.0f, 5.0f, -4.0f, 16.0f, 10.0f, 4.0f );

	//void dbPhySetEmitterLinearVelocityRange ( int iID, float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ )

	dbPhySetEmitterLinearVelocityRange	( iID, -4.0f, 1.0f, -5.0f, 8.0f, 4.0f, 8.0f );
	dbPhySetEmitterGrowthRate			( iID, 5.8f );
	*/

	/*
	dbPhyMakeParticleEmitter			( iID, 512, 0.01f, 0.0, 5.0, 0.0, 0.01, 0, 5, 0 );
	dbPhySetEmitterLifeSpan				( iID, 0.008f );
	dbPhySetEmitterSize					( iID, 1.0f );
	dbPhySetEmitterMaxSize				( iID, 20.0f );
	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	//dbGhostObjectOn						( iID, 0 );
	dbPhySetEmitterLinearVelocityRange	( iID, -8.0f, 4.0f, -4.0f, 16.0f, 12.0f, 4.0f );
	dbPhySetEmitterGravity				( iID, 1 );
	dbPhySetEmitterGrowthRate			( iID, 4.0f );
	*/

//	dbTextureObject ( 3, 2 );




	//pObject->pFrame->pMesh->pTextures [ 0 ].dwBlendMode = D3DTOP_MODULATE;
	//pObject->pFrame->pMesh->pTextures [ 0 ].dwBlendArg1 = D3DTA_DIFFUSE;
	//pObject->pFrame->pMesh->pTextures [ 0 ].dwBlendArg2 = D3DTA_TEXTURE;

	while ( !dbEscapeKey ( ) )
	{
		if ( dbRnd ( 500 ) == 250 )
		{
			//dbPhySetEmitterStartColor ( 3, dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 )  );
			//dbPhySetEmitterEndColor ( 3, dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) );
		}


		//dbPhySetEmitterLinearVelocity ( 3, -8 + dbRnd ( 16 ), 5 + dbRnd ( 15 ), -4 + dbRnd ( 4 ) );
		//dbPhySetEmitterLinearVelocity ( 3, -8 + dbRnd ( 16 ), 5 + dbRnd ( 10 ), -4 + dbRnd ( 4 ) );

		//dbPhySetEmitterLinearVelocity ( 3, -2 + dbRnd ( 4 ), 5, -2 + dbRnd ( 4 ) );
		//dbPhySetEmitterLinearVelocity ( 3, -5 + dbRnd ( 10 ), 0, -2 + dbRnd ( 1 ) );

		//dbPhySetEmitterLinearVelocity ( 3, -2 + dbRnd ( 4 ), 10 + dbRnd ( 10 ), -2 + dbRnd ( 4 ) );
		//dbPhySetEmitterLinearVelocity ( 3, -1 + dbRnd ( 1 ), 10 + dbRnd ( 10 ), -1 + dbRnd ( 1 ) );

		showFPS   ( );
		userInput ( );

		dbPhyUpdate ( );
		dbSync      ( );
		dbUpdateParticles ( );
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