#include "stdafx.h"


DECLARE_PLUGIN(Effector_Drag);

const int Effector_Drag::classID='DRAG';

Effector_Drag::Effector_Drag(int id)
:Effector(id), drag(50.0f)
{
}

#ifdef USINGMODULE_P
void Effector_Drag::onUpdateParticles()
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
				globFalloff*=drag*ei->fade;

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
							falloff*=drag*ei->fade;
							particle->force-=(particle->pos-particle->lastPos)*falloff;

						}while(alive->gotoNextItem());
					}
					else
					{
						do
						{
							particle = alive->getCurLink()->data;
							particle->force-=(particle->pos-particle->lastPos)*globFalloff;

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
void Effector_Drag::onUpdateCloth()
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
				globFalloff*=drag*ci->fade;

				//This effector mode is different, here we apply forces based on the cloth segment normal
				ClothSegment * segment;
				clothList.getCurItem_WARNING()->getSegments(&segment);
				int numSegments = clothList.getCurItem_WARNING()->getNumSegments();

				if(ci->useFalloff && ci->falloffPerParticle)
				{
					for(int i=0;i<numSegments;i++,segment++)
					{
						ClothParticle ** particle = segment->particle;

						float falloff=SPHFALLOFF(ci,particle[0]->pos,m.pos());
						CLAMP(falloff,0.0f,1.0f);
						falloff*=drag*ci->fade;

						Vector3 norm = (particle[1]->pos-particle[0]->pos)*(particle[2]->pos-particle[0]->pos);
						Vector3 vel = particle[0]->pos-particle[0]->lastPos;
						vel += particle[1]->pos-particle[1]->lastPos;
						vel += particle[2]->pos-particle[2]->lastPos;
						vel*=0.33333333333f;
						float fweight = norm.Dot(vel);
						if(fweight<0) fweight=-fweight;
						vel*=fweight*falloff;

						particle[0]->force-=vel;
						particle[1]->force-=vel;
						particle[2]->force-=vel;
						if(particle[3]) particle[3]->force-=vel;
					}
				}
				else
				{
					for(int i=0;i<numSegments;i++,segment++)
					{			
						ClothParticle ** particle = segment->particle;

						Vector3 norm = (particle[1]->pos-particle[0]->pos)*(particle[2]->pos-particle[0]->pos);
						Vector3 vel = particle[0]->pos-particle[0]->lastPos;
						vel += particle[1]->pos-particle[1]->lastPos;
						vel += particle[2]->pos-particle[2]->lastPos;
						vel*=0.33333333333f;
						float fweight = norm.Dot(vel);
						if(fweight<0) fweight=-fweight;

						vel*=fweight*globFalloff;

						particle[0]->force-=vel*particle[0]->segmentWeighting;
						particle[1]->force-=vel*particle[1]->segmentWeighting;
						particle[2]->force-=vel*particle[2]->segmentWeighting;
						if(particle[3]) particle[3]->force-=vel*particle[3]->segmentWeighting;
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
void Effector_Drag::onUpdateRagdoll()
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
					globFalloff=SPHFALLOFF(ri,emitterList.getCurItem_WARNING()->getMatrix().pos(),m.pos());
					CLAMP(globFalloff,0.0f,1.0f);	
				}
				globFalloff*=drag*ri->fade;

				RagdollParticle * particle;
				ragdollList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = ragdollList.getCurItem_WARNING()->getNumParticles();

				if(ri->useFalloff && ri->falloffPerParticle)
				{
					for(int i=0;i<numParticles;i++,particle++)
					{							
						float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
						CLAMP(falloff,0.0f,1.0f);
						falloff*=drag*ri->fade;
						particle->force-=(particle->pos-particle->lastPos)*falloff;
					}					
				}
				else
				{
					for(int i=0;i<numParticles;i++,particle++)
					{
						particle->force-=(particle->pos-particle->lastPos)*globFalloff;
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


DLLEXPORT void makeDragEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Drag)),PCMD_MAKE_DRAG_EFFECTOR);
}


DLLEXPORT void setDragEffector(int id, float drag)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_DAMPING_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Drag::classID),PCMD_SET_DRAG_EFFECTOR);

	Effector_Drag * drg = reinterpret_cast<Effector_Drag *>(&(*tmp));
	drg->setAmount(drag);
}
