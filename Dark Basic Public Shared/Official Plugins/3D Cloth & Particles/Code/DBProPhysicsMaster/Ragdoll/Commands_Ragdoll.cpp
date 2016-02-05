#include "stdafx.h"

DLLEXPORT void freezeRagdoll(int id, DWORD state)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_FREEZE_RAGDOLL);

	if(state) tmp->Freeze();
	else tmp->unFreeze();
}

DLLEXPORT void fixRagdollPointToObject(int id, int pntNo, int objectID)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_FIX_RAGDOLL_POINT_TO_OBJECT);

	RETURN_ON_PERR(tmp->fixPointToObject(pntNo,objectID,0),PCMD_FIX_RAGDOLL_POINT_TO_OBJECT);
}

DLLEXPORT void fixRagdollPointToObject(int id, int pntNo, int objectID, float px, float py, float pz)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_FIX_RAGDOLL_POINT_TO_OBJECT);

	Vector3 pos(px,py,pz);
	RETURN_ON_PERR(tmp->fixPointToObject(pntNo,objectID,&pos),PCMD_FIX_RAGDOLL_POINT_TO_OBJECT);
}

DLLEXPORT void setRagdollObjectOffset(int id, int partNo, float ox, float oy, float oz)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_SET_RAGDOLL_OBJECT_OFFSET);

	RETURN_ON_PERR(tmp->setRagdollObjectOffset(partNo,ox,oy,oz),PCMD_SET_RAGDOLL_OBJECT_OFFSET);
}

DLLEXPORT void freeAllRagdollPoints(int id)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_FREE_ALL_RAGDOLL_POINTS);

	tmp->freeAllPoints();
}

DLLEXPORT void drawRagdollDebugLines(int ragdollId, DWORD state)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(ragdollId,tmp),PCMD_DRAW_RAGDOLL_DEBUG_LINES);

	tmp->setDebugLines(state?true:false);
}

DLLEXPORT void addRagdollForce(int id, int pntNo, float fx, float fy, float fz)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_ADD_RAGDOLL_FORCE);

	RETURN_ON_PERR(tmp->applyForce(pntNo,fx,fy,fz),PCMD_ADD_RAGDOLL_FORCE);
}

DLLEXPORT void addRagdollImpulse(int id, int pntNo, float ix, float iy, float iz)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_ADD_RAGDOLL_IMPULSE);

	RETURN_ON_PERR(tmp->applyImpulse(pntNo,ix,iy,iz),PCMD_ADD_RAGDOLL_IMPULSE);
}
