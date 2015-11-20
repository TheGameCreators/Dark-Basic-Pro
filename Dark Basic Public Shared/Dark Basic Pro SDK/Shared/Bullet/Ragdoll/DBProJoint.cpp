///#include "StdAfx.h"
#include "DBProJoint.h"

// externs to globals elsewhere
extern btDiscreteDynamicsWorld* g_dynamicsWorld;
#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

DBProJoint::DBProJoint(int jointID, btTypedConstraint* constraint) : BaseItem(jointID)
{
	m_constraint = constraint;
}

DBProJoint::~DBProJoint()
{
	if(m_constraint != NULL)
	{
		g_dynamicsWorld->removeConstraint(m_constraint);
		SAFE_DELETE(m_constraint);	
	}
}

btTypedConstraint* DBProJoint::GetConstraint()
{
	return m_constraint;
}

//FHB: TODO: handel all types of joints
btTransform* DBProJoint::GetFrameOffsetA()
{
	btTypedConstraintType type = m_constraint->getConstraintType();
	if(type == btTypedConstraintType::D6_CONSTRAINT_TYPE)
	{
		btGeneric6DofConstraint* pickJoint = static_cast<btGeneric6DofConstraint*>(m_constraint);
		return &pickJoint->getFrameOffsetA();
	}
	return NULL;
}

//FHB: TODO: handel all types of joints
btTransform* DBProJoint::GetFrameOffsetB()  
{
	btTypedConstraintType type = m_constraint->getConstraintType();
	if(type == btTypedConstraintType::D6_CONSTRAINT_TYPE)
	{
		btGeneric6DofConstraint* pickJoint = static_cast<btGeneric6DofConstraint*>(m_constraint);
		return &pickJoint->getFrameOffsetB();
	}
	return NULL;
} 