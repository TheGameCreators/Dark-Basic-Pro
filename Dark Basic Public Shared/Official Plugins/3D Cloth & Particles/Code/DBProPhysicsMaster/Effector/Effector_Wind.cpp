#include "stdafx.h"


DECLARE_PLUGIN(Effector_Wind);

const int Effector_Wind::classID='WIND';

Effector_Wind::Effector_Wind(int id)
:Effector(id),windSizeSqr(0.0f)
{
}

#ifdef USINGMODULE_P
void Effector_Wind::onUpdateParticles()
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

								Vector3 diff = particle->pos-particle->lastPos;
								if(diff.MagSqr()<windSizeSqr)
								{
									float falloff=ei->CylindricalFalloff(particle->pos,m);
									CLAMP(falloff,0.0f,1.0f);
									falloff*=ei->fade;
									particle->force+=(wind-diff)*falloff;
								}
							}while(alive->gotoNextItem());
						}
						else
						{	
							//Per particle spherical falloff
							do
							{
								Particle * particle = alive->getCurLink()->data;

								Vector3 diff = particle->pos-particle->lastPos;
								if(diff.MagSqr()<windSizeSqr)
								{
									float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
									CLAMP(falloff,0.0f,1.0f);
									falloff*=ei->fade;
									particle->force+=(wind-diff)*falloff;
								}
							}while(alive->gotoNextItem());
						}
					}
					else					
					{
						//Per emitter falloff (cylindrical or spherical - decided earlier)
						globFalloff*=ei->fade;
						do
						{
							Particle * particle = alive->getCurLink()->data;
							Vector3 diff = particle->pos-particle->lastPos;
							if(diff.MagSqr()<windSizeSqr)
							{
								particle->force+=(wind-diff)*globFalloff;
							}
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
void Effector_Wind::onUpdateCloth()
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

				//This effector mode is different, here we apply forces based on the cloth segment normal
				ClothSegment * segment;
				clothList.getCurItem_WARNING()->getSegments(&segment);
				int numSegments = clothList.getCurItem_WARNING()->getNumSegments();


				if(ci->useFalloff && ci->falloffPerParticle)
				{
					if(ci->useCylFalloff)
					{
						//Per particle cylindrical falloff
						for(int i=0;i<numSegments;i++,segment++)
						{		
							ClothParticle ** particle = segment->particle;
							Vector3 diff = particle[0]->pos-particle[0]->lastPos;
							if(diff.MagSqr()<windSizeSqr)
							{
								float falloff=ci->CylindricalFalloff(particle[0]->pos,m);
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ci->fade;

								Vector3 norm = (particle[1]->pos-particle[0]->pos)*(particle[2]->pos-particle[0]->pos);
								float area=(wind-diff).Dot(norm);
								if(area<0) area=-area;
								falloff*=area;
								Vector3 frc=(wind-diff)*falloff;
								particle[0]->force+=frc;
								particle[1]->force+=frc;
								particle[2]->force+=frc;
								if(particle[3]) particle[3]->force+=frc;
							}
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numSegments;i++,segment++)
						{
							ClothParticle ** particle = segment->particle;
							Vector3 diff = particle[0]->pos-particle[0]->lastPos;
							if(diff.MagSqr()<windSizeSqr)
							{
								float falloff=SPHFALLOFF(ci,particle[0]->pos,m.pos());
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ci->fade;

								Vector3 norm = (particle[1]->pos-particle[0]->pos)*(particle[2]->pos-particle[0]->pos);
								float area=(wind-diff).Dot(norm);
								if(area<0) area=-area;
								falloff*=area;
								Vector3 frc=(wind-diff)*falloff;
								particle[0]->force+=frc;
								particle[1]->force+=frc;
								particle[2]->force+=frc;
								if(particle[3]) particle[3]->force+=frc;								
							}
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					globFalloff*=ci->fade;
					for(int i=0;i<numSegments;i++,segment++)
					{
						ClothParticle ** particle = segment->particle;
						Vector3 diff = particle[0]->pos-particle[0]->lastPos;
						if(diff.MagSqr()<windSizeSqr)
						{
							Vector3 norm = (particle[1]->pos-particle[0]->pos)*(particle[2]->pos-particle[0]->pos);
							float area=(wind-diff).Dot(norm);
							if(area<0) area=-area;
							Vector3 frc=(wind-diff)*globFalloff*area;
							particle[0]->force+=frc;
							particle[1]->force+=frc;
							particle[2]->force+=frc;
							if(particle[3]) particle[3]->force+=frc;
						}
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
void Effector_Wind::onUpdateRagdoll()
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
							Vector3 diff = particle->pos-particle->lastPos;
							if(diff.MagSqr()<windSizeSqr)
							{
								float falloff=ri->CylindricalFalloff(particle->pos,m);
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ri->fade;
								particle->force+=(wind-diff)*falloff;
							}
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							Vector3 diff = particle->pos-particle->lastPos;
							if(diff.MagSqr()<windSizeSqr)
							{
								float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
								CLAMP(falloff,0.0f,1.0f);
								falloff*=ri->fade;
								particle->force+=(wind-diff)*falloff;
							}
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)
					globFalloff*=ri->fade;
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 diff = particle->pos-particle->lastPos;
						if(diff.MagSqr()<windSizeSqr)
						{
							particle->force+=(wind-diff)*globFalloff;
						}
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



DLLEXPORT void makeWindEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Wind)),PCMD_MAKE_WIND_EFFECTOR);
}


DLLEXPORT void setWindEffector(int id, float fx, float fy, float fz)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_WIND_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Wind::classID),PCMD_SET_WIND_EFFECTOR);

	Effector_Wind * wnd = reinterpret_cast<Effector_Wind *>(&(*tmp));
	wnd->setAmount(Vector3(fx,fy,fz));
}

