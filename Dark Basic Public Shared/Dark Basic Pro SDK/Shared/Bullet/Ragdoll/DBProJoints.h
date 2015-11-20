
// BulletPhysicsWrapper for DarkBasic Professional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletDynamics\ConstraintSolver\btFixedConstraint.h"

struct DBProJoints
{
public:
	static void CreateLocalTransforms(btRigidBody *bodyA, btRigidBody *bodyB,  btTransform& jointTrans, btTransform& localATrans, btTransform& localBTrans);
	static int GetContactPosition(btRigidBody *bodyA, btRigidBody *bodyB,btTransform &outJointTrans);
	static btConeTwistConstraint* CreateTwistConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, btScalar swingSpan1, 
																							btScalar swingSpan2, btScalar twistSpan, bool disableCollisionsBetweenLinkedBodies);
	static btHingeConstraint* CreateHingeConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, btScalar minLimit, 
																							btScalar maxLimit, bool disableCollisionsBetweenLinkedBodies);
	static btPoint2PointConstraint* CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB);
	static btPoint2PointConstraint* CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btVector3 &pivotPosInA, btVector3 &pivotPosInB);
	static btSliderConstraint* CreateSliderConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB);
	static btFixedConstraint* DBProJoints::CreateFixedConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB, btScalar breakThreshold);
	static btGeneric6DofConstraint* DBProJoints::Create6DofConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB, btScalar breakThreshold);
	static btGeneric6DofConstraint* DBProJoints::Create6DofConstraint(btRigidBody *b_body, btTransform &localb, bool useLinearReferenceFrameB); 
	static int SetBreakingThreshold(btTypedConstraint* constraint,btScalar breakThreshold); 
}; 

