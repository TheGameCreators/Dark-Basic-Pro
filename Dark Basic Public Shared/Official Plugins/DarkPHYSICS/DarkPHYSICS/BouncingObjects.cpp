
#include "DarkSDK.h"
#include "physics.h"

void DarkSDK ( void )
{
	dbPhyStart ( );
	
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
	dbPositionObject ( 10, 35, -10, 0 );
	dbRotateObject ( 10, 0, 0, 60 );
	dbGhostObjectOn ( 10, 0 );
	dbTextureObject ( 10, 3 );
	dbPhyMakeRigidBodyStaticBox ( 10 );

	dbMakeObjectBox ( 11, 50, 1, 50 );
	dbPositionObject ( 11, 0, -40, 0 );
	dbRotateObject ( 11, 0, 0, -20 );
	dbTextureObject ( 11, 3 );
	dbPhyMakeRigidBodyStaticBox ( 11 );

	dbPositionCamera ( -46.8f, 26.8f, -49.0f );
	dbRotateCamera ( 34.7f, 46.4f, 0.0f );
	dbSetAmbientLight ( 30 );

	int y = 20;
	int x = 40;
	int z = 0;

	for ( int a = 100; a < 500; a++ )
	{
		dbMakeObjectSphere				( a, 5, 20, 20 );
		dbPositionObject				( a, ( float ) x, ( float ) y, ( float ) z );
		dbPhyMakeRigidBodyDynamicSphere ( a );
		dbColorObject					( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular				( a, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower		( a, 255 );
		dbSetObjectAmbient				( a, 0 );
		
		y = y + 20;
		z = -10 + dbRnd ( 20 );
	}

	dbSetDir ( ".." );

	float r     =  80;
	float h     = -10;
	float angle = 180;
	int   count =   0;
	float xpos  =   0;
	float zpos  =   0;

	while ( !dbEscapeKey ( ) )
	{
		count++;
		xpos  = 10.0f * dbSin ( ( float ) count );
		angle = angle - 0.4f;
		
		if ( r < 1.0f )
			r = 1.0f;

		xpos = r * dbSin ( angle );
		zpos = r * dbCos ( angle );
		dbPositionCamera ( 0, xpos, h, zpos );
		dbPointCamera ( 0, 0.0f, h, 0.0f );

		dbCenterText ( dbScreenWidth ( ) / 2, 20, "Bouncing Objects" );

		dbRotateObject ( 2, 0.0f, dbObjectAngleY ( 2 ) + 0.3f, 0.0f );

		dbPhyUpdate ( );
		dbSync      ( );
	}
}