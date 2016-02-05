
#include "DarkSDK.h"
#include "physics.h"

void DarkSDK ( void )
{
	dbPhyEngineStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	
	dbSetTextFont ( "tahoma" );
	dbSetTextSize ( 32 );

	dbSetDir ( "media" );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbLoadImage ( "stripe5.png", 1 );
	dbMakeObjectSphere ( 2, 400, 48, 48 );

	dbTextureObject			( 2, 1 );
	dbScaleObjectTexture	( 2, 6, 6 );
	dbSetObjectCull			( 2, 0 );
	dbSetAmbientLight		( 80 );
	dbGhostObjectOn			( 2, 0 );
	dbRotateObject			( 2, 0, 0, 90 );
	dbMoveCamera			( -60 );

	dbLoadImage ( "stripe6.png", 3 );

	dbMakeObjectBox ( 10, 50, 1, 50 );
	dbPositionObject ( 10, 0, -10, 0 );
	dbTextureObject ( 10, 3 );
	dbPhyRigidBodyStaticBoxMake ( 10 );


	dbPositionCamera ( -46.8, 26.8, -49 );
	dbRotateCamera ( 34.7, 46.4, 0 );
	dbSetAmbientLight ( 30 );

	int y = 20;
	int x = 0;
	int z = 0;

	for ( int a = 100; a < 500; a++ )
	{
		dbMakeObjectSphere				( a, 5, 20, 20 );
		dbPositionObject				( a, x, y, z );
		dbPhyRigidBodyDynamicSphereMake ( a );
		dbColorObject					( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular				( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower		( a, 255 );
		dbSetObjectAmbient				( a, 0 );
		
		x = -25 + dbRnd ( 50 );
		y = y + 15;
		z = -25 + dbRnd ( 50 );
	}

	dbSetDir ( ".." );

	while ( !dbEscapeKey ( ) )
	{
		dbCenterText ( dbScreenWidth ( ) / 2, 20, "Falling Objects" );

		dbRotateObject ( 2, 0, dbObjectAngleY ( 2 ) + 0.3, 0 );

		dbPhyEngineUpdate ( );
		dbSync            ( );
	}

	dbPhyEngineEnd ( );
}