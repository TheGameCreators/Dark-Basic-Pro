#include "stdafx.h"


DECLARE_PLUGIN(Effector_Color);

const int Effector_Color::classID='SIZE';

Effector_Color::Effector_Color(int id)
:Effector(id),activateOnCollide(false),colorEnvelope(EnvColor(255,255,255,255))
{
}

#ifdef USINGMODULE_P
void Effector_Color::onUpdateParticles()
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
						EnvColor val;
						colorEnvelope.getValue(time,&val);
						particle->color=val.b + (val.g<<8) + (val.r<<16) + (val.a<<24);;
						
					}while(alive->gotoNextItem());					
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

int Effector_Color::addKey(float time, int r, int g, int b, int a)
{
	// mike - 300305 - return the correct key
	colorEnvelope.addKey(time,&EnvColor(r,g,b,a));
	return colorEnvelope.keys.Count( );

	//return colorEnvelope.addKey(time,&EnvColor(r,g,b,a));
}

ePhysError Effector_Color::deleteKeyAtTime(float time)
{
	if(!colorEnvelope.deleteKeyAtTime(time)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}

ePhysError Effector_Color::deleteKey(int n)
{
	if(!colorEnvelope.deleteKey(n)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}

int Effector_Color::getNumKeys()
{
	return colorEnvelope.Count();
}

ePhysError Effector_Color::getKeyRedValue(int n, int * val)
{
	EnvColor tmp;
	if(colorEnvelope.getKeyValue(n,&tmp))
	{
		*val = tmp.r;
		return PERR_AOK;
	}
	return PERR_KEY_DOESNT_EXIST;
}

ePhysError Effector_Color::getKeyGreenValue(int n, int * val)
{
	EnvColor tmp;
	if(colorEnvelope.getKeyValue(n,&tmp))
	{
		*val = tmp.g;
		return PERR_AOK;
	}
	return PERR_KEY_DOESNT_EXIST;
}

ePhysError Effector_Color::getKeyBlueValue(int n, int * val)
{
	EnvColor tmp;
	if(colorEnvelope.getKeyValue(n,&tmp))
	{
		*val = tmp.b;
		return PERR_AOK;
	}
	return PERR_KEY_DOESNT_EXIST;
}

ePhysError Effector_Color::getKeyAlphaValue(int n, int * val)
{
	EnvColor tmp;
	if(colorEnvelope.getKeyValue(n,&tmp))
	{
		*val = tmp.a;
		return PERR_AOK;
	}
	return PERR_KEY_DOESNT_EXIST;
}

ePhysError Effector_Color::getKeyTime(int n, float * time)
{	
	if(!colorEnvelope.getKeyTime(n, time)) return PERR_KEY_DOESNT_EXIST;
	return PERR_AOK;
}


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeColorEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Color)),PCMD_MAKE_COLOR_EFFECTOR);
}

DLLEXPORT int colorEffectorAddKey(int id, float time, int r, int g, int b, int a)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_ADD_KEY,-1);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_ADD_KEY,-1);
	
	if(r<0 || g<0 || b<0 || a<0 ||
		r>255 || g>255 || b>255 || a>255 ||	time<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_ADD_KEY,-1);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	return col->addKey(time,r,g,b,a);
}

DLLEXPORT void colorEffectorDeleteKeyAtTime(int id, float time)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_DELETE_KEY_AT_TIME);
	// mike
//	RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_DELETE_KEY_AT_TIME);

	if(time<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_DELETE_KEY_AT_TIME);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	RETURN_ON_PERR(col->deleteKeyAtTime(time),PCMD_COLOR_EFFECTOR_DELETE_KEY_AT_TIME);
}

DLLEXPORT void colorEffectorDeleteKey(int id, int n)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_DELETE_KEY);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_DELETE_KEY);

	if(n<0) RETURN_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_DELETE_KEY);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	RETURN_ON_PERR(col->deleteKey(n),PCMD_COLOR_EFFECTOR_DELETE_KEY);
}

DLLEXPORT int colorEffectorGetNumKeys(int id)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_NUM_KEYS,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_NUM_KEYS,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	return col->getNumKeys();
}

DLLEXPORT int colorEffectorGetKeyRedValue(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_KEY_RED_VALUE,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_KEY_RED_VALUE,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_GET_KEY_RED_VALUE,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	int val=0;
	RETURNVALUE_ON_PERR(col->getKeyRedValue(n,&val),PCMD_COLOR_EFFECTOR_GET_KEY_RED_VALUE,0);
	return val;
}

DLLEXPORT int colorEffectorGetKeyGreenValue(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_KEY_GREEN_VALUE,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_KEY_GREEN_VALUE,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_GET_KEY_GREEN_VALUE,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	int val=0;
	RETURNVALUE_ON_PERR(col->getKeyGreenValue(n,&val),PCMD_COLOR_EFFECTOR_GET_KEY_GREEN_VALUE,0);
	return val;
}

DLLEXPORT int colorEffectorGetKeyBlueValue(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_KEY_BLUE_VALUE,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_KEY_BLUE_VALUE,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_GET_KEY_BLUE_VALUE,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	int val=0;
	RETURNVALUE_ON_PERR(col->getKeyBlueValue(n,&val),PCMD_COLOR_EFFECTOR_GET_KEY_BLUE_VALUE,0);
	return val;
}

DLLEXPORT int colorEffectorGetKeyAlphaValue(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_KEY_ALPHA_VALUE,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_KEY_ALPHA_VALUE,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_GET_KEY_ALPHA_VALUE,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	int val=0;
	RETURNVALUE_ON_PERR(col->getKeyAlphaValue(n,&val),PCMD_COLOR_EFFECTOR_GET_KEY_ALPHA_VALUE,0);
	return val;
}


DLLEXPORT DWORD colorEffectorGetKeyTime(int id, int n)
{
	EffectorPtr tmp;
	RETURNVALUE_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_GET_KEY_TIME,0);
	// mike
	//RETURNVALUE_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_GET_KEY_TIME,0);

	if(n<0) RETURNVALUE_ON_PERR(PERR_BAD_PARAMETER,PCMD_COLOR_EFFECTOR_GET_KEY_TIME,0);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	float val=0.0f;
	RETURNVALUE_ON_PERR(col->getKeyTime(n,&val),PCMD_COLOR_EFFECTOR_GET_KEY_TIME,0);
	return *(DWORD*)&val;
}

DLLEXPORT void colorEffectorUseOnCollide(int id, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_COLOR_EFFECTOR_USE_ON_COLLIDE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Size::classID),PCMD_COLOR_EFFECTOR_USE_ON_COLLIDE);

	Effector_Color * col = reinterpret_cast<Effector_Color *>(&(*tmp));
	col->activateOnCollide=state?true:false;
}

