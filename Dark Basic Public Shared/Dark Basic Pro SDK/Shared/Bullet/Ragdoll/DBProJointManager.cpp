
// BulletPhysicsWrapper for DarkBasic Professional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

///#include "StdAfx.h"
#include "DBProJointManager.h"
#include "DBPro.hpp"

DBProJointManager* jointManager = NULL;//moved to Bullet construct/destruct new DBProJointManager();

DBProJointManager::DBProJointManager() : BaseItemManager()
{
}

DBProJointManager::~DBProJointManager()
{
}

void DBProJointManager::AssertValidJointID(int jointID, LPCSTR message)
{
	if(jointManager->GetItem(jointID)==NULL)
	{
		///DBPro::ReportError(message,"Bullet Physics Wrapper");
		MessageBox(NULL,message,"Bullet Physics Wrapper",MB_OK );
	}
}

int DBProJointManager::GetNumberOfJoints()
{
	return m_data.size();
}

DBProJoint* DBProJointManager::GetJoint(int jointID)
{
	return (DBProJoint*)GetItem(jointID);
}

int DBProJointManager::AddJoint(btTypedConstraint* constraint)
{
	int jointID = GetFreeID();
	constraint->setUserConstraintId(jointID);
	AddItem(new DBProJoint(jointID, constraint));
	return jointID;
}

void DBProJointManager::DeleteJoint(int jointID)
{
	if(!RemoveItem(jointID))
	{
		//sprintf_s(data, 255, "Failed to Remove Joint: %d", jointID);
		//DBPro::PrintConsole(data);
		//DBPro::PrintConsole();
	}
}