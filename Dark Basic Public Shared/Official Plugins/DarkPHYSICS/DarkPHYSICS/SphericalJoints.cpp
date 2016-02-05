
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

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );


	dbPositionCamera ( 0, 20, -70 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbMakeObjectBox ( 1, 50, 1, 50 );
	dbColorObject ( 1, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	float fX = -35.0f;
	float fY =   5.0f;

	for ( int i = 2; i < 10; i++ )
	{
		dbMakeObjectCube			( i, 5.0f );
		dbPositionObject			( i, 0.0f, fY, 0.0f );
		dbColorObject				( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular			( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower	( i, 255 );
		dbSetObjectAmbient			( i, 0 );

		dbPhyMakeRigidBodyDynamicBox ( i );

		fY = fY + 5.0f;
	}

	//fX = -15;
	fX = -25;
	fY = 80.0f;

	int iJointID = 1;

	for ( int i = 10; i < 25; i++ )
	{
		dbMakeObjectSphere			( i, 5.0f );
		dbPositionObject			( i, fX, fY, -2.0f );
		dbColorObject				( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecular			( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
		dbSetObjectSpecularPower	( i, 255 );
		dbSetObjectAmbient			( i, 0 );

		dbPhyMakeRigidBodyDynamicSphere ( i );
		
		//fY = fY + 2.5f;
		fX = fX + 5.0f;
	}

	for ( int i = 10; i < 24; i++ )
	{
		dbPhyMakeSphereJoint ( iJointID++, i, i + 1, dbObjectPositionX ( i ), dbObjectPositionY ( i ), dbObjectPositionZ ( i ) );
	}
	
	

	while ( !dbEscapeKey ( ) )
	{
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
}