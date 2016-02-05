
#include "DarkSDK.h"
#include "physics.h"

void DarkSDK ( void )
{
	dbPhyStart ( );
	
	dbAutoCamOff ( );
	dbSyncOn ( );
	dbSyncRate ( 60 );
	dbColorBackdrop ( 0 );
	
	dbMakeObjectBox ( 1, 50, 1, 50 );
	dbPhyMakeRigidBodyStaticBox ( 1 );

	dbMakeObjectCube ( 2, 5 );
	dbMakeObjectCube ( 3, 5 );
	dbMakeObjectCube ( 4, 5 );

	dbPositionObject ( 2, 0, 10, 0 );
	dbPositionObject ( 3, 2, 20, 0 );
	dbPositionObject ( 4, 3, 30, 0 );

	dbPhyMakeRigidBodyStaticBox  ( 2 );
	dbPhyMakeRigidBodyDynamicBox ( 3 );
	dbPhyMakeRigidBodyDynamicBox ( 4 );

	dbPositionCamera ( 0, 20, -40 );

	while ( !dbEscapeKey ( ) )
	{
		while ( dbPhyGetCollisionData ( ) )
		{
			int iObjectA = dbPhyGetCollisionObjectA ( );
			int iObjectB = dbPhyGetCollisionObjectB ( );
	
			if ( iObjectA == 3 && iObjectB == 4 )
			{
				dbColorObject ( iObjectA, dbRgb ( 255, 0, 0 ) );
				dbColorObject ( iObjectB, dbRgb ( 0, 255, 0 ) );
			}

			if ( iObjectA == 2 || iObjectB == 2 )
				dbColorObject ( 2, dbRgb ( 255, 255, 0 ) );

			if ( iObjectA == 1 || iObjectB == 1 )
				dbColorObject ( 1, dbRgb ( 0, 0, 255 ) );

		}

		dbPhyUpdate ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
}