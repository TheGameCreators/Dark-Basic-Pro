
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
	
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );


	/*
	dbMakeObjectBox ( 1, 1, 10, 1 );
	dbPositionObject ( 1, 0, 5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	
	dbPhyMakeCloth ( 2 );
	dbPhySetClothDimensions ( 2, 4, 4, 0.2 );
	dbPhySetClothPosition ( 2, 0, 10, 0 );
	//dbPhySetClothStatic ( 2, 1 );
	dbPhySetClothRotation ( 2, 90, 0, 0 );
	//dbPhySetClothGravity ( 2, 1 );
	//dbPhySetClothDamping ( 2, 0 );

	dbPhySetClothExternalAcceleration ( 2, 20, -10, -10 );

	dbPhyBuildCloth ( 2 );

	dbPhyAttachClothToShape ( 2, 1, 1 );

	dbLoadImage ( "banner_horse.dds", 2 );
	dbTextureObject ( 2, 2 );

	
	dbPositionCamera ( 1.8, 10.8, 7.8 );
	dbRotateCamera ( 32, 190, 0 );
	*/

	//dbPositionCamera ( 0, 10, 0 );



	dbMakeObjectBox ( 11, 1, 1, 5 );
	dbPositionObject ( 11, 8, 3, 0 );
	dbPhyMakeRigidBodyStaticBox ( 11 );
	//dbSetObjectWireframe ( 11, 1 );

	dbMakeObjectBox ( 12, 1, 1, 5 );
	dbPositionObject ( 12, 0, 3, 0 );
	dbPhyMakeRigidBodyStaticBox ( 12 );
	//dbSetObjectWireframe ( 12, 1 );

	
	dbPhyMakeCloth ( 13 );
	dbPhySetClothDimensions ( 13, 8, 5, 0.5 );
	dbPhySetClothPosition ( 13, 0, 3, -2.5 );
	//dbPhySetClothStatic ( 13, 1 );
	dbPhyBuildCloth ( 13 );
	//dbSetObjectWireframe ( 13, 1 );
	//dbTextureObject ( 13, 1 );

	dbLoadImage ( "rug512.bmp", 1 );
	dbTextureObject ( 13, 1 );

	dbPhyAttachClothToAllShapes ( 13, 0 );
	

	dbPositionCamera ( 0.3, 5, -7 );
	dbRotateCamera ( 23, 28, 0 );

	while ( !dbEscapeKey ( ) )
	{
		//showFPS ( );
		//userInput ( );

		

		//dbPhySetClothExternalAcceleration ( 2, 35, 0, 5 + dbRnd ( 20 ) );

		static int i = 0;

		//if ( i++ <= 1 )
			dbPhyUpdate ( );
		dbPhyUpdateCloth ( );
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

	strcpy ( szFPS, dbStr ( dbStatistic ( 1 ) ) );
	dbPrint ( szFPS );

	strcpy ( szFPS, dbStr ( dbCameraAngleX ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleY ( ) ) );
	dbPrint ( szFPS );
	strcpy ( szFPS, dbStr ( dbCameraAngleZ ( ) ) );
	dbPrint ( szFPS );

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