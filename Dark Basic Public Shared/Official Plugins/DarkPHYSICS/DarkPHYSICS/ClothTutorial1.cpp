
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

	dbMakeObjectBox ( 2, 10, 1, 1 );
	dbPositionObject ( 2, 1, 6.5, 0 );
	dbPhyMakeRigidBodyStaticBox ( 2 );
	//dbSetObjectWireframe ( 2, 1 );

	
	dbPhyMakeCloth ( 3 );
	dbPhySetClothDimensions ( 3, 8, 7, 0.2 );
	dbPhySetClothPosition ( 3, -3, 7.1, 0 );
	dbPhyBuildCloth ( 3 );

	//dbSetObjectWireframe ( 3, 1 );

	dbPhyAttachClothToShape ( 3, 2, 1 );

	
	dbLoadImage ( "banner_horse.dds", 2 );
	dbTextureObject ( 3, 2 );
	

	
	dbPositionCamera ( 5.5, 8.1, 8.5 );
	dbRotateCamera ( 27.5, 205, 0 );

	//dbPositionCamera ( 0, 10, 0 );

	while ( !dbEscapeKey ( ) )
	{
		//showFPS ( );
	//	userInput ( );

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