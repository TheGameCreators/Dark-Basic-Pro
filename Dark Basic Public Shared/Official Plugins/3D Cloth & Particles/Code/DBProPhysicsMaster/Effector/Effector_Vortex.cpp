#include "stdafx.h"


DECLARE_PLUGIN(Effector_Vortex);

const int Effector_Vortex::classID='VRTX';

Effector_Vortex::Effector_Vortex(int id)
:Effector(id), vortex(50.0f)
{
}

#ifdef USINGMODULE_P
void Effector_Vortex::onUpdateParticles()
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
				Vector3 yVec=m.unitY();
				yVec*=globFalloff*ei->fade;

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

								float falloff=ei->CylindricalFalloff(particle->pos,m)*vortex*ei->fade;
								particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
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
								falloff*=vortex*ei->fade;
								particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
							}while(alive->gotoNextItem());
						}
					}
					else					
					{
						//Per emitter falloff (cylindrical or spherical - decided earlier)
						do
						{
							Particle * particle = alive->getCurLink()->data;
							particle->force+=(particle->pos-particle->lastPos)*yVec*vortex;
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
void Effector_Vortex::onUpdateCloth()
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
				Vector3 yVec=m.unitY();
				yVec*=globFalloff*ci->fade;

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
							float falloff=ci->CylindricalFalloff(particle->pos,m)*vortex*ci->fade;
							particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=SPHFALLOFF(ci,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=vortex*ci->fade;
							particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					for(int i=0;i<numParticles;i++,particle++)
					{
						particle->force+=(particle->pos-particle->lastPos)*yVec*vortex;
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
void Effector_Vortex::onUpdateRagdoll()
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
				Vector3 yVec=m.unitY();
				yVec*=globFalloff*ri->fade;

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
							float falloff=ri->CylindricalFalloff(particle->pos,m)*vortex*ri->fade;
							particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=vortex*ri->fade;
							particle->force+=(particle->pos-particle->lastPos)*m.unitY()*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					for(int i=0;i<numParticles;i++,particle++)
					{
						particle->force+=(particle->pos-particle->lastPos)*yVec*vortex;
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


DLLEXPORT void makeVortexEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Vortex)),PCMD_MAKE_VORTEX_EFFECTOR);
}


DLLEXPORT void setVortexEffector(int id, float vortex)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_VORTEX_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Vortex::classID),PCMD_SET_VORTEX_EFFECTOR);

	Effector_Vortex * vrt = reinterpret_cast<Effector_Vortex *>(&(*tmp));
	vrt->setAmount(vortex);
}

