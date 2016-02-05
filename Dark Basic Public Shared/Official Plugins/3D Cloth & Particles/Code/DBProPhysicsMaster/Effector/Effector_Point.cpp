#include "stdafx.h"


DECLARE_PLUGIN(Effector_Point);

const int Effector_Point::classID='POIN';

Effector_Point::Effector_Point(int id)
:Effector(id),force(1.0f),falloffType(FALLOFF_NONE),killAtRadius(false),
killRadius(1.0f),falloffFunc(falloff_None)
{
}


#ifdef USINGMODULE_P
void Effector_Point::onUpdateParticles()
{	
	//clipVal is set to maximum amount particle can be moved towards center
	clipVal_Upd=0.0f;
	if(force!=0.0f) clipVal_Upd = 0.5f/(PhysicsManager::iterationInterval*PhysicsManager::iterationInterval*force);		
	if(clipVal_Upd<0.0f) clipVal_Upd = -clipVal_Upd ;

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
								Vector3 frc = particle->pos-m.pos();
								float dist = frc.NormaliseRet();								
								if(killAtRadius && dist<killRadius)
								{
									particle->energy=0;
								}
								else
								{
									float falloff=ei->CylindricalFalloff(particle->pos,m);
									CLAMP(falloff,0.0f,1.0f);
									falloff*=force*ei->fade;
									falloff*=falloffFunc(dist,clipVal_Upd*dist/particle->invMass);
									particle->force+=frc*falloff;
								}
							}while(alive->gotoNextItem());
						}
						else
						{	
							//Per particle spherical falloff
							do
							{
								Particle * particle = alive->getCurLink()->data;

								Vector3 frc = particle->pos-m.pos();
								float dist = frc.NormaliseRet();
								if(killAtRadius && dist<killRadius)
								{
									particle->energy=0;
								}
								else
								{
									float falloff=SPHFALLOFF(ei,particle->pos,m.pos());
									CLAMP(falloff,0.0f,1.0f);
									falloff*=force*ei->fade;
									falloff*=falloffFunc(dist,clipVal_Upd*dist/particle->invMass);
									particle->force+=frc*falloff;
								}
							}while(alive->gotoNextItem());
						}
					}
					else					
					{
						//Per emitter falloff (cylindrical or spherical - decided earlier)												
						do
						{
							Particle * particle = alive->getCurLink()->data;
							Vector3 frc = particle->pos-m.pos();
							float dist = frc.NormaliseRet();
							if(killAtRadius && dist<killRadius)
							{
								particle->energy=0;
							}
							else
							{
								float falloff=globFalloff*force*ei->fade;							
								falloff*=falloffFunc(dist,clipVal_Upd*dist/particle->invMass);
								particle->force+=frc*falloff;
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
void Effector_Point::onUpdateCloth()
{
	//clipVal is set to maximum amount particle can be moved towards center
	clipVal_Upd=0.0f;
	if(force!=0.0f) clipVal_Upd = 0.5f/(PhysicsManager::iterationInterval*PhysicsManager::iterationInterval*force);		
	if(clipVal_Upd<0.0f) clipVal_Upd = -clipVal_Upd ;

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
				float invMass = clothList.getCurItem_WARNING()->getParticleInvMass();
				int numParticles = clothList.getCurItem_WARNING()->getNumParticles();

				if(ci->useFalloff && ci->falloffPerParticle)
				{
					if(ci->useCylFalloff)
					{
						//Per particle cylindrical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							Vector3 frc = particle->pos-m.pos();
							float dist = frc.NormaliseRet();								
							float falloff=ci->CylindricalFalloff(particle->pos,m);
							CLAMP(falloff,0.0f,1.0f);
							falloff*=force*ci->fade;
							falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
							particle->force+=frc*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							Vector3 frc = particle->pos-m.pos();
							float dist = frc.NormaliseRet();
							
							float falloff=SPHFALLOFF(ci,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=force*ci->fade;
							falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
							particle->force+=frc*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)												
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 frc = particle->pos-m.pos();
						float dist = frc.NormaliseRet();

						float falloff=globFalloff*force*ci->fade;							
						falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
						particle->force+=frc*falloff;
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
void Effector_Point::onUpdateRagdoll()
{
	//clipVal is set to maximum amount particle can be moved towards center
	clipVal_Upd=0.0f;
	if(force!=0.0f) clipVal_Upd = 0.5f/(PhysicsManager::iterationInterval*PhysicsManager::iterationInterval*force);		
	if(clipVal_Upd<0.0f) clipVal_Upd = -clipVal_Upd ;

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
				float invMass = ragdollList.getCurItem_WARNING()->getParticleInvMass();
				int numParticles = ragdollList.getCurItem_WARNING()->getNumParticles();

				if(ri->useFalloff && ri->falloffPerParticle)
				{
					if(ri->useCylFalloff)
					{
						//Per particle cylindrical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							Vector3 frc = particle->pos-m.pos();
							float dist = frc.NormaliseRet();								
							float falloff=ri->CylindricalFalloff(particle->pos,m);
							CLAMP(falloff,0.0f,1.0f);
							falloff*=force*ri->fade;
							falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
							particle->force+=frc*falloff;
						}
					}
					else
					{	
						//Per particle spherical falloff
						for(int i=0;i<numParticles;i++,particle++)
						{
							Vector3 frc = particle->pos-m.pos();
							float dist = frc.NormaliseRet();
							
							float falloff=SPHFALLOFF(ri,particle->pos,m.pos());
							CLAMP(falloff,0.0f,1.0f);
							falloff*=force*ri->fade;
							falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
							particle->force+=frc*falloff;
						}
					}
				}
				else					
				{
					//Per emitter falloff (cylindrical or spherical - decided earlier)												
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 frc = particle->pos-m.pos();
						float dist = frc.NormaliseRet();

						float falloff=globFalloff*force*ri->fade;							
						falloff*=falloffFunc(dist,clipVal_Upd*dist/invMass);
						particle->force+=frc*falloff;
					}
				}
			}
			ragdollInfo.gotoNextItem();
		}
		while(ragdollList.gotoNextItem());
	}
}
#endif

float falloff_None(float dist,float clipVal)
{
	return 1.0f;
}

float falloff_Inv(float dist,float clipVal)
{
	float val;
	if(dist>0)
	{
		val=1.0f/dist;		
		if(val>clipVal) val=clipVal;
		return val;
	}
	return 1.0f;
}

float falloff_InvSquare(float dist,float clipVal)
{
	float val;
	if(dist>0)
	{
		val=1.0f/(dist*dist);
		if(val>clipVal) val=clipVal;
		return val;
	}
	return 1.0f;
}



//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makePointEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_Point)),PCMD_MAKE_POINT_EFFECTOR);
}


DLLEXPORT void setPointEffector(int id, float frc)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR);

	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	pnt->setAmount(frc);
}

DLLEXPORT void setPointEffectorFalloffNone(int id)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR_FALLOFF_NONE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR_FALLOFF_NONE);

	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	pnt->setForceFalloff(FALLOFF_NONE);
}

DLLEXPORT void setPointEffectorFalloffInverse(int id)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR_FALLOFF_INVERSE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR_FALLOFF_INVERSE);

	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	pnt->setForceFalloff(FALLOFF_INV);
}

DLLEXPORT void setPointEffectorFalloffInverseSquare(int id)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR_FALLOFF_INVERSE_SQUARE);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR_FALLOFF_INVERSE_SQUARE);

	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	pnt->setForceFalloff(FALLOFF_INVSQUARE);
}


DLLEXPORT void setPointEffectorUseKillRadius(int id, DWORD state)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR_USE_KILL_RADIUS);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR_USE_KILL_RADIUS);
	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	if(state) pnt->useKillRad(true);
	else pnt->useKillRad(false);
}

DLLEXPORT void setPointEffectorKillRadius(int id, float rad)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_POINT_EFFECTOR_KILL_RADIUS);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_Point::classID),PCMD_SET_POINT_EFFECTOR_KILL_RADIUS);

	Effector_Point * pnt = reinterpret_cast<Effector_Point *>(&(*tmp));
	pnt->setKillRad(rad);
}
