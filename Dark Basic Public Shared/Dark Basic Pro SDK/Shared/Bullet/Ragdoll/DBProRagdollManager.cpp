
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

///#include "StdAfx.h"
#include "DBProRagdollManager.h"
#include "DBProRagDoll.h"
#include "DBPro.hpp"

DBProRagdollManager* ragdollManager = NULL;// now in constructor new DBProRagdollManager();

//-------------------------------------

DBProRagdollManager::DBProRagdollManager()
{
}

DBProRagdollManager::~DBProRagdollManager()
{
}

void DBProRagdollManager::AddRagdoll(DBProRagDoll* ragdoll)
{
	AddItem(ragdoll);
}

void DBProRagdollManager::DeleteRagdoll(int ragdollID)
{
	DBProRagDoll* ragdoll = GetRagdoll(ragdollID);
	if(ragdoll)
	{
		ragdoll->ResetObjectParametersForAnimation();
		ragdoll->ResetObjectParametersForCulling();
		RemoveItem(ragdollID);
	}
}

DBProRagDoll* DBProRagdollManager::GetRagdoll(int ragdollID)
{
	return (DBProRagDoll*)GetItem(ragdollID);
}

int DBProRagdollManager::GetIDFromBoneObject(int objectID)
{
	for(int i = 0; i < m_data.size(); i++)
	{
		DBProRagDoll* ragdoll = (DBProRagDoll*)m_data[i];
		if(ragdoll->IsBoneObject(objectID))
		{
			return ragdoll->GetID();
		}
	}
	return -1;
}

void DBProRagdollManager::Update()
{
	for(int i = 0; i < m_data.size(); i++)
	{	
		DBProRagDoll* ragdoll = ((DBProRagDoll*)m_data[i]);
		if(ragdoll->IsSleeping()==false)
		{
			ragdoll->Update();
		}
		else
		{
			ragdoll->ResetObjectParametersForCulling();
		}
	}
}

void DBProRagdollManager::AssertRagdollExist(int ragdollID, LPCSTR message, bool bExist /*= true*/) 
{
	if(bExist && ragdollManager->GetRagdoll(ragdollID)==NULL)
	{
		DBPro::ReportError(message,"Bullet Physics Wrapper");
	}
	else if(!bExist && ragdollManager->GetRagdoll(ragdollID)!=NULL)
	{
		DBPro::ReportError(message,"Bullet Physics Wrapper");
	}
}



