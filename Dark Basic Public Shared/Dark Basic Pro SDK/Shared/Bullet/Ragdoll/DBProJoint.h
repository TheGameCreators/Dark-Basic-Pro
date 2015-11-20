#pragma once

#include "BaseItem.h"
#include "btBulletDynamicsCommon.h"

class DBProJoint: public BaseItem
{
public:
	//DBProJoint::DBProJoint();
	DBProJoint(int jointID, btTypedConstraint* constraint);
	virtual ~DBProJoint();

	btTypedConstraint* GetConstraint();
	btTransform* GetFrameOffsetA();  
	btTransform* GetFrameOffsetB();   

private:
	btTypedConstraint* m_constraint;
};

