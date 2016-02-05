#include "stdafx.h"


DECLARE_PLUGIN(Effector_Damping);

const int Effector_Damping::classID='DAMP';

Effector_Damping::Effector_Damping(int id)
:Effector(id),damping(0.01f)
{
}

void Effector_Damping::setAmount(float dmp)
{
	if(dmp>=0 && dmp<1) damping=dmp;
	needUpdate[0]=true;
	needUpdate[1]=true;
	needUpdate[2]=true;
	needUpdate[3]=true;
}


void Effector_Damping::updateParticles()
{
#ifdef USINGMODULE_P
	if(needUpdate[0]) 
	{
		onUpdateParticles();
		needUpdate[0]=false;
	}
#endif
}

void Effector_Damping::updateCloth()
{
#ifdef USINGMODULE_C
	if(needUpdate[1]) 
	{
		onUpdateCloth();
		needUpdate[1]=false;
	}
#endif
}


void Effector_Damping::updateRagdoll()
{
	#ifdef USINGMODULE_R
	if(needUpdate[2]) 
	{
		onUpdateRagdoll();
		needUpdate[2]=false;
	}
	#endif
}

void Effector_Damping::updateVehicle()
{
	if(needUpdate[3]) 
	{
		onUpdateVehicle();
		needUpdate[3]=false;
	}
}

#ifdef USINGMODULE_P
void Effector_Damping::onUpdateParticles()
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
				if(enabled && ei->enabled) emitterList.getCurItem_WARNING()->setDamping(damping*ei->fade);
				else emitterList.getCurItem_WARNING()->setDamping(0);			
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_C
void Effector_Damping::onUpdateCloth()
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
				if(enabled && ci->enabled) clothList.getCurItem_WARNING()->setDamping(damping*ci->fade);
				else clothList.getCurItem_WARNING()->setDamping(0);
			}
			clothInfo.gotoNextItem();
		}
		while(clothList.gotoNextItem());
	}		
}
#endif

#ifdef USINGMODULE_R
void Effector_Damping::onUpdateRagdoll()
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
				if(enabled && ri->enabled) ragdollList.getCurItem_WARNING()->setDamping(damping*ri->fade);
				else ragdollList.getCurItem_WARNING()->setDamping(0);
			}
			ragdollInfo.gotoNextItem();
		}
		while(ragdollList.gotoNextItem());
	}		
}
#endif

#ifdef USINGMODULE_V
void Effector_Damping::onUpdateVehicle()
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
				if(enabled && vi->enabled) vehicleList.getCurItem_WARNING()->setDamping(damping*vi->fade);
				else vehicleList.getCurItem_WARNING()->setDamping(0);
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



DLLEXPORT void makeDampingEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Damping)),PCMD_MAKE_DAMPING_EFFECTOR);
}


DLLEXPORT void setDampingEffector(int id, float damping)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_DAMPING_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Damping::classID),PCMD_SET_DAMPING_EFFECTOR);

	Effector_Damping * dmp = reinterpret_cast<Effector_Damping *>(&(*tmp));
	dmp->setAmount(damping);
}

