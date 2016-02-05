#include "stdafx.h"

const int Collider::classID='COLL';

void Collider::onUpdate() //Must be called before other update functions are called
{
	if(!enabled) return;
	updateInit();
}


void Collider::updateParticles()
{
	if(!enabled) return;	
	if(canHandleParticles())
	{
		onUpdateParticles();
	}
}

void Collider::updateCloth()
{
	if(!enabled) return;	
	if(canHandleCloth())
	{
		onUpdateCloth();
	}
}

void Collider::updateRagdoll()
{
	if(!enabled) return;	
	if(canHandleRagdoll())
	{
		onUpdateRagdoll();
	}
}

void Collider::updateVehicle()
{
	if(!enabled) return;	
	if(canHandleVehicle())
	{
		onUpdateVehicle();
	}
}

ePhysError Collider::addEmitter(EmitterPtr& ptr)
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
	emitterInfo.Add(ptr->getID(),new ColliderTargetInfo);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Collider::removeEmitter(int id)
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


ePhysError Collider::addCloth(ClothPtr& ptr)
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
	clothInfo.Add(ptr->getID(),new ColliderTargetInfo);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Collider::removeCloth(int id)
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

ePhysError Collider::addRagdoll(RagdollPtr& ptr)
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
	ragdollInfo.Add(ptr->getID(),new ColliderTargetInfo);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Collider::removeRagdoll(int id)
{
#ifdef USINGMODULE_R
	if(!ragdollList.Exists(id)) return PERR_OBJECT_NOT_LINKED;
	
	ragdollList.Delete(id);
	ragdollInfo.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Collider::addVehicle(VehiclePtr& ptr)
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
	vehicleInfo.Add(ptr->getID(),new ColliderTargetInfo);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError Collider::removeVehicle(int id)
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

ePhysError Collider::getObjectInfo(int id,ColliderTargetInfo ** info)
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
#ifdef USINGMODULE_R
	if(ragdollInfo.Exists(id) && ragdollList.Get(id)->isValid())
	{
		*info=ragdollInfo.Get(id);
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
	if(!emitterInfo.Exists(id) && !clothInfo.Exists(id) && !ragdollInfo.Exists(id) && !vehicleInfo.Exists(id)) return PERR_OBJECT_NOT_LINKED;

	//else object must be marked as invalid
	return PERR_ITEM_DOESNT_EXIST;
}


ePhysError Collider::setInteraction(int id, bool state)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->enabled=state;

	return PERR_AOK;
}

ePhysError Collider::setBounce(int id, float bounce)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	if(bounce<0.0f || bounce>1.0f) return PERR_BAD_PARAMETER;
	info->bounce=bounce;

	return PERR_AOK;
}

ePhysError Collider::setFriction(int id, float friction)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	if(friction<0.0f || friction>1.0f) return PERR_BAD_PARAMETER;
	info->smoothness=1.0f-friction;

	return PERR_AOK;
}
ePhysError Collider::setInvert(int id, bool state)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->invert=state;

	return PERR_AOK;
}

ePhysError Collider::setKiller(int id, bool state)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->killer=state;

	return PERR_AOK;
}

ePhysError Collider::setUseColor(int id, bool state)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->useColor=state;

	return PERR_AOK;
}

ePhysError Collider::setColor(int id, DWORD color)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->color=color;

	return PERR_AOK;
}

ePhysError Collider::setResolve(int id, bool state)
{
	ColliderTargetInfo * info=0;
	ePhysError result = getObjectInfo(id,&info);
	if(result!=PERR_AOK) return result;

	info->noResolve=!state;

	return PERR_AOK;
}

