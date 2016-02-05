
#include "DarkSDK.h"
#include "physics.h"

float g_fSpeed        = 10.1f;
float g_fTurn         = 0.03f;
float g_fOldCamAngleX = 0.0f;
float g_fOldCamAngleY = 0.0f;
float g_fCameraAngleX = 0.0f;
float g_fCameraAngleY = 0.0f;

void userInput ( void );
void showFPS   ( void );

class game
{
	public:
		int score;
};

game winkle;

class editor
{
	public:
		const game g;

	void test ( void )
	{
		winkle.score = 1;
	};
};

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	
	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbPositionCamera ( -20, 20, -25 );
	dbPointCamera ( 0, 10, 0 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbLoadImage ( "road2.png", 1 );
	

	
	dbMakeObjectBox ( 1, 1000, 1, 1000 );
	dbPositionObject ( 1, 0, -20, 0 );
	dbTextureObject ( 1, 1 );
	dbPhyMakeRigidBodyStaticBox ( 1 );
	
	
	/*
	dbLoadObject ( "corsa//corsa.x", 2 );
	dbSetObjectTransparency ( 2 ,  2 );
	dbRotateObject ( 2 , 0, 90, 0 );
	//dbOffsetLimb ( 2 , 0 , 0 , 0 , -1 );
	dbFixObjectPivot ( 2 );
	dbPositionObject ( 2, 0, 0, 0 );
	dbPhyTestVehicle ( 2 );

	
	dbMakeObjectBox ( 3, 50, 1, 50 );
	dbPositionObject ( 3, 100, -11.5, 0 );
	dbTextureObject ( 3, 3 );
	dbPhyMakeRigidBodyStaticBox ( 3 );
	*/


	
	dbLoadObject ( "humvee.x", 2 );
	dbRotateObject ( 2, 0, -90, 0 );
	dbFixObjectPivot ( 2 );
	//dbRotateObject ( 2 , 0, 120, 0 );
	

	//dbPositionObject ( 2, 0, 100, 0 );
	
	sObject* pObject = dbGetObject ( 2 );


	//dbMakeObjectBox			( 10, dbObjectSizeX ( 2 ), dbObjectSizeY ( 2 ) - 20, dbObjectSizeZ ( 2 ) );
	//dbMakeObjectBox			( 10, 88, 78 / 2, 193);
	dbMakeObjectBox			( 10, 88, 78, 193);
	dbRotateObject ( 10, 0, -90, 0 );
	dbSetObjectWireframe	( 10, 1 );

	float fWinkle = dbObjectSizeY ( 2 ) / 2 + 20;

	//dbOffsetLimb			( 10, 0, 0, dbObjectSizeY ( 2 ) / 2 + 20, 0 );
	dbOffsetLimb			( 10, 0, 0, dbObjectSizeY ( 2 ) / 2, 0 );

	dbMakeObjectSphere ( 11, 30.0f );
	dbMakeObjectSphere ( 12, 30.0f );
	dbMakeObjectSphere ( 13, 30.0f );
	dbMakeObjectSphere ( 14, 30.0f );

	
	// front left
	dbPositionObject ( 11,  dbLimbPositionX ( 2, 3 ), dbLimbPositionY ( 2, 3 ),  dbLimbPositionZ ( 2, 3 ) );

	// rear left
	dbPositionObject ( 12,  dbLimbPositionX ( 2, 5 ), dbLimbPositionY ( 2, 5 ),  dbLimbPositionZ ( 2, 5 ) );

	// rear right
	dbPositionObject ( 13,  dbLimbPositionX ( 2, 7 ), dbLimbPositionY ( 2, 7 ),  dbLimbPositionZ ( 2, 7 ) );

	// front right
	dbPositionObject ( 14,  dbLimbPositionX ( 2, 9 ), dbLimbPositionY ( 2, 9 ),  dbLimbPositionZ ( 2, 9 ) );

	
	//
	
	dbPhyCreateVehicle		( 2 );

	float a = dbObjectSizeY ( 2 );
	float b = dbObjectSizeY ( 2 ) / 2 + 20;

	float x = dbObjectSizeX ( 2 );
	float y = dbObjectSizeY ( 2 );
	float z = dbObjectSizeZ ( 2 );

	//dbPhyAddVehicleBody		( 2, dbObjectSizeX ( 2 ), dbObjectSizeY ( 2 ) / 2 - 20, dbObjectSizeZ ( 2 ), 0.0f, dbObjectSizeY ( 2 ) / 2 + 20, 0.0f );
	//dbPhyAddVehicleBody		( 2, dbObjectSizeX ( 2 ), dbObjectSizeY ( 2 ) / 2 - 20, dbObjectSizeZ ( 2 ), 0.0f, 0.0, 0.0f );
	//dbPhyAddVehicleBody		( 2, 88, 78 / 2, 193, 0.0f, dbObjectSizeY ( 2 ) / 2 + 20, 0.0f );
	//dbPhyAddVehicleBody		( 2, 88, 78 / 2, 193, 0.0f, 0, 0.0f );
	
	//dbPhyAddVehicleBody		( 2, 88, 78 / 2, 193, 0.0f, dbObjectSizeY ( 2 ) / 2, 0.0f );
	//dbPhyAddVehicleBody		( 2, 88, 78 / 2, 193, 0.0f, dbObjectSizeY ( 2 ) / 2 + 20, 0.0f );
	dbPhyAddVehicleBody		( 2, 88, 78 / 2, 193, 0.0f, dbObjectSizeY ( 2 ) / 2, 0.0f );


	//dbPhyAddVehicleBody		( 2, 4, 4, 4, 0.0f, 0.0f, 0.0f );

	//float fRadius = 15.0f;
	//float fHeight = 15.0f;

	float fRadius = 30.0f;
	float fHeight = 30.0f;

	dbPhyAddVehicleWheel	( 2, 5, dbLimbPositionX ( 2, 5 ), dbLimbPositionY ( 2, 5 ) - 10,  dbLimbPositionZ ( 2, 5 ), fRadius, fHeight, 0, 1 );
	dbPhyAddVehicleWheel	( 2, 7, dbLimbPositionX ( 2, 7 ), dbLimbPositionY ( 2, 7 ) - 10,  dbLimbPositionZ ( 2, 7 ), fRadius, fHeight, 0, 1 );
	dbPhyAddVehicleWheel	( 2, 3, dbLimbPositionX ( 2, 3 ), dbLimbPositionY ( 2, 3 ) - 10,  dbLimbPositionZ ( 2, 3 ), fRadius, fHeight, 1, 1 );
	dbPhyAddVehicleWheel	( 2, 9, dbLimbPositionX ( 2, 9 ), dbLimbPositionY ( 2, 9 ) - 10,  dbLimbPositionZ ( 2, 9 ), fRadius, fHeight, 1, 1 );
	
	
	dbPhySetVehicleDynamicFriction ( 2, 1.0f );
	dbPhySetVehicleStaticFriction ( 2, 2.0f );
	dbPhySetVehicleMaxMotor ( 2, 500.0f );
	dbPhySetVehicleSteeringDelta ( 2, 0.25f );
	dbPhySetVehicleStaticFrictionV ( 2, 0.01f );
	dbPhySetVehicleDynamicFrictionV ( 2, 0.01f );
	dbPhySetVehicleMaxSteeringAngle ( 2, NxMath::degToRad ( 60.0f ) );

	dbPhySetVehicleMass ( 2, 500.0f );

	
	dbPhySetVehicleAuto		( 2, 1 );
	dbPhyBuildVehicle		( 2 );
	//
	
	

	dbPositionCamera ( 0, 10, 0 );

	
	dbSyncRate ( 60 );

	while ( !dbEscapeKey ( ) )
	{
		showFPS   ( );
		//userInput ( );

		//dbPhySetVehicleMotorForce ( 2, 250 );
		//dbPhySetVehicleSteeringDelta ( 2, NxMath::degToRad ( 45.0f ) );


		

		float x = dbObjectPositionX ( 2 );
		float y = dbObjectPositionY ( 2 );
		float z = dbObjectPositionZ ( 2 );

		dbSetCameraToFollow ( x, y, z, 45.0f, 160.0f, 115.0f, 5.0f, 0 );

		
		//float x = dbObjectPositionX ( 2 );
		//float y = dbObjectPositionY ( 2 );
		//float z = dbObjectPositionZ ( 2 );

		for ( int i = 10; i < 11; i++ )
		{
			dbPositionObject ( i, x, y, z );
			//dbRotateObject ( i, dbObjectAngleX ( 2 ), dbObjectAngleY ( 2 ), dbObjectAngleZ ( 2 ) );
		}

		dbPositionObject ( 11,  dbLimbPositionX ( 2, 3 ), dbLimbPositionY ( 2, 3 ),  dbLimbPositionZ ( 2, 3 ) );

	// rear left
	dbPositionObject ( 12,  dbLimbPositionX ( 2, 5 ), dbLimbPositionY ( 2, 5 ),  dbLimbPositionZ ( 2, 5 ) );

	// rear right
	dbPositionObject ( 13,  dbLimbPositionX ( 2, 7 ), dbLimbPositionY ( 2, 7 ),  dbLimbPositionZ ( 2, 7 ) );

	// front right
	dbPositionObject ( 14,  dbLimbPositionX ( 2, 9 ), dbLimbPositionY ( 2, 9 ),  dbLimbPositionZ ( 2, 9 ) );
		
		dbPhyUpdate ( );
		dbPhyUpdateVehicle ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
}


void showFPS ( void )
{
	dbSetCursor ( 0, 0 );

	char szFPS [ 256 ] = "";
	strcpy ( szFPS, "fps = " );
	strcat ( szFPS, dbStr ( dbScreenFPS ( ) ) );
	dbText ( dbScreenWidth ( ) - 20 - dbTextWidth ( szFPS ), dbScreenHeight ( ) - 40, szFPS );



	strcpy ( szFPS, dbStr ( dbCameraPositionX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraPositionZ ( ) ) );
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

	//float fHeight = dbGetTerrainGroundHeight ( 1, dbCameraPositionX ( ), dbCameraPositionZ ( ) );
	//dbPositionCamera ( dbCameraPositionX ( ), fHeight + 3.0f, dbCameraPositionZ ( ) );
}