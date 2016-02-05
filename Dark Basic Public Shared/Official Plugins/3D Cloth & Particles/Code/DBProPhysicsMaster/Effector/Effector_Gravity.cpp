#include "stdafx.h"


DECLARE_PLUGIN(Effector_Gravity);

const int Effector_Gravity::classID='GRAV';

Effector_Gravity::Effector_Gravity(int id)
:Effector(id),gravity(0,-4.0f,0)
{
}

void Effector_Gravity::setForce(float x, float y, float z)
{
	gravity.x=x;
	gravity.y=y;
	gravity.z=z;
	needUpdate[0]=true;
	needUpdate[1]=true;
	needUpdate[2]=true;
	needUpdate[3]=true;
}


void Effector_Gravity::updateParticles()
{
#ifdef USINGMODULE_P
	if(needUpdate[0]) 
	{
		onUpdateParticles();
		needUpdate[0]=false;
	}
#endif
}
void Effector_Gravity::updateCloth()
{
#ifdef USINGMODULE_C
	if(needUpdate[1]) 
	{
		onUpdateCloth();
		needUpdate[1]=false;
	}
#endif
}
void Effector_Gravity::updateRagdoll()
{
	#ifdef USINGMODULE_R
	if(needUpdate[2]) 
	{
		onUpdateRagdoll();
		needUpdate[2]=false;
	}
	#endif
}
void Effector_Gravity::updateVehicle()
{
#ifdef USINGMODULE_V
	if(needUpdate[3]) 
	{
		onUpdateVehicle();
		needUpdate[3]=false;
	}
#endif
}

#ifdef USINGMODULE_P
void Effector_Gravity::onUpdateParticles()
{
	if(emitterList.gotoFirstItem())
	{
		emitterInfo.gotoFirstItem();
		do
		{				
			if(!emitterList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else
			{
				EffectorTargetInfo * ei = emitterInfo.getCurItem();
				if(enabled && ei->enabled) emitterList.getCurItem_WARNING()->setGravity(gravity*ei->fade);
				else emitterList.getCurItem_WARNING()->setGravity(Vector3(0,0,0));
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_C
void Effector_Gravity::onUpdateCloth()
{
	if(clothList.gotoFirstItem())
	{
		clothInfo.gotoFirstItem();
		do
		{			
			if(!clothList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else
			{
				EffectorTargetInfo * ci = clothInfo.getCurItem();
				if(enabled && ci->enabled) clothList.getCurItem_WARNING()->setGravity(gravity*ci->fade);
				else clothList.getCurItem_WARNING()->setGravity(Vector3(0,0,0));
			}
			clothInfo.gotoNextItem();
		}
		while(clothList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_R
void Effector_Gravity::onUpdateRagdoll()
{
	if(ragdollList.gotoFirstItem())
	{
		ragdollInfo.gotoFirstItem();
		do
		{				
			if(!ragdollList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else
			{
				EffectorTargetInfo * ri = ragdollInfo.getCurItem();
				if(enabled && ri->enabled) ragdollList.getCurItem_WARNING()->setGravity(gravity*ri->fade);
				else ragdollList.getCurItem_WARNING()->setGravity(Vector3(0,0,0));
			}
			ragdollInfo.gotoNextItem();
		}
		while(ragdollList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_V
void Effector_Gravity::onUpdateVehicle()
{
	if(vehicleList.gotoFirstItem())
	{
		vehicleInfo.gotoFirstItem();
		do
		{				
			if(!vehicleList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else
			{
				EffectorTargetInfo * vi = vehicleInfo.getCurItem();
				if(enabled && vi->enabled) vehicleList.getCurItem_WARNING()->setGravity(gravity*vi->fade);
				else vehicleList.getCurItem_WARNING()->setGravity(Vector3(0,0,0));
			}
			vehicleInfo.gotoNextItem();
		}
		while(vehicleList.gotoNextItem());
	}
}
#endif
//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeGravityEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Gravity)),PCMD_MAKE_GRAVITY_EFFECTOR);
}


DLLEXPORT void setGravityEffector(int id, float x, float y, float z)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_GRAVITY_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Gravity::classID),PCMD_SET_GRAVITY_EFFECTOR);

	Effector_Gravity * grv = reinterpret_cast<Effector_Gravity *>(&(*tmp));
	grv->setForce(x,y,z);
}