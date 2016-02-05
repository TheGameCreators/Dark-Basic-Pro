#include "stdafx.h"


DECLARE_PLUGIN(Effector_Chaos);

const int Effector_Chaos::classID='KAOS';

Effector_Chaos::Effector_Chaos(int id)
:Effector(id), chaos(50.0f)
{
}

#ifdef USINGMODULE_P
void Effector_Chaos::onUpdateParticles()
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
				globFalloff*=chaos*ei->fade;

				emitterList.getCurItem_WARNING()->getParticleLists(&alive,&dead);

				if(alive->gotoFirstItem())
				{
					Particle * particle;
					if(ei->useFalloff && ei->falloffPerParticle)
					{
						do
						{
							particle = alive->getCurLink()->data;
							float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=chaos*ei->fade;
							Vector3 rv;
							int dir = 3*(rand()%LookupTables::numSphereCoords);
							rv.x=LookupTables::sphereCoords[dir];
							rv.y=LookupTables::sphereCoords[dir+1];
							rv.z=LookupTables::sphereCoords[dir+2];

							particle->force+=(particle->pos-particle->lastPos)*rv*falloff;

						}while(alive->gotoNextItem());
					}
					else
					{
						do
						{
							particle = alive->getCurLink()->data;
							Vector3 rv;
							int dir = 3*(rand()%LookupTables::numSphereCoords);
							rv.x=LookupTables::sphereCoords[dir];
							rv.y=LookupTables::sphereCoords[dir+1];
							rv.z=LookupTables::sphereCoords[dir+2];

							particle->force+=(particle->pos-particle->lastPos)*rv*globFalloff;

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
void Effector_Chaos::onUpdateCloth()
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
					globFalloff=SPHFALLOFF(ci,clothList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
					CLAMP(globFalloff,0.0f,1.0f);	
				}
				globFalloff*=chaos*ci->fade;

				ClothParticle * particle;
				clothList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = clothList.getCurItem_WARNING()->getNumParticles();
				
				if(ci->useFalloff && ci->falloffPerParticle)
				{
					for(int i=0;i<numParticles;i++,particle++)
					{							
						float falloff=SPHFALLOFF(ci,particle->pos,m.pos());
						CLAMP(falloff,0.0f,1.0f);
						falloff*=chaos*ci->fade;
						Vector3 rv;
						int dir = 3*(rand()%LookupTables::numSphereCoords);
						rv.x=LookupTables::sphereCoords[dir];
						rv.y=LookupTables::sphereCoords[dir+1];
						rv.z=LookupTables::sphereCoords[dir+2];

						particle->force+=(particle->pos-particle->lastPos)*rv*falloff;
					}
				}
				else
				{
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 rv;
						int dir = 3*(rand()%LookupTables::numSphereCoords);
						rv.x=LookupTables::sphereCoords[dir];
						rv.y=LookupTables::sphereCoords[dir+1];
						rv.z=LookupTables::sphereCoords[dir+2];

						particle->force+=(particle->pos-particle->lastPos)*rv*globFalloff;
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
void Effector_Chaos::onUpdateRagdoll()
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
					globFalloff=SPHFALLOFF(ri,ragdollList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
					CLAMP(globFalloff,0.0f,1.0f);	
				}
				globFalloff*=chaos*ri->fade;

				RagdollParticle * particle;
				ragdollList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = ragdollList.getCurItem_WARNING()->getNumParticles();
				
				if(ri->useFalloff && ri->falloffPerParticle)
				{
					for(int i=0;i<numParticles;i++,particle++)
					{							
						float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
						CLAMP(falloff,0.0f,1.0f);
						falloff*=chaos*ri->fade;
						Vector3 rv;
						int dir = 3*(rand()%LookupTables::numSphereCoords);
						rv.x=LookupTables::sphereCoords[dir];
						rv.y=LookupTables::sphereCoords[dir+1];
						rv.z=LookupTables::sphereCoords[dir+2];

						particle->force+=(particle->pos-particle->lastPos)*rv*falloff;
					}
				}
				else
				{
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 rv;
						int dir = 3*(rand()%LookupTables::numSphereCoords);
						rv.x=LookupTables::sphereCoords[dir];
						rv.y=LookupTables::sphereCoords[dir+1];
						rv.z=LookupTables::sphereCoords[dir+2];

						particle->force+=(particle->pos-particle->lastPos)*rv*globFalloff;
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



DLLEXPORT void makeChaosEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Chaos)),PCMD_MAKE_CHAOS_EFFECTOR);
}


DLLEXPORT void setChaosEffector(int id, float chaos)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_CHAOS_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Chaos::classID),PCMD_SET_CHAOS_EFFECTOR);

	Effector_Chaos * chs = reinterpret_cast<Effector_Chaos *>(&(*tmp));
	chs->setAmount(chaos);
}
