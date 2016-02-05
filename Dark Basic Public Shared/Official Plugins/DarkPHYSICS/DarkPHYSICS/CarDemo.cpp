
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
	//dbSetCameraRange ( 0.5f, 30000 );
	
	//dbSetTextFont ( "tahoma" );
	//dbSetTextSize ( 32 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbPositionCamera ( -20, 20, -25 );
	dbPointCamera ( 0, 10, 0 );

	dbMakeLight ( 1 );
	dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbLoadImage ( "road2.png", 1 );
	
	//dbMakeObjectBox ( 1, 200, 1, 200 );
	//dbMakeObjectBox ( 1, 1000, 1, 1000 );
	//dbPositionObject ( 1, 0, 0, 0 );
	
	//dbTextureObject ( 1, 1 );
	//dbPhyMakeRigidBodyStaticBox ( 1 );
	
	

	
	/*
	dbSetupTerrain ( );

	dbLoadImage ( "media\\texture.bmp", 500 );
	dbLoadImage ( "media\\detail.tga", 501 );

	dbMakeObjectTerrain ( 500 );
	dbSetTerrainHeightMap ( 500, "media\\map.bmp" );
	dbSetTerrainScale ( 500, 4, 0.6, 4 );
	dbSetTerrainSplit ( 500, 16 );
	dbSetTerrainTiling ( 500, 4 );
	dbSetTerrainLight ( 500, 1, -0.25, 0, 1, 1, 0.78, 0.5 );
	dbSetTerrainTexture ( 500, 500, 501 );
	dbBuildTerrain ( 500 );
	dbPositionObject ( 500, -200, -50, -150 );
	//dbPhyMakeRigidBodyStaticTerrain ( 500 );
	//dbPhyMakeRigidBodyStaticMesh ( 500 );
	*/
	
	

	/*
	dbLoadObject ( "corsa//corsa.x", 2 );
	dbSetObjectTransparency ( 2 ,  2 );
	dbRotateObject ( 2 , 0, 90, 0 );
	//dbOffsetLimb ( 2 , 0 , 0 , 0 , -1 );
	dbFixObjectPivot ( 2 );
	dbPositionObject ( 2, 0, 0, 0 );

	//initCar ( );
	//createCar ( 2, 0, 12, 0, 0, 0, 0 );

	dbPhyTestVehicle ( 2 );
	*/
	
	
	dbMakeObjectBox ( 1, 1000, 1, 1000 );
	dbPositionObject ( 1, 0, -12, 0 );
	dbTextureObject ( 1, 1 );
	dbPhyMakeRigidBodyStaticBox ( 1 );
	
	
	
	
	/////////
	/*
	dbMakeObjectBox ( 197, 25, 1, 10 );
	dbTextureObject ( 197, 1 );
	dbRotateObject ( 197, 0, 0, 20 );
	dbPositionObject ( 197, -43.0f, -7.6f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 197 );

	dbMakeObjectBox ( 198, 5, 1, 10 );
	dbTextureObject ( 198, 1 );
	dbPositionObject ( 198, -29.0f, -3.4f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 198 );

	dbMakeObjectBox ( 199, 20, 1, 10 );
	dbTextureObject ( 199, 1 );
	dbRotateObject ( 199, 0, 0, 10 );
	dbPositionObject ( 199, -17.1f, -1.7f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 199 );



	dbMakeObjectBox ( 200, 15, 1, 10 );
	dbTextureObject ( 200, 1 );
	dbPositionObject ( 200, 0.0f, 0.0f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 200 );

	dbMakeObjectBox ( 201, 15, 1, 10 );
	dbTextureObject ( 201, 1 );
	dbPositionObject ( 201, 40.2f, 0.0f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 201 );

	dbMakeObjectBox ( 225, 35, 1, 10 );
	dbTextureObject ( 225, 1 );
	dbRotateObject ( 225, 0, 0, -10 );
	dbPositionObject ( 225, 64.8, -3.1f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 225 );

	dbMakeObjectBox ( 226, 35, 1, 10 );
	dbTextureObject ( 226, 1 );
	dbPositionObject ( 226, 110, -10.0f, 0.0f );
	dbPhyMakeRigidBodyStaticBox ( 226 );

	
	//
	float fX, fY, fZ = 0.0f;

	fY = -9.0f;
	fZ = 4.0f;
	int iStart = 230;

	for ( int z = 0; z < 9; z++ )
	{
		for ( int y = 0; y < 8; y++ )
		{
			dbMakeObjectBox ( iStart, 1, 1, 1 );
			dbPositionObject ( iStart, 115, fY, fZ );
			dbTextureObject ( iStart, 1 );
			dbPhyMakeRigidBodyDynamicBox ( iStart++ );

			fY += 1.0f;
		}

		fY = -9.0f;
		fZ -= 1.0f;
		//iStart++;
	}

	//dbMakeObjectBox ( 400, 35, 1, 10 );
	//dbTextureObject ( 400, 1 );
	//dbPositionObject ( 400, 145, -15.0f, 0.0f );
	//dbPhyMakeRigidBodyStaticBox ( 400 );

	fX = 8.0f;

	for ( int i = 202; i < 225; i++ )
	{
		dbMakeObjectBox ( i, 1, 1, 10 );
		dbPositionObject ( i, fX, 0.0f, 0.0f );
		dbTextureObject ( i, 1 );
		dbPhyMakeRigidBodyDynamicBox ( i );
		//dbPhySetRigidBodyMaxAngularVelocity ( i, 0.01f );
		//dbPhySetRigidBodyMass ( i, 1 );
		//dbPhySetRigidBodyLinearDamping ( i, 10000000.0f );
		//dbPhySetRigidBodyAngularDamping ( i, 10000000.0f );

		dbPhyWakeUpRigidBody ( i, 1e10 );

		fX = fX + 1.1f;
		//fX = fX + 0.9f;
	}

	int iJoint = 1;

	//dbPhyMakeRevoluteJoint ( iJoint++, 200, 202, 0, 1, 0, dbObjectPositionX ( 202 ), dbObjectPositionY ( 202 ), dbObjectPositionZ ( 202 ) );
	//dbPhyMakeRevoluteJoint ( iJoint++, 201, 224, 0, 1, 0, dbObjectPositionX ( 224 ), dbObjectPositionY ( 224 ), dbObjectPositionZ ( 224 ) );

	dbPhyMakeFixedJoint ( iJoint++, 200, 202 );
	dbPhyMakeFixedJoint ( iJoint++, 201, 224 );

	for ( int i = 202; i < 224; i++, iJoint++ )
	//for ( int i = 202; i < 208; i++, iJoint++ )
	{
		
		//dbPhyMakeRevoluteJoint ( iJoint, i, i + 1, 0, 1, 0, dbObjectPositionX ( i ) + 0.5f, dbObjectPositionY ( i ), dbObjectPositionZ ( i ) );

		//dbPhyMakeRevoluteJoint ( iJoint, 200, i, 0, 1, 0, dbObjectPositionX ( i ) + 0.5f, dbObjectPositionY ( i ), dbObjectPositionZ ( i ) );

		//dbPhyMakeFixedJoint ( iJoint, i, i + 1 );
		dbPhyMakeFixedJoint ( iJoint, 200, i + 1 );

		

		//dbPhyMakeSphereJoint		( iJoint, i, i + 1, dbObjectPositionX ( i ) + 0.5f, dbObjectPositionY ( i ), dbObjectPositionZ ( i ) );

		dbPhySetJointBreakLimits ( iJoint, 9000000.0f, 9000000.0f );
	}

	//
	fX = 8.0f;
	for ( int i = 500; i < 524; i++ )
	{
		//dbMakeObjectBox ( i, 10, 10, 1 );
		//dbPositionObject ( i, fX, -6, 0.0f );
		//dbTextureObject ( i, 1 );
		//dbPhyMakeRigidBodyStaticBox ( i );
		//dbPhyMakeRigidBodyDynamicBox ( i );
		//dbPhySetRigidBodyLinearDamping ( i, 10000000.0f );
		//dbPhySetRigidBodyAngularDamping ( i, 10000000.0f );

		fX = fX + 1.1f;
	}
	
	//


*/

/*
dbLoadObject ( "corsa//corsa.x", 2 );
	dbSetObjectTransparency ( 2 ,  2 );
	dbRotateObject ( 2 , 0, 90, 0 );
	//dbOffsetLimb ( 2 , 0 , 0 , 0 , -1 );
	dbFixObjectPivot ( 2 );
	dbPositionObject ( 2, 0, 0, 0 );
	dbPhyTestVehicle ( 2 );
*/
	
	//dbMakeObjectBox ( 3, 50, 1, 50 );
	//dbPositionObject ( 3, 100, -11.5, 0 );
	//dbTextureObject ( 3, 3 );
	//dbPhyMakeRigidBodyStaticBox ( 3 );


	dbLoadObject ( "beach.x", 2 );

	//dbPositionObject ( 2, -80, -9, 0 );
	dbPositionObject ( 2, 0, -8, 0 );
	dbRotateObject ( 2, 0, -90, 0 );
	dbFixObjectPivot ( 2 );
	//dbPhyTestVehicle ( 2 );

	{
		int iObject = 2;
		NxReal width  = 1.1f;
		NxReal height = 1.0f;
		NxReal length = 2.2f;

		float fWheelX = 1.5f;
		float fWheelY = 0.0f;
		float fWheelZ = 1.0f;


		dbPhyCreateVehicle		( iObject );

		dbPhyAddVehicleBody		( iObject, width, height, length, 0.0f, height, 0.0f );

		// 5.0, 0.1

		//float fRadius = 1.0f;
		//float fHeight = 0.5f;

		float fRadius = 1.0f;
		float fHeight = 0.2f;

		dbPhyAddVehicleWheel	( iObject, 5,  fWheelX, fWheelY,  fWheelZ, fRadius, fHeight, 0, 0 );
		dbPhyAddVehicleWheel	( iObject, 7,  fWheelX, fWheelY, -fWheelZ, fRadius, fHeight, 0, 1 );
		dbPhyAddVehicleWheel	( iObject, 3, -fWheelX, fWheelY, -fWheelZ, fRadius, fHeight, 1, 0 );
		dbPhyAddVehicleWheel	( iObject, 9, -fWheelX, fWheelY,  fWheelZ, fRadius, fHeight, 1, 1 );

		dbPhySetVehicleAuto		( iObject, 1 );


		//dbPhySetVehicleStaticFriction ( iObject, 12.0f );
		dbPhySetVehicleDynamicFriction ( iObject, 0.4f );
		dbPhySetVehicleStaticFriction ( iObject, 15.0f );
		//dbPhySetVehicleStaticFriction ( iObject, 2.0f );
		//dbPhySetVehicleMaxMotor ( iObject, 2.0f );
		dbPhySetVehicleMaxMotor ( iObject, 500.0f );
		dbPhySetVehicleSteeringDelta ( iObject, 0.25f );
		dbPhySetVehicleStaticFrictionV ( iObject, 0.01f );
		dbPhySetVehicleDynamicFrictionV ( iObject, 0.01f );
		float fValue = NxMath::degToRad ( 60.0f );
		dbPhySetVehicleMaxSteeringAngle ( iObject, NxMath::degToRad ( 60.0f ) );

		dbPhyBuildVehicle		( iObject );
	}


	//dbMakeObjectSphere ( 3, 5 );
	//dbPositionObject ( 3, 3, 5, 0 );
	//dbPhyMakeRigidBodyDynamicSphere ( 3 );
	
	// 87, -5, 0

	//dbLoadImage ( "fire.png", 3 );
	//dbPhyMakeFireEmitter ( 3, 512, 87.0f, -5.0f, 0.0f );
	//dbTextureObject ( 3, 3 );
	

//	float fX = -35.0f;
	//float fY =   5.0f;



	// 250, 400




	dbPositionCamera ( 0, 10, 0 );

	
	dbSyncRate ( 60 );

	//dbPhySetVehicleAuto ( 2, 0 );

	//dbPhySetGravity ( 0, -2.0f, 0 );

	while ( !dbEscapeKey ( ) )
	{
		showFPS   ( );
		//userInput ( );

		float x = dbObjectPositionX ( 2 );
		float y = dbObjectPositionY ( 2 );
		float z = dbObjectPositionZ ( 2 );

		char* szKey = dbInKey ( );

		//if ( strcmp ( szKey, "w" ) == 0 )
		//	dbPhySetVehicleMotorForce ( 2, 150.0f );


		//dbPositionCamera ( x, y, z );
		//dbSetCameraToObjectOrientation ( 2 );
		//dbTurnCameraRight ( 90 );

		//dbSetCameraToFollow ( x, y, z, 45.0f, 40.0f, 15.0f, 5.0f, 0 );
		//dbSetCameraToFollow ( x, y, z, 45.0f, 10.0f, 5.0f, 5.0f, 0 );
		//dbSetCameraToFollow ( x, y, z, dbObjectAngleY ( 2 ) + 90, 7.0f, 4.0f, 1.0f, 0 );
		//dbSetCameraToFollow ( x, y, z, dbObjectAngleY ( 2 ) + 90, 6.0f, 4.0f, 1.0f, 0 );
		dbSetCameraToFollow ( x, y, z, 0, 6.0f, 4.0f, 1.0f, 0 );
		//dbSetCameraToFollow ( x, y, z, dbObjectAngleY ( 2 ) + 90, 20.0f, 20.0f, 10.0f, 0 );
		
		//dbSetCameraToFollow ( x, y, z, 45.0f, 6.0f, 4.0f, 1.0f, 0 );
		
		
		//
		//dbSetCameraToFollow ( x, y, z, 45.0f, 6.0f, 3.0f, 5.0f, 0 );

		

		/*
		for ( int i = 10; i < 16; i++ )
		{
			dbPositionObject ( i, x, y, z );
			dbRotateObject ( i, dbObjectAngleX ( 2 ), dbObjectAngleY ( 2 ), dbObjectAngleZ ( 2 ) );
		}
		*/

		dbUpdateTerrain ( );
		
		dbPhyUpdate ( );
		dbPhyUpdateVehicle ( );
//		dbUpdateParticles ( );
		//tickCar ( );
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