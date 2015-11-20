#pragma once

#include "Windows.h"
#include "DBProRagDoll.h"
#include "BaseItemManager.h"

class DBProRagdollManager : public BaseItemManager
{
public:
	DBProRagdollManager();
	~DBProRagdollManager();

	void AddRagdoll(DBProRagDoll* ragdoll);
	void DeleteRagdoll(int ragdollID);
	DBProRagDoll* GetRagdoll(int ragdollID);
	void Update(); 
	int GetIDFromBoneObject(int objectID);
	static void AssertRagdollExist(int ragdollID, LPCSTR message, bool bExist = true); 

private:

};

extern DBProRagdollManager* ragdollManager;
