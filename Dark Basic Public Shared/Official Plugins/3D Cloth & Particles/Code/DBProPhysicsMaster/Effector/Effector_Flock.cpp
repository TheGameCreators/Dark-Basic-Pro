#include "stdafx.h"


DECLARE_PLUGIN(Effector_Flock);

const int Effector_Flock::classID='FLCK';

Effector_Flock::Effector_Flock(int id)
:Effector(id), flock(1.0f),useEffectorPosition(false),lastMode(false)
{
}

void Effector_Flock::updateParticles()
{
#ifdef USINGMODULE_P
	if(enabled)
	{
		onUpdateParticles();
	}
	lastMode=useEffectorPosition;
#endif
}

#ifdef USINGMODULE_P
void Effector_Flock::onUpdateParticles()
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
				float globFalloff=1.0f;
				if(ei->useFalloff && !ei->falloffPerParticle)
				{
					globFalloff=SPHFALLOFF(ei,emitterList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
					CLAMP(globFalloff,0.0f,1.0f);	
				}
				globFalloff*=flock*ei->fade;

				emitterList.getCurItem_WARNING()->getParticleLists(&alive,&dead);
								
				if(alive->gotoFirstItem())
				{
					Particle * particle;
					Vector3 curForce;
					if(useEffectorPosition)
					{
						if(ei->useFalloff && ei->falloffPerParticle)
						{
							do
							{
								particle = alive->getCurLink()->data;
								float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
								CLAMP(falloff,0.0f,1.0f);
								falloff*=flock*ei->fade;									
								particle->force+=(m.pos()-particle->pos)*falloff;
							}while(alive->gotoNextItem());
						}
						else
						{
							do
							{
								particle = alive->getCurLink()->data;
								particle->force+=(m.pos()-particle->pos)*globFalloff;
							}while(alive->gotoNextItem());
						}
					}
					else
					{
						if(lastMode!=useEffectorPosition)
						{
							//We've just changed mode, update average position without applying forces
							ei->v.Init();
							int count=0;
							do
							{
								count++;
								ei->v+=alive->getCurLink()->data->pos;
							}while(alive->gotoNextItem());

							if(count>0) ei->v/=float(count);
						}
						else
						{
							Vector3 curAvPos=ei->v;
							ei->v.Init();
							int count=0;
							if(ei->useFalloff && ei->falloffPerParticle)
							{
								do
								{
									count++;
									particle = alive->getCurLink()->data;
									ei->v+=particle->pos;
									float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
									CLAMP(falloff,0.0f,1.0f);
									falloff*=flock*ei->fade;						
									particle->force+=(curAvPos-particle->pos)*falloff;
								}while(alive->gotoNextItem());
							}
							else
							{
								do
								{
									count++;
									particle = alive->getCurLink()->data;
									ei->v+=particle->pos;
									particle->force+=(curAvPos-particle->pos)*globFalloff;
								}while(alive->gotoNextItem());
							}

							if(count>0) ei->v/=float(count);
						}
					}
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeFlockEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Flock)),PCMD_MAKE_FLOCK_EFFECTOR);
}


DLLEXPORT void setFlockEffector(int id, float flock)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_FLOCK_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Flock::classID),PCMD_SET_FLOCK_EFFECTOR);

	Effector_Flock * flk = reinterpret_cast<Effector_Flock *>(&(*tmp));
	flk->setAmount(flock);
}

DLLEXPORT void setFlockEffectorUseCenter(int id, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_FLOCK_EFFECTOR_USE_CENTER);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Flock::classID),PCMD_SET_FLOCK_EFFECTOR_USE_CENTER);
	Effector_Flock * flk = reinterpret_cast<Effector_Flock *>(&(*tmp));
	if(state) flk->useEffPos(true);
	else flk->useEffPos(false);
}
