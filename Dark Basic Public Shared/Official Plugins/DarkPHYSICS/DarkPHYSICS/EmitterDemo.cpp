
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
// creating emitters
void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 0 );
	dbColorBackdrop ( 0 );

	dbPhyMakeParticleEmitter ( 1, 512, 0.01f, 0.0, 0.0, 0.0, 2.0, 0, 1, 0 );

	dbLoadImage ( "fire.png", 1 );
	dbGhostObjectOn						( 1, 0 );
	dbTextureObject ( 1, 1 );
	

	dbPositionCamera ( 0, 0, -10 );
	
	while ( !dbEscapeKey ( ) )
	{
		dbPhyUpdate ( );
		//showFPS ( );
		//userInput ( );
		
		dbSync      ( );
	}

	dbPhyEnd ( );
}
*/

/*
// setting emitter properties
void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 0 );
	dbColorBackdrop ( 0 );

	//dbPhyMakeParticleEmitter ( 1, 512, 0.01f, 0.0, 0.0, 0.0, 2.0, 0, 1, 0 );
	//dbPhyMakeParticleEmitter ( 1, 512, 0.01f, 0.0, 0.0, 0.0, 20.0005, 0, 1, 0 );
	dbPhyMakeParticleEmitter ( 1, 512, 0.01f, 0.0, 0.0, 0.0, 1.0, 0, 1.5, 0 );

	
	dbLoadImage ( "fire.png", 1 );
	dbGhostObjectOn ( 1, 0 );
	dbTextureObject ( 1, 1 );

	dbPhySetEmitterLifeSpan ( 1, 0.001f );
	dbPhySetEmitterSize ( 1, 1.0f );
	dbPhySetEmitterMaxSize ( 1, 10.0f );
	dbPhySetEmitterGrowthRate ( 1, 2.0f );
	dbPhySetEmitterStartColor ( 1, 0, 255, 255 );
	dbPhySetEmitterEndColor	( 1, 255, 255, 255 );
	dbPhySetEmitterLinearVelocityRange ( 1, -1, -2, 0, 2, 3, 1 );
	dbPhySetEmitterRange ( 1, -20, 20, -20, 40, 0, 40 );


	dbPositionCamera ( 0, 30, -40 );

	while ( !dbEscapeKey ( ) )
	{
		dbPhyUpdate ( );
		//showFPS ( );
		//userInput ( );
		
		dbSync      ( );
	}

	dbPhyEnd ( );
}
*/


// prebuilt emitters
void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 0 );
	dbColorBackdrop ( 0 );

	//dbPhyMakeFireEmitter ( 1, 512, 0, 5, 0 );
	//dbPhyMakeSmokeEmitter ( 1, 512, 0, 0, 0 );
	dbPhyMakeSnowEmitter ( 1, 512, 0, 0, 0 );

	//dbPhyMakeParticleEmitter ( 1, 512, 0.01f, 0.0, 0.0, 0.0, 2.0, 0, 1, 0 );

	//dbLoadImage ( "fire.png", 1 );
	dbLoadImage ( "smoke.jpg", 1 );
	dbGhostObjectOn ( 1, 0 );
	dbTextureObject ( 1, 1 );
	
	//dbPhySetEmitterEndColor	( 1, 255, 0, 0 );

	dbPositionCamera ( 0, 40, -80 );
	
	while ( !dbEscapeKey ( ) )
	{
		dbPhyUpdate ( );
		//showFPS ( );
		//userInput ( );
		
		//dbPhyUpdateParticles ( );
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