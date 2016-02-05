#include "stdafx.h"


DLLEXPORT void deleteCollider(int id)
{
	ALERT_ON_PERR(physics->deleteCollider(id),PCMD_DELETE_COLLIDER);
}

DLLEXPORT void bindColliderToObject(int colId, int objId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_BIND_COLLIDER_TO_OBJECT);

	if(objId<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_BIND_COLLIDER_TO_OBJECT);

	EmitterPtr emt;
	if(physics->getEmitter(objId,emt)==PERR_AOK)
	{
		if(!col->canHandleParticles()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_PARTICLE,PCMD_BIND_COLLIDER_TO_OBJECT);		
		RETURN_ON_PERR(col->addEmitter(emt),PCMD_BIND_COLLIDER_TO_OBJECT);
		return;
	}

	ClothPtr clt;
	if(physics->getCloth(objId,clt)==PERR_AOK)
	{
		if(!col->canHandleCloth()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_CLOTH,PCMD_BIND_COLLIDER_TO_OBJECT);		
		RETURN_ON_PERR(col->addCloth(clt),PCMD_BIND_COLLIDER_TO_OBJECT);
		return;
	}

	RagdollPtr rag;
	if(physics->getRagdoll(objId,rag)==PERR_AOK)
	{
		if(!col->canHandleRagdoll()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_RAGDOLL,PCMD_BIND_COLLIDER_TO_OBJECT);		
		RETURN_ON_PERR(col->addRagdoll(rag),PCMD_BIND_COLLIDER_TO_OBJECT);
		return;
	}

	VehiclePtr veh;
	if(physics->getVehicle(objId,veh)==PERR_AOK)
	{
		if(!col->canHandleVehicle()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_VEHICLE,PCMD_BIND_COLLIDER_TO_OBJECT);		
		RETURN_ON_PERR(col->addVehicle(veh),PCMD_BIND_COLLIDER_TO_OBJECT);
		return;
	}

	RETURN_ON_PERR(PERR_WRONG_TYPE,PCMD_BIND_COLLIDER_TO_OBJECT);	
}

DLLEXPORT void unbindColliderFromObject(int colId, int objId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_UNBIND_COLLIDER_FROM_OBJECT);
	if(objId<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_UNBIND_COLLIDER_FROM_OBJECT);

	if(col->removeEmitter(objId)==PERR_AOK) return;	
	if(col->removeCloth(objId)==PERR_AOK) return;
	if(col->removeRagdoll(objId)==PERR_AOK) return;
	if(col->removeVehicle(objId)==PERR_AOK) return;

	RETURN_ON_PERR(PERR_OBJECT_NOT_LINKED,PCMD_UNBIND_COLLIDER_FROM_OBJECT);	
}

/*
DLLEXPORT void bindColliderToEmitter(int colId, int parId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_BIND_COLLIDER_TO_EMITTER);

	if(!col->canHandleParticles()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_PARTICLE,PCMD_BIND_COLLIDER_TO_EMITTER);

	EmitterPtr emt;
	RETURN_ON_PERR(physics->getEmitter(parId,emt),PCMD_BIND_COLLIDER_TO_EMITTER);

	RETURN_ON_PERR(col->addEmitter(emt),PCMD_BIND_COLLIDER_TO_EMITTER);
}

DLLEXPORT void unbindColliderFromEmitter(int colId, int parId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_UNBIND_COLLIDER_FROM_EMITTER);

	RETURN_ON_PERR(col->removeEmitter(parId),PCMD_UNBIND_COLLIDER_FROM_EMITTER);
}

DLLEXPORT void bindColliderToCloth(int colId, int clothId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_BIND_COLLIDER_TO_CLOTH);

	if(!col->canHandleCloth()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_CLOTH,PCMD_BIND_COLLIDER_TO_CLOTH);

	ClothPtr clt;
	RETURN_ON_PERR(physics->getCloth(clothId,clt),PCMD_BIND_COLLIDER_TO_CLOTH);

	RETURN_ON_PERR(col->addCloth(clt),PCMD_BIND_COLLIDER_TO_CLOTH);
}

DLLEXPORT void unbindColliderFromCloth(int colId, int clothId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_UNBIND_COLLIDER_FROM_CLOTH);

	RETURN_ON_PERR(col->removeCloth(clothId),PCMD_UNBIND_COLLIDER_FROM_CLOTH);
}

DLLEXPORT void bindColliderToRagdoll(int colId, int ragdollId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_BIND_COLLIDER_TO_RAGDOLL);

	if(!col->canHandleCloth()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_CLOTH,PCMD_BIND_COLLIDER_TO_RAGDOLL);

	RagdollPtr rag;
	RETURN_ON_PERR(physics->getRagdoll(ragdollId,rag),PCMD_BIND_COLLIDER_TO_RAGDOLL);

	RETURN_ON_PERR(col->addRagdoll(rag),PCMD_BIND_COLLIDER_TO_RAGDOLL);
}

DLLEXPORT void unbindColliderFromRagdoll(int colId, int ragdollId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_UNBIND_COLLIDER_FROM_RAGDOLL);

	RETURN_ON_PERR(col->removeRagdoll(ragdollId),PCMD_UNBIND_COLLIDER_FROM_RAGDOLL);
}

DLLEXPORT void bindColliderToVehicle(int colId, int vehicleId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_BIND_COLLIDER_TO_VEHICLE);

	if(!col->canHandleCloth()) RETURN_ON_PERR(PERR_COL_INCOMPATIBLE_CLOTH,PCMD_BIND_COLLIDER_TO_VEHICLE);

	VehiclePtr vhc;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,vhc),PCMD_BIND_COLLIDER_TO_VEHICLE);

	RETURN_ON_PERR(col->addVehicle(vhc),PCMD_BIND_COLLIDER_TO_VEHICLE);
}

DLLEXPORT void unbindColliderFromVehicle(int colId, int vehicleId)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_UNBIND_COLLIDER_FROM_VEHICLE);

	RETURN_ON_PERR(col->removeVehicle(vehicleId),PCMD_UNBIND_COLLIDER_FROM_VEHICLE);
}
*/
DLLEXPORT void setColliderInteraction(int colId, int objectId, DWORD state)
{
	ColliderPtr col;
	RETURN_ON_PERR(physics->getCollider(colId,col),PCMD_SET_COLLIDER_INTERACTION);

	if(state) col->setInteraction(objectId,true);
	else col->setInteraction(objectId,false);	
}

DLLEXPORT void enableCollider(int id,DWORD state)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(id,tmp),PCMD_ENABLE_COLLIDER);
	if(state) tmp->enable(true);
	else tmp->enable(false);
}

DLLEXPORT DWORD isColliderEnabled(int id)
{
	ColliderPtr tmp;
	RETURNVALUE_ON_PERR(physics->getCollider(id,tmp),PCMD_IS_COLLIDER_ENABLED,0);
	if(tmp->isEnabled()) return 1;
	return 0;	
}

DLLEXPORT void setColliderBounce(int colId, int objectId, float bnc)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_BOUNCE);	

	RETURN_ON_PERR(tmp->setBounce(objectId,bnc),PCMD_SET_COLLIDER_BOUNCE);
}

DLLEXPORT void setColliderFriction(int colId, int objectId, float frc)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_FRICTION);	

	RETURN_ON_PERR(tmp->setFriction(objectId,frc),PCMD_SET_COLLIDER_FRICTION);
}

DLLEXPORT void setColliderInvert(int colId, int objectId, DWORD state)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_INVERT);	

	if(state) 
	{
		RETURN_ON_PERR(tmp->setInvert(objectId,true),PCMD_SET_COLLIDER_INVERT);
	}
	else 
	{
		RETURN_ON_PERR(tmp->setInvert(objectId,false),PCMD_SET_COLLIDER_INVERT);
	}
}

DLLEXPORT void setColliderKiller(int colId, int objectId, DWORD state)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_KILLER);	

	if(state)
	{
		RETURN_ON_PERR(tmp->setKiller(objectId,true),PCMD_SET_COLLIDER_KILLER);
	}
	else
	{
		RETURN_ON_PERR(tmp->setKiller(objectId,false),PCMD_SET_COLLIDER_KILLER);
	}
}

DLLEXPORT void setColliderUseColor(int colId, int objectId, DWORD state)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_USE_COLOR);	

	if(state)
	{
		RETURN_ON_PERR(tmp->setUseColor(objectId,true),PCMD_SET_COLLIDER_USE_COLOR);
	}
	else
	{
		RETURN_ON_PERR(tmp->setUseColor(objectId,false),PCMD_SET_COLLIDER_USE_COLOR);
	}
}

DLLEXPORT void setColliderColor(int colId, int objectId, int R, int G, int B, int A)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_COLOR);	

	bool result;
	result |= R<=255 && R>=0;
	result |= G<=255 && G>=0;
	result |= B<=255 && B>=0;
	result |= A<=255 && A>=0;
	if(!result)	RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_SET_COLLIDER_COLOR);	

	unsigned int color = B + (G<<8) + (R<<16) + (A<<24);

	RETURN_ON_PERR(tmp->setColor(objectId,color),PCMD_SET_COLLIDER_COLOR);
}

DLLEXPORT void setColliderResolve(int colId, int objectId, DWORD state)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(colId,tmp),PCMD_SET_COLLIDER_RESOLVE);	

	if(state)
	{
		RETURN_ON_PERR(tmp->setResolve(objectId,true),PCMD_SET_COLLIDER_RESOLVE);
	}
	else
	{
		RETURN_ON_PERR(tmp->setResolve(objectId,false),PCMD_SET_COLLIDER_RESOLVE);
	}
}
