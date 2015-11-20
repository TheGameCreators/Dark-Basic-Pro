
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

///#include "StdAfx.h"
#include "DBProJoints.h"

// externs to globals elsewhere
extern btDiscreteDynamicsWorld* g_dynamicsWorld;

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	btManifoldPoint m_cp;
	bool m_connected;
	btScalar m_margin;
	MyContactResultCallback() :m_connected(false),m_margin(0.05)
	{
	}
	virtual   btScalar   addSingleResult(btManifoldPoint& cp,const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
	{
		if (cp.getDistance()<=m_margin)
		{
			m_connected = true;
			m_cp = cp;
		}
		return 1.f;
	}
};

void DBProJoints::CreateLocalTransforms(btRigidBody *bodyA, btRigidBody *bodyB, btTransform& jointTrans, btTransform& localATrans, btTransform& localBTrans )
{
	//Get world Transform for rigid bodies 
	btTransform bodyAWorldTrans, bodyBWorldTrans; 
	bodyAWorldTrans = bodyA->getWorldTransform();
	bodyBWorldTrans = bodyB->getWorldTransform();

	localATrans.setIdentity();
	localBTrans.setIdentity();

	localATrans = bodyAWorldTrans.inverse() * jointTrans;
	localBTrans = bodyBWorldTrans.inverse() * jointTrans;
}


int DBProJoints::GetContactPosition(btRigidBody *bodyA, btRigidBody *bodyB, btTransform &outJointTrans)
{
	MyContactResultCallback result;

	btCollisionObject collObjectA, collObjectB;

	collObjectA.setWorldTransform(bodyA->getWorldTransform());
	collObjectA.setCollisionShape(bodyA->getCollisionShape());

	collObjectB.setWorldTransform(bodyB->getWorldTransform());
	collObjectB.setCollisionShape(bodyB->getCollisionShape());

	g_dynamicsWorld->contactPairTest(&collObjectA, &collObjectB, result);

	if(result.m_connected)
	{
		btVector3 contactPosWorld = result.m_cp.getPositionWorldOnA();
		outJointTrans.setIdentity();
		outJointTrans.setOrigin(contactPosWorld);
		return 0;
	}
	return -1;
}

btConeTwistConstraint* DBProJoints::CreateTwistConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, btScalar swingSpan1, btScalar swingSpan2, btScalar twistSpan, bool disableCollisionsBetweenLinkedBodies)
{
	btConeTwistConstraint *twistC = new btConeTwistConstraint(*a_body, *b_body, localA, localB);
	twistC->setLimit(swingSpan1, swingSpan2, twistSpan);
	g_dynamicsWorld->addConstraint(twistC, disableCollisionsBetweenLinkedBodies);
	return twistC;
	//return DynamicsWorldArray[0]->m_dynamicsWorld->getNumConstraints()-1;
}

btHingeConstraint* DBProJoints::CreateHingeConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, btScalar minLimit, btScalar maxLimit, bool disableCollisionsBetweenLinkedBodies)
{		
	btHingeConstraint *hingeC = new btHingeConstraint(*a_body, *b_body, localA, localB);			
	hingeC->setLimit(minLimit, maxLimit);
	g_dynamicsWorld->addConstraint(hingeC, disableCollisionsBetweenLinkedBodies);
	return hingeC;
	//return DynamicsWorldArray[0]->m_dynamicsWorld->getNumConstraints()-1;
}

btPoint2PointConstraint* DBProJoints::CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB)
{		
	btPoint2PointConstraint *pointC = new btPoint2PointConstraint(*a_body, *b_body, localA.getOrigin(), localB.getOrigin());
	//TODO: set limits
	g_dynamicsWorld->addConstraint(pointC, true);
	return pointC;
	//return DynamicsWorldArray[0]->m_dynamicsWorld->getNumConstraints()-1;
}

btPoint2PointConstraint* DBProJoints::CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btVector3 &pivotPosInA, btVector3 &pivotPosInB)
{		
	btPoint2PointConstraint *pointC = new btPoint2PointConstraint(*a_body, *b_body, pivotPosInA, pivotPosInB);
	//TODO: set limits
	g_dynamicsWorld->addConstraint(pointC, true);	
	return pointC;
	//return DynamicsWorldArray[0]->m_dynamicsWorld->getNumConstraints()-1;
}

btSliderConstraint* DBProJoints::CreateSliderConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB)
{
	btSliderConstraint *sliderC = new btSliderConstraint(*a_body, *b_body, localA, localB, true);
	//TODO: set limits
	g_dynamicsWorld->addConstraint(sliderC, true);	
	return sliderC;
	//return DynamicsWorldArray[0]->m_dynamicsWorld->getNumConstraints()-1;
}

btFixedConstraint* DBProJoints::CreateFixedConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB, btScalar breakThreshold)
{	
	btFixedConstraint* fixed = new btFixedConstraint(*a_body,*b_body,localA,localB);
	DBProJoints::SetBreakingThreshold(fixed, breakThreshold);
	//FHB: Default is 10 ,pass a -1. Demo was set to 30. Need to investigate this so no slow down is caused.
	fixed->setOverrideNumSolverIterations(-1);
	g_dynamicsWorld->addConstraint(fixed,true);
	return fixed;
}


btGeneric6DofConstraint* DBProJoints::Create6DofConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB, btScalar breakThreshold)
{
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*a_body,*b_body,localA,localB,true);
	DBProJoints::SetBreakingThreshold(dof6, breakThreshold);
	g_dynamicsWorld->addConstraint(dof6,true);
	return dof6;
}

btGeneric6DofConstraint* DBProJoints::Create6DofConstraint(btRigidBody *b_body, btTransform &localb, bool useLinearReferenceFrameB)
{
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*b_body,localb, useLinearReferenceFrameB);
	g_dynamicsWorld->addConstraint(dof6,true);
	return dof6;
}

int DBProJoints::SetBreakingThreshold(btTypedConstraint* constraint,btScalar breakThreshold)
{
	if( !constraint->getRigidBodyA().isStaticOrKinematicObject() && !constraint->getRigidBodyB().isStaticOrKinematicObject()
		&& constraint->getRigidBodyA().getInvMass() != 0 && constraint->getRigidBodyB().getInvMass() != 0)
	{
		btScalar totalMass = 1.0/constraint->getRigidBodyA().getInvMass() + 1.0/constraint->getRigidBodyB().getInvMass();
		constraint->setBreakingImpulseThreshold(breakThreshold*totalMass);
		return 0;
	}
	return -1;
}





