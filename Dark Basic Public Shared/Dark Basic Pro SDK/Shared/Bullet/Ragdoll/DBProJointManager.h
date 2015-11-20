
// BulletPhysicsWrapper for DarkBasic Professional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com


#pragma once

#include "DBProJoint.h"
#include "Windows.h"
#include "BaseItemManager.h"
#include "btBulletDynamicsCommon.h"

//bool btTypeConstraintSortPredicate(const DBProJoint& d1, const DBProJoint& d2)
//{
//  return d1.GetID() < d2.GetID();
//}

class DBProJointManager : public BaseItemManager
{
private: 
	//btAlignedObjectArray<DBProJoint*> m_dbproJoints;
	//int findBinarySearch(int jointID);

public:
	DBProJointManager(void);
	virtual ~DBProJointManager(void);
	//
	int AddJoint(btTypedConstraint* constraint);
	void DeleteJoint(int jointID);
	int GetNumberOfJoints();
	DBProJoint* GetJoint(int jointID);
	static void AssertValidJointID(int jointID, LPCSTR message);
};

extern DBProJointManager* jointManager;


