#include "stdafx.h"


DECLARE_PLUGIN(Effector_Force);

const int Effector_Force::classID='FRCE';

Effector_Force::Effector_Force(int id)
:Effector(id)
{
}

#ifdef USINGMODULE_P
void Effector_Force::onUpdateParticles()
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
					//Set falloff for the emitter/effector pair if not using per particle falloff
					if(ei->useCylFalloff)
					{
 						globFalloff=ei->CylindricalFalloff(emitterList.getCurItem_WARNING()->getMatrix().pos(),m);
					}
					else
					{
						globFalloff=SPHFALLOFF(ei,emitterList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
						CLAMP(globFalloff,0.0f,1.0f);	
					}									
				}				

				emitterList.getCurItem_WARNING()->getParticleLists(&alive,&dead);

				if(alive->gotoFirstItem())
				{
					if(ei->useFalloff && ei->falloffPerParticle)
					{
						if(ei->useCylFalloff)
						{
							//Per particle cylindrical falloff
							do
							{
								Particle * particle = alive->getCurLink()->data;

								float falloff=ei->CylindricalFalloff(particle->pos,m);
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ei->fade;
								particle->force+=force*falloff;
							}while(alive->gotoNextItem());
						}
						else
						{	
							//Per particle spherical falloff
							do
							{
								Particle * particle = alive->getCurLink()->data;

								float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ei->fade;
								particle->force+=force*falloff;
							}while(alive->gotoNextItem());
						}
					}
					else					
					{
						//Per emitter falloff (cylindrical or spherical - decided earlier)
						Vector3 frc = force*globFalloff*ei->fade;
						do
						{
							Particle * particle = alive->getCurLink()->data;
							particle->force+=frc;
						}while(alive->gotoNextItem());
					}
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_C
void Effector_Force::onUpdateCloth()
{
	if(clothList.gotoFirstItem())
	{
		clothInfo.gotoFirstItem();

		do
		{
			EffectorTargetInfo * ci = clothInfo.getCurItem();

			if(!clothList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!clothList.getCurItem_WARNING()->isFrozen() && ci->enabled)
			{
				float globFalloff=1.0f;
				if(ci->useFalloff && !ci->falloffPerParticle)
				{
					//Set falloff for the emitter/effector pair if not using per particle falloff
					if(ci->useCylFalloff)
					{
 						globFalloff=ci->CylindricalFalloff(clothList.getCurItem_WARNING()->getMatrix().pos(),m);
					}
					else
					{
						globFalloff=SPHFALLOFF(ci,clothList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
						CLAMP(globFalloff,0.0f,1.0f);	
					}									
				}				

				ClothParticle * particle;
				clothList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = clothList.getCurItem_WARNING()->getNumParticles();


				if(ci->useFalloff && ci->falloffPerParticle)
				{
					if(ci->useCylFalloff)
					{
						//Per particle cylindrical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=ci->CylindricalFalloff(particle->pos,m);
							CLAMP(falloff,0.0f,1.0f);
							falloff*=ci->fade;
							particle->force+=force*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=SPHFALLOFF(ci,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=ci->fade;
							particle->force+=force*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					Vector3 frc = force*globFalloff*ci->fade;
					for(int i=0;i<numParticles;i++,particle++)
					{
						particle->force+=frc;
					}
				}
			
			}
			clothInfo.gotoNextItem();
		}
		while(clothList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_R
void Effector_Force::onUpdateRagdoll()
{
	if(ragdollList.gotoFirstItem())
	{
		ragdollInfo.gotoFirstItem();

		do
		{
			EffectorTargetInfo * ri = ragdollInfo.getCurItem();

			if(!ragdollList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!ragdollList.getCurItem_WARNING()->isFrozen() && ri->enabled)
			{
				float globFalloff=1.0f;
				if(ri->useFalloff && !ri->falloffPerParticle)
				{
					//Set falloff for the emitter/effector pair if not using per particle falloff
					if(ri->useCylFalloff)
					{
 						globFalloff=ri->CylindricalFalloff(emitterList.getCurItem_WARNING()->getMatrix().pos(),m);
					}
					else
					{
						globFalloff=SPHFALLOFF(ri,emitterList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
						CLAMP(globFalloff,0.0f,1.0f);	
					}									
				}				

				RagdollParticle * particle;
				ragdollList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = ragdollList.getCurItem_WARNING()->getNumParticles();


				if(ri->useFalloff && ri->falloffPerParticle)
				{
					if(ri->useCylFalloff)
					{
						//Per particle cylindrical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=ri->CylindricalFalloff(particle->pos,m);
							CLAMP(falloff,0.0f,1.0f);
							falloff*=ri->fade;
							particle->force+=force*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=ri->fade;
							particle->force+=force*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					Vector3 frc = force*globFalloff*ri->fade;
					for(int i=0;i<numParticles;i++,particle++)
					{
						particle->force+=frc;
					}
				}
			
			}
			ragdollInfo.gotoNextItem();
		}
		while(ragdollList.gotoNextItem());
	}
}
#endif

//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeForceEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Force)),PCMD_MAKE_FORCE_EFFECTOR);
}


DLLEXPORT void setForceEffector(int id, float fx, float fy, float fz)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_FORCE_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Force::classID),PCMD_SET_FORCE_EFFECTOR);

	Effector_Force * frc = reinterpret_cast<Effector_Force *>(&(*tmp));
	frc->setAmount(Vector3(fx,fy,fz));
}


