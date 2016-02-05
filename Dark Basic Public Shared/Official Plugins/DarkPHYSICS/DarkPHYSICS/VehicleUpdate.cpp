
#include "globals.h"
#include "vehiclestructure.h"
#include "vehicleupdate.h"

/* is this used any more?
float GetLimbAngleXEx ( int iID, int iLimbID )
{
	sObject* pObject = PhysX::GetObject ( iID );
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	//UpdateRealtimeFrameVectors ( pObject, pFrame );
	return pFrame->vecRotation.x;
	return 0.0f;
}
*/

void dbPhyUpdateVehicleWheels ( PhysXVehicle::sVehicle* pVehicle )
{
	sObject* pObject = PhysX::GetObject ( pVehicle->iID );

	if ( !pObject )
		return;

	if ( !pVehicle->bBuilt )
		return;

	//  - 120706 - new system has an axle speed value
	DWORD dwA = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb );
	DWORD dwB = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb );
	DWORD dwC = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb );
	DWORD dwD = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb );
	float fA = (*(float*)&dwA);
	float fB = (*(float*)&dwB);
	float fC = (*(float*)&dwC);
	float fD = (*(float*)&dwD);
	float fNegA=1.0f, fNegB=1.0f, fNegC=1.0f, fNegD=1.0f;
	if ( pVehicle->wheels [ 0 ].iDirection == 1 ) fNegA = -1.0f;
	if ( pVehicle->wheels [ 1 ].iDirection == 1 ) fNegB = -1.0f;
	if ( pVehicle->wheels [ 2 ].iDirection == 1 ) fNegC = -1.0f;
	if ( pVehicle->wheels [ 3 ].iDirection == 1 ) fNegD = -1.0f;
	if ( pVehicle->wheelptr[0] ) fA += pVehicle->wheelptr [ 0 ]->getAxleSpeed() * fNegA;
	if ( pVehicle->wheelptr[1] ) fB += pVehicle->wheelptr [ 1 ]->getAxleSpeed() * fNegB;
	if ( pVehicle->wheelptr[2] ) fC += pVehicle->wheelptr [ 2 ]->getAxleSpeed() * fNegC;
	if ( pVehicle->wheelptr[3] ) fD += pVehicle->wheelptr [ 3 ]->getAxleSpeed() * fNegD;
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb, fA, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb, fB, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb, fC, pVehicle->fSteeringValue, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb, fD, pVehicle->fSteeringValue, 0 );

	

	pVehicle->fOldPosX = pVehicle->pObject->position.vecPosition.x;
	pVehicle->fOldPosY = pVehicle->pObject->position.vecPosition.y;
	pVehicle->fOldPosZ = pVehicle->pObject->position.vecPosition.z;
	
}

void dbPhyDriveVehicle ( PhysXVehicle::sVehicle* pVehicle )
{
	if ( !pVehicle )
		return;

	if ( !pVehicle->bBuilt )
		return;

	if ( pVehicle->bAutoDrive == false )
		return;

	if ( PhysX::GetKeyState == NULL )
		return;

	if ( PhysX::GetKeyState ( 200 ) == 1 )
	{
		pVehicle->fMotorForce = pVehicle->fMaxMotorPower;
	}
	else if ( PhysX::GetKeyState ( 208 ) == 1 )
	{
		pVehicle->fMotorForce = 0.0f;
	}
	else if ( pVehicle->fMotorForce != 0.0f )
	{
		pVehicle->fMotorForce = 0.0f;

		if ( pVehicle->fMotorForce < 0.0f )
			pVehicle->fMotorForce = 0.0f;
	}

	if ( PhysX::GetKeyState ( 205 ) == 1 )
	{
		if ( pVehicle->fSteeringAngle > ( -1.0f + pVehicle->fSteeringDelta ) )
			pVehicle->fSteeringAngle -= pVehicle->fSteeringDelta / 2;
	}
	else if ( PhysX::GetKeyState ( 203 ) )
	{
		if ( pVehicle->fSteeringAngle < ( 1.0f - pVehicle->fSteeringDelta ) )
			pVehicle->fSteeringAngle += pVehicle->fSteeringDelta / 2;
	}
	else 
	{
		if ( pVehicle->fSteeringAngle > 0 )
		{
			pVehicle->fSteeringAngle -= pVehicle->fSteeringDelta * 2;

			if ( pVehicle->fSteeringAngle < 0 )
				pVehicle->fSteeringAngle = 0;
		}
		else if ( pVehicle->fSteeringAngle < 0 )
		{
			pVehicle->fSteeringAngle += pVehicle->fSteeringDelta * 2;

			if ( pVehicle->fSteeringAngle > 0 )
				pVehicle->fSteeringAngle = 0;
		}
	}

	pVehicle->fSteeringValue = -( pVehicle->fSteeringAngle * 25 );
}

void dbPhyMoveVehicle ( PhysXVehicle::sVehicle* pVehicle )
{
	if ( !pVehicle )
		return;

	if ( !pVehicle->bBuilt )
		return;

	//  - 120706 - new wheel system from 2.5.0
	NxReal steeringAngle = pVehicle->fSteeringAngle * pVehicle->fMaxSteeringAngle;
	for ( int iW=0; iW<pVehicle->iWheelCount; iW++ )
	{
		pVehicle->wheelptr [ iW ]->setMotorTorque ( pVehicle->fMotorForce * 2 );
		if ( pVehicle->wheels [ iW ].bFront==true ) pVehicle->wheelptr [ iW ]->setSteerAngle ( -steeringAngle );

		if ( pVehicle->fMotorForce == 0.0f && pVehicle->bAutoDrive == true )
			pVehicle->wheelptr [ iW ]->setBrakeTorque ( 250.0f );
	}

	pVehicle->wheelContactPoints.clear ( );

}

void dbPhyUpdateVehicle ( void )
{
	for ( DWORD dwVehicle = 0; dwVehicle < PhysXVehicle::pVehicleList.size ( ); dwVehicle++ )
	{
		PhysXVehicle::sVehicle* pVehicle = PhysXVehicle::pVehicleList [ dwVehicle ];

		dbPhyUpdateVehicleWheels ( pVehicle );
		dbPhyDriveVehicle		 ( pVehicle );
		dbPhyMoveVehicle		 ( pVehicle );
	}
}

float dbPhyGetDistance ( float x1 , float y1 , float z1 , float x2 , float y2 , float z2)
{
	float dist;
	float dx;
	float dy;
	float dz;

	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;

	dist = ( float ) sqrt ( ( dx * dx ) + ( dy * dy ) + ( dz * dz ) );

	return dist;
}
