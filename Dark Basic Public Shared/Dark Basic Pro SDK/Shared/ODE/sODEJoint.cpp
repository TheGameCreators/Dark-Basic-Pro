#include "ODEPhysics.h"
#include "sODEJoint.h"

extern dWorldID g_ODEWorld;

sODEJoint* sODEJoint::pODEJointList = NULL;

sODEJoint::sODEJoint( )
{
	pNextJoint = 0;
	pPrevJoint = 0;

	bMovingLimit = false;
	fLowStart = 0;
	fHighStart = 0;
	fLowEnd = 0;
	fHighEnd = 0;
	fMoveTime = 0;
	fOrigMoveTime = 0;
}

sODEJoint::~sODEJoint() 
{
	dJointDestroy( oJoint );
}

sODEJoint* sODEJoint::AddFixedJoint( int id, dBodyID body1, dBodyID body2 )
{
	sODEJoint *pJoint = new sODEJoint( );

	//modify prev and next pointers of any involved joints, make new joint the new head
	pJoint->pNextJoint = pODEJointList;
	if ( pODEJointList ) pODEJointList->pPrevJoint = pJoint;
	pODEJointList = pJoint;

	//create the ODE hinge joint
	pJoint->oJoint = dJointCreateFixed( g_ODEWorld, 0 );
	dJointAttach( pJoint->oJoint, body1, body2 );
	dJointSetFixed( pJoint->oJoint );

	pJoint->iID = id;

	return pJoint;
}

sODEJoint* sODEJoint::AddHingeJoint( int id, dBodyID body1, dBodyID body2, float ax, float ay, float az, float x, float y, float z )
{
	sODEJoint *pJoint = new sODEJoint( );

	//modify prev and next pointers of any involved joints, make new joint the new head
	pJoint->pNextJoint = pODEJointList;
	if ( pODEJointList ) pODEJointList->pPrevJoint = pJoint;
	pODEJointList = pJoint;

	//create the ODE hinge joint
	pJoint->oJoint = dJointCreateHinge( g_ODEWorld, 0 );
	dJointAttach( pJoint->oJoint, body1, body2 );
	dJointSetHingeAnchor( pJoint->oJoint, x,y,z );
	dJointSetHingeAxis( pJoint->oJoint, ax,ay,az );

	pJoint->iID = id;

	return pJoint;
}

sODEJoint* sODEJoint::AddHinge2Joint( int id, dBodyID body1, dBodyID body2, float ax, float ay, float az, float ax2, float ay2, float az2, float x, float y, float z )
{
	sODEJoint *pJoint = new sODEJoint( );

	//modify prev and next pointers of any involved joints, make new joint the new head
	pJoint->pNextJoint = pODEJointList;
	if ( pODEJointList ) pODEJointList->pPrevJoint = pJoint;
	pODEJointList = pJoint;

	//create the ODE hinge joint
	pJoint->oJoint = dJointCreateHinge2( g_ODEWorld, 0 );
	dJointAttach( pJoint->oJoint, body1, body2 );
	dJointSetHinge2Anchor( pJoint->oJoint, x,y,z );
	dJointSetHinge2Axis1( pJoint->oJoint, ax,ay,az );
	dJointSetHinge2Axis2( pJoint->oJoint, ax2,ay2,az2 );
	
	pJoint->iID = id;

	return pJoint;
}

sODEJoint* sODEJoint::AddBallJoint( int id, dBodyID body1, dBodyID body2, float x, float y, float z )
{
	sODEJoint *pJoint = new sODEJoint( );

	//modify prev and next pointers of any involved joints, make new joint the new head
	pJoint->pNextJoint = pODEJointList;
	if ( pODEJointList ) pODEJointList->pPrevJoint = pJoint;
	pODEJointList = pJoint;

	//create the ODE hinge joint
	pJoint->oJoint = dJointCreateBall( g_ODEWorld, 0 );
	dJointAttach( pJoint->oJoint, body1, body2 );
	dJointSetBallAnchor( pJoint->oJoint, x,y,z );

	pJoint->iID = id;

	return pJoint;
}

void sODEJoint::RemoveJoint( sODEJoint* pDelJoint )
{
	if ( !pDelJoint ) return ;

	if ( pDelJoint->pPrevJoint )
	{
		//assign the previous joint to skip over this joint in the list so it can be deleted
		pDelJoint->pPrevJoint->pNextJoint = pDelJoint->pNextJoint;
	}
	else
	{
		//joint must be at the head of the list, assign the head pointer to skip over it
		pODEJointList = pDelJoint->pNextJoint;
	}

	if ( pDelJoint->pNextJoint )
	{
		//assign the next joint to skip over this joint when pointing backwards
		pDelJoint->pNextJoint->pPrevJoint = pDelJoint->pPrevJoint;
	}

	delete pDelJoint;
}

sODEJoint* sODEJoint::GetJoint( int iID )
{
	sODEJoint* pJoint = pODEJointList;

	//cycle through the external joint list to find the id
	while ( pJoint )
	{
		if ( pJoint->iID == iID ) return pJoint;			
		pJoint = pJoint->pNextJoint;
	}

	return 0;
}

void sODEJoint::SetHingeLimit( float low, float high )
{
	dJointSetHingeParam( oJoint, dParamLoStop, low );
	dJointSetHingeParam( oJoint, dParamHiStop, high );
}

void sODEJoint::SetMovingHingeLimit( float startlow, float starthigh, float endlow, float endhigh, float time )
{
	if ( time <= 0 ) 
	{
		dJointSetHingeParam( oJoint, dParamLoStop, endlow );
		dJointSetHingeParam( oJoint, dParamHiStop, endhigh );
		return;
	}

	bMovingLimit = true;
	fLowStart = startlow;
	fHighStart = starthigh;
	fLowEnd = endlow;
	fHighEnd = endhigh;
	fMoveTime = time;
	fOrigMoveTime = time;

	dJointSetHingeParam( oJoint, dParamLoStop, startlow );
	dJointSetHingeParam( oJoint, dParamHiStop, starthigh );
}

void sODEJoint::SetHinge2Limit( float axis1low, float axis1high, float axis2low, float axis2high )
{
	dJointSetHinge2Param( oJoint, dParamLoStop, axis1low );
	dJointSetHinge2Param( oJoint, dParamHiStop, axis1high );

	dJointSetHinge2Param( oJoint, dParamLoStop2, axis2low );
	dJointSetHinge2Param( oJoint, dParamHiStop2, axis2high );
}

void sODEJoint::UpdateJoint( float fTimeDelta )
{
	if ( bMovingLimit )
	{
		fMoveTime -= fTimeDelta;
		if ( fMoveTime < 0 ) 
		{
			bMovingLimit = false;
			fMoveTime = 0;
		}

		float lerp = fMoveTime / fOrigMoveTime;

		float fNewLow = fLowStart*lerp + fLowEnd*(1-lerp);
		float fNewHigh = fHighStart*lerp + fHighEnd*(1-lerp);

		dJointSetHingeParam( oJoint, dParamLoStop, fNewLow );
		dJointSetHingeParam( oJoint, dParamHiStop, fNewHigh );
	}
}