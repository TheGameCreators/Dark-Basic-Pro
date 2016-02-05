#include "stdafx.h"

const int Effector::classID='EFCT';

void Effector::onUpdate() //Must be called before other update functions are called
{
	if(!enabled) return;
	updateInit();
}


void Effector::updateParticles()
{
	if(!enabled) return;	
	if(canHandleParticles())
	{
		onUpdateParticles();
	}
}

void Effector::updateCloth()
{
	if(!enabled) return;	
	if(canHandleCloth())
	{
		onUpdateCloth();
	}
}

void Effector::updateRagdoll()
{
	if(!enabled) return;	
	if(canHandleRagdoll())
	{
		onUpdateRagdoll();
	}
}

void Effector::updateVehicle()
{
	if(!enabled) return;	
	if(canHandleVehicle())
	{
		onUpdateVehicle();
	}
}


ePhysError Effector::addEmitter(EmitterPtr& ptr)
{
#ifdef USINGMODULE_P
	if(emitterList.Exists(ptr->getID()))
	{
		if(emitterList.Get(ptr->getID())->isValid())
		{
			return PERR_ITEM_EXISTS;	
		}
		else
		{
			//We can safely remove it if it's declared as invalid
			removeEmitter(ptr->getID());
		}
	}

	emitterList.Add(ptr->getID(),ptr);
	emitterInfo.Add(ptr->getID(),new EffectorTargetInfo);
	needUpdate[0]=true;
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::removeEmitter(int id)
{
#ifdef USINGMODULE_P
	if(!emitterList.Exists(id)) return PERR_OBJECT_NOT_LINKED;
	
	emitterList.Delete(id);
	emitterInfo.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::addCloth(ClothPtr& ptr)
{
#ifdef USINGMODULE_C
	if(clothList.Exists(ptr->getID()))
	{
		if(clothList.Get(ptr->getID())->isValid())
		{
			return PERR_ITEM_EXISTS;	
		}
		else
		{
			//We can safely remove it if it's declared as invalid
			removeCloth(ptr->getID());
		}
	}

	clothList.Add(ptr->getID(),ptr);
	clothInfo.Add(ptr->getID(),new EffectorTargetInfo);
	needUpdate[1]=true;
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::removeCloth(int id)
{
#ifdef USINGMODULE_C
	if(!clothList.Exists(id)) return PERR_OBJECT_NOT_LINKED;
	
	clothList.Delete(id);
	clothInfo.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::addRagdoll(RagdollPtr& ptr)
{
#ifdef USINGMODULE_R
	if(ragdollList.Exists(ptr->getID()))
	{
		if(ragdollList.Get(ptr->getID())->isValid())
		{
			return PERR_ITEM_EXISTS;	
		}
		else
		{
			//We can safely remove it if it's declared as invalid
			removeRagdoll(ptr->getID());
		}
	}

	ragdollList.Add(ptr->getID(),ptr);
	ragdollInfo.Add(ptr->getID(),new EffectorTargetInfo);
	needUpdate[2]=true;
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::removeRagdoll(int id)
{
	if(!ragdollList.Exists(id)) return PERR_OBJECT_NOT_LINKED;
	
	ragdollList.Delete(id);
	ragdollInfo.Delete(id);

	return PERR_AOK;
}

ePhysError Effector::addVehicle(VehiclePtr& ptr)
{
#ifdef USINGMODULE_V
	if(vehicleList.Exists(ptr->getID()))
	{
		if(vehicleList.Get(ptr->getID())->isValid())
		{
			return PERR_ITEM_EXISTS;	
		}
		else
		{
			//We can safely remove it if it's declared as invalid
			removeVehicle(ptr->getID());
		}
	}

	vehicleList.Add(ptr->getID(),ptr);
	vehicleInfo.Add(ptr->getID(),new EffectorTargetInfo);
	needUpdate[3]=true;
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Effector::removeVehicle(int id)
{
#ifdef USINGMODULE_V
	if(!vehicleList.Exists(id)) return PERR_OBJECT_NOT_LINKED;
	
	vehicleList.Delete(id);
	vehicleInfo.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}



ePhysError Effector::getObjectInfo(int id,EffectorTargetInfo ** info)
{
#ifdef USINGMODULE_P
	if(emitterInfo.Exists(id) && emitterList.Get(id)->isValid()) 
	{
		*info=emitterInfo.Get(id);
		return PERR_AOK;
	}
#endif
#ifdef USINGMODULE_C
	if(clothInfo.Exists(id) && clothList.Get(id)->isValid())
	{
		*info=clothInfo.Get(id);
		return PERR_AOK;
	}
#endif
#ifdef USINGMODULE_V
	if(vehicleInfo.Exists(id) && vehicleList.Get(id)->isValid())
	{
		*info=vehicleInfo.Get(id);
		return PERR_AOK;
	}
#endif
#ifdef USINGMODULE_R
	if(ragdollInfo.Exists(id) && ragdollList.Get(id)->isValid())
	{
		*info=ragdollInfo.Get(id);
		return PERR_AOK;
	}
#endif
	if(!emitterInfo.Exists(id) && !clothInfo.Exists(id) && !vehicleInfo.Exists(id) && !ragdollInfo.Exists(id)) return PERR_OBJECT_NOT_LINKED;

	//so object must be marked as invalid
	return PERR_ITEM_DOESNT_EXIST;
}

ePhysError Effector::setInteraction(int id, float pc)
{
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->fade=pc*0.01f;
	if(pc>-0.00001f && pc<0.00001f) info->enabled=false;
	else info->enabled=true;
	
	needUpdate[0]=true;
	needUpdate[1]=true;
	needUpdate[2]=true;
	needUpdate[3]=true;

	return PERR_AOK;
}

ePhysError Effector::setFallOff(int id, bool state)
{
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->useFalloff=state;
	return PERR_AOK;
}

ePhysError Effector::setFallOffPerParticle(int id, bool state)
{
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->falloffPerParticle=state;
	return PERR_AOK;
}

ePhysError Effector::useSphereFalloff(int id)
{
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->useCylFalloff=false;
	return PERR_AOK;
}

ePhysError Effector::useCylinderFalloff(int id)
{
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->useCylFalloff=true;
	return PERR_AOK;
}

ePhysError Effector::setFalloffRadius(int id, float mnRad, float mxRad)
{
	if(mnRad<0 || mxRad<0 || mnRad>=mxRad) return PERR_BAD_PARAMETER;
	
	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->minRadSqr=mnRad*mnRad;
	info->normRadSqr=(mxRad-mnRad)*(mxRad-mnRad);
	info->normRadSqr=1.0f/info->normRadSqr;
	return PERR_AOK;
}

ePhysError Effector::setFalloffHeight(int id, float mnHgt, float mxHgt)
{
	if(mnHgt<0 || mxHgt<0 || mnHgt>=mxHgt) return PERR_BAD_PARAMETER;

	EffectorTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;	

	info->minHgt=mnHgt;
	info->normHgt=1.0f/(mxHgt-mnHgt);
	return PERR_AOK;
}

