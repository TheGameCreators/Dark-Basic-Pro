#include "stdafx.h"


DECLARE_PLUGIN(Effector_Size);

const int Effector_Size::classID='SIZE';

Effector_Size::Effector_Size(int id)
:Effector(id),activateOnCollide(false),sizeEnvelope(EnvFloat(1.0f))
{
}

#ifdef USINGMODULE_P
void Effector_Size::onUpdateParticles()
{
	if(emitterList.gotoFirstItem())
	{
		emitterInfo.gotoFirstItem();

		ParticleList * alive;
		ParticleList * dead;
		
		do
		{
			EffectorTargetInfo * ei = emitterInfo.getCurItem();

			if(!emitterList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!emitterList.getCurItem_WARNING()->isFrozen() && ei->enabled)
			{
				emitterList.getCurItem_WARNING()->getParticleLists(&alive,&dead);

				if(alive->gotoFirstItem())
				{
					Particle * particle;
					do
					{
						particle = alive->getCurLink()->data;
						float time=0.0f;
						if(activateOnCollide)
						{
							if(!(particle->flags&PARTICLEDATA_COLLIDED)) continue;
							time = (particle->frameCount-particle->baseFrameCount)*PhysicsManager::iterationInterval;
						}
						else
						{
							time = particle->frameCount*PhysicsManager::iterationInterval;
						}
						EnvFloat val;
						sizeEnvelope.getValue(time,&val);
						particle->size=val.val;
						
					}while(alive->gotoNextItem());					
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

int Effector_Size::addKey(float time, float size)
{
	// leefix - 030306 - u60 - return correct index of added keyframe (mikes approach)
	//return sizeEnvelope.addKey(time,&EnvFloat(size));
	sizeEnvelope.addKey(time,&EnvFloat(size));
	return sizeEnvelope.keys.Count( );
}

ePhysError Effector_Size::deleteKeyAtTime(float time)
{
	if(!sizeEnvelope.deleteKeyAtTime(time)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}

ePhysError Effector_Size::deleteKey(int n)
{
	if(!sizeEnvelope.deleteKey(n)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}

int Effector_Size::getNumKeys()
{
	return sizeEnvelope.Count();
}

ePhysError Effector_Size::getKeyValue(int n, float * val)
{
	EnvFloat tmp;
	if(sizeEnvelope.getKeyValue(n,&tmp))
	{
		*val = tmp.val;
		return PERR_AOK;
	}
	return PERR_KEY_DOESNT_EXIST;
}

ePhysError Effector_Size::getKeyTime(int n, float * time)
{	
	if(!sizeEnvelope.getKeyTime(n, time)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}




//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeSizeEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Size)),PCMD_MAKE_SIZE_EFFECTOR);
}

DLLEXPORT int sizeEffectorAddKey(int id, float time, float size)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_ADD_KEY,-1);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_ADD_KEY,-1);
	
	if(size<0 || time<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_SIZE_EFFECTOR_ADD_KEY,-1);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	return siz->addKey(time,size);
}

DLLEXPORT void sizeEffectorDeleteKeyAtTime(int id, float time)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_DELETE_KEY_AT_TIME);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_DELETE_KEY_AT_TIME);

	if(time<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_SIZE_EFFECTOR_DELETE_KEY_AT_TIME);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	RETURN_ON_PERR(siz->deleteKeyAtTime(time),PCMD_SIZE_EFFECTOR_DELETE_KEY_AT_TIME);
}

DLLEXPORT void sizeEffectorDeleteKey(int id, int n)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_DELETE_KEY);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_DELETE_KEY);

	if(n<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_SIZE_EFFECTOR_DELETE_KEY);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	RETURN_ON_PERR(siz->deleteKey(n),PCMD_SIZE_EFFECTOR_DELETE_KEY);
}

DLLEXPORT int sizeEffectorGetNumKeys(int id)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_GET_NUM_KEYS,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_GET_NUM_KEYS,0);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	return siz->getNumKeys();
}

DLLEXPORT DWORD sizeEffectorGetKeyValue(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_GET_KEY_VALUE,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_GET_KEY_VALUE,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_SIZE_EFFECTOR_GET_KEY_VALUE,0);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	float val=0.0f;
	RETURNVALUE_ON_PERR(siz->getKeyValue(n,&val),PCMD_SIZE_EFFECTOR_GET_KEY_VALUE,0);
	return *(DWORD*)&val;
}

DLLEXPORT DWORD sizeEffectorGetKeyTime(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_GET_KEY_TIME,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_GET_KEY_TIME,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_SIZE_EFFECTOR_GET_KEY_TIME,0);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	float val=0.0f;
	RETURNVALUE_ON_PERR(siz->getKeyTime(n,&val),PCMD_SIZE_EFFECTOR_GET_KEY_TIME,0);
	return *(DWORD*)&val;
}

DLLEXPORT void sizeEffectorUseOnCollide(int id, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SIZE_EFFECTOR_USE_ON_COLLIDE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_SIZE_EFFECTOR_USE_ON_COLLIDE);

	Effector_Size * siz = reinterpret_cast<Effector_Size *>(&(*tmp));
	siz->activateOnCollide=state?true:false;
}


