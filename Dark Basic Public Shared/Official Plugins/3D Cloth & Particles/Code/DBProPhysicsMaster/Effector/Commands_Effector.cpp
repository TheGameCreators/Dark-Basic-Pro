#include "stdafx.h"

DLLEXPORT void deleteEffector(int id)
{
	ALERT_ON_PERR(physics->deleteEffector(id),PCMD_DELETE_EFFECTOR);
}


DLLEXPORT void bindEffectorToObject(int effId, int objId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_BIND_EFFECTOR_TO_OBJECT);

	if(objId<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_BIND_EFFECTOR_TO_OBJECT);

	EmitterPtr emt;
	if(physics->getEmitter(objId,emt)==PERR_AOK)
	{
		if(!eff->canHandleParticles()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_PARTICLE,PCMD_BIND_EFFECTOR_TO_OBJECT);		
		RETURN_ON_PERR(eff->addEmitter(emt),PCMD_BIND_EFFECTOR_TO_OBJECT);
		return;
	}

	ClothPtr clt;
	if(physics->getCloth(objId,clt)==PERR_AOK)
	{
		if(!eff->canHandleCloth()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_CLOTH,PCMD_BIND_EFFECTOR_TO_OBJECT);		
		RETURN_ON_PERR(eff->addCloth(clt),PCMD_BIND_EFFECTOR_TO_OBJECT);
		return;
	}

	RagdollPtr rag;
	if(physics->getRagdoll(objId,rag)==PERR_AOK)
	{
		if(!eff->canHandleRagdoll()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_RAGDOLL,PCMD_BIND_EFFECTOR_TO_OBJECT);		
		RETURN_ON_PERR(eff->addRagdoll(rag),PCMD_BIND_EFFECTOR_TO_OBJECT);
		return;
	}

	VehiclePtr veh;
	if(physics->getVehicle(objId,veh)==PERR_AOK)
	{
		if(!eff->canHandleVehicle()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_VEHICLE,PCMD_BIND_EFFECTOR_TO_OBJECT);		
		RETURN_ON_PERR(eff->addVehicle(veh),PCMD_BIND_EFFECTOR_TO_OBJECT);
		return;
	}

	RETURN_ON_PERR(PERR_WRONG_TYPE,PCMD_BIND_EFFECTOR_TO_OBJECT);	
}

DLLEXPORT void unbindEffectorFromObject(int effId, int objId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_UNBIND_EFFECTOR_FROM_OBJECT);
	if(objId<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_UNBIND_EFFECTOR_FROM_OBJECT);

	if(eff->removeEmitter(objId)==PERR_AOK) return;	
	if(eff->removeCloth(objId)==PERR_AOK) return;
	if(eff->removeRagdoll(objId)==PERR_AOK) return;
	if(eff->removeVehicle(objId)==PERR_AOK) return;

	RETURN_ON_PERR(PERR_OBJECT_NOT_LINKED,PCMD_UNBIND_EFFECTOR_FROM_OBJECT);	
}

/*
DLLEXPORT void bindEffectorToEmitter(int effId, int parId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_BIND_EFFECTOR_TO_EMITTER);

	if(!eff->canHandleParticles()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_PARTICLE,PCMD_BIND_EFFECTOR_TO_EMITTER);

	EmitterPtr emt;
	RETURN_ON_PERR(physics->getEmitter(parId,emt),PCMD_BIND_EFFECTOR_TO_EMITTER);

	RETURN_ON_PERR(eff->addEmitter(emt),PCMD_BIND_EFFECTOR_TO_EMITTER);
}

DLLEXPORT void unbindEffectorFromEmitter(int effId, int parId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_UNBIND_EFFECTOR_FROM_EMITTER);

	RETURN_ON_PERR(eff->removeEmitter(parId),PCMD_UNBIND_EFFECTOR_FROM_EMITTER);
}

DLLEXPORT void bindEffectorToCloth(int effId, int clothId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_BIND_EFFECTOR_TO_CLOTH);

	if(!eff->canHandleCloth()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_CLOTH,PCMD_BIND_EFFECTOR_TO_CLOTH);

	ClothPtr clt;
	RETURN_ON_PERR(physics->getCloth(clothId,clt),PCMD_BIND_EFFECTOR_TO_CLOTH);

	RETURN_ON_PERR(eff->addCloth(clt),PCMD_BIND_EFFECTOR_TO_CLOTH);
}

DLLEXPORT void unbindEffectorFromCloth(int effId, int clothId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_UNBIND_EFFECTOR_FROM_CLOTH);
	
	if(!eff->canHandleCloth()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_CLOTH,PCMD_UNBIND_EFFECTOR_FROM_CLOTH);

	RETURN_ON_PERR(eff->removeCloth(clothId),PCMD_UNBIND_EFFECTOR_FROM_CLOTH);
}

DLLEXPORT void bindEffectorToRagdoll(int effId, int ragdollId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_BIND_EFFECTOR_TO_RAGDOLL);

	if(!eff->canHandleRagdoll()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_RAGDOLL,PCMD_BIND_EFFECTOR_TO_RAGDOLL);

	RagdollPtr rag;
	RETURN_ON_PERR(physics->getRagdoll(ragdollId,rag),PCMD_BIND_EFFECTOR_TO_RAGDOLL);

	RETURN_ON_PERR(eff->addRagdoll(rag),PCMD_BIND_EFFECTOR_TO_RAGDOLL);
}

DLLEXPORT void unbindEffectorFromRagdoll(int effId, int ragdollId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_UNBIND_EFFECTOR_FROM_RAGDOLL);
	
	if(!eff->canHandleRagdoll()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_RAGDOLL,PCMD_UNBIND_EFFECTOR_FROM_RAGDOLL);

	RETURN_ON_PERR(eff->removeRagdoll(ragdollId),PCMD_UNBIND_EFFECTOR_FROM_RAGDOLL);
}

DLLEXPORT void bindEffectorToVehicle(int effId, int vehicleId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_BIND_EFFECTOR_TO_VEHICLE);

	if(!eff->canHandleVehicle()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_VEHICLE,PCMD_BIND_EFFECTOR_TO_VEHICLE);

	VehiclePtr vhc;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,vhc),PCMD_BIND_EFFECTOR_TO_VEHICLE);

	RETURN_ON_PERR(eff->addVehicle(vhc),PCMD_BIND_EFFECTOR_TO_VEHICLE);
}

DLLEXPORT void unbindEffectorFromVehicle(int effId, int vehicleId)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_UNBIND_EFFECTOR_FROM_VEHICLE);
	
	if(!eff->canHandleVehicle()) RETURN_ON_PERR(PERR_EFF_INCOMPATIBLE_VEHICLE,PCMD_UNBIND_EFFECTOR_FROM_VEHICLE);

	RETURN_ON_PERR(eff->removeVehicle(vehicleId),PCMD_UNBIND_EFFECTOR_FROM_VEHICLE);
}
*/

DLLEXPORT void setEffectorInteraction(int effId, int objectId, float pc)
{
	EffectorPtr eff;
	RETURN_ON_PERR(physics->getEffector(effId,eff),PCMD_SET_EFFECTOR_INTERACTION);

	RETURN_ON_PERR(eff->setInteraction(objectId,pc),PCMD_SET_EFFECTOR_INTERACTION);
}

DLLEXPORT void enableEffector(int id,DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_ENABLE_EFFECTOR);
	if(state) tmp->enable(true);
	else tmp->enable(false);
}


DLLEXPORT DWORD isEffectorEnabled(int id)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_IS_EFFECTOR_ENABLED,0);
	if(tmp->isEnabled()) return 1;
	return 0;	
}

DLLEXPORT void setEffectorFallOff(int effId, int objectId, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_SET_EFFECTOR_FALLOFF);	
	
	if(state) 
	{
		RETURN_ON_PERR(tmp->setFallOff(objectId,true),PCMD_SET_EFFECTOR_FALLOFF);
	}
	else 
	{
		RETURN_ON_PERR(tmp->setFallOff(objectId,false),PCMD_SET_EFFECTOR_FALLOFF);
	}
}

DLLEXPORT void setEffectorFallOffPerParticle(int effId, int objectId, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_SET_EFFECTOR_FALLOFF_PERPARTICLE);	
	
	if(state) 
	{
		RETURN_ON_PERR(tmp->setFallOffPerParticle(objectId,true),PCMD_SET_EFFECTOR_FALLOFF_PERPARTICLE);
	}
	else
	{
		RETURN_ON_PERR(tmp->setFallOffPerParticle(objectId,false),PCMD_SET_EFFECTOR_FALLOFF_PERPARTICLE);
	}
}

DLLEXPORT void useEffectorSphereFalloff(int effId, int objectId)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_USE_EFFECTOR_SPHERE_FALLOFF);	

	RETURN_ON_PERR(tmp->useSphereFalloff(objectId),PCMD_USE_EFFECTOR_SPHERE_FALLOFF);
}

DLLEXPORT void useEffectorCylinderFalloff(int effId, int objectId)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_USE_EFFECTOR_CYLINDER_FALLOFF);	

	RETURN_ON_PERR(tmp->useCylinderFalloff(objectId),PCMD_USE_EFFECTOR_CYLINDER_FALLOFF);
}

DLLEXPORT void setEffectorFalloffRadius(int effId, int objectId, float mnRad, float mxRad)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_SET_EFFECTOR_FALLOFF_RADIUS);	

	RETURN_ON_PERR(tmp->setFalloffRadius(objectId,mnRad,mxRad),PCMD_SET_EFFECTOR_FALLOFF_RADIUS);
}

DLLEXPORT void setEffectorFalloffHeight(int effId, int objectId, float mnHgt, float mxHgt)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(effId,tmp),PCMD_SET_EFFECTOR_FALLOFF_HEIGHT);	

	RETURN_ON_PERR(tmp->setFalloffHeight(objectId,mnHgt,mxHgt),PCMD_SET_EFFECTOR_FALLOFF_HEIGHT);
}
