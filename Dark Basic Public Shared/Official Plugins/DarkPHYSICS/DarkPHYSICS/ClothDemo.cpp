
#include "DarkSDK.h"
#include "physics.h"
#include "clothcommands.h"

float g_fSpeed        = 0.1f;
float g_fTurn         = 0.03f;
float g_fOldCamAngleX = 0.0f;
float g_fOldCamAngleY = 0.0f;
float g_fCameraAngleX = 0.0f;
float g_fCameraAngleY = 0.0f;

void userInput ( void );
void showFPS   ( void );

void dbPhyMakeCloth ( int iID );
void dbPhyUpdateCloth ( void );

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );

	//dbMakeLight ( 1 );
	//dbSetDirectionalLight ( 1, -5, -5, 5 );


	//dbMakeLight ( 1 );
	//dbSetDirectionalLight ( 1, -5, -5, 5 );

	dbMakeObjectBox ( 1, 200, 1, 200 );
	dbSetObjectWireframe ( 1, 1 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	
	dbMakeObjectBox ( 2, 10, 1, 1 );
	dbPositionObject ( 2, 1, 6.5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 2 );

	
	dbPhyMakeCloth ( 3 );
	dbPhySetClothDimensions ( 3, 8, 7, 0.2 );
	dbPhySetClothPosition ( 3, -3, 7.1, 0 );
	dbPhySetClothTearing ( 3, 1 );
	dbPhyBuildCloth ( 3 );

	dbPhyAttachClothToShape ( 3, 2, 1 );

	
	dbLoadImage ( "rug512.bmp", 1 );
	dbLoadImage ( "banner_horse.dds", 2 );
	dbTextureObject ( 3, 2 );

	//dbMakeObjectBox ( 4, 1, 10, 1 );
	dbMakeObjectBox ( 4, 5, 2, 5 );
	dbPositionObject ( 4, 10, 2.5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 4 );
	
	
	dbPhyMakeCloth ( 5 );
	dbPhySetClothDimensions ( 5, 8, 8, 0.2 );
	dbPhySetClothPosition ( 5, 6, 9, -4.5 );
	dbPhySetClothGravity ( 5, 0 );
	dbPhySetClothDamping ( 5, 1 );
	dbPhyBuildCloth ( 5 );

	dbPhyAttachClothToShape ( 5, 4, 1 );

	dbTextureObject ( 5, 1 );

	dbMakeObjectBox ( 6, 1, 10, 1 );
	dbPositionObject ( 6, 15, 5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 6 );

	dbPhyMakeCloth ( 7 );
	dbPhySetClothDimensions ( 7, 4, 4, 0.2 );
	dbPhySetClothPosition ( 7, 15.1, 10, 0 );
	dbPhySetClothGravity ( 7, 1 );
	dbPhySetClothDamping ( 7, 0 );

	//dbPhySetClothStatic ( 7, 1 );
	dbPhySetClothRotation ( 7, 90, 0, 0 );

	dbPhySetClothExternalAcceleration ( 7, 35, 0, 10 );

	dbPhyBuildCloth ( 7 );

	dbPhyAttachClothToShape ( 7, 6, 1 );
	dbTextureObject ( 7, 2 );
	

	//dbMakeObjectCube ( 4, 1);
	//dbPositionObject ( 4, 0, 15, 4 );
	//dbPhyMakeRigidBodyDynamicBox ( 4 );

	dbMakeObjectBox ( 8, 2, 2, 2 );
	dbPositionObject ( 8, 22, 10, 0 );
	dbPhyMakeRigidBodyStaticBox ( 8 );

	dbMakeObjectSphere ( 9, 1 );
	dbPositionObject ( 9, 22, 9, 6.5 );
	dbPhyMakeRigidBodyDynamicSphere ( 9 );

	dbPhyMakeCloth ( 10 );
	dbPhySetClothDimensions ( 10, 2, 7, 0.4 );
	dbPhySetClothPosition ( 10, 21, 9, 0 );
	//dbPhySetClothStatic ( 10, 1 );
	dbPhyBuildCloth ( 10 );
	dbTextureObject ( 10, 1 );

	
	dbPhyAttachClothToAllShapes ( 10, 0 );

	

	//
	dbMakeObjectBox ( 11, 1, 1, 5 );
	dbPositionObject ( 11, -8, 3, 0 );
	dbPhyMakeRigidBodyStaticBox ( 11 );

	dbMakeObjectBox ( 12, 1, 1, 5 );
	dbPositionObject ( 12, -16, 3, 0 );
	dbPhyMakeRigidBodyStaticBox ( 12 );

	dbPhyMakeCloth ( 13 );
	dbPhySetClothDimensions ( 13, 8, 5, 0.5 );
	dbPhySetClothPosition ( 13, -16, 3, -2.5 );
	dbPhyBuildCloth ( 13 );
	dbTextureObject ( 13, 1 );

	dbPhyAttachClothToAllShapes ( 13, 0 );
	//

	//dbPhySetSkinWidth ( 2.005f );

	
	

	
	

	for ( int i = 1; i < 30; i++ )
	{
		if ( dbObjectExist ( i ) )
		{
			//dbColorObject			 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecular		 ( i, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbSetObjectSpecularPower ( i, 255 );
			//dbSetObjectAmbient		 ( i, 0 );
		}
	}

	dbPositionCamera ( 22, 15, 20 );
	dbRotateCamera ( 30, 210, 0 );

int iID = 20;
	int iDelay = 25;

	while ( !dbEscapeKey ( ) )
	{
		if ( dbSpaceKey ( ) && iDelay > 5 )
		{
			dbMakeObjectSphere ( iID, 0.6f );
			dbColorObject ( iID, dbRgb ( dbRnd ( 255 ), dbRnd ( 255 ), dbRnd ( 255 ) ) );
			dbPositionObject ( iID, dbCameraPositionX ( ), dbCameraPositionY ( ), dbCameraPositionZ ( ) );
			dbPhyMakeRigidBodyDynamicSphere ( iID );

			D3DXVECTOR3 vecVelocity = D3DXVECTOR3 ( dbCameraLookX ( 0 ), dbCameraLookY ( 0 ), dbCameraLookZ ( 0 ) );

			vecVelocity += ( 10 * vecVelocity );

			dbPhySetRigidBodyLinearVelocity ( iID, vecVelocity.x, vecVelocity.y, vecVelocity.z );
			iDelay = 0;
			iID++;
		}

		iDelay++;

	//	showFPS   ( );
	//	userInput ( );

		dbPhyUpdate ( );
		//dbPhyUpdateCloth ( );
		dbSync      ( );
	}

	//dbPhyEnd ( );
}


void showFPS ( void )
{
	dbSetCursor ( 0, 0 );

	char szFPS [ 256 ] = "";
	strcpy ( szFPS, "fps = " );
	strcat ( szFPS, dbStr ( dbScreenFPS ( ) ) );
	dbText ( dbScreenWidth ( ) - 20 - dbTextWidth ( szFPS ), dbScreenHeight ( ) - 40, szFPS );

	strcpy ( szFPS, dbStr ( dbStatistic ( 1 ) ) );
	dbPrint ( szFPS );


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

	//float fHeight = dbGetTerrainGroundHeight ( 1, dbCameraPositionX ( ), dbCameraPositionZ ( ) );
	//dbPositionCamera ( dbCameraPositionX ( ), fHeight + 3.0f, dbCameraPositionZ ( ) );
}