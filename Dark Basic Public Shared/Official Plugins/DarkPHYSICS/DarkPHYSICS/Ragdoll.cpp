
#include "DarkSDK.h"

void dbPhyEnableDebug					( void );
void dbPhyStart							( void );
void dbPhyStart							( int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
void dbPhyEnd							( void );
void dbPhyClear							( void );
int	 dbPhysX							( void );
void dbPhyMakeScene						( int iIndex, int iSimulationType, int iSimulationSceneType, int iSimulationFlags );
void dbPhySetCurrentScene				( int iIndex );
void dbPhySetGroundPlane				( int iState );
void dbPhyUpdate						( void );
void dbPhyMakeRigidBodyStaticBox		( int iObject );
void dbPhyMakeRigidBodyDynamicSphere	( int iObject );
void dbPhyMakeFixedJoint	( int iJoint, int iA, int iB );

void dbPhyMakeRagdoll       ( int iRagdollID, int iObject, int iPhysicsObject );
void dbPhyAssignRagdollPart ( int iRagdollID, int iMesh, int iBone, int iObject );

void userInput ( void );

float g_fSpeed        = 5.0f;
float g_fTurn         = 0.03f;
float g_fOldCamAngleX = 0.0f;
float g_fOldCamAngleY = 0.0f;
float g_fCameraAngleX = 0.0f;
float g_fCameraAngleY = 0.0f;



void MakeObject ( int iID, sObject* pObject, int iBoneIndex )
{
	dbMakeObjectSphere ( iID, 5 );
	dbPositionObject ( 
						iID,
						pObject->ppMeshList [ 0 ]->pFrameMatrices [ iBoneIndex ]->_41,
						pObject->ppMeshList [ 0 ]->pFrameMatrices [ iBoneIndex ]->_42,
						pObject->ppMeshList [ 0 ]->pFrameMatrices [ 5 ]->_43
					);
}

void DarkGDK ( void )
{
	dbPhyEnableDebug ( );
	dbPhyStart      ( );
	dbAutoCamOff    ( );
	dbSyncOn        ( );
	dbSyncRate      ( 60 );
	
	dbLoadObject ( "NAZ_GREN.X", 1 );
	dbSetObjectWireframe ( 1, 1 );

	dbMakeObjectBox             ( 2, 100, 1, 100 );
	dbPositionObject            ( 2, 0, -10, 0 );
	dbPhyMakeRigidBodyStaticBox ( 2 );

	dbPositionCamera ( 0, 50, -100 );

	sObject* pObject = dbGetObject ( 1 );

	MakeObject ( 100, pObject, 5 );	// head
	MakeObject ( 101, pObject, 3 );	// neck

	MakeObject ( 102, pObject, 4 );	// leftneck
	MakeObject ( 103, pObject, 8 );	// rightneck

	MakeObject ( 104, pObject,  0 );	// leftshoulder
	MakeObject ( 105, pObject, 21 );	// leftelbow
	MakeObject ( 106, pObject, 22 );	// leftwrist
	MakeObject ( 107, pObject, 23 );	// leftheand

	MakeObject ( 108, pObject,  7 );	// rightshoulder
	MakeObject ( 109, pObject, 18 );	// rightelbow
	MakeObject ( 110, pObject, 19 );	// rightwrist
	MakeObject ( 111, pObject, 20 );	// rightheand

	MakeObject ( 112, pObject,  1 );	// spine
	MakeObject ( 113, pObject,  2 );	// spine1
	MakeObject ( 114, pObject,  6 );	// spine2
	MakeObject ( 115, pObject,  9 );	// pelvis

	MakeObject ( 116, pObject, 10 );	// lefthip
	MakeObject ( 117, pObject, 12 );	// righthip
	MakeObject ( 118, pObject, 11 );	// leftknee
	MakeObject ( 119, pObject, 13 );	// leftankle
	MakeObject ( 120, pObject, 14 );	// leftfoot
	MakeObject ( 121, pObject, 15 );	// rightknee
	MakeObject ( 122, pObject, 16 );	// rightankle
	MakeObject ( 123, pObject, 17 );	// rightfoot

	int iJoint = 1;

	for ( int i = 100; i < 124; i++ )
		dbPhyMakeRigidBodyDynamicSphere ( i );
	
	dbPhyMakeFixedJoint	( iJoint++, 100, 101 );
	dbPhyMakeFixedJoint	( iJoint++, 101, 102 );
	dbPhyMakeFixedJoint	( iJoint++, 101, 103 );

	dbPhyMakeFixedJoint	( iJoint++, 102, 104 );
	dbPhyMakeFixedJoint	( iJoint++, 104, 105 );
	dbPhyMakeFixedJoint	( iJoint++, 105, 106 );
	dbPhyMakeFixedJoint	( iJoint++, 106, 107 );

	dbPhyMakeFixedJoint	( iJoint++, 103, 108 );
	dbPhyMakeFixedJoint	( iJoint++, 108, 109 );
	dbPhyMakeFixedJoint	( iJoint++, 109, 110 );
	dbPhyMakeFixedJoint	( iJoint++, 110, 111 );

	dbPhyMakeFixedJoint	( iJoint++, 101, 112 );
	dbPhyMakeFixedJoint	( iJoint++, 112, 113 );
	dbPhyMakeFixedJoint	( iJoint++, 113, 114 );
	dbPhyMakeFixedJoint	( iJoint++, 114, 115 );

	dbPhyMakeFixedJoint	( iJoint++, 115, 116 );
	dbPhyMakeFixedJoint	( iJoint++, 116, 118 );
	dbPhyMakeFixedJoint	( iJoint++, 118, 119 );
	dbPhyMakeFixedJoint	( iJoint++, 119, 120 );

	dbPhyMakeFixedJoint	( iJoint++, 115, 117 );
	dbPhyMakeFixedJoint	( iJoint++, 117, 121 );
	dbPhyMakeFixedJoint	( iJoint++, 121, 122 );
	dbPhyMakeFixedJoint	( iJoint++, 122, 123 );

	dbPhyMakeRagdoll       ( 1, 1, 100 );

	dbPhyAssignRagdollPart ( 1, 0, 5, 100 );
	dbPhyAssignRagdollPart ( 1, 0, 3, 101 );
	dbPhyAssignRagdollPart ( 1, 0, 4, 102 );
	dbPhyAssignRagdollPart ( 1, 0, 8, 103 );

	dbPhyAssignRagdollPart ( 1, 0, 0, 104 );
	dbPhyAssignRagdollPart ( 1, 0, 21, 105 );
	dbPhyAssignRagdollPart ( 1, 0, 22, 106 );
	dbPhyAssignRagdollPart ( 1, 0, 23, 107 );

	dbPhyAssignRagdollPart ( 1, 0, 7, 108 );
	dbPhyAssignRagdollPart ( 1, 0, 18, 109 );
	dbPhyAssignRagdollPart ( 1, 0, 19, 110 );
	dbPhyAssignRagdollPart ( 1, 0, 20, 111 );
	
	dbPhyAssignRagdollPart ( 1, 0, 1, 112 );
	dbPhyAssignRagdollPart ( 1, 0, 2, 113 );
	dbPhyAssignRagdollPart ( 1, 0, 6, 114 );
	dbPhyAssignRagdollPart ( 1, 0, 9, 115 );
		
	dbPhyAssignRagdollPart ( 1, 0, 10, 116 );
	dbPhyAssignRagdollPart ( 1, 0, 12, 117 );
	dbPhyAssignRagdollPart ( 1, 0, 11, 118 );
	dbPhyAssignRagdollPart ( 1, 0, 13, 119 );
	dbPhyAssignRagdollPart ( 1, 0, 14, 120 );
	dbPhyAssignRagdollPart ( 1, 0, 15, 121 );
	dbPhyAssignRagdollPart ( 1, 0, 16, 122 );
	dbPhyAssignRagdollPart ( 1, 0, 17, 123 );

	while ( !dbEscapeKey ( ) )
	{
		userInput ( );

		dbPhyUpdate ( );
		dbSync      ( );
	}

	dbPhyEnd ( );
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